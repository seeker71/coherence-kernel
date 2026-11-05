#include "membrane-boot.h"

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>

#if defined(__APPLE__)
#include <pthread.h>
extern void sys_icache_invalidate(void *, size_t);
#endif

#define MAX_CELLS 8192
#define MAX_CHILDREN 32
#define MAX_PAYLOAD (1u << 20)
#define MAX_FRAME 4096
#define MAX_HELD 128
#define MAX_CARRIERS 24
#define HASH_OFF 14695981039346656037ull
#define HASH_PRIME 1099511628211ull

typedef struct Cell {
    uint64_t id;
    uint32_t kind;
    uint32_t nchild;
    uint32_t nbytes;
    uint32_t refs;
    int live;
    uint32_t child[MAX_CHILDREN];
    uint8_t *payload;
    uint8_t *canon;
    uint32_t canon_len;
    void *page;
    size_t page_bytes;
} Cell;

typedef struct Carrier {
    int present;
    uint8_t direction;
    uint8_t shape;
    char name[32];
    uint64_t offers;
    uint64_t nothings;
    void *driver;
} Carrier;

typedef struct Held {
    int live;
    uint32_t slot;
    uint32_t left;
} Held;

struct Machine {
    Cell cells[MAX_CELLS];
    uint32_t ncells;
    uint32_t channel;
    uint32_t frame[MAX_FRAME];
    uint32_t nframe;
    Held held[MAX_HELD];
    Carrier carriers[MAX_CARRIERS];
    uint32_t ncarriers;
    HostFacts facts;
    uint32_t depth;
    int event_r;
    int event_w;
    int fault_ready;
};

static void wr_u32(uint8_t **p, uint32_t v);
static int rd_u32(const uint8_t **p, const uint8_t *end, uint32_t *v);

static uint64_t hash_byte(uint64_t h, uint8_t b) {
    h ^= b;
    h *= HASH_PRIME;
    return h;
}

static uint64_t hash_bytes(uint64_t h, const void *p, uint32_t n) {
    const uint8_t *b = p;
    uint32_t i;
    for (i = 0; i < n; i++) h = hash_byte(h, b[i]);
    return h;
}

uint64_t word_nothing(void) { return 2; }

uint64_t word_state(int bit) {
    if (bit != 0 && bit != 1) return word_nothing();
    return (uint64_t)bit;
}

uint64_t word_node(uint32_t slot) {
    return ((uint64_t)slot << 2) | 3ull;
}

int word_class(uint64_t word) {
    if (word == 0 || word == 1) return CLASS_STATE;
    if (word == 2) return CLASS_NOTHING;
    if ((word & 3ull) == 3ull && (word >> 2) != 0) return CLASS_NODE;
    return -1;
}

uint64_t word_payload(uint64_t word) {
    if (word_class(word) == CLASS_NODE) return word >> 2;
    return word;
}

static Outcome arrived_ack(uint64_t ack) {
    Outcome o;
    o.arrived = 1;
    o.ack = ack;
    return o;
}

static Outcome held_out(void) {
    Outcome o;
    o.arrived = 0;
    o.ack = 0;
    return o;
}

static Outcome ack_nothing(void) { return arrived_ack(word_nothing()); }

static void frame_push(Machine *m, int64_t phase, int64_t nbytes, int64_t dur, int64_t cls);

static Outcome ack_breach(Machine *m, uint32_t why) {
    (void)why;
    /* The membrane answers 0. The recipient was not entered. */
    frame_push(m, 4, 0, 0, CLASS_STATE);
    return arrived_ack(word_state(0));
}

const char *boot_arch_name(int arch) {
    if (arch == 1) return "aarch64";
    if (arch == 2) return "x86_64";
    return "unknown";
}

static void carriers_init(Machine *m);

static void discover(Machine *m) {
    long page;
    struct rlimit rl;
    memset(&m->facts, 0, sizeof m->facts);
    page = sysconf(_SC_PAGESIZE);
    m->facts.page_size = page > 0 ? page : 0;
    if (getrlimit(RLIMIT_STACK, &rl) == 0 && rl.rlim_cur != RLIM_INFINITY)
        m->facts.max_depth = (uint64_t)rl.rlim_cur / 8192ull;
    if (m->facts.max_depth < 32) m->facts.max_depth = 32;
#if defined(__aarch64__) || defined(__arm64__)
    m->facts.arch = 1;
#elif defined(__x86_64__)
    m->facts.arch = 2;
#else
    m->facts.arch = 0;
#endif
    m->facts.clock_present = 1;
    m->facts.entropy_present = access("/dev/urandom", R_OK) == 0;
    m->event_r = -1;
    m->event_w = -1;
    {
        int p[2];
        if (pipe(p) == 0) {
            m->event_r = p[0];
            m->event_w = p[1];
        }
    }
}

void machine_facts(const Machine *m, HostFacts *out) { *out = m->facts; }

Machine *machine_open(void) {
    Machine *m = calloc(1, sizeof *m);
    if (!m) return 0;
    discover(m);
    m->ncells = 1; /* slot 0 is never a cell */
    carriers_init(m);
    return m;
}

static void unmap_leaf(Cell *c) {
    if (!c->page || !c->page_bytes) return;
    munmap(c->page, c->page_bytes);
    c->page = 0;
    c->page_bytes = 0;
}

void machine_close(Machine *m) {
    uint32_t i;
    if (!m) return;
    for (i = 1; i < m->ncells; i++) {
        unmap_leaf(&m->cells[i]);
        free(m->cells[i].payload);
        free(m->cells[i].canon);
    }
    if (m->event_r >= 0) close(m->event_r);
    if (m->event_w >= 0) close(m->event_w);
    if (m->facts.gpu_present) {
        /* driver handle lives on the gpu carrier, closed with the process */
    }
    free(m);
}

uint32_t cell_compose(Machine *m, uint32_t kind,
                      const uint32_t *children, uint32_t nchild,
                      const void *payload, uint32_t nbytes) {
    uint64_t index;
    uint32_t i;
    uint32_t slot;
    uint32_t total;
    uint8_t *canon;
    uint8_t *p;
    Cell *c;
    if (!m || kind == 0 || nchild > MAX_CHILDREN || nbytes > MAX_PAYLOAD) return 0;
    total = 12 + nbytes;
    for (i = 0; i < nchild; i++) {
        if (!children[i] || children[i] >= m->ncells || !m->cells[children[i]].live) return 0;
        if (!m->cells[children[i]].canon) return 0;
        if (total > (4u << 20) - 4u - m->cells[children[i]].canon_len) return 0;
        total += 4 + m->cells[children[i]].canon_len;
    }
    canon = malloc(total);
    if (!canon) return 0;
    p = canon;
    wr_u32(&p, kind);
    wr_u32(&p, nchild);
    wr_u32(&p, nbytes);
    for (i = 0; i < nchild; i++) {
        Cell *ch = &m->cells[children[i]];
        wr_u32(&p, ch->canon_len);
        memcpy(p, ch->canon, ch->canon_len);
        p += ch->canon_len;
    }
    if (nbytes) memcpy(p, payload, nbytes);
    index = hash_bytes(HASH_OFF, canon, total);
    for (i = 1; i < m->ncells; i++) {
        if (!m->cells[i].live || m->cells[i].canon_len != total) continue;
        if (memcmp(m->cells[i].canon, canon, total) != 0) continue;
        free(canon);
        return i;
    }
    (void)index;
    if (m->ncells >= MAX_CELLS) {
        free(canon);
        return 0;
    }
    slot = m->ncells++;
    c = &m->cells[slot];
    memset(c, 0, sizeof *c);
    c->id = index;
    c->kind = kind;
    c->nchild = nchild;
    c->nbytes = nbytes;
    c->live = 1;
    c->canon = canon;
    c->canon_len = total;
    for (i = 0; i < nchild; i++) {
        c->child[i] = children[i];
        m->cells[children[i]].refs++;
    }
    if (nbytes) {
        c->payload = malloc(nbytes);
        if (!c->payload) {
            c->live = 0;
            free(canon);
            c->canon = 0;
            return 0;
        }
        memcpy(c->payload, payload, nbytes);
    }
    return slot;
}

void cell_pin(Machine *m, uint32_t slot) {
    if (slot && slot < m->ncells && m->cells[slot].live) m->cells[slot].refs++;
}

void cell_unpin(Machine *m, uint32_t slot) {
    if (slot && slot < m->ncells && m->cells[slot].live && m->cells[slot].refs) m->cells[slot].refs--;
}

static void release_slot(Machine *m, uint32_t slot) {
    Cell *c;
    uint32_t i;
    uint32_t kids[MAX_CHILDREN];
    uint32_t n;
    if (!slot || slot >= m->ncells) return;
    c = &m->cells[slot];
    if (!c->live || c->refs) return;
    n = c->nchild;
    for (i = 0; i < n; i++) kids[i] = c->child[i];
    c->live = 0;
    unmap_leaf(c);
    free(c->payload);
    c->payload = 0;
    free(c->canon);
    c->canon = 0;
    c->canon_len = 0;
    c->nchild = 0;
    for (i = 0; i < n; i++) {
        if (m->cells[kids[i]].refs) m->cells[kids[i]].refs--;
        release_slot(m, kids[i]);
    }
}

void machine_release_unpinned(Machine *m) {
    uint32_t i;
    for (i = 1; i < m->ncells; i++) release_slot(m, i);
}

int cell_live(const Machine *m, uint32_t slot) {
    return slot && slot < m->ncells && m->cells[slot].live;
}

uint64_t cell_id(const Machine *m, uint32_t slot) {
    return cell_live(m, slot) ? m->cells[slot].id : 0;
}

uint32_t cell_kind(const Machine *m, uint32_t slot) {
    return cell_live(m, slot) ? m->cells[slot].kind : 0;
}

uint32_t cell_nchild(const Machine *m, uint32_t slot) {
    return cell_live(m, slot) ? m->cells[slot].nchild : 0;
}

uint32_t cell_child(const Machine *m, uint32_t slot, uint32_t i) {
    if (!cell_live(m, slot) || i >= m->cells[slot].nchild) return 0;
    return m->cells[slot].child[i];
}

uint32_t cell_payload(const Machine *m, uint32_t slot, const uint8_t **ptr) {
    if (!cell_live(m, slot)) {
        *ptr = 0;
        return 0;
    }
    *ptr = m->cells[slot].payload;
    return m->cells[slot].nbytes;
}

uint32_t cell_canon(const Machine *m, uint32_t slot, const uint8_t **ptr) {
    if (!cell_live(m, slot) || !m->cells[slot].canon) {
        *ptr = 0;
        return 0;
    }
    *ptr = m->cells[slot].canon;
    return m->cells[slot].canon_len;
}

int cell_i64(const Machine *m, uint32_t slot, int64_t *out) {
    const uint8_t *p;
    uint32_t n;
    uint64_t v = 0;
    uint32_t i;
    n = cell_payload(m, slot, &p);
    if (cell_kind(m, slot) != KIND_SCALAR || n != 8) return 0;
    for (i = 0; i < 8; i++) v |= (uint64_t)p[i] << (8 * i);
    *out = (int64_t)v;
    return 1;
}

uint32_t cell_find(const Machine *m, uint64_t id) {
    uint32_t i;
    for (i = 1; i < m->ncells; i++)
        if (m->cells[i].live && m->cells[i].id == id) return i;
    return 0;
}

uint32_t cell_live_count(const Machine *m) {
    uint32_t i, n = 0;
    for (i = 1; i < m->ncells; i++) if (m->cells[i].live) n++;
    return n;
}

uint32_t make_scalar(Machine *m, int64_t n) {
    uint8_t b[8];
    uint64_t u = (uint64_t)n;
    int i;
    for (i = 0; i < 8; i++) b[i] = (uint8_t)(u >> (8 * i));
    return cell_compose(m, KIND_SCALAR, 0, 0, b, 8);
}

uint32_t make_bytes(Machine *m, const void *p, uint32_t n) {
    return cell_compose(m, KIND_BYTES, 0, 0, p, n);
}

uint32_t make_text(Machine *m, const char *s) {
    return cell_compose(m, KIND_TEXT, 0, 0, s, (uint32_t)strlen(s));
}

uint32_t make_leaf(Machine *m, uint8_t abi, const void *code, uint32_t n) {
    uint8_t *buf;
    uint32_t slot;
    if (abi > 1 || !code || n == 0 || n > MAX_PAYLOAD - 1) return 0;
    buf = malloc((size_t)n + 1);
    if (!buf) return 0;
    buf[0] = abi;
    memcpy(buf + 1, code, n);
    slot = cell_compose(m, KIND_LEAF, 0, 0, buf, n + 1);
    free(buf);
    return slot;
}

uint32_t make_export(Machine *m, uint32_t name, uint32_t leaf) {
    uint32_t kids[2];
    kids[0] = name;
    kids[1] = leaf;
    return cell_compose(m, KIND_EXPORT, kids, 2, 0, 0);
}

uint32_t make_port(Machine *m, uint8_t direction, uint8_t shape, const char *name) {
    uint8_t buf[40];
    uint32_t n = (uint32_t)strlen(name);
    if (n > 32) return 0;
    buf[0] = direction;
    buf[1] = shape;
    buf[2] = (uint8_t)n;
    memcpy(buf + 3, name, n);
    return cell_compose(m, KIND_PORT, 0, 0, buf, 3 + n);
}

uint32_t make_channel(Machine *m, const char *name, const uint32_t *children, uint32_t nchild) {
    return cell_compose(m, KIND_CHANNEL, children, nchild, name, (uint32_t)strlen(name));
}

void machine_set_channel(Machine *m, uint32_t slot) {
    if (m->channel) cell_unpin(m, m->channel);
    m->channel = slot;
    cell_pin(m, slot);
}

uint32_t machine_channel(const Machine *m) { return m->channel; }

static Carrier *carrier_lookup(Machine *m, uint8_t direction, uint8_t shape, const char *name, int nlen) {
    uint32_t i;
    for (i = 0; i < m->ncarriers; i++) {
        Carrier *c = &m->carriers[i];
        if (!c->present) continue;
        if (c->direction != direction || c->shape != shape) continue;
        if ((int)strlen(c->name) != nlen) continue;
        if (memcmp(c->name, name, (size_t)nlen) != 0) continue;
        return c;
    }
    return 0;
}

static int carrier_name_known(Machine *m, const char *name, int nlen) {
    uint32_t i;
    for (i = 0; i < m->ncarriers; i++) {
        if ((int)strlen(m->carriers[i].name) == nlen && memcmp(m->carriers[i].name, name, (size_t)nlen) == 0)
            return 1;
    }
    return 0;
}

static void carrier_add(Machine *m, int present, uint8_t dir, uint8_t shape, const char *name) {
    Carrier *c;
    if (m->ncarriers >= MAX_CARRIERS) return;
    c = &m->carriers[m->ncarriers++];
    memset(c, 0, sizeof *c);
    c->present = present;
    c->direction = dir;
    c->shape = shape;
    snprintf(c->name, sizeof c->name, "%s", name);
}

static void carriers_init(Machine *m) {
    void *gpu;
    carrier_add(m, 1, DIR_AFFERENT, SHAPE_SCALAR, "clock.monotonic");
    carrier_add(m, 1, DIR_AFFERENT, SHAPE_SCALAR, "clock.realtime");
    carrier_add(m, m->facts.entropy_present, DIR_AFFERENT, SHAPE_BYTES, "entropy");
    carrier_add(m, 1, DIR_AFFERENT, SHAPE_BYTES, "file.read");
    carrier_add(m, 1, DIR_EFFERENT, SHAPE_BYTES, "file.write");
    carrier_add(m, m->facts.arch != 0 && m->facts.page_size > 0, DIR_EFFERENT, SHAPE_BYTES, "cpu.leaf");
    carrier_add(m, m->event_r >= 0, DIR_AFFERENT, SHAPE_SCALAR, "event.wait");
    carrier_add(m, m->event_w >= 0, DIR_EFFERENT, SHAPE_BOOL, "event.signal");
    carrier_add(m, 1, DIR_EFFERENT, SHAPE_TEXT, "process");
    carrier_add(m, 1, DIR_EFFERENT, SHAPE_BOOL, "identity.eq");
    gpu = dlopen("/System/Library/Frameworks/Metal.framework/Metal", RTLD_LAZY | RTLD_LOCAL);
    m->facts.gpu_present = gpu != 0;
    carrier_add(m, gpu != 0, DIR_AFFERENT, SHAPE_BOOL, "gpu.admit");
    if (gpu) {
        m->carriers[m->ncarriers - 1].driver = gpu;
    }
    /* video, audio, pixel, and key stay unregistered until a driver is discovered. */
}

static uint64_t now_ns(int realtime) {
    struct timespec ts;
    clockid_t id = realtime ? CLOCK_REALTIME : CLOCK_MONOTONIC;
    if (clock_gettime(id, &ts) != 0) return 0;
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static void frame_push(Machine *m, int64_t phase, int64_t nbytes, int64_t dur, int64_t cls) {
    uint32_t kids[4];
    uint32_t row;
    if (m->nframe >= MAX_FRAME) return;
    kids[0] = make_scalar(m, phase);
    kids[1] = make_scalar(m, nbytes);
    kids[2] = make_scalar(m, dur);
    kids[3] = make_scalar(m, cls);
    if (!kids[0] || !kids[1] || !kids[2] || !kids[3]) return;
    row = cell_compose(m, KIND_FRAME, kids, 4, 0, 0);
    if (!row) return;
    cell_pin(m, row);
    m->frame[m->nframe++] = row;
}

uint32_t frame_count(const Machine *m) { return m->nframe; }
uint32_t frame_row(const Machine *m, uint32_t i) { return i < m->nframe ? m->frame[i] : 0; }

static int port_fields(const Machine *m, uint32_t slot, uint8_t *dir, uint8_t *shape, const char **name, int *nlen) {
    const uint8_t *p;
    uint32_t n = cell_payload(m, slot, &p);
    if (cell_kind(m, slot) != KIND_PORT || n < 3) return 0;
    if ((uint32_t)(3 + p[2]) > n) return 0;
    *dir = p[0];
    *shape = p[1];
    *nlen = p[2];
    *name = (const char *)(p + 3);
    return 1;
}

static Outcome run_carrier(Machine *m, uint32_t port, const uint32_t *args, uint32_t nargs);

static int64_t arg_bytes_len(Machine *m, uint32_t slot) {
    const uint8_t *p;
    return (int64_t)cell_payload(m, slot, &p);
}

static Outcome offer_inner(Machine *m, uint32_t slot, const uint32_t *args, uint32_t nargs);

Outcome offer(Machine *m, uint32_t slot, const uint32_t *args, uint32_t nargs) {
    uint64_t t0 = now_ns(0);
    Outcome o;
    int64_t nbytes = 0;
    uint32_t i;
    if (!m || !cell_live(m, slot)) return ack_breach(m, BREACH_SHAPE);
    for (i = 0; i < nargs; i++) nbytes += arg_bytes_len(m, args[i]);
    o = offer_inner(m, slot, args, nargs);
    if (m->depth == 0) {
        int cls = o.arrived ? word_class(o.ack) : 3;
        frame_push(m, 1, nbytes, (int64_t)(now_ns(0) - t0), cls);
    }
    return o;
}

static void *map_executable(Machine *m, const uint8_t *code, uint32_t n, size_t *mapped) {
    size_t page = (size_t)m->facts.page_size;
    size_t bytes;
    void *mem;
    if (!page || n == 0 || n > page) return 0;
    bytes = page;
    mem = mmap(0, bytes, PROT_READ | PROT_WRITE | PROT_EXEC,
#if defined(MAP_JIT)
               MAP_PRIVATE | MAP_ANON | MAP_JIT,
#else
               MAP_PRIVATE | MAP_ANON,
#endif
               -1, 0);
    if (mem == MAP_FAILED) return 0;
#if defined(__APPLE__) && (defined(__aarch64__) || defined(__arm64__))
    pthread_jit_write_protect_np(0);
#endif
    memcpy(mem, code, n);
#if defined(__APPLE__) && (defined(__aarch64__) || defined(__arm64__))
    pthread_jit_write_protect_np(1);
    sys_icache_invalidate(mem, n);
#else
    __builtin___clear_cache((char *)mem, (char *)mem + n);
#endif
    *mapped = bytes;
    return mem;
}

static Machine *call_m;

static uint64_t leaf_offer(uint64_t target, const uint64_t *args, uint64_t argc) {
    uint32_t slots[8];
    uint64_t i;
    Outcome o;
    if (!call_m || word_class(target) != CLASS_NODE || argc > 8) return word_state(0);
    for (i = 0; i < argc; i++) {
        if (word_class(args[i]) != CLASS_NODE) return word_state(0);
        slots[i] = (uint32_t)word_payload(args[i]);
    }
    o = offer(call_m, (uint32_t)word_payload(target), slots, (uint32_t)argc);
    if (!o.arrived) return word_nothing();
    return o.ack;
}

static int map_leaf(Machine *m, uint32_t slot) {
    const uint8_t *pay;
    uint32_t n = cell_payload(m, slot, &pay);
    Cell *c = &m->cells[slot];
    if (n < 2 || pay[0] > 1) return 0;
    if (c->page) return 1;
    c->page = map_executable(m, pay + 1, n - 1, &c->page_bytes);
    return c->page != 0;
}

static Outcome enter_leaf(Machine *m, uint32_t slot, const uint32_t *args, uint32_t nargs) {
    const uint8_t *pay;
    Cell *c;
    if (cell_kind(m, slot) != KIND_LEAF) return ack_breach(m, BREACH_SHAPE);
    if (!cell_payload(m, slot, &pay)) return ack_nothing();
    if (!map_leaf(m, slot)) return ack_nothing();
    c = &m->cells[slot];
    if (pay[0] == 0) {
        int64_t a[4];
        int64_t got;
        uint32_t i;
        uint32_t out;
        typedef int64_t (*Fn)(int64_t, int64_t, int64_t, int64_t);
        if (nargs > 4) return ack_breach(m, BREACH_SHAPE);
        for (i = 0; i < 4; i++) a[i] = 0;
        for (i = 0; i < nargs; i++)
            if (!cell_i64(m, args[i], &a[i])) return ack_breach(m, BREACH_SHAPE);
        got = ((Fn)c->page)(a[0], a[1], a[2], a[3]);
        out = make_scalar(m, got);
        if (!out) return ack_nothing();
        return arrived_ack(word_node(out));
    }
    if (pay[0] == 1) {
        uint64_t words[8];
        uint64_t got;
        uint32_t i;
        Machine *prev;
        typedef uint64_t (*OfferFn)(uint64_t, const uint64_t *, uint64_t);
        typedef uint64_t (*Leaf1)(const uint64_t *, uint64_t, OfferFn);
        if (nargs > 8) return ack_breach(m, BREACH_SHAPE);
        for (i = 0; i < nargs; i++) {
            int64_t bit;
            if (cell_i64(m, args[i], &bit) && (bit == 0 || bit == 1)) words[i] = word_state((int)bit);
            else words[i] = word_node(args[i]);
        }
        prev = call_m;
        call_m = m;
        got = ((Leaf1)c->page)(words, nargs, leaf_offer);
        call_m = prev;
        if (word_class(got) < 0) return ack_breach(m, BREACH_SHAPE);
        return arrived_ack(got);
    }
    return ack_nothing();
}

static Outcome cpu_leaf(Machine *m, const uint32_t *args, uint32_t nargs) {
    const uint8_t *code;
    uint32_t n;
    uint32_t slot;
    uint32_t i;
    if (nargs < 1 || nargs > 5 || cell_kind(m, args[0]) != KIND_BYTES) return ack_breach(m, BREACH_SHAPE);
    for (i = 1; i < nargs; i++) {
        int64_t ignore;
        if (!cell_i64(m, args[i], &ignore)) return ack_breach(m, BREACH_SHAPE);
    }
    n = cell_payload(m, args[0], &code);
    slot = make_leaf(m, 0, code, n);
    if (!slot) return ack_nothing();
    if (nargs == 1) return arrived_ack(word_node(slot));
    return enter_leaf(m, slot, args + 1, nargs - 1);
}

static Outcome file_read(Machine *m, const uint32_t *args, uint32_t nargs) {
    const uint8_t *path;
    uint32_t npath;
    char buf[512];
    uint8_t *body = 0;
    size_t cap = 0, len = 0;
    int fd;
    if (nargs != 1 || cell_kind(m, args[0]) != KIND_TEXT) return ack_breach(m, BREACH_SHAPE);
    npath = cell_payload(m, args[0], &path);
    if (npath >= sizeof buf) return ack_breach(m, BREACH_SHAPE);
    memcpy(buf, path, npath);
    buf[npath] = 0;
    fd = open(buf, O_RDONLY);
    if (fd < 0) return ack_nothing();
    for (;;) {
        uint8_t tmp[4096];
        ssize_t r = read(fd, tmp, sizeof tmp);
        uint8_t *next;
        if (r == 0) break;
        if (r < 0) {
            close(fd);
            free(body);
            return ack_nothing();
        }
        next = realloc(body, len + (size_t)r);
        if (!next) {
            close(fd);
            free(body);
            return ack_nothing();
        }
        body = next;
        memcpy(body + len, tmp, (size_t)r);
        len += (size_t)r;
        cap = len;
        (void)cap;
    }
    close(fd);
    {
        uint32_t slot = cell_compose(m, KIND_BYTES, 0, 0, body, (uint32_t)len);
        free(body);
        if (!slot) return ack_nothing();
        return arrived_ack(word_node(slot));
    }
}

static Outcome file_write(Machine *m, const uint32_t *args, uint32_t nargs) {
    const uint8_t *path;
    const uint8_t *body;
    uint32_t npath, nbody;
    char final[512], tmp[520];
    int fd;
    if (nargs != 2 || cell_kind(m, args[0]) != KIND_TEXT || cell_kind(m, args[1]) != KIND_BYTES)
        return ack_breach(m, BREACH_SHAPE);
    npath = cell_payload(m, args[0], &path);
    nbody = cell_payload(m, args[1], &body);
    if (npath == 0 || npath >= sizeof final) return ack_breach(m, BREACH_SHAPE);
    memcpy(final, path, npath);
    final[npath] = 0;
    snprintf(tmp, sizeof tmp, "%s.tmp", final);
    fd = open(tmp, O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (fd < 0) return ack_nothing();
    if (write(fd, body, nbody) != (ssize_t)nbody || fsync(fd) != 0) {
        close(fd);
        unlink(tmp);
        return ack_nothing();
    }
    if (close(fd) != 0) {
        unlink(tmp);
        return ack_nothing();
    }
    if (rename(tmp, final) != 0) {
        unlink(tmp);
        return ack_nothing();
    }
    return arrived_ack(word_state(1));
}

static Outcome clock_port(Machine *m, int realtime, uint32_t nargs) {
    uint32_t count;
    if (nargs != 0) return ack_breach(m, BREACH_SHAPE);
    count = make_scalar(m, (int64_t)now_ns(realtime));
    uint32_t domain = make_text(m, realtime ? "realtime-ns" : "monotonic-ns");
    uint32_t kids[2];
    uint32_t slot;
    if (!count || !domain) return ack_nothing();
    kids[0] = count;
    kids[1] = domain;
    slot = cell_compose(m, KIND_READING, kids, 2, 0, 0);
    if (!slot) return ack_nothing();
    return arrived_ack(word_node(slot));
}

static Outcome entropy_port(Machine *m, const uint32_t *args, uint32_t nargs) {
    int64_t n = 16;
    int fd;
    uint8_t buf[32];
    uint32_t slot;
    if (!m->facts.entropy_present) return ack_nothing();
    if (nargs == 1) {
        if (!cell_i64(m, args[0], &n)) return ack_breach(m, BREACH_SHAPE);
    } else if (nargs != 0) return ack_breach(m, BREACH_SHAPE);
    if (n < 1 || n > 32) return ack_breach(m, BREACH_SHAPE);
    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) return ack_nothing();
    if (read(fd, buf, (size_t)n) != (ssize_t)n) {
        close(fd);
        return ack_nothing();
    }
    close(fd);
    slot = make_bytes(m, buf, (uint32_t)n);
    if (!slot) return ack_nothing();
    return arrived_ack(word_node(slot));
}

static Outcome identity_eq(Machine *m, const uint32_t *args, uint32_t nargs) {
    const uint8_t *a;
    const uint8_t *b;
    uint32_t na;
    uint32_t nb;
    if (nargs != 2 || !cell_live(m, args[0]) || !cell_live(m, args[1])) return ack_breach(m, BREACH_SHAPE);
    na = cell_canon(m, args[0], &a);
    nb = cell_canon(m, args[1], &b);
    if (!na || !nb) return ack_nothing();
    return arrived_ack(word_state(na == nb && memcmp(a, b, na) == 0));
}

static Outcome event_wait(Machine *m, const uint32_t *args, uint32_t nargs) {
    int64_t ns = 0;
    struct pollfd p;
    int r;
    if (m->event_r < 0) return ack_nothing();
    if (nargs == 1) {
        if (!cell_i64(m, args[0], &ns)) return ack_breach(m, BREACH_SHAPE);
    } else return ack_breach(m, BREACH_SHAPE);
    if (ns < 0) return ack_breach(m, BREACH_SHAPE);
    p.fd = m->event_r;
    p.events = POLLIN;
    r = poll(&p, 1, 0);
    if (r < 0) return ack_nothing();
    if (r == 0) return ns == 0 ? ack_nothing() : held_out();
    {
        char bit;
        if (read(m->event_r, &bit, 1) != 1) return ack_nothing();
    }
    return arrived_ack(word_state(1));
}

static Outcome event_signal(Machine *m, const uint32_t *args, uint32_t nargs) {
    char bit = 1;
    (void)args;
    if (m->event_w < 0) return ack_nothing();
    if (nargs != 0) return ack_breach(m, BREACH_SHAPE);
    if (write(m->event_w, &bit, 1) != 1) return ack_nothing();
    return arrived_ack(word_state(1));
}

static Outcome process_port(Machine *m, const uint32_t *args, uint32_t nargs) {
    const uint8_t *path;
    uint32_t npath;
    char file[512];
    pid_t pid;
    int status = 0;
    uint32_t i;
    char *argv[8];
    char storage[8][256];
    (void)m;
    if (nargs < 1 || nargs > 8 || cell_kind(m, args[0]) != KIND_TEXT) return ack_breach(m, BREACH_SHAPE);
    npath = cell_payload(m, args[0], &path);
    if (npath == 0 || npath >= sizeof file) return ack_breach(m, BREACH_SHAPE);
    memcpy(file, path, npath);
    file[npath] = 0;
    for (i = 0; i < nargs; i++) {
        const uint8_t *p;
        uint32_t n = cell_payload(m, args[i], &p);
        if (cell_kind(m, args[i]) != KIND_TEXT || n >= sizeof storage[i]) return ack_breach(m, BREACH_SHAPE);
        memcpy(storage[i], p, n);
        storage[i][n] = 0;
        argv[i] = storage[i];
    }
    argv[nargs] = 0;
    pid = fork();
    if (pid < 0) return ack_nothing();
    if (pid == 0) {
        execv(file, argv);
        _exit(127);
    }
    if (waitpid(pid, &status, 0) < 0) return ack_nothing();
    if (WIFEXITED(status)) {
        uint32_t s = make_scalar(m, WEXITSTATUS(status));
        if (!s) return ack_nothing();
        return arrived_ack(word_node(s));
    }
    return ack_nothing();
}

static Outcome gpu_admit(Machine *m) {
    if (!m->facts.gpu_present) return ack_nothing();
    return arrived_ack(word_state(1));
}

static Outcome run_carrier(Machine *m, uint32_t port, const uint32_t *args, uint32_t nargs) {
    uint8_t dir, shape;
    const char *name;
    int nlen;
    Carrier *c;
    uint64_t t0 = now_ns(0);
    Outcome o;
    int64_t nbytes = 0;
    uint32_t i;
    if (!port_fields(m, port, &dir, &shape, &name, &nlen)) return ack_breach(m, BREACH_SHAPE);
    for (i = 0; i < nargs; i++) nbytes += arg_bytes_len(m, args[i]);
    if (carrier_name_known(m, name, nlen)) {
        c = carrier_lookup(m, dir, shape, name, nlen);
        if (!c) return ack_breach(m, BREACH_SHAPE);
    } else {
        c = 0;
    }
    if (!c || !c->present) {
        if (c) c->nothings++;
        frame_push(m, 3, nbytes, (int64_t)(now_ns(0) - t0), CLASS_NOTHING);
        return ack_nothing();
    }
    c->offers++;
    if (strcmp(c->name, "clock.monotonic") == 0) o = clock_port(m, 0, nargs);
    else if (strcmp(c->name, "clock.realtime") == 0) o = clock_port(m, 1, nargs);
    else if (strcmp(c->name, "entropy") == 0) o = entropy_port(m, args, nargs);
    else if (strcmp(c->name, "file.read") == 0) o = file_read(m, args, nargs);
    else if (strcmp(c->name, "file.write") == 0) o = file_write(m, args, nargs);
    else if (strcmp(c->name, "cpu.leaf") == 0) o = cpu_leaf(m, args, nargs);
    else if (strcmp(c->name, "event.wait") == 0) o = event_wait(m, args, nargs);
    else if (strcmp(c->name, "event.signal") == 0) o = event_signal(m, args, nargs);
    else if (strcmp(c->name, "process") == 0) o = process_port(m, args, nargs);
    else if (strcmp(c->name, "gpu.admit") == 0) o = gpu_admit(m);
    else if (strcmp(c->name, "identity.eq") == 0) o = identity_eq(m, args, nargs);
    else o = ack_nothing();
    if (o.arrived && word_class(o.ack) == CLASS_NOTHING) c->nothings++;
    frame_push(m, 3, nbytes, (int64_t)(now_ns(0) - t0), o.arrived ? word_class(o.ack) : 3);
    return o;
}

static Outcome offer_inner(Machine *m, uint32_t slot, const uint32_t *args, uint32_t nargs) {
    uint32_t kind;
    if (m->depth >= m->facts.max_depth) return ack_nothing();
    if (!cell_live(m, slot)) return ack_breach(m, BREACH_SHAPE);
    kind = cell_kind(m, slot);
    m->depth++;
    if (kind == KIND_SCALAR || kind == KIND_BYTES || kind == KIND_TEXT || kind == KIND_READING || kind == KIND_FRAME) {
        m->depth--;
        if (nargs) return ack_breach(m, BREACH_SHAPE);
        return arrived_ack(word_node(slot));
    }
    if (kind == KIND_PORT) {
        Outcome o = run_carrier(m, slot, args, nargs);
        m->depth--;
        return o;
    }
    if (kind == KIND_LEAF || kind == KIND_EXPORT) {
        Outcome o;
        uint32_t leaf = slot;
        if (kind == KIND_EXPORT) {
            if (cell_nchild(m, slot) < 2) {
                m->depth--;
                return ack_breach(m, BREACH_SHAPE);
            }
            leaf = cell_child(m, slot, 1);
        }
        o = enter_leaf(m, leaf, args, nargs);
        m->depth--;
        return o;
    }
    if (kind == KIND_CHANNEL) {
        if (nargs == 0) {
            uint8_t seen[MAX_CELLS];
            int ok = 1;
            uint32_t stack[MAX_CELLS];
            uint32_t nstack = 0;
            memset(seen, 0, sizeof seen);
            stack[nstack++] = slot;
            while (nstack) {
                uint32_t cur = stack[--nstack];
                uint32_t ci;
                if (!cur || seen[cur]) continue;
                seen[cur] = 1;
                if (cell_kind(m, cur) == KIND_LEAF && !map_leaf(m, cur)) ok = 0;
                for (ci = 0; ci < cell_nchild(m, cur); ci++) {
                    uint32_t ch = cell_child(m, cur, ci);
                    if (ch && !seen[ch] && nstack < MAX_CELLS) stack[nstack++] = ch;
                }
            }
            m->depth--;
            return ok ? arrived_ack(word_state(1)) : ack_nothing();
        }
        {
            uint32_t kids[MAX_CHILDREN];
            uint32_t nc = cell_nchild(m, slot);
            uint32_t i;
            uint32_t born;
            const uint8_t *name;
            uint32_t nname;
            if (nc + nargs > MAX_CHILDREN) {
                m->depth--;
                return ack_breach(m, BREACH_SHAPE);
            }
            for (i = 0; i < nc; i++) kids[i] = cell_child(m, slot, i);
            for (i = 0; i < nargs; i++) kids[nc + i] = args[i];
            nname = cell_payload(m, slot, &name);
            born = cell_compose(m, KIND_CHANNEL, kids, nc + nargs, name, nname);
            m->depth--;
            if (!born) return ack_nothing();
            if (m->channel == slot) machine_set_channel(m, born);
            return arrived_ack(word_node(born));
        }
    }
    m->depth--;
    return ack_breach(m, BREACH_SHAPE);
}

int carrier_health(const Machine *m, const char *name, uint64_t *offers, uint64_t *nothings) {
    uint32_t i;
    for (i = 0; i < m->ncarriers; i++) {
        if (strcmp(m->carriers[i].name, name) == 0) {
            *offers = m->carriers[i].offers;
            *nothings = m->carriers[i].nothings;
            return m->carriers[i].present;
        }
    }
    *offers = 0;
    *nothings = 0;
    return 0;
}

/* Image: magic FBOOT2, then little-endian records. A mismatch refuses the
 * whole thaw. Child links in the file are content identities, never slots. */

static void wr_u32(uint8_t **p, uint32_t v) {
    uint8_t *d = *p;
    d[0] = (uint8_t)v;
    d[1] = (uint8_t)(v >> 8);
    d[2] = (uint8_t)(v >> 16);
    d[3] = (uint8_t)(v >> 24);
    *p += 4;
}

static void wr_u64(uint8_t **p, uint64_t v) {
    int i;
    for (i = 0; i < 8; i++) *(*p)++ = (uint8_t)(v >> (8 * i));
}

static int rd_u32(const uint8_t **p, const uint8_t *end, uint32_t *v) {
    const uint8_t *d = *p;
    if (end - d < 4) return 0;
    *v = (uint32_t)d[0] | ((uint32_t)d[1] << 8) | ((uint32_t)d[2] << 16) | ((uint32_t)d[3] << 24);
    *p += 4;
    return 1;
}

static int rd_u64(const uint8_t **p, const uint8_t *end, uint64_t *v) {
    int i;
    if (end - *p < 8) return 0;
    *v = 0;
    for (i = 0; i < 8; i++) *v |= (uint64_t)(*p)[i] << (8 * i);
    *p += 8;
    return 1;
}

static uint32_t parse_canon(Machine *m, const uint8_t **p, const uint8_t *end) {
    const uint8_t *start = *p;
    uint32_t kind, nchild, nbytes, i;
    uint32_t kids[MAX_CHILDREN];
    const uint8_t *payload;
    uint32_t slot;
    const uint8_t *got;
    uint32_t gn;
    if (!rd_u32(p, end, &kind) || !rd_u32(p, end, &nchild) || !rd_u32(p, end, &nbytes)) return 0;
    if (kind == 0 || nchild > MAX_CHILDREN || nbytes > MAX_PAYLOAD) return 0;
    for (i = 0; i < nchild; i++) {
        uint32_t clen;
        const uint8_t *cend;
        if (!rd_u32(p, end, &clen)) return 0;
        if ((size_t)(end - *p) < clen) return 0;
        cend = *p + clen;
        kids[i] = parse_canon(m, p, cend);
        if (!kids[i] || *p != cend) return 0;
    }
    if ((size_t)(end - *p) < nbytes) return 0;
    payload = *p;
    *p += nbytes;
    slot = cell_compose(m, kind, nchild ? kids : 0, nchild, nbytes ? payload : 0, nbytes);
    if (!slot) return 0;
    gn = cell_canon(m, slot, &got);
    if (gn != (uint32_t)(*p - start) || !got || memcmp(got, start, gn) != 0) return 0;
    return slot;
}

int freeze(Machine *m, uint8_t **out, size_t *nout) {
    const uint8_t *canon;
    uint32_t n;
    uint8_t *buf;
    uint8_t *p;
    if (!m->channel) return 0;
    n = cell_canon(m, m->channel, &canon);
    if (!n || !canon) return 0;
    buf = malloc(8u + 8u + n + 8u + n);
    if (!buf) return 0;
    p = buf;
    memcpy(p, "FBOOT3\0", 8);
    p += 8;
    wr_u64(&p, n);
    memcpy(p, canon, n);
    p += n;
    wr_u64(&p, n);
    memcpy(p, canon, n);
    p += n;
    *out = buf;
    *nout = (size_t)(p - buf);
    return 1;
}

int thaw(Machine *m, const uint8_t *bytes, size_t n) {
    const uint8_t *p;
    const uint8_t *end;
    const uint8_t *id;
    const uint8_t *graph;
    const uint8_t *cursor;
    const uint8_t *rebuilt;
    uint64_t id_len = 0;
    uint64_t graph_len = 0;
    uint32_t root;
    uint32_t rn;
    uint32_t i;
    Machine *born;
    if (!m || n < 24 || memcmp(bytes, "FBOOT3\0", 8) != 0) return 0;
    p = bytes + 8;
    end = bytes + n;
    if (!rd_u64(&p, end, &id_len) || id_len == 0 || id_len > (4u << 20)) return 0;
    if ((uint64_t)(end - p) < id_len) return 0;
    id = p;
    p += id_len;
    if (!rd_u64(&p, end, &graph_len)) return 0;
    if ((uint64_t)(end - p) != graph_len) return 0;
    graph = p;
    born = machine_open();
    if (!born) return 0;
    cursor = graph;
    root = parse_canon(born, &cursor, graph + graph_len);
    rn = root ? cell_canon(born, root, &rebuilt) : 0;
    if (!root || cursor != graph + graph_len || rn != id_len || !rebuilt ||
        memcmp(rebuilt, id, (size_t)rn) != 0 || cell_kind(born, root) != KIND_CHANNEL) {
        machine_close(born);
        return 0;
    }
    machine_set_channel(born, root);
    {
        int er = m->event_r, ew = m->event_w;
        uint32_t ci;
        for (ci = 0; ci < m->ncarriers; ci++)
            if (strcmp(m->carriers[ci].name, "gpu.admit") == 0) m->carriers[ci].driver = 0;
        if (m->event_r >= 0) close(m->event_r);
        if (m->event_w >= 0) close(m->event_w);
        for (i = 1; i < m->ncells; i++) {
            unmap_leaf(&m->cells[i]);
            free(m->cells[i].payload);
            free(m->cells[i].canon);
        }
        memcpy(m->cells, born->cells, sizeof m->cells);
        m->ncells = born->ncells;
        m->channel = born->channel;
        m->nframe = 0;
        memset(m->held, 0, sizeof m->held);
        memcpy(m->carriers, born->carriers, sizeof m->carriers);
        m->ncarriers = born->ncarriers;
        m->facts = born->facts;
        m->depth = 0;
        m->event_r = born->event_r;
        m->event_w = born->event_w;
        born->event_r = er;
        born->event_w = ew;
        born->ncells = 1;
        for (ci = 0; ci < born->ncarriers; ci++) born->carriers[ci].driver = 0;
        for (i = 1; i < MAX_CELLS; i++) {
            born->cells[i].payload = 0;
            born->cells[i].canon = 0;
            born->cells[i].page = 0;
            born->cells[i].live = 0;
        }
    }
    machine_close(born);
    return 1;
}
static volatile int fault_fd = -1;

static void on_fault(int sig, siginfo_t *info, void *ctx) {
    int s = sig;
    (void)info;
    (void)ctx;
    if (fault_fd >= 0) {
        ssize_t w = write(fault_fd, &s, sizeof s);
        (void)w;
    }
    /* Returning would re-execute the trap and re-enter this handler.
     * Restore the host's default and let that termination stand. */
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
    _exit(128 + sig);
}

void fault_arm(int evidence_fd) {
    stack_t ss;
    struct sigaction sa;
    fault_fd = evidence_fd;
    ss.ss_sp = malloc((size_t)SIGSTKSZ);
    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;
    sigaltstack(&ss, 0);
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = on_fault;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_RESETHAND | SA_NODEFER;
    sigaction(SIGSEGV, &sa, 0);
    sigaction(SIGBUS, &sa, 0);
    sigaction(SIGTRAP, &sa, 0);
    sigaction(SIGILL, &sa, 0);
}

/* carriers_init is called from thaw. A machine built by compose, not thaw,
 * needs it too. */
void machine_arm_carriers(Machine *m) { carriers_init(m); }
