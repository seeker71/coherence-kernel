/* kernel.c — HatiOS: a small, honest, 100% native operating system.
 *
 * Freestanding C on bare i386 metal (qemu-witnessed): no libc, no host OS,
 * nothing beneath this file and boot.S but the CPU.
 *
 * What it truly has (and its band of honesty about what it does not):
 *   - drivers: VGA text console + 16550 serial (COM1) — serial is the
 *     witness channel and the shell's terminal
 *   - interrupts: IDT, remapped 8259 PICs, PIT timer at 100 Hz
 *   - PREEMPTIVE round-robin scheduler: real per-task stacks, context
 *     switched by the timer interrupt through switch_ctx (see entry.S)
 *   - memory: page-granular physical allocator (bitmap over 1 MB..8 MB)
 *   - ramfs: a small in-memory filesystem (name -> bytes), no persistence
 *   - shell: help ps mem ls cat write echo uptime spin halt
 *
 * Honest boundaries, named: single CPU, ring 0 only (no user/kernel
 * privilege split), no paging/virtual memory, no disk filesystem, ramfs
 * only. Each is a floor, not a ceiling — the shape of the next stone is
 * visible from here.
 */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

/* ── port I/O ─────────────────────────────────────────────────────────── */
static inline void outb(u16 p, u8 v) { __asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p)); }
static inline u8  inb(u16 p) { u8 v; __asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p)); return v; }

/* ── VGA text console ─────────────────────────────────────────────────── */
static volatile u16 *vga = (u16 *)0xB8000;
static int vrow = 0, vcol = 0;
static void vga_putc(char c) {
    if (c == '\n') { vrow++; vcol = 0; }
    else { vga[vrow * 80 + vcol] = (u16)((0x0F << 8) | (u8)c); vcol++; if (vcol >= 80) { vcol = 0; vrow++; } }
    if (vrow >= 25) {
        for (int r = 1; r < 25; r++)
            for (int cx = 0; cx < 80; cx++) vga[(r - 1) * 80 + cx] = vga[r * 80 + cx];
        for (int cx = 0; cx < 80; cx++) vga[24 * 80 + cx] = (0x0F << 8) | ' ';
        vrow = 24;
    }
}

/* ── 16550 serial (COM1) — the witness channel ────────────────────────── */
#define COM1 0x3F8
static void serial_init(void) {
    outb(COM1 + 1, 0x00);      /* no interrupts: the shell polls */
    outb(COM1 + 3, 0x80);      /* DLAB on */
    outb(COM1 + 0, 0x01);      /* 115200 baud */
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);      /* 8N1 */
    outb(COM1 + 2, 0xC7);      /* FIFO on */
}
static void serial_putc(char c) {
    while (!(inb(COM1 + 5) & 0x20)) { }
    outb(COM1, (u8)c);
}
static int serial_ready(void) { return inb(COM1 + 5) & 0x01; }
static char serial_getc(void) { while (!serial_ready()) { } return (char)inb(COM1); }

static void putc(char c) { if (c == '\n') serial_putc('\r'); serial_putc(c); vga_putc(c); }
static void puts(const char *s) { while (*s) putc(*s++); }
static void putu(u32 n) {
    char b[12]; int i = 0;
    if (!n) { putc('0'); return; }
    while (n) { b[i++] = (char)('0' + n % 10); n /= 10; }
    while (i) putc(b[--i]);
}

/* ── tiny string room ─────────────────────────────────────────────────── */
static int streq(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a == 0 && *b == 0;
}
static int slen(const char *s) { int n = 0; while (s[n]) n++; return n; }
static void scpy(char *d, const char *s, int cap) {
    int i = 0;
    while (s[i] && i < cap - 1) { d[i] = s[i]; i++; }
    d[i] = 0;
}

/* ── physical memory: page bitmap over 1 MB..8 MB ─────────────────────── */
#define PAGE 4096u
#define PHYS_BASE 0x100000u
#define PHYS_PAGES 1792u            /* 7 MB of allocatable pages */
static u8 page_used[PHYS_PAGES];
static u32 pages_alloced = 0;
static void *page_alloc(void) {
    for (u32 i = 0; i < PHYS_PAGES; i++)
        if (!page_used[i]) { page_used[i] = 1; pages_alloced++; return (void *)(PHYS_BASE + i * PAGE); }
    return (void *)0;
}
static void page_free(void *p) {
    u32 i = ((u32)p - PHYS_BASE) / PAGE;
    if (i < PHYS_PAGES && page_used[i]) { page_used[i] = 0; pages_alloced--; }
}

/* ── IDT + PIC + PIT ──────────────────────────────────────────────────── */
struct idt_entry { u16 lo; u16 sel; u8 zero; u8 flags; u16 hi; } __attribute__((packed));
struct idt_ptr { u16 limit; u32 base; } __attribute__((packed));
static struct idt_entry idt[256];
static struct idt_ptr idtp;
extern void timer_stub(void);       /* entry.S */
extern void default_stub(void);     /* entry.S */
static void idt_set(int n, u32 handler) {
    idt[n].lo = handler & 0xFFFF; idt[n].sel = 0x08; idt[n].zero = 0;
    idt[n].flags = 0x8E; idt[n].hi = (handler >> 16) & 0xFFFF;
}
static void pic_remap(void) {
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);   /* IRQs at vectors 0x20.. */
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0xFE); outb(0xA1, 0xFF);   /* unmask only IRQ0 (timer) */
}
#define HZ 100
static void pit_init(void) {
    u32 div = 1193182 / HZ;
    outb(0x43, 0x36);
    outb(0x40, (u8)(div & 0xFF));
    outb(0x40, (u8)((div >> 8) & 0xFF));
}
static volatile u32 ticks = 0;

/* ── tasks: real stacks, preempted by the clock ───────────────────────── */
struct task {
    u32 esp;                 /* saved stack pointer (top of pushed context) */
    u32 stack_page;
    const char *name;
    volatile u32 runs;       /* how many times the scheduler gave it breath */
    int live;
};
#define NTASKS 8
static struct task tasks[NTASKS];
static int ntasks = 0;
static volatile int cur = 0;
static int sched_on = 0;

extern void switch_ctx(u32 *save_esp, u32 load_esp);   /* entry.S */

static void task_create(const char *name, void (*fn)(void)) {
    struct task *t = &tasks[ntasks];
    u32 *sp = (u32 *)((u32)page_alloc() + PAGE);
    /* a fresh task "returns" into fn with interrupts enabled */
    *--sp = (u32)fn;         /* ret target */
    *--sp = 0x202;           /* eflags with IF */
    *--sp = 0; *--sp = 0; *--sp = 0; *--sp = 0;   /* ebp edi esi ebx */
    t->esp = (u32)sp;
    t->stack_page = (u32)sp & ~(PAGE - 1);
    t->name = name; t->runs = 0; t->live = 1;
    ntasks++;
}

/* the timer interrupt lands here (from entry.S) and may hand the CPU on */
void timer_tick(void) {
    ticks++;
    outb(0x20, 0x20);        /* EOI first: the switch below may not return here */
    if (!sched_on || ntasks < 2) return;
    int prev = cur;
    int next = prev;
    do { next = (next + 1) % ntasks; } while (!tasks[next].live && next != prev);
    if (next == prev) return;
    cur = next;
    tasks[next].runs++;
    switch_ctx(&tasks[prev].esp, tasks[next].esp);
}

/* ── ramfs: name -> bytes, page-backed ────────────────────────────────── */
struct file { char name[24]; char *data; u32 size; int used; };
#define NFILES 16
static struct file files[NFILES];
static struct file *fs_find(const char *name) {
    for (int i = 0; i < NFILES; i++)
        if (files[i].used && streq(files[i].name, name)) return &files[i];
    return (struct file *)0;
}
static int fs_write(const char *name, const char *text) {
    struct file *f = fs_find(name);
    if (!f) {
        for (int i = 0; i < NFILES; i++) if (!files[i].used) { f = &files[i]; break; }
        if (!f) return -1;
        f->used = 1; scpy(f->name, name, sizeof f->name);
        f->data = (char *)page_alloc();
        if (!f->data) { f->used = 0; return -1; }
    }
    u32 n = (u32)slen(text); if (n > PAGE - 1) n = PAGE - 1;
    for (u32 i = 0; i < n; i++) f->data[i] = text[i];
    f->data[n] = 0; f->size = n;
    return (int)n;
}

/* ── two background breaths: proof the preemption is real ─────────────── */
static volatile u32 heart_a = 0, heart_b = 0;
static void task_heart_a(void) { for (;;) heart_a++; }
static void task_heart_b(void) { for (;;) heart_b++; }

/* ── the shell: HatiOS speaking on the serial witness channel ─────────── */
static void read_line(char *buf, int cap) {
    int n = 0;
    for (;;) {
        char c = serial_getc();
        if (c == '\r' || c == '\n') { putc('\n'); buf[n] = 0; return; }
        if ((c == 8 || c == 127) && n > 0) { n--; puts("\b \b"); continue; }
        if (n < cap - 1 && c >= 32 && c < 127) { buf[n++] = c; putc(c); }
    }
}
static char *arg_split(char *line) {          /* "cmd rest" -> rest (or "") */
    while (*line && *line != ' ') line++;
    if (!*line) return line;
    *line++ = 0;
    while (*line == ' ') line++;
    return line;
}
static void qemu_exit(void) { outb(0xF4, 0x31); for (;;) __asm__ volatile("hlt"); }

static void shell(void) {
    char line[128];
    puts("\nhati> ");
    for (;;) {
        read_line(line, sizeof line);
        char *arg = arg_split(line);
        if (streq(line, "help")) {
            puts("help ps mem ls cat <f> write <f> <text> echo <s> uptime spin halt\n");
        } else if (streq(line, "ps")) {
            for (int i = 0; i < ntasks; i++) {
                puts("  task "); putu((u32)i); puts(" "); puts(tasks[i].name);
                puts(" runs="); putu(tasks[i].runs);
                puts(i == cur ? " [running]\n" : "\n");
            }
            puts("  hearts a="); putu(heart_a); puts(" b="); putu(heart_b);
            puts(" (climbing between calls = preemption is real)\n");
        } else if (streq(line, "mem")) {
            puts("  pages used "); putu(pages_alloced); puts(" / "); putu(PHYS_PAGES);
            puts(" (4096 bytes each, phys 1MB..8MB)\n");
        } else if (streq(line, "ls")) {
            for (int i = 0; i < NFILES; i++)
                if (files[i].used) { puts("  "); puts(files[i].name); puts(" ("); putu(files[i].size); puts(" bytes)\n"); }
        } else if (streq(line, "cat")) {
            struct file *f = fs_find(arg);
            if (f) { puts(f->data); putc('\n'); } else puts("  no such file (honest miss)\n");
        } else if (streq(line, "write")) {
            char *text = arg_split(arg);
            int n = fs_write(arg, text);
            if (n < 0) puts("  fs full\n");
            else { puts("  wrote "); putu((u32)n); puts(" bytes to "); puts(arg); putc('\n'); }
        } else if (streq(line, "echo")) {
            puts(arg); putc('\n');
        } else if (streq(line, "uptime")) {
            puts("  ticks="); putu(ticks); puts(" ("); putu(ticks / HZ); puts("s at 100 Hz)\n");
        } else if (streq(line, "spin")) {
            u32 until = ticks + 2 * HZ;
            puts("  spinning 2s under preemption...\n");
            while (ticks < until) { }
            puts("  back. hearts kept beating: a="); putu(heart_a); puts(" b="); putu(heart_b); putc('\n');
        } else if (streq(line, "halt")) {
            puts("  hati-os: witnessed, resting. goodbye.\n");
            qemu_exit();
        } else if (line[0]) {
            puts("  unknown verb (help lists what this body can do)\n");
        }
        puts("hati> ");
    }
}

/* ── kmain: first C breath after boot.S ───────────────────────────────── */
void kmain(void) {
    serial_init();
    for (int i = 0; i < 80 * 25; i++) vga[i] = (0x0F << 8) | ' ';
    puts("\nHatiOS 0.1 - bare-metal i386, freestanding C + GNU as\n");
    puts("boot witness: BIOS sector -> A20 -> GDT -> protected mode -> kmain\n");

    for (int i = 0; i < 256; i++) idt_set(i, (u32)default_stub);
    idt_set(0x20, (u32)timer_stub);
    idtp.limit = sizeof(idt) - 1; idtp.base = (u32)idt;
    __asm__ volatile("lidt %0" :: "m"(idtp));
    pic_remap();
    pit_init();
    puts("interrupts: IDT loaded, PIC remapped, PIT at 100 Hz\n");

    fs_write("manifest", "HatiOS: drivers, interrupts, preemptive scheduler, pages, ramfs, shell. Honest floors: ring0 only, no paging, no disk fs.");

    task_create("shell", shell);
    task_create("heart-a", task_heart_a);
    task_create("heart-b", task_heart_b);
    puts("tasks: shell + two hearts, preemptive round-robin\n");

    sched_on = 1;
    __asm__ volatile("sti");

    /* become task 0 by jumping into the shell; the clock owns the CPU now */
    cur = 0; tasks[0].runs = 1;
    shell();
}
