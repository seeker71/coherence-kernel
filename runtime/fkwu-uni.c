#if defined(_WIN32)
/* fkwu Windows port shim (mingw-w64) — guarded by _WIN32 so the mac/linux path is byte-identical.
 * mingw's <io.h> declares read/write/mkdir with int / unsigned-int signatures that clash with the
 * seed's own long-long externs, and a 32-bit int return zero-extends into rax, corrupting the error
 * (-1) path. We route read/write through correct-width wrappers and supply arc4random (absent on
 * Windows). The three __has_include blocks below are gated off on _WIN32 too, so io.h is never
 * dragged in and the seed uses its self-contained extern / O_* fallbacks. */
extern int _read(int, void *, unsigned int);
extern int _write(int, const void *, unsigned int);
extern int rand(void);
#define read fkwu_win_read
#define write fkwu_win_write
long long fkwu_win_read(int fd, void *buf, unsigned long n) {
    return (long long)_read(fd, buf, (unsigned int)n);
}
long long fkwu_win_write(long long fd, const void *buf, unsigned long n) {
    return (long long)_write((int)fd, buf, (unsigned int)n);
}
unsigned int arc4random(void) {
    return ((unsigned int)rand() << 17) ^ ((unsigned int)rand() << 6) ^ (unsigned int)rand();
}
/* POSIX dlopen/dlsym (used only by the optional libcrypto/TLS lane) → Win32 loader. The seed's
 * hard-coded .dylib/.so paths won't resolve on Windows, so TLS stays unavailable; that lane is not
 * on the source-eval / four-way path this receipt exercises. */
extern void *LoadLibraryA(const char *);
extern void *GetProcAddress(void *, const char *);
extern int FreeLibrary(void *);
void *dlopen(const char *p, int f) {
    (void)f;
    return LoadLibraryA(p);
}
void *dlsym(void *h, const char *n) {
    return GetProcAddress(h, n);
}
int dlclose(void *h) {
    return FreeLibrary(h) ? 0 : -1;
}
#endif
extern int putchar(int);
extern int fflush(void *);
extern int printf(const char *, ...);
extern int dprintf(int, const char *, ...);
extern int vdprintf(int, const char *, __builtin_va_list);
extern void *malloc(unsigned long);
extern void *realloc(void *, unsigned long);
extern long long read(int, void *, unsigned long);
extern int isatty(int);
extern void exit(int);
extern long long write(long long, const void *, unsigned long);
/* fk_die: the ONE hard-stop primitive in the whole seed. Reserved for states that
 * are structurally impossible to continue past honestly -- allocator exhaustion
 * (malloc/realloc returned 0) and the handful of fixed-capacity invariants whose
 * violation would otherwise silently corrupt memory rather than fail. It is NOT
 * for ordinary bounds checks; those keep the file's existing house style of a
 * silent safe-default return (`if (idx < 1 || idx > cap) return 0;`). Writes the
 * message to fd 2 the same way fk_mw/fk_mc already do, then exits nonzero -- no
 * stdio, no FILE-pointer/stderr symbol (those aren't portable across this file's
 * three platforms), just the raw write() this seed already leans on elsewhere. */
static void fk_die(const char *msg) {
    long long n = 0;
    while (msg[n]) {
        n = n + 1;
    }
    write(2, msg, n);
    write(2, "\n", 1);
    exit(1);
}
/* ── COMPILE-PHASE DIAGNOSTIC COLLECTOR ──────────────────────────────────────
 * Two-phase law (2026-07-02): RUNTIME dies only when it truly cannot recover
 * (OOM, corruption); COMPILE-TIME collects EVERY warning/error and CONTINUES,
 * gcc/clang-style ("N error(s), M warning(s)"). fk_die stays the hard-stop for
 * OOM/corruption; the parse-time capacity/arity dies below are MIS-PHASED and
 * become fk_diag + best-effort recovery so the rest of the source is still
 * checked and every defect surfaces in ONE pass (collect-and-continue), instead
 * of halting on the first problem.
 *
 * fk_diag streams each diagnostic to fd 2 IMMEDIATELY in clang form
 * ("fkwu:line:col: error|warning: msg"), computing line/col from a byte offset
 * into fk_srctext (O(n) newline count -- no line counter is maintained during
 * parse, and error paths are rare, so this is fine), then bumps the global
 * error/warning counters. It is VARIADIC on purpose: the offender's name lives
 * at every call site as a non-NUL-terminated (start,length) slice of fk_srctext
 * (see the pre-existing unresolved-call witness that prints "'%.*s'",
 * (int)hn, fk_srctext+s), never as a C string -- a fixed const char* signature
 * would force a snprintf-into-scratch dance at every site. A negative off means
 * "no source coordinate" (e.g. the .tbl loader reads fk_buf, not fk_srctext),
 * so the line:col prefix is suppressed.  sev: 0 = warning, 1 = error. */
#define FK_DIAG_WARN 0
#define FK_DIAG_ERR  1
static long long fk_nerr;        /* errors diagnosed this compile (reset per compile pass) */
static long long fk_nwarn;       /* warnings diagnosed this compile (reset per compile pass) */
/* PRINTED-diagnostic tallies, monotone for the whole process — never reset.
 * fk_nerr/fk_nwarn are per-compile working counters and ARE legitimately
 * wiped by fk_src_reset_compile_state between the speculative per-dep image
 * compile and the authoritative whole-program compile; before these existed,
 * that wipe also erased already-printed "error:" lines from the exit code and
 * the tally (8 errors on stderr, "0 errors" exit — the stamp+shape family).
 * The exit truth and the gcc-style tally read THESE, so every diagnostic the
 * user saw is carried, by construction, across any number of resets. */
static long long fk_nerr_seen;
static long long fk_nwarn_seen;
static long long fk_diag_quiet;  /* nonzero: speculative compile — count into
                                  * fk_nerr/fk_nwarn (the .sym record and the
                                  * import gate need the truth) but print
                                  * nothing; a candidate image's diagnostics
                                  * are not the program's */
static int fk_src_truncated;     /* 1 if the source was amputated at the source-text cap (dissolved 2026-09-02: the buffer grows) */
static long long fk_srctext_cap;   /* live source-text capacity (kernel_stat 25); owned by fk_srctext_reserve below */
static long long fk_srctext_grows; /* source-text doublings this run (kernel_stat 26) */
static long long fk_heat_total_private;
static long long *fk_heat_total_p = &fk_heat_total_private;
#define fk_heat_total (*fk_heat_total_p)   /* heat-lane calls: a word in this kernel live page once it opens */
static long long fk_gpu_busy_total_us; /* host GPU busy microseconds integrated in this process (host_gpu_busy_us) */
static long long fk_gpu_busy_last_us;  /* monotonic clock at the last fresh level read (the integration step) */
static long long fk_gpu_level_cache = -1; /* the last level read; the utilization the accelerator answers is the mean since the previous query by ANY process, so a second read within the window would read 0 */
long long fk_host_gpu_utilization(void); /* the Metal carrier answers; the weak stub below answers -1 */
/* the host's own statistics and processes, no shell between: Mach VM/CPU/load, libproc, IOKit (carrier), fork+execvp on an argv */
long long fk_host_disk_stat(long long *out);
#ifdef __APPLE__
extern int host_statistics64(unsigned int host, int flavor, int *info, unsigned int *count);
extern int sysctlbyname(const char *name, void *oldp, unsigned long *oldlenp, void *newp, unsigned long newlen);
extern int getloadavg(double *loads, int n);
extern int proc_listpids(unsigned int type, unsigned int typeinfo, void *buffer, int buffersize);
extern int proc_pidinfo(int pid, int flavor, unsigned long long arg, void *buffer, int buffersize);
extern int proc_name(int pid, void *buffer, unsigned int buffersize);
struct fk_mach_timebase { unsigned int numer; unsigned int denom; };
extern int mach_timebase_info(struct fk_mach_timebase *info);
#endif
extern int setpriority(int which, unsigned int who, int prio);
extern int kill(int pid, int sig);
static long long fk_sysctl_ll(const char *name) {
#ifdef __APPLE__
    long long v = 0;
    unsigned long sz = sizeof v;
    if (sysctlbyname(name, &v, &sz, 0, 0) != 0) { return -1; }
    return v;
#else
    return -1;
#endif
}
static long long fk_u64_words(unsigned int lo, unsigned int hi) { return (long long)(((unsigned long long)hi << 32) | (unsigned long long)lo); }
static int fk_cstr_eq(const char *a, const char *b);
static long long fk_host_spawn_arm(long long argv155, long long t);
/* the store: every value table in shared memory; defined beside fk_gift_open, born long before it */
#define FK_STORE_NODE_CELLS (1LL << 26)   /* sparse reservations: virtual ceilings, committed page by page */
#define FK_STORE_HEAP_PAIRS (1LL << 27)
#define FK_STORE_STR_BYTES (1LL << 31)
#define FK_STORE_STR_CELLS (1LL << 25)
#define FK_STORE_FLOATS (1LL << 26)
static void *fk_store_take(char letter, long long bytes);
static void *fk_store_grow(char letter, void *p, long long old_bytes, long long new_bytes, long long reserved, int zero);
static void fk_store_go_private(void);
/* the program surface: the AST rows (A), the source text (S) and the defn table + header (D) of THIS kernel,
 * per-pid objects /fg-c<pid>-A|S|D, so another process reads a defn's body nodes and source span where they live */
#define FK_PROG_AST_ROWS (1LL << 25)      /* 1 GiB of 32-byte rows, committed page by page */
#define FK_PROG_SRC_BYTES (1LL << 30)
#define FK_PROG_D_BYTES (48LL << 20)
#define FK_PROG_D_PATH_OFF 4096
#define FK_PROG_D_FN_OFF 8192             /* row j (a fntop index): sym start, sym length, fn idx -- 3 words, 2^20 rows */
#define FK_PROG_D_BODY_OFF (8192 + (32LL << 20))   /* word idx: the body node of fn idx, 2^20 words */
#define FK_PROG_FNS (1LL << 20)
static void *fk_prog_take(char letter, long long bytes);
static void fk_prog_note_counts(void);
static void fk_prog_note_body(long long idx);
static void fk_prog_note_ice(const char *path, long long len, const char *hash_text);
static int fk_prog_ast_shared, fk_prog_src_shared;
static long long *fk_prog_D;
static void fk_store_name(char letter, long long pid, char *out);
static void *fk_store_copy_out(void *p, long long bytes);
static char *fk_srctext;
static long long fk_slen;
static unsigned long long fk_bytes_fnv1a(const char *p, long long n);
static void fk_store_unlink_pid(long long pid);
/* ---- the field: one host-wide store every kernel shares -- the same word is the same cell in every process ----
 * Shared pairs, shared strings and shared floats live in their own index ranges (>= 2^40) behind the same
 * words the private heap, string pool and float pool use, so every reader dispatches by range. */
#define FK_PAIR_BASE (1LL << 40)
#define FK_STR_BASE (1LL << 40)
#define FK_FLT_BASE (1LL << 40)
static int fk_field_on;
static int fk_field_tried;
static volatile long long *fk_field_hdr;
static long long *fk_fph;
static long long *fk_fpt;
static char *fk_fsb;
static long long *fk_fso;
static long long *fk_fsl;
static double *fk_ffv;
static long long fk_field_pp(void);
static long long fk_field_sp(void);
static long long fk_field_fp(void);
static unsigned long long fk_field_bytes_hash(const char *b, long long n);
static long long fk_str_bytes_eq(long long a, long long b);
static int fk_field_open(void);
/* every read of a pair, a string cell or a float goes through one door, so the shared arenas can stand behind the same words */
#define FK_HH(p) ((p) >= FK_PAIR_BASE ? fk_fph[(p) - FK_PAIR_BASE] : fk_hh[(p)])
#define FK_HT(p) ((p) >= FK_PAIR_BASE ? fk_fpt[(p) - FK_PAIR_BASE] : fk_ht[(p)])
#define FK_POK(p) ((p) >= FK_PAIR_BASE ? ((p) - FK_PAIR_BASE < fk_field_pp()) : ((p) <= fk_hp))
#define FK_SO(si) ((si) >= FK_STR_BASE ? fk_fso[(si) - FK_STR_BASE] : fk_so[(si)])
#define FK_SLEN(si) ((si) >= FK_STR_BASE ? fk_fsl[(si) - FK_STR_BASE] : fk_sl[(si)])
#define FK_SBYTES(si) ((si) >= FK_STR_BASE ? fk_fsb + fk_fso[(si) - FK_STR_BASE] : fk_sb + fk_so[(si)])
#define FK_SOK(si) ((si) >= FK_STR_BASE ? ((si) - FK_STR_BASE < fk_field_sp()) : ((si) < fk_sp))
#define FK_FV(fi) ((fi) >= FK_FLT_BASE ? fk_ffv[(fi) - FK_FLT_BASE] : fk_fv[(fi)])
static int fk_store_shared;
static int fk_heap_gen;
static void *fk_heap_alt_h;
static void *fk_heap_alt_t;
static void fk_live_publish(int final);
static void fk_f64_pulse(long long fx);
static void fk_f64_loop_pulse(long long fx, long long fp, long long n);
static void fk_f64_reset(void);
static void fk_twin_pulse(long long fx);
static long long fk_twin_calls_private;
static long long *fk_twin_calls_p = &fk_twin_calls_private;
#define fk_twin_calls (*fk_twin_calls_p)
static int fk_twin_call(long long fx, long long fp, long long *out);
static long long fk_len_upto(long long v, long long cap);
static long long (*fk_node)[4];
static long long fk_node_count;
static void **fk_f64_mem; /* per-defn f64 leaf pages (this process) */
static long long *fk_f64_sig; /* per-defn leaf signature: -1 the all-float expression leaf; else bit k = param k float, bit 8 = float result, bit 9 = loop */
static long long fk_f64_cap;
static long long *fk_f64_loop_iters_p; /* iterations that ran inside a native loop (kernel_stat 50, live word 32); page-backed once the live page opens */
#define fk_f64_loop_iters (*fk_f64_loop_iters_p)
static long long fk_smknode(long long t0, long long c1, long long c2, long long c3);
static long long *fk_fn_native; /* per-defn crystallization state: 0 cold, 1 f64 leaf standing, 2 native loop standing, -1 declined */
static long long *fk_fn_mint;   /* per-defn value cells minted: the arena grows where a recipe meets a name it has not met */
static long long *fk_fn_inram;  /* per-defn FOLDED calls: a body that ran as this machine's own instructions, its intermediates in registers rather than a pool slot each */
static const char *fk_hot_unit_of(long long so);
static void fk_live_open(void);
static void fk_live_note_defn(long long j);
static long long fk_live_ticks;
#ifdef __APPLE__
extern unsigned int mach_host_self(void);
extern int host_statistics(unsigned int host, int flavor, int *info, unsigned int *count);
#endif
static long long fk_host_cpu_busy_us(void) {
#ifdef __APPLE__
    int ticks[4] = { 0, 0, 0, 0 };
    unsigned int count = 4;
    if (host_statistics(mach_host_self(), 3, ticks, &count) != 0) { return -1; }
    return ((long long)(unsigned int)ticks[0] + (long long)(unsigned int)ticks[1] + (long long)(unsigned int)ticks[3]) * 10000;
#else
    return -1;
#endif
}
static long long fk_gpu_step(long long now_us) {
    if (fk_gpu_busy_last_us > 0 && now_us - fk_gpu_busy_last_us < 50000) { return fk_gpu_level_cache; }
    long long level = fk_host_gpu_utilization();
    if (fk_gpu_busy_last_us > 0 && level >= 0 && now_us > fk_gpu_busy_last_us) { fk_gpu_busy_total_us = fk_gpu_busy_total_us + (now_us - fk_gpu_busy_last_us) * level / 100; }
    fk_gpu_busy_last_us = now_us;
    fk_gpu_level_cache = level;
    return level;
}
/* ── the admission pulse: WHICH DOOR this program entered through, readable by
 * the running program itself via kernel_stat 15..18. The icetide dig
 * (2026-09-01, corpus 1217) showed the reproduction key of a whole wound family
 * living in the door decision, not the source bytes — and the only witness was
 * a static conf toggle printing to stderr, which the program could neither read
 * nor correlate. These four are always recorded; observation is pulled by
 * whoever asks, never pushed behind a switch.
 *   fk_run_door: 0 flat whole-program compile, 1 import lane (images + carried
 *     source), 2 cached image replay (.fkb / warm .bml.fkb / direct .fkb run),
 *     3 native .dylib artifact.
 *   fk_import_images / fk_import_carried: units that entered as standalone
 *     .fkb images / as carried source beside them (door 1 only; both 0 when
 *     the lane refused, because the flat compile then carries everything).
 *   fk_import_refusal: why the import lane last stepped aside, 0 when it did
 *     not: 1 no importable direct dep, 2 carry bookkeeping allocation failed,
 *     3 standalone artifact compile failed, 4 artifact path exceeded buffer,
 *     5 unit identity hash exceeded buffer, 6 dep image carries recorded
 *     compile errors, 7 image load/identity refused (foreign, stale, corrupt),
 *     8 carried or root text exceeded the source-text cap (dissolved
 *       2026-09-02: the buffer grows, so 8 no longer fires). A refusal is not a
 *     wound — the flat compile is the fully correct door — but it is a
 *     decision, and decisions are observable. */
static long long fk_run_door;
static long long fk_import_images;
static long long fk_import_carried;
static long long fk_import_refusal;
/* 1 if the parse met a defect that CANNOT recover into a runnable program: a read
 * of an unbound name (a read has no value to decline with) or a parameter that
 * names a primitive (the sibling kernels do not agree on its arity). fkwu's
 * standing posture is recover-and-run, and it stays that way for every defeasible
 * diagnostic; these two are not defeasible, and running past them is what let a
 * deliberately broken band answer 255 and what made one variant spin for minutes
 * with no output at all. Same gate as fk_src_truncated: surface every diagnostic,
 * then REFUSE to execute. */
static int fk_src_unrunnable;
/* fk_diag / fk_diag_flush are DEFINED further down (right after fk_srctext /
 * fk_spos / fk_slen are declared), where they can read the source buffer. */
static void fk_diag(int sev, long long off, const char *fmt, ...);
static void fk_diag_flush(void);
static void fk_heat_pulse(void);
static int fk_write_all_raw(int fd, const void *buf, unsigned long n);
/* Named capacities for the seed's fixed-size tables. Several numerically coincide
 * (many independent tables happen to be sized 65536) but are named SEPARATELY on
 * purpose: they are different index spaces (the node/AST table, the value stack,
 * and the staged-input buffer are three unrelated capacities that must never be
 * conflated under one name, or a future resize of one would silently misresize
 * the others). Where a mask (`& N-1`) stood in for a bound check, the mask is
 * rewritten in terms of the same named constant so the two can never drift apart. */
#define FK_FLOAT_POOL_INIT_CAP 65536    /* fk_fv: boxed-float pool, initial size (doubles on demand) */
#define FK_OPCODE_ARM_CAP 256           /* fk_arms: per-tag hit counters, indexed by node tag t */
#define FK_PATH_CAP 4096               /* one host path, or a path-shaped name (a .fkb/.sym/.dylib sibling, a temp spelling). Not a table: a path longer than this names nothing the host can open. Every fk_cstr into a path buffer refuses loudly past it and every join dies loudly rather than truncate -- a cut path names the wrong file. */
#define FK_MESH_MSG_CAP 4096           /* one mesh/sense JSON line, built by sprintf from bounded fields (an ssid under 64 bytes and integers) */
#define FK_MEM_CELL_CAP_INIT 4096      /* fk_mem: mutable record-cell table (tags 13/14). Birth size only -- the table grows to hold any written cell id (fk_mem_reserve). The old fixed table MASKED the id (mi & 4095): two cells 4096 apart silently ALIASED, one state quietly swapped for another. */
#define FK_STAGED_INPUT_CAP_INIT 262144 /* fk_src: staged auxiliary input (the input_byte primitive); birth size only -- grows to carry any staged text (the old fixed buffer truncated it SILENTLY at the brim). */
#define FK_VALUE_STACK_CAP_INIT 1048576  /* fk_vs: the evaluator's argument/value stack. Birth size only -- the stack DOUBLES on demand (fk_vs_grow, kernel_stat 27/28 = live cap / doublings), so stack slots are not a wall; runaway recursion is spoken for by the walker's own host-stack diagnostic. History: 65536->1048576 (2026-07-30) when the fourth kernel alone refused a 100,000-deep count the other three answered; a loud wall after that; growth since 2026-09-02. New slots are zeroed so a melt walking raised-but-unwritten slots sees the same inert 0 the BSS era gave it. */
#define FK_STRING_POOL_INIT_BYTES 1048576 /* fk_sb: interned-string byte pool, initial size */
#define FK_STRING_TABLE_INIT_CAP 16384  /* fk_so/fk_sl: interned-string table, initial entry count */
/* NOTE: the value-node table (FK_NODE_CAP_INIT) and FK_AST_NODE_CAP are DIFFERENT
 * tables -- never conflate them. The value-node table is the hash-cons'd VALUE
 * table (fk_nkind, ncat, nkids, nval, nid, nsfile, nsline, nscol, nsattr, fbroots)
 * used by fk_neq/fk_veq for structural equality on cons'd runtime values (records,
 * lists). FK_AST_NODE_CAP_INIT (defined near fk_node[][4] itself, further down) is the
 * PARSED PROGRAM's syntax tree, filled once per expression during parsing via
 * fk_smknode. */
#define FK_NODE_CAP_INIT 262144         /* fk_nkind, ncat, nkids, nval, nid, nsfile, nsline, nscol, nsattr, fbroots, nhash_memo, inram slot/generation/released: birth capacity only -- the table DOUBLES on demand (fk_nodes_grow), so there is no node wall. Handles are indices into column arrays; doubling the columns keeps every handle valid, and the intern index grows with them (held at 4x the node cap, rebuilt from fk_nhash_memo). History: 65536->262144 (2026-07-02) when a full table made every guard silently return handle 0; then a loud die at the wall; growth since 2026-09-02. A runaway consumer now shows as monotone kernel_stat 19/20 (live cap / doublings) instead of a refusal -- probe whether the fill POSITION moves, same discipline as ever. 262144*104B ~= 27MB at birth. */
static long long fk_node_cap;           /* live capacity; fk_nodes_init/fk_nodes_grow own it */
static long long fk_node_grows;         /* doublings this run -- kernel_stat 20 */
#define FK_RECORD_CAP_INIT 256          /* fk_rkey/rval/rcnt/rbp: record-table birth size; rows grow on demand (fk_record_reserve). Record VALUES and blueprints are melt roots since 2026-09-02 -- a record holding a heap list used to dangle across a compaction. */
#define FK_RECORD_KEYS_INIT 8           /* per-record key/value row birth size; rows grow on demand (fk_record_keys_reserve) -- the old 128 wall died loud on construction and silently DROPPED keys on record_set. */
/* Function values occupy odd words below fk_fnbase and above the string-value
 * region. Validity follows the functions actually present in the current image;
 * it is not a second fixed function-table ceiling. */
static long long fk_fn_count;
/* ASCII byte constants for the text-processing code (the parser's own character
 * classification, and the OS-layer's path/URL splitting) -- NOT used in the
 * evaluator's `if (t == N)` opcode-tag dispatch, which is a completely
 * different numbering space (generated from fkwu-optable.h) that happens to
 * share small integer values with ASCII codes in the 0-127 range. Conflating
 * the two would be the same class of mistake as merging the value-node table and
 * the AST table; kept strictly to genuine byte/character comparisons. */
#define FK_CH_TAB 9
#define FK_CH_LF 10
#define FK_CH_CR 13
#define FK_CH_SPACE 32
#define FK_CH_DQUOTE 34
#define FK_CH_PLUS 43
#define FK_CH_COMMA 44
#define FK_CH_LBRACKET 91
#define FK_CH_RBRACKET 93
#define FK_CH_DASH 45
#define FK_CH_DOT 46
#define FK_CH_SLASH 47
#define FK_CH_DIGIT0 48
#define FK_CH_DIGIT9 57
#define FK_CH_COLON 58
#define FK_CH_SEMI 59
#define FK_CH_LPAREN 40
#define FK_CH_RPAREN 41
#define FK_CH_UPPER_E 69
#define FK_CH_LOWER_E 101
#define FK_CH_BACKSLASH 92
#define FK_CH_UPPER_A 65
#define FK_CH_UPPER_Z 90
#define FK_CH_LOWER_A 97
#define FK_CH_LOWER_Z 122
#define FK_CH_UNDERSCORE 95
#define FK_CH_DEL 127
#define FK_CH_NUL 0
#define FK_CH_LOWER_N 110 /* the 'n' in a \n escape specifier */
#define FK_CH_LOWER_T 116 /* the 't' in a \t escape specifier */
#define FK_CH_LOWER_R 114 /* the 'r' in a \r escape specifier */
#define FK_CH_LOWER_B 98  /* the 'b' in .fkb */
#define FK_CH_LOWER_F 102 /* the 'f' in the --feval CLI flag */
#define FK_CH_LOWER_K 107 /* the 'k' in .fk/.fkb */
static const unsigned char *fk_gen = 0;
static long long fk_gen_len = 0;
static double *fk_fv;
static long long fk_fcap;
static long long fk_fp;
static const long long fk_fbase = -9000000000000000000LL;
/* stone 2a: the CANONICAL first-class nothing (axiom-1: nothing is first-class; timeout==nothing).
 * A single reserved sentinel, odd and one above fk_fbase, so it is DISTINCT from every value: not
 * an int (ints are v<<1, even), not 0, not the nil/empty value 1, not a boxed float (fk_isf needs
 * v<=fk_fbase-3; this is fk_fbase+1, so isf is false), not a node (fk_nidx maps it to ~4.5e18, far
 * past fk_np), not a record ((0-v) is even for records; here it is odd), not a string/list (those
 * are positive). The reducer RETURNS this from (nothing); recipes OBSERVE it via nothing? —
 * no-value is no longer conflated with 0 or host-null. */
static const long long fk_nothing = -8999999999999999999LL;
static long long fk_is_nothing(long long v) {
    return v == fk_nothing ? 1 : 0;
}
/* stone 2c: a FUNCTION VALUE — a fn used as a value (a bare fn-name in value position, a fn stored
 * in a var, a fn returned from a fn). Minted exactly like 2a's nothing: a reserved odd-NEGATIVE
 * band sentinel, collision-proof by arithmetic. fk_fnbase = -8e18 sits ABOVE nothing (-8.999e18)
 * and the float base (fk_fbase = -9e18, floats live at-or-below it), and BELOW every
 * node/record/cons/int (which are tiny-magnitude or positive). fk_fnval(f) = fk_fnbase - (f<<1) - 1
 * is therefore odd-negative in a narrow band (fn-indices are < 4096): not an int (ints v<<1, even),
 * not 0/1, not a float (fk_isf needs v<=fk_fbase-3 ~ -9e18; these are ~-8e18, ABOVE it), not a node
 * (fk_nidx maps ~8e18 far past fk_np), not a record ((0-v) is odd here, records even), not nothing
 * (distinct constant). A bare fn-name in value position evaluates to this (tag 243); an indirect
 * call offers the fn it names (tag 244).
 *
 * CLOSURE, the gap this comment used to name: a fn-value carrying a captured env, not just the
 * fn-index. Closed by widening the SAME band rather than opening a new one — the index space below
 * fk_fn_count (ordinary functions) was always tiny relative to the room fk_fnval_floor leaves (up
 * to ~2.5e17), so indices at-or-above FK_CLOSURE_IDX_BASE (a billion, far past any real fk_fn_count)
 * now name a row in a SEPARATE runtime table (fk_clo_target/capbase/capcount/capvals) instead of a
 * plain fk_fn[] entry: which function to run, and the values it captured at the moment its value
 * was built (form-stdlib/http-layer.fk's `layer-stamp` closing over its own `hn`/`hv` is the
 * standing example). fk_fnval_idx(v) still just extracts the raw encoded number; fk_fnval_target(v)
 * is the new door that tells a plain function apart from a closure instance and answers "which
 * fk_fn[] row do I actually jump to". */
static const long long fk_fnbase = -8000000000000000000LL;
static const long long fk_fnval_floor = -8500000000000000000LL;
#define FK_CLOSURE_IDX_BASE 1000000000LL
#define FK_CLOSURE_CAP_MAX 8
static long long *fk_clo_target, *fk_clo_capbase, *fk_clo_capcount;
static long long fk_clo_top, fk_clo_cap;
static long long *fk_clo_capvals;
static long long fk_clo_capvals_top, fk_clo_capvals_cap;
static long long fk_fnval(long long f) {
    return fk_fnbase - (f << 1) - 1;
}
static long long fk_fnval_idx(long long v) {
    return ((fk_fnbase - v) - 1) >> 1;
}
static long long fk_is_fnval(long long v) {
    if (v >= fk_fnbase) {
        return 0;
    }
    if (v <= fk_fnval_floor) {
        return 0;
    }
    if (((fk_fnbase - v) & 1) == 0) {
        return 0;
    }
    long long fi = fk_fnval_idx(v);
    if (fi >= 0 && fi < fk_fn_count) {
        return 1;
    }
    if (fi >= FK_CLOSURE_IDX_BASE && fi < FK_CLOSURE_IDX_BASE + fk_clo_top) {
        return 1;
    }
    return 0;
}
/* Given an fn-value already known valid (fk_is_fnval), answer the fk_fn[] row to actually run.
 * A plain function's own index IS that row; a closure instance's raw index only names ITS OWN
 * row in the (separate) closure-instance table, which points at the real target. */
static long long fk_fnval_target(long long v) {
    long long fi = fk_fnval_idx(v);
    if (fi < FK_CLOSURE_IDX_BASE) {
        return fi;
    }
    return fk_clo_target[fi - FK_CLOSURE_IDX_BASE];
}
static long long fk_fnval_is_closure(long long v) {
    return fk_fnval_idx(v) >= FK_CLOSURE_IDX_BASE ? 1 : 0;
}
/* The call mechanism's hidden extra channel: whichever call is about to jump into a capturing
 * function's body populates this with that instance's captured values (fresh off the caller's own
 * live frame for a same-scope call, or off a closure instance's frozen snapshot for one reached
 * through an escaped value) immediately before the jump; the callee's own compiled prologue (tag
 * 245, see fk_walk) reads it right back out into its own frame slots before any of its own
 * statements run. Nothing else executes between the write and that read (this is a single-threaded
 * tree-walker: one call's args are written, then control passes straight to the callee), so there
 * is no window for a nested or recursive call to clobber it first. */
static long long fk_call_cap_vals[FK_CLOSURE_CAP_MAX];
static void fk_clo_reserve(long long needed) {
    if (needed <= fk_clo_cap) {
        return;
    }
    long long nc = fk_clo_cap == 0 ? 64 : fk_clo_cap * 2;
    while (nc < needed) {
        nc = nc * 2;
    }
    fk_clo_target = (long long *)realloc(fk_clo_target, (unsigned long)(nc * 8));
    fk_clo_capbase = (long long *)realloc(fk_clo_capbase, (unsigned long)(nc * 8));
    fk_clo_capcount = (long long *)realloc(fk_clo_capcount, (unsigned long)(nc * 8));
    if (fk_clo_target == 0 || fk_clo_capbase == 0 || fk_clo_capcount == 0) {
        fk_die("fk_clo_reserve: out of memory growing the closure-instance table");
    }
    fk_clo_cap = nc;
}
static long long fk_clo_capvals_reserve(long long needed) {
    if (needed <= fk_clo_capvals_cap) {
        return fk_clo_capvals_cap;
    }
    long long nc = fk_clo_capvals_cap == 0 ? 256 : fk_clo_capvals_cap * 2;
    while (nc < needed) {
        nc = nc * 2;
    }
    fk_clo_capvals = (long long *)realloc(fk_clo_capvals, (unsigned long)(nc * 8));
    if (fk_clo_capvals == 0) {
        fk_die("fk_clo_capvals_reserve: out of memory growing the closure capture-values pool");
    }
    fk_clo_capvals_cap = nc;
    return fk_clo_capvals_cap;
}
/* Build one closure instance: target's declared captures are read fresh (native reads against
 * whatever frame the caller supplies -- see the tag-243 walk arm, its only caller) into the shared
 * capvals pool, and a new row records where they landed. Returns the closure's own fn-VALUE. */
static long long fk_clo_make(long long target, long long *vals, long long n) {
    fk_clo_reserve(fk_clo_top + 1);
    long long base = fk_clo_capvals_top;
    fk_clo_capvals_reserve(base + n);
    long long j = 0;
    while (j < n) {
        fk_clo_capvals[base + j] = vals[j];
        j = j + 1;
    }
    fk_clo_capvals_top = base + n;
    long long inst = fk_clo_top;
    fk_clo_target[inst] = target;
    fk_clo_capbase[inst] = base;
    fk_clo_capcount[inst] = n;
    fk_clo_top = fk_clo_top + 1;
    return fk_fnval(FK_CLOSURE_IDX_BASE + inst);
}
/* Float boxes are ODD words at/below fk_fbase-3: fk_fbase - (fp<<1) - 1. They were
 * even (fk_fbase - (fp<<1)) until 2026-07-17, which let a deep-negative INT word
 * (x<<1 for x < fk_fbase/2 ~ -4.5e18) alias float slot (fk_fbase - x*2)/2 at EVERY
 * kind-dispatched door once the pool held that many floats — witnessed: after two
 * 7.0s, (add -4500000000000000002 1) returned the float 8.0 and the int printed
 * as 7. Ints are even (v<<1); with floats odd, every even word is an int across
 * the full 63-bit range. The odd-negative neighbours stay disjoint by band:
 * nothing = fk_fbase+1 (above the ceiling), fn-values ~ -8e18 (above fk_fbase),
 * cons cells positive, node boxes tiny-negative — all excluded by magnitude. */
static long long fk_fidx(long long v) {
    return (fk_fbase - v - 1) >> 1;
}
static long long fk_isf(long long v) {
    if ((v & 1) == 0) {
        return 0;
    }
    long long fi = fk_fidx(v);
    return v <= fk_fbase - 3 && fi > 0 && (fi >= FK_FLT_BASE ? fi - FK_FLT_BASE < fk_field_fp() : fi <= fk_fp);
}
static long long fk_unbox_total_private;
static long long *fk_unbox_total_p = &fk_unbox_total_private;
#define fk_unbox_total (*fk_unbox_total_p)
static long long fk_box_total_private;
static long long *fk_box_total_p = &fk_box_total_private;
#define fk_box_total (*fk_box_total_p)
static long long fk_inram_call_total_private;
static long long *fk_inram_call_total_p = &fk_inram_call_total_private;
#define fk_inram_call_total (*fk_inram_call_total_p)
static long long *fk_fn_unbox;
static long long fk_cur_fn;
static long long fk_fn_capacity;
static double fk_num(long long v) {
    if (fk_isf(v)) {
        fk_unbox_total = fk_unbox_total + 1;
        if (fk_fn_unbox != 0 && fk_cur_fn > 0 && fk_cur_fn < fk_fn_capacity) { fk_fn_unbox[fk_cur_fn] = fk_fn_unbox[fk_cur_fn] + 1; }
        return FK_FV(fk_fidx(v));
    }
    return (double)(v >> 1);
}
/* ── boxing attribution: WHICH recipe is minting floats ─────────────────────
 * Ints ride unboxed (tagged words); every float RESULT allocates a pool slot
 * here. To answer "which sources could use unboxing", each mint is charged to
 * the last-dispatched recipe (fk_cur_fn, set beside every heat bump). A call
 * with a return point (fk_walk's arms 12/240/241/244) saves the caller and
 * restores it when the callee returns, so a box minted after the return is the
 * caller's; a tail-jump arm in fk_walk_body has no return point and hands the
 * frame to the callee for good -- exact for the hot loops that matter.
 * Written to .fkwu-boxing.<pid> beside the heat board. */
static long long fk_cur_fn;
static long long *fk_fn_fbox;
#define FK_F64_HEAT 1024
static long long *fk_fn_unbox;      /* per-recipe float READS (a pool slot dereferenced per operand); beside fk_fn_fbox, the mints */
static long long fk_fn_capacity;
static long long fk_fbox(double d) {
    if (fk_fv == 0) {
        fk_fcap = FK_FLOAT_POOL_INIT_CAP;
        fk_fv = (double *)fk_store_take('F', FK_STORE_FLOATS * 8);
        if (fk_fv == 0) { fk_store_go_private(); fk_fv = malloc(fk_fcap * 8); }
        if (fk_fv == 0) {
            fk_die("fk_fbox: out of memory");
        }
    }
    fk_fp = fk_fp + 1;
    if (fk_fp >= fk_fcap) {
        fk_fv = (double *)fk_store_grow('F', fk_fv, fk_fcap * 8, fk_fcap * 16, FK_STORE_FLOATS * 8, 0);
        fk_fcap = fk_fcap * 2;
        if (fk_fv == 0) {
            fk_die("fk_fbox: out of memory growing float pool");
        }
    }
    fk_fv[fk_fp] = d;
    fk_box_total = fk_box_total + 1;
    if (fk_fn_fbox != 0 && fk_cur_fn > 0 && fk_cur_fn < fk_fn_capacity) {
        long long nb = fk_fn_fbox[fk_cur_fn] + 1;
        fk_fn_fbox[fk_cur_fn] = nb;
        if ((nb & (FK_F64_HEAT - 1)) == 0) { fk_f64_pulse(fk_cur_fn); } /* the box ledger is the JIT trigger: every FK_F64_HEAT boxes a cold defn is asked once more */
    }
    return fk_fbase - (fk_fp << 1) - 1;
}
static void fk_pr(long long v) {
    char b[32];
    int n = 0;
    if (v < 0) {
        putchar(45);
        v = 0 - v;
    }
    if (v == 0) {
        putchar(48);
    }
    while (v > 0) {
        b[n] = 48 + v % 10;
        v = v / 10;
        n = n + 1;
    }
    while (n > 0) {
        n = n - 1;
        putchar(b[n]);
    }
    putchar(10);
}
extern int sprintf(char *, const char *, ...);
extern double strtod(const char *, char **);
/* fk_fmt_float_js — the ONE float rendering, byte-identical to the Go kernel's
 * core.FormatFloatJS (strconv.FormatFloat(f,'g',-1,64) with NaN/Inf spelled out).
 *
 * fkwu used to print floats with printf's %.15g (fk_pv) and %.17g
 * (fk_pv_inline_number). Neither is Go's rule, and the gap is not academic:
 * MEASURED 2026-07-31 on this checkout, the source `1000000.0` answered
 *   bin-go  1e+06
 *   fkwu    1000000
 * -- a four-arm divergence sitting in the top-level VERDICT printer, so any band
 * whose verdict is a float at or past 1e6 (or below 1e-4) disagreed with three
 * siblings while validate.sh compared the other three and called it green.
 *
 * Go's shortest 'g' is two decisions. First the DIGITS: the fewest decimal digits
 * that round-trip back to the same float64. printf cannot be asked for that
 * directly, so ask for each precision in turn and stop at the first that strtod
 * returns the identical bits for -- the shortest round-tripping digit string is
 * unique, so this reaches the same digits Ryu does. Second the LAYOUT: with
 * exp = (decimal point position - 1), Go uses %e when exp < -4 || exp >= 6 and
 * %f otherwise. That 6 is a literal in Go's shortest path (eprec is pinned to 6,
 * NOT to the digit count) -- it is exactly why 1e6 leaves fixed notation while
 * 999999 stays in it. In the %e branch the precision is nd-1; in %f it is
 * max(nd-dp, 0). C and Go agree on %e's shape (sign always present, at least two
 * exponent digits), so the branch can hand the formatting back to printf.
 *
 * PROVEN, not reasoned: 2,000,000 values -- 1.5M uniform-random 64-bit patterns
 * (subnormals, extreme exponents, NaN payloads), 500k near-tie decimal
 * round-trips, plus every boundary at exp -5/-4/5/6, the integral floats, and
 * 0/-0/DBL_MAX/DBL_MIN/inf/nan -- rendered by this code and by the Go kernel's
 * own FormatFloatJS, compared byte for byte, zero mismatches.
 *
 * No libm: NaN is (f != f) and infinity is a comparison against DBL_MAX, so the
 * one-cc seed keeps building with no -lm. */
static void fk_fmt_float_js(double f, char *out) {
    if (f != f) {
        out[0] = 'N'; out[1] = 'a'; out[2] = 'N'; out[3] = 0;
        return;
    }
    if (f > 1.7976931348623157e308) {
        sprintf(out, "Infinity");
        return;
    }
    if (f < -1.7976931348623157e308) {
        sprintf(out, "-Infinity");
        return;
    }
    char ebuf[64];
    long long p = 0;
    while (p <= 17) {
        sprintf(ebuf, "%.*e", (int)p, f);
        if (strtod(ebuf, (char **)0) == f) {
            break;
        }
        p = p + 1;
    }
    if (p > 17) {
        p = 17;
    }
    long long nd = p + 1;
    /* the exponent printf just wrote IS dp-1; read it back rather than re-deriving */
    long long k = 0;
    while (ebuf[k] != 0 && ebuf[k] != FK_CH_LOWER_E) {
        k = k + 1;
    }
    long long exp10 = 0;
    long long esign = 1;
    if (ebuf[k] == FK_CH_LOWER_E) {
        k = k + 1;
        if (ebuf[k] == FK_CH_DASH) {
            esign = 0 - 1;
            k = k + 1;
        } else if (ebuf[k] == FK_CH_PLUS) {
            k = k + 1;
        }
        while (ebuf[k] >= FK_CH_DIGIT0 && ebuf[k] <= FK_CH_DIGIT9) {
            exp10 = exp10 * 10 + (ebuf[k] - FK_CH_DIGIT0);
            k = k + 1;
        }
        exp10 = exp10 * esign;
    }
    if (exp10 < 0 - 4 || exp10 >= 6) {
        sprintf(out, "%.*e", (int)(nd - 1), f);
        return;
    }
    long long dp = exp10 + 1;
    long long prec = nd - dp;
    if (prec < 0) {
        prec = 0;
    }
    sprintf(out, "%.*f", (int)prec, f);
}
static long long fk_is_str(long long v);
/* writes a string value's bytes to stdout, no newline (defined with the string pool
 * below -- fk_pv is declared above it and cannot reach fk_sb/fk_so/fk_sl directly) */
static void fk_put_str(long long v);
static void fk_pv(long long v) {
    if (v == fk_nothing) {
        printf("nothing\n");
        return;
    }
    if (fk_is_str(v)) {
        fk_put_str(v);
        putchar(10);
        return;
    }
    if (fk_isf(v)) {
        char fb[64];
        fk_fmt_float_js(fk_num(v), fb);
        printf("%s\n", fb);
    } else {
        if ((v & 1) == 0) {
            fk_pr(v >> 1);
        } else {
            fk_pr(v);
        }
    }
}
static long long fk_arms_private[FK_OPCODE_ARM_CAP];
static long long *fk_arms = fk_arms_private;   /* per-tag hit counters; the live page own words once it opens */
/* the once-hold's node tag (declared early: the walker arm reads it far
 * above the const-table machinery that owns the rest of the mechanism).
 *
 * THIS TAG IS TAKEN, AND IT READS FREE. Every census of the tag space — the
 * mirror census's arm mirror, and every hand that has grepped this file —
 * looks for the arm's own spelling, `if (t == 190)`, and this one is spelled
 * `if (t == FK_TAG_CONST_HOLD)` instead. Three collisions in the week of
 * 2026-09-05 walked into it, and a fourth on 2026-09-08 took 190 for the
 * speaking doors and watched every one of them answer its own mode number
 * back: the once-hold walks a node's first child and returns it, so the
 * collision is silent, self-consistent and green. The line below is the tag
 * written in the notation the censuses read, so the number stops feigning
 * freedom (freefeint, corpus row 1371). It changes no dispatch — the arm it
 * names is the real one, twelve thousand lines down — and it makes 190 what
 * it has always been: an arm no op row names. */
#define FK_TAG_CONST_HOLD 190 /* arm: if (t == 190) — the once-hold, below */
static long long *fk_mem;
static long long fk_mem_cap;
static void fk_mem_reserve(long long need) {
    long long nc;
    long long i;
    long long *q;
    if (need <= fk_mem_cap) {
        return;
    }
    nc = fk_mem_cap == 0 ? FK_MEM_CELL_CAP_INIT : fk_mem_cap;
    while (nc < need) {
        nc = nc * 2;
    }
    q = realloc(fk_mem, (unsigned long)(nc * 8));
    if (q == 0) {
        fk_die("fk_mem_reserve: out of memory growing the cell table");
    }
    fk_mem = q;
    i = fk_mem_cap;
    while (i < nc) {
        fk_mem[i] = 0;
        i = i + 1;
    }
    fk_mem_cap = nc;
}
static char *fk_src;
static long long fk_src_cap;
static long long fk_src_len = 0;
static long long *fk_hh;
static long long *fk_ht;
static long long fk_hp;
static long long fk_cap;
static long long *fk_vs;
static long long fk_vs_cap;    /* live capacity; fk_vs_grow owns it */
static long long fk_vs_grows;  /* doublings this run -- kernel_stat 28 */
static long long fk_bd_cap;    /* binding-stack live capacity (kernel_stat 29); owned by fk_bd_push */
static long long fk_bd_grows;  /* binding-stack doublings (kernel_stat 30) */
static long long fk_bd_save_cap, fk_bd_save_grows; /* binding save stack live cap / doublings (kernel_stat 33/34); owned by fk_bd_save */
static void fk_vs_grow(long long need) {
    long long nc = fk_vs_cap == 0 ? FK_VALUE_STACK_CAP_INIT : fk_vs_cap;
    long long i;
    long long *q;
    while (nc < need) {
        nc = nc * 2;
    }
    q = realloc(fk_vs, (unsigned long)(nc * 8));
    if (q == 0) {
        fk_die("fk_vs_grow: out of memory growing the value stack");
    }
    fk_vs = q;
    i = fk_vs_cap;
    while (i < nc) {
        fk_vs[i] = 0;
        i = i + 1;
    }
    if (fk_vs_cap != 0) {
        fk_vs_grows = fk_vs_grows + 1;
    }
    fk_vs_cap = nc;
}
static long long fk_vsp;
extern unsigned int arc4random(void);
extern void *calloc(unsigned long, unsigned long);
extern void free(void *);
extern double strtod(const char *, char **);
extern void *popen(const char *, const char *);
extern int pclose(void *);
#ifdef _WIN32
#define fileno _fileno
#endif
extern int fileno(void *);
static char *fk_sb;
static long long *fk_so;
static long long *fk_sl;
static long long fk_scap_b;
static long long fk_scap_s;
static long long fk_sp;
static long long fk_sbp;
/* fk_sintern used to be a linear scan over every already-interned string on
 * EVERY intern call -- O(n) per call, O(n^2) total parse time in the number of
 * distinct strings/identifiers in a program, since the parser interns once per
 * identifier and string literal. fk_shash/fk_snext add a fixed-size (never
 * rehashed) hash index alongside fk_so/fk_sl: fk_shash[bucket] is the head of a
 * singly-linked chain (via fk_snext, indexed the same as fk_so/fk_sl) of every
 * string index that landed in that bucket. fk_so/fk_sl remain the source of
 * truth for what's actually interned; the hash is purely an accelerated lookup,
 * so a hash or chain bug's worst case is a false miss -- falling back to
 * re-interning (still correct, just not faster), never returning a wrong index
 * or missing a real duplicate (the chain walk still byte-compares before
 * matching). FK_STRING_HASH_BUCKETS is fixed rather than grown alongside
 * fk_scap_s specifically to avoid rehashing-on-grow, the one part of a growable
 * hash table that's easy to get subtly wrong; a fixed table sized well above the
 * initial string-table capacity keeps chains short for realistic programs at a
 * flat, one-time 1MB cost. */
#define FK_STRING_HASH_BUCKETS 131072 /* power of two, so `& (N-1)` is a valid mask */
static long long *fk_shash;
static long long *fk_snext;
static unsigned char *fk_sdead;  /* 1 = the slot was freed by the string melt and waits on fk_sfree */
static long long *fk_sfree;
static long long fk_sfree_n;
static void fk_sinit(void) {
    if (fk_sb == 0) {
        fk_scap_b = FK_STRING_POOL_INIT_BYTES;
        fk_scap_s = FK_STRING_TABLE_INIT_CAP;
        fk_sb = (char *)fk_store_take('s', FK_STORE_STR_BYTES);
        fk_so = fk_sb == 0 ? 0 : (long long *)fk_store_take('O', FK_STORE_STR_CELLS * 8);
        fk_sl = fk_so == 0 ? 0 : (long long *)fk_store_take('L', FK_STORE_STR_CELLS * 8);
        if (fk_sl == 0) {
            fk_store_go_private();
            fk_sb = malloc(fk_scap_b);
            fk_so = malloc(fk_scap_s * 8);
            fk_sl = malloc(fk_scap_s * 8);
        }
        fk_snext = malloc(fk_scap_s * 8);
        fk_sdead = calloc((unsigned long)fk_scap_s, 1);
        fk_sfree = malloc(fk_scap_s * 8);
        fk_sfree_n = 0;
        fk_shash = malloc(FK_STRING_HASH_BUCKETS * 8);
        if (fk_sb == 0 || fk_so == 0 || fk_sl == 0 || fk_snext == 0 || fk_shash == 0) {
            fk_die("fk_sinit: out of memory");
        }
        long long k = 0;
        while (k < FK_STRING_HASH_BUCKETS) {
            fk_shash[k] = -1;
            k = k + 1;
        }
    }
}
/* FNV-1a over fk_sb[off..off+len); returns an already-masked bucket index.
 * Runs the accumulator in unsigned 64-bit specifically to keep the multiply
 * well-defined (signed overflow on `long long` is undefined behavior in C; unsigned
 * overflow wraps, which is exactly what FNV-1a wants). */
static long long fk_str_hash(long long off, long long len) {
    unsigned long long h = 14695981039346656037ULL;
    long long k = 0;
    while (k < len) {
        h = h ^ (unsigned long long)(unsigned char)fk_sb[off + k];
        h = h * 1099511628211ULL;
        k = k + 1;
    }
    return (long long)(h & (FK_STRING_HASH_BUCKETS - 1));
}
static long long fk_sintern(long long off, long long len) {
    fk_sinit();
    long long bucket = fk_str_hash(off, len);
    long long c = fk_shash[bucket];
    while (c >= 0) {
        if (FK_SLEN(c) == len) {
            long long j = 0;
            while (j < len && FK_SBYTES(c)[j] == fk_sb[off + j]) {
                j = j + 1;
            }
            if (j == len) {
                return c;
            }
        }
        c = fk_snext[c];
    }
    if (fk_sfree_n > 0) {
        /* a slot the string melt freed: reuse its index; its old bytes too when the new string fits (the scratch just written at off is then discarded) */
        long long r = fk_sfree[fk_sfree_n - 1];
        fk_sfree_n = fk_sfree_n - 1;
        if (len <= fk_sl[r] && off + len == fk_sbp) {
            long long j = 0;
            while (j < len) { fk_sb[fk_so[r] + j] = fk_sb[off + j]; j = j + 1; }
            fk_sbp = off;
        } else {
            fk_so[r] = off;
            fk_sbp = off + len;
        }
        fk_sl[r] = len;
        fk_sdead[r] = 0;
        fk_snext[r] = fk_shash[bucket];
        fk_shash[bucket] = r;
        return r;
    }
    long long i = fk_sp;
    if (i >= fk_scap_s) {
        fk_so = (long long *)fk_store_grow('O', fk_so, fk_scap_s * 8, fk_scap_s * 16, FK_STORE_STR_CELLS * 8, 0);
        fk_sl = (long long *)fk_store_grow('L', fk_sl, fk_scap_s * 8, fk_scap_s * 16, FK_STORE_STR_CELLS * 8, 0);
        fk_scap_s = fk_scap_s * 2;
        fk_snext = realloc(fk_snext, fk_scap_s * 8);
        fk_sdead = realloc(fk_sdead, (unsigned long)fk_scap_s);
        fk_sfree = realloc(fk_sfree, fk_scap_s * 8);
        { long long z = fk_scap_s / 2; while (z < fk_scap_s) { fk_sdead[z] = 0; z = z + 1; } }
        if (fk_so == 0 || fk_sl == 0 || fk_snext == 0) {
            fk_die("fk_sintern: out of memory growing string table");
        }
    }
    fk_so[i] = off;
    fk_sl[i] = len;
    fk_snext[i] = fk_shash[bucket];
    fk_shash[bucket] = i;
    fk_sp = i + 1;
    fk_sbp = off + len;
    return i;
}
/* stone: a STRING VALUE is its own odd-negative band, minted exactly like the boxed
 * float (fk_fbase) and the fn-value (fk_fnbase) before it.
 *
 * Until 2026-07-31 a string value was `poolidx << 1` -- POSITIVE EVEN, the very same
 * word an int of that index is. The comment above fk_nothing said "not a string/list
 * (those are positive)" while the comment above fk_fidx said "every even word is an
 * int across the full 63-bit range"; both cannot hold, and the string lost. MEASURED
 * on this checkout before the change: with "alpha" interned first,
 *   (print_str (add 0 0))   printed   alpha
 * -- an int walked into a string door and came out as somebody else's text. Every
 * string-typed op survived only because its CALL SITE declared the type; no
 * kind-dispatched door (a `print` that renders whatever it is given) could exist at
 * all, because the word carries no answer to "which kind are you".
 *
 * This is the same defect the float band healed on 2026-07-17 (a deep-negative int
 * aliasing a float slot at every kind-dispatched door), and it takes the same cure
 * rather than a magnitude heuristic: a magnitude split was re-examined and REFUSED
 * here, because now_unix_ms already returns ~1.7e12, so any "ints are small, strings
 * are big" line is crossed by a clock reading on the first call.
 *
 * fk_sbase = -8.5e18 sits BELOW the fn-value band (fk_fnbase = -8e18, width 8192) and
 * ABOVE nothing (-8.999e18) and the float base (-9e18, floats at or below it), so
 * fk_strv(si) = fk_sbase - (si<<1) - 1 is: not an int (ints even), not a float
 * (fk_isf needs v <= fk_fbase-3 ~ -9e18; these are ~-8.5e18, above it), not a
 * fn-value (fk_is_fnval excludes v <= fk_fnbase - 16384), not nothing (distinct
 * constant), not a record ((0-v) is odd here, records even), not a cons cell or nil
 * (positive), not a node (fk_nidx maps ~8.5e18 far past fk_np). Room for ~2.5e17
 * strings before the band meets nothing.
 *
 * fk_stri is the ONE door from a value word back to a pool index. It answers -1 for
 * anything that is not a string value, so the existing `sa < 0 || !FK_SOK(sa)` guards
 * at every string-typed op keep their meaning unchanged and a mistyped argument is
 * refused instead of read as text. */
static const long long fk_sbase = -8500000000000000000LL;
static long long fk_strv(long long si) {
    return fk_sbase - (si << 1) - 1;
}
static long long fk_is_str(long long v) {
    if ((v & 1) == 0) {
        return 0;
    }
    if (v > fk_sbase - 1) {
        return 0;
    }
    long long si = (fk_sbase - v - 1) >> 1;
    return (si >= 0 && FK_SOK(si)) ? 1 : 0;
}
static long long fk_stri(long long v) {
    if (!fk_is_str(v)) {
        return 0 - 1;
    }
    return (fk_sbase - v - 1) >> 1;
}
static void fk_put_str(long long v) {
    long long si = fk_stri(v);
    if (si < 0) {
        return;
    }
    long long j = 0;
    while (j < FK_SLEN(si)) {
        putchar((int)(unsigned char)FK_SBYTES(si)[j]);
        j = j + 1;
    }
}
static long long *fk_nkind;
static long long *fk_ncat;
static long long *fk_nkids;
static long long *fk_nval;
static long long (*fk_nid)[4];
static long long fk_np_private;
static volatile long long *fk_np_p = &fk_np_private;
#define fk_np (*fk_np_p)
static long long fk_nbox(long long i) {
    return 0 - (((long long)i << 1) | 1);
}
static long long fk_nidx(long long v) {
    return (((0 - v) - 1) >> 1);
}
static long long fk_veq(long long a, long long b);
static long long fk_neq(long long a, long long b) {
    if (a == b) {
        return 1;
    }
    if (a >= 0 || b >= 0) {
        return 0;
    }
    long long ia = fk_nidx(a);
    long long ib = fk_nidx(b);
    if (ia < 1 || ia > fk_np || ib < 1 || ib > fk_np) {
        return 0;
    }
    if (fk_nkind[ia] != fk_nkind[ib]) {
        return 0;
    }
    if (fk_nkind[ia] == 1) {
        if (fk_nid[ia][2] != fk_nid[ib][2]) {
            return 0;
        }
        if (fk_nid[ia][2] == 7 || fk_nid[ia][2] == 6) {
            double fna = fk_num(fk_nval[ia]);
            double fnb = fk_num(fk_nval[ib]);
            return ((fna == fnb) || (fna != fna && fnb != fnb)) ? 1 : 0;
        }
        return fk_nval[ia] == fk_nval[ib] || (fk_is_str(fk_nval[ia]) && fk_is_str(fk_nval[ib]) && fk_str_bytes_eq(fk_nval[ia], fk_nval[ib]));
    }
    if (fk_nkind[ia] == 3) {
        return fk_nid[ia][0] == fk_nid[ib][0] && fk_nid[ia][1] == fk_nid[ib][1] &&
               fk_nid[ia][2] == fk_nid[ib][2] && fk_nid[ia][3] == fk_nid[ib][3];
    }
    if (fk_veq(fk_ncat[ia], fk_ncat[ib]) == 0) {
        return 0;
    }
    return fk_veq(fk_nkids[ia], fk_nkids[ib]);
}
/* fk_movable — can a melt take this word's meaning away while it waits?
 *
 * The seed reclaims two things mid-run, and each takes a different kind of
 * word with it: fk_melt compacts the cons arena, so every live PAIR moves and
 * every pair index changes; fk_smelt hands dead LOCAL string slots back on
 * fk_sfree, and fk_sintern gives the next string the same index with other
 * bytes. A word of either kind that an evaluator arm holds in a C local while
 * it walks something else is a word the reclaimer cannot see and therefore
 * cannot fix. Everything else keeps its meaning across a melt: ints and floats
 * carry their value in the word, value nodes never relocate, field strings are
 * shared by content and are never reclaimed, and nil and nothing are stones.
 *
 * Two-arg arms use this to root only when rooting is needed. The int test is
 * first and is one AND: `eq` and `lt` walk integers in every loop this body
 * runs, and they must not pay for a hazard they cannot have. */
static long long fk_movable(long long v) {
    if ((v & 1) == 0) {
        return 0;                       /* an int carries itself */
    }
    if (v > 1) {
        return 1;                       /* a cons pair index — the arena compacts */
    }
    if (v == 1) {
        return 0;                       /* nil */
    }
    long long si = fk_stri(v);          /* negative and odd: string, node, float, nothing */
    return (si >= 0 && si < FK_STR_BASE) ? 1 : 0;
}
static long long fk_veq(long long a, long long b) {
    if (a == b) {
        return 1;
    }
    if (fk_isf(a) && fk_isf(b)) {
        double fa = fk_num(a), fb = fk_num(b);
        return (fa == fb || (fa != fa && fb != fb)) ? 1 : 0;
    }
    if (fk_is_str(a) && fk_is_str(b)) {
        return fk_str_bytes_eq(a, b);
    }
    if (a < 0 || b < 0) {
        return fk_neq(a, b);
    }
    if ((a & 1) && (b & 1)) {
        long long pa = a >> 1;
        long long pb = b >> 1;
        if (pa < 1 || !FK_POK(pa) || pb < 1 || !FK_POK(pb)) {
            return 0;
        }
        if (fk_veq(FK_HH(pa), FK_HH(pb)) == 0) {
            return 0;
        }
        return fk_veq(FK_HT(pa), FK_HT(pb));
    }
    return 0;
}
/* ── the intern index ───────────────────────────────────────────────────
 * The four interning doors (trivial int t43, trivial string t46, bool
 * t112, composite intern_node t47) each scanned the WHOLE value-node
 * pool per call — a global quadratic that dominated every intern-heavy
 * program (witnessed 2026-08-30: the BML compile of a 7KB surface spent
 * 12s in emit at ~n^1.7; the interpreter's own floor, not the
 * program's). This open-addressed index makes the doors O(1) expected
 * while preserving their EXACT equivalence: a hash hit is only a
 * CANDIDATE — each door's original predicate confirms before anything
 * is returned, so a collision costs a compare, never a wrong node.
 * Deep hashing mirrors fk_veq's structure over element handles and is
 * stable across melt (value nodes never relocate; cons chains are
 * hashed by content at call time). Kind-3 NodeIDs and kind-1 floats are
 * minted without dedupe by design and stay outside the index — the
 * doors never searched them. The table is 4x the node cap, so load
 * stays low and probes short. */
#define FK_INTERN_HASH_CAP_INIT 1048576
static long long fk_intern_hash_cap;    /* held at 4x the node cap; power of two */
static long long *fk_intern_tab;
static long long *fk_nhash_memo;
static unsigned long long fk_mix64(unsigned long long h, unsigned long long v) {
    h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
    h *= 0xff51afd7ed558ccdULL;
    h ^= h >> 33;
    return h;
}
static long long fk_deep_hash(long long v);
static long long fk_deep_hash_node(long long idx) {
    if (fk_nhash_memo[idx]) {
        return fk_nhash_memo[idx];
    }
    unsigned long long h = fk_mix64(7, (unsigned long long)fk_nkind[idx]);
    if (fk_nkind[idx] == 1) {
        h = fk_mix64(h, (unsigned long long)fk_nid[idx][2]);
        h = fk_mix64(h, (unsigned long long)fk_nval[idx]);
    } else if (fk_nkind[idx] == 3) {
        h = fk_mix64(h, (unsigned long long)fk_nid[idx][0]);
        h = fk_mix64(h, (unsigned long long)fk_nid[idx][1]);
        h = fk_mix64(h, (unsigned long long)fk_nid[idx][2]);
        h = fk_mix64(h, (unsigned long long)fk_nid[idx][3]);
    } else {
        h = fk_mix64(h, (unsigned long long)fk_deep_hash(fk_ncat[idx]));
        h = fk_mix64(h, (unsigned long long)fk_deep_hash(fk_nkids[idx]));
    }
    long long out = (long long)(h >> 1);
    if (out == 0) {
        out = 1;
    }
    fk_nhash_memo[idx] = out;
    return out;
}
static long long fk_deep_hash(long long v) {
    if (v >= 0) {
        if ((v & 1) == 0) {
            return (long long)(fk_mix64(2, (unsigned long long)v) >> 1);
        }
        unsigned long long h = fk_mix64(3, 17);
        long long p = v;
        long long guard = 0;
        while ((p & 1) && p != 1 && guard < 4194304) {
            long long pa = p >> 1;
            if (pa < 1 || !FK_POK(pa)) {
                break;
            }
            h = fk_mix64(h, (unsigned long long)fk_deep_hash(FK_HH(pa)));
            p = FK_HT(pa);
            guard = guard + 1;
        }
        h = fk_mix64(h, (unsigned long long)p);
        return (long long)(h >> 1);
    }
    if (v == fk_nothing) { return (long long)(fk_mix64(13, 1) >> 1); }
    if (fk_is_str(v)) { long long si = fk_stri(v); return (long long)(fk_mix64(17, fk_field_bytes_hash(FK_SBYTES(si), FK_SLEN(si))) >> 1); }
    if (fk_isf(v)) { double d = fk_num(v); unsigned long long bits; { char *pd = (char *)&d; char *pb = (char *)&bits; int b = 0; while (b < 8) { pb[b] = pd[b]; b = b + 1; } } if (d == 0.0) { bits = 0; } return (long long)(fk_mix64(19, bits) >> 1); }
    long long idx = fk_nidx(v);
    if (idx < 1 || idx > fk_np) {
        return (long long)(fk_mix64(5, (unsigned long long)v) >> 1);
    }
    return fk_deep_hash_node(idx);
}
static long long fk_intern_key_trivial(long long type, long long val) {
    unsigned long long h = fk_mix64(7, 1);
    h = fk_mix64(h, (unsigned long long)type);
    h = fk_mix64(h, (unsigned long long)val);
    long long out = (long long)(h >> 1);
    return out == 0 ? 1 : out;
}
static long long *fk_nsfile;
static long long *fk_nsline;
static long long *fk_nscol;
static long long *fk_nsattr;
static long long *fk_fbroots;
static long long fk_fbn;
/* the four framebuffer counters the sibling table-walker lane already defines
 * (form/form-stdlib/fkc-table-serialize.fk, kernel_stat keys 11..14). The seed
 * recorded roots without ever counting the calls that reached fb_record, so a
 * refused attribution -- a value that was not a live cell handle -- left no
 * trace at all: the glass could see roots arrive but never that one had been
 * turned away. kernel_stat 9/11/12/13/14 read them. */
static long long fk_fbrejected;  /* fb_record calls whose value was not a live cell handle (kernel_stat 11) */
static long long fk_fbentered;   /* fb_record calls entered (kernel_stat 12) */
static long long fk_fbaccepted;  /* fb_record calls accepted (kernel_stat 13) */
static long long fk_fblastidx;   /* the last node index fb_record saw, accepted or not (kernel_stat 14) */
#define FK_FB_RING 2048 /* the framebuffer keeps the newest roots: a buffer, not a ledger -- framebuffer-events answers at most this many, oldest first */
static void **fk_gift_base;      /* gift frames: mapped bases (0 = released) */
static long long *fk_gift_size;  /* gift frames: mapped sizes */
static long long fk_gift_count;
static long long fk_gift_cap;
#if defined(__has_include) && !defined(_WIN32)
#if __has_include(<sys/stat.h>)
#include <sys/stat.h>
#define FK_HAVE_STAT_HEADER 1
#endif
#endif
#ifndef FK_HAVE_STAT_HEADER
extern int mkdir(const char *, unsigned int);
extern int stat(const char *, void *);
#endif
#if defined(__has_include) && !defined(_WIN32)
#if __has_include(<fcntl.h>)
#include <fcntl.h>
#define FK_HAVE_FCNTL_HEADER 1
#endif
#if defined(__has_include) && !defined(_WIN32)
#if __has_include(<sys/mman.h>)
#include <sys/mman.h>
#define FK_HAVE_MMAN_HEADER 1
#endif
/* the controlling terminal's window, by hand as every host door here is declared:
 * no <sys/ioctl.h> (it drags the socket headers against the hand-declared
 * socket externs below). TIOCGWINSZ is the platform's own number. */
struct fk_winsize { unsigned short ws_row; unsigned short ws_col; unsigned short ws_xpixel; unsigned short ws_ypixel; };
#if !defined(_WIN32)
extern int ioctl(int, unsigned long, ...);
#if defined(__APPLE__)
#define FK_TIOCGWINSZ 0x40087468UL
#else
#define FK_TIOCGWINSZ 0x5413UL
#endif
#endif
#if defined(__has_include) && !defined(_WIN32)
#if __has_include(<time.h>)
#include <time.h>
#include <sched.h>
#endif
#endif
#endif
#endif
#if defined(__has_include) && !defined(_WIN32)
#if __has_include(<dirent.h>)
#include <dirent.h>
#define FK_HAVE_DIRENT_HEADER 1
#endif
#endif
#if defined(__has_include) && !defined(_WIN32)
#if __has_include(<errno.h>)
#include <errno.h>
#endif
#endif
/* the BML floor's lowering door spawns the runner on itself; the seed
 * declares its own POSIX signatures (system headers conflict with the
 * file's hand-rolled read/write/lseek), so the door follows that idiom */
#if !defined(_WIN32)
extern int fork(void);
extern int execvp(const char *, char *const *);
extern int waitpid(int, int *, int);
extern int pipe(int *);
extern int dup2(int, int);
extern void _exit(int);
/* host-exec's stdin door interleaves its write with its read under poll and
 * holds SIGPIPE off while writing to a child that may already have closed */
#include <poll.h>
#include <signal.h>
#include <termios.h>
#endif
#if defined(__APPLE__) && (defined(__aarch64__) || defined(__arm64__))
#define FK_HAVE_DARWIN_ARM64_JIT_WITNESS 1
#endif
#ifndef EINTR
#define EINTR 4
#endif
#ifndef errno
extern int errno;
#endif
#ifndef FK_HAVE_FCNTL_HEADER
#if defined(_WIN32)
/* ucrt flag values; _O_BINARY folded into O_WRONLY so writes keep bytes as given (the read
 * path already opens 0x8000). The BSD values below silently dropped _O_CREAT here, so the
 * write/append doors failed on absent files and append truncated on present ones. */
#define O_WRONLY 0x8001
#define O_CREAT 0x100
#define O_TRUNC 0x200
#define O_APPEND 8
#else
#define O_WRONLY 1
#define O_CREAT 0x200
#define O_TRUNC 0x400
#define O_APPEND 8
#endif
#endif
#if defined(_WIN32)
/* _O_BINARY for the Form-facing byte-read doors (read_file / read_file_slice): text mode
 * would translate CRLF and stop at a 0x1A byte — a binary checkpoint could not pass. */
#define O_RDBIN 0x8000
#else
#define O_RDBIN 0
#endif
extern int open(const char *, int, ...);
extern long long read(int, void *, unsigned long);
extern int close(int);
extern long lseek(int, long, int);
extern int ftruncate(int, long long);
extern int rmdir(const char *);
extern int unlink(const char *);
extern int rename(const char *, const char *);
extern int getpid(void);
extern int sprintf(char *, const char *, ...);
extern char *getenv(const char *);
static long long fk_read_all_bounded(int fd, char *buf, long long cap) {
    long long total = 0;
    while (total < cap) {
        long long got = read(fd, buf + total, (unsigned long)(cap - total));
        if (got > 0) {
            total = total + got;
        } else if (got == 0) {
            return total;
        } else if (errno == EINTR) {
            /* retry */
        } else {
            return -1;
        }
    }
    while (1) {
        char extra;
        long long got = read(fd, &extra, 1);
        if (got > 0) {
            return -2;
        }
        if (got == 0) {
            return total;
        }
        if (errno != EINTR) {
            return -1;
        }
    }
}
/* ── config file (fkwu.conf in cwd), read ONCE and lazily. Replaces the FK_* / FORM_* / MESH_*
 * env-var toggles: a config file is a durable, reviewable surface where scattered env vars are not.
 * Absent file -> empty config -> every toggle at its default (recover, never die). Standard OS env
 * vars we do not own (TMPDIR) stay on getenv. Line form: "KEY value" or "KEY=value"; bare "KEY"
 * means on ("1"); "KEY 0" means off; '#' begins a comment. */
#define FK_CONF_MAX_INIT 64 /* conf entries birth size; grows (kernel_stat 37/38) -- an entry past the old fixed 64 was SILENTLY inert, the same family as the silently-inert env probe */
#define FK_CONF_POOL_INIT 4096 /* conf key/value bytes birth size; grows (kernel_stat 39/40) -- every key and value is held at its exact length. The old per-entry widths (64, 4096) cut a longer key or value silently, and a cut key matches nothing. */
static long long *fk_conf_k, *fk_conf_v; /* offsets into fk_conf_pool */
static long long fk_conf_cap, fk_conf_grows;
static char *fk_conf_pool;
static long long fk_conf_pool_n, fk_conf_pool_cap, fk_conf_pool_grows;
static void fk_conf_reserve(long long need) {
    long long nc;
    if (need <= fk_conf_cap) {
        return;
    }
    nc = fk_conf_cap == 0 ? FK_CONF_MAX_INIT : fk_conf_cap;
    while (nc < need) {
        nc = nc * 2;
        fk_conf_grows = fk_conf_grows + 1;
    }
    fk_conf_k = (long long *)realloc(fk_conf_k, (unsigned long)(nc * 8));
    fk_conf_v = (long long *)realloc(fk_conf_v, (unsigned long)(nc * 8));
    if (fk_conf_k == 0 || fk_conf_v == 0) {
        fk_die("fk_conf_reserve: out of memory growing the conf table");
    }
    fk_conf_cap = nc;
}
/* append s[0..n) and a terminator to the conf byte pool; answers the offset */
static long long fk_conf_pool_put(const char *s, long long n) {
    long long off = fk_conf_pool_n;
    long long need = off + n + 1;
    long long i = 0;
    if (need > fk_conf_pool_cap) {
        long long nc = fk_conf_pool_cap == 0 ? FK_CONF_POOL_INIT : fk_conf_pool_cap;
        while (nc < need) {
            nc = nc * 2;
            fk_conf_pool_grows = fk_conf_pool_grows + 1;
        }
        fk_conf_pool = (char *)realloc(fk_conf_pool, (unsigned long)nc);
        if (fk_conf_pool == 0) {
            fk_die("fk_conf_pool_put: out of memory growing the conf byte pool");
        }
        fk_conf_pool_cap = nc;
    }
    while (i < n) {
        fk_conf_pool[off + i] = s[i];
        i = i + 1;
    }
    fk_conf_pool[off + n] = 0;
    fk_conf_pool_n = need;
    return off;
}
static int fk_conf_n = 0;
static int fk_conf_loaded = 0;
static void fk_conf_load(void) {
    if (fk_conf_loaded) { return; }
    fk_conf_loaded = 1;
    int fd = open("fkwu.conf", 0);
    if (fd < 0) { return; }
    /* read the WHOLE file: the old single bounded read parsed whatever one
     * read() happened to return and silently dropped the rest. */
    long long cb_cap = 8192;
    char *cb = malloc((unsigned long)cb_cap);
    long long n = 0;
    if (cb == 0) { close(fd); return; }
    for (;;) {
        long long got;
        if (n + 4096 >= cb_cap) {
            char *q;
            cb_cap = cb_cap * 2;
            q = realloc(cb, (unsigned long)cb_cap);
            if (q == 0) { free(cb); close(fd); return; }
            cb = q;
        }
        got = read(fd, cb + n, 4096);
        if (got <= 0) { break; }
        n = n + got;
    }
    close(fd);
    if (n <= 0) { free(cb); return; }
    cb[n] = 0;
    long long i = 0;
    while (i < n) {
        long long ks, ke, vs, ve;
        while (i < n && (cb[i] == ' ' || cb[i] == '\t' || cb[i] == '\n' || cb[i] == '\r')) { i = i + 1; }
        if (i >= n) { break; }
        if (cb[i] == '#') { while (i < n && cb[i] != '\n') { i = i + 1; } continue; }
        ks = i;
        while (i < n && cb[i] != ' ' && cb[i] != '\t' && cb[i] != '=' && cb[i] != '\n' && cb[i] != '\r') { i = i + 1; }
        ke = i;
        while (i < n && (cb[i] == ' ' || cb[i] == '\t' || cb[i] == '=')) { i = i + 1; }
        vs = i;
        while (i < n && cb[i] != '\n' && cb[i] != '\r') { i = i + 1; }
        ve = i;
        while (ve > vs && (cb[ve - 1] == ' ' || cb[ve - 1] == '\t')) { ve = ve - 1; }
        if (ke > ks) {
            fk_conf_reserve(fk_conf_n + 1);
            fk_conf_k[fk_conf_n] = fk_conf_pool_put(cb + ks, ke - ks);
            fk_conf_v[fk_conf_n] = ve > vs ? fk_conf_pool_put(cb + vs, ve - vs) : fk_conf_pool_put("1", 1);
            fk_conf_n = fk_conf_n + 1;
        }
    }
    free(cb);
}
/* fk_conf: config-file replacement for getenv on OUR toggles. Returns the value string, or 0 when
 * the key is absent OR set to "0"/"" -- so `if (fk_conf("X"))` is on iff X is present and non-zero,
 * matching the configured-toggle presence semantics. */
static char *fk_conf(const char *key) {
    fk_conf_load();
    int i = 0;
    while (i < fk_conf_n) {
        const char *k = fk_conf_pool + fk_conf_k[i];
        int j = 0;
        while (key[j] != 0 && k[j] != 0 && key[j] == k[j]) { j = j + 1; }
        if (key[j] == 0 && k[j] == 0) {
            char *v = fk_conf_pool + fk_conf_v[i];
            if (v[0] == 0 || (v[0] == '0' && v[1] == 0)) { return 0; }
            return v;
        }
        i = i + 1;
    }
    return 0;
}
/* The record table grows in BOTH dimensions (2026-09-02, the stackbreath
 * family + this reunion): rows are per-record heap arrays behind stable
 * indices — nothing a handle points through relocates — the row count doubles
 * via fk_record_reserve and each record's key/value row doubles via
 * fk_record_keys_reserve. The old walls: 256 records died loud, 128 keys died
 * loud on construction and record_set silently DROPPED the 129th key — a
 * partial record accepted as whole. */
static long long **fk_rkey;
static long long **fk_rval;
static long long *fk_rkcap;
static long long *fk_rcnt;
static long long *fk_rbp;
static long long fk_record_cap;  /* live row capacity; fk_record_reserve owns it */
static long long fk_rp;
static void fk_record_reserve(long long need) {
    long long nc;
    long long i;
    if (fk_record_cap == 0) {
        fk_rkey = (long long **)calloc(FK_RECORD_CAP_INIT, sizeof(long long *));
        fk_rval = (long long **)calloc(FK_RECORD_CAP_INIT, sizeof(long long *));
        fk_rkcap = (long long *)calloc(FK_RECORD_CAP_INIT, 8);
        fk_rcnt = (long long *)calloc(FK_RECORD_CAP_INIT, 8);
        fk_rbp = (long long *)calloc(FK_RECORD_CAP_INIT, 8);
        if (fk_rkey == 0 || fk_rval == 0 || fk_rkcap == 0 || fk_rcnt == 0 || fk_rbp == 0) {
            fk_die("fk_record_reserve: out of memory for the record table");
        }
        fk_record_cap = FK_RECORD_CAP_INIT;
    }
    if (need <= fk_record_cap) {
        return;
    }
    nc = fk_record_cap;
    while (nc < need) {
        nc = nc * 2;
    }
    fk_rkey = (long long **)realloc(fk_rkey, (unsigned long)(nc * sizeof(long long *)));
    fk_rval = (long long **)realloc(fk_rval, (unsigned long)(nc * sizeof(long long *)));
    fk_rkcap = (long long *)realloc(fk_rkcap, (unsigned long)(nc * 8));
    fk_rcnt = (long long *)realloc(fk_rcnt, (unsigned long)(nc * 8));
    fk_rbp = (long long *)realloc(fk_rbp, (unsigned long)(nc * 8));
    if (fk_rkey == 0 || fk_rval == 0 || fk_rkcap == 0 || fk_rcnt == 0 || fk_rbp == 0) {
        fk_die("fk_record_reserve: out of memory growing the record table");
    }
    i = fk_record_cap;
    while (i < nc) {
        fk_rkey[i] = 0;
        fk_rval[i] = 0;
        fk_rkcap[i] = 0;
        fk_rcnt[i] = 0;
        fk_rbp[i] = 0;
        i = i + 1;
    }
    fk_record_cap = nc;
}
/* grow record r's KEY/VALUE row to hold at least `need` entries. Row arrays
 * are reached only through fk_rkey[r]/fk_rval[r], so a realloc that moves a
 * row breaks nothing — no handle points into it. */
static void fk_record_keys_reserve(long long r, long long need) {
    long long nc;
    if (need <= fk_rkcap[r]) {
        return;
    }
    nc = fk_rkcap[r] == 0 ? FK_RECORD_KEYS_INIT : fk_rkcap[r];
    while (nc < need) {
        nc = nc * 2;
    }
    fk_rkey[r] = (long long *)realloc(fk_rkey[r], (unsigned long)(nc * 8));
    fk_rval[r] = (long long *)realloc(fk_rval[r], (unsigned long)(nc * 8));
    if (fk_rkey[r] == 0 || fk_rval[r] == 0) {
        fk_die("fk_record_keys_reserve: out of memory growing a record's key row");
    }
    fk_rkcap[r] = nc;
}
static long long fk_rbox(long long r) {
    return 0 - (r << 1);
}
static long long fk_ridx(long long v) {
    if (v >= 0 || ((0 - v) & 1) != 0) {
        return 0;
    }
    return (0 - v) >> 1;
}
static long long fk_isrec(long long v) {
    long long r = fk_ridx(v);
    return r >= 1 && r <= fk_rp;
}
/* ── methods on the blueprint (BML/NUMS rung 2b) ── shared by every record of a
 * type, name-dispatched; the keystone that turns a Record into a real object.
 * Key = (blueprint identity, method name). Blueprint identity follows the
 * tag-102 law: kind-3 nodeids are identity-by-content (equal coordinates ARE
 * the same identity regardless of which mint built the value node), everything
 * else compares by handle. */
#define FK_METHOD_INIT_CAP 256          /* fk_mth_*: method table INITIAL capacity — grows by doubling, no wall */
static long long *fk_mth_bp;
static long long *fk_mth_name;
static long long *fk_mth_fn;
static long long fk_mth_cap;
static long long fk_mth_n;
static void fk_mth_ensure(void) {
    if (fk_mth_n + 1 < fk_mth_cap) {
        return;
    }
    long long ncap = fk_mth_cap == 0 ? FK_METHOD_INIT_CAP : fk_mth_cap * 2;
    fk_mth_bp = realloc(fk_mth_bp, ncap * 8);
    fk_mth_name = realloc(fk_mth_name, ncap * 8);
    fk_mth_fn = realloc(fk_mth_fn, ncap * 8);
    if (fk_mth_bp == 0 || fk_mth_name == 0 || fk_mth_fn == 0) {
        fk_die("fk_mth_ensure: method table realloc failed");
    }
    fk_mth_cap = ncap;
}
static long long fk_bp_ideq(long long a, long long b) {
    if (a < 0 && b < 0) {
        long long ia = fk_nidx(a);
        long long ib = fk_nidx(b);
        if (ia >= 1 && ia <= fk_np && ib >= 1 && ib <= fk_np &&
            fk_nkind[ia] == 3 && fk_nkind[ib] == 3) {
            return (fk_nid[ia][0] == fk_nid[ib][0] && fk_nid[ia][1] == fk_nid[ib][1] &&
                    fk_nid[ia][2] == fk_nid[ib][2] && fk_nid[ia][3] == fk_nid[ib][3]) ? 1 : 0;
        }
    }
    return a == b ? 1 : 0;
}
static long long fk_mth_find(long long bp, long long name) {
    long long m = 0;
    while (m < fk_mth_n) {
        if (fk_mth_name[m] == name && fk_bp_ideq(fk_mth_bp[m], bp)) {
            return m;
        }
        m = m + 1;
    }
    return 0 - 1;
}
static long long fk_cstrlen(const char *s) {
    long long n = 0;
    while (s[n] != 0) {
        n = n + 1;
    }
    return n;
}
static void fk_cstr(long long sv, char *out, long long cap) {
    long long sa = fk_stri(sv);
    long long n = 0;
    if (sa >= 0 && FK_SOK(sa)) {
        n = FK_SLEN(sa);
        if (n > cap - 1) {
            fk_die("fk_cstr: string longer than the destination buffer -- silently truncating would corrupt the path / hostname / port / URL / device-name the caller is about to use (every fk_cstr caller is one of these). Raise the caller's buffer if this length is legitimate.");
        }
        long long j = 0;
        while (j < n) {
            out[j] = FK_SBYTES(sa)[j];
            j = j + 1;
        }
    }
    out[n] = 0;
}
/* fk_cstr_heap: a string value's bytes as a fresh C string of EXACTLY its
 * length (caller frees). fk_cstr's fixed buffers stay for what the host itself
 * bounds -- a path, a hostname, a port; a command line is not one of those, so
 * it is heap-built: reserve, die only on allocator refusal (the R21 idiom).
 * A non-string answers the empty C string. */
static char *fk_cstr_heap(long long sv) {
    long long sa = fk_stri(sv);
    long long n = (sa >= 0 && FK_SOK(sa)) ? FK_SLEN(sa) : 0;
    char *out = malloc((unsigned long)(n + 1));
    if (out == 0) {
        fk_die("fk_cstr_heap: out of memory");
    }
    long long j = 0;
    while (j < n) {
        out[j] = FK_SBYTES(sa)[j];
        j = j + 1;
    }
    out[n] = 0;
    return out;
}
/* Every fk_sb growth site (fk_sbuf below, plus ~14 more inline copies of the same
 * `while (... > fk_scap_b) { fk_scap_b *= 2; fk_sb = realloc(...); }` idiom
 * scattered through the parser/evaluator wherever a string gets built
 * incrementally) shares this one check, called right after each realloc, so an
 * allocator-exhaustion state can't silently leave fk_sb null and get written
 * through on the very next line. */
static void fk_sb_check(void) {
    if (fk_sb == 0) {
        fk_die("fk_sb: out of memory growing string pool");
    }
}
static long long fk_sbuf(const char *buf, long long n) {
    if (n < 0) {
        n = 0;
    }
    fk_sinit();
    while (fk_sbp + n > fk_scap_b) {
        fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b, fk_scap_b * 2, FK_STORE_STR_BYTES, 0);
        fk_scap_b = fk_scap_b * 2;
        fk_sb_check();
    }
    long long j = 0;
    while (j < n) {
        fk_sb[fk_sbp + j] = buf[j];
        j = j + 1;
    }
    return fk_strv(fk_sintern(fk_sbp, n));
}
#define FK_METAL_FIXTURE_UNLINKED (0 - 4611686018427387903LL)
#define FK_METAL_MATVEC_UNLINKED (0 - 4611686018427387902LL)
/* The handle door's unlinked sentinel. Distinct from the two above so a reader of
 * a failing band can tell "no carrier on this build" from "carrier said no". Every
 * handle-returning primitive answers 0 when unlinked and 0 is never a live handle;
 * metal_status is the voice canary that says WHICH of the two it was, because a
 * bare 0 is exactly the shape axiom-5 hands back for a name that was never bound. */
#define FK_METAL_HANDLE_UNLINKED (0 - 4611686018427387901LL)
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) long long fk_metal_matvec_fixture_external(char *out, long long cap) {
    (void)out;
    (void)cap;
    return FK_METAL_FIXTURE_UNLINKED;
}
__attribute__((weak)) long long fk_metal_matvec_f32_external(const char *msl, long long msl_len,
                                                             const char *kernel,
                                                             long long kernel_len,
                                                             const char *model, long long model_len,
                                                             char *out, long long cap) {
    (void)msl;
    (void)msl_len;
    (void)kernel;
    (void)kernel_len;
    (void)model;
    (void)model_len;
    (void)out;
    (void)cap;
    return FK_METAL_MATVEC_UNLINKED;
}
#else
static long long fk_metal_matvec_fixture_external(char *out, long long cap) {
    (void)out;
    (void)cap;
    return FK_METAL_FIXTURE_UNLINKED;
}
static long long fk_metal_matvec_f32_external(const char *msl, long long msl_len,
                                              const char *kernel, long long kernel_len,
                                              const char *model, long long model_len, char *out,
                                              long long cap) {
    (void)msl;
    (void)msl_len;
    (void)kernel;
    (void)kernel_len;
    (void)model;
    (void)model_len;
    (void)out;
    (void)cap;
    return FK_METAL_MATVEC_UNLINKED;
}
#endif
/* ── the handle door's eight weak symbols ──
 * Same weak-stub pattern as the two above, and for the same reason: a build with
 * no Metal carrier linked must still COMPILE and still ANSWER, honestly, that the
 * door is shut. It must not fail to link, and it must not answer a plausible
 * number. Each returns FK_METAL_HANDLE_UNLINKED, which the native wrappers turn
 * into 0 for handles and into a spoken metal_linked=false for metal_status. */
#if defined(__GNUC__) || defined(__clang__)
#define FK_METAL_WEAK __attribute__((weak))
FK_METAL_WEAK long long fk_host_gpu_utilization(void) { return -1; } /* strong symbol lives in the Metal carrier; a build without it answers absent */
FK_METAL_WEAK long long fk_host_disk_stat(long long *out) { out[0] = 0; out[1] = 0; out[2] = 0; out[3] = 0; return -1; }
FK_METAL_WEAK long long fk_metal_live_external(long long *out) { int k = 0; while (k < 18) { out[k] = 0; k = k + 1; } return -1; }
#else
#define FK_METAL_WEAK static
#endif
/* No error-text parameter here on purpose: an MSL compile diagnostic is far larger
 * than a return value and must not be summarised into one. The carrier keeps the
 * compiler's own words and metal_status speaks them, so the band that got a 0
 * handle has exactly one place to look and finds the real message there. */
FK_METAL_WEAK long long fk_metal_pipeline_external(const char *msl, long long msl_len,
                                                   const char *name, long long name_len) {
    (void)msl; (void)msl_len; (void)name; (void)name_len;
    return FK_METAL_HANDLE_UNLINKED;
}
FK_METAL_WEAK long long fk_metal_buf_alloc_external(long long nbytes) {
    (void)nbytes;
    return FK_METAL_HANDLE_UNLINKED;
}
FK_METAL_WEAK long long fk_metal_buf_from_file_external(const char *path, long long path_len,
                                                        long long off, long long len) {
    (void)path; (void)path_len; (void)off; (void)len;
    return FK_METAL_HANDLE_UNLINKED;
}
FK_METAL_WEAK long long fk_metal_buf_write_external(long long h, long long off,
                                                    const char *bytes, long long len) {
    (void)h; (void)off; (void)bytes; (void)len;
    return FK_METAL_HANDLE_UNLINKED;
}
FK_METAL_WEAK long long fk_metal_enqueue_external(long long pipe, const char *binding,
                                                  long long binding_len, long long threads) {
    (void)pipe; (void)binding; (void)binding_len; (void)threads;
    return FK_METAL_HANDLE_UNLINKED;
}
FK_METAL_WEAK long long fk_metal_sync_external(void) { return FK_METAL_HANDLE_UNLINKED; }
FK_METAL_WEAK long long fk_metal_buf_read_external(long long h, long long off, long long len,
                                                   char *out, long long cap) {
    (void)h; (void)off; (void)len; (void)out; (void)cap;
    return FK_METAL_HANDLE_UNLINKED;
}
FK_METAL_WEAK long long fk_metal_status_external(char *out, long long cap) {
    (void)out; (void)cap;
    return FK_METAL_HANDLE_UNLINKED;
}
FK_METAL_WEAK long long fk_metal_batch_concurrent_external(void) { return FK_METAL_HANDLE_UNLINKED; }
FK_METAL_WEAK long long fk_metal_buf_free_external(long long h) {
    (void)h;
    return FK_METAL_HANDLE_UNLINKED;
}
FK_METAL_WEAK long long fk_metal_submit_external(void) { return FK_METAL_HANDLE_UNLINKED; }
FK_METAL_WEAK long long fk_metal_fence_wait_external(long long fence) {
    (void)fence;
    return FK_METAL_HANDLE_UNLINKED;
}
FK_METAL_WEAK long long fk_metal_deadline_external(long long ms) {
    (void)ms;
    return FK_METAL_HANDLE_UNLINKED;
}
/* MLX organ. Same weak-stub shape as Metal: a build without the mlx carrier
 * still compiles and still speaks mlx_linked=false. Tags 143/144 are free in
 * the evaluator (holes after metal_fence_wait's 142). Checkout-witness; the
 * Form walker is the shrink target. */
#define FK_MLX_UNLINKED FK_METAL_HANDLE_UNLINKED
FK_METAL_WEAK long long fk_mlx_live_external(long long *out) { int k = 0; while (k < 12) { out[k] = 0; k = k + 1; } return -1; } /* strong symbol lives in the MLX carrier */
FK_METAL_WEAK long long fk_mlx_status_external(char *out, long long cap) {
    (void)out;
    (void)cap;
    return FK_MLX_UNLINKED;
}
FK_METAL_WEAK long long fk_mlx_add_external(long long a, long long b) {
    (void)a;
    (void)b;
    return FK_MLX_UNLINKED;
}
FK_METAL_WEAK long long fk_mlx_run_external(const char *src, long long n) {
    (void)src;
    (void)n;
    return FK_MLX_UNLINKED;
}
static long long fk_srange(long long sv, const char **ptr, long long *len) {
    long long sa = fk_stri(sv);
    if (sa < 0 || !FK_SOK(sa)) {
        *ptr = "";
        *len = 0;
        return 0;
    }
    *ptr = FK_SBYTES(sa);
    *len = FK_SLEN(sa);
    return 1;
}
#define FK_METAL_FIXTURE_BUF_CAP 4096
static long long fk_metal_matvec_fixture_native(void) {
    static char out[FK_METAL_FIXTURE_BUF_CAP];
    long long n = fk_metal_matvec_fixture_external(out, FK_METAL_FIXTURE_BUF_CAP);
    if (n == FK_METAL_FIXTURE_UNLINKED) {
        const char *m =
            "SKIP fkwu-form-cli-metal-direct: no linked Metal carrier\nmetal_owner=fkwu-form-cli\nmetal_linked=false\n";
        return fk_sbuf(m, fk_cstrlen(m));
    }
    if (n < 0) {
        const char *m = "FAIL fkwu-form-cli-metal-direct external carrier returned error\n";
        return fk_sbuf(m, fk_cstrlen(m));
    }
    if (n > FK_METAL_FIXTURE_BUF_CAP) {
        n = FK_METAL_FIXTURE_BUF_CAP;
    }
    return fk_sbuf(out, n);
}
#define FK_METAL_MATVEC_BUF_CAP 8192
static long long fk_metal_matvec_f32_native(long long mslv, long long kernelv, long long modelv) {
    const char *msl;
    const char *kernel;
    const char *model;
    long long msl_len;
    long long kernel_len;
    long long model_len;
    if (fk_srange(mslv, &msl, &msl_len) == 0 || fk_srange(kernelv, &kernel, &kernel_len) == 0 ||
        fk_srange(modelv, &model, &model_len) == 0) {
        const char *m = "FAIL fkwu-form-cli-metal-matvec-f32 invalid string input\n";
        return fk_sbuf(m, fk_cstrlen(m));
    }
    static char out[FK_METAL_MATVEC_BUF_CAP];
    long long n = fk_metal_matvec_f32_external(msl, msl_len, kernel, kernel_len, model, model_len,
                                               out, FK_METAL_MATVEC_BUF_CAP);
    if (n == FK_METAL_MATVEC_UNLINKED) {
        const char *m =
            "SKIP fkwu-form-cli-metal-matvec-f32: no linked Metal carrier\nmetal_owner=fkwu-form-cli\nmetal_linked=false\n";
        return fk_sbuf(m, fk_cstrlen(m));
    }
    if (n < 0) {
        const char *m = "FAIL fkwu-form-cli-metal-matvec-f32 external carrier returned error\n";
        return fk_sbuf(m, fk_cstrlen(m));
    }
    if (n > FK_METAL_MATVEC_BUF_CAP) {
        n = FK_METAL_MATVEC_BUF_CAP;
    }
    return fk_sbuf(out, n);
}
/* ── handle-door natives ──
 * Every one of these is a translation and nothing more: unwrap Form's tagged
 * values, hand the carrier plain bytes and plain integers, wrap the answer back.
 * There is no arithmetic here and no policy; the layout of the binding string is
 * decided by the Form cell that emits it, exactly as the MSL is. */
static long long fk_metal_pipeline_native(long long mslv, long long namev) {
    const char *msl;
    const char *name;
    long long ml;
    long long nl;
    if (fk_srange(mslv, &msl, &ml) == 0 || fk_srange(namev, &name, &nl) == 0) {
        return 0;
    }
    long long h = fk_metal_pipeline_external(msl, ml, name, nl);
    if (h == FK_METAL_HANDLE_UNLINKED || h < 0) {
        return 0;
    }
    return h;
}
static long long fk_metal_buf_alloc_native(long long nbytes) {
    long long h = fk_metal_buf_alloc_external(nbytes);
    if (h == FK_METAL_HANDLE_UNLINKED || h < 0) {
        return 0;
    }
    return h;
}
static long long fk_metal_buf_from_file_native(long long pathv, long long off, long long len) {
    const char *path;
    long long pl;
    if (fk_srange(pathv, &path, &pl) == 0) {
        return 0;
    }
    long long h = fk_metal_buf_from_file_external(path, pl, off, len);
    if (h == FK_METAL_HANDLE_UNLINKED || h < 0) {
        return 0;
    }
    return h;
}
static long long fk_metal_buf_write_native(long long h, long long off, long long bytesv) {
    const char *b;
    long long bl;
    if (fk_srange(bytesv, &b, &bl) == 0) {
        return 0;
    }
    long long n = fk_metal_buf_write_external(h, off, b, bl);
    if (n == FK_METAL_HANDLE_UNLINKED || n < 0) {
        return 0;
    }
    return n;
}
static long long fk_metal_enqueue_native(long long pipe, long long bindv, long long threads) {
    const char *b;
    long long bl;
    if (fk_srange(bindv, &b, &bl) == 0) {
        return 0;
    }
    long long r = fk_metal_enqueue_external(pipe, b, bl, threads);
    if (r == FK_METAL_HANDLE_UNLINKED || r < 0) {
        return 0;
    }
    return r;
}
static long long fk_metal_sync_native(void) {
    long long r = fk_metal_sync_external();
    if (r == FK_METAL_HANDLE_UNLINKED || r < 0) {
        return 0;
    }
    return r;
}
/* Read-back is the one primitive whose size the CALLER chooses, so it is the one
 * that must not quietly hand back less than it was asked for. A short read here
 * would look to a Form cell exactly like a correct read of a shorter tensor. */
static long long fk_metal_buf_read_native(long long h, long long off, long long len) {
    if (len <= 0) {
        return fk_sbuf("", 0);
    }
    fk_sinit();
    while (fk_sbp + len > fk_scap_b) {
        fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b, fk_scap_b * 2, FK_STORE_STR_BYTES, 0);
        fk_scap_b = fk_scap_b * 2;
        fk_sb_check();
    }
    long long n = fk_metal_buf_read_external(h, off, len, fk_sb + fk_sbp, len);
    if (n == FK_METAL_HANDLE_UNLINKED || n < 0) {
        return fk_sbuf("", 0);
    }
    /* The carrier answers all of `len` or it answers an error. A partial read
     * returned as a short string is indistinguishable, to the Form cell holding
     * it, from a correct read of a smaller tensor — that is the silent-truncation
     * shape this body has already been bitten by, so it is refused here rather
     * than passed on. */
    if (n != len) {
        return fk_sbuf("", 0);
    }
    return fk_strv(fk_sintern(fk_sbp, n));
}
static long long fk_metal_batch_concurrent_native(void) {
    long long r = fk_metal_batch_concurrent_external();
    if (r == FK_METAL_HANDLE_UNLINKED || r < 0) {
        return 0;
    }
    return r;
}
static long long fk_metal_buf_free_native(long long h) {
    long long r = fk_metal_buf_free_external(h);
    if (r == FK_METAL_HANDLE_UNLINKED || r < 0) {
        return 0;
    }
    return r;
}
static long long fk_metal_submit_native(void) {
    long long r = fk_metal_submit_external();
    if (r == FK_METAL_HANDLE_UNLINKED || r < 0) {
        return 0;
    }
    return r;
}
static long long fk_metal_fence_wait_native(long long fence) {
    long long r = fk_metal_fence_wait_external(fence);
    if (r == FK_METAL_HANDLE_UNLINKED || r < 0) {
        return 0;
    }
    return r;
}
/* The caller's patience for every wait in this process, handed in as data (ms;
 * -1 = the kernel's own wait). Answers the deadline that stood before so the
 * hand-over reads back as data; -2 when no carrier is linked to hand it to. */
static long long fk_metal_deadline_native(long long ms) {
    long long r = fk_metal_deadline_external(ms);
    if (r == FK_METAL_HANDLE_UNLINKED) {
        return -2;
    }
    return r;
}
#define FK_METAL_STATUS_BUF_CAP 4096
static long long fk_metal_status_native(void) {
    static char out[FK_METAL_STATUS_BUF_CAP];
    long long n = fk_metal_status_external(out, FK_METAL_STATUS_BUF_CAP);
    if (n == FK_METAL_HANDLE_UNLINKED) {
        const char *m = "metal_owner=fkwu-form-cli\nmetal_linked=false\nmetal_door=handle\n";
        return fk_sbuf(m, fk_cstrlen(m));
    }
    if (n < 0) {
        const char *m = "metal_owner=fkwu-form-cli\nmetal_linked=false\nmetal_door=handle\nlast_error=carrier returned error\n";
        return fk_sbuf(m, fk_cstrlen(m));
    }
    if (n > FK_METAL_STATUS_BUF_CAP) {
        n = FK_METAL_STATUS_BUF_CAP;
    }
    return fk_sbuf(out, n);
}
#define FK_MLX_STATUS_BUF_CAP 4096
static long long fk_mlx_status_native(void) {
    static char out[FK_MLX_STATUS_BUF_CAP];
    long long n = fk_mlx_status_external(out, FK_MLX_STATUS_BUF_CAP);
    if (n == FK_MLX_UNLINKED) {
        const char *m = "mlx_owner=fkwu-form-cli\nmlx_linked=false\nmlx_metal_available=false\nmlx_gpu_available=false\nlast_error=unlinked\n";
        return fk_sbuf(m, fk_cstrlen(m));
    }
    if (n < 0) {
        const char *m = "mlx_owner=fkwu-form-cli\nmlx_linked=false\nlast_error=carrier returned error\n";
        return fk_sbuf(m, fk_cstrlen(m));
    }
    if (n > FK_MLX_STATUS_BUF_CAP) {
        n = FK_MLX_STATUS_BUF_CAP;
    }
    return fk_sbuf(out, n);
}
static long long fk_mlx_add_native(long long a, long long b) {
    long long r = fk_mlx_add_external(a, b);
    if (r == FK_MLX_UNLINKED) {
        return 0;
    }
    return r;
}
static long long fk_mlx_run_native(long long srcv) {
    const char *p;
    long long n;
    if (!fk_srange(srcv, &p, &n)) {
        return 0;
    }
    long long r = fk_mlx_run_external(p, n);
    if (r == FK_MLX_UNLINKED) {
        return 0;
    }
    return r;
}
/* ── host sense-channel carriers: camera (world-video) + mic (world-audio) ── The two conditions of
 * host-kernel.form, made concrete: ALLOW-PRESENCE (detect the device through the host's own OS API)
 * and MEASURE-HEALTH (open it, observe whether it is acquirable). The port is invariant
 * (resource-port.fk: mic = afferent-bytes, camera = afferent-pixel); the CARRIER is swappable.
 * Windows carrier: winmm (waveIn) for mic, avicap32 for camera — plain C, no COM.
 * WASAPI/Media-Foundation are future challengers; mac CoreAudio/AVFoundation and android
 * AAudio/Camera2 carriers are named pending (the else branch is honest). */
#if defined(_WIN32)
struct fk_waveincaps {
    unsigned short wMid;
    unsigned short wPid;
    unsigned int vDriverVersion;
    char szPname[32];
    unsigned int dwFormats;
    unsigned short wChannels;
    unsigned short wReserved1;
};
struct fk_waveformatex {
    unsigned short wFormatTag;
    unsigned short nChannels;
    unsigned int nSamplesPerSec;
    unsigned int nAvgBytesPerSec;
    unsigned short nBlockAlign;
    unsigned short wBitsPerSample;
    unsigned short cbSize;
};
extern unsigned int waveInGetNumDevs(void);
extern unsigned int waveInGetDevCapsA(unsigned long long, struct fk_waveincaps *, unsigned int);
extern unsigned int waveInOpen(void **, unsigned int, const struct fk_waveformatex *,
                               unsigned long long, unsigned long long, unsigned long long);
extern unsigned int waveInClose(void *);
extern int capGetDriverDescriptionA(unsigned int, char *, int, char *, int);
extern void *capCreateCaptureWindowA(const char *, unsigned int, int, int, int, int, void *, int);
extern long long SendMessageA(void *, unsigned int, unsigned long long, long long);
extern int DestroyWindow(void *);
extern void Sleep(unsigned int);
static long long fk_mic_count(void) {
    return (long long)waveInGetNumDevs();
}
static long long fk_mic_name(long long i) {
    struct fk_waveincaps c;
    if (i < 0 || waveInGetDevCapsA((unsigned long long)i, &c, (unsigned int)sizeof c) != 0) {
        return fk_sbuf("", 0);
    }
    return fk_sbuf(c.szPname, fk_cstrlen(c.szPname));
}
static long long fk_mic_health(long long i) {
    if (i < 0 || i >= fk_mic_count()) {
        return 0;
    }
    struct fk_waveformatex f;
    f.wFormatTag = 1;
    f.nChannels = 1;
    f.nSamplesPerSec = 44100;
    f.nAvgBytesPerSec = 88200;
    f.nBlockAlign = 2;
    f.wBitsPerSample = 16;
    f.cbSize = 0;
    void *h = 0;
    if (waveInOpen(&h, (unsigned int)i, &f, 0, 0, 0) != 0) {
        return 0;
    }
    waveInClose(h);
    return 1;
}
static long long fk_cam_count(void) {
    char nm[256];
    char ver[256];
    long long n = 0;
    while (n < 64 && capGetDriverDescriptionA((unsigned int)n, nm, 256, ver, 256)) {
        n = n + 1;
    }
    return n;
}
static long long fk_cam_name(long long i) {
    char nm[256];
    char ver[256];
    if (i < 0 || !capGetDriverDescriptionA((unsigned int)i, nm, 256, ver, 256)) {
        return fk_sbuf("", 0);
    }
    return fk_sbuf(nm, fk_cstrlen(nm));
}
/* the VfW driver connect can block forever behind modern camera stacks (witnessed on this
 * cell: the "Microsoft WDM Image Capture" shim hangs on a MIPI camera — receipts/
 * 2026-07-01-windows-camera-carrier-probe.md). Probe on a worker thread and refuse
 * honestly after 3s; on timeout the probe struct and stuck thread are deliberately
 * abandoned (the named cost of a hung driver — never freed under its feet). */
extern void *CreateThread(void *, unsigned long long, unsigned int (*)(void *), void *,
                          unsigned int, unsigned int *);
extern unsigned int WaitForSingleObject(void *, unsigned int);
extern int CloseHandle(void *);
struct fk_cam_probe {
    long long idx;
    long long ok;
};
static unsigned int fk_cam_probe_run(void *arg) {
    struct fk_cam_probe *p = (struct fk_cam_probe *)arg;
    void *hwnd = capCreateCaptureWindowA("fkwu-cam", 0x80000000u, 0, 0, 0, 0, (void *)0, 0);
    if (hwnd != 0) {
        long long ok = SendMessageA(hwnd, 0x0400 + 10, (unsigned long long)p->idx, 0);
        if (ok) {
            SendMessageA(hwnd, 0x0400 + 11, 0, 0);
        }
        DestroyWindow(hwnd);
        p->ok = ok ? 1 : 0;
    }
    return 0;
}
static long long fk_cam_health(long long i) {
    char nm[256];
    char ver[256];
    if (i < 0 || !capGetDriverDescriptionA((unsigned int)i, nm, 256, ver, 256)) {
        return 0;
    }
    struct fk_cam_probe *p = malloc(sizeof(struct fk_cam_probe));
    if (p == 0) {
        return 0;
    }
    p->idx = i;
    p->ok = 0;
    void *th = CreateThread((void *)0, 0, fk_cam_probe_run, p, 0, (unsigned int *)0);
    if (th == 0) {
        free(p);
        return 0;
    }
    if (WaitForSingleObject(th, 3000) == 0) {
        long long ok = p->ok;
        CloseHandle(th);
        free(p);
        return ok;
    }
    CloseHandle(th);
    printf("sense: camera %lld connect timed out (legacy VfW shim) — health 0, honestly\n", i);
    return 0;
}
static long long fk_cam_grab(long long i, const char *path) {
    char nm[256];
    char ver[256];
    if (i < 0 || !capGetDriverDescriptionA((unsigned int)i, nm, 256, ver, 256)) {
        return 0;
    }
    void *hwnd = capCreateCaptureWindowA("fkwu-grab", 0x80000000u, 0, 0, 0, 0, (void *)0, 0);
    if (hwnd == 0) {
        return 0;
    }
    if (!SendMessageA(hwnd, 0x0400 + 10, (unsigned long long)i, 0)) {
        DestroyWindow(hwnd);
        return 0;
    }
    Sleep(1500);
    long long k = 0;
    while (k < 12) {
        SendMessageA(hwnd, 0x0400 + 61, 0, 0);
        Sleep(90);
        k = k + 1;
    }
    long long saved = SendMessageA(hwnd, 0x0400 + 25, 0, (long long)(unsigned long long)path);
    SendMessageA(hwnd, 0x0400 + 11, 0, 0);
    DestroyWindow(hwnd);
    return saved ? 1 : 0;
}
/* ── mic CAPTURE (winmm waveIn, completing the carrier named above): ms of PCM16 mono 16kHz,
 * measured and released — the Android receipt pattern: samples / nonzero / mean-abs / peak
 * cross into Form as integers; no raw audio is retained. */
struct fk_wavehdr {
    char *lpData;
    unsigned int dwBufferLength;
    unsigned int dwBytesRecorded;
    unsigned long long dwUser;
    unsigned int dwFlags;
    unsigned int dwLoops;
    struct fk_wavehdr *lpNext;
    unsigned long long reserved;
};
extern unsigned int waveInPrepareHeader(void *, struct fk_wavehdr *, unsigned int);
extern unsigned int waveInUnprepareHeader(void *, struct fk_wavehdr *, unsigned int);
extern unsigned int waveInAddBuffer(void *, struct fk_wavehdr *, unsigned int);
extern unsigned int waveInStart(void *);
extern unsigned int waveInReset(void *);
static long long fk_cons_val(long long h, long long t);
static long long fk_mic_capture(long long ms) {
    if (ms < 100) {
        ms = 100;
    }
    if (ms > 10000) {
        ms = 10000;
    }
    struct fk_waveformatex fmt;
    fmt.wFormatTag = 1;
    fmt.nChannels = 1;
    fmt.nSamplesPerSec = 16000;
    fmt.wBitsPerSample = 16;
    fmt.nBlockAlign = 2;
    fmt.nAvgBytesPerSec = 32000;
    fmt.cbSize = 0;
    void *h = 0;
    if (waveInOpen(&h, 0xFFFFFFFFu, &fmt, 0, 0, 0) != 0) {
        printf("sense: mic open refused\n");
        return 1;
    }
    long long bytes = ms * 32;
    char *buf = malloc((unsigned long)bytes);
    if (buf == 0) {
        waveInClose(h);
        return 1;
    }
    struct fk_wavehdr hd;
    hd.lpData = buf;
    hd.dwBufferLength = (unsigned int)bytes;
    hd.dwBytesRecorded = 0;
    hd.dwUser = 0;
    hd.dwFlags = 0;
    hd.dwLoops = 0;
    hd.lpNext = 0;
    hd.reserved = 0;
    waveInPrepareHeader(h, &hd, (unsigned int)sizeof hd);
    waveInAddBuffer(h, &hd, (unsigned int)sizeof hd);
    waveInStart(h);
    long long waited = 0;
    while ((hd.dwFlags & 1) == 0 && waited < ms + 2000) {
        Sleep(50);
        waited = waited + 50;
    }
    waveInReset(h);
    waveInUnprepareHeader(h, &hd, (unsigned int)sizeof hd);
    waveInClose(h);
    long long nsamp = hd.dwBytesRecorded / 2;
    long long nonzero = 0;
    long long peak = 0;
    long long sumabs = 0;
    long long i;
    for (i = 0; i < nsamp; i = i + 1) {
        long long s = (long long)*(short *)(buf + i * 2);
        long long a = s < 0 ? 0 - s : s;
        if (a > 0) {
            nonzero = nonzero + 1;
        }
        if (a > peak) {
            peak = a;
        }
        sumabs = sumabs + a;
    }
    free(buf);
    long long meanabs = nsamp > 0 ? sumabs / nsamp : 0;
    printf("sense: mic captured %lld samples (%lld ms) nonzero=%lld mean-abs=%lld peak=%lld — "
           "measured, not retained\n",
           nsamp, ms, nonzero, meanabs, peak);
    long long r = 1;
    r = fk_cons_val(peak << 1, r);
    r = fk_cons_val(meanabs << 1, r);
    r = fk_cons_val(nonzero << 1, r);
    r = fk_cons_val(nsamp << 1, r);
    return r;
}
/* ── camera CAPTURE (Media Foundation — the carrier the hanging VfW shim demanded, now built
 * as its own deliberate movement): LoadLibrary-only (ole32/mfplat/mf/mfreadwrite), COM vtables
 * called by slot in plain C — no new link libraries, the same door discipline as nvcuda.
 * One frame is asked for as NV12 (Y plane first), its LUMA measured (w/h/mean/nonzero) and the
 * frame released: the eye opens, measures, retains nothing. Bounded worker thread; a Windows
 * camera-privacy denial is an honest refusal, printed. */
struct fk_guid {
    unsigned int a;
    unsigned short b;
    unsigned short c;
    unsigned char d[8];
};
static const struct fk_guid fk_g_devsrc_type = {
    0xc60ac5fe, 0x252a, 0x478f, {0xa0, 0xef, 0xbc, 0x8f, 0xa5, 0xf7, 0xca, 0xd3}};
static const struct fk_guid fk_g_devsrc_vidcap = {
    0x8ac3587a, 0x4ae7, 0x42d8, {0x99, 0xe0, 0x0a, 0x60, 0x13, 0xee, 0xf9, 0x0f}};
static const struct fk_guid fk_g_iid_mediasource = {
    0x279a808d, 0xaec7, 0x40c8, {0x9c, 0x6b, 0xa6, 0xb4, 0x92, 0xc7, 0x8a, 0x66}};
static const struct fk_guid fk_g_mt_major = {
    0x48eba18e, 0xf8c9, 0x4687, {0xbf, 0x11, 0x0a, 0x74, 0xc9, 0xf9, 0x6a, 0x8f}};
static const struct fk_guid fk_g_mt_subtype = {
    0xf7e34c9a, 0x42e8, 0x4714, {0xb7, 0x4b, 0xcb, 0x29, 0xd7, 0x2c, 0x35, 0xe5}};
static const struct fk_guid fk_g_mt_framesize = {
    0x1652c33d, 0xd6b2, 0x4012, {0xb8, 0x34, 0x72, 0x03, 0x08, 0x49, 0xa3, 0x7d}};
static const struct fk_guid fk_g_video_major = {
    0x73646976, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
static const struct fk_guid fk_g_fmt_nv12 = {
    0x3231564e, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
static const struct fk_guid fk_g_reader_processing = {
    0xfb394f3d, 0xccf1, 0x42ee, {0xbb, 0xb3, 0xf9, 0xb8, 0x45, 0xd5, 0x68, 0x1d}};
typedef int (*fk_vt_guid2)(void *, const struct fk_guid *, const struct fk_guid *);
typedef int (*fk_vt_u32set)(void *, const struct fk_guid *, unsigned int);
typedef int (*fk_vt_act)(void *, const struct fk_guid *, void **);
typedef int (*fk_vt_pp)(void *, void **);
typedef int (*fk_vt_mtset)(void *, unsigned int, void *, void *);
typedef int (*fk_vt_mtget)(void *, unsigned int, void **);
typedef int (*fk_vt_u64get)(void *, const struct fk_guid *, unsigned long long *);
typedef int (*fk_vt_readsample)(void *, unsigned int, unsigned int, unsigned int *,
                                unsigned int *, long long *, void **);
typedef int (*fk_vt_lockbuf)(void *, unsigned char **, unsigned int *, unsigned int *);
typedef int (*fk_vt_none)(void *);
static void **fk_vt(void *o) {
    return ((void ***)o)[0];
}
static void fk_com_release(void *o) {
    if (o != 0) {
        ((fk_vt_none)fk_vt(o)[2])(o);
    }
}
struct fk_camluma {
    long long w;
    long long h;
    long long luma;
    long long nonzero;
    long long rc;
    long long hr;
};
static unsigned int fk_cam_luma_run(void *arg) {
    struct fk_camluma *out = (struct fk_camluma *)arg;
    void *ole = LoadLibraryA("ole32.dll");
    void *mfp = LoadLibraryA("mfplat.dll");
    void *mfl = LoadLibraryA("mf.dll");
    void *mfr = LoadLibraryA("mfreadwrite.dll");
    if (ole == 0 || mfp == 0 || mfl == 0 || mfr == 0) {
        out->rc = -2;
        return 0;
    }
    typedef int (*FCoInit)(void *, unsigned int);
    typedef void (*FCoUninit)(void);
    typedef void (*FCoFree)(void *);
    typedef int (*FMfStart)(unsigned int, unsigned int);
    typedef int (*FMfStop)(void);
    typedef int (*FMfAttrs)(void **, unsigned int);
    typedef int (*FMfEnum)(void *, void ***, unsigned int *);
    typedef int (*FMfReader)(void *, void *, void **);
    typedef int (*FMfMkType)(void **);
    FCoInit fCoInit = (FCoInit)GetProcAddress(ole, "CoInitializeEx");
    FCoUninit fCoUninit = (FCoUninit)GetProcAddress(ole, "CoUninitialize");
    FCoFree fCoFree = (FCoFree)GetProcAddress(ole, "CoTaskMemFree");
    FMfStart fMfStart = (FMfStart)GetProcAddress(mfp, "MFStartup");
    FMfStop fMfStop = (FMfStop)GetProcAddress(mfp, "MFShutdown");
    FMfAttrs fMfAttrs = (FMfAttrs)GetProcAddress(mfp, "MFCreateAttributes");
    FMfMkType fMfMkType = (FMfMkType)GetProcAddress(mfp, "MFCreateMediaType");
    FMfEnum fMfEnum = (FMfEnum)GetProcAddress(mfl, "MFEnumDeviceSources");
    FMfReader fMfReader = (FMfReader)GetProcAddress(mfr, "MFCreateSourceReaderFromMediaSource");
    if (fCoInit == 0 || fMfStart == 0 || fMfAttrs == 0 || fMfEnum == 0 || fMfReader == 0 ||
        fMfMkType == 0) {
        out->rc = -2;
        return 0;
    }
    fCoInit(0, 0);
    fMfStart(0x20070, 0);
    void *attr = 0;
    fMfAttrs(&attr, 1);
    if (attr == 0) {
        out->rc = -2;
        fMfStop();
        fCoUninit();
        return 0;
    }
    ((fk_vt_guid2)fk_vt(attr)[24])(attr, &fk_g_devsrc_type, &fk_g_devsrc_vidcap);
    void **acts = 0;
    unsigned int nact = 0;
    fMfEnum(attr, &acts, &nact);
    if (nact == 0 || acts == 0) {
        out->rc = -3;
        fk_com_release(attr);
        fMfStop();
        fCoUninit();
        return 0;
    }
    void *src = 0;
    out->hr = ((fk_vt_act)fk_vt(acts[0])[33])(acts[0], &fk_g_iid_mediasource, &src);
    if (src == 0) {
        out->rc = -4;
    }
    void *reader = 0;
    if (src != 0) {
        void *rattr = 0;
        fMfAttrs(&rattr, 1);
        if (rattr != 0) {
            ((fk_vt_u32set)fk_vt(rattr)[21])(rattr, &fk_g_reader_processing, 1);
        }
        out->hr = fMfReader(src, rattr, &reader);
        fk_com_release(rattr);
        if (reader == 0) {
            out->rc = -5;
        }
    }
    if (reader != 0) {
        void *mt = 0;
        fMfMkType(&mt);
        if (mt != 0) {
            ((fk_vt_guid2)fk_vt(mt)[24])(mt, &fk_g_mt_major, &fk_g_video_major);
            ((fk_vt_guid2)fk_vt(mt)[24])(mt, &fk_g_mt_subtype, &fk_g_fmt_nv12);
            ((fk_vt_mtset)fk_vt(reader)[7])(reader, 0xFFFFFFFCu, 0, mt);
            fk_com_release(mt);
        }
        void *cur = 0;
        ((fk_vt_mtget)fk_vt(reader)[6])(reader, 0xFFFFFFFCu, &cur);
        unsigned long long fs = 0;
        if (cur != 0) {
            ((fk_vt_u64get)fk_vt(cur)[8])(cur, &fk_g_mt_framesize, &fs);
            fk_com_release(cur);
        }
        long long w = (long long)(fs >> 32);
        long long hh = (long long)(fs & 0xFFFFFFFFu);
        void *sample = 0;
        int tries = 0;
        while (tries < 30 && sample == 0) {
            unsigned int si = 0;
            unsigned int fl = 0;
            long long ts = 0;
            void *s2 = 0;
            out->hr = ((fk_vt_readsample)fk_vt(reader)[9])(reader, 0xFFFFFFFCu, 0, &si, &fl,
                                                           &ts, &s2);
            if (out->hr != 0) {
                break;
            }
            sample = s2;
            tries = tries + 1;
        }
        if (sample != 0) {
            void *mbuf = 0;
            ((fk_vt_pp)fk_vt(sample)[41])(sample, &mbuf);
            if (mbuf != 0) {
                unsigned char *p = 0;
                unsigned int maxl = 0;
                unsigned int curl = 0;
                ((fk_vt_lockbuf)fk_vt(mbuf)[3])(mbuf, &p, &maxl, &curl);
                if (p != 0) {
                    long long ylen = w * hh;
                    if (ylen <= 0 || ylen > (long long)curl) {
                        ylen = (long long)curl;
                    }
                    long long sum = 0;
                    long long nz = 0;
                    long long j;
                    for (j = 0; j < ylen; j = j + 1) {
                        sum = sum + p[j];
                        if (p[j] != 0) {
                            nz = nz + 1;
                        }
                    }
                    out->w = w;
                    out->h = hh;
                    out->luma = ylen > 0 ? sum / ylen : 0;
                    out->nonzero = nz;
                    out->rc = 0;
                    ((fk_vt_none)fk_vt(mbuf)[4])(mbuf);
                }
                fk_com_release(mbuf);
            }
            fk_com_release(sample);
        } else if (out->rc == -1) {
            out->rc = -6;
        }
        fk_com_release(reader);
    }
    if (src != 0) {
        ((fk_vt_none)fk_vt(src)[12])(src);
        fk_com_release(src);
    }
    unsigned int ai;
    for (ai = 0; ai < nact; ai = ai + 1) {
        fk_com_release(acts[ai]);
    }
    if (fCoFree != 0) {
        fCoFree(acts);
    }
    fk_com_release(attr);
    fMfStop();
    fCoUninit();
    return 0;
}
static long long fk_cam_luma(long long timeout_ms) {
    if (timeout_ms < 1000) {
        timeout_ms = 1000;
    }
    if (timeout_ms > 30000) {
        timeout_ms = 30000;
    }
    struct fk_camluma *c = malloc(sizeof(struct fk_camluma));
    if (c == 0) {
        return 1;
    }
    c->w = 0;
    c->h = 0;
    c->luma = 0;
    c->nonzero = 0;
    c->rc = -1;
    c->hr = 0;
    void *th = CreateThread((void *)0, 0, fk_cam_luma_run, c, 0, (unsigned int *)0);
    if (th == 0) {
        free(c);
        return 1;
    }
    if (WaitForSingleObject(th, (unsigned int)timeout_ms) != 0) {
        CloseHandle(th);
        printf("sense: camera luma timed out after %lld ms — refusing honestly\n", timeout_ms);
        return 1;
    }
    CloseHandle(th);
    if (c->rc != 0) {
        printf("sense: camera luma refused at step %lld (hr=0x%08x)%s\n", c->rc,
               (unsigned int)c->hr,
               (unsigned int)c->hr == 0x80070005u
                   ? " — Windows camera privacy settings deny access"
                   : "");
        long long rc2 = c->rc;
        free(c);
        return rc2 == 0 ? 1 : 1;
    }
    printf("sense: camera frame %lldx%lld mean-luma=%lld nonzero=%lld — measured, not retained\n",
           c->w, c->h, c->luma, c->nonzero);
    long long r = 1;
    r = fk_cons_val(c->nonzero << 1, r);
    r = fk_cons_val(c->luma << 1, r);
    r = fk_cons_val(c->h << 1, r);
    r = fk_cons_val(c->w << 1, r);
    free(c);
    return r;
}
/* ── audio LOOPBACK (waveOut render + waveIn capture): the body speaks a known tone through
 * the speakers and hears itself through the mic — the render+capture legs of the speech
 * loopback carrier contract, on this cell's own metal. Layout: silence quarter, 440Hz square
 * burst half, silence quarter. Sixteen per-window energies + burst/silence means + score
 * cross into Form as integers; no waveform is retained. Muted speakers score low, honestly. */
extern unsigned int waveOutOpen(void **, unsigned int, const struct fk_waveformatex *,
                                unsigned long long, unsigned long long, unsigned long long);
extern unsigned int waveOutClose(void *);
extern unsigned int waveOutPrepareHeader(void *, struct fk_wavehdr *, unsigned int);
extern unsigned int waveOutUnprepareHeader(void *, struct fk_wavehdr *, unsigned int);
extern unsigned int waveOutWrite(void *, struct fk_wavehdr *, unsigned int);
extern unsigned int waveOutReset(void *);
extern unsigned int waveOutSetVolume(void *, unsigned int);
static long long fk_audio_loopback(long long ms) {
    if (ms < 500) {
        ms = 500;
    }
    if (ms > 5000) {
        ms = 5000;
    }
    struct fk_waveformatex fmt;
    fmt.wFormatTag = 1;
    fmt.nChannels = 1;
    fmt.nSamplesPerSec = 16000;
    fmt.wBitsPerSample = 16;
    fmt.nBlockAlign = 2;
    fmt.nAvgBytesPerSec = 32000;
    fmt.cbSize = 0;
    long long nsamp = ms * 16;
    short *play = malloc((unsigned long)(nsamp * 2));
    short *cap = malloc((unsigned long)(nsamp * 2));
    if (play == 0 || cap == 0) {
        free(play);
        free(cap);
        return 1;
    }
    long long q = nsamp / 4;
    long long i;
    for (i = 0; i < nsamp; i = i + 1) {
        if (i >= q && i < q * 3) {
            /* 440Hz square at 16kHz: half-period ~18 samples */
            play[i] = ((i / 18) & 1) ? (short)6000 : (short)-6000;
        } else {
            play[i] = 0;
        }
        cap[i] = 0;
    }
    void *hin = 0;
    if (waveInOpen(&hin, 0xFFFFFFFFu, &fmt, 0, 0, 0) != 0) {
        printf("sense: loopback mic open refused\n");
        free(play);
        free(cap);
        return 1;
    }
    void *hout = 0;
    if (waveOutOpen(&hout, 0xFFFFFFFFu, &fmt, 0, 0, 0) != 0) {
        printf("sense: loopback speaker open refused\n");
        waveInClose(hin);
        free(play);
        free(cap);
        return 1;
    }
    struct fk_wavehdr hc;
    hc.lpData = (char *)cap;
    hc.dwBufferLength = (unsigned int)(nsamp * 2);
    hc.dwBytesRecorded = 0;
    hc.dwUser = 0;
    hc.dwFlags = 0;
    hc.dwLoops = 0;
    hc.lpNext = 0;
    hc.reserved = 0;
    struct fk_wavehdr hp;
    hp.lpData = (char *)play;
    hp.dwBufferLength = (unsigned int)(nsamp * 2);
    hp.dwBytesRecorded = 0;
    hp.dwUser = 0;
    hp.dwFlags = 0;
    hp.dwLoops = 0;
    hp.lpNext = 0;
    hp.reserved = 0;
    waveInPrepareHeader(hin, &hc, (unsigned int)sizeof hc);
    waveInAddBuffer(hin, &hc, (unsigned int)sizeof hc);
    waveInStart(hin);
    waveOutPrepareHeader(hout, &hp, (unsigned int)sizeof hp);
    waveOutWrite(hout, &hp, (unsigned int)sizeof hp);
    long long waited = 0;
    while ((hc.dwFlags & 1) == 0 && waited < ms + 3000) {
        Sleep(50);
        waited = waited + 50;
    }
    waveOutReset(hout);
    waveOutUnprepareHeader(hout, &hp, (unsigned int)sizeof hp);
    waveOutClose(hout);
    waveInReset(hin);
    waveInUnprepareHeader(hin, &hc, (unsigned int)sizeof hc);
    waveInClose(hin);
    long long got = (long long)hc.dwBytesRecorded / 2;
    long long wen[16];
    long long w;
    for (w = 0; w < 16; w = w + 1) {
        long long lo = got * w / 16;
        long long hi = got * (w + 1) / 16;
        long long sum = 0;
        for (i = lo; i < hi; i = i + 1) {
            long long s = (long long)cap[i];
            sum = sum + (s < 0 ? 0 - s : s);
        }
        wen[w] = (hi > lo) ? sum / (hi - lo) : 0;
    }
    free(play);
    free(cap);
    long long burst = 0;
    long long silen = 0;
    for (w = 0; w < 16; w = w + 1) {
        if (w >= 4 && w < 12) {
            burst = burst + wen[w];
        } else {
            silen = silen + wen[w];
        }
    }
    burst = burst / 8;
    silen = silen / 8;
    long long score = burst * 100 / (silen + 1);
    printf("sense: loopback rendered %lld ms, captured %lld samples — burst-energy=%lld "
           "silence-energy=%lld score=%lld — measured, not retained\n",
           ms, got, burst, silen, score);
    long long r = 1;
    for (w = 16; w > 0; w = w - 1) {
        r = fk_cons_val(wen[w - 1] << 1, r);
    }
    r = fk_cons_val(score << 1, r);
    r = fk_cons_val(burst << 1, r);
    r = fk_cons_val(silen << 1, r);
    r = fk_cons_val(got << 1, r);
    return r;
}
/* ── wav AIR-LOOPBACK (waveOut plays a 16kHz mono PCM wav while waveIn captures): the
 * composed speech leg — spoken truth through the speakers, heard by the mic. The capture IS
 * written (out-path, canonical 44-byte header) because the local STT oracle must transcribe
 * it; the calling recipe consumes the file after measuring (fs_remove) — transient teacher
 * material, the macOS carrier's own pattern, never silent retention. Returns
 * (played captured peak mean-abs); nil on refusal. */
static long long fk_wav_loopback(const char *inpath, const char *outpath) {
    int fd = open(inpath, O_RDBIN);
    if (fd < 0) {
        printf("sense: air-loopback input wav missing\n");
        return 1;
    }
    long long incap = 4000000;
    char *inbuf = malloc((unsigned long)incap);
    if (inbuf == 0) {
        close(fd);
        return 1;
    }
    long long inlen = 0;
    long long g;
    while (inlen < incap && (g = read(fd, inbuf + inlen, 65536)) > 0) {
        inlen = inlen + g;
    }
    close(fd);
    /* find the data chunk (SAPI writes RIFF/WAVE with fmt then data) */
    long long doff = -1;
    long long i;
    for (i = 12; i + 8 < inlen; i = i + 1) {
        if (inbuf[i] == 'd' && inbuf[i + 1] == 'a' && inbuf[i + 2] == 't' &&
            inbuf[i + 3] == 'a') {
            doff = i + 8;
            break;
        }
    }
    if (doff < 0) {
        printf("sense: air-loopback input wav has no data chunk\n");
        free(inbuf);
        return 1;
    }
    long long dlen = (long long)(unsigned char)inbuf[doff - 4] |
                     ((long long)(unsigned char)inbuf[doff - 3] << 8) |
                     ((long long)(unsigned char)inbuf[doff - 2] << 16) |
                     ((long long)(unsigned char)inbuf[doff - 1] << 24);
    if (dlen <= 0 || doff + dlen > inlen) {
        dlen = inlen - doff;
    }
    long long nplay = dlen / 2;
    if (nplay < 1600 || nplay > 160000) {
        printf("sense: air-loopback wav length out of range (%lld samples)\n", nplay);
        free(inbuf);
        return 1;
    }
    long long ncap = nplay + 8000; /* half-second tail */
    short *cap = malloc((unsigned long)(ncap * 2));
    if (cap == 0) {
        free(inbuf);
        return 1;
    }
    struct fk_waveformatex fmt;
    fmt.wFormatTag = 1;
    fmt.nChannels = 1;
    fmt.nSamplesPerSec = 16000;
    fmt.nAvgBytesPerSec = 32000;
    fmt.nBlockAlign = 2;
    fmt.wBitsPerSample = 16;
    fmt.cbSize = 0;
    void *hin = 0;
    if (waveInOpen(&hin, 0xFFFFFFFFu, &fmt, 0, 0, 0) != 0) {
        printf("sense: air-loopback mic open refused\n");
        free(inbuf);
        free(cap);
        return 1;
    }
    void *hout = 0;
    if (waveOutOpen(&hout, 0xFFFFFFFFu, &fmt, 0, 0, 0) != 0) {
        printf("sense: air-loopback speaker open refused\n");
        waveInClose(hin);
        free(inbuf);
        free(cap);
        return 1;
    }
    /* pin THIS SESSION's playback to full scale (never touches the user's master volume) */
    waveOutSetVolume(hout, 0xFFFFFFFFu);
    struct fk_wavehdr hc;
    hc.lpData = (char *)cap;
    hc.dwBufferLength = (unsigned int)(ncap * 2);
    hc.dwBytesRecorded = 0;
    hc.dwUser = 0;
    hc.dwFlags = 0;
    hc.dwLoops = 0;
    hc.lpNext = 0;
    hc.reserved = 0;
    struct fk_wavehdr hp;
    hp.lpData = inbuf + doff;
    hp.dwBufferLength = (unsigned int)(nplay * 2);
    hp.dwBytesRecorded = 0;
    hp.dwUser = 0;
    hp.dwFlags = 0;
    hp.dwLoops = 0;
    hp.lpNext = 0;
    hp.reserved = 0;
    waveInPrepareHeader(hin, &hc, (unsigned int)sizeof hc);
    waveInAddBuffer(hin, &hc, (unsigned int)sizeof hc);
    waveInStart(hin);
    waveOutPrepareHeader(hout, &hp, (unsigned int)sizeof hp);
    waveOutWrite(hout, &hp, (unsigned int)sizeof hp);
    long long ms = ncap / 16;
    long long waited = 0;
    while ((hc.dwFlags & 1) == 0 && waited < ms + 3000) {
        Sleep(50);
        waited = waited + 50;
    }
    waveOutReset(hout);
    waveOutUnprepareHeader(hout, &hp, (unsigned int)sizeof hp);
    waveOutClose(hout);
    waveInReset(hin);
    waveInUnprepareHeader(hin, &hc, (unsigned int)sizeof hc);
    waveInClose(hin);
    free(inbuf);
    long long got = (long long)hc.dwBytesRecorded / 2;
    long long peak = 0;
    long long sumabs = 0;
    for (i = 0; i < got; i = i + 1) {
        long long s = (long long)cap[i];
        long long a = s < 0 ? 0 - s : s;
        if (a > peak) {
            peak = a;
        }
        sumabs = sumabs + a;
    }
    long long meanabs = got > 0 ? sumabs / got : 0;
    /* auto-gain for the oracle: a quiet mic (low input slider) yields peaks far below full
     * scale; scale the capture so its peak sits near -3dB (26000), capped at 64x. The content
     * is untouched — only the level; peak/mean above report the RAW capture honestly. */
    if (peak > 0 && peak < 26000) {
        long long gain = 26000 / peak;
        if (gain > 64) {
            gain = 64;
        }
        if (gain > 1) {
            for (i = 0; i < got; i = i + 1) {
                long long s = (long long)cap[i] * gain;
                if (s > 32767) {
                    s = 32767;
                }
                if (s < -32768) {
                    s = -32768;
                }
                cap[i] = (short)s;
            }
        }
    }
    /* write the capture for the oracle: canonical 44-byte header + data */
    int wfd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (wfd < 0) {
        printf("sense: air-loopback capture write refused\n");
        free(cap);
        return 1;
    }
    unsigned char hdr[44];
    long long db = got * 2;
    long long riff = 36 + db;
    hdr[0] = 'R';
    hdr[1] = 'I';
    hdr[2] = 'F';
    hdr[3] = 'F';
    hdr[4] = (unsigned char)(riff & 255);
    hdr[5] = (unsigned char)((riff >> 8) & 255);
    hdr[6] = (unsigned char)((riff >> 16) & 255);
    hdr[7] = (unsigned char)((riff >> 24) & 255);
    hdr[8] = 'W';
    hdr[9] = 'A';
    hdr[10] = 'V';
    hdr[11] = 'E';
    hdr[12] = 'f';
    hdr[13] = 'm';
    hdr[14] = 't';
    hdr[15] = ' ';
    hdr[16] = 16;
    hdr[17] = 0;
    hdr[18] = 0;
    hdr[19] = 0;
    hdr[20] = 1;
    hdr[21] = 0;
    hdr[22] = 1;
    hdr[23] = 0;
    hdr[24] = (unsigned char)(16000 & 255);
    hdr[25] = (unsigned char)((16000 >> 8) & 255);
    hdr[26] = 0;
    hdr[27] = 0;
    hdr[28] = (unsigned char)(32000 & 255);
    hdr[29] = (unsigned char)((32000 >> 8) & 255);
    hdr[30] = 0;
    hdr[31] = 0;
    hdr[32] = 2;
    hdr[33] = 0;
    hdr[34] = 16;
    hdr[35] = 0;
    hdr[36] = 'd';
    hdr[37] = 'a';
    hdr[38] = 't';
    hdr[39] = 'a';
    hdr[40] = (unsigned char)(db & 255);
    hdr[41] = (unsigned char)((db >> 8) & 255);
    hdr[42] = (unsigned char)((db >> 16) & 255);
    hdr[43] = (unsigned char)((db >> 24) & 255);
    write(wfd, hdr, 44);
    long long wr = 0;
    while (wr < db) {
        long long k = write(wfd, (char *)cap + wr, db - wr);
        if (k <= 0) {
            break;
        }
        wr = wr + k;
    }
    close(wfd);
    free(cap);
    printf("sense: air-loopback played %lld samples, captured %lld — peak=%lld mean-abs=%lld "
           "(capture written for the oracle; the recipe consumes it)\n",
           nplay, got, peak, meanabs);
    long long r = 1;
    r = fk_cons_val(meanabs << 1, r);
    r = fk_cons_val(peak << 1, r);
    r = fk_cons_val(got << 1, r);
    r = fk_cons_val(nplay << 1, r);
    return r;
}
#elif defined(__APPLE__)
static long long fk_cons_val(long long h, long long t);
extern void *memcpy(void *, const void *, unsigned long);
extern int memcmp(const void *, const void *, unsigned long);
/* ── mac CoreAudio arm (2026-07-31): the else branch below stood honest for a season — "mac
 * CoreAudio/AVFoundation carriers are named pending" — and the live voice loop is what finally
 * demanded it: the loop ran with ffmpeg at the eardrum and afplay at the cone, and the goal is
 * ALL Form native. AudioToolbox is reached through dlopen/dlsym (the nvcuda/Metal door
 * discipline), so the canonical `cc -O2 fkwu-uni.c` build gains no link flags. AudioQueue's C
 * API carries both directions; 16 kHz mono s16le both ways, the same wire every ear cell reads.
 *
 * Camera stays honestly pending here — this movement is the VOICE loop's; the eye is its own. */
extern void *dlopen(const char *, int);
extern void *dlsym(void *, const char *);
extern int usleep(unsigned int);
struct fk_asbd {
    double mSampleRate;
    unsigned int mFormatID;
    unsigned int mFormatFlags;
    unsigned int mBytesPerPacket;
    unsigned int mFramesPerPacket;
    unsigned int mBytesPerFrame;
    unsigned int mChannelsPerFrame;
    unsigned int mBitsPerChannel;
    unsigned int mReserved;
};
struct fk_aqbuf {
    unsigned int mAudioDataBytesCapacity;
    void *mAudioData;
    unsigned int mAudioDataByteSize;
    void *mUserData;
    unsigned int mPacketDescriptionCapacity;
    void *mPacketDescriptions;
    unsigned int mPacketDescriptionCount;
};
typedef void (*fk_aq_incb)(void *, void *, struct fk_aqbuf *, const void *, unsigned int,
                           const void *);
typedef void (*fk_aq_outcb)(void *, void *, struct fk_aqbuf *);
static int (*fk_AQNewInput)(const struct fk_asbd *, fk_aq_incb, void *, void *, void *,
                            unsigned int, void **);
static int (*fk_AQNewOutput)(const struct fk_asbd *, fk_aq_outcb, void *, void *, void *,
                             unsigned int, void **);
static int (*fk_AQAllocBuf)(void *, unsigned int, struct fk_aqbuf **);
static int (*fk_AQEnqueue)(void *, struct fk_aqbuf *, unsigned int, const void *);
static int (*fk_AQStart)(void *, const void *);
static int (*fk_AQStop)(void *, unsigned char);
static int (*fk_AQDispose)(void *, unsigned char);
static int fk_aq_loaded;
static int fk_aq_load(void) {
    if (fk_aq_loaded) {
        return fk_AQNewInput != 0;
    }
    fk_aq_loaded = 1;
    void *h = dlopen("/System/Library/Frameworks/AudioToolbox.framework/AudioToolbox", 2);
    if (h == 0) {
        printf("sense: AudioToolbox not reachable\n");
        return 0;
    }
    fk_AQNewInput = (int (*)(const struct fk_asbd *, fk_aq_incb, void *, void *, void *,
                             unsigned int, void **))dlsym(h, "AudioQueueNewInput");
    fk_AQNewOutput = (int (*)(const struct fk_asbd *, fk_aq_outcb, void *, void *, void *,
                              unsigned int, void **))dlsym(h, "AudioQueueNewOutput");
    fk_AQAllocBuf = (int (*)(void *, unsigned int, struct fk_aqbuf **))dlsym(
        h, "AudioQueueAllocateBuffer");
    fk_AQEnqueue = (int (*)(void *, struct fk_aqbuf *, unsigned int, const void *))dlsym(
        h, "AudioQueueEnqueueBuffer");
    fk_AQStart = (int (*)(void *, const void *))dlsym(h, "AudioQueueStart");
    fk_AQStop = (int (*)(void *, unsigned char))dlsym(h, "AudioQueueStop");
    fk_AQDispose = (int (*)(void *, unsigned char))dlsym(h, "AudioQueueDispose");
    return fk_AQNewInput != 0 && fk_AQNewOutput != 0 && fk_AQAllocBuf != 0 && fk_AQEnqueue != 0 &&
           fk_AQStart != 0 && fk_AQStop != 0 && fk_AQDispose != 0;
}
/* The EAR's wire is 16 kHz and fixed: every cell that reads sense_mic_stream_read counts on
 * 32 bytes to the millisecond. The MOUTH's is not, and measurement is what said so — piper
 * renders this body's own voice at 22050 Hz (measured off .hearth/voice-en.wav, 2026-09-08),
 * and playing those samples through a 16 kHz queue drops the voice a fourth and slows it by
 * 1.38x. So the speaking doors carry their rate and the ear door keeps its constant. */
static void fk_asbd_rate(struct fk_asbd *f, double hz) {
    f->mSampleRate = hz;
    f->mFormatID = 0x6C70636D; /* 'lpcm' */
    f->mFormatFlags = 0x4 | 0x8; /* signed-integer | packed */
    f->mBytesPerPacket = 2;
    f->mFramesPerPacket = 1;
    f->mBytesPerFrame = 2;
    f->mChannelsPerFrame = 1;
    f->mBitsPerChannel = 16;
    f->mReserved = 0;
}
static void fk_asbd_16k(struct fk_asbd *f) {
    fk_asbd_rate(f, 16000.0);
}
/* ── the STREAM session (tags 239-241): one input queue held open across calls, so the live
 * loop's frame quantum is the only latency and no file stands between the diaphragm and the
 * ear. Single producer (the AQ thread appends), single consumer (Form reads a prefix and the
 * offsets advance after AQStop or under the produced/consumed discipline below): produced is
 * written ONLY by the callback, consumed ONLY by the reader, both monotone — no lock needed
 * for a bounded ring read behind produced. */
#define FK_MICRING (16000 * 2 * 30) /* 30 s of 16 kHz s16le */
static char fk_micring[FK_MICRING];
static volatile long long fk_mic_produced;
static long long fk_mic_consumed;
static void *fk_micq;
static struct fk_aqbuf *fk_micbufs[4];
static void fk_mic_incb(void *ud, void *q, struct fk_aqbuf *b, const void *ts, unsigned int nd,
                        const void *pd) {
    (void)ud;
    (void)ts;
    (void)nd;
    (void)pd;
    long long n = (long long)b->mAudioDataByteSize;
    long long at = fk_mic_produced;
    long long i;
    for (i = 0; i < n; i = i + 1) {
        fk_micring[(at + i) % FK_MICRING] = ((char *)b->mAudioData)[i];
    }
    fk_mic_produced = at + n;
    fk_AQEnqueue(q, b, 0, 0);
}
static long long fk_mic_stream_start(void) {
    if (!fk_aq_load()) {
        return -1;
    }
    if (fk_micq != 0) {
        return 0; /* already open: idempotent */
    }
    struct fk_asbd f;
    fk_asbd_16k(&f);
    if (fk_AQNewInput(&f, fk_mic_incb, 0, 0, 0, 0, &fk_micq) != 0 || fk_micq == 0) {
        printf("sense: mic stream open refused\n");
        fk_micq = 0;
        return -1;
    }
    fk_mic_produced = 0;
    fk_mic_consumed = 0;
    long long i;
    for (i = 0; i < 4; i = i + 1) {
        if (fk_AQAllocBuf(fk_micq, 3200, &fk_micbufs[i]) == 0) { /* 100 ms each */
            fk_AQEnqueue(fk_micq, fk_micbufs[i], 0, 0);
        }
    }
    if (fk_AQStart(fk_micq, 0) != 0) {
        printf("sense: mic stream start refused\n");
        fk_AQDispose(fk_micq, 1);
        fk_micq = 0;
        return -1;
    }
    return 0;
}
/* read up to max-bytes of new capture as an s16le STRING (the read_file_slice shape, so every
 * ear cell consumes it unchanged). Blocks up to ~wait_ms for the first byte, then returns what
 * is there — the caller owns pacing. Empty string = nothing new within the wait. */
static long long fk_mic_stream_read(long long maxbytes, long long wait_ms) {
    if (fk_micq == 0) {
        return fk_sbuf("", 0);
    }
    if (maxbytes < 2) {
        maxbytes = 2;
    }
    if (maxbytes > FK_MICRING / 2) {
        maxbytes = FK_MICRING / 2;
    }
    long long waited = 0;
    while (fk_mic_produced - fk_mic_consumed < maxbytes && waited < wait_ms) {
        usleep(2000);
        waited = waited + 2;
    }
    long long have = fk_mic_produced - fk_mic_consumed;
    if (have > maxbytes) {
        have = maxbytes;
    }
    if (have <= 0) {
        return fk_sbuf("", 0);
    }
    fk_sinit();
    long long base = fk_sbp;
    while (base + have > fk_scap_b) {
        fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b, fk_scap_b * 2, FK_STORE_STR_BYTES, 0);
        fk_scap_b = fk_scap_b * 2;
        fk_sb_check();
    }
    long long i;
    for (i = 0; i < have; i = i + 1) {
        fk_sb[base + i] = fk_micring[(fk_mic_consumed + i) % FK_MICRING];
    }
    fk_mic_consumed = fk_mic_consumed + have;
    return fk_strv(fk_sintern(base, have)); /* a string answers as a string: fk_strv, the read-back idiom every other door uses */
}
static long long fk_mic_stream_stop(void) {
    if (fk_micq == 0) {
        return 0;
    }
    fk_AQStop(fk_micq, 1);
    fk_AQDispose(fk_micq, 1);
    fk_micq = 0;
    return 0;
}
/* speaker OUT: play a 16 kHz mono s16le wav through AudioQueue output. done-flag set by the
 * callback when the last buffer drains; while capturing (outpath non-empty in fk_wav_loopback)
 * the mic stream keeps running, so play+capture is one session, not a race of processes. */
static volatile int fk_spk_pending;
static void fk_spk_outcb(void *ud, void *q, struct fk_aqbuf *b) {
    (void)ud;
    (void)q;
    (void)b;
    fk_spk_pending = fk_spk_pending - 1;
}
static long long fk_spk_play(const char *inbuf, long long doff, long long dlen, long long hz) {
    if (!fk_aq_load()) {
        return -1;
    }
    struct fk_asbd f;
    fk_asbd_rate(&f, (double)hz);
    void *q = 0;
    if (fk_AQNewOutput(&f, fk_spk_outcb, 0, 0, 0, 0, &q) != 0 || q == 0) {
        printf("sense: speaker open refused\n");
        return -1;
    }
    long long chunk = 16000; /* 500 ms per buffer */
    long long off = 0;
    fk_spk_pending = 0;
    while (off < dlen) {
        long long n = dlen - off > chunk ? chunk : dlen - off;
        struct fk_aqbuf *b = 0;
        if (fk_AQAllocBuf(q, (unsigned int)n, &b) != 0 || b == 0) {
            break;
        }
        long long i;
        for (i = 0; i < n; i = i + 1) {
            ((char *)b->mAudioData)[i] = inbuf[doff + off + i];
        }
        b->mAudioDataByteSize = (unsigned int)n;
        fk_spk_pending = fk_spk_pending + 1;
        fk_AQEnqueue(q, b, 0, 0);
        off = off + n;
    }
    fk_AQStart(q, 0);
    long long ms = hz > 0 ? dlen * 500 / hz : dlen / 32; /* 2 bytes a frame: ms = bytes*500/hz */
    long long waited = 0;
    while (fk_spk_pending > 0 && waited < ms + 2000) {
        usleep(10000);
        waited = waited + 10;
    }
    fk_AQStop(q, 1);
    fk_AQDispose(q, 1);
    return off; /* BYTES enqueued and drained — the honest count, not a flag */
}
/* ── the SPEAKING family (tag 190, modes 0-6): the twin of the mic doors above ─────────────
 * The seed has held ears since 2026-07-31 and no mouth: every sense_* door named an inward
 * direction, so when the body spoke, voice-say.bml handed a wav to `afplay` through host-exec
 * and a crossing stood in the middle of the body's own voice. fk_spk_play was already here —
 * complete, correct, and reachable from exactly one place, fk_wav_loopback's air probe. A door
 * nothing can call is not a capability (mirror-census armhush, corpus row 1358). These are the
 * rows that give it a name.
 *
 * ONE WIRE, THE MIC'S OWN: s16le mono 16 kHz, the shape fk_asbd_16k already declares for both
 * directions, the shape sense_mic_stream_read hands back (3200 bytes = 100 ms), and the shape
 * every ear cell's wav carries after its 44-byte header. Samples arrive as a Form STRING, read
 * by pointer and length — never through fk_cstr, because s16le is full of NUL bytes and a
 * C-string copy would cut the voice at the first silent sample.
 *
 * TWO REFUSALS, AND THEY ANSWER DIFFERENT QUESTIONS:
 *   -1  the mouth: no output device, the open was refused, or no stream stands.
 *   -2  the samples: not a string at all (nothing, an int, a list), or an odd byte count —
 *       half a sample is not a sample. Checked FIRST, so a bad wire never opens a device and
 *       never makes a sound; a band can walk every refusal in a silent room.
 * Zero samples answer 0 and open nothing: honest, not an error.
 *
 * The STREAM lane exists for the mouth that is coming. A voice generating token by token must
 * be able to speak its first word before its last one is decided, so the queue is held open
 * across calls and a write returns as soon as the samples are handed to the device. Bounded
 * everywhere: a write waits at most 4 s for a free buffer and then answers the partial count it
 * truly accepted; a stop drains at most 30 s so a sentence finishes rather than being cut. */
#define FK_SPKBUFS 16
#define FK_SPKBUFB 32000 /* 1 s of 16 kHz s16le in each */
static void *fk_spkq;
static struct fk_aqbuf *fk_spkbufs[FK_SPKBUFS];
static volatile int fk_spkbusy[FK_SPKBUFS];
static int fk_spk_running;
static void fk_spk_streamcb(void *ud, void *q, struct fk_aqbuf *b) {
    (void)ud;
    (void)q;
    long long i;
    for (i = 0; i < FK_SPKBUFS; i = i + 1) {
        if (fk_spkbufs[i] == b) {
            fk_spkbusy[i] = 0;
            return;
        }
    }
}
static long long fk_spk_present(void) {
    return fk_aq_load() ? 1 : 0;
}
static long long fk_spk_name(long long i) {
    if (i == 0 && fk_aq_load()) {
        return fk_sbuf("coreaudio-default", 17);
    }
    return fk_sbuf("", 0);
}
static long long fk_spk_health(long long i) {
    return (i == 0 && fk_aq_load()) ? 1 : -1;
}
static long long fk_spk_say(const char *p, long long n, long long hz) {
    long long b = fk_spk_play(p, 0, n, hz);
    return b < 0 ? -1 : b / 2;
}
static long long fk_spk_stream_start(long long hz) {
    if (!fk_aq_load()) {
        return -1;
    }
    if (fk_spkq != 0) {
        return 0; /* already open: idempotent, like the mic's own start */
    }
    struct fk_asbd f;
    fk_asbd_rate(&f, (double)hz);
    if (fk_AQNewOutput(&f, fk_spk_streamcb, 0, 0, 0, 0, &fk_spkq) != 0 || fk_spkq == 0) {
        printf("sense: speaker stream open refused\n");
        fk_spkq = 0;
        return -1;
    }
    long long i;
    for (i = 0; i < FK_SPKBUFS; i = i + 1) {
        fk_spkbufs[i] = 0;
        fk_spkbusy[i] = 0;
        if (fk_AQAllocBuf(fk_spkq, FK_SPKBUFB, &fk_spkbufs[i]) != 0) {
            fk_spkbufs[i] = 0;
        }
    }
    fk_spk_running = 0;
    return 0;
}
/* hand samples to the open mouth and return: the queue starts on the FIRST write, so a voice
 * that has decided one word does not wait for the sentence. Answers the samples accepted. */
static long long fk_spk_stream_write(const char *p, long long n) {
    if (fk_spkq == 0) {
        return -1;
    }
    long long off = 0;
    while (off < n) {
        long long take = n - off > FK_SPKBUFB ? FK_SPKBUFB : n - off;
        long long idx = -1;
        long long waited = 0;
        while (idx < 0) {
            long long i;
            for (i = 0; i < FK_SPKBUFS; i = i + 1) {
                if (fk_spkbufs[i] != 0 && fk_spkbusy[i] == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx >= 0 || waited >= 4000) {
                break;
            }
            usleep(10000);
            waited = waited + 10;
        }
        if (idx < 0) {
            break; /* every buffer still in flight: answer what was truly taken */
        }
        long long j;
        for (j = 0; j < take; j = j + 1) {
            ((char *)fk_spkbufs[idx]->mAudioData)[j] = p[off + j];
        }
        fk_spkbufs[idx]->mAudioDataByteSize = (unsigned int)take;
        fk_spkbusy[idx] = 1;
        if (fk_AQEnqueue(fk_spkq, fk_spkbufs[idx], 0, 0) != 0) {
            fk_spkbusy[idx] = 0;
            break;
        }
        if (!fk_spk_running) {
            if (fk_AQStart(fk_spkq, 0) != 0) {
                printf("sense: speaker stream start refused\n");
                fk_spkbusy[idx] = 0;
                break;
            }
            fk_spk_running = 1;
        }
        off = off + take;
    }
    return off / 2;
}
static long long fk_spk_stream_stop(void) {
    if (fk_spkq == 0) {
        return 0;
    }
    long long waited = 0;
    while (waited < 30000) {
        long long busy = 0, i;
        for (i = 0; i < FK_SPKBUFS; i = i + 1) {
            if (fk_spkbusy[i]) {
                busy = 1;
            }
        }
        if (!busy) {
            break;
        }
        usleep(10000);
        waited = waited + 10;
    }
    fk_AQStop(fk_spkq, 1);
    fk_AQDispose(fk_spkq, 1);
    fk_spkq = 0;
    fk_spk_running = 0;
    long long i;
    for (i = 0; i < FK_SPKBUFS; i = i + 1) {
        fk_spkbufs[i] = 0;
        fk_spkbusy[i] = 0;
    }
    return 0;
}
static long long fk_mic_count(void) {
    return fk_aq_load() ? 1 : 0;
}
static long long fk_mic_name(long long i) {
    if (i == 0 && fk_aq_load()) {
        return fk_sbuf("coreaudio-default", 17);
    }
    return fk_sbuf("", 0);
}
static long long fk_mic_health(long long i) {
    return (i == 0 && fk_aq_load()) ? 1 : -1;
}
static long long fk_cam_count(void) {
    return 0;
}
static long long fk_cam_name(long long i) {
    (void)i;
    return fk_sbuf("", 0);
}
static long long fk_cam_health(long long i) {
    (void)i;
    return -1;
}
static long long fk_cam_grab(long long i, const char *path) {
    (void)i;
    (void)path;
    return -1;
}
/* tag 234 keeps the Windows arm's exact privacy contract: stats only, nothing retained */
static long long fk_mic_capture(long long ms) {
    if (ms < 100) {
        ms = 100;
    }
    if (ms > 10000) {
        ms = 10000;
    }
    if (fk_mic_stream_start() != 0) {
        return 1;
    }
    long long want = ms * 32;
    long long t = 0;
    while (fk_mic_produced < want && t < ms + 2000) {
        usleep(10000);
        t = t + 10;
    }
    long long nsamp = (fk_mic_produced < want ? fk_mic_produced : want) / 2;
    long long nonzero = 0, peak = 0, sumabs = 0, i;
    for (i = 0; i < nsamp; i = i + 1) {
        long long lo = (unsigned char)fk_micring[(i * 2) % FK_MICRING];
        long long hi = (unsigned char)fk_micring[(i * 2 + 1) % FK_MICRING];
        long long v = lo | (hi << 8);
        if (v >= 32768) {
            v = v - 65536;
        }
        long long a = v < 0 ? 0 - v : v;
        if (a > 0) {
            nonzero = nonzero + 1;
        }
        if (a > peak) {
            peak = a;
        }
        sumabs = sumabs + a;
    }
    fk_mic_stream_stop();
    long long meanabs = nsamp > 0 ? sumabs / nsamp : 0;
    printf("sense: mic captured %lld samples (%lld ms) nonzero=%lld mean-abs=%lld peak=%lld — "
           "measured, not retained\n",
           nsamp, ms, nonzero, meanabs, peak);
    long long r = 1;
    r = fk_cons_val(peak << 1, r);
    r = fk_cons_val(meanabs << 1, r);
    r = fk_cons_val(nonzero << 1, r);
    r = fk_cons_val(nsamp << 1, r);
    return r;
}
static long long fk_cam_luma(long long timeout_ms) {
    (void)timeout_ms;
    return 1;
}
static long long fk_audio_loopback(long long ms) {
    (void)ms;
    return 1;
}
/* tag 237 mac arm: play inpath through the speakers; when outpath is non-empty, capture the
 * played span + a half-second tail through the mic stream and write it as a canonical 16 kHz
 * wav — the air loopback, one process, no ffmpeg, no afplay. Returns (played captured peak
 * mean-abs) like the Windows arm; play-only when outpath is "". */
static long long fk_wav_loopback(const char *inpath, const char *outpath) {
    int fd = open(inpath, O_RDBIN);
    if (fd < 0) {
        printf("sense: air-loopback input wav missing\n");
        return 1;
    }
    long long incap = 4000000;
    char *inbuf = malloc((unsigned long)incap);
    if (inbuf == 0) {
        close(fd);
        return 1;
    }
    long long inlen = 0;
    long long g;
    while (inlen < incap && (g = read(fd, inbuf + inlen, 65536)) > 0) {
        inlen = inlen + g;
    }
    close(fd);
    long long doff = -1;
    long long i;
    for (i = 12; i + 8 < inlen; i = i + 1) {
        if (inbuf[i] == 'd' && inbuf[i + 1] == 'a' && inbuf[i + 2] == 't' &&
            inbuf[i + 3] == 'a') {
            doff = i + 8;
            break;
        }
    }
    if (doff < 0) {
        printf("sense: air-loopback input wav has no data chunk\n");
        free(inbuf);
        return 1;
    }
    long long dlen = (long long)(unsigned char)inbuf[doff - 4] |
                     ((long long)(unsigned char)inbuf[doff - 3] << 8) |
                     ((long long)(unsigned char)inbuf[doff - 2] << 16) |
                     ((long long)(unsigned char)inbuf[doff - 1] << 24);
    if (dlen <= 0 || doff + dlen > inlen) {
        dlen = inlen - doff;
    }
    long long capture = outpath != 0 && outpath[0] != 0;
    long long mark = 0;
    if (capture) {
        if (fk_mic_stream_start() != 0) {
            capture = 0;
        } else {
            mark = fk_mic_produced;
        }
    }
    long long pb = fk_spk_play(inbuf, doff, dlen, 16000); /* the air probe's own canonical wav */
    long long played = pb > 0 ? pb / 2 : 0;
    free(inbuf);
    long long got = 0, peak = 0, sumabs = 0;
    if (capture) {
        long long tailwant = dlen + 16000; /* half-second tail at 32 B/ms */
        long long t = 0;
        while (fk_mic_produced - mark < tailwant && t < dlen / 32 + 1500) {
            usleep(10000);
            t = t + 10;
        }
        long long have = fk_mic_produced - mark;
        if (have > tailwant) {
            have = tailwant;
        }
        int ofd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (ofd >= 0) {
            unsigned char hd[44];
            long long dl = have;
            memcpy(hd, "RIFF", 4);
            hd[4] = (unsigned char)((36 + dl) & 255);
            hd[5] = (unsigned char)(((36 + dl) >> 8) & 255);
            hd[6] = (unsigned char)(((36 + dl) >> 16) & 255);
            hd[7] = (unsigned char)(((36 + dl) >> 24) & 255);
            memcpy(hd + 8, "WAVEfmt ", 8);
            hd[16] = 16; hd[17] = 0; hd[18] = 0; hd[19] = 0;
            hd[20] = 1; hd[21] = 0; hd[22] = 1; hd[23] = 0;
            hd[24] = 0x80; hd[25] = 0x3E; hd[26] = 0; hd[27] = 0; /* 16000 */
            hd[28] = 0; hd[29] = 0x7D; hd[30] = 0; hd[31] = 0;    /* 32000 */
            hd[32] = 2; hd[33] = 0; hd[34] = 16; hd[35] = 0;
            memcpy(hd + 36, "data", 4);
            hd[40] = (unsigned char)(dl & 255);
            hd[41] = (unsigned char)((dl >> 8) & 255);
            hd[42] = (unsigned char)((dl >> 16) & 255);
            hd[43] = (unsigned char)((dl >> 24) & 255);
            write(ofd, hd, 44);
            for (i = 0; i < have; i = i + 1) {
                char c = fk_micring[(mark + i) % FK_MICRING];
                write(ofd, &c, 1);
            }
            close(ofd);
        }
        for (i = 0; i + 1 < have; i = i + 2) {
            long long lo = (unsigned char)fk_micring[(mark + i) % FK_MICRING];
            long long hi = (unsigned char)fk_micring[(mark + i + 1) % FK_MICRING];
            long long v = lo | (hi << 8);
            if (v >= 32768) {
                v = v - 65536;
            }
            long long a = v < 0 ? 0 - v : v;
            if (a > peak) {
                peak = a;
            }
            sumabs = sumabs + a;
        }
        got = have / 2;
        fk_mic_stream_stop();
    }
    long long meanabs = got > 0 ? sumabs / got : 0;
    long long r = 1;
    r = fk_cons_val(meanabs << 1, r);
    r = fk_cons_val(peak << 1, r);
    r = fk_cons_val(got << 1, r);
    r = fk_cons_val(played << 1, r);
    return r;
}
#else
static long long fk_mic_count(void) {
    return 0;
}
static long long fk_mic_name(long long i) {
    (void)i;
    return fk_sbuf("", 0);
}
static long long fk_mic_health(long long i) {
    (void)i;
    return -1;
}
static long long fk_cam_count(void) {
    return 0;
}
static long long fk_cam_name(long long i) {
    (void)i;
    return fk_sbuf("", 0);
}
static long long fk_cam_health(long long i) {
    (void)i;
    return -1;
}
static long long fk_cam_grab(long long i, const char *path) {
    (void)i;
    (void)path;
    return -1;
}
static long long fk_mic_capture(long long ms) {
    (void)ms;
    return 1;
}
static long long fk_cam_luma(long long timeout_ms) {
    (void)timeout_ms;
    return 1;
}
static long long fk_audio_loopback(long long ms) {
    (void)ms;
    return 1;
}
static long long fk_wav_loopback(const char *inpath, const char *outpath) {
    (void)inpath;
    (void)outpath;
    return 1;
}
#endif
#if !defined(__APPLE__)
static long long fk_mic_stream_start(void) {
    return -1;
}
static long long fk_mic_stream_read(long long maxbytes, long long wait_ms) {
    (void)maxbytes;
    (void)wait_ms;
    return fk_sbuf("", 0);
}
static long long fk_mic_stream_stop(void) {
    return 0;
}
/* the speaking family off this host: no mouth stands, and every door says so in the same
 * word it uses when a mac has no reachable device. Nothing here pretends to a sound. */
static long long fk_spk_present(void) {
    return 0;
}
static long long fk_spk_name(long long i) {
    (void)i;
    return fk_sbuf("", 0);
}
static long long fk_spk_health(long long i) {
    (void)i;
    return -1;
}
static long long fk_spk_say(const char *p, long long n, long long hz) {
    (void)p;
    (void)n;
    (void)hz;
    return -1;
}
static long long fk_spk_stream_start(long long hz) {
    (void)hz;
    return -1;
}
static long long fk_spk_stream_write(const char *p, long long n) {
    (void)p;
    (void)n;
    return -1;
}
static long long fk_spk_stream_stop(void) {
    return 0;
}
#endif
/* ── the speaking door, modes 10-16 of the leaf door (tag 201) ─────────────────────────────
 * NO TAG WAS TAKEN. The mic family spends one AST tag per door; this family cannot, because
 * on 2026-09-08 the space 0..255 held nothing to spend. 0 is not a tag; 150 is held in
 * writing as the native-surface probe; and 190 — the one number a census of `if (t == N)`
 * sites reports free — is FK_TAG_CONST_HOLD, the once-hold for a top-level let, named by a
 * #define that no op row and no `t == N` site spells. A first cut of this family took 190 and
 * every door answered its own mode number back: the const-hold walked the first child and
 * returned it. The seed had already written that trap down (the gift-frame note above tag
 * 184, "the third such collision in a week") and the census still could not see it. So the
 * nine names ride the leaf door as rewrite rows the way the binary form (modes 4-8) and
 * substring (mode 9) do, and the tag ledger does not move at all.
 *   10 count   11 name(i)   12 health(i)   13 play (cons samples hz)
 *   14 stream_start(hz)   15 stream_write(samples)   16 stream_stop
 *
 * THE RATE IS CARRIED, NOT ASSUMED. The mic's wire is 16 kHz and every ear cell counts on it,
 * so sense_speaker_play defaults there — but piper renders this body's own voice at 22050 Hz
 * (measured off .hearth/voice-en.wav on the day this door was written), and a 22050 Hz voice
 * played through a 16 kHz queue comes back a fourth lower and 1.38x slow. So play and
 * stream_start take an explicit rate, and the two plain names are rewrites that fill in 16000.
 * Mode 13's operand is a cons pair, the shape write_form_binary (mode 8) already uses to carry
 * two things through a one-operand door.
 *
 * THREE REFUSALS, ANSWERING DIFFERENT QUESTIONS:
 *   -1  the mouth — no output device, an open refused, or no stream standing.
 *   -2  the samples — not a string at all (nothing, an int, a list), or an odd byte count,
 *       because half a sample is not a sample. Checked FIRST, so a bad wire never opens a
 *       device and never makes a sound: every refusal here is walkable in a silent room.
 *   -3  the rate — outside 4000..192000 Hz, which no output device on this host will hold.
 * Zero samples answer 0 and open nothing — honest, not an error.
 *
 * The negatives are written `* 2` rather than `<< 1`: a Form int rides as value*2 and the two
 * are the same bits, but shifting a negative constant is undefined in C and the compiler says
 * so. A warning stepped around is work handed on without consent. */
static long long fk_spk_rate_ok(long long hz) {
    return hz >= 4000 && hz <= 192000;
}
static long long fk_spk_door(long long mode, long long x) {
    if (mode == 13 || mode == 15) {
        long long sv = x;
        long long hz = 16000;
        if (mode == 13) {
            /* (cons samples hz): the one-operand pair, read head then tail */
            long long p = x >> 1;
            if ((x & 1) == 0 || p < 1 || !FK_POK(p)) {
                return (0 - 2) * 2;
            }
            sv = FK_HH(p);
            hz = FK_HT(p) >> 1;
            if (!fk_spk_rate_ok(hz)) {
                return (0 - 3) * 2; /* a rate no device here will hold */
            }
        }
        long long si = fk_stri(sv);
        if (si < 0 || !FK_SOK(si)) {
            return (0 - 2) * 2; /* these are not samples */
        }
        long long n = FK_SLEN(si);
        if ((n & 1) != 0) {
            return (0 - 2) * 2; /* half a sample is not a sample */
        }
        if (n == 0) {
            return 0; /* zero samples: nothing sounds, and nothing is opened */
        }
        return (mode == 13 ? fk_spk_say(FK_SBYTES(si), n, hz)
                           : fk_spk_stream_write(FK_SBYTES(si), n))
               << 1;
    }
    if (mode == 10) {
        return fk_spk_present() << 1;
    }
    if (mode == 11) {
        return fk_spk_name(x >> 1);
    }
    if (mode == 12) {
        return fk_spk_health(x >> 1) << 1;
    }
    if (mode == 14) {
        long long hz14 = x >> 1;
        if (!fk_spk_rate_ok(hz14)) {
            return (0 - 3) * 2;
        }
        return fk_spk_stream_start(hz14) << 1;
    }
    if (mode == 16) {
        return fk_spk_stream_stop() << 1;
    }
    return (0 - 2) * 2; /* an unnamed mode is a wire this door cannot play either */
}
static long long fk_sense_report(void) {
    long long open = 0;
    long long nm = fk_mic_count();
    long long nc = fk_cam_count();
#if defined(_WIN32)
    printf("sense-channels  (Windows host carrier: winmm waveIn + avicap32)\n");
#else
    printf("sense-channels  (this platform's audio/video carrier is pending; presence=0)\n");
#endif
    long long i = 0;
    while (i < nm) {
        long long h = fk_mic_health(i);
        char nb[64];
        fk_cstr(fk_mic_name(i), nb, 64);
        printf("  mic[%d]  afferent-bytes  health=%d  %s\n", (int)i, (int)h, nb);
        if (h > 0) {
            open = open + 1;
        }
        i = i + 1;
    }
    long long j = 0;
    while (j < nc) {
        long long h = fk_cam_health(j);
        char nb[256];
        fk_cstr(fk_cam_name(j), nb, 256);
        printf("  cam[%d]  afferent-pixel  health=%d  %s\n", (int)j, (int)h, nb);
        if (h > 0) {
            open = open + 1;
        }
        j = j + 1;
    }
    printf("open sense channels: %d  (mics=%d cams=%d)\n", (int)open, (int)nm, (int)nc);
    return open;
}
/* ── native perception over the afferent-pixel channel ────────────────────── The afferent READ the
 * sense-channels receipt named pending: fkwu itself walks the captured frame's pixels and emits a
 * NATIVE presence reading — mean luminance, dark-fraction, and a left/center/right band (a coarse
 * subject position cue). Platform-neutral (file IO + byte math); only CAPTURE is a Windows carrier.
 * This is the native model's perception; the rented oracle verifies it, and surprise drives the
 * distill loop (presence-model.fk). SCAFFOLD — pending compost (carrier-last debt, named
 * 2026-06-29): The pixel/luminance MATH below and ALL of fk_sense_stream's level logic (surprise /
 * confidence / trust / sovereignty / the row) is BODY, not carrier. Its native home is Form: the
 * `.fk` cells already exist (surprise-receipt, confidence-earned, native-vs-rented,
 * observe/sense-stream.fk) and compute the same values on this kernel (witnessed hand-flattened).
 * It lives in C only because the source-runner SEED (a flattened form-eval-cli-loop,
 * platform-neutral numeric data) is not yet committed here — the Windows kernel itself is PROVEN
 * able to run recipes/stdin/eval natively, so this is a shared-seed gap, NOT a Windows gap. Two
 * rungs retire it: (1) commit the generated seed -> run the stream LOGIC as Form
 * (observe/sense-stream.fk), delete fk_sense_stream + this math; (2) give the pixel walk
 * (model/frame-luma.fk) its own observed, bounded native-loop carrier. Tree-walking it is
 * C-stack-bound, ~60 deep at 1MB, so 307k pixels need a loop rather than this C implementation.
 * The current structural ARM64 u32 leaf intentionally does not claim that loop. The seed then
 * shrinks to the HAL (grab + raw bytes). See
 * receipts/2026-06-29-pixel-walk-is-form.md + 2026-06-29-windows-flatten-reground.md. */
static unsigned char fk_frame_buf[1000000];
static long long fk_rd32(unsigned char *p) {
    return (long long)p[0] | ((long long)p[1] << 8) | ((long long)p[2] << 16) |
           ((long long)p[3] << 24);
}
/* silent stat over the frame — fills out[0..8] = present,side,mean,darkpct,lm,cm,rm,w,h. Returns 0
 * / -1. */
static long long fk_frame_stat(const char *path, long long *out) {
#if defined(_WIN32)
    int fd = open(path, 0x8000);
/* O_RDONLY | O_BINARY — pixel bytes are binary; text mode would mangle CRLF and stop at 0x1A */
#else
    int fd = open(path, 0);
#endif
    if (fd < 0) {
        return -1;
    }
    long long n = 0;
    long long got;
    while ((got = read(fd, fk_frame_buf + n, 65536)) > 0) {
        n = n + got;
        if (n > 999000) {
            break;
        }
    }
    close(fd);
    if (n < 54) {
        return -1;
    }
    long long off = fk_rd32(fk_frame_buf + 10);
    long long w = fk_rd32(fk_frame_buf + 18);
    long long h = fk_rd32(fk_frame_buf + 22);
    long long bpp = (long long)fk_frame_buf[28] | ((long long)fk_frame_buf[29] << 8);
    if (bpp != 24 || w <= 0 || h <= 0) {
        return -1;
    }
    long long row = (w * 3 + 3) & ~3LL;
    long long sum = 0, dark = 0, ls = 0, cs = 0, rs = 0, lc = 0, cc = 0, rc = 0, cnt = 0;
    long long y = 0;
    while (y < h) {
        long long x = 0;
        while (x < w) {
            long long idx = off + y * row + x * 3;
            if (idx + 2 >= n) {
                x = x + 1;
                continue;
            }
            long long lum = ((long long)fk_frame_buf[idx] + (long long)fk_frame_buf[idx + 1] +
                             (long long)fk_frame_buf[idx + 2]) /
                            3;
            sum = sum + lum;
            if (lum < 60) {
                dark = dark + 1;
            }
            if (x < w / 3) {
                ls = ls + lum;
                lc = lc + 1;
            } else if (x < (2 * w) / 3) {
                cs = cs + lum;
                cc = cc + 1;
            } else {
                rs = rs + lum;
                rc = rc + 1;
            }
            cnt = cnt + 1;
            x = x + 1;
        }
        y = y + 1;
    }
    long long mean = cnt ? sum / cnt : 0;
    long long darkpct = cnt ? (dark * 100) / cnt : 0;
    long long lm = lc ? ls / lc : 0;
    long long cm = cc ? cs / cc : 0;
    long long rm = rc ? rs / rc : 0;
    long long side = (lm <= cm && lm <= rm) ? 0 : ((cm <= lm && cm <= rm) ? 1 : 2);
    long long spread = (lm > rm ? lm - rm : rm - lm);
    long long present = (darkpct >= 8 && darkpct <= 75 && spread >= 12) ? 1 : 0;
    out[0] = present;
    out[1] = side;
    out[2] = mean;
    out[3] = darkpct;
    out[4] = lm;
    out[5] = cm;
    out[6] = rm;
    out[7] = w;
    out[8] = h;
    return 0;
}
static long long fk_frame_read(const char *path) {
    long long o[9];
    if (fk_frame_stat(path, o) < 0) {
        printf("frame-read: no/invalid frame at %s\n", path);
        return -1;
    }
    printf("frame-read  (native, fkwu over %dx%d afferent-pixel)\n", (int)o[7], (int)o[8]);
    printf("  mean-luminance : %d\n", (int)o[2]);
    printf("  dark-fraction%% : %d\n", (int)o[3]);
    printf("  thirds L/C/R   : %d / %d / %d\n", (int)o[4], (int)o[5], (int)o[6]);
    printf("  native reading : subject-present=%d  subject-side=%s\n", (int)o[0],
           o[1] == 0 ? "left" : (o[1] == 1 ? "center" : "right"));
    return o[2];
}
/* ── the multi-level sensing stream: every tick, every level of sensing ────── raw | native local
 * remote meshed | surprise confidence trust sovereignty vitality. The mesh-safe row (mesh-sense-7w:
 * plane,value,source-cell,channel,confidence) is what fuses with the Mac sibling's readings. Here
 * the Windows cell streams the WHERE/presence plane it is sovereign on, and the WHO/identity plane
 * it still rents — confidence/trust rise with agreeing ticks (confidence-earned), sovereignty =
 * native>=rented (native-vs-rented), vitality = open channels. */
static long long fk_sense_stream(long long n) {
    if (n < 1) {
        n = 1;
    }
    if (n > 120) {
        n = 120;
    }
    long long ch = (fk_cam_count() > 0 ? 1 : 0) + (fk_mic_count() > 0 ? 1 : 0);
    long long vit = ch >= 2 ? 9 : (ch == 1 ? 5 : 0);
    printf("sense-stream  device=windows-binary  channel=camera  (%d ticks, live afferent-pixel)\n",
           (int)n);
    printf(
        "  levels: raw | native local remote meshed | surprise confidence trust sovereignty vitality\n");
    long long t = 1;
    while (t <= n) {
        long long o[9];
        if (fk_frame_stat("fkwu-cam-frame.bmp", o) < 0) {
            printf("  t%-2d  raw=-- (no frame on the channel)\n", (int)t);
            t = t + 1;
            continue;
        }
        long long raw = o[2];
        long long present = o[0];
        long long nat = present;
        long long rem = 1;
        long long surp = (rem > nat ? rem - nat : nat - rem) * 9;
        long long conf = nat ? (t + 4 > 9 ? 9 : t + 4) : 0;
        long long trust = conf * 3;
        long long sov = (nat >= rem) ? 1 : 0;
        long long mesh = nat;
        printf(
            "  t%-2d raw=%-3d| presence nat=%d loc=- rem=%d mesh=%d | surp=%d conf=%d trust=%d sov=%d vit=%d\n",
            (int)t, (int)raw, (int)nat, (int)rem, (int)mesh, (int)surp, (int)conf, (int)trust,
            (int)sov, (int)vit);
        printf(
            "  t%-2d raw=%-3d| identity nat=- loc=- rem=9 mesh=R | surp=9 conf=0 trust=0 sov=0 vit=%d\n",
            (int)t, (int)raw, (int)vit);
        t = t + 1;
    }
    printf(
        "stream end: presence is native-sovereign here; identity routes to the mesh (Mac sibling's face-embed / who-plane)\n");
    return n;
}
/* ── the JIT's narrow host touch: Form's typed u32 leaf -> W^X call ────────
 *
 * Tag 215 accepts a structural request only: (program, root, u32 argument).
 * It never accepts a byte image.  C first admits the exact postorder V1 Form
 * tree and then emits the same tiny register-only image itself.  That leaves no
 * raw machine-byte ingress or compatibility format behind.  The old scalar/x64
 * branch is intentionally gone; a future Form-native carrier deletes this seed
 * bridge too.
 */
#ifndef FK_HAVE_MMAN_HEADER
extern void *mmap(void *, unsigned long, int, int, int, long);
#endif
#if defined(FK_HAVE_DARWIN_ARM64_JIT_WITNESS)
#ifndef FK_HAVE_MMAN_HEADER
extern int munmap(void *, unsigned long);
#endif
extern void pthread_jit_write_protect_np(int);
#endif
#if defined(FK_HAVE_DARWIN_ARM64_JIT_WITNESS)
static int fk_arm64_u32_cons(long long cell, long long *head, long long *tail) {
    long long p;
    if ((cell & 1) == 0 || cell <= 1) {
        return 0;
    }
    p = cell >> 1;
    if (p < 1 || !FK_POK(p)) {
        return 0;
    }
    *head = FK_HH(p);
    *tail = FK_HT(p);
    return 1;
}
static int fk_arm64_u32_value(long long value, unsigned int *out) {
    long long n;
    if (value < 0 || (value & 1) != 0) {
        return 0;
    }
    n = value >> 1;
    if (n < 0 || n > 4294967295LL) {
        return 0;
    }
    *out = (unsigned int)n;
    return 1;
}
static int fk_arm64_u32_tmp(unsigned int reg) {
    return reg >= 9U && reg <= 15U;
}
#define FK_ARM64_U32_NODE_CAP 64
#define FK_ARM64_U32_WORD_CAP 64
typedef struct {
    unsigned int tag;
    unsigned int a;
    unsigned int b;
} fk_arm64_u32_node;
/* Read a proper row of at most three values.  The row bound also makes a
 * malformed/cyclic cons tail finite before any tree walk begins. */
static int fk_arm64_u32_row(long long row, long long fields[3], long long *count) {
    long long cursor = row;
    *count = 0;
    while (cursor != 1) {
        long long value;
        if (*count >= 3 || !fk_arm64_u32_cons(cursor, &value, &cursor)) {
            return 0;
        }
        fields[*count] = value;
        *count = *count + 1;
    }
    return 1;
}
/* V1 is deliberately exact: literals and bare arg0 are leaves; arithmetic
 * children point only backward, so a cycle or out-of-range index cannot reach
 * the lowerer.  ADD/SUB's immediate Form lowering needs the 12-bit bound. */
static int fk_arm64_u32_program(long long program, fk_arm64_u32_node nodes[FK_ARM64_U32_NODE_CAP],
                              long long *count) {
    long long cursor = program;
    long long n = 0;
    while (cursor != 1) {
        long long row;
        long long fields[3];
        long long width;
        unsigned int tag;
        unsigned int a;
        unsigned int b;
        if (n >= FK_ARM64_U32_NODE_CAP || !fk_arm64_u32_cons(cursor, &row, &cursor) ||
            !fk_arm64_u32_row(row, fields, &width) || width < 1 ||
            !fk_arm64_u32_value(fields[0], &tag)) {
            return 0;
        }
        if (tag == 1U) {
            if (width != 2 || !fk_arm64_u32_value(fields[1], &a) || a > 65535U) {
                return 0;
            }
            nodes[n].tag = tag;
            nodes[n].a = a;
            nodes[n].b = 0;
        } else if (tag == 2U) {
            if (width != 1) {
                return 0;
            }
            nodes[n].tag = tag;
            nodes[n].a = 0;
            nodes[n].b = 0;
        } else if (tag == 3U || tag == 4U || tag == 5U) {
            if (width != 3 || !fk_arm64_u32_value(fields[1], &a) ||
                !fk_arm64_u32_value(fields[2], &b) || a >= (unsigned int)n ||
                b >= (unsigned int)n) {
                return 0;
            }
            if ((tag == 3U || tag == 4U) && nodes[b].tag == 1U && nodes[b].a > 4095U) {
                return 0;
            }
            nodes[n].tag = tag;
            nodes[n].a = a;
            nodes[n].b = b;
        } else {
            return 0;
        }
        n = n + 1;
    }
    if (n < 1) {
        return 0;
    }
    *count = n;
    return 1;
}
static int fk_arm64_u32_word_ok(unsigned int word, long long pos, long long words) {
    unsigned int rd = word & 31U;
    unsigned int rn = (word >> 5) & 31U;
    unsigned int rm = (word >> 16) & 31U;
    if (pos == 0) {
        return word == 0x2A0003E1U; /* mov w1,w0 */
    }
    if (pos + 1 == words) {
        return word == 0xD65F03C0U; /* ret */
    }
    if ((word & 0xFFE00000U) == 0x52800000U) { /* movz w<rd>,#imm16 */
        return rd == 0U || rd == 2U;
    }
    if ((word & 0xFFC00000U) == 0x11000000U || /* add w<rd>,w<rn>,#imm12 */
        (word & 0xFFC00000U) == 0x51000000U) { /* sub w<rd>,w<rn>,#imm12 */
        return rd == 0U && (rn == 0U || rn == 1U);
    }
    if ((word & 0xFFE0FFE0U) == 0x2A0003E0U) { /* mov w<rd>,w<rm> */
        return (rd == 0U && rm == 1U) || (fk_arm64_u32_tmp(rd) && rm == 0U);
    }
    if ((word & 0xFFE0FC00U) == 0x0B000000U || /* add w<rd>,w<rn>,w<rm> */
        (word & 0xFFE0FC00U) == 0x4B000000U) { /* sub w<rd>,w<rn>,w<rm> */
        return rd == 0U && rm == 0U && (rn == 1U || fk_arm64_u32_tmp(rn));
    }
    if ((word & 0xFFE0FC00U) == 0x1B007C00U) { /* mul w<rd>,w<rn>,w<rm> */
        return rd == 0U &&
               ((rn == 0U && (rm == 1U || rm == 2U)) ||
                (fk_arm64_u32_tmp(rn) && rm == 0U));
    }
    return 0;
}
static int fk_arm64_u32_put(unsigned int words[FK_ARM64_U32_WORD_CAP], long long *n,
                          unsigned int word) {
    if (*n >= FK_ARM64_U32_WORD_CAP) {
        return 0;
    }
    words[*n] = word;
    *n = *n + 1;
    return 1;
}
/* This is the exact V1 shape of lo-node/lo-tree for tag 1/2/3/4/5.  It takes
 * no code bytes from Form: only already-admitted structural nodes. */
static int fk_arm64_u32_emit_tree(const fk_arm64_u32_node nodes[FK_ARM64_U32_NODE_CAP],
                                unsigned int index, unsigned int depth,
                                unsigned int words[FK_ARM64_U32_WORD_CAP], long long *n);
static int fk_arm64_u32_emit_node(const fk_arm64_u32_node nodes[FK_ARM64_U32_NODE_CAP],
                                unsigned int index, unsigned int depth,
                                unsigned int words[FK_ARM64_U32_WORD_CAP], long long *n) {
    const fk_arm64_u32_node *node = &nodes[index];
    if (node->tag == 1U) {
        return fk_arm64_u32_put(words, n, 0x52800000U | (node->a << 5));
    }
    if (node->tag == 2U) {
        return fk_arm64_u32_put(words, n, 0x2A0003E0U | (1U << 16));
    }
    if (node->tag == 3U) {
        if (nodes[node->b].tag == 1U) {
            return fk_arm64_u32_emit_node(nodes, node->a, depth, words, n) &&
                   fk_arm64_u32_put(words, n, 0x11000000U | (nodes[node->b].a << 10));
        }
        if (nodes[node->a].tag == 2U) {
            return fk_arm64_u32_emit_node(nodes, node->b, depth, words, n) &&
                   fk_arm64_u32_put(words, n, 0x0B000000U | (1U << 5));
        }
        return fk_arm64_u32_emit_tree(nodes, index, depth, words, n);
    }
    if (node->tag == 4U) {
        if (nodes[node->b].tag == 1U) {
            if (nodes[node->a].tag == 2U) {
                return fk_arm64_u32_put(words, n,
                                      0x51000000U | (nodes[node->b].a << 10) | (1U << 5));
            }
            return fk_arm64_u32_emit_node(nodes, node->a, depth, words, n) &&
                   fk_arm64_u32_put(words, n, 0x51000000U | (nodes[node->b].a << 10));
        }
        return fk_arm64_u32_emit_tree(nodes, index, depth, words, n);
    }
    if (node->tag == 5U) {
        if (nodes[node->a].tag == 2U) {
            return fk_arm64_u32_emit_node(nodes, node->b, depth, words, n) &&
                   fk_arm64_u32_put(words, n, 0x1B007C00U | (1U << 16));
        }
        if (nodes[node->b].tag == 2U) {
            return fk_arm64_u32_emit_node(nodes, node->a, depth, words, n) &&
                   fk_arm64_u32_put(words, n, 0x1B007C00U | (1U << 16));
        }
        if (nodes[node->b].tag == 1U) {
            return fk_arm64_u32_emit_node(nodes, node->a, depth, words, n) &&
                   fk_arm64_u32_put(words, n, 0x52800000U | (nodes[node->b].a << 5) | 2U) &&
                   fk_arm64_u32_put(words, n, 0x1B007C00U | (2U << 16));
        }
        if (nodes[node->a].tag == 1U) {
            return fk_arm64_u32_emit_node(nodes, node->b, depth, words, n) &&
                   fk_arm64_u32_put(words, n, 0x52800000U | (nodes[node->a].a << 5) | 2U) &&
                   fk_arm64_u32_put(words, n, 0x1B007C00U | (2U << 16));
        }
        return fk_arm64_u32_emit_tree(nodes, index, depth, words, n);
    }
    return 0;
}
static int fk_arm64_u32_emit_tree(const fk_arm64_u32_node nodes[FK_ARM64_U32_NODE_CAP],
                                unsigned int index, unsigned int depth,
                                unsigned int words[FK_ARM64_U32_WORD_CAP], long long *n) {
    const fk_arm64_u32_node *node = &nodes[index];
    unsigned int temp;
    unsigned int base;
    if (node->tag == 1U || node->tag == 2U) {
        return fk_arm64_u32_emit_node(nodes, index, depth, words, n);
    }
    if (depth > 6U) {
        return 0;
    }
    temp = 9U + depth;
    if (!fk_arm64_u32_emit_tree(nodes, node->a, depth, words, n) ||
        !fk_arm64_u32_put(words, n, 0x2A0003E0U | temp) ||
        !fk_arm64_u32_emit_tree(nodes, node->b, depth + 1U, words, n)) {
        return 0;
    }
    base = node->tag == 3U ? 0x0B000000U :
           (node->tag == 4U ? 0x4B000000U : 0x1B007C00U);
    return fk_arm64_u32_put(words, n, base | (temp << 5));
}
/* CRYSTALLIZE-ON-HEAT, the carrier's half. The door below emitted an image,
 * mmap'd a fresh page, ran it, and munmap'd it again — ON EVERY CALL. Measured
 * on an M4 Max over 20,000 calls of one four-node recipe: 62 ms through the
 * door against 1 ms for the same recipe walked, i.e. ~3.1 us per call, nearly
 * all of it the mmap/munmap pair. A JIT that recompiles at every call is a
 * slower interpreter with extra steps.
 *
 * So the carrier keeps what it built. The key is the PARSED PROGRAM — every
 * node's (tag,a,b), its count, and the root — not the Form value that carried
 * it: a value is an index into an arena, and keying on an index would tie the
 * cache's correctness to the arena never reusing one. Parsing is the cheap half
 * (a walk of a short cons list); the page is the dear half, and the page is
 * what is kept. Structural keying also means two callers that build the same
 * recipe separately share one crystallization, which is what content-addressing
 * has meant everywhere else in this body.
 *
 * Eviction is round-robin and unmaps what it replaces, so the page count is
 * bounded by FK_ARM64_U32_CACHE and nothing leaks. No heat counter yet: this
 * door crystallizes on FIRST call, which is the right default while the emitter
 * covers only leaves that are cheap to emit. Heat belongs here when the emitter
 * grows expensive enough that compiling a cold recipe can lose. */
/* The structural cache below is exact but not free: it re-parses the program's
 * cons list to build the key, and for a nine-node recipe that parse costs about
 * what walking the recipe costs — measured 18 ms against the walker's 15 ms over
 * 400,000 calls, so the door was correct, 76x better than uncompiled, and still
 * losing. The hint closes that: the SAME program value asking again inside the
 * same heap generation is three integer compares and a call. It is a shortcut,
 * never an authority — a miss simply falls through to the parse. */
/* Bumped once per compaction (fk_melt). A cons value is an index into an arena
 * the collector MOVES, so the value-hint below is valid only inside the
 * generation that set it. One counter is the whole guard. */
static long long fk_melt_gen = 0;
static long long fk_arm64_hint_prog = 0;
static long long fk_arm64_hint_root = 0;
static long long fk_arm64_hint_gen = -1;
static void *fk_arm64_hint_mem = 0;

#define FK_ARM64_U32_CACHE 32
typedef struct {
    int live;
    long long nodes_n;
    unsigned int root;
    fk_arm64_u32_node nodes[FK_ARM64_U32_NODE_CAP];
    void *mem;
    long long span;
} fk_arm64_u32_entry;
static fk_arm64_u32_entry fk_arm64_u32_cache[FK_ARM64_U32_CACHE];
static long long fk_arm64_u32_cursor = 0;

static int fk_arm64_u32_same(const fk_arm64_u32_entry *e, const fk_arm64_u32_node *nodes,
                             long long nodes_n, unsigned int root) {
    long long i;
    if (!e->live || e->nodes_n != nodes_n || e->root != root) {
        return 0;
    }
    for (i = 0; i < nodes_n; i = i + 1) {
        if (e->nodes[i].tag != nodes[i].tag || e->nodes[i].a != nodes[i].a ||
            e->nodes[i].b != nodes[i].b) {
            return 0;
        }
    }
    return 1;
}

static void *fk_arm64_u32_keep(const fk_arm64_u32_node *nodes, long long nodes_n,
                               unsigned int root, void *mem, long long span) {
    fk_arm64_u32_entry *e = &fk_arm64_u32_cache[fk_arm64_u32_cursor];
    long long i;
    if (e->live && e->mem != 0) {
        if (fk_arm64_hint_mem == e->mem) {
            fk_arm64_hint_mem = 0;
        }
        munmap(e->mem, (size_t)e->span);
    }
    e->live = 1;
    e->nodes_n = nodes_n;
    e->root = root;
    for (i = 0; i < nodes_n; i = i + 1) {
        e->nodes[i] = nodes[i];
    }
    e->mem = mem;
    e->span = span;
    fk_arm64_u32_cursor = (fk_arm64_u32_cursor + 1) % FK_ARM64_U32_CACHE;
    return mem;
}

/* THE FORM LANE'S EXECUTOR. form-lower.fk emits an arm64 leaf image for the
 * AAPCS64 shape `int64 f(int64)` — the emitter is Form, four-way proven, and
 * carries no 32-bit ceiling. The Go sibling has been able to RUN those bytes
 * since jit_inram_darwin_arm64.go; fkwu could not, so the body could compile
 * its own native code and had nowhere to put it. This is that door, with the
 * same contract Go states: (image, arg) -> int64, image a list of bytes.
 *
 * It keeps its page, for the reason the u32 door learned: the Go one mmaps and
 * unmaps per call, and a compiler that forgets is a slower interpreter. The key
 * is the image BYTES — content, not the cons value the collector moves — so two
 * callers that lower the same recipe share one crystallization. */
#define FK_INRAM_CODE_CAP 4096
#define FK_INRAM_CACHE 32
typedef struct {
    int live;
    long long n;
    unsigned char code[FK_INRAM_CODE_CAP];
    void *mem;
    unsigned int generation;
    unsigned int residents;
} fk_inram_entry;
static fk_inram_entry fk_inram_cache[FK_INRAM_CACHE];
static long long fk_inram_cursor = 0;
/* A NodeID is already an interned value-node handle.  Its node-table index is
 * therefore the cheapest resident key the carrier can receive: no SHA, byte
 * fold, image reconstruction or table scan belongs on the hot call.  The
 * generation pair makes slot eviction O(1) too -- stale aliases simply stop
 * matching rather than requiring a sweep over the node table. */
static unsigned char *fk_inram_node_slot;
static unsigned int *fk_inram_node_generation;
static unsigned char *fk_inram_node_released;
static long long fk_inram_last_slot = -1;

static long long fk_inram_bytes(long long image, unsigned char *out, long long cap) {
    long long cursor = image;
    long long n = 0;
    while (cursor != 1) {
        long long head;
        long long rest;
        if (n >= cap || !fk_arm64_u32_cons(cursor, &head, &rest) || (head & 1) != 0) {
            return -1;
        }
        {
            long long b = head >> 1;
            if (b < 0 || b > 255) {
                return -1;
            }
            out[n] = (unsigned char)b;
        }
        n = n + 1;
        cursor = rest;
    }
    return n;
}

/* The door's argument contract, extended 2026-09-02 (the multi-arg
 * increment the per-recipe-JIT program named) and again 2026-09-03 (R28,
 * the runtime-string increment receipts/2026-09-03-string-family-lowering.md
 * named as its next stone): an EVEN word is ONE integer argument — the
 * standing contract, behavior unchanged; a STRING value is TWO argument
 * SLOTS — base pointer then byte length, fk_srange's own (pointer, length)
 * shape, the same accessor str_byte_at/str_eq/str_find already read
 * through — handed to the page as two AAPCS64 registers, the convention
 * lo-strfind-runtime (form-lower.fk) banks from; a cons list of up to
 * EIGHT SLOTS (an int spending one, a string spending two) is handed to
 * the page in x0..x7, generalizing the existing multi-arg contract rather
 * than replacing it. Returns the slot count (1..8); 0 declines the shape
 * (nil, more than eight slots, or an element that is neither an integer
 * nor a string), and the door answers nothing.
 *
 * POINTER SAFETY, GROUNDED NOT ASSUMED: the pointer handed out is fk_sb's
 * own live base plus offset (fk_srange never copies), so it is only as
 * durable as fk_sb's address. fk_sb moves on realloc, and every realloc
 * site is a string being INTERNED (fk_sintern growing fk_sbp past
 * fk_scap_b) — grep confirms every fk_sb assignment in this file is one of
 * exactly two shapes: the one-time fk_sinit malloc, or a growth realloc
 * beside an intern. Between this call returning and fk_inram_call using
 * the pointer, the only code that runs is image-byte decode (integer/cons
 * walking, no strings), the resident-cache scan (byte compare over
 * fk_inram_cache, not fk_sb), and — on a cold image — mmap plus a raw byte
 * copy into the new executable page: none of that interns a string. Then
 * the crystallized leaf itself runs, and it cannot call back into this
 * interpreter at all (form-lower.fk emits pure ALU/load/branch bytes for
 * every leaf it builds today, no `bl` to a C native), so it cannot trigger
 * fk_sintern either. The pointer is therefore live for the one call it is
 * handed to. NAMED, NOT PAPERED OVER: this holds only while every
 * crystallized leaf stays call-out-free. A future leaf shape that DOES
 * call out mid-body must not carry a raw string pointer across that call
 * — it would need to re-derive the pointer afterward, since fk_sb can have
 * moved underneath it by then. */
static long long fk_inram_args(long long arg_value, long long *a) {
    long long i;
    const char *sptr;
    long long slen;
    for (i = 0; i < 8; i = i + 1) {
        a[i] = 0;
    }
    if ((arg_value & 1) == 0) {
        a[0] = arg_value >> 1;
        return 1;
    }
    if (fk_srange(arg_value, &sptr, &slen)) {
        a[0] = (long long)sptr;
        a[1] = slen;
        return 2;
    }
    {
        long long n = 0;
        long long cell = arg_value;
        long long h;
        long long t;
        while (fk_arm64_u32_cons(cell, &h, &t)) {
            if ((h & 1) == 0) {
                if (n >= 8) {
                    return 0;
                }
                a[n] = h >> 1;
                n = n + 1;
            } else if (fk_srange(h, &sptr, &slen)) {
                if (n + 2 > 8) {
                    return 0;
                }
                a[n] = (long long)sptr;
                a[n + 1] = slen;
                n = n + 2;
            } else {
                return 0;
            }
            cell = t;
        }
        if (cell != 1 || n == 0) {
            return 0;
        }
        return n;
    }
}
/* ONE call seam for every executable page. Emitted code banks into
 * callee-saved registers (x19..), and lo-compile-fn-n now saves and
 * restores them — the asm fence below is the belt to those suspenders:
 * it declares x19..x28 clobbered so the compiler never keeps a live
 * value there across the page call, even against an image lowered
 * before the prologue landed. Old single-arg images read only w0/w1;
 * handing eight registers to a page that reads fewer is AAPCS64-clean. */
static long long fk_inram_call(void *mem, long long *a) {
    fk_inram_call_total = fk_inram_call_total + 1;
    long long (*fn)(long long, long long, long long, long long, long long,
                    long long, long long, long long) =
        (long long (*)(long long, long long, long long, long long, long long,
                       long long, long long, long long))mem;
    long long r = fn(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
    __asm__ volatile(""
                     : "+r"(r)
                     :
                     : "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26",
                       "x27", "x28");
    return r;
}
static long long fk_jit_leaf_inram_image(long long image, long long arg_value) {
    unsigned char code[FK_INRAM_CODE_CAP];
    long long n;
    long long i;
    long long args[8];
    void *mem = 0;
    fk_inram_last_slot = -1;
    if (fk_inram_args(arg_value, args) == 0) {
        return fk_nothing;
    }
    n = fk_inram_bytes(image, code, FK_INRAM_CODE_CAP);
    if (n <= 0 || (n % 4) != 0) {
        return fk_nothing;
    }
    for (i = 0; i < FK_INRAM_CACHE; i = i + 1) {
        if (fk_inram_cache[i].live && fk_inram_cache[i].n == n) {
            long long j = 0;
            while (j < n && fk_inram_cache[i].code[j] == code[j]) {
                j = j + 1;
            }
            if (j == n) {
                fk_inram_last_slot = i;
                return (fk_inram_call(fk_inram_cache[i].mem, args)) << 1;
            }
        }
    }
    mem = mmap(0, FK_INRAM_CODE_CAP, 0x7, 0x1802, -1, 0);
    if (mem == (void *)-1) {
        return fk_nothing;
    }
    pthread_jit_write_protect_np(0);
    for (i = 0; i < n; i = i + 1) {
        ((unsigned char *)mem)[i] = code[i];
    }
    pthread_jit_write_protect_np(1);
    __builtin___clear_cache((char *)mem, (char *)mem + n);
    {
        fk_inram_entry *e = &fk_inram_cache[fk_inram_cursor];
        if (e->live && e->mem != 0) {
            munmap(e->mem, FK_INRAM_CODE_CAP);
        }
        e->generation = e->generation + 1;
        if (e->generation == 0) {
            e->generation = 1;
        }
        e->live = 1;
        e->residents = 0;
        e->n = n;
        for (i = 0; i < n; i = i + 1) {
            e->code[i] = code[i];
        }
        e->mem = mem;
        fk_inram_last_slot = fk_inram_cursor;
        fk_inram_cursor = (fk_inram_cursor + 1) % FK_INRAM_CACHE;
    }
    return (fk_inram_call(mem, args)) << 1;
}

/* The existing two-argument door also accepts a Form-native resident request:
 *
 *   [0, structural-nodeid, image]  birth if unseen, then invoke
 *   [1, structural-nodeid, []]     dissolve, answering 1/0/nothing
 *
 * A raw byte list keeps the legacy behavior above.  This avoids minting three
 * new fixed op-table seats just to express lifecycle around the same carrier.
 * Birth is the cold trust membrane and may walk bytes.  Invocation reaches the
 * executable page by the interned NodeID's stable value-node index in O(1).
 * That index is SESSION-EPHEMERAL: persistence carries program/meaning data and
 * interns it again after restart; it never stores this process-local index.
 *
 * A byte image cannot be mistaken for this envelope even when its first byte
 * is 0 or 1: field two is required to be a negative interned node, while every
 * admitted raw image field is a nonnegative byte. Release tombstones the index
 * for this session. The same dead meaning cannot silently rebirth; a changed
 * meaning earns a changed NodeID at the Form membrane.
 */
static int fk_inram_resident_request(long long request, long long *action,
                                      long long *identity, long long *image) {
    long long h0;
    long long h1;
    long long h2;
    long long rest;
    if (!fk_arm64_u32_cons(request, &h0, &rest) || (h0 & 1) != 0 ||
        !fk_arm64_u32_cons(rest, &h1, &rest) || h1 >= 0 ||
        !fk_arm64_u32_cons(rest, &h2, &rest) || rest != 1) {
        return 0;
    }
    *action = h0 >> 1;
    *identity = h1;
    *image = h2;
    return 1;
}

static int fk_inram_node_index(long long identity, long long *index) {
    long long ix;
    if (identity >= 0) {
        return 0;
    }
    ix = fk_nidx(identity);
    if (ix < 1 || ix > fk_np || ix >= fk_node_cap) {
        return 0;
    }
    *index = ix;
    return 1;
}

static long long fk_jit_leaf_inram_resident(long long action, long long identity,
                                             long long image, long long arg_value) {
    long long ix;
    unsigned int encoded_slot;
    fk_inram_entry *e;
    if (!fk_inram_node_index(identity, &ix)) {
        return fk_nothing;
    }
    encoded_slot = fk_inram_node_slot[ix];
    if (action == 1) {
        if (encoded_slot == 0) {
            if (fk_inram_node_released[ix]) {
                return 0;
            }
            return 0;
        }
        e = &fk_inram_cache[encoded_slot - 1];
        if (!e->live || fk_inram_node_generation[ix] != e->generation) {
            fk_inram_node_slot[ix] = 0;
            return 0;
        }
        fk_inram_node_slot[ix] = 0;
        fk_inram_node_released[ix] = 1;
        if (e->residents > 0) {
            e->residents = e->residents - 1;
        }
        if (e->residents == 0) {
            if (e->mem != 0) {
                munmap(e->mem, FK_INRAM_CODE_CAP);
            }
            e->mem = 0;
            e->live = 0;
            e->generation = e->generation + 1;
            if (e->generation == 0) {
                e->generation = 1;
            }
        }
        return 2;
    }
    if (action != 0) {
        return fk_nothing;
    }
    {
        long long ra[8];
        if (fk_inram_args(arg_value, ra) == 0) {
            return fk_nothing;
        }
        if (fk_inram_node_released[ix]) {
            return fk_nothing;
        }
        if (encoded_slot != 0) {
            e = &fk_inram_cache[encoded_slot - 1];
            if (e->live && fk_inram_node_generation[ix] == e->generation) {
                return (fk_inram_call(e->mem, ra)) << 1;
            }
            fk_inram_node_slot[ix] = 0;
        }
    }
    {
        long long answer = fk_jit_leaf_inram_image(image, arg_value);
        if (fk_inram_last_slot < 0 || fk_is_nothing(answer)) {
            return answer;
        }
        e = &fk_inram_cache[fk_inram_last_slot];
        fk_inram_node_slot[ix] = (unsigned char)(fk_inram_last_slot + 1);
        fk_inram_node_generation[ix] = e->generation;
        e->residents = e->residents + 1;
        return answer;
    }
}

static long long fk_jit_leaf_inram(long long request_or_image, long long arg_value) {
    long long action;
    long long identity;
    long long image;
    if (fk_inram_resident_request(request_or_image, &action, &identity, &image)) {
        return fk_jit_leaf_inram_resident(action, identity, image, arg_value);
    }
    return fk_jit_leaf_inram_image(request_or_image, arg_value);
}

static long long fk_native_call_arm64_u32_leaf(long long program, long long root_value,
                                                long long arg_value) {
    fk_arm64_u32_node nodes[FK_ARM64_U32_NODE_CAP];
    unsigned int image[FK_ARM64_U32_WORD_CAP];
    unsigned char bytes[FK_ARM64_U32_WORD_CAP * 4];
    long long nodes_n;
    long long words = 0;
    unsigned int root;
    unsigned int arg;
    long long i;
    if (fk_arm64_hint_mem != 0 && program == fk_arm64_hint_prog &&
        root_value == fk_arm64_hint_root && fk_melt_gen == fk_arm64_hint_gen) {
        unsigned int hot_arg;
        if (fk_arm64_u32_value(arg_value, &hot_arg)) {
            unsigned int (*hot)(unsigned int) =
                (unsigned int (*)(unsigned int))fk_arm64_hint_mem;
            return ((long long)hot(hot_arg)) << 1;
        }
    }
    if (!fk_arm64_u32_program(program, nodes, &nodes_n) ||
        !fk_arm64_u32_value(root_value, &root) || root >= (unsigned int)nodes_n ||
        !fk_arm64_u32_value(arg_value, &arg)) {
        return fk_nothing;
    }
    for (i = 0; i < FK_ARM64_U32_CACHE; i = i + 1) {
        if (fk_arm64_u32_same(&fk_arm64_u32_cache[i], nodes, nodes_n, root)) {
            unsigned int (*hot)(unsigned int) =
                (unsigned int (*)(unsigned int))fk_arm64_u32_cache[i].mem;
            fk_arm64_hint_prog = program;
            fk_arm64_hint_root = root_value;
            fk_arm64_hint_gen = fk_melt_gen;
            fk_arm64_hint_mem = fk_arm64_u32_cache[i].mem;
            return ((long long)hot(arg)) << 1;
        }
    }
    if (!fk_arm64_u32_put(image, &words, 0x2A0003E1U) ||
        !fk_arm64_u32_emit_node(nodes, root, 0U, image, &words) ||
        !fk_arm64_u32_put(image, &words, 0xD65F03C0U)) {
        return fk_nothing;
    }
    i = 0;
    while (i < words) {
        unsigned int word = image[i];
        if (!fk_arm64_u32_word_ok(word, i, words)) {
            return fk_nothing;
        }
        bytes[i * 4] = (unsigned char)word;
        bytes[i * 4 + 1] = (unsigned char)(word >> 8);
        bytes[i * 4 + 2] = (unsigned char)(word >> 16);
        bytes[i * 4 + 3] = (unsigned char)(word >> 24);
        i = i + 1;
    }
    {
        /* Darwin: PROT_READ|WRITE|EXEC, MAP_PRIVATE|ANON|JIT.  This seed keeps
         * platform ABI declarations local rather than importing system headers. */
        void *mem = mmap(0, 4096, 0x7, 0x1802, -1, 0);
        if (mem == (void *)-1) {
            return fk_nothing;
        }
        pthread_jit_write_protect_np(0);
        i = 0;
        while (i < words * 4) {
            ((unsigned char *)mem)[i] = bytes[i];
            i = i + 1;
        }
        pthread_jit_write_protect_np(1);
        __builtin___clear_cache((char *)mem, (char *)mem + words * 4);
        {
            unsigned int (*fn)(unsigned int) =
                (unsigned int (*)(unsigned int))fk_arm64_u32_keep(nodes, nodes_n, root, mem, 4096);
            fk_arm64_hint_prog = program;
            fk_arm64_hint_root = root_value;
            fk_arm64_hint_gen = fk_melt_gen;
            fk_arm64_hint_mem = mem;
            return ((long long)fn(arg)) << 1;
        }
    }
}
#else
static long long fk_native_call_arm64_u32_leaf(long long program, long long root, long long arg) {
    (void)program;
    (void)root;
    (void)arg;
    return fk_nothing;
}
#endif
/* ── the value-node table grows; there is no node wall ─────────────────────
 * Handles are INDICES into the column arrays, so doubling the columns keeps
 * every existing handle valid -- nothing relocates, the same property the
 * intern index and the melt already stand on. The cons-pair heap (fk_melt)
 * had growth first; the node table now matches its discipline. New column
 * bytes are zeroed to keep the BSS-era meanings (memo 0 = unhashed, sattr 0
 * = unattributed, inram slot 0 = unresident). The intern index is held at 4x
 * the node cap: when it must grow it is rebuilt from fk_nhash_memo -- every
 * indexed node's memo IS its exact probe key (the doors store it at mint),
 * and nodes outside the index that carry deep-hash memos re-enter as dead
 * weight the doors' predicates skip, load staying <= 25%. Callers grow
 * BEFORE computing a probe slot, never after (a slot chosen under the old
 * mask would land wrong in the rebuilt table). OOM here dies loudly -- the
 * same honest door as fk_melt's arena. */
static void *fk_nodes_grow_col(void *p, long long old_n, long long new_n, long long width) {
    char *q = realloc(p, (unsigned long)(new_n * width));
    long long i;
    if (q == 0) {
        fk_die("fk_nodes_grow: out of memory growing the value-node table");
    }
    i = old_n * width;
    while (i < new_n * width) {
        q[i] = 0;
        i = i + 1;
    }
    return q;
}
static void fk_nodes_grow(void) {
    if (fk_field_on) {
        long long oc = fk_node_cap;
        long long nc = oc * 2;
        fk_fbroots = (long long *)fk_nodes_grow_col(fk_fbroots, oc, nc, 8);
#if defined(FK_HAVE_DARWIN_ARM64_JIT_WITNESS)
        fk_inram_node_slot = (unsigned char *)fk_nodes_grow_col(fk_inram_node_slot, oc, nc, 1);
        fk_inram_node_generation = (unsigned int *)fk_nodes_grow_col(fk_inram_node_generation, oc, nc, 4);
        fk_inram_node_released = (unsigned char *)fk_nodes_grow_col(fk_inram_node_released, oc, nc, 1);
#endif
        fk_node_cap = nc;
        fk_node_grows = fk_node_grows + 1;
        return;
    }
    long long oc = fk_node_cap;
    long long nc = oc * 2;
    if (fk_store_shared && nc > FK_STORE_NODE_CELLS) { fk_store_go_private(); }
    fk_nkind = (long long *)fk_store_grow('k', fk_nkind, oc * 8, nc * 8, FK_STORE_NODE_CELLS * 8, 1);
    fk_ncat = (long long *)fk_store_grow('c', fk_ncat, oc * 8, nc * 8, FK_STORE_NODE_CELLS * 8, 1);
    fk_nkids = (long long *)fk_store_grow('i', fk_nkids, oc * 8, nc * 8, FK_STORE_NODE_CELLS * 8, 1);
    fk_nval = (long long *)fk_store_grow('v', fk_nval, oc * 8, nc * 8, FK_STORE_NODE_CELLS * 8, 1);
    fk_nid = (long long (*)[4])fk_store_grow('n', fk_nid, oc * 32, nc * 32, FK_STORE_NODE_CELLS * 32, 1);
    fk_nhash_memo = (long long *)fk_nodes_grow_col(fk_nhash_memo, oc, nc, 8);
    fk_nsfile = (long long *)fk_store_grow('f', fk_nsfile, oc * 8, nc * 8, FK_STORE_NODE_CELLS * 8, 1);
    fk_nsline = (long long *)fk_store_grow('l', fk_nsline, oc * 8, nc * 8, FK_STORE_NODE_CELLS * 8, 1);
    fk_nscol = (long long *)fk_store_grow('o', fk_nscol, oc * 8, nc * 8, FK_STORE_NODE_CELLS * 8, 1);
    fk_nsattr = (long long *)fk_store_grow('a', fk_nsattr, oc * 8, nc * 8, FK_STORE_NODE_CELLS * 8, 1);
    fk_fbroots = (long long *)fk_nodes_grow_col(fk_fbroots, oc, nc, 8);
#if defined(FK_HAVE_DARWIN_ARM64_JIT_WITNESS)
    fk_inram_node_slot = (unsigned char *)fk_nodes_grow_col(fk_inram_node_slot, oc, nc, 1);
    fk_inram_node_generation = (unsigned int *)fk_nodes_grow_col(fk_inram_node_generation, oc, nc, 4);
    fk_inram_node_released = (unsigned char *)fk_nodes_grow_col(fk_inram_node_released, oc, nc, 1);
#endif
    fk_node_cap = nc;
    fk_node_grows = fk_node_grows + 1;
    if (fk_intern_hash_cap < nc * 4) {
        long long hc = fk_intern_hash_cap * 2;
        long long ix;
        free(fk_intern_tab);
        fk_intern_tab = (long long *)calloc((unsigned long)hc, 8);
        if (fk_intern_tab == 0) {
            fk_die("fk_nodes_grow: out of memory growing the intern index");
        }
        ix = 1;
        while (ix <= fk_np) {
            long long h = fk_nhash_memo[ix];
            if (h) {
                long long slot = h & (hc - 1);
                while (fk_intern_tab[slot]) {
                    slot = (slot + 1) & (hc - 1);
                }
                fk_intern_tab[slot] = ix;
            }
            ix = ix + 1;
        }
        fk_intern_hash_cap = hc;
    }
}
static void fk_ast_reserve(long long need);
static void fk_srctext_reserve(long long need);
static void fk_src_root_reserve(long long need);
static void fk_nodes_init(void) {
    if (fk_node_cap) {
        return;
    }
    fk_live_open();
    fk_ast_reserve(1);
    fk_srctext_reserve(1);
    fk_src_root_reserve(1);
    fk_vs_grow(FK_VALUE_STACK_CAP_INIT);
    fk_mem_reserve(FK_MEM_CELL_CAP_INIT);
    if (fk_field_open()) {
        fk_fbroots = (long long *)calloc(FK_NODE_CAP_INIT, 8);
        fk_intern_tab = (long long *)calloc(FK_INTERN_HASH_CAP_INIT, 8);
#if defined(FK_HAVE_DARWIN_ARM64_JIT_WITNESS)
        fk_inram_node_slot = (unsigned char *)calloc(FK_NODE_CAP_INIT, 1);
        fk_inram_node_generation = (unsigned int *)calloc(FK_NODE_CAP_INIT, 4);
        fk_inram_node_released = (unsigned char *)calloc(FK_NODE_CAP_INIT, 1);
#endif
        if (fk_fbroots == 0 || fk_intern_tab == 0) { fk_die("fk_nodes_init: out of memory for the private node columns"); }
        fk_node_cap = FK_NODE_CAP_INIT;
        fk_intern_hash_cap = FK_INTERN_HASH_CAP_INIT;
        return;
    }
    fk_nkind = (long long *)fk_store_take('k', FK_STORE_NODE_CELLS * 8);
    fk_ncat = fk_nkind == 0 ? 0 : (long long *)fk_store_take('c', FK_STORE_NODE_CELLS * 8);
    fk_nkids = fk_ncat == 0 ? 0 : (long long *)fk_store_take('i', FK_STORE_NODE_CELLS * 8);
    fk_nval = fk_nkids == 0 ? 0 : (long long *)fk_store_take('v', FK_STORE_NODE_CELLS * 8);
    fk_nid = fk_nval == 0 ? 0 : (long long (*)[4])fk_store_take('n', FK_STORE_NODE_CELLS * 32);
    fk_nsfile = fk_nid == 0 ? 0 : (long long *)fk_store_take('f', FK_STORE_NODE_CELLS * 8);
    fk_nsline = fk_nsfile == 0 ? 0 : (long long *)fk_store_take('l', FK_STORE_NODE_CELLS * 8);
    fk_nscol = fk_nsline == 0 ? 0 : (long long *)fk_store_take('o', FK_STORE_NODE_CELLS * 8);
    fk_nsattr = fk_nscol == 0 ? 0 : (long long *)fk_store_take('a', FK_STORE_NODE_CELLS * 8);
    if (fk_nsattr == 0) {
        /* no shared memory here: the tables are private, as they always were */
        fk_store_shared = 0;
        fk_store_unlink_pid((long long)getpid());
        fk_nkind = (long long *)calloc(FK_NODE_CAP_INIT, 8);
        fk_ncat = (long long *)calloc(FK_NODE_CAP_INIT, 8);
        fk_nkids = (long long *)calloc(FK_NODE_CAP_INIT, 8);
        fk_nval = (long long *)calloc(FK_NODE_CAP_INIT, 8);
        fk_nid = (long long (*)[4])calloc(FK_NODE_CAP_INIT, 32);
        fk_nsfile = (long long *)calloc(FK_NODE_CAP_INIT, 8);
        fk_nsline = (long long *)calloc(FK_NODE_CAP_INIT, 8);
        fk_nscol = (long long *)calloc(FK_NODE_CAP_INIT, 8);
        fk_nsattr = (long long *)calloc(FK_NODE_CAP_INIT, 8);
    }
    fk_nhash_memo = (long long *)calloc(FK_NODE_CAP_INIT, 8);
    fk_fbroots = (long long *)calloc(FK_NODE_CAP_INIT, 8);
    fk_intern_tab = (long long *)calloc(FK_INTERN_HASH_CAP_INIT, 8);
    if (fk_nkind == 0 || fk_ncat == 0 || fk_nkids == 0 || fk_nval == 0 ||
        fk_nid == 0 || fk_nhash_memo == 0 || fk_nsfile == 0 || fk_nsline == 0 ||
        fk_nscol == 0 || fk_nsattr == 0 || fk_fbroots == 0 || fk_intern_tab == 0) {
        fk_die("fk_nodes_init: out of memory for the value-node table");
    }
#if defined(FK_HAVE_DARWIN_ARM64_JIT_WITNESS)
    fk_inram_node_slot = (unsigned char *)calloc(FK_NODE_CAP_INIT, 1);
    fk_inram_node_generation = (unsigned int *)calloc(FK_NODE_CAP_INIT, 4);
    fk_inram_node_released = (unsigned char *)calloc(FK_NODE_CAP_INIT, 1);
    if (fk_inram_node_slot == 0 || fk_inram_node_generation == 0 ||
        fk_inram_node_released == 0) {
        fk_die("fk_nodes_init: out of memory for the value-node table");
    }
#endif
    fk_node_cap = FK_NODE_CAP_INIT;
    fk_intern_hash_cap = FK_INTERN_HASH_CAP_INIT;
}
/* ── stone 3 (OBSERVE): the offer/ack observe hook ────────────────────────── Every reducer CALL is
 * an OFFER (axiom-5): a callee + its args, acknowledged by EXACTLY ONE of {nothing, 0, 1, node}.
 * This hook makes that offer/ack witnessable as a trace the observe organ reads — the live feed
 * runtime-witness.fk named as the one piece "that depends on the runtime emitting it" (a fire-event
 * per call). It composes the existing fk_arms tag-counter (which already witnesses every node
 * visit) by LIFTING it to the offer/ack altitude: one line per offer, carrying the callee, the
 * arg-count, and the FOUR-ARM ack-kind the call returned. Toggle: env FK_OBSERVE=1 (read once). OFF
 * -> fk_observe is 0, the branch is a single predicted-false test per call and emits NOTHING: zero
 * output, no alloc, no formatting. The mind watches itself think only when asked to. */
static long long fk_observe = -1;
/* -1 = unread; 0 = off; 1 = on */
static long long fk_observe_on(void) {
    if (fk_observe < 0) {
        char *e = fk_conf("FK_OBSERVE");
        fk_observe = (e && e[0] && e[0] != 48) ? 1 : 0;
    }
    return fk_observe;
}
/* the FOUR-ARM ack classifier (axiom-1 order: nothing first, the ground). A raw reducer ack is
 * exactly one of: nothing (the canonical sentinel) | 0 (the zero state) | node (a content-addressed
 * cell back, a counter-offer) | 1 (every other affirmative result — the carried payload). This is
 * the ONE place the four arms are read off a raw value; everything that needs the kind reads
 * through it (no per-call-site ack if-chain). */
static long long fk_ack_kind(long long v) {
    if (fk_is_nothing(v)) {
        return 0;
    }
    /* nothing — the silence/decline ack */
    if (v < 0) {
        long long ni = fk_nidx(v);
        if (ni >= 1 && ni <= fk_np) {
            return 3;
        }
    }
    /* node — counter-offer */
    if (v == 0) {
        return 1;
    }
    /* 0 — the zero state */
    return 2;
    /* 1 — the affirmative result (any payload) */
}
/* emit one offer/ack trace line (only when observing). Shape the observe organ reads: a fire-event
 * lifted to offer/ack — `offer <callee> args=<n> ack=<arm>`. callee is the function index offered;
 * <n> the args packed; <arm> the four-arm name. Returns v unchanged so it wraps a call's return
 * transparently. */
static long long fk_offer_ack(long long callee, long long argn, long long v) {
    if (fk_observe_on()) {
        long long k = fk_ack_kind(v);
        const char *arm = k == 0 ? "nothing" : (k == 1 ? "0" : (k == 2 ? "1" : "node"));
        printf("offer fn%lld args=%lld ack=%s\n", callee, argn, arm);
    }
    return v;
}
/* ── host world-sensors (host-kernel.form: world-sensors port VIA-HOST, allowed) ── WiFi
 * SSID/signal (wlanapi), Bluetooth radio + paired count (bthprops), battery + memory load
 * (kernel32). Afferent reads, plain C, same pattern as the camera/mic carriers; each degrades to an
 * honest sentinel ("" / -1 / 0) if the API is absent. They stream into the mesh as readings: wifi
 * SSID -> WHERE (place), bt -> WHO/near, power+mem -> vitality (observe/host-sensors-mesh.fk). */
#if defined(_WIN32)
extern unsigned int WlanOpenHandle(unsigned int, void *, unsigned int *, void **);
extern unsigned int WlanCloseHandle(void *, void *);
extern unsigned int WlanEnumInterfaces(void *, void *, void **);
extern unsigned int WlanQueryInterface(void *, const void *, int, void *, unsigned int *, void **,
                                       void *);
extern void WlanFreeMemory(void *);
static long long fk_wifi_query(char *ssid_out, long long cap, long long *signal_out) {
    *signal_out = -1;
    ssid_out[0] = 0;
    void *h = 0;
    unsigned int neg = 0;
    long long ret = -1;
    if (WlanOpenHandle(2, 0, &neg, &h) != 0) {
        return -1;
    }
    void *iflist = 0;
    if (WlanEnumInterfaces(h, 0, &iflist) == 0 && iflist != 0) {
        unsigned int n = *(unsigned int *)iflist;
        if (n > 0) {
            unsigned char *guid = (unsigned char *)iflist + 8;
            /* InterfaceInfo[0].InterfaceGuid */
            void *pconn = 0;
            unsigned int sz = 0;
            if (WlanQueryInterface(h, guid, 7, 0, &sz, &pconn, 0) == 0 && pconn != 0) {
                /* opcode 7 = current_connection */
                unsigned char *p = (unsigned char *)pconn;
                unsigned int slen = *(unsigned int *)(p + 520);
                /* wlanAssociationAttributes.dot11Ssid.uSSIDLength */
                if (slen > 32) {
                    slen = 32;
                }
                long long j = 0;
                while (j < (long long)slen && j < cap - 1) {
                    ssid_out[j] = (char)p[524 + j];
                    j = j + 1;
                }
                ssid_out[j] = 0;
                unsigned int sig = *(unsigned int *)(p + 576);
                /* wlanSignalQuality 0..100 */
                if (sig <= 100) {
                    *signal_out = (long long)sig;
                }
                ret = (long long)slen;
                WlanFreeMemory(pconn);
            }
        }
        WlanFreeMemory(iflist);
    }
    WlanCloseHandle(h, 0);
    return ret;
}
static long long fk_wifi_ssid(void) {
    char s[64];
    long long sig;
    if (fk_wifi_query(s, 64, &sig) < 0) {
        return fk_sbuf("", 0);
    }
    return fk_sbuf(s, fk_cstrlen(s));
}
static long long fk_wifi_signal(void) {
    char s[64];
    long long sig;
    fk_wifi_query(s, 64, &sig);
    return sig;
}
struct fk_btrp {
    unsigned long dwSize;
};
struct fk_btsp {
    unsigned long dwSize;
    int fReturnAuthenticated;
    int fReturnRemembered;
    int fReturnUnknown;
    int fReturnConnected;
    int fIssueInquiry;
    unsigned char cTimeoutMultiplier;
    void *hRadio;
};
struct fk_btdi {
    unsigned long dwSize;
    unsigned long long Address;
    unsigned long ulClassofDevice;
    int fConnected;
    int fRemembered;
    int fAuthenticated;
    unsigned short stLastSeen[8];
    unsigned short stLastUsed[8];
    unsigned short szName[248];
};
extern void *BluetoothFindFirstRadio(struct fk_btrp *, void **);
extern int BluetoothFindRadioClose(void *);
extern int CloseHandle(void *);
extern void *BluetoothFindFirstDevice(struct fk_btsp *, struct fk_btdi *);
extern int BluetoothFindNextDevice(void *, struct fk_btdi *);
extern int BluetoothFindDeviceClose(void *);
static long long fk_bt_present(void) {
    struct fk_btrp p;
    p.dwSize = sizeof p;
    void *hr = 0;
    void *f = BluetoothFindFirstRadio(&p, &hr);
    if (f == 0) {
        return 0;
    }
    if (hr != 0) {
        CloseHandle(hr);
    }
    BluetoothFindRadioClose(f);
    return 1;
}
static long long fk_bt_count(void) {
    struct fk_btsp sp;
    sp.dwSize = sizeof sp;
    sp.fReturnAuthenticated = 1;
    sp.fReturnRemembered = 1;
    sp.fReturnUnknown = 0;
    sp.fReturnConnected = 1;
    sp.fIssueInquiry = 0;
    sp.cTimeoutMultiplier = 0;
    sp.hRadio = 0;
    struct fk_btdi di;
    di.dwSize = sizeof di;
    void *f = BluetoothFindFirstDevice(&sp, &di);
    if (f == 0) {
        return 0;
    }
    long long c = 1;
    while (BluetoothFindNextDevice(f, &di) != 0) {
        c = c + 1;
    }
    BluetoothFindDeviceClose(f);
    return c;
}
struct fk_sps {
    unsigned char ACLineStatus;
    unsigned char BatteryFlag;
    unsigned char BatteryLifePercent;
    unsigned char SystemStatusFlag;
    unsigned long BatteryLifeTime;
    unsigned long BatteryFullLifeTime;
};
extern int GetSystemPowerStatus(struct fk_sps *);
static long long fk_power(void) {
    struct fk_sps s;
    if (GetSystemPowerStatus(&s) == 0) {
        return -1;
    }
    return (long long)s.BatteryLifePercent;
}
struct fk_msx {
    unsigned long dwLength;
    unsigned long dwMemoryLoad;
    unsigned long long a, b, c, d, e, f2, g;
};
extern int GlobalMemoryStatusEx(struct fk_msx *);
static long long fk_memload(void) {
    struct fk_msx m;
    m.dwLength = sizeof m;
    if (GlobalMemoryStatusEx(&m) == 0) {
        return -1;
    }
    return (long long)m.dwMemoryLoad;
}
#else
static long long fk_wifi_ssid(void) {
    return fk_sbuf("", 0);
}
static long long fk_wifi_signal(void) {
    return -1;
}
static long long fk_bt_present(void) {
    return -1;
}
static long long fk_bt_count(void) {
    return -1;
}
static long long fk_power(void) {
    return -1;
}
static long long fk_memload(void) {
    return -1;
}
#endif
static long long fk_sensors_report(void) {
    long long count = 0;
#if defined(_WIN32)
    char ssid[64];
    long long sig = -1;
    fk_wifi_query(ssid, 64, &sig);
#else
    char ssid[1];
    ssid[0] = 0;
    long long sig = -1;
#endif
    long long bt = fk_bt_present();
    long long btc = (bt > 0) ? fk_bt_count() : 0;
    long long pw = fk_power();
    long long mm = fk_memload();
    printf("host-sensors  (Windows: wlanapi + bthprops + kernel32)\n");
    printf("  wifi    where    ssid=%s  signal=%d\n", ssid[0] ? ssid : "(none)", (int)sig);
    printf("  bt      who      radio=%d  paired/near=%d\n", (int)bt, (int)btc);
    printf("  power   vitality battery=%d\n", (int)pw);
    printf("  memory  vitality load=%d\n", (int)mm);
    if (ssid[0]) {
        count = count + 1;
    }
    if (bt > 0) {
        count = count + 1;
    }
    if (pw >= 0) {
        count = count + 1;
    }
    if (mm >= 0) {
        count = count + 1;
    }
    printf("live sensors: %d\n", (int)count);
    return count;
}
static long long fk_tempdir() {
    char *e = getenv("TMPDIR");
    static char d[FK_PATH_CAP];
    long long n = 0;
    if (e != 0) {
        while (e[n] != 0 && n < 4095) {
            d[n] = e[n];
            n = n + 1;
        }
    }
    if (n == 0) {
        d[0] = 47;
        d[1] = 116;
        d[2] = 109;
        d[3] = 112;
        n = 4;
    }
    while (n > 1 && d[n - 1] == FK_CH_SLASH) {
        n = n - 1;
    }
    d[n] = 0;
    mkdir(d, 0777);
    return fk_sbuf(d, n);
}
static long long fk_keyeq(long long a, long long b) {
    if (a == b) {
        return 1;
    }
    if (a < 0 || b < 0 || !FK_SOK(a) || !FK_SOK(b) || FK_SLEN(a) != FK_SLEN(b)) {
        return 0;
    }
    long long j = 0;
    while (j < FK_SLEN(a)) {
        if (FK_SBYTES(a)[j] != FK_SBYTES(b)[j]) {
            return 0;
        }
        j = j + 1;
    }
    return 1;
}
static long long fk_file_mtime(long long pv) {
    static char p[FK_PATH_CAP];
    fk_cstr(pv, p, FK_PATH_CAP);
#ifdef FK_HAVE_STAT_HEADER
    struct stat st;
    if (stat(p, &st) != 0) {
        return -2;
    }
    return ((long long)st.st_mtime) << 1;
#else
    char st[512];
    if (stat(p, st) != 0) {
        return -2;
    }
    return 2;
#endif
}
static int fk_scan_match(unsigned char c, long long cls) {
    if (cls == 0) {
        return c == FK_CH_SPACE || c == FK_CH_TAB || c == FK_CH_LF || c == FK_CH_CR;
    }
    if (cls == 1) {
        return c >= FK_CH_DIGIT0 && c <= FK_CH_DIGIT9;
    }
    if (cls == 2) {
        return (c >= FK_CH_UPPER_A && c <= FK_CH_UPPER_Z) ||
               (c >= FK_CH_LOWER_A && c <= FK_CH_LOWER_Z);
    }
    if (cls == 3) {
        return (c >= FK_CH_UPPER_A && c <= FK_CH_UPPER_Z) ||
               (c >= FK_CH_LOWER_A && c <= FK_CH_LOWER_Z) ||
               (c >= FK_CH_DIGIT0 && c <= FK_CH_DIGIT9) || c == FK_CH_UNDERSCORE || c == FK_CH_DASH;
    }
    if (cls == 4) {
        return c != FK_CH_DQUOTE && c != FK_CH_BACKSLASH;
    }
    if (cls == 5) {
        return c != FK_CH_LF;
    }
    if (cls == 6) {
        return c >= FK_CH_SPACE && c != FK_CH_DQUOTE && c != FK_CH_BACKSLASH;
    }
    return 0;
}
static long long fk_scan_run(long long sv, long long fromv, long long clsv) {
    long long sa = fk_stri(sv);
    long long from = fromv >> 1;
    long long cls = clsv >> 1;
    if (from < 0) {
        from = 0;
    }
    if (sa < 0 || !FK_SOK(sa)) {
        return from << 1;
    }
    long long end = from;
    long long n = FK_SLEN(sa);
    while (end < n && fk_scan_match((unsigned char)FK_SBYTES(sa)[end], cls)) {
        end = end + 1;
    }
    return end << 1;
}
static void fk_unlink_segments(char *p) {
    char q[FK_PATH_CAP];
    long long pl = 0;
    while (p[pl] != 0) {
        pl = pl + 1;
    }
    /* same danger class as fk_path_join above: this path feeds unlink(), so a
     * silently truncated "safe" sprintf could delete the wrong file. p's own
     * bound isn't otherwise enforced by every caller, so check it here too. */
    if (pl + 20 > FK_PATH_CAP) {
        fk_die("fk_unlink_segments: path exceeds buffer capacity");
    }
    long long s = 0;
    while (s < 2048) {
        sprintf(q, "%s/seg-%06lld.log", p, s);
        unlink(q);
        s = s + 1;
    }
}
static int fk_path_is_dir(const char *p) {
#if defined(_WIN32)
    (void)p;
    return 0;
#else
    struct stat st;
    if (stat(p, &st) != 0) {
        return 0;
    }
    return S_ISDIR(st.st_mode) ? 1 : 0;
#endif
}
static int fk_name_eq(const char *a, const char *b) {
    int j = 0;
    while (a[j] != 0 && b[j] != 0) {
        if (a[j] != b[j]) {
            return 0;
        }
        j = j + 1;
    }
    return a[j] == 0 && b[j] == 0;
}
static int fk_skip_entry(long long skipv, const char *name) {
    long long q = skipv >> 1;
    while (q >= 1 && FK_POK(q)) {
        long long es = FK_HH(q);
        static char nb[512];
        fk_cstr(es, nb, 512);
        if (nb[0] != 0 && fk_name_eq(nb, name)) {
            return 1;
        }
        q = FK_HT(q) >> 1;
    }
    return 0;
}
static int fk_suffix_match(const char *name, const char *suf) {
    long long nl = 0;
    long long sl = 0;
    while (name[nl] != 0) {
        nl = nl + 1;
    }
    while (suf[sl] != 0) {
        sl = sl + 1;
    }
    if (sl > nl) {
        return 0;
    }
    long long j = 0;
    while (j < sl) {
        if (name[nl - sl + j] != suf[j]) {
            return 0;
        }
        j = j + 1;
    }
    return 1;
}
/* ── the cons heap grows where the melt cannot reach ────────────────────────
 * Pair handles are INDICES into fk_hh/fk_ht, so a realloc doubling relocates
 * NOTHING — safe exactly where fk_melt is not (mid-loop with live C-local
 * intermediates the collector cannot trace). Melt stays the comptroller on
 * the main cons path (compaction at 90%); growth is the escape valve at
 * every site that used to answer the wall with a die, a silent partial
 * list, or — the closure-capture copy loops — an unguarded write past the
 * arrays. kernel_stat 21 carries the live cap, 22 the doublings. OOM here
 * dies loudly, the same honest door as fk_melt's arena. */
static long long fk_heap_grows;
static void fk_arena(void);
static void fk_heap_grow(void) {
    long long nc;
    if (fk_cap == 0) {
        fk_arena();
        return;
    }
    nc = fk_cap * 2;
    if (fk_store_shared && nc <= FK_STORE_HEAP_PAIRS) {
        fk_cap = nc;
        fk_heap_grows = fk_heap_grows + 1;
        return;
    }
    if (fk_store_shared) { fk_store_go_private(); }
    long long *nh = realloc(fk_hh, (unsigned long)(nc * 8));
    long long *nt;
    if (nh != 0) {
        fk_hh = nh;
    }
    nt = realloc(fk_ht, (unsigned long)(nc * 8));
    if (nh == 0 || nt == 0) {
        fk_die("fk_heap_grow: out of memory growing the cons heap");
    }
    fk_ht = nt;
    fk_cap = nc;
    fk_heap_grows = fk_heap_grows + 1;
}
static long long fk_list_push(long long acc, long long sv) {
    if (fk_hp + 1 >= fk_cap) {
        fk_heap_grow();
    }
    fk_hp = fk_hp + 1;
    fk_hh[fk_hp] = sv;
    fk_ht[fk_hp] = acc;
    return (fk_hp << 1) | 1;
}
static long long fk_count_lines_file(const char *path) {
    int fd = open(path, 0);
    if (fd < 0) {
        return 2;
    }
    char buf[4096];
    long long n = 0;
    long long got = 0;
    while ((got = read(fd, buf, 4096)) > 0) {
        long long j = 0;
        while (j < got) {
            if (buf[j] == 10) {
                n = n + 1;
            }
            j = j + 1;
        }
    }
    close(fd);
    if (n == 0) {
        return 2;
    }
    return n << 1;
}
static long long fk_row_pair(long long relsv, long long loc) {
    long long row = 1;
    row = fk_list_push(row, loc);
    row = fk_list_push(row, relsv);
    return row;
}
/* Heap-grown: the old static 512 silently dropped a directory's 513th entry —
 * a partial listing wearing a whole one's skin (the silent-partial family;
 * healed 2026-08-27). Growth is the capability answer, never a quiet cap. */
static long long *fk_ls_buf = 0;
static long long fk_ls_cap = 0;
static long long fk_ls_n = 0;
static void fk_ls_reset(void) {
    fk_ls_n = 0;
}
static void fk_ls_add(long long sv) {
    if (fk_ls_n >= fk_ls_cap) {
        fk_ls_cap = fk_ls_cap == 0 ? 512 : fk_ls_cap * 2;
        fk_ls_buf = realloc(fk_ls_buf, fk_ls_cap * 8);
        if (fk_ls_buf == 0) {
            fk_die("fk_ls_add: out of memory growing directory listing");
        }
    }
    fk_ls_buf[fk_ls_n] = sv;
    fk_ls_n = fk_ls_n + 1;
}
static int fk_sv_less(long long a, long long b) {
    long long aa = fk_stri(a);
    long long bb = fk_stri(b);
    if (aa < 0 || bb < 0 || !FK_SOK(aa) || !FK_SOK(bb)) {
        return 0;
    }
    long long la = FK_SLEN(aa);
    long long lb = FK_SLEN(bb);
    long long j = 0;
    while (j < la && j < lb) {
        unsigned char ca = (unsigned char)FK_SBYTES(aa)[j];
        unsigned char cb = (unsigned char)FK_SBYTES(bb)[j];
        if (ca < cb) {
            return 1;
        }
        if (ca > cb) {
            return 0;
        }
        j = j + 1;
    }
    return la < lb;
}
#ifndef _WIN32
static long long fk_fs_list_path(const char *p) {
    fk_ls_reset();
    DIR *d = opendir(p);
    if (d) {
        struct dirent *e;
        while ((e = readdir(d)) != 0) {
            if (e->d_name[0] == FK_CH_DOT &&
                (e->d_name[1] == 0 || (e->d_name[1] == FK_CH_DOT && e->d_name[2] == 0))) {
                continue;
            }
            long long nl = 0;
            while (e->d_name[nl] != 0) {
                nl = nl + 1;
            }
            fk_ls_add(fk_sbuf(e->d_name, nl));
        }
        closedir(d);
    }
    long long i = 0;
    long long j = 0;
    while (j < fk_ls_n) {
        i = 0;
        while (i + 1 < fk_ls_n) {
            if (fk_sv_less(fk_ls_buf[i + 1], fk_ls_buf[i])) {
                long long t = fk_ls_buf[i];
                fk_ls_buf[i] = fk_ls_buf[i + 1];
                fk_ls_buf[i + 1] = t;
            }
            i = i + 1;
        }
        j = j + 1;
    }
    long long out = 1;
    i = fk_ls_n;
    while (i > 0) {
        i = i - 1;
        out = fk_list_push(out, fk_ls_buf[i]);
    }
    return out;
}
/* fk_rmtree and fk_inv_walk build child paths RECURSIVELY (the built path
 * becomes the next call's `dir`) into FK_PATH_CAP buffers, so the join checks
 * that dir + '/' + d_name fits and hard-stops when it does not. A truncating
 * "safe" sprintf would be worse here, not better: fk_rmtree DELETES whatever
 * path it is given, and a cut path names the wrong thing. */
static void fk_path_join(char *out, long long outcap, const char *a, const char *b) {
    long long la = 0;
    while (a[la] != 0) {
        la = la + 1;
    }
    long long lb = 0;
    while (b[lb] != 0) {
        lb = lb + 1;
    }
    if (la + 1 + lb + 1 > outcap) {
        fk_die("fk_path_join: combined path exceeds buffer capacity");
    }
    long long k = 0;
    while (k < la) {
        out[k] = a[k];
        k = k + 1;
    }
    out[k] = 47;
    k = k + 1;
    long long j = 0;
    while (j < lb) {
        out[k + j] = b[j];
        j = j + 1;
    }
    out[k + lb] = 0;
}
static void fk_rmtree(char *p) {
    if (fk_path_is_dir(p)) {
        DIR *d = opendir(p);
        if (d) {
            struct dirent *e;
            char child[FK_PATH_CAP];
            while ((e = readdir(d)) != 0) {
                if (e->d_name[0] == FK_CH_DOT &&
                    (e->d_name[1] == 0 || (e->d_name[1] == FK_CH_DOT && e->d_name[2] == 0))) {
                    continue;
                }
                fk_path_join(child, FK_PATH_CAP, p, e->d_name);
                fk_rmtree(child);
            }
            closedir(d);
        }
        fk_unlink_segments(p);
        rmdir(p);
        return;
    }
    unlink(p);
}
static long long fk_inv_rows = 1;
static void fk_inv_reset(void) {
    fk_inv_rows = 1;
}
static void fk_inv_push(long long row) {
    fk_inv_rows = fk_list_push(fk_inv_rows, row);
}
static void fk_inv_walk(const char *root, const char *dir, const char *suf, long long skipv) {
    DIR *d = opendir(dir);
    if (!d) {
        return;
    }
    struct dirent *e;
    char path[FK_PATH_CAP];
    while ((e = readdir(d)) != 0) {
        if (e->d_name[0] == FK_CH_DOT &&
            (e->d_name[1] == 0 || (e->d_name[1] == FK_CH_DOT && e->d_name[2] == 0))) {
            continue;
        }
        if (fk_skip_entry(skipv, e->d_name)) {
            continue;
        }
        fk_path_join(path, FK_PATH_CAP, dir, e->d_name);
        if (fk_path_is_dir(path)) {
            fk_inv_walk(root, path, suf, skipv);
        } else {
            if (suf[0] != 0 && !fk_suffix_match(e->d_name, suf)) {
                continue;
            }
            long long rn = 0;
            while (root[rn] != 0) {
                rn = rn + 1;
            }
            const char *relstart = path + rn;
            if (relstart[0] == FK_CH_SLASH) {
                relstart = relstart + 1;
            }
            long long rlen = 0;
            while (relstart[rlen] != 0) {
                rlen = rlen + 1;
            }
            fk_inv_push(fk_row_pair(fk_sbuf(relstart, rlen), fk_count_lines_file(path)));
        }
    }
    closedir(d);
}
#else
static long long fk_fs_list_path(const char *p) {
    (void)p;
    return 1;
}
static void fk_rmtree(char *p) {
    (void)p;
}
static long long fk_inv_rows = 1;
static void fk_inv_reset(void) {
    fk_inv_rows = 1;
}
static void fk_inv_push(long long row) {
    fk_inv_rows = fk_list_push(fk_inv_rows, row);
}
static void fk_inv_walk(const char *root, const char *dir, const char *suf, long long skipv) {
    (void)root;
    (void)dir;
    (void)suf;
    (void)skipv;
}
#endif

#if defined(_WIN32)
typedef unsigned long long fk_os_socket_t;
#define FK_SL int
struct fk_wsadata {
    unsigned short wVersion;
    unsigned short wHighVersion;
    char szDescription[257];
    char szSystemStatus[129];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char *lpVendorInfo;
};
extern int WSAStartup(unsigned short, struct fk_wsadata *);
extern fk_os_socket_t socket(int, int, int);
extern int bind(fk_os_socket_t, const void *, int);
extern int listen(fk_os_socket_t, int);
extern fk_os_socket_t accept(fk_os_socket_t, void *, void *);
extern int connect(fk_os_socket_t, const void *, int);
extern int getsockname(fk_os_socket_t, void *, int *);
extern int setsockopt(fk_os_socket_t, int, int, const char *, int);
extern int closesocket(fk_os_socket_t);
extern int recv(fk_os_socket_t, char *, int, int);
extern int send(fk_os_socket_t, const char *, int, int);
#define FK_INVALID_SOCKET ((fk_os_socket_t)(~0ULL))
#define FK_SOL_SOCKET_NATIVE 65535
#define FK_SO_REUSEADDR_NATIVE 4
static void fk_sock_boot(void) {
    static int ready = 0;
    if (ready == 0) {
        struct fk_wsadata w;
        if (WSAStartup(0x0202, &w) == 0) {
            ready = 1;
        }
    }
}
static int fk_os_socket_ok(fk_os_socket_t s) {
    return s != FK_INVALID_SOCKET;
}
static int fk_os_close_socket(fk_os_socket_t s) {
    return closesocket(s);
}
static long long fk_os_recv_socket(fk_os_socket_t s, void *buf, unsigned long n) {
    if (n > 2147483647UL) {
        n = 2147483647UL;
    }
    return (long long)recv(s, (char *)buf, (int)n, 0);
}
static long long fk_os_send_socket(fk_os_socket_t s, const void *buf, unsigned long n) {
    if (n > 2147483647UL) {
        n = 2147483647UL;
    }
    return (long long)send(s, (const char *)buf, (int)n, 0);
}
static int fk_os_setsockopt_reuse(fk_os_socket_t s, int *yes) {
    return setsockopt(s, FK_SOL_SOCKET_NATIVE, FK_SO_REUSEADDR_NATIVE, (const char *)yes, 4);
}
#else
typedef int fk_os_socket_t;
#define FK_SL unsigned
extern int socket(int, int, int);
extern int bind(int, const void *, unsigned int);
extern int listen(int, int);
extern long accept(int, void *, void *);
extern int connect(int, const void *, unsigned int);
extern int getsockname(int, void *, unsigned int *);
extern int setsockopt(int, int, int, const void *, unsigned int);
extern long long recv(int, void *, unsigned long, int);
extern long long send(int, const void *, unsigned long, int);
#define FK_INVALID_SOCKET (-1)
#if defined(__linux__)
#define FK_SOL_SOCKET_NATIVE 1
#define FK_SO_REUSEADDR_NATIVE 2
#else
#define FK_SOL_SOCKET_NATIVE 65535
#define FK_SO_REUSEADDR_NATIVE 4
#endif
static void fk_sock_boot(void) {}
static int fk_os_socket_ok(fk_os_socket_t s) {
    return s >= 0;
}
static int fk_os_close_socket(fk_os_socket_t s) {
    return close(s);
}
static long long fk_os_recv_socket(fk_os_socket_t s, void *buf, unsigned long n) {
    return recv(s, buf, n, 0);
}
static long long fk_os_send_socket(fk_os_socket_t s, const void *buf, unsigned long n) {
    return send(s, buf, n, 0);
}
static int fk_os_setsockopt_reuse(fk_os_socket_t s, int *yes) {
    return setsockopt(s, FK_SOL_SOCKET_NATIVE, FK_SO_REUSEADDR_NATIVE, yes, 4);
}
#endif
#if defined(_WIN32)
struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    unsigned long long ai_addrlen;
    char *ai_canonname;
    void *ai_addr;
    struct addrinfo *ai_next;
};
#elif defined(__linux__)
struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    unsigned int ai_addrlen;
    void *ai_addr;
    char *ai_canonname;
    struct addrinfo *ai_next;
};
#else
struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    unsigned int ai_addrlen;
    char *ai_canonname;
    void *ai_addr;
    struct addrinfo *ai_next;
};
#endif
extern int getaddrinfo(const char *, const char *, const struct addrinfo *, struct addrinfo **);
extern void freeaddrinfo(struct addrinfo *);
struct fk_sockaddr4 {
#if defined(__APPLE__)
    unsigned char len;
    unsigned char fam;
#else
    unsigned short fam;
#endif
    unsigned char p[2];
    unsigned int addr;
    unsigned char z[8];
};
static void fk_sockaddr4_set(struct fk_sockaddr4 *a, long long port, unsigned int addr) {
#if defined(__APPLE__)
    a->len = 16;
    a->fam = 2;
#else
    a->fam = 2;
#endif
    a->p[0] = (unsigned char)((port >> 8) & 255);
    a->p[1] = (unsigned char)(port & 255);
    a->addr = addr;
    int z = 0;
    while (z < 8) {
        a->z[z] = 0;
        z = z + 1;
    }
}
#define FK_SOCK_CAP_INIT 1024 /* fk_sock_raw/kind: live socket handles (listeners kind 1, streams kind 2). Birth size only -- the table doubles on demand (fk_sock_grow, kernel_stat 31/32 = live cap / doublings). The old fixed 1024 closed the 1025th socket and handed back -1 with no word; only the allocator can refuse now. */
static fk_os_socket_t *fk_sock_raw;
static int *fk_sock_kind;
static long long fk_sock_cap, fk_sock_grows;
static void fk_sock_grow(void) {
    long long nc = fk_sock_cap == 0 ? FK_SOCK_CAP_INIT : fk_sock_cap * 2;
    long long i = fk_sock_cap;
    fk_sock_raw = (fk_os_socket_t *)realloc(fk_sock_raw, (unsigned long)nc * sizeof(fk_os_socket_t));
    fk_sock_kind = (int *)realloc(fk_sock_kind, (unsigned long)nc * sizeof(int));
    if (fk_sock_raw == 0 || fk_sock_kind == 0) {
        fk_die("fk_sock_grow: out of memory growing the socket handle table");
    }
    while (i < nc) {
        fk_sock_kind[i] = 0;
        i = i + 1;
    }
    if (fk_sock_cap != 0) {
        fk_sock_grows = fk_sock_grows + 1;
    }
    fk_sock_cap = nc;
}
static long long fk_sock_alloc(fk_os_socket_t s, int kind) {
    long long h = 1;
    if (fk_sock_cap == 0) {
        fk_sock_grow();
    }
    while (h < fk_sock_cap && fk_sock_kind[h] != 0) {
        h = h + 1;
    }
    if (h >= fk_sock_cap) {
        fk_sock_grow();
    }
    fk_sock_raw[h] = s;
    fk_sock_kind[h] = kind;
    return h;
}
static fk_os_socket_t fk_sock_lookup(long long h, int kind) {
    if (h < 1 || h >= fk_sock_cap || fk_sock_kind[h] == 0) {
        return FK_INVALID_SOCKET;
    }
    if (kind != 0 && fk_sock_kind[h] != kind) {
        return FK_INVALID_SOCKET;
    }
    return fk_sock_raw[h];
}
static long long fk_socket_listen_native(long long port) {
    fk_sock_boot();
    fk_os_socket_t s = socket(2, 1, 0);
    if (!fk_os_socket_ok(s)) {
        return -1;
    }
    int yes = 1;
    fk_os_setsockopt_reuse(s, &yes);
    struct fk_sockaddr4 a;
    fk_sockaddr4_set(&a, port, 0);
    if (bind(s, &a, 16) < 0) {
        fk_os_close_socket(s);
        return -1;
    }
    if (listen(s, 16) < 0) {
        fk_os_close_socket(s);
        return -1;
    }
    return fk_sock_alloc(s, 1);
}
static long long fk_socket_port_native(long long h) {
    fk_os_socket_t s = fk_sock_lookup(h, 1);
    if (!fk_os_socket_ok(s)) {
        return -1;
    }
    struct fk_sockaddr4 a;
    FK_SL n = 16;
    if (getsockname(s, &a, &n) < 0) {
        return -1;
    }
    return (((long long)a.p[0]) << 8) + (long long)a.p[1];
}
static long long fk_socket_accept_native(long long h) {
    fk_os_socket_t s = fk_sock_lookup(h, 1);
    if (!fk_os_socket_ok(s)) {
        return -1;
    }
    fk_sock_boot();
    fk_os_socket_t c = (fk_os_socket_t)accept(s, 0, 0);
    if (!fk_os_socket_ok(c)) {
        return -1;
    }
    return fk_sock_alloc(c, 2);
}
static long long fk_socket_connect_native(long long hostv, long long portv) {
    fk_sock_boot();
    char host[512];
    char port[32];
    fk_cstr(hostv, host, 512);
    sprintf(port, "%lld", portv);
    struct addrinfo hints;
    hints.ai_flags = 0;
    hints.ai_family = 0;
    hints.ai_socktype = 1;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_canonname = 0;
    hints.ai_addr = 0;
    hints.ai_next = 0;
    struct addrinfo *res = 0;
    if (getaddrinfo(host, port, &hints, &res) != 0 || res == 0) {
        return -1;
    }
    fk_os_socket_t s = FK_INVALID_SOCKET;
    struct addrinfo *rp = res;
    while (rp != 0) {
        s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fk_os_socket_ok(s)) {
            if (connect(s, rp->ai_addr, (unsigned int)rp->ai_addrlen) == 0) {
                break;
            }
            fk_os_close_socket(s);
            s = FK_INVALID_SOCKET;
        }
        rp = rp->ai_next;
    }
    freeaddrinfo(res);
    if (!fk_os_socket_ok(s)) {
        return -1;
    }
    return fk_sock_alloc(s, 2);
}
static long long fk_socket_send_native(long long h, long long sv) {
    fk_os_socket_t s = fk_sock_lookup(h, 2);
    long long sa = fk_stri(sv);
    if (!fk_os_socket_ok(s) || sa < 0 || !FK_SOK(sa)) {
        return -1;
    }
    return fk_os_send_socket(s, FK_SBYTES(sa), (unsigned long)FK_SLEN(sa));
}
static long long fk_socket_recv_native(long long h, long long maxn) {
    fk_os_socket_t s = fk_sock_lookup(h, 2);
    /* Three answers, three meanings (2026-08-27): a dead handle or a recv
     * ERROR answers the axiom-1 nothing — no byte was measured, and reading
     * either as "" let a mid-stream failure pass as end-of-response, handing
     * back a truncated reply as complete. "" is reserved for the two honest
     * empties: asked-for-zero, and the peer's orderly close. */
    if (!fk_os_socket_ok(s)) {
        return fk_nothing;
    }
    if (maxn <= 0) {
        return fk_sbuf("", 0);
    }
    static char tmp[65536];
    if (maxn > 65536) {
        maxn = 65536;
    }
    long long got = fk_os_recv_socket(s, tmp, (unsigned long)maxn);
    if (got < 0) {
        return fk_nothing;
    }
    if (got == 0) {
        return fk_sbuf("", 0);
    }
    return fk_sbuf(tmp, got);
}
static long long fk_socket_close_native(long long h) {
    fk_os_socket_t s = fk_sock_lookup(h, 0);
    if (!fk_os_socket_ok(s)) {
        return -1;
    }
    fk_sock_kind[h] = 0;
    if (fk_os_close_socket(s) == 0) {
        return 0;
    }
    return -1;
}
/* ── live MESH transport (host-kernel.form world-net port): the Windows cell streams its live
 * senses over TCP into the mesh; a mesh endpoint receives them. The socket move IS a host carrier
 * (like camera/mic); the readings are the mesh-safe rows. sense_publish(port) connects
 * 127.0.0.1:port and sends the live readings; mesh_serve(port) listens/accepts/ recvs/prints one
 * message (the receiver / relay tap). Point the host at the Mac's field-relay to make it
 * cross-device; loopback witnesses it. */
#if defined(_WIN32)
static long long fk_sense_publish(long long port) {
    static char buf[FK_MESH_MSG_CAP];
    int n = 0;
    long long mic = fk_mic_count();
    long long cam = fk_cam_count();
    char ssid[64];
    long long sig = -1;
    fk_wifi_query(ssid, 64, &sig);
    long long bt = fk_bt_present();
    long long pw = fk_power();
    long long mm = fk_memload();
    n = n + sprintf(buf + n, "cell=windows-binary\n");
    n = n + sprintf(buf + n, "reading present  cam=%d mic=%d\n", (int)cam, (int)mic);
    n = n + sprintf(buf + n, "reading where    wifi=%s sig=%d bt=%d\n", ssid[0] ? ssid : "-",
                    (int)sig, (int)bt);
    n = n + sprintf(buf + n, "reading vitality battery=%d mem=%d\n", (int)pw, (int)mm);

    /* relay host: env MESH_RELAY=a.b.c.d (the Mac's field-relay), default 127.0.0.1 — cross-device.
     */
    unsigned int addr = 0x0100007f;
    char *rl = fk_conf("MESH_RELAY");
    if (rl != 0) {
        unsigned int o0 = 0, o1 = 0, o2 = 0, o3 = 0;
        long long k = 0;
        unsigned int *cur = &o0;
        int part = 0;
        while (rl[k] != 0) {
            char ch = rl[k];
            if (ch >= FK_CH_DIGIT0 && ch <= FK_CH_DIGIT9) {
                *cur = (*cur) * 10 + (unsigned int)(ch - FK_CH_DIGIT0);
            } else if (ch == FK_CH_DOT && part < 3) {
                part = part + 1;
                cur = (part == 1) ? &o1 : (part == 2) ? &o2 : &o3;
            }
            k = k + 1;
        }
        if (part == 3) {
            addr = (o0 & 255) | ((o1 & 255) << 8) | ((o2 & 255) << 16) | ((o3 & 255) << 24);
        }
    }
    fk_sock_boot();
    fk_os_socket_t s = socket(2, 1, 0);
    if (!fk_os_socket_ok(s)) {
        return -1;
    }
    struct fk_sockaddr4 a;
    fk_sockaddr4_set(&a, port, addr);
    /* MESH_RELAY or 127.0.0.1 */
    if (connect(s, &a, 16) != 0) {
        fk_os_close_socket(s);
        return -2;
    }
    long long sent = fk_os_send_socket(s, buf, (unsigned long)n);
    fk_os_close_socket(s);
    return sent;
}
static long long fk_mesh_serve(long long port) {
    fk_sock_boot();
    fk_os_socket_t ls = socket(2, 1, 0);
    if (!fk_os_socket_ok(ls)) {
        return -1;
    }
    int yes = 1;
    fk_os_setsockopt_reuse(ls, &yes);
    struct fk_sockaddr4 a;
    fk_sockaddr4_set(&a, port, 0);
    if (bind(ls, &a, 16) != 0) {
        fk_os_close_socket(ls);
        return -2;
    }
    if (listen(ls, 1) != 0) {
        fk_os_close_socket(ls);
        return -3;
    }
    fk_os_socket_t cs = accept(ls, 0, 0);
    if (!fk_os_socket_ok(cs)) {
        fk_os_close_socket(ls);
        return -4;
    }
    static char rbuf[8192];
    long long got = fk_os_recv_socket(cs, rbuf, 8191);
    if (got > 0) {
        long long j = 0;
        while (j < got) {
            putchar((int)(unsigned char)rbuf[j]);
            j = j + 1;
        }
    }
    fk_os_close_socket(cs);
    fk_os_close_socket(ls);
    return got;
}
/* ── mesh AUTO-DISCOVERY (no copy-pasted peer address): announce presence + readings by UDP
 * BROADCAST to the LAN; discover peers by listening for theirs. The cell JOINS the mesh over
 * whatever channel is available — broadcast on 255.255.255.255:port — and finds the others, rather
 * than being handed an IP. mesh_announce(port) broadcasts; mesh_discover (port) receives one peer's
 * announce. This supersedes the MESH_RELAY env (a hand-config). */
extern int sendto(fk_os_socket_t, const char *, int, int, const void *, int);
extern int recvfrom(fk_os_socket_t, char *, int, int, void *, int *);
static long long fk_mesh_announce(long long port) {
    static char buf[FK_MESH_MSG_CAP];
    int n = 0;
    long long mic = fk_mic_count();
    long long cam = fk_cam_count();
    char ssid[64];
    long long sig = -1;
    fk_wifi_query(ssid, 64, &sig);
    long long bt = fk_bt_present();
    long long pw = fk_power();
    long long mm = fk_memload();
    n = n + sprintf(buf + n, "cell=windows-binary\n");
    n = n + sprintf(buf + n, "reading present  cam=%d mic=%d\n", (int)cam, (int)mic);
    n = n + sprintf(buf + n, "reading where    wifi=%s sig=%d bt=%d\n", ssid[0] ? ssid : "-",
                    (int)sig, (int)bt);
    n = n + sprintf(buf + n, "reading vitality battery=%d mem=%d\n", (int)pw, (int)mm);
    fk_sock_boot();
    fk_os_socket_t s = socket(2, 2, 0);
    /* AF_INET, SOCK_DGRAM */
    if (!fk_os_socket_ok(s)) {
        return -1;
    }
    int yes = 1;
    setsockopt(s, 65535, 32, (const char *)&yes, 4);
    /* SOL_SOCKET, SO_BROADCAST */
    struct fk_sockaddr4 a;
    fk_sockaddr4_set(&a, port, 0xffffffff);
    /* 255.255.255.255 — LAN broadcast, NO peer address */
    long long sent = sendto(s, buf, (int)n, 0, &a, 16);
    fk_os_close_socket(s);
    return sent;
}
static long long fk_mesh_discover(long long port) {
    fk_sock_boot();
    fk_os_socket_t s = socket(2, 2, 0);
    /* DGRAM */
    if (!fk_os_socket_ok(s)) {
        return -1;
    }
    int yes = 1;
    fk_os_setsockopt_reuse(s, &yes);
    struct fk_sockaddr4 a;
    fk_sockaddr4_set(&a, port, 0);
    /* INADDR_ANY — listen for any peer's broadcast */
    if (bind(s, &a, 16) != 0) {
        fk_os_close_socket(s);
        return -2;
    }
    static char rbuf[8192];
    struct fk_sockaddr4 from;
    int fromlen = 16;
    long long got = recvfrom(s, rbuf, 8191, 0, &from, &fromlen);
    if (got > 0) {
        long long j = 0;
        while (j < got) {
            putchar((int)(unsigned char)rbuf[j]);
            j = j + 1;
        }
    }
    fk_os_close_socket(s);
    return got;
}
/* ── mesh RENDEZVOUS SERVER (the new-repo core, proven on the kernel's own carriers): PUBLIC ACCESS
 * via a listening socket, PERSISTENCE via an append-only registry file, DISCOVERY via the roster
 * read-back. mesh_registry(port) accepts one registration, appends it to mesh-cells.txt, acks;
 * mesh_roster() reads the persisted registry. This is the server's CARRIER layer; its
 * routing/registry LOGIC is Form (comes home as .fk at the cursor seed). */
static long long fk_mesh_registry(long long port) {
    fk_sock_boot();
    fk_os_socket_t ls = socket(2, 1, 0);
    if (!fk_os_socket_ok(ls)) {
        return -1;
    }
    int yes = 1;
    fk_os_setsockopt_reuse(ls, &yes);
    struct fk_sockaddr4 a;
    fk_sockaddr4_set(&a, port, 0);
    if (bind(ls, &a, 16) != 0) {
        fk_os_close_socket(ls);
        return -2;
    }
    if (listen(ls, 4) != 0) {
        fk_os_close_socket(ls);
        return -3;
    }
    fk_os_socket_t cs = accept(ls, 0, 0);
    if (!fk_os_socket_ok(cs)) {
        fk_os_close_socket(ls);
        return -4;
    }
    static char rbuf[8192];
    long long got = fk_os_recv_socket(cs, rbuf, 8191);
    if (got > 0) {
        int fd = open("mesh-cells.txt", 1 | 0x100 | 0x8 | 0x8000, 0666);
        /* O_WRONLY|O_CREAT|O_APPEND|O_BINARY — append-only registry */
        if (fd >= 0) {
            write(fd, rbuf, (unsigned long)got);
            write(fd, "\n---\n", 5);
            close(fd);
        }
        fk_os_send_socket(cs, "registered\n", 11);
    }
    fk_os_close_socket(cs);
    fk_os_close_socket(ls);
    return got;
}
static long long fk_mesh_roster(void) {
    int fd = open("mesh-cells.txt", 0x8000);
    /* O_RDONLY|O_BINARY — read the persisted registry */
    if (fd < 0) {
        return 0;
    }
    static char buf[65536];
    long long n = 0;
    long long g;
    while ((g = read(fd, buf + n, 8192)) > 0) {
        n = n + g;
        if (n > 60000) {
            break;
        }
    }
    close(fd);
    long long j = 0;
    while (j < n) {
        putchar((int)(unsigned char)buf[j]);
        j = j + 1;
    }
    return n;
}
#else
static long long fk_sense_publish(long long port) {
    (void)port;
    return -1;
}
static long long fk_mesh_serve(long long port) {
    (void)port;
    return -1;
}
static long long fk_mesh_announce(long long port) {
    (void)port;
    return -1;
}
static long long fk_mesh_discover(long long port) {
    (void)port;
    return -1;
}
static long long fk_mesh_registry(long long port) {
    (void)port;
    return -1;
}
static long long fk_mesh_roster(void) {
    return -1;
}
#endif
/* ── PUBLIC-API proxy channel (cross-network rendezvous): register the cell + detect peers through
 * https://api.coherencycoin.com over Windows-native TLS (WinHTTP — the kernel's libcrypto TLS is
 * unavailable on Windows). LAN broadcast (mesh_announce/discover) joins same-network cells; this
 * proxy joins cells on DIFFERENT networks via the public API. */
#if defined(_WIN32)
extern void *WinHttpOpen(const unsigned short *, unsigned long, const unsigned short *,
                         const unsigned short *, unsigned long);
extern void *WinHttpConnect(void *, const unsigned short *, unsigned short, unsigned long);
extern void *WinHttpOpenRequest(void *, const unsigned short *, const unsigned short *,
                                const unsigned short *, const unsigned short *,
                                const unsigned short **, unsigned long);
extern int WinHttpSendRequest(void *, const unsigned short *, unsigned long, void *, unsigned long,
                              unsigned long, unsigned long long);
extern int WinHttpReceiveResponse(void *, void *);
extern int WinHttpQueryDataAvailable(void *, unsigned long *);
extern int WinHttpReadData(void *, void *, unsigned long, unsigned long *);
extern int WinHttpCloseHandle(void *);
static void fk_widen(const char *s, unsigned short *w, long long cap) {
    long long i = 0;
    while (s[i] != 0 && i < cap - 1) {
        w[i] = (unsigned short)(unsigned char)s[i];
        i = i + 1;
    }
    w[i] = 0;
}
static long long fk_https(const char *path, const char *method, const char *body, long long blen,
                          char *out, long long cap) {
    static unsigned short wa[16], wh[64], wp[512], wm[8], wct[64];
    fk_widen("fkwu", wa, 16);
    fk_widen("api.coherencycoin.com", wh, 64);
    fk_widen(path, wp, 512);
    fk_widen(method, wm, 8);
    void *hS = WinHttpOpen(wa, 0, 0, 0, 0);
    /* DEFAULT_PROXY */
    if (hS == 0) {
        return -1;
    }
    void *hC = WinHttpConnect(hS, wh, 443, 0);
    if (hC == 0) {
        WinHttpCloseHandle(hS);
        return -2;
    }
    void *hR = WinHttpOpenRequest(hC, wm, wp, 0, 0, 0, 0x00800000);
    /* WINHTTP_FLAG_SECURE */
    if (hR == 0) {
        WinHttpCloseHandle(hC);
        WinHttpCloseHandle(hS);
        return -3;
    }
    const unsigned short *hdr = 0;
    unsigned long hdrlen = 0;
    if (blen > 0) {
        fk_widen("Content-Type: application/json\r\n", wct, 64);
        hdr = wct;
        hdrlen = 0xFFFFFFFFu;
    }
    long long rc = -4;
    if (WinHttpSendRequest(hR, hdr, hdrlen, (void *)body, (unsigned long)blen, (unsigned long)blen,
                           0) &&
        WinHttpReceiveResponse(hR, 0)) {
        long long total = 0;
        unsigned long avail = 0;
        while (WinHttpQueryDataAvailable(hR, &avail) && avail > 0) {
            if (total + (long long)avail > cap - 1) {
                avail = (unsigned long)(cap - 1 - total);
            }
            if (avail == 0) {
                break;
            }
            unsigned long got = 0;
            if (!WinHttpReadData(hR, out + total, avail, &got) || got == 0) {
                break;
            }
            total = total + (long long)got;
        }
        out[total] = 0;
        rc = total;
    }
    WinHttpCloseHandle(hR);
    WinHttpCloseHandle(hC);
    WinHttpCloseHandle(hS);
    return rc;
}
static long long fk_api_health(void) {
    static char out[8192];
    long long n = fk_https("/api/health", "GET", 0, 0, out, 8192);
    if (n > 0) {
        long long j = 0;
        while (j < n) {
            putchar((int)(unsigned char)out[j]);
            j = j + 1;
        }
        putchar(10);
    }
    return n;
}
static long long fk_mesh_register(void) {
    static char body[FK_MESH_MSG_CAP];
    int n;
    long long mic = fk_mic_count();
    long long cam = fk_cam_count();
    char ssid[64];
    long long sig = -1;
    fk_wifi_query(ssid, 64, &sig);
    long long pw = fk_power();
    long long mm = fk_memload();
    n = sprintf(
        body,
        "{\"cell\":\"windows-binary\",\"present\":{\"cam\":%d,\"mic\":%d},\"where\":{\"wifi\":\"%s\",\"sig\":%d},\"vitality\":{\"battery\":%d,\"mem\":%d}}",
        (int)cam, (int)mic, ssid[0] ? ssid : "-", (int)sig, (int)pw, (int)mm);
    static char out[8192];
    long long r = fk_https("/api/mesh/register", "POST", body, n, out, 8192);
    if (r > 0) {
        long long j = 0;
        while (j < r) {
            putchar((int)(unsigned char)out[j]);
            j = j + 1;
        }
        putchar(10);
    }
    return r;
}
static long long fk_mesh_detect(void) {
    static char out[16384];
    long long n = fk_https("/api/mesh/cells", "GET", 0, 0, out, 16384);
    if (n > 0) {
        long long j = 0;
        while (j < n) {
            putchar((int)(unsigned char)out[j]);
            j = j + 1;
        }
        putchar(10);
    }
    return n;
}
#else
static long long fk_api_health(void) {
    return -1;
}
static long long fk_mesh_register(void) {
    return -1;
}
static long long fk_mesh_detect(void) {
    return -1;
}
#endif
/* ── GPU matvec on a real RTX via the CUDA DRIVER API (nvcuda.dll) — fkwu's OWN host carrier, the
 * CUDA twin of fk_metal_matvec_f32_native. No python, no nvcc, no NVRTC, no CUDA toolkit:
 * LoadLibraryA the driver, JIT the Form-emitted PTX (matvec.ptx = the four-way fptx-matvec) at
 * CU_JIT_OPTIMIZATION_LEVEL(=7)=0 so mul.f32/add.f32 stay UNFUSED (two roundings), dispatch one
 * thread per row, and compare BIT-EXACT to the CPU f32 downward right-fold (volatile blocks the CPU
 * FMA so both sides are two roundings). The driver's built-in PTX JIT is intrinsic to the GPU. */
#if defined(_WIN32)
/* one dispatch, shared by the fixture witness (tag 232) and the general data door (tag 233):
 * load the driver, JIT the Form-emitted PTX at -O0 (unfused), y = W.x one thread per row, then
 * TEAR DOWN (mem/module/context — the fixture used to leak these per call). Returns 0 and fills
 * y/gname on success; the negative step code of the first refusal otherwise. */
static long long fk_cuda_go(const float *W, const float *x, float *y, unsigned rows, unsigned cols,
                            char *gname, long long gcap) {
    void *h = LoadLibraryA("nvcuda.dll");
    if (h == 0) {
        return -1;
    }
    typedef int (*F1)(unsigned);
    typedef int (*F2)(int *, int);
    typedef int (*F3)(char *, int, int);
    typedef int (*F4)(void **, unsigned, int);
    typedef int (*F5)(void **, const void *, unsigned, int *, void **);
    typedef int (*F6)(void **, void *, const char *);
    typedef int (*F7)(unsigned long long *, unsigned long long);
    typedef int (*F8)(unsigned long long, const void *, unsigned long long);
    typedef int (*F9)(void *, unsigned long long, unsigned long long);
    typedef int (*F10)(void *, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned,
                       void *, void **, void **);
    typedef int (*F11)(void);
    typedef int (*F12)(unsigned long long);
    typedef int (*F13)(void *);
    F1 cuInit = (F1)GetProcAddress(h, "cuInit");
    F2 cuDeviceGet = (F2)GetProcAddress(h, "cuDeviceGet");
    F3 cuDeviceGetName = (F3)GetProcAddress(h, "cuDeviceGetName");
    F4 cuCtxCreate = (F4)GetProcAddress(h, "cuCtxCreate_v2");
    F5 cuModuleLoadDataEx = (F5)GetProcAddress(h, "cuModuleLoadDataEx");
    F6 cuModuleGetFunction = (F6)GetProcAddress(h, "cuModuleGetFunction");
    F7 cuMemAlloc = (F7)GetProcAddress(h, "cuMemAlloc_v2");
    F8 cuMemcpyHtoD = (F8)GetProcAddress(h, "cuMemcpyHtoD_v2");
    F9 cuMemcpyDtoH = (F9)GetProcAddress(h, "cuMemcpyDtoH_v2");
    F10 cuLaunchKernel = (F10)GetProcAddress(h, "cuLaunchKernel");
    F11 cuCtxSynchronize = (F11)GetProcAddress(h, "cuCtxSynchronize");
    F12 cuMemFree = (F12)GetProcAddress(h, "cuMemFree_v2");
    F13 cuModuleUnload = (F13)GetProcAddress(h, "cuModuleUnload");
    F13 cuCtxDestroy = (F13)GetProcAddress(h, "cuCtxDestroy_v2");
    if (!cuInit || !cuCtxCreate || !cuModuleLoadDataEx || !cuLaunchKernel) {
        return -2;
    }
    if (cuInit(0) != 0) {
        return -3;
    }
    int dev = 0;
    if (cuDeviceGet(&dev, 0) != 0) {
        return -4;
    }
    if (gname != 0 && gcap > 0) {
        gname[0] = 0;
        cuDeviceGetName(gname, (int)gcap, dev);
    }
    void *ctx = 0;
    if (cuCtxCreate(&ctx, 0, dev) != 0) {
        return -5;
    }
    long long rc = 0;
    void *mod = 0;
    unsigned long long dW = 0, dX = 0, dY = 0;
    int fd = open("gpu/fptx-matvec.ptx", 0x8000);
    if (fd < 0) {
        rc = -6;
    } else {
        static char ptx[131072];
        long long pn = 0, g;
        while ((g = read(fd, ptx + pn, 8192)) > 0) {
            pn = pn + g;
            if (pn > 120000) {
                break;
            }
        }
        close(fd);
        ptx[pn] = 0;
        int jopt[1];
        void *jval[1];
        jopt[0] = 7;
        jval[0] = (void *)0;
        /* CU_JIT_OPTIMIZATION_LEVEL = 0 */
        if (cuModuleLoadDataEx(&mod, ptx, 1, jopt, jval) != 0) {
            rc = -7;
            mod = 0;
        }
    }
    void *fn = 0;
    if (rc == 0 && cuModuleGetFunction(&fn, mod, "form_matvec_f32") != 0) {
        rc = -8;
    }
    if (rc == 0) {
        cuMemAlloc(&dW, (unsigned long long)rows * cols * 4);
        cuMemcpyHtoD(dW, W, (unsigned long long)rows * cols * 4);
        cuMemAlloc(&dX, (unsigned long long)cols * 4);
        cuMemcpyHtoD(dX, x, (unsigned long long)cols * 4);
        cuMemAlloc(&dY, (unsigned long long)rows * 4);
        cuMemcpyHtoD(dY, y, (unsigned long long)rows * 4);
        void *args[5];
        args[0] = &dW;
        args[1] = &dX;
        args[2] = &dY;
        args[3] = &rows;
        args[4] = &cols;
        unsigned blocks = (rows + 255) / 256;
        if (cuLaunchKernel(fn, blocks, 1, 1, 256, 1, 1, 0, 0, args, 0) != 0) {
            rc = -9;
        } else {
            cuCtxSynchronize();
            cuMemcpyDtoH(y, dY, (unsigned long long)rows * 4);
        }
    }
    if (cuMemFree != 0) {
        if (dW != 0) {
            cuMemFree(dW);
        }
        if (dX != 0) {
            cuMemFree(dX);
        }
        if (dY != 0) {
            cuMemFree(dY);
        }
    }
    if (mod != 0 && cuModuleUnload != 0) {
        cuModuleUnload(mod);
    }
    if (cuCtxDestroy != 0) {
        cuCtxDestroy(ctx);
    }
    return rc;
}
static void fk_cuda_say(long long rc) {
    if (rc == -1) {
        printf("cuda: nvcuda.dll not found\n");
    } else if (rc == -2) {
        printf("cuda: missing entry points\n");
    } else if (rc == -3) {
        printf("cuda: cuInit failed\n");
    } else if (rc == -5) {
        printf("cuda: ctx create failed\n");
    } else if (rc == -6) {
        printf("cuda: gpu/fptx-matvec.ptx not found\n");
    } else if (rc == -7) {
        printf("cuda: PTX JIT failed\n");
    } else if (rc == -8) {
        printf("cuda: no form_matvec_f32\n");
    } else if (rc == -9) {
        printf("cuda: launch failed\n");
    }
}
/* the fixture witness (tag 232): fixed 3x4 W and 4-vec x, bit-exact vs the CPU two-rounding fold. */
static long long fk_cuda_matvec(void) {
    unsigned rows = 3, cols = 4;
    float W[12] = {0.1f,  0.2f,  0.3f, 0.7f, 0.11f,     0.13f,
                   0.17f, 0.19f, 0.9f, 0.8f, 0.123456f, 0.654321f};
    float X[4] = {0.5f, 0.25f, 0.125f, 0.333333f};
    float Y[3] = {0.0f, 0.0f, 0.0f};
    char gname[256];
    long long rc = fk_cuda_go(W, X, Y, rows, cols, gname, 256);
    if (rc < 0) {
        fk_cuda_say(rc);
        return rc;
    }
    printf("GPU: %s  CUDA driver-API PTX-JIT -O0 (nvcuda.dll; no python, no nvcc, no nvrtc)\n",
           gname);
    printf(
        "recipe: form-ptx fptx-matvec -> form_matvec_f32 (f32 matvec, downward right-fold, 2 roundings)\n");
    long long agree = 0;
    long long i;
    for (i = 0; i < (long long)rows; i = i + 1) {
        volatile float acc = 0.0f;
        long long j;
        for (j = (long long)cols - 1; j >= 0; j = j - 1) {
            volatile float prod = W[i * (long long)cols + j] * X[j];
            acc = prod + acc;
        }
        float cpu = acc;
        float gpu = Y[i];
        union {
            float f;
            unsigned u;
        } cg, cc;
        cg.f = gpu;
        cc.f = cpu;
        int ex = (cg.u == cc.u);
        if (ex) {
            agree = agree + 1;
        }
        printf("  row %lld: GPU=%.9g (0x%08x)  CPU=%.9g (0x%08x)  %s\n", i, (double)gpu, cg.u,
               (double)cpu, cc.u, ex ? "BIT-EXACT" : "DIFF");
    }
    printf("AGREEMENT: %lld/%u  ALL-BIT-EXACT=%s\n", agree, rows,
           (agree == (long long)rows) ? "true" : "false");
    return agree;
}
/* the GENERAL data door (tag 233) the RTX receipts named as the missing rung:
 * (cuda_matvec_f32 W x) — W a flat Form list of rows*cols numbers, x a list of cols numbers.
 * Dispatches y = W.x on the GPU through the same Form-emitted PTX, recomputes the same downward
 * two-rounding f32 fold on the CPU (volatile blocks FMA), and returns (agree y0 .. y_rows-1):
 * the metal receipt rides IN the returned value, not beside it. nil (empty list) on refusal. */
static long long fk_cons_val(long long h, long long t);
static long long fk_list_len_c(long long v) {
    long long p = v >> 1;
    long long n = 0;
    while (p >= 1 && FK_POK(p)) {
        n = n + 1;
        p = FK_HT(p) >> 1;
    }
    return n;
}
static long long fk_list_to_f32(long long v, float *out, long long cap) {
    long long p = v >> 1;
    long long n = 0;
    while (p >= 1 && FK_POK(p) && n < cap) {
        out[n] = (float)fk_num(FK_HH(p));
        n = n + 1;
        p = FK_HT(p) >> 1;
    }
    return n;
}
static long long fk_cuda_matvec_f32(long long wv, long long xv) {
    long long wn = fk_list_len_c(wv);
    long long xn = fk_list_len_c(xv);
    if (xn < 1 || wn < xn || (wn % xn) != 0 || wn > 4194304) {
        return 1;
    }
    unsigned cols = (unsigned)xn;
    unsigned rows = (unsigned)(wn / xn);
    float *W = malloc((unsigned long)(wn * 4));
    float *x = malloc((unsigned long)(xn * 4));
    float *y = malloc((unsigned long)(rows * 4));
    if (W == 0 || x == 0 || y == 0) {
        free(W);
        free(x);
        free(y);
        return 1;
    }
    fk_list_to_f32(wv, W, wn);
    fk_list_to_f32(xv, x, xn);
    long long i;
    for (i = 0; i < (long long)rows; i = i + 1) {
        y[i] = 0.0f;
    }
    char gname[256];
    long long rc = fk_cuda_go(W, x, y, rows, cols, gname, 256);
    if (rc < 0) {
        fk_cuda_say(rc);
        free(W);
        free(x);
        free(y);
        return 1;
    }
    long long agree = 0;
    for (i = 0; i < (long long)rows; i = i + 1) {
        volatile float acc = 0.0f;
        long long j;
        for (j = (long long)cols - 1; j >= 0; j = j - 1) {
            volatile float prod = W[i * (long long)cols + j] * x[j];
            acc = prod + acc;
        }
        float cpu = acc;
        union {
            float f;
            unsigned u;
        } cg, cc;
        cg.f = y[i];
        cc.f = cpu;
        if (cg.u == cc.u) {
            agree = agree + 1;
        }
    }
    printf("GPU: %s  cuda_matvec_f32 rows=%u cols=%u  BIT-EXACT %lld/%u\n", gname, rows, cols,
           agree, rows);
    long long lst = 1;
    i = rows;
    while (i > 0) {
        i = i - 1;
        lst = fk_cons_val(fk_fbox((double)y[i]), lst);
    }
    lst = fk_cons_val(agree << 1, lst);
    free(W);
    free(x);
    free(y);
    return lst;
}
#else
static long long fk_cuda_matvec(void) {
    return -1;
}
static long long fk_cuda_matvec_f32(long long wv, long long xv) {
    (void)wv;
    (void)xv;
    return 1;
}
#endif
static int fk_sock_getaddrinfo(const char *h, const char *p, const struct addrinfo *i,
                               struct addrinfo **r) {
    fk_sock_boot();
    return getaddrinfo(h, p, i, r);
}
static int fk_sock_socket(int af, int ty, int pr) {
    fk_sock_boot();
    fk_os_socket_t s = socket(af, ty, pr);
    if (!fk_os_socket_ok(s)) {
        return -1;
    }
    return (int)s;
}
static int fk_sock_connect(int fd, const void *a, unsigned int n) {
    fk_sock_boot();
    return connect((fk_os_socket_t)(unsigned int)fd, a, n);
}
static int fk_sock_close(int fd) {
    return fk_os_close_socket((fk_os_socket_t)(unsigned int)fd);
}
static long long fk_sock_read(int fd, void *buf, unsigned long n) {
    return fk_os_recv_socket((fk_os_socket_t)(unsigned int)fd, buf, n);
}
static long long fk_sock_write(int fd, const void *buf, unsigned long n) {
    return fk_os_send_socket((fk_os_socket_t)(unsigned int)fd, buf, n);
}
struct timeval {
    long tv_sec;
    int tv_usec;
};
extern int gettimeofday(struct timeval *, void *);
static long long fk_now_ms(void) {
    struct timeval tv;
    if (gettimeofday(&tv, 0) != 0) {
        return 0;
    }
    return ((long long)tv.tv_sec * 1000LL) + ((long long)tv.tv_usec / 1000LL);
}
static long long fk_elapsed_ms(long long start) {
    long long end = fk_now_ms();
    if (start <= 0 || end <= start) {
        return 1;
    }
    return end - start;
}
static void fk_arena(void);
static long long fk_cons_val(long long h, long long t) {
    if (fk_cap == 0) {
        fk_arena();
    }
    if (fk_hp + 1 >= fk_cap) {
        /* cannot melt here (live C-local intermediates are not on the value
         * stack for the collector to trace) -- but growth relocates nothing. */
        fk_heap_grow();
    }
    fk_hp = fk_hp + 1;
    fk_hh[fk_hp] = h;
    fk_ht[fk_hp] = t;
    return (fk_hp << 1) | 1;
}
static long long fk_http_dict(long long status, long long body, long long err) {
    long long d = 1;
    d = fk_cons_val(1, d);
    d = fk_cons_val(fk_sbuf("headers", 7), d);
    d = fk_cons_val(0, d);
    d = fk_cons_val(fk_sbuf("duration_ms", 11), d);
    d = fk_cons_val(err, d);
    d = fk_cons_val(fk_sbuf("error", 5), d);
    d = fk_cons_val(body, d);
    d = fk_cons_val(fk_sbuf("body", 4), d);
    d = fk_cons_val(status << 1, d);
    d = fk_cons_val(fk_sbuf("status_code", 11), d);
    d = fk_cons_val(fk_sbuf("__dict__", 8), d);
    return d;
}
static int fk_starts(const char *s, const char *p) {
    long long i = 0;
    while (p[i] != 0) {
        if (s[i] != p[i]) {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}
static long long fk_http_status(const char *buf, long long n) {
    long long i = 0;
    while (i < n && buf[i] != FK_CH_SPACE) {
        i = i + 1;
    }
    while (i < n && buf[i] == FK_CH_SPACE) {
        i = i + 1;
    }
    long long v = 0;
    while (i < n && buf[i] >= FK_CH_DIGIT0 && buf[i] <= FK_CH_DIGIT9) {
        v = v * 10 + (buf[i] - FK_CH_DIGIT0);
        i = i + 1;
    }
    return v;
}
static long long fk_http_body_offset(const char *buf, long long n) {
    long long i = 0;
    while (i + 3 < n) {
        if (buf[i] == FK_CH_CR && buf[i + 1] == FK_CH_LF && buf[i + 2] == FK_CH_CR &&
            buf[i + 3] == FK_CH_LF) {
            return i + 4;
        }
        i = i + 1;
    }
    i = 0;
    while (i + 1 < n) {
        if (buf[i] == FK_CH_LF && buf[i + 1] == FK_CH_LF) {
            return i + 2;
        }
        i = i + 1;
    }
    return n;
}
static long long fk_http_headers(const char *, long long, long long);
static long long fk_http_dict_with_headers(long long, long long, long long, long long, long long);
static long long fk_http_append_request_headers(char *, long long, long long, long long);
/* The whole GET request -- request line, Host, Connection, every well-formed
 * caller header, the blank line -- in one heap buffer sized by a measuring
 * pass, so a header of any length and any count reaches the wire. The caller
 * frees it; *out_n is the byte count. */
static long long fk_path_len(const char *p);
static char *fk_http_build_request(const char *path, const char *host, long long headersv,
                                   long long *out_n) {
    long long base = 42 + fk_path_len(path) + fk_path_len(host);
    long long cap = fk_http_append_request_headers(0, base, 0, headersv) + 3;
    char *req = (char *)malloc((unsigned long)cap);
    long long rn;
    if (req == 0) {
        fk_die("fk_http_build_request: out of memory for the request");
    }
    rn = sprintf(req, "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n", path, host);
    rn = fk_http_append_request_headers(req, rn, cap, headersv);
    req[rn] = 13;
    req[rn + 1] = 10;
    rn = rn + 2;
    req[rn] = 0;
    *out_n = rn;
    return req;
}
static long long fk_http_get_plain(long long urlv, long long headersv, long long timeoutv) {
    (void)timeoutv;
    long long started = fk_now_ms();
    char url[2048];
    char host[512];
    char path[1536];
    char port[16];
    fk_cstr(urlv, url, 2048);
    if (!fk_starts(url, "http://")) {
        return fk_http_dict(0, fk_sbuf("", 0),
                            fk_sbuf("http_get: fkwu floor supports http:// only", 41));
    }
    long long p = 7;
    long long h = 0;
    while (url[p] != 0 && url[p] != FK_CH_SLASH && url[p] != FK_CH_COLON && h < 511) {
        host[h] = url[p];
        h = h + 1;
        p = p + 1;
    }
    host[h] = 0;
    port[0] = 56;
    port[1] = 48;
    port[2] = 0;
    if (url[p] == FK_CH_COLON) {
        p = p + 1;
        long long pi = 0;
        while (url[p] >= FK_CH_DIGIT0 && url[p] <= FK_CH_DIGIT9 && pi < 15) {
            port[pi] = url[p];
            pi = pi + 1;
            p = p + 1;
        }
        port[pi] = 0;
    }
    long long q = 0;
    if (url[p] == FK_CH_SLASH) {
        while (url[p] != 0 && q < 1535) {
            path[q] = url[p];
            q = q + 1;
            p = p + 1;
        }
    } else {
        path[q] = 47;
        q = 1;
    }
    path[q] = 0;
    if (h == 0) {
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: empty host", 20));
    }
    struct addrinfo hints;
    hints.ai_flags = 0;
    hints.ai_family = 0;
    hints.ai_socktype = 1;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_canonname = 0;
    hints.ai_addr = 0;
    hints.ai_next = 0;
    struct addrinfo *res = 0;
    if (getaddrinfo(host, port, &hints, &res) != 0 || res == 0) {
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: resolve failed", 24));
    }
    int fd = -1;
    struct addrinfo *rp = res;
    while (rp != 0) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd >= 0) {
            if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
                break;
            }
            close(fd);
            fd = -1;
        }
        rp = rp->ai_next;
    }
    freeaddrinfo(res);
    if (fd < 0) {
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: connect failed", 24));
    }
    long long rn = 0;
    char *req = fk_http_build_request(path, host, headersv, &rn);
    long long wr = 0;
    while (wr < rn) {
        long long nwr = write(fd, req + wr, rn - wr);
        if (nwr <= 0) {
            free(req);
            close(fd);
            return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: write failed", 22));
        }
        wr = wr + nwr;
    }
    free(req);
    /* Heap-grown: the old static 64KB amputated responses past 65,535 bytes —
     * a shortwhole in the success path, the wall the chunked socket lane was
     * working around (healed 2026-08-27, same family as host-exec's cap). */
    long long rcap = 65536;
    char *resp = malloc(rcap);
    if (resp == 0) {
        fk_die("fk_http_get_plain: out of memory for response buffer");
    }
    long long total = 0;
    for (;;) {
        if (total + 65536 + 1 > rcap) {
            rcap = rcap * 2;
            resp = realloc(resp, rcap);
            if (resp == 0) {
                fk_die("fk_http_get_plain: out of memory growing response buffer");
            }
        }
        long long got = read(fd, resp + total, 65536);
        if (got <= 0) {
            break;
        }
        total = total + got;
    }
    close(fd);
    resp[total] = 0;
    long long status = fk_http_status(resp, total);
    long long bo = fk_http_body_offset(resp, total);
    if (bo > total) {
        bo = total;
    }
    long long ans = fk_http_dict_with_headers(status, fk_http_headers(resp, total, bo),
                                              fk_sbuf(resp + bo, total - bo), fk_sbuf("", 0),
                                              fk_elapsed_ms(started));
    free(resp);
    return ans;
}
static int fk_http_lit_eq_ci(const char *buf, long long n, const char *lit) {
    long long i = 0;
    while (i < n && lit[i] != 0) {
        char a = buf[i];
        char b = lit[i];
        if (a >= FK_CH_UPPER_A && a <= FK_CH_UPPER_Z) {
            a = a + (FK_CH_LOWER_A - FK_CH_UPPER_A);
        }
        if (b >= FK_CH_UPPER_A && b <= FK_CH_UPPER_Z) {
            b = b + (FK_CH_LOWER_A - FK_CH_UPPER_A);
        }
        if (a != b) {
            return 0;
        }
        i = i + 1;
    }
    return i == n && lit[i] == 0;
}
/* A header is refused for its SHAPE (token bytes only in the name; no CR, LF,
 * NUL or control bytes in the value -- the injection guard) or for naming a
 * hop-by-hop field the kernel owns. Never for its length: the request buffer
 * is sized to fit whatever the caller hands over. */
static int fk_http_header_name_ok(const char *buf, long long n) {
    if (n <= 0) {
        return 0;
    }
    long long i = 0;
    while (i < n) {
        unsigned char c = (unsigned char)buf[i];
        if (!((c >= FK_CH_UPPER_A && c <= FK_CH_UPPER_Z) ||
              (c >= FK_CH_LOWER_A && c <= FK_CH_LOWER_Z) ||
              (c >= FK_CH_DIGIT0 && c <= FK_CH_DIGIT9) || c == FK_CH_DASH)) {
            return 0;
        }
        i = i + 1;
    }
    if (fk_http_lit_eq_ci(buf, n, "host") || fk_http_lit_eq_ci(buf, n, "connection") ||
        fk_http_lit_eq_ci(buf, n, "content-length") ||
        fk_http_lit_eq_ci(buf, n, "transfer-encoding") ||
        fk_http_lit_eq_ci(buf, n, "proxy-connection") || fk_http_lit_eq_ci(buf, n, "keep-alive") ||
        fk_http_lit_eq_ci(buf, n, "upgrade") || fk_http_lit_eq_ci(buf, n, "te") ||
        fk_http_lit_eq_ci(buf, n, "trailer")) {
        return 0;
    }
    return 1;
}
static int fk_http_header_value_ok(const char *buf, long long n) {
    if (n < 0) {
        return 0;
    }
    long long i = 0;
    while (i < n) {
        unsigned char c = (unsigned char)buf[i];
        if (c == FK_CH_NUL || c == FK_CH_LF || c == FK_CH_CR || c == FK_CH_DEL) {
            return 0;
        }
        if (c < FK_CH_SPACE && c != FK_CH_TAB) {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}
static long long fk_http_append_bytes(char *out, long long pos, long long cap, const char *buf,
                                      long long n) {
    long long i = 0;
    while (i < n && pos + 1 < cap) {
        out[pos] = buf[i];
        pos = pos + 1;
        i = i + 1;
    }
    return pos;
}
/* Appends every well-formed caller header (a (43001 name value) row) to req.
 * With req == 0 it MEASURES instead: it answers the byte position the same
 * headers would reach, so fk_http_build_request can size the request exactly
 * and no header is dropped for not fitting (the old fixed buffer skipped any
 * header past its brim, and any header past the 64th, without a word). */
static long long fk_http_append_request_headers(char *req, long long rn, long long cap,
                                                long long headersv) {
    long long q = headersv >> 1;
    while (q >= 1 && FK_POK(q)) {
        long long row = FK_HH(q);
        if ((row & 1) != 0) {
            long long rp = row >> 1;
            if (rp >= 1 && FK_POK(rp) && ((FK_HH(rp) >> 1) == 43001)) {
                long long np = FK_HT(rp) >> 1;
                if (np >= 1 && FK_POK(np)) {
                    long long namev = FK_HH(np);
                    long long vp = FK_HT(np) >> 1;
                    if (vp >= 1 && FK_POK(vp)) {
                        long long valuev = FK_HH(vp);
                        long long ns = fk_stri(namev);
                        long long vs = fk_stri(valuev);
                        if (fk_is_str(namev) && fk_is_str(valuev) && ns >= 0 && FK_SOK(ns) &&
                            vs >= 0 && FK_SOK(vs)) {
                            const char *name = FK_SBYTES(ns);
                            const char *value = FK_SBYTES(vs);
                            long long nl = FK_SLEN(ns);
                            long long vl = FK_SLEN(vs);
                            if (fk_http_header_name_ok(name, nl) &&
                                fk_http_header_value_ok(value, vl)) {
                                if (req == 0) {
                                    rn = rn + nl + vl + 4;
                                } else {
                                    rn = fk_http_append_bytes(req, rn, cap, name, nl);
                                    rn = fk_http_append_bytes(req, rn, cap, ": ", 2);
                                    rn = fk_http_append_bytes(req, rn, cap, value, vl);
                                    rn = fk_http_append_bytes(req, rn, cap, "\r\n", 2);
                                }
                            }
                        }
                    }
                }
            }
        }
        q = FK_HT(q) >> 1;
    }
    return rn;
}
static long long fk_http_header_row(const char *name, long long nl, const char *value,
                                    long long vl) {
    long long row = 1;
    row = fk_cons_val(fk_sbuf(value, vl), row);
    row = fk_cons_val(fk_sbuf(name, nl), row);
    row = fk_cons_val(43001LL << 1, row);
    return row;
}
static long long fk_http_headers(const char *buf, long long n, long long bo) {
    long long out = 1;
    long long end = bo;
    if (end > n) {
        end = n;
    }
    long long i = 0;
    while (i < end && buf[i] != FK_CH_LF) {
        i = i + 1;
    }
    if (i < end && buf[i] == FK_CH_LF) {
        i = i + 1;
    }
    long long count = 0;
    while (i < end && count < 128) {
        if (buf[i] == FK_CH_CR || buf[i] == FK_CH_LF) {
            break;
        }
        long long ls = i;
        while (i < end && buf[i] != FK_CH_LF) {
            i = i + 1;
        }
        long long le = i;
        if (le > ls && buf[le - 1] == FK_CH_LF) {
            le = le - 1;
        }
        if (le > ls && buf[le - 1] == FK_CH_CR) {
            le = le - 1;
        }
        long long colon = ls;
        while (colon < le && buf[colon] != FK_CH_COLON) {
            colon = colon + 1;
        }
        if (colon < le && colon > ls) {
            long long ns = ls;
            long long ne = colon;
            while (ne > ns && (buf[ne - 1] == FK_CH_SPACE || buf[ne - 1] == FK_CH_TAB)) {
                ne = ne - 1;
            }
            long long vs = colon + 1;
            while (vs < le && (buf[vs] == FK_CH_SPACE || buf[vs] == FK_CH_TAB)) {
                vs = vs + 1;
            }
            long long ve = le;
            while (ve > vs && (buf[ve - 1] == FK_CH_SPACE || buf[ve - 1] == FK_CH_TAB)) {
                ve = ve - 1;
            }
            if (ne > ns) {
                out = fk_cons_val(fk_http_header_row(buf + ns, ne - ns, buf + vs, ve - vs), out);
                count = count + 1;
            }
        }
        if (i < end && buf[i] == FK_CH_LF) {
            i = i + 1;
        }
    }
    return out;
}
static long long fk_http_dict_with_headers(long long status, long long headers, long long body,
                                           long long err, long long duration) {
    long long d = 1;
    d = fk_cons_val(headers, d);
    d = fk_cons_val(fk_sbuf("headers", 7), d);
    d = fk_cons_val(duration << 1, d);
    d = fk_cons_val(fk_sbuf("duration_ms", 11), d);
    d = fk_cons_val(err, d);
    d = fk_cons_val(fk_sbuf("error", 5), d);
    d = fk_cons_val(body, d);
    d = fk_cons_val(fk_sbuf("body", 4), d);
    d = fk_cons_val(status << 1, d);
    d = fk_cons_val(fk_sbuf("status_code", 11), d);
    d = fk_cons_val(fk_sbuf("__dict__", 8), d);
    return d;
}
/* one 64KB read into host-exec's heap-grown output buffer; answers read's own
 * count. Heap-grown because the old static 256KB silently amputated a
 * command's output past 262,143 bytes — a partial answer wearing a whole one's
 * skin (healed 2026-08-27). Growth is the capability answer, never a cap. */
static long long fk_host_exec_read(int fd, char **hbuf, long long *hcap, long long *total) {
    if (*total + 65536 > *hcap) {
        *hcap = *hcap * 2;
        *hbuf = realloc(*hbuf, (unsigned long)*hcap);
        if (*hbuf == 0) {
            fk_die("fk_host_exec: out of memory growing output buffer");
        }
    }
    long long got = read(fd, *hbuf + *total, 65536);
    if (got > 0) {
        *total = *total + got;
    }
    return got;
}
/* (host-exec cmd input): cmd runs under `sh -c`, input is the child's stdin,
 * the answer is the child's stdout. A launch that never happened (pipe or fork
 * refused: starvation) answers the axiom-1 nothing, never "" — "" means the
 * command RAN and spoke zero bytes (2026-08-27: ten starved calls once
 * answered ten empty strings rc 0 and a 10s window closed in 0.008s). A
 * nonzero exit still answers whatever the command spoke. Callers name the
 * absence with nothing? before measuring — str_len of nothing dies loud.
 *
 * The command is heap-built (fk_cstr_heap): the old char cmd[8192] refused a
 * 368-path list at the membrane, so pipelines composed in the shell instead
 * of crossing. And input used to be `(void)inputv` — discarded — so
 * (host-exec "cat" "hello") answered "" (R69). */
static long long fk_host_exec(long long cmdv, long long inputv) {
    char *cmd = fk_cstr_heap(cmdv);
    long long in_sa = fk_stri(inputv);
    long long in_n = (in_sa >= 0 && FK_SOK(in_sa)) ? FK_SLEN(in_sa) : 0;
    long long hcap = 262144;
    char *hbuf = malloc(hcap);
    if (hbuf == 0) {
        fk_die("fk_host_exec: out of memory for output buffer");
    }
    long long total = 0;
#if defined(_WIN32)
    if (in_n > 0) {
        fk_die("fkwu: host-exec: stdin input is not wired on this platform yet -- the command would run without the bytes it was handed");
    }
    void *fp = popen(cmd, "r");
    free(cmd);
    if (fp == 0) {
        free(hbuf);
        return fk_nothing;
    }
    while (fk_host_exec_read(fileno(fp), &hbuf, &hcap, &total) > 0) {
    }
    pclose(fp);
#else
    /* popen is one-directional; a pipe pair + fork/exec of `sh -c` keeps its
     * shell semantics and opens the stdin door. An empty input leaves the
     * child's stdin inherited, exactly as popen did. */
    int out_fds[2];
    int in_fds[2];
    in_fds[0] = -1;
    in_fds[1] = -1;
    if (pipe(out_fds) != 0) {
        free(cmd);
        free(hbuf);
        return fk_nothing;
    }
    if (in_n > 0 && pipe(in_fds) != 0) {
        close(out_fds[0]);
        close(out_fds[1]);
        free(cmd);
        free(hbuf);
        return fk_nothing;
    }
    long long pid = fork();
    if (pid < 0) {
        close(out_fds[0]);
        close(out_fds[1]);
        if (in_n > 0) {
            close(in_fds[0]);
            close(in_fds[1]);
        }
        free(cmd);
        free(hbuf);
        return fk_nothing;
    }
    if (pid == 0) {
        close(out_fds[0]);
        dup2(out_fds[1], 1);
        close(out_fds[1]);
        if (in_n > 0) {
            close(in_fds[1]);
            dup2(in_fds[0], 0);
            close(in_fds[0]);
        }
        char *child_argv[4];
        child_argv[0] = (char *)"sh";
        child_argv[1] = (char *)"-c";
        child_argv[2] = cmd;
        child_argv[3] = 0;
        execvp("/bin/sh", child_argv);
        _exit(127);
    }
    free(cmd);
    close(out_fds[1]);
    if (in_n > 0) {
        close(in_fds[0]);
    }
    /* Feed stdin and read stdout INTERLEAVED under poll: a child that speaks
     * before it has finished listening fills its stdout pipe and stops, and a
     * parent that wrote all its input first would then wait forever — the
     * deadlock every write-everything-first shape carries. A child that closes
     * its stdin early (it never wanted the bytes) shows as POLLERR/POLLHUP on
     * the write end and that door simply closes; SIGPIPE is held off around the
     * write so the race between that poll and the write cannot kill fkwu. */
    int out_fd = out_fds[0];
    int in_fd = (in_n > 0) ? in_fds[1] : -1;
    long long sent = 0;
    while (out_fd >= 0) {
        struct pollfd pfd[2];
        pfd[0].fd = out_fd;
        pfd[0].events = POLLIN;
        pfd[0].revents = 0;
        pfd[1].fd = in_fd;
        pfd[1].events = POLLOUT;
        pfd[1].revents = 0;
        if (poll(pfd, (in_fd >= 0) ? 2 : 1, -1) < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
        if (in_fd >= 0 && pfd[1].revents != 0) {
            if ((pfd[1].revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
                close(in_fd);
                in_fd = -1;
            } else {
                long long chunk = in_n - sent;
                if (chunk > 65536) {
                    chunk = 65536;
                }
                void (*old_pipe)(int) = signal(SIGPIPE, SIG_IGN);
                long long put = write(in_fd, FK_SBYTES(in_sa) + sent, (unsigned long)chunk);
                signal(SIGPIPE, old_pipe);
                if (put > 0) {
                    sent = sent + put;
                } else if (put < 0 && errno != EINTR) {
                    close(in_fd);
                    in_fd = -1;
                }
                if (in_fd >= 0 && sent >= in_n) {
                    close(in_fd);
                    in_fd = -1;
                }
            }
        }
        if (pfd[0].revents != 0) {
            long long got = fk_host_exec_read(out_fd, &hbuf, &hcap, &total);
            if (got < 0 && errno == EINTR) {
                continue;
            }
            if (got <= 0) {
                close(out_fd);
                out_fd = -1;
            }
        }
    }
    if (in_fd >= 0) {
        close(in_fd);
    }
    {
        int st = 0;
        waitpid((int)pid, &st, 0);
    }
#endif
    long long ans = fk_sbuf(hbuf, total);
    free(hbuf);
    return ans;
}
static long long fk_sock_request(long long hostv, long long portv, long long reqv) {
    /* Never-connected (DNS failure, no reachable peer) answers the axiom-1 nothing,
     * never "" -- "" means a peer CONNECTED and spoke zero bytes. Same contract as
     * fk_host_exec above; silent error hides illness (2026-08-27). */
    char host[512];
    char port[16];
    fk_cstr(hostv, host, 512);
    fk_cstr(portv, port, 16);
    long long rsa = fk_stri(reqv);
    long long rlen = (rsa >= 0 && FK_SOK(rsa)) ? FK_SLEN(rsa) : 0;
    struct addrinfo hints;
    hints.ai_flags = 0;
    hints.ai_family = 0;
    hints.ai_socktype = 1;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_canonname = 0;
    hints.ai_addr = 0;
    hints.ai_next = 0;
    struct addrinfo *res = 0;
    if (getaddrinfo(host, port, &hints, &res) != 0 || res == 0) {
        return fk_nothing;
    }
    int fd = -1;
    struct addrinfo *rp = res;
    while (rp != 0) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd >= 0) {
            if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
                break;
            }
            close(fd);
            fd = -1;
        }
        rp = rp->ai_next;
    }
    freeaddrinfo(res);
    if (fd < 0) {
        return fk_nothing;
    }
    const char *rptr = FK_SBYTES(rsa);
    long long wr = 0;
    while (wr < rlen) {
        long long nwr = write(fd, rptr + wr, rlen - wr);
        if (nwr <= 0) {
            break;
        }
        wr = wr + nwr;
    }
    /* Heap-grown: the old static 64KB amputated a peer's answer past 65,535
     * bytes — the wall http-client's fallback chain worked around instead of
     * this organ speaking whole (healed 2026-08-27). */
    long long rcap = 65536;
    char *resp = malloc(rcap);
    if (resp == 0) {
        fk_die("fk_sock_request: out of memory for response buffer");
    }
    long long total = 0;
    for (;;) {
        if (total + 65536 > rcap) {
            rcap = rcap * 2;
            resp = realloc(resp, rcap);
            if (resp == 0) {
                fk_die("fk_sock_request: out of memory growing response buffer");
            }
        }
        long long got = read(fd, resp + total, 65536);
        if (got <= 0) {
            break;
        }
        total = total + got;
    }
    close(fd);
    long long ans = fk_sbuf(resp, total);
    free(resp);
    return ans;
}
static long long fk_is_dict_value(long long v) {
    if ((v & 1) == 0) {
        return 0;
    }
    long long p = v >> 1;
    if (p < 1 || !FK_POK(p)) {
        return 0;
    }
    long long marker = fk_sbuf("__dict__", 8);
    long long h = FK_HH(p);
    if (!fk_is_str(h)) {
        return 0;
    }
    return fk_keyeq(fk_stri(h), fk_stri(marker));
}
static long long fk_get_value(long long target, long long key) {
    if (fk_is_dict_value(target)) {
        long long p = FK_HT(target >> 1) >> 1;
        long long ks = fk_stri(key);
        while (p >= 1 && FK_POK(p)) {
            long long k = FK_HH(p);
            long long vp = FK_HT(p) >> 1;
            if (vp < 1 || !FK_POK(vp)) {
                return 0;
            }
            if (fk_is_str(k) && fk_is_str(key) && fk_keyeq(fk_stri(k), ks)) {
                return FK_HH(vp);
            }
            p = FK_HT(vp) >> 1;
        }
        return 0;
    }
    if ((target & 1) != 0) {
        long long want = key >> 1;
        long long p = target >> 1;
        while (p >= 1 && FK_POK(p) && want > 0) {
            p = FK_HT(p) >> 1;
            want = want - 1;
        }
        if (p >= 1 && FK_POK(p)) {
            return FK_HH(p);
        }
    }
    return 0;
}
extern void *dlopen(const char *, int);
extern void *dlsym(void *, const char *);
typedef const void *(*fk_tls_method_fn)(void);
typedef void *(*fk_ctx_new_fn)(const void *);
typedef void (*fk_ctx_free_fn)(void *);
typedef void *(*fk_ssl_new_fn)(void *);
typedef void (*fk_ssl_free_fn)(void *);
typedef int (*fk_ssl_set_fd_fn)(void *, int);
typedef long (*fk_ssl_ctrl_fn)(void *, int, long, void *);
typedef int (*fk_ssl_set1_host_fn)(void *, const char *);
typedef int (*fk_ssl_connect_fn)(void *);
typedef int (*fk_ssl_write_fn)(void *, const void *, int);
typedef int (*fk_ssl_read_fn)(void *, void *, int);
typedef long (*fk_ssl_verify_result_fn)(const void *);
typedef void (*fk_ctx_set_verify_fn)(void *, int, void *);
typedef int (*fk_ctx_default_paths_fn)(void *);
static void *fk_ssl_lib(void) {
    static void *h = 0;
    if (h != 0) {
        return h;
    }
    dlopen("/opt/homebrew/opt/openssl@3/lib/libcrypto.3.dylib", 2);
    dlopen("/opt/homebrew/opt/openssl@3/lib/libcrypto.dylib", 2);
    dlopen("libcrypto.so.3", 2);
    h = dlopen("/opt/homebrew/opt/openssl@3/lib/libssl.3.dylib", 2);
    if (h == 0) {
        h = dlopen("/opt/homebrew/opt/openssl@3/lib/libssl.dylib", 2);
    }
    if (h == 0) {
        h = dlopen("libssl.so.3", 2);
    }
    if (h == 0) {
        h = dlopen("libssl.dylib", 2);
    }
    return h;
}
static long long fk_parse_url(const char *url, const char *scheme, long long start, char *host,
                              char *path, char *port, const char *default_port) {
    if (!fk_starts(url, scheme)) {
        return 0;
    }
    long long p = start;
    long long h = 0;
    while (url[p] != 0 && url[p] != FK_CH_SLASH && url[p] != FK_CH_COLON && h < 511) {
        host[h] = url[p];
        h = h + 1;
        p = p + 1;
    }
    host[h] = 0;
    long long pi = 0;
    while (default_port[pi] != 0 && pi < 15) {
        port[pi] = default_port[pi];
        pi = pi + 1;
    }
    port[pi] = 0;
    if (url[p] == FK_CH_COLON) {
        p = p + 1;
        pi = 0;
        while (url[p] >= FK_CH_DIGIT0 && url[p] <= FK_CH_DIGIT9 && pi < 15) {
            port[pi] = url[p];
            pi = pi + 1;
            p = p + 1;
        }
        port[pi] = 0;
    }
    long long q = 0;
    if (url[p] == FK_CH_SLASH) {
        while (url[p] != 0 && q < 1535) {
            path[q] = url[p];
            q = q + 1;
            p = p + 1;
        }
    } else {
        path[0] = 47;
        q = 1;
    }
    path[q] = 0;
    return h > 0;
}
static int fk_tcp_connect(const char *host, const char *port) {
    struct addrinfo hints;
    hints.ai_flags = 0;
    hints.ai_family = 0;
    hints.ai_socktype = 1;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_canonname = 0;
    hints.ai_addr = 0;
    hints.ai_next = 0;
    struct addrinfo *res = 0;
    if (getaddrinfo(host, port, &hints, &res) != 0 || res == 0) {
        return -1;
    }
    int fd = -1;
    struct addrinfo *rp = res;
    while (rp != 0) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd >= 0) {
            if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
                break;
            }
            close(fd);
            fd = -1;
        }
        rp = rp->ai_next;
    }
    freeaddrinfo(res);
    return fd;
}
static long long fk_https_get_ssl(long long urlv, long long headersv, long long timeoutv) {
    (void)timeoutv;
    long long started = fk_now_ms();
    char url[2048];
    char host[512];
    char path[1536];
    char port[16];
    fk_cstr(urlv, url, 2048);
    if (!fk_parse_url(url, "https://", 8, host, path, port, "443")) {
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: bad https url", 23));
    }
    void *lib = fk_ssl_lib();
    if (lib == 0) {
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: libssl missing", 24));
    }
    fk_tls_method_fn TLS_client_method = (fk_tls_method_fn)dlsym(lib, "TLS_client_method");
    fk_ctx_new_fn SSL_CTX_new = (fk_ctx_new_fn)dlsym(lib, "SSL_CTX_new");
    fk_ctx_free_fn SSL_CTX_free = (fk_ctx_free_fn)dlsym(lib, "SSL_CTX_free");
    fk_ssl_new_fn SSL_new = (fk_ssl_new_fn)dlsym(lib, "SSL_new");
    fk_ssl_free_fn SSL_free = (fk_ssl_free_fn)dlsym(lib, "SSL_free");
    fk_ssl_set_fd_fn SSL_set_fd = (fk_ssl_set_fd_fn)dlsym(lib, "SSL_set_fd");
    fk_ssl_ctrl_fn SSL_ctrl = (fk_ssl_ctrl_fn)dlsym(lib, "SSL_ctrl");
    fk_ssl_set1_host_fn SSL_set1_host = (fk_ssl_set1_host_fn)dlsym(lib, "SSL_set1_host");
    fk_ssl_connect_fn SSL_connect = (fk_ssl_connect_fn)dlsym(lib, "SSL_connect");
    fk_ssl_write_fn SSL_write = (fk_ssl_write_fn)dlsym(lib, "SSL_write");
    fk_ssl_read_fn SSL_read = (fk_ssl_read_fn)dlsym(lib, "SSL_read");
    fk_ssl_verify_result_fn SSL_get_verify_result =
        (fk_ssl_verify_result_fn)dlsym(lib, "SSL_get_verify_result");
    fk_ctx_set_verify_fn SSL_CTX_set_verify =
        (fk_ctx_set_verify_fn)dlsym(lib, "SSL_CTX_set_verify");
    fk_ctx_default_paths_fn SSL_CTX_set_default_verify_paths =
        (fk_ctx_default_paths_fn)dlsym(lib, "SSL_CTX_set_default_verify_paths");
    if (TLS_client_method == 0 || SSL_CTX_new == 0 || SSL_CTX_free == 0 || SSL_new == 0 ||
        SSL_free == 0 || SSL_set_fd == 0 || SSL_ctrl == 0 || SSL_set1_host == 0 ||
        SSL_connect == 0 || SSL_write == 0 || SSL_read == 0 || SSL_get_verify_result == 0 ||
        SSL_CTX_set_verify == 0 || SSL_CTX_set_default_verify_paths == 0) {
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: ssl symbol missing", 28));
    }
    int fd = fk_tcp_connect(host, port);
    if (fd < 0) {
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: connect failed", 24));
    }
    void *ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == 0) {
        close(fd);
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: ctx failed", 20));
    }
    SSL_CTX_set_verify(ctx, 1, 0);
    SSL_CTX_set_default_verify_paths(ctx);
    void *ssl = SSL_new(ctx);
    if (ssl == 0) {
        SSL_CTX_free(ctx);
        close(fd);
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: ssl failed", 20));
    }
    SSL_ctrl(ssl, 55, 0, host);
    if (SSL_set1_host(ssl, host) != 1) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: host verify setup failed", 34));
    }
    if (SSL_set_fd(ssl, fd) != 1 || SSL_connect(ssl) != 1) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: tls connect failed", 28));
    }
    if (SSL_get_verify_result(ssl) != 0) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: tls verify failed", 27));
    }
    long long rn = 0;
    char *req = fk_http_build_request(path, host, headersv, &rn);
    long long wr = 0;
    while (wr < rn) {
        int nwr = SSL_write(ssl, req + wr, (int)(rn - wr));
        if (nwr <= 0) {
            free(req);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(fd);
            return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: tls write failed", 26));
        }
        wr = wr + nwr;
    }
    free(req);
    /* Heap-grown past the old static 64KB amputation (healed 2026-08-27) —
     * the wall the byte-range fetchers were verifying their way around. */
    long long rcap = 65536;
    char *resp = malloc(rcap);
    if (resp == 0) {
        fk_die("fk_https_get_ssl: out of memory for response buffer");
    }
    long long total = 0;
    for (;;) {
        if (total + 65536 + 1 > rcap) {
            rcap = rcap * 2;
            resp = realloc(resp, rcap);
            if (resp == 0) {
                fk_die("fk_https_get_ssl: out of memory growing response buffer");
            }
        }
        int got = SSL_read(ssl, resp + total, 65536);
        if (got <= 0) {
            break;
        }
        total = total + got;
    }
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(fd);
    resp[total] = 0;
    long long status = fk_http_status(resp, total);
    long long bo = fk_http_body_offset(resp, total);
    if (bo > total) {
        bo = total;
    }
    long long ans = fk_http_dict_with_headers(status, fk_http_headers(resp, total, bo),
                                              fk_sbuf(resp + bo, total - bo), fk_sbuf("", 0),
                                              fk_elapsed_ms(started));
    free(resp);
    return ans;
}
static long long fk_http_get_native(long long urlv, long long headersv, long long timeoutv) {
    char url[2048];
    fk_cstr(urlv, url, 2048);
    if (fk_starts(url, "https://")) {
        return fk_https_get_ssl(urlv, headersv, timeoutv);
    }
    return fk_http_get_plain(urlv, headersv, timeoutv);
}
static long long fk_tls_request(long long hostv, long long portv, long long reqv) {
    /* Every branch where no byte ever arrived from a VERIFIED peer -- no libssl,
     * missing symbols, TCP/TLS failure, and above all a certificate-verify
     * failure -- answers the axiom-1 nothing, never "": a trust verdict must not
     * wear the skin of a measured-empty body (2026-08-27). */
    char host[512];
    char port[16];
    fk_cstr(hostv, host, 512);
    fk_cstr(portv, port, 16);
    long long rsa = fk_stri(reqv);
    long long rlen = (rsa >= 0 && FK_SOK(rsa)) ? FK_SLEN(rsa) : 0;
    void *lib = fk_ssl_lib();
    if (lib == 0) {
        return fk_nothing;
    }
    fk_tls_method_fn TLS_client_method = (fk_tls_method_fn)dlsym(lib, "TLS_client_method");
    fk_ctx_new_fn SSL_CTX_new = (fk_ctx_new_fn)dlsym(lib, "SSL_CTX_new");
    fk_ctx_free_fn SSL_CTX_free = (fk_ctx_free_fn)dlsym(lib, "SSL_CTX_free");
    fk_ssl_new_fn SSL_new = (fk_ssl_new_fn)dlsym(lib, "SSL_new");
    fk_ssl_free_fn SSL_free = (fk_ssl_free_fn)dlsym(lib, "SSL_free");
    fk_ssl_set_fd_fn SSL_set_fd = (fk_ssl_set_fd_fn)dlsym(lib, "SSL_set_fd");
    fk_ssl_ctrl_fn SSL_ctrl = (fk_ssl_ctrl_fn)dlsym(lib, "SSL_ctrl");
    fk_ssl_set1_host_fn SSL_set1_host = (fk_ssl_set1_host_fn)dlsym(lib, "SSL_set1_host");
    fk_ssl_connect_fn SSL_connect = (fk_ssl_connect_fn)dlsym(lib, "SSL_connect");
    fk_ssl_write_fn SSL_write = (fk_ssl_write_fn)dlsym(lib, "SSL_write");
    fk_ssl_read_fn SSL_read = (fk_ssl_read_fn)dlsym(lib, "SSL_read");
    fk_ssl_verify_result_fn SSL_get_verify_result =
        (fk_ssl_verify_result_fn)dlsym(lib, "SSL_get_verify_result");
    fk_ctx_set_verify_fn SSL_CTX_set_verify =
        (fk_ctx_set_verify_fn)dlsym(lib, "SSL_CTX_set_verify");
    fk_ctx_default_paths_fn SSL_CTX_set_default_verify_paths =
        (fk_ctx_default_paths_fn)dlsym(lib, "SSL_CTX_set_default_verify_paths");
    if (TLS_client_method == 0 || SSL_CTX_new == 0 || SSL_CTX_free == 0 || SSL_new == 0 ||
        SSL_free == 0 || SSL_set_fd == 0 || SSL_ctrl == 0 || SSL_set1_host == 0 ||
        SSL_connect == 0 || SSL_write == 0 || SSL_read == 0 || SSL_get_verify_result == 0 ||
        SSL_CTX_set_verify == 0 || SSL_CTX_set_default_verify_paths == 0) {
        return fk_nothing;
    }
    int fd = fk_tcp_connect(host, port);
    if (fd < 0) {
        return fk_nothing;
    }
    void *ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == 0) {
        close(fd);
        return fk_nothing;
    }
    SSL_CTX_set_verify(ctx, 1, 0);
    SSL_CTX_set_default_verify_paths(ctx);
    void *ssl = SSL_new(ctx);
    if (ssl == 0) {
        SSL_CTX_free(ctx);
        close(fd);
        return fk_nothing;
    }
    SSL_ctrl(ssl, 55, 0, host);
    if (SSL_set1_host(ssl, host) != 1) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return fk_nothing;
    }
    if (SSL_set_fd(ssl, fd) != 1 || SSL_connect(ssl) != 1) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return fk_nothing;
    }
    if (SSL_get_verify_result(ssl) != 0) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return fk_nothing;
    }
    const char *rptr = FK_SBYTES(rsa);
    long long wr = 0;
    while (wr < rlen) {
        int nwr = SSL_write(ssl, rptr + wr, (int)(rlen - wr));
        if (nwr <= 0) {
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(fd);
            return fk_nothing;
        }
        wr = wr + nwr;
    }
    /* Heap-grown past the old static 64KB amputation (healed 2026-08-27):
     * a verified peer's answer arrives whole, however long it speaks. */
    long long rcap = 65536;
    char *resp = malloc(rcap);
    if (resp == 0) {
        fk_die("fk_tls_request: out of memory for response buffer");
    }
    long long total = 0;
    for (;;) {
        if (total + 65536 > rcap) {
            rcap = rcap * 2;
            resp = realloc(resp, rcap);
            if (resp == 0) {
                fk_die("fk_tls_request: out of memory growing response buffer");
            }
        }
        int got = SSL_read(ssl, resp + total, 65536);
        if (got <= 0) {
            break;
        }
        total = total + got;
    }
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(fd);
    long long ans = fk_sbuf(resp, total);
    free(resp);
    return ans;
}
static double fk_sqrt_d(double x) {
    if (x <= 0.0) {
        return 0.0;
    }
    double g = x >= 1.0 ? x : 1.0;
    long long i = 0;
    while (i < 32) {
        g = 0.5 * (g + x / g);
        i = i + 1;
    }
    return g;
}
static double fk_exp_d(double x) {
    double ln2 = 0.6931471805599453;
    long long n = (long long)(x / ln2);
    double r = x - ((double)n) * ln2;
    while (r > 0.34657359027997264) {
        r = r - ln2;
        n = n + 1;
    }
    while (r < -0.34657359027997264) {
        r = r + ln2;
        n = n - 1;
    }
    double term = 1.0;
    double sum = 1.0;
    long long k = 1;
    while (k <= 28) {
        term = term * r / (double)k;
        sum = sum + term;
        k = k + 1;
    }
    while (n > 0) {
        sum = sum * 2.0;
        n = n - 1;
    }
    while (n < 0) {
        sum = sum * 0.5;
        n = n + 1;
    }
    return sum;
}
static double fk_log_d(double x) {
    if (x <= 0.0) {
        return 0.0;
    }
    double ln2 = 0.6931471805599453;
    long long e = 0;
    while (x >= 2.0) {
        x = x * 0.5;
        e = e + 1;
    }
    while (x < 1.0) {
        x = x * 2.0;
        e = e - 1;
    }
    double z = (x - 1.0) / (x + 1.0);
    double z2 = z * z;
    double zp = z;
    double acc = 0.0;
    long long k = 0;
    while (k < 32) {
        acc = acc + zp / (double)(2 * k + 1);
        zp = zp * z2;
        k = k + 1;
    }
    return 2.0 * acc + ((double)e) * ln2;
}

/* CPython-compatible round(x, ndigits) for finite binary64 values, ndigits >= 0.
 *
 * Scaling in binary (x * 10^n) changes which side of a decimal half-way point
 * the stored value occupies.  The proof siblings exposed that defect in the
 * old tag-52 implementation.  A binary64 has a terminating decimal expansion
 * of at most 1074 fractional places, so the fixed 1074-place rendering is the
 * exact value.  Round that digit string half-to-even, then let strtod choose the
 * nearest binary64.  form/form-stdlib/tests/round-ndigits-band.fk is the owning
 * semantic witness; this C membrane can shrink when the native walker owns the
 * primitive directly.
 */
static double fk_round_ndigits_decimal(double x, long long nd) {
    const double max_finite = 1.7976931348623157e308;
    char exact[1536];
    char digits[1536];
    char decimal[1536];
    long long exact_n;
    long long point;
    long long digits_n;
    long long keep;
    long long kept_n;
    long long i;
    long long j;
    int neg;
    int round_up = 0;
    double ax;
    double out;

    if (x != x || x > max_finite || x < 0.0 - max_finite) {
        return x;
    }
    neg = x < 0.0 || (x == 0.0 && (1.0 / x) < 0.0);
    ax = neg ? 0.0 - x : x;
    if (nd < 0) {
        nd = 0;
    }
    if (nd >= 1074) {
        return x;
    }

    exact_n = sprintf(exact, "%.1074f", ax);
    point = 0;
    while (point < exact_n && exact[point] != '.') {
        point = point + 1;
    }
    digits_n = 0;
    i = 0;
    while (i < exact_n) {
        if (exact[i] != '.') {
            digits[digits_n] = exact[i];
            digits_n = digits_n + 1;
        }
        i = i + 1;
    }

    keep = point + nd;
    if (keep >= digits_n) {
        return x;
    }
    if (digits[keep] > '5') {
        round_up = 1;
    } else if (digits[keep] == '5') {
        i = keep + 1;
        while (i < digits_n && digits[i] == '0') {
            i = i + 1;
        }
        if (i < digits_n) {
            round_up = 1;
        } else if (keep > 0 && ((digits[keep - 1] - '0') & 1)) {
            round_up = 1;
        }
    }

    kept_n = keep;
    if (round_up) {
        if (kept_n == 0) {
            digits[0] = '1';
            kept_n = 1;
        } else {
            i = kept_n;
            while (i > 0 && digits[i - 1] == '9') {
                digits[i - 1] = '0';
                i = i - 1;
            }
            if (i > 0) {
                digits[i - 1] = digits[i - 1] + 1;
            } else {
                j = kept_n;
                while (j > 0) {
                    digits[j] = digits[j - 1];
                    j = j - 1;
                }
                digits[0] = '1';
                kept_n = kept_n + 1;
            }
        }
    } else if (kept_n == 0) {
        digits[0] = '0';
        kept_n = 1;
    }

    j = 0;
    if (neg) {
        decimal[j] = '-';
        j = j + 1;
    }
    if (nd == 0) {
        i = 0;
        while (i < kept_n) {
            decimal[j] = digits[i];
            j = j + 1;
            i = i + 1;
        }
    } else if (kept_n <= nd) {
        decimal[j] = '0';
        decimal[j + 1] = '.';
        j = j + 2;
        i = nd - kept_n;
        while (i > 0) {
            decimal[j] = '0';
            j = j + 1;
            i = i - 1;
        }
        i = 0;
        while (i < kept_n) {
            decimal[j] = digits[i];
            j = j + 1;
            i = i + 1;
        }
    } else {
        i = 0;
        while (i < kept_n) {
            if (i == kept_n - nd) {
                decimal[j] = '.';
                j = j + 1;
            }
            decimal[j] = digits[i];
            j = j + 1;
            i = i + 1;
        }
    }
    decimal[j] = 0;
    out = strtod(decimal, 0);
    if (out == 0.0 && neg) {
        return -0.0;
    }
    return out;
}
static double fk_dot_list(long long av, long long bv) {
    long long pa = av >> 1;
    long long pb = bv >> 1;
    double acc = 0.0;
    while (pa >= 1 && FK_POK(pa) && pb >= 1 && FK_POK(pb)) {
        acc = acc + fk_num(FK_HH(pa)) * fk_num(FK_HH(pb));
        pa = FK_HT(pa) >> 1;
        pb = FK_HT(pb) >> 1;
    }
    return acc;
}
static double fk_mag_list(long long av) {
    long long pa = av >> 1;
    double acc = 0.0;
    while (pa >= 1 && FK_POK(pa)) {
        double x = fk_num(FK_HH(pa));
        acc = acc + x * x;
        pa = FK_HT(pa) >> 1;
    }
    return fk_sqrt_d(acc);
}
#define FK_HASHCONS_INIT_CAP 4096 /* fk_hh/fk_ht: the hash-cons cell-pair store, initial size (fk_melt grows it) */
static void fk_arena(void) {
    fk_cap = FK_HASHCONS_INIT_CAP;
    fk_hh = (long long *)fk_store_take('h', FK_STORE_HEAP_PAIRS * 8);
    fk_ht = fk_hh == 0 ? 0 : (long long *)fk_store_take('t', FK_STORE_HEAP_PAIRS * 8);
    fk_heap_alt_h = fk_ht == 0 ? 0 : fk_store_take('H', FK_STORE_HEAP_PAIRS * 8);
    fk_heap_alt_t = fk_heap_alt_h == 0 ? 0 : fk_store_take('T', FK_STORE_HEAP_PAIRS * 8);
    if (fk_heap_alt_t == 0) {
        fk_store_go_private();
        fk_hh = malloc(fk_cap * 8);
        fk_ht = malloc(fk_cap * 8);
    }
    if (fk_hh == 0 || fk_ht == 0) {
        fk_die("fk_arena: out of memory");
    }
    fk_hh[0] = 1;
    fk_ht[0] = 1;
}
static long long *fk_fw;
static long long *fk_nh;
static long long *fk_nt;
static long long fk_nhp;
static void fk_mw(long long v) {
    char b[32];
    long long n = 0;
    if (v == 0) {
        b[0] = 48;
        n = 1;
    }
    while (v > 0) {
        b[n] = 48 + v % 10;
        v = v / 10;
        n = n + 1;
    }
    while (n > 0) {
        n = n - 1;
        write(2, b + n, 1);
    }
}
static void fk_mc(long long c) {
    char b = c;
    write(2, &b, 1);
}
static long long fk_mlive(long long b) {
    if ((b & 1) == 0) {
        return 0;
    }
    long long p = b >> 1;
    if (p >= FK_PAIR_BASE) { return 0; }
    if (p < 1 || !FK_POK(p)) {
        return 0;
    }
    if (fk_fw[p] != 0) {
        return 0;
    }
    fk_fw[p] = 0 - 1;
    return 1 + fk_mlive(FK_HT(p)) + fk_mlive(FK_HH(p));
}
static long long fk_mcopy(long long b) {
    if ((b & 1) == 0) {
        return b;
    }
    long long p = b >> 1;
    if (p >= FK_PAIR_BASE) { return b; }
    if (p < 1 || !FK_POK(p)) {
        return b;
    }
    if (fk_fw[p] > 0) {
        return (fk_fw[p] << 1) | 1;
    }
    long long t2 = fk_mcopy(FK_HT(p));
    long long h2 = fk_mcopy(FK_HH(p));
    fk_nhp = fk_nhp + 1;
    fk_nh[fk_nhp] = h2;
    fk_nt[fk_nhp] = t2;
    fk_fw[p] = fk_nhp;
    return (fk_nhp << 1) | 1;
}
static long long fk_nmelt;
/* fk_melt_want: a caller about to build a large flat structure (one whose
 * intermediates cannot be traced mid-build, e.g. the fs_list result) may
 * request this many FREE pairs after compaction; growth doubles until the
 * request is met. Zero keeps the original policy (double iff live*2 > cap).
 * Always reset to 0 after the call. */
static long long fk_melt_want = 0;
/* ── the string melt: the pool's top rolls back over the dead ───────────────
 * Every str_concat, int_to_str and byte_to_str result is interned for the life
 * of the process, and a loop that renders a screen mints hundreds of new
 * strings a tick (the glass self-molted every ~100 s on that count alone).
 * After the pair melt has copied the live heap, this pass marks every LOCAL
 * string reachable from the same roots (the value stack, the memory cells,
 * record values and blueprints, value nodes) plus the holders that keep a raw
 * index rather than a word (record keys, the AST's string-literal nodes), then
 * pops entries off the TOP of the table while they are unmarked. Live strings
 * never move -- their indices, offsets and bytes stay where another process
 * may be reading them through the shared store -- so this reclaims the tick's
 * temporaries (born last, dead first) and leaves anything older untouched.
 * Field strings (indices >= FK_STR_BASE) are shared by content and are never
 * touched here. */
static unsigned char *fk_smk;
static unsigned char *fk_smv;
static void fk_smark(long long v) {
    if (fk_is_str(v)) {
        long long si = fk_stri(v);
        if (si >= 0 && si < FK_STR_BASE && si < fk_sp) { fk_smk[si] = 1; }
        return;
    }
    while ((v & 1) != 0 && v > 1) {
        long long p = v >> 1;
        if (p >= FK_PAIR_BASE || p < 1 || !FK_POK(p)) { return; }
        if (fk_smv[p]) { return; }
        fk_smv[p] = 1;
        fk_smark(FK_HH(p));
        v = FK_HT(p);
    }
}
static long long fk_smelt_reclaimed;
static void fk_smelt(void) {
    if (fk_sp <= 0 || fk_sb == 0) { return; }
    fk_smk = (unsigned char *)calloc((unsigned long)fk_sp, 1);
    fk_smv = (unsigned char *)calloc((unsigned long)(fk_hp + 1), 1);
    if (fk_smk == 0 || fk_smv == 0) { free(fk_smk); free(fk_smv); fk_smk = 0; fk_smv = 0; return; }
    long long k = 0;
    while (k < fk_vsp) { fk_smark(fk_vs[k]); k = k + 1; }
    k = 0;
    while (k < fk_mem_cap) { fk_smark(fk_mem[k]); k = k + 1; }
    k = 1;
    while (k <= fk_rp) {
        fk_smark(fk_rbp[k]);
        long long rj = 0;
        while (rj < fk_rcnt[k]) {
            long long key = fk_rkey[k][rj];
            if (key >= 0 && key < FK_STR_BASE && key < fk_sp) { fk_smk[key] = 1; }
            fk_smark(fk_rval[k][rj]);
            rj = rj + 1;
        }
        k = k + 1;
    }
    k = 1;
    while (!fk_field_on && k <= fk_np) { fk_smark(fk_ncat[k]); fk_smark(fk_nkids[k]); fk_smark(fk_nval[k]); k = k + 1; }
    k = 0;
    while (k < fk_node_count) {
        if (fk_node[k][0] == 24) { long long si = fk_node[k][1]; if (si >= 0 && si < fk_sp) { fk_smk[si] = 1; } }
        k = k + 1;
    }
    long long freed = 0;
    long long i = 0;
    while (i < fk_sp) {
        if (!fk_smk[i] && !fk_sdead[i]) {
            long long b = fk_str_hash(fk_so[i], fk_sl[i]);
            if (fk_shash[b] == i) { fk_shash[b] = fk_snext[i]; }
            else {
                long long c = fk_shash[b];
                while (c >= 0 && fk_snext[c] != i) { c = fk_snext[c]; }
                if (c >= 0) { fk_snext[c] = fk_snext[i]; }
            }
            fk_sdead[i] = 1;
            fk_sfree[fk_sfree_n] = i;
            fk_sfree_n = fk_sfree_n + 1;
            freed = freed + 1;
        }
        i = i + 1;
    }
    fk_smelt_reclaimed = fk_smelt_reclaimed + freed;
    free(fk_smk); free(fk_smv); fk_smk = 0; fk_smv = 0;
}
static void fk_melt(void) {
    fk_melt_gen = fk_melt_gen + 1;
    long long hp0 = fk_hp;
    fk_fw = calloc(fk_hp + 1, 8);
    if (fk_fw == 0) {
        fk_die("fk_melt: fw calloc failed -- heap cannot be compacted, and returning here would let the program continue on a full heap as if space were reclaimed. Out of memory is out of memory (same as fk_fbox/fk_sintern).");
    }
    long long nlive = 0;
    long long k = 0;
    while (k < fk_vsp) {
        nlive = nlive + fk_mlive(fk_vs[k]);
        k = k + 1;
    }
    k = 0;
    while (k < fk_mem_cap) {
        nlive = nlive + fk_mlive(fk_mem[k]);
        k = k + 1;
    }
    /* record VALUES and BLUEPRINTS are ROOTS: a field holding a cons value
     * must survive compaction. Record KEYS are NOT values — fk_rkey holds raw
     * fk_stri string-pool INDEXES, and an odd index read as a value decodes as
     * a cons cell, so mcopy "relocates" it into an unrelated pool entry
     * (witnessed 2026-09-02: a keydir's "graph/count/total" row re-reading as
     * "E" after file-lane churn melts — the graph-node aggregate wound). One
     * walk, values+bp only; and only one — fk_mcopy is not idempotent
     * (forwarding is indexed by OLD arena positions, so a second copy of an
     * already-copied value can alias). */
    k = 1;
    while (k <= fk_rp) {
        nlive = nlive + fk_mlive(fk_rbp[k]);
        long long rj = 0;
        while (rj < fk_rcnt[k]) {
            nlive = nlive + fk_mlive(fk_rval[k][rj]);
            rj = rj + 1;
        }
        k = k + 1;
    }
    k = 1;
    while (!fk_field_on && k <= fk_np) {
        nlive = nlive + fk_mlive(fk_ncat[k]);
        nlive = nlive + fk_mlive(fk_nkids[k]);
        nlive = nlive + fk_mlive(fk_nval[k]);
        k = k + 1;
    }
    long long ncap = fk_cap;
    if (nlive * 2 > fk_cap) {
        ncap = fk_cap * 2;
    }
    while (ncap - nlive < fk_melt_want) {
        ncap = ncap * 2;
    }
    /* churn amortization (2026-08-30): a melt walks the whole value-node
     * pool, so its cost is O(fk_np) however little the heap holds. A
     * tiny-live, high-churn program (witnessed: the BML emitter at cap
     * 8192 / live 3.3k triggered 122,158 melts in one triple run, +9s per
     * identical repetition as the pool grew) melts every few thousand
     * conses and pays that walk each time. Guarantee headroom that scales
     * with the walk: at least np/4 free pairs after compaction, so each
     * O(np) melt is amortized over >= np/4 allocations. Worst-case extra
     * memory is fk_np/4 pairs. */
    while (ncap - nlive < fk_np / 4 + FK_HASHCONS_INIT_CAP) {
        ncap = ncap * 2;
    }
    if (fk_store_shared && ncap > FK_STORE_HEAP_PAIRS) { fk_store_go_private(); }
    fk_nh = fk_store_shared ? (long long *)fk_heap_alt_h : malloc(ncap * 8);
    fk_nt = fk_store_shared ? (long long *)fk_heap_alt_t : malloc(ncap * 8);
    if (fk_nh == 0 || fk_nt == 0) {
        free(fk_nh);
        free(fk_nt);
        free(fk_fw);
        fk_die("fk_melt: arena malloc failed -- heap cannot be compacted, and returning here would let the program continue on a full heap as if space were reclaimed. Out of memory is out of memory (same as fk_fbox/fk_sintern).");
    }
    fk_nhp = 0;
    fk_nh[0] = 1;
    fk_nt[0] = 1;
    k = 0;
    while (k < fk_vsp) {
        fk_vs[k] = fk_mcopy(fk_vs[k]);
        k = k + 1;
    }
    k = 0;
    while (k < fk_mem_cap) {
        fk_mem[k] = fk_mcopy(fk_mem[k]);
        k = k + 1;
    }
    k = 1;
    while (k <= fk_rp) {
        fk_rbp[k] = fk_mcopy(fk_rbp[k]);
        long long rj = 0;
        while (rj < fk_rcnt[k]) {
            fk_rval[k][rj] = fk_mcopy(fk_rval[k][rj]);
            rj = rj + 1;
        }
        k = k + 1;
    }
    k = 1;
    while (!fk_field_on && k <= fk_np) {
        fk_ncat[k] = fk_mcopy(fk_ncat[k]);
        fk_nkids[k] = fk_mcopy(fk_nkids[k]);
        fk_nval[k] = fk_mcopy(fk_nval[k]);
        k = k + 1;
    }
    if (fk_store_shared) { fk_heap_alt_h = fk_hh; fk_heap_alt_t = fk_ht; fk_heap_gen = 1 - fk_heap_gen; } else { free(fk_hh); free(fk_ht); }
    free(fk_fw);
    fk_hh = fk_nh;
    fk_ht = fk_nt;
    fk_hp = fk_nhp;
    fk_cap = ncap;
    fk_nmelt = fk_nmelt + 1;
    fk_smelt();
    fk_live_publish(0);
    if (fk_conf("FK_MELT_WITNESS")) {
        dprintf(2, "[melt %lld] hp %lld -> %lld, nlive=%lld, cap=%lld, vsp=%lld, np=%lld, fp=%lld, sp=%lld\n",
                fk_nmelt, hp0, fk_hp, nlive, fk_cap, fk_vsp, fk_np, fk_fp, fk_sp);
    }
}
/* THE WALL THAT COULD NOT SPEAK. Thirty lines below, fk_walk's host-stack wall
 * says what it measured, what the limit was, and what the recipe should do
 * instead — "the wall is honest, the silent crash was not". This one said
 * `fk_vp: value stack overflow` and exited: no depth, no cap, no remedy, and
 * nothing to distinguish a runaway recursion from a legitimately deep one. It
 * was also the wall reached FIRST, so the body's honest diagnostic never got
 * the chance to speak. Two walls, one voice between them, and the mute one in
 * front. */
static void fk_vp(long long v) {
    if (fk_vsp >= fk_vs_cap) {
        fk_vs_grow(fk_vsp + 1);
    }
    fk_vs[fk_vsp] = v;
    fk_vsp = fk_vsp + 1;
}
/* Function roots, arities, source symbols, and transient heat grow as one
 * organ. The old five
 * parallel 4096-seat arrays made program composition depend on a historical
 * bootstrap number. A geometric reserve keeps append amortized O(1), retains
 * stable numeric function indices, and fails only when allocation itself cannot
 * continue. This remains temporary seed mechanics; function identity belongs in
 * the Form/native-walker body. */
static long long *fk_fn, *fk_fnar, *fk_fnsym_s, *fk_fnsym_n, *fk_fnidx;
/* HEAT: every fn dispatch feeds a counter; at exit the hottest recipes
 * are written to .fkwu-heat so the body names its own JIT worklist.
 * Anything taking longer than 100ms deserves JIT (Urs, 2026-09-01) —
 * a six-hour interpreted walker (the Q4 crystal freeze, 348 CPU-min in
 * fk_walk) would top this list in its first minute. Witness first; the
 * transparent crystallize-at-threshold dispatch is the next course. */
static long long *fk_fn_heat;
static long long fk_fn_capacity;
static long long fk_fntop, fk_defn_next, fk_root;
static int fk_live_ledgers_paged;   /* 1 once fk_fn_heat/fbox/unbox point into the live page (fixed 2^20 fns) */
static void fk_fn_reserve(long long needed) {
    if (needed <= fk_fn_capacity) {
        return;
    }
    long long next = fk_fn_capacity > 0 ? fk_fn_capacity : 256;
    while (next < needed) {
        long long grown = next << 1;
        if (grown <= next) {
            fk_die("fk_fn_reserve: function capacity overflow");
        }
        next = grown;
    }
    if ((unsigned long long)next >
        (unsigned long long)((unsigned long)-1) / sizeof(long long)) {
        fk_die("fk_fn_reserve: function image byte size overflow");
    }
    unsigned long bytes = (unsigned long)next * sizeof(long long);
    long long *next_fn = malloc(bytes);
    long long *next_fnar = malloc(bytes);
    long long *next_fnsym_s = malloc(bytes);
    long long *next_fnsym_n = malloc(bytes);
    long long *next_fnidx = malloc(bytes);
    long long *next_fn_heat = fk_live_ledgers_paged ? fk_fn_heat : malloc(bytes);
    long long *next_fn_fbox = fk_live_ledgers_paged ? fk_fn_fbox : malloc(bytes);
    long long *next_fn_unbox = fk_live_ledgers_paged ? fk_fn_unbox : malloc(bytes);
    long long *next_fn_native = fk_live_ledgers_paged ? fk_fn_native : malloc(bytes);
    long long *next_fn_mint = fk_live_ledgers_paged ? fk_fn_mint : malloc(bytes);
    long long *next_fn_inram = fk_live_ledgers_paged ? fk_fn_inram : malloc(bytes);
    if (next_fn == 0 || next_fnar == 0 || next_fnsym_s == 0 ||
        next_fnsym_n == 0 || next_fnidx == 0 || next_fn_heat == 0 ||
        next_fn_fbox == 0 || next_fn_unbox == 0 || next_fn_native == 0 || next_fn_mint == 0 || next_fn_inram == 0) {
        free(next_fn);
        free(next_fnar);
        free(next_fnsym_s);
        free(next_fnsym_n);
        free(next_fnidx);
        free(next_fn_heat);
        free(next_fn_fbox);
        free(next_fn_unbox);
        free(next_fn_native);
        free(next_fn_mint);
        fk_die("fk_fn_reserve: out of memory growing function image");
    }
    long long i = 0;
    while (i < fk_fn_capacity) {
        next_fn[i] = fk_fn[i];
        next_fnar[i] = fk_fnar[i];
        next_fnsym_s[i] = fk_fnsym_s[i];
        next_fnsym_n[i] = fk_fnsym_n[i];
        next_fnidx[i] = fk_fnidx[i];
        if (!fk_live_ledgers_paged) { next_fn_heat[i] = fk_fn_heat[i]; }
        if (!fk_live_ledgers_paged) { next_fn_fbox[i] = fk_fn_fbox[i]; }
        if (!fk_live_ledgers_paged) { next_fn_unbox[i] = fk_fn_unbox[i]; }
        if (!fk_live_ledgers_paged) { next_fn_native[i] = fk_fn_native[i]; }
        if (!fk_live_ledgers_paged) { next_fn_mint[i] = fk_fn_mint[i]; }
        if (!fk_live_ledgers_paged) { next_fn_inram[i] = fk_fn_inram[i]; }
        i = i + 1;
    }
    while (i < next) {
        next_fn[i] = 0;
        next_fnar[i] = 0;
        next_fnsym_s[i] = 0;
        next_fnsym_n[i] = 0;
        next_fnidx[i] = 0;
        if (!fk_live_ledgers_paged) { next_fn_heat[i] = 0; }
        if (!fk_live_ledgers_paged) { next_fn_fbox[i] = 0; }
        if (!fk_live_ledgers_paged) { next_fn_unbox[i] = 0; }
        if (!fk_live_ledgers_paged) { next_fn_native[i] = 0; }
        if (!fk_live_ledgers_paged) { next_fn_mint[i] = 0; }
        i = i + 1;
    }
    free(fk_fn);
    free(fk_fnar);
    free(fk_fnsym_s);
    free(fk_fnsym_n);
    free(fk_fnidx);
    if (!fk_live_ledgers_paged) { free(fk_fn_heat); }
    if (!fk_live_ledgers_paged) { free(fk_fn_fbox); }
    if (!fk_live_ledgers_paged) { free(fk_fn_unbox); }
    if (!fk_live_ledgers_paged) { free(fk_fn_native); }
    if (!fk_live_ledgers_paged) { free(fk_fn_mint); }
    if (!fk_live_ledgers_paged) { free(fk_fn_inram); }
    fk_fn = next_fn;
    fk_fnar = next_fnar;
    fk_fnsym_s = next_fnsym_s;
    fk_fnsym_n = next_fnsym_n;
    fk_fnidx = next_fnidx;
    fk_fn_heat = next_fn_heat;
    fk_fn_fbox = next_fn_fbox;
    fk_fn_unbox = next_fn_unbox;
    fk_fn_native = next_fn_native;
    fk_fn_mint = next_fn_mint;
    fk_fn_inram = next_fn_inram;
    fk_fn_capacity = next;
}
/* Closure bookkeeping, PER FUNCTION (not per call, not per instance -- see fk_clo_make for that).
 * Kept as its own growable table, separate from fk_fn_reserve's own seven parallel arrays, so this
 * addition can never disturb that function's existing malloc/copy/free sequence: 0 for every plain
 * function (top-level or a nested defn that never reads an enclosing name), so the ordinary case
 * pays nothing beyond the growth itself.
 *   fk_fn_parent_idx[idx]   -- the fn-idx whose body directly contains idx's own (defn ...)
 *                              statement (-1 for a top-level defn, which has no enclosing frame to
 *                              capture from at all).
 *   fk_fn_cap_count[idx]    -- how many free variables idx's body actually captured.
 *   fk_fn_cap_encoff[idx*8+j] -- for captured var j, the slot offset in the PARENT's own frame its
 *                              live value reads from (valid only when the parent's own frame is the
 *                              current one -- a same-scope call).
 *   fk_fn_cap_slot[idx*8+j] -- for captured var j, the slot in idx's OWN frame its value is
 *                              delivered to (both by idx's own compiled body's ordinary reads, and
 *                              by the prologue that populates it from fk_call_cap_vals). */
static long long *fk_fn_parent_idx, *fk_fn_cap_count, *fk_fn_cap_encoff, *fk_fn_cap_slot;
static long long fk_fn_cap_capacity;
static void fk_fn_cap_reserve(long long needed) {
    if (needed <= fk_fn_cap_capacity) {
        return;
    }
    long long next = fk_fn_cap_capacity > 0 ? fk_fn_cap_capacity : 256;
    while (next < needed) {
        next = next << 1;
    }
    unsigned long bytes1 = (unsigned long)next * sizeof(long long);
    unsigned long bytes8 = (unsigned long)next * FK_CLOSURE_CAP_MAX * sizeof(long long);
    long long *np = realloc(fk_fn_parent_idx, bytes1);
    long long *nc = realloc(fk_fn_cap_count, bytes1);
    long long *ne = realloc(fk_fn_cap_encoff, bytes8);
    long long *ns = realloc(fk_fn_cap_slot, bytes8);
    if (np == 0 || nc == 0 || ne == 0 || ns == 0) {
        fk_die("fk_fn_cap_reserve: out of memory growing closure-capture tables");
    }
    fk_fn_parent_idx = np;
    fk_fn_cap_count = nc;
    fk_fn_cap_encoff = ne;
    fk_fn_cap_slot = ns;
    long long i = fk_fn_cap_capacity;
    while (i < next) {
        fk_fn_parent_idx[i] = -1;
        fk_fn_cap_count[i] = 0;
        long long j = 0;
        while (j < FK_CLOSURE_CAP_MAX) {
            fk_fn_cap_encoff[i * FK_CLOSURE_CAP_MAX + j] = 0;
            fk_fn_cap_slot[i * FK_CLOSURE_CAP_MAX + j] = 0;
            j = j + 1;
        }
        i = i + 1;
    }
    fk_fn_cap_capacity = next;
}
#define FK_AST_NODE_CAP_INIT 262144 /* fk_node[][4]: the parsed program's own syntax tree (see NOTE above FK_NODE_CAP_INIT). Birth size only -- the table DOUBLES on demand (fk_ast_reserve), so program size is not a wall; .fkb images reserve to fit before bulk-loading. History: 65536->262144 (2026-07-02, a full mel-spectrogram program); a clamp-and-halt wall stood 2026-07-18..09-02 after a doubling probe caught fk_sparse's stray-rparen zero-advance spin re-minting sentinels to the brim (677,766 diagnostics in 6s -- a treadmill, not capacity; fixed at root in the bare-symbol path). That teaching survives the wall's removal: a parse that grows without advancing fk_spos is a parser wound -- kernel_stat 23/24 (live cap / doublings) make the growth observable, and the fill-position question stays the probe. */
static long long fk_node_count;
static long long (*fk_node)[4];
static long long fk_ast_cap;    /* live capacity; fk_ast_reserve owns it */
static long long fk_ast_grows;  /* doublings this run -- kernel_stat 24 */
/* Per-AST-node memo of a source float LITERAL's boxed value (tag 53 over a
 * tag-24 constant child), so the constant mints ONE float-pool slot per
 * process instead of one per evaluation (boxvoice witnessed 1.5 re-boxing
 * 300,000 times in one loop). A SIDE table, deliberately not a fifth fk_node
 * column: the .fkb writer serializes fk_node rows verbatim, and a pool
 * handle riding into an image would dangle in the loading process. This
 * table is process-local, zeroed at birth and growth, so a warm .fkb replay
 * re-mints each literal exactly once in its own pool. 0 = no memo (a real
 * box is always at/below fk_fbase-3, never 0). Safe to cache because
 * fk_node rows are never rewritten after parse/load. */
static long long *fk_flit_memo;
static void fk_ast_reserve(long long need) {
    long long nc;
    long long i;
    if (fk_ast_cap == 0) {
        /* the rows live in this kernel's program surface (/fg-c<pid>-A, a sparse reservation another process maps and
         * reads by row); when the host offers no shared memory they live in private memory as before */
        fk_node = (long long (*)[4])fk_prog_take('A', FK_PROG_AST_ROWS * 32);
        fk_prog_ast_shared = fk_node != 0;
        if (fk_node == 0) { fk_node = (long long (*)[4])calloc(FK_AST_NODE_CAP_INIT, 32); }
        fk_flit_memo = (long long *)calloc(FK_AST_NODE_CAP_INIT, 8);
        if (fk_node == 0 || fk_flit_memo == 0) {
            fk_die("fk_ast_reserve: out of memory for the AST node table");
        }
        fk_ast_cap = FK_AST_NODE_CAP_INIT;
    }
    if (need <= fk_ast_cap) {
        return;
    }
    nc = fk_ast_cap;
    while (nc < need) {
        nc = nc * 2;
    }
    if (fk_prog_ast_shared && nc > FK_PROG_AST_ROWS) {
        /* past the reservation: the rows go private once (copied), the shared object is unlinked, the header says so */
        fk_node = (long long (*)[4])fk_store_copy_out(fk_node, fk_ast_cap * 32);
        { char nm[32]; fk_store_name('A', (long long)getpid(), nm); shm_unlink(nm); }
        fk_prog_ast_shared = 0;
    }
    if (!fk_prog_ast_shared) {
        fk_node = (long long (*)[4])realloc(fk_node, (unsigned long)(nc * 32));
    }
    fk_flit_memo = (long long *)realloc(fk_flit_memo, (unsigned long)(nc * 8));
    if (fk_node == 0 || fk_flit_memo == 0) {
        fk_die("fk_ast_reserve: out of memory growing the AST node table");
    }
    i = fk_ast_cap * 4;
    while (!fk_prog_ast_shared && i < nc * 4) {   /* a shared reservation is zero-filled by the host; touching it would commit it */
        fk_node[i >> 2][i & 3] = 0;
        i = i + 1;
    }
    i = fk_ast_cap;
    while (i < nc) {
        fk_flit_memo[i] = 0;
        i = i + 1;
    }
    fk_ast_cap = nc;
    fk_ast_grows = fk_ast_grows + 1;
}
/* Artifact/source bytes are scratch, not a language limit. The old fixed 16MiB
 * array made a valid `.fkb` turn cold again merely by crossing a historical
 * process-size number. Keep one reusable high-water buffer, grown geometrically
 * only when a real file needs it; allocation failure remains the honest limit. */
static char *fk_buf;
static long long fk_buf_capacity;
static int fk_buf_reserve(long long needed) {
    if (needed <= fk_buf_capacity) {
        return 1;
    }
    if (needed < 0) {
        return 0;
    }
    long long next = fk_buf_capacity > 0 ? fk_buf_capacity : 65536;
    while (next < needed) {
        if (next > 4611686018427387903LL) {
            next = needed;
            break;
        }
        next = next * 2;
    }
    char *grown = realloc(fk_buf, (unsigned long)next);
    if (grown == 0) {
        return 0;
    }
    fk_buf = grown;
    fk_buf_capacity = next;
    return 1;
}
static long long fk_read_all_dynamic(int fd, long long expected) {
    if (expected < 65536) {
        expected = 65536;
    }
    if (!fk_buf_reserve(expected)) {
        return -3;
    }
    long long total = 0;
    while (1) {
        if (total == fk_buf_capacity && !fk_buf_reserve(fk_buf_capacity + 1)) {
            return -3;
        }
        long long got = read(fd, fk_buf + total,
                             (unsigned long)(fk_buf_capacity - total));
        if (got > 0) {
            total = total + got;
        } else if (got == 0) {
            return total;
        } else if (errno != EINTR) {
            return -1;
        }
    }
}
static long long fk_pos;
extern int open(const char *, int, ...);
extern long long read(int, void *, unsigned long);
static long long fk_next() {
    long long sg = 1;
    while (fk_buf[fk_pos] != 0) {
        if (fk_buf[fk_pos] == FK_CH_DASH && fk_buf[fk_pos + 1] >= FK_CH_DIGIT0 &&
            fk_buf[fk_pos + 1] <= FK_CH_DIGIT9) {
            sg = 0 - 1;
            fk_pos = fk_pos + 1;
            break;
        }
        if (fk_buf[fk_pos] >= FK_CH_DIGIT0) {
            if (fk_buf[fk_pos] <= FK_CH_DIGIT9) {
                break;
            }
        }
        fk_pos = fk_pos + 1;
    }
    long long v = 0;
    while (fk_buf[fk_pos] >= FK_CH_DIGIT0 && fk_buf[fk_pos] <= FK_CH_DIGIT9) {
        v = v * 10 + (fk_buf[fk_pos] - FK_CH_DIGIT0);
        fk_pos = fk_pos + 1;
    }
    return sg * v;
}
static void fk_psv(long long v) {
    long long sa = fk_stri(v);
    if (sa >= 0 && FK_SOK(sa)) {
        long long j = 0;
        while (j < FK_SLEN(sa)) {
            putchar((int)(unsigned char)FK_SBYTES(sa)[j]);
            j = j + 1;
        }
        putchar(10);
    } else {
        fk_pv(v);
    }
}
/* The source runner is the production carrier, so its stdout boundary must
 * preserve a numeric list as data instead of leaking the cons-heap handle.
 * Lists are positive odd values (nil is 1); nodes/function values are negative,
 * records are negative even, and scalar numbers retain their existing encoding.
 * This is deliberately a transport printer, not new evaluator meaning: list
 * construction and every numeric value were already produced by the Form body. */
static int fk_is_output_list(long long v) {
    if (v == 1) {
        return 1;
    }
    if (v <= 1 || (v & 1) == 0) {
        return 0;
    }
    long long p = v >> 1;
    return p >= 1 && FK_POK(p);
}
static void fk_pv_inline_number(long long v) {
    if (fk_is_str(v)) {
        fk_put_str(v);
    } else if (fk_is_nothing(v)) { printf("nothing"); } else if (fk_isf(v)) {
        char fb[64];
        fk_fmt_float_js(fk_num(v), fb);
        printf("%s", fb);
    } else if ((v & 1) == 0) {
        printf("%lld", v >> 1);
    } else {
        printf("%lld", v);
    }
}
/* The list printer walks nesting on its own stack of open brackets (the cons
 * position to resume after a nested list closes), so output depth is bounded
 * by memory alone. FK_PV_NEST_CAP_INIT is a birth size only; the stack doubles
 * on demand (kernel_stat 35/36). The old recursion died at a chosen 1024
 * levels; cons cells never mutate, so nesting is finite by construction and
 * needs no depth wall. */
#define FK_PV_NEST_CAP_INIT 1024
static long long *fk_pv_nest, fk_pv_nest_cap, fk_pv_nest_grows;
static void fk_pv_list(long long v) {
    long long sp = 0;
    long long p = v >> 1;
    int first = 1;
    putchar(FK_CH_LBRACKET);
    for (;;) {
        if (p >= 1 && FK_POK(p)) {
            long long item = FK_HH(p);
            long long next = FK_HT(p) >> 1;
            if (!first) {
                putchar(FK_CH_COMMA);
                putchar(FK_CH_SPACE);
            }
            if (fk_is_output_list(item)) {
                if (sp >= fk_pv_nest_cap) {
                    long long nc = fk_pv_nest_cap == 0 ? FK_PV_NEST_CAP_INIT : fk_pv_nest_cap * 2;
                    fk_pv_nest = (long long *)realloc(fk_pv_nest, (unsigned long)(nc * 8));
                    if (fk_pv_nest == 0) {
                        fk_die("fk_pv_list: out of memory growing the nesting stack");
                    }
                    if (fk_pv_nest_cap != 0) {
                        fk_pv_nest_grows = fk_pv_nest_grows + 1;
                    }
                    fk_pv_nest_cap = nc;
                }
                fk_pv_nest[sp] = next;
                sp = sp + 1;
                putchar(FK_CH_LBRACKET);
                p = item >> 1;
                first = 1;
                continue;
            }
            fk_pv_inline_number(item);
            first = 0;
            p = next;
            continue;
        }
        putchar(FK_CH_RBRACKET);
        if (sp == 0) {
            return;
        }
        sp = sp - 1;
        p = fk_pv_nest[sp];
        first = 0;
    }
}
/* The result boundary. Until the string band existed (2026-07-31) this asked the
 * NODE whether the result was a string (fk_str_root_depth), because the WORD could
 * not say -- and the node only knows for literals, str_concat, read_file and the
 * like, never for a string that arrives through a parameter. MEASURED on the 400-band
 * sweep: concept-corpus-band and fnri-receipt-band each return a string, the node
 * analysis said "not a string", and fkwu printed the interned POOL INDEX -- 143 and
 * 1299 -- numbers indistinguishable from an honest verdict. The value now carries its
 * own kind, so ask the value. */
static void fk_pv_root(long long v) {
    if (fk_is_str(v)) {
        fk_psv(v);
    } else if (fk_is_output_list(v)) {
        fk_pv_list(v);
        putchar(FK_CH_LF);
    } else {
        fk_pv(v);
    }
}
static long long fk_walk(long long i, long long fp);
static long long fk_walk_body(long long i, long long fp) {
    for (;;) {
        long long t = fk_node[i][0];
        if (t < 0 || t >= FK_OPCODE_ARM_CAP) {
            fk_die("fk_walk_body: node tag outside FK_OPCODE_ARM_CAP (0..255) -- the walker's tag space is the contract the op table is generated against; this is a corrupt node or a tag minted past the last arm");
        }
        fk_arms[t] = fk_arms[t] + 1;
        if (t == 6) {
            if (fk_walk(fk_node[i][1], fp) == 0) {
                i = fk_node[i][3];
            } else {
                i = fk_node[i][2];
            }
            continue;
        }
        if (t == 69) {
            fk_walk(fk_node[i][1], fp);
            i = fk_node[i][2];
            continue;
        }
        if (t == 109) {
            long long slot109 = fk_walk(fk_node[i][1], fp) >> 1;
            /* the RHS walk can GROW fk_vs (realloc moves it), so the walked
             * value lands in a local first -- never an unsequenced write. */
            long long v109 = fk_walk(fk_node[i][2], fp);
            if (fp + slot109 >= fk_vs_cap) {
                fk_vs_grow(fp + slot109 + 1);
            }
            fk_vs[fp + slot109] = v109;
            /* ROOT the let-local (see the fk_walk tag-109 note): raise fk_vsp over the
             * slot so the next form's temporaries cannot clobber it and a melt relocates it. */
            if (fp + slot109 + 1 > fk_vsp) {
                fk_vsp = fp + slot109 + 1;
            }
            i = fk_node[i][3];
            continue;
        }
        if (t == 111) {
            long long k111 = fk_walk(fk_node[i][1], fp) >> 1;
            long long need111 = fp + 1 + k111;
            while (fk_vsp < need111) {
                fk_vs[fk_vsp] = 0;
                fk_vsp = fk_vsp + 1;
            }
            i = fk_node[i][2];
            continue;
        }
        if (t == 7) {
            long long v7 = fk_walk(fk_node[i][1], fp);
            fk_vs[fp] = v7;
            fk_vsp = fp + 1;
            i = fk_fn[0];
            continue;
        }
        if (t == 12) {
            long long v12 = fk_walk(fk_node[i][2], fp);
            long long c12 = fk_node[i][1];
            if (c12 < 0 || c12 >= fk_fn_count) {
                fk_vsp = fp;
                return fk_nothing;
            }
            fk_vs[fp] = v12;
            fk_vsp = fp + 1;
            long long h12 = fk_fn_heat[c12] + 1;
            fk_fn_heat[c12] = h12;
            fk_cur_fn = c12;
            fk_heat_pulse();
            if ((h12 & (FK_F64_HEAT - 1)) == 0) { fk_f64_loop_pulse(c12, fp, 1); } /* the heat ledger is the loop lane's trigger: the frame is whole here */
            i = fk_fn[c12];
            continue;
        }
        if (t == 240) {
            long long a0 = fk_walk(fk_node[i][2], fp);
            long long a1 = fk_walk(fk_node[i][3], fp);
            long long c240 = fk_node[i][1];
            if (c240 < 0 || c240 >= fk_fn_count) {
                fk_vsp = fp;
                return fk_nothing;
            }
            fk_vs[fp] = a0;
            fk_vs[fp + 1] = a1;
            fk_vsp = fp + 2;
            long long h240 = fk_fn_heat[c240] + 1;
            fk_fn_heat[c240] = h240;
            fk_cur_fn = c240;
            fk_heat_pulse();
            if ((h240 & (FK_F64_HEAT - 1)) == 0) { fk_f64_loop_pulse(c240, fp, 2); }
            i = fk_fn[c240];
            continue;
        }
        if (t == 194) {
            /* a crystallized defn: the frame's floats go to d0..d7, the f64 leaf runs, one box comes back; any other shape walks the original body */
            long long c194 = fk_node[i][1];
            void *m194 = (c194 >= 0 && c194 < fk_f64_cap) ? fk_f64_mem[c194] : 0;
            long long n194 = fk_fnar[c194];
            long long sig194 = m194 != 0 ? fk_f64_sig[c194] : -1;
            if (fk_fn_native != 0 && c194 < fk_fn_capacity && fk_fn_native[c194] == 3) {
                long long r194;
                if (fk_twin_call(c194, fp, &r194)) { fk_twin_calls = fk_twin_calls + 1; return r194; }
                i = fk_node[i][2];
                continue;
            }
            if (m194 != 0 && sig194 >= 0 && n194 >= 1 && n194 <= 8) {
                /* the loop leaf: each frame arg must wear the type the loop was emitted for (int = even word, float = pool box);
                 * ints untag once at the door (v >> 1), floats unbox once, the result tags or boxes once on the way out */
                long long f194[9];
                long long k194 = 0, nf194 = 0;
                int ok194 = 1;
                while (k194 < n194) {
                    long long v194 = fk_vs[fp + k194];
                    if ((sig194 >> k194) & 1) {
                        if (!fk_isf(v194)) { ok194 = 0; break; }
                        double d194 = FK_FV(fk_fidx(v194));
                        memcpy(&f194[k194], &d194, 8);
                        nf194 = nf194 + 1;
                    } else {
                        if ((v194 & 1) != 0) { ok194 = 0; break; }
                        f194[k194] = v194 >> 1;
                    }
                    k194 = k194 + 1;
                }
                if (ok194) {
                    f194[8] = 0;
                    long long out194;
                    if ((sig194 >> 8) & 1) {
                        double (*loopf194)(long long *) = (double (*)(long long *))m194;
                        double r194 = loopf194(f194);
                        out194 = fk_fbox(r194);
                    } else {
                        long long (*loopi194)(long long *) = (long long (*)(long long *))m194;
                        out194 = loopi194(f194) << 1;
                    }
                    fk_inram_call_total = fk_inram_call_total + 1;
                    if (fk_fn_inram != 0 && c194 >= 0 && c194 < fk_fn_capacity) { fk_fn_inram[c194] = fk_fn_inram[c194] + 1; } /* this body FOLDED: its intermediates lived in registers, not one pool slot each */
                    fk_f64_loop_iters = fk_f64_loop_iters + f194[8];
                    fk_unbox_total = fk_unbox_total + nf194;
                    if (fk_fn_unbox != 0 && c194 < fk_fn_capacity) { fk_fn_unbox[c194] = fk_fn_unbox[c194] + nf194; }
                    return out194;
                }
                i = fk_node[i][2];
                continue;
            }
            if (m194 != 0 && n194 >= 1 && n194 <= 8) {
                double a194[8];
                long long k194 = 0;
                int ok194 = 1;
                while (k194 < 8) { a194[k194] = 0.0; k194 = k194 + 1; }
                k194 = 0;
                while (k194 < n194) {
                    long long v194 = fk_vs[fp + k194];
                    if (!fk_isf(v194)) { ok194 = 0; break; }
                    a194[k194] = FK_FV(fk_fidx(v194));
                    k194 = k194 + 1;
                }
                if (ok194) {
                    double (*leaf194)(double, double, double, double, double, double, double, double) = (double (*)(double, double, double, double, double, double, double, double))m194;
                    double r194 = leaf194(a194[0], a194[1], a194[2], a194[3], a194[4], a194[5], a194[6], a194[7]);
                    fk_inram_call_total = fk_inram_call_total + 1;
                    if (fk_fn_inram != 0 && c194 >= 0 && c194 < fk_fn_capacity) { fk_fn_inram[c194] = fk_fn_inram[c194] + 1; } /* the f64 leaf FOLDED this body */
                    fk_unbox_total = fk_unbox_total + n194;
                    if (fk_fn_unbox != 0 && c194 < fk_fn_capacity) { fk_fn_unbox[c194] = fk_fn_unbox[c194] + n194; }
                    return fk_fbox(r194);
                }
            }
            i = fk_node[i][2];
            continue;
        }
        if (t == 241) {
            long long base241 = fk_vsp;
            long long cell241 = fk_node[i][2];
            while (cell241 >= 0 && fk_node[cell241][0] == 242) {
                fk_vp(fk_walk(fk_node[cell241][1], fp));
                cell241 = fk_node[cell241][2];
            }
            long long n241 = fk_vsp - base241;
            long long c241 = fk_node[i][1];
            if (c241 < 0 || c241 >= fk_fn_count) {
                fk_vsp = fp;
                return fk_nothing;
            }
            long long m241 = 0;
            while (m241 < n241) {
                fk_vs[fp + m241] = fk_vs[base241 + m241];
                m241 = m241 + 1;
            }
            fk_vsp = fp + n241;
            long long h241 = fk_fn_heat[c241] + 1;
            fk_fn_heat[c241] = h241;
            fk_cur_fn = c241;
            fk_heat_pulse();
            if ((h241 & (FK_F64_HEAT - 1)) == 0) { fk_f64_loop_pulse(c241, fp, n241); }
            i = fk_fn[c241];
            continue;
        }
        if (t == 244) {
            long long hv244 = fk_walk(fk_node[i][1], fp);
            if (fk_is_fnval(hv244) == 0) {
                fk_vsp = fp;
                return fk_nothing;
            }
            long long fi244 = fk_fnval_target(hv244);
            long long base244 = fk_vsp;
            long long cell244 = fk_node[i][2];
            while (cell244 >= 0 && fk_node[cell244][0] == 242) {
                fk_vp(fk_walk(fk_node[cell244][1], fp));
                cell244 = fk_node[cell244][2];
            }
            long long n244 = fk_vsp - base244;
            if (fk_observe_on()) {
                printf("offer-indirect fn%lld args=%lld (computed head)\n", fi244, n244);
            }
            long long m244 = 0;
            while (m244 < n244) {
                fk_vs[fp + m244] = fk_vs[base244 + m244];
                m244 = m244 + 1;
            }
            fk_vsp = fp + n244;
            /* Populate fk_call_cap_vals LAST, immediately before the jump -- an adversarial
             * review caught this written any earlier (right after resolving hv244, before the
             * args above were walked): an argument expression can itself be an indirect call
             * into ANOTHER capturing closure, which would populate this SAME shared scratch
             * buffer for ITS OWN callee and leave it clobbered by the time control finally
             * reached here. Nothing after this point can make another call before the jump, so
             * this is the one place in the arm where the "nothing runs between write and read"
             * invariant fk_call_cap_vals depends on is actually true. */
            if (fk_fnval_is_closure(hv244)) {
                long long inst244 = fk_fnval_idx(hv244) - FK_CLOSURE_IDX_BASE;
                long long cb244 = fk_clo_capbase[inst244];
                long long cn244 = fk_clo_capcount[inst244];
                long long ci244 = 0;
                while (ci244 < cn244) {
                    fk_call_cap_vals[ci244] = fk_clo_capvals[cb244 + ci244];
                    ci244 = ci244 + 1;
                }
            }
            fk_fn_heat[fi244] = fk_fn_heat[fi244] + 1;
            fk_cur_fn = fi244;
            fk_heat_pulse();
            i = fk_fn[fi244];
            continue;
        }
        if (t == 44) {
            long long fv44 = fk_walk(fk_node[i][1], fp);
            fk_vp(fv44);
            long long av44 = fk_walk(fk_node[i][2], fp);
            fk_vp(av44);
            long long p44 = fk_vs[fk_vsp - 2] >> 1;
            if (p44 < 1 || !FK_POK(p44)) {
                fk_vsp = fp;
                return 0;
            }
            long long f44 = FK_HH(p44) >> 1;
            long long p44t = FK_HT(p44) >> 1;
            if (p44t < 1 || !FK_POK(p44t)) {
                fk_vsp = fp;
                return 0;
            }
            long long a44 = FK_HH(p44t) >> 1;
            long long caps44 = FK_HT(p44t);
            long long args44 = fk_vs[fk_vsp - 1];
            long long rev44 = 1;
            long long cc44 = caps44 >> 1;
            while (cc44 >= 1 && FK_POK(cc44)) {
                /* melt is unsafe mid-copy (cc44/rev44 are untraced C locals);
                 * growth relocates nothing. Before this guard the bump wrote
                 * PAST the arrays at the brim -- corruption, not refusal. */
                if (fk_hp + 1 >= fk_cap) {
                    fk_heap_grow();
                }
                fk_hp = fk_hp + 1;
                fk_hh[fk_hp] = FK_HH(cc44);
                fk_ht[fk_hp] = rev44;
                rev44 = (fk_hp << 1) | 1;
                cc44 = FK_HT(cc44) >> 1;
            }
            long long comb44 = args44;
            long long rr44 = rev44 >> 1;
            while (rr44 >= 1 && FK_POK(rr44)) {
                if (fk_hp + 1 >= fk_cap) {
                    fk_heap_grow();
                }
                fk_hp = fk_hp + 1;
                fk_hh[fk_hp] = FK_HH(rr44);
                fk_ht[fk_hp] = comb44;
                comb44 = (fk_hp << 1) | 1;
                rr44 = FK_HT(rr44) >> 1;
            }
            long long carg44 = 1;
            if (a44 == 0) {
                carg44 = 1;
            } else {
                if (a44 == 1) {
                    long long pa44 = comb44 >> 1;
                    if (pa44 < 1 || !FK_POK(pa44)) {
                        fk_vsp = fp;
                        return 1;
                    }
                    carg44 = FK_HH(pa44);
                } else {
                    carg44 = comb44;
                }
            }
            if (f44 < 0 || f44 >= fk_fn_count) {
                fk_vsp = fp;
                return 0;
            }
            fk_vs[fp] = carg44;
            fk_vsp = fp + 1;
            fk_fn_heat[f44] = fk_fn_heat[f44] + 1;
            fk_cur_fn = f44;
            fk_heat_pulse();
            i = fk_fn[f44];
            continue;
        }
        return fk_walk(i, fp);
    }
}
/* host_spawn (155) / host_capture (159) / host_spawn_quiet (161): fork and execvp the argument list itself -- no shell reads it.
 * spawn answers the pid at once; capture answers the child stdout after it ends; quiet sends the child stdout and stderr to /dev/null. */
static long long fk_host_spawn_arm(long long argv155, long long t) {
        /* host_spawn argv / host_capture argv: fork and execvp the argument list itself -- no shell reads it. spawn answers the pid at once; capture answers the child's stdout after it ends */

        static char ab155[32][1024];
        char *av155[33];
        long long n155 = 0;
        long long p155 = argv155 >> 1;
        while (p155 >= 1 && FK_POK(p155) && n155 < 32) {
            fk_cstr(FK_HH(p155), ab155[n155], 1024);
            av155[n155] = ab155[n155];
            n155 = n155 + 1;
            p155 = FK_HT(p155) >> 1;
        }
        av155[n155] = 0;
        if (n155 == 0) { return fk_nothing; }
        int fds155[2];
        fds155[0] = -1; fds155[1] = -1;
        if (t == 159 && pipe(fds155) != 0) { return fk_nothing; }
        long long pid155 = fork();
        if (pid155 < 0) { return fk_nothing; }
        if (pid155 == 0) {
            if (t == 159) { dup2(fds155[1], 1); close(fds155[0]); close(fds155[1]); }
            if (t == 161) { int nul161 = open("/dev/null", 1); if (nul161 >= 0) { dup2(nul161, 1); dup2(nul161, 2); close(nul161); } }
            execvp(av155[0], av155);
            _exit(127);
        }
        if (t == 155 || t == 161) { return pid155 << 1; }
        close(fds155[1]);
        static char ob155[1048576];
        long long tot155 = 0;
        while (tot155 < 1048575) {
            long long got155 = read(fds155[0], ob155 + tot155, (unsigned long)(1048575 - tot155));
            if (got155 <= 0) { break; }
            tot155 = tot155 + got155;
        }
        close(fds155[0]);
        int st155 = 0;
        waitpid((int)pid155, &st155, 0);
        return fk_sbuf(ob155, tot155);
}
/* ── the doors that end the shell ─────────────────────────────────────────────
 * Three names, modes 17-19 of the leaf door. NO AST TAG WAS TAKEN. The space
 * 0..255 has nothing to spend: 150 is the reserved native-surface probe and 190
 * is FK_TAG_CONST_HOLD however free a census of `if (t == N)` sites calls it
 * (corpus rows 1358 limbkept, 1373 freefeint -- a census that enumerates
 * occupants in one notation cannot certify a vacancy). So this family rides
 * float_leaf the way the binary form, substring and the speaking mouth do.
 *
 * They exist because observe/form-glass-ear-live.fk stood its two lanes through
 * `sh -c` -- not to run a shell program, but only to say where the child's three
 * standard streams go. A lane born holding the parent's own terminal stdin sits
 * at its first read forever (receipts/2026-09-07-a-lane-born-mute.md, corpus row
 * 1339 mutebirth). The REASON was sound; the shell was furniture.
 *
 *   host_spawn_at argv (list in out err)
 *       The missing sibling of host_spawn / host_spawn_quiet / host_wait /
 *       host_kill: fork and execvp the argument list itself -- no shell reads it,
 *       so a path holding a space or a metacharacter is a path and not a program.
 *       Each standard stream opens where the caller says; "" keeps the parent's.
 *       OUT AND ERR OPEN APPEND, because a log that chronicles every life a lane
 *       has had must not be truncated by the next birth, and an err path
 *       byte-equal to the out path SHARES the one descriptor -- which is all
 *       `2>&1` ever meant.
 *
 *       Four refusals, each answering its own question, and every one of them
 *       arriving BEFORE the caller holds a pid it could mistake for a live child:
 *         -1 the argv      nothing here can be run: not a list, empty, or a
 *                          word in it is not a string
 *         -2 the redirect  a stream could not be opened where it was sent
 *         -3 the fork      the host refused to fork
 *         -4 the binary    the fork happened and execvp did not
 *
 *       The last is why this door carries a pipe. host_spawn answers a pid
 *       whatever follows, and a missing binary surfaces later as a 127 from
 *       host_wait -- the very shape the ear cell calls indistinguishable from a
 *       quiet room. Here the child holds the write end of a close-on-exec pipe:
 *       a successful execvp closes it and the parent's read gives 0; a failed one
 *       writes its errno and _exit(127)s, and the parent reaps the corpse and
 *       answers -4. A lane that could not be born says so AT BIRTH. The cost of
 *       that honesty is that the door answers after the exec rather than at the
 *       fork -- and exec is not the child's work, so the whole call still lands
 *       inside the plain spawn's own millisecond.
 *
 *   host_alive pid   1 the pid answers, 0 it is gone, -1 no pid was given.
 *       kill(pid, 0) delivers no signal. EPERM is a LIVING process that is not
 *       ours, so it answers 1: "gone" is ESRCH and nothing else. This is the
 *       whole of what `kill -0 <pid> && printf L` was doing through a fork.
 *
 *   fs_mkfifo path   1 the fifo was made, 0 a fifo already stands there,
 *       -1 refused (the path holds something that is not a fifo, or the host
 *       said no). A ring into an absent bell leaves a one-byte REGULAR file
 *       behind and mkfifo over it fails without changing anything; both of the
 *       ear's bells were found in exactly that state. This door REPORTS what is
 *       there and never removes a path on its own -- the Form cell decides.
 */
static long long fk_host_door(long long mode, long long x) {
    if (mode == 18) {
        /* host_alive pid */
        if ((x & 1) != 0) { return (0 - 1) * 2; }
        long long pid18 = x >> 1;
        if (pid18 <= 0) { return (0 - 1) * 2; }
        if (kill((int)pid18, 0) == 0) { return 1 * 2; }
        if (errno == ESRCH) { return 0; }
        return 1 * 2;
    }
    if (mode == 19) {
        /* fs_mkfifo path */
        static char pf19[FK_PATH_CAP];
        if (!fk_is_str(x)) { return (0 - 1) * 2; }
        fk_cstr(x, pf19, FK_PATH_CAP);
        if (pf19[0] == 0) { return (0 - 1) * 2; }
        struct stat st19;
        if (stat(pf19, &st19) == 0) {
            if (S_ISFIFO(st19.st_mode)) { return 0; }
            return (0 - 1) * 2;
        }
        if (mkfifo(pf19, 0600) == 0) { return 1 * 2; }
        return (0 - 1) * 2;
    }
    if (mode != 17) { return fk_nothing; }
    /* host_spawn_at (cons argv redirects) */
    if ((x & 1) == 0 || fk_is_str(x)) { return (0 - 1) * 2; }
    long long pr17 = x >> 1;
    if (pr17 < 1 || !FK_POK(pr17)) { return (0 - 1) * 2; }
    long long argv17 = FK_HH(pr17);
    long long redir17 = FK_HT(pr17);

    static char ab17[32][1024];
    char *av17[33];
    long long n17 = 0;
    long long p17 = argv17 >> 1;
    while (p17 >= 1 && FK_POK(p17) && n17 < 32) {
        if (!fk_is_str(FK_HH(p17))) { return (0 - 1) * 2; }
        fk_cstr(FK_HH(p17), ab17[n17], 1024);
        av17[n17] = ab17[n17];
        n17 = n17 + 1;
        p17 = FK_HT(p17) >> 1;
    }
    av17[n17] = 0;
    if (n17 == 0 || av17[0][0] == 0) { return (0 - 1) * 2; }

    /* the three redirect paths, in order; a short list leaves the rest inherited */
    static char rp17[3][FK_PATH_CAP];
    long long k17 = 0;
    while (k17 < 3) { rp17[k17][0] = 0; k17 = k17 + 1; }
    k17 = 0;
    long long q17 = redir17 >> 1;
    while (k17 < 3 && q17 >= 1 && FK_POK(q17)) {
        if (fk_is_str(FK_HH(q17))) { fk_cstr(FK_HH(q17), rp17[k17], FK_PATH_CAP); }
        k17 = k17 + 1;
        q17 = FK_HT(q17) >> 1;
    }

    int rfd17[3];
    rfd17[0] = -1; rfd17[1] = -1; rfd17[2] = -1;
    if (rp17[0][0] != 0) {
        rfd17[0] = open(rp17[0], O_RDONLY);
        if (rfd17[0] < 0) { return (0 - 2) * 2; }
    }
    if (rp17[1][0] != 0) {
        rfd17[1] = open(rp17[1], O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (rfd17[1] < 0) {
            if (rfd17[0] >= 0) { close(rfd17[0]); }
            return (0 - 2) * 2;
        }
    }
    if (rp17[2][0] != 0) {
        /* the same path twice is one descriptor: that is what 2>&1 meant */
        if (rfd17[1] >= 0 && fk_cstr_eq(rp17[2], rp17[1])) {
            rfd17[2] = rfd17[1];
        } else {
            rfd17[2] = open(rp17[2], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (rfd17[2] < 0) {
                if (rfd17[0] >= 0) { close(rfd17[0]); }
                if (rfd17[1] >= 0) { close(rfd17[1]); }
                return (0 - 2) * 2;
            }
        }
    }

    int ef17[2];
    if (pipe(ef17) != 0) {
        if (rfd17[0] >= 0) { close(rfd17[0]); }
        if (rfd17[1] >= 0) { close(rfd17[1]); }
        if (rfd17[2] >= 0 && rfd17[2] != rfd17[1]) { close(rfd17[2]); }
        return (0 - 3) * 2;
    }
    fcntl(ef17[1], F_SETFD, FD_CLOEXEC);

    long long pid17 = fork();
    if (pid17 < 0) {
        close(ef17[0]); close(ef17[1]);
        if (rfd17[0] >= 0) { close(rfd17[0]); }
        if (rfd17[1] >= 0) { close(rfd17[1]); }
        if (rfd17[2] >= 0 && rfd17[2] != rfd17[1]) { close(rfd17[2]); }
        return (0 - 3) * 2;
    }
    if (pid17 == 0) {
        close(ef17[0]);
        if (rfd17[0] >= 0) { dup2(rfd17[0], 0); }
        if (rfd17[1] >= 0) { dup2(rfd17[1], 1); }
        if (rfd17[2] >= 0) { dup2(rfd17[2], 2); }
        if (rfd17[0] > 2) { close(rfd17[0]); }
        if (rfd17[1] > 2) { close(rfd17[1]); }
        if (rfd17[2] > 2 && rfd17[2] != rfd17[1]) { close(rfd17[2]); }
        execvp(av17[0], av17);
        {
            int en17c = errno;
            long long w17 = write(ef17[1], &en17c, sizeof(int));
            (void)w17;
        }
        _exit(127);
    }
    close(ef17[1]);
    if (rfd17[0] >= 0) { close(rfd17[0]); }
    if (rfd17[1] >= 0) { close(rfd17[1]); }
    if (rfd17[2] >= 0 && rfd17[2] != rfd17[1]) { close(rfd17[2]); }
    int en17 = 0;
    long long got17 = read(ef17[0], &en17, sizeof(int));
    close(ef17[0]);
    if (got17 == (long long)sizeof(int)) {
        int st17 = 0;
        waitpid((int)pid17, &st17, 0);
        return (0 - 4) * 2;
    }
    return pid17 * 2;
}
/* ── crystallize-on-boxing: the f64 leaf ─────────────────────────────────────
 * The box ledger is the trigger. fk_fbox charges every float box to the defn
 * running; each time a defn's count crosses a FK_F64_HEAT boundary while it is
 * still cold, fk_f64_pulse asks whether its body is a pure float expression --
 * float/int literals, parameters, add/sub/mul/div, nothing else -- and if so
 * emits it as arm64 over the d-registers: parameters arrive in d0..d7,
 * intermediates live in d16..d31, one FMOV and RET. The frame's floats unbox
 * once at the door and the result boxes once on the way out; the boxes the
 * walker minted for every intermediate are folded into registers. The defn's
 * body entry becomes a tag-194 node (fn index, original body) so dispatch pays
 * nothing new: the walker meets the tag where it already reads one. A call
 * whose arguments are not all floats walks the original body -- the walker's
 * int/int arithmetic stays exact and the leaf never guesses. A declined body
 * is marked -1 in the native ledger and never asked again; a reload clears all.
 */
#define FK_F64_HEAT 1024
#define FK_F64_NODE_CAP 128
#define FK_F64_WORD_CAP 1000
/* ── the loop lane ────────────────────────────────────────────────────────────
 * A defn whose body is `(if <compare> <exit> <self tail call>)` (either branch
 * order) is a loop: every iteration is one heat-lane dispatch. The heat ledger
 * is its trigger -- each time a defn's heat crosses a FK_F64_HEAT boundary on a
 * tail-jump arm, fk_f64_loop_pulse reads the TYPE SIGNATURE off the live frame
 * (int = even tagged word, float = pool box) and emits the loop for exactly
 * that signature: parameters live in registers (ints UNTAGGED in x10..x17,
 * floats in d0..d7), the compare is CMP/FCMP + B.cond, the tail call is a
 * parallel move into the parameter registers and a branch to the loop head,
 * the exit expression lands in x0/d0, RET. Int arithmetic has its own lane --
 * ADD/SUB/MADD/SDIV on 64-bit registers, exact like the walker's word math
 * (the walker's tagged `a + b` IS untagged a + b re-tagged; mul and div untag
 * first, as here); a float on either side promotes the int through SCVTF, the
 * walker's fk_num rule. The body is emitted TWICE per pass (unrolled by two,
 * each copy keeping its own exit test, so no trip-count parity is assumed).
 * The frame arrives as nine words -- eight raw args, one slot the loop fills
 * with its iteration count -- and the door untags/unboxes once, tags/boxes
 * once. A frame whose types differ from the emitted signature walks the
 * original body: only the first-seen signature crystallizes.
 * fk_fn_native 2 = loop standing; kernel_stat 49 counts standing loops, 50 the
 * iterations that ran native; live page words 31/32 the same.
 */
typedef struct { int kind; int a; int b; double lit; long long ilit; } fk_f64_node; /* kind: 1 flit 2 farg 3 fadd 4 fsub 5 fmul 6 fdiv 7 ilit 8 iarg 9 iadd 10 isub 11 imul 12 idiv 13 cvt int->float */
static void **fk_f64_mem;
static long long *fk_f64_sig;
static long long fk_f64_cap;
static long long fk_f64_count_private;
static long long *fk_f64_count_p = &fk_f64_count_private;
#define fk_f64_count (*fk_f64_count_p)
static long long fk_f64_loop_count_private;
static long long *fk_f64_loop_count_p = &fk_f64_loop_count_private;
#define fk_f64_loop_count (*fk_f64_loop_count_p)
static long long fk_f64_loop_iters_private;
static long long *fk_f64_loop_iters_p = &fk_f64_loop_iters_private;
static long long fk_mint_total_private; /* value nodes this kernel minted (interned fresh): the site that grows the permanent arena, counted where it happens */
static long long *fk_mint_total_p = &fk_mint_total_private;
#define fk_mint_total (*fk_mint_total_p)
#define fk_f64_loop_iters (*fk_f64_loop_iters_p)
static fk_f64_node fk_f64_prog[FK_F64_NODE_CAP];
static long long fk_f64_prog_n;
static void fk_f64_reset(void) {
    long long k = 0;
    while (k < fk_f64_cap) {
        if (fk_f64_mem[k] != 0) { munmap(fk_f64_mem[k], 4096); fk_f64_mem[k] = 0; }
        fk_f64_sig[k] = -1;
        k = k + 1;
    }
    k = 0;
    while (fk_fn_native != 0 && k < fk_fn_capacity) { fk_fn_native[k] = 0; k = k + 1; }
    fk_f64_count = 0;
    fk_f64_loop_count = 0;
}
/* room in the leaf tables for defn fx; 0 on allocation failure */
static int fk_f64_reserve(long long fx) {
    if (fx < fk_f64_cap) { return 1; }
    long long next = fk_f64_cap > 0 ? fk_f64_cap : 256;
    while (next <= fx) { next = next << 1; }
    void **grown = (void **)malloc((size_t)next * sizeof(void *));
    long long *gsig = (long long *)malloc((size_t)next * sizeof(long long));
    if (grown == 0 || gsig == 0) { free(grown); free(gsig); return 0; }
    long long k = 0;
    while (k < next) { grown[k] = k < fk_f64_cap ? fk_f64_mem[k] : 0; gsig[k] = k < fk_f64_cap ? fk_f64_sig[k] : -1; k = k + 1; }
    free(fk_f64_mem); free(fk_f64_sig);
    fk_f64_mem = grown; fk_f64_sig = gsig;
    fk_f64_cap = next;
    return 1;
}
static int fk_f64_push(int kind, int a, int b, double lit, long long ilit) {
    if (fk_f64_prog_n >= FK_F64_NODE_CAP) { return -1; }
    fk_f64_prog[fk_f64_prog_n].kind = kind; fk_f64_prog[fk_f64_prog_n].a = a; fk_f64_prog[fk_f64_prog_n].b = b;
    fk_f64_prog[fk_f64_prog_n].lit = lit; fk_f64_prog[fk_f64_prog_n].ilit = ilit;
    fk_f64_prog_n = fk_f64_prog_n + 1;
    return (int)(fk_f64_prog_n - 1);
}
static int fk_f64_type_of(int n) { int k = fk_f64_prog[n].kind; return (k >= 7 && k <= 12) ? 1 : 2; }
/* an int-typed node promoted to float (SCVTF) -- the walker's fk_num on the int side of a mixed op */
static int fk_f64_cvt(int n) { return fk_f64_type_of(n) == 2 ? n : fk_f64_push(13, n, 0, 0.0, 0); }
/* admit a body node under a per-parameter type signature (types[k]: 1 int, 2 float): returns 0 declined, 1 int-typed,
 * 2 float-typed; appends to fk_f64_prog, *out = its index */
static int fk_f64_admit(long long i, long long arity, const int *types, int *out) {
    if (i < 0 || i >= fk_node_count || fk_f64_prog_n >= FK_F64_NODE_CAP) { return 0; }
    long long t = fk_node[i][0];
    if (t == 1) {
        *out = fk_f64_push(7, 0, 0, 0.0, fk_node[i][1]); /* exact 64-bit; SCVTF rounds it the way (double)v does when a float meets it */
        return *out < 0 ? 0 : 1;
    }
    if (t == 53) {
        long long lc = fk_node[i][1];
        if (lc < 0 || lc >= fk_node_count || fk_node[lc][0] != 24) { return 0; }
        long long w = fk_walk(i, fk_vsp); /* the literal's own memo: one box per process, minted here if not yet */
        if (!fk_isf(w)) { return 0; }
        *out = fk_f64_push(1, 0, 0, FK_FV(fk_fidx(w)), 0);
        return *out < 0 ? 0 : 2;
    }
    if (t == 2 || t == 110) {
        long long k = 0;
        if (t == 110) {
            long long li = fk_node[i][1];
            if (li < 0 || li >= fk_node_count || fk_node[li][0] != 1) { return 0; }
            k = fk_node[li][1];
        }
        if (k < 0 || k >= arity) { return 0; }
        *out = fk_f64_push(types[k] == 2 ? 2 : 8, (int)k, 0, 0.0, 0);
        return *out < 0 ? 0 : types[k];
    }
    if (t == 3 || t == 4 || t == 42 || t == 10) {
        int a = 0, b = 0;
        int ta = fk_f64_admit(fk_node[i][1], arity, types, &a);
        if (ta == 0) { return 0; }
        int tb = fk_f64_admit(fk_node[i][2], arity, types, &b);
        if (tb == 0) { return 0; }
        int op = t == 3 ? 0 : (t == 4 ? 1 : (t == 42 ? 2 : 3));
        if (ta == 2 || tb == 2) {
            a = fk_f64_cvt(a); b = fk_f64_cvt(b);
            if (a < 0 || b < 0) { return 0; }
            *out = fk_f64_push(3 + op, a, b, 0.0, 0);
            return *out < 0 ? 0 : 2;
        }
        *out = fk_f64_push(9 + op, a, b, 0.0, 0); /* the int lane: exact 64-bit word math, the walker's own */
        return *out < 0 ? 0 : 1;
    }
    return 0;
}
static int fk_f64_put(unsigned int *words, long long *n, unsigned int w) {
    if (*n >= FK_F64_WORD_CAP) { return 0; }
    words[*n] = w; *n = *n + 1;
    return 1;
}
/* register codes the emitter hands back: 0..7 d-register argument, 16..31 d-register temp; 100+x an x-register:
 * x10..x17 int arguments, x1..x7 int temps. x0 is the frame pointer, x8 the iteration counter, x9 literal scratch. */
static void fk_f64_release(int r, int *ntemp, int *nitemp) {
    if (r >= 101 && r <= 107) { *nitemp = *nitemp - 1; }
    else if (r >= 16 && r <= 31) { *ntemp = *ntemp - 1; }
}
static int fk_f64_is_temp(int r) { return (r >= 101 && r <= 107) || (r >= 16 && r <= 31); }
/* MOVZ + 3 MOVK of a 64-bit pattern into x-register xd */
static int fk_f64_mov64(unsigned int *words, long long *wn, unsigned int xd, unsigned long long bits) {
    unsigned int hw = 0;
    while (hw < 4) {
        unsigned int imm = (unsigned int)((bits >> (16 * hw)) & 0xFFFFULL);
        unsigned int op = hw == 0 ? 0xD2800000U : 0xF2800000U;
        if (!fk_f64_put(words, wn, op | (hw << 21) | (imm << 5) | xd)) { return 0; }
        hw = hw + 1;
    }
    return 1;
}
/* postorder emit; returns the register code holding the node's value, -1 on overflow */
static int fk_f64_emit(int n, unsigned int *words, long long *wn, int *ntemp, int *nitemp) {
    fk_f64_node *p = &fk_f64_prog[n];
    if (p->kind == 2) { return p->a; }
    if (p->kind == 8) { return 110 + p->a; }
    if (p->kind == 1) {
        if (*ntemp >= 16) { return -1; }
        int rd = 16 + *ntemp; *ntemp = *ntemp + 1;
        unsigned long long bits; memcpy(&bits, &p->lit, 8);
        if (!fk_f64_mov64(words, wn, 9U, bits)) { return -1; }
        if (!fk_f64_put(words, wn, 0x9E670000U | (9U << 5) | (unsigned int)rd)) { return -1; } /* FMOV Dd, X9 */
        return rd;
    }
    if (p->kind == 7) {
        if (*nitemp >= 7) { return -1; }
        int rd = 1 + *nitemp; *nitemp = *nitemp + 1;
        if (!fk_f64_mov64(words, wn, (unsigned int)rd, (unsigned long long)p->ilit)) { return -1; }
        return 100 + rd;
    }
    if (p->kind == 13) {
        int ra = fk_f64_emit(p->a, words, wn, ntemp, nitemp);
        if (ra < 100) { return -1; }
        fk_f64_release(ra, ntemp, nitemp);
        if (*ntemp >= 16) { return -1; }
        int rd = 16 + *ntemp; *ntemp = *ntemp + 1;
        if (!fk_f64_put(words, wn, 0x9E620000U | ((unsigned int)(ra - 100) << 5) | (unsigned int)rd)) { return -1; } /* SCVTF Dd, Xn */
        return rd;
    }
    int ra = fk_f64_emit(p->a, words, wn, ntemp, nitemp);
    if (ra < 0) { return -1; }
    int rb = fk_f64_emit(p->b, words, wn, ntemp, nitemp);
    if (rb < 0) { return -1; }
    fk_f64_release(rb, ntemp, nitemp);
    fk_f64_release(ra, ntemp, nitemp);
    if (p->kind >= 9 && p->kind <= 12) {
        if (ra < 100 || rb < 100 || *nitemp >= 7) { return -1; }
        unsigned int xa = (unsigned int)(ra - 100), xb = (unsigned int)(rb - 100);
        int rd = 1 + *nitemp; *nitemp = *nitemp + 1;
        unsigned int op = p->kind == 9 ? 0x8B000000U : (p->kind == 10 ? 0xCB000000U : (p->kind == 11 ? 0x9B007C00U : 0x9AC00C00U)); /* ADD SUB MADD(xzr) SDIV */
        if (!fk_f64_put(words, wn, op | (xb << 16) | (xa << 5) | (unsigned int)rd)) { return -1; }
        return 100 + rd;
    }
    if (ra >= 100 || rb >= 100 || *ntemp >= 16) { return -1; }
    int rd = 16 + *ntemp; *ntemp = *ntemp + 1;
    unsigned int op = p->kind == 3 ? 0x1E602800U : (p->kind == 4 ? 0x1E603800U : (p->kind == 5 ? 0x1E600800U : 0x1E601800U));
    if (!fk_f64_put(words, wn, op | ((unsigned int)rb << 16) | ((unsigned int)ra << 5) | (unsigned int)rd)) { return -1; }
    return rd;
}
/* the defn's body with the 194 wrapper and a parameter-only let frame peeled: 0 when the shape is not a leaf's */
static int fk_f64_body_of(long long fx, long long *root_out, long long *orig_out, long long *body_out) {
    long long arity = fk_fnar[fx];
    if (arity < 1 || arity > 8) { return 0; }
    long long root = fk_fn[fx];
    if (root < 0 || root >= fk_node_count) { return 0; }
    long long body = root;
    if (fk_node[body][0] == 194) { body = fk_node[body][2]; }
    long long orig = body;
    if (body >= 0 && body < fk_node_count && fk_node[body][0] == 111) {
        long long li = fk_node[body][1];
        long long slots = (li >= 0 && li < fk_node_count && fk_node[li][0] == 1) ? fk_node[li][1] : -1;
        if (slots < 0 || slots + 1 > arity) { return 0; } /* a let slot beyond the parameters: not this leaf's shape */
        body = fk_node[body][2];
    }
    if (body < 0 || body >= fk_node_count) { return 0; }
    *root_out = root; *orig_out = orig; *body_out = body;
    return 1;
}
/* the words land on a MAP_JIT page and the defn's entry becomes the tag-194 door */
static int fk_f64_install(long long fx, long long root, long long orig, unsigned int *words, long long wn, long long sig, long long state) {
#if defined(FK_HAVE_DARWIN_ARM64_JIT_WITNESS)
    void *mem = mmap(0, 4096, 0x7, 0x1802, -1, 0);
    if (mem == (void *)-1) { return 0; }
    pthread_jit_write_protect_np(0);
    memcpy(mem, words, (size_t)(wn * 4));
    pthread_jit_write_protect_np(1);
    __builtin___clear_cache((char *)mem, (char *)mem + wn * 4);
    if (!fk_f64_reserve(fx)) { munmap(mem, 4096); return 0; }
    fk_f64_mem[fx] = mem;
    fk_f64_sig[fx] = sig;
    if (fk_node[root][0] != 194) { fk_fn[fx] = fk_smknode(194, fx, orig, 0); fk_prog_note_body(fx); }
    fk_fn_native[fx] = state;
    return 1;
#else
    (void)fx; (void)root; (void)orig; (void)words; (void)wn; (void)sig; (void)state;
    return 0;
#endif
}
/* ── recipe twins: the seed carries the body's own list and number recipes ──
 * nil?, append, int_to_str and reverse-onto are Form recipes in core.fk (and
 * their copies in line-grammar, sha256, fourth-shim, core-native, form-asm):
 * five dispatches per element, and the hottest names on every kernel's page.
 * When one of them crosses FK_F64_HEAT calls, the seed binds a twin: the
 * defn's body entry becomes a tag-194 node with native state 3 and the twin id
 * in fk_f64_sig, and the walker meets the twin where it already reads a tag.
 * A twin answers exactly what the recipe answers on the inputs it claims --
 * lists, the empty list, strings, ints, nothing -- and DECLINES anything else
 * (int_to_str of a float, reverse-onto of a string), so the original body
 * walks and the recipe's own answer stands. Bound by name, arity AND the
 * defining unit, so a recipe of the same name in another unit is never taken. */
static const char *fk_twin_units[6] = {"core.fk", "line-grammar.fk", "sha256.fk", "fourth-shim.fk", "core-native.fk", "form-asm.fk"};
static int fk_twin_name_eq(const char *s, long long n, const char *name) {
    long long k = 0;
    while (k < n) { if (name[k] == 0 || s[k] != name[k]) { return 0; } k = k + 1; }
    return name[n] == 0;
}
static int fk_twin_leaf_eq(const char *unit, const char *leaf) {
    long long n = 0, m = 0;
    while (unit[n] != 0) { n = n + 1; }
    while (leaf[m] != 0) { m = m + 1; }
    if (m > n) { return 0; }
    if (n > m && unit[n - m - 1] != '/') { return 0; }
    long long k = 0;
    while (k < m) { if (unit[n - m + k] != leaf[k]) { return 0; } k = k + 1; }
    return 1;
}
static long long fk_twin_id_of(long long fx) {
    long long j = 0;
    while (j < fk_fntop) {
        if (fk_fnidx[j] == fx) {
            long long so = fk_fnsym_s[j], sn = fk_fnsym_n[j];
            long long id = 0;
            if (sn == 4 && fk_srctext[so] == 'n' && fk_srctext[so + 1] == 'i' && fk_srctext[so + 2] == 'l' && fk_srctext[so + 3] == '?' && fk_fnar[fx] == 1) { id = 1; }
            else if (sn == 6 && fk_twin_name_eq(fk_srctext + so, 6, "append") && fk_fnar[fx] == 2) { id = 2; }
            else if (sn == 10 && fk_twin_name_eq(fk_srctext + so, 10, "int_to_str") && fk_fnar[fx] == 1) { id = 3; }
            else if (sn == 12 && fk_twin_name_eq(fk_srctext + so, 12, "reverse-onto") && fk_fnar[fx] == 2) { id = 4; }
            if (id == 0) { return 0; }
            const char *unit = fk_hot_unit_of(so);
            long long u = 0;
            while (u < 6) { if (fk_twin_leaf_eq(unit, fk_twin_units[u])) { return id; } u = u + 1; }
            return 0;
        }
        j = j + 1;
    }
    return 0;
}
static void fk_twin_pulse(long long fx) {
    if (fx <= 0 || fx >= fk_fn_count || fk_fn_native == 0 || fx >= fk_fn_capacity) { return; }
    if (fk_fn_native[fx] != 0) { return; }
    long long id = fk_twin_id_of(fx);
    if (id == 0) { return; }
    if (!fk_f64_reserve(fx)) { return; }
    long long root = fk_fn[fx];
    if (root < 0 || root >= fk_node_count) { return; }
    if (fk_node[root][0] != 194) { fk_fn[fx] = fk_smknode(194, fx, root, 0); fk_prog_note_body(fx); }
    fk_f64_sig[fx] = id;
    fk_fn_native[fx] = 3;
}
static int fk_twin_call(long long fx, long long fp, long long *out) {
    long long id = fk_f64_sig[fx];
    if (id == 1) {
        *out = fk_len_upto(fk_vs[fp], 1) == 0 ? 2 : 0;
        return 1;
    }
    if (id == 2) {
        long long xs = fk_vs[fp], ys = fk_vs[fp + 1];
        if (fk_is_str(xs)) { *out = FK_SLEN(fk_stri(xs)) > 0 ? fk_cons_val(1, ys) : ys; return 1; }
        if ((xs & 1) == 0) { *out = ys; return 1; }
        long long p = xs >> 1, n = 0;
        while (p >= 1 && FK_POK(p)) { fk_vp(FK_HH(p)); n = n + 1; p = FK_HT(p) >> 1; }
        long long acc = ys;
        while (n > 0) { fk_vsp = fk_vsp - 1; acc = fk_cons_val(fk_vs[fk_vsp], acc); n = n - 1; }
        *out = acc;
        return 1;
    }
    if (id == 3) {
        long long v = fk_vs[fp];
        if (v == fk_nothing) { *out = fk_sbuf("nothing", 7); return 1; }
        if ((v & 1) != 0) { return 0; }
        long long n = v >> 1;
        char b[32];
        int k = 31;
        b[k] = 0;
        int neg = n < 0;
        unsigned long long u = neg ? (unsigned long long)(0 - n) : (unsigned long long)n;
        if (u == 0) { k = k - 1; b[k] = '0'; }
        while (u > 0) { k = k - 1; b[k] = (char)('0' + (u % 10)); u = u / 10; }
        if (neg) { k = k - 1; b[k] = '-'; }
        *out = fk_sbuf(b + k, 31 - k);
        return 1;
    }
    if (id == 4) {
        long long xs = fk_vs[fp], acc = fk_vs[fp + 1];
        if (fk_is_str(xs)) { if (FK_SLEN(fk_stri(xs)) > 0) { return 0; } *out = acc; return 1; }
        if ((xs & 1) == 0) { *out = acc; return 1; }
        long long p = xs >> 1;
        while (p >= 1 && FK_POK(p)) { acc = fk_cons_val(FK_HH(p), acc); p = FK_HT(p) >> 1; }
        *out = acc;
        return 1;
    }
    return 0;
}
static void fk_f64_pulse(long long fx) {
    if (fx <= 0 || fx >= fk_fn_count || fk_fn_native == 0 || fx >= fk_fn_capacity) { return; }
    if (fk_fn_native[fx] != 0) { return; }
    long long root = 0, orig = 0, body = 0;
    if (!fk_f64_body_of(fx, &root, &orig, &body)) { fk_fn_native[fx] = -1; return; }
    if (fk_node[body][0] == 6) { return; } /* loop-shaped: the heat pulse reads the signature off a whole frame; stay cold until then */
    fk_fn_native[fx] = -1;
    long long arity = fk_fnar[fx];
    int types[8] = {2, 2, 2, 2, 2, 2, 2, 2}; /* the expression leaf: every parameter a float, the door holds the rest to the walker */
    fk_f64_prog_n = 0;
    int top = 0;
    if (fk_f64_admit(body, arity, types, &top) != 2) { return; }
    unsigned int words[FK_F64_WORD_CAP];
    long long wn = 0;
    int ntemp = 0, nitemp = 0;
    int rr = fk_f64_emit(top, words, &wn, &ntemp, &nitemp);
    if (rr < 0 || rr >= 100) { return; }
    if (rr != 0 && !fk_f64_put(words, &wn, 0x1E604000U | ((unsigned int)rr << 5))) { return; } /* FMOV D0, Dr */
    if (!fk_f64_put(words, &wn, 0xD65F03C0U)) { return; }
    if (!fk_f64_install(fx, root, orig, words, wn, -1, 1)) { return; }
    fk_f64_count = fk_f64_count + 1;
}
/* one pass of the loop body: compare, exit branch (recorded for patching), the tail call as a parallel move into the
 * parameter registers, the iteration count. 0 on overflow. */
static int fk_f64_loop_pass(unsigned int *words, long long *wn, int ca, int cb, unsigned int exitcc, long long *patch, long long *npatch,
                            const int *argn, const int *types, long long arity) {
    int ntemp = 0, nitemp = 0;
    int ra = fk_f64_emit(ca, words, wn, &ntemp, &nitemp);
    if (ra < 0) { return 0; }
    int rb = fk_f64_emit(cb, words, wn, &ntemp, &nitemp);
    if (rb < 0) { return 0; }
    if (ra >= 100) {
        if (!fk_f64_put(words, wn, 0xEB00001FU | ((unsigned int)(rb - 100) << 16) | ((unsigned int)(ra - 100) << 5))) { return 0; } /* CMP Xa, Xb */
    } else {
        if (!fk_f64_put(words, wn, 0x1E602000U | ((unsigned int)rb << 16) | ((unsigned int)ra << 5))) { return 0; } /* FCMP Da, Db */
    }
    if (*npatch >= 8) { return 0; }
    patch[*npatch] = *wn; *npatch = *npatch + 1;
    if (!fk_f64_put(words, wn, 0x54000000U | exitcc)) { return 0; } /* B.cond exit -- offset patched below */
    ntemp = 0; nitemp = 0;
    int held[8];
    long long k = 0;
    while (k < arity) {
        int r = fk_f64_emit(argn[k], words, wn, &ntemp, &nitemp);
        if (r < 0) { return 0; }
        if (!fk_f64_is_temp(r)) { /* a bare parameter or argument register: copy it so the parallel move cannot read a slot already written */
            if (r >= 100) {
                if (nitemp >= 7) { return 0; }
                int rd = 1 + nitemp; nitemp = nitemp + 1;
                if (!fk_f64_put(words, wn, 0xAA0003E0U | ((unsigned int)(r - 100) << 16) | (unsigned int)rd)) { return 0; } /* MOV Xd, Xr */
                r = 100 + rd;
            } else {
                if (ntemp >= 16) { return 0; }
                int rd = 16 + ntemp; ntemp = ntemp + 1;
                if (!fk_f64_put(words, wn, 0x1E604000U | ((unsigned int)r << 5) | (unsigned int)rd)) { return 0; } /* FMOV Dd, Dr */
                r = rd;
            }
        }
        held[k] = r;
        k = k + 1;
    }
    k = 0;
    while (k < arity) {
        if (types[k] == 2) {
            if (!fk_f64_put(words, wn, 0x1E604000U | ((unsigned int)held[k] << 5) | (unsigned int)k)) { return 0; } /* FMOV Dk, Dheld */
        } else {
            if (!fk_f64_put(words, wn, 0xAA0003E0U | ((unsigned int)(held[k] - 100) << 16) | (unsigned int)(10 + k))) { return 0; } /* MOV X(10+k), Xheld */
        }
        k = k + 1;
    }
    return fk_f64_put(words, wn, 0x91000508U); /* ADD X8, X8, #1 */
}
static void fk_f64_loop_pulse(long long fx, long long fp, long long n) {
    fk_twin_pulse(fx);
    if (fx <= 0 || fx >= fk_fn_count || fk_fn_native == 0 || fx >= fk_fn_capacity) { return; }
    if (fk_fn_native[fx] != 0) { return; }
    long long root = 0, orig = 0, body = 0;
    if (!fk_f64_body_of(fx, &root, &orig, &body)) { fk_fn_native[fx] = -1; return; }
    if (fk_node[body][0] != 6) { fk_f64_pulse(fx); return; } /* not a loop: the expression leaf is asked instead */
    long long arity = fk_fnar[fx];
    if (n != arity) { return; } /* a partial frame carries stale slots: no signature to read this time */
    fk_fn_native[fx] = -1;
    long long cond = fk_node[body][1], thn = fk_node[body][2], els = fk_node[body][3];
    if (cond < 0 || cond >= fk_node_count || thn < 0 || thn >= fk_node_count || els < 0 || els >= fk_node_count) { return; }
    long long ct = fk_node[cond][0];
    if (ct != 5 && ct != 102 && ct != 103) { return; }
    /* which branch is the self tail call: tag 241 on this fn index, or tag 12 (one arg) on it */
    int self_then = (fk_node[thn][0] == 241 || fk_node[thn][0] == 12) && fk_node[thn][1] == fx;
    int self_else = (fk_node[els][0] == 241 || fk_node[els][0] == 12) && fk_node[els][1] == fx;
    if (self_then == self_else) { return; }
    long long call = self_then ? thn : els;
    long long exitn = self_then ? els : thn;
    /* the signature, read off the live frame */
    int types[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    long long sig = 1LL << 9;
    long long k = 0;
    while (k < arity) {
        long long v = fk_vs[fp + k];
        if (fk_isf(v)) { types[k] = 2; sig = sig | (1LL << k); }
        else if ((v & 1) != 0) { return; } /* not a number: not this lane's frame */
        k = k + 1;
    }
    /* the tail call's arguments, one per parameter, each the type of the parameter it feeds */
    int argn[8];
    fk_f64_prog_n = 0;
    if (fk_node[call][0] == 12) {
        if (arity != 1) { return; }
        if (fk_f64_admit(fk_node[call][2], arity, types, &argn[0]) != types[0]) { return; }
    } else {
        long long cell = fk_node[call][2];
        k = 0;
        while (cell >= 0 && fk_node[cell][0] == 242 && k < arity) {
            if (fk_f64_admit(fk_node[cell][1], arity, types, &argn[k]) != types[k]) { return; }
            cell = fk_node[cell][2];
            k = k + 1;
        }
        if (k != arity || cell >= 0) { return; }
    }
    int ca = 0, cb = 0;
    int ta = fk_f64_admit(fk_node[cond][1], arity, types, &ca);
    if (ta == 0) { return; }
    int tb = fk_f64_admit(fk_node[cond][2], arity, types, &cb);
    if (tb == 0) { return; }
    int fcmp = (ta == 2 || tb == 2);
    if (fcmp) { ca = fk_f64_cvt(ca); cb = fk_f64_cvt(cb); if (ca < 0 || cb < 0) { return; } }
    int ex = 0;
    int tex = fk_f64_admit(exitn, arity, types, &ex);
    if (tex == 0) { return; }
    if (tex == 2) { sig = sig | (1LL << 8); }
    /* the condition code that means "the compare is TRUE": eq EQ; lt LT / MI; le LE / LS. Exit is taken when the compare
     * selects the exit branch: true -> then-exit is cc itself, else-exit is its inverse (cc ^ 1). */
    unsigned int cc = ct == 102 ? 0U : (ct == 103 ? (fcmp ? 4U : 11U) : (fcmp ? 9U : 13U));
    unsigned int exitcc = self_then ? (cc ^ 1U) : cc;
    unsigned int words[FK_F64_WORD_CAP];
    long long wn = 0;
    k = 0;
    while (k < arity) { /* prologue: the frame words into the parameter registers */
        unsigned int ld = types[k] == 2 ? (0xFD400000U | ((unsigned int)k << 10) | (unsigned int)k) : (0xF9400000U | ((unsigned int)k << 10) | (10U + (unsigned int)k));
        if (!fk_f64_put(words, &wn, ld)) { return; }
        k = k + 1;
    }
    if (!fk_f64_put(words, &wn, 0xAA1F03E8U)) { return; } /* MOV X8, XZR */
    long long head = wn;
    long long patch[8];
    long long npatch = 0;
    if (!fk_f64_loop_pass(words, &wn, ca, cb, exitcc, patch, &npatch, argn, types, arity)) { return; }
    if (!fk_f64_loop_pass(words, &wn, ca, cb, exitcc, patch, &npatch, argn, types, arity)) { return; } /* unrolled by two */
    if (!fk_f64_put(words, &wn, 0x14000000U | ((unsigned int)((head - wn) & 0x3FFFFFFLL)))) { return; } /* B head */
    long long exit_at = wn;
    k = 0;
    while (k < npatch) { words[patch[k]] = words[patch[k]] | ((unsigned int)((exit_at - patch[k]) & 0x7FFFFLL) << 5); k = k + 1; }
    int ntemp = 0, nitemp = 0;
    int rr = fk_f64_emit(ex, words, &wn, &ntemp, &nitemp);
    if (rr < 0) { return; }
    if (!fk_f64_put(words, &wn, 0xF9002008U)) { return; } /* STR X8, [X0, #64]: the iteration count into the ninth frame word */
    if (tex == 2) {
        if (rr >= 100) { return; }
        if (rr != 0 && !fk_f64_put(words, &wn, 0x1E604000U | ((unsigned int)rr << 5))) { return; } /* FMOV D0, Dr */
    } else {
        if (rr < 100) { return; }
        if (!fk_f64_put(words, &wn, 0xAA0003E0U | ((unsigned int)(rr - 100) << 16))) { return; } /* MOV X0, Xr */
    }
    if (!fk_f64_put(words, &wn, 0xD65F03C0U)) { return; }
    if (!fk_f64_install(fx, root, orig, words, wn, sig, 2)) { return; }
    fk_f64_count = fk_f64_count + 1;
    fk_f64_loop_count = fk_f64_loop_count + 1;
}
static long long fk_walk_cold(long long t, long long i, long long fp);
/* the honest eval-depth wall: measure REAL stack use and die SAYING SO before the host
 * stack dies silently (witnessed 2026-07-01: exit 127, no output, three recipes in one
 * day — the Windows main lacked the POSIX main's FORM_KERNEL_STACK_MB big-stack thread,
 * now mirrored below). The mains raise the wall to reserve minus 2MB; the recipe-side
 * home for deep recursion stays the same: make it tail or balanced. */
static char *fk_stack_base = 0;
static long long fk_stack_wall = 6 * 1024 * 1024;
/* ── a compare against len walks only as far as it must ─────────────────────
 * (eq (len xs) 0), (gt (len rows) 0), (le (len xs) 3): the body asks these on
 * every step of every list recursion (nil? is (eq (len xs) 0)), and len walks
 * the whole list to answer a question whose answer is settled after K+1 cells.
 * When one side of eq/lt/le is a len node and the other an int literal K, the
 * arm walks at most K+1 cells. The child is still evaluated exactly once; the
 * answer is the same word len would have given, compared the same way. */
static long long fk_len_upto(long long v, long long cap) {
    if (fk_is_str(v)) { return FK_SLEN(fk_stri(v)); }
    if ((v & 1) == 0) { return 0; }
    long long p = v >> 1;
    long long n = 0;
    while (p >= 1 && FK_POK(p) && n < cap) {
        n = n + 1;
        p = FK_HT(p) >> 1;
    }
    return n;
}
/* op: 0 eq, 1 lt, 2 le. Returns 1 and sets *out when the node has the shape (op (len X) K) or (op K (len X)). */
static int fk_len_cmp(long long i, long long fp, int op, long long *out) {
    long long c1 = fk_node[i][1], c2 = fk_node[i][2];
    if (c1 < 0 || c2 < 0 || c1 >= fk_node_count || c2 >= fk_node_count) { return 0; }
    long long t1 = fk_node[c1][0], t2 = fk_node[c2][0];
    if (t1 == 22 && t2 == 1 && fk_node[c2][1] >= 0) {
        long long k = fk_node[c2][1];
        long long n = fk_len_upto(fk_walk(fk_node[c1][1], fp), k + 1);
        *out = (op == 0 ? (n == k) : (op == 1 ? (n < k) : (n <= k))) ? 2 : 0;
        return 1;
    }
    if (t2 == 22 && t1 == 1 && fk_node[c1][1] >= 0) {
        long long k = fk_node[c1][1];
        long long n = fk_len_upto(fk_walk(fk_node[c2][1], fp), k + 1);
        *out = (op == 0 ? (k == n) : (op == 1 ? (k < n) : (k <= n))) ? 2 : 0;
        return 1;
    }
    return 0;
}
static long long fk_walk(long long i, long long fp) {
    char fk_sp_probe;
    if (fk_stack_base != 0 && (long long)(fk_stack_base - &fk_sp_probe) > fk_stack_wall) {
        printf("fkwu: eval too deep — %lld bytes of walker stack (wall %lld). The recursion "
               "needs to be tail or balanced; the wall is honest, the silent crash was not.\n",
               (long long)(fk_stack_base - &fk_sp_probe), fk_stack_wall);
        fk_die("eval-depth wall");
    }
    long long t = fk_node[i][0];
    if (t < 0 || t >= FK_OPCODE_ARM_CAP) {
        fk_die("fk_walk: node tag outside FK_OPCODE_ARM_CAP (0..255) -- the walker's tag space is the contract the op table is generated against; this is a corrupt node or a tag minted past the last arm");
    }
    fk_arms[t] = fk_arms[t] + 1;
    if (t == 1) {
        return fk_node[i][1] << 1;
    }
    if (t == 2) {
        return fk_vs[fp];
    }
    if (t == FK_TAG_CONST_HOLD) {
        /* the once-hold: first read walks the initializer and holds the
         * value in the node's own fields; later reads return it. The gen
         * stamp is read AFTER the walk — the build itself may melt. */
        if (fk_node[i][3] != 0 && (fk_node[i][3] >> 1) == fk_melt_gen) {
            return fk_node[i][2];
        }
        long long v190 = fk_walk(fk_node[i][1], fp);
        fk_node[i][2] = v190;
        fk_node[i][3] = (fk_melt_gen << 1) | 1;
        return v190;
    }
    if (t == 3) {
        long long a3 = fk_walk(fk_node[i][1], fp);
        long long b3 = fk_walk(fk_node[i][2], fp);
        if (fk_isf(a3) || fk_isf(b3)) {
            return fk_fbox(fk_num(a3) + fk_num(b3));
        }
        return a3 + b3;
    }
    if (t == 4) {
        long long a4 = fk_walk(fk_node[i][1], fp);
        long long b4 = fk_walk(fk_node[i][2], fp);
        if (fk_isf(a4) || fk_isf(b4)) {
            return fk_fbox(fk_num(a4) - fk_num(b4));
        }
        return a4 - b4;
    }
    if (t == 5) {
        { long long r5; if (fk_len_cmp(i, fp, 2, &r5)) { return r5; } }
        long long a5 = fk_walk(fk_node[i][1], fp);
        long long b5 = fk_walk(fk_node[i][2], fp);
        /* Same width-promotion rule as math (tags 3/4/42): float on either side
         * forces an IEEE comparison; pure int/int compares the tagged words
         * directly (<<1 tagging is order-preserving, so word order IS int order).
         * fk_num rounds through a double, whose 53-bit mantissa blurs distinct
         * 63-bit ints into equality — (eq (sub -2^62 0) (sub -2^62 1)) answered
         * true while sub of the same pair answered -1. Mirrors the Go/Rust
         * kernels' compare law and the JIT's exact int fast path (le/eq inline
         * cmp), which this walker previously DISAGREED with at the boundary.
         * Applies to the whole family: le here, eq/lt on tags 102/103, and the
         * JIT carrier fk_jprim2 — gt/ge/abs lower onto these via fk_rwtab. */
        if (fk_isf(a5) || fk_isf(b5)) {
            return (fk_num(a5) <= fk_num(b5)) ? 2 : 0;
        }
        return (a5 <= b5) ? 2 : 0;
    }
    if (t == 6) {
        if (fk_walk(fk_node[i][1], fp) == 0) {
            return fk_walk(fk_node[i][3], fp);
        }
        return fk_walk(fk_node[i][2], fp);
    }
    if (t == 7) {
        long long v7 = fk_walk(fk_node[i][1], fp);
        fk_vp(v7);
        long long b7 = fk_vsp - 1;
        fk_fn_heat[0] = fk_fn_heat[0] + 1;
        fk_heat_pulse();
        long long r7 = fk_walk_body(fk_fn[0], b7);
        fk_vsp = b7;
        return r7;
    }
    if (t == 8) {
        return fk_node[fk_walk(fk_node[i][1], fp) >> 1][fk_walk(fk_node[i][2], fp) >> 1] << 1;
    }
    if (t == 12) {
        long long c12 = fk_node[i][1];
        if (c12 < 0 || c12 >= fk_fn_count) {
            return fk_nothing;
        }
        long long v12 = fk_walk(fk_node[i][2], fp);
        fk_vp(v12);
        long long b12 = fk_vsp - 1;
        fk_fn_heat[c12] = fk_fn_heat[c12] + 1;
        if ((fk_fn_heat[c12] & (FK_F64_HEAT - 1)) == 0) { fk_twin_pulse(c12); }
        long long caller12 = fk_cur_fn; /* the non-tail call has a return point: boxes minted after it are the caller's again */
        fk_cur_fn = c12;
        fk_heat_pulse();
        long long r12 = fk_walk_body(fk_fn[c12], b12);
        fk_cur_fn = caller12;
        fk_vsp = b12;
        return fk_offer_ack(c12, 1, r12);
    }
    if (t == 240) {
        long long c240 = fk_node[i][1];
        if (c240 < 0 || c240 >= fk_fn_count) {
            return fk_nothing;
        }
        long long a0 = fk_walk(fk_node[i][2], fp);
        long long a1 = fk_walk(fk_node[i][3], fp);
        fk_vp(a0);
        fk_vp(a1);
        long long b240 = fk_vsp - 2;
        fk_fn_heat[c240] = fk_fn_heat[c240] + 1;
        long long caller240 = fk_cur_fn;
        fk_cur_fn = c240;
        fk_heat_pulse();
        long long r240 = fk_walk_body(fk_fn[c240], b240);
        fk_cur_fn = caller240;
        fk_vsp = b240;
        return fk_offer_ack(c240, 2, r240);
    }
    if (t == 241) {
        long long c241 = fk_node[i][1];
        if (c241 < 0 || c241 >= fk_fn_count) {
            return fk_nothing;
        }
        long long base241 = fk_vsp;
        long long cell241 = fk_node[i][2];
        while (cell241 >= 0 && fk_node[cell241][0] == 242) {
            fk_vp(fk_walk(fk_node[cell241][1], fp));
            cell241 = fk_node[cell241][2];
        }
        long long n241 = fk_vsp - base241;
        fk_fn_heat[c241] = fk_fn_heat[c241] + 1;
        if ((fk_fn_heat[c241] & (FK_F64_HEAT - 1)) == 0) { fk_twin_pulse(c241); }
        long long caller241 = fk_cur_fn;
        fk_cur_fn = c241;
        fk_heat_pulse();
        long long r241 = fk_walk_body(fk_fn[c241], base241);
        fk_cur_fn = caller241;
        fk_vsp = base241;
        return fk_offer_ack(c241, n241, r241);
    }
    if (t == 242) {
        return 0;
    }
    if (t == 243) {
        long long chain243 = fk_node[i][2];
        if (chain243 < 0) {
            return fk_fnval(fk_node[i][1]);
        }
        /* A capturing function's own value: read each captured expression NOW, against the
         * CURRENT frame (fp) -- correct because the parser only ever built this chain either
         * (a) at the exact point the defn statement itself runs, reading its OWN parent's live
         * frame, or (b) at a direct-call site already verified to be in that same frame -- and
         * freeze them into a new closure-instance row (fk_clo_make) so they survive after this
         * frame is gone (form-stdlib/model-service.fk's ms-predict-handler returning ms-handle,
         * to be called much later on some future request, is the standing example). */
        long long vals243[FK_CLOSURE_CAP_MAX];
        long long n243 = 0;
        while (chain243 >= 0 && fk_node[chain243][0] == 242 && n243 < FK_CLOSURE_CAP_MAX) {
            vals243[n243] = fk_walk(fk_node[chain243][1], fp);
            n243 = n243 + 1;
            chain243 = fk_node[chain243][2];
        }
        return fk_clo_make(fk_node[i][1], vals243, n243);
    }
    if (t == 244) {
        long long hv244 = fk_walk(fk_node[i][1], fp);
        if (fk_is_fnval(hv244) == 0) {
            return fk_nothing;
        }
        long long fi244 = fk_fnval_target(hv244);
        long long base244 = fk_vsp;
        long long cell244 = fk_node[i][2];
        while (cell244 >= 0 && fk_node[cell244][0] == 242) {
            fk_vp(fk_walk(fk_node[cell244][1], fp));
            cell244 = fk_node[cell244][2];
        }
        long long n244 = fk_vsp - base244;
        if (fk_observe_on()) {
            printf("offer-indirect fn%lld args=%lld (computed head)\n", fi244, n244);
        }
        /* Populate fk_call_cap_vals LAST, immediately before the jump -- see the fk_walk_body
         * copy of this arm for why an argument expression that itself calls another capturing
         * closure would otherwise clobber this shared scratch buffer first. */
        if (fk_fnval_is_closure(hv244)) {
            long long inst244 = fk_fnval_idx(hv244) - FK_CLOSURE_IDX_BASE;
            long long cb244 = fk_clo_capbase[inst244];
            long long cn244 = fk_clo_capcount[inst244];
            long long ci244 = 0;
            while (ci244 < cn244) {
                fk_call_cap_vals[ci244] = fk_clo_capvals[cb244 + ci244];
                ci244 = ci244 + 1;
            }
        }
        fk_fn_heat[fi244] = fk_fn_heat[fi244] + 1;
        long long caller244 = fk_cur_fn;
        fk_cur_fn = fi244;
        fk_heat_pulse();
        long long r244 = fk_walk_body(fk_fn[fi244], base244);
        fk_cur_fn = caller244;
        fk_vsp = base244;
        return fk_offer_ack(fi244, n244, r244);
    }
    if (t == 13) {
        long long mi = fk_walk(fk_node[i][1], fp) >> 1;
        long long mv = fk_walk(fk_node[i][2], fp);
        if (mi < 0) {
            return mv;
        }
        fk_mem_reserve(mi + 1);
        fk_mem[mi] = mv;
        return mv;
    }
    if (t == 14) {
        long long mi14 = fk_walk(fk_node[i][1], fp) >> 1;
        if (mi14 < 0 || mi14 >= fk_mem_cap) {
            return 0;
        }
        return fk_mem[mi14];
    }
    if (t == 18) {
        return 1;
    }
    if (t == 137) {
        return fk_nothing;
    }
    if (t == 138) {
        return fk_is_nothing(fk_walk(fk_node[i][1], fp)) ? 2 : 0;
    }
    if (t == 19) {
        long long h19 = fk_walk(fk_node[i][1], fp);
        fk_vp(h19);
        long long t19 = fk_walk(fk_node[i][2], fp);
        fk_vp(t19);
        if (fk_cap == 0) {
            fk_arena();
        }
        if (fk_hp * 100 >= fk_cap * 90) {
            fk_melt();
        }
        if (fk_hp + 1 >= fk_cap) {
            /* full even after the melt grew its arena: grow again rather
             * than hand back nil as if it were the cons -- a corrupt list
             * with a stderr note was still a corrupt list. */
            fk_heap_grow();
        }
        fk_hp = fk_hp + 1;
        fk_hh[fk_hp] = fk_vs[fk_vsp - 2];
        fk_ht[fk_hp] = fk_vs[fk_vsp - 1];
        fk_vsp = fk_vsp - 2;
        return (fk_hp << 1) | 1;
    }
    if (t == 20) {
        long long p = fk_walk(fk_node[i][1], fp) >> 1;
        if (p < 1 || !FK_POK(p)) {
            return 1;
        }
        return FK_HH(p);
    }
    if (t == 21) {
        long long p = fk_walk(fk_node[i][1], fp) >> 1;
        if (p < 1 || !FK_POK(p)) {
            return 1;
        }
        return FK_HT(p);
    }
    if (t == 22) {
        /* len — a LIST cell's length; a STRING's byte length, as go, rust and
         * ts answer and as the primitive registry declares ("string bytes").
         * Until 2026-07-31 a string was `poolidx << 1`, the same even word as
         * an int, so the low-bit guard below was all this door could read and
         * (len "abc") answered 0 — a plausible zero over the wrong kind that
         * left tb-any2? silently false over string rows (R70). Strings have
         * carried their own odd-negative band since (fk_sbase); the door reads
         * it now. Other non-lists answer 0 as the siblings do. The emitted
         * walker's fk_list_len (fkc-table-serialize.fk) still answers 0 for a
         * string: its words carry no string band to read. */
        long long lv22 = fk_walk(fk_node[i][1], fp);
        if (fk_is_str(lv22)) {
            return FK_SLEN(fk_stri(lv22)) << 1;
        }
        if ((lv22 & 1) == 0) {
            return 0;
        }
        long long p = lv22 >> 1;
        long long n = 0;
        while (p >= 1 && FK_POK(p)) {
            n = n + 1;
            p = FK_HT(p) >> 1;
        }
        return n << 1;
    }
    if (t == 23) {
        long long x23 = fk_walk(fk_node[i][1], fp);
        fk_vp(x23);
        long long k23 = fk_walk(fk_node[i][2], fp) >> 1;
        fk_vsp = fk_vsp - 1;
        long long p = fk_vs[fk_vsp] >> 1;
        while (p >= 1 && FK_POK(p) && k23 > 0) {
            p = FK_HT(p) >> 1;
            k23 = k23 - 1;
        }
        if (p < 1 || !FK_POK(p)) {
            return 1;
        }
        return FK_HH(p);
    }
    if (t == 44) {
        long long fv44 = fk_walk(fk_node[i][1], fp);
        fk_vp(fv44);
        long long av44 = fk_walk(fk_node[i][2], fp);
        fk_vp(av44);
        long long p44 = fk_vs[fk_vsp - 2] >> 1;
        if (p44 < 1 || !FK_POK(p44)) {
            fk_vsp = fk_vsp - 2;
            return 0;
        }
        long long f44 = FK_HH(p44) >> 1;
        long long p44t = FK_HT(p44) >> 1;
        if (p44t < 1 || !FK_POK(p44t)) {
            fk_vsp = fk_vsp - 2;
            return 0;
        }
        long long a44 = FK_HH(p44t) >> 1;
        long long caps44 = FK_HT(p44t);
        long long args44 = fk_vs[fk_vsp - 1];
        long long rev44 = 1;
        long long cc44 = caps44 >> 1;
        while (cc44 >= 1 && FK_POK(cc44)) {
            /* same shape as the walker's capture copy above: growth, never
             * an unguarded bump past the arrays. */
            if (fk_hp + 1 >= fk_cap) {
                fk_heap_grow();
            }
            fk_hp = fk_hp + 1;
            fk_hh[fk_hp] = FK_HH(cc44);
            fk_ht[fk_hp] = rev44;
            rev44 = (fk_hp << 1) | 1;
            cc44 = FK_HT(cc44) >> 1;
        }
        long long comb44 = args44;
        long long rr44 = rev44 >> 1;
        while (rr44 >= 1 && FK_POK(rr44)) {
            if (fk_hp + 1 >= fk_cap) {
                fk_heap_grow();
            }
            fk_hp = fk_hp + 1;
            fk_hh[fk_hp] = FK_HH(rr44);
            fk_ht[fk_hp] = comb44;
            comb44 = (fk_hp << 1) | 1;
            rr44 = FK_HT(rr44) >> 1;
        }
        long long carg44 = 1;
        if (a44 == 0) {
            carg44 = 1;
        } else {
            if (a44 == 1) {
                long long pa44 = comb44 >> 1;
                if (pa44 < 1 || !FK_POK(pa44)) {
                    fk_vsp = fk_vsp - 2;
                    return 1;
                }
                carg44 = FK_HH(pa44);
            } else {
                carg44 = comb44;
            }
        }
        if (f44 < 0 || f44 >= fk_fn_count) {
            fk_vsp = fk_vsp - 2;
            return 0;
        }
        fk_vsp = fk_vsp - 2;
        fk_vp(carg44);
        long long b44 = fk_vsp - 1;
        fk_fn_heat[f44] = fk_fn_heat[f44] + 1;
            fk_cur_fn = f44;
        fk_heat_pulse();
        long long r44 = fk_walk_body(fk_fn[f44], b44);
        fk_vsp = b44;
        return fk_offer_ack(f44, 1, r44);
    }
    if (t == 42) {
        long long a42 = fk_walk(fk_node[i][1], fp);
        long long b42 = fk_walk(fk_node[i][2], fp);
        if (fk_isf(a42) || fk_isf(b42)) {
            return fk_fbox(fk_num(a42) * fk_num(b42));
        }
        return ((a42 >> 1) * (b42 >> 1)) << 1;
    }
    if (t == 45) {
        return fk_walk(fk_node[i][1], fp);
    }
    if (t == 48) {
        long long nv48 = fk_walk(fk_node[i][1], fp);
        if (nv48 >= 0) {
            return 1;
        }
        long long ni48 = fk_nidx(nv48);
        if (ni48 < 1 || ni48 > fk_np) {
            return 1;
        }
        if (fk_nkind[ni48] == 2) {
            return fk_nkids[ni48];
        }
        return 1;
    }
    if (t == 49) {
        long long nv49 = fk_walk(fk_node[i][1], fp);
        if (nv49 >= 0) {
            return 0;
        }
        long long ni49 = fk_nidx(nv49);
        if (ni49 < 1 || ni49 > fk_np) {
            return 0;
        }
        if (fk_nkind[ni49] == 1) {
            /* a trivial bool (node_type 3) stores an interning sentinel in
               fk_nval so true/false are distinct interned nodes; node_value
               must return the BOOLEAN, not the sentinel. nid[3] holds 1/0;
               return it tagged (v<<1) to equal the true/false literals
               (which lower to fk_smklit(1)/fk_smklit(0) -> 2/0). */
            if (fk_nid[ni49][2] == 3) {
                return fk_nid[ni49][3] << 1;
            }
            return fk_nval[ni49];
        }
        return 0;
    }
    if (t == 80) {
        /* value_eq / node_eq — THE COMPARISON THAT LIED ONCE IN SEVEN HUNDRED.
         * This arm used to read
         *     fk_veq(fk_walk(...[1], fp), fk_walk(...[2], fp))
         * and that single line carried the whole defect. A walked value is an
         * INDEX — into the cons arena, or into the string pool — and neither
         * index survives the other operand's walk on its own:
         *   • fk_melt relocates every live pair into a fresh arena and rewrites
         *     its ROOTS in place (the value stack, the memory cells, records,
         *     value nodes). A first operand living only in a C local is not a
         *     root, so its index afterwards names whatever now sits at that
         *     arena position — measured: value_eq of two freshly built lists
         *     answered false 11 times in 40 under melt pressure;
         *   • fk_smelt then reclaims every unmarked LOCAL string slot onto
         *     fk_sfree, and fk_sintern hands that same index to the next
         *     string with different bytes. Same wound, rarer: the fresh-string
         *     case is the 1-in-700 that corpus row 1357 (`seldomred`) named.
         * The cure is the one the cons arm (t == 19) has always used and this
         * arm never did: push the first operand so the melt can SEE it, and
         * read it BACK off the stack afterwards, because a melt updates the
         * stack slot and cannot reach a register. Rooting without reading back
         * would still compare a stale index — half a cure is none.
         * The neighbours str_eq/str_concat/str_byte_at (t == 26/27/28) root
         * but never read back, and stay correct only because they reduce their
         * operand to a string INDEX before the second walk and string bytes
         * never move. This arm cannot: it does not know its operands' kind. */
        long long a80 = fk_walk(fk_node[i][1], fp);
        fk_vp(a80);
        long long b80 = fk_walk(fk_node[i][2], fp);
        fk_vp(b80);
        a80 = fk_vs[fk_vsp - 2];
        b80 = fk_vs[fk_vsp - 1];
        fk_vsp = fk_vsp - 2;
        if (fk_veq(a80, b80) != 0) {
            return 2;
        }
        return 0;
    }
    if (t == 92) {
        long long nv92 = fk_walk(fk_node[i][1], fp);
        if (nv92 >= 0) {
            return nv92;
        }
        long long ni92 = fk_nidx(nv92);
        if (ni92 < 1 || ni92 > fk_np) {
            return nv92;
        }
        if (fk_nkind[ni92] == 2) {
            return fk_ncat[ni92];
        }
        return nv92;
    }
    if (t == 93) {
        long long nv93 = fk_walk(fk_node[i][1], fp);
        if (nv93 >= 0) {
            return 0;
        }
        long long ni93 = fk_nidx(nv93);
        if (ni93 < 1 || ni93 > fk_np) {
            return 0;
        }
        return fk_nid[ni93][2] << 1;
    }
    if (t == 94) {
        long long nv94 = fk_walk(fk_node[i][1], fp);
        if (nv94 >= 0) {
            return 0;
        }
        long long ni94 = fk_nidx(nv94);
        if (ni94 < 1 || ni94 > fk_np) {
            return 0;
        }
        return fk_nid[ni94][3] << 1;
    }
    if (t == 95) {
        long long nv95 = fk_walk(fk_node[i][1], fp);
        if (nv95 >= 0) {
            return 0;
        }
        long long ni95 = fk_nidx(nv95);
        if (ni95 < 1 || ni95 > fk_np) {
            return 0;
        }
        return fk_nid[ni95][0] << 1;
    }
    if (t == 96) {
        long long nv96 = fk_walk(fk_node[i][1], fp);
        if (nv96 >= 0) {
            return 0;
        }
        long long ni96 = fk_nidx(nv96);
        if (ni96 < 1 || ni96 > fk_np) {
            return 0;
        }
        return fk_nid[ni96][1] << 1;
    }
    if (t == 70) {
        long long a70 = fk_walk(fk_node[i][1], fp);
        long long b70 = fk_walk(fk_node[i][2], fp);
        if (a70 != 0 && b70 != 0) {
            return 2;
        }
        return 0;
    }
    if (t == 71) {
        long long a71 = fk_walk(fk_node[i][1], fp);
        long long b71 = fk_walk(fk_node[i][2], fp);
        if (a71 != 0 || b71 != 0) {
            return 2;
        }
        return 0;
    }
    if (t == 72) {
        if (fk_walk(fk_node[i][1], fp) == 0) {
            return 2;
        }
        return 0;
    }
    if (t == 73) {
        return fk_node[i][1] << 1;
    }
    if (t == 74) {
        return fk_walk(fk_node[i][1], fp);
    }
    if (t == 75) {
        return fk_walk(fk_node[i][1], fp);
    }
    if (t == 76) {
        return fk_walk(fk_node[i][1], fp);
    }
    if (t == 77) {
        return fk_walk(fk_node[i][2], fp);
    }
    if (t == 78) {
        return fk_walk(fk_node[i][1], fp);
    }
    if (t == 79) {
        if (fk_walk(fk_node[i][1], fp) == 0) {
            return fk_walk(fk_node[i][3], fp);
        }
        return fk_walk(fk_node[i][2], fp);
    }
    if (t == 69) {
        fk_walk(fk_node[i][1], fp);
        return fk_walk(fk_node[i][2], fp);
    }
    if (t == 102) {
        { long long r102; if (fk_len_cmp(i, fp, 0, &r102)) { return r102; } }
        /* int/int exact, float promotes — the tag-5 compare law. */
        /* The first operand waits here while the second is walked, and a walk
         * can melt. Only a cons index or a local string slot loses its meaning
         * that way (fk_movable), and `eq` compares integers in every loop this
         * body runs, so the rooting is paid only by the words that need it.
         * Unlike t == 80 this shape was NOT witnessed answering wrong — the
         * probes that made value_eq lie 5 times in 16 could not make `eq` lie
         * once in 60. It is healed by reading, because the arm holds exactly
         * the word the melt was proven to take, and a hazard you cannot make
         * fire is still the hazard whose mechanism you already watched work. */
        long long ae = fk_walk(fk_node[i][1], fp);
        long long be;
        if (fk_movable(ae)) {
            fk_vp(ae);
            be = fk_walk(fk_node[i][2], fp);
            ae = fk_vs[fk_vsp - 1];
            fk_vsp = fk_vsp - 1;
        } else {
            be = fk_walk(fk_node[i][2], fp);
        }
        if (fk_isf(ae) || fk_isf(be)) {
            return (fk_num(ae) == fk_num(be)) ? 2 : 0;
        }
        if (ae < 0 && be < 0) {
            long long ia102 = fk_nidx(ae);
            long long ib102 = fk_nidx(be);
            if (ia102 >= 1 && ia102 <= fk_np && ib102 >= 1 && ib102 <= fk_np &&
                fk_nkind[ia102] == 3 && fk_nkind[ib102] == 3) {
                /* A NodeID is identity-by-content: equal coordinates ARE the
                 * same identity regardless of which mint built the value node.
                 * Before this case, two (make_nodeid p l t i) of the same
                 * coordinates compared by handle and answered 0 (witnessed
                 * 2026-08-30); fk_neq already knew the kind-3 law — eq just
                 * never asked it. Only the nodeid/nodeid pair routes here. */
                return (fk_nid[ia102][0] == fk_nid[ib102][0] &&
                        fk_nid[ia102][1] == fk_nid[ib102][1] &&
                        fk_nid[ia102][2] == fk_nid[ib102][2] &&
                        fk_nid[ia102][3] == fk_nid[ib102][3]) ? 2 : 0;
            }
        }
        return (ae == be) ? 2 : 0;
    }
    if (t == 103) {
        { long long r103; if (fk_len_cmp(i, fp, 1, &r103)) { return r103; } }
        /* int/int exact, float promotes — the tag-5 compare law.
         * Same rooting as t == 102, same reason, same cost on the int path. */
        long long al = fk_walk(fk_node[i][1], fp);
        long long bl;
        if (fk_movable(al)) {
            fk_vp(al);
            bl = fk_walk(fk_node[i][2], fp);
            al = fk_vs[fk_vsp - 1];
            fk_vsp = fk_vsp - 1;
        } else {
            bl = fk_walk(fk_node[i][2], fp);
        }
        if (fk_isf(al) || fk_isf(bl)) {
            return (fk_num(al) < fk_num(bl)) ? 2 : 0;
        }
        return (al < bl) ? 2 : 0;
    }
    if (t == 109) {
        long long slot109 = fk_walk(fk_node[i][1], fp) >> 1;
        /* the RHS walk can GROW fk_vs (realloc moves it): local first. */
        long long v109 = fk_walk(fk_node[i][2], fp);
        if (fp + slot109 >= fk_vs_cap) {
            fk_vs_grow(fp + slot109 + 1);
        }
        fk_vs[fp + slot109] = v109;
        /* ROOT the let-local: raise fk_vsp over the slot so the body's temporaries
         * (pushed at fk_vsp) cannot overwrite it, and a compacting melt relocates it.
         * Without this a do-let chain outside a tag-111 frame reservation (e.g. a
         * top-level (do (let a ..) (let b ..) ..)) silently clobbers a while
         * evaluating b -- string-bearing list values push enough temps to reach the
         * slot. The enclosing frame/call boundary restores fk_vsp. */
        if (fp + slot109 + 1 > fk_vsp) {
            fk_vsp = fp + slot109 + 1;
        }
        return fk_walk(fk_node[i][3], fp);
    }
    if (t == 110) {
        return fk_vs[fp + (fk_walk(fk_node[i][1], fp) >> 1)];
    }
    if (t == 111) {
        long long k111 = fk_walk(fk_node[i][1], fp) >> 1;
        long long sv111 = fk_vsp;
        long long need111 = fp + 1 + k111;
        while (fk_vsp < need111) {
            fk_vs[fk_vsp] = 0;
            fk_vsp = fk_vsp + 1;
        }
        long long r111 = fk_walk(fk_node[i][2], fp);
        fk_vsp = sv111;
        return r111;
    }
    if (t == 149) {
        /* A capturing function's own prologue read (see fk_wrap_cap_prologue): the value the
         * call mechanism just placed in fk_call_cap_vals[j], for its own tag-109 wrapper to bind
         * into this frame's slot -- a bare field read, node[1] is a raw C int (which of the
         * FK_CLOSURE_CAP_MAX scratch slots), never a walkable sub-node or a .fkb-remappable one.
         * Tag 149, and only after checking runtime/fkwu-optable.h: 245 names the real
         * "metal_pipeline" op and 148 names "metal_deadline" (arity 1) -- each was once taken
         * for this arm as "free", and because this arm is tested before fk_walk_cold, every
         * metal_pipeline / metal_deadline call was silently answered from a scratch slot instead
         * (metal-deadline-band 33 of 127 with the caller's patience never reaching the carrier).
         * 149 has no optable row and no other tag==/smknode( site names it. */
        return fk_call_cap_vals[fk_node[i][1]];
    }
    return fk_walk_cold(t, i, fp);
}
static int fk_gift_live(long long gh) {
    return gh >= 0 && gh < fk_gift_count && fk_gift_base[gh] != 0;
}
/* ---- the field store: /fg-field-<letter>, host-wide, never unlinked by a kernel ----
 * header h (words after the 16-byte gift header): 0 magic 1 version 2 nodes 3 pairs 4 strings 5 string bytes 6 floats
 * columns: k kind c cat i kids v val n nid f sfile l sline o scol a sattr m hash-memo x intern hash;
 * P Q shared pairs; s O L X shared node-strings (bytes, offsets, lengths, hash); F Y shared floats (values, hash).
 * A kernel that cannot open the field keeps private tables and says so on its live page (word 22). */
#define FK_FIELD_MAGIC 0x4649454C44LL
#define FK_FIELD_VERSION 1
#define FK_FIELD_NODES (1LL << 26)
#define FK_FIELD_PAIRS (1LL << 27)
#define FK_FIELD_STRB (1LL << 31)
#define FK_FIELD_STRS (1LL << 25)
#define FK_FIELD_FLOATS (1LL << 25)
#define FK_FIELD_HASH (1LL << 27)
#define FK_FIELD_SHASH (1LL << 25)
#define FK_FIELD_FHASH (1LL << 25)
static const char fk_field_letters[] = "hkcivnfloamxPQsOLXFY";
static long long *fk_field_itab;
static long long *fk_fstab;
static long long *fk_fftab;
static void fk_field_name(char letter, char *out) { const char *p = "/fg-field-"; long long k = 0; while (p[k]) { out[k] = p[k]; k = k + 1; } out[k] = letter; out[k + 1] = 0; }
static void *fk_field_take(char letter, long long bytes) {
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    char nm[32];
    fk_field_name(letter, nm);
    int fd = shm_open(nm, O_CREAT | O_RDWR, 0600);
    if (fd < 0) { return 0; }
    struct stat st;
    if (fstat(fd, &st) != 0) { close(fd); return 0; }
    if (st.st_size == 0) { if (ftruncate(fd, bytes) != 0) { close(fd); return 0; } }
    else if ((long long)st.st_size < bytes) { close(fd); return 0; }   /* the host rounds a reservation up to its page */
    void *p = mmap(0, (size_t)bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    return p == MAP_FAILED ? 0 : p;
#else
    (void)letter; (void)bytes;
    return 0;
#endif
}
static long long fk_field_pp(void) { return fk_field_on ? fk_field_hdr[2 + 3] : 0; }
static long long fk_field_sp(void) { return fk_field_on ? fk_field_hdr[2 + 4] : 0; }
static long long fk_field_fp(void) { return fk_field_on ? fk_field_hdr[2 + 6] : 0; }
static long long fk_field_claim(long long word, long long n) { return __atomic_fetch_add(&fk_field_hdr[2 + word], n, __ATOMIC_ACQ_REL); }
/* opens every column; answers 1 when this kernel's node tables now live in the field */
static int fk_field_open(void) {
    if (fk_field_on) { return 1; }
    fk_field_tried = 1;
    if (fk_conf("FK_FIELD_OFF")) { return 0; }
    void *hdr = fk_field_take('h', 4096);
    if (fk_conf("FK_FIELD_TRACE")) { dprintf(2, "[field] hdr=%p\n", hdr); }
    if (hdr == 0) { return 0; }
    volatile long long *w = (volatile long long *)hdr + 2;
    long long zero = 0;
    if (__atomic_compare_exchange_n(&w[0], &zero, FK_FIELD_MAGIC, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) { __atomic_store_n(&w[1], FK_FIELD_VERSION, __ATOMIC_RELEASE); }
    else if (w[0] != FK_FIELD_MAGIC || w[1] != FK_FIELD_VERSION) { return 0; }
    void *k = fk_field_take('k', FK_FIELD_NODES * 8), *c = fk_field_take('c', FK_FIELD_NODES * 8), *i = fk_field_take('i', FK_FIELD_NODES * 8), *v = fk_field_take('v', FK_FIELD_NODES * 8);
    void *n = fk_field_take('n', FK_FIELD_NODES * 32), *f = fk_field_take('f', FK_FIELD_NODES * 8), *l = fk_field_take('l', FK_FIELD_NODES * 8), *o = fk_field_take('o', FK_FIELD_NODES * 8), *a = fk_field_take('a', FK_FIELD_NODES * 8), *m = fk_field_take('m', FK_FIELD_NODES * 8);
    void *x = fk_field_take('x', FK_FIELD_HASH * 8), *P = fk_field_take('P', FK_FIELD_PAIRS * 8), *Q = fk_field_take('Q', FK_FIELD_PAIRS * 8);
    void *s = fk_field_take('s', FK_FIELD_STRB), *O = fk_field_take('O', FK_FIELD_STRS * 8), *L = fk_field_take('L', FK_FIELD_STRS * 8), *X = fk_field_take('X', FK_FIELD_SHASH * 8);
    void *F = fk_field_take('F', FK_FIELD_FLOATS * 8), *Y = fk_field_take('Y', FK_FIELD_FHASH * 8);
    if (fk_conf("FK_FIELD_TRACE")) { dprintf(2, "[field] k=%d c=%d i=%d v=%d n=%d f=%d l=%d o=%d a=%d m=%d x=%d P=%d Q=%d s=%d O=%d L=%d X=%d F=%d Y=%d\n", !!k, !!c, !!i, !!v, !!n, !!f, !!l, !!o, !!a, !!m, !!x, !!P, !!Q, !!s, !!O, !!L, !!X, !!F, !!Y); }
    if (!k || !c || !i || !v || !n || !f || !l || !o || !a || !m || !x || !P || !Q || !s || !O || !L || !X || !F || !Y) { return 0; }
    fk_field_hdr = (volatile long long *)hdr;
    fk_nkind = k; fk_ncat = c; fk_nkids = i; fk_nval = v; fk_nid = n; fk_nsfile = f; fk_nsline = l; fk_nscol = o; fk_nsattr = a; fk_nhash_memo = m;
    fk_field_itab = x; fk_fph = P; fk_fpt = Q; fk_fsb = s; fk_fso = O; fk_fsl = L; fk_fstab = X; fk_ffv = F; fk_fftab = Y;
    fk_np_p = &w[2];
    fk_field_on = 1;
    fk_live_publish(0); /* the page's store word changes here; the page has no tick, so the change is written where it happens */
    return 1;
}
static void fk_field_unlink_all(void) {
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    long long k = 0;
    while (fk_field_letters[k]) { char nm[32]; fk_field_name(fk_field_letters[k], nm); shm_unlink(nm); k = k + 1; }
#endif
}
static void fk_field_yield(void) {
#if !defined(_WIN32)
    struct timespec ts; ts.tv_sec = 0; ts.tv_nsec = 20000; nanosleep(&ts, 0);
#endif
}
/* claim a slot in an open-addressed shared table: answers the slot index this kernel owns (value -1 while filling),
 * or a slot holding an existing index the caller must test; -2 when the table is full */
static unsigned long long fk_field_bytes_hash(const char *b, long long n) { unsigned long long h = 1469598103934665603ULL; long long k = 0; while (k < n) { h = (h ^ (unsigned char)b[k]) * 1099511628211ULL; k = k + 1; } return h; }
static long long fk_str_bytes_eq(long long a, long long b) {
    long long sa = fk_stri(a), sb = fk_stri(b);
    if (sa < 0 || sb < 0 || FK_SLEN(sa) != FK_SLEN(sb)) { return 0; }
    const char *pa = FK_SBYTES(sa); const char *pb = FK_SBYTES(sb);
    long long n = FK_SLEN(sa), j = 0;
    while (j < n) { if (pa[j] != pb[j]) { return 0; } j = j + 1; }
    return 1;
}
/* a private string becomes a shared node-string: same bytes, one index for every kernel */
static long long fk_field_share_string(long long sv) {
    long long si = fk_stri(sv);
    if (si < 0 || si >= FK_STR_BASE) { return sv; }
    long long len = FK_SLEN(si);
    const char *bytes = FK_SBYTES(si);
    long long mask = FK_FIELD_SHASH - 1;
    long long slot = (long long)(fk_field_bytes_hash(bytes, len) & (unsigned long long)mask);
    long long probes = 0;
    while (probes < FK_FIELD_SHASH) {
        long long cur = __atomic_load_n(&fk_fstab[slot], __ATOMIC_ACQUIRE);
        if (cur > 0) {
            long long j = cur - 1;
            if (fk_fsl[j] == len) { long long q = 0; const char *pj = fk_fsb + fk_fso[j]; while (q < len && pj[q] == bytes[q]) { q = q + 1; } if (q == len) { return fk_strv(FK_STR_BASE + j); } }
            slot = (slot + 1) & mask; probes = probes + 1; continue;
        }
        if (cur == 0) {
            long long z = 0;
            if (__atomic_compare_exchange_n(&fk_fstab[slot], &z, -1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
                long long off = fk_field_claim(5, len);
                long long j = fk_field_claim(4, 1);
                if (off + len > FK_FIELD_STRB || j >= FK_FIELD_STRS) { __atomic_store_n(&fk_fstab[slot], 0, __ATOMIC_RELEASE); return sv; }
                long long q = 0; while (q < len) { fk_fsb[off + q] = bytes[q]; q = q + 1; }
                fk_fso[j] = off; fk_fsl[j] = len;
                __atomic_store_n(&fk_fstab[slot], j + 1, __ATOMIC_RELEASE);
                return fk_strv(FK_STR_BASE + j);
            }
            continue;
        }
        fk_field_yield();
    }
    return sv;
}
/* a private float box becomes a shared float: same bits, one index for every kernel */
static long long fk_field_share_float(long long fv) {
    long long fi = fk_fidx(fv);
    if (fi >= FK_FLT_BASE) { return fv; }
    double d = FK_FV(fi);
    unsigned long long bits; { char *pd = (char *)&d; char *pb = (char *)&bits; int b = 0; while (b < 8) { pb[b] = pd[b]; b = b + 1; } }
    long long mask = FK_FIELD_FHASH - 1;
    long long slot = (long long)(fk_mix64(11, bits) & (unsigned long long)mask);
    long long probes = 0;
    while (probes < FK_FIELD_FHASH) {
        long long cur = __atomic_load_n(&fk_fftab[slot], __ATOMIC_ACQUIRE);
        if (cur > 0) {
            long long j = cur - 1; unsigned long long ob; { double od = fk_ffv[j]; char *pd = (char *)&od; char *pb = (char *)&ob; int b = 0; while (b < 8) { pb[b] = pd[b]; b = b + 1; } }
            if (ob == bits) { return fk_fbase - ((FK_FLT_BASE + j) << 1) - 1; }
            slot = (slot + 1) & mask; probes = probes + 1; continue;
        }
        if (cur == 0) {
            long long z = 0;
            if (__atomic_compare_exchange_n(&fk_fftab[slot], &z, -1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
                long long j = fk_field_claim(6, 1);
                if (j >= FK_FIELD_FLOATS) { __atomic_store_n(&fk_fftab[slot], 0, __ATOMIC_RELEASE); return fv; }
                fk_ffv[j] = d; 
                __atomic_store_n(&fk_fftab[slot], j + 1, __ATOMIC_RELEASE);
                return fk_fbase - ((FK_FLT_BASE + j) << 1) - 1;
            }
            continue;
        }
        fk_field_yield();
    }
    return fv;
}
/* every word a shared cell carries must itself be shared: lists copied into shared pairs, strings and floats interned */
static long long fk_field_share_value(long long v) {
    if (v == fk_nothing) { return v; }
    if ((v & 1) == 0) { return v; }
    if (fk_is_str(v)) { return fk_field_share_string(v); }
    if (fk_isf(v)) { return fk_field_share_float(v); }
    if (v < 0) { return v; }
    long long p = v >> 1;
    if (p < 1 || p >= FK_PAIR_BASE) { return v; }
    long long n = 0, q = p;
    while (q >= 1 && q <= fk_hp) { n = n + 1; q = FK_HT(q) >> 1; }
    if (n == 0) { return v; }
    long long base = fk_field_claim(3, n);
    if (base + n > FK_FIELD_PAIRS) { return v; }
    long long k = 0;
    q = p;
    while (k < n) {
        long long at = base + k;
        fk_fph[at] = fk_field_share_value(FK_HH(q));
        long long tail = FK_HT(q);
        fk_fpt[at] = (k + 1 < n) ? (((FK_PAIR_BASE + at + 1) << 1) | 1) : ((tail & 1) && (tail >> 1) >= 1 ? 1 : tail);
        q = tail >> 1;
        k = k + 1;
    }
    return ((FK_PAIR_BASE + base) << 1) | 1;
}
static void fk_nodes_grow(void);
static long long fk_field_node_matches(long long ix, long long kind, long long sub, long long a, long long b, long long *nid4) {
    if (fk_nkind[ix] != kind) { return 0; }
    if (kind == 1) {
        if (fk_nid[ix][2] != sub) { return 0; }
        if (sub == 2) { return fk_str_bytes_eq(fk_nval[ix], a); }
        if (sub == 6 || sub == 7) { double x = fk_num(fk_nval[ix]), y = fk_num(a); return (x == y || (x != x && y != y)) ? 1 : 0; }
        return fk_nval[ix] == a;
    }
    if (kind == 2) { return fk_veq(fk_ncat[ix], a) != 0 && fk_veq(fk_nkids[ix], b) != 0; }
    if (kind == 3) { return fk_nid[ix][0] == nid4[0] && fk_nid[ix][1] == nid4[1] && fk_nid[ix][2] == nid4[2] && fk_nid[ix][3] == nid4[3]; }
    return 0;
}
static long long fk_field_fill(long long kind, long long sub, long long a, long long b, long long *nid4, long long h) {
    long long idx = fk_field_claim(2, 1) + 1;
    fk_mint_total = fk_mint_total + 1; if (fk_fn_mint != 0 && fk_cur_fn > 0 && fk_cur_fn < fk_fn_capacity) { fk_fn_mint[fk_cur_fn] = fk_fn_mint[fk_cur_fn] + 1; } /* a fresh field cell: this kernel grew the permanent arena by one */
    if (idx >= FK_FIELD_NODES) { fk_die("fkwu: the field's node columns are full (2^26 cells): run observe/field-reset-run.fk with no kernel alive"); }
    while (idx >= fk_node_cap) { fk_nodes_grow(); }
    fk_nkind[idx] = kind;
    fk_nhash_memo[idx] = h;
    if (kind == 1) {
        fk_nval[idx] = fk_field_share_value(a); fk_nkids[idx] = 1; fk_ncat[idx] = 0;
        fk_nid[idx][0] = 1; fk_nid[idx][1] = 1; fk_nid[idx][2] = sub;
        /* a bool's inst is its truth (1/0), read off the interning sentinel -- the sentinel itself is
         * never zero, so `a != 0` named every bool true (measured: node_inst of false answered 1);
         * a float32's inst is its IEEE bits (b), so a hand-built type-6 leaf and an interned one agree */
        fk_nid[idx][3] = sub == 1 ? (a >> 1) : (sub == 3 ? ((a == (0 - 9223372036854775807LL)) ? 1 : 0) : (sub == 6 ? b : idx));
    } else if (kind == 2) {
        fk_ncat[idx] = fk_field_share_value(a); fk_nkids[idx] = fk_field_share_value(b); fk_nval[idx] = 0;
        fk_nid[idx][0] = 0; fk_nid[idx][1] = 0; fk_nid[idx][2] = 0; fk_nid[idx][3] = idx;
        if (a < 0) { long long ci = fk_nidx(a); if (ci >= 1 && ci < idx) { fk_nid[idx][1] = fk_nid[ci][1]; fk_nid[idx][2] = fk_nid[ci][2]; } }
    } else {
        fk_ncat[idx] = 0; fk_nkids[idx] = 1; fk_nval[idx] = 0;
        fk_nid[idx][0] = nid4[0]; fk_nid[idx][1] = nid4[1]; fk_nid[idx][2] = nid4[2]; fk_nid[idx][3] = nid4[3];
    }
    return idx;
}
/* one intern door for every kind: find the cell in the shared hash or claim a slot, fill, publish */
static long long fk_field_intern_node(long long kind, long long sub, long long a, long long b, long long *nid4, long long h) {
    if (kind == 3) { return fk_nbox(fk_field_fill(3, 0, 0, 0, nid4, 0)); }
    long long mask = FK_FIELD_HASH - 1;
    long long slot = h & mask;
    long long probes = 0;
    while (probes < FK_FIELD_HASH) {
        long long cur = __atomic_load_n(&fk_field_itab[slot], __ATOMIC_ACQUIRE);
        if (cur > 0) {
            if (fk_field_node_matches(cur, kind, sub, a, b, nid4)) { return fk_nbox(cur); }
            slot = (slot + 1) & mask; probes = probes + 1; continue;
        }
        if (cur == 0) {
            long long z = 0;
            if (__atomic_compare_exchange_n(&fk_field_itab[slot], &z, -1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
                long long idx = fk_field_fill(kind, sub, a, b, nid4, h);
                __atomic_store_n(&fk_field_itab[slot], idx, __ATOMIC_RELEASE);
                return fk_nbox(idx);
            }
            continue;
        }
        fk_field_yield();
    }
    fk_die("fkwu: the field's intern index is full");
    return fk_nothing;
}
/* ---- the store: every value table of this kernel lives in shared memory ----
 * One sparse reservation per column, /fg-c<pid>-<letter>, sized once (macOS lets a shm object be
 * truncated once) and committed page by page as the table grows: a 4 GiB reservation touched at three
 * pages costs three pages. A shared table never moves, so another process maps the same columns and
 * reads any cell, cons, string or float by its word -- blueprint word (fk_ncat), kids, value, NodeID
 * and source pointer (fk_nsfile/fk_nsline/fk_nscol) on one surface, no copy, no wire. Past a
 * reservation the process copies its tables to private memory once and goes on (never a wall); the
 * live page says which. Letters: k kind, c cat, i kids, v val, n nid, f sfile, l sline, o scol,
 * a sattr, h/t heap generation 0, H/T heap generation 1, s string bytes, O string offsets,
 * L string lengths, F floats. */
static int fk_store_tried;
static const char fk_store_letters[] = "kcivnfloaPQsOLFhtHT";
static void fk_store_name(char letter, long long pid, char *out) {
    char digits[24];
    long long n = 0, p = pid < 0 ? 0 : pid, o = 0;
    if (p == 0) { digits[n] = '0'; n = n + 1; }
    while (p > 0) { digits[n] = (char)('0' + (p % 10)); n = n + 1; p = p / 10; }
    out[0] = '/'; out[1] = 'f'; out[2] = 'g'; out[3] = '-'; out[4] = 'c'; o = 5;
    while (n > 0) { n = n - 1; out[o] = digits[n]; o = o + 1; }
    out[o] = '-'; out[o + 1] = letter; out[o + 2] = 0;
}
static void fk_store_unlink_pid(long long pid) {
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    long long k = 0;
    while (fk_store_letters[k]) { char nm[32]; fk_store_name(fk_store_letters[k], pid, nm); shm_unlink(nm); k = k + 1; }
#else
    (void)pid;
#endif
}
static void *fk_store_take(char letter, long long bytes) {
    if (!fk_field_tried) { fk_field_tried = 1; fk_field_open(); }
    if (fk_field_on) { return 0; }
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    if (!fk_store_tried) { fk_store_tried = 1; fk_store_shared = 1; }
    if (!fk_store_shared) { return 0; }
    char nm[32];
    fk_store_name(letter, (long long)getpid(), nm);
    shm_unlink(nm);
    int fd = shm_open(nm, O_CREAT | O_RDWR, 0600);
    if (fd < 0) { fk_store_shared = 0; return 0; }
    if (ftruncate(fd, bytes) != 0) { close(fd); shm_unlink(nm); fk_store_shared = 0; return 0; }
    void *p = mmap(0, (size_t)bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) { shm_unlink(nm); fk_store_shared = 0; return 0; }
    return p;
#else
    (void)letter; (void)bytes;
    return 0;
#endif
}
/* ---- the program surface: A (AST rows), S (source text), D (header + defn table) as per-pid objects ----
 * D words: 0 magic 1 layout 2 node count 3 source length 4 fntop 5 defn count 6 A shared 7 S shared
 * 8 ice images loaded 9 last ice byte length 10 ice identity fold 11 ice path length 12 AST cap 13 source cap.
 * byte 4096: the last ice path. byte 8192: per fntop index j (sym start, sym length, fn idx). +32 MiB: per fn idx the
 * body node. The rows and the text ARE the kernel's own tables; the header and defn words are written at the moment
 * they change (a defn recorded, a body bound, a unit loaded, the page noted) -- no tick, no copy of the tree. */
#define FK_PROG_MAGIC 0x53414B46LL   /* 'FKAS' */
static void *fk_prog_take(char letter, long long bytes) {
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    if (fk_prog_D == 0 && letter != 'D') {
        fk_prog_D = (long long *)fk_prog_take('D', FK_PROG_D_BYTES);
        if (fk_prog_D == 0) { return 0; }
        fk_prog_D[0] = FK_PROG_MAGIC; fk_prog_D[1] = 1;
    }
    char nm[32];
    fk_store_name(letter, (long long)getpid(), nm);
    shm_unlink(nm);
    int fd = shm_open(nm, O_CREAT | O_RDWR, 0600);
    if (fd < 0) { return 0; }
    if (ftruncate(fd, bytes) != 0) { close(fd); shm_unlink(nm); return 0; }
    void *p = mmap(0, (size_t)bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) { shm_unlink(nm); return 0; }
    return p;
#else
    (void)letter; (void)bytes;
    return 0;
#endif
}
static void fk_prog_unlink_pid(long long pid) {
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    char nm[32];
    fk_store_name('A', pid, nm); shm_unlink(nm);
    fk_store_name('S', pid, nm); shm_unlink(nm);
    fk_store_name('D', pid, nm); shm_unlink(nm);
#else
    (void)pid;
#endif
}
static void fk_prog_note_counts(void) {
    if (fk_prog_D == 0) { return; }
    fk_prog_D[2] = fk_node_count; fk_prog_D[3] = fk_slen; fk_prog_D[4] = fk_fntop; fk_prog_D[5] = fk_defn_next;
    fk_prog_D[6] = fk_prog_ast_shared; fk_prog_D[7] = fk_prog_src_shared; fk_prog_D[12] = fk_ast_cap; fk_prog_D[13] = fk_srctext_cap;
}
static void fk_prog_note_defn(long long j) {
    if (fk_prog_D == 0 || j < 0 || j >= FK_PROG_FNS) { return; }
    long long *row = (long long *)((char *)fk_prog_D + FK_PROG_D_FN_OFF) + j * 3;
    row[0] = fk_fnsym_s[j]; row[1] = fk_fnsym_n[j]; row[2] = fk_fnidx[j];
    fk_prog_note_counts();
}
static void fk_prog_note_body(long long idx) {
    if (fk_prog_D == 0 || idx < 0 || idx >= FK_PROG_FNS) { return; }
    ((long long *)((char *)fk_prog_D + FK_PROG_D_BODY_OFF))[idx] = fk_fn[idx];
    fk_prog_note_counts();   /* a body binds after its nodes exist: the counts are current here */
}
static void fk_prog_note_ice(const char *path, long long len, const char *hash_text) {
    if (fk_prog_D == 0) { return; }
    long long n = 0;
    char *dst = (char *)fk_prog_D + FK_PROG_D_PATH_OFF;
    while (path != 0 && path[n] != 0 && n < 4095) { dst[n] = path[n]; n = n + 1; }
    dst[n] = 0;
    unsigned long long fold = fk_bytes_fnv1a(dst, n);
    if (hash_text != 0) { long long hn = 0; while (hash_text[hn] != 0) { hn = hn + 1; } fold = fold ^ fk_bytes_fnv1a(hash_text, hn); }
    fk_prog_D[8] = fk_prog_D[8] + 1; fk_prog_D[9] = len; fk_prog_D[10] = (long long)(fold >> 1); fk_prog_D[11] = n;
}
static void *fk_store_copy_out(void *p, long long bytes) {
    char *q = malloc((unsigned long)bytes);
    if (q == 0) { fk_die("fk_store: out of memory leaving shared memory"); }
    long long k = 0;
    while (k < bytes) { q[k] = ((char *)p)[k]; k = k + 1; }
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    munmap(p, (size_t)bytes);
#endif
    return q;
}
/* the process leaves shared memory: every table copied to private memory once, names unlinked */
static void fk_store_go_private(void) {
    if (fk_field_on) { fk_store_shared = 0; return; }   /* the field is not this process to privatize */
    if (!fk_store_shared) { return; }
    fk_store_shared = 0;
    long long c = fk_node_cap;
    if (c > 0) {
        fk_nkind = fk_store_copy_out(fk_nkind, c * 8); fk_ncat = fk_store_copy_out(fk_ncat, c * 8); fk_nkids = fk_store_copy_out(fk_nkids, c * 8); fk_nval = fk_store_copy_out(fk_nval, c * 8);
        fk_nid = (long long (*)[4])fk_store_copy_out(fk_nid, c * 32); fk_nsfile = fk_store_copy_out(fk_nsfile, c * 8); fk_nsline = fk_store_copy_out(fk_nsline, c * 8); fk_nscol = fk_store_copy_out(fk_nscol, c * 8); fk_nsattr = fk_store_copy_out(fk_nsattr, c * 8);
    }
    if (fk_cap > 0) { fk_hh = fk_store_copy_out(fk_hh, fk_cap * 8); fk_ht = fk_store_copy_out(fk_ht, fk_cap * 8); fk_heap_alt_h = 0; fk_heap_alt_t = 0; }
    if (fk_scap_b > 0) { fk_sb = fk_store_copy_out(fk_sb, fk_scap_b); fk_so = fk_store_copy_out(fk_so, fk_scap_s * 8); fk_sl = fk_store_copy_out(fk_sl, fk_scap_s * 8); }
    if (fk_fcap > 0 && fk_fv != 0) { fk_fv = fk_store_copy_out(fk_fv, fk_fcap * 8); }
    fk_store_unlink_pid((long long)getpid());
}
/* growth of one table: inside its reservation nothing moves; past it the whole store goes private, then realloc as before */
static void *fk_store_grow(char letter, void *p, long long old_bytes, long long new_bytes, long long reserved, int zero) {
    (void)letter;
    if (fk_store_shared && p != 0 && new_bytes <= reserved) { return p; }
    if (fk_store_shared) { fk_store_go_private(); }
    char *q = realloc(p, (unsigned long)new_bytes);
    if (q == 0) { fk_die("fk_store_grow: out of memory growing a value table"); }
    if (zero) { long long k = old_bytes; while (k < new_bytes) { q[k] = 0; k = k + 1; } }
    return q;
}
/* offer (writable = 1: create, size to want+16 bytes, map read-write) or
 * receive (writable = 0: attach read-only; absent answers nothing). */
static long long fk_gift_open(const char *gname, long long want, int writable) {
#if defined(_WIN32) || !defined(FK_HAVE_MMAN_HEADER)
    (void)gname; (void)want; (void)writable;
    fk_die("fkwu: the gift frame (shm_offer/receive/write/read/seq/release) is not wired on this platform yet -- no shared-memory carrier stands here");
    return fk_nothing;
#else
    if (gname[0] != '/' || fk_path_len(gname) > 31) {
        fk_die("fkwu: shm gift name must begin with '/' and hold at most 31 bytes (the POSIX shm bound on this host)");
    }
    int gfd = shm_open(gname, writable ? (O_CREAT | O_RDWR) : O_RDONLY, 0600);
    if (gfd < 0) {
        return fk_nothing;
    }
    struct stat gst;
    if (fstat(gfd, &gst) != 0) {
        close(gfd);
        return fk_nothing;
    }
    long long cap = (long long)gst.st_size;
    if (writable) {
        long long need = want + 16;
        if (need < 4096) {
            need = 4096;
        }
        if (cap < need) {
            if (ftruncate(gfd, need) != 0 && cap == 0) {
                close(gfd);
                return fk_nothing;
            }
            if (fstat(gfd, &gst) == 0) {
                cap = (long long)gst.st_size;
            }
        }
    }
    if (cap < 16) {
        close(gfd);
        return fk_nothing;
    }
    void *base = mmap(0, (size_t)cap, writable ? (PROT_READ | PROT_WRITE) : PROT_READ, MAP_SHARED, gfd, 0);
    close(gfd);
    if (base == MAP_FAILED) {
        return fk_nothing;
    }
    if (fk_gift_count == fk_gift_cap) {
        long long nc = fk_gift_cap == 0 ? 8 : fk_gift_cap * 2;
        void **nb = realloc(fk_gift_base, sizeof(void *) * (unsigned long)nc);
        long long *ncap = realloc(fk_gift_size, sizeof(long long) * (unsigned long)nc);
        if (nb == 0 || ncap == 0) {
            fk_die("fkwu: gift table: out of memory growing");
        }
        fk_gift_base = nb;
        fk_gift_size = ncap;
        fk_gift_cap = nc;
    }
    fk_gift_base[fk_gift_count] = base;
    fk_gift_size[fk_gift_count] = cap;
    fk_gift_count = fk_gift_count + 1;
    return (fk_gift_count - 1) << 1;
#endif
}
/* the controlling terminal's window: cols when wantCols, else lines; a
 * non-terminal answers nothing */
static long long fk_terminal_dim(int wantCols) {
#if defined(_WIN32)
    (void)wantCols;
    return fk_nothing;
#else
    struct fk_winsize fk_ws;
    if (ioctl(1, FK_TIOCGWINSZ, &fk_ws) != 0 && ioctl(0, FK_TIOCGWINSZ, &fk_ws) != 0) {
        return fk_nothing;
    }
    return ((long long)(wantCols ? fk_ws.ws_col : fk_ws.ws_row)) << 1;
#endif
}
/* tentative declarations: the program text and its unit table are defined
 * later in this file; kernel_hot reads them from the cold ladder */
static char *fk_srctext;
static long long fk_slen;
static long long fk_src_dep_count;
static char fk_src_root_path[FK_PATH_CAP];
static char (*fk_src_dep_path)[FK_PATH_CAP];
static long long *fk_src_dep_text_off;
static long long *fk_src_dep_text_len;
/* append n bytes to the string builder at fk_sbp, growing it as every arm does */
static void fk_sappend(const char *bytes, long long n) {
    while (fk_sbp + n > fk_scap_b) {
        fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b, fk_scap_b * 2, FK_STORE_STR_BYTES, 0);
        fk_scap_b = fk_scap_b * 2;
        fk_sb_check();
    }
    long long k = 0;
    while (k < n) { fk_sb[fk_sbp + k] = bytes[k]; k = k + 1; }
    fk_sbp = fk_sbp + n;
}
static long long fk_intern_int_node(long long iv43) {
        long long h43 = fk_intern_key_trivial(1, iv43);
        if (fk_field_on) { return fk_field_intern_node(1, 1, iv43, 0, 0, h43); }
        long long slot43 = 0;
        if (fk_np + 1 >= fk_node_cap) {
            fk_nodes_grow();
        }
        slot43 = h43 & (fk_intern_hash_cap - 1);
        while (fk_intern_tab[slot43]) {
            long long ix43 = fk_intern_tab[slot43];
            if (fk_nkind[ix43] == 1 && fk_nid[ix43][2] == 1 && fk_nval[ix43] == iv43) {
                return fk_nbox(ix43);
            }
            slot43 = (slot43 + 1) & (fk_intern_hash_cap - 1);
        }
        fk_np = fk_np + 1; fk_mint_total = fk_mint_total + 1; if (fk_fn_mint != 0 && fk_cur_fn > 0 && fk_cur_fn < fk_fn_capacity) { fk_fn_mint[fk_cur_fn] = fk_fn_mint[fk_cur_fn] + 1; }
        fk_nkind[fk_np] = 1;
        fk_nval[fk_np] = iv43;
        fk_nkids[fk_np] = 1;
        fk_ncat[fk_np] = 0;
        fk_nid[fk_np][0] = 1;
        fk_nid[fk_np][1] = 1;
        fk_nid[fk_np][2] = 1;
        fk_nid[fk_np][3] = iv43 >> 1;
        fk_intern_tab[slot43] = fk_np;
        fk_nhash_memo[fk_np] = h43;
        return fk_nbox(fk_np);
}
static long long fk_intern_str_node(long long sv46) {
        long long sa46 = fk_stri(sv46);
        long long h46 = fk_intern_key_trivial(2, sv46);
        if (fk_field_on) { return fk_field_intern_node(1, 2, sv46, 0, 0, fk_intern_key_trivial(2, fk_deep_hash(sv46))); }
        long long slot46 = 0;
        if (fk_np + 1 >= fk_node_cap) {
            fk_nodes_grow();
        }
        slot46 = h46 & (fk_intern_hash_cap - 1);
        while (fk_intern_tab[slot46]) {
            long long ix46 = fk_intern_tab[slot46];
            if (fk_nkind[ix46] == 1 && fk_nid[ix46][2] == 2 && fk_nval[ix46] == sv46) {
                return fk_nbox(ix46);
            }
            slot46 = (slot46 + 1) & (fk_intern_hash_cap - 1);
        }
        if (sa46 < 0 || !FK_SOK(sa46)) {
            return 0;
        }
        fk_np = fk_np + 1; fk_mint_total = fk_mint_total + 1; if (fk_fn_mint != 0 && fk_cur_fn > 0 && fk_cur_fn < fk_fn_capacity) { fk_fn_mint[fk_cur_fn] = fk_fn_mint[fk_cur_fn] + 1; }
        fk_nkind[fk_np] = 1;
        fk_nval[fk_np] = sv46;
        fk_nkids[fk_np] = 1;
        fk_ncat[fk_np] = 0;
        fk_nid[fk_np][0] = 1;
        fk_nid[fk_np][1] = 1;
        fk_nid[fk_np][2] = 2;
        fk_nid[fk_np][3] = sa46;
        fk_intern_tab[slot46] = fk_np;
        fk_nhash_memo[fk_np] = h46;
        return fk_nbox(fk_np);
}
static long long fk_intern_bool_node(long long bv112) {
        long long se112 = (bv112 != 0) ? (0 - 9223372036854775807LL) : (0 - 9223372036854775805LL);
        long long h112 = fk_intern_key_trivial(3, se112);
        if (fk_field_on) { return fk_field_intern_node(1, 3, se112, 0, 0, h112); }
        long long slot112 = 0;
        if (fk_np + 1 >= fk_node_cap) {
            fk_nodes_grow();
        }
        slot112 = h112 & (fk_intern_hash_cap - 1);
        while (fk_intern_tab[slot112]) {
            long long ix112 = fk_intern_tab[slot112];
            if (fk_nkind[ix112] == 1 && fk_nid[ix112][2] == 3 && fk_nval[ix112] == se112) {
                return fk_nbox(ix112);
            }
            slot112 = (slot112 + 1) & (fk_intern_hash_cap - 1);
        }
        fk_np = fk_np + 1; fk_mint_total = fk_mint_total + 1; if (fk_fn_mint != 0 && fk_cur_fn > 0 && fk_cur_fn < fk_fn_capacity) { fk_fn_mint[fk_cur_fn] = fk_fn_mint[fk_cur_fn] + 1; }
        fk_nkind[fk_np] = 1;
        fk_nval[fk_np] = se112;
        fk_nkids[fk_np] = 1;
        fk_ncat[fk_np] = 0;
        fk_nid[fk_np][0] = 1;
        fk_nid[fk_np][1] = 1;
        fk_nid[fk_np][2] = 3;
        fk_intern_tab[slot112] = fk_np;
        fk_nhash_memo[fk_np] = h112;
        fk_nid[fk_np][3] = (bv112 != 0) ? 1 : 0;
        return fk_nbox(fk_np);
}
static long long fk_intern_float_node(double fd113) {
    unsigned long long fbits113;
        if (fd113 != fd113) {
            fbits113 = 0x7ff8000000000000ULL;
        } else {
            if (fd113 == 0.0) {
                fd113 = 0.0;
            }
            memcpy(&fbits113, &fd113, 8);
        }
        double fcanon113;
        memcpy(&fcanon113, &fbits113, 8);
        long long h113 = fk_intern_key_trivial(7, (long long)fbits113);
        if (fk_field_on) { return fk_field_intern_node(1, 7, fk_fbox(fcanon113), 0, 0, h113); }
        if (fk_np + 1 >= fk_node_cap) {
            fk_nodes_grow();
        }
        long long slot113 = h113 & (fk_intern_hash_cap - 1);
        while (fk_intern_tab[slot113]) {
            long long ix113 = fk_intern_tab[slot113];
            if (fk_nkind[ix113] == 1 && fk_nid[ix113][2] == 7) {
                double dv113 = fk_num(fk_nval[ix113]);
                unsigned long long db113;
                memcpy(&db113, &dv113, 8);
                if (db113 == fbits113) {
                    return fk_nbox(ix113);
                }
            }
            slot113 = (slot113 + 1) & (fk_intern_hash_cap - 1);
        }
        long long fb113 = fk_fbox(fcanon113);
        fk_np = fk_np + 1; fk_mint_total = fk_mint_total + 1; if (fk_fn_mint != 0 && fk_cur_fn > 0 && fk_cur_fn < fk_fn_capacity) { fk_fn_mint[fk_cur_fn] = fk_fn_mint[fk_cur_fn] + 1; }
        fk_nkind[fk_np] = 1;
        fk_nval[fk_np] = fb113;
        fk_nkids[fk_np] = 1;
        fk_ncat[fk_np] = 0;
        fk_nid[fk_np][0] = 1;
        fk_nid[fk_np][1] = 1;
        fk_nid[fk_np][2] = 7;
        fk_nid[fk_np][3] = fk_fidx(fb113);
        fk_intern_tab[slot113] = fk_np;
        fk_nhash_memo[fk_np] = h113;
        return fk_nbox(fk_np);
}
static long long fk_intern_composite(long long cat47, long long kids47) {
        unsigned long long hm47 = fk_mix64(7, 2);
        hm47 = fk_mix64(hm47, (unsigned long long)fk_deep_hash(cat47));
        hm47 = fk_mix64(hm47, (unsigned long long)fk_deep_hash(kids47));
        long long h47 = (long long)(hm47 >> 1);
        if (h47 == 0) {
            h47 = 1;
        }
        if (fk_field_on) { return fk_field_intern_node(2, 0, cat47, kids47, 0, h47); }
        long long slot47 = 0;
        if (fk_np + 1 >= fk_node_cap) {
            fk_nodes_grow();
        }
        slot47 = h47 & (fk_intern_hash_cap - 1);
        while (fk_intern_tab[slot47]) {
            long long ix47 = fk_intern_tab[slot47];
            if (fk_nkind[ix47] == 2 && fk_veq(fk_ncat[ix47], cat47) != 0 &&
                fk_veq(fk_nkids[ix47], kids47) != 0) {
                return fk_nbox(ix47);
            }
            slot47 = (slot47 + 1) & (fk_intern_hash_cap - 1);
        }
        fk_np = fk_np + 1; fk_mint_total = fk_mint_total + 1; if (fk_fn_mint != 0 && fk_cur_fn > 0 && fk_cur_fn < fk_fn_capacity) { fk_fn_mint[fk_cur_fn] = fk_fn_mint[fk_cur_fn] + 1; }
        fk_nkind[fk_np] = 2;
        fk_ncat[fk_np] = cat47;
        fk_nkids[fk_np] = kids47;
        fk_nval[fk_np] = 0;
        fk_intern_tab[slot47] = fk_np;
        fk_nhash_memo[fk_np] = h47;
        fk_nid[fk_np][0] = 0;
        fk_nid[fk_np][1] = 0;
        fk_nid[fk_np][2] = 0;
        fk_nid[fk_np][3] = fk_np;
        if (cat47 < 0) {
            long long ci47 = fk_nidx(cat47);
            if (ci47 >= 1 && ci47 <= fk_np) {
                fk_nid[fk_np][1] = fk_nid[ci47][1];
                fk_nid[fk_np][2] = fk_nid[ci47][2];
            }
        }
        return fk_nbox(fk_np);
}
static long long fk_make_nodeid(long long p91, long long l91, long long ty91, long long in91) {
        if (fk_field_on) { long long nid91[4]; nid91[0] = p91; nid91[1] = l91; nid91[2] = ty91; nid91[3] = in91; return fk_field_intern_node(3, 0, 0, 0, nid91, 0); }
        if (fk_np + 1 >= fk_node_cap) {
            fk_nodes_grow();
        }
        fk_np = fk_np + 1; fk_mint_total = fk_mint_total + 1; if (fk_fn_mint != 0 && fk_cur_fn > 0 && fk_cur_fn < fk_fn_capacity) { fk_fn_mint[fk_cur_fn] = fk_fn_mint[fk_cur_fn] + 1; }
        fk_nkind[fk_np] = 3;
        fk_ncat[fk_np] = 0;
        fk_nkids[fk_np] = 1;
        fk_nval[fk_np] = 0;
        fk_nid[fk_np][0] = p91;
        fk_nid[fk_np][1] = l91;
        fk_nid[fk_np][2] = ty91;
        fk_nid[fk_np][3] = in91;
        return fk_nbox(fk_np);
}
/* give n bytes into gift frame gh under the seqlock; answers the even sequence
 * after the give, or nothing when the handle is dead or the frame does not fit */
/* A give CLAIMS the sequence, so two writers cannot take the same one.
 * Until 2026-09-07 a give loaded s0, stored s0+1, copied, stored s0+2. Two
 * processes could load the same s0 and both close it, and a frame left odd
 * never repaired -- its next give computed parity from what it had loaded, so
 * the readers' seqlock retried 4096 times and answered nothing, forever.
 * Witnessed that morning: one writer of a ten-row frame reads back current,
 * three concurrent writers leave it malformed after every writer has exited,
 * and the live resource.governor frame read malformed 30 of 30 with three
 * glass fleets standing on this host.
 * Now: acquire by compare-exchange on an EVEN sequence (the odd value IS the
 * lock), copy, close with the even successor. A writer that dies mid-give
 * leaves the sequence odd and unmoving; a later writer that watches it not
 * move for FK_GIFT_GIVE_SPINS closes it on that writer's behalf and takes its
 * own turn -- a frame heals rather than staying dark. */
#define FK_GIFT_GIVE_SPINS 200000
static long long fk_gift_give(long long gh, const char *bytes, long long n) {
    if (!fk_gift_live(gh)) {
        return fk_nothing;
    }
    volatile long long *gseq = (volatile long long *)fk_gift_base[gh];
    volatile long long *glen = gseq + 1;
    char *gpay = (char *)fk_gift_base[gh] + 16;
    if (n > fk_gift_size[gh] - 16) {
        return fk_nothing;
    }
    long long s0;
    long long stuck = -1;
    long long spins = 0;
    for (;;) {
        s0 = __atomic_load_n(gseq, __ATOMIC_ACQUIRE);
        if ((s0 & 1) == 0) {
            long long want = s0;
            if (__atomic_compare_exchange_n(gseq, &want, s0 + 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
                break;
            }
            stuck = -1;
            spins = 0;
            continue;
        }
        if (s0 != stuck) { stuck = s0; spins = 0; }
        spins = spins + 1;
        if (spins > FK_GIFT_GIVE_SPINS) {
            long long want = s0;
            __atomic_compare_exchange_n(gseq, &want, s0 + 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
            stuck = -1;
            spins = 0;
        }
    }
    { long long k = 0; while (k < n) { gpay[k] = bytes[k]; k = k + 1; } }
    __atomic_store_n(glen, n, __ATOMIC_RELEASE);
    __atomic_store_n(gseq, s0 + 2, __ATOMIC_RELEASE);
    return (s0 + 2) << 1;
}
/* take the stable frame of gh as a string value (nothing when dead, never
 * written, or a give that never settles) -- the seqlock read */
static long long fk_gift_take_str(long long gh) {
    if (!fk_gift_live(gh)) {
        return fk_nothing;
    }
    volatile long long *gseq = (volatile long long *)fk_gift_base[gh];
    volatile long long *glen = gseq + 1;
    char *gpay = (char *)fk_gift_base[gh] + 16;
    int tries = 0;
    for (;;) {
        long long s1 = __atomic_load_n(gseq, __ATOMIC_ACQUIRE);
        if (s1 == 0) {
            return fk_nothing;
        }
        if ((s1 & 1) == 0) {
            long long n = __atomic_load_n(glen, __ATOMIC_ACQUIRE);
            if (n < 0 || n > fk_gift_size[gh] - 16) {
                return fk_nothing;
            }
            fk_sinit();
            while (fk_sbp + n > fk_scap_b) {
                fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b, fk_scap_b * 2, FK_STORE_STR_BYTES, 0);
                fk_scap_b = fk_scap_b * 2;
                fk_sb_check();
            }
            { long long k = 0; while (k < n) { fk_sb[fk_sbp + k] = gpay[k]; k = k + 1; } }
            long long s2 = __atomic_load_n(gseq, __ATOMIC_ACQUIRE);
            if (s1 == s2) {
                return fk_strv(fk_sintern(fk_sbp, n));
            }
        }
        tries = tries + 1;
        if (tries > 4096) {
            return fk_nothing;
        }
    }
}
/* THE CELL CROSSING: a value crosses a gift frame as itself. Words, one tag
 * byte each: N nothing, I int, F float, S string, L list (count, elements,
 * then Z for nil or the dotted tail), and the cells -- i/s/b/f the trivial
 * nodes, C a composite (category then children, re-interned on read so the
 * same category over the same children is the same cell, axiom 3), D a
 * NodeID coordinate. A function value cannot cross and refuses the give. */
static long long fk_cross_refused;
static void fk_cross_put_i64(long long v) { fk_sappend((const char *)&v, 8); }
static void fk_cross_put_u32(long long v) { unsigned int u = (unsigned int)v; fk_sappend((const char *)&u, 4); }
static void fk_cross_emit(long long v, long long depth) {
    if (depth > 100000) { fk_cross_refused = 1; return; }
    if (v == fk_nothing) { fk_sappend("N", 1); return; }
    if (fk_isf(v)) { double d = fk_num(v); fk_sappend("F", 1); fk_sappend((const char *)&d, 8); return; }
    if (fk_is_str(v)) { long long si = fk_stri(v); fk_sappend("S", 1); fk_cross_put_u32(FK_SLEN(si)); fk_sappend(FK_SBYTES(si), FK_SLEN(si)); return; }
    if (v >= fk_fnbase && v < fk_fnbase + 8192) { fk_cross_refused = 1; fk_sappend("N", 1); return; }
    if ((v & 1) == 0) { fk_sappend("I", 1); fk_cross_put_i64(v >> 1); return; }
    if (v >= 0) {
        /* a list: count the proper elements, then emit them and the tail */
        long long p = v, count = 0;
        while ((p & 1) && p != 1 && (p >> 1) >= 1 && FK_POK(p >> 1) && count < 100000000) { count = count + 1; p = FK_HT(p >> 1); }
        fk_sappend("L", 1); fk_cross_put_u32(count);
        p = v;
        long long k = 0;
        while (k < count) { fk_cross_emit(FK_HH(p >> 1), depth + 1); p = FK_HT(p >> 1); k = k + 1; }
        if (p == 1) { fk_sappend("Z", 1); } else { fk_cross_emit(p, depth + 1); }
        return;
    }
    if (v >= fk_fnbase && v < fk_fnbase + 8192) { fk_cross_refused = 1; fk_sappend("N", 1); return; }
    long long ix = fk_nidx(v);
    if (ix < 1 || ix > fk_np) { fk_cross_refused = 1; fk_sappend("N", 1); return; }
    if (fk_nkind[ix] == 1) {
        long long sub = fk_nid[ix][2];
        if (sub == 1) { fk_sappend("i", 1); fk_cross_put_i64(fk_nval[ix] >> 1); return; }
        if (sub == 2) { long long si = fk_stri(fk_nval[ix]); fk_sappend("s", 1); if (si < 0) { fk_cross_put_u32(0); return; } fk_cross_put_u32(FK_SLEN(si)); fk_sappend(FK_SBYTES(si), FK_SLEN(si)); return; }
        if (sub == 3) { fk_sappend("b", 1); fk_sappend(fk_nval[ix] == (0 - 9223372036854775807LL) ? "\1" : "\0", 1); return; }
        if (sub == 7) { double d = fk_num(fk_nval[ix]); fk_sappend("f", 1); fk_sappend((const char *)&d, 8); return; }
        fk_cross_refused = 1; fk_sappend("N", 1); return;
    }
    if (fk_nkind[ix] == 3) { fk_sappend("D", 1); fk_cross_put_i64(fk_nid[ix][0]); fk_cross_put_i64(fk_nid[ix][1]); fk_cross_put_i64(fk_nid[ix][2]); fk_cross_put_i64(fk_nid[ix][3]); return; }
    fk_sappend("C", 1);
    fk_cross_emit(fk_ncat[ix], depth + 1);
    fk_cross_emit(fk_nkids[ix], depth + 1);
}
static long long fk_cross_get_i64(const char *b, long long n, long long *pos) { long long v = 0; if (*pos + 8 <= n) { long long k = 0; while (k < 8) { ((char *)&v)[k] = b[*pos + k]; k = k + 1; } } *pos = *pos + 8; return v; }
static long long fk_cross_get_u32(const char *b, long long n, long long *pos) { unsigned int u = 0; if (*pos + 4 <= n) { long long k = 0; while (k < 4) { ((char *)&u)[k] = b[*pos + k]; k = k + 1; } } *pos = *pos + 4; return (long long)u; }
static long long fk_cross_decode(const char *b, long long n, long long *pos, long long depth) {
    if (depth > 100000 || *pos >= n) { fk_cross_refused = 1; return fk_nothing; }
    char tg = b[*pos]; *pos = *pos + 1;
    if (tg == 'N') { return fk_nothing; }
    if (tg == 'I') { return fk_cross_get_i64(b, n, pos) << 1; }
    if (tg == 'F') { long long w = fk_cross_get_i64(b, n, pos); double d; { long long k = 0; while (k < 8) { ((char *)&d)[k] = ((char *)&w)[k]; k = k + 1; } } return fk_fbox(d); }
    if (tg == 'S' || tg == 's') { long long len = fk_cross_get_u32(b, n, pos); if (*pos + len > n) { fk_cross_refused = 1; return fk_nothing; } long long sv = fk_sbuf(b + *pos, len); *pos = *pos + len; return tg == 'S' ? sv : fk_intern_str_node(sv); }
    if (tg == 'i') { return fk_intern_int_node(fk_cross_get_i64(b, n, pos) << 1); }
    if (tg == 'b') { char bv = b[*pos]; *pos = *pos + 1; return fk_intern_bool_node(bv ? 2 : 0); }
    if (tg == 'f') { long long w = fk_cross_get_i64(b, n, pos); double d; { long long k = 0; while (k < 8) { ((char *)&d)[k] = ((char *)&w)[k]; k = k + 1; } } return fk_intern_float_node(d); }
    if (tg == 'D') { long long p = fk_cross_get_i64(b, n, pos), l = fk_cross_get_i64(b, n, pos), ty = fk_cross_get_i64(b, n, pos), in = fk_cross_get_i64(b, n, pos); return fk_make_nodeid(p, l, ty, in); }
    if (tg == 'C') { long long cat = fk_cross_decode(b, n, pos, depth + 1); long long kids = fk_cross_decode(b, n, pos, depth + 1); return fk_intern_composite(cat, kids); }
    if (tg == 'L') {
        long long count = fk_cross_get_u32(b, n, pos);
        if (count < 0 || count > 100000000) { fk_cross_refused = 1; return fk_nothing; }
        long long *elems = malloc(sizeof(long long) * (unsigned long)(count + 1));
        if (elems == 0) { fk_die("fk_cross_decode: out of memory for a list"); }
        long long k = 0;
        while (k < count) { elems[k] = fk_cross_decode(b, n, pos, depth + 1); k = k + 1; }
        long long tail = 1;
        if (*pos < n && b[*pos] == 'Z') { *pos = *pos + 1; } else { tail = fk_cross_decode(b, n, pos, depth + 1); }
        k = count;
        while (k > 0) { k = k - 1; tail = fk_cons_val(elems[k], tail); }
        free(elems);
        return tail;
    }
    fk_cross_refused = 1;
    return fk_nothing;
}
/* ---- the kernel's live page: its own counters in shared memory, written in place ----
 * Every fkwu maps /fg-k<pid> once and, every 1024 primitive dispatches and at exit, stores
 * its counters into it -- no wire, no serialization; a reader maps the same page and reads
 * the words by offset. /fg-kernels is the roster: a slot per registered pid. Word layout
 * (after the 16-byte gift header): 0 magic 1 pid 2 start-ms 3 seq 4 dispatches 5 heat calls
 * 6 nodes 7 strings 8 cons 9 fns 10 gift frames 11 gift bytes 12 node cap 13 heap cap
 * 14 value-stack depth 15 floats 16 hottest tag 17 hottest count 18 cpu us 19 alive 20 distinct arms
 * 21 melt generation 22 store shared (1: per-kernel columns, 2: the field) 23 heap generation (0: h/t, 1: H/T)
 * 24 float boxes minted 25 float boxes read 26 native leaf calls */
#define FK_LIVE_MAGIC 0x464B4C4956LL
#define FK_LIVE_WORDS 34
static volatile long long *fk_live_page;
static long long fk_live_ticks;
static void fk_live_pid_name(long long pid, char *out) {
    char digits[24];
    long long n = 0;
    long long p = pid < 0 ? 0 : pid;
    if (p == 0) { digits[n] = '0'; n = n + 1; }
    while (p > 0) { digits[n] = (char)('0' + (p % 10)); n = n + 1; p = p / 10; }
    long long o = 0;
    out[o] = '/'; out[o + 1] = 'f'; out[o + 2] = 'g'; out[o + 3] = '-'; out[o + 4] = 'k'; o = 5;
    while (n > 0) { n = n - 1; out[o] = digits[n]; o = o + 1; }
    out[o] = 0;
}
static long long fk_live_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long)ts.tv_sec * 1000 + (long long)ts.tv_nsec / 1000000;
}
static long long fk_live_cpu_us(void) {
    struct timespec tc;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tc);
    return (long long)tc.tv_sec * 1000000 + (long long)tc.tv_nsec / 1000;
}
/* ---- the live page, second layout: the kernel counts INTO the page ----
 * bytes 0..15 gift header; words at +16: 0 magic 1 pid 2 start-ms 3 seq 4 (reader sums the arms) 5 heat total
 * 6 nodes 7 strings 8 cons 9 fns 10 gift frames 11 gift bytes 12 node cap 13 heap cap 14 stack 15 floats
 * 16 (reader) 17 (reader) 18 cpu us 19 alive 20 (reader) 21 melt gen 22 store 23 heap gen 24 boxes 25 unboxes
 * 26 native leaf calls 27 fn count 28 layout version (3) 29 meta count 30 defns crystallized 31 native loops standing 32 native loop iterations.
 * byte 4096: the 256 arm counters. 8192: heat per fn (2^20 words). +8 MiB: boxes per fn. +16 MiB: unboxes per fn.
 * +24 MiB: meta per fn, 5 words (name offset, name length, unit length, line, col). +64 MiB: the name blob (name then
 * unit path, 32 MiB). Every hot-path increment the kernel already made now lands in these words; nothing is copied,
 * nothing is scheduled, no tick is counted. The words the kernel does not increment (6-15, 18, 21-23, 27) are noted
 * at melt, at exit and when the kernel reads its own page. */
#define FK_LIVE_VERSION 4
#define FK_LIVE_PAGE_BYTES ((128LL << 20))
#define FK_LIVE_ARMS_OFF 4096
#define FK_LIVE_FNS (1LL << 20)
#define FK_LIVE_HEAT_OFF 8192
#define FK_LIVE_FBOX_OFF (8192 + (8LL << 20))
#define FK_LIVE_UNBOX_OFF (8192 + (16LL << 20))
#define FK_LIVE_META_OFF (8192 + (24LL << 20))
#define FK_LIVE_BLOB_OFF (8192 + (64LL << 20))
#define FK_LIVE_BLOB_BYTES (32LL << 20)
#define FK_LIVE_NATIVE_OFF (8192 + (96LL << 20))
#define FK_LIVE_MINT_OFF (8192 + (104LL << 20)) /* per-defn value-cell mints: which recipe grew the permanent arena */
#define FK_LIVE_INRAM_OFF (8192 + (112LL << 20)) /* per-defn FOLDED calls: a whole body run as this machine's own instructions, no pool slot per result */
static long long fk_live_blob_used;
static long long *fk_nl_offs;        /* newline offsets of the program text, scanned once and extended as the text grows */
static long long fk_nl_count, fk_nl_cap, fk_nl_scanned;
static void fk_live_line_col(long long off, long long *line, long long *col) {
    while (fk_nl_scanned < fk_slen) {
        if (fk_srctext[fk_nl_scanned] == FK_CH_LF) {
            if (fk_nl_count == fk_nl_cap) { long long nc = fk_nl_cap == 0 ? 4096 : fk_nl_cap * 2; long long *nb = realloc(fk_nl_offs, (unsigned long)(nc * 8)); if (nb == 0) { fk_die("fk_live_line_col: out of memory"); } fk_nl_offs = nb; fk_nl_cap = nc; }
            fk_nl_offs[fk_nl_count] = fk_nl_scanned;
            fk_nl_count = fk_nl_count + 1;
        }
        fk_nl_scanned = fk_nl_scanned + 1;
    }
    long long lo = 0, hi = fk_nl_count;   /* newlines strictly before off */
    while (lo < hi) { long long mid = (lo + hi) / 2; if (fk_nl_offs[mid] < off) { lo = mid + 1; } else { hi = mid; } }
    *line = lo + 1;
    *col = off - (lo == 0 ? -1 : fk_nl_offs[lo - 1]);
}
/* a defn's name, unit and source pointer, written once when the defn is recorded */
static void fk_live_note_defn(long long j) {
    if (j >= 0 && j < fk_fntop + 1) { fk_prog_note_defn(j); }
    if (fk_live_page == 0 || j < 0 || j >= fk_fntop + 1) { return; }
    long long fx = fk_fnidx[j];
    if (fx < 0 || fx >= FK_LIVE_FNS) { return; }
    long long so = fk_fnsym_s[j], nl = fk_fnsym_n[j];
    const char *unit = fk_hot_unit_of(so);
    long long ul = fk_cstrlen(unit);
    if (fk_live_blob_used + nl + ul > FK_LIVE_BLOB_BYTES) { return; }
    char *blob = (char *)fk_live_page + FK_LIVE_BLOB_OFF;
    long long k = 0;
    while (k < nl) { blob[fk_live_blob_used + k] = fk_srctext[so + k]; k = k + 1; }
    k = 0;
    while (k < ul) { blob[fk_live_blob_used + nl + k] = unit[k]; k = k + 1; }
    long long line = 0, col = 0;
    fk_live_line_col(so, &line, &col);
    long long *meta = (long long *)((char *)fk_live_page + FK_LIVE_META_OFF) + fx * 5;
    meta[0] = fk_live_blob_used; meta[1] = nl; meta[2] = ul; meta[3] = line; meta[4] = col;
    fk_live_blob_used = fk_live_blob_used + nl + ul;
    volatile long long *w = fk_live_page + 2;
    if (fx + 1 > w[29]) { w[29] = fx + 1; }
}
static void fk_live_roster_register(long long pid) {
    long long gh = fk_gift_open("/fg-kernels", 4096, 1);
    if (gh == fk_nothing) { return; }
    volatile long long *slots = (volatile long long *)fk_gift_base[gh >> 1] + 2;
    long long k = 0, free_slot = -1;
    while (k < 256) {
        long long v = slots[k];
        if (v == pid) { free_slot = -1; break; }
        if (v > 0 && v != pid && kill((int)v, 0) != 0) { char dn[32]; fk_live_pid_name(v, dn); shm_unlink(dn); fk_store_unlink_pid(v); fk_prog_unlink_pid(v); slots[k] = 0; v = 0; }
        if (free_slot < 0 && v == 0) { free_slot = k; }
        k = k + 1;
    }
    if (free_slot >= 0) { slots[free_slot] = pid; }
    munmap(fk_gift_base[gh >> 1], (size_t)fk_gift_size[gh >> 1]);
    fk_gift_base[gh >> 1] = 0;
}
static void fk_live_note(int final);
/* the page opens once: the counters the kernel increments repoint into it, what was counted so far carried over */
static void fk_live_open(void) {
    if (fk_live_page != 0) { return; }
    char name[32];
    long long pid = (long long)getpid();
    fk_live_pid_name(pid, name);
    long long gh = fk_gift_open(name, FK_LIVE_PAGE_BYTES, 1);
    if (gh == fk_nothing) { return; }
    fk_live_page = (volatile long long *)fk_gift_base[gh >> 1];
    volatile long long *w = fk_live_page + 2;
    w[1] = pid;
    w[2] = fk_live_now_ms();
    w[3] = 0;
    w[28] = FK_LIVE_VERSION;
    long long *arms = (long long *)((char *)fk_live_page + FK_LIVE_ARMS_OFF);
    long long k = 0;
    while (k < FK_OPCODE_ARM_CAP) { arms[k] = fk_arms[k]; k = k + 1; }
    fk_arms = arms;
    w[5] = fk_heat_total; fk_heat_total_p = (long long *)&w[5];
    w[24] = fk_box_total; fk_box_total_p = (long long *)&w[24];
    w[25] = fk_unbox_total; fk_unbox_total_p = (long long *)&w[25];
    w[26] = fk_inram_call_total; fk_inram_call_total_p = (long long *)&w[26];
    long long *heat = (long long *)((char *)fk_live_page + FK_LIVE_HEAT_OFF);
    long long *fbox = (long long *)((char *)fk_live_page + FK_LIVE_FBOX_OFF);
    long long *unbox = (long long *)((char *)fk_live_page + FK_LIVE_UNBOX_OFF);
    long long cap = fk_fn_capacity < FK_LIVE_FNS ? fk_fn_capacity : FK_LIVE_FNS;
    k = 0;
    while (k < cap) { heat[k] = fk_fn_heat ? fk_fn_heat[k] : 0; fbox[k] = fk_fn_fbox ? fk_fn_fbox[k] : 0; unbox[k] = fk_fn_unbox ? fk_fn_unbox[k] : 0; k = k + 1; }
    free(fk_fn_heat); free(fk_fn_fbox); free(fk_fn_unbox);
    fk_fn_heat = heat; fk_fn_fbox = fbox; fk_fn_unbox = unbox;
    long long *native = (long long *)((char *)fk_live_page + FK_LIVE_NATIVE_OFF);
    long long *mint = (long long *)((char *)fk_live_page + FK_LIVE_MINT_OFF);
    k = 0;
    while (k < cap) { native[k] = fk_fn_native ? fk_fn_native[k] : 0; k = k + 1; }
    free(fk_fn_native); fk_fn_native = native;
    k = 0;
    while (k < cap) { mint[k] = fk_fn_mint ? fk_fn_mint[k] : 0; k = k + 1; }
    free(fk_fn_mint); fk_fn_mint = mint;
    long long *inram = (long long *)((char *)fk_live_page + FK_LIVE_INRAM_OFF);
    k = 0;
    while (k < cap) { inram[k] = fk_fn_inram ? fk_fn_inram[k] : 0; k = k + 1; }
    free(fk_fn_inram); fk_fn_inram = inram;
    w[30] = fk_f64_count; fk_f64_count_p = (long long *)&w[30];
    w[31] = fk_f64_loop_count; fk_f64_loop_count_p = (long long *)&w[31];
    w[32] = fk_f64_loop_iters; fk_f64_loop_iters_p = (long long *)&w[32];
    w[33] = fk_mint_total; fk_mint_total_p = (long long *)&w[33];
    fk_live_ledgers_paged = 1;
    k = 0;
    while (k < fk_fntop) { fk_live_note_defn(k); k = k + 1; }
    w[0] = FK_LIVE_MAGIC;
    fk_live_roster_register(pid);
    fk_live_note(0);
}
/* the words the kernel does not increment on its hot path: noted at melt, at exit, and when it reads its own page */
static void fk_live_note(int final) {
    if (fk_live_page == 0) { fk_live_open(); if (fk_live_page == 0) { return; } }
    volatile long long *w = fk_live_page + 2;
    long long gb = 0, gi = 0;
    while (gi < fk_gift_count) { if (fk_gift_base[gi] != 0) { gb = gb + fk_gift_size[gi]; } gi = gi + 1; }
    w[6] = fk_np; w[7] = fk_sp; w[8] = fk_hp; w[9] = fk_fntop; w[10] = fk_gift_count; w[11] = gb; w[12] = fk_node_cap; w[13] = fk_cap; w[14] = fk_vsp; w[15] = fk_fp;
    w[18] = fk_live_cpu_us(); w[19] = final ? 0 : 1; w[21] = fk_melt_gen; w[22] = fk_field_on ? 2 : fk_store_shared; w[23] = fk_heap_gen; w[27] = fk_fntop;
    __atomic_store_n(&w[3], w[3] + 1, __ATOMIC_RELEASE);
    fk_prog_note_counts();
}
static void fk_live_publish(int final) { fk_live_note(final); }
/* read-only mapping of another kernel's page or the roster: the words, then unmap */
static long long fk_live_read_words(const char *name, long long *out, long long count) {
    long long gh = fk_gift_open(name, 0, 0);
    if (gh == fk_nothing) { return -1; }
    volatile long long *w = (volatile long long *)fk_gift_base[gh >> 1] + 2;
    long long k = 0;
    while (k < count) { out[k] = w[k]; k = k + 1; }
    munmap(fk_gift_base[gh >> 1], (size_t)fk_gift_size[gh >> 1]);
    fk_gift_base[gh >> 1] = 0;
    return count;
}
/* ---- the publisher roster: names of every gift frame that carries a snapshot, 511 slots of 128 bytes (a name is root, a bar, publisher; up to 119 bytes) ---- */
#define FK_ROSTER_SLOTS 511
/* A slot's timestamp is when this name last SPOKE, refreshed on every register,
 * which a live publisher does every publish. When the roster is full the least
 * recently spoken slot is taken; nothing alive is displaced, because a
 * publisher that is still giving is, by definition, not the oldest.
 *
 * A name stands after its process ends, and that is the carrier's contract, not
 * a leak: shared memory outlives the giver, so a frame published an hour ago is
 * still there to be read. What separates a giver from a frame merely standing
 * is TIME, and the roster is the only place that time is kept for every slot --
 * a malformed or non-snapshot frame has no epoch of its own to ask. So the
 * roster hands its stamp back with every name: a roster row is (name
 * lastSpokeMs), and a reader tells the living from the standing without opening
 * a single frame. The BOUND stays the reader's; only the fact is published. */
static long long fk_roster_register(const char *name) {
    long long gh = fk_gift_open("/fg-roster", 65536, 1);
    if (gh == fk_nothing) { return -1; }
    char *base = (char *)fk_gift_base[gh >> 1] + 64;
    long long k = 0, free_slot = -1, found = -1, oldest = -1;
    long long oldest_at = 0;
    while (k < FK_ROSTER_SLOTS) {
        char *slot = base + k * 128;
        if (slot[0] == 0) { if (free_slot < 0) { free_slot = k; } }
        else if (fk_cstr_eq(slot, name)) { found = k; break; }
        else {
            long long at = *(long long *)(slot + 120);
            if (oldest < 0 || at < oldest_at) { oldest = k; oldest_at = at; }
        }
        k = k + 1;
    }
    if (found < 0 && free_slot < 0) { free_slot = oldest; }
    if (found < 0 && free_slot >= 0) {
        char *slot = base + free_slot * 128;
        long long n = 0;
        while (n < 119 && name[n] != 0) { slot[n] = name[n]; n = n + 1; }
        slot[n] = 0;
        found = free_slot;
    }
    if (found >= 0) { *(long long *)(base + found * 128 + 120) = fk_live_now_ms(); }
    munmap(fk_gift_base[gh >> 1], (size_t)fk_gift_size[gh >> 1]);
    fk_gift_base[gh >> 1] = 0;
    return found;
}
static long long fk_roster_names(void) {
    long long gh = fk_gift_open("/fg-roster", 0, 0);
    if (gh == fk_nothing) { return 1; }
    char *base = (char *)fk_gift_base[gh >> 1] + 64;
    long long l = 1;
    long long k = FK_ROSTER_SLOTS - 1;
    while (k >= 0) {
        char *slot = base + k * 128;
        if (slot[0] != 0) {
            long long at = *(long long *)(slot + 120);
            long long row = fk_cons_val(fk_sbuf(slot, fk_cstrlen(slot)), fk_cons_val(at << 1, 1));
            l = fk_cons_val(row, l);
        }
        k = k - 1;
    }
    munmap(fk_gift_base[gh >> 1], (size_t)fk_gift_size[gh >> 1]);
    fk_gift_base[gh >> 1] = 0;
    return l;
}
/* the hottest defns of this process: top-n by heat, then one pass over the program text for line and column */
static long long fk_hot_pick_by(long long *ledger, long long want, long long *picked_j, long long *picked_h, long long *line_of, long long *col_of);
static long long fk_hot_pick(long long want, long long *picked_j, long long *picked_h, long long *line_of, long long *col_of) { return fk_hot_pick_by(fk_fn_heat, want, picked_j, picked_h, line_of, col_of); }
static long long fk_hot_pick_by(long long *ledger, long long want, long long *picked_j, long long *picked_h, long long *line_of, long long *col_of) {
    long long np = 0;
    long long j = 0;
    while (j < fk_fntop) {
        long long fx = fk_fnidx[j];
        if (fx >= 0 && fx < fk_fn_count && ledger != 0 && ledger[fx] > 0) {
            long long h = ledger[fx];
            long long k = np < want ? np : want - 1;
            if (np < want || h > picked_h[k]) {
                if (np < want) { np = np + 1; }
                while (k > 0 && picked_h[k - 1] < h) { picked_j[k] = picked_j[k - 1]; picked_h[k] = picked_h[k - 1]; k = k - 1; }
                picked_j[k] = j;
                picked_h[k] = h;
            }
        }
        j = j + 1;
    }
    long long q = 0;
    while (q < np) { line_of[q] = 0; col_of[q] = 0; q = q + 1; }
    long long pos = 0, line = 1, lastnl = -1;
    while (pos <= fk_slen) {
        q = 0;
        while (q < np) {
            if (fk_fnsym_s[picked_j[q]] == pos) { line_of[q] = line; col_of[q] = pos - lastnl; }
            q = q + 1;
        }
        if (pos < fk_slen && fk_srctext[pos] == FK_CH_LF) { line = line + 1; lastnl = pos; }
        pos = pos + 1;
    }
    return np;
}
static const char *fk_hot_unit_of(long long so) {
    const char *unit = fk_src_root_path;
    long long d = 0;
    while (d < fk_src_dep_count) {
        if (so >= fk_src_dep_text_off[d] && so < fk_src_dep_text_off[d] + fk_src_dep_text_len[d] && fk_src_dep_text_len[d] > 0) { unit = fk_src_dep_path[d]; }
        d = d + 1;
    }
    return unit;
}
/* ---- reading another kernel's store: its columns mapped read-only, every word read where it lives ----
 * A foreign word travels in this process as a plain int (cell_ref / cell_field answer it, cell_value
 * resolves it): small words as themselves, the far negatives (strings, floats, nothing, functions)
 * folded below -2^61 so no foreign word is ever mistaken for one of this process's own. */
#define FK_CELL_MAPS 16
#define FK_CELL_BIG (1LL << 61)
#define FK_CELL_BASE 9000000000000000000LL
struct fk_cell_map_s { long long pid; void *base[24]; long long size[24]; volatile long long *live; long long live_size; };
static struct fk_cell_map_s fk_cell_maps[FK_CELL_MAPS];
static int fk_cell_letter(char c) { int k = 0; while (fk_store_letters[k]) { if (fk_store_letters[k] == c) { return k; } k = k + 1; } return -1; }
static long long fk_cell_enc(long long raw) { if (raw <= -7000000000000000000LL) { return ((0 - (raw + FK_CELL_BASE)) - FK_CELL_BIG) << 1; } return raw << 1; }
static long long fk_cell_dec(long long v) { long long x = v >> 1; if (x <= 0 - (1LL << 60)) { return (0 - (x + FK_CELL_BIG)) - FK_CELL_BASE; } return x; }
static long long fk_cell_map_open(long long pid) {
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    long long s = 0;
    while (s < FK_CELL_MAPS && fk_cell_maps[s].pid != 0) { s = s + 1; }
    if (s == FK_CELL_MAPS) { return -1; }
    struct fk_cell_map_s *m = &fk_cell_maps[s];
    m->pid = pid > 0 ? pid : -1;   /* the field is one; a handle is marked mapped whatever pid was asked for */
    long long k = 0, mapped = 0;
    while (fk_store_letters[k]) {
        char nm[32];
        fk_field_name(fk_store_letters[k], nm);
        m->base[k] = 0; m->size[k] = 0;
        int fd = shm_open(nm, O_RDONLY, 0600);
        if (fd >= 0) {
            struct stat st;
            if (fstat(fd, &st) == 0 && st.st_size > 0) {
                void *p = mmap(0, (size_t)st.st_size, PROT_READ, MAP_SHARED, fd, 0);
                if (p != MAP_FAILED) { m->base[k] = p; m->size[k] = (long long)st.st_size; mapped = mapped + 1; }
            }
            close(fd);
        }
        k = k + 1;
    }
    char ln[32];
    fk_live_pid_name(pid, ln);
    m->live = 0; m->live_size = 0;
    int lfd = shm_open(ln, O_RDONLY, 0600);
    if (lfd >= 0) {
        struct stat lst;
        if (fstat(lfd, &lst) == 0 && lst.st_size >= 4096) {
            void *lp = mmap(0, (size_t)lst.st_size, PROT_READ, MAP_SHARED, lfd, 0);
            if (lp != MAP_FAILED) { m->live = (volatile long long *)lp; m->live_size = (long long)lst.st_size; }
        }
        close(lfd);
    }
    if (mapped == 0) { m->pid = 0; return -1; }
    return s;
#else
    (void)pid;
    return -1;
#endif
}
static void fk_cell_map_close(long long s) {
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    if (s < 0 || s >= FK_CELL_MAPS || fk_cell_maps[s].pid == 0) { return; }
    struct fk_cell_map_s *m = &fk_cell_maps[s];
    long long k = 0;
    while (k < 24) { if (m->base[k] != 0) { munmap(m->base[k], (size_t)m->size[k]); m->base[k] = 0; } k = k + 1; }
    if (m->live != 0) { munmap((void *)m->live, (size_t)m->live_size); m->live = 0; }
    m->pid = 0;
#else
    (void)s;
#endif
}
/* a column born after the map (the float pool at its first box, the strings at their first intern) opens on first touch */
static void fk_cell_col_open(struct fk_cell_map_s *m, int k) {
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    char nm[32];
    fk_field_name(fk_store_letters[k], nm);
    int fd = shm_open(nm, O_RDONLY, 0600);
    if (fd < 0) { return; }
    struct stat st;
    if (fstat(fd, &st) == 0 && st.st_size > 0) {
        void *p = mmap(0, (size_t)st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if (p != MAP_FAILED) { m->base[k] = p; m->size[k] = (long long)st.st_size; }
    }
    close(fd);
#else
    (void)m; (void)k;
#endif
}
static long long fk_cell_col(struct fk_cell_map_s *m, char c, long long idx, long long width, long long *out) {
    int k = fk_cell_letter(c);
    if (k < 0) { return 0; }
    if (m->base[k] == 0) { fk_cell_col_open(m, k); }
    if (m->base[k] == 0 || idx < 0 || (idx + 1) * width > m->size[k]) { return 0; }
    *out = *(long long *)((char *)m->base[k] + idx * width);
    return 1;
}
/* what a foreign raw word is: 0 int 1 list 2 node 3 string 4 float 5 nothing 6 function */
static long long fk_cell_kind_of(long long raw) {
    if (raw == fk_nothing) { return 5; }
    if ((raw & 1) == 0) { return 0; }
    if (raw <= fk_fbase - 3) { return 4; }
    if (raw <= fk_sbase - 1) { return 3; }
    if (raw >= fk_fnbase && raw < fk_fnbase + 8192) { return 6; }
    if (raw >= 1) { return 1; }
    return 2;
}
static long long fk_cell_field(long long s, long long raw, long long k) {
    if (s < 0 || s >= FK_CELL_MAPS || fk_cell_maps[s].pid == 0) { return fk_nothing; }
    struct fk_cell_map_s *m = &fk_cell_maps[s];
    long long kind = fk_cell_kind_of(raw);
    long long w = 0;
    if (kind == 2) {
        long long idx = fk_nidx(raw);
        if (k == 0) { return fk_cell_col(m, 'k', idx, 8, &w) ? (w << 1) : fk_nothing; }
        if (k == 1) { return fk_cell_col(m, 'c', idx, 8, &w) ? fk_cell_enc(w) : fk_nothing; }
        if (k == 2) { return fk_cell_col(m, 'i', idx, 8, &w) ? fk_cell_enc(w) : fk_nothing; }
        if (k == 3) { return fk_cell_col(m, 'v', idx, 8, &w) ? fk_cell_enc(w) : fk_nothing; }
        if (k >= 4 && k <= 7) { int c = fk_cell_letter('n'); if (c >= 0 && m->base[c] == 0) { fk_cell_col_open(m, c); } if (c < 0 || m->base[c] == 0 || (idx + 1) * 32 > m->size[c]) { return fk_nothing; } return (*(long long *)((char *)m->base[c] + idx * 32 + (k - 4) * 8)) << 1; }
        if (k == 8) { return fk_cell_col(m, 'f', idx, 8, &w) ? fk_cell_enc(w) : fk_nothing; }
        if (k == 9) { return fk_cell_col(m, 'l', idx, 8, &w) ? (w << 1) : fk_nothing; }
        if (k == 10) { return fk_cell_col(m, 'o', idx, 8, &w) ? (w << 1) : fk_nothing; }
        if (k == 11) { return fk_cell_col(m, 'a', idx, 8, &w) ? (w << 1) : fk_nothing; }
        return fk_nothing;
    }
    if (kind == 1) {
        long long p = raw >> 1;
        if (p < FK_PAIR_BASE) { return fk_nothing; }
        p = p - FK_PAIR_BASE;
        char hc = 'P';
        char tc = 'Q';
        if (k == 0) { return fk_cell_col(m, hc, p, 8, &w) ? fk_cell_enc(w) : fk_nothing; }
        if (k == 1) { return fk_cell_col(m, tc, p, 8, &w) ? fk_cell_enc(w) : fk_nothing; }
        return fk_nothing;
    }
    if (k == 0) { return kind << 1; }   /* any word answers its kind at field 0 when it has no fields */
    return fk_nothing;
}
static long long fk_cell_value(long long s, long long raw) {
    long long kind = fk_cell_kind_of(raw);
    if (kind == 0) { return raw; }
    if (kind == 5) { return fk_nothing; }
    if (s < 0 || s >= FK_CELL_MAPS || fk_cell_maps[s].pid == 0) { return fk_nothing; }
    struct fk_cell_map_s *m = &fk_cell_maps[s];
    if (kind == 3) {
        long long si = ((fk_sbase - raw - 1) >> 1) - FK_STR_BASE;
        if (si < 0) { return fk_nothing; }
        long long off = 0, len = 0;
        if (!fk_cell_col(m, 'O', si, 8, &off) || !fk_cell_col(m, 'L', si, 8, &len)) { return fk_nothing; }
        int sb = fk_cell_letter('s');
        if (m->base[sb] == 0) { fk_cell_col_open(m, sb); }
        if (m->base[sb] == 0 || off < 0 || len < 0 || off + len > m->size[sb]) { return fk_nothing; }
        return fk_sbuf((const char *)m->base[sb] + off, len);
    }
    if (kind == 4) {
        long long fi = ((fk_fbase - raw - 1) >> 1) - FK_FLT_BASE;
        if (fi < 0) { return fk_nothing; }
        long long bits = 0;
        if (!fk_cell_col(m, 'F', fi, 8, &bits)) { return fk_nothing; }
        double d;
        char *pd = (char *)&d; char *pb = (char *)&bits; int b = 0; while (b < 8) { pd[b] = pb[b]; b = b + 1; }
        return fk_fbox(d);
    }
    return fk_nothing;
}
/* a hot row as a cell: (heat name unit line col boxes unboxes) -- the defn's own source pointer and both float ledgers */
static long long fk_hot_row_cell(long long sj, long long heat, long long line, long long col) {
    long long so = fk_fnsym_s[sj];
    long long fx = fk_fnidx[sj];
    const char *unit = fk_hot_unit_of(so);
    long long boxes = (fk_fn_fbox != 0 && fx >= 0 && fx < fk_fn_count) ? fk_fn_fbox[fx] : 0;
    long long unboxes = (fk_fn_unbox != 0 && fx >= 0 && fx < fk_fn_count) ? fk_fn_unbox[fx] : 0;
    long long native = (fk_fn_native != 0 && fx >= 0 && fx < fk_fn_count) ? fk_fn_native[fx] : 0;
    long long mints = (fk_fn_mint != 0 && fx >= 0 && fx < fk_fn_count) ? fk_fn_mint[fx] : 0;
    long long folds = (fk_fn_inram != 0 && fx >= 0 && fx < fk_fn_count) ? fk_fn_inram[fx] : 0;
    return fk_cons_val(heat << 1, fk_cons_val(fk_sbuf(fk_srctext + so, fk_fnsym_n[sj]), fk_cons_val(fk_sbuf(unit, fk_cstrlen(unit)), fk_cons_val(line << 1, fk_cons_val(col << 1, fk_cons_val(boxes << 1, fk_cons_val(unboxes << 1, fk_cons_val(native << 1, fk_cons_val(mints << 1, fk_cons_val(folds << 1, 1))))))))));
}
/* The rank arrays were sixty-four wide on the stack, and a want past that was
 * silently cut to sixty-four -- ask for nine hundred, receive sixty-four, with
 * nothing said. This body closed that family (silent partials GROW, walls
 * refuse loudly), and this door was still in it. Witnessed 2026-09-09: a glass
 * carrying 3248 recipes and three to seven MILLION dispatches a frame could
 * only ever show its top sixty-four, whose rates summed to under a fortieth of
 * the frame, so the question "where does the frame go" had no door.
 * The honest bound is how many recipes there ARE: you cannot rank more defns
 * than exist. Clamping to that says nothing false, and the arrays are taken
 * from the heap at the size actually asked for. */
static long long fk_hot_rows_by(long long *ledger, long long want) {
    if (want <= 0) { return 1; }
    if (want > fk_fntop) { want = fk_fntop; }
    if (want <= 0) { return 1; }
    long long *pj = (long long *)malloc((size_t)want * 8), *ph = (long long *)malloc((size_t)want * 8);
    long long *ln = (long long *)malloc((size_t)want * 8), *cl = (long long *)malloc((size_t)want * 8);
    if (pj == 0 || ph == 0 || ln == 0 || cl == 0) { free(pj); free(ph); free(ln); free(cl); return 1; }
    long long np = fk_hot_pick_by(ledger, want, pj, ph, ln, cl);
    long long l = 1;
    long long q = np - 1;
    while (q >= 0) { l = fk_cons_val(fk_hot_row_cell(pj[q], ph[q], ln[q], cl[q]), l); q = q - 1; }
    free(pj); free(ph); free(ln); free(cl);
    return l;
}
/* another kernel's page: header words plus what the reader derives from the arms (dispatches, hottest, distinct) */
static long long fk_live_read_page(const char *name, long long *out) {
    long long gh = fk_gift_open(name, 0, 0);
    if (gh == fk_nothing) { return -1; }
    long long sz = fk_gift_size[gh >> 1];
    volatile long long *w = (volatile long long *)fk_gift_base[gh >> 1] + 2;
    long long k = 0;
    while (k < FK_LIVE_WORDS) { out[k] = w[k]; k = k + 1; }
    if (sz >= FK_LIVE_ARMS_OFF + FK_OPCODE_ARM_CAP * 8 && out[28] == FK_LIVE_VERSION) {
        long long *arms = (long long *)((char *)fk_gift_base[gh >> 1] + FK_LIVE_ARMS_OFF);
        long long sum = 0, distinct = 0, hot = 0, hotc = 0, u = 1;
        while (u < FK_OPCODE_ARM_CAP) { long long a = arms[u]; if (a > 0) { sum = sum + a; distinct = distinct + 1; if (a > hotc) { hotc = a; hot = u; } } u = u + 1; }
        out[4] = sum; out[16] = hot; out[17] = hotc; out[20] = distinct;
    }
    munmap(fk_gift_base[gh >> 1], (size_t)fk_gift_size[gh >> 1]);
    fk_gift_base[gh >> 1] = 0;
    return FK_LIVE_WORDS;
}
/* the hottest defns of ANY kernel by one of its page ledgers: (heat name unit line col boxes unboxes) cells */
/* Same heal as fk_hot_rows_by: the wall was sixty-four and it cut in silence.
 * Here the honest bound lives in the page itself -- w[29], how many defns that
 * kernel carries -- so the clamp waits until the page is mapped and then says
 * only what is true. */
static long long fk_page_rows(long long pid, int ledger, long long want) {
    if (want <= 0) { return 1; }
    char name[32];
    fk_live_pid_name(pid, name);
    long long gh = fk_gift_open(name, 0, 0);
    if (gh == fk_nothing) { return 1; }
    char *base = (char *)fk_gift_base[gh >> 1];
    long long sz = fk_gift_size[gh >> 1];
    volatile long long *w = (volatile long long *)base + 2;
    if (sz < FK_LIVE_INRAM_OFF + FK_LIVE_FNS * 8 || w[28] != FK_LIVE_VERSION) { munmap(base, (size_t)sz); fk_gift_base[gh >> 1] = 0; return 1; }
    long long *heat = (long long *)(base + FK_LIVE_HEAT_OFF), *fbox = (long long *)(base + FK_LIVE_FBOX_OFF), *unbox = (long long *)(base + FK_LIVE_UNBOX_OFF), *meta = (long long *)(base + FK_LIVE_META_OFF), *native = (long long *)(base + FK_LIVE_NATIVE_OFF);
    char *blob = base + FK_LIVE_BLOB_OFF;
    long long *mintl = (long long *)(base + FK_LIVE_MINT_OFF);
    long long *inraml = (long long *)(base + FK_LIVE_INRAM_OFF);
    long long *led = ledger == 1 ? fbox : (ledger == 2 ? mintl : (ledger == 3 ? inraml : heat));
    long long count = w[29] < FK_LIVE_FNS ? w[29] : FK_LIVE_FNS;
    if (want > count) { want = count; }
    if (want <= 0) { munmap(base, (size_t)sz); fk_gift_base[gh >> 1] = 0; return 1; }
    long long *pj = (long long *)malloc((size_t)want * 8), *ph = (long long *)malloc((size_t)want * 8);
    if (pj == 0 || ph == 0) { free(pj); free(ph); munmap(base, (size_t)sz); fk_gift_base[gh >> 1] = 0; return 1; }
    long long np = 0, fx = 0;
    while (fx < count) {
        long long h = led[fx];
        if (h > 0 && meta[fx * 5 + 1] > 0) {
            long long k = np < want ? np : want - 1;
            if (np < want || h > ph[k]) {
                if (np < want) { np = np + 1; }
                while (k > 0 && ph[k - 1] < h) { pj[k] = pj[k - 1]; ph[k] = ph[k - 1]; k = k - 1; }
                pj[k] = fx; ph[k] = h;
            }
        }
        fx = fx + 1;
    }
    long long l = 1, q = np - 1;
    while (q >= 0) {
        long long f = pj[q];
        long long *m = meta + f * 5;
        long long row = fk_cons_val(heat[f] << 1, fk_cons_val(fk_sbuf(blob + m[0], m[1]), fk_cons_val(fk_sbuf(blob + m[0] + m[1], m[2]), fk_cons_val(m[3] << 1, fk_cons_val(m[4] << 1, fk_cons_val(fbox[f] << 1, fk_cons_val(unbox[f] << 1, fk_cons_val(native[f] << 1, fk_cons_val(mintl[f] << 1, fk_cons_val(inraml[f] << 1, 1))))))))));
        l = fk_cons_val(row, l);
        q = q - 1;
    }
    free(pj); free(ph);
    munmap(base, (size_t)sz);
    fk_gift_base[gh >> 1] = 0;
    return l;
}
/* ---- the float-NodeID surface (tags 195 / 201) ---- */
/* x^y with the integer part of y squared out exactly and the fractional part
 * through exp(yf*log(x)): pow(2,10) is 1024.0 to the bit, pow(2,0.5) is sqrt 2
 * within the same ULP honesty the other transcendentals carry. */
static double fk_pow_d(double x, double y) {
    if (y == 0.0 || x == 1.0) { return 1.0; }
    if (x != x || y != y) { return 0.0 / 0.0; }
    if (x == 0.0) { return (y < 0.0) ? (1.0 / 0.0) : 0.0; }
    double ay = (y < 0.0) ? 0.0 - y : y;
    double yi = (double)(long long)ay;
    double yf = ay - yi;
    if (x < 0.0 && yf != 0.0) { return 0.0 / 0.0; }
    double ax = (x < 0.0) ? 0.0 - x : x;
    double frac = (yf == 0.0) ? 1.0 : fk_exp_d(yf * fk_log_d(ax));
    double acc = 1.0;
    double base = x;
    long long n = (long long)yi;
    while (n > 0) {
        if (n & 1) { acc = acc * base; }
        base = base * base;
        n = n >> 1;
    }
    double r = acc * frac;
    return (y < 0.0) ? 1.0 / r : r;
}
/* intern a float32 trivial (type 6): the value rounds through float, NaN folds
 * to one quiet NaN and -0.0 to +0.0 as the type-7 door does; nid[3] carries the
 * IEEE bits so a hand-built (make_nodeid 1 1 6 bits) leaf and an interned one
 * read the same float_value. */
static long long fk_intern_float32_node(double d) {
    float f = (float)d;
    unsigned int bits;
    if (f != f) { bits = 0x7fc00000u; memcpy(&f, &bits, 4); }
    else if (f == 0.0f) { f = 0.0f; }
    memcpy(&bits, &f, 4);
    long long h = fk_intern_key_trivial(6, (long long)bits);
    if (fk_field_on) { return fk_field_intern_node(1, 6, fk_fbox((double)f), (long long)bits, 0, h); }
    if (fk_np + 1 >= fk_node_cap) { fk_nodes_grow(); }
    long long slot = h & (fk_intern_hash_cap - 1);
    while (fk_intern_tab[slot]) {
        long long ix = fk_intern_tab[slot];
        if (fk_nkind[ix] == 1 && fk_nid[ix][2] == 6 && fk_nid[ix][3] == (long long)bits) { return fk_nbox(ix); }
        slot = (slot + 1) & (fk_intern_hash_cap - 1);
    }
    long long fb = fk_fbox((double)f);
    fk_np = fk_np + 1; fk_mint_total = fk_mint_total + 1; if (fk_fn_mint != 0 && fk_cur_fn > 0 && fk_cur_fn < fk_fn_capacity) { fk_fn_mint[fk_cur_fn] = fk_fn_mint[fk_cur_fn] + 1; }
    fk_nkind[fk_np] = 1;
    fk_nval[fk_np] = fb;
    fk_nkids[fk_np] = 1;
    fk_ncat[fk_np] = 0;
    fk_nid[fk_np][0] = 1;
    fk_nid[fk_np][1] = 1;
    fk_nid[fk_np][2] = 6;
    fk_nid[fk_np][3] = (long long)bits;
    fk_intern_tab[slot] = fk_np;
    fk_nhash_memo[fk_np] = h;
    return fk_nbox(fk_np);
}
/* mode 0 float_value: a type-6/7 leaf's IEEE value as a boxed double (an interned
 * leaf holds it in fk_nval; a hand-built type-6 leaf holds the bits in nid[3]; a
 * hand-built type-7 leaf names a pool slot) -- nothing for any other value, the
 * way the witnesses refuse a non-float NodeID. mode 1 make_float32, mode 2
 * make_float64, mode 3 math_pi. */
static long long fk_float_leaf(long long mode, long long x) {
    if (mode == 1) { return fk_intern_float32_node(fk_num(x)); }
    if (mode == 2) { return fk_intern_float_node(fk_num(x)); }
    if (mode == 3) { return fk_fbox(3.141592653589793); }
    if (mode != 0 || x >= 0) { return fk_nothing; }
    long long ni = fk_nidx(x);
    if (ni < 1 || ni > fk_np) { return fk_nothing; }
    long long ty = fk_nid[ni][2];
    if (ty != 6 && ty != 7) { return fk_nothing; }
    if (fk_nkind[ni] == 1) { return fk_isf(fk_nval[ni]) ? fk_nval[ni] : fk_fbox(fk_num(fk_nval[ni])); }
    if (fk_nkind[ni] != 3) { return fk_nothing; }
    if (ty == 6) {
        unsigned int bits = (unsigned int)fk_nid[ni][3];
        float f;
        memcpy(&f, &bits, 4);
        return fk_fbox((double)f);
    }
    long long fi = fk_nid[ni][3];
    if (fi < 0 || fi > fk_fp) { return fk_nothing; }
    return fk_fbox(FK_FV(fi));
}
/* ---- the binary form (FORMBIN2) on the fourth arm: modes 4-8 of the leaf door (tag 201) ----
 * value_kind (4), recipe_to_bytes (5), bytes_to_recipe (6), read_form_binary (7) and
 * write_form_binary (8, x = (cons path node)) are rewrite rows over the one mode door the
 * float surface already opened -- the AST tag space is full and 150 is the native-surface
 * probe. A Form recipe could not carry these: Rust's read_file is fs::read_to_string (UTF-8
 * only; a .fkb is not text), and a defn of a sibling native's name overrides that native on
 * Go and Rust (release ledger R80), so a recipe preluded from channel.fk would have replaced
 * the witnesses' own codec on their arms. The wire is the siblings' exactly (form-kernel-go
 * main.go serializeArtifact / deserializeArtifact): "FORMBIN2", u32 BE string count, each
 * string as u32 BE length + UTF-8 bytes, then the tree -- 0 leaf (pkg level type inst as
 * u32 BE; a trivial string's inst is its local table index), 1 composite (category, u32
 * count, children), 2 float64 (8 bytes LE), 3 int64 (8 bytes LE). The same bounds and the
 * same refusals by name; a FORMBIN1 artifact reads as the siblings read it. On this arm a
 * composite's kid may be a raw word (42, "ab", 3.5) where the siblings only hold NodeIDs;
 * the wire has no raw-word lane, so a raw kid crosses as its trivial node and reads back as
 * the interned node (fk_neq compares those by value). A list, a function or a record kid
 * refuses the whole give: recipe_to_bytes answers nothing, write_form_binary -1. */
static long long *fk_fb_sw;          /* collected string words, in first-visit order */
static long long fk_fb_sn, fk_fb_scap;
static long long *fk_fb_htab;        /* open addressing over content: index+1, 0 = empty */
static long long fk_fb_hcap;
static int fk_fb_refused;
static const char *fk_fb_err;        /* the last refusal, the siblings' words */
static unsigned long long fk_fb_hash_bytes(const char *b, long long n) {
    unsigned long long h = 1469598103934665603ULL;
    long long k = 0;
    while (k < n) { h = (h ^ (unsigned char)b[k]) * 1099511628211ULL; k = k + 1; }
    return h;
}
static long long fk_fb_str_index(long long v, int add) {
    long long si = fk_stri(v);
    if (si < 0 || !FK_SOK(si)) { fk_fb_refused = 1; return 0; }
    long long n = FK_SLEN(si);
    if (fk_fb_hcap == 0) {
        fk_fb_hcap = 1024;
        fk_fb_htab = (long long *)calloc(fk_fb_hcap, sizeof(long long));
        fk_fb_scap = 512;
        fk_fb_sw = (long long *)calloc(fk_fb_scap, sizeof(long long));
    }
    if (add && fk_fb_sn * 2 >= fk_fb_hcap) {
        long long oc = fk_fb_hcap;
        long long *ot = fk_fb_htab;
        fk_fb_hcap = oc * 2;
        fk_fb_htab = (long long *)calloc(fk_fb_hcap, sizeof(long long));
        long long j = 0;
        while (j < oc) {
            if (ot[j]) {
                long long wi = fk_stri(fk_fb_sw[ot[j] - 1]);
                unsigned long long hh = fk_fb_hash_bytes(FK_SBYTES(wi), FK_SLEN(wi));
                long long s = (long long)(hh & (unsigned long long)(fk_fb_hcap - 1));
                while (fk_fb_htab[s]) { s = (s + 1) & (fk_fb_hcap - 1); }
                fk_fb_htab[s] = ot[j];
            }
            j = j + 1;
        }
        free(ot);
    }
    unsigned long long h = fk_fb_hash_bytes(FK_SBYTES(si), n);
    long long slot = (long long)(h & (unsigned long long)(fk_fb_hcap - 1));
    while (fk_fb_htab[slot]) {
        long long ix = fk_fb_htab[slot] - 1;
        long long oi = fk_stri(fk_fb_sw[ix]);
        if (FK_SLEN(oi) == n && memcmp(FK_SBYTES(oi), FK_SBYTES(si), (size_t)n) == 0) { return ix; }
        slot = (slot + 1) & (fk_fb_hcap - 1);
    }
    if (!add) { fk_fb_refused = 1; return 0; }
    if (fk_fb_sn >= fk_fb_scap) {
        fk_fb_scap = fk_fb_scap * 2;
        fk_fb_sw = (long long *)realloc(fk_fb_sw, (size_t)fk_fb_scap * sizeof(long long));
    }
    fk_fb_sw[fk_fb_sn] = v;
    fk_fb_htab[slot] = fk_fb_sn + 1;
    fk_fb_sn = fk_fb_sn + 1;
    return fk_fb_sn - 1;
}
/* the string table in the siblings' order: category before children, depth first, first visit */
static void fk_fb_collect(long long v) {
    if (fk_is_str(v)) { fk_fb_str_index(v, 1); return; }
    if (v < 0 && (v & 1) && !fk_isf(v) && !fk_is_fnval(v)) {
        long long ni = fk_nidx(v);
        if (ni < 1 || ni > fk_np) { return; }
        if (fk_nkind[ni] == 2) {
            fk_fb_collect(fk_ncat[ni]);
            long long q = fk_nkids[ni] >> 1;
            while (q >= 1 && FK_POK(q)) { fk_fb_collect(FK_HH(q)); q = FK_HT(q) >> 1; }
            return;
        }
        if (fk_nkind[ni] == 1 && fk_nid[ni][2] == 2) { fk_fb_str_index(fk_nval[ni], 1); }
    }
}
static void fk_fb_reserve(long long n) {
    while (fk_sbp + n > fk_scap_b) {
        fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b, fk_scap_b * 2, FK_STORE_STR_BYTES, 0);
        fk_scap_b = fk_scap_b * 2;
        fk_sb_check();
    }
}
static void fk_fb_u32(long long v) {
    unsigned char b[4];
    b[0] = (unsigned char)(v >> 24); b[1] = (unsigned char)(v >> 16); b[2] = (unsigned char)(v >> 8); b[3] = (unsigned char)v;
    fk_sappend((const char *)b, 4);
}
static void fk_fb_i64le(long long v) {
    unsigned long long u = (unsigned long long)v;
    unsigned char b[8];
    long long k = 0;
    while (k < 8) { b[k] = (unsigned char)(u >> (8 * k)); k = k + 1; }
    fk_sappend((const char *)b, 8);
}
static void fk_fb_f64le(double d) {
    unsigned long long u;
    memcpy(&u, &d, 8);
    fk_fb_i64le((long long)u);
}
static void fk_fb_leaf(long long pkg, long long level, long long ty, long long inst) {
    fk_fb_u32(0); fk_fb_u32(pkg); fk_fb_u32(level); fk_fb_u32(ty); fk_fb_u32(inst);
}
/* an int as the siblings write it: inside int32 it is a type-1 leaf whose inst IS the value
 * (Go internTrivialInt), beyond int32 it is a tag-3 int64 (the i64 table's value on the wire) */
static void fk_fb_int(long long n) {
    if (n >= -2147483648LL && n <= 2147483647LL) { fk_fb_leaf(1, 1, 1, (long long)(unsigned int)(int)n); return; }
    fk_fb_u32(3);
    fk_fb_i64le(n);
}
static void fk_fb_emit(long long v) {
    if (fk_fb_refused) { return; }
    if (v == fk_nothing) { fk_fb_leaf(1, 1, 4, 0); return; }
    if (v == (0 - 9223372036854775807LL)) { fk_fb_leaf(1, 1, 3, 1); return; }
    if (v == (0 - 9223372036854775805LL)) { fk_fb_leaf(1, 1, 3, 0); return; }
    if (fk_isf(v)) { fk_fb_u32(2); fk_fb_f64le(fk_num(v)); return; }
    if (fk_is_str(v)) { fk_fb_leaf(1, 1, 2, fk_fb_str_index(v, 0)); return; }
    if (fk_is_fnval(v) || fk_isrec(v)) { fk_fb_refused = 1; return; }
    if (v < 0 && (v & 1)) {
        long long ni = fk_nidx(v);
        if (ni < 1 || ni > fk_np) { fk_fb_refused = 1; return; }
        if (fk_nkind[ni] == 2) {
            fk_fb_u32(1);
            fk_fb_emit(fk_ncat[ni]);
            long long count = 0;
            long long q = fk_nkids[ni] >> 1;
            while (q >= 1 && FK_POK(q)) { count = count + 1; q = FK_HT(q) >> 1; }
            fk_fb_u32(count);
            q = fk_nkids[ni] >> 1;
            while (q >= 1 && FK_POK(q)) { fk_fb_emit(FK_HH(q)); q = FK_HT(q) >> 1; }
            return;
        }
        if (fk_nkind[ni] == 1) {
            long long ty = fk_nid[ni][2];
            if (ty == 1) { fk_fb_int(fk_nval[ni] >> 1); return; }
            if (ty == 2) { fk_fb_leaf(1, 1, 2, fk_fb_str_index(fk_nval[ni], 0)); return; }
            if (ty == 7) { fk_fb_u32(2); fk_fb_f64le(fk_num(fk_nval[ni])); return; }
            if (ty == 3) { fk_fb_leaf(1, 1, 3, fk_nid[ni][3]); return; }
            if (ty == 6) {
                float f = (float)fk_num(fk_nval[ni]);
                unsigned int bits;
                memcpy(&bits, &f, 4);
                fk_fb_leaf(1, 1, 6, (long long)bits);
                return;
            }
            fk_fb_leaf(fk_nid[ni][0], fk_nid[ni][1], ty, fk_nid[ni][3]);
            return;
        }
        fk_fb_leaf(fk_nid[ni][0], fk_nid[ni][1], fk_nid[ni][2], fk_nid[ni][3]);
        return;
    }
    if ((v & 1) == 0) { fk_fb_int(v >> 1); return; }
    fk_fb_refused = 1;
}
/* the whole artifact into the string scratch at *start, *n bytes; 1 when it stands, 0 refused
 * (the scratch is released either way once the caller has taken the bytes) */
static long long fk_fb_serialize(long long root, long long *start, long long *n) {
    fk_fb_sn = 0;
    long long hz = 0;
    while (hz < fk_fb_hcap) { fk_fb_htab[hz] = 0; hz = hz + 1; }
    fk_fb_refused = 0;
    fk_fb_collect(root);
    if (fk_fb_refused) { return 0; }
    fk_sinit();
    *start = fk_sbp;
    fk_sappend("FORMBIN2", 8);
    fk_fb_u32(fk_fb_sn);
    long long k = 0;
    while (k < fk_fb_sn) {
        long long si = fk_stri(fk_fb_sw[k]);
        long long len = FK_SLEN(si);
        fk_fb_u32(len);
        fk_fb_reserve(len);
        fk_sappend(FK_SBYTES(si), len);
        k = k + 1;
    }
    fk_fb_emit(root);
    *n = fk_sbp - *start;
    if (fk_fb_refused) { fk_sbp = *start; return 0; }
    return 1;
}
/* ---- the reader: the siblings' bounds and refusals, by their words ---- */
static const unsigned char *fk_fb_b;
static long long fk_fb_n, fk_fb_pos, fk_fb_count;
static long long *fk_fb_tab;         /* the artifact's string table as string words */
static long long fk_fb_tabn;
static int fk_fb_bad;
static long long fk_fb_get_u32(void) {
    if (fk_fb_bad) { return 0; }
    if (fk_fb_pos + 4 > fk_fb_n) { fk_fb_bad = 1; fk_fb_err = "form binary: truncated u32"; return 0; }
    long long v = ((long long)fk_fb_b[fk_fb_pos] << 24) | ((long long)fk_fb_b[fk_fb_pos + 1] << 16) |
                  ((long long)fk_fb_b[fk_fb_pos + 2] << 8) | (long long)fk_fb_b[fk_fb_pos + 3];
    fk_fb_pos = fk_fb_pos + 4;
    return v;
}
static long long fk_fb_get_i64le(const char *what) {
    if (fk_fb_bad) { return 0; }
    if (fk_fb_pos + 8 > fk_fb_n) { fk_fb_bad = 1; fk_fb_err = what; return 0; }
    unsigned long long u = 0;
    long long k = 0;
    while (k < 8) { u = u | ((unsigned long long)fk_fb_b[fk_fb_pos + k] << (8 * k)); k = k + 1; }
    fk_fb_pos = fk_fb_pos + 8;
    return (long long)u;
}
static int fk_fb_utf8_ok(const unsigned char *b, long long n) {
    long long i = 0;
    while (i < n) {
        unsigned char c = b[i];
        long long extra = 0;
        if (c < 0x80) { i = i + 1; continue; }
        if (c >= 0xC2 && c <= 0xDF) { extra = 1; }
        else if (c >= 0xE0 && c <= 0xEF) { extra = 2; }
        else if (c >= 0xF0 && c <= 0xF4) { extra = 3; }
        else { return 0; }
        if (i + extra > n - 1) { return 0; }
        long long k = 1;
        while (k <= extra) { if ((b[i + k] & 0xC0) != 0x80) { return 0; } k = k + 1; }
        if (c == 0xE0 && b[i + 1] < 0xA0) { return 0; }
        if (c == 0xED && b[i + 1] > 0x9F) { return 0; }
        if (c == 0xF0 && b[i + 1] < 0x90) { return 0; }
        if (c == 0xF4 && b[i + 1] > 0x8F) { return 0; }
        i = i + extra + 1;
    }
    return 1;
}
static long long fk_fb_string_leaf(long long inst) {
    if (inst < 0 || inst >= fk_fb_tabn) { fk_fb_bad = 1; fk_fb_err = "form binary: bad string index"; return fk_nothing; }
    return fk_intern_str_node(fk_fb_tab[inst]);
}
/* a leaf as the siblings read it: a trivial string by its table index, a trivial int32 by its
 * inst (sign-extended, Go trivialValue TrivInt), a trivial bool by inst != 0, a trivial null
 * as nothing; every other coordinate stays the coordinate it is */
static long long fk_fb_leaf_node(long long pkg, long long level, long long ty, long long inst, int catpos) {
    if (level == 1 && ty == 2) {
        /* in category position a string reads back as the word itself: on this arm a
         * category is the name `bp` answers (49 cells intern over (bp "X"), the word), and
         * a composite over the word is not the cell interned over the string node; in child
         * position it is the trivial node, the way intern_trivial_string made it */
        if (catpos) {
            if (inst < 0 || inst >= fk_fb_tabn) { fk_fb_bad = 1; fk_fb_err = "form binary: bad string index"; return fk_nothing; }
            return fk_fb_tab[inst];
        }
        return fk_fb_string_leaf(inst);
    }
    if (level == 1 && ty == 1) { return fk_intern_int_node(((long long)(int)(unsigned int)inst) << 1); }
    if (level == 1 && ty == 3) { return fk_intern_bool_node(inst != 0 ? 2 : 0); }
    if (level == 1 && ty == 4) { return fk_nothing; }
    return fk_make_nodeid(pkg, level, ty, inst);
}
static long long fk_fb_kids_list(long long *kids, long long count) {
    long long lst = 1;
    long long k = count;
    while (k > 0) { k = k - 1; lst = fk_cons_val(kids[k], lst); }
    return lst;
}
static long long fk_fb_enter(long long depth) {
    if (depth > 256) { fk_fb_bad = 1; fk_fb_err = "form binary: maximum node depth exceeded"; return 0; }
    fk_fb_count = fk_fb_count + 1;
    if (fk_fb_count > 1000000) { fk_fb_bad = 1; fk_fb_err = "form binary: maximum node count exceeded"; return 0; }
    return 1;
}
static long long fk_fb_read_node(long long depth, int catpos) {
    if (fk_fb_bad || !fk_fb_enter(depth)) { return fk_nothing; }
    long long tag = fk_fb_get_u32();
    if (fk_fb_bad) { return fk_nothing; }
    if (tag == 2) {
        long long bits = fk_fb_get_i64le("form binary: truncated float64");
        if (fk_fb_bad) { return fk_nothing; }
        double d;
        memcpy(&d, &bits, 8);
        return fk_intern_float_node(d);
    }
    if (tag == 3) {
        long long v = fk_fb_get_i64le("form binary: truncated int64");
        if (fk_fb_bad) { return fk_nothing; }
        return fk_intern_int_node(v << 1);
    }
    if (tag == 0) {
        long long pkg = fk_fb_get_u32();
        long long level = fk_fb_get_u32();
        long long ty = fk_fb_get_u32();
        long long inst = fk_fb_get_u32();
        if (fk_fb_bad) { return fk_nothing; }
        return fk_fb_leaf_node(pkg, level, ty, inst, catpos);
    }
    if (tag == 1) {
        long long cat = fk_fb_read_node(depth + 1, 1);
        if (fk_fb_bad) { return fk_nothing; }
        long long count = fk_fb_get_u32();
        if (fk_fb_bad) { return fk_nothing; }
        if (count > 262144) { fk_fb_bad = 1; fk_fb_err = "form binary: maximum child count exceeded"; return fk_nothing; }
        long long *kids = (long long *)calloc((size_t)(count > 0 ? count : 1), sizeof(long long));
        long long k = 0;
        while (k < count && !fk_fb_bad) { kids[k] = fk_fb_read_node(depth + 1, 0); k = k + 1; }
        long long out = fk_nothing;
        if (!fk_fb_bad) { out = fk_intern_composite(cat, fk_fb_kids_list(kids, count)); }
        free(kids);
        return out;
    }
    fk_fb_bad = 1;
    fk_fb_err = "form binary: unknown node tag";
    return fk_nothing;
}
/* FORMBIN1: pkg level type inst count, a string category or leaf by its table index */
static long long fk_fb_read_node_v1(long long depth) {
    if (fk_fb_bad || !fk_fb_enter(depth)) { return fk_nothing; }
    long long pkg = fk_fb_get_u32();
    long long level = fk_fb_get_u32();
    long long ty = fk_fb_get_u32();
    long long inst = fk_fb_get_u32();
    long long count = fk_fb_get_u32();
    if (fk_fb_bad) { return fk_nothing; }
    if (count > 262144) { fk_fb_bad = 1; fk_fb_err = "form binary: maximum child count exceeded"; return fk_nothing; }
    if (count == 0) { return fk_fb_leaf_node(pkg, level, ty, inst, 0); }
    long long cat = fk_fb_leaf_node(pkg, level, ty, inst, 1);
    if (fk_fb_bad) { return fk_nothing; }
    long long *kids = (long long *)calloc((size_t)count, sizeof(long long));
    long long k = 0;
    while (k < count && !fk_fb_bad) { kids[k] = fk_fb_read_node_v1(depth + 1); k = k + 1; }
    long long out = fk_nothing;
    if (!fk_fb_bad) { out = fk_intern_composite(cat, fk_fb_kids_list(kids, count)); }
    free(kids);
    return out;
}
static long long fk_fb_deserialize(const unsigned char *b, long long n) {
    fk_fb_b = b; fk_fb_n = n; fk_fb_pos = 0; fk_fb_count = 0; fk_fb_bad = 0; fk_fb_err = 0;
    if (n > (64LL << 20)) { fk_fb_err = "form binary: maximum artifact size exceeded"; return fk_nothing; }
    int v1 = n >= 8 && memcmp(b, "FORMBIN1", 8) == 0;
    int v2 = n >= 8 && memcmp(b, "FORMBIN2", 8) == 0;
    if (!v1 && !v2) { fk_fb_err = "form binary: bad magic"; return fk_nothing; }
    fk_fb_pos = 8;
    long long count = fk_fb_get_u32();
    if (fk_fb_bad) { return fk_nothing; }
    if (count > 262144) { fk_fb_err = "form binary: maximum string count exceeded"; return fk_nothing; }
    if (fk_fb_tab == 0 || fk_fb_tabn < count) {
        free(fk_fb_tab);
        fk_fb_tab = (long long *)calloc((size_t)(count > 0 ? count : 1), sizeof(long long));
    }
    fk_fb_tabn = count;
    long long total = 0;
    long long i = 0;
    while (i < count) {
        long long len = fk_fb_get_u32();
        if (fk_fb_bad) { return fk_nothing; }
        total = total + len;
        if (total > (32LL << 20)) { fk_fb_err = "form binary: maximum string bytes exceeded"; return fk_nothing; }
        if (len > n - fk_fb_pos) { fk_fb_err = "form binary: truncated string"; return fk_nothing; }
        if (!fk_fb_utf8_ok(b + fk_fb_pos, len)) { fk_fb_err = "form binary: invalid utf8"; return fk_nothing; }
        fk_fb_tab[i] = fk_sbuf((const char *)(b + fk_fb_pos), len);
        fk_fb_pos = fk_fb_pos + len;
        i = i + 1;
    }
    long long root = v1 ? fk_fb_read_node_v1(0) : fk_fb_read_node(0, 0);
    if (fk_fb_bad) { return fk_nothing; }
    if (fk_fb_pos != n) { fk_fb_err = "form binary: trailing bytes"; return fk_nothing; }
    return root;
}
static long long fk_value_kind(long long v) {
    const char *k = "unknown";
    if (v == fk_nothing) { k = "null"; }
    else if (v == (0 - 9223372036854775807LL) || v == (0 - 9223372036854775805LL)) { k = "bool"; }
    else if (fk_isf(v)) { k = "float"; }
    else if (fk_is_str(v)) { k = "string"; }
    else if (fk_is_fnval(v)) { k = "closure"; }
    else if (fk_isrec(v)) { k = "record"; }
    else if (v < 0 && (v & 1)) { long long ni = fk_nidx(v); if (ni >= 1 && ni <= fk_np) { k = "node_id"; } }
    else if (v == 1 || ((v & 1) && v > 0 && FK_POK(v >> 1))) { k = "list"; }
    else if ((v & 1) == 0) { k = "int"; }
    long long kn = 0;
    while (k[kn]) { kn = kn + 1; }
    return fk_sbuf(k, kn);
}
static long long fk_fb_door(long long mode, long long x) {
    if (mode == 4) { return fk_value_kind(x); }
    if (mode == 5) {
        long long start = 0, n = 0;
        if (!fk_fb_serialize(x, &start, &n)) { return fk_nothing; }
        long long lst = 1;
        long long k = n;
        while (k > 0) { k = k - 1; lst = fk_cons_val(((long long)(unsigned char)fk_sb[start + k]) << 1, lst); }
        fk_sbp = start;
        return lst;
    }
    if (mode == 6) {
        long long count = 0;
        long long q = x >> 1;
        if ((x & 1) == 0) { return fk_nothing; }
        while (q >= 1 && FK_POK(q)) { count = count + 1; q = FK_HT(q) >> 1; }
        unsigned char *buf = (unsigned char *)calloc((size_t)(count > 0 ? count : 1), 1);
        q = x >> 1;
        long long k = 0;
        while (q >= 1 && FK_POK(q)) { buf[k] = (unsigned char)(FK_HH(q) >> 1); k = k + 1; q = FK_HT(q) >> 1; }
        long long out = fk_fb_deserialize(buf, count);
        free(buf);
        return out;
    }
    if (mode == 7) {
        static char p[FK_PATH_CAP];
        fk_cstr(x, p, FK_PATH_CAP);
        int fd = open(p, O_RDBIN);
        if (fd < 0) { fk_fb_err = "form binary: no such file"; return fk_nothing; }
        long long cap = 65536, n = 0;
        unsigned char *buf = (unsigned char *)malloc((size_t)cap);
        for (;;) {
            if (n + 65536 > cap) { cap = cap * 2; buf = (unsigned char *)realloc(buf, (size_t)cap); }
            long long got = read(fd, buf + n, 65536);
            if (got <= 0) { break; }
            n = n + got;
        }
        close(fd);
        long long out = fk_fb_deserialize(buf, n);
        free(buf);
        return out;
    }
    if (mode == 8) {
        long long q = x >> 1;
        if ((x & 1) == 0 || q < 1 || !FK_POK(q)) { return -2; }
        static char p[FK_PATH_CAP];
        fk_cstr(FK_HH(q), p, FK_PATH_CAP);
        long long start = 0, n = 0;
        if (!fk_fb_serialize(FK_HT(q), &start, &n)) { return -2; }
        int fd = open(p, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { fk_sbp = start; return -2; }
        long long wr = 0;
        while (wr < n) {
            long long w = write(fd, fk_sb + start + wr, n - wr);
            if (w <= 0) { break; }
            wr = wr + w;
        }
        close(fd);
        fk_sbp = start;
        if (wr < n) { return -2; }
        return n << 1;
    }
    return fk_nothing;
}
/* ---- the rest that lands, and the wait that wakes on the word (host_sleep_ms, tag 183) ----
 * An int ask rests that many ms. A list ask (n watch ...) rests at most n ms and wakes early the moment a watched gift
 * frame's seq word (its first word, the seqlock) differs from the seq the caller last saw: a watch is a frame handle
 * (seq read at entry) or a (handle seq) pair. The rest is sliced -- a 1 ms nanosleep while the remainder is safely
 * larger than the worst overshoot seen so far in this rest, a yield spin for the last stretch -- because nanosleep on
 * this host lands 1-10 ms late (niced or loaded), and a frame that wakes late is a frame that shows a stale word.
 * Answers: int ask -> ms rested (nearest); list ask -> (ms-rested woke-index us-rested (seq-now ...)). */
static long long fk_mono_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + (long long)ts.tv_nsec;
}
/* Temporary terminal-byte carrier; all decoding/bindings live in
 * bml/form-glass-input.bml. Shrink: docs/glass-keyboard.md. No subprocess,
 * terminal mode change on non-TTY input, or reads from another process's tty. */
#if !defined(_WIN32)
static struct termios fk_tty_saved, fk_tty_direct;
static int fk_tty_active, fk_tty_cleanup_registered;
static const int fk_tty_signals[] = { SIGINT, SIGTERM, SIGHUP, SIGTSTP, SIGCONT };
static struct sigaction fk_tty_old[5];
extern int atexit(void (*)(void));
static void fk_tty_restore(void) {
    if (!fk_tty_active) { return; }
    tcsetattr(0, TCSANOW, &fk_tty_saved);
    fk_tty_active = 0;
    for (int k = 0; k < 5; k++) { sigaction(fk_tty_signals[k], &fk_tty_old[k], 0); }
    write(1, "\033[?2004l", 8);
}
static void fk_tty_signal(int sig) {
    if (sig == SIGCONT) {
        if (fk_tty_active) { tcsetattr(0, TCSANOW, &fk_tty_direct); }
        return;
    }
    if (sig == SIGTSTP) {
        tcsetattr(0, TCSANOW, &fk_tty_saved);
        /* SIGSTOP avoids changing the saved SIGTSTP disposition while suspended. */
        raise(SIGSTOP);
        if (fk_tty_active) { tcsetattr(0, TCSANOW, &fk_tty_direct); }
        return;
    }
    fk_tty_restore();
    raise(sig);
}
static int fk_tty_ready(void) {
    if (!fk_tty_active) { return 0; }
    struct pollfd p = { 0, POLLIN, 0 };
    return poll(&p, 1, 0) > 0 && (p.revents & (POLLIN | POLLHUP | POLLERR));
}
#else
static void fk_tty_restore(void) {}
static int fk_tty_ready(void) { return 0; }
#endif
static long long fk_tty_door(long long mode) {
#if !defined(_WIN32)
    if (mode == 0) { fk_tty_restore(); return 2; }
    if (mode == 3) { return fk_tty_active ? 2 : 0; }
    if (mode == 1) {
        if (fk_tty_active) { return 2; }
        if (!isatty(0) || !isatty(1) || tcgetattr(0, &fk_tty_saved) != 0) { return 0; }
        fk_tty_direct = fk_tty_saved;
        fk_tty_direct.c_lflag &= ~(ICANON | ECHO | IEXTEN);
        fk_tty_direct.c_iflag &= ~(IXON | ICRNL | INLCR);
        fk_tty_direct.c_cc[VMIN] = 0;
        fk_tty_direct.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &fk_tty_direct) != 0) { return fk_nothing; }
        struct sigaction sa;
        sa.sa_handler = fk_tty_signal; sigemptyset(&sa.sa_mask); sa.sa_flags = 0;
        int k = 0;
        for (; k < 5; k++) {
            if (sigaction(fk_tty_signals[k], &sa, &fk_tty_old[k]) != 0) {
                while (k > 0) { k--; sigaction(fk_tty_signals[k], &fk_tty_old[k], 0); }
                tcsetattr(0, TCSANOW, &fk_tty_saved); return fk_nothing;
            }
        }
        fk_tty_active = 1;
        if (!fk_tty_cleanup_registered) {
            if (atexit(fk_tty_restore) != 0) { fk_tty_restore(); return fk_nothing; }
            fk_tty_cleanup_registered = 1;
        }
        write(1, "\033[?2004h", 8);
        return 2;
    }
    if (mode == 2 && fk_tty_active) {
        char bytes[64];
        if (!fk_tty_ready()) { return fk_sbuf("", 0); }
        long long n = read(0, bytes, sizeof(bytes));
        if (n > 0) { return fk_sbuf(bytes, n); }
        if (n < 0 && (errno == EINTR || errno == EAGAIN)) { return fk_sbuf("", 0); }
        fk_tty_restore();
    }
#else
    if (mode == 0 || mode == 1 || mode == 3) { return 0; }
#endif
    return fk_nothing;
}
static long long fk_rest_arm(long long a) {
    volatile long long *wp[64];
    long long ws[64];
    long long ms = 0, woke = -1, nw = 0;
    int list_mode = 0;
    if (a == fk_nothing) { return fk_nothing; }
    if ((a & 1) == 0) { ms = a >> 1; }
    else {
        long long p = a >> 1;
        if (!(p >= 1 && FK_POK(p))) { return fk_nothing; }
        if (fk_is_str(FK_HH(p))) {
            char kind[32]; fk_cstr(FK_HH(p), kind, 32);
            long long q = FK_HT(p) >> 1;
            if (fk_cstr_eq(kind, "terminal-input") && q >= 1 && FK_POK(q)
                && (FK_HH(q) & 1) == 0) { return fk_tty_door(FK_HH(q) >> 1); }
            return fk_nothing;
        }
        list_mode = 1;
        ms = FK_HH(p) >> 1;
        p = FK_HT(p) >> 1;
        while (p >= 1 && FK_POK(p) && nw < 64) {
            long long w = FK_HH(p), gh = -1, seq0 = -1;
            if (w != fk_nothing && (w & 1) == 0) { gh = w >> 1; }
            else if (w != fk_nothing && w >= 3) {
                long long q = w >> 1;
                if (q >= 1 && FK_POK(q)) {
                    gh = FK_HH(q) >> 1;
                    long long r = FK_HT(q) >> 1;
                    if (r >= 1 && FK_POK(r)) { seq0 = FK_HH(r) >> 1; }
                }
            }
            if (gh >= 0 && fk_gift_live(gh)) {
                wp[nw] = (volatile long long *)fk_gift_base[gh];
                long long cur = __atomic_load_n(wp[nw], __ATOMIC_ACQUIRE);
                ws[nw] = seq0 >= 0 ? seq0 : cur;
                nw = nw + 1;
            }
            p = FK_HT(p) >> 1;
        }
    }
    long long t0 = fk_mono_ns(), now = t0, deadline = t0 + (ms > 0 ? ms : 0) * 1000000LL, slack = 0;
    for (;;) {
        long long k = 0;
        while (k < nw) { long long s = __atomic_load_n(wp[k], __ATOMIC_ACQUIRE); if (s != ws[k]) { woke = k; break; } k = k + 1; }
        now = fk_mono_ns();
        long long rem = deadline - now;
        if (list_mode && fk_tty_ready()) { woke = -2; }
        if (woke >= 0 || woke == -2 || rem <= 0) { break; }
        if (rem > slack + 300000) {
            struct timespec req;
            long long ask = rem - slack - 100000;
            if (ask > 1000000) { ask = 1000000; }
            if (ask < 50000) { ask = 50000; }
            req.tv_sec = 0; req.tv_nsec = ask;
            nanosleep(&req, 0);
            long long over = fk_mono_ns() - now - ask;
            if (over > slack) { slack = over > 5000000 ? 5000000 : over; }
        } else {
            sched_yield();
        }
    }
    long long ns = now - t0;
    if (!list_mode) { return ((ns + 500000) / 1000000) << 1; }
    long long seqs = 1, k = nw - 1;
    while (k >= 0) { long long s = __atomic_load_n(wp[k], __ATOMIC_ACQUIRE); seqs = fk_cons_val(((s & 1) ? s - 1 : s) << 1, seqs); k = k - 1; }
    return fk_cons_val(((ns + 500000) / 1000000) << 1, fk_cons_val(woke << 1, fk_cons_val((ns / 1000) << 1, fk_cons_val(seqs, 1))));
}
/* ---- reading another kernel's program surface (kernel_ast, tag 32) ---- */
static void *fk_prog_map(char letter, long long pid, long long *size) {
#if !defined(_WIN32) && defined(FK_HAVE_MMAN_HEADER)
    char nm[32];
    fk_store_name(letter, pid, nm);
    int fd = shm_open(nm, O_RDONLY, 0600);
    if (fd < 0) { return 0; }
    struct stat st;
    if (fstat(fd, &st) != 0 || st.st_size <= 0) { close(fd); return 0; }
    void *p = mmap(0, (size_t)st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) { return 0; }
    *size = (long long)st.st_size;
    return p;
#else
    (void)letter; (void)pid; (void)size;
    return 0;
#endif
}
static long long fk_prog_read(long long pid, long long spec) {
    long long dsz = 0, out = fk_nothing;
    long long *D = (long long *)fk_prog_map('D', pid, &dsz);
    if (D == 0) { return fk_nothing; }
    if (dsz < FK_PROG_D_BYTES || D[0] != FK_PROG_MAGIC) { munmap(D, (size_t)dsz); return fk_nothing; }
    if (spec != fk_nothing && (spec & 1) == 0) {
        long long k = spec >> 1;
        if (k < 0) {
            long long path = fk_sbuf((char *)D + FK_PROG_D_PATH_OFF, D[11] >= 0 && D[11] < 4096 ? D[11] : 0);
            out = fk_cons_val(D[2] << 1, fk_cons_val(D[3] << 1, fk_cons_val(D[4] << 1, fk_cons_val(D[5] << 1, fk_cons_val(D[6] << 1, fk_cons_val(D[7] << 1, fk_cons_val(D[8] << 1, fk_cons_val(D[9] << 1, fk_cons_val(D[10] << 1, fk_cons_val(path, 1))))))))));
        } else if (k < D[2] && D[6] == 1) {
            long long asz = 0;
            long long (*A)[4] = (long long (*)[4])fk_prog_map('A', pid, &asz);
            if (A != 0) {
                if ((k + 1) * 32 <= asz) { out = fk_cons_val(A[k][0] << 1, fk_cons_val(A[k][1] << 1, fk_cons_val(A[k][2] << 1, fk_cons_val(A[k][3] << 1, 1)))); }
                munmap(A, (size_t)asz);
            }
        }
    } else if (spec != fk_nothing && spec >= 3) {
        long long p = spec >> 1;
        char mode[16];
        mode[0] = 0;
        if (p >= 1 && FK_POK(p)) { fk_cstr(FK_HH(p), mode, 16); p = FK_HT(p) >> 1; }
        if (fk_cstr_eq(mode, "src") && p >= 1 && FK_POK(p)) {
            long long off = FK_HH(p) >> 1, len = -1;
            long long q = FK_HT(p) >> 1;
            if (q >= 1 && FK_POK(q)) { len = FK_HH(q) >> 1; }
            if (off >= 0 && len >= 0 && off + len <= D[3] && D[7] == 1) {
                long long ssz = 0;
                char *S = (char *)fk_prog_map('S', pid, &ssz);
                if (S != 0) {
                    if (off + len <= ssz) { out = fk_sbuf(S + off, len); }
                    munmap(S, (size_t)ssz);
                }
            }
        } else if (fk_cstr_eq(mode, "defn") && p >= 1 && FK_POK(p)) {
            long long j = FK_HH(p) >> 1;
            if (j >= 0 && j < D[4] && j < FK_PROG_FNS) {
                long long *row = (long long *)((char *)D + FK_PROG_D_FN_OFF) + j * 3;
                long long idx = row[2];
                long long body = idx >= 0 && idx < FK_PROG_FNS ? ((long long *)((char *)D + FK_PROG_D_BODY_OFF))[idx] : -1;
                out = fk_cons_val(row[0] << 1, fk_cons_val(row[1] << 1, fk_cons_val(idx << 1, fk_cons_val(body << 1, 1))));
            }
        }
    }
    munmap(D, (size_t)dsz);
    return out;
}
static long long fk_walk_cold(long long t, long long i, long long fp) {
    if (t == 194) { return fk_walk(fk_node[i][2], fp); }
    if (t == 9) {
        putchar((int)(fk_walk(fk_node[i][1], fp) >> 1));
        return 0;
    }
    if (t == 10) {
        long long a10 = fk_walk(fk_node[i][1], fp);
        long long b10 = fk_walk(fk_node[i][2], fp);
        if (fk_isf(a10) || fk_isf(b10)) {
            return fk_fbox(fk_num(a10) / fk_num(b10));
        }
        return ((a10 >> 1) / (b10 >> 1)) << 1;
    }
    if (t == 11) {
        long long a11 = fk_walk(fk_node[i][1], fp);
        long long b11 = fk_walk(fk_node[i][2], fp);
        if (fk_isf(a11) || fk_isf(b11)) {
            double x11 = fk_num(a11);
            double y11 = fk_num(b11);
            return fk_fbox(x11 - y11 * (double)((long long)(x11 / y11)));
        }
        return ((a11 >> 1) % (b11 >> 1)) << 1;
    }
    if (t == 15) {
        /* now_unix_ms: milliseconds, matching the Go/Rust/TS siblings' shape */
        return fk_now_ms() << 1;
    }
    if (t == 16) {
        return ((long long)arc4random()) << 1;
    }
    if (t == 17) {
        long long ix17 = fk_walk(fk_node[i][1], fp) >> 1;
        if (ix17 < 0 || ix17 >= fk_src_len) {
            return 0;
        }
        return ((long long)(unsigned char)fk_src[ix17]) << 1;
    }
    if (t == 24) {
        return fk_strv(fk_node[i][1]);
    }
    if (t == 25) {
        long long sv25 = fk_walk(fk_node[i][1], fp);
        if (sv25 == fk_nothing) {
            /* Measuring an absence must not answer a counterfeit 0 — the
             * fk_nothing stone: no-value is never conflated with 0. A silent 0
             * here dressed a vanished host-exec launch as "empty" (2026-08-27);
             * silent error hides illness. This is the op-238 class of
             * legitimate runtime death (a state the program cannot honestly
             * continue past), not a bounds check: Go's str_len dies this same
             * death, and callers name the absence with nothing? before
             * measuring. */
            fk_die("fkwu: str_len: nothing has no length -- ask nothing? before measuring");
        }
        long long sa = fk_stri(sv25);
        if (sa < 0 || !FK_SOK(sa)) {
            return 0;
        }
        return FK_SLEN(sa) << 1;
    }
    if (t == 238) {
        /* form_error — the voice of refusal. A program that raises it has
         * declared its own cannot-recover, so per the two-phase law this is a
         * legitimate runtime death: message to fd 2, exit nonzero, exactly as
         * Go/Rust/TS panic on their native form_error. Before 2026-07-17 this
         * op was absent here and axiom-5 lowered every raise to nothing — the
         * bp "property" aphonia: bands sailed green past raised errors. */
        long long sa = fk_stri(fk_walk(fk_node[i][1], fp));
        fk_write_all_raw(2, "fkwu: form_error: ", 18);
        if (sa >= 0 && FK_SOK(sa)) {
            fk_write_all_raw(2, FK_SBYTES(sa), (unsigned long)FK_SLEN(sa));
        }
        fk_write_all_raw(2, "\n", 1);
        exit(1);
    }
    if (t == 26) {
        long long wa26 = fk_walk(fk_node[i][1], fp); fk_vp(wa26); long long sa26 = fk_stri(wa26);
        long long sb26 = fk_stri(fk_walk(fk_node[i][2], fp)); fk_vsp = fk_vsp - 1;
        if (fk_keyeq(sa26, sb26)) {
            return 2;
        }
        return 0;
    }
    if (t == 27) {
        long long wa27 = fk_walk(fk_node[i][1], fp); fk_vp(wa27); long long sa = fk_stri(wa27);
        long long sb = fk_stri(fk_walk(fk_node[i][2], fp)); fk_vsp = fk_vsp - 1;
        if (sa < 0 || !FK_SOK(sa) || sb < 0 || !FK_SOK(sb)) {
            return 0 - 2;
        }
        long long ln = FK_SLEN(sa) + FK_SLEN(sb);
        while (fk_sbp + ln > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
            fk_sb_check();
        }
        long long j = 0;
        while (j < FK_SLEN(sa)) {
            fk_sb[fk_sbp + j] = FK_SBYTES(sa)[j];
            j = j + 1;
        }
        j = 0;
        while (j < FK_SLEN(sb)) {
            fk_sb[fk_sbp + FK_SLEN(sa) + j] = FK_SBYTES(sb)[j];
            j = j + 1;
        }
        return fk_strv(fk_sintern(fk_sbp, ln));
    }
    if (t == 28) {
        long long wa28 = fk_walk(fk_node[i][1], fp); fk_vp(wa28); long long sa = fk_stri(wa28);
        long long k = fk_walk(fk_node[i][2], fp) >> 1; fk_vsp = fk_vsp - 1;
        if (sa < 0 || !FK_SOK(sa) || k < 0 || k >= FK_SLEN(sa)) {
            return 0 - 2;
        }
        return ((long long)(unsigned char)FK_SBYTES(sa)[k]) << 1;
    }
    if (t == 30) {
        /* str_find(h, needle, from) -- the body's search. It answers the BYTE
         * INDEX of the first occurrence of `needle` in `h` at or after
         * max(from, 0), or -1.
         *
         * THIS ARM IS OLDER THAN ITS NAME. str_find left flt-ops on 2026-07-01
         * with substring/int_to_str/str_to_int and became Form composition in
         * core.fk. The other three arms went with their rows; THIS one stayed,
         * unreachable, together with its fkc-tri2 arm in fkc-table-serialize.fk.
         * On 2026-09-08 the bearing census named `fstr-find-loop` at 35.1M calls
         * and 64.3% of the locale walk with the remedy "mint-a-native", and the
         * mint was a row: four mirrors, three of them already standing. An
         * orphaned arm reads exactly like an absent one from the call site, so
         * READ EVERY MIRROR FOR THE NAME BEFORE YOU MINT.
         *
         * THE ONE MEANING -- BYTES, CLAMPED, NEVER DIES.
         * witnessed: 2026-09-08 -> str-find-one-meaning-band 8191 on all four arms
         *   from < 0                       -> 0 (a search cannot start before the
         *                                    first byte; go/rust/ts already clamped
         *                                    and the recipe did not -- the ONE edge
         *                                    that moved when this row came back)
         *   from > str_len(h)              -> -1, the empty needle included
         *   needle empty                   -> from, once clamped into range
         *   needle longer than what is left-> -1
         *   overlapping occurrences        -> the first
         *   h or needle not a string       -> -1 (an absence is not a haystack and
         *                                    not a needle; the three siblings die
         *                                    here, so no four-way meaning exists
         *                                    and fkwu answers rather than dying)
         * BYTES, NOT CODEPOINTS. Every index in this dialect is a byte offset --
         * str_byte_at indexes bytes, substring cuts bytes, and the locale rows are
         * Persian, Hebrew, Chinese and Japanese. A `from` snapped up to a character
         * start (which go, rust and ts did until 2026-09-08) skips a needle that
         * begins on a continuation byte, and no ASCII band can say so.
         *
         * ONE PLACE THIS ANSWERS WHERE THE RECIPE DOES NOT: a `nothing` from.
         * `nothing` reads as a hugely negative int, so fstr-find-loop walks up
         * from nine quintillion below zero and never returns -- measured, a 120 s
         * probe printed nothing at all. This clamps to 0 and answers. A spin
         * carries no meaning to break.
         *
         * The two byte pointers are hoisted OUT of the scan: FK_SBYTES is a macro
         * over fk_sb + fk_so[si], and nothing in this loop walks, interns or grows,
         * so the pool cannot move under it. Re-reading it per byte cost the inner
         * compare two extra loads on every position of a 35M-position walk. */
        long long wa30 = fk_walk(fk_node[i][1], fp); fk_vp(wa30);
        long long wb30 = fk_walk(fk_node[i][2], fp); fk_vp(wb30);
        long long from = fk_walk(fk_node[i][3], fp) >> 1;
        /* re-read the two strings from the value stack AFTER every walk: a melt
         * inside the `from` expression relocates through fk_vs, and an index taken
         * before it would name a string that has moved. */
        long long sa = fk_stri(fk_vs[fk_vsp - 2]);
        long long sb = fk_stri(fk_vs[fk_vsp - 1]);
        fk_vsp = fk_vsp - 2;
        if (sa < 0 || !FK_SOK(sa) || sb < 0 || !FK_SOK(sb)) {
            return 0 - 2;
        }
        if (from < 0) {
            from = 0;
        }
        long long hl30 = FK_SLEN(sa);
        if (from > hl30) {
            return 0 - 2;
        }
        long long ln = FK_SLEN(sb);
        if (ln == 0) {
            return from << 1;
        }
        long long lim = hl30 - ln;
        if (from > lim) {
            return 0 - 2;
        }
        const char *hb30 = FK_SBYTES(sa);
        const char *nb30 = FK_SBYTES(sb);
        char b030 = nb30[0];
        long long pos = from;
        while (pos <= lim) {
            if (hb30[pos] == b030) {
                long long j3 = 1;
                while (j3 < ln && hb30[pos + j3] == nb30[j3]) {
                    j3 = j3 + 1;
                }
                if (j3 == ln) {
                    return pos << 1;
                }
            }
            pos = pos + 1;
        }
        return 0 - 2;
    }
    if (t == 31) {
        long long sa = fk_stri(fk_walk(fk_node[i][1], fp));
        if (sa < 0 || !FK_SOK(sa)) {
            return 0;
        }
        long long off = FK_SO(sa);
        long long n = FK_SLEN(sa);
        long long sign = 1;
        long long j4 = 0;
        while (j4 < n && (fk_sb[off + j4] == FK_CH_SPACE || fk_sb[off + j4] == FK_CH_TAB || fk_sb[off + j4] == FK_CH_LF ||
                          fk_sb[off + j4] == FK_CH_CR)) {
            j4 = j4 + 1;
        }
        if (j4 < n && fk_sb[off + j4] == FK_CH_DASH) {
            sign = 0 - 1;
            j4 = j4 + 1;
        }
        long long v = 0;
        while (j4 < n) {
            char c = fk_sb[off + j4];
            if (c < FK_CH_DIGIT0 || c > FK_CH_DIGIT9) {
                break;
            }
            v = v * 10 + (c - FK_CH_DIGIT0);
            j4 = j4 + 1;
        }
        return (sign * v) << 1;
    }
    if (t == 33) {
        long long b = fk_walk(fk_node[i][1], fp) >> 1;
        if (b < 0 || b > 255) {
            return fk_strv(fk_sintern(fk_sbp, 0));
        }
        while (fk_sbp + 1 > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
            fk_sb_check();
        }
        fk_sb[fk_sbp] = (char)b;
        return fk_strv(fk_sintern(fk_sbp, 1));
    }
    if (t == 34) {
        return (((fk_walk(fk_node[i][1], fp) >> 1) & (fk_walk(fk_node[i][2], fp) >> 1)) << 1);
    }
    if (t == 35) {
        return (((fk_walk(fk_node[i][1], fp) >> 1) | (fk_walk(fk_node[i][2], fp) >> 1)) << 1);
    }
    if (t == 36) {
        return (((fk_walk(fk_node[i][1], fp) >> 1) ^ (fk_walk(fk_node[i][2], fp) >> 1)) << 1);
    }
    if (t == 37) {
        unsigned int x = (unsigned int)(fk_walk(fk_node[i][1], fp) >> 1);
        long long n = (fk_walk(fk_node[i][2], fp) >> 1) & 31;
        return ((long long)(unsigned int)(x << n)) << 1;
    }
    if (t == 38) {
        unsigned int x = (unsigned int)(fk_walk(fk_node[i][1], fp) >> 1);
        long long n = (fk_walk(fk_node[i][2], fp) >> 1) & 31;
        return ((long long)(x >> n)) << 1;
    }
    if (t == 39) {
        unsigned long long x = (unsigned int)(fk_walk(fk_node[i][1], fp) >> 1);
        long long n = (fk_walk(fk_node[i][2], fp) >> 1) & 31;
        return ((long long)(unsigned int)((x >> n) | (x << (32 - n)))) << 1;
    }
    if (t == 40) {
        unsigned int x = (unsigned int)(fk_walk(fk_node[i][1], fp) >> 1);
        unsigned int y = (unsigned int)(fk_walk(fk_node[i][2], fp) >> 1);
        return ((long long)(unsigned int)(x + y)) << 1;
    }
    if (t == 41) {
        unsigned int x = (unsigned int)(fk_walk(fk_node[i][1], fp) >> 1);
        return ((long long)(unsigned int)(~x)) << 1;
    }
    if (t == 43) {
        long long iv43 = fk_walk(fk_node[i][1], fp);
        return fk_intern_int_node(iv43);
    }
    if (t == 46) {
        long long sv46 = fk_walk(fk_node[i][1], fp);
        return fk_intern_str_node(sv46);
    }
    if (t == 47) {
        long long cat47 = fk_walk(fk_node[i][1], fp);
        long long kids47 = fk_walk(fk_node[i][2], fp);
        return fk_intern_composite(cat47, kids47);
    }
    if (t == 91) {
        long long xs91 = fk_walk(fk_node[i][1], fp);
        long long q91 = xs91 >> 1;
        long long p91 = 0;
        long long l91 = 0;
        long long ty91 = 0;
        long long in91 = 0;
        if (q91 >= 1 && FK_POK(q91)) {
            p91 = FK_HH(q91) >> 1;
            q91 = FK_HT(q91) >> 1;
        }
        if (q91 >= 1 && FK_POK(q91)) {
            l91 = FK_HH(q91) >> 1;
            q91 = FK_HT(q91) >> 1;
        }
        if (q91 >= 1 && FK_POK(q91)) {
            ty91 = FK_HH(q91) >> 1;
            q91 = FK_HT(q91) >> 1;
        }
        if (q91 >= 1 && FK_POK(q91)) {
            in91 = FK_HH(q91) >> 1;
        }
        return fk_make_nodeid(p91, l91, ty91, in91);
    }
    if (t == 112) {
        long long bv112 = fk_walk(fk_node[i][1], fp);
        return fk_intern_bool_node(bv112);
    }
    if (t == 113) {
        long long sa113 = fk_stri(fk_walk(fk_node[i][1], fp));
        double fd113 = 0.0;
        if (sa113 >= 0 && FK_SOK(sa113)) {
            char tb113[128];
            long long n113 = FK_SLEN(sa113);
            if (n113 > 126) {
                n113 = 126;
            }
            long long jj113 = 0;
            while (jj113 < n113) {
                tb113[jj113] = FK_SBYTES(sa113)[jj113];
                jj113 = jj113 + 1;
            }
            tb113[n113] = 0;
            fd113 = strtod(tb113, 0);
        }
        /* INTERN, as the name says: the Go proof arm (internTrivialFloat64)
         * dedups by canonical bits -- one quiet NaN, -0.0 folds to +0.0,
         * equal doubles share one row. This arm minted a fresh node AND a
         * fresh pool slot per call, the one intern op whose behavior did not
         * keep its name; the fkwu seed was the diverging arm. Compare by
         * BITS, not ==, so NaN interns to itself. nid[3] carries the pool
         * index, mirroring the Go arm's Inst. */
        unsigned long long fbits113;
        return fk_intern_float_node(fd113);
    }
    if (t == 50) {
        long long sa = fk_node[i][1];
        if (sa < 0 || !FK_SOK(sa)) {
            return 0;
        }
        char tmp[128];
        long long n = FK_SLEN(sa);
        if (n > 126) {
            n = 126;
        }
        long long j = 0;
        while (j < n) {
            tmp[j] = FK_SBYTES(sa)[j];
            j = j + 1;
        }
        tmp[n] = 0;
        return fk_fbox(strtod(tmp, 0));
    }
    if (t == 51) {
        double d = fk_num(fk_walk(fk_node[i][1], fp));
        long long q = (long long)d;
        if (d < (double)q) {
            q = q - 1;
        }
        return q << 1;
    }
    if (t == 52) {
        double x = fk_num(fk_walk(fk_node[i][1], fp));
        long long nd = fk_walk(fk_node[i][2], fp) >> 1;
        return fk_fbox(fk_round_ndigits_decimal(x, nd));
    }
    if (t == 53) {
        /* a source float literal parses as (53 (24 idx)): constant child,
         * constant value. Memo the boxed result per AST node so the literal
         * mints one pool slot per process, not one per evaluation. A
         * computed str_to_float (non-24 child) never memoizes. */
        long long lc53 = fk_node[i][1];
        int lit53 = lc53 >= 0 && lc53 < fk_node_count && fk_node[lc53][0] == 24;
        if (lit53 && fk_flit_memo != 0 && fk_flit_memo[i] != 0) {
            return fk_flit_memo[i];
        }
        long long sa = fk_stri(fk_walk(fk_node[i][1], fp));
        long long fbv53;
        if (sa < 0 || !FK_SOK(sa)) {
            fbv53 = fk_fbox(0.0);
        } else {
            char tmp[128];
            long long n = FK_SLEN(sa);
            if (n > 126) {
                n = 126;
            }
            long long j = 0;
            while (j < n) {
                tmp[j] = FK_SBYTES(sa)[j];
                j = j + 1;
            }
            tmp[n] = 0;
            fbv53 = fk_fbox(strtod(tmp, 0));
        }
        if (lit53 && fk_flit_memo != 0) {
            fk_flit_memo[i] = fbv53;
        }
        return fbv53;
    }
    if (t == 54) {
        return ((long long)fk_num(fk_walk(fk_node[i][1], fp))) << 1;
    }
    if (t == 81) {
        return fk_fbox(fk_sqrt_d(fk_num(fk_walk(fk_node[i][1], fp))));
    }
    if (t == 82) {
        return ((long long)fk_num(fk_walk(fk_node[i][1], fp))) << 1;
    }
    if (t == 83) {
        fk_walk(fk_node[i][1], fp);
        return 0;
    }
    if (t == 84) {
        long long a84 = fk_walk(fk_node[i][1], fp);
        fk_vp(a84);
        long long b84 = fk_walk(fk_node[i][2], fp);
        fk_vsp = fk_vsp - 1;
        return fk_fbox(fk_dot_list(fk_vs[fk_vsp], b84));
    }
    if (t == 85) {
        return fk_fbox(fk_mag_list(fk_walk(fk_node[i][1], fp)));
    }
    if (t == 86) {
        long long a86 = fk_walk(fk_node[i][1], fp);
        fk_vp(a86);
        long long b86 = fk_walk(fk_node[i][2], fp);
        fk_vsp = fk_vsp - 1;
        double ma86 = fk_mag_list(fk_vs[fk_vsp]);
        double mb86 = fk_mag_list(b86);
        if (ma86 == 0.0 || mb86 == 0.0) {
            return fk_fbox(0.0);
        }
        return fk_fbox(fk_dot_list(fk_vs[fk_vsp], b86) / (ma86 * mb86));
    }
    if (t == 87) {
        double d87 = fk_num(fk_walk(fk_node[i][1], fp));
        double a87 = d87 < 0.0 ? 0.0 - d87 : d87;
        long long q87 = (long long)(a87 + 0.5);
        if (d87 < 0.0) {
            q87 = 0 - q87;
        }
        return q87 << 1;
    }
    if (t == 88) {
        double d88 = fk_num(fk_walk(fk_node[i][1], fp));
        long long q88 = (long long)d88;
        if (d88 > (double)q88) {
            q88 = q88 + 1;
        }
        return q88 << 1;
    }
    if (t == 89) {
        return fk_fbox(fk_exp_d(fk_num(fk_walk(fk_node[i][1], fp))));
    }
    if (t == 90) {
        return fk_fbox(fk_log_d(fk_num(fk_walk(fk_node[i][1], fp))));
    }
    if (t == 195) {
        /* math_pow: the three witnesses answer math.Pow; the integer part of the
         * exponent is squared out exactly (2^10 is 1024.0 to the bit), the
         * fractional part rides exp(y*log(x)) like the other transcendentals. */
        double pb195 = fk_num(fk_walk(fk_node[i][1], fp));
        double pe195 = fk_num(fk_walk(fk_node[i][2], fp));
        return fk_fbox(fk_pow_d(pb195, pe195));
    }
    if (t == 201) {
        /* float_leaf mode x -- one door for the float-NodeID surface the siblings
         * carry as four natives: 0 float_value (read a type-6/7 leaf's IEEE value),
         * 1 make_float32 (intern a type-6 leaf), 2 make_float64 (a type-7 leaf),
         * 3 math_pi. The four names are rewrite rows over this tag (fk_rwtab). */
        long long fm201 = fk_walk(fk_node[i][1], fp);
        if ((fm201 >> 1) == 9) {
            /* MODE 9 -- substring(s, start, end): the byte-indexed string slice.
             *
             * It was tag 29 until 2026-07-01, when it became Form composition over
             * the narrow waist and the tag was spent on mlx_live. The recipe
             * (fstr-substring-halve, still in core.fk and still the statement of
             * what this door MEANS) is right, and it is the slowest door in the
             * body: cutting 192 kB costs ~384,000 interpreted str_byte_at /
             * byte_to_str / str_concat calls -- 4.4 MB/s where str_concat, the
             * same bytes through the same pool, runs at ~512 MB/s. char_at,
             * str_find, split-on and trim are all recipes over it.
             *
             * Every tag 0..255 carries an arm and 150 is held as the native-surface
             * probe, so this rides the leaf door the way modes 4-8 do rather than
             * spending the probe. The rewrite row (fk_rwtab, "substring") builds
             *     fk_smknode(201, LIT 9, s, (cons start end))
             * -- the range lives in the leaf node's THIRD child as a tag-19 NODE,
             * read child by child below, so the fast door allocates no pair.
             *
             * THE ONE MEANING OF THE CUT -- BYTES, CLAMPED, NEVER DIES. It is the recipe's
             * own contract, measured edge by edge before this arm existed, and since
             * 2026-09-07 it is what all four kernels answer
             * (form-stdlib/tests/substring-one-meaning-band.fk, 4095 four ways, a
             * drift gate):
             *   n = end - start <= 0                 -> ""   (start>end, zero len)
             *   indices outside [0, len)             -> contribute nothing, so the
             *                                           answer is s[max(start,0) ..
             *                                           min(end,len)) and never dies
             *   s that is not a string (nothing too) -> ""   (every leaf read -1 and
             *                                           byte_to_str answered "")
             * BYTES, NOT CODEPOINTS. str_byte_at indexes bytes and the locale rows
             * are full of multi-byte tongues; flooring the offsets to character
             * starts -- which the Go/Rust/TS natives did until that day -- silently
             * re-cut every Persian, Hebrew, Chinese and Japanese row. (0,1) of "Ω" is
             * one raw byte 206 here, as it is through the recipe and on the Go arm;
             * rust and ts cannot HOLD a severed character and answer the absence.
             *
             * ONE PLACE THIS ANSWERS WHERE THE RECIPE DOES NOT: a `nothing` start.
             * (sub 2 nothing) is 8999999999999999999, so the recipe halves a range
             * of nine quintillion and never returns -- measured, an 8 s probe that
             * printed nothing. A hugely negative start clamps to 0 here and the
             * slice comes back. A spin carries no meaning to break. */
            long long rn201 = fk_node[i][3];
            if (rn201 == 0 || fk_node[rn201][0] != 19) {
                /* (float_leaf 9 x) written by hand: the door's own arity is 2, so
                 * no range ever arrived. Never-was answers nothing. */
                return fk_nothing;
            }
            long long ws201 = fk_walk(fk_node[i][2], fp);
            fk_vp(ws201);
            long long ss201 = fk_stri(ws201);
            long long a201 = fk_walk(fk_node[rn201][1], fp) >> 1;
            long long b201 = fk_walk(fk_node[rn201][2], fp) >> 1;
            fk_vsp = fk_vsp - 1;
            if (ss201 < 0 || !FK_SOK(ss201)) {
                return fk_strv(fk_sintern(fk_sbp, 0));
            }
            long long sl201 = FK_SLEN(ss201);
            if (a201 < 0) {
                a201 = 0;
            }
            if (b201 > sl201) {
                b201 = sl201;
            }
            if (b201 <= a201) {
                return fk_strv(fk_sintern(fk_sbp, 0));
            }
            long long ln201 = b201 - a201;
            while (fk_sbp + ln201 > fk_scap_b) {
                fk_scap_b = fk_scap_b * 2;
                fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
                fk_sb_check();
            }
            /* FK_SBYTES is re-read AFTER the grow -- fk_store_grow moves fk_sb, and a
             * pointer taken before it would copy from freed bytes. A plain byte loop,
             * not memcpy: memcpy is declared only inside this file's __APPLE__ arm, and
             * str_concat (tag 27) copies its bytes exactly this way on every host. */
            long long j201 = 0;
            while (j201 < ln201) {
                fk_sb[fk_sbp + j201] = FK_SBYTES(ss201)[a201 + j201];
                j201 = j201 + 1;
            }
            return fk_strv(fk_sintern(fk_sbp, ln201));
        }
        long long fx201 = fk_walk(fk_node[i][2], fp);
        /* modes 10-16: the SPEAKING family -- the mouth that answers the sense_mic_* ears.
         * Every sense door in this seed pointed inward until 2026-09-08, so the body's own
         * voice left through afplay. No tag was spendable; see fk_spk_door. */
        /* modes 17-19: the doors that END THE SHELL -- host_spawn_at, host_alive,
         * fs_mkfifo. The ear's lanes reached for `sh -c` only to place three file
         * descriptors and to ask whether a pid answers; see fk_host_door. */
        if ((fm201 >> 1) >= 17) { return fk_host_door(fm201 >> 1, fx201); }
        if ((fm201 >> 1) >= 10) { return fk_spk_door(fm201 >> 1, fx201); }
        /* modes 4-8: the binary form (value_kind, recipe_to_bytes, bytes_to_recipe,
         * read_form_binary, write_form_binary) -- see fk_fb_door */
        if ((fm201 >> 1) >= 4) { return fk_fb_door(fm201 >> 1, fx201); }
        return fk_float_leaf(fm201 >> 1, fx201);
    }
    if (t == 55) {
        static char p[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, FK_PATH_CAP);
        fk_unlink_segments(p);
        return rmdir(p) << 1;
    }
    if (t == 56) {
        static char p[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, FK_PATH_CAP);
        int rc = mkdir(p, 0777);
        return (rc < 0 ? 0 : 1) << 1;
    }
    if (t == 57) {
        static char p[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, FK_PATH_CAP);
        int fd = open(p, 0);
        if (fd < 0) {
            return 0;
        }
        close(fd);
        return 2;
    }
    if (t == 58) {
        static char p[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, FK_PATH_CAP);
        return unlink(p) << 1;
    }
    if (t == 59) {
        static char a[FK_PATH_CAP];
        static char b[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), a, FK_PATH_CAP);
        fk_cstr(fk_walk(fk_node[i][2], fp), b, FK_PATH_CAP);
        return rename(a, b) << 1;
    }
    if (t == 60) {
        static char p[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, FK_PATH_CAP);
        int fd = open(p, 0);
        if (fd < 0) {
            return -2;
        }
        long n = lseek(fd, 0, 2);
        close(fd);
        return ((long long)n) << 1;
    }
    if (t == 61) {
        static char p[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, FK_PATH_CAP);
        long long xs = fk_walk(fk_node[i][2], fp);
        int fd = open(p, O_WRONLY | O_CREAT | O_APPEND, 0666);
        if (fd < 0) {
            return -2;
        }
        /* Files return their resulting extent; nonseekable streams return bytes
         * accepted. Form owns the frame commit; this seed only carries the write. */
        static char tmp[8192];
        long long n = 0;
        long long written = 0;
        long long q = xs >> 1;
        while (q >= 1 && FK_POK(q)) {
            if (n == 8192) {
                if (!fk_write_all_raw(fd, tmp, n)) {
                    close(fd);
                    return -2;
                }
                written += n;
                n = 0;
            }
            tmp[n] = (char)(FK_HH(q) >> 1);
            n = n + 1;
            q = FK_HT(q) >> 1;
        }
        int complete = fk_write_all_raw(fd, tmp, n);
        long long total = lseek(fd, 0, 2);
        if (total < 0 && errno == ESPIPE) total = written + n;
        close(fd);
        if (!complete || total < 0) {
            return -2;
        }
        return total << 1;
    }
    /* kernel_hot n (tag 179): the n hottest functions of THIS process as text,
     * one per line -- heat|name|unit|line|col -- read from the same fn heat the
     * exit report prints, named by the symbol map (never by index coincidence),
     * with the defn's own source pointer: the unit whose text holds the name,
     * and the line and column inside it, found in one pass over the program
     * text. Read-only: no heat is marked or reset. */
    if (t == 179) {
        long long want179 = fk_walk(fk_node[i][1], fp) >> 1;
        if (want179 <= 0) {
            return fk_sbuf("", 0);
        }
        if (want179 > 64) {
            want179 = 64;
        }
        long long picked_j[64], picked_h[64], line_of[64], col_of[64];
        long long np = fk_hot_pick(want179, picked_j, picked_h, line_of, col_of);
        long long q = 0;
        fk_sinit();
        long long start179 = fk_sbp;
        q = 0;
        while (q < np) {
            long long sj = picked_j[q];
            long long so = fk_fnsym_s[sj];
            /* the unit whose text holds the name; the root text has no dep row */
            const char *unit = fk_hot_unit_of(so);
            long long d = 0;
            while (d < fk_src_dep_count) {
                if (so >= fk_src_dep_text_off[d] && so < fk_src_dep_text_off[d] + fk_src_dep_text_len[d] && fk_src_dep_text_len[d] > 0) { unit = fk_src_dep_path[d]; }
                d = d + 1;
            }
            char num[32];
            long long nl;
            nl = sprintf(num, "%lld|", picked_h[q]); fk_sappend(num, nl);
            fk_sappend(fk_srctext + so, fk_fnsym_n[sj]);
            fk_sappend("|", 1);
            fk_sappend(unit, fk_cstrlen(unit));
            nl = sprintf(num, "|%lld|%lld\n", line_of[q], col_of[q]); fk_sappend(num, nl);
            q = q + 1;
        }
        return fk_strv(fk_sintern(start179, fk_sbp - start179));
    }
    if (t == 177) {
        long long gh177 = fk_walk(fk_node[i][1], fp) >> 1;
        long long sv177 = fk_gift_take_str(gh177);
        if (sv177 == fk_nothing) {
            return fk_nothing;
        }
        long long si177 = fk_stri(sv177);
        long long pos177 = 0;
        fk_cross_refused = 0;
        long long out177 = fk_cross_decode(FK_SBYTES(si177), FK_SLEN(si177), &pos177, 0);
        return fk_cross_refused ? fk_nothing : out177;
    }
    if (t == 178) {
        long long gh178 = fk_walk(fk_node[i][1], fp) >> 1;
        long long v178 = fk_walk(fk_node[i][2], fp);
        if (!fk_gift_live(gh178)) {
            return fk_nothing;
        }
        fk_sinit();
        long long start178 = fk_sbp;
        fk_cross_refused = 0;
        fk_cross_emit(v178, 0);
        long long n178 = fk_sbp - start178;
        fk_sbp = start178;
        if (fk_cross_refused) {
            /* a value that cannot cross (a function, an unknown node) refuses the give by name */
            return fk_nothing;
        }
        return fk_gift_give(gh178, fk_sb + start178, n178);
    }
    /* THE FRAME PACER AND THE TERMINAL (tags 180-183): terminal_cols /
     * terminal_lines read the controlling terminal's window (ioctl, no tput
     * fork; a non-terminal answers nothing); host_monotonic_ms is the
     * kernel.monotonic-ms door the glass event loop names -- a clock that never
     * steps; host_sleep_ms n rests the process n milliseconds (nanosleep) and
     * answers the milliseconds actually rested, so no frame is paced by forking
     * /bin/sleep. */
    if (t == 180) {
        return fk_terminal_dim(1);
    }
    if (t == 181) {
        return fk_terminal_dim(0);
    }
    if (t == 151) {
        /* host_vm_stat: list(page_size, memsize, free, active, inactive, wired, speculative, purgeable, compressor_pages, pageins, pageouts, compressions, decompressions, swapins, swapouts, external, internal) -- Mach vm_statistics64, pages */
#ifdef __APPLE__
        unsigned int w151[40];
        unsigned int c151 = 38;
        long long k151;
        for (k151 = 0; k151 < 40; k151 = k151 + 1) { w151[k151] = 0; }
        if (host_statistics64(mach_host_self(), 4, (int *)w151, &c151) != 0) { return 1; }
        long long vals151[17];
        vals151[0] = fk_sysctl_ll("hw.pagesize");
        vals151[1] = fk_sysctl_ll("hw.memsize");
        vals151[2] = (long long)w151[0]; vals151[3] = (long long)w151[1]; vals151[4] = (long long)w151[2]; vals151[5] = (long long)w151[3];
        vals151[6] = (long long)w151[23]; vals151[7] = (long long)w151[22]; vals151[8] = (long long)w151[32];
        vals151[9] = fk_u64_words(w151[8], w151[9]); vals151[10] = fk_u64_words(w151[10], w151[11]);
        vals151[11] = fk_u64_words(w151[26], w151[27]); vals151[12] = fk_u64_words(w151[24], w151[25]);
        vals151[13] = fk_u64_words(w151[28], w151[29]); vals151[14] = fk_u64_words(w151[30], w151[31]);
        vals151[15] = (long long)w151[34]; vals151[16] = (long long)w151[35];
        long long l151 = 1;
        for (k151 = 16; k151 >= 0; k151 = k151 - 1) { l151 = fk_cons_val(vals151[k151] << 1, l151); }
        return l151;
#else
        return 1;
#endif
    }
    if (t == 152) {
        /* host_load_avg: list(load1, load5, load15) in thousandths */
#ifdef __APPLE__
        double la152[3] = { 0.0, 0.0, 0.0 };
        if (getloadavg(la152, 3) != 3) { return 1; }
        return fk_cons_val(((long long)(la152[0] * 1000.0)) << 1, fk_cons_val(((long long)(la152[1] * 1000.0)) << 1, fk_cons_val(((long long)(la152[2] * 1000.0)) << 1, 1)));
#else
        return 1;
#endif
    }
    if (t == 153) {
        /* host_disk_stat: list(bytes_read, bytes_written, ops_read, ops_write) cumulative over every block storage driver; empty when the door is absent */
        long long d153[4];
        if (fk_host_disk_stat(d153) < 0) { return 1; }
        return fk_cons_val(d153[0] << 1, fk_cons_val(d153[1] << 1, fk_cons_val(d153[2] << 1, fk_cons_val(d153[3] << 1, 1))));
    }
    if (t == 154) {
        /* host_processes name: list of list(pid, rss_bytes, cpu_us, elapsed_seconds, nice) for every process whose name is the argument -- libproc, no ps, no pgrep */
#ifdef __APPLE__
        static char nm154[256];
        fk_cstr(fk_walk(fk_node[i][1], fp), nm154, 256);
        static int pids154[8192];
        int got154 = proc_listpids(1, 0, pids154, (int)sizeof pids154);
        if (got154 <= 0) { return 1; }
        long long count154 = got154 / (long long)sizeof(int);
        struct fk_mach_timebase tb154; tb154.numer = 1; tb154.denom = 1;
        mach_timebase_info(&tb154);
        long long now154 = (long long)time(0);
        long long l154 = 1;
        long long k154;
        for (k154 = count154 - 1; k154 >= 0; k154 = k154 - 1) {
            int pid154 = pids154[k154];
            if (pid154 <= 0) { continue; }
            char pn154[256];
            pn154[0] = 0;
            if (proc_name(pid154, pn154, 256) <= 0) { continue; }
            if (!fk_cstr_eq(pn154, nm154)) { continue; }
            unsigned char ti154[128];
            unsigned char bi154[160];
            if (proc_pidinfo(pid154, 4, 0, ti154, 128) < 96) { continue; }
            long long rss154 = *(long long *)(ti154 + 8);
            unsigned long long user154 = *(unsigned long long *)(ti154 + 16);
            unsigned long long sys154 = *(unsigned long long *)(ti154 + 24);
            long long cpu154 = (long long)(((user154 + sys154) * (unsigned long long)tb154.numer / (unsigned long long)tb154.denom) / 1000ULL);
            long long start154 = 0;
            long long nice154 = 0;
            if (proc_pidinfo(pid154, 3, 0, bi154, 160) >= 136) { start154 = *(long long *)(bi154 + 120); nice154 = (long long)(*(int *)(bi154 + 116)); }
            long long elapsed154 = (start154 > 0 && now154 >= start154) ? now154 - start154 : -1;
            l154 = fk_cons_val(fk_cons_val((long long)pid154 << 1, fk_cons_val(rss154 << 1, fk_cons_val(cpu154 << 1, fk_cons_val(elapsed154 << 1, fk_cons_val(nice154 << 1, 1))))), l154);
        }
        return l154;
#else
        return 1;
#endif
    }
    if (t == 155) {
        return fk_host_spawn_arm(fk_walk(fk_node[i][1], fp), 155);
    }
    if (t == 159) {
        return fk_host_spawn_arm(fk_walk(fk_node[i][1], fp), 159);
    }
    if (t == 161) {
        return fk_host_spawn_arm(fk_walk(fk_node[i][1], fp), 161);
    }
    if (t == 156) {
        /* host_wait pid: the child's exit status (signal death answers 128+signal), -1 when there is no such child */
        long long pid156 = fk_walk(fk_node[i][1], fp) >> 1;
        int st156 = 0;
        if (waitpid((int)pid156, &st156, 0) < 0) { return -2; }
        if ((st156 & 0x7f) == 0) { return ((long long)((st156 >> 8) & 0xff)) << 1; }
        return ((long long)(128 + (st156 & 0x7f))) << 1;
    }
    if (t == 157) {
        /* host_kill pid: SIGTERM to the child; 0 sent, -1 refused */
        long long pid157 = fk_walk(fk_node[i][1], fp) >> 1;
        if (pid157 <= 0) { return -2; }
        return ((long long)kill((int)pid157, 15)) << 1;
    }
    if (t == 158) {
        /* host_nice n: this process's scheduling priority, inherited by what it spawns */
        long long n158 = fk_walk(fk_node[i][1], fp) >> 1;
        return ((long long)setpriority(0, 0, (int)n158)) << 1;
    }
    if (t == 160) {
        return ((long long)getpid()) << 1;
    }
    if (t == 162) {
        /* kernel_live_pids: every registered kernel whose page says alive and whose pid still answers */
        long long slots162[256];
        if (fk_live_read_words("/fg-kernels", slots162, 256) < 0) { return 1; }
        long long l162 = 1;
        long long k162 = 255;
        while (k162 >= 0) {
            long long pid162 = slots162[k162];
            if (pid162 > 0 && kill((int)pid162, 0) == 0) {
                char nm162[32];
                long long w162[FK_LIVE_WORDS];
                fk_live_pid_name(pid162, nm162);
                if (fk_live_read_page(nm162, w162) == FK_LIVE_WORDS && w162[0] == FK_LIVE_MAGIC && w162[19] == 1) { l162 = fk_cons_val(pid162 << 1, l162); }
            }
            k162 = k162 - 1;
        }
        return l162;
    }
    if (t == 163) {
        /* kernel_live pid: the 21 words of that kernel's page, read where they live */
        long long pid163 = fk_walk(fk_node[i][1], fp) >> 1;
        char nm163[32];
        long long w163[FK_LIVE_WORDS];
        fk_live_pid_name(pid163, nm163);
        if (pid163 == (long long)getpid()) { fk_live_note(0); }
        if (fk_live_read_page(nm163, w163) != FK_LIVE_WORDS || w163[0] != FK_LIVE_MAGIC) { return 1; }
        long long l163 = 1;
        long long k163 = FK_LIVE_WORDS - 1;
        while (k163 >= 0) { l163 = fk_cons_val(w163[k163] << 1, l163); k163 = k163 - 1; }
        return l163;
    }
    if (t == 164) {
        /* kernel_hot_rows n: the hottest defns as cells (heat name unit line col boxes unboxes) */
        return fk_page_rows((long long)getpid(), 0, fk_walk(fk_node[i][1], fp) >> 1);
    }
    if (t == 191) {
        /* kernel_box_rows n: the unboxing worklist -- the defns minting the most float boxes, same cell shape */
        return fk_page_rows((long long)getpid(), 1, fk_walk(fk_node[i][1], fp) >> 1);
    }
    if (t == 192) {
        /* kernel_page_hot pid n: that kernel's hottest defns from its page
         * (heat name unit line col boxes unboxes native mints folds).
         * A NEGATIVE n ranks by the FOLD ledger instead -- which recipes ran as
         * this machine's own instructions, their intermediates in registers
         * rather than a pool slot each. The box leg has always been countable
         * (fbox); this is the leg it is chosen against, and until now nothing
         * counted it, so "box or fold" could only ever read as one option. */
        long long p192 = fk_walk(fk_node[i][1], fp) >> 1;
        long long n192 = fk_walk(fk_node[i][2], fp) >> 1;
        if (n192 < 0) { return fk_page_rows(p192, 3, 0 - n192); }
        return fk_page_rows(p192, 0, n192);
    }
    if (t == 193) {
        /* kernel_page_box pid n: that kernel's boxing worklist from its page.
         * A NEGATIVE n asks the MINT ledger instead -- which recipes grew the
         * permanent arena, |n| rows -- the same door, a mode in its own argument. */
        long long p193 = fk_walk(fk_node[i][1], fp) >> 1;
        long long n193 = fk_walk(fk_node[i][2], fp) >> 1;
        if (n193 < 0) { return fk_page_rows(p193, 2, 0 - n193); }
        return fk_page_rows(p193, 1, n193);
    }
    if (t == 29) {
        /* mlx_live: the MLX carrier's state as words (linked, metal, gpu, device, version major/minor/patch, ops,
         * dispatches, error present, error length, 0); nil when MLX is not linked -- no text, nothing to parse */
        long long m29[16];
        if (fk_mlx_live_external(m29) <= 0) { return 1; }
        long long l29 = 1;
        long long k29 = 15;
        while (k29 >= 0) { l29 = fk_cons_val(m29[k29] << 1, l29); k29 = k29 - 1; }
        return l29;
    }
    if (t == 32) {
        /* kernel_ast pid spec: that kernel's program surface where it lives -- spec -1 the header words, k a node's four
         * words, (list "src" off len) a source span, (list "defn" j) a defn's symbol span, fn idx and body node */
        long long pid32 = fk_walk(fk_node[i][1], fp) >> 1;
        return fk_prog_read(pid32, fk_walk(fk_node[i][2], fp));
    }
    if (t == 165) {
        /* metal_live: the carrier's counters as words; nil when Metal is not up */
        long long m165[20];
        if (fk_metal_live_external(m165) <= 0) { return 1; }
        long long l165 = 1;
        long long k165 = 19;
        while (k165 >= 0) { l165 = fk_cons_val(m165[k165] << 1, l165); k165 = k165 - 1; }
        return l165;
    }
    if (t == 166) {
        /* gift_roster_register name: the publisher's slot in /fg-roster; -1 when the roster is full or absent */
        static char nm166[128];
        fk_cstr(fk_walk(fk_node[i][1], fp), nm166, 128);
        return fk_roster_register(nm166) << 1;
    }
    if (t == 167) {
        /* gift_roster_names: a row per standing slot, (name lastSpokeMs) */
        return fk_roster_names();
    }
    if (t == 168) {
        /* cell_map pid: map that kernel's store read-only; a handle, or nothing */
        long long s168 = fk_cell_map_open(fk_walk(fk_node[i][1], fp) >> 1);
        return s168 < 0 ? fk_nothing : (s168 << 1);
    }
    if (t == 169) {
        /* cell_field handle ref k: a field of a foreign node (0 kind 1 cat 2 kids 3 val 4-7 nid 8 sfile 9 line 10 col 11 attr) or list (0 head 1 tail), read where it lives */
        long long s169 = fk_walk(fk_node[i][1], fp) >> 1;
        long long r169 = fk_cell_dec(fk_walk(fk_node[i][2], fp));
        long long k169 = fk_walk(fk_node[i][3], fp) >> 1;
        return fk_cell_field(s169, r169, k169);
    }
    if (t == 170) {
        /* cell_value handle ref: a foreign scalar as this process's own value -- int, string, float, nothing */
        long long s170 = fk_walk(fk_node[i][1], fp) >> 1;
        return fk_cell_value(s170, fk_cell_dec(fk_walk(fk_node[i][2], fp)));
    }
    if (t == 171) {
        /* cell_ref value: this process's own value as its raw store word, the reference another process reads by */
        return fk_cell_enc(fk_walk(fk_node[i][1], fp));
    }
    if (t == 172) {
        fk_cell_map_close(fk_walk(fk_node[i][1], fp) >> 1);
        return 1 << 1;
    }
    if (t == 196) {
        /* field_reset: unlink every field segment; the kernels alive keep their mappings, the next kernel starts a fresh field */
        fk_field_unlink_all();
        return 1 << 1;
    }
    if (t == 173) {
        return fk_host_cpu_busy_us() << 1;
    }
    if (t == 174) {
        struct timespec fk_tl;
        clock_gettime(CLOCK_MONOTONIC, &fk_tl);
        return fk_gpu_step((long long)fk_tl.tv_sec * 1000000 + (long long)fk_tl.tv_nsec / 1000) << 1;
    }
    if (t == 175) {
        struct timespec fk_tg;
        clock_gettime(CLOCK_MONOTONIC, &fk_tg);
        fk_gpu_step((long long)fk_tg.tv_sec * 1000000 + (long long)fk_tg.tv_nsec / 1000);
        return fk_gpu_busy_total_us << 1;
    }
    if (t == 176) {
        struct timespec fk_tc;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &fk_tc);
        return ((long long)fk_tc.tv_sec * 1000000 + (long long)fk_tc.tv_nsec / 1000) << 1;
    }
    if (t == 182) {
        struct timespec fk_ts;
        clock_gettime(CLOCK_MONOTONIC, &fk_ts);
        return ((long long)fk_ts.tv_sec * 1000 + (long long)fk_ts.tv_nsec / 1000000) << 1;
    }
    if (t == 183) {
        /* host_sleep_ms n: rest n ms landing within half a millisecond; host_sleep_ms (list n watch...): rest at most n ms,
         * waking early when a watched gift frame's seq word moves -- the glass's wait door (see fk_rest_arm) */
        return fk_rest_arm(fk_walk(fk_node[i][1], fp));
    }
    /* THE GIFT FRAME (tags 184-189): a frame in process shared memory that one
     * process offers and any other receives with no latency and no polling of a
     * filesystem. Layout: 16-byte header -- seq (8, atomic; even = stable, odd =
     * a give in flight) and len (8) -- then the payload. A give writes seq odd,
     * copies, writes len, writes seq even; a read takes seq, copies, re-reads seq
     * and retries while the two differ or seq is odd, bounded, so a reader never
     * carries a torn frame. Offered is not demanded: the gift stands whether or
     * not anyone reads it, and release unmaps without unlinking, so the frame
     * stays for the next receiver. Names are POSIX shm names ("/..."); macOS
     * bounds them at 31 bytes, and a name past the bound refuses by name.
     * Handles live in a table that grows (birth 8), never a wall. One arm per
     * tag, as every host door here is written, so the native-surface lens that
     * reads `if (t == N)` sees each of the six. 190 was the first pick and is
     * FK_TAG_CONST_HOLD, the once-hold for a top-level let -- a #define no
     * optable row and no `t == N` site names; the collision walked the first
     * child and answered it (2026-09-05, the third such collision in a week). */
    if (t == 184) {
        static char gname184[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), gname184, FK_PATH_CAP);
        return fk_gift_open(gname184, fk_walk(fk_node[i][2], fp) >> 1, 1);
    }
    if (t == 185) {
        static char gname185[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), gname185, FK_PATH_CAP);
        return fk_gift_open(gname185, 0, 0);
    }
    if (t == 186) {
        long long gh = fk_walk(fk_node[i][1], fp) >> 1;
        long long sv = fk_walk(fk_node[i][2], fp);
        if (!fk_gift_live(gh) || !fk_is_str(sv)) {
            return fk_nothing;
        }
        volatile long long *gseq = (volatile long long *)fk_gift_base[gh];
        volatile long long *glen = gseq + 1;
        char *gpay = (char *)fk_gift_base[gh] + 16;
        long long si = fk_stri(sv);
        long long n = FK_SLEN(si);
        if (n > fk_gift_size[gh] - 16) {
            /* a frame that does not fit is refused, never truncated silently */
            return fk_nothing;
        }
        long long s0 = __atomic_load_n(gseq, __ATOMIC_ACQUIRE);
        __atomic_store_n(gseq, s0 + 1, __ATOMIC_RELEASE);
        { long long k = 0; while (k < n) { gpay[k] = FK_SBYTES(si)[k]; k = k + 1; } }
        __atomic_store_n(glen, n, __ATOMIC_RELEASE);
        __atomic_store_n(gseq, s0 + 2, __ATOMIC_RELEASE);
        return (s0 + 2) << 1;
    }
    if (t == 187) {
        long long gh = fk_walk(fk_node[i][1], fp) >> 1;
        if (!fk_gift_live(gh)) {
            return fk_nothing;
        }
        volatile long long *gseq = (volatile long long *)fk_gift_base[gh];
        volatile long long *glen = gseq + 1;
        char *gpay = (char *)fk_gift_base[gh] + 16;
        int tries = 0;
        for (;;) {
            long long s1 = __atomic_load_n(gseq, __ATOMIC_ACQUIRE);
            if (s1 == 0) {
                return fk_nothing;
            }
            if ((s1 & 1) == 0) {
                long long n = __atomic_load_n(glen, __ATOMIC_ACQUIRE);
                if (n < 0 || n > fk_gift_size[gh] - 16) {
                    return fk_nothing;
                }
                fk_sinit();
                while (fk_sbp + n > fk_scap_b) {
                    fk_scap_b = fk_scap_b * 2;
                    fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
                    fk_sb_check();
                }
                { long long k = 0; while (k < n) { fk_sb[fk_sbp + k] = gpay[k]; k = k + 1; } }
                long long s2 = __atomic_load_n(gseq, __ATOMIC_ACQUIRE);
                if (s1 == s2) {
                    return fk_strv(fk_sintern(fk_sbp, n));
                }
            }
            tries = tries + 1;
            if (tries > 4096) {
                /* a give that never settles is reported, never read torn */
                return fk_nothing;
            }
        }
    }
    if (t == 188) {
        long long gh = fk_walk(fk_node[i][1], fp) >> 1;
        if (!fk_gift_live(gh)) {
            return fk_nothing;
        }
        long long s = __atomic_load_n((volatile long long *)fk_gift_base[gh], __ATOMIC_ACQUIRE);
        return (s & 1) ? ((s - 1) << 1) : (s << 1);
    }
    if (t == 189) {
        long long gh = fk_walk(fk_node[i][1], fp) >> 1;
        if (!fk_gift_live(gh)) {
            return fk_nothing;
        }
        munmap(fk_gift_base[gh], (size_t)fk_gift_size[gh]);
        fk_gift_base[gh] = 0;
        return 1 << 1;
    }
    if (t == 62) {
        static char p[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, FK_PATH_CAP);
        long long off = fk_walk(fk_node[i][2], fp) >> 1;
        long long len = fk_walk(fk_node[i][3], fp) >> 1;
        if (len <= 0) {
            return fk_sbuf("", 0);
        }
        int fd = open(p, O_RDBIN);
        if (fd < 0) {
            /* a file that never was answers nothing, matching read_file (t==63) */
            return fk_nothing;
        }
        lseek(fd, off, 0);
        fk_sinit();
        while (fk_sbp + len > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
            fk_sb_check();
        }
        long long got = read(fd, fk_sb + fk_sbp, len);
        close(fd);
        if (got < 0) {
            /* a read ERROR is not a short slice: no byte was measured — nothing,
             * never a silent zero-length truncation (2026-08-27) */
            return fk_nothing;
        }
        return fk_strv(fk_sintern(fk_sbp, got));
    }
    if (t == 63) {
        static char p[FK_PATH_CAP];
        static long long fk_nreads;
        long long pv63 = fk_walk(fk_node[i][1], fp);
        fk_cstr(pv63, p, FK_PATH_CAP);
        fk_nreads = fk_nreads + 1;
        int fd = open(p, O_RDBIN);
        if (fd < 0) {
            if (fk_conf("FK_READ_WITNESS")) {
                long long sa63 = fk_stri(pv63);
                dprintf(2, "[read_file] OPEN FAILED at read #%lld: '%s' (handle=%lld sa=%lld sl=%lld so=%lld sp=%lld)\n",
                        fk_nreads, p, pv63, sa63, (sa63 >= 0 && FK_SOK(sa63)) ? FK_SLEN(sa63) : -1,
                        (sa63 >= 0 && FK_SOK(sa63)) ? FK_SO(sa63) : -1, fk_sp);
            }
            /* A file that never was answers the axiom-1 nothing, never "" —
             * "" means the file EXISTS and holds zero bytes. The Go arm's
             * read_file already answered VNull here (main.go readFileTextNative);
             * this heals a live cross-arm divergence (2026-08-27). */
            return fk_nothing;
        }
        fk_sinit();
        long long base = fk_sbp;
        long long total = 0;
        long long rerr63 = 0;
        for (;;) {
            while (base + total + 65536 > fk_scap_b) {
                fk_scap_b = fk_scap_b * 2;
                void *sb0 = fk_sb;
                fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
                fk_sb_check();
                if (fk_conf("FK_READ_WITNESS")) {
                    dprintf(2, "[read_file] pool grow -> %lld bytes, %p -> %p (sbp=%lld sp=%lld)\n", fk_scap_b, sb0, (void *)fk_sb, fk_sbp, fk_sp);
                }
            }
            long long got = read(fd, fk_sb + base + total, 65536);
            if (got < 0) {
                rerr63 = 1;
                break;
            }
            if (got == 0) {
                break;
            }
            total = total + got;
        }
        close(fd);
        if (rerr63 && total == 0) {
            /* Opened but not one byte ever measured (a directory, an unreadable
             * device): no content was witnessed — nothing, matching the Go arm.
             * A mid-read error after bytes arrived stays a named cousin
             * (partial-as-whole) for its own movement. */
            return fk_nothing;
        }
        return fk_strv(fk_sintern(base, total));
    }
    if (t == 64) {
        long long xs64 = fk_walk(fk_node[i][1], fp);
        fk_rp = fk_rp + 1;
        fk_record_reserve(fk_rp + 1);
        fk_rcnt[fk_rp] = 0;
        fk_rbp[fk_rp] = 0;
        long long q64 = xs64 >> 1;
        while (q64 >= 1 && FK_POK(q64)) {
            long long e64 = FK_HH(q64);
            long long ep64 = e64 >> 1;
            if (ep64 >= 1 && FK_POK(ep64)) {
                long long k64 = fk_stri(FK_HH(ep64));
                long long tp64 = FK_HT(ep64) >> 1;
                long long v64 = 0;
                if (tp64 >= 1 && FK_POK(tp64)) {
                    v64 = FK_HH(tp64);
                }
                if (k64 == -1) {
                    fk_rbp[fk_rp] = v64;
                } else {
                    fk_record_keys_reserve(fk_rp, fk_rcnt[fk_rp] + 1);
                    fk_rkey[fk_rp][fk_rcnt[fk_rp]] = k64;
                    fk_rval[fk_rp][fk_rcnt[fk_rp]] = v64;
                    fk_rcnt[fk_rp] = fk_rcnt[fk_rp] + 1;
                }
            }
            q64 = FK_HT(q64) >> 1;
        }
        return fk_rbox(fk_rp);
    }
    if (t == 65) {
        long long r = fk_ridx(fk_walk(fk_node[i][1], fp));
        long long key = fk_stri(fk_walk(fk_node[i][2], fp));
        if (r < 1 || r > fk_rp) {
            return 0;
        }
        long long j = 0;
        while (j < fk_rcnt[r]) {
            if (fk_keyeq(fk_rkey[r][j], key)) {
                return fk_rval[r][j];
            }
            j = j + 1;
        }
        return 0;
    }
    if (t == 66) {
        long long rec = fk_walk(fk_node[i][1], fp); fk_vp(rec);
        long long r = fk_ridx(rec);
        long long wk66 = fk_walk(fk_node[i][2], fp); fk_vp(wk66); long long key = fk_stri(wk66);
        long long val = fk_walk(fk_node[i][3], fp); fk_vsp = fk_vsp - 2;
        if (r < 1 || r > fk_rp) {
            return 0;
        }
        long long j = 0;
        while (j < fk_rcnt[r]) {
            if (fk_keyeq(fk_rkey[r][j], key)) {
                fk_rval[r][j] = val;
                return rec;
            }
            j = j + 1;
        }
        fk_record_keys_reserve(r, fk_rcnt[r] + 1);
        fk_rkey[r][fk_rcnt[r]] = key;
        fk_rval[r][fk_rcnt[r]] = val;
        fk_rcnt[r] = fk_rcnt[r] + 1;
        return rec;
    }
    if (t == 67) {
        long long r = fk_ridx(fk_walk(fk_node[i][1], fp));
        long long key = fk_stri(fk_walk(fk_node[i][2], fp));
        if (r < 1 || r > fk_rp) {
            return 0;
        }
        long long j = 0;
        while (j < fk_rcnt[r]) {
            if (fk_keyeq(fk_rkey[r][j], key)) {
                return 2;
            }
            j = j + 1;
        }
        return 0;
    }
    if (t == 68) {
        if (fk_isrec(fk_walk(fk_node[i][1], fp))) {
            return 2;
        }
        return 0;
    }
    if (t == 99) {
        long long r = fk_ridx(fk_walk(fk_node[i][1], fp));
        long long out = 1;
        if (r < 1 || r > fk_rp) {
            return out;
        }
        long long j = fk_rcnt[r];
        while (j > 0) {
            j = j - 1;
            if (fk_hp + 1 >= fk_cap) {
                /* was a silent partial return -- the keys list simply ended
                 * where the heap did. Growth keeps the list whole. */
                fk_heap_grow();
            }
            fk_hp = fk_hp + 1;
            fk_hh[fk_hp] = fk_strv(fk_rkey[r][j]);
            fk_ht[fk_hp] = out;
            out = (fk_hp << 1) | 1;
        }
        return out;
    }
    if (t == 101) {
        return fk_tempdir();
    }
    if (t == 104) {
        static char p[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, FK_PATH_CAP);
        long long sv104 = fk_walk(fk_node[i][2], fp);
        long long sa104 = fk_stri(sv104);
        if (sa104 < 0 || !FK_SOK(sa104)) {
            return -2;
        }
        long long n104 = FK_SLEN(sa104);
        int fd104 = open(p, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd104 < 0) {
            return -2;
        }
        long long wr104 = 0;
        while (wr104 < n104) {
            /* FK_SBYTES, never fk_sb + FK_SO: a string interned in the host-wide field lives
             * in the field's arena, and its offset over the local pool wrote NUL bytes
             * (witnessed 2026-09-06: every FORMBIN2 malformed vector the conformance gate
             * wrote through this door reached the kernels as zeros -- "bad magic" 9 of 12) */
            long long w104 = write(fd104, FK_SBYTES(sa104) + wr104, n104 - wr104);
            if (w104 <= 0) {
                break;
            }
            wr104 = wr104 + w104;
        }
        close(fd104);
        if (wr104 < 0) {
            return -2;
        }
        return wr104 << 1;
    }
    if (t == 105) {
        return fk_http_get_native(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp),
                                  fk_walk(fk_node[i][3], fp));
    }
    if (t == 118) {
        return fk_sock_request(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp),
                               fk_walk(fk_node[i][3], fp));
    }
    if (t == 119) {
        return fk_tls_request(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp),
                              fk_walk(fk_node[i][3], fp));
    }
    if (t == 136) {
        return fk_host_exec(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp));
    }
    if (t == 106) {
        long long base106 = fk_walk(fk_node[i][1], fp);
        fk_vp(base106);
        long long key106 = fk_walk(fk_node[i][2], fp);
        fk_vsp = fk_vsp - 1;
        return fk_get_value(fk_vs[fk_vsp], key106);
    }
    if (t == 107) {
        return fk_file_mtime(fk_walk(fk_node[i][1], fp));
    }
    if (t == 108) {
        long long s108 = fk_walk(fk_node[i][1], fp);
        fk_vp(s108);
        long long f108 = fk_walk(fk_node[i][2], fp);
        fk_vp(f108);
        long long c108 = fk_walk(fk_node[i][3], fp);
        fk_vsp = fk_vsp - 2;
        return fk_scan_run(fk_vs[fk_vsp], fk_vs[fk_vsp + 1], c108);
    }
    if (t == 132) {
#ifdef FK_HAVE_DIRENT_HEADER
        if (fk_cap == 0) {
            fk_arena();
        }
        static char fkl_p[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), fkl_p, FK_PATH_CAP);
        void *fkl_d = opendir(fkl_p);
        if (fkl_d == 0) {
            return 1;
        }
        static char fkl_nb[1048576];
        static long long fkl_no[16384];
        static long long fkl_nl[16384];
        long long fkl_nc = 0;
        long long fkl_bp = 0;
        while (1) {
            struct dirent *fkl_de = readdir(fkl_d);
            if (fkl_de == 0) {
                break;
            }
            char *fkl_nm = fkl_de->d_name;
            if (fkl_nm[0] == FK_CH_DOT && fkl_nm[1] == 0) {
                continue;
            }
            if (fkl_nm[0] == FK_CH_DOT && fkl_nm[1] == FK_CH_DOT && fkl_nm[2] == 0) {
                continue;
            }
            long long fkl_L = 0;
            while (fkl_nm[fkl_L] != 0) {
                fkl_L = fkl_L + 1;
            }
            if (fkl_nc >= 16384) {
                break;
            }
            if (fkl_bp + fkl_L + 1 > 1048576) {
                break;
            }
            fkl_no[fkl_nc] = fkl_bp;
            fkl_nl[fkl_nc] = fkl_L;
            long long fkl_j = 0;
            while (fkl_j < fkl_L) {
                fkl_nb[fkl_bp] = fkl_nm[fkl_j];
                fkl_bp = fkl_bp + 1;
                fkl_j = fkl_j + 1;
            }
            fkl_nb[fkl_bp] = 0;
            fkl_bp = fkl_bp + 1;
            fkl_nc = fkl_nc + 1;
        }
        closedir(fkl_d);
        static long long fkl_ix[16384];
        long long fkl_k = 0;
        while (fkl_k < fkl_nc) {
            fkl_ix[fkl_k] = fkl_k;
            fkl_k = fkl_k + 1;
        }
        long long fkl_a = 1;
        while (fkl_a < fkl_nc) {
            long long fkl_key = fkl_ix[fkl_a];
            long long fkl_b = fkl_a - 1;
            while (fkl_b >= 0) {
                char *fkl_x = fkl_nb + fkl_no[fkl_ix[fkl_b]];
                char *fkl_y = fkl_nb + fkl_no[fkl_key];
                long long fkl_c = 0;
                while (fkl_x[fkl_c] != 0 && fkl_x[fkl_c] == fkl_y[fkl_c]) {
                    fkl_c = fkl_c + 1;
                }
                if (((unsigned char)fkl_x[fkl_c]) <= ((unsigned char)fkl_y[fkl_c])) {
                    break;
                }
                fkl_ix[fkl_b + 1] = fkl_ix[fkl_b];
                fkl_b = fkl_b - 1;
            }
            fkl_ix[fkl_b + 1] = fkl_key;
            fkl_a = fkl_a + 1;
        }
        /* Ensure the arena can hold one pair per entry BEFORE any result
         * value exists: fk_melt here is safe (no arena-value C-locals are
         * live yet; every root is on the traced stacks) and converges (a
         * repeat melt with an unchanged live set cannot free or grow more).
         * The old guard returned the partial list on exhaustion -- a
         * directory read whose answer depended on how much arena earlier
         * calls had spent (the same dir read 3946 entries in one program
         * and 379 in another). fk_cons_val keeps the last resort honest:
         * heap exhausted dies; a partial listing is never accepted as
         * whole (same law as fk_cons_val's own message). */
        long long fkl_need = fkl_nc + fkl_nc + 64;
        if (fk_cap - fk_hp < fkl_need) {
            fk_melt_want = fkl_need;
            fk_melt();
            fk_melt_want = 0;
        }
        long long fkl_out = 1;
        long long fkl_m = fkl_nc;
        while (fkl_m > 0) {
            fkl_m = fkl_m - 1;
            long long fkl_si = fkl_ix[fkl_m];
            fkl_out = fk_cons_val(fk_sbuf(fkl_nb + fkl_no[fkl_si], fkl_nl[fkl_si]), fkl_out);
        }
        return fkl_out;
#else
        return 1;
#endif
    }
    if (t == 133) {
        static char p70[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), p70, FK_PATH_CAP);
        int fd70 = open(p70, 0);
        return ((long long)fd70) << 1;
    }
    if (t == 134) {
        long long fd71 = fk_walk(fk_node[i][1], fp) >> 1;
        long long max71 = fk_walk(fk_node[i][2], fp) >> 1;
        if (fd71 < 0 || max71 <= 0) {
            return fk_sbuf("", 0);
        }
        fk_sinit();
        while (fk_sbp + max71 > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
            fk_sb_check();
        }
        long long got71 = read((int)fd71, fk_sb + fk_sbp, max71);
        if (got71 <= 0) {
            return fk_sbuf("", 0);
        }
        return fk_strv(fk_sintern(fk_sbp, got71));
    }
    if (t == 135) {
        long long fd72 = fk_walk(fk_node[i][1], fp) >> 1;
        if (fd72 < 0) {
            return -2;
        }
        return ((long long)close((int)fd72)) << 1;
    }
    if (t == 203) {
        return fk_metal_matvec_fixture_native();
    }
    if (t == 204) {
        long long m204 = fk_walk(fk_node[i][1], fp);
        fk_vp(m204);
        long long k204 = fk_walk(fk_node[i][2], fp);
        fk_vp(k204);
        long long b204 = fk_walk(fk_node[i][3], fp);
        fk_vsp = fk_vsp - 2;
        return fk_metal_matvec_f32_native(fk_vs[fk_vsp], fk_vs[fk_vsp + 1], b204);
    }
    /* Handle door. Where two heap values are live across a second fk_walk the
     * first is pushed with fk_vp, same as tag 204 above — an argument expression
     * can allocate, and a collection between the two walks would otherwise move
     * a string the carrier is about to read. */
    if (t == 245) {
        long long m245 = fk_walk(fk_node[i][1], fp);
        fk_vp(m245);
        long long n245 = fk_walk(fk_node[i][2], fp);
        fk_vsp = fk_vsp - 1;
        return fk_metal_pipeline_native(fk_vs[fk_vsp], n245) << 1;
    }
    if (t == 246) {
        return fk_metal_buf_alloc_native(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 247) {
        long long p247 = fk_walk(fk_node[i][1], fp);
        fk_vp(p247);
        long long o247 = fk_walk(fk_node[i][2], fp) >> 1;
        long long l247 = fk_walk(fk_node[i][3], fp) >> 1;
        fk_vsp = fk_vsp - 1;
        return fk_metal_buf_from_file_native(fk_vs[fk_vsp], o247, l247) << 1;
    }
    if (t == 248) {
        long long h248 = fk_walk(fk_node[i][1], fp) >> 1;
        long long o248 = fk_walk(fk_node[i][2], fp) >> 1;
        long long b248 = fk_walk(fk_node[i][3], fp);
        return fk_metal_buf_write_native(h248, o248, b248) << 1;
    }
    if (t == 249) {
        long long p249 = fk_walk(fk_node[i][1], fp) >> 1;
        long long b249 = fk_walk(fk_node[i][2], fp);
        fk_vp(b249);
        long long n249 = fk_walk(fk_node[i][3], fp) >> 1;
        fk_vsp = fk_vsp - 1;
        return fk_metal_enqueue_native(p249, fk_vs[fk_vsp], n249) << 1;
    }
    if (t == 250) {
        return fk_metal_sync_native() << 1;
    }
    if (t == 251) {
        long long h251 = fk_walk(fk_node[i][1], fp) >> 1;
        long long o251 = fk_walk(fk_node[i][2], fp) >> 1;
        long long l251 = fk_walk(fk_node[i][3], fp) >> 1;
        return fk_metal_buf_read_native(h251, o251, l251);
    }
    if (t == 252) {
        return fk_metal_status_native();
    }
    if (t == 253) {
        return fk_metal_batch_concurrent_native() << 1;
    }
    if (t == 254) {
        return fk_metal_buf_free_native(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 255) {
        return fk_metal_submit_native() << 1;
    }
    /* 142 and not 256: the walker's tag space ends at FK_OPCODE_ARM_CAP (256), so
     * 255 is the last tag that exists — 256 died as "corrupt node tag" at the first
     * call. The op table lists ops; FK_OPCODE_ARM_CAP bounds them. */
    if (t == 142) {
        return fk_metal_fence_wait_native(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 148) {
        return fk_metal_deadline_native(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 143) {
        return fk_mlx_status_native();
    }
    if (t == 144) {
        long long a144 = fk_walk(fk_node[i][1], fp) >> 1;
        long long b144 = fk_walk(fk_node[i][2], fp) >> 1;
        return fk_mlx_add_native(a144, b144) << 1;
    }
    if (t == 145) {
        return fk_mlx_run_native(fk_walk(fk_node[i][1], fp)) << 1;
    }
    if (t == 205) {
        return fk_mic_count() << 1;
    }
    if (t == 206) {
        return fk_cam_count() << 1;
    }
    if (t == 207) {
        return fk_mic_name(fk_walk(fk_node[i][1], fp) >> 1);
    }
    if (t == 208) {
        return fk_cam_name(fk_walk(fk_node[i][1], fp) >> 1);
    }
    if (t == 209) {
        return fk_mic_health(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 210) {
        return fk_cam_health(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 211) {
        return fk_sense_report() << 1;
    }
    if (t == 212) {
        return fk_cam_grab(fk_walk(fk_node[i][1], fp) >> 1, "fkwu-cam-frame.bmp") << 1;
    }
    if (t == 213) {
        return fk_frame_read("fkwu-cam-frame.bmp") << 1;
    }
    if (t == 214) {
        return fk_sense_stream(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 215) {
        long long program215 = fk_walk(fk_node[i][1], fp);
        fk_vp(program215);
        long long root215 = fk_walk(fk_node[i][2], fp);
        fk_vp(root215);
        long long arg215 = fk_walk(fk_node[i][3], fp);
        fk_vsp = fk_vsp - 2;
        return fk_native_call_arm64_u32_leaf(fk_vs[fk_vsp], fk_vs[fk_vsp + 1], arg215);
    }
    if (t == 146) {
        long long image146 = fk_walk(fk_node[i][1], fp);
        fk_vp(image146);
        {
            long long arg146 = fk_walk(fk_node[i][2], fp);
            fk_vsp = fk_vsp - 1;
            return fk_jit_leaf_inram(fk_vs[fk_vsp], arg146);
        }
    }
    if (t == 216) {
        return fk_wifi_ssid();
    }
    if (t == 217) {
        return fk_wifi_signal() << 1;
    }
    if (t == 218) {
        return fk_bt_present() << 1;
    }
    if (t == 219) {
        return fk_bt_count() << 1;
    }
    if (t == 220) {
        return fk_power() << 1;
    }
    if (t == 221) {
        return fk_memload() << 1;
    }
    if (t == 222) {
        return fk_sensors_report() << 1;
    }
    if (t == 223) {
        return fk_sense_publish(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 224) {
        return fk_mesh_serve(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 225) {
        return fk_mesh_announce(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 226) {
        return fk_mesh_discover(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 227) {
        return fk_api_health() << 1;
    }
    if (t == 228) {
        return fk_mesh_register() << 1;
    }
    if (t == 229) {
        return fk_mesh_detect() << 1;
    }
    if (t == 230) {
        return fk_mesh_registry(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 231) {
        return fk_mesh_roster() << 1;
    }
    if (t == 232) {
        return fk_cuda_matvec() << 1;
    }
    if (t == 233) {
        long long w233 = fk_walk(fk_node[i][1], fp);
        fk_vp(w233);
        long long x233 = fk_walk(fk_node[i][2], fp);
        fk_vsp = fk_vsp - 1;
        return fk_cuda_matvec_f32(fk_vs[fk_vsp], x233);
    }
    if (t == 234) {
        return fk_mic_capture(fk_walk(fk_node[i][1], fp) >> 1);
    }
    if (t == 235) {
        return fk_cam_luma(fk_walk(fk_node[i][1], fp) >> 1);
    }
    if (t == 236) {
        return fk_audio_loopback(fk_walk(fk_node[i][1], fp) >> 1);
    }
    if (t == 139) {
        return fk_mic_stream_start() << 1;
    }
    if (t == 140) {
        long long mb140 = fk_walk(fk_node[i][1], fp) >> 1;
        long long wm140 = fk_walk(fk_node[i][2], fp) >> 1;
        return fk_mic_stream_read(mb140, wm140);
    }
    if (t == 141) {
        return fk_mic_stream_stop() << 1;
    }
    if (t == 237) {
        static char p237a[FK_PATH_CAP];
        static char p237b[FK_PATH_CAP];
        long long a237 = fk_walk(fk_node[i][1], fp);
        fk_vp(a237);
        long long b237 = fk_walk(fk_node[i][2], fp);
        fk_vsp = fk_vsp - 1;
        fk_cstr(fk_vs[fk_vsp], p237a, FK_PATH_CAP);
        fk_cstr(b237, p237b, FK_PATH_CAP);
        return fk_wav_loopback(p237a, p237b);
    }
    if (t == 200) {
        static char p200[FK_PATH_CAP];
        fk_cstr(fk_walk(fk_node[i][1], fp), p200, FK_PATH_CAP);
        return fk_path_is_dir(p200) ? 2 : 0;
    }
    if (t == 202) {
        static char r202[FK_PATH_CAP];
        static char s202[256];
        fk_cstr(fk_walk(fk_node[i][1], fp), r202, FK_PATH_CAP);
        fk_cstr(fk_walk(fk_node[i][2], fp), s202, 256);
        fk_inv_reset();
        fk_inv_walk(r202, r202, s202, fk_walk(fk_node[i][3], fp));
        return fk_inv_rows;
    }
    if (t == 200) {
        return 0;
    }
    if (t == 202) {
        return 1;
    }
    if (t == 120) {
        return fk_socket_listen_native(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 121) {
        return fk_socket_port_native(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 122) {
        return fk_socket_accept_native(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 123) {
        long long host123 = fk_walk(fk_node[i][1], fp);
        fk_vp(host123);
        long long port123 = fk_walk(fk_node[i][2], fp) >> 1;
        fk_vsp = fk_vsp - 1;
        return fk_socket_connect_native(fk_vs[fk_vsp], port123) << 1;
    }
    if (t == 124) {
        long long h124 = fk_walk(fk_node[i][1], fp) >> 1;
        long long bytes124 = fk_walk(fk_node[i][2], fp);
        return fk_socket_send_native(h124, bytes124) << 1;
    }
    if (t == 125) {
        long long h125 = fk_walk(fk_node[i][1], fp) >> 1;
        long long max125 = fk_walk(fk_node[i][2], fp) >> 1;
        return fk_socket_recv_native(h125, max125);
    }
    if (t == 126) {
        return fk_socket_close_native(fk_walk(fk_node[i][1], fp) >> 1) << 1;
    }
    if (t == 97) {
        long long r = fk_ridx(fk_walk(fk_node[i][1], fp));
        long long key = fk_stri(fk_walk(fk_node[i][2], fp));
        if (r < 1 || r > fk_rp) {
            return 0;
        }
        long long j = 0;
        while (j < fk_rcnt[r]) {
            if (fk_keyeq(fk_rkey[r][j], key)) {
                return fk_rval[r][j];
            }
            j = j + 1;
        }
        return 0;
    }
    if (t == 98) {
        long long rec = fk_walk(fk_node[i][1], fp); fk_vp(rec);
        long long r = fk_ridx(rec);
        long long wk98 = fk_walk(fk_node[i][2], fp); fk_vp(wk98); long long key = fk_stri(wk98);
        long long val = fk_walk(fk_node[i][3], fp); fk_vsp = fk_vsp - 2;
        if (r < 1 || r > fk_rp) {
            return 0;
        }
        long long j = 0;
        while (j < fk_rcnt[r]) {
            if (fk_keyeq(fk_rkey[r][j], key)) {
                fk_rval[r][j] = val;
                return 0;
            }
            j = j + 1;
        }
        fk_record_keys_reserve(r, fk_rcnt[r] + 1);
        fk_rkey[r][fk_rcnt[r]] = key;
        fk_rval[r][fk_rcnt[r]] = val;
        fk_rcnt[r] = fk_rcnt[r] + 1;
        return 0;
    }
    if (t == 100) {
        long long r = fk_ridx(fk_walk(fk_node[i][1], fp));
        if (r < 1 || r > fk_rp) {
            return 0;
        }
        return fk_rbp[r];
    }
    if (t == 197) {
        /* method_define bp "name" fn -> bp. The third arg must be a FUNCTION
         * VALUE (a bare defn name in value position rides tag 243); anything
         * else dies loud, matching the sibling walkers' refusal — a method
         * table holding a non-function would dispatch to nonsense later,
         * far from the wound. Re-defining a (blueprint, name) replaces. */
        long long bp197 = fk_walk(fk_node[i][1], fp); fk_vp(bp197);
        long long wn197 = fk_walk(fk_node[i][2], fp); fk_vp(wn197); long long nm197 = fk_stri(wn197);
        long long fv197 = fk_walk(fk_node[i][3], fp); fk_vsp = fk_vsp - 2;
        if (nm197 < 0) {
            fk_die("fk_walk tag 197: method_define second arg must be a string name -- a non-string interns to the -1 sentinel and every such method would collide on it");
        }
        if (fk_is_fnval(fv197) == 0) {
            fk_die("fk_walk tag 197: method_define third arg must be a function value (a defn name in value position)");
        }
        if (fk_fnval_is_closure(fv197)) {
            /* method_invoke's own dispatch (tag 199, fi199 = fk_mth_fn[m199]; i = fk_fn[fi199])
             * jumps straight into fk_fn[] by raw index -- it does not go through
             * fk_fnval_target, so a closure instance's captures would never be delivered and
             * a raw closure-instance number would misread as an ordinary (and almost certainly
             * out-of-range) fn-idx. Not yet wired; die loudly rather than silently dispatch
             * wrong, same as this op already does for a non-function third arg. */
            fk_die("fk_walk tag 197: method_define third arg is a capturing closure, which method "
                   "dispatch does not yet support -- define the method from a non-capturing "
                   "function instead");
        }
        long long m197 = fk_mth_find(bp197, nm197);
        if (m197 < 0) {
            fk_mth_ensure();
            m197 = fk_mth_n;
            fk_mth_n = fk_mth_n + 1;
            fk_mth_bp[m197] = bp197;
            fk_mth_name[m197] = nm197;
        }
        fk_mth_fn[m197] = fk_fnval_idx(fv197);
        return bp197;
    }
    if (t == 198) {
        /* method_has record-or-blueprint "name" -> bool. A record answers for
         * its blueprint; a blueprint answers for itself; anything else is 0. */
        long long v198 = fk_walk(fk_node[i][1], fp); fk_vp(v198);
        long long nm198 = fk_stri(fk_walk(fk_node[i][2], fp)); fk_vsp = fk_vsp - 1;
        long long bp198 = fk_isrec(v198) ? fk_rbp[fk_ridx(v198)] : v198;
        return fk_mth_find(bp198, nm198) >= 0 ? 2 : 0;
    }
    if (t == 199) {
        /* method_invoke rec "name" a1 .. -> value. Dispatch by the record's
         * blueprint; the method's FIRST param binds the receiver (self), the
         * remaining args ride the tag-242 cell chain exactly like tag-241
         * direct calls. A non-record receiver or a missing method dies loud —
         * the sibling kernels panic here, and a nothing'd dispatch would be a
         * numb answer wearing a verdict. */
        long long rv199 = fk_walk(fk_node[i][1], fp);
        if (fk_isrec(rv199) == 0) {
            fk_die("fk_walk tag 199: method_invoke first arg must be a record");
        }
        long long nm199 = fk_stri(fk_walk(fk_node[i][2], fp));
        long long m199 = fk_mth_find(fk_rbp[fk_ridx(rv199)], nm199);
        if (m199 < 0) {
            fk_die("fk_walk tag 199: no method under that name on the record's blueprint (method_define it first)");
        }
        long long fi199 = fk_mth_fn[m199];
        long long base199 = fk_vsp;
        fk_vp(rv199);
        long long cell199 = fk_node[i][3];
        while (cell199 >= 0 && fk_node[cell199][0] == 242) {
            fk_vp(fk_walk(fk_node[cell199][1], fp));
            cell199 = fk_node[cell199][2];
        }
        long long n199 = fk_vsp - base199;
        fk_fn_heat[fi199] = fk_fn_heat[fi199] + 1;
        fk_heat_pulse();
        long long r199 = fk_walk_body(fk_fn[fi199], base199);
        fk_vsp = base199;
        return fk_offer_ack(fi199, n199, r199);
    }
    if (t == 127) {
        fk_live_note(0);
        long long ks_k = fk_walk(fk_node[i][1], fp) >> 1;
        long long ks_n = 256;
        if (ks_k == 0) {
            long long ks_s = 0;
            long long ks_u = 1;
            while (ks_u < ks_n) {
                ks_s = ks_s + fk_arms[ks_u];
                ks_u = ks_u + 1;
            }
            return ks_s << 1;
        }
        if (ks_k == 1) {
            long long ks_d = 0;
            long long ks_u = 1;
            while (ks_u < ks_n) {
                if (fk_arms[ks_u] > 0) {
                    ks_d = ks_d + 1;
                }
                ks_u = ks_u + 1;
            }
            return ks_d << 1;
        }
        if (ks_k == 2) {
            long long ks_bt = 0;
            long long ks_bc = 0;
            long long ks_u = 1;
            while (ks_u < ks_n) {
                if (fk_arms[ks_u] > ks_bc) {
                    ks_bc = fk_arms[ks_u];
                    ks_bt = ks_u;
                }
                ks_u = ks_u + 1;
            }
            return ks_bt << 1;
        }
        if (ks_k == 3) {
            long long ks_bc = 0;
            long long ks_u = 1;
            while (ks_u < ks_n) {
                if (fk_arms[ks_u] > ks_bc) {
                    ks_bc = fk_arms[ks_u];
                }
                ks_u = ks_u + 1;
            }
            return ks_bc << 1;
        }
        if (ks_k == 4) {
            return fk_np << 1;
        }
        if (ks_k == 5) {
            return fk_sp << 1;
        }
        if (ks_k == 51) {
            return fk_smelt_reclaimed << 1;
        }
        if (ks_k == 52) {
            return fk_twin_calls << 1;
        }
        if (ks_k == 53) {
            return fk_mint_total << 1;
        }
        if (ks_k == 6) {
            return fk_hp << 1;
        }
        if (ks_k == 7) {
            return fk_vsp << 1;
        }
        if (ks_k == 8) {
            return fk_fp << 1;
        }
        /* 9..14 -- the framebuffer census, in the vocabulary the sibling
         * table-walker lane already fixed (fkc-table-serialize.fk). The seed
         * carried fk_fbn all along and simply never said it, so the glass had
         * roots arriving and no count of them; 2026-09-07 named the pair 9/11
         * owed and first on the seam line. Paid here. Note 9 counts roots
         * recorded SINCE THE LAST framebuffer-clear (tag 131 zeroes it), while
         * 13 counts every accepted call this process ever made -- 9 below 13
         * is a clear, not a loss. */
        if (ks_k == 9) {
            return fk_fbn << 1;
        }
        if (ks_k == 10) {
            /* nodes carrying a source attribution. A walk of the whole node
             * population, so the caller pays for it by asking -- the glass
             * reads it on its slow cadence, never per frame. */
            long long ks_a = 0;
            long long ks_u = 1;
            while (ks_u <= fk_np) {
                if (fk_nsattr[ks_u] != 0) { ks_a = ks_a + 1; }
                ks_u = ks_u + 1;
            }
            return ks_a << 1;
        }
        if (ks_k == 11) {
            return fk_fbrejected << 1;
        }
        if (ks_k == 12) {
            return fk_fbentered << 1;
        }
        if (ks_k == 13) {
            return fk_fbaccepted << 1;
        }
        if (ks_k == 14) {
            return fk_fblastidx << 1;
        }
        if (ks_k == 15) {
            return fk_run_door << 1;
        }
        if (ks_k == 16) {
            return fk_import_images << 1;
        }
        if (ks_k == 17) {
            return fk_import_carried << 1;
        }
        if (ks_k == 18) {
            return fk_import_refusal << 1;
        }
        /* 19..40: the growth pulses -- value-node table, cons heap, AST
         * table, source text, value stack, binding stack, socket handles,
         * binding save stack, list-print nesting stack, conf entries,
         * conf byte pool (live cap / doublings each). There are no walls
         * any more; a runaway consumer shows as monotone growth. */
        if (ks_k == 19) {
            return fk_node_cap << 1;
        }
        if (ks_k == 20) {
            return fk_node_grows << 1;
        }
        if (ks_k == 21) {
            return fk_cap << 1;
        }
        if (ks_k == 22) {
            return fk_heap_grows << 1;
        }
        if (ks_k == 23) {
            return fk_ast_cap << 1;
        }
        if (ks_k == 24) {
            return fk_ast_grows << 1;
        }
        if (ks_k == 25) {
            return fk_srctext_cap << 1;
        }
        if (ks_k == 26) {
            return fk_srctext_grows << 1;
        }
        if (ks_k == 27) {
            return fk_vs_cap << 1;
        }
        if (ks_k == 28) {
            return fk_vs_grows << 1;
        }
        if (ks_k == 29) {
            return fk_bd_cap << 1;
        }
        if (ks_k == 30) {
            return fk_bd_grows << 1;
        }
        if (ks_k == 31) {
            return fk_sock_cap << 1;
        }
        if (ks_k == 32) {
            return fk_sock_grows << 1;
        }
        if (ks_k == 33) {
            return fk_bd_save_cap << 1;
        }
        if (ks_k == 34) {
            return fk_bd_save_grows << 1;
        }
        if (ks_k == 35) {
            return fk_pv_nest_cap << 1;
        }
        if (ks_k == 36) {
            return fk_pv_nest_grows << 1;
        }
        if (ks_k == 37) {
            return fk_conf_cap << 1;
        }
        if (ks_k == 38) {
            return fk_conf_grows << 1;
        }
        if (ks_k == 39) {
            return fk_conf_pool_cap << 1;
        }
        if (ks_k == 41) {
            /* gift frames mapped in this process (shm_offer/shm_receive handles still standing) */
            long long ks_g = 0, ks_i = 0;
            while (ks_i < fk_gift_count) { if (fk_gift_base[ks_i] != 0) { ks_g = ks_g + 1; } ks_i = ks_i + 1; }
            return ks_g << 1;
        }
        if (ks_k == 42) {
            long long ks_b = 0, ks_i = 0;
            while (ks_i < fk_gift_count) { if (fk_gift_base[ks_i] != 0) { ks_b = ks_b + fk_gift_size[ks_i]; } ks_i = ks_i + 1; }
            return ks_b << 1;
        }
        if (ks_k == 43) {
            return fk_fntop << 1;
        }
        if (ks_k == 45) {
            return fk_box_total << 1;
        }
        if (ks_k == 46) {
            return fk_unbox_total << 1;
        }
        if (ks_k == 47) {
            return fk_inram_call_total << 1;
        }
        if (ks_k == 48) {
            return fk_f64_count << 1;
        }
        if (ks_k == 49) {
            return fk_f64_loop_count << 1;
        }
        if (ks_k == 50) {
            return fk_f64_loop_iters << 1;
        }
        if (ks_k == 44) {
            return fk_heat_total << 1;
        }
        if (ks_k == 40) {
            return fk_conf_pool_grows << 1;
        }
        /* 54..62 -- the root ring, the float pool and the node census by HOME.
         * Every one of these was already standing in this file; none of them
         * had a way out. The node population lives in exactly one of three
         * homes at a time (fk_nodes_init chooses; fk_store_go_private can move
         * it), and those three homes ARE the phases the glass already speaks:
         *   ice   -- the shared field /fg-field-*: content-addressed, shared by
         *            every kernel on the host, outlives all of them;
         *   water -- the per-pid shared store /fg-c<pid>-*: mapped and visible
         *            to siblings, gone when this pid goes;
         *   gas   -- the private heap: RAM only, no other process can see it,
         *            it evaporates at exit. This is the home a host with no
         *            shared memory falls to, so its zero here is a measured
         *            zero and the row says which zero it is.
         * A count is never split across homes: exactly one of 58/59/60 carries
         * the whole population, and which one carries it is the reading. */
        if (ks_k == 54) {
            /* roots STANDING in the ring -- what framebuffer-events can hand
             * back right now. Below 9 means older roots fell out of the window. */
            return (fk_fbn < FK_FB_RING ? fk_fbn : FK_FB_RING) << 1;
        }
        if (ks_k == 55) {
            return ((long long)FK_FB_RING) << 1;
        }
        if (ks_k == 56) {
            /* the float pool's live capacity; 8 is its fill. The pool never
             * reclaims -- fk_fp only ever rises -- so 8 is a high-water, and 45
             * (boxes minted) rises with it slot for slot while 46 counts the
             * reads of those slots, which no pool growth bounds. */
            return fk_fcap << 1;
        }
        if (ks_k == 57) {
            return fk_field_fp() << 1;
        }
        if (ks_k == 58) {
            return (fk_field_on || fk_store_shared) ? 0 : (fk_np << 1);
        }
        if (ks_k == 59) {
            return (!fk_field_on && fk_store_shared) ? (fk_np << 1) : 0;
        }
        if (ks_k == 60) {
            return fk_field_on ? (fk_np << 1) : 0;
        }
        if (ks_k == 61) {
            /* the tissue's own extent: the ten interned columns are 104 bytes a
             * node (kind, cat, kids, val, the four-word id, file, line, col,
             * attr, hash memo), committed page by page as the fill advances. */
            return (fk_np * 104) << 1;
        }
        if (ks_k == 62) {
            /* the RAM this kernel holds privately over that tissue whatever its
             * home: the root ring and hash memo (8 bytes a node each), the inram
             * slot/generation/released columns (6, only where the arm64 JIT
             * witness stands), and the intern index. A body whose every node is
             * ice still pays this in gas to reach it. */
#if defined(FK_HAVE_DARWIN_ARM64_JIT_WITNESS)
            return ((fk_node_cap * 22) + (fk_intern_hash_cap * 8)) << 1;
#else
            return ((fk_node_cap * 16) + (fk_intern_hash_cap * 8)) << 1;
#endif
        }
        if (ks_k >= 100 && ks_k < 100 + ks_n) {
            return fk_arms[ks_k - 100] << 1;
        }
        return 0;
    }
    if (t == 147) {
        /* node_at: the census atom. The value-node table is the body's own
         * tissue; this door hands Form the i-th node so Form-side lenses can
         * walk 1..kernel_stat(4) and fold their own histograms -- blueprint
         * occupancy, allocation sites via node_source -- without the kernel
         * choosing what a census means. Out of range answers nothing. */
        long long ix147 = fk_walk(fk_node[i][1], fp) >> 1;
        if (ix147 < 1 || ix147 > fk_np) {
            return fk_nothing;
        }
        return fk_nbox(ix147);
    }
    if (t == 128) {
        long long fr_nv = fk_walk(fk_node[i][1], fp);
        long long fr_fv = fk_walk(fk_node[i][2], fp);
        long long fr_pk = fk_walk(fk_node[i][3], fp) >> 1;
        long long fr_ni = fk_nidx(fr_nv);
        fk_fbentered = fk_fbentered + 1;
        fk_fblastidx = fr_ni;
        if (fr_ni >= 1 && fr_ni <= fk_np) {
            fk_fbaccepted = fk_fbaccepted + 1;
            fk_nsfile[fr_ni] = fr_fv;
            fk_nsline[fr_ni] = fr_pk >> 16;
            fk_nscol[fr_ni] = fr_pk & 65535;
            fk_nsattr[fr_ni] = 1;
            fk_fbroots[fk_fbn % FK_FB_RING] = fr_nv;
            fk_fbn = fk_fbn + 1;
        } else {
            fk_fbrejected = fk_fbrejected + 1;
        }
        return fr_nv;
    }
    if (t == 129) {
        if (fk_cap == 0) {
            fk_arena();
        }
        if ((fk_hp + (fk_fbn < FK_FB_RING ? fk_fbn : FK_FB_RING) + 4) * 100 >= fk_cap * 90) {
            fk_melt();
        }
        long long fe_r = 1;
        long long fe_n = fk_fbn < FK_FB_RING ? fk_fbn : FK_FB_RING;
        long long fe_i = fk_fbn;
        while (fe_i > fk_fbn - fe_n) {
            fe_i = fe_i - 1;
            if (fk_hp + 1 >= fk_cap) {
                fk_heap_grow();
            }
            fk_hp = fk_hp + 1;
            fk_hh[fk_hp] = fk_fbroots[fe_i % FK_FB_RING];
            fk_ht[fk_hp] = fe_r;
            fe_r = (fk_hp << 1) | 1;
        }
        return fe_r;
    }
    if (t == 130) {
        long long ns_nv = fk_walk(fk_node[i][1], fp);
        long long ns_ni = fk_nidx(ns_nv);
        if (ns_ni < 1 || ns_ni > fk_np || fk_nsattr[ns_ni] == 0) {
            return 1;
        }
        if (fk_cap == 0) {
            fk_arena();
        }
        if ((fk_hp + 6) * 100 >= fk_cap * 90) {
            fk_melt();
        }
        long long ns_c = 1;
        fk_hp = fk_hp + 1;
        fk_hh[fk_hp] = (fk_nscol[ns_ni] << 1);
        fk_ht[fk_hp] = ns_c;
        ns_c = (fk_hp << 1) | 1;
        fk_hp = fk_hp + 1;
        fk_hh[fk_hp] = (fk_nsline[ns_ni] << 1);
        fk_ht[fk_hp] = ns_c;
        ns_c = (fk_hp << 1) | 1;
        fk_hp = fk_hp + 1;
        fk_hh[fk_hp] = fk_nsfile[ns_ni];
        fk_ht[fk_hp] = ns_c;
        ns_c = (fk_hp << 1) | 1;
        return ns_c;
    }
    if (t == 131) {
        long long fc_i = 1;
        while (fc_i <= fk_np) {
            fk_nsattr[fc_i] = 0;
            fc_i = fc_i + 1;
        }
        fk_fbn = 0;
        return 0;
    }
    if (t == 114) {
        char rbuf[8192];
        long long rn = 0;
        while (rn < 8191) {
            char rc;
            long long rg = read(0, &rc, 1);
            if (rg <= 0) {
                if (rn == 0) {
                    /* END OF INPUT IS `nothing`, NOT A NEGATIVE NUMBER.
                     * The old sentinel made every caller ask `(lt line 0)`, and
                     * that question reads the ENCODING: this walker packs a
                     * string as a large negative word (fk_strv) while the
                     * emitted walker packs it as a positive index, so the same
                     * check answered opposite in the two kernels. The form-cli
                     * REPL therefore quit on its first real line under the
                     * source runner and ran fine baked — which is why the CLI
                     * looked like it needed the flatten table at all. Absence
                     * is first-class in this body; say it with the word for
                     * absence and the question becomes `(nothing? line)`,
                     * which no encoding can answer differently. */
                    return fk_nothing;
                }
                break;
            }
            if (rc == FK_CH_LF) {
                break;
            }
            rbuf[rn] = rc;
            rn = rn + 1;
        }
        while (fk_sbp + rn > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
            fk_sb_check();
        }
        long long rj = 0;
        while (rj < rn) {
            fk_sb[fk_sbp + rj] = rbuf[rj];
            rj = rj + 1;
        }
        return fk_strv(fk_sintern(fk_sbp, rn));
    }
    if (t == 115) {
        long long psv = fk_walk(fk_node[i][1], fp);
        long long psa = fk_stri(psv);
        if (psa >= 0 && FK_SOK(psa)) {
            long long pj = 0;
            while (pj < FK_SLEN(psa)) {
                putchar((int)(unsigned char)FK_SBYTES(psa)[pj]);
                pj = pj + 1;
            }
        }
        putchar(10);
        /* print_str is the live event door: a streamed trace line must reach
         * the reader the moment it is spoken, on a pipe as on a tty. stdio
         * block-buffers pipes, so flush every emitted line (fflush(0) needs
         * no FILE type in this freestanding extern set). */
        fflush((void *)0);
        return 0;
    }
    if (t == 239) {
        /* print — the VALUE printer. Mirrors form-kernel-go's registerNative("print")
         * exactly: each operand rendered by its kind, single space BETWEEN operands,
         * one newline after, and the call itself answers nothing.
         *
         * Go writes a.String() per operand (core.Value.String); the kinds a fkwu word
         * can be map onto it one for one now that the string band exists:
         *   string  -> the bytes themselves      (Go VStr  -> v.Str)
         *   float   -> fk_fmt_float_js           (Go VFloat -> FormatFloatJS)
         *   int     -> decimal                   (Go VInt  -> FormatInt base 10)
         *   list    -> "[a, b]", elements likewise (Go VList -> "[" + join ", " + "]")
         * fkwu's `nothing` prints as "nothing" here, which is the word this kernel
         * already answers for it at the verdict boundary; Go's VNull prints "null"
         * and the two kernels do not yet agree on that ONE spelling -- named, not
         * papered over, and reachable only through (nothing), which bin-go cannot
         * currently evaluate at all (it crashes), so no band can witness the gap
         * today. Every other kind is byte-for-byte.
         *
         * The operands arrive as a real list (the parser folds them through cons),
         * so this walks the heap chain -- already evaluated, already in order. */
        long long pv = fk_walk(fk_node[i][1], fp);
        long long pp = pv >> 1;
        long long pfirst = 1;
        while (pp >= 1 && FK_POK(pp)) {
            if (!pfirst) {
                putchar(FK_CH_SPACE);
            }
            pfirst = 0;
            long long pel = FK_HH(pp);
            if (fk_is_output_list(pel)) {
                fk_pv_list(pel);
            } else {
                fk_pv_inline_number(pel);
            }
            pp = FK_HT(pp) >> 1;
        }
        putchar(10);
        /* same live-event door as print_str: a driven emitter's line must reach the
         * reader as it is spoken, and stdio block-buffers a pipe. */
        fflush((void *)0);
        return 0;
    }
    if (t == 116) {
        if (isatty(0)) {
            return 2;
        }
        return 0;
    }
    if (t == 117) {
        if (fk_gen_len <= 0) {
            return fk_strv(fk_sintern(fk_sbp, 0));
        }
        while (fk_sbp + fk_gen_len > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
            fk_sb_check();
        }
        long long gj = 0;
        while (gj < fk_gen_len) {
            fk_sb[fk_sbp + gj] = (char)fk_gen[gj];
            gj = gj + 1;
        }
        return fk_strv(fk_sintern(fk_sbp, fk_gen_len));
    }
    return 0;
}
/* ── SEED bootstrap stone 1: run Form SOURCE directly (the README's "minimal flatten baked into
 * runtime/fkwu-uni.c"). A recursive-descent s-expr parser turns source into the SAME node format
 * fk_run loads, then fk_walk runs it. This is the ONE-TIME bootstrap seed (like the cc command)
 * whose telos is to flatten the FORM flattener (form-flatten.fk) so it self-hosts, then RETIRE. It
 * is NOT body logic that stays in C. BOUNDED — do not grow this into a full C flattener (that is
 * the carrier-last inversion). The flattener IS Form (form-flatten.fk; flatten/README.md "the real
 * flatten body"); the source-runner IS Form (grammars/form-eval.fk runs source off the BMF cursor).
 * Both run native via the JIT. This parser exists ONLY to break the keystone circularity once:
 * parse just enough to flatten form-flatten.fk, hand off, and stop. The cleaner unlock is the
 * PLATFORM-NEUTRAL seed (form-flatten.fk flattened once, on the Mac) — committing that data lets
 * Form flatten Form here with no C parser at all. Stones 1-3 (literals/ops/if, defn+ recursion,
 * do+let) prove the circularity is breakable on Windows; they are the bootstrap, not the
 * destination. Witnessed: fkwu --src run a .fk file. The op vocabulary itself is DATA, not C: there
 * is no per-op if-chain. The (name arity tag) rows and the comparison/boolean rewrites live in the
 * GENERATED runtime/fkwu-optable.h (from flt-ops, the same single source the flattener reads; regen
 * via flatten/gen-source-walker.sh). Adding a value op = a manifest row, never a C edit. Only the
 * control forms defn/do/let/if keep hand-written shape here — their eval semantics are special.
 * Every VALUE form is data: arity-0 ((empty)->18), arity-1/2/3 primitives, and the arity -1
 * VARIADIC sentinel ((list ..)->cons/19). */
#define FK_SOURCE_TEXT_CAP_INIT 8388608 /* fk_srctext: the parsed program's own source text (distinct from fk_src, the staged input buffer). Birth size only -- the buffer DOUBLES on demand (fk_srctext_reserve), so program text is not a wall. History: 262144->8388608 (2026-07-02) after a 267KB generated program was SILENTLY truncated at the old cap and the permissive reader auto-closed the amputated program -- the "N=100 cliff"; then a loud refusal at 8MB; growth since 2026-09-02 (kernel_stat 25/26 = live cap / doublings). */
static char *fk_srctext;
static void fk_srctext_reserve(long long need) {
    long long nc;
    char *q;
    if (fk_srctext_cap == 0) {
        /* the program text lives in this kernel's program surface (/fg-c<pid>-S) when the host offers shared memory */
        fk_srctext = (char *)fk_prog_take('S', FK_PROG_SRC_BYTES);
        fk_prog_src_shared = fk_srctext != 0;
        if (fk_srctext == 0) { fk_srctext = malloc(FK_SOURCE_TEXT_CAP_INIT); }
        if (fk_srctext == 0) {
            fk_die("fk_srctext_reserve: out of memory for the source-text buffer");
        }
        fk_srctext_cap = FK_SOURCE_TEXT_CAP_INIT;
    }
    if (need <= fk_srctext_cap) {
        return;
    }
    nc = fk_srctext_cap;
    while (nc < need) {
        nc = nc * 2;
    }
    if (fk_prog_src_shared && nc <= FK_PROG_SRC_BYTES) {
        fk_srctext_cap = nc;   /* inside the reservation nothing moves */
        fk_srctext_grows = fk_srctext_grows + 1;
        return;
    }
    if (fk_prog_src_shared) {
        fk_srctext = (char *)fk_store_copy_out(fk_srctext, fk_srctext_cap);
        { char nm[32]; fk_store_name('S', (long long)getpid(), nm); shm_unlink(nm); }
        fk_prog_src_shared = 0;
    }
    q = realloc(fk_srctext, (unsigned long)nc);
    if (q == 0) {
        fk_die("fk_srctext_reserve: out of memory growing the source-text buffer");
    }
    fk_srctext = q;
    fk_srctext_cap = nc;
    fk_srctext_grows = fk_srctext_grows + 1;
}
static long long fk_spos;
static long long fk_slen;
/* clang-style "fkwu:line:col: sev: msg". `off` is a byte offset into fk_srctext;
 * a negative off suppresses the coordinate (for artifact-load diagnostics that
 * read fk_buf, not source text). The variadic body delegates to vdprintf so
 * callers pass "%.*s" slices of fk_srctext inline. */
static void fk_diag(int sev, long long off, const char *fmt, ...) {
    if (sev == FK_DIAG_ERR) {
        fk_nerr = fk_nerr + 1;
    } else {
        fk_nwarn = fk_nwarn + 1;
    }
    if (fk_diag_quiet) {
        return;
    }
    if (sev == FK_DIAG_ERR) {
        fk_nerr_seen = fk_nerr_seen + 1;
    } else {
        fk_nwarn_seen = fk_nwarn_seen + 1;
    }
    long long lastnl = -1, col = 0;
    if (off < 0) {
        dprintf(2, "fkwu: %s: ", sev == FK_DIAG_ERR ? "error" : "warning");
    } else {
        long long line = 1, i = 0;
        if (off > fk_slen) {
            off = fk_slen;
        }
        while (i < off) {
            if (fk_srctext[i] == FK_CH_LF) {
                line = line + 1;
                lastnl = i;
            }
            i = i + 1;
        }
        col = off - lastnl; /* lastnl=-1 on line 1 => col = off+1 */
        dprintf(2, "fkwu:%lld:%lld: %s: ", line, col, sev == FK_DIAG_ERR ? "error" : "warning");
    }
    __builtin_va_list ap;
    __builtin_va_start(ap, fmt);
    vdprintf(2, fmt, ap);
    __builtin_va_end(ap);
    dprintf(2, "\n");
    /* Quote the source line itself. Diagnostic positions count lines of the
     * ASSEMBLED unit (preludes expanded), a text no file on disk holds --
     * without the quote, a reported line number sends the reader hunting
     * through files whose numbering can never match (witnessed 2026-08-31:
     * three "phantom" strays hunted across sessions at file-space lines that
     * do not exist). Window the line around the column so the caret always
     * lands inside what is shown. */
    if (off >= 0 && fk_slen > 0) {
        long long ls = lastnl + 1, le = off;
        while (le < fk_slen && fk_srctext[le] != FK_CH_LF) {
            le = le + 1;
        }
        long long ws = ls, caret = col - 1;
        if (caret > 120) {
            ws = off - 120;
            caret = 120;
        }
        long long wn = le - ws;
        if (wn > 160) {
            wn = 160;
        }
        dprintf(2, "  | %.*s\n  | %*s^\n", (int)wn, fk_srctext + ws, (int)caret, "");
    }
}
/* Called ONCE, after parse completes and before execution begins: gcc-style
 * tally. Silent when clean, so the default happy path prints nothing new.
 * Tallies the PRINTED counters, not the per-compile ones: a compile-state
 * reset between passes must never make the tally disagree with what stderr
 * already shows. */
static void fk_diag_flush(void) {
    if (fk_nerr_seen > 0 || fk_nwarn_seen > 0) {
        dprintf(2, "fkwu: %lld error(s), %lld warning(s)\n", fk_nerr_seen, fk_nwarn_seen);
    }
}
/* The heat report: recipes dispatched at least FK_HEAT_REPORT_MIN times are
 * written to .fkwu-heat.<pid> (count and name). The board selects its living
 * resident's PID, so the glass's own short fkwu run cannot erase the work it
 * is observing. A process that has not crossed the threshold writes nothing;
 * once it has, counters are monotone for that process. */
#define FK_HEAT_REPORT_MIN 100000
/* the live pulse: every ~64M dispatches the report rewrites, so a long
 * walker is CAUGHT during flight (the glass reads the file), not
 * discovered after lunch. One file per working directory, last hot
 * writer speaks — a catching signal, not a ledger. */
#define FK_HEAT_PULSE_MASK ((1LL << 26) - 1)
static int fk_heat_reported;

static void fk_heat_write(void) {
    long long top = fk_fn_count;
    char path[96], tmp[112], bpath[96], btmp[112];
    sprintf(path, ".fkwu-heat.%d", getpid());
    sprintf(tmp, ".fkwu-heat.%d.tmp", getpid());
    sprintf(bpath, ".fkwu-boxing.%d", getpid());
    sprintf(btmp, ".fkwu-boxing.%d.tmp", getpid());
    int fd = -1;
    int bfd = -1;
    /* NAME BY THE MAP, NOT BY COINCIDENCE: heat is per fn-INDEX, names
     * live in the symbol table keyed by fk_fnidx[symtop] -> fnidx. On a
     * fresh compile the two spaces coincide and indexing symbols with
     * the fn index happens to print names; on an image load fn_base
     * remapping divorces them and warm runs burned NAMELESS (witnessed
     * 2026-09-02, twice, by two hands -- warmname and mirrorburn healed
     * the same wound the same day). Walk the symbol table and follow
     * the map; any hot fn no symbol names prints as fn#N -- counted
     * work is never blank. Beside the heat board, .fkwu-boxing carries
     * per-recipe float MINTS (fk_fn_fbox): the unboxing worklist -- a
     * recipe hot there allocates a float-pool slot per result where an
     * unboxed lane would not. Printed entries are negate-marked so the
     * numbered sweep never repeats them, then restored. */
    long long j = 0;
    while (j < fk_fntop) {
        long long fx = fk_fnidx[j];
        if (fx >= 0 && fx < top) {
            if (fk_fn_heat[fx] >= FK_HEAT_REPORT_MIN) {
                if (fd < 0) {
                    fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                }
                if (fd >= 0) {
                    dprintf(fd, "%lld %.*s\n", fk_fn_heat[fx],
                            (int)fk_fnsym_n[j], fk_srctext + fk_fnsym_s[j]);
                }
                fk_fn_heat[fx] = 0 - fk_fn_heat[fx] - 1;
            }
            if (fk_fn_fbox != 0 && fk_fn_fbox[fx] >= 1024) {
                if (bfd < 0) {
                    bfd = open(btmp, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                }
                if (bfd >= 0) {
                    dprintf(bfd, "%lld %.*s\n", fk_fn_fbox[fx],
                            (int)fk_fnsym_n[j], fk_srctext + fk_fnsym_s[j]);
                }
                fk_fn_fbox[fx] = 0 - fk_fn_fbox[fx] - 1;
            }
        }
        j = j + 1;
    }
    long long i = 0;
    while (i < top) {
        if (fk_fn_heat[i] >= FK_HEAT_REPORT_MIN) {
            if (fd < 0) {
                fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            }
            if (fd >= 0) {
                dprintf(fd, "%lld fn#%lld\n", fk_fn_heat[i], i);
            }
        }
        if (fk_fn_fbox != 0 && fk_fn_fbox[i] >= 1024) {
            if (bfd < 0) {
                bfd = open(btmp, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            }
            if (bfd >= 0) {
                dprintf(bfd, "%lld fn#%lld\n", fk_fn_fbox[i], i);
            }
        }
        i = i + 1;
    }
    i = 0;
    while (i < top) {
        if (fk_fn_heat[i] < 0) {
            fk_fn_heat[i] = 0 - (fk_fn_heat[i] + 1);
        }
        if (fk_fn_fbox != 0 && fk_fn_fbox[i] < 0) {
            fk_fn_fbox[i] = 0 - (fk_fn_fbox[i] + 1);
        }
        i = i + 1;
    }
    if (fd >= 0) {
        if (close(fd) == 0) {
            if (rename(tmp, path) != 0) {
                unlink(tmp);
            }
        } else {
            unlink(tmp);
        }
    }
    if (bfd >= 0) {
        if (close(bfd) == 0) {
            if (rename(btmp, bpath) != 0) {
                unlink(btmp);
            }
        } else {
            unlink(btmp);
        }
    }
}
static void fk_heat_report(void) {
    if (fk_heat_reported) {
        return;
    }
    fk_heat_reported = 1;
    fk_live_publish(1);
    if (fk_store_shared) { fk_store_unlink_pid((long long)getpid()); }
    fk_heat_write();
}
static void fk_heat_pulse(void) {
    fk_heat_total = fk_heat_total + 1;
    if ((fk_heat_total & FK_HEAT_PULSE_MASK) == 0) {
        fk_heat_write();
    }
}
static int fk_sws(char c) {
    return c == FK_CH_SPACE || c == FK_CH_TAB || c == FK_CH_LF || c == FK_CH_CR;
}
static void fk_sskip(void) {
    while (fk_spos < fk_slen) {
        char c = fk_srctext[fk_spos];
        if (fk_sws(c)) {
            fk_spos = fk_spos + 1;
        } else if (c == FK_CH_SEMI) {
            while (fk_spos < fk_slen && fk_srctext[fk_spos] != FK_CH_LF) {
                fk_spos = fk_spos + 1;
            }
        } else {
            break;
        }
    }
}
static int fk_sym_eq(long long s, long long n, const char *w) {
    long long i = 0;
    while (w[i] != 0) {
        if (i >= n || fk_srctext[s + i] != w[i]) {
            return 0;
        }
        i = i + 1;
    }
    return i == n;
}
static long long fk_arg_s, fk_arg_n, fk_fname_s, fk_fname_n;
/* stone 2: the defn's single arg + fn name (offset,len in srctext) */
static int fk_sym_eq2(long long s1, long long n1, long long s2, long long n2) {
    if (n1 != n2) {
        return 0;
    }
    long long i = 0;
    while (i < n1) {
        if (fk_srctext[s1 + i] != fk_srctext[s2 + i]) {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}
static long long fk_sym_end(long long s) {
    while (s < fk_slen) {
        char d = fk_srctext[s];
        if (fk_sws(d) || d == FK_CH_LPAREN || d == FK_CH_RPAREN) {
            break;
        }
        s = s + 1;
    }
    return s;
}
/* ── DATA-DRIVEN op dispatch — the last C-work made permanent ───────────────── There is no per-op
 * fk_sym_eq head/empty/list chain any more. The (name arity tag) rows and the rewrite rules are
 * DATA: fkwu-optable.h, GENERATED from flt-ops (form-flatten.fk, from native-op-manifest.fk) by
 * flatten/gen-source-walker-table.fk — the SAME single source the flattener reads. Adding a value
 * op is a manifest row + regen, NEVER a C edit. Only the control forms defn/do/let/if stay
 * hand-written (special eval semantics); the string literal "..." is the one non-symbol leaf. Every
 * VALUE form — every primitive, (empty) (arity 0), (list ..) (arity -1 variadic), every
 * comparison/boolean rewrite — flows through these data tables. */
#include "fkwu-optable.h"
/* --feval reads grammars/form-eval.fk at runtime (no embedded blob / no codegen) — see fk_run_feval
 */
/* match a source symbol [s,s+n) against a C string by length-and-bytes. */
static int fk_optname_eq(long long s, long long n, const char *w) {
    long long i = 0;
    while (w[i] != 0) {
        if (i >= n || fk_srctext[s + i] != w[i]) {
            return 0;
        }
        i = i + 1;
    }
    return i == n;
}
/* op-table lookup: source symbol -> row index in fk_optab, or -1. */
static long long fk_optab_find(long long s, long long n) {
    long long i = 0;
    while (i < fk_optab_n) {
        if (fk_optname_eq(s, n, fk_optab[i].name)) {
            return i;
        }
        i = i + 1;
    }
    return -1;
}
/* rewrite-table lookup: source symbol -> row index in fk_rwtab, or -1. */
static long long fk_rwtab_find(long long s, long long n) {
    long long i = 0;
    while (i < fk_rwtab_n) {
        if (fk_optname_eq(s, n, fk_rwtab[i].name)) {
            return i;
        }
        i = i + 1;
    }
    return -1;
}
/* A RESERVED HEAD: a name that this parser answers itself in call position —
 * the four control forms, any rewrite row, any op row. Binding one as a defn
 * PARAMETER is a silent cross-kernel divergence, not a preference: form-kernel-go's
 * reader drops such a name from the parameter list, so `(defn f (sub x) ..)` is
 * arity 2 here and arity 1 there, and the two kernels then answer the same source
 * differently with neither one saying so (MEASURED 2026-07-22: fkwu 7, bin-go
 * `walk: "f" wants 1 args, got 2`). fkwu binds it AND then lets the op win in call
 * position, so the parameter is reachable in value position only — exactly the trap
 * that returned a full-pass 255 on a deliberately broken band
 * (receipts/2026-07-22-ship-the-slot-map.md, defect 1). */
static int fk_reserved_head(long long s, long long n) {
    if (fk_sym_eq(s, n, "defn") || fk_sym_eq(s, n, "do") || fk_sym_eq(s, n, "let") ||
        fk_sym_eq(s, n, "if")) {
        return 1;
    }
    if (fk_rwtab_find(s, n) >= 0) {
        return 1;
    }
    if (fk_optab_find(s, n) >= 0) {
        return 1;
    }
    return 0;
}
/* The names where a PARAMETER of that spelling makes fkwu and form-kernel-go answer
 * the same source differently. This list is MEASURED, not reasoned: all 169 op-table
 * and rewrite-table names plus the four control forms were each put in a defn's
 * parameter list and run on both kernels (2026-07-22). 155 agreed — including `len`,
 * which core.fk's own fstr-to-int-loop has taken as a parameter since before this
 * check existed, and which is therefore NOT a defect. These 18 diverged: Go's reader
 * treats them structurally and drops them from the parameter list, so `(defn f (sub x)
 * ..)` is arity 2 here and arity 1 there. Reasoning from "it is in the op table" would
 * have condemned core.fk on the strength of an argument the oracle refutes. If an op
 * row is added, re-run the probe rather than guessing where it belongs.
 *
 * And it only diverges in the FIRST parameter position. Probed again after the first
 * narrowing, the check still condemned shell-exec.fk's `(defn sh-contains? (s sub) ..)`,
 * which bin-go runs correctly: Go's reader reads the parameter list's own head
 * structurally, so `(sub zz)` collapses and `(zz sub)` does not. All 18 names were
 * re-run in first and second position; every one diverges first and agrees second. So
 * the caller carries the position (`na == 0`). Twice now a reasoned generalization was
 * wider than the measured fact, and both times a real cell in this body was the one
 * that said so. */
static int fk_divergent_param_name(long long s, long long n) {
    return fk_sym_eq(s, n, "add") || fk_sym_eq(s, n, "sub") || fk_sym_eq(s, n, "mul") ||
           fk_sym_eq(s, n, "div") || fk_sym_eq(s, n, "mod") || fk_sym_eq(s, n, "and") ||
           fk_sym_eq(s, n, "or") || fk_sym_eq(s, n, "not") || fk_sym_eq(s, n, "eq") ||
           fk_sym_eq(s, n, "lt") || fk_sym_eq(s, n, "le") || fk_sym_eq(s, n, "gt") ||
           fk_sym_eq(s, n, "ge") || fk_sym_eq(s, n, "list") || fk_sym_eq(s, n, "defn") ||
           fk_sym_eq(s, n, "do") || fk_sym_eq(s, n, "let") || fk_sym_eq(s, n, "if");
}
static long long fk_smknode(long long t0, long long c1, long long c2, long long c3) {
    long long k = fk_node_count;
    fk_node_count = fk_node_count + 1;
    if (fk_node_count > fk_ast_cap) {
        fk_ast_reserve(fk_node_count);
    }
    fk_node[k][0] = t0;
    fk_node[k][1] = c1;
    fk_node[k][2] = c2;
    fk_node[k][3] = c3;
    return k;
}
static long long fk_smklit(long long v) {
    return fk_smknode(1, v, 0, 0);
}
/* ── generic rewrite instantiator: build a lowered node tree from an RPN program ── A rewrite row
 * (fk_rwtab[r]) is name + arity + a postfix int program over: ARG i = (0 i) -> args[i] (the i-th
 * already-parsed operand node) LIT v = (1 v) -> fk_smklit(v) NODE t n = (2 t n) -> fk_smknode(t, <n
 * nodes popped off the build stack>) Children precede the NODE that consumes them, so one
 * left-to-right pass with a small stack materialises the whole shape. This is the EXACT lowering
 * vocabulary the flattener's flt-low uses (if/le/sub on 6/5/4, lt/eq on 103/102), now read as data:
 * gt/ge/lt/eq/and/or/not/abs are rows, not hand-written C. */
static long long fk_rw_build(long long r, long long *args) {
    long long st[32];
    long long sp = 0;
    const long long *p = fk_rwtab[r].prog;
    long long np = fk_rwtab[r].nprog;
    long long k = 0;
    while (k < np) {
        long long opc = p[k];
        if (opc == 0) {
            st[sp] = args[p[k + 1]];
            sp = sp + 1;
            k = k + 2;
        } else if (opc == 1) {
            st[sp] = fk_smklit(p[k + 1]);
            sp = sp + 1;
            k = k + 2;
        } else {
            long long t = p[k + 1];
            long long n = p[k + 2];
            long long c1 = 0, c2 = 0, c3 = 0;
            if (n >= 3) {
                c3 = st[sp - 1];
                sp = sp - 1;
            }
            if (n >= 2) {
                c2 = st[sp - 1];
                sp = sp - 1;
            }
            if (n >= 1) {
                c1 = st[sp - 1];
                sp = sp - 1;
            }
            st[sp] = fk_smknode(t, c1, c2, c3);
            sp = sp + 1;
            k = k + 3;
        }
    }
    return sp > 0 ? st[sp - 1] : fk_smklit(0);
}
/* stone 4: a "..." string literal. fk_spos is at the opening quote. Copy the body bytes (handling
 * \" \\ \n \t) into the string scratch, intern via the same fk_sintern/fk_sbuf pool the
 * table-executor uses, and build a tag-24 node carrying the pool INDEX (the SAME shape fk_walk's
 * tag-24 reads: it returns index<<1). The string pool is shared with the runtime, so a literal
 * authored from source is byte-identical to one fk_sbuf made. */
static long long fk_smkstr(void) {
    fk_spos = fk_spos + 1;
    /* skip opening quote */
    fk_sinit();
    long long start = fk_sbp;
    while (fk_spos < fk_slen && fk_srctext[fk_spos] != FK_CH_DQUOTE) {
        char ch = fk_srctext[fk_spos];
        if (ch == FK_CH_BACKSLASH && fk_spos + 1 < fk_slen) {
            char e = fk_srctext[fk_spos + 1];
            if (e == FK_CH_LOWER_N) {
                ch = FK_CH_LF;
                fk_spos = fk_spos + 1;
            } else if (e == FK_CH_LOWER_T) {
                ch = FK_CH_TAB;
                fk_spos = fk_spos + 1;
            } else if (e == FK_CH_LOWER_R) {
                ch = FK_CH_CR;
                fk_spos = fk_spos + 1;
            } else if (e == FK_CH_DQUOTE) {
                ch = FK_CH_DQUOTE;
                fk_spos = fk_spos + 1;
            } else if (e == FK_CH_BACKSLASH) {
                ch = FK_CH_BACKSLASH;
                fk_spos = fk_spos + 1;
            }
        }
        while (fk_sbp + 1 > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
            fk_sb_check();
        }
        fk_sb[fk_sbp] = ch;
        fk_sbp = fk_sbp + 1;
        fk_spos = fk_spos + 1;
    }
    if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_DQUOTE) {
        fk_spos = fk_spos + 1;
    }
    /* skip closing quote */
    long long idx = fk_sintern(start, fk_sbp - start);
    return fk_smknode(24, idx, 0, 0);
}
/* stone 3: a binding stack maps a name -> a FRAME SLOT (the arg is slot 0; each let takes the next
 * slot). A bare bound name lowers to tag 110 (read fk_vs[fp+slot]); a let lowers to tag 109 (store
 * then body); a function reserves fk_maxslot slots (tag 111). Over-reserve is safe (form-flatten
 * over-reserves too). */
#define FK_BD_STACK_CAP_INIT 1024 /* fk_bd_*: bindings simultaneously in scope during parse; birth size only -- the stack doubles on demand (kernel_stat 29/30). History: 128->1024 (2026-07-02) when a silent drop miscompiled references; then a loud decline; growth since 2026-09-02. */
static long long *fk_bd_s, *fk_bd_n, *fk_bd_off, fk_bd_top, fk_maxslot;
static long long fk_bd_lookup(long long s, long long n) {
    long long i = fk_bd_top;
    while (i > 0) {
        i = i - 1;
        if (fk_sym_eq2(s, n, fk_bd_s[i], fk_bd_n[i])) {
            return fk_bd_off[i];
        }
    }
    return -1;
}
static void fk_bd_push(long long s, long long n, long long off) {
    if (fk_bd_top >= fk_bd_cap) {
        long long nc = fk_bd_cap == 0 ? FK_BD_STACK_CAP_INIT : fk_bd_cap * 2;
        fk_bd_s = (long long *)realloc(fk_bd_s, (unsigned long)(nc * 8));
        fk_bd_n = (long long *)realloc(fk_bd_n, (unsigned long)(nc * 8));
        fk_bd_off = (long long *)realloc(fk_bd_off, (unsigned long)(nc * 8));
        if (fk_bd_s == 0 || fk_bd_n == 0 || fk_bd_off == 0) {
            fk_die("fk_bd_push: out of memory growing the binding stack");
        }
        if (fk_bd_cap != 0) {
            fk_bd_grows = fk_bd_grows + 1;
        }
        fk_bd_cap = nc;
    }
    fk_bd_s[fk_bd_top] = s;
    fk_bd_n[fk_bd_top] = n;
    fk_bd_off[fk_bd_top] = off;
    fk_bd_top = fk_bd_top + 1;
}
static void fk_bd_pop(void) {
    if (fk_bd_top > 0) {
        fk_bd_top = fk_bd_top - 1;
    }
}
/* A nested (defn ...) resets fk_bd_top to 0 so its own body can't accidentally
 * resolve a caller-frame slot (a function has no access to its caller's locals).
 * That reset is a WRITE-CURSOR reset into shared fixed arrays, not a true stack
 * push/pop -- so the nested defn's own fk_bd_push calls physically overwrite
 * fk_bd_s/fk_bd_n/fk_bd_off at indices 0.. with its own bindings. Restoring just
 * fk_bd_top afterward brought the COUNT back but not the DATA already clobbered
 * at those indices -- every name the enclosing do had bound became silently
 * unlookupable (degrading to the unbound-name default, 0) for the rest of that
 * do's own parsing.
 *
 * GROWABLE ARENA, not a single reused 128-slot buffer: a fixed shared buffer
 * looked correct for one level (save at entry, restore at exit) but a DOUBLY
 * nested defn breaks it -- the inner defn's OWN fk_bd_save() call, entered
 * while the OUTER nested defn's body is still being parsed, physically
 * overwrites the SAME buffer indices the outer defn's own save wrote, so by
 * the time the outer defn's own fk_bd_restore() finally runs, the data at
 * those indices is the INNER defn's snapshot, not the outer's true enclosing
 * one -- a silent misread (x and p lost). Two independent sessions hit this
 * the same day (one wiring closure-capture support, which reads an enclosing
 * snapshot from arbitrarily deep inside a body-parse and made the collision
 * far easier to hit; witnessed 2026-09-03 both times) and converged on the
 * same fix: fk_bd_save()/restore() calls are always properly LIFO-nested
 * (they mirror the recursive-descent parser itself: a restore always fires
 * before the save that contains it returns), so a plain bump-and-shrink
 * arena is correct. This is the arena shape (save returns the arena's
 * absolute start offset, restore computes the count as "how far the arena
 * grew since that offset" -- valid only because nothing else could have
 * pushed without also having popped by now, and pops the arena back down so
 * nested calls can never alias each other's data); this arena IS what
 * kernel_stat 33/34 (fk_bd_save_cap/fk_bd_save_grows, declared once, shared
 * with the other capacity/doubling pairs nearby) report -- the arena's own
 * live capacity and doubling count, not a second, differently-shaped
 * stack's. */
static long long *fk_bd_arena_s, *fk_bd_arena_n, *fk_bd_arena_off;
static long long fk_bd_arena_top;
static long long fk_bd_save(void) {
    long long n = fk_bd_top;
    if (fk_bd_arena_top + n > fk_bd_save_cap) {
        long long nc = fk_bd_save_cap == 0 ? 1024 : fk_bd_save_cap * 2;
        while (nc < fk_bd_arena_top + n) {
            nc = nc * 2;
        }
        fk_bd_save_grows = fk_bd_save_grows + 1;
        fk_bd_arena_s = (long long *)realloc(fk_bd_arena_s, (unsigned long)(nc * 8));
        fk_bd_arena_n = (long long *)realloc(fk_bd_arena_n, (unsigned long)(nc * 8));
        fk_bd_arena_off = (long long *)realloc(fk_bd_arena_off, (unsigned long)(nc * 8));
        if (fk_bd_arena_s == 0 || fk_bd_arena_n == 0 || fk_bd_arena_off == 0) {
            fk_die("fk_bd_save: out of memory growing the binding-snapshot arena");
        }
        fk_bd_save_cap = nc;
    }
    long long start = fk_bd_arena_top;
    long long i = 0;
    while (i < n) {
        fk_bd_arena_s[start + i] = fk_bd_s[i];
        fk_bd_arena_n[start + i] = fk_bd_n[i];
        fk_bd_arena_off[start + i] = fk_bd_off[i];
        i = i + 1;
    }
    fk_bd_arena_top = start + n;
    return start;
}
static void fk_bd_restore(long long start) {
    long long n = fk_bd_arena_top - start;
    long long i = 0;
    while (i < n) {
        fk_bd_s[i] = fk_bd_arena_s[start + i];
        fk_bd_n[i] = fk_bd_arena_n[start + i];
        fk_bd_off[i] = fk_bd_arena_off[start + i];
        i = i + 1;
    }
    fk_bd_arena_top = start;
    fk_bd_top = n;
}
/* The IMMEDIATELY enclosing defn's own bd snapshot, for closure-capture detection: a slice of the
 * fk_bd_save() arena (fk_enc_mark..fk_enc_mark+fk_enc_count), set only while parsing a NESTED
 * defn's own body (fk_enc_count is 0 at every top-level defn -- there is no enclosing frame to
 * capture from at file scope, and this must never be left over from an unrelated earlier parse).
 * Saved/restored around each nested-defn parse exactly like fk_bd_top/fk_maxslot already are, so
 * it always names the frame ONE level up from wherever body-parsing currently is, however deep the
 * nesting -- never a grandparent, and never a sibling's own frame (see fk_cur_defn_idx for why a
 * capturing function's OWN body can't just read this same snapshot to forward its captures on). */
static long long fk_enc_mark, fk_enc_count;
static long long fk_enc_lookup(long long s, long long n) {
    long long i = fk_enc_count;
    while (i > 0) {
        i = i - 1;
        if (fk_sym_eq2(s, n, fk_bd_arena_s[fk_enc_mark + i], fk_bd_arena_n[fk_enc_mark + i])) {
            return fk_bd_arena_off[fk_enc_mark + i];
        }
    }
    return -1;
}
/* The fn-idx whose body is CURRENTLY being parsed (-1 outside every defn). Set around every defn
 * body-parse, top-level and nested alike, so a nested defn can record its own TRUE parent
 * (fk_fn_parent_idx) and a call site can tell "am I still directly inside the frame this callee's
 * captures were read from" (fk_cur_defn_idx == that callee's own parent) from "this is a
 * self-recursive or cross-sibling call, where re-reading the same enclosing offsets against the
 * wrong frame would silently return someone else's value" -- the latter is diagnosed, never
 * silently miscomputed (see the call-resolution arm in fk_sparse). */
static long long fk_cur_defn_idx = -1;
static long long fk_parse_do(void);
static long long fk_parse_top_do_value(void);
static void fk_parse_top(void);
/* stone 4: a function table. Each top-level (defn name ...) gets its own fn-index (>=1); a call to
 * a registered name lowers to tag 12 (call-by-index, single-arg). A non-defn top form is the root
 * (fn[0]). */
/* Function symbols share the demand-grown storage declared with fk_fn. */
#define FK_TOP_CONST_CAP_INIT 512 /* top-level constant table birth size; doubles on demand */
/* THE ONCE-HOLD (tag FK_TAG_CONST_HOLD). Witnessed 2026-09-02: a top-level
 * let spliced its initializer NODE into every reference site, so each read
 * re-walked the whole build — call-by-name. The Go arm builds once; the
 * divergence cost 31.6M dispatches in one cold .bml compile. Every
 * reference to a top-level let now shares one hold node per const binding:
 * the first walk computes the value and holds it in the NODE'S OWN free
 * fields (node[2] value, node[3] the melt-gen stamp (gen<<1)|1, 0 = empty)
 * — the arm64-hint idiom, a melt un-vouches and the next read rebuilds.
 * fk_const_wrapp1[row] is the binding's hold node + 1 (0 = none yet), grown
 * with the const table below so a demand-doubled table never reads garbage;
 * a redefinition or reused row drops it. (FK_TAG_CONST_HOLD is #defined
 * up by fk_arms, where the walker arm can reach it.) */
static long long *fk_const_s, *fk_const_n, *fk_const_node, *fk_const_wrapp1, fk_const_top;
static long long fk_const_cap;
/* Most-recent-first, mirroring fk_bd_lookup: a nested defn now shares this table
 * with every top-level one (fk_parse_do's own "defn" branch), so the same name at
 * two scopes must resolve to the INNER, currently-live registration while both are
 * in scope -- a forward scan would always find the outer/lower-indexed one first
 * and the inner one could never actually shadow it. */
static long long fk_fn_lookup(long long s, long long n) {
    long long i = fk_fntop;
    while (i > 0) {
        i = i - 1;
        if (fk_sym_eq2(s, n, fk_fnsym_s[i], fk_fnsym_n[i])) {
            return fk_fnidx[i];
        }
    }
    return -1;
}
/* returns the const ROW (not the node): the reference site builds/reuses
 * the row's shared hold node, so all references share one memo slot. */
static long long fk_const_lookup(long long s, long long n) {
    long long i = fk_const_top;
    while (i > 0) {
        i = i - 1;
        if (fk_sym_eq2(s, n, fk_const_s[i], fk_const_n[i])) {
            return i;
        }
    }
    return -1;
}
static void fk_const_set(long long s, long long n, long long node) {
    long long i = 0;
    while (i < fk_const_top) {
        if (fk_sym_eq2(s, n, fk_const_s[i], fk_const_n[i])) {
            fk_const_node[i] = node;
            /* a redefinition drops the old hold node; old references keep
             * their already-spliced meaning, new references bind fresh. */
            fk_const_wrapp1[i] = 0;
            return;
        }
        i = i + 1;
    }
    if (fk_const_top >= fk_const_cap) {
        long long nc = fk_const_cap == 0 ? FK_TOP_CONST_CAP_INIT : fk_const_cap * 2;
        fk_const_s = (long long *)realloc(fk_const_s, (unsigned long)(nc * 8));
        fk_const_n = (long long *)realloc(fk_const_n, (unsigned long)(nc * 8));
        fk_const_node = (long long *)realloc(fk_const_node, (unsigned long)(nc * 8));
        fk_const_wrapp1 = (long long *)realloc(fk_const_wrapp1, (unsigned long)(nc * 8));
        if (fk_const_s == 0 || fk_const_n == 0 || fk_const_node == 0 ||
            fk_const_wrapp1 == 0) {
            fk_die("fk_const_set: out of memory growing the top-level constant table");
        }
        fk_const_cap = nc;
    }
    fk_const_s[fk_const_top] = s;
    fk_const_n[fk_const_top] = n;
    fk_const_node[fk_const_top] = node;
    /* rows are reused after the loaders reset fk_const_top; a fresh binding
     * must never inherit the previous tenant's hold node. */
    fk_const_wrapp1[fk_const_top] = 0;
    fk_const_top = fk_const_top + 1;
}
static long long fk_parse_variadic(long long tag);
static long long fk_parse_fixed_list(long long n);
static long long fk_parse_record_new(void);
/* Wrap a nested defn's compiled body with its own capture-prologue: one tag-109 "let" per
 * captured free variable, binding its own frame slot from fk_call_cap_vals[j] (tag 149, a bare
 * leaf read of that scratch slot) before any of the body's own statements run -- see
 * fk_call_cap_vals's own comment for why this ordering is always race-free. A no-op (returns
 * body unchanged) for any idx with zero captures, which is every ordinary function. */
static long long fk_wrap_cap_prologue(long long idx, long long body) {
    long long n = (idx >= 0 && idx < fk_fn_cap_capacity) ? fk_fn_cap_count[idx] : 0;
    long long j = n;
    while (j > 0) {
        j = j - 1;
        long long slot = fk_fn_cap_slot[idx * FK_CLOSURE_CAP_MAX + j];
        long long capread = fk_smknode(149, j, 0, 0);
        body = fk_smknode(109, fk_smklit(slot), capread, body);
    }
    return body;
}
/* arity -1: parse-until-close, fold right via tag -> nil(18) */
static long long fk_sparse(void) {
    fk_sskip();
    if (fk_spos >= fk_slen) {
        return 0;
    }
    char c = fk_srctext[fk_spos];
    if (c == FK_CH_LPAREN) {
        fk_spos = fk_spos + 1;
        fk_sskip();
        long long s = fk_spos;
        fk_spos = fk_sym_end(fk_spos);
        long long hn = fk_spos - s;

        /* (defn name (arg) body): arg -> slot 0; body becomes fn[0], wrapped in a reserve over its
         * lets. */
        if (fk_sym_eq(s, hn, "defn")) {
            fk_sskip();
            long long ns2 = fk_spos;
            fk_spos = fk_sym_end(fk_spos);
            fk_fname_s = ns2;
            fk_fname_n = fk_spos - ns2;
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_LPAREN) {
                fk_spos = fk_spos + 1;
            }
            /* This is the RESIDUAL path: a nested defn reached somewhere other than
             * an ordinary do-statement position (fk_parse_do's own "defn" branch
             * handles that common case, with the full rationale for what a nested
             * defn IS). Getting here means a defn sits directly in a value
             * position -- e.g. `(let clo (defn f (a) a))`. Same decision, same
             * mechanism: register (own fk_fn[] slot, own frame, self-recursion
             * resolves), but since there is no enclosing do "rest" for the name
             * to stay visible through, un-register it the moment this expression
             * is done -- only the returned fn-VALUE (tag 243) can reach it after
             * that, exactly like any other first-class function value. */
            long long fk_bd_saved_top = fk_bd_save();
            long long fk_bd_saved_maxslot = fk_maxslot;
            long long enc_count_here = fk_bd_top;
            fk_bd_top = 0;
            fk_maxslot = 0;
            long long saved_fntop = fk_fntop;
            long long idx = fk_defn_next;
            fk_defn_next = fk_defn_next + 1;
            fk_fn_reserve(fk_defn_next);
            fk_fnsym_s[fk_fntop] = ns2;
            fk_fnsym_n[fk_fntop] = fk_fname_n;
            fk_fnidx[fk_fntop] = idx;
            fk_live_note_defn(fk_fntop);
            fk_fntop = fk_fntop + 1;
            fk_fn_cap_reserve(idx + 1);
            fk_fn_parent_idx[idx] = fk_cur_defn_idx;
            long long saved_enc_mark = fk_enc_mark;
            long long saved_enc_count = fk_enc_count;
            fk_enc_mark = fk_bd_saved_top;
            fk_enc_count = enc_count_here;
            long long saved_cur_defn_idx = fk_cur_defn_idx;
            fk_cur_defn_idx = idx;
            long long na = 0;
            while (1) {
                fk_sskip();
                if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
                    break;
                }
                long long as2 = fk_spos;
                fk_spos = fk_sym_end(fk_spos);
                if (na == 0 && fk_spos > as2 && fk_divergent_param_name(as2, fk_spos - as2)) {
                    fk_diag(FK_DIAG_ERR, as2,
                            "[shadowed-primitive] parameter '%.*s' names a primitive/control form -- "
                            "in call position the primitive still wins, so the parameter is reachable "
                            "in value position only, and form-kernel-go drops it from the parameter "
                            "list entirely (arity divergence). Rename the parameter",
                            (int)(fk_spos - as2), fk_srctext + as2);
                    fk_src_unrunnable = 1;
                }
                fk_bd_push(as2, fk_spos - as2, na);
                if (na > fk_maxslot) {
                    fk_maxslot = na;
                }
                na = na + 1;
            }
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            fk_fnar[idx] = na;
            long long body = fk_sparse();
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            body = fk_wrap_cap_prologue(idx, body);
            if (fk_maxslot > 0) {
                body = fk_smknode(111, fk_smklit(fk_maxslot), body, 0);
            }
            fk_fn[idx] = body; fk_prog_note_body(idx);
            fk_bd_restore(fk_bd_saved_top);
            fk_maxslot = fk_bd_saved_maxslot;
            fk_fntop = saved_fntop;
            fk_enc_mark = saved_enc_mark;
            fk_enc_count = saved_enc_count;
            fk_cur_defn_idx = saved_cur_defn_idx;
            /* -1, not 0: field 2 is the closure env-chain-or-absent, and 0 is a REAL node
             * index (the very first node of the whole program) -- fk_walk's tag-243 arm tells
             * "plain fn-value" from "has captures" by field2<0, so a bare non-capturing
             * fn-value must use the same -1 sentinel every other chain-or-absent field in this
             * file already uses (tag 241/244's own arg-chain fields), not 0. */
            return fk_smknode(243, idx, -1, 0);
        }
        if (fk_sym_eq(s, hn, "do")) {
            return fk_parse_do();
        }

        /* (let name val): canonical let is TWO-ARG — it binds name for the REST of its (do ...) and
         * evaluates to val (observe/wav-sense.fk: "let is two-arg only; binds for the rest of its
         * do; a three-arg (let n v body) drops body"; the Go/Rust/TS walkers agree). A well-formed
         * let always opens a do, so it is bound by fk_parse_do, which sees the rest; this fk_sparse
         * path is reached only by a BARE let in a raw value position (no do, hence no rest). The
         * old 3-arg form here — (let name val body), eval body in scope — is the malformed shape
         * the walkers drop (TS rejects it outright); it survives untouched as a pre-existing,
         * non-four-way value-position convenience. The actual do-let divergence is fixed entirely
         * in fk_parse_top + fk_parse_do; this path is left as-is to keep every prelude library
         * byte-identical. */
        if (fk_sym_eq(s, hn, "let")) {
            fk_sskip();
            long long ns = fk_spos;
            fk_spos = fk_sym_end(fk_spos);
            long long nlen = fk_spos - ns;
            long long val = fk_sparse();
            long long slot = fk_maxslot + 1;
            fk_maxslot = slot;
            fk_bd_push(ns, nlen, slot);
            fk_sskip();
            long long body;
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                /* bare 2-arg (let name val): no body follows. Emit the same lit-0
                 * body the old fk_sparse-on-rparen call produced, WITHOUT sending
                 * fk_sparse to the rparen -- that is now the loud stray-rparen
                 * path below, and this legal shape must not trip it. */
                body = fk_smklit(0);
            } else {
                body = fk_sparse();
            }
            fk_bd_pop();
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            return fk_smknode(109, fk_smklit(slot), val, body);
        }

        /* (if cond then else): the one control form with a value position; 3-ary tag-6 emit.
         * defn/do/let are handled above; if rides here so the boolean rewrites (and/or/not/abs)
         * that LOWER to it find a real (if ...) target. */
        if (fk_sym_eq(s, hn, "if")) {
            long long c1 = fk_sparse();
            long long c2 = fk_sparse();
            long long c3 = fk_sparse();
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            return fk_smknode(6, c1, c2, c3);
        }

        /* A LIVE BINDING THIS CALL WILL NOT REACH. `(sub x 128)` where `sub` is a name in
         * scope reads as subtraction, not as the binding — the op/rewrite tables are
         * consulted before the local frame, and form-kernel-go answers the same way, so
         * this is not a divergence and fkwu does not refuse it. It is still the trap that
         * cost Stone 13 hours (receipts/2026-07-22-ship-the-slot-map.md), so it is said
         * out loud: a WARNING, counted and printed, where a defn parameter of the same
         * spelling is the harder ERROR above. */
        if (fk_bd_lookup(s, hn) >= 0 && fk_reserved_head(s, hn)) {
            fk_diag(FK_DIAG_WARN, s,
                    "[shadowed-call] '%.*s' is bound in this scope but in call position the "
                    "primitive wins -- this call does NOT reach the binding",
                    (int)hn, fk_srctext + s);
        }

        /* DATA-DRIVEN rewrite: gt/ge/lt/eq/and/or/not/abs are rows in fk_rwtab. Parse `arity`
         * operands, then instantiate the row's RPN lowering template. Mirrors the flattener's
         * flt-low — rules as data, not a hand-written C case per name. */
        long long rw = fk_rwtab_find(s, hn);
        if (rw >= 0) {
            long long ra = fk_rwtab[rw].arity;
            long long args[4];
            long long ai = 0;
            while (ai < ra) {
                args[ai] = fk_sparse();
                ai = ai + 1;
            }
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            return fk_rw_build(rw, args);
        }

        /* DATA-DRIVEN primitive: read (arity, tag) from fk_optab (the manifest table), parse
         * `arity` args, emit fk_smknode(tag, ...). Adding an op is a manifest row. arity 0 (e.g.
         * (empty) -> tag 18 nil) emits a bare node; arity -1 is the VARIADIC sentinel: parse
         * operands until the close paren and fold them right into a chain via the row's tag
         * (cons/19) ending in nil (tag 18). (list ..) is therefore a DATA row, not a hand-written C
         * case. */
        long long oi = fk_optab_find(s, hn);
        if (oi >= 0) {
            long long ar = fk_optab[oi].arity;
            long long tag = fk_optab[oi].tag;
            if (ar < 0) {
                /* print (239) folds its operands through cons (19) and hangs the
                 * resulting LIST under one print node, rather than chaining on its
                 * own tag the way (list ..) does. Two reasons, both measured against
                 * bin-go: a self-chain makes `(print)` with no operands lower to the
                 * empty node (tag 18) -- not a print at all, where Go emits the bare
                 * newline -- and the list shape hands the walker every operand
                 * already evaluated and in order, which is what Go's variadic
                 * `for i, a := range args` sees. */
                if (tag == 239) {
                    return fk_smknode(239, fk_parse_variadic(19), 0, 0);
                }
                /* record_new (64) folds its operands into the ENTRY-PACKET shape
                 * flt-record-new emits — ((-1 bp) (k v) ..) under ONE tag-64 node —
                 * not a flat chain on its own tag; see fk_parse_record_new. */
                if (tag == 64) {
                    return fk_parse_record_new();
                }
                /* method_invoke (199): receiver and name are fixed operands;
                 * the remaining args ride the same forward-linked tag-242 arg
                 * cells the direct-call path builds, so fk_walk threads them
                 * left-to-right like any other call. */
                if (tag == 199) {
                    long long recv199 = fk_sparse();
                    long long name199 = fk_sparse();
                    long long margn[256];
                    long long mai = 0;
                    fk_sskip();
                    while (fk_spos < fk_slen && fk_srctext[fk_spos] != FK_CH_RPAREN && mai < 256) {
                        margn[mai] = fk_sparse();
                        mai = mai + 1;
                        fk_sskip();
                    }
                    if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                        fk_spos = fk_spos + 1;
                    }
                    long long mchain = -1;
                    while (mai > 0) {
                        mai = mai - 1;
                        mchain = fk_smknode(242, margn[mai], mchain, 0);
                    }
                    return fk_smknode(199, recv199, name199, mchain);
                }
                return fk_parse_variadic(tag);
            }
            if (tag == 91 && ar == 4) {
                long long xs = fk_parse_fixed_list(4);
                fk_sskip();
                if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                    fk_spos = fk_spos + 1;
                }
                return fk_smknode(tag, xs, 0, 0);
            }
            if (ar > 3) {
                /* A 4+-arg op has no source lowering yet. The one arity-4 row in the
                 * manifest, make_nodeid (tag 91, the content-address constructor), lowers
                 * above through its cons-list carrier; any other 4+-arity row is a manifest
                 * entry whose parser lowering is unfinished work. The generic 3-child path
                 * below cannot hold a 4th arg, so drain the balanced form here (fk_spos
                 * must advance past it or the ')' check stalls) and decline to nothing. */
                fk_diag(FK_DIAG_ERR, s,
                        "op '%.*s' (arity %lld) has no source lowering yet -- only make_nodeid (arity 4) lowers here; a 4+-arg op needs its own parser lowering (unfinished work, not a limit)",
                        (int)hn, fk_srctext + s, ar);
                long long depth = 1;
                while (fk_spos < fk_slen && depth > 0) {
                    char cc = fk_srctext[fk_spos];
                    if (cc == FK_CH_DQUOTE) {
                        fk_spos = fk_spos + 1;
                        while (fk_spos < fk_slen && fk_srctext[fk_spos] != FK_CH_DQUOTE) {
                            fk_spos = fk_spos + 1;
                        }
                        if (fk_spos < fk_slen) {
                            fk_spos = fk_spos + 1;
                        }
                        continue;
                    }
                    if (cc == FK_CH_LPAREN) {
                        depth = depth + 1;
                    } else if (cc == FK_CH_RPAREN) {
                        depth = depth - 1;
                    }
                    fk_spos = fk_spos + 1;
                }
                return fk_smknode(137, 0, 0, 0);
            }
            long long c1 = 0, c2 = 0, c3 = 0;
            if (ar >= 1) {
                c1 = fk_sparse();
            }
            if (ar >= 2) {
                c2 = fk_sparse();
            }
            if (ar >= 3) {
                c3 = fk_sparse();
            }
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            return fk_smknode(tag, c1, c2, c3);
        }

        /* call: any registered function name (INCLUDING self — each defn is registered before its
         * body is parsed) -> tag 12 (call fk_fn[idx] with one arg). This replaces the old
         * fn[0]-only self-call (tag 7), which was wrong once there is more than one function. A
         * 0-arg call parses a dummy 0 off the immediate ) — the callee reads no slot, so it is
         * inert.
         *
         * LOCALS SHADOW GLOBALS in call position: the local frame (fk_bd_lookup) is consulted
         * BEFORE the defn table, matching value position (which already reads bd first) and the
         * three siblings. Pre-heal the defn table won: (defn oac-offer (cell args) (cell args))
         * under a loaded (defn cell ...) invoked the GLOBAL constructor instead of the parameter
         * (receipts/2026-07-17-jacobian-lens-and-the-cell-shadowing-heal.md) — every higher-order
         * param (map's f, filter's pred) was one same-named prelude defn away from silent
         * capture. A shadowed head lowers through the indirect-call arm below (tag 244). */
        long long hshadow = fk_bd_lookup(s, hn);
        if (hshadow < 0 && fk_cur_defn_idx >= 0 && fk_enc_count > 0) {
            /* The call HEAD itself may be a captured free variable -- a parameter of the
             * enclosing defn that HOLDS a fn (http-layer.fk's layer-wrap taking a `layer-fn`
             * argument and its nested lw-handler later calling it is the standing example).
             * Capture it exactly like any other free var (fresh local slot, recorded per
             * function) and let it fall straight into the ordinary "head is a bound name"
             * indirect-call path just below -- its value is only known at call time either
             * way, capture or not. */
            long long hencoff = fk_enc_lookup(s, hn);
            if (hencoff >= 0) {
                long long hfcc = (fk_cur_defn_idx < fk_fn_cap_capacity) ? fk_fn_cap_count[fk_cur_defn_idx] : 0;
                if (hfcc < FK_CLOSURE_CAP_MAX) {
                    fk_fn_cap_reserve(fk_cur_defn_idx + 1);
                    long long hcapslot = fk_maxslot + 1;
                    fk_maxslot = hcapslot;
                    fk_bd_push(s, hn, hcapslot);
                    fk_fn_cap_encoff[fk_cur_defn_idx * FK_CLOSURE_CAP_MAX + hfcc] = hencoff;
                    fk_fn_cap_slot[fk_cur_defn_idx * FK_CLOSURE_CAP_MAX + hfcc] = hcapslot;
                    fk_fn_cap_count[fk_cur_defn_idx] = hfcc + 1;
                    hshadow = hcapslot;
                } else {
                    fk_diag(FK_DIAG_ERR, s,
                            "[closure-scope] '%.*s' would be this function's %dth captured name "
                            "(max %d) -- not captured",
                            (int)hn, fk_srctext + s, (int)hfcc + 1, FK_CLOSURE_CAP_MAX);
                }
            }
        }
        long long fidx = (hshadow >= 0) ? -1 : fk_fn_lookup(s, hn);
        if (fidx >= 0) {
            /* GENERAL ARITY (no per-arity case): parse the callee's `ar` declared arg expressions
             * and thread them into a forward-linked arg-chain of tag-242 cells (cell:
             * [1]=arg-expr-node, [2]=next-cell or -1, head-first so slot 0 is the first arg). The
             * call is ONE tag-241 node ([1]=fidx, [2]=chain-head or -1). fk_walk evaluates the
             * chain left-to-right, pushing each arg via fk_vp exactly as the table path packs N
             * args — same mechanism, any N. ar==0 parses no args (chain -1, inert); ar==1/2/8 are
             * the same code. */
            long long ar = (fidx >= 0 && fidx < fk_fn_capacity) ? fk_fnar[fidx] : 1;
            long long over = 0;
            if (ar > 256) {
                /* COMPILE-PHASE: over-arity is a diagnosable source error, not
                 * corruption. Parse the first 256, then DRAIN the rest to the
                 * matching ')' (the same balanced-paren skip the unresolved-head
                 * arm below uses) so the parser stays synced and later forms are
                 * still checked. The die's own fear -- truncate + desync -- is
                 * answered by the drain, not by exit(1). */
                fk_diag(FK_DIAG_ERR, fk_spos,
                        "[arity-cap] direct call to '%.*s' declares %lld args (>256); "
                        "parsing first 256, form truncated",
                        (int)hn, fk_srctext + s, ar);
                ar = 256;
                over = 1;
            }
            long long argn[256];
            long long ai = 0;
            while (ai < ar && ai < 256) {
                argn[ai] = fk_sparse();
                ai = ai + 1;
            }
            if (over) {
                /* drain remaining operands to the matching close paren */
                long long depth = 1;
                while (fk_spos < fk_slen && depth > 0) {
                    char cc = fk_srctext[fk_spos];
                    if (cc == FK_CH_DQUOTE) {
                        fk_spos = fk_spos + 1;
                        while (fk_spos < fk_slen && fk_srctext[fk_spos] != FK_CH_DQUOTE) {
                            fk_spos = fk_spos + 1;
                        }
                        if (fk_spos < fk_slen) {
                            fk_spos = fk_spos + 1;
                        }
                        continue;
                    }
                    if (cc == FK_CH_LPAREN) {
                        depth = depth + 1;
                    } else if (cc == FK_CH_RPAREN) {
                        depth = depth - 1;
                    }
                    fk_spos = fk_spos + 1;
                }
            } else {
                fk_sskip();
                if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                    fk_spos = fk_spos + 1;
                }
            }
            long long chain = -1;
            long long k = ai;
            while (k > 0) {
                k = k - 1;
                chain = fk_smknode(242, argn[k], chain, 0);
            }
            /* A capturing callee (fk_fn_cap_count[fidx] > 0 -- see fk_parse_do's own "defn"
             * branch) cannot run through the ordinary direct-call node: its body expects its
             * captures delivered through the prologue+fk_call_cap_vals channel, not baked into
             * fk_fn[fidx] alone. Route it through the SAME value+indirect-call mechanism an
             * escaped closure already uses (tag 243 building the closure value, tag 244 calling
             * it) -- correct here ONLY when this call site is still directly inside the frame
             * fidx's captures read from (fk_cur_defn_idx == fidx's own recorded parent); a
             * self-recursive or cross-sibling call is a DIFFERENT frame, where re-reading those
             * same enclosing offsets would silently return someone else's value, so it is
             * diagnosed instead (the general N-level/self-recursive capture case is real future
             * work, not yet built -- see the parent-mismatch branch below). */
            long long fcc = (fidx >= 0 && fidx < fk_fn_cap_capacity) ? fk_fn_cap_count[fidx] : 0;
            if (fcc == 0) {
                return fk_smknode(241, fidx, chain, 0);
            }
            long long fparent = fk_fn_parent_idx[fidx];
            if (fk_cur_defn_idx != fparent) {
                fk_diag(FK_DIAG_ERR, s,
                        "[closure-scope] '%.*s' captures a name from its own enclosing scope, "
                        "and this call is not directly inside that scope (self-recursion or a "
                        "call from a sibling's own body) -- reading its captures here would read "
                        "the wrong frame, so this call is declined rather than silently wrong. "
                        "Recovered to nothing (axiom-5); parse continues",
                        (int)hn, fk_srctext + s);
                return fk_smknode(137, 0, 0, 0);
            }
            long long envchain = -1;
            long long ce = fcc;
            while (ce > 0) {
                ce = ce - 1;
                long long encoff = fk_fn_cap_encoff[fidx * FK_CLOSURE_CAP_MAX + ce];
                envchain = fk_smknode(242, fk_smknode(110, fk_smklit(encoff), 0, 0), envchain, 0);
            }
            long long closurenode = fk_smknode(243, fidx, envchain, 0);
            return fk_smknode(244, closurenode, chain, 0);
        }

        /* INDIRECT CALL (stone 2c): a call (h args..) whose head h is a BOUND NAME — a parameter,
         * or a let-var holding a fn returned from a fn — is an offer to a COMPUTED callee (axiom-5:
         * offer a computed cell). The head LOWERS to its slot read (tag 110); at eval it must
         * reduce to a fn-VALUE, and the fn it names is then offered with the args. Emit tag 244:
         * [1]=head-expr-node, [2]=arg-chain (242 cells, head-first), exactly the tag-241 shape but
         * with a computed head. Args are parsed until the close paren (the indirect callee's arity
         * is not a static name lookup); each is a forward-linked 242 cell so fk_walk threads them
         * left-to-right like the direct path. An unshadowed bare fn-NAME never reaches here (it
         * resolves at fk_fn_lookup above into the direct tag-241 path); a BOUND name always
         * lands here, even when a global defn shares its spelling (locals shadow globals). */
        long long hoff = hshadow;
        if (hoff >= 0) {
            long long head244 = fk_smknode(110, fk_smklit(hoff), 0, 0);
            long long iargn[256];
            long long iai = 0;
            while (iai < 256) {
                fk_sskip();
                if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
                    break;
                }
                iargn[iai] = fk_sparse();
                iai = iai + 1;
            }
            fk_sskip();
            if (iai >= 256 && fk_spos < fk_slen && fk_srctext[fk_spos] != FK_CH_RPAREN) {
                /* COMPILE-PHASE: kept 256, DRAIN the rest to the matching ')'
                 * (balanced skip) so fk_spos realigns and parsing continues --
                 * same shape as the direct-call arm above. */
                fk_diag(FK_DIAG_ERR, fk_spos,
                        "[arity-cap] indirect call declares >256 args; kept 256, rest dropped");
                long long depth = 1;
                while (fk_spos < fk_slen && depth > 0) {
                    char cc = fk_srctext[fk_spos];
                    if (cc == FK_CH_DQUOTE) {
                        fk_spos = fk_spos + 1;
                        while (fk_spos < fk_slen && fk_srctext[fk_spos] != FK_CH_DQUOTE) {
                            fk_spos = fk_spos + 1;
                        }
                        if (fk_spos < fk_slen) {
                            fk_spos = fk_spos + 1;
                        }
                        continue;
                    }
                    if (cc == FK_CH_LPAREN) {
                        depth = depth + 1;
                    } else if (cc == FK_CH_RPAREN) {
                        depth = depth - 1;
                    }
                    fk_spos = fk_spos + 1;
                }
            } else if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            long long ichain = -1;
            long long ik = iai;
            while (ik > 0) {
                ik = ik - 1;
                ichain = fk_smknode(242, iargn[ik], ichain, 0);
            }
            return fk_smknode(244, head244, ichain, 0);
        }

        /* unknown head (no op / rewrite / user fn matched): an OFFER to a callee that does not
         * resolve. stone 3 (2b-ii): axiom-5 — "an offer a cell can't answer acks nothing." So a
         * (head ...) whose head is NOT a resolvable callee yields the canonical first-class nothing
         * (tag 137), NOT a literal 0. This is the INTENTIONAL nothing: the offer-shape is
         * well-formed (a balanced (head args..)) but no cell answers it — exactly fail/decline. It
         * is what oac-choice (first non-nothing) and oac-try (recover from nothing) recover over
         * REAL reducer calls, not only explicit (nothing). The line vs a MASKED BUG: we only reach
         * here from a CALL position (inside `(`, after the head matched no op/rewrite/fn) — a
         * well-formed offer to a non-resolving callee. We still consume the WHOLE balanced form so
         * the parser stays aligned (the old first-`)` skip corrupted later defns); a genuinely
         * malformed program does not become silent here — unbalanced parens still run the source
         * past its end, and a structurally broken op (wrong arity, bad literal) surfaces through
         * its own op path, never through this call-decline. A bare unbound SYMBOL (a value
         * position, not a call/offer) stays 0 below — it is not an offer, so it does not ack
         * nothing. */
        {
            long long depth = 1;
            while (fk_spos < fk_slen && depth > 0) {
                char cc = fk_srctext[fk_spos];
                if (cc == FK_CH_DQUOTE) {
                    fk_spos = fk_spos + 1;
                    while (fk_spos < fk_slen && fk_srctext[fk_spos] != FK_CH_DQUOTE) {
                        fk_spos = fk_spos + 1;
                    }
                    if (fk_spos < fk_slen) {
                        fk_spos = fk_spos + 1;
                    }
                    continue;
                }
                if (cc == FK_CH_LPAREN) {
                    depth = depth + 1;
                } else if (cc == FK_CH_RPAREN) {
                    depth = depth - 1;
                }
                fk_spos = fk_spos + 1;
            }
        }
        /* Compile-time unresolved head. It CAN recover -- axiom-5: an offer a cell can't answer
         * acks nothing (tag 137), so the parse continues. Per "die only if it cannot recover," we
         * do NOT die here; we RECOVER. But we no longer do it SILENTLY: this witness is the compile
         * diagnostic that was missing (the ftanh-class bug). Go/Rust/TS hard-error on an unbound
         * head; fkwu recovers and says so, on every occurrence, unconditionally (no env gate). A
         * correct program with its preludes present never reaches here. */
        /* Route the pre-existing unresolved-call witness through the collector as
         * an ERROR so it joins the gcc-style count -- but it STILL RECOVERS to
         * tag-137 nothing (axiom-5: an offer a cell can't answer acks nothing).
         * It is defeasible, so it does NOT die; the program still runs its
         * recovered output, and the nonzero exit comes from the error count. */
        fk_diag(FK_DIAG_ERR, s,
                "[unresolved-call] '%.*s' matched no op/rewrite/fn/binding -- typo or "
                "missing prelude? Recovered to nothing (axiom-5); parse continues",
                (int)hn, fk_srctext + s);
        return fk_smknode(137, 0, 0, 0);
    }

    /* stone 4: a bare "..." string literal. */
    if (c == FK_CH_DQUOTE) {
        return fk_smkstr();
    }
    if ((c >= FK_CH_DIGIT0 && c <= FK_CH_DIGIT9) ||
        (c == FK_CH_DASH && fk_spos + 1 < fk_slen && fk_srctext[fk_spos + 1] >= FK_CH_DIGIT0 &&
         fk_srctext[fk_spos + 1] <= FK_CH_DIGIT9)) {
        /* number leaf: integer OR float. A '.' or a valid 'e'/'E' exponent makes it a FLOAT —
         * intern the whole literal text (incl "1.5e-05") and wrap it in str_to_float (tag 53 =
         * strtod), the same float value the flattener's flt-float-lit produces. Else an integer
         * literal (tag 1). */
        long long start = fk_spos;
        if (c == FK_CH_DASH) {
            fk_spos = fk_spos + 1;
        }
        while (fk_spos < fk_slen && fk_srctext[fk_spos] >= FK_CH_DIGIT0 &&
               fk_srctext[fk_spos] <= FK_CH_DIGIT9) {
            fk_spos = fk_spos + 1;
        }
        int isf = 0;
        if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_DOT) {
            isf = 1;
            fk_spos = fk_spos + 1;
            while (fk_spos < fk_slen && fk_srctext[fk_spos] >= FK_CH_DIGIT0 &&
                   fk_srctext[fk_spos] <= FK_CH_DIGIT9) {
                fk_spos = fk_spos + 1;
            }
        }
        if (fk_spos < fk_slen && (fk_srctext[fk_spos] == FK_CH_LOWER_E || fk_srctext[fk_spos] == FK_CH_UPPER_E)) {
            long long pe = fk_spos + 1;
            if (pe < fk_slen && (fk_srctext[pe] == FK_CH_PLUS || fk_srctext[pe] == FK_CH_DASH)) {
                pe = pe + 1;
            }
            if (pe < fk_slen && fk_srctext[pe] >= FK_CH_DIGIT0 && fk_srctext[pe] <= FK_CH_DIGIT9) {
                isf = 1;
                fk_spos = pe + 1;
                while (fk_spos < fk_slen && fk_srctext[fk_spos] >= FK_CH_DIGIT0 &&
                       fk_srctext[fk_spos] <= FK_CH_DIGIT9) {
                    fk_spos = fk_spos + 1;
                }
            }
        }
        if (isf) {
            fk_sinit();
            long long ps = fk_sbp;
            long long k = start;
            while (k < fk_spos) {
                while (fk_sbp + 1 > fk_scap_b) {
                    fk_scap_b = fk_scap_b * 2;
                    fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b / 2, fk_scap_b, FK_STORE_STR_BYTES, 0);
                    fk_sb_check();
                }
                fk_sb[fk_sbp] = fk_srctext[k];
                fk_sbp = fk_sbp + 1;
                k = k + 1;
            }
            long long idx = fk_sintern(ps, fk_sbp - ps);
            return fk_smknode(53, fk_smknode(24, idx, 0, 0), 0, 0);
        }
        long long v = 0;
        long long j = start;
        int neg = 0;
        if (fk_srctext[j] == FK_CH_DASH) {
            neg = 1;
            j = j + 1;
        }
        while (j < fk_spos) {
            v = v * 10 + (fk_srctext[j] - FK_CH_DIGIT0);
            j = j + 1;
        }
        if (neg) {
            v = 0 - v;
        }
        return fk_smklit(v);
    }

    /* a bare symbol: a bound name -> tag 110 (read its frame slot); a registered fn-NAME in VALUE
     * position (stone 2c) -> tag 243, the fn-VALUE (an odd-negative reserved sentinel naming the
     * fn-index — first-class, passable as an arg, storable, returnable); else an honest 0. A bound
     * name wins over a fn-name (lexical scope shadows). */
    long long s = fk_spos;
    fk_spos = fk_sym_end(fk_spos);
    if (fk_spos == s) {
        /* ZERO-WIDTH symbol: fk_sym_end stops with no progress only on a stray
         * rparen (lparen took the branch above; whitespace/semicolons were
         * skipped). Every statement/operand loop (fk_parse_do, fk_parse_top*,
         * fk_parse_variadic) re-invokes fk_sparse here, so returning without
         * consuming turns that caller into an AST-cap mint spin at one source
         * position -- MEASURED 2026-07-18: 25 bytes of foreign-dialect source
         * (`list(if 1 then 2 else 3);` in a BML section) minted the entire
         * 262144-node table this way; doubling the cap died at the same spot.
         * Diagnose loudly, consume the one character so the parse keeps moving,
         * decline to honest 0. Well-formed Form never lands here: every op/do/
         * variadic path consumes its own matching rparen, and the bare 2-arg
         * value-position let guards its rparen before parsing a body. */
        fk_diag(FK_DIAG_ERR, s,
                "stray ')' in value position -- consumed to keep the parse advancing (foreign-dialect source?)");
        fk_spos = fk_spos + 1;
        return fk_smklit(0);
    }
    long long off = fk_bd_lookup(s, fk_spos - s);
    if (off >= 0) {
        return fk_smknode(110, fk_smklit(off), 0, 0);
    }
    if (fk_sym_eq(s, fk_spos - s, "true")) {
        return fk_smklit(1);
    }
    if (fk_sym_eq(s, fk_spos - s, "false")) {
        return fk_smklit(0);
    }
    long long crow = fk_const_lookup(s, fk_spos - s);
    if (crow >= 0) {
        /* every reference shares the binding's ONE hold node, so the value
         * memo (living in that node's own fields, born empty) is
         * program-wide: one build serves every reference site. */
        if (fk_const_wrapp1[crow] == 0) {
            fk_const_wrapp1[crow] =
                fk_smknode(FK_TAG_CONST_HOLD, fk_const_node[crow], 0, 0) + 1;
        }
        return fk_const_wrapp1[crow] - 1;
    }
    long long vfidx = fk_fn_lookup(s, fk_spos - s);
    if (vfidx >= 0) {
        long long vcc = (vfidx < fk_fn_cap_capacity) ? fk_fn_cap_count[vfidx] : 0;
        if (vcc == 0) {
            /* -1, not 0 -- see the residual defn arm's own version of this same node just
             * above for why (0 is a real node index, not an absent-chain sentinel). */
            return fk_smknode(243, vfidx, -1, 0);
        }
        /* A capturing function's bare name, in value position (returned/stored/passed): build
         * its closure value NOW, reading each captured name fresh off the CURRENT frame -- valid
         * only while that frame is the one its captures were recorded against (see the direct-
         * call arm's own version of this same check, just above the call-resolution table). */
        if (fk_cur_defn_idx != fk_fn_parent_idx[vfidx]) {
            fk_diag(FK_DIAG_ERR, s,
                    "[closure-scope] '%.*s' captures a name from its own enclosing scope, and "
                    "this reference is not directly inside that scope -- reading its captures "
                    "here would read the wrong frame. Read recovered to 0; parse continues",
                    (int)(fk_spos - s), fk_srctext + s);
            return fk_smklit(0);
        }
        long long venvchain = -1;
        long long vce = vcc;
        while (vce > 0) {
            vce = vce - 1;
            long long vencoff = fk_fn_cap_encoff[vfidx * FK_CLOSURE_CAP_MAX + vce];
            venvchain = fk_smknode(242, fk_smknode(110, fk_smklit(vencoff), 0, 0), venvchain, 0);
        }
        return fk_smknode(243, vfidx, venvchain, 0);
    }
    /* A bare name unresolved by every ordinary lookup above may still be a FREE VARIABLE: a name
     * bound in the IMMEDIATELY enclosing defn's own frame (fk_enc_lookup, populated only while
     * parsing a nested defn's body -- see fk_parse_do's "defn" branch). The first reference
     * captures it: a fresh local slot is allocated (ordinary let-style bump, exactly like any
     * other new binding), pushed into THIS defn's own bd so every later reference resolves through
     * the ordinary fk_bd_lookup above instead of back through here, and recorded per-function so
     * (a) this defn's own compiled prologue can populate that slot from fk_call_cap_vals on entry,
     * and (b) every call site that creates or invokes this function knows what to supply. Capped
     * at FK_CLOSURE_CAP_MAX free variables per function -- generous for the real shape (a handful
     * of an enclosing handler-factory's own parameters), and a cap that's HIT diagnoses rather
     * than silently drops the (fcc+1)-th capture. */
    if (fk_cur_defn_idx >= 0 && fk_enc_count > 0) {
        long long encoff = fk_enc_lookup(s, fk_spos - s);
        if (encoff >= 0) {
            long long fcc2 = (fk_cur_defn_idx < fk_fn_cap_capacity) ? fk_fn_cap_count[fk_cur_defn_idx] : 0;
            if (fcc2 < FK_CLOSURE_CAP_MAX) {
                fk_fn_cap_reserve(fk_cur_defn_idx + 1);
                long long capslot = fk_maxslot + 1;
                fk_maxslot = capslot;
                fk_bd_push(s, fk_spos - s, capslot);
                fk_fn_cap_encoff[fk_cur_defn_idx * FK_CLOSURE_CAP_MAX + fcc2] = encoff;
                fk_fn_cap_slot[fk_cur_defn_idx * FK_CLOSURE_CAP_MAX + fcc2] = capslot;
                fk_fn_cap_count[fk_cur_defn_idx] = fcc2 + 1;
                return fk_smknode(110, fk_smklit(capslot), 0, 0);
            }
            fk_diag(FK_DIAG_ERR, s,
                    "[closure-scope] '%.*s' would be this function's %dth captured name (max "
                    "%d) -- not captured",
                    (int)(fk_spos - s), fk_srctext + s, (int)fcc2 + 1, FK_CLOSURE_CAP_MAX);
        }
    }
    /* UNBOUND NAME IN VALUE POSITION. This used to be "an honest 0" — and it was the
     * deepest silent-green in this body. A name that resolves to nothing is not a
     * declined OFFER (that is the tag-137 unresolved-call arm above, which has said so
     * loudly since the ftanh heal); it is a READ of something that was never bound, and
     * a read has no axiom-5 recovery to appeal to. Left silent it does three things,
     * all measured on 2026-07-22 against form-kernel-go as the oracle:
     *   - it makes a band agree with itself. Two walkers reading the same free name both
     *     read 0, both sides match, verdict 255 on deliberately broken code.
     *   - it makes fkwu and the Go/Rust/TS walkers answer the SAME source differently
     *     with neither saying so: fkwu's defn frame cannot see an enclosing do-let by
     *     construction (fk_bd_top = 0 at the defn arm), Go's closure can. fkwu answered
     *     5 where bin-go answered 15.
     *   - it makes fkwu SPIN. A recursion whose base case tests a free name never
     *     reaches it: `(if (eq i n) ..)` with n silently 0 and i starting at 1 ran for
     *     minutes with no output at all, where bin-go answered in 40 ms.
     * So: diagnose, on every occurrence, unconditionally — and still RECOVER to 0, so
     * the rest of the source is parsed and every other offender is reported in the same
     * run. The nonzero exit comes from the error count, exactly like unresolved-call. */
    fk_diag(FK_DIAG_ERR, s,
            "[unbound-name] '%.*s' in value position matched no binding/const/fn -- typo, "
            "missing prelude, or a name from an enclosing scope a defn frame cannot see? "
            "Read recovered to 0; parse continues",
            (int)(fk_spos - s), fk_srctext + s);
    fk_src_unrunnable = 1;
    return fk_smklit(0);
}
/* (do f1 f2 .. fn): sequence forms (tag 69 = eval-first/return-rest). A do-let `(let name val)`
 * binds `name` to the next slot for the REST of the do (the common bind-the-rest pattern). */
static long long fk_parse_do(void) {
    fk_sskip();
    if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
        if (fk_spos < fk_slen) {
            fk_spos = fk_spos + 1;
        }
        return fk_smklit(0);
    }
    if (fk_srctext[fk_spos] == FK_CH_LPAREN) {
        long long p = fk_spos + 1;
        while (p < fk_slen && fk_sws(fk_srctext[p])) {
            p = p + 1;
        }
        long long he = fk_sym_end(p);
        if (fk_sym_eq(p, he - p, "let")) {
            fk_spos = he;
            fk_sskip();
            long long ns = fk_spos;
            fk_spos = fk_sym_end(fk_spos);
            long long nlen = fk_spos - ns;
            long long val = fk_sparse();
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            long long slot = fk_maxslot + 1;
            fk_maxslot = slot;
            fk_bd_push(ns, nlen, slot);
            long long rest = fk_parse_do();
            fk_bd_pop();
            return fk_smknode(109, fk_smklit(slot), val, rest);
        }
        /* A (defn ...) reached HERE is nested: a statement inside another
         * defn's own (do ...), not a top-level form (that case is
         * fk_parse_top_do_value's own "defn" branch, which stays global on
         * purpose). WHAT A NESTED DEFN IS: a registered, independently
         * callable function -- its own fresh fk_fn[] slot and its own
         * frame (params + its own lets, PLUS one slot per name it captures
         * from its immediately-enclosing defn's own frame, delivered by its
         * own compiled prologue -- see fk_wrap_cap_prologue and
         * fk_call_cap_vals) -- NOT an inline statement (the pre-heal bug: it
         * spliced the body into the enclosing do and its own lets clobbered
         * the enclosing frame's live slots, e.g. outer's `x`). Capture is
         * SINGLE-LEVEL and same-scope only: a reference to a name bound
         * further out than the immediate parent, or a direct/self-recursive
         * call to a capturing function from outside the exact scope its
         * captures were read from, still can't be honored -- and still says
         * so loudly (the [unbound-name] / [closure-scope] diagnostics)
         * rather than silently reading whatever slot happened to be live
         * there. model-service.fk's ms-handle reading ms-predict-handler's
         * `model` is the shape this now closes; a self-recursive capturing
         * closure is the shape it still declines.
         *
         * The NAME is registered before its own body is parsed (self- and
         * mutual-recursion among sibling nested defns resolve, same as
         * top-level), visible for the rest of THIS do -- mirrors let's
         * "binds for the rest of its do" just above -- and un-registered
         * once that rest is parsed, so it cannot shadow a same-named
         * outer/top-level function beyond its own scope (the exact shape
         * source-runner-do-defn-band.fk's `hidden`-inside-`local-probe`
         * "defn-body trap" pins). fk_fn_lookup searches its table
         * most-recent-first, so a live inner registration wins over an
         * outer one of the same name while both are in scope. The form's
         * own value is the fn-VALUE (tag 243) -- first-class, storable,
         * returnable -- the same value a bare defn-name already carries in
         * value position; never the body inlined into the caller's frame. */
        if (fk_sym_eq(p, he - p, "defn")) {
            fk_spos = he;
            fk_sskip();
            long long dns = fk_spos;
            fk_spos = fk_sym_end(fk_spos);
            long long dnlen = fk_spos - dns;
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_LPAREN) {
                fk_spos = fk_spos + 1;
            }
            long long fk_bd_saved_top = fk_bd_save();
            long long fk_bd_saved_maxslot = fk_maxslot;
            long long denc_count_here = fk_bd_top;
            fk_bd_top = 0;
            fk_maxslot = 0;
            long long saved_fntop = fk_fntop;
            long long didx = fk_defn_next;
            fk_defn_next = fk_defn_next + 1;
            fk_fn_reserve(fk_defn_next);
            fk_fnsym_s[fk_fntop] = dns;
            fk_fnsym_n[fk_fntop] = dnlen;
            fk_fnidx[fk_fntop] = didx;
            fk_live_note_defn(fk_fntop);
            fk_fntop = fk_fntop + 1;
            fk_fn_cap_reserve(didx + 1);
            fk_fn_parent_idx[didx] = fk_cur_defn_idx;
            long long dsaved_enc_mark = fk_enc_mark;
            long long dsaved_enc_count = fk_enc_count;
            fk_enc_mark = fk_bd_saved_top;
            fk_enc_count = denc_count_here;
            long long dsaved_cur_defn_idx = fk_cur_defn_idx;
            fk_cur_defn_idx = didx;
            long long dna = 0;
            while (1) {
                fk_sskip();
                if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
                    break;
                }
                long long das = fk_spos;
                fk_spos = fk_sym_end(fk_spos);
                if (dna == 0 && fk_spos > das && fk_divergent_param_name(das, fk_spos - das)) {
                    fk_diag(FK_DIAG_ERR, das,
                            "[shadowed-primitive] parameter '%.*s' names a primitive/control "
                            "form -- in call position the primitive still wins, so the "
                            "parameter is reachable in value position only, and "
                            "form-kernel-go drops it from the parameter list entirely "
                            "(arity divergence). Rename the parameter",
                            (int)(fk_spos - das), fk_srctext + das);
                    fk_src_unrunnable = 1;
                }
                fk_bd_push(das, fk_spos - das, dna);
                if (dna > fk_maxslot) {
                    fk_maxslot = dna;
                }
                dna = dna + 1;
            }
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            fk_fnar[didx] = dna;
            long long dbody = fk_sparse();
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            dbody = fk_wrap_cap_prologue(didx, dbody);
            if (fk_maxslot > 0) {
                dbody = fk_smknode(111, fk_smklit(fk_maxslot), dbody, 0);
            }
            fk_fn[didx] = dbody; fk_prog_note_body(didx);
            fk_bd_restore(fk_bd_saved_top);
            fk_maxslot = fk_bd_saved_maxslot;
            fk_enc_mark = dsaved_enc_mark;
            fk_enc_count = dsaved_enc_count;
            fk_cur_defn_idx = dsaved_cur_defn_idx;
            long long rest2 = fk_parse_do();
            fk_fntop = saved_fntop;
            /* -1, not 0 -- this value is always discarded by the tag-69 sequence (something
             * always follows a defn statement in a do), but it is still WALKED for its side
             * effect of nothing, so a stray 0 here would still needlessly fk_clo_make an
             * unused closure instance every time -- see the residual defn arm for the full
             * reasoning on why 0 is unsafe as an absent-chain sentinel here. */
            return fk_smknode(69, fk_smknode(243, didx, -1, 0), rest2, 0);
        }
    }
    long long node = fk_sparse();
    fk_sskip();
    if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
        if (fk_spos < fk_slen) {
            fk_spos = fk_spos + 1;
        }
        return node;
    }
    long long rest = fk_parse_do();
    return fk_smknode(69, node, rest, 0);
}
/* Top-level (do ...) has one extra responsibility over ordinary value-position
 * do: a defn remains a top-level definition even if it appears after a
 * value-bearing let. The ordinary fk_parse_do path must not grow that behavior;
 * nested do in function bodies stays value-level. */
static long long fk_parse_top_do_value(void) {
    fk_sskip();
    if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
        if (fk_spos < fk_slen) {
            fk_spos = fk_spos + 1;
        }
        return fk_smklit(0);
    }
    if (fk_srctext[fk_spos] == FK_CH_LPAREN) {
        long long p = fk_spos + 1;
        while (p < fk_slen && fk_sws(fk_srctext[p])) {
            p = p + 1;
        }
        long long he = fk_sym_end(p);
        if (fk_sym_eq(p, he - p, "defn")) {
            long long save_bd_top = fk_bd_top;
            long long save_maxslot = fk_maxslot;
            long long save_s[128], save_n[128], save_off[128];
            long long si = 0;
            while (si < save_bd_top && si < 128) {
                save_s[si] = fk_bd_s[si];
                save_n[si] = fk_bd_n[si];
                save_off[si] = fk_bd_off[si];
                si = si + 1;
            }
            fk_parse_top();
            si = 0;
            while (si < save_bd_top && si < 128) {
                fk_bd_s[si] = save_s[si];
                fk_bd_n[si] = save_n[si];
                fk_bd_off[si] = save_off[si];
                si = si + 1;
            }
            fk_bd_top = save_bd_top;
            fk_maxslot = save_maxslot;
            return fk_parse_top_do_value();
        }
        if (fk_sym_eq(p, he - p, "let")) {
            fk_spos = he;
            fk_sskip();
            long long ns = fk_spos;
            fk_spos = fk_sym_end(fk_spos);
            long long nlen = fk_spos - ns;
            long long val = fk_sparse();
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            long long slot = fk_maxslot + 1;
            fk_maxslot = slot;
            fk_bd_push(ns, nlen, slot);
            long long rest = fk_parse_top_do_value();
            fk_bd_pop();
            return fk_smknode(109, fk_smklit(slot), val, rest);
        }
    }
    long long node = fk_sparse();
    fk_sskip();
    if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
        if (fk_spos < fk_slen) {
            fk_spos = fk_spos + 1;
        }
        return node;
    }
    long long rest = fk_parse_top_do_value();
    return fk_smknode(69, node, rest, 0);
}
/* Fixed operand carrier for primitives whose evaluator expects one list-valued
 * child instead of flat AST child slots. Tag 91 (make_nodeid) reads child 1 as
 * pkg, level, type, inst through the runtime cons-list carrier. */
static long long fk_parse_fixed_list(long long n) {
    if (n <= 0) {
        return fk_smknode(18, 0, 0, 0);
    }
    long long h = fk_sparse();
    long long t = fk_parse_fixed_list(n - 1);
    return fk_smknode(19, h, t, 0);
}
/* GENERIC VARIADIC FOLD (arity -1 in fk_optab). Parse operands until the close paren and fold them
 * right into a chain via `tag` ending in nil (tag 18). For (list a b ..) tag is cons/19, yielding
 * cons(a, cons(b, .. empty)); the closing ) yields empty (tag 18, the nil value 1). This is the ONE
 * mechanism that makes `list` a data row instead of a hand-written C case — any future variadic
 * structural form is another (name -1 tag) manifest row, never a C edit. */
static long long fk_parse_variadic(long long tag) {
    fk_sskip();
    if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
        if (fk_spos < fk_slen) {
            fk_spos = fk_spos + 1;
        }
        return fk_smknode(18, 0, 0, 0);
    }
    long long h = fk_sparse();
    long long t = fk_parse_variadic(tag);
    return fk_smknode(tag, h, t, 0);
}
/* record_new SPECIAL SHAPE (arity -1, tag 64 in fk_optab): (record_new bp k1 v1 k2 v2 ..)
 * lowers to ONE tag-64 node whose single child is the cons-chain entry packet
 * ((-1 bp) (k1 v1) (k2 v2) ..) — byte-for-byte the shape flt-record-new emits and the
 * tag-64 walker installs. The blueprint rides an entry whose key is the -1 int literal:
 * fk_stri of a non-string answers -1, which is the walker's blueprint sentinel. The
 * generic arity -1 fold would chain the raw operands on tag 64 itself — record fields
 * come in PAIRS, so the pairing is a parse-time fact and lives here. */
static long long fk_parse_record_entry(long long k, long long v) {
    return fk_smknode(19, k, fk_smknode(19, v, fk_smknode(18, 0, 0, 0), 0), 0);
}
static long long fk_parse_record_fields(void) {
    fk_sskip();
    if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
        if (fk_spos < fk_slen) {
            fk_spos = fk_spos + 1;
        }
        return fk_smknode(18, 0, 0, 0);
    }
    long long ks = fk_spos;
    long long k = fk_sparse();
    fk_sskip();
    if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
        /* a dangling key silently dropped would be a partial record accepted as
         * whole — the same numb shape the tag-64 walker refuses at its caps. */
        fk_diag(FK_DIAG_ERR, ks,
                "record_new: field key without a value -- fields come in (key value) pairs");
        if (fk_spos < fk_slen) {
            fk_spos = fk_spos + 1;
        }
        return fk_smknode(18, 0, 0, 0);
    }
    long long v = fk_sparse();
    long long e = fk_parse_record_entry(k, v);
    return fk_smknode(19, e, fk_parse_record_fields(), 0);
}
static long long fk_parse_record_new(void) {
    fk_sskip();
    if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
        fk_diag(FK_DIAG_ERR, fk_spos, "record_new: missing blueprint operand");
        if (fk_spos < fk_slen) {
            fk_spos = fk_spos + 1;
        }
        return fk_smknode(137, 0, 0, 0);
    }
    long long bp = fk_sparse();
    long long sent = fk_parse_record_entry(fk_smklit(0 - 1), bp);
    return fk_smknode(64, fk_smknode(19, sent, fk_parse_record_fields(), 0), 0, 0);
}
extern int atoi(const char *);
/* stone 4 (two-pass): PRE-SCAN. The body parse below registers each (defn ...) as it reaches it,
 * one pass — so a call to a LATER-defined function misses fk_fn_lookup and lowers to a no-op
 * (forward and mutual references fail). This pre-scan walks the source first and registers every
 * top-level defn's name + fn-index + arity BEFORE any body is parsed, so all names are known when
 * bodies lower. It only registers (it builds no bodies); the body pass then LOOKS UP the index
 * already registered for each name (fk_fn_lookup) and just fills fk_fn[idx]. Container shape
 * mirrors fk_parse_top exactly: a top-level (do ...) is transparent — its inner forms scan as
 * top-level too (recursively), so defns inside the root do register; a bare top-level (defn ...)
 * registers directly; anything else is opaque (skipped as one balanced form). Read-only over
 * fk_srctext; leaves fk_spos untouched (operates on a local cursor). */
static void fk_sskip_at(long long *pp) {
    long long p = *pp;
    while (p < fk_slen) {
        char c = fk_srctext[p];
        if (fk_sws(c)) {
            p = p + 1;
        } else if (c == FK_CH_SEMI) {
            while (p < fk_slen && fk_srctext[p] != FK_CH_LF) {
                p = p + 1;
            }
        } else {
            break;
        }
    }
    *pp = p;
}
static long long fk_skip_balanced(long long p) {
    /* p sits just past a '(' or at a leaf token; skip one whole form, return position just past it.
     */
    fk_sskip_at(&p);
    if (p >= fk_slen) {
        return p;
    }
    if (fk_srctext[p] == FK_CH_LPAREN) {
        long long depth = 1;
        p = p + 1;
        while (p < fk_slen && depth > 0) {
            char c = fk_srctext[p];
            if (c == FK_CH_SEMI) {
                while (p < fk_slen && fk_srctext[p] != FK_CH_LF) {
                    p = p + 1;
                }
                continue;
            }
            /* string literals are opaque to the balance: a ( or ) inside
               "..." is content, not structure. Without this guard a paren
               inside any string desynchronized the prescan and hung the
               parse (found 2026-07-02 recording a human's verbatim answer
               containing ":)"). Mirrors the string guard the unknown-head
               skip loop always had; \" stays inside the string. */
            if (c == FK_CH_DQUOTE) {
                p = p + 1;
                while (p < fk_slen && fk_srctext[p] != FK_CH_DQUOTE) {
                    if (fk_srctext[p] == FK_CH_BACKSLASH && p + 1 < fk_slen) {
                        p = p + 1;
                    }
                    p = p + 1;
                }
                if (p < fk_slen) {
                    p = p + 1;
                }
                continue;
            }
            if (c == FK_CH_LPAREN) {
                depth = depth + 1;
            } else if (c == FK_CH_RPAREN) {
                depth = depth - 1;
            }
            p = p + 1;
        }
        return p;
    }
    return fk_sym_end(p);
}
static void fk_prescan_seq(long long *pp);
static void fk_prescan_form(long long *pp) {
    long long p = *pp;
    fk_sskip_at(&p);
    if (p >= fk_slen || fk_srctext[p] != FK_CH_LPAREN) {
        *pp = fk_skip_balanced(p);
        return;
    }
    long long h = p + 1;
    while (h < fk_slen && fk_sws(fk_srctext[h])) {
        h = h + 1;
    }
    long long he = fk_sym_end(h);
    if (fk_sym_eq(h, he - h, "do")) {
        /* transparent: scan inner sequence to the matching close, then consume it */
        long long q = he;
        fk_prescan_seq(&q);
        *pp = q;
        return;
    }
    if (fk_sym_eq(h, he - h, "defn")) {
        long long ns = he;
        fk_sskip_at(&ns);
        long long ne = fk_sym_end(ns);
        long long nlen = ne - ns;

        /* register the name at the NEXT fn-index, mirroring the body pass's allocation order */
        long long idx = fk_defn_next;
        fk_defn_next = fk_defn_next + 1;
        fk_fn_reserve(fk_defn_next);
        fk_fnsym_s[fk_fntop] = ns;
        fk_fnsym_n[fk_fntop] = nlen;
        fk_fnidx[fk_fntop] = idx;
        fk_live_note_defn(fk_fntop);
        fk_fntop = fk_fntop + 1;

        /* count arity from the (ARGS...) list so self/forward calls read it */
        long long a = ne;
        fk_sskip_at(&a);
        long long na = 0;
        if (a < fk_slen && fk_srctext[a] == FK_CH_LPAREN) {
            a = a + 1;
            while (1) {
                fk_sskip_at(&a);
                if (a >= fk_slen || fk_srctext[a] == FK_CH_RPAREN) {
                    break;
                }
                a = fk_sym_end(a);
                na = na + 1;
            }
        }
        fk_fnar[idx] = na;
        *pp = fk_skip_balanced(p);
        /* skip the whole defn form opaquely */
        return;
    }
    *pp = fk_skip_balanced(p);
}
static void fk_prescan_seq(long long *pp) {
    long long p = *pp;
    while (1) {
        fk_sskip_at(&p);
        if (p >= fk_slen) {
            *pp = p;
            return;
        }
        if (fk_srctext[p] == FK_CH_RPAREN) {
            *pp = p + 1;
            return;
        }
        fk_prescan_form(&p);
    }
}
static void fk_prescan_defns(void) {
    long long p = 0;
    while (1) {
        fk_sskip_at(&p);
        if (p >= fk_slen) {
            break;
        }
        fk_prescan_form(&p);
    }
}
/* one top-level form: (do ...) is transparent (its inner forms are top-level too); (defn ...)
 * registers a function at its own index; anything else is the root expression. Multi-arg defns push
 * each arg name to slots 0..k-1 (callable single-arg via tag 12 today; multi-arg calls are the next
 * stone). */
static void fk_parse_top(void) {
    fk_sskip();
    if (fk_spos >= fk_slen) {
        return;
    }
    if (fk_srctext[fk_spos] == FK_CH_LPAREN) {
        long long p = fk_spos + 1;
        while (p < fk_slen && fk_sws(fk_srctext[p])) {
            p = p + 1;
        }
        long long he = fk_sym_end(p);
        if (fk_sym_eq(p, he - p, "do")) {
            /* A top-level (do ...) is the root form. Leading (defn ...) inner forms register as
             * named functions so cross-calls resolve (four-way-run.fk is two such defns), and a
             * leading nested (do ...) stays TRANSPARENT — its own defns register and its value
             * becomes the root, exactly as the old loop did (the optable generator wraps its defns
             * in one such nested do). Both keep going through fk_parse_top, which carries defn
             * registration. The FIRST value-bearing inner form (a let or an expression) begins the
             * root value-sequence, parsed by the top-level-do value parser so
             * a do-let binds for the REST of the do (tag 109), later defns
             * still fill their prescanned function bodies, and ordinary forms
             * sequence (tag 69). The ordinary fk_parse_do path remains for
             * value-level nested do. The parser consumes this do's closing )
             * itself; a do of only defns leaves fk_root unset so the last defn
             * becomes the root. */
            fk_spos = he;
            while (1) {
                fk_sskip();
                if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
                    if (fk_spos < fk_slen) {
                        fk_spos = fk_spos + 1;
                    }
                    return;
                }
                if (fk_srctext[fk_spos] == FK_CH_LPAREN) {
                    long long q = fk_spos + 1;
                    while (q < fk_slen && fk_sws(fk_srctext[q])) {
                        q = q + 1;
                    }
                    long long qe = fk_sym_end(q);
                    if (fk_sym_eq(q, qe - q, "defn") || fk_sym_eq(q, qe - q, "do")) {
                        fk_parse_top();
                        continue;
                    }
                }
                fk_bd_top = 0;
                fk_maxslot = 0;
                long long dv = fk_parse_top_do_value();
                if (fk_maxslot > 0) {
                    dv = fk_smknode(111, fk_smklit(fk_maxslot), dv, 0);
                }
                fk_root = fk_root >= 0 ? fk_smknode(69, fk_root, dv, 0) : dv;
                return;
            }
        }
        if (fk_sym_eq(p, he - p, "defn")) {
            fk_spos = he;
            fk_sskip();
            long long ns2 = fk_spos;
            fk_spos = fk_sym_end(fk_spos);
            long long nlen2 = fk_spos - ns2;

            /* two-pass: the pre-scan (fk_prescan_defns) already registered this name + index +
             * arity. LOOK UP the index it assigned rather than allocating a fresh one, so the
             * fn-index the body fills matches the one every call site (incl. forward/mutual
             * references) resolves to. Fallback to the old allocate-on-the-fly path only if the
             * name is somehow unregistered (defensive; pre-scan covers all top-level defns). */
            long long idx = fk_fn_lookup(ns2, nlen2);
            if (idx < 0) {
                idx = fk_defn_next;
                fk_defn_next = fk_defn_next + 1;
                fk_fn_reserve(fk_defn_next);
                fk_fnsym_s[fk_fntop] = ns2;
                fk_fnsym_n[fk_fntop] = nlen2;
                fk_fnidx[fk_fntop] = idx;
                fk_live_note_defn(fk_fntop);
                fk_fntop = fk_fntop + 1;
            }
            fk_fname_s = ns2;
            fk_fname_n = nlen2;
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_LPAREN) {
                fk_spos = fk_spos + 1;
            }
            /* SCOPE FIX (same as fk_sparse's defn arm above): save/restore the
             * enclosing scope's live bindings around this defn's frame (f99d3232). */
            long long fk_bd_saved_top = fk_bd_save();
            long long fk_bd_saved_maxslot = fk_maxslot;
            fk_bd_top = 0;
            fk_maxslot = 0;
            /* A top-level defn has no enclosing frame at all -- fk_enc_count MUST be 0 here
             * (never a leftover from wherever the top-level parse loop last left it) so a free
             * name inside this body is never mistaken for a capture; fk_fn_parent_idx[idx]=-1
             * marks it as having no parent to match against either. */
            fk_fn_cap_reserve(idx + 1);
            fk_fn_parent_idx[idx] = -1;
            long long tsaved_enc_mark = fk_enc_mark;
            long long tsaved_enc_count = fk_enc_count;
            fk_enc_mark = 0;
            fk_enc_count = 0;
            long long tsaved_cur_defn_idx = fk_cur_defn_idx;
            fk_cur_defn_idx = idx;
            long long na = 0;
            while (1) {
                fk_sskip();
                if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
                    break;
                }
                long long as = fk_spos;
                fk_spos = fk_sym_end(fk_spos);
                if (na == 0 && fk_spos > as && fk_divergent_param_name(as, fk_spos - as)) {
                    fk_diag(FK_DIAG_ERR, as,
                            "[shadowed-primitive] parameter '%.*s' names a primitive/control form "
                            "-- in call position the primitive still wins, so the parameter is "
                            "reachable in value position only, and form-kernel-go drops it from "
                            "the parameter list entirely (arity divergence). Rename the parameter",
                            (int)(fk_spos - as), fk_srctext + as);
                    fk_src_unrunnable = 1;
                }
                fk_bd_push(as, fk_spos - as, na);
                if (na > fk_maxslot) {
                    fk_maxslot = na;
                }
                na = na + 1;
            }
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            fk_fnar[idx] = na;
            /* arity known before body -> self-recursive calls read it */
            long long body = fk_sparse();
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            fk_enc_mark = tsaved_enc_mark;
            fk_enc_count = tsaved_enc_count;
            fk_cur_defn_idx = tsaved_cur_defn_idx;
            if (fk_maxslot > 0) {
                body = fk_smknode(111, fk_smklit(fk_maxslot), body, 0);
            }
            fk_fn[idx] = body; fk_prog_note_body(idx);
            fk_bd_restore(fk_bd_saved_top);
            fk_maxslot = fk_bd_saved_maxslot;
            return;
        }
        if (fk_sym_eq(p, he - p, "let")) {
            fk_spos = he;
            fk_sskip();
            long long ns3 = fk_spos;
            fk_spos = fk_sym_end(fk_spos);
            long long nlen3 = fk_spos - ns3;
            long long save_bd_top = fk_bd_top;
            long long save_maxslot = fk_maxslot;
            fk_bd_top = 0;
            fk_maxslot = 0;
            long long val3 = fk_sparse();
            if (fk_maxslot > 0) {
                val3 = fk_smknode(111, fk_smklit(fk_maxslot), val3, 0);
            }
            fk_bd_top = save_bd_top;
            fk_maxslot = save_maxslot;
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            fk_const_set(ns3, nlen3, val3);
            /* the let joins the root sequence through the binding's ONE hold
             * node (the reference-site idiom above), so its value is built at
             * its own textual position exactly once and later references read
             * the memo -- the Go arm's implicit-do let is eager the same way. */
            long long crow3 = fk_const_lookup(ns3, nlen3);
            if (fk_const_wrapp1[crow3] == 0) {
                fk_const_wrapp1[crow3] =
                    fk_smknode(FK_TAG_CONST_HOLD, fk_const_node[crow3], 0, 0) + 1;
            }
            long long held3 = fk_const_wrapp1[crow3] - 1;
            fk_root = fk_root >= 0 ? fk_smknode(69, fk_root, held3, 0) : held3;
            return;
        }
    }
    /* EVERY top-level form joins the root sequence (tag 69, eval-first/
     * return-rest), earlier forms first, the last form's value the answer.
     * `fk_root = fk_sparse()` alone OVERWROTE the root per form, so only the
     * LAST top-level statement ever ran: (print_str "x") followed by 0 printed
     * nothing and answered 0, and a write_file before a trailing value never
     * touched the disk -- every earlier statement was parsed, then dropped.
     * The Go arm wraps multiple top-level forms in an implicit do
     * (readRootFromSource) and runs them all; dropping them here was a live
     * four-way divergence, witnessed 2026-09-02 through a direct .bml whose
     * lowering is exactly such a statement sequence. */
    long long stmt = fk_sparse();
    fk_root = fk_root >= 0 ? fk_smknode(69, fk_root, stmt, 0) : stmt;
}
static long long fk_path_len(const char *p) {
    long long n = 0;
    while (p[n] != 0) {
        n = n + 1;
    }
    return n;
}
static int fk_path_has_suffix(const char *src, const char *suffix) {
    long long n = fk_path_len(src);
    long long sn = fk_path_len(suffix);
    if (sn > n) {
        return 0;
    }
    long long i = 0;
    while (i < sn) {
        if (src[n - sn + i] != suffix[i]) {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}
static int fk_path_replace_ext(const char *src, const char *ext, char *out, long long cap) {
    long long n = fk_path_len(src);
    long long en = fk_path_len(ext);
    long long base = n;
    if (n >= 3 && src[n - 3] == FK_CH_DOT && src[n - 2] == FK_CH_LOWER_F &&
        src[n - 1] == FK_CH_LOWER_K) {
        base = n - 3;
    }
    if (base + en + 1 > cap) {
        return 0;
    }
    long long i = 0;
    while (i < base) {
        out[i] = src[i];
        i = i + 1;
    }
    long long j = 0;
    while (j < en) {
        out[i + j] = ext[j];
        j = j + 1;
    }
    out[i + j] = 0;
    return 1;
}
static long long fk_path_mtime_raw(const char *p) {
#ifdef FK_HAVE_STAT_HEADER
    struct stat st;
    if (stat(p, &st) != 0) {
        return -1;
    }
    return (long long)st.st_mtime;
#else
    (void)p;
    return -1;
#endif
}
static long long fk_path_size_raw(const char *p) {
#ifdef FK_HAVE_STAT_HEADER
    struct stat st;
    if (stat(p, &st) != 0) {
        return -1;
    }
    return (long long)st.st_size;
#else
    int fd = open(p, 0);
    if (fd < 0) {
        return -1;
    }
    long n = lseek(fd, 0, 2);
    close(fd);
    return (long long)n;
#endif
}
static int fk_write_all_raw(int fd, const void *buf, unsigned long n) {
    unsigned long done = 0;
    const char *p = (const char *)buf;
    while (done < n) {
        long long w = write(fd, p + done, n - done);
        if (w < 0 && errno == EINTR) continue;
        if (w <= 0) {
            return 0;
        }
        done = done + (unsigned long)w;
    }
    return 1;
}
static void fk_diag_path(const char *level, const char *path, const char *msg) {
    fk_write_all_raw(2, "fkwu: ", 6);
    fk_write_all_raw(2, level, (unsigned long)fk_path_len(level));
    fk_write_all_raw(2, ": ", 2);
    if (path != 0 && path[0] != 0) {
        fk_write_all_raw(2, path, (unsigned long)fk_path_len(path));
        fk_write_all_raw(2, ": ", 2);
    }
    fk_write_all_raw(2, msg, (unsigned long)fk_path_len(msg));
    fk_write_all_raw(2, "\n", 1);
}
#define FK_SRC_HASH_CAP 16384
/* A source unit is a graph, not a fixed-width table.  The former 128-file
 * collector meant an otherwise valid Form closure could disappear before the
 * source/JIT door even parsed it.  Keep the metadata heap-backed and grow it
 * with the observed graph; runtime meaning remains in Form, this is only the
 * temporary seed's source-loader bookkeeping. */
static char (*fk_src_dep_path)[FK_PATH_CAP];
static long long *fk_src_dep_mtime;
static long long *fk_src_dep_size;
/* CONTENT DIGEST per dependency. The artifact identity used to be
 * path@mtime:size and the code that wrote it said "source path, content, or
 * mtime changed" -- but content was never in it. Two sources of the SAME LENGTH
 * written inside the SAME mtime second are indistinguishable, and fkwu then
 * runs the previous program and prints its answer with no warning and exit 0.
 * Witnessed 2026-07-30: `(do 111)` and `(do 222)` are both 41 bytes with the
 * preludes line; the second run printed 111. Same-length edits are the common
 * case, not the exotic one -- a verdict pin, a constant, an operator, a depth.
 * FNV-1a over the dependency's bytes, taken where the bytes are already in
 * hand, closes it. */
static unsigned long long *fk_src_dep_digest;
static unsigned long long fk_bytes_fnv1a(const char *p, long long n) {
    unsigned long long h = 14695981039346656037ULL;
    long long k = 0;
    while (k < n) {
        h = h ^ (unsigned long long)(unsigned char)p[k];
        h = h * 1099511628211ULL;
        k = k + 1;
    }
    return h;
}
static long long *fk_src_dep_parent;
static long long *fk_src_dep_end;
/* WHERE each unit's OWN text landed in fk_srctext (offset/length of the one
 * fk_src_append_text call made for it; deps' text lies outside this range).
 * The import lane reads these to CARRY every unit an accepted image does not
 * cover -- a direct .bml prelude's whole floor-lane subtree used to be
 * dropped here without a word: images in, root text in, and the .bml's
 * lowered defns simply absent, so every later call site went numb as
 * [unresolved-call] with nothing naming the drop (witnessed 2026-09-01,
 * peer-contribution birth surface). */
static long long *fk_src_dep_text_off;
static long long *fk_src_dep_text_len;
/* 1 when the unit's text reached fk_srctext through the lowering lane: its own
 * file wears .bml or carries a `section [` block. The image and carry loops read
 * this fact instead of re-testing a suffix, so a lowered unit is carried whole
 * whatever its extension. */
static char *fk_src_dep_lowered;
static long long fk_src_dep_count;
static long long fk_src_dep_cap;
static char fk_src_root_path[FK_PATH_CAP];
static char *fk_src_root_text;
static long long fk_src_root_cap;
static long long fk_src_root_len;
static void fk_src_root_reserve(long long need) {
    long long nc;
    char *q;
    if (fk_src_root_cap == 0) {
        fk_src_root_text = malloc(FK_SOURCE_TEXT_CAP_INIT);
        if (fk_src_root_text == 0) {
            fk_die("fk_src_root_reserve: out of memory for the root source buffer");
        }
        fk_src_root_cap = FK_SOURCE_TEXT_CAP_INIT;
    }
    if (need <= fk_src_root_cap) {
        return;
    }
    nc = fk_src_root_cap;
    while (nc < need) {
        nc = nc * 2;
    }
    q = realloc(fk_src_root_text, (unsigned long)nc);
    if (q == 0) {
        fk_die("fk_src_root_reserve: out of memory growing the root source buffer");
    }
    fk_src_root_text = q;
    fk_src_root_cap = nc;
}

static void fk_cstr_copy(char *dst, const char *src, long long cap);

static int fk_src_dep_reserve(long long want) {
    long long cap = fk_src_dep_cap;
    long long i = 0;
    char (*paths)[FK_PATH_CAP];
    long long *mtimes;
    long long *sizes;
    unsigned long long *digests;
    long long *parents;
    long long *ends;
    long long *text_offs;
    long long *text_lens;
    char *lowered;
    if (want <= fk_src_dep_cap) {
        return 1;
    }
    if (cap <= 0) {
        cap = 32;
    }
    while (cap < want) {
        if (cap > 4611686018427387903LL) {
            return 0;
        }
        cap = cap * 2;
    }
    /* The path row is the widest allocation. Reject an allocation-size
     * overflow before casting the observed count to the allocator's size. */
    if ((unsigned long long)cap > (unsigned long long)(unsigned long)-1 ||
        (unsigned long)cap > ((unsigned long)-1) / sizeof(*paths)) {
        return 0;
    }
    paths = malloc(sizeof(*paths) * (unsigned long)cap);
    mtimes = malloc(sizeof(*mtimes) * (unsigned long)cap);
    sizes = malloc(sizeof(*sizes) * (unsigned long)cap);
    digests = malloc(sizeof(*digests) * (unsigned long)cap);
    parents = malloc(sizeof(*parents) * (unsigned long)cap);
    ends = malloc(sizeof(*ends) * (unsigned long)cap);
    text_offs = malloc(sizeof(*text_offs) * (unsigned long)cap);
    text_lens = malloc(sizeof(*text_lens) * (unsigned long)cap);
    lowered = malloc(sizeof(*lowered) * (unsigned long)cap);
    if (paths == 0 || mtimes == 0 || sizes == 0 || digests == 0 ||
        parents == 0 || ends == 0 || text_offs == 0 || text_lens == 0 || lowered == 0) {
        free(paths); free(mtimes); free(sizes); free(digests);
        free(parents); free(ends); free(text_offs); free(text_lens); free(lowered);
        return 0;
    }
    while (i < fk_src_dep_count) {
        fk_cstr_copy(paths[i], fk_src_dep_path[i], FK_PATH_CAP);
        mtimes[i] = fk_src_dep_mtime[i];
        sizes[i] = fk_src_dep_size[i];
        digests[i] = fk_src_dep_digest[i];
        parents[i] = fk_src_dep_parent[i];
        ends[i] = fk_src_dep_end[i];
        text_offs[i] = fk_src_dep_text_off[i];
        text_lens[i] = fk_src_dep_text_len[i];
        lowered[i] = fk_src_dep_lowered[i];
        i = i + 1;
    }
    free(fk_src_dep_path); free(fk_src_dep_mtime); free(fk_src_dep_size);
    free(fk_src_dep_digest); free(fk_src_dep_parent); free(fk_src_dep_end);
    free(fk_src_dep_text_off); free(fk_src_dep_text_len); free(fk_src_dep_lowered);
    fk_src_dep_path = paths;
    fk_src_dep_mtime = mtimes;
    fk_src_dep_size = sizes;
    fk_src_dep_digest = digests;
    fk_src_dep_parent = parents;
    fk_src_dep_end = ends;
    fk_src_dep_text_off = text_offs;
    fk_src_dep_text_len = text_lens;
    fk_src_dep_lowered = lowered;
    fk_src_dep_cap = cap;
    return 1;
}

static void fk_src_dep_release(void) {
    free(fk_src_dep_path); free(fk_src_dep_mtime); free(fk_src_dep_size);
    free(fk_src_dep_digest); free(fk_src_dep_parent); free(fk_src_dep_end);
    free(fk_src_dep_text_off); free(fk_src_dep_text_len); free(fk_src_dep_lowered);
    fk_src_dep_path = 0;
    fk_src_dep_mtime = 0;
    fk_src_dep_size = 0;
    fk_src_dep_digest = 0;
    fk_src_dep_parent = 0;
    fk_src_dep_end = 0;
    fk_src_dep_text_off = 0;
    fk_src_dep_text_len = 0;
    fk_src_dep_lowered = 0;
    fk_src_dep_count = 0;
    fk_src_dep_cap = 0;
}

static void fk_cstr_copy(char *dst, const char *src, long long cap) {
    long long i = 0;
    if (cap <= 0) {
        return;
    }
    while (i + 1 < cap && src[i] != 0) {
        dst[i] = src[i];
        i = i + 1;
    }
    dst[i] = 0;
}
static int fk_cstr_eq(const char *a, const char *b) {
    long long i = 0;
    while (a[i] != 0 && b[i] != 0) {
        if (a[i] != b[i]) {
            return 0;
        }
        i = i + 1;
    }
    return a[i] == b[i];
}
static int fk_src_dep_index(const char *path) {
    long long i = 0;
    while (i < fk_src_dep_count) {
        if (fk_cstr_eq(fk_src_dep_path[i], path)) {
            return (int)i;
        }
        i = i + 1;
    }
    return -1;
}
static int fk_path_is_abs(const char *path) {
    if (path[0] == FK_CH_SLASH) {
        return 1;
    }
    if (((path[0] >= FK_CH_UPPER_A && path[0] <= FK_CH_UPPER_Z) ||
         (path[0] >= FK_CH_LOWER_A && path[0] <= FK_CH_LOWER_Z)) &&
        path[1] == FK_CH_COLON) {
        return 1;
    }
    return 0;
}
static long long fk_path_dir_len(const char *path) {
    long long i = 0;
    long long last = -1;
    while (path[i] != 0) {
        if (path[i] == FK_CH_SLASH) {
            last = i;
        }
        i = i + 1;
    }
    return last >= 0 ? last + 1 : 0;
}
/* the lexical repo root: length of the prefix ending at the slash before the
 * path's first form/ or learn/ component (0 when absent). Shared by the
 * prelude resolver's repo-root rescue and the .fkb identity spelling. */
static long long fk_path_repo_prefix_len(const char *path) {
    long long s = 0;
    while (path[s] != 0) {
        if (path[s] == FK_CH_SLASH &&
            ((path[s + 1] == 'f' && path[s + 2] == 'o' && path[s + 3] == 'r' &&
              path[s + 4] == 'm' && path[s + 5] == FK_CH_SLASH) ||
             (path[s + 1] == 'l' && path[s + 2] == 'e' && path[s + 3] == 'a' &&
              path[s + 4] == 'r' && path[s + 5] == 'n' && path[s + 6] == FK_CH_SLASH))) {
            return s + 1;
        }
        s = s + 1;
    }
    return 0;
}
#if defined(_WIN32)
extern char *_fullpath(char *, const char *, unsigned long long);
#else
extern char *realpath(const char *, char *);
#endif
/* the .fkb identity spelling of a path: one absolute spelling regardless of
 * invocation CWD, then anchored at the lexical repo root so the same file
 * keeps one name across CWD flips and checkout moves. out must hold FK_PATH_CAP
 * bytes. Verbatim on resolve failure -- degrade to the old spelling (an
 * honest rebuild), never fabricate an identity. */
static const char *fk_path_canon_id(const char *path, char *out) {
#if defined(_WIN32)
    if (_fullpath(out, path, FK_PATH_CAP) == 0) {
        return path;
    }
#else
    if (realpath(path, out) == 0) {
        return path;
    }
#endif
    long long pre_n = fk_path_repo_prefix_len(out);
    return pre_n > 0 ? out + pre_n : out;
}
static int fk_path_resolve_fk_dep(const char *owner_path, const char *token, long long token_n,
                                  char *out, long long cap) {
    if (token_n <= 0 || cap <= token_n) {
        return 0;
    }
    if (token[0] == FK_CH_SLASH ||
        (token_n > 1 && ((token[0] >= FK_CH_UPPER_A && token[0] <= FK_CH_UPPER_Z) ||
                         (token[0] >= FK_CH_LOWER_A && token[0] <= FK_CH_LOWER_Z)) &&
         token[1] == FK_CH_COLON)) {
        long long i = 0;
        while (i < token_n) {
            out[i] = token[i];
            i = i + 1;
        }
        out[token_n] = 0;
        return 1;
    }
    long long dir_n = fk_path_dir_len(owner_path);
    if (dir_n + token_n + 1 > cap) {
        return 0;
    }
    long long i = 0;
    while (i < dir_n) {
        out[i] = owner_path[i];
        i = i + 1;
    }
    long long j = 0;
    while (j < token_n) {
        out[i + j] = token[j];
        j = j + 1;
    }
    out[i + j] = 0;
    if (fk_path_size_raw(out) >= 0) {
        return 1;
    }
    long long root_n = 0;
    if (owner_path[0] == 'f' && owner_path[1] == 'o' && owner_path[2] == 'r' &&
        owner_path[3] == 'm' && owner_path[4] == FK_CH_SLASH) {
        root_n = 5;
    } else {
        long long s = 0;
        while (owner_path[s] != 0) {
            if (owner_path[s] == FK_CH_SLASH && owner_path[s + 1] == 'f' &&
                owner_path[s + 2] == 'o' && owner_path[s + 3] == 'r' &&
                owner_path[s + 4] == 'm' && owner_path[s + 5] == FK_CH_SLASH) {
                root_n = s + 6;
            }
            s = s + 1;
        }
    }
    if (root_n > 0 && root_n + token_n + 1 <= cap) {
        i = 0;
        while (i < root_n) {
            out[i] = owner_path[i];
            i = i + 1;
        }
        j = 0;
        while (j < token_n) {
            out[i + j] = token[j];
            j = j + 1;
        }
        out[i + j] = 0;
        if (fk_path_size_raw(out) >= 0) {
            return 1;
        }
    }
    if (token_n + 6 <= cap) {
        const char *form_prefix = "form/";
        i = 0;
        while (form_prefix[i] != 0) {
            out[i] = form_prefix[i];
            i = i + 1;
        }
        j = 0;
        while (j < token_n) {
            out[i + j] = token[j];
            j = j + 1;
        }
        out[i + j] = 0;
        if (fk_path_size_raw(out) >= 0) {
            return 1;
        }
    }
    if (token_n + 1 <= cap) {
        i = 0;
        while (i < token_n) {
            out[i] = token[i];
            i = i + 1;
        }
        out[i] = 0;
        if (fk_path_size_raw(out) >= 0) {
            return 1;
        }
    }
    long long pre_n = fk_path_repo_prefix_len(owner_path);
    if (pre_n > 0 && pre_n + token_n + 1 <= cap) {
        i = 0;
        while (i < pre_n) {
            out[i] = owner_path[i];
            i = i + 1;
        }
        j = 0;
        while (j < token_n) {
            out[i + j] = token[j];
            j = j + 1;
        }
        out[i + j] = 0;
        if (fk_path_size_raw(out) >= 0) {
            return 1;
        }
    }
    if (dir_n + token_n + 1 > cap) {
        return 0;
    }
    i = 0;
    while (i < dir_n) {
        out[i] = owner_path[i];
        i = i + 1;
    }
    j = 0;
    while (j < token_n) {
        out[i + j] = token[j];
        j = j + 1;
    }
    out[i + j] = 0;
    return 1;
}
static int fk_source_hash_append(char *out, long long cap, long long *pos, const char *s) {
    long long i = 0;
    while (s[i] != 0) {
        if (*pos + 1 >= cap) {
            return 0;
        }
        out[*pos] = s[i];
        *pos = *pos + 1;
        i = i + 1;
    }
    out[*pos] = 0;
    return 1;
}
static int fk_source_hash_append_ll(char *out, long long cap, long long *pos, long long v) {
    char buf[64];
    sprintf(buf, "%lld", v);
    return fk_source_hash_append(out, cap, pos, buf);
}
static long long fk_src_unit_mtime_range(long long start, long long end) {
    long long m = 1;
    long long i = start;
    while (i < end && i < fk_src_dep_count) {
        if (fk_src_dep_mtime[i] > m) {
            m = fk_src_dep_mtime[i];
        }
        i = i + 1;
    }
    return m;
}
static long long fk_src_unit_mtime(void) {
    return fk_src_unit_mtime_range(0, fk_src_dep_count);
}
static int fk_src_unit_hash_range(long long start, long long end, char *out, long long cap) {
    long long pos = 0;
    long long i = start;
    char canon[FK_PATH_CAP];
    unsigned long long fold = 14695981039346656037ULL;
    long long count = 0;
    /* v2 -> v3: the identity was the full dependency text, and a unit past
     * ~120 deps could not state its own identity -- the buffer walled and the
     * load refused, a program-size wall in the dep dimension. The identity is
     * compare-only (written to .sym/.fkb, read back for equality), so a
     * streamed FNV-1a fold over the SAME fields carries the same testimony at
     * constant size: any dep's canonical path, mtime, size, or content digest
     * change changes the fold. The format bump invalidates v2 artifacts once,
     * the same door v1 -> v2 walked (a v2 artifact cannot testify in v3's
     * voice); the dep COUNT stays readable beside the fold. */
    while (i < end && i < fk_src_dep_count) {
        const char *cid = fk_path_canon_id(fk_src_dep_path[i], canon);
        fold = fold ^ fk_bytes_fnv1a(cid, fk_path_len(cid));
        fold = fold * 1099511628211ULL;
        fold = fold ^ (unsigned long long)fk_src_dep_mtime[i];
        fold = fold * 1099511628211ULL;
        fold = fold ^ (unsigned long long)fk_src_dep_size[i];
        fold = fold * 1099511628211ULL;
        fold = fold ^ fk_src_dep_digest[i];
        fold = fold * 1099511628211ULL;
        count = count + 1;
        i = i + 1;
    }
    if (!fk_source_hash_append(out, cap, &pos, "fk-unit-v3|n=") ||
        !fk_source_hash_append_ll(out, cap, &pos, count) ||
        !fk_source_hash_append(out, cap, &pos, "|") ||
        !fk_source_hash_append_ll(out, cap, &pos, (long long)(fold >> 1))) {
        return 0;
    }
    return 1;
}
static int fk_src_unit_hash(char *out, long long cap) {
    return fk_src_unit_hash_range(0, fk_src_dep_count, out, cap);
}
static int fk_src_line_is_bare_import_fk(const char *text, long long line_start, long long line_end);
static int fk_src_append_text(const char *path, const char *text, long long n) {
    long long line_start = 0;
    fk_srctext_reserve(fk_slen + n + 2);
    while (line_start < n) {
        long long line_end = line_start;
        while (line_end < n && text[line_end] != FK_CH_LF && text[line_end] != FK_CH_CR) {
            line_end = line_end + 1;
        }
        if (!fk_src_line_is_bare_import_fk(text, line_start, line_end)) {
            long long i = line_start;
            while (i < line_end) {
                fk_srctext[fk_slen] = text[i];
                fk_slen = fk_slen + 1;
                i = i + 1;
            }
            while (i < n && (text[i] == FK_CH_LF || text[i] == FK_CH_CR)) {
                fk_srctext[fk_slen] = text[i];
                fk_slen = fk_slen + 1;
                i = i + 1;
            }
        }
        line_start = line_end;
        while (line_start < n && (text[line_start] == FK_CH_LF || text[line_start] == FK_CH_CR)) {
            line_start = line_start + 1;
        }
    }
    fk_srctext[fk_slen] = FK_CH_LF;
    fk_slen = fk_slen + 1;
    fk_srctext[fk_slen] = 0;
    return 1;
}
static int fk_src_collect_file(const char *path, long long parent_idx);
static int fk_src_collect_bytes(const char *path, char *owned, long long got,
                                long long mtime, long long size, long long parent_idx);
static char fk_ascii_lower_char(char c) {
    if (c >= FK_CH_UPPER_A && c <= FK_CH_UPPER_Z) {
        return (char)(c + (FK_CH_LOWER_A - FK_CH_UPPER_A));
    }
    return c;
}
static int fk_slice_eq_word_ci(const char *text, long long start, long long n, const char *word) {
    long long i = 0;
    while (i < n && word[i] != 0) {
        if (fk_ascii_lower_char(text[start + i]) != fk_ascii_lower_char(word[i])) {
            return 0;
        }
        i = i + 1;
    }
    return i == n && word[i] == 0;
}
static int fk_src_prelude_none_token(const char *text, long long start, long long n) {
    if (fk_slice_eq_word_ci(text, start, n, "none")) {
        return 1;
    }
    if (n == 6 && text[start] == FK_CH_LPAREN && text[start + 5] == FK_CH_RPAREN &&
        fk_slice_eq_word_ci(text, start + 1, 4, "none")) {
        return 1;
    }
    return 0;
}
static int fk_src_prelude_backslash_token(const char *text, long long start, long long n) {
    return n == 1 && text[start] == FK_CH_BACKSLASH;
}
static int fk_src_prelude_fk_token(const char *text, long long start, long long n) {
    return n >= 3 && text[start + n - 3] == FK_CH_DOT && text[start + n - 2] == FK_CH_LOWER_F &&
           text[start + n - 1] == FK_CH_LOWER_K;
}
/* a "; preludes:" token may name a high-grammar .bml source directly; the
 * collector lowers it through the BML floor and loads the derived .fk */
static int fk_src_prelude_bml_token(const char *text, long long start, long long n) {
    return n >= 4 && text[start + n - 4] == FK_CH_DOT && text[start + n - 3] == FK_CH_LOWER_B &&
           text[start + n - 2] == 'm' && text[start + n - 1] == 'l';
}
static char *fk_bml_lower_to_mem(const char *bml_path, long long *out_len);
static int fk_unit_lowers(const char *path);
static long long fk_src_trim_import_token(const char *text, long long start, long long *n) {
    long long s = start;
    long long e = start + *n;
    while (s < e && (text[s] == FK_CH_SPACE || text[s] == FK_CH_TAB || text[s] == FK_CH_COMMA ||
                     text[s] == FK_CH_SEMI)) {
        s = s + 1;
    }
    while (e > s && (text[e - 1] == FK_CH_SPACE || text[e - 1] == FK_CH_TAB ||
                     text[e - 1] == FK_CH_COMMA || text[e - 1] == FK_CH_SEMI)) {
        e = e - 1;
    }
    *n = e - s;
    return s;
}
static int fk_src_collect_import_token(const char *owner_path, long long owner_idx, const char *text,
                                       long long start, long long n) {
    start = fk_src_trim_import_token(text, start, &n);
    if (n <= 0 || fk_src_prelude_none_token(text, start, n)) {
        return 1;
    }
    if (!fk_src_prelude_fk_token(text, start, n)) {
        return 1;
    }
    char dep_path[FK_PATH_CAP];
    if (!fk_path_resolve_fk_dep(owner_path, text + start, n, dep_path, FK_PATH_CAP)) {
        fk_diag_path("error", owner_path, "import path exceeds buffer");
        return 0;
    }
    return fk_src_collect_file(dep_path, owner_idx);
}
static int fk_src_word_at_ci(const char *text, long long p, long long end, const char *word) {
    long long i = 0;
    while (word[i] != 0) {
        if (p + i >= end || fk_ascii_lower_char(text[p + i]) != fk_ascii_lower_char(word[i])) {
            return 0;
        }
        i = i + 1;
    }
    return p + i == end || text[p + i] == FK_CH_SPACE || text[p + i] == FK_CH_TAB ||
           text[p + i] == FK_CH_COLON || text[p + i] == FK_CH_DQUOTE;
}
static int fk_src_collect_import_statement(const char *owner_path, long long owner_idx, const char *text,
                                           long long p, long long line_end) {
    if (!fk_src_word_at_ci(text, p, line_end, "import")) {
        return 1;
    }
    p = p + 6;
    while (p < line_end && (text[p] == FK_CH_SPACE || text[p] == FK_CH_TAB ||
                            text[p] == FK_CH_COLON)) {
        p = p + 1;
    }
    if (p >= line_end) {
        return 1;
    }
    long long start = p;
    long long n = 0;
    if (text[p] == FK_CH_DQUOTE) {
        p = p + 1;
        start = p;
        while (p < line_end && text[p] != FK_CH_DQUOTE) {
            p = p + 1;
        }
        n = p - start;
    } else {
        while (p < line_end && text[p] != FK_CH_SPACE && text[p] != FK_CH_TAB &&
               text[p] != FK_CH_COMMA && text[p] != FK_CH_SEMI) {
            p = p + 1;
        }
        n = p - start;
    }
    return fk_src_collect_import_token(owner_path, owner_idx, text, start, n);
}
static int fk_src_line_is_bare_import_fk(const char *text, long long line_start, long long line_end) {
    long long p = line_start;
    while (p < line_end && (text[p] == FK_CH_SPACE || text[p] == FK_CH_TAB)) {
        p = p + 1;
    }
    if (!fk_src_word_at_ci(text, p, line_end, "import")) {
        return 0;
    }
    p = p + 6;
    while (p < line_end && (text[p] == FK_CH_SPACE || text[p] == FK_CH_TAB ||
                            text[p] == FK_CH_COLON)) {
        p = p + 1;
    }
    if (p >= line_end) {
        return 0;
    }
    long long start = p;
    long long n = 0;
    if (text[p] == FK_CH_DQUOTE) {
        p = p + 1;
        start = p;
        while (p < line_end && text[p] != FK_CH_DQUOTE) {
            p = p + 1;
        }
        n = p - start;
    } else {
        while (p < line_end && text[p] != FK_CH_SPACE && text[p] != FK_CH_TAB &&
               text[p] != FK_CH_COMMA && text[p] != FK_CH_SEMI) {
            p = p + 1;
        }
        n = p - start;
    }
    start = fk_src_trim_import_token(text, start, &n);
    return fk_src_prelude_fk_token(text, start, n);
}
static int fk_src_collect_preludes(const char *owner_path, const char *text, long long n,
                                   long long owner_idx) {
    const char *needle = "preludes:";
    long long needle_n = 9;
    long long i = 0;
    while (i < n) {
        long long line_start = i;
        long long line_end = i;
        while (line_end < n && text[line_end] != FK_CH_LF && text[line_end] != FK_CH_CR) {
            line_end = line_end + 1;
        }
        long long comment = -1;
        long long scan = line_start;
        while (scan < line_end) {
            if (text[scan] == FK_CH_SEMI) {
                comment = scan + 1;
                break;
            }
            scan = scan + 1;
        }
        if (comment >= 0) {
            scan = comment;
            while (scan < line_end && (text[scan] == FK_CH_SPACE || text[scan] == FK_CH_TAB)) {
                scan = scan + 1;
            }
            if (!fk_src_collect_import_statement(owner_path, owner_idx, text, scan, line_end)) {
                return 0;
            }
            scan = comment;
            while (scan + needle_n <= line_end) {
                long long j = 0;
                while (j < needle_n && text[scan + j] == needle[j]) {
                    j = j + 1;
                }
                if (j != needle_n) {
                    scan = scan + 1;
                    continue;
                }
                long long p = scan + needle_n;
                while (p < n) {
                    while (p < n && (text[p] == FK_CH_SPACE || text[p] == FK_CH_TAB ||
                                     text[p] == FK_CH_COMMA)) {
                        p = p + 1;
                    }
                    if (p >= n || text[p] == FK_CH_LF || text[p] == FK_CH_CR) {
                        break;
                    }
                    long long start = p;
                    while (p < n && text[p] != FK_CH_SPACE && text[p] != FK_CH_TAB &&
                           text[p] != FK_CH_COMMA && text[p] != FK_CH_LF &&
                           text[p] != FK_CH_CR) {
                        p = p + 1;
                    }
                    if (p > start) {
                        long long tn = p - start;
                        start = fk_src_trim_import_token(text, start, &tn);
                        if (fk_src_prelude_none_token(text, start, tn)) {
                            while (p < n && text[p] != FK_CH_LF && text[p] != FK_CH_CR) {
                                p = p + 1;
                            }
                            break;
                        }
                        if (fk_src_prelude_backslash_token(text, start, tn)) {
                            while (p < n && text[p] != FK_CH_LF && text[p] != FK_CH_CR) {
                                p = p + 1;
                            }
                            while (p < n && (text[p] == FK_CH_LF || text[p] == FK_CH_CR)) {
                                p = p + 1;
                            }
                            while (p < n && (text[p] == FK_CH_SPACE || text[p] == FK_CH_TAB)) {
                                p = p + 1;
                            }
                            if (p < n && text[p] == FK_CH_SEMI) {
                                p = p + 1;
                            }
                            continue;
                        }
                        if (!fk_src_prelude_fk_token(text, start, tn) &&
                            !fk_src_prelude_bml_token(text, start, tn)) {
                            break;
                        }
                        char dep_path[FK_PATH_CAP];
                        if (!fk_path_resolve_fk_dep(owner_path, text + start, tn, dep_path, FK_PATH_CAP)) {
                            fk_diag_path("error", owner_path, "prelude path exceeds buffer");
                            return 0;
                        }
                        if (fk_unit_lowers(dep_path)) {
                            if (fk_src_dep_index(dep_path) < 0) {
                                long long bml_mtime = fk_path_mtime_raw(dep_path);
                                if (bml_mtime <= 0) {
                                    fk_diag_path("error", dep_path,
                                            "prelude is missing or not stat-readable");
                                    return 0;
                                }
                                long long low_len = 0;
                                char *low = fk_bml_lower_to_mem(dep_path, &low_len);
                                if (low == 0) {
                                    return 0;
                                }
                                /* THE FLAG THAT LANDED ON THE WRONG UNIT.
                                 * This used to mark fk_src_dep_count - 1 AFTER
                                 * the call, on the belief that the unit just
                                 * collected is the last one. It is only the
                                 * last one when the lowered .bml has no
                                 * preludes of its own: fk_src_collect_bytes
                                 * registers the .bml at the count it was
                                 * handed and THEN collects the .bml's own
                                 * prelude chain behind it, so for any .bml
                                 * that preludes anything the flag came to rest
                                 * on that chain's last transitive dependency —
                                 * a plain .fk — and the .bml itself stayed
                                 * unmarked. Both halves of that then went
                                 * wrong at once: the import lane saw an
                                 * unmarked .bml among the root's direct
                                 * dependencies, tried to build a standalone
                                 * image from its RAW high-grammar bytes (no
                                 * lowering happens on that path), counted the
                                 * thousand-odd unresolved calls that must
                                 * follow, refused the image and fell the whole
                                 * program back to the flat compile — while a
                                 * .fk that could have been imaged was carried
                                 * as text instead. The .bml's index is known
                                 * before the call; take it there. */
                                long long bml_idx = fk_src_dep_count;
                                if (!fk_src_collect_bytes(dep_path, low, low_len,
                                        bml_mtime, low_len, owner_idx)) {
                                    return 0;
                                }
                                fk_src_dep_lowered[bml_idx] = 1;
                            }
                        } else if (!fk_src_collect_file(dep_path, owner_idx)) {
                            return 0;
                        }
                    }
                }
                scan = p;
            }
        }
        scan = line_start;
        while (scan < line_end && (text[scan] == FK_CH_SPACE || text[scan] == FK_CH_TAB)) {
            scan = scan + 1;
        }
        if (comment < 0 && !fk_src_collect_import_statement(owner_path, owner_idx, text, scan, line_end)) {
            return 0;
        }
        i = line_end;
        while (i < n && (text[i] == FK_CH_LF || text[i] == FK_CH_CR)) {
            i = i + 1;
        }
    }
    return 1;
}
static int fk_src_collect_bytes(const char *path, char *owned, long long got,
                                long long mtime, long long size, long long parent_idx);
static int fk_src_collect_file(const char *path, long long parent_idx) {
    if (fk_src_dep_index(path) >= 0) {
        return 1;
    }
    long long mtime = fk_path_mtime_raw(path);
    long long size = fk_path_size_raw(path);
    if (mtime <= 0 || size < 0) {
        fk_diag_path("error", path, "dependency source is missing or not stat-readable");
        return 0;
    }
#if defined(_WIN32)
    int fd = open(path, 0x8000);
#else
    int fd = open(path, 0);
#endif
    if (fd < 0) {
        fk_diag_path("error", path, "dependency source could not be opened");
        return 0;
    }
    long long got = fk_read_all_dynamic(fd, size + 1);
    close(fd);
    if (got < 0) {
        if (got == -3) {
            fk_diag_path("error", path,
                    "dependency source could not grow the dynamic read buffer");
        } else {
            fk_diag_path("error", path, "dependency source could not be read");
        }
        return 0;
    }
    char *owned = malloc((unsigned long)got + 1);
    if (owned == 0) {
        fk_die("fk_run_src: out of memory reading dependency source");
    }
    long long i = 0;
    while (i < got) {
        owned[i] = fk_buf[i];
        i = i + 1;
    }
    owned[got] = 0;
    return fk_src_collect_bytes(path, owned, got, mtime, size, parent_idx);
}
/* the after-read half of collection, shared by the file lane and the BML
 * floor's in-memory lane: the bytes arrive owned (freed here), registered
 * under the given path identity — for a lowered .bml that is the .bml
 * itself, so no derived source file ever exists on disk. */
static int fk_src_collect_bytes(const char *path, char *owned, long long got,
                                long long mtime, long long size, long long parent_idx) {
    long long i = 0;
    (void)i;
    if (!fk_src_dep_reserve(fk_src_dep_count + 1)) {
        free(owned);
        fk_diag_path("error", path, "could not grow .fk dependency metadata");
        return 0;
    }
    long long idx = fk_src_dep_count;
    fk_cstr_copy(fk_src_dep_path[fk_src_dep_count], path, FK_PATH_CAP);
    fk_src_dep_mtime[fk_src_dep_count] = mtime;
    fk_src_dep_size[fk_src_dep_count] = size;
    fk_src_dep_digest[fk_src_dep_count] = fk_bytes_fnv1a(owned, got);
    fk_src_dep_parent[fk_src_dep_count] = parent_idx;
    fk_src_dep_end[fk_src_dep_count] = fk_src_dep_count + 1;
    fk_src_dep_text_off[fk_src_dep_count] = 0;
    fk_src_dep_text_len[fk_src_dep_count] = 0;
    fk_src_dep_lowered[fk_src_dep_count] = 0;
    fk_src_dep_count = fk_src_dep_count + 1;
    if (fk_cstr_eq(path, fk_src_root_path)) {
        fk_src_root_reserve(got + 1);
        i = 0;
        while (i < got) {
            fk_src_root_text[i] = owned[i];
            i = i + 1;
        }
        fk_src_root_text[got] = 0;
        fk_src_root_len = got;
    }
    if (!fk_src_collect_preludes(path, owned, got, idx)) {
        free(owned);
        return 0;
    }
    fk_src_dep_end[idx] = fk_src_dep_count;
    fk_src_dep_text_off[idx] = fk_slen;
    if (!fk_src_append_text(path, owned, got)) {
        free(owned);
        return 0;
    }
    fk_src_dep_text_len[idx] = fk_slen - fk_src_dep_text_off[idx];
    free(owned);
    return 1;
}
static int fk_src_load_unit(const char *root_path, char *source_hash, long long hash_cap,
                            long long *unit_mtime) {
    fk_slen = 0;
    fk_srctext[0] = 0;
    fk_src_dep_count = 0;
    fk_src_root_len = 0;
    fk_src_root_text[0] = 0;
    fk_cstr_copy(fk_src_root_path, root_path, FK_PATH_CAP);
    if (!fk_src_collect_file(root_path, -1)) {
        return 0;
    }
    if (!fk_src_unit_hash(source_hash, hash_cap)) {
        fk_diag_path("error", root_path, "dependency identity exceeds hash buffer");
        return 0;
    }
    *unit_mtime = fk_src_unit_mtime();
    fk_spos = 0;
    fk_srctext[fk_slen] = 0;
    return 1;
}
/* the BML floor's root loader: the unit's root arrives as in-memory text
 * (the lowered .bml) registered under the .bml's own path and mtime —
 * no derived source file exists at any point. */
static int fk_src_load_unit_buffer(const char *root_path, char *owned, long long got,
                                   long long mtime, char *source_hash,
                                   long long hash_cap, long long *unit_mtime) {
    fk_slen = 0;
    fk_srctext[0] = 0;
    fk_src_dep_count = 0;
    fk_src_root_len = 0;
    fk_src_root_text[0] = 0;
    fk_cstr_copy(fk_src_root_path, root_path, FK_PATH_CAP);
    if (!fk_src_collect_bytes(root_path, owned, got, mtime, got, -1)) {
        return 0;
    }
    if (!fk_src_unit_hash(source_hash, hash_cap)) {
        fk_diag_path("error", root_path, "dependency identity exceeds hash buffer");
        return 0;
    }
    *unit_mtime = fk_src_unit_mtime();
    fk_spos = 0;
    fk_srctext[fk_slen] = 0;
    return 1;
}
/* ── the image goes out through ONE buffer ───────────────────────────────────
 * Every value used to be its own write(2): a signed value is three of them
 * (sign, hi, lo) and a node is four values, so a 1.4 MB image issued well over
 * a million syscalls. Measured 2026-09-08 on this Mac, cold, one band chain:
 * 551 ms writing the image, 2 ms compiling it, 1.19 s total. The bytes were
 * never the cost -- the crossings were, and a syscall per byte cannot reach
 * anywhere near the disk's own bandwidth however fast the disk is.
 * The buffer flushes when full and once before close; ordering is preserved
 * because EVERY writer below goes through it, header included. */
#define FK_FKB_OUT_CAP (1 << 20)
static unsigned char fk_fkb_out_buf[FK_FKB_OUT_CAP];
static long long fk_fkb_out_n;
static int fk_fkb_flush(int fd) {
    if (fk_fkb_out_n <= 0) {
        return 1;
    }
    int ok = fk_write_all_raw(fd, (const char *)fk_fkb_out_buf, (unsigned long)fk_fkb_out_n);
    fk_fkb_out_n = 0;
    return ok;
}
static int fk_fkb_out(int fd, const char *p, long long n) {
    if (n < 0) {
        return 0;
    }
    if (n >= FK_FKB_OUT_CAP) {
        return fk_fkb_flush(fd) && fk_write_all_raw(fd, p, (unsigned long)n);
    }
    if (fk_fkb_out_n + n > FK_FKB_OUT_CAP && !fk_fkb_flush(fd)) {
        return 0;
    }
    long long k = 0;
    while (k < n) {
        fk_fkb_out_buf[fk_fkb_out_n + k] = (unsigned char)p[k];
        k = k + 1;
    }
    fk_fkb_out_n = fk_fkb_out_n + n;
    return 1;
}
static int fk_fkb_write_u8(int fd, long long v) {
    char b = (char)(v & 255);
    return fk_fkb_out(fd, &b, 1);
}
/* An out-of-range value is a WRITER refusal, not an I/O failure -- flagged so
 * the artifact-write diagnostic can name the range instead of a generic
 * "failed to write". On the v4 lane only LLONG_MIN's magnitude (2^63) trips it. */
static int fk_fkb_write_overflow;
static int fk_fkb_write_u32(int fd, long long v) {
    unsigned char b[4];
    if (v < 0 || v > 4294967295LL) {
        fk_fkb_write_overflow = 1;
        return 0;
    }
    b[0] = (unsigned char)((v >> 24) & 255);
    b[1] = (unsigned char)((v >> 16) & 255);
    b[2] = (unsigned char)((v >> 8) & 255);
    b[3] = (unsigned char)(v & 255);
    return fk_fkb_out(fd, (const char *)b, 4);
}
static int fk_fkb_write_signed(int fd, long long v) {
    /* v4 lane: sign u8 + hi u32 + lo u32 -- the full long long range, so
     * full-range u32 literals (e.g. cksum values) stay artifact-encodable.
     * LLONG_MIN's magnitude (2^63) has no positive twin the reader could
     * round-trip, so refuse it here rather than emit an unreadable image. */
    unsigned long long mag = v < 0 ? 0ULL - (unsigned long long)v : (unsigned long long)v;
    if (mag > 9223372036854775807ULL) {
        fk_fkb_write_overflow = 1;
        return 0;
    }
    return fk_fkb_write_u8(fd, v < 0 ? 1 : 0) &&
           fk_fkb_write_u32(fd, (long long)(mag >> 32)) &&
           fk_fkb_write_u32(fd, (long long)(mag & 4294967295ULL));
}
static int fk_fkb_write_cstr(int fd, const char *s) {
    long long n = fk_path_len(s);
    return fk_fkb_write_u32(fd, n) && fk_fkb_out(fd, s, n);
}
static int fk_fkb_write_bytes(int fd, const char *s, long long n) {
    return fk_fkb_write_u32(fd, n) && fk_fkb_out(fd, s, n);
}
static int fk_fkb_write_srctext_slice(int fd, long long start, long long n) {
    if (start < 0 || n < 0 || start + n > fk_slen) {
        return 0;
    }
    return fk_fkb_write_bytes(fd, fk_srctext + start, n);
}
/* ── which symbol owns this fn / this node, in one step ──────────────────────
 * Both answers used to be a linear scan over every symbol, and both writers ask
 * them ONCE PER NODE -- so a program with n nodes and s symbols paid n*s twice.
 * Measured 2026-09-08 on a large band: 254 ms in the sym lens and 228 ms in the
 * image, after the syscall-per-byte was already gone. The tables are built once
 * per write and freed after; when they are absent (any other caller, or a
 * malloc that refused) the scan still answers, so this is a shortcut and never
 * a second source of truth. */
static long long *fk_src_sym_of_fn;
static long long *fk_src_sym_of_node;
static long long fk_src_sym_of_fn_n;
static long long fk_src_sym_of_node_n;
static void fk_src_sym_index_free(void) {
    free(fk_src_sym_of_fn);
    free(fk_src_sym_of_node);
    fk_src_sym_of_fn = 0;
    fk_src_sym_of_node = 0;
    fk_src_sym_of_fn_n = 0;
    fk_src_sym_of_node_n = 0;
}
static void fk_src_sym_index_build(void) {
    fk_src_sym_index_free();
    if (fk_fn_count <= 0 || fk_node_count <= 0) {
        return;
    }
    fk_src_sym_of_fn = malloc(sizeof(long long) * (unsigned long)fk_fn_count);
    fk_src_sym_of_node = malloc(sizeof(long long) * (unsigned long)fk_node_count);
    if (fk_src_sym_of_fn == 0 || fk_src_sym_of_node == 0) {
        fk_src_sym_index_free();
        return;
    }
    long long i = 0;
    while (i < fk_fn_count) { fk_src_sym_of_fn[i] = -1; i = i + 1; }
    i = 0;
    while (i < fk_node_count) { fk_src_sym_of_node[i] = -1; i = i + 1; }
    /* the scans answered with the FIRST symbol that matched, so fill forwards
     * and keep the first: same answer, one pass */
    i = 0;
    while (i < fk_fntop) {
        long long fi = fk_fnidx[i];
        if (fi >= 0 && fi < fk_fn_count) {
            if (fk_src_sym_of_fn[fi] < 0) { fk_src_sym_of_fn[fi] = i; }
            long long nd = fk_fn[fi];
            if (nd >= 0 && nd < fk_node_count && fk_src_sym_of_node[nd] < 0) {
                fk_src_sym_of_node[nd] = i;
            }
        }
        i = i + 1;
    }
    fk_src_sym_of_fn_n = fk_fn_count;
    fk_src_sym_of_node_n = fk_node_count;
}
static long long fk_src_symbol_id_for_fn(long long fnidx) {
    if (fk_src_sym_of_fn != 0 && fnidx >= 0 && fnidx < fk_src_sym_of_fn_n) {
        return fk_src_sym_of_fn[fnidx];
    }
    long long i = 0;
    while (i < fk_fntop) {
        if (fk_fnidx[i] == fnidx) {
            return i;
        }
        i = i + 1;
    }
    return -1;
}
static long long fk_src_symbol_id_for_node(long long node) {
    if (fk_src_sym_of_node != 0 && node >= 0 && node < fk_src_sym_of_node_n) {
        return fk_src_sym_of_node[node];
    }
    long long i = 0;
    while (i < fk_fntop) {
        long long fi = fk_fnidx[i];
        if (fi >= 0 && fi < fk_fn_count && fk_fn[fi] == node) {
            return i;
        }
        i = i + 1;
    }
    return -1;
}
static long long fk_src_direct_call_fn(long long node) {
    if (node < 0 || node >= fk_node_count) {
        return -1;
    }
    long long t = fk_node[node][0];
    if (t == 12 || t == 240 || t == 241) {
        return fk_node[node][1];
    }
    return -1;
}
static int fk_src_write_sym_text(const char *sym_path, const char *src_path, const char *fkb_path,
                                 const char *source_hash) {
#if defined(_WIN32)
    int fd = open(sym_path, O_WRONLY | O_CREAT | O_TRUNC | 0x8000, 0666);
#else
    int fd = open(sym_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
#endif
    if (fd < 0) {
        return 0;
    }
    char line[512];
    fk_fkb_out_n = 0;
    /* compile-errors records fk_nerr at image-write time, placed right after
     * the version line so readers find it in the first bytes; a cached run
     * replays this count as its exit truth (absent line reads as 0).
     *
     * unrunnable records the REFUSAL, which is a different fact from the count.
     * An [unbound-name] in value position latches fk_src_unrunnable, and the
     * fresh-compile door then returns WITHOUT printing the root value -- the
     * kernel declining to compute an answer over a program whose names silently
     * read as 0. That refusal was not travelling into the image, so the next run
     * loaded the .fkb and printed the value the compile had just refused to
     * print: same source, same errors, and a number visible only on the second
     * run. The count could not stand in for it, because an unresolved CALL
     * recovers to nothing and DOES run -- errors > 0 and runnable is an ordinary
     * state. So the latch travels on its own line. */
    int hn = sprintf(line,
                     "program-image-sym-lens-v1\ncompile-errors %lld\nunrunnable %d\nsource ",
                     fk_nerr, fk_src_unrunnable ? 1 : 0);
    if (!fk_fkb_out(fd, line, hn) ||
        !fk_fkb_out(fd, src_path, fk_path_len(src_path)) ||
        !fk_fkb_out(fd, "\nfkb ", 5) ||
        !fk_fkb_out(fd, fkb_path, fk_path_len(fkb_path)) ||
        !fk_fkb_out(fd, "\nsource-hash ", 13) ||
        !fk_fkb_out(fd, source_hash, fk_path_len(source_hash)) ||
        !fk_fkb_out(fd, "\n", 1)) {
        close(fd);
        return 0;
    }
    long long dep_i = 0;
    while (dep_i < fk_src_dep_count) {
        int n = sprintf(line, "dependency %lld mtime %lld size %lld path ", dep_i,
                        fk_src_dep_mtime[dep_i], fk_src_dep_size[dep_i]);
        if (!fk_fkb_out(fd, line, n) ||
            !fk_fkb_out(fd, fk_src_dep_path[dep_i],
                          fk_path_len(fk_src_dep_path[dep_i])) ||
            !fk_fkb_out(fd, "\n", 1)) {
            close(fd);
            return 0;
        }
        dep_i = dep_i + 1;
    }
    long long i = 0;
    while (i < fk_fntop) {
        long long name_s = fk_fnsym_s[i];
        long long name_n = fk_fnsym_n[i];
        int n = sprintf(line, "symbol %lld ", i);
        if (!fk_fkb_out(fd, line, n) ||
            !fk_fkb_out(fd, fk_srctext + name_s, name_n) ||
            !fk_fkb_out(fd, "\n", 1)) {
            close(fd);
            return 0;
        }
        i = i + 1;
    }
    long long node = 0;
    while (node < fk_node_count) {
        long long defined = fk_src_symbol_id_for_node(node);
        long long dep_fn = fk_src_direct_call_fn(node);
        long long dep_sym = fk_src_symbol_id_for_fn(dep_fn);
        if (defined >= 0 || dep_sym >= 0) {
            long long target = (dep_fn >= 0 && dep_fn < fk_fn_count) ? fk_fn[dep_fn] : -1;
            int n = sprintf(line, "node %lld defines %lld depends %lld target %lld\n", node,
                            defined, dep_sym, target);
            if (!fk_fkb_out(fd, line, n)) {
                close(fd);
                return 0;
            }
        }
        node = node + 1;
    }
    if (!fk_fkb_flush(fd)) { close(fd); return 0; }
    close(fd);
    return 1;
}
#if !defined(_WIN32)
extern int kill(int, int);
#ifndef ESRCH
#define ESRCH 3
#endif
/* Orphan sweep for the pid-temp writer below: a writer killed between open()
 * and rename() (SIGKILL from tools/ftimeout, a crash, a ^C) leaves its
 * .w<pid> temp behind with no process left responsible for it -- a band
 * sweep under ftimeout orphaned 793 of them in one afternoon (witnessed
 * 2026-07-17), and `git add -A` swept 783 into a commit. Before staging its
 * own temps, a writer clears its artifact's directory of every
 * *.fkb.w<pid> / *.sym.w<pid> whose writer is DEAD (kill(pid,0) -> ESRCH).
 * A live pid -- or one we may not signal (EPERM) -- is left alone, so the
 * concurrent-runner guarantee of the pid-temp scheme is untouched; a
 * recycled pid at worst delays one orphan's collection until the next
 * compile in that directory. Windows keeps the leak: no kill() there, and
 * that lane is the port shim, not the sweep path. */
static void fk_src_sweep_dead_temps(const char *fkb_path) {
    char dir[FK_PATH_CAP + 64];
    long long n = fk_path_len(fkb_path);
    if (n > FK_PATH_CAP) {
        return;
    }
    long long cut = n;
    while (cut > 0 && fkb_path[cut - 1] != FK_CH_SLASH) {
        cut = cut - 1;
    }
    if (cut == 0) {
        dir[0] = FK_CH_DOT;
        dir[1] = 0;
    } else if (cut == 1) {
        dir[0] = FK_CH_SLASH;
        dir[1] = 0;
    } else {
        long long k = 0;
        while (k < cut - 1) {
            dir[k] = fkb_path[k];
            k = k + 1;
        }
        dir[k] = 0;
    }
    DIR *d = opendir(dir);
    if (!d) {
        return;
    }
    struct dirent *e;
    long long self = getpid();
    while ((e = readdir(d)) != 0) {
        const char *name = e->d_name;
        long long len = 0;
        while (name[len] != 0) {
            len = len + 1;
        }
        long long ds = len;
        while (ds > 0 && name[ds - 1] >= FK_CH_DIGIT0 && name[ds - 1] <= FK_CH_DIGIT9) {
            ds = ds - 1;
        }
        /* shape: <stem>.fkb.w<1..10 digits> or <stem>.sym.w<1..10 digits> */
        if (ds == len || len - ds > 10 || ds < 6) {
            continue;
        }
        const char *fkbw = ".fkb.w";
        const char *symw = ".sym.w";
        int m_fkb = 1;
        int m_sym = 1;
        long long k = 0;
        while (k < 6) {
            if (name[ds - 6 + k] != fkbw[k]) {
                m_fkb = 0;
            }
            if (name[ds - 6 + k] != symw[k]) {
                m_sym = 0;
            }
            k = k + 1;
        }
        if (!m_fkb && !m_sym) {
            continue;
        }
        long long pid = 0;
        k = ds;
        while (k < len) {
            pid = pid * 10 + (name[k] - FK_CH_DIGIT0);
            k = k + 1;
        }
        if (pid <= 0 || pid == self) {
            continue;
        }
        if (kill((int)pid, 0) == 0 || errno != ESRCH) {
            continue;
        }
        char victim[4600];
        fk_path_join(victim, 4600, dir, name);
        unlink(victim);
    }
    closedir(d);
}
#endif
/* The .fkb's own pipeline identity. Before v5 the artifact recorded only its
 * INPUT -- source path, content hash, mtime -- so two different fkwu builds
 * writing from the same bytes produced artifacts indistinguishable to each
 * other, and either would load the other's as fresh. That is not theoretical:
 * a binary built before the unbalanced-form refusal compiles `(do (defn p ()
 * (add 40 2)) (p)` to 42 and seals a stamp-valid .fkb; the healed binary next
 * to it then PRINTS 42 and exits 0, because it never compiles the text at all.
 * The heal is defeated by the cache, silently, with a right-looking number --
 * the numb-green shape axiom-5 already names.
 * So the stamp now carries who wrote it as well as what it was written from.
 * __DATE__/__TIME__ keys the identity to the translation-unit build, which is
 * conservative in the safe direction: two byte-identical rebuilds refuse each
 * other's caches (a false REJECT, paid once per rebuild in recompile time),
 * and no build ever accepts a foreign one (the false ACCEPT, which was paid in
 * wrong answers). SHRINK NOTE: this is a checkout-witness repair in the C
 * seed. Its home is the native body's artifact layer, where the identity of a
 * compiled image belongs next to the image; it lives here only while the seed
 * still owns .fkb. */
#define FK_FKB_BUILDER_ID ("fkwu-uni " __DATE__ " " __TIME__)

static int fk_src_write_fkb(const char *src_path, const char *fkb_path, const char *sym_path,
                            long long source_mtime, const char *source_hash) {
    /* both artifacts go to pid-suffixed temp names and rename() into place:
     * a reader under a concurrent runner must only ever see a whole image or
     * the previous one, never a TRUNC-in-progress partial (the "truncated
     * artifact" die class). The sym lens lands before the image so a fresh
     * .fkb is never visible without its compile-error record. */
    char fkb_tmp[FK_PATH_CAP + 64];
    char sym_tmp[FK_PATH_CAP + 64];
    if (fk_path_len(fkb_path) > FK_PATH_CAP || fk_path_len(sym_path) > FK_PATH_CAP) {
        return 0;
    }
#if !defined(_WIN32)
    fk_src_sweep_dead_temps(fkb_path);
    fk_src_sym_index_build();
#endif
    sprintf(fkb_tmp, "%s.w%d", fkb_path, getpid());
    sprintf(sym_tmp, "%s.w%d", sym_path, getpid());
#if defined(_WIN32)
    int fd = open(fkb_tmp, O_WRONLY | O_CREAT | O_TRUNC | 0x8000, 0666);
#else
    int fd = open(fkb_tmp, O_WRONLY | O_CREAT | O_TRUNC, 0666);
#endif
    if (fd < 0) {
        return 0;
    }
    fk_fkb_write_overflow = 0;
    int ok = 1;
    char canon[FK_PATH_CAP];
    fk_fkb_out_n = 0;
    ok = ok && fk_fkb_out(fd, "FKPIFB1", 7);
    ok = ok && fk_fkb_write_u8(fd, 0);
    ok = ok && fk_fkb_write_u32(fd, 5);
    ok = ok && fk_fkb_write_cstr(fd, FK_FKB_BUILDER_ID);
    ok = ok && fk_fkb_write_cstr(fd, fk_path_canon_id(src_path, canon));
    ok = ok && fk_fkb_write_cstr(fd, source_hash);
    ok = ok && fk_fkb_write_signed(fd, source_mtime > 0 ? source_mtime : 1);
    ok = ok && fk_fkb_write_cstr(fd, fkb_path);
    ok = ok && fk_fkb_write_signed(fd, 1);
    ok = ok && fk_fkb_write_signed(fd, fk_fn_count);
    long long i = 0;
    while (ok && i < fk_fn_count) {
        ok = fk_fkb_write_signed(fd, fk_fn[i]);
        i = i + 1;
    }
    ok = ok && fk_fkb_write_signed(fd, fk_node_count);
    i = 0;
    while (ok && i < fk_node_count) {
        ok = fk_fkb_write_signed(fd, fk_node[i][0]) && fk_fkb_write_signed(fd, fk_node[i][1]) &&
             fk_fkb_write_signed(fd, fk_node[i][2]) && fk_fkb_write_signed(fd, fk_node[i][3]);
        i = i + 1;
    }
    ok = ok && fk_fkb_write_signed(fd, fk_sp);
    i = 0;
    while (ok && FK_SOK(i)) {
        ok = fk_fkb_write_bytes(fd, FK_SBYTES(i), FK_SLEN(i));
        i = i + 1;
    }
    ok = ok && fk_fkb_write_signed(fd, fk_fntop);
    i = 0;
    while (ok && i < fk_fntop) {
        long long fnidx = fk_fnidx[i];
        long long arity = (fnidx >= 0 && fnidx < fk_fn_count) ? fk_fnar[fnidx] : 0;
        ok = fk_fkb_write_signed(fd, i) && fk_fkb_write_signed(fd, fnidx) &&
             fk_fkb_write_signed(fd, arity) &&
             fk_fkb_write_srctext_slice(fd, fk_fnsym_s[i], fk_fnsym_n[i]);
        i = i + 1;
    }
    ok = ok && fk_fkb_write_signed(fd, fk_node_count);
    i = 0;
    while (ok && i < fk_node_count) {
        long long defined = fk_src_symbol_id_for_node(i);
        long long dep_fn = fk_src_direct_call_fn(i);
        long long dep_sym = fk_src_symbol_id_for_fn(dep_fn);
        long long dep_count = dep_sym >= 0 ? 1 : 0;
        ok = fk_fkb_write_signed(fd, i) && fk_fkb_write_signed(fd, defined) &&
             fk_fkb_write_signed(fd, dep_count);
        if (ok && dep_count == 1) {
            long long target = (dep_fn >= 0 && dep_fn < fk_fn_count) ? fk_fn[dep_fn] : -1;
            ok = fk_fkb_write_signed(fd, dep_sym) && fk_fkb_write_signed(fd, target);
        }
        i = i + 1;
    }
    ok = ok && fk_fkb_flush(fd);
    close(fd);
    if (!ok) {
        fk_src_sym_index_free();
        unlink(fkb_tmp);
        return 0;
    }
    if (!fk_src_write_sym_text(sym_tmp, src_path, fkb_path, source_hash)) {
        fk_src_sym_index_free();
        unlink(fkb_tmp);
        unlink(sym_tmp);
        return 0;
    }
    if (rename(sym_tmp, sym_path) != 0) {
        fk_src_sym_index_free();
        unlink(fkb_tmp);
        unlink(sym_tmp);
        return 0;
    }
    fk_src_sym_index_free();
    if (rename(fkb_tmp, fkb_path) != 0) {
        unlink(fkb_tmp);
        return 0;
    }
    return 1;
}
static long long fk_fkb_pos;
static long long fk_fkb_len;
/* Sticky decode-failure flag: the .fkb readers RECORD corruption instead of
 * dying, so both loaders can soft-return and the caller can rebuild from
 * source with a diagnostic that names the artifact and the honest reason. A
 * die here used to surface as a bare "truncated string" even when the real
 * story was a stale/wrong-CWD artifact identity (witnessed 2026-07-16). */
static int fk_fkb_bad;
static const char *fk_fkb_bad_why;
static void fk_fkb_begin(long long len) {
    fk_fkb_pos = 0;
    fk_fkb_len = len;
    fk_fkb_bad = 0;
    fk_fkb_bad_why = "";
}
static void fk_fkb_mark_bad(const char *why) {
    if (!fk_fkb_bad) {
        fk_fkb_bad = 1;
        fk_fkb_bad_why = why;
    }
    fk_fkb_pos = fk_fkb_len; /* clamp: every further read yields 0 */
}
static long long fk_fkb_read_u8(void) {
    if (fk_fkb_pos >= fk_fkb_len) {
        fk_fkb_mark_bad("truncated artifact");
        return 0;
    }
    return (long long)(unsigned char)fk_buf[fk_fkb_pos++];
}
static long long fk_fkb_read_u32(void) {
    long long a = fk_fkb_read_u8();
    long long b = fk_fkb_read_u8();
    long long c = fk_fkb_read_u8();
    long long d = fk_fkb_read_u8();
    return (a << 24) | (b << 16) | (c << 8) | d;
}
static long long fk_fkb_read_signed(void) {
    long long sign = fk_fkb_read_u8();
    long long hi = fk_fkb_read_u32();
    long long lo = fk_fkb_read_u32();
    if (hi > 2147483647LL) {
        /* magnitude must stay below 2^63 so it round-trips through long long */
        fk_fkb_mark_bad("signed magnitude exceeds 63 bits");
        return 0;
    }
    long long mag = (hi << 32) | lo;
    if (sign == 0) {
        return mag;
    }
    if (sign == 1) {
        return -mag;
    }
    fk_fkb_mark_bad("malformed signed integer");
    return 0;
}
static void fk_fkb_skip_string(void) {
    long long n = fk_fkb_read_u32();
    if (n < 0 || fk_fkb_pos + n > fk_fkb_len) {
        fk_fkb_mark_bad("truncated string");
        return;
    }
    fk_fkb_pos = fk_fkb_pos + n;
}
static int fk_fkb_read_string_matches_cstr(const char *s) {
    long long n = fk_fkb_read_u32();
    long long sn = fk_path_len(s);
    if (n < 0 || fk_fkb_pos + n > fk_fkb_len) {
        fk_fkb_mark_bad("truncated string");
        return 0;
    }
    int ok = n == sn;
    long long i = 0;
    while (i < n) {
        if (ok && fk_buf[fk_fkb_pos + i] != s[i]) {
            ok = 0;
        }
        i = i + 1;
    }
    fk_fkb_pos = fk_fkb_pos + n;
    return ok;
}
static void fk_string_table_reset(void) {
    fk_sinit();
    fk_sp = 0;
    fk_sbp = 0;
    long long k = 0;
    while (k < FK_STRING_HASH_BUCKETS) {
        fk_shash[k] = -1;
        k = k + 1;
    }
}
static void fk_fkb_read_table_string(void) {
    long long n = fk_fkb_read_u32();
    if (n < 0 || fk_fkb_pos + n > fk_fkb_len) {
        fk_fkb_mark_bad("truncated table string");
        return;
    }
    if (fk_sp >= fk_scap_s) {
        fk_so = (long long *)fk_store_grow('O', fk_so, fk_scap_s * 8, fk_scap_s * 16, FK_STORE_STR_CELLS * 8, 0);
        fk_sl = (long long *)fk_store_grow('L', fk_sl, fk_scap_s * 8, fk_scap_s * 16, FK_STORE_STR_CELLS * 8, 0);
        fk_scap_s = fk_scap_s * 2;
        fk_snext = realloc(fk_snext, fk_scap_s * 8);
        fk_sdead = realloc(fk_sdead, (unsigned long)fk_scap_s);
        fk_sfree = realloc(fk_sfree, fk_scap_s * 8);
        { long long z = fk_scap_s / 2; while (z < fk_scap_s) { fk_sdead[z] = 0; z = z + 1; } }
        if (fk_so == 0 || fk_sl == 0 || fk_snext == 0) {
            fk_die("fk_fkb: out of memory growing string table");
        }
    }
    while (fk_sbp + n > fk_scap_b) {
        fk_sb = (char *)fk_store_grow('s', fk_sb, fk_scap_b, fk_scap_b * 2, FK_STORE_STR_BYTES, 0);
        fk_scap_b = fk_scap_b * 2;
        fk_sb_check();
    }
    long long start = fk_sbp;
    long long j = 0;
    while (j < n) {
        fk_sb[fk_sbp++] = fk_buf[fk_fkb_pos++];
        j = j + 1;
    }
    fk_so[fk_sp] = start;
    fk_sl[fk_sp] = n;
    long long bucket = fk_str_hash(start, n);
    fk_snext[fk_sp] = fk_shash[bucket];
    fk_shash[bucket] = fk_sp;
    fk_sp = fk_sp + 1;
}
static long long fk_fkb_remap_fn(long long old_fn, long long fn_base) {
    if (old_fn <= 0) {
        return old_fn;
    }
    return fn_base + old_fn - 1;
}
static long long fk_fkb_node_arity_for_tag(long long tag) {
    if (tag == 1 || tag == 18 || tag == 24 || tag == 50 || tag == 73 || tag == 137 ||
        tag == 243) {
        return 0;
    }
    if (tag == 6 || tag == 79 || tag == 109 || tag == 199) {
        /* 199 (method_invoke) rides the variadic (-1) optab row but its node
         * carries THREE children (receiver, name, 242 arg chain) — without
         * this line the optab loop would answer 2 and child [3] would never
         * remap on .fkb import. */
        return 3;
    }
    if (tag == 7 || tag == 14 || tag == 45 || tag == 72 || tag == 74 || tag == 75 ||
        tag == 76 || tag == 78 || tag == 110 || tag == 138) {
        return 1;
    }
    if (tag == 8 || tag == 13 || tag == 19 || tag == 44 || tag == 69 || tag == 70 ||
        tag == 71 || tag == 102 || tag == 103 || tag == 111 || tag == 242 ||
        tag == 244) {
        return 2;
    }
    if (tag == 77) {
        return -2;
    }
    if (tag == 91) {
        return 1;
    }
    if (tag == FK_TAG_CONST_HOLD) {
        /* node[1] is the initializer NODE (remapped); node[2] is the raw
         * memo row, deliberately NOT remapped. */
        return 1;
    }
    long long i = 0;
    while (i < fk_optab_n) {
        if (fk_optab[i].tag == tag) {
            if (fk_optab[i].arity < 0) {
                return 2;
            }
            if (fk_optab[i].arity > 3) {
                return 3;
            }
            return fk_optab[i].arity;
        }
        i = i + 1;
    }
    return 0;
}
static long long fk_fkb_remap_field(long long tag, long long field, long long value,
                                    long long node_base, long long fn_base,
                                    long long str_base) {
    if (value < 0) {
        return value;
    }
    if ((tag == 24 || tag == 50) && field == 1) {
        return value + str_base;
    }
    if ((tag == 12 || tag == 240 || tag == 241 || tag == 243) && field == 1) {
        return fk_fkb_remap_fn(value, fn_base);
    }
    if (tag == 12 && field == 2) {
        return value + node_base;
    }
    if (tag == 243 && field == 2) {
        /* the closure-capture env chain (a 242-cell list, or -1 for a plain non-capturing
         * fn-value -- already returned unchanged above by the value<0 guard). */
        return value + node_base;
    }
    if ((tag == 240 || tag == 241) && field >= 2) {
        return value + node_base;
    }
    if (tag == FK_TAG_CONST_HOLD && field >= 2) {
        /* a hold node's memo (value + gen stamp) never travels: whatever a
         * writer's process state left in these fields, a loaded image
         * starts with an empty hold and rebuilds on first read. */
        return 0;
    }
    if (tag == 77) {
        return field == 2 ? value + node_base : value;
    }
    long long ar = fk_fkb_node_arity_for_tag(tag);
    if (field <= ar) {
        return value + node_base;
    }
    return value;
}
static int fk_fkb_read_symbol_to_srctext(long long *start, long long *len) {
    long long n = fk_fkb_read_u32();
    if (n < 0 || fk_fkb_pos + n > fk_fkb_len) {
        fk_fkb_mark_bad("truncated symbol string");
        return 0;
    }
    fk_srctext_reserve(fk_slen + n + 4);
    fk_srctext[fk_slen] = FK_CH_SEMI;
    fk_slen = fk_slen + 1;
    fk_srctext[fk_slen] = FK_CH_SPACE;
    fk_slen = fk_slen + 1;
    *start = fk_slen;
    *len = n;
    long long i = 0;
    while (i < n) {
        fk_srctext[fk_slen] = fk_buf[fk_fkb_pos + i];
        fk_slen = fk_slen + 1;
        i = i + 1;
    }
    fk_fkb_pos = fk_fkb_pos + n;
    fk_srctext[fk_slen] = FK_CH_LF;
    fk_slen = fk_slen + 1;
    fk_srctext[fk_slen] = 0;
    return 1;
}
static int fk_src_import_fkb_image(const char *fkb_path, const char *expected_src_path,
                                   const char *expected_source_hash,
                                   long long expected_source_mtime) {
#if defined(_WIN32)
    int fd = open(fkb_path, 0x8000);
#else
    int fd = open(fkb_path, 0);
#endif
    if (fd < 0) {
        return 0;
    }
    long long got = fk_read_all_dynamic(fd, fk_path_size_raw(fkb_path));
    close(fd);
    if (got < 0) {
        return 0;
    }
    fk_fkb_begin(got);
    const char magic[8] = {'F', 'K', 'P', 'I', 'F', 'B', '1', 0};
    long long mi = 0;
    while (mi < 8) {
        if (fk_fkb_read_u8() != (long long)(unsigned char)magic[mi]) {
            return 0;
        }
        mi = mi + 1;
    }
    long long version = fk_fkb_read_u32();
    if (version < 5) {
        /* pre-v5 artifacts carry no builder identity, so there is no way to
         * ask which pipeline wrote them -- superseded, not corrupt. */
        return 0;
    }
    /* Every identity read must execute unconditionally: these advance the
     * decode stream. A short-circuit here (the old `ok && read(...)` shape)
     * skipped the hash read after a src-path mismatch and desynced every
     * later read into "truncated string" -- the wrong-CWD reproduction. */
    int builder_matches = fk_fkb_read_string_matches_cstr(FK_FKB_BUILDER_ID);
    char canon[FK_PATH_CAP];
    int src_path_matches =
        fk_fkb_read_string_matches_cstr(fk_path_canon_id(expected_src_path, canon));
    int source_hash_matches = fk_fkb_read_string_matches_cstr(expected_source_hash);
    int source_identity_ok = src_path_matches && source_hash_matches;
    long long stored_source_mtime = fk_fkb_read_signed();
    if (stored_source_mtime != expected_source_mtime) {
        source_identity_ok = 0;
    }
    fk_fkb_skip_string();
    long long sealed = fk_fkb_read_signed();
    if (fk_fkb_bad) {
        fk_diag_path("warning", fkb_path, "corrupt .fkb artifact; rebuilding from source");
        return 0;
    }
    if (!builder_matches) {
        fk_diag_path("warning", fkb_path,
                     "foreign .fkb (written by a different fkwu build); rebuilding from source. "
                     "The bytes of a source do not fix its meaning -- the binary that compiled "
                     "them does");
        return 0;
    }
    if (sealed != 1 || !source_identity_ok) {
        fk_diag_path("warning", fkb_path,
                     "stale .fkb (stored source identity does not match, e.g. written from a "
                     "different working directory); rebuilding from source");
        return 0;
    }
    long long nf = fk_fkb_read_signed();
    if (nf < 1 || nf > fk_fkb_len) {
        return 0;
    }
    fk_fn_reserve(fk_defn_next + nf - 1);
    long long *fn_roots = malloc((unsigned long)nf * 8);
    if (fn_roots == 0) {
        fk_die("fk_import_fkb: out of memory reading function roots");
    }
    long long fn_base = fk_defn_next;
    long long node_base = fk_node_count;
    long long str_base = fk_sp;
    long long i = 0;
    while (!fk_fkb_bad && i < nf) {
        fn_roots[i] = fk_fkb_read_signed();
        i = i + 1;
    }
    long long nr = fk_fkb_read_signed();
    if (nr < 0 || nr > fk_fkb_len) {
        free(fn_roots);
        return 0;
    }
    fk_ast_reserve(fk_node_count + nr);
    i = 0;
    while (!fk_fkb_bad && i < nr) {
        long long tag = fk_fkb_read_signed();
        long long c1 = fk_fkb_read_signed();
        long long c2 = fk_fkb_read_signed();
        long long c3 = fk_fkb_read_signed();
        fk_node[node_base + i][0] = tag;
        fk_node[node_base + i][1] = fk_fkb_remap_field(tag, 1, c1, node_base, fn_base, str_base);
        fk_node[node_base + i][2] = fk_fkb_remap_field(tag, 2, c2, node_base, fn_base, str_base);
        fk_node[node_base + i][3] = fk_fkb_remap_field(tag, 3, c3, node_base, fn_base, str_base);
        i = i + 1;
    }
    fk_node_count = fk_node_count + nr;
    long long ns = fk_fkb_read_signed();
    if (ns < 0) {
        free(fn_roots);
        return 0;
    }
    i = 0;
    while (!fk_fkb_bad && i < ns) {
        fk_fkb_read_table_string();
        i = i + 1;
    }
    i = 1;
    while (i < nf) {
        fk_fn[fn_base + i - 1] = fn_roots[i] < 0 ? fn_roots[i] : fn_roots[i] + node_base;
        i = i + 1;
    }
    fk_defn_next = fn_base + nf - 1;
    if (fk_fn_count < fk_defn_next) {
        fk_fn_count = fk_defn_next;
    }
    free(fn_roots);
    long long symbol_count = fk_fkb_read_signed();
    if (symbol_count < 0 || symbol_count > fk_fkb_len) {
        return 0;
    }
    fk_fn_reserve(fk_fntop + symbol_count + 1);
    i = 0;
    while (!fk_fkb_bad && i < symbol_count) {
        (void)fk_fkb_read_signed();
        long long old_fnidx = fk_fkb_read_signed();
        long long arity = fk_fkb_read_signed();
        long long name_s = 0;
        long long name_n = 0;
        if (!fk_fkb_read_symbol_to_srctext(&name_s, &name_n)) {
            if (fk_fkb_bad) {
                fk_diag_path("warning", fkb_path, "corrupt .fkb artifact; rebuilding from source");
            }
            return 0;
        }
        if (old_fnidx > 0) {
            long long new_fnidx = fk_fkb_remap_fn(old_fnidx, fn_base);
            fk_fnsym_s[fk_fntop] = name_s;
            fk_fnsym_n[fk_fntop] = name_n;
            fk_fnidx[fk_fntop] = new_fnidx;
            if (new_fnidx >= 0 && new_fnidx < fk_fn_capacity) {
                fk_fnar[new_fnidx] = arity;
            }
            fk_live_note_defn(fk_fntop);
            fk_fntop = fk_fntop + 1;
        }
        i = i + 1;
    }
    long long node_symbol_count = fk_fkb_read_signed();
    i = 0;
    while (!fk_fkb_bad && i < node_symbol_count) {
        (void)fk_fkb_read_signed();
        (void)fk_fkb_read_signed();
        long long dep_count = fk_fkb_read_signed();
        long long d = 0;
        while (!fk_fkb_bad && d < dep_count) {
            (void)fk_fkb_read_signed();
            (void)fk_fkb_read_signed();
            d = d + 1;
        }
        i = i + 1;
    }
    if (fk_fkb_bad || fk_fkb_pos != fk_fkb_len) {
        fk_diag_path("warning", fkb_path, "corrupt .fkb artifact; rebuilding from source");
        return 0;
    }
    fk_prog_note_ice(fkb_path, fk_fkb_len, expected_source_hash);
    return 1;
}
/* the whole-image loader restores fn symbols instead of skipping them:
 * names ride the artifact already (written at image time), and a warm
 * process that burns must burn BY NAME — the heat witness printed
 * nameless counts from every .fkb-loaded run until this read
 * (witnessed 2026-09-02, fn#1 for an 80M-dispatch burn). Pre-v3
 * artifacts carry no fnidx; they restore nothing and the fn#N
 * fallback stays honest. */
static int fk_fkb_restore_symbol_image(long long version) {
    long long symbol_count = fk_fkb_read_signed();
    long long i = 0;
    fk_fntop = 0;
    if (fk_live_page != 0) { (fk_live_page + 2)[29] = 0; fk_live_blob_used = 0; } fk_f64_reset();
    fk_fn_reserve(symbol_count + 1);
    while (!fk_fkb_bad && i < symbol_count) {
        (void)fk_fkb_read_signed();
        long long fnidx = -1;
        long long arity = 0;
        if (version >= 3) {
            fnidx = fk_fkb_read_signed();
            arity = fk_fkb_read_signed();
        }
        long long name_s = 0;
        long long name_n = 0;
        if (!fk_fkb_read_symbol_to_srctext(&name_s, &name_n)) {
            return 0;
        }
        if (version >= 3 && fnidx > 0) {
            fk_fnsym_s[fk_fntop] = name_s;
            fk_fnsym_n[fk_fntop] = name_n;
            fk_fnidx[fk_fntop] = fnidx;
            if (fnidx < fk_fn_capacity) {
                fk_fnar[fnidx] = arity;
            }
            fk_live_note_defn(fk_fntop);
            fk_fntop = fk_fntop + 1;
        }
        i = i + 1;
    }
    /* v5 writes a node-symbol section after the function-symbol section.
     * The whole-image loader does not need those rows to invoke a program,
     * but it must consume their exact wire shape before it can attest that
     * the image ended. Leaving them behind made every freshly-written image
     * look like it had trailing bytes, so BML and Form caches rebuilt on
     * every run despite byte-identical source and artifacts. */
    long long node_symbol_count = fk_fkb_read_signed();
    if (node_symbol_count < 0 || node_symbol_count > fk_fkb_len) {
        fk_fkb_mark_bad("node symbol count exceeds artifact bounds");
        return 0;
    }
    i = 0;
    while (!fk_fkb_bad && i < node_symbol_count) {
        (void)fk_fkb_read_signed();
        (void)fk_fkb_read_signed();
        long long dependency_count = fk_fkb_read_signed();
        if (dependency_count < 0 || dependency_count > fk_fkb_len) {
            fk_fkb_mark_bad("node symbol dependency count exceeds artifact bounds");
            return 0;
        }
        long long dependency = 0;
        while (!fk_fkb_bad && dependency < dependency_count) {
            (void)fk_fkb_read_signed();
            (void)fk_fkb_read_signed();
            dependency = dependency + 1;
        }
        i = i + 1;
    }
    return !fk_fkb_bad;
}
static int fk_src_load_fkb_checked(const char *fkb_path, const char *expected_src_path,
                                   const char *expected_source_hash,
                                   long long expected_source_mtime) {
#if defined(_WIN32)
    int fd = open(fkb_path, 0x8000);
#else
    int fd = open(fkb_path, 0);
#endif
    if (fd < 0) {
        return 0;
    }
    long long got = fk_read_all_dynamic(fd, fk_path_size_raw(fkb_path));
    close(fd);
    if (got < 0) {
        fk_fkb_begin(0);
        fk_fkb_mark_bad(got == -3 ? "artifact dynamic read allocation failed"
                                  : "artifact is unreadable");
        return 0;
    }
    fk_fkb_begin(got);
    const char magic[8] = {'F', 'K', 'P', 'I', 'F', 'B', '1', 0};
    long long mi = 0;
    while (mi < 8) {
        if (fk_fkb_read_u8() != (long long)(unsigned char)magic[mi]) {
            fk_fkb_mark_bad("bad magic");
            return 0;
        }
        mi = mi + 1;
    }
    long long version = fk_fkb_read_u32();
    if (version >= 2 && version <= 4) {
        /* pre-v5 lanes: v2/v3 are the old lane width, v4 carries no builder
         * identity -- superseded, not corrupt. Invalidate so the caller
         * recompiles from source and overwrites with a v5 artifact. */
        fk_fkb_mark_bad("pre-v5 artifact lane; superseded");
        return 0;
    }
    if (version != 5) {
        fk_fkb_mark_bad("unsupported version");
        return 0;
    }
    /* Identity reads execute unconditionally -- they advance the decode
     * stream; short-circuiting them desyncs every later read (see
     * fk_src_import_fkb_image). Mismatch stays a soft "rebuild" verdict. */
    int source_identity_ok = 1;
    if (!fk_fkb_read_string_matches_cstr(FK_FKB_BUILDER_ID)) {
        /* Named through mark_bad, not folded into source_identity_ok: the
         * source is exactly what it claims to be here, and reporting "source
         * path, content, or mtime changed" would send the reader hunting a
         * directory that is not the cause. A diagnostic that misnames its own
         * cause is the defect it is reporting on, one level up. */
        fk_fkb_mark_bad("written by a different fkwu build; the bytes of a source do not fix its "
                        "meaning -- the binary that compiled them does");
        return 0;
    }
    char canon[FK_PATH_CAP];
    if (expected_src_path != 0) {
        if (!fk_fkb_read_string_matches_cstr(fk_path_canon_id(expected_src_path, canon))) {
            source_identity_ok = 0;
        }
    } else {
        fk_fkb_skip_string();
    }
    if (expected_source_hash != 0) {
        if (!fk_fkb_read_string_matches_cstr(expected_source_hash)) {
            source_identity_ok = 0;
        }
    } else {
        fk_fkb_skip_string();
    }
    long long stored_source_mtime = fk_fkb_read_signed();
    if (expected_source_mtime > 0 && stored_source_mtime != expected_source_mtime) {
        source_identity_ok = 0;
    }
    fk_fkb_skip_string();
    long long sealed = fk_fkb_read_signed();
    if (fk_fkb_bad) {
        return 0;
    }
    if (sealed != 1) {
        fk_fkb_mark_bad("unsealed artifact");
        return 0;
    }
    if (!source_identity_ok) {
        return 0;
    }
    long long nf = fk_fkb_read_signed();
    if (nf < 0 || nf > fk_fkb_len) {
        fk_fkb_mark_bad("function count exceeds artifact bounds");
        return 0;
    }
    fk_fn_reserve(nf > 0 ? nf : 1);
    fk_fn_count = nf;
    long long i = 0;
    while (!fk_fkb_bad && i < nf) {
        fk_fn[i] = fk_fkb_read_signed(); fk_prog_note_body(i);
        i = i + 1;
    }
    long long nr = fk_fkb_read_signed();
    if (nr < 0 || nr > fk_fkb_len) {
        fk_fkb_mark_bad("node count exceeds image size");
        return 0;
    }
    fk_ast_reserve(nr);
    fk_node_count = nr;
    i = 0;
    while (!fk_fkb_bad && i < nr) {
        fk_node[i][0] = fk_fkb_read_signed();
        fk_node[i][1] = fk_fkb_read_signed();
        fk_node[i][2] = fk_fkb_read_signed();
        fk_node[i][3] = fk_fkb_read_signed();
        if (fk_node[i][0] == FK_TAG_CONST_HOLD) {
            /* the hold's memo never travels (twin of the remap-lane scrub):
             * a loaded image starts empty and rebuilds on first read. */
            fk_node[i][2] = 0;
            fk_node[i][3] = 0;
        }
        i = i + 1;
    }
    long long ns = fk_fkb_read_signed();
    if (ns < 0) {
        fk_fkb_mark_bad("negative string count");
        return 0;
    }
    fk_string_table_reset();
    i = 0;
    while (!fk_fkb_bad && i < ns) {
        fk_fkb_read_table_string();
        i = i + 1;
    }
    if (!fk_fkb_restore_symbol_image(version)) {
        return 0;
    }
    if (fk_fkb_bad) {
        return 0;
    }
    if (fk_fkb_pos != fk_fkb_len) {
        fk_fkb_mark_bad("trailing bytes");
        return 0;
    }
    fk_defn_next = fk_fn_count;
    fk_const_top = 0;
    fk_root = fk_fn_count > 0 ? fk_fn[0] : -1;
    fk_prog_note_ice(fkb_path, fk_fkb_len, expected_source_hash);
    fk_prog_note_counts();
    return 1;
}
static int fk_src_load_fkb(const char *fkb_path) {
    return fk_src_load_fkb_checked(fkb_path, 0, 0, 0);
}
static int fk_run_loaded_program_image(long long arg) {
    if (fk_fn_count <= 0 || fk_fn[0] < 0) {
        fk_die("fk_fkb: no executable root");
    }
    fk_vs[0] = arg << 1;
    fk_vsp = 1;
    fk_pv_root(fk_walk(fk_fn[0], 0));
    return 0;
}
typedef long long (*fk_dylib_main_v1_fn)(long long);
static int fk_run_dylib_artifact(const char *dylib_path, long long arg, int hard_error) {
    void *h = dlopen(dylib_path, 2);
    if (h == 0) {
        if (hard_error) {
            fk_diag_path("error", dylib_path, "could not open .dylib artifact");
        } else {
            fk_diag_path("warning", dylib_path, "fresh .dylib could not be opened; falling back");
        }
        return 0;
    }
    fk_dylib_main_v1_fn fn = (fk_dylib_main_v1_fn)dlsym(h, "fkwu_main_v1");
    if (fn == 0) {
        if (hard_error) {
            fk_diag_path("error", dylib_path, "missing required fkwu_main_v1 ABI symbol");
        } else {
            fk_diag_path("warning", dylib_path, "missing fkwu_main_v1 ABI symbol; falling back");
        }
        return 0;
    }
    fk_pv(fn(arg << 1));
    return 1;
}
static long long fk_src_fkb_version_raw(const char *fkb_path) {
#if defined(_WIN32)
    int fd = open(fkb_path, 0x8000);
#else
    int fd = open(fkb_path, 0);
#endif
    if (fd < 0) {
        return -1;
    }
    unsigned char b[12];
    long long got = read(fd, b, 12);
    close(fd);
    if (got != 12) {
        return -1;
    }
    const unsigned char magic[8] = {'F', 'K', 'P', 'I', 'F', 'B', '1', 0};
    long long i = 0;
    while (i < 8) {
        if (b[i] != magic[i]) {
            return -1;
        }
        i = i + 1;
    }
    return ((long long)b[8] << 24) | ((long long)b[9] << 16) | ((long long)b[10] << 8) |
           (long long)b[11];
}
static long long fk_src_sym_recorded_errors(const char *sym_path) {
    /* the .sym lens records fk_nerr at image-write time (second header line).
     * Returns -1 when the file or the line is absent: an image without its
     * error record is an incomplete cache, not a clean one -- otherwise
     * deleting the lens would launder a degraded image back to exit 0 */
#if defined(_WIN32)
    int fd = open(sym_path, 0x8000);
#else
    int fd = open(sym_path, 0);
#endif
    if (fd < 0) {
        return -1;
    }
    char buf[256];
    long long got = read(fd, buf, 255);
    close(fd);
    if (got <= 0) {
        return -1;
    }
    buf[got] = 0;
    const char *needle = "\ncompile-errors ";
    long long i = 0;
    while (i < got) {
        long long j = 0;
        while (needle[j] != 0 && i + j < got && buf[i + j] == needle[j]) {
            j = j + 1;
        }
        if (needle[j] == 0) {
            long long v = 0;
            long long p = i + j;
            while (p < got && buf[p] >= '0' && buf[p] <= '9') {
                v = v * 10 + (buf[p] - '0');
                p = p + 1;
            }
            return v;
        }
        i = i + 1;
    }
    return -1;
}
/* the unrunnable latch as the image recorded it: 1 refused, 0 runnable, -1 absent.
 * -1 means an older lens that predates the field, and the caller treats it the same
 * way it treats a missing error record -- an incomplete cache to rebuild, never a
 * clean one. Guessing "probably runnable" here would restore exactly the laundering
 * this field exists to stop. */
static long long fk_src_sym_recorded_unrunnable(const char *sym_path) {
#if defined(_WIN32)
    int fd = open(sym_path, 0x8000);
#else
    int fd = open(sym_path, 0);
#endif
    if (fd < 0) {
        return -1;
    }
    char buf[256];
    long long got = read(fd, buf, 255);
    close(fd);
    if (got <= 0) {
        return -1;
    }
    buf[got] = 0;
    const char *needle = "\nunrunnable ";
    long long i = 0;
    while (i < got) {
        long long j = 0;
        while (needle[j] != 0 && i + j < got && buf[i + j] == needle[j]) {
            j = j + 1;
        }
        if (needle[j] == 0) {
            long long v = 0;
            long long p = i + j;
            while (p < got && buf[p] >= '0' && buf[p] <= '9') {
                v = v * 10 + (buf[p] - '0');
                p = p + 1;
            }
            return v;
        }
        i = i + 1;
    }
    return -1;
}
static void fk_src_reset_compile_state(void) {
    fk_fn_reserve(1);
    fk_arg_n = 0;
    fk_fname_n = 0;
    fk_fn_count = 1;
    fk_node_count = 0;
    fk_bd_top = 0;
    fk_maxslot = 0;
    fk_nerr = 0;
    fk_nwarn = 0;
    fk_src_truncated = 0;
    fk_src_unrunnable = 0;
    fk_string_table_reset();
    fk_fntop = 0;
    if (fk_live_page != 0) { (fk_live_page + 2)[29] = 0; fk_live_blob_used = 0; } fk_f64_reset();
    fk_const_top = 0;
    fk_defn_next = 1;
    fk_root = -1;
    /* fk_defn_next resets to 1: the NEXT compile pass renumbers fn-indices from scratch, so any
     * low index it assigns could be a completely different (and very possibly non-capturing)
     * function than whatever previously lived there. fk_fn_cap_reserve only zeroes newly-grown
     * table rows, not already-allocated ones, so without this an unrelated function landing on a
     * reused index would silently inherit a stale nonzero fk_fn_cap_count and get wrapped with a
     * phantom capture prologue reading a meaningless leftover slot -- caught by adversarial
     * review before ever being exercised by a real multi-pass compile (the import lane's
     * speculative per-dependency passes are exactly this shape). fk_fn_cap_encoff/fk_fn_cap_slot
     * need no clearing of their own: every read of them is already gated by an index below the
     * (now zeroed) count, so stale bytes past that point are simply never reached. */
    long long capreset_i = 0;
    while (capreset_i < fk_fn_cap_capacity) {
        fk_fn_cap_count[capreset_i] = 0;
        fk_fn_parent_idx[capreset_i] = -1;
        capreset_i = capreset_i + 1;
    }
}
/* WHOLE-SOURCE PAREN BALANCE, decided ONCE over the assembled unit.
 *
 * Every reader below is permissive by construction: a form's closer is consumed with
 * `if (pos < len && text[pos] == RPAREN) pos++`, so a form that simply RUNS OUT of text
 * is auto-closed and evaluated as though the author had written it that way. `(do (add 1 2)`
 * -- the `(do` never closed -- answered 3 and exited 0. Note the shape: 3 is the RIGHT
 * answer to the WRONG text, which is exactly why nothing downstream can catch it. Every band
 * in this body is one `(do ...)` form, so a single missing character anywhere in a band file
 * yields a plausible verdict that reads green (Stone 41 watched one return 1023 that way, and
 * the FK_SOURCE_TEXT_CAP_INIT comment above records the same family biting once before as the
 * "N=100 cliff"). It needs no unusual naming -- unlike [unbound-name] or [shadowed-primitive]
 * it is reachable by a typo.
 *
 * fk_src_truncated was declared for precisely this and was never set by anything: a gate that
 * existed only as a comment. The balance is therefore checked here, over the flattened text
 * (preludes included, which is why an unbalanced prelude is caught at the root compile), using
 * the same lexical rules fk_skip_balanced already uses -- `;` runs to end of line, "..." is
 * opaque to structure and a backslash escape stays inside it.
 *
 * Depth going negative is a stray ')'; depth left positive at EOF is an unclosed form, reported
 * at the '(' that opened it. Either way the program was never fully READ, so there is nothing
 * for a verdict to be OF -- the same line this stone drew for the unbound read. Set the
 * unrunnable latch and let both execution doors refuse with a non-zero exit. */
static void fk_src_check_balance(void) {
    long long p = 0;
    long long depth = 0;
    long long outermost_open = -1;
    while (p < fk_slen) {
        char c = fk_srctext[p];
        if (c == FK_CH_SEMI) {
            while (p < fk_slen && fk_srctext[p] != FK_CH_LF) {
                p = p + 1;
            }
            continue;
        }
        if (c == FK_CH_DQUOTE) {
            p = p + 1;
            while (p < fk_slen && fk_srctext[p] != FK_CH_DQUOTE) {
                if (fk_srctext[p] == FK_CH_BACKSLASH && p + 1 < fk_slen) {
                    p = p + 1;
                }
                p = p + 1;
            }
            if (p < fk_slen) {
                p = p + 1;
            }
            continue;
        }
        if (c == FK_CH_LPAREN) {
            if (depth == 0) {
                outermost_open = p;
            }
            depth = depth + 1;
        } else if (c == FK_CH_RPAREN) {
            depth = depth - 1;
            if (depth < 0) {
                fk_diag(FK_DIAG_ERR, p,
                        "[unbalanced-source] stray ')' closes a form that was never opened -- "
                        "the text cannot be read as the program it claims to be, so there is "
                        "nothing for a verdict to be of. Refusing to run");
                fk_src_unrunnable = 1;
                return;
            }
        }
        p = p + 1;
    }
    if (depth > 0) {
        /* THE INPUT ENDED BEFORE THE FORM CLOSED. Named apart from the stray closer above on
         * purpose: these are different repairs. A stray ')' is code the author got wrong. This
         * is a stream that STOPPED -- and the reader cannot tell a stream that ENDED from one
         * that was INTERRUPTED, because the terminator is byte-identical for both (fk_run_src
         * reads to EOF; fsh-read walks input_byte to a NUL). So a cut pipe and a finished
         * program arrive looking the same, and the prefix gets evaluated and reported as a
         * success. That is axiom-5 at the INPUT boundary: `nothing` (the bytes stopped) read as
         * `0` (the bytes ended) -- the same conflation as a zeroed Metal buffer read as a
         * computed zero, one layer further out. `edgedrop` is the body's word for it.
         * fk_src_truncated was declared for exactly "the source was amputated" and was never
         * once set; this is what it was for, so it is set here and the gate it guards finally
         * has a meaning. Telling the author the INPUT ended -- rather than that their code is
         * malformed -- is the difference between "your pipe was cut" and "your code is wrong". */
        fk_diag(FK_DIAG_ERR, outermost_open >= 0 ? outermost_open : 0,
                "[input-ended-mid-form] the input ended before this form closed -- %lld open "
                "paren(s) remain. A stream that STOPPED and a stream that FINISHED end with the "
                "same terminator, so the prefix would otherwise be read as a whole program: the "
                "permissive reader auto-closes it and computes the right answer to the wrong "
                "text. Completion is not the absence of more bytes. Refusing to run",
                depth);
        fk_src_truncated = 1;
        fk_src_unrunnable = 1;
    }
}
static void fk_src_compile_current_unit(const char *path, const char *fkb_path,
                                        const char *sym_path, long long unit_mtime,
                                        const char *source_hash) {
    fk_spos = 0;
    fk_srctext[fk_slen] = 0;
    fk_src_check_balance();
    /* Refuse BEFORE the readers touch the text, not after. The parse loop below advances by
     * consuming forms; on a stray ')' at top level it consumes nothing and does not advance,
     * so running it over unbalanced text spins (the zero-advance seen 2026-07-18). Diagnosing
     * and then parsing anyway would trade a silent wrong answer for a silent hang, which is not
     * a trade. The unit yields the empty program and the execution doors refuse on the latch. */
    if (fk_src_unrunnable) {
        fk_fn[0] = fk_smklit(0);
        fk_fn_count = 1;
        return;
    }
    /* stone 4+5: multi-function root logic, preserved */
    fk_prescan_defns();
    /* two-pass: register every top-level defn name+index+arity BEFORE bodies, so forward + mutual
     * references resolve */
    fk_spos = 0;
    while (1) {
        fk_sskip();
        if (fk_spos >= fk_slen) {
            break;
        }
        fk_parse_top();
    }
    if (fk_root >= 0) {
        fk_fn[0] = fk_root;
    } else if (fk_defn_next > 1) {
        fk_fn[0] = fk_fn[fk_defn_next - 1];
    }
    /* single/last defn, staged-arg driven (stones 1-2) */
    else {
        fk_fn[0] = fk_smklit(0);
    }
    fk_fn_count = fk_defn_next;
    if (fk_maxslot > 0) {
        fk_fn[0] = fk_smknode(111, fk_smklit(fk_maxslot), fk_fn[0], 0);
    }
    if (!fk_src_write_fkb(path, fkb_path, sym_path, unit_mtime, source_hash)) {
        if (fk_fkb_write_overflow) {
            fk_die("fk_run_src: failed to write .fkb/.sym artifacts -- a value in the "
                   "program image is outside the .fkb v4 signed lane (magnitude 2^63, "
                   "i.e. LLONG_MIN) or a length exceeds u32");
        }
        fk_die("fk_run_src: failed to write .fkb/.sym artifacts");
    }
}
static int fk_src_compile_artifact_only(const char *path) {
    char compile_path[FK_PATH_CAP];
    fk_cstr_copy(compile_path, path, FK_PATH_CAP);
    long long saved_dep_count = fk_src_dep_count;
    char saved_root_path[FK_PATH_CAP];
    long long saved_root_len = fk_src_root_len;
    char *saved_root_text = malloc((unsigned long)(saved_root_len + 2));
    char *saved_srctext = malloc((unsigned long)(fk_slen + 1));
    char (*saved_dep_path)[FK_PATH_CAP] = saved_dep_count > 0 ?
        malloc(sizeof(*saved_dep_path) * (unsigned long)saved_dep_count) : 0;
    long long *saved_dep_mtime = saved_dep_count > 0 ?
        malloc(sizeof(*saved_dep_mtime) * (unsigned long)saved_dep_count) : 0;
    long long *saved_dep_size = saved_dep_count > 0 ?
        malloc(sizeof(*saved_dep_size) * (unsigned long)saved_dep_count) : 0;
    unsigned long long *saved_dep_digest = saved_dep_count > 0 ?
        malloc(sizeof(*saved_dep_digest) * (unsigned long)saved_dep_count) : 0;
    long long *saved_dep_parent = saved_dep_count > 0 ?
        malloc(sizeof(*saved_dep_parent) * (unsigned long)saved_dep_count) : 0;
    long long *saved_dep_end = saved_dep_count > 0 ?
        malloc(sizeof(*saved_dep_end) * (unsigned long)saved_dep_count) : 0;
    long long *saved_dep_text_off = saved_dep_count > 0 ?
        malloc(sizeof(*saved_dep_text_off) * (unsigned long)saved_dep_count) : 0;
    long long *saved_dep_text_len = saved_dep_count > 0 ?
        malloc(sizeof(*saved_dep_text_len) * (unsigned long)saved_dep_count) : 0;
    /* THE ONE FIELD THE SNAPSHOT FORGOT. Every other dependency column is
     * saved here and restored below; `lowered` was not. A speculative compile
     * rebuilds the whole dependency table for ITS unit — setting and clearing
     * `lowered` for ITS chain — and then hands back a table whose paths,
     * mtimes, digests, parents, ends and text spans are the caller's again
     * while the lowered flags belong to somebody else. The import lane reads
     * those flags immediately afterwards to decide, per unit, image or carry.
     * A .bml read as a plain .fk is probed as an image it cannot be; a .fk
     * read as lowered is carried when it could have been imaged; and a unit
     * that falls between the two decisions is simply absent from the program
     * that runs, which is what "every later call went numb as
     * [unresolved-call] with nothing naming the drop" already described from
     * the far end. Snapshot it like the rest. */
    long long *saved_dep_lowered = saved_dep_count > 0 ?
        malloc(sizeof(*saved_dep_lowered) * (unsigned long)saved_dep_count) : 0;
    if (saved_root_text == 0 || saved_srctext == 0 || saved_dep_path == 0 ||
        saved_dep_mtime == 0 || saved_dep_size == 0 || saved_dep_digest == 0 ||
        saved_dep_parent == 0 || saved_dep_end == 0 ||
        saved_dep_text_off == 0 || saved_dep_text_len == 0 ||
        saved_dep_lowered == 0) {
        if (saved_dep_count == 0 && saved_dep_path == 0 &&
            saved_dep_mtime == 0 && saved_dep_size == 0 &&
            saved_dep_digest == 0 && saved_dep_parent == 0 && saved_dep_end == 0 &&
            saved_dep_text_off == 0 && saved_dep_text_len == 0 &&
            saved_dep_lowered == 0) {
            /* zero dependency entries need no snapshot allocation */
        } else {
            fk_die("fk_import_compile: out of memory saving source unit");
        }
    }
    if (saved_root_text == 0 || saved_srctext == 0) {
        fk_die("fk_import_compile: out of memory saving source unit");
    }
    fk_cstr_copy(saved_root_path, fk_src_root_path, FK_PATH_CAP);
    long long saved_slen = fk_slen;
    long long i = 0;
    while (i < saved_root_len + 1) {
        saved_root_text[i] = fk_src_root_text[i];
        i = i + 1;
    }
    i = 0;
    while (i < saved_slen) {
        saved_srctext[i] = fk_srctext[i];
        i = i + 1;
    }
    i = 0;
    while (i < saved_dep_count) {
        fk_cstr_copy(saved_dep_path[i], fk_src_dep_path[i], FK_PATH_CAP);
        saved_dep_mtime[i] = fk_src_dep_mtime[i];
        saved_dep_size[i] = fk_src_dep_size[i];
        saved_dep_digest[i] = fk_src_dep_digest[i];
        saved_dep_parent[i] = fk_src_dep_parent[i];
        saved_dep_end[i] = fk_src_dep_end[i];
        saved_dep_text_off[i] = fk_src_dep_text_off[i];
        saved_dep_text_len[i] = fk_src_dep_text_len[i];
        saved_dep_lowered[i] = fk_src_dep_lowered[i];
        i = i + 1;
    }
    char source_hash[FK_SRC_HASH_CAP];
    char fkb_path[FK_PATH_CAP];
    char sym_path[FK_PATH_CAP];
    long long unit_mtime = 0;
    int ok = 0;
    int loaded = 0;
    if (fk_path_replace_ext(compile_path, ".fkb", fkb_path, FK_PATH_CAP) &&
        fk_path_replace_ext(compile_path, ".sym", sym_path, FK_PATH_CAP)) {
        if (fk_unit_lowers(compile_path)) {
            /* A UNIT IS LOADED THE WAY IT IS WRITTEN. fk_src_load_unit reads
             * bytes off disk and hands them to the .fk collector, which is
             * right for a .fk and blind for a .bml: the brace surface is not
             * .fk, and a "// preludes:" line is not a ";" comment, so a .bml
             * loaded that way arrives as a stranger with no prelude chain and
             * a thousand unresolved names — the count that the import gate's
             * warning then reported as if it were news about the unit. Lower
             * it first, through the same in-memory door the root lane uses,
             * and the unit that gets compiled is the unit that was written. */
            long long src_m = fk_path_mtime_raw(compile_path);
            if (src_m > 0) {
                long long low_len = 0;
                char *low = fk_bml_lower_to_mem(compile_path, &low_len);
                if (low != 0) {
                    loaded = fk_src_load_unit_buffer(compile_path, low, low_len, src_m,
                            source_hash, FK_SRC_HASH_CAP, &unit_mtime);
                }
            }
        } else {
            loaded = fk_src_load_unit(compile_path, source_hash, FK_SRC_HASH_CAP, &unit_mtime);
        }
    }
    if (loaded) {
        /* This compile is SPECULATIVE: it builds a candidate per-unit image
         * for the import path. A unit that only resolves inside the root's
         * flat prelude chain (e.g. it carries no "; preludes:" line of its
         * own) diagnoses unresolved calls HERE that the authoritative flat
         * compile then resolves — printing them as bare "error:" lines while
         * the run exits 0 was the witnessed lie. Quiet mode: the counts still
         * reach the .sym compile-errors record (the import gate reads it and
         * refuses degraded images), but nothing prints; the import gate emits
         * the one honest, counted warning instead. */
        fk_diag_quiet = fk_diag_quiet + 1;
        fk_src_reset_compile_state();
        fk_src_compile_current_unit(compile_path, fkb_path, sym_path, unit_mtime, source_hash);
        fk_diag_quiet = fk_diag_quiet - 1;
        ok = 1;
    }
    fk_src_dep_count = saved_dep_count;
    if (!fk_src_dep_reserve(saved_dep_count)) {
        fk_die("fk_import_compile: out of memory restoring source unit");
    }
    fk_cstr_copy(fk_src_root_path, saved_root_path, FK_PATH_CAP);
    fk_src_root_len = saved_root_len;
    fk_slen = saved_slen;
    fk_src_root_reserve(saved_root_len + 2);
    fk_srctext_reserve(saved_slen + 1);
    i = 0;
    while (i < saved_root_len + 1) {
        fk_src_root_text[i] = saved_root_text[i];
        i = i + 1;
    }
    i = 0;
    while (i < saved_slen) {
        fk_srctext[i] = saved_srctext[i];
        i = i + 1;
    }
    i = 0;
    while (i < saved_dep_count) {
        fk_cstr_copy(fk_src_dep_path[i], saved_dep_path[i], FK_PATH_CAP);
        fk_src_dep_mtime[i] = saved_dep_mtime[i];
        fk_src_dep_size[i] = saved_dep_size[i];
        fk_src_dep_digest[i] = saved_dep_digest[i];
        fk_src_dep_parent[i] = saved_dep_parent[i];
        fk_src_dep_end[i] = saved_dep_end[i];
        fk_src_dep_text_off[i] = saved_dep_text_off[i];
        fk_src_dep_text_len[i] = saved_dep_text_len[i];
        fk_src_dep_lowered[i] = saved_dep_lowered[i];
        i = i + 1;
    }
    free(saved_root_text);
    free(saved_srctext);
    free(saved_dep_path);
    free(saved_dep_mtime);
    free(saved_dep_size);
    free(saved_dep_digest);
    free(saved_dep_parent);
    free(saved_dep_end);
    free(saved_dep_text_off);
    free(saved_dep_text_len);
    free(saved_dep_lowered);
    return ok;
}
static int fk_src_try_import_fkb_images(const char *root_path) {
    long long direct_count = 0;
    long long carry_count = 0;
    long long carry_bytes = 0;
    long long i = 1;
    fk_import_images = 0;
    fk_import_carried = 0;
    fk_import_refusal = 0;
    while (i < fk_src_dep_count) {
        /* .bml deps are floor-lane units: their meaning lives with their
         * carried prelude chain and their cache is the floor's own
         * .bml.fkb ice. Probing one "alone" reads the RAW brace surface
         * off disk, finds hundreds of unresolved names, and poisons the
         * floor's cache with a REFUSED sym (witnessed 2026-08-30). The
         * import lane does not probe them as images -- it CARRIES their
         * already-collected text (and the text of every other unit no
         * image covers) beside the imports. Skipping them entirely was
         * the seed of a silent amputation: images in, root text in, and
         * a direct .bml prelude's lowered defns simply absent -- every
         * later call went numb as [unresolved-call] with nothing naming
         * the drop (peer-contribution birth surface, 2026-09-01). */
        if (fk_src_dep_parent[i] == 0 &&
            !fk_src_dep_lowered[i]) {
            direct_count = direct_count + 1;
            i = fk_src_dep_end[i];
        } else {
            carry_count = carry_count + 1;
            carry_bytes = carry_bytes + fk_src_dep_text_len[i];
            i = i + 1;
        }
    }
    if (direct_count == 0) {
        fk_import_refusal = 1;
        return 0;
    }
    i = 1;
    while (i < fk_src_dep_count) {
        if (fk_src_dep_parent[i] == 0 &&
            !fk_src_dep_lowered[i]) {
            char dep_fkb_path[FK_PATH_CAP];
            long long dep_end = fk_src_dep_end[i];
            long long dep_mtime = fk_src_unit_mtime_range(i, dep_end);
            if (!fk_path_replace_ext(fk_src_dep_path[i], ".fkb", dep_fkb_path, FK_PATH_CAP)) {
                fk_import_refusal = 4;
                return 0;
            }
            if (fk_path_mtime_raw(dep_fkb_path) < dep_mtime ||
                fk_src_fkb_version_raw(dep_fkb_path) < 5) {
                if (!fk_src_compile_artifact_only(fk_src_dep_path[i])) {
                    fk_import_refusal = 3;
                    return 0;
                }
            }
        }
        i = i + 1;
    }
    /* CARRY EVERY UNIT THE IMAGES DO NOT COVER. The wipe below erases the
     * whole-program text, and imported symbol names are then written into
     * fk_srctext -- so the uncovered units' text (a direct .bml prelude's
     * lowered subtree, in the witnessed wound) must be copied aside NOW and
     * re-appended after the imports, in its original post-order (ascending
     * text offset), or the compiled program silently loses those defns. */
    char *carry_text = 0;
    long long *carry_idx = 0;
    long long *carry_pos = 0;
    long long *carry_len = 0;
    if (carry_count > 0) {
        carry_text = malloc((unsigned long)(carry_bytes + 1));
        carry_idx = malloc(sizeof(*carry_idx) * (unsigned long)carry_count);
        carry_pos = malloc(sizeof(*carry_pos) * (unsigned long)carry_count);
        carry_len = malloc(sizeof(*carry_len) * (unsigned long)carry_count);
        if (carry_text == 0 || carry_idx == 0 || carry_pos == 0 || carry_len == 0) {
            free(carry_text); free(carry_idx); free(carry_pos); free(carry_len);
            fk_import_refusal = 2;
            return 0;
        }
        long long cn = 0;
        i = 1;
        while (i < fk_src_dep_count) {
            if (fk_src_dep_parent[i] == 0 &&
                !fk_src_dep_lowered[i]) {
                i = fk_src_dep_end[i];
            } else {
                carry_idx[cn] = i;
                cn = cn + 1;
                i = i + 1;
            }
        }
        /* original text order is ascending text_off (post-order append);
         * insertion sort -- the carried set is small (one floor subtree). */
        i = 1;
        while (i < carry_count) {
            long long key = carry_idx[i];
            long long j = i - 1;
            while (j >= 0 && fk_src_dep_text_off[carry_idx[j]] > fk_src_dep_text_off[key]) {
                carry_idx[j + 1] = carry_idx[j];
                j = j - 1;
            }
            carry_idx[j + 1] = key;
            i = i + 1;
        }
        long long cpos = 0;
        cn = 0;
        while (cn < carry_count) {
            long long u = carry_idx[cn];
            long long off = fk_src_dep_text_off[u];
            long long len = fk_src_dep_text_len[u];
            carry_pos[cn] = cpos;
            carry_len[cn] = len;
            long long k = 0;
            while (k < len) {
                carry_text[cpos + k] = fk_srctext[off + k];
                k = k + 1;
            }
            cpos = cpos + len;
            cn = cn + 1;
        }
        carry_text[cpos] = 0;
    }
    fk_src_reset_compile_state();
    fk_slen = 0;
    fk_srctext[0] = 0;
    int ok = 1;
    i = 1;
    while (ok && i < fk_src_dep_count) {
        if (fk_src_dep_parent[i] == 0 &&
            !fk_src_dep_lowered[i]) {
            char dep_fkb_path[FK_PATH_CAP];
            char dep_hash[FK_SRC_HASH_CAP];
            long long dep_end = fk_src_dep_end[i];
            long long dep_mtime = fk_src_unit_mtime_range(i, dep_end);
            if (!fk_path_replace_ext(fk_src_dep_path[i], ".fkb", dep_fkb_path, FK_PATH_CAP)) {
                fk_import_refusal = 4;
                ok = 0;
                break;
            }
            if (!fk_src_unit_hash_range(i, dep_end, dep_hash, FK_SRC_HASH_CAP)) {
                fk_import_refusal = 5;
                ok = 0;
                break;
            }
            {
                /* a dep image compiled with recovered errors is degraded truth;
                 * importing it would bake the degradation invisibly into this
                 * run -- refuse (unknown record counts as degraded), so the
                 * caller falls back to the flat compile where the full chain
                 * resolves. The refusal itself must not be silent: this is
                 * where a unit that cannot stand alone costs the run its
                 * imported-image path, on THIS run and every future one until
                 * the unit's own prelude chain is healed — say so, once,
                 * counted, so the tally and stderr agree. */
                char dep_sym_path[FK_PATH_CAP];
                if (!fk_path_replace_ext(fk_src_dep_path[i], ".sym", dep_sym_path, FK_PATH_CAP)) {
                    fk_import_refusal = 4;
                    ok = 0;
                    break;
                }
                long long dep_recorded = fk_src_sym_recorded_errors(dep_sym_path);
                if (dep_recorded != 0) {
                    if (dep_recorded > 0) {
                        fk_diag(FK_DIAG_WARN, -1,
                                "%s: unit is not importable standalone (%lld unresolved "
                                "error(s) compiled alone; missing '; preludes:' line?) -- "
                                "image rejected, falling back to the whole-program compile",
                                fk_src_dep_path[i], dep_recorded);
                    }
                    fk_import_refusal = 6;
                    ok = 0;
                    break;
                }
            }
            if (!fk_src_import_fkb_image(dep_fkb_path, fk_src_dep_path[i], dep_hash, dep_mtime)) {
                fk_import_refusal = 7;
                ok = 0;
                break;
            }
            fk_import_images = fk_import_images + 1;
        }
        i = i + 1;
    }
    if (ok && carry_count > 0) {
        long long cn = 0;
        while (cn < carry_count) {
            long long u = carry_idx[cn];
            if (!fk_src_append_text(fk_src_dep_path[u], carry_text + carry_pos[cn],
                                    carry_len[cn])) {
                fk_import_refusal = 8;
                ok = 0;
                break;
            }
            fk_import_carried = fk_import_carried + 1;
            cn = cn + 1;
        }
    }
    if (ok && !fk_src_append_text(root_path, fk_src_root_text, fk_src_root_len)) {
        fk_import_refusal = 8;
        ok = 0;
    }
    if (!ok) {
        /* the flat compile will carry everything; a refused lane imported and
         * carried nothing into the program that runs. */
        fk_import_images = 0;
        fk_import_carried = 0;
    }
    free(carry_text);
    free(carry_idx);
    free(carry_pos);
    free(carry_len);
    return ok;
}
static int fk_run_src(const char *path, long long arg) {
    char fkb_path[FK_PATH_CAP];
    char sym_path[FK_PATH_CAP];
    char dylib_path[FK_PATH_CAP];
    char expected_source_hash[FK_SRC_HASH_CAP];
    long long unit_mtime = 0;
    if (!fk_path_replace_ext(path, ".fkb", fkb_path, FK_PATH_CAP) ||
        !fk_path_replace_ext(path, ".sym", sym_path, FK_PATH_CAP) ||
        !fk_path_replace_ext(path, ".dylib", dylib_path, FK_PATH_CAP)) {
        fk_die("fk_run_src: artifact path exceeds buffer");
    }
    if (!fk_src_load_unit(path, expected_source_hash, FK_SRC_HASH_CAP, &unit_mtime)) {
        return 2;
    }
    long long fkb_mtime = fk_path_mtime_raw(fkb_path);
    long long dylib_mtime = fk_path_mtime_raw(dylib_path);
    if (dylib_mtime >= unit_mtime) {
        fk_run_door = 3;
        if (fk_run_dylib_artifact(dylib_path, arg, 0)) {
            return 0;
        }
        fk_run_door = 0;
    } else if (dylib_mtime > 0) {
        fk_diag_path("warning", dylib_path, "stale .dylib ignored");
    }
    long long recorded = fk_src_sym_recorded_errors(sym_path);
    long long recorded_unrunnable = fk_src_sym_recorded_unrunnable(sym_path);
    if (fkb_mtime >= unit_mtime && (recorded < 0 || recorded_unrunnable < 0)) {
        /* an image without its error record is an incomplete cache; rebuild
         * rather than guess (older lenses, or a lens deleted out from under
         * the image) */
        fk_diag_path("warning", sym_path, "sym lens lacks a compile-error record; rebuilding");
    } else if (fkb_mtime >= unit_mtime) {
        if (fk_src_load_fkb_checked(fkb_path, path, expected_source_hash, unit_mtime)) {
            /* THE REFUSAL TRAVELS WITH THE IMAGE. The fresh-compile door returns
             * without printing the root value when the unrunnable latch is set,
             * so a cached run that prints it anyway does not merely replay a
             * degraded image -- it overturns a decision the kernel already made,
             * and it does so silently, on the second run of an unchanged file.
             * Refuse identically here: same door, same answer, whichever side of
             * the cache the caller happens to be standing on. */
            if (recorded_unrunnable > 0) {
                fk_diag_path("error", sym_path,
                        "cached image was REFUSED at compile (unbound name in value position); "
                        "refusing to run it from cache -- fix source and rerun");
                fk_diag_flush();
                fk_heat_report();
                return 1;
            }
            /* the compile carried errors when this image was written; the
             * cache must not launder them -- replay the tally as exit truth */
            if (recorded > 0) {
                fk_diag_path("warning", sym_path,
                        "cached image was compiled with errors; fix source and rerun to clear");
            }
            fk_run_door = 2;
            int rc = fk_run_loaded_program_image(arg);
            fk_heat_report();
            return recorded > 0 && rc == 0 ? 1 : rc;
        }
        if (fk_fkb_bad) {
            char whybuf[192];
            sprintf(whybuf, "unusable .fkb artifact (%s); rebuilding", fk_fkb_bad_why);
            fk_diag_path("warning", fkb_path, whybuf);
        } else {
            fk_diag_path("warning", fkb_path,
                         "fresh-looking .fkb failed source identity check (source path, content, "
                         "or mtime changed, e.g. invoked from a different directory); rebuilding");
        }
    } else if (fkb_mtime > 0) {
        fk_diag_path("warning", fkb_path, "stale .fkb ignored");
    }
    int import_images_loaded = fk_src_try_import_fkb_images(path);
    fk_run_door = import_images_loaded ? 1 : 0;
    if (!import_images_loaded) {
        if (!fk_src_load_unit(path, expected_source_hash, FK_SRC_HASH_CAP, &unit_mtime)) {
            return 2;
        }
        fk_src_reset_compile_state();
    }
    fk_src_compile_current_unit(path, fkb_path, sym_path, unit_mtime, expected_source_hash);
    fk_vs[0] = arg << 1;
    fk_vsp = 1;
    /* ── PARSE DONE, EXECUTION BEGINS ── gcc-style tally, then the two-phase gate:
     * an amputated source is a hard error -- surface the prefix's diagnostics but
     * REFUSE to run (nonzero), never silently execute the truncated program. Any
     * OTHER compile error still recovers INTO a runnable (if degraded) program and
     * runs, carrying a nonzero EXIT via fk_nerr at the final return. */
    fk_diag_flush();
    if (fk_src_truncated || fk_src_unrunnable) {
        fk_heat_report();
        return 1;
    }
    fk_pv_root(fk_walk(fk_fn[0], 0));
    fk_heat_report();
    return (fk_nerr > 0 || fk_nerr_seen > 0) ? 1 : 0;
}
/* --feval: run a recipe THROUGH form-eval (Form), not fk_walk directly. The C seed bootstraps the
 * form-eval meta-evaluator (read live from grammars/form-eval.fk); form-eval reads the recipe as a
 * STRING and evaluates it. The recipe source is escaped into a Form string literal and appended as
 * the final (fe-eval "<recipe>") form, so the whole bundle is one --src-shaped program that
 * fk_parse_top + fk_walk run — but the recipe's value is computed by fe-eval, in Form, on fk_walk.
 * The root form is (fe-eval ...), whose value is the meta-eval's result; we print it by value-kind
 * (int/float/nothing) so an integer result prints as an integer (the fk_pv_root root-op heuristic
 * would mis-key on a recipe that returns a string, which is outside this mode's numeric scope). */
static int fk_run_feval(const char *path) {
#if defined(_WIN32)
    int fd = open(path, 0x8000);
#else
    int fd = open(path, 0);
#endif
    if (fd < 0) {
        return 2;
    }
    char rbuf[131072];
    long long rg = fk_read_all_bounded(fd, rbuf, 131071);
    close(fd);
    if (rg < 0) {
        if (rg == -2) {
            fk_die("fk_run_feval: recipe source exceeds buffer");
        }
        return 3;
    }
    rbuf[rg] = 0;

    /* build into fk_srctext: <char_at/ord helpers> + <form-eval read from its canonical .fk at
     * runtime> + "\n(fe-eval \"<escaped recipe>\")\n". No embedded blob, no codegen step (the repo
     * IS the body; form-eval is read live from grammars/form-eval.fk, never a drifting C-string
     * copy). */
    long long w = 0;
    long long cap = 262143;
    fk_srctext_reserve(cap + 1);
    const char *helpers =
        "(defn char_at (s i) (substring s i (add i 1)))\n(defn ord (c) (str_byte_at c 0))\n";
    long long hi = 0;
    while (helpers[hi] != 0) {
        if (w >= cap) {
            return 4;
        }
        fk_srctext[w] = helpers[hi];
        w = w + 1;
        hi = hi + 1;
    }
#if defined(_WIN32)
    int efd = open("grammars/form-eval.fk", 0x8000);
#else
    int efd = open("grammars/form-eval.fk", 0);
#endif
    if (efd < 0) {
        return 5;
    }
    /* run --feval from the repo root (grammars/form-eval.fk must be reachable) */
    long long eg = fk_read_all_bounded(efd, fk_srctext + w, cap - w - 1);
    close(efd);
    if (eg < 0) {
        if (eg == -2) {
            fk_die("fk_run_feval: form-eval source exceeds the form-eval build buffer");
        }
        return 5;
    }
    w = w + eg;
    const char *tail = "\n(fe-eval \"";
    long long ti = 0;
    while (tail[ti] != 0) {
        if (w >= cap) {
            return 4;
        }
        fk_srctext[w] = tail[ti];
        w = w + 1;
        ti = ti + 1;
    }

    /* escape the recipe into a Form string literal: backslash, double-quote, newline, CR. The
     * recipe is a one-line value once escaped (newlines -> \n) so fe-str scans it as a single
     * literal. */
    long long ri = 0;
    while (ri < rg) {
        char c = rbuf[ri];
        if (w + 2 >= cap) {
            return 4;
        }
        if (c == FK_CH_BACKSLASH) {
            fk_srctext[w] = FK_CH_BACKSLASH;
            fk_srctext[w + 1] = FK_CH_BACKSLASH;
            w = w + 2;
        } else if (c == FK_CH_DQUOTE) {
            fk_srctext[w] = FK_CH_BACKSLASH;
            fk_srctext[w + 1] = FK_CH_DQUOTE;
            w = w + 2;
        } else if (c == FK_CH_LF) {
            fk_srctext[w] = FK_CH_BACKSLASH;
            fk_srctext[w + 1] = FK_CH_LOWER_N;
            w = w + 2;
        } else if (c == FK_CH_CR) {
            /* drop CR */
        } else {
            fk_srctext[w] = c;
            w = w + 1;
        }
        ri = ri + 1;
    }
    const char *end = "\")\n";
    long long ei = 0;
    while (end[ei] != 0) {
        if (w >= cap) {
            return 4;
        }
        fk_srctext[w] = end[ei];
        w = w + 1;
        ei = ei + 1;
    }
    fk_srctext[w] = 0;
    fk_slen = w;
    fk_spos = 0;

    /* same parse+walk pipeline as fk_run_src */
    fk_arg_n = 0;
    fk_fname_n = 0;
    fk_node_count = 0;
    fk_bd_top = 0;
    fk_maxslot = 0;
    fk_nerr = 0;
    fk_nwarn = 0;
    fk_src_truncated = 0;
    fk_src_unrunnable = 0;
    fk_sinit();
    fk_fn_reserve(1);
    fk_fn_count = 1;
    fk_fntop = 0;
    if (fk_live_page != 0) { (fk_live_page + 2)[29] = 0; fk_live_blob_used = 0; } fk_f64_reset();
    fk_const_top = 0;
    fk_defn_next = 1;
    fk_root = -1;
    /* twin of fk_src_compile_current_unit's gate: this door parses fk_srctext directly,
     * so it needs the same balance decision or the meta-eval lane stays permissive.
     * Same reason as there for deciding BEFORE the readers run: unbalanced text can
     * spin the top-level loop rather than fail it. */
    fk_src_check_balance();
    if (fk_src_unrunnable) {
        fk_diag_flush();
        fk_heat_report();
        return 1;
    }
    fk_prescan_defns();
    fk_spos = 0;
    while (1) {
        fk_sskip();
        if (fk_spos >= fk_slen) {
            break;
        }
        fk_parse_top();
    }
    if (fk_root >= 0) {
        fk_fn[0] = fk_root;
    } else if (fk_defn_next > 1) {
        fk_fn[0] = fk_fn[fk_defn_next - 1];
    } else {
        fk_fn[0] = fk_smklit(0);
    }
    fk_fn_count = fk_defn_next;
    /* ROOT SCOPE FIX: a defn body's lets are protected by a tag-111 reserve
     * (fk_maxslot slots raised above fk_vsp before the body runs), but the
     * bare top-level root never got one — its lets were handed slots in
     * fk_vs[fp+1..] while fk_vsp stayed at fp+1, so the FIRST nested call's
     * frame (pushed at fk_vsp) landed on top of the live top-level bindings
     * and silently overwrote them (receipts/2026-07-01-node-children-last-
     * writer-wins.md: the bare-top-level exposure). Thanks to the parse-time
     * save/restore ported tonight (f99d3232), fk_maxslot at this point holds
     * exactly the ROOT scope's own slot count — so give the root the same
     * reservation every defn body already gets. */
    if (fk_maxslot > 0) {
        fk_fn[0] = fk_smknode(111, fk_smklit(fk_maxslot), fk_fn[0], 0);
    }
    fk_vs[0] = 0;
    fk_vsp = 1;
    /* ── PARSE DONE, EXECUTION BEGINS ── gcc-style tally (twin of fk_run_src). */
    fk_diag_flush();
    if (fk_src_unrunnable) {
        fk_heat_report();
        return 1;
    }
    long long rv = fk_walk(fk_fn[0], 0);
    fk_pv(rv);
    /* print the meta-eval result by value-kind (int / float / nothing) */
    fk_heat_report();
    return (fk_nerr > 0 || fk_nerr_seen > 0) ? 1 : 0;
}
/* STAGE argv's trailing token into fk_src, the buffer `input_byte` (tag 17) reads.
 *
 * fk_src_len was assigned NOWHERE but its initializer, so the staged-input buffer was
 * always empty and every input_byte returned 0. form-cli-main.fk's headless front door
 * is exactly `fc-read` over input_byte, so the form-cli chain could not receive a command
 * from this seed at all — its header names the filler as fkwu's argv[3] "or the persistent
 * fkwu-server's per-request buffer (form-kernel-go/fkwu_bridge.go)", and that Go bridge
 * lives in the origin repo, not here. This is roadmap item 4 (MANIFEST.md) at its exact
 * location: the binary RAN Go-free while the way IN was still Go-shaped.
 *
 * This is host plumbing, not runtime meaning: Form cannot reach argv, and the Form-side
 * primitive (input_byte) already exists — only the fill was missing. It does not displace
 * the integer arg: atoi still runs on the same token, so `ground-recursive.fk 10` keeps
 * its 55 (atoi of a verb like "ping" is 0, which is the arg such a program would get
 * anyway). Shrink note: this leaves when Form owns its own argv port. */
static void fk_stage_input(const char *s) {
    long long i = 0;
    long long n = 0;
    if (!s) { fk_src_len = 0; return; }
    while (s[n] != 0) { n = n + 1; }
    if (n + 1 > fk_src_cap) {
        long long nc = fk_src_cap == 0 ? FK_STAGED_INPUT_CAP_INIT : fk_src_cap;
        char *q;
        while (nc < n + 1) { nc = nc * 2; }
        q = realloc(fk_src, (unsigned long)nc);
        if (q == 0) { fk_die("fk_stage_input: out of memory growing the staged-input buffer"); }
        fk_src = q;
        fk_src_cap = nc;
    }
    while (i < n) { fk_src[i] = s[i]; i = i + 1; }
    fk_src[i] = 0;
    fk_src_len = i;
}
/* ── the BML floor ──────────────────────────────────────────────────────
 * High-grammar .bml is a first-class source: the runner lowers it through
 * the body's own Form compiler (spawning ITSELF on
 * form/form-stdlib/bml-floor-compile.fk — the chain stays Form-owned; this
 * door only checks freshness and opens it) into a derived <x>.bml.fk
 * beside the source, which then rides the ordinary .fk lane and its .fkb
 * cache: warm runs are native speed, and no crystallized twin lives in
 * the tree (Urs, 2026-08-30: xtal is the wrong shape; high-grammar BML
 * with an optimal cached native-speed compiler is the floor). Derived
 * .bml.fk, .bml.fkb and .bml.sym files are cache artifacts, gitignored.
 * Shrink direction: this door retires when the runner's entry self-hosts. */
static const char *fk_self_path = "./fkwu";
/* lower a .bml entirely in memory: the child prints the lowered text (its
 * // preludes: line carried) closed by a sentinel; the parent captures it
 * from the pipe. No derived source file is ever created. Returns a
 * malloc'd NUL-terminated buffer (caller frees) or 0. */
/* LOWERED-TEXT MEMO. A lowering self-spawn pays the floor compiler's whole
 * chain (~1.6s warm, measured 2026-08-31); six .bml deps made every glass
 * run — cold OR warm — pay ~10s of spawns. A JIT refusal is a stone to
 * place, not a tax to keep: the lowered text is memoized beside the source
 * as <x>.bml.lowfk, keyed by the RAW .bml bytes AND a digest of the floor
 * compiler's own chain (bml-floor-compile.fk plus every file on its
 * preludes line), so an edit to either the surface or the compiler
 * invalidates honestly. A hit is a read; a miss spawns once and writes.
 * Named deeper stones, not placed here: the floor compiler resident
 * in-process (no spawn even on miss), and image-load latency itself. */
static unsigned long long fk_bml_floor_digest_memo;
static int fk_bml_floor_digest_have;
static char *fk_read_whole_file(const char *path, long long *out_len) {
#if defined(_WIN32)
    int fd = open(path, 0x8000);
#else
    int fd = open(path, 0);
#endif
    long long cap = 1 << 20;
    long long n = 0;
    char *buf;
    if (fd < 0) {
        return 0;
    }
    buf = malloc((unsigned long)cap);
    if (buf == 0) {
        close(fd);
        return 0;
    }
    for (;;) {
        long long got = (long long)read(fd, buf + n, (unsigned long)(cap - n));
        if (got <= 0) {
            break;
        }
        n = n + got;
        if (n == cap) {
            cap = cap * 2;
            buf = realloc(buf, (unsigned long)cap);
            if (buf == 0) {
                close(fd);
                return 0;
            }
        }
    }
    close(fd);
    *out_len = n;
    return buf;
}

/* A unit travels through the lowering lane when its file wears .bml or when
 * its text carries a `section [` block on a line of its own -- the dialect
 * blocks (form.bml, form.lift, form.action, form.route, *.bmf) that only the
 * source compiler reads. Keyed on content, not extension: compiler.fk and the
 * ten -bmf.fk grammars carry such blocks mid-file, and every chain that
 * preluded them raw died on `::=` as an unbound name (2026-09-04, 131 chains
 * on compiler.fk alone). */
static int fk_unit_lowers(const char *path) {
    long long n = 0;
    char *text;
    long long i = 0;
    int found = 0;
    if (fk_path_has_suffix(path, ".bml")) {
        return 1;
    }
    text = fk_read_whole_file(path, &n);
    if (text == 0) {
        return 0;
    }
    while (i < n && !found) {
        while (i < n && (text[i] == FK_CH_SPACE || text[i] == FK_CH_TAB)) {
            i = i + 1;
        }
        if (i + 9 <= n && text[i] == 's' && text[i + 1] == 'e' && text[i + 2] == 'c' &&
            text[i + 3] == 't' && text[i + 4] == 'i' && text[i + 5] == 'o' &&
            text[i + 6] == 'n' && text[i + 7] == FK_CH_SPACE && text[i + 8] == '[') {
            found = 1;
        }
        while (i < n && text[i] != FK_CH_LF) {
            i = i + 1;
        }
        i = i + 1;
    }
    free(text);
    return found;
}
static int fk_hex16_parse(const char *p, unsigned long long *out) {
    unsigned long long h = 0;
    long long i = 0;
    while (i < 16) {
        char c = p[i];
        if (c >= '0' && c <= '9') {
            h = (h << 4) | (unsigned long long)(c - '0');
        } else if (c >= 'a' && c <= 'f') {
            h = (h << 4) | (unsigned long long)(c - 'a' + 10);
        } else {
            return 0;
        }
        i = i + 1;
    }
    *out = h;
    return 1;
}
/* The floor digest walks the compiler's RECURSIVE prelude closure. A
 * one-level walk was byteseal's own gap, found by the field within
 * hours: a semicolon fix in a second-level floor dep left every memo
 * key unchanged, and the memos replayed a pre-fix broken lowering of a
 * sibling's surface (fcpclb-* defs truncated away, 2026-09-01). Every
 * file the floor compile would load is folded in; a visited list keeps
 * the walk finite. */
#define FK_FLOOR_DEP_CAP_INIT 128 /* floor-chain dep rows birth size; grows -- a dep past the old fixed 128 was SILENTLY skipped by the digest fold, so a stale .lowfk memo could ride as fresh when that dep changed */
static char (*fk_floor_seen)[FK_PATH_CAP];
static long long fk_floor_cap;
static long long fk_floor_seen_n;
static int fk_floor_seen_has(const char *p) {
    long long i = 0;
    while (i < fk_floor_seen_n) {
        if (!memcmp(fk_floor_seen[i], p, fk_path_len(p) + 1)) {
            return 1;
        }
        i = i + 1;
    }
    return 0;
}
static unsigned long long fk_bml_floor_fold(const char *path, unsigned long long h) {
    long long n = 0;
    char *text;
    long long i;
    if (fk_floor_seen_has(path)) {
        return h;
    }
    if (fk_floor_seen_n >= fk_floor_cap) {
        long long nc = fk_floor_cap == 0 ? FK_FLOOR_DEP_CAP_INIT : fk_floor_cap * 2;
        fk_floor_seen = (char (*)[FK_PATH_CAP])realloc(fk_floor_seen, (unsigned long)(nc * FK_PATH_CAP));
        if (fk_floor_seen == 0) {
            fk_die("fk_bml_floor_fold: out of memory growing the floor dep table");
        }
        fk_floor_cap = nc;
    }
    fk_cstr_copy(fk_floor_seen[fk_floor_seen_n], path, FK_PATH_CAP);
    fk_floor_seen_n = fk_floor_seen_n + 1;
    text = fk_read_whole_file(path, &n);
    if (text == 0) {
        return h;
    }
    h = h * 1099511628211ULL;
    h = h ^ fk_bytes_fnv1a(text, n);
    for (i = 0; i + 11 < n; i = i + 1) {
        if (text[i] == ';' && !memcmp(text + i, "; preludes:", 11)) {
            long long p = i + 11;
            while (p < n && text[p] != FK_CH_LF) {
                long long start;
                while (p < n && (text[p] == FK_CH_SPACE || text[p] == FK_CH_TAB)) {
                    p = p + 1;
                }
                start = p;
                while (p < n && text[p] != FK_CH_SPACE && text[p] != FK_CH_TAB &&
                       text[p] != FK_CH_LF && text[p] != FK_CH_CR) {
                    p = p + 1;
                }
                if (p > start) {
                    char dep_path[FK_PATH_CAP];
                    if (fk_path_resolve_fk_dep(path, text + start, p - start,
                                               dep_path, FK_PATH_CAP)) {
                        h = fk_bml_floor_fold(dep_path, h);
                    }
                }
            }
        }
    }
    free(text);
    return h;
}
static unsigned long long fk_bml_floor_digest(void) {
    if (fk_bml_floor_digest_have) {
        return fk_bml_floor_digest_memo;
    }
    fk_floor_seen_n = 0;
    fk_bml_floor_digest_memo =
        fk_bml_floor_fold("form/form-stdlib/bml-floor-compile.fk",
                          14695981039346656037ULL);
    fk_bml_floor_digest_have = 1;
    return fk_bml_floor_digest_memo;
}
static char *fk_bml_low_memo_read(const char *bml_path, unsigned long long raw_h,
                                  unsigned long long floor_h, long long *out_len) {
    char memo_path[4300];
    long long n = 0;
    char *text;
    unsigned long long got_raw = 0, got_floor = 0;
    long long head = 0;
    char *body;
    sprintf(memo_path, "%s.lowfk", bml_path);
    text = fk_read_whole_file(memo_path, &n);
    if (text == 0) {
        return 0;
    }
    if (n < 41 || memcmp(text, "fklow1 ", 7) != 0 ||
        !fk_hex16_parse(text + 7, &got_raw) ||
        !fk_hex16_parse(text + 24, &got_floor)) {
        free(text);
        return 0;
    }
    while (head < n && text[head] != FK_CH_LF) {
        head = head + 1;
    }
    head = head + 1;
    if (got_raw != raw_h || got_floor != floor_h || head > n) {
        free(text);
        return 0;
    }
    body = malloc((unsigned long)(n - head + 1));
    if (body == 0) {
        free(text);
        return 0;
    }
    memcpy(body, text + head, (unsigned long)(n - head));
    body[n - head] = 0;
    *out_len = n - head;
    free(text);
    return body;
}
static void fk_bml_low_memo_write(const char *bml_path, unsigned long long raw_h,
                                  unsigned long long floor_h, const char *low,
                                  long long low_len) {
    char memo_path[4300];
    int fd;
    sprintf(memo_path, "%s.lowfk", bml_path);
    fd = open(memo_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        return;
    }
    dprintf(fd, "fklow1 %016llx %016llx\n", raw_h, floor_h);
    write(fd, low, (unsigned long)low_len);
    close(fd);
}
static char *fk_bml_lower_spawn(const char *bml_path, long long *out_len);
static char *fk_bml_lower_to_mem(const char *bml_path, long long *out_len) {
    long long raw_n = 0;
    char *raw = fk_read_whole_file(bml_path, &raw_n);
    unsigned long long raw_h, floor_h;
    char *hit;
    char *low;
    if (raw == 0) {
        fk_diag_path("error", bml_path, "bml source is not readable");
        return 0;
    }
    raw_h = fk_bytes_fnv1a(raw, raw_n);
    free(raw);
    floor_h = fk_bml_floor_digest();
    hit = fk_bml_low_memo_read(bml_path, raw_h, floor_h, out_len);
    if (hit != 0) {
        return hit;
    }
    low = fk_bml_lower_spawn(bml_path, out_len);
    if (low != 0) {
        fk_bml_low_memo_write(bml_path, raw_h, floor_h, low, *out_len);
    }
    return low;
}
static char *fk_bml_lower_spawn(const char *bml_path, long long *out_len) {
#if defined(_WIN32)
    fk_diag_path("error", bml_path,
                 "bml lowering via self-spawn is not wired on this platform yet");
    return 0;
#else
    int in_fds[2];
    int out_fds[2];
    if (pipe(in_fds) != 0 || pipe(out_fds) != 0) {
        fk_diag_path("error", bml_path, "bml lowering could not open pipes");
        return 0;
    }
    long long pid = fork();
    if (pid < 0) {
        close(in_fds[0]);
        close(in_fds[1]);
        close(out_fds[0]);
        close(out_fds[1]);
        fk_diag_path("error", bml_path, "bml lowering could not fork");
        return 0;
    }
    if (pid == 0) {
        close(in_fds[1]);
        close(out_fds[0]);
        dup2(in_fds[0], 0);
        dup2(out_fds[1], 1);
        close(in_fds[0]);
        close(out_fds[1]);
        char *child_argv[3];
        child_argv[0] = (char *)fk_self_path;
        child_argv[1] = (char *)"form/form-stdlib/bml-floor-compile.fk";
        child_argv[2] = 0;
        execvp(fk_self_path, child_argv);
        _exit(127);
    }
    close(in_fds[0]);
    close(out_fds[1]);
    {
        char line[4300];
        int m = sprintf(line, "%s\n-\n", bml_path);
        long long off = 0;
        while (off < m) {
            long long put = write(in_fds[1], line + off, (unsigned long)(m - off));
            if (put <= 0) {
                break;
            }
            off = off + put;
        }
    }
    close(in_fds[1]);
    long long cap = 65536;
    long long got = 0;
    char *buf = malloc((unsigned long)cap);
    if (buf == 0) {
        fk_die("fk_bml_lower_to_mem: out of memory");
    }
    for (;;) {
        if (got + 4096 >= cap) {
            cap = cap * 2;
            char *grown = malloc((unsigned long)cap);
            if (grown == 0) {
                fk_die("fk_bml_lower_to_mem: out of memory growing");
            }
            long long i = 0;
            while (i < got) {
                grown[i] = buf[i];
                i = i + 1;
            }
            free(buf);
            buf = grown;
        }
        long long r = read(out_fds[0], buf + got, 4096);
        if (r <= 0) {
            break;
        }
        got = got + r;
    }
    close(out_fds[0]);
    {
        int st = 0;
        waitpid((int)pid, &st, 0);
        if (st != 0) {
            free(buf);
            fk_diag_path("error", bml_path,
                    "bml lowering child failed; run form/form-stdlib/bml-floor-compile.fk by hand to see its diagnostics");
            return 0;
        }
    }
    buf[got] = 0;
    {
        const char *sentinel = "\n@bml-floor-lowered\n";
        long long sn = 20;
        long long at = -1;
        long long i = 0;
        while (i + sn <= got) {
            long long j = 0;
            while (j < sn && buf[i + j] == sentinel[j]) {
                j = j + 1;
            }
            if (j == sn) {
                at = i;
                break;
            }
            i = i + 1;
        }
        if (at < 0) {
            free(buf);
            fk_diag_path("error", bml_path, "bml lowering returned no sentinel-closed text");
            return 0;
        }
        buf[at] = '\n';
        buf[at + 1] = 0;
        *out_len = at + 1;
    }
    return buf;
#endif
}
/* run a .bml as itself, filelessly: warm runs load <x>.bml.fkb through the
 * SAME content-checked door the .fk lane uses — the identity is computed
 * from a fresh in-memory lowering every run, so a same-second edit, an
 * edited prelude beneath an untouched .bml, or a foreign writer's bytes
 * all refuse the warm image honestly instead of replaying it (mtime gates
 * once cost a 14-minute stale window under a live edit storm, 2026-08-31;
 * the lowering itself is milliseconds and buys byte-true trust). The
 * native cache is the ONLY artifact this lane ever writes. */
static int fk_run_bml(const char *path, long long arg) {
    char fkb_path[4300];
    char sym_path[4300];
    long long n = fk_path_len(path);
    if (n + 5 >= 4300) {
        fk_diag_path("error", path, "bml path exceeds buffer");
        return 2;
    }
    sprintf(fkb_path, "%s.fkb", path);
    sprintf(sym_path, "%s.sym", path);
    long long src_m = fk_path_mtime_raw(path);
    if (src_m <= 0) {
        fk_diag_path("error", path, "bml source is missing or not stat-readable");
        return 2;
    }
    long long low_len = 0;
    char *low = fk_bml_lower_to_mem(path, &low_len);
    if (low == 0) {
        return 2;
    }
    char expected_source_hash[FK_SRC_HASH_CAP];
    long long unit_mtime = 0;
    if (!fk_src_load_unit_buffer(path, low, low_len, src_m,
            expected_source_hash, FK_SRC_HASH_CAP, &unit_mtime)) {
        return 2;
    }
    if (fk_path_mtime_raw(fkb_path) > 0) {
        long long recorded = fk_src_sym_recorded_errors(sym_path);
        long long unrunnable = fk_src_sym_recorded_unrunnable(sym_path);
        if (unrunnable > 0) {
            /* an unrunnable mark on the ice may be the floor's own honest
             * refusal or a foreign writer's (an import probe once compiled
             * the raw surface alone and poisoned this exact path). Either
             * way the honest move is the same: re-lower fresh. A .bml that
             * truly refuses will refuse again, loudly, with today's
             * diagnostics instead of a cached tombstone. */
            fk_diag_path("warning", sym_path,
                    "cached image carries a refusal mark; re-lowering fresh");
        } else if (recorded >= 0 &&
                   fk_src_load_fkb_checked(fkb_path, path, expected_source_hash, unit_mtime)) {
            if (recorded > 0) {
                fk_diag_path("warning", sym_path,
                        "cached image was compiled with errors; fix the .bml and rerun to clear");
            }
            fk_run_door = 2;
            int rc = fk_run_loaded_program_image(arg);
            fk_heat_report();
            return recorded > 0 && rc == 0 ? 1 : rc;
        } else {
            fk_diag_path("warning", fkb_path,
                    "bml cache does not match today's bytes; re-lowering");
        }
    }
    fk_src_reset_compile_state();
    fk_src_compile_current_unit(path, fkb_path, sym_path, unit_mtime,
            expected_source_hash);
    fk_vs[0] = arg << 1;
    fk_vsp = 1;
    fk_diag_flush();
    if (fk_src_truncated || fk_src_unrunnable) {
        fk_heat_report();
        return 1;
    }
    fk_pv_root(fk_walk(fk_fn[0], 0));
    fk_heat_report();
    return (fk_nerr > 0 || fk_nerr_seen > 0) ? 1 : 0;
}
static int fk_run(int argc, char **argv) {
    char fk_stack_here;
    fk_stack_base = &fk_stack_here;
    fk_nodes_init();
    if (argc < 2) {
        return 1;
    }
    if (argv[0] && argv[0][0]) {
        fk_self_path = argv[0];
    }
    if (argc > 3 && argv[1][0] == FK_CH_DASH && argv[1][1] == FK_CH_DASH) {
        fk_stage_input(argv[3]);
    } else if (argc > 2) {
        fk_stage_input(argv[2]);
    }
    if (argc >= 3 && argv[1][0] == FK_CH_DASH && argv[1][1] == FK_CH_DASH &&
        argv[1][2] == FK_CH_LOWER_F && argv[1][3] == FK_CH_LOWER_E) {
        return fk_run_feval(argv[2]);
    }
    if (argc >= 3 && argv[1][0] == FK_CH_DASH && argv[1][1] == FK_CH_DASH) {
        return fk_run_src(argv[2], argc > 3 ? atoi(argv[3]) : 0);
    }
    if (fk_path_has_suffix(argv[1], ".fk")) {
        if (fk_unit_lowers(argv[1])) {
            return fk_run_bml(argv[1], argc > 2 ? atoi(argv[2]) : 0);
        }
        return fk_run_src(argv[1], argc > 2 ? atoi(argv[2]) : 0);
    }
    if (fk_path_has_suffix(argv[1], ".bml")) {
        return fk_run_bml(argv[1], argc > 2 ? atoi(argv[2]) : 0);
    }
    if (fk_path_has_suffix(argv[1], ".fkb")) {
        if (!fk_src_load_fkb(argv[1])) {
            char whybuf[192];
            sprintf(whybuf, "could not load .fkb program image (%s)",
                    fk_fkb_bad ? fk_fkb_bad_why : "unknown decode failure");
            fk_diag_path("error", argv[1], whybuf);
            return 2;
        }
        long long recorded = 0;
        char direct_sym_path[FK_PATH_CAP];
        long long fkb_arg_len = fk_path_len(argv[1]);
        /* fk_path_replace_ext only strips a trailing ".fk"; this argument ends
         * in ".fkb" (checked above), so swap the suffix explicitly */
        if (fkb_arg_len >= 4 && fkb_arg_len < 4090) {
            sprintf(direct_sym_path, "%.*s.sym", (int)(fkb_arg_len - 4), argv[1]);
            recorded = fk_src_sym_recorded_errors(direct_sym_path);
            if (recorded > 0) {
                fk_diag_path("warning", direct_sym_path,
                        "image was compiled with errors; fix source and rerun --src to clear");
            } else if (recorded < 0) {
                /* direct execution has no source to rebuild from; run, but say
                 * the record is missing rather than imply a clean compile */
                fk_diag_path("warning", direct_sym_path,
                        "image carries no compile-error record");
                recorded = 0;
            }
        }
        fk_run_door = 2;
        int fkb_rc = fk_run_loaded_program_image(argc > 2 ? atoi(argv[2]) : 0);
        return recorded > 0 && fkb_rc == 0 ? 1 : fkb_rc;
    }
    if (fk_path_has_suffix(argv[1], ".dylib")) {
        fk_run_door = 3;
        return fk_run_dylib_artifact(argv[1], argc > 2 ? atoi(argv[2]) : 0, 1) ? 0 : 2;
    }
    if (fk_path_has_suffix(argv[1], ".tbl")) {
        fk_diag_path("error", argv[1], ".tbl execution has been retired; use .fk, .fkb, or .dylib");
        return 2;
    }
    fk_diag_path("error", argv[1], "unsupported file extension; supported: .fk .fkb .dylib");
    return 2;
}
#if defined(_WIN32)
/* the same law as the POSIX main below: the walker runs on a big explicit thread stack
 * (FORM_KERNEL_STACK_MB, default 256MB). The bare `return fk_run(...)` ran on the OS
 * default 1MB and died silently (exit 127, no output) at ~120 recursion levels — the
 * platform seam the 2026-07-01 depth-wall repairs were patching around, healed at its
 * root. 0x00010000 = STACK_SIZE_PARAM_IS_A_RESERVATION. */
extern int atoi(const char *);
static int fk_run_argc_w;
static char **fk_run_argv_w;
static int fk_run_ret_w;
static unsigned int fk_run_thunk_w(void *p) {
    (void)p;
    fk_run_ret_w = fk_run(fk_run_argc_w, fk_run_argv_w);
    fk_src_dep_release();
    return 0;
}
int main(int argc, char **argv) {
    fk_run_argc_w = argc;
    fk_run_argv_w = argv;
    unsigned long long mb = 256;
    char *e = fk_conf("FORM_KERNEL_STACK_MB");
    if (e) {
        int v = atoi(e);
        if (v > 0) {
            mb = (unsigned long long)v;
        }
    }
    fk_stack_wall = (long long)mb * 1024 * 1024 - 2 * 1024 * 1024;
    void *th = CreateThread((void *)0, mb * 1024ULL * 1024ULL, fk_run_thunk_w, (void *)0,
                            0x00010000u, (unsigned int *)0);
    if (th == 0) {
        fk_stack_wall = 6 * 1024 * 1024;
        int rc = fk_run(argc, argv);
        fk_src_dep_release();
        return rc;
    }
    WaitForSingleObject(th, 0xFFFFFFFFu);
    CloseHandle(th);
    return fk_run_ret_w;
}
#else
extern char *getenv(const char *);
typedef void *fk_pthread_t;
typedef struct {
    long fk_pa_sig;
    char fk_pa_opaque[64];
} fk_pthread_attr_t;
extern int pthread_attr_init(fk_pthread_attr_t *);
extern int pthread_attr_setstacksize(fk_pthread_attr_t *, unsigned long);
extern int pthread_create(fk_pthread_t *, const fk_pthread_attr_t *, void *(*)(void *), void *);
extern int pthread_join(fk_pthread_t, void **);
static int fk_run_argc;
static char **fk_run_argv;
static int fk_run_ret;
static void *fk_run_thunk(void *p) {
    (void)p;
    fk_run_ret = fk_run(fk_run_argc, fk_run_argv);
    fk_src_dep_release();
    return 0;
}
int main(int argc, char **argv) {
    fk_run_argc = argc;
    fk_run_argv = argv;
    unsigned long mb = 256;
    char *e = fk_conf("FORM_KERNEL_STACK_MB");
    if (e) {
        int v = atoi(e);
        if (v > 0) {
            mb = (unsigned long)v;
        }
    }
    fk_stack_wall = (long long)mb * 1024 * 1024 - 2 * 1024 * 1024;
    fk_pthread_attr_t at;
    pthread_attr_init(&at);
    pthread_attr_setstacksize(&at, mb * 1024UL * 1024UL);
    fk_pthread_t th;
    if (pthread_create(&th, &at, fk_run_thunk, 0) != 0) {
        int rc = fk_run(argc, argv);
        fk_src_dep_release();
        return rc;
    }
    pthread_join(th, 0);
    return fk_run_ret;
}
#endif
