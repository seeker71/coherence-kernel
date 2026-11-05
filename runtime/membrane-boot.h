/* The boot membrane. It offers cells and leaves, and it produces fkwu and
 * form-cli from the same seed the old bootstrap compiles. The products
 * answer the same ground and the same form-cli build.
 *
 * The carrier exists before a parser, a language, or a model. It implements
 * the five axioms on this hardware and nothing else. A function earns its
 * place from an axiom or from a host fact (a page, a trap, a driver). The
 * floor image is cells. Source text is a port an image may read later.
 *
 * Word, 64 bits.
 *   0                          state 0
 *   1                          state 1
 *   2                          nothing
 *   (slot << 2) | 3            a node in this carrier
 * Any other encoding is not a word. Held work is not a word: offer sets
 * arrived=0 and writes no ack. Nothing and state 0 are different words.
 *
 * A cell is immutable. Its identity is the canonical byte string of its
 * kind, its children's canonical strings, and its payload. A digest of those
 * bytes only indexes the store. Equality is the bytes. A second compose of
 * the same bytes returns the same cell. A new payload mints a new identity.
 * Refs come from parent links and from pins. A cell whose refs reach 0
 * returns its storage. A cell a parent still names survives.
 *
 * offer is the only act. The ack is nothing, state 0, state 1, or a node.
 * Timeout and a missing driver are an arrived nothing. An unoffered
 * interface acks state 0 from the membrane and records a breach row. The
 * recipient is not entered, and the ack is not a cell the recipient returned.
 * An outstanding host operation stays open: arrived=0, no ack.
 *
 * C does not evaluate a recipe. Kinds are scalar, bytes, text, port, leaf,
 * export, channel, reading, and frame. A leaf is bytes the image supplied.
 * Publication maps those bytes and does not enter them. A later offer enters
 * the published leaf. The leaf may call back through the offer entry. Branch
 * and hold are that leaf, or an outstanding host flight. They are not opcodes.
 * There is no arithmetic in this file. The page size is discovered. The
 * executable mapping uses the protection this host requires at creation.
 *
 * Ports are (direction, shape, name). Shapes are bool, scalar, bytes, text,
 * pixel, key. Direction is afferent or efferent. Discovery fills the carrier
 * table. An undiscovered name acks nothing. A clock reading is a node whose
 * children are the count and the domain text. Entropy comes from the host
 * device or it is nothing. A GPU admission is the driver's library or it is
 * nothing. The driver's pointer is not a cell identity.
 *
 * Freeze writes the channel's canonical bytes twice: the declared identity,
 * then the graph. Thaw rebuilds the graph and compares its canonical bytes
 * to that identity. A mismatch leaves the caller empty. The first offer of
 * the channel publishes exports and maps leaves. It does not enter them.
 *
 * The frame records membrane crossings: phase, byte count, duration, ack
 * class. It does not copy text or byte payloads. A fault handler writes the
 * signal number to a buffer opened beforehand and returns to the host's own
 * termination. It does not resume the offer.
 */
#ifndef GREENFIELD_BOOT_H
#define GREENFIELD_BOOT_H

#include <stddef.h>
#include <stdint.h>

enum {
    CLASS_NOTHING = 0,
    CLASS_STATE = 1,
    CLASS_NODE = 2
};

enum {
    KIND_SCALAR = 1,
    KIND_BYTES = 2,
    KIND_TEXT = 3,
    KIND_PORT = 5,
    KIND_LEAF = 6,
    KIND_CHANNEL = 7,
    KIND_READING = 8,
    KIND_FRAME = 10,
    KIND_EXPORT = 11
};

enum {
    DIR_AFFERENT = 0,
    DIR_EFFERENT = 1
};

enum {
    SHAPE_BOOL = 1,
    SHAPE_SCALAR = 2,
    SHAPE_BYTES = 3,
    SHAPE_TEXT = 4,
    SHAPE_PIXEL = 5,
    SHAPE_KEY = 6
};

enum {
    BREACH_SHAPE = 1,
    BREACH_CYCLE = 2,
    BREACH_DEPTH = 3,
    BREACH_IMAGE = 4
};

typedef struct Machine Machine;

typedef struct Outcome {
    int arrived;
    uint64_t ack;
} Outcome;

typedef struct HostFacts {
    long page_size;
    int arch; /* 1 aarch64, 2 x86_64, 0 unknown */
    uint64_t max_depth;
    int entropy_present;
    int gpu_present;
    int clock_present;
} HostFacts;

uint64_t word_nothing(void);
uint64_t word_state(int bit);
uint64_t word_node(uint32_t slot);
int word_class(uint64_t word);
uint64_t word_payload(uint64_t word);

Machine *machine_open(void);
void machine_close(Machine *m);
void machine_facts(const Machine *m, HostFacts *out);

uint32_t cell_compose(Machine *m, uint32_t kind,
                      const uint32_t *children, uint32_t nchild,
                      const void *payload, uint32_t nbytes);
void machine_arm_carriers(Machine *m);
void cell_pin(Machine *m, uint32_t slot);
void cell_unpin(Machine *m, uint32_t slot);
void machine_release_unpinned(Machine *m);
int cell_live(const Machine *m, uint32_t slot);
uint64_t cell_id(const Machine *m, uint32_t slot);
uint32_t cell_kind(const Machine *m, uint32_t slot);
uint32_t cell_nchild(const Machine *m, uint32_t slot);
uint32_t cell_child(const Machine *m, uint32_t slot, uint32_t i);
uint32_t cell_payload(const Machine *m, uint32_t slot, const uint8_t **ptr);
uint32_t cell_canon(const Machine *m, uint32_t slot, const uint8_t **ptr);
int cell_i64(const Machine *m, uint32_t slot, int64_t *out);
uint32_t cell_find(const Machine *m, uint64_t id);
uint32_t cell_live_count(const Machine *m);

uint32_t make_scalar(Machine *m, int64_t n);
uint32_t make_bytes(Machine *m, const void *p, uint32_t n);
uint32_t make_text(Machine *m, const char *s);
uint32_t make_leaf(Machine *m, uint8_t abi, const void *code, uint32_t n);
uint32_t make_export(Machine *m, uint32_t name, uint32_t leaf);
uint32_t make_port(Machine *m, uint8_t direction, uint8_t shape, const char *name);
uint32_t make_channel(Machine *m, const char *name, const uint32_t *children, uint32_t nchild);

void machine_set_channel(Machine *m, uint32_t slot);
uint32_t machine_channel(const Machine *m);

Outcome offer(Machine *m, uint32_t slot, const uint32_t *args, uint32_t nargs);

int freeze(Machine *m, uint8_t **out, size_t *n);
int thaw(Machine *m, const uint8_t *bytes, size_t n);

uint32_t frame_count(const Machine *m);
uint32_t frame_row(const Machine *m, uint32_t i);

int carrier_health(const Machine *m, const char *name, uint64_t *offers, uint64_t *nothings);

void fault_arm(int evidence_fd);
const char *boot_arch_name(int arch);

#endif
