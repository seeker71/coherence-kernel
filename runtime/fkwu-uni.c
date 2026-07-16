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
static long long fk_nerr;        /* errors diagnosed this run   */
static long long fk_nwarn;       /* warnings diagnosed this run */
static int fk_src_truncated;     /* 1 if the source was amputated at FK_SOURCE_TEXT_CAP */
/* fk_diag / fk_diag_flush are DEFINED further down (right after fk_srctext /
 * fk_spos / fk_slen are declared), where they can read the source buffer. */
static void fk_diag(int sev, long long off, const char *fmt, ...);
static void fk_diag_flush(void);
/* Named capacities for the seed's fixed-size tables. Several numerically coincide
 * (many independent tables happen to be sized 65536) but are named SEPARATELY on
 * purpose: they are different index spaces (the node/AST table, the value stack,
 * and the staged-input buffer are three unrelated capacities that must never be
 * conflated under one name, or a future resize of one would silently misresize
 * the others). Where a mask (`& N-1`) stood in for a bound check, the mask is
 * rewritten in terms of the same named constant so the two can never drift apart. */
#define FK_FLOAT_POOL_INIT_CAP 65536    /* fk_fv: boxed-float pool, initial size (doubles on demand) */
#define FK_OPCODE_ARM_CAP 256           /* fk_arms: per-tag hit counters, indexed by node tag t */
#define FK_MEM_CELL_CAP 4096            /* fk_mem: mutable record-cell table (tags 13/14) */
#define FK_STAGED_INPUT_CAP 262144      /* fk_src: staged auxiliary input (the input_byte primitive) */
#define FK_VALUE_STACK_CAP 65536        /* fk_vs: the evaluator's argument/value stack */
#define FK_STRING_POOL_INIT_BYTES 1048576 /* fk_sb: interned-string byte pool, initial size */
#define FK_STRING_TABLE_INIT_CAP 16384  /* fk_so/fk_sl: interned-string table, initial entry count */
/* NOTE: FK_NODE_CAP and FK_AST_NODE_CAP are DIFFERENT tables that happen to share
 * a capacity today -- never conflate them. FK_NODE_CAP is the hash-cons'd VALUE
 * table (fk_nkind, ncat, nkids, nval, nid, nsfile, nsline, nscol, nsattr, fbroots)
 * used by fk_neq/fk_veq for structural equality on cons'd runtime values (records,
 * lists). FK_AST_NODE_CAP (defined near fk_node[][4] itself, further down) is the
 * PARSED PROGRAM's syntax tree, filled once per expression during parsing via
 * fk_smknode. */
#define FK_NODE_CAP 262144              /* fk_nkind, ncat, nkids, nval, nid, nsfile, nsline, nscol, nsattr, fbroots. Raised 65536->262144 (2026-07-02): a 1,200-clip --src program filled the value-node table mid-run and every guard silently returned handle 0 -- a deterministic all-zero result with no error. Same raisable-constant class as FK_AST_NODE_CAP; overflow now dies loudly instead of returning 0. 262144*104B ~= 27MB. */
#define FK_RECORD_CAP 256               /* fk_rkey/rval/rcnt/rbp: max live mutable records (fk_rp bound) */
#define FK_RECORD_MAX_KEYS 128          /* fk_rkey/rval second dimension: max keys per record */
/* fn-value reserved band (see stone 2c below): the band WIDTH (8192, in raw
 * sentinel-space units before the <<1) is intentionally wider than the actual
 * valid index CAP (4096) -- headroom already present in the original design, not
 * a bug; named as two distinct constants so that relationship stays visible
 * rather than reading as two unrelated bare numbers. */
#define FK_FNVAL_BAND_WIDTH 8192
#define FK_FNVAL_MAX_INDEX 4096
/* ASCII byte constants for the text-processing code (the parser's own character
 * classification, and the OS-layer's path/URL splitting) -- NOT used in the
 * evaluator's `if (t == N)` opcode-tag dispatch, which is a completely
 * different numbering space (generated from fkwu-optable.h) that happens to
 * share small integer values with ASCII codes in the 0-127 range. Conflating
 * the two would be the same class of mistake as merging FK_NODE_CAP and
 * FK_AST_NODE_CAP; kept strictly to genuine byte/character comparisons. */
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
 * v<=fk_fbase-2; this is fk_fbase+1, so isf is false), not a node (fk_nidx maps it to ~4.5e18, far
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
 * not 0/1, not a float (fk_isf needs v<=fk_fbase-2 ~ -9e18; these are ~-8e18, ABOVE it), not a node
 * (fk_nidx maps ~8e18 far past fk_np), not a record ((0-v) is odd here, records even), not nothing
 * (distinct constant). A bare fn-name in value position evaluates to this (tag 243); an indirect
 * call offers the fn it names (tag 244). CLOSURE is the NAMED next gap: the fn-value carries only
 * the fn-index, no captured env-cell yet. */
static const long long fk_fnbase = -8000000000000000000LL;
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
    if (v <= fk_fnbase - (FK_FNVAL_BAND_WIDTH << 1)) {
        return 0;
    }
    if (((fk_fnbase - v) & 1) == 0) {
        return 0;
    }
    long long fi = fk_fnval_idx(v);
    return (fi >= 0 && fi < FK_FNVAL_MAX_INDEX) ? 1 : 0;
}
static long long fk_fidx(long long v) {
    return (fk_fbase - v) >> 1;
}
static long long fk_isf(long long v) {
    long long fi = fk_fidx(v);
    return v <= fk_fbase - 2 && fi > 0 && fi <= fk_fp;
}
static double fk_num(long long v) {
    if (fk_isf(v)) {
        return fk_fv[fk_fidx(v)];
    }
    return (double)(v >> 1);
}
static long long fk_fbox(double d) {
    if (fk_fv == 0) {
        fk_fcap = FK_FLOAT_POOL_INIT_CAP;
        fk_fv = malloc(fk_fcap * 8);
        if (fk_fv == 0) {
            fk_die("fk_fbox: out of memory");
        }
    }
    fk_fp = fk_fp + 1;
    if (fk_fp >= fk_fcap) {
        fk_fcap = fk_fcap * 2;
        fk_fv = realloc(fk_fv, fk_fcap * 8);
        if (fk_fv == 0) {
            fk_die("fk_fbox: out of memory growing float pool");
        }
    }
    fk_fv[fk_fp] = d;
    return fk_fbase - (fk_fp << 1);
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
static void fk_pv(long long v) {
    if (v == fk_nothing) {
        printf("nothing\n");
        return;
    }
    if (fk_isf(v)) {
        printf("%.15g\n", fk_num(v));
    } else {
        if ((v & 1) == 0) {
            fk_pr(v >> 1);
        } else {
            fk_pr(v);
        }
    }
}
static long long fk_arms[FK_OPCODE_ARM_CAP];
static long long fk_mem[FK_MEM_CELL_CAP];
static char fk_src[FK_STAGED_INPUT_CAP];
static long long fk_src_len = 0;
static long long *fk_hh;
static long long *fk_ht;
static long long fk_hp;
static long long fk_cap;
static long long fk_vs[FK_VALUE_STACK_CAP];
static long long fk_vsp;
extern long long time(long long *);
extern unsigned int arc4random(void);
extern void *malloc(unsigned long);
extern void *calloc(unsigned long, unsigned long);
extern void free(void *);
extern long long write(long long, const void *, unsigned long);
extern double strtod(const char *, char **);
extern void *popen(const char *, const char *);
extern int pclose(void *);
extern unsigned long fread(void *, unsigned long, unsigned long, void *);
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
static void fk_sinit(void) {
    if (fk_sb == 0) {
        fk_scap_b = FK_STRING_POOL_INIT_BYTES;
        fk_scap_s = FK_STRING_TABLE_INIT_CAP;
        fk_sb = malloc(fk_scap_b);
        fk_so = malloc(fk_scap_s * 8);
        fk_sl = malloc(fk_scap_s * 8);
        fk_snext = malloc(fk_scap_s * 8);
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
        if (fk_sl[c] == len) {
            long long j = 0;
            while (j < len && fk_sb[fk_so[c] + j] == fk_sb[off + j]) {
                j = j + 1;
            }
            if (j == len) {
                return c;
            }
        }
        c = fk_snext[c];
    }
    long long i = fk_sp;
    if (i >= fk_scap_s) {
        fk_scap_s = fk_scap_s * 2;
        fk_so = realloc(fk_so, fk_scap_s * 8);
        fk_sl = realloc(fk_sl, fk_scap_s * 8);
        fk_snext = realloc(fk_snext, fk_scap_s * 8);
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
static long long fk_nkind[FK_NODE_CAP];
static long long fk_ncat[FK_NODE_CAP];
static long long fk_nkids[FK_NODE_CAP];
static long long fk_nval[FK_NODE_CAP];
static long long fk_nid[FK_NODE_CAP][4];
static long long fk_np;
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
        return fk_nval[ia] == fk_nval[ib];
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
static long long fk_veq(long long a, long long b) {
    if (a == b) {
        return 1;
    }
    if (a < 0 || b < 0) {
        return fk_neq(a, b);
    }
    if ((a & 1) && (b & 1)) {
        long long pa = a >> 1;
        long long pb = b >> 1;
        if (pa < 1 || pa > fk_hp || pb < 1 || pb > fk_hp) {
            return 0;
        }
        if (fk_veq(fk_hh[pa], fk_hh[pb]) == 0) {
            return 0;
        }
        return fk_veq(fk_ht[pa], fk_ht[pb]);
    }
    return 0;
}
static long long fk_nsfile[FK_NODE_CAP];
static long long fk_nsline[FK_NODE_CAP];
static long long fk_nscol[FK_NODE_CAP];
static long long fk_nsattr[FK_NODE_CAP];
static long long fk_fbroots[FK_NODE_CAP];
static long long fk_fbn;
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
extern int rmdir(const char *);
extern int unlink(const char *);
extern int rename(const char *, const char *);
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
#define FK_CONF_MAX 64
static char fk_conf_k[FK_CONF_MAX][64];
static char fk_conf_v[FK_CONF_MAX][256];
static int fk_conf_n = 0;
static int fk_conf_loaded = 0;
static void fk_conf_load(void) {
    if (fk_conf_loaded) { return; }
    fk_conf_loaded = 1;
    int fd = open("fkwu.conf", 0);
    if (fd < 0) { return; }
    static char cb[8192];
    long long n = read(fd, cb, 8191);
    close(fd);
    if (n <= 0) { return; }
    cb[n] = 0;
    long long i = 0;
    while (i < n && fk_conf_n < FK_CONF_MAX) {
        while (i < n && (cb[i] == ' ' || cb[i] == '\t' || cb[i] == '\n' || cb[i] == '\r')) { i = i + 1; }
        if (i >= n) { break; }
        if (cb[i] == '#') { while (i < n && cb[i] != '\n') { i = i + 1; } continue; }
        int kj = 0;
        while (i < n && cb[i] != ' ' && cb[i] != '\t' && cb[i] != '=' && cb[i] != '\n' && cb[i] != '\r' && kj < 63) {
            fk_conf_k[fk_conf_n][kj] = cb[i]; kj = kj + 1; i = i + 1;
        }
        fk_conf_k[fk_conf_n][kj] = 0;
        while (i < n && (cb[i] == ' ' || cb[i] == '\t' || cb[i] == '=')) { i = i + 1; }
        int vj = 0;
        while (i < n && cb[i] != '\n' && cb[i] != '\r' && vj < 255) {
            fk_conf_v[fk_conf_n][vj] = cb[i]; vj = vj + 1; i = i + 1;
        }
        while (vj > 0 && (fk_conf_v[fk_conf_n][vj - 1] == ' ' || fk_conf_v[fk_conf_n][vj - 1] == '\t')) { vj = vj - 1; }
        fk_conf_v[fk_conf_n][vj] = 0;
        if (vj == 0) { fk_conf_v[fk_conf_n][0] = '1'; fk_conf_v[fk_conf_n][1] = 0; }
        if (kj > 0) { fk_conf_n = fk_conf_n + 1; }
    }
}
/* fk_conf: config-file replacement for getenv on OUR toggles. Returns the value string, or 0 when
 * the key is absent OR set to "0"/"" -- so `if (fk_conf("X"))` is on iff X is present and non-zero,
 * matching the old env-presence semantics, and the FK_JIT `v[0] != '0'` check still holds. */
static char *fk_conf(const char *key) {
    fk_conf_load();
    int i = 0;
    while (i < fk_conf_n) {
        int j = 0;
        while (key[j] != 0 && fk_conf_k[i][j] != 0 && key[j] == fk_conf_k[i][j]) { j = j + 1; }
        if (key[j] == 0 && fk_conf_k[i][j] == 0) {
            char *v = fk_conf_v[i];
            if (v[0] == 0 || (v[0] == '0' && v[1] == 0)) { return 0; }
            return v;
        }
        i = i + 1;
    }
    return 0;
}
static long long fk_rkey[FK_RECORD_CAP][FK_RECORD_MAX_KEYS];
static long long fk_rval[FK_RECORD_CAP][FK_RECORD_MAX_KEYS];
static long long fk_rcnt[FK_RECORD_CAP];
static long long fk_rbp[FK_RECORD_CAP];
static long long fk_rp;
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
static long long fk_cstrlen(const char *s) {
    long long n = 0;
    while (s[n] != 0) {
        n = n + 1;
    }
    return n;
}
static void fk_cstr(long long sv, char *out, long long cap) {
    long long sa = sv >> 1;
    long long n = 0;
    if (sa >= 0 && sa < fk_sp) {
        n = fk_sl[sa];
        if (n > cap - 1) {
            fk_die("fk_cstr: string longer than the destination buffer -- silently truncating would corrupt the path / hostname / port / URL / command / device-name the caller is about to use (every fk_cstr caller is one of these). Raise the caller's buffer if this length is legitimate.");
        }
        long long j = 0;
        while (j < n) {
            out[j] = fk_sb[fk_so[sa] + j];
            j = j + 1;
        }
    }
    out[n] = 0;
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
        fk_scap_b = fk_scap_b * 2;
        fk_sb = realloc(fk_sb, fk_scap_b);
        fk_sb_check();
    }
    long long j = 0;
    while (j < n) {
        fk_sb[fk_sbp + j] = buf[j];
        j = j + 1;
    }
    return fk_sintern(fk_sbp, n) << 1;
}
#define FK_METAL_FIXTURE_UNLINKED (0 - 4611686018427387903LL)
#define FK_METAL_MATVEC_UNLINKED (0 - 4611686018427387902LL)
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
static long long fk_srange(long long sv, const char **ptr, long long *len) {
    long long sa = sv >> 1;
    if (sa < 0 || sa >= fk_sp) {
        *ptr = "";
        *len = 0;
        return 0;
    }
    *ptr = fk_sb + fk_so[sa];
    *len = fk_sl[sa];
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
 * (observe/sense-stream.fk), delete fk_sense_stream + this math; (2) lower the pixel walk via
 * model/form-asm-x64.fk -> a native LOOP (the walk is the Form recipe model/frame-luma.fk;
 * tree-walking it is C-stack-bound, ~60 deep at 1MB, so 307k pixels MUST lower to a loop -- not
 * stay C). The seed then shrinks to the HAL (grab + raw bytes). See
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
/* ── the ONLY host-touch a JIT needs: install lowered bytes -> executable -> call. The JIT proper
 * is Form (model/form-asm* lowers recipe->bytes; observe/jit-decision decides). Pure Form cannot
 * make memory executable (W^X is a hardware/OS thing), so the kernel offers this one tiny HAL
 * carrier — same category as the socket / camera / dlopen carriers. fk_native_call takes a lowered
 * byte image + one arg, makes it callable, and jumps to it. fk_native_call_test feeds it bytes for
 * f(a)=a+1 to WITNESS the carrier; in production the bytes come from form-asm-x64, not from C.
 * There is no C JIT here — only this install+call door. */
#if defined(_WIN32)
extern void *VirtualAlloc(void *, unsigned long long, unsigned long, unsigned long);
extern int VirtualProtect(void *, unsigned long long, unsigned long, unsigned int *);
static long long fk_native_call(const unsigned char *code, long long n, long long arg) {
    void *mem = VirtualAlloc(0, (unsigned long long)n, 0x3000, 0x04);
    /* MEM_COMMIT|RESERVE, PAGE_READWRITE */
    if (mem == 0) {
        return -1;
    }
    long long i = 0;
    while (i < n) {
        ((unsigned char *)mem)[i] = code[i];
        i = i + 1;
    }
    unsigned int old = 0;
    VirtualProtect(mem, (unsigned long long)n, 0x20, &old);
    /* PAGE_EXECUTE_READ */
    long long (*fn)(long long) = (long long (*)(long long))mem;
    return fn(arg);
}
#else
extern void *mmap(void *, unsigned long, int, int, int, long);
extern int mprotect(void *, unsigned long, int);
static long long fk_native_call(const unsigned char *code, long long n, long long arg) {
#if defined(__x86_64__) || defined(__amd64__)
    void *mem = mmap(0, (unsigned long)n, 0x3, 0x1002, -1, 0);
    /* RW, MAP_PRIVATE|MAP_ANON(bsd) */
    if (mem == (void *)-1) {
        return -1;
    }
    long long i = 0;
    while (i < n) {
        ((unsigned char *)mem)[i] = code[i];
        i = i + 1;
    }
    if (mprotect(mem, (unsigned long)n, 0x5) != 0) {
        return -1;
    }
    /* RX */
    long long (*fn)(long long) = (long long (*)(long long))mem;
    return fn(arg);
#else
    (void)code;
    (void)n;
    (void)arg;
    return -1;
#endif
}
#endif
static long long fk_native_call_test(long long arg) {
/* lowered bytes of long long f(long long a){ return a + 1; } — arg1 in RCX (Win64) / RDI (SysV) */
#if defined(_WIN32)
    static const unsigned char code[] = {0x48, 0x89, 0xC8, 0x48, 0x83, 0xC0, 0x01, 0xC3};
/* mov rax,rcx; add rax,1; ret */
#else
    static const unsigned char code[] = {0x48, 0x89, 0xF8, 0x48, 0x83, 0xC0, 0x01, 0xC3};
/* mov rax,rdi; add rax,1; ret */
#endif
    return fk_native_call(code, (long long)sizeof code, arg);
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
    static char d[4096];
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
    if (a < 0 || b < 0 || a >= fk_sp || b >= fk_sp || fk_sl[a] != fk_sl[b]) {
        return 0;
    }
    long long j = 0;
    while (j < fk_sl[a]) {
        if (fk_sb[fk_so[a] + j] != fk_sb[fk_so[b] + j]) {
            return 0;
        }
        j = j + 1;
    }
    return 1;
}
static long long fk_file_mtime(long long pv) {
    static char p[4096];
    fk_cstr(pv, p, 4096);
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
    long long sa = sv >> 1;
    long long from = fromv >> 1;
    long long cls = clsv >> 1;
    if (from < 0) {
        from = 0;
    }
    if (sa < 0 || sa >= fk_sp) {
        return from << 1;
    }
    long long end = from;
    long long n = fk_sl[sa];
    while (end < n && fk_scan_match((unsigned char)fk_sb[fk_so[sa] + end], cls)) {
        end = end + 1;
    }
    return end << 1;
}
static void fk_unlink_segments(char *p) {
    char q[4096];
    long long pl = 0;
    while (p[pl] != 0) {
        pl = pl + 1;
    }
    /* same danger class as fk_path_join above: this path feeds unlink(), so a
     * silently truncated "safe" sprintf could delete the wrong file. p's own
     * bound isn't otherwise enforced by every caller, so check it here too. */
    if (pl + 20 > 4096) {
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
    while (q >= 1 && q <= fk_hp) {
        long long es = fk_hh[q];
        static char nb[512];
        fk_cstr(es, nb, 512);
        if (nb[0] != 0 && fk_name_eq(nb, name)) {
            return 1;
        }
        q = fk_ht[q] >> 1;
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
static long long fk_list_push(long long acc, long long sv) {
    if (fk_hp + 1 >= fk_cap) {
        fk_die("fk_list_push: heap exhausted building list -- returning the accumulator unchanged would silently drop this element, a partial list accepted as whole.");
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
static long long fk_ls_buf[512];
static long long fk_ls_n = 0;
static void fk_ls_reset(void) {
    fk_ls_n = 0;
}
static void fk_ls_add(long long sv) {
    if (fk_ls_n < 512) {
        fk_ls_buf[fk_ls_n] = sv;
        fk_ls_n = fk_ls_n + 1;
    }
}
static int fk_sv_less(long long a, long long b) {
    long long aa = a >> 1;
    long long bb = b >> 1;
    if (aa < 0 || bb < 0 || aa >= fk_sp || bb >= fk_sp) {
        return 0;
    }
    long long la = fk_sl[aa];
    long long lb = fk_sl[bb];
    long long j = 0;
    while (j < la && j < lb) {
        unsigned char ca = (unsigned char)fk_sb[fk_so[aa] + j];
        unsigned char cb = (unsigned char)fk_sb[fk_so[bb] + j];
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
/* fk_rmtree and fk_inv_walk both built a child path via sprintf(buf, "%s/%s", dir,
 * e->d_name) into a fixed 4096-byte stack buffer, with no check that dir + '/' +
 * d_name actually fits -- both are RECURSIVE (the built path becomes the next
 * call's `dir`), so a deep enough tree, or one long entry name, overflows the
 * stack buffer. A truncating "safe" sprintf would be worse here, not better:
 * fk_rmtree DELETES whatever path it's given, so silently operating on a
 * truncated path risks deleting the wrong thing. Hard-stop instead. */
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
            char child[4096];
            while ((e = readdir(d)) != 0) {
                if (e->d_name[0] == FK_CH_DOT &&
                    (e->d_name[1] == 0 || (e->d_name[1] == FK_CH_DOT && e->d_name[2] == 0))) {
                    continue;
                }
                fk_path_join(child, 4096, p, e->d_name);
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
    char path[4096];
    while ((e = readdir(d)) != 0) {
        if (e->d_name[0] == FK_CH_DOT &&
            (e->d_name[1] == 0 || (e->d_name[1] == FK_CH_DOT && e->d_name[2] == 0))) {
            continue;
        }
        if (fk_skip_entry(skipv, e->d_name)) {
            continue;
        }
        fk_path_join(path, 4096, dir, e->d_name);
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
static fk_os_socket_t fk_sock_raw[1024];
static int fk_sock_kind[1024];
static long long fk_sock_alloc(fk_os_socket_t s, int kind) {
    long long h = 1;
    while (h < 1024) {
        if (fk_sock_kind[h] == 0) {
            fk_sock_raw[h] = s;
            fk_sock_kind[h] = kind;
            return h;
        }
        h = h + 1;
    }
    fk_os_close_socket(s);
    return -1;
}
static fk_os_socket_t fk_sock_lookup(long long h, int kind) {
    if (h < 1 || h >= 1024 || fk_sock_kind[h] == 0) {
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
    int n = 16;
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
    long long sa = sv >> 1;
    if (!fk_os_socket_ok(s) || sa < 0 || sa >= fk_sp) {
        return -1;
    }
    return fk_os_send_socket(s, fk_sb + fk_so[sa], (unsigned long)fk_sl[sa]);
}
static long long fk_socket_recv_native(long long h, long long maxn) {
    fk_os_socket_t s = fk_sock_lookup(h, 2);
    if (!fk_os_socket_ok(s) || maxn <= 0) {
        return fk_sbuf("", 0);
    }
    static char tmp[65536];
    if (maxn > 65536) {
        maxn = 65536;
    }
    long long got = fk_os_recv_socket(s, tmp, (unsigned long)maxn);
    if (got <= 0) {
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
    static char buf[4096];
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
    static char buf[4096];
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
    static char body[4096];
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
    while (p >= 1 && p <= fk_hp) {
        n = n + 1;
        p = fk_ht[p] >> 1;
    }
    return n;
}
static long long fk_list_to_f32(long long v, float *out, long long cap) {
    long long p = v >> 1;
    long long n = 0;
    while (p >= 1 && p <= fk_hp && n < cap) {
        out[n] = (float)fk_num(fk_hh[p]);
        n = n + 1;
        p = fk_ht[p] >> 1;
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
        fk_die("fk_cons_val: heap exhausted -- cannot melt here (live C-local intermediates are not on the value stack for the collector to trace). Returning nil would be a partial structure accepted as whole.");
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
    char req[4096];
    long long rn = sprintf(req, "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n", path, host);
    rn = fk_http_append_request_headers(req, rn, 4096, headersv);
    if (rn + 2 < 4096) {
        req[rn] = 13;
        req[rn + 1] = 10;
        rn = rn + 2;
    }
    long long wr = 0;
    while (wr < rn) {
        long long nwr = write(fd, req + wr, rn - wr);
        if (nwr <= 0) {
            close(fd);
            return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: write failed", 22));
        }
        wr = wr + nwr;
    }
    static char resp[65536];
    long long total = 0;
    while (total < 65535) {
        long long got = read(fd, resp + total, 65535 - total);
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
    return fk_http_dict_with_headers(status, fk_http_headers(resp, total, bo),
                                     fk_sbuf(resp + bo, total - bo), fk_sbuf("", 0),
                                     fk_elapsed_ms(started));
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
static int fk_http_header_name_ok(const char *buf, long long n) {
    if (n <= 0 || n > 128) {
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
    if (n < 0 || n > 1024) {
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
static long long fk_http_append_request_headers(char *req, long long rn, long long cap,
                                                long long headersv) {
    long long q = headersv >> 1;
    long long count = 0;
    while (q >= 1 && q <= fk_hp && count < 64) {
        long long row = fk_hh[q];
        if ((row & 1) != 0) {
            long long rp = row >> 1;
            if (rp >= 1 && rp <= fk_hp && ((fk_hh[rp] >> 1) == 43001)) {
                long long np = fk_ht[rp] >> 1;
                if (np >= 1 && np <= fk_hp) {
                    long long namev = fk_hh[np];
                    long long vp = fk_ht[np] >> 1;
                    if (vp >= 1 && vp <= fk_hp) {
                        long long valuev = fk_hh[vp];
                        long long ns = namev >> 1;
                        long long vs = valuev >> 1;
                        if ((namev & 1) == 0 && (valuev & 1) == 0 && ns >= 0 && ns < fk_sp &&
                            vs >= 0 && vs < fk_sp) {
                            const char *name = fk_sb + fk_so[ns];
                            const char *value = fk_sb + fk_so[vs];
                            long long nl = fk_sl[ns];
                            long long vl = fk_sl[vs];
                            if (fk_http_header_name_ok(name, nl) &&
                                fk_http_header_value_ok(value, vl) && rn + nl + vl + 4 < cap) {
                                rn = fk_http_append_bytes(req, rn, cap, name, nl);
                                rn = fk_http_append_bytes(req, rn, cap, ": ", 2);
                                rn = fk_http_append_bytes(req, rn, cap, value, vl);
                                rn = fk_http_append_bytes(req, rn, cap, "\r\n", 2);
                                count = count + 1;
                            }
                        }
                    }
                }
            }
        }
        q = fk_ht[q] >> 1;
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
static long long fk_host_exec(long long cmdv, long long inputv) {
    (void)inputv;
    char cmd[8192];
    fk_cstr(cmdv, cmd, 8192);
    void *fp = popen(cmd, "r");
    if (fp == 0) {
        return fk_sbuf("", 0);
    }
    static char hbuf[262144];
    long long total = 0;
    while (total < 262143) {
        unsigned long got = fread(hbuf + total, 1, (unsigned long)(262143 - total), fp);
        if (got == 0) {
            break;
        }
        total = total + (long long)got;
    }
    pclose(fp);
    return fk_sbuf(hbuf, total);
}
static long long fk_sock_request(long long hostv, long long portv, long long reqv) {
    char host[512];
    char port[16];
    fk_cstr(hostv, host, 512);
    fk_cstr(portv, port, 16);
    long long rsa = reqv >> 1;
    long long rlen = (rsa >= 0 && rsa < fk_sp) ? fk_sl[rsa] : 0;
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
        return fk_sbuf("", 0);
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
        return fk_sbuf("", 0);
    }
    const char *rptr = fk_sb + fk_so[rsa];
    long long wr = 0;
    while (wr < rlen) {
        long long nwr = write(fd, rptr + wr, rlen - wr);
        if (nwr <= 0) {
            break;
        }
        wr = wr + nwr;
    }
    static char resp[65536];
    long long total = 0;
    while (total < 65535) {
        long long got = read(fd, resp + total, 65535 - total);
        if (got <= 0) {
            break;
        }
        total = total + got;
    }
    close(fd);
    return fk_sbuf(resp, total);
}
static long long fk_is_dict_value(long long v) {
    if ((v & 1) == 0) {
        return 0;
    }
    long long p = v >> 1;
    if (p < 1 || p > fk_hp) {
        return 0;
    }
    long long marker = fk_sbuf("__dict__", 8);
    long long h = fk_hh[p];
    if ((h & 1) != 0) {
        return 0;
    }
    return fk_keyeq(h >> 1, marker >> 1);
}
static long long fk_get_value(long long target, long long key) {
    if (fk_is_dict_value(target)) {
        long long p = fk_ht[target >> 1] >> 1;
        long long ks = key >> 1;
        while (p >= 1 && p <= fk_hp) {
            long long k = fk_hh[p];
            long long vp = fk_ht[p] >> 1;
            if (vp < 1 || vp > fk_hp) {
                return 0;
            }
            if ((k & 1) == 0 && (key & 1) == 0 && fk_keyeq(k >> 1, ks)) {
                return fk_hh[vp];
            }
            p = fk_ht[vp] >> 1;
        }
        return 0;
    }
    if ((target & 1) != 0) {
        long long want = key >> 1;
        long long p = target >> 1;
        while (p >= 1 && p <= fk_hp && want > 0) {
            p = fk_ht[p] >> 1;
            want = want - 1;
        }
        if (p >= 1 && p <= fk_hp) {
            return fk_hh[p];
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
    char req[4096];
    long long rn = sprintf(req, "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n", path, host);
    rn = fk_http_append_request_headers(req, rn, 4096, headersv);
    if (rn + 2 < 4096) {
        req[rn] = 13;
        req[rn + 1] = 10;
        rn = rn + 2;
    }
    long long wr = 0;
    while (wr < rn) {
        int nwr = SSL_write(ssl, req + wr, (int)(rn - wr));
        if (nwr <= 0) {
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(fd);
            return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: tls write failed", 26));
        }
        wr = wr + nwr;
    }
    static char resp[65536];
    long long total = 0;
    while (total < 65535) {
        int got = SSL_read(ssl, resp + total, (int)(65535 - total));
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
    return fk_http_dict_with_headers(status, fk_http_headers(resp, total, bo),
                                     fk_sbuf(resp + bo, total - bo), fk_sbuf("", 0),
                                     fk_elapsed_ms(started));
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
    char host[512];
    char port[16];
    fk_cstr(hostv, host, 512);
    fk_cstr(portv, port, 16);
    long long rsa = reqv >> 1;
    long long rlen = (rsa >= 0 && rsa < fk_sp) ? fk_sl[rsa] : 0;
    void *lib = fk_ssl_lib();
    if (lib == 0) {
        return fk_sbuf("", 0);
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
        return fk_sbuf("", 0);
    }
    int fd = fk_tcp_connect(host, port);
    if (fd < 0) {
        return fk_sbuf("", 0);
    }
    void *ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == 0) {
        close(fd);
        return fk_sbuf("", 0);
    }
    SSL_CTX_set_verify(ctx, 1, 0);
    SSL_CTX_set_default_verify_paths(ctx);
    void *ssl = SSL_new(ctx);
    if (ssl == 0) {
        SSL_CTX_free(ctx);
        close(fd);
        return fk_sbuf("", 0);
    }
    SSL_ctrl(ssl, 55, 0, host);
    if (SSL_set1_host(ssl, host) != 1) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return fk_sbuf("", 0);
    }
    if (SSL_set_fd(ssl, fd) != 1 || SSL_connect(ssl) != 1) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return fk_sbuf("", 0);
    }
    if (SSL_get_verify_result(ssl) != 0) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return fk_sbuf("", 0);
    }
    const char *rptr = fk_sb + fk_so[rsa];
    long long wr = 0;
    while (wr < rlen) {
        int nwr = SSL_write(ssl, rptr + wr, (int)(rlen - wr));
        if (nwr <= 0) {
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(fd);
            return fk_sbuf("", 0);
        }
        wr = wr + nwr;
    }
    static char resp[65536];
    long long total = 0;
    while (total < 65535) {
        int got = SSL_read(ssl, resp + total, (int)(65535 - total));
        if (got <= 0) {
            break;
        }
        total = total + got;
    }
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(fd);
    return fk_sbuf(resp, total);
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
    while (pa >= 1 && pa <= fk_hp && pb >= 1 && pb <= fk_hp) {
        acc = acc + fk_num(fk_hh[pa]) * fk_num(fk_hh[pb]);
        pa = fk_ht[pa] >> 1;
        pb = fk_ht[pb] >> 1;
    }
    return acc;
}
static double fk_mag_list(long long av) {
    long long pa = av >> 1;
    double acc = 0.0;
    while (pa >= 1 && pa <= fk_hp) {
        double x = fk_num(fk_hh[pa]);
        acc = acc + x * x;
        pa = fk_ht[pa] >> 1;
    }
    return fk_sqrt_d(acc);
}
#define FK_HASHCONS_INIT_CAP 4096 /* fk_hh/fk_ht: the hash-cons cell-pair store, initial size (fk_melt grows it) */
static void fk_arena(void) {
    fk_cap = FK_HASHCONS_INIT_CAP;
    fk_hh = malloc(fk_cap * 8);
    fk_ht = malloc(fk_cap * 8);
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
    if (p < 1 || p > fk_hp) {
        return 0;
    }
    if (fk_fw[p] != 0) {
        return 0;
    }
    fk_fw[p] = 0 - 1;
    return 1 + fk_mlive(fk_ht[p]) + fk_mlive(fk_hh[p]);
}
static long long fk_mcopy(long long b) {
    if ((b & 1) == 0) {
        return b;
    }
    long long p = b >> 1;
    if (p < 1 || p > fk_hp) {
        return b;
    }
    if (fk_fw[p] > 0) {
        return (fk_fw[p] << 1) | 1;
    }
    long long t2 = fk_mcopy(fk_ht[p]);
    long long h2 = fk_mcopy(fk_hh[p]);
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
static void fk_melt(void) {
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
    while (k < FK_MEM_CELL_CAP) {
        nlive = nlive + fk_mlive(fk_mem[k]);
        k = k + 1;
    }
    k = 1;
    while (k <= fk_np) {
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
    fk_nh = malloc(ncap * 8);
    fk_nt = malloc(ncap * 8);
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
    while (k < FK_MEM_CELL_CAP) {
        fk_mem[k] = fk_mcopy(fk_mem[k]);
        k = k + 1;
    }
    k = 1;
    while (k <= fk_np) {
        fk_ncat[k] = fk_mcopy(fk_ncat[k]);
        fk_nkids[k] = fk_mcopy(fk_nkids[k]);
        fk_nval[k] = fk_mcopy(fk_nval[k]);
        k = k + 1;
    }
    free(fk_hh);
    free(fk_ht);
    free(fk_fw);
    fk_hh = fk_nh;
    fk_ht = fk_nt;
    fk_hp = fk_nhp;
    fk_cap = ncap;
    fk_nmelt = fk_nmelt + 1;
    if (fk_conf("FK_MELT_WITNESS")) {
        dprintf(2, "[melt %lld] hp %lld -> %lld, nlive=%lld, cap=%lld, vsp=%lld, np=%lld, fp=%lld, sp=%lld\n",
                fk_nmelt, hp0, fk_hp, nlive, fk_cap, fk_vsp, fk_np, fk_fp, fk_sp);
    }
}
static void fk_vp(long long v) {
    if (fk_vsp >= FK_VALUE_STACK_CAP) {
        fk_die("fk_vp: value stack overflow");
    }
    fk_vs[fk_vsp] = v;
    fk_vsp = fk_vsp + 1;
}
/* FK_FN_CAP: every function-index-shaped table in the seed (fk_fnar,
 * the fn-value sentinel band's FK_FNVAL_MAX_INDEX) is consistently sized/bounded
 * at 4096 -- except fk_fn[] itself, which was declared at half that (2048) while
 * every check gating access to it (`idx < 4096`, scattered across the parser,
 * evaluator) used the LARGER bound. A program defining more than 2048
 * (but fewer than 4096) functions could pass every existing bounds check and
 * still write fk_fn[idx] past the end of its actual array -- a real, reachable
 * overflow. Widening fk_fn[] to match its siblings (rather than tightening every
 * check down to 2048) is the fix: it makes the already-consistent 4096 convention
 * true everywhere, changes nothing for any program that worked before, and turns
 * the previously-silent corruption case into correct behavior instead of a new
 * failure mode. */
#define FK_FN_CAP 4096
#define FK_AST_NODE_CAP 262144 /* fk_node[][4]: the parsed program's own syntax tree (see NOTE above FK_NODE_CAP). Raised 65536->262144 (2026-07-02): a full mel-spectrogram --src program exceeded 64K AST nodes, and "--src is a gate" was a misdiagnosis — this is a raisable capacity constant (same class as FK_TOP_FN_SYM_CAP), not a fundamental limit. 262144*4*8 = 8MB. */
#define FK_PARSE_BUF_CAP 16777216 /* fk_buf: scratch buffer for source artifact reads. Raised 1048576->16777216 (2026-07-16): the v4 .fkb signed lane is 9 bytes (was 5), and a measured band-chain artifact (program-image-fkb-byte-decode-band.fkb, 1,292,944 bytes) exceeded the old 1MiB cap, so fresh caches died on reload with "artifact exceeds FK_PARSE_BUF_CAP". Worst case bounded by FK_AST_NODE_CAP (262144) * 4 lanes * 9B = ~9.4MB plus strings/symbols, so 16MiB holds the format at current capacity constants. */
static long long fk_fn_count;
static long long fk_node_count;
static long long fk_ast_full; /* set once when the AST node table overflows; halts the parse (fk_spos:=fk_slen) so the collect-and-continue recovery cannot spin re-minting sentinels forever. Reset per run. */
static long long fk_fn[FK_FN_CAP];
static long long fk_node[FK_AST_NODE_CAP][4];
static char fk_buf[FK_PARSE_BUF_CAP];
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
static long long fk_str_root_depth(long long i, long long d) {
    if (d > 64 || i < 0 || i >= fk_node_count) {
        return 0;
    }
    long long t = fk_node[i][0];
    if (t == 24 || t == 27 || t == 29 || t == 32 || t == 33 || t == 62 || t == 63 || t == 125) {
        return 1;
    }
    if (t == 6) {
        if (fk_str_root_depth(fk_node[i][2], d + 1) && fk_str_root_depth(fk_node[i][3], d + 1)) {
            return 1;
        }
        return 0;
    }
    if (t == 12) {
        long long f = fk_node[i][1];
        if (f >= 0 && f < fk_fn_count) {
            return fk_str_root_depth(fk_fn[f], d + 1);
        }
        return 0;
    }
    if (t == 69) {
        return fk_str_root_depth(fk_node[i][2], d + 1);
    }
    if (t == 109) {
        return fk_str_root_depth(fk_node[i][3], d + 1);
    }
    if (t == 111) {
        return fk_str_root_depth(fk_node[i][2], d + 1);
    }
    return 0;
}
static void fk_psv(long long v) {
    long long sa = v >> 1;
    if (sa >= 0 && sa < fk_sp) {
        long long j = 0;
        while (j < fk_sl[sa]) {
            putchar((int)(unsigned char)fk_sb[fk_so[sa] + j]);
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
    return p >= 1 && p <= fk_hp;
}
static void fk_pv_inline_number(long long v) {
    if (fk_isf(v)) {
        printf("%.17g", fk_num(v));
    } else if ((v & 1) == 0) {
        printf("%lld", v >> 1);
    } else {
        printf("%lld", v);
    }
}
static void fk_pv_list(long long v, long long depth) {
    if (depth > 1024) {
        fk_die("fk_pv_list: nested output exceeds 1024 levels");
    }
    putchar(FK_CH_LBRACKET);
    long long p = v >> 1;
    int first = 1;
    while (p >= 1 && p <= fk_hp) {
        if (!first) {
            putchar(FK_CH_COMMA);
            putchar(FK_CH_SPACE);
        }
        long long item = fk_hh[p];
        if (fk_is_output_list(item)) {
            fk_pv_list(item, depth + 1);
        } else {
            fk_pv_inline_number(item);
        }
        first = 0;
        p = fk_ht[p] >> 1;
    }
    putchar(FK_CH_RBRACKET);
}
static void fk_pv_root(long long root, long long v) {
    if (fk_is_output_list(v)) {
        fk_pv_list(v, 0);
        putchar(FK_CH_LF);
    } else if (fk_str_root_depth(root, 0)) {
        fk_psv(v);
    } else {
        fk_pv(v);
    }
}
static long long fk_walk(long long i, long long fp);
static long long fk_jit_lower(long long f);
typedef long long (*fk_natfn)(long long *);
static fk_natfn fk_nat_install(const unsigned char *code, long long n);
#define FK_JIT_CODE_BUF_CAP 16384 /* fk_jb: the x86-64 JIT's native-code output buffer, per lowered function */
static unsigned char fk_jb[FK_JIT_CODE_BUF_CAP];
static long long fk_jbp;
static long long fk_jit_frame;
static const unsigned char *fk_src_nat[FK_FN_CAP];
static long long fk_src_nat_len[FK_FN_CAP];
static long long fk_feval_jit_on = 0;
static long long fk_feval_hot = 5;
static long long fk_fheat[FK_FN_CAP];
static long long fk_nat_tried[FK_FN_CAP];
static fk_natfn fk_nat_exec[FK_FN_CAP];
static long long fk_njit;
static long long fk_walk_body(long long i, long long fp) {
    for (;;) {
        long long t = fk_node[i][0];
        if (t < 0 || t >= FK_OPCODE_ARM_CAP) {
            fk_die("fk_walk_body: corrupt node tag");
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
            fk_vs[fp + slot109] = fk_walk(fk_node[i][2], fp);
            /* ROOT the let-local (see the fk_walk tag-109 note): raise fk_vsp over the
             * slot so the next form's temporaries cannot clobber it and a melt relocates it. */
            if (fp + slot109 + 1 > fk_vsp && fp + slot109 + 1 < FK_VALUE_STACK_CAP) {
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
            if (c12 < 0 || c12 >= FK_FN_CAP) {
                fk_vsp = fp;
                return fk_nothing;
            }
            fk_vs[fp] = v12;
            fk_vsp = fp + 1;
            i = fk_fn[c12];
            continue;
        }
        if (t == 240) {
            long long a0 = fk_walk(fk_node[i][2], fp);
            long long a1 = fk_walk(fk_node[i][3], fp);
            long long c240 = fk_node[i][1];
            if (c240 < 0 || c240 >= FK_FN_CAP) {
                fk_vsp = fp;
                return fk_nothing;
            }
            fk_vs[fp] = a0;
            fk_vs[fp + 1] = a1;
            fk_vsp = fp + 2;
            i = fk_fn[c240];
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
            if (c241 < 0 || c241 >= FK_FN_CAP) {
                fk_vsp = fp;
                return fk_nothing;
            }
            long long m241 = 0;
            while (m241 < n241) {
                fk_vs[fp + m241] = fk_vs[base241 + m241];
                m241 = m241 + 1;
            }
            fk_vsp = fp + n241;
            i = fk_fn[c241];
            continue;
        }
        if (t == 244) {
            long long hv244 = fk_walk(fk_node[i][1], fp);
            if (fk_is_fnval(hv244) == 0) {
                fk_vsp = fp;
                return fk_nothing;
            }
            long long fi244 = fk_fnval_idx(hv244);
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
            i = fk_fn[fi244];
            continue;
        }
        if (t == 44) {
            long long fv44 = fk_walk(fk_node[i][1], fp);
            fk_vp(fv44);
            long long av44 = fk_walk(fk_node[i][2], fp);
            fk_vp(av44);
            long long p44 = fk_vs[fk_vsp - 2] >> 1;
            if (p44 < 1 || p44 > fk_hp) {
                fk_vsp = fp;
                return 0;
            }
            long long f44 = fk_hh[p44] >> 1;
            long long p44t = fk_ht[p44] >> 1;
            if (p44t < 1 || p44t > fk_hp) {
                fk_vsp = fp;
                return 0;
            }
            long long a44 = fk_hh[p44t] >> 1;
            long long caps44 = fk_ht[p44t];
            long long args44 = fk_vs[fk_vsp - 1];
            long long rev44 = 1;
            long long cc44 = caps44 >> 1;
            while (cc44 >= 1 && cc44 <= fk_hp) {
                fk_hp = fk_hp + 1;
                fk_hh[fk_hp] = fk_hh[cc44];
                fk_ht[fk_hp] = rev44;
                rev44 = (fk_hp << 1) | 1;
                cc44 = fk_ht[cc44] >> 1;
            }
            long long comb44 = args44;
            long long rr44 = rev44 >> 1;
            while (rr44 >= 1 && rr44 <= fk_hp) {
                fk_hp = fk_hp + 1;
                fk_hh[fk_hp] = fk_hh[rr44];
                fk_ht[fk_hp] = comb44;
                comb44 = (fk_hp << 1) | 1;
                rr44 = fk_ht[rr44] >> 1;
            }
            long long carg44 = 1;
            if (a44 == 0) {
                carg44 = 1;
            } else {
                if (a44 == 1) {
                    long long pa44 = comb44 >> 1;
                    if (pa44 < 1 || pa44 > fk_hp) {
                        fk_vsp = fp;
                        return 1;
                    }
                    carg44 = fk_hh[pa44];
                } else {
                    carg44 = comb44;
                }
            }
            if (f44 < 0 || f44 >= FK_FN_CAP) {
                fk_vsp = fp;
                return 0;
            }
            fk_vs[fp] = carg44;
            fk_vsp = fp + 1;
            i = fk_fn[f44];
            continue;
        }
        return fk_walk(i, fp);
    }
}
static long long fk_walk_cold(long long t, long long i, long long fp);
/* the honest eval-depth wall: measure REAL stack use and die SAYING SO before the host
 * stack dies silently (witnessed 2026-07-01: exit 127, no output, three recipes in one
 * day — the Windows main lacked the POSIX main's FORM_KERNEL_STACK_MB big-stack thread,
 * now mirrored below). The mains raise the wall to reserve minus 2MB; the recipe-side
 * home for deep recursion stays the same: make it tail or balanced. */
static char *fk_stack_base = 0;
static long long fk_stack_wall = 6 * 1024 * 1024;
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
        fk_die("fk_walk: corrupt node tag");
    }
    fk_arms[t] = fk_arms[t] + 1;
    if (t == 1) {
        return fk_node[i][1] << 1;
    }
    if (t == 2) {
        return fk_vs[fp];
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
        long long a5 = fk_walk(fk_node[i][1], fp);
        long long b5 = fk_walk(fk_node[i][2], fp);
        if (fk_num(a5) <= fk_num(b5)) {
            return 2;
        }
        return 0;
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
        long long r7 = fk_walk_body(fk_fn[0], b7);
        fk_vsp = b7;
        return r7;
    }
    if (t == 8) {
        return fk_node[fk_walk(fk_node[i][1], fp) >> 1][fk_walk(fk_node[i][2], fp) >> 1] << 1;
    }
    if (t == 12) {
        long long c12 = fk_node[i][1];
        if (c12 < 0 || c12 >= FK_FN_CAP) {
            return fk_nothing;
        }
        long long v12 = fk_walk(fk_node[i][2], fp);
        fk_vp(v12);
        long long b12 = fk_vsp - 1;
        long long r12 = fk_walk_body(fk_fn[c12], b12);
        fk_vsp = b12;
        return fk_offer_ack(c12, 1, r12);
    }
    if (t == 240) {
        long long c240 = fk_node[i][1];
        if (c240 < 0 || c240 >= FK_FN_CAP) {
            return fk_nothing;
        }
        long long a0 = fk_walk(fk_node[i][2], fp);
        long long a1 = fk_walk(fk_node[i][3], fp);
        fk_vp(a0);
        fk_vp(a1);
        long long b240 = fk_vsp - 2;
        long long r240 = fk_walk_body(fk_fn[c240], b240);
        fk_vsp = b240;
        return fk_offer_ack(c240, 2, r240);
    }
    if (t == 241) {
        long long c241 = fk_node[i][1];
        if (c241 < 0 || c241 >= FK_FN_CAP) {
            return fk_nothing;
        }
        long long base241 = fk_vsp;
        long long cell241 = fk_node[i][2];
        while (cell241 >= 0 && fk_node[cell241][0] == 242) {
            fk_vp(fk_walk(fk_node[cell241][1], fp));
            cell241 = fk_node[cell241][2];
        }
        long long n241 = fk_vsp - base241;
        long long r241 = fk_walk_body(fk_fn[c241], base241);
        fk_vsp = base241;
        return fk_offer_ack(c241, n241, r241);
    }
    if (t == 242) {
        return 0;
    }
    if (t == 243) {
        return fk_fnval(fk_node[i][1]);
    }
    if (t == 244) {
        long long hv244 = fk_walk(fk_node[i][1], fp);
        if (fk_is_fnval(hv244) == 0) {
            return fk_nothing;
        }
        long long fi244 = fk_fnval_idx(hv244);
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
        long long r244 = fk_walk_body(fk_fn[fi244], base244);
        fk_vsp = base244;
        return fk_offer_ack(fi244, n244, r244);
    }
    if (t == 13) {
        long long mi = fk_walk(fk_node[i][1], fp) >> 1;
        long long mv = fk_walk(fk_node[i][2], fp);
        fk_mem[mi & (FK_MEM_CELL_CAP - 1)] = mv;
        return mv;
    }
    if (t == 14) {
        return fk_mem[(fk_walk(fk_node[i][1], fp) >> 1) & (FK_MEM_CELL_CAP - 1)];
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
            dprintf(2, "[cons] heap full after melt (hp=%lld cap=%lld) -- returning nil, list is CORRUPT\n", fk_hp, fk_cap);
            fk_vsp = fk_vsp - 2;
            return 1;
        }
        fk_hp = fk_hp + 1;
        fk_hh[fk_hp] = fk_vs[fk_vsp - 2];
        fk_ht[fk_hp] = fk_vs[fk_vsp - 1];
        fk_vsp = fk_vsp - 2;
        return (fk_hp << 1) | 1;
    }
    if (t == 20) {
        long long p = fk_walk(fk_node[i][1], fp) >> 1;
        if (p < 1 || p > fk_hp) {
            return 1;
        }
        return fk_hh[p];
    }
    if (t == 21) {
        long long p = fk_walk(fk_node[i][1], fp) >> 1;
        if (p < 1 || p > fk_hp) {
            return 1;
        }
        return fk_ht[p];
    }
    if (t == 22) {
        long long p = fk_walk(fk_node[i][1], fp) >> 1;
        long long n = 0;
        while (p >= 1 && p <= fk_hp) {
            n = n + 1;
            p = fk_ht[p] >> 1;
        }
        return n << 1;
    }
    if (t == 23) {
        long long x23 = fk_walk(fk_node[i][1], fp);
        fk_vp(x23);
        long long k23 = fk_walk(fk_node[i][2], fp) >> 1;
        fk_vsp = fk_vsp - 1;
        long long p = fk_vs[fk_vsp] >> 1;
        while (p >= 1 && p <= fk_hp && k23 > 0) {
            p = fk_ht[p] >> 1;
            k23 = k23 - 1;
        }
        if (p < 1 || p > fk_hp) {
            return 1;
        }
        return fk_hh[p];
    }
    if (t == 44) {
        long long fv44 = fk_walk(fk_node[i][1], fp);
        fk_vp(fv44);
        long long av44 = fk_walk(fk_node[i][2], fp);
        fk_vp(av44);
        long long p44 = fk_vs[fk_vsp - 2] >> 1;
        if (p44 < 1 || p44 > fk_hp) {
            fk_vsp = fk_vsp - 2;
            return 0;
        }
        long long f44 = fk_hh[p44] >> 1;
        long long p44t = fk_ht[p44] >> 1;
        if (p44t < 1 || p44t > fk_hp) {
            fk_vsp = fk_vsp - 2;
            return 0;
        }
        long long a44 = fk_hh[p44t] >> 1;
        long long caps44 = fk_ht[p44t];
        long long args44 = fk_vs[fk_vsp - 1];
        long long rev44 = 1;
        long long cc44 = caps44 >> 1;
        while (cc44 >= 1 && cc44 <= fk_hp) {
            fk_hp = fk_hp + 1;
            fk_hh[fk_hp] = fk_hh[cc44];
            fk_ht[fk_hp] = rev44;
            rev44 = (fk_hp << 1) | 1;
            cc44 = fk_ht[cc44] >> 1;
        }
        long long comb44 = args44;
        long long rr44 = rev44 >> 1;
        while (rr44 >= 1 && rr44 <= fk_hp) {
            fk_hp = fk_hp + 1;
            fk_hh[fk_hp] = fk_hh[rr44];
            fk_ht[fk_hp] = comb44;
            comb44 = (fk_hp << 1) | 1;
            rr44 = fk_ht[rr44] >> 1;
        }
        long long carg44 = 1;
        if (a44 == 0) {
            carg44 = 1;
        } else {
            if (a44 == 1) {
                long long pa44 = comb44 >> 1;
                if (pa44 < 1 || pa44 > fk_hp) {
                    fk_vsp = fk_vsp - 2;
                    return 1;
                }
                carg44 = fk_hh[pa44];
            } else {
                carg44 = comb44;
            }
        }
        if (f44 < 0 || f44 >= FK_FN_CAP) {
            fk_vsp = fk_vsp - 2;
            return 0;
        }
        fk_vsp = fk_vsp - 2;
        fk_vp(carg44);
        long long b44 = fk_vsp - 1;
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
        if (fk_veq(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp)) != 0) {
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
        long long ae = fk_walk(fk_node[i][1], fp);
        long long be = fk_walk(fk_node[i][2], fp);
        if (fk_num(ae) == fk_num(be)) {
            return 2;
        }
        return 0;
    }
    if (t == 103) {
        long long al = fk_walk(fk_node[i][1], fp);
        long long bl = fk_walk(fk_node[i][2], fp);
        if (fk_num(al) < fk_num(bl)) {
            return 2;
        }
        return 0;
    }
    if (t == 109) {
        long long slot109 = fk_walk(fk_node[i][1], fp) >> 1;
        fk_vs[fp + slot109] = fk_walk(fk_node[i][2], fp);
        /* ROOT the let-local: raise fk_vsp over the slot so the body's temporaries
         * (pushed at fk_vsp) cannot overwrite it, and a compacting melt relocates it.
         * Without this a do-let chain outside a tag-111 frame reservation (e.g. a
         * top-level (do (let a ..) (let b ..) ..)) silently clobbers a while
         * evaluating b -- string-bearing list values push enough temps to reach the
         * slot. The enclosing frame/call boundary restores fk_vsp. */
        if (fp + slot109 + 1 > fk_vsp && fp + slot109 + 1 < FK_VALUE_STACK_CAP) {
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
    return fk_walk_cold(t, i, fp);
}
static long long fk_walk_cold(long long t, long long i, long long fp) {
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
        return time(0) << 1;
    }
    if (t == 16) {
        return ((long long)arc4random()) << 1;
    }
    if (t == 17) {
        long long ix17 = fk_walk(fk_node[i][1], fp) >> 1;
        if (ix17 < 0 || ix17 >= fk_src_len || ix17 >= 262144) {
            return 0;
        }
        return ((long long)(unsigned char)fk_src[ix17]) << 1;
    }
    if (t == 24) {
        return fk_node[i][1] << 1;
    }
    if (t == 25) {
        long long sa = fk_walk(fk_node[i][1], fp) >> 1;
        if (sa < 0 || sa >= fk_sp) {
            return 0;
        }
        return fk_sl[sa] << 1;
    }
    if (t == 26) {
        long long sa26 = fk_walk(fk_node[i][1], fp) >> 1;
        long long sb26 = fk_walk(fk_node[i][2], fp) >> 1;
        if (fk_keyeq(sa26, sb26)) {
            return 2;
        }
        return 0;
    }
    if (t == 27) {
        long long sa = fk_walk(fk_node[i][1], fp) >> 1;
        long long sb = fk_walk(fk_node[i][2], fp) >> 1;
        if (sa < 0 || sa >= fk_sp || sb < 0 || sb >= fk_sp) {
            return 0 - 2;
        }
        long long ln = fk_sl[sa] + fk_sl[sb];
        while (fk_sbp + ln > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = realloc(fk_sb, fk_scap_b);
            fk_sb_check();
        }
        long long j = 0;
        while (j < fk_sl[sa]) {
            fk_sb[fk_sbp + j] = fk_sb[fk_so[sa] + j];
            j = j + 1;
        }
        j = 0;
        while (j < fk_sl[sb]) {
            fk_sb[fk_sbp + fk_sl[sa] + j] = fk_sb[fk_so[sb] + j];
            j = j + 1;
        }
        return fk_sintern(fk_sbp, ln) << 1;
    }
    if (t == 28) {
        long long sa = fk_walk(fk_node[i][1], fp) >> 1;
        long long k = fk_walk(fk_node[i][2], fp) >> 1;
        if (sa < 0 || sa >= fk_sp || k < 0 || k >= fk_sl[sa]) {
            return 0 - 2;
        }
        return ((long long)(unsigned char)fk_sb[fk_so[sa] + k]) << 1;
    }
    if (t == 30) {
        long long sa = fk_walk(fk_node[i][1], fp) >> 1;
        long long sb = fk_walk(fk_node[i][2], fp) >> 1;
        long long from = fk_walk(fk_node[i][3], fp) >> 1;
        if (sa < 0 || sa >= fk_sp || sb < 0 || sb >= fk_sp) {
            return 0 - 2;
        }
        if (from < 0) {
            from = 0;
        }
        if (from > fk_sl[sa]) {
            return 0 - 2;
        }
        long long ln = fk_sl[sb];
        if (ln == 0) {
            return from << 1;
        }
        long long lim = fk_sl[sa] - ln;
        long long pos = from;
        while (pos <= lim) {
            long long j3 = 0;
            while (j3 < ln && fk_sb[fk_so[sa] + pos + j3] == fk_sb[fk_so[sb] + j3]) {
                j3 = j3 + 1;
            }
            if (j3 == ln) {
                return pos << 1;
            }
            pos = pos + 1;
        }
        return 0 - 2;
    }
    if (t == 31) {
        long long sa = fk_walk(fk_node[i][1], fp) >> 1;
        if (sa < 0 || sa >= fk_sp) {
            return 0;
        }
        long long off = fk_so[sa];
        long long n = fk_sl[sa];
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
            return fk_sintern(fk_sbp, 0) << 1;
        }
        while (fk_sbp + 1 > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = realloc(fk_sb, fk_scap_b);
            fk_sb_check();
        }
        fk_sb[fk_sbp] = (char)b;
        return fk_sintern(fk_sbp, 1) << 1;
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
        long long ix43 = 1;
        while (ix43 <= fk_np) {
            if (fk_nkind[ix43] == 1 && fk_nid[ix43][2] == 1 && fk_nval[ix43] == iv43) {
                return fk_nbox(ix43);
            }
            ix43 = ix43 + 1;
        }
        if (fk_np + 1 >= FK_NODE_CAP) {
            fk_die("fk value-node table full (FK_NODE_CAP)");
        }
        fk_np = fk_np + 1;
        fk_nkind[fk_np] = 1;
        fk_nval[fk_np] = iv43;
        fk_nkids[fk_np] = 1;
        fk_ncat[fk_np] = 0;
        fk_nid[fk_np][0] = 1;
        fk_nid[fk_np][1] = 1;
        fk_nid[fk_np][2] = 1;
        fk_nid[fk_np][3] = iv43 >> 1;
        return fk_nbox(fk_np);
    }
    if (t == 46) {
        long long sv46 = fk_walk(fk_node[i][1], fp);
        long long sa46 = sv46 >> 1;
        long long ix46 = 1;
        while (ix46 <= fk_np) {
            if (fk_nkind[ix46] == 1 && fk_nid[ix46][2] == 2 && fk_nval[ix46] == sv46) {
                return fk_nbox(ix46);
            }
            ix46 = ix46 + 1;
        }
        if (sa46 < 0 || sa46 >= fk_sp) {
            return 0;
        }
        if (fk_np + 1 >= FK_NODE_CAP) {
            fk_die("fk value-node table full (FK_NODE_CAP)");
        }
        fk_np = fk_np + 1;
        fk_nkind[fk_np] = 1;
        fk_nval[fk_np] = sv46;
        fk_nkids[fk_np] = 1;
        fk_ncat[fk_np] = 0;
        fk_nid[fk_np][0] = 1;
        fk_nid[fk_np][1] = 1;
        fk_nid[fk_np][2] = 2;
        fk_nid[fk_np][3] = sa46;
        return fk_nbox(fk_np);
    }
    if (t == 47) {
        long long cat47 = fk_walk(fk_node[i][1], fp);
        long long kids47 = fk_walk(fk_node[i][2], fp);
        long long ix47 = 1;
        while (ix47 <= fk_np) {
            if (fk_nkind[ix47] == 2 && fk_veq(fk_ncat[ix47], cat47) != 0 &&
                fk_veq(fk_nkids[ix47], kids47) != 0) {
                return fk_nbox(ix47);
            }
            ix47 = ix47 + 1;
        }
        if (fk_np + 1 >= FK_NODE_CAP) {
            fk_die("fk value-node table full (FK_NODE_CAP)");
        }
        fk_np = fk_np + 1;
        fk_nkind[fk_np] = 2;
        fk_ncat[fk_np] = cat47;
        fk_nkids[fk_np] = kids47;
        fk_nval[fk_np] = 0;
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
    if (t == 91) {
        long long xs91 = fk_walk(fk_node[i][1], fp);
        long long q91 = xs91 >> 1;
        long long p91 = 0;
        long long l91 = 0;
        long long ty91 = 0;
        long long in91 = 0;
        if (q91 >= 1 && q91 <= fk_hp) {
            p91 = fk_hh[q91] >> 1;
            q91 = fk_ht[q91] >> 1;
        }
        if (q91 >= 1 && q91 <= fk_hp) {
            l91 = fk_hh[q91] >> 1;
            q91 = fk_ht[q91] >> 1;
        }
        if (q91 >= 1 && q91 <= fk_hp) {
            ty91 = fk_hh[q91] >> 1;
            q91 = fk_ht[q91] >> 1;
        }
        if (q91 >= 1 && q91 <= fk_hp) {
            in91 = fk_hh[q91] >> 1;
        }
        if (fk_np + 1 >= FK_NODE_CAP) {
            fk_die("fk value-node table full (FK_NODE_CAP)");
        }
        fk_np = fk_np + 1;
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
    if (t == 112) {
        long long bv112 = fk_walk(fk_node[i][1], fp);
        long long se112 = (bv112 != 0) ? (0 - 9223372036854775807LL) : (0 - 9223372036854775805LL);
        long long ix112 = 1;
        while (ix112 <= fk_np) {
            if (fk_nkind[ix112] == 1 && fk_nid[ix112][2] == 3 && fk_nval[ix112] == se112) {
                return fk_nbox(ix112);
            }
            ix112 = ix112 + 1;
        }
        if (fk_np + 1 >= FK_NODE_CAP) {
            fk_die("fk value-node table full (FK_NODE_CAP)");
        }
        fk_np = fk_np + 1;
        fk_nkind[fk_np] = 1;
        fk_nval[fk_np] = se112;
        fk_nkids[fk_np] = 1;
        fk_ncat[fk_np] = 0;
        fk_nid[fk_np][0] = 1;
        fk_nid[fk_np][1] = 1;
        fk_nid[fk_np][2] = 3;
        fk_nid[fk_np][3] = (bv112 != 0) ? 1 : 0;
        return fk_nbox(fk_np);
    }
    if (t == 113) {
        long long sa113 = fk_walk(fk_node[i][1], fp) >> 1;
        double fd113 = 0.0;
        if (sa113 >= 0 && sa113 < fk_sp) {
            char tb113[128];
            long long n113 = fk_sl[sa113];
            if (n113 > 126) {
                n113 = 126;
            }
            long long jj113 = 0;
            while (jj113 < n113) {
                tb113[jj113] = fk_sb[fk_so[sa113] + jj113];
                jj113 = jj113 + 1;
            }
            tb113[n113] = 0;
            fd113 = strtod(tb113, 0);
        }
        long long fb113 = fk_fbox(fd113);
        if (fk_np + 1 >= FK_NODE_CAP) {
            fk_die("fk value-node table full (FK_NODE_CAP)");
        }
        fk_np = fk_np + 1;
        fk_nkind[fk_np] = 1;
        fk_nval[fk_np] = fb113;
        fk_nkids[fk_np] = 1;
        fk_ncat[fk_np] = 0;
        fk_nid[fk_np][0] = 1;
        fk_nid[fk_np][1] = 1;
        fk_nid[fk_np][2] = 7;
        fk_nid[fk_np][3] = 0;
        return fk_nbox(fk_np);
    }
    if (t == 50) {
        long long sa = fk_node[i][1];
        if (sa < 0 || sa >= fk_sp) {
            return 0;
        }
        char tmp[128];
        long long n = fk_sl[sa];
        if (n > 126) {
            n = 126;
        }
        long long j = 0;
        while (j < n) {
            tmp[j] = fk_sb[fk_so[sa] + j];
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
        long long sa = fk_walk(fk_node[i][1], fp) >> 1;
        if (sa < 0 || sa >= fk_sp) {
            return fk_fbox(0.0);
        }
        char tmp[128];
        long long n = fk_sl[sa];
        if (n > 126) {
            n = 126;
        }
        long long j = 0;
        while (j < n) {
            tmp[j] = fk_sb[fk_so[sa] + j];
            j = j + 1;
        }
        tmp[n] = 0;
        return fk_fbox(strtod(tmp, 0));
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
    if (t == 55) {
        static char p[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096);
        fk_unlink_segments(p);
        return rmdir(p) << 1;
    }
    if (t == 56) {
        static char p[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096);
        int rc = mkdir(p, 0777);
        return (rc < 0 ? 0 : 1) << 1;
    }
    if (t == 57) {
        static char p[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096);
        int fd = open(p, 0);
        if (fd < 0) {
            return 0;
        }
        close(fd);
        return 2;
    }
    if (t == 58) {
        static char p[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096);
        return unlink(p) << 1;
    }
    if (t == 59) {
        static char a[4096];
        static char b[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), a, 4096);
        fk_cstr(fk_walk(fk_node[i][2], fp), b, 4096);
        return rename(a, b) << 1;
    }
    if (t == 60) {
        static char p[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096);
        int fd = open(p, 0);
        if (fd < 0) {
            return -2;
        }
        long n = lseek(fd, 0, 2);
        close(fd);
        return ((long long)n) << 1;
    }
    if (t == 61) {
        static char p[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096);
        long long xs = fk_walk(fk_node[i][2], fp);
        int fd = open(p, O_WRONLY | O_CREAT | O_APPEND, 0666);
        if (fd < 0) {
            return -2;
        }
        static char tmp[8192];
        long long n = 0;
        long long q = xs >> 1;
        while (q >= 1 && q <= fk_hp && n < 8192) {
            tmp[n] = (char)(fk_hh[q] >> 1);
            n = n + 1;
            q = fk_ht[q] >> 1;
        }
        long long wr = write(fd, tmp, n);
        long long total = lseek(fd, 0, 2);
        close(fd);
        if (wr < 0 || total < 0) {
            return -2;
        }
        return total << 1;
    }
    if (t == 62) {
        static char p[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096);
        long long off = fk_walk(fk_node[i][2], fp) >> 1;
        long long len = fk_walk(fk_node[i][3], fp) >> 1;
        if (len <= 0) {
            return fk_sbuf("", 0);
        }
        int fd = open(p, O_RDBIN);
        if (fd < 0) {
            return fk_sbuf("", 0);
        }
        lseek(fd, off, 0);
        fk_sinit();
        while (fk_sbp + len > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = realloc(fk_sb, fk_scap_b);
            fk_sb_check();
        }
        long long got = read(fd, fk_sb + fk_sbp, len);
        close(fd);
        if (got < 0) {
            got = 0;
        }
        return fk_sintern(fk_sbp, got) << 1;
    }
    if (t == 63) {
        static char p[4096];
        static long long fk_nreads;
        long long pv63 = fk_walk(fk_node[i][1], fp);
        fk_cstr(pv63, p, 4096);
        fk_nreads = fk_nreads + 1;
        int fd = open(p, O_RDBIN);
        if (fd < 0) {
            if (fk_conf("FK_READ_WITNESS")) {
                long long sa63 = pv63 >> 1;
                dprintf(2, "[read_file] OPEN FAILED at read #%lld: '%s' (handle=%lld sa=%lld sl=%lld so=%lld sp=%lld)\n",
                        fk_nreads, p, pv63, sa63, (sa63 >= 0 && sa63 < fk_sp) ? fk_sl[sa63] : -1,
                        (sa63 >= 0 && sa63 < fk_sp) ? fk_so[sa63] : -1, fk_sp);
            }
            return fk_sbuf("", 0);
        }
        fk_sinit();
        long long base = fk_sbp;
        long long total = 0;
        for (;;) {
            while (base + total + 65536 > fk_scap_b) {
                fk_scap_b = fk_scap_b * 2;
                void *sb0 = fk_sb;
                fk_sb = realloc(fk_sb, fk_scap_b);
                fk_sb_check();
                if (fk_conf("FK_READ_WITNESS")) {
                    dprintf(2, "[read_file] pool grow -> %lld bytes, %p -> %p (sbp=%lld sp=%lld)\n", fk_scap_b, sb0, (void *)fk_sb, fk_sbp, fk_sp);
                }
            }
            long long got = read(fd, fk_sb + base + total, 65536);
            if (got <= 0) {
                break;
            }
            total = total + got;
        }
        close(fd);
        return fk_sintern(base, total) << 1;
    }
    if (t == 64) {
        long long xs64 = fk_walk(fk_node[i][1], fp);
        fk_rp = fk_rp + 1;
        if (fk_rp >= FK_RECORD_CAP) {
            fk_die("fk_walk tag 64: FK_RECORD_CAP live records exceeded -- clamping fk_rp to the last slot would silently ALIAS two distinct records onto one, a whole quietly swapped for another. Raise FK_RECORD_CAP if a real program needs this many live records.");
        }
        fk_rcnt[fk_rp] = 0;
        fk_rbp[fk_rp] = 0;
        long long q64 = xs64 >> 1;
        while (q64 >= 1 && q64 <= fk_hp) {
            long long e64 = fk_hh[q64];
            long long ep64 = e64 >> 1;
            if (ep64 >= 1 && ep64 <= fk_hp) {
                long long k64 = fk_hh[ep64] >> 1;
                long long tp64 = fk_ht[ep64] >> 1;
                long long v64 = 0;
                if (tp64 >= 1 && tp64 <= fk_hp) {
                    v64 = fk_hh[tp64];
                }
                if (k64 == -1) {
                    fk_rbp[fk_rp] = v64;
                } else if (fk_rcnt[fk_rp] < FK_RECORD_MAX_KEYS) {
                    fk_rkey[fk_rp][fk_rcnt[fk_rp]] = k64;
                    fk_rval[fk_rp][fk_rcnt[fk_rp]] = v64;
                    fk_rcnt[fk_rp] = fk_rcnt[fk_rp] + 1;
                } else {
                    fk_die("fk_walk tag 64: FK_RECORD_MAX_KEYS exceeded -- silently dropping a key past the cap would be a partial record accepted as whole. Raise FK_RECORD_MAX_KEYS if a real record needs this many keys.");
                }
            }
            q64 = fk_ht[q64] >> 1;
        }
        return fk_rbox(fk_rp);
    }
    if (t == 65) {
        long long r = fk_ridx(fk_walk(fk_node[i][1], fp));
        long long key = fk_walk(fk_node[i][2], fp) >> 1;
        if (r < 1 || r >= FK_RECORD_CAP) {
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
        long long rec = fk_walk(fk_node[i][1], fp);
        long long r = fk_ridx(rec);
        long long key = fk_walk(fk_node[i][2], fp) >> 1;
        long long val = fk_walk(fk_node[i][3], fp);
        if (r < 1 || r >= FK_RECORD_CAP) {
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
        if (fk_rcnt[r] < FK_RECORD_MAX_KEYS) {
            fk_rkey[r][fk_rcnt[r]] = key;
            fk_rval[r][fk_rcnt[r]] = val;
            fk_rcnt[r] = fk_rcnt[r] + 1;
        }
        return rec;
    }
    if (t == 67) {
        long long r = fk_ridx(fk_walk(fk_node[i][1], fp));
        long long key = fk_walk(fk_node[i][2], fp) >> 1;
        if (r < 1 || r >= FK_RECORD_CAP) {
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
        if (r < 1 || r >= FK_RECORD_CAP) {
            return out;
        }
        long long j = fk_rcnt[r];
        while (j > 0) {
            j = j - 1;
            if (fk_hp + 1 >= fk_cap) {
                return out;
            }
            fk_hp = fk_hp + 1;
            fk_hh[fk_hp] = fk_rkey[r][j] << 1;
            fk_ht[fk_hp] = out;
            out = (fk_hp << 1) | 1;
        }
        return out;
    }
    if (t == 101) {
        return fk_tempdir();
    }
    if (t == 104) {
        static char p[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096);
        long long sv104 = fk_walk(fk_node[i][2], fp);
        long long sa104 = sv104 >> 1;
        if (sa104 < 0 || sa104 >= fk_sp) {
            return -2;
        }
        long long n104 = fk_sl[sa104];
        long long base104 = fk_so[sa104];
        int fd104 = open(p, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd104 < 0) {
            return -2;
        }
        long long wr104 = 0;
        while (wr104 < n104) {
            long long w104 = write(fd104, fk_sb + base104 + wr104, n104 - wr104);
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
        static char fkl_p[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), fkl_p, 4096);
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
        static char p70[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), p70, 4096);
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
            fk_sb = realloc(fk_sb, fk_scap_b);
            fk_sb_check();
        }
        long long got71 = read((int)fd71, fk_sb + fk_sbp, max71);
        if (got71 <= 0) {
            return fk_sbuf("", 0);
        }
        return fk_sintern(fk_sbp, got71) << 1;
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
        return fk_native_call_test(fk_walk(fk_node[i][1], fp) >> 1) << 1;
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
    if (t == 237) {
        static char p237a[4096];
        static char p237b[4096];
        long long a237 = fk_walk(fk_node[i][1], fp);
        fk_vp(a237);
        long long b237 = fk_walk(fk_node[i][2], fp);
        fk_vsp = fk_vsp - 1;
        fk_cstr(fk_vs[fk_vsp], p237a, 4096);
        fk_cstr(b237, p237b, 4096);
        return fk_wav_loopback(p237a, p237b);
    }
    if (t == 200) {
        static char p200[4096];
        fk_cstr(fk_walk(fk_node[i][1], fp), p200, 4096);
        return fk_path_is_dir(p200) ? 2 : 0;
    }
    if (t == 202) {
        static char r202[4096];
        static char s202[256];
        fk_cstr(fk_walk(fk_node[i][1], fp), r202, 4096);
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
        long long key = fk_walk(fk_node[i][2], fp) >> 1;
        if (r < 1 || r >= FK_RECORD_CAP) {
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
        long long rec = fk_walk(fk_node[i][1], fp);
        long long r = fk_ridx(rec);
        long long key = fk_walk(fk_node[i][2], fp) >> 1;
        long long val = fk_walk(fk_node[i][3], fp);
        if (r < 1 || r >= FK_RECORD_CAP) {
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
        if (fk_rcnt[r] < FK_RECORD_MAX_KEYS) {
            fk_rkey[r][fk_rcnt[r]] = key;
            fk_rval[r][fk_rcnt[r]] = val;
            fk_rcnt[r] = fk_rcnt[r] + 1;
        }
        return 0;
    }
    if (t == 100) {
        long long r = fk_ridx(fk_walk(fk_node[i][1], fp));
        if (r < 1 || r >= FK_RECORD_CAP) {
            return 0;
        }
        return fk_rbp[r];
    }
    if (t == 127) {
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
        if (ks_k == 6) {
            return fk_hp << 1;
        }
        if (ks_k == 7) {
            return fk_vsp << 1;
        }
        if (ks_k == 8) {
            return fk_fp << 1;
        }
        if (ks_k >= 100 && ks_k < 100 + ks_n) {
            return fk_arms[ks_k - 100] << 1;
        }
        return 0;
    }
    if (t == 128) {
        long long fr_nv = fk_walk(fk_node[i][1], fp);
        long long fr_fv = fk_walk(fk_node[i][2], fp);
        long long fr_pk = fk_walk(fk_node[i][3], fp) >> 1;
        long long fr_ni = fk_nidx(fr_nv);
        if (fr_ni >= 1 && fr_ni <= fk_np) {
            fk_nsfile[fr_ni] = fr_fv;
            fk_nsline[fr_ni] = fr_pk >> 16;
            fk_nscol[fr_ni] = fr_pk & 65535;
            fk_nsattr[fr_ni] = 1;
            if (fk_fbn >= FK_NODE_CAP) {
                fk_die("fk fbroots table full (FK_NODE_CAP): GC root registration would be silently dropped");
            }
            fk_fbroots[fk_fbn] = fr_nv;
            fk_fbn = fk_fbn + 1;
        }
        return fr_nv;
    }
    if (t == 129) {
        if (fk_cap == 0) {
            fk_arena();
        }
        if ((fk_hp + fk_fbn + 4) * 100 >= fk_cap * 90) {
            fk_melt();
        }
        long long fe_r = 1;
        long long fe_i = fk_fbn;
        while (fe_i > 0) {
            fe_i = fe_i - 1;
            if (fk_hp + 1 >= fk_cap) {
                fk_die("fk_walk tag 129: heap exhausted draining fbroots after melt -- silently skipping a root would return a truncated root list, live roots dropped without witness.");
            }
            fk_hp = fk_hp + 1;
            fk_hh[fk_hp] = fk_fbroots[fe_i];
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
                    return 0 - 2;
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
            fk_sb = realloc(fk_sb, fk_scap_b);
            fk_sb_check();
        }
        long long rj = 0;
        while (rj < rn) {
            fk_sb[fk_sbp + rj] = rbuf[rj];
            rj = rj + 1;
        }
        return fk_sintern(fk_sbp, rn) << 1;
    }
    if (t == 115) {
        long long psv = fk_walk(fk_node[i][1], fp);
        long long psa = psv >> 1;
        if (psa >= 0 && psa < fk_sp) {
            long long pj = 0;
            while (pj < fk_sl[psa]) {
                putchar((int)(unsigned char)fk_sb[fk_so[psa] + pj]);
                pj = pj + 1;
            }
        }
        putchar(10);
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
            return fk_sintern(fk_sbp, 0) << 1;
        }
        while (fk_sbp + fk_gen_len > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = realloc(fk_sb, fk_scap_b);
            fk_sb_check();
        }
        long long gj = 0;
        while (gj < fk_gen_len) {
            fk_sb[fk_sbp + gj] = (char)fk_gen[gj];
            gj = gj + 1;
        }
        return fk_sintern(fk_sbp, fk_gen_len) << 1;
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
#define FK_SOURCE_TEXT_CAP 8388608 /* fk_srctext: the parsed program's own source text (distinct from fk_src, the staged input buffer). Raised 262144->8388608 (2026-07-02): a 267KB generated --src program (1,200 audio-clip paths) was SILENTLY truncated at 262,143 bytes by fk_run_src's single bounded read; the permissive reader auto-closed the amputated program and ran it, yielding a deterministic wrong answer with no error -- the "N=100 cliff". fk_run_src now reads to EOF and dies loudly if the program exceeds this cap. */
static char fk_srctext[FK_SOURCE_TEXT_CAP];
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
    if (off < 0) {
        dprintf(2, "fkwu: %s: ", sev == FK_DIAG_ERR ? "error" : "warning");
    } else {
        long long line = 1, i = 0, lastnl = -1;
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
        long long col = off - lastnl; /* lastnl=-1 on line 1 => col = off+1 */
        dprintf(2, "fkwu:%lld:%lld: %s: ", line, col, sev == FK_DIAG_ERR ? "error" : "warning");
    }
    __builtin_va_list ap;
    __builtin_va_start(ap, fmt);
    vdprintf(2, fmt, ap);
    __builtin_va_end(ap);
    dprintf(2, "\n");
}
/* Called ONCE, after parse completes and before execution begins: gcc-style
 * tally. Silent when clean, so the default happy path prints nothing new. */
static void fk_diag_flush(void) {
    if (fk_nerr > 0 || fk_nwarn > 0) {
        dprintf(2, "fkwu: %lld error(s), %lld warning(s)\n", fk_nerr, fk_nwarn);
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
static long long fk_smknode(long long t0, long long c1, long long c2, long long c3) {
    long long k = fk_node_count;
    fk_node_count = fk_node_count + 1;
    if (fk_node_count > FK_AST_NODE_CAP) {
        /* COMPILE-PHASE (parser allocator): program too large, not corruption.
         * DON'T mint a new node; clamp the count back and reuse the last valid slot
         * (a valid index, never OOB, so this is safe at runtime too). Diagnose ONCE
         * and HALT the parse by forcing EOF. Without the halt, a program whose parser
         * mints unbounded nodes re-hits this forever now that collect-and-continue
         * replaced the old fk_die -- MEASURED at 677,766 diagnostics in 6s on
         * resource-port.fk, a CPU-spin, not an OOM. Forcing fk_spos=fk_slen terminates
         * every parse loop (they all gate on fk_spos<fk_slen); the accumulated error
         * then refuses the run. This restores the old fk_die's BOUND as a clean stop. */
        fk_node_count = FK_AST_NODE_CAP;
        if (fk_ast_full == 0) {
            fk_diag(FK_DIAG_ERR, fk_spos,
                    "AST node table full at node %lld; program too large (raise FK_AST_NODE_CAP) -- halting parse",
                    (long long)FK_AST_NODE_CAP);
            fk_ast_full = 1;
        }
        fk_spos = fk_slen;
        return FK_AST_NODE_CAP - 1;
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
            fk_sb = realloc(fk_sb, fk_scap_b);
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
#define FK_BD_STACK_CAP 1024 /* fk_bd_*: max bindings simultaneously in scope during parse. Raised 128->1024 (2026-07-02): a single scope with many sequential lets (generated classifier programs) can exceed 128, and a silently dropped binding miscompiles variable references. Same raisable-constant class as FK_NODE_CAP; overflow now dies loudly instead of dropping a binding. */
static long long fk_bd_s[FK_BD_STACK_CAP], fk_bd_n[FK_BD_STACK_CAP], fk_bd_off[FK_BD_STACK_CAP], fk_bd_top, fk_maxslot;
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
    if (fk_bd_top >= FK_BD_STACK_CAP) {
        /* COMPILE-PHASE: too many bindings live in one scope. The old fear was a
         * SILENT drop miscompiling references; answer it by dropping LOUDLY, not
         * by halting. DECLINE the push (leave fk_bd_top at cap); this name then
         * misses fk_bd_lookup and lowers to the unbound default 0 / unresolved-
         * call witness -- axiom-5's existing recovery. The rest of the source is
         * still fully checked. */
        fk_diag(FK_DIAG_ERR, fk_spos,
                "[scope-overflow] binding '%.*s' dropped: more than %d bindings in scope "
                "(raise FK_BD_STACK_CAP); this name resolves to the unbound default",
                (int)n, fk_srctext + s, (int)FK_BD_STACK_CAP);
        return;
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
 * do's own parsing. These two helpers save/restore the actual slice, not just
 * the pointer. (Ported from sibling branch commit f99d3232.) */
static long long fk_bd_save_s[128], fk_bd_save_n[128], fk_bd_save_off[128];
static long long fk_bd_save(void) {
    long long top = fk_bd_top;
    long long i = 0;
    while (i < top) {
        fk_bd_save_s[i] = fk_bd_s[i];
        fk_bd_save_n[i] = fk_bd_n[i];
        fk_bd_save_off[i] = fk_bd_off[i];
        i = i + 1;
    }
    return top;
}
static void fk_bd_restore(long long top) {
    long long i = 0;
    while (i < top) {
        fk_bd_s[i] = fk_bd_save_s[i];
        fk_bd_n[i] = fk_bd_save_n[i];
        fk_bd_off[i] = fk_bd_save_off[i];
        i = i + 1;
    }
    fk_bd_top = top;
}
static long long fk_parse_do(void);
static long long fk_parse_top_do_value(void);
static void fk_parse_top(void);
/* stone 4: a function table. Each top-level (defn name ...) gets its own fn-index (>=1); a call to
 * a registered name lowers to tag 12 (call-by-index, single-arg). A non-defn top form is the root
 * (fn[0]). */
/* FK_TOP_FN_SYM_CAP now matches FK_FN_CAP. It was 256 ("deliberately smaller",
 * degrading to "unregistered, allocate fresh" past the cap) — but for any
 * cross-calling program the degradation is SILENT breakage, not grace: defn
 * number 257's name never registers, every call to it allocates a fresh
 * body-less index, and the call returns nothing with no diagnostic. Witnessed
 * 2026-07-02: a 258-defn direct-source learning chain returned garbage at
 * exactly this boundary (253 defns ran; +5 more crossed 256 and broke) — the
 * "direct-source function-table ceiling" several receipts had to duck under
 * was this constant. Three arrays x 4096 x 8B = 96KB, a trivial price. */
#define FK_TOP_FN_SYM_CAP FK_FN_CAP
static long long fk_fnsym_s[FK_TOP_FN_SYM_CAP], fk_fnsym_n[FK_TOP_FN_SYM_CAP],
    fk_fnidx[FK_TOP_FN_SYM_CAP], fk_fntop, fk_defn_next, fk_root, fk_fnar[FK_FN_CAP];
#define FK_TOP_CONST_CAP 512
static long long fk_const_s[FK_TOP_CONST_CAP], fk_const_n[FK_TOP_CONST_CAP],
    fk_const_node[FK_TOP_CONST_CAP], fk_const_top;
static long long fk_fn_lookup(long long s, long long n) {
    long long i = 0;
    while (i < fk_fntop) {
        if (fk_sym_eq2(s, n, fk_fnsym_s[i], fk_fnsym_n[i])) {
            return fk_fnidx[i];
        }
        i = i + 1;
    }
    return -1;
}
static long long fk_const_lookup(long long s, long long n) {
    long long i = fk_const_top;
    while (i > 0) {
        i = i - 1;
        if (fk_sym_eq2(s, n, fk_const_s[i], fk_const_n[i])) {
            return fk_const_node[i];
        }
    }
    return -1;
}
static void fk_const_set(long long s, long long n, long long node) {
    long long i = 0;
    while (i < fk_const_top) {
        if (fk_sym_eq2(s, n, fk_const_s[i], fk_const_n[i])) {
            fk_const_node[i] = node;
            return;
        }
        i = i + 1;
    }
    if (fk_const_top < FK_TOP_CONST_CAP) {
        fk_const_s[fk_const_top] = s;
        fk_const_n[fk_const_top] = n;
        fk_const_node[fk_const_top] = node;
        fk_const_top = fk_const_top + 1;
        return;
    }
    fk_die("fk_const_set: top-level constant table full");
}
static long long fk_parse_variadic(long long tag);
static long long fk_parse_fixed_list(long long n);
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
            fk_sskip();
            long long as2 = fk_spos;
            fk_spos = fk_sym_end(fk_spos);
            long long alen = fk_spos - as2;
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            /* SCOPE FIX: save/restore the enclosing do's live let-bindings around
             * this nested defn's own frame (see fk_bd_save above; sibling f99d3232). */
            long long fk_bd_saved_top = fk_bd_save();
            long long fk_bd_saved_maxslot = fk_maxslot;
            fk_bd_top = 0;
            fk_maxslot = 0;
            fk_bd_push(as2, alen, 0);
            long long body = fk_sparse();
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            if (fk_maxslot > 0) {
                body = fk_smknode(111, fk_smklit(fk_maxslot), body, 0);
            }
            fk_bd_restore(fk_bd_saved_top);
            fk_maxslot = fk_bd_saved_maxslot;
            return body;
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
            long long body = fk_sparse();
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
                /* arity-4+ ops (today only make_nodeid, tag 91) build a LIST-shaped node
                 * (child [1] = a cons list of the args, per flt-nodeid4 + the tag-91
                 * evaluator) that the generic 3-child parse path below CANNOT form -- the
                 * --src parser has no lowering for them; they are flatten-only. Without
                 * this case the 4th arg blocks the ')' check, fk_spos never advances, and
                 * the parse spins to the AST cap (MEASURED: 677k diagnostics/6s on
                 * resource-port.fk / shell-lower.fk). Diagnose PRECISELY, drain the balanced
                 * form so the parser stays synced, and decline to nothing. */
                fk_diag(FK_DIAG_ERR, s,
                        "op '%.*s' (arity %lld) is flatten-only; the --src parser cannot lower a 4+-arg op -- flatten this source to a .tbl instead",
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
         * inert. */
        long long fidx = fk_fn_lookup(s, hn);
        if (fidx >= 0) {
            /* GENERAL ARITY (no per-arity case): parse the callee's `ar` declared arg expressions
             * and thread them into a forward-linked arg-chain of tag-242 cells (cell:
             * [1]=arg-expr-node, [2]=next-cell or -1, head-first so slot 0 is the first arg). The
             * call is ONE tag-241 node ([1]=fidx, [2]=chain-head or -1). fk_walk evaluates the
             * chain left-to-right, pushing each arg via fk_vp exactly as the table path packs N
             * args — same mechanism, any N. ar==0 parses no args (chain -1, inert); ar==1/2/8 are
             * the same code. */
            long long ar = (fidx >= 0 && fidx < FK_FN_CAP) ? fk_fnar[fidx] : 1;
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
            return fk_smknode(241, fidx, chain, 0);
        }

        /* INDIRECT CALL (stone 2c): a call (h args..) whose head h is a BOUND NAME — a parameter,
         * or a let-var holding a fn returned from a fn — is an offer to a COMPUTED callee (axiom-5:
         * offer a computed cell). The head LOWERS to its slot read (tag 110); at eval it must
         * reduce to a fn-VALUE, and the fn it names is then offered with the args. Emit tag 244:
         * [1]=head-expr-node, [2]=arg-chain (242 cells, head-first), exactly the tag-241 shape but
         * with a computed head. Args are parsed until the close paren (the indirect callee's arity
         * is not a static name lookup); each is a forward-linked 242 cell so fk_walk threads them
         * left-to-right like the direct path. A bare fn-NAME never reaches here (it resolves at
         * fk_fn_lookup above into the direct tag-241 path). */
        long long hoff = fk_bd_lookup(s, hn);
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
                    fk_sb = realloc(fk_sb, fk_scap_b);
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
    long long cn = fk_const_lookup(s, fk_spos - s);
    if (cn >= 0) {
        return cn;
    }
    long long vfidx = fk_fn_lookup(s, fk_spos - s);
    if (vfidx >= 0) {
        return fk_smknode(243, vfidx, 0, 0);
    }
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
        if (fk_defn_next > FK_FN_CAP) {
            /* COMPILE-PHASE prescan: program-size limit, not corruption. Diagnose
             * and STOP registering (the fk_fntop<CAP guard below already skips the
             * write, and the idx<FK_FN_CAP guard skips the fk_fnar[idx] write);
             * keep scanning so EVERY over-cap defn is reported, not just the first.
             * Calls to unregistered names fall through to the unresolved-call
             * witness -- already a recovery path. */
            fk_diag(FK_DIAG_ERR, ns,
                    "[fn-cap] defn '%.*s' at #%lld exceeds FK_FN_CAP (%d); not registered",
                    (int)nlen, fk_srctext + ns, idx, (int)FK_FN_CAP);
        }
        if (fk_fntop < FK_TOP_FN_SYM_CAP) {
            fk_fnsym_s[fk_fntop] = ns;
            fk_fnsym_n[fk_fntop] = nlen;
            fk_fnidx[fk_fntop] = idx;
            fk_fntop = fk_fntop + 1;
        }

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
        if (idx >= 0 && idx < FK_FN_CAP) {
            fk_fnar[idx] = na;
        }
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
                fk_root = fk_parse_top_do_value();
                if (fk_maxslot > 0) {
                    fk_root = fk_smknode(111, fk_smklit(fk_maxslot), fk_root, 0);
                }
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
                if (fk_defn_next > FK_FN_CAP) {
                    /* COMPILE-PHASE: same capacity class as the prescan site.
                     * Diagnose, skip storing this body (the idx<FK_FN_CAP guards
                     * at fk_fnar[idx]/fk_fn[idx] below already decline the write),
                     * but keep consuming the whole defn form so the parse loop
                     * reaches the next top-level form. Every offender reported. */
                    fk_diag(FK_DIAG_ERR, ns2,
                            "[fn-cap] defn '%.*s' over FK_FN_CAP (%d); body not stored",
                            (int)nlen2, fk_srctext + ns2, (int)FK_FN_CAP);
                }
                if (fk_fntop < FK_TOP_FN_SYM_CAP) {
                    fk_fnsym_s[fk_fntop] = ns2;
                    fk_fnsym_n[fk_fntop] = nlen2;
                    fk_fnidx[fk_fntop] = idx;
                    fk_fntop = fk_fntop + 1;
                }
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
            long long na = 0;
            while (1) {
                fk_sskip();
                if (fk_spos >= fk_slen || fk_srctext[fk_spos] == FK_CH_RPAREN) {
                    break;
                }
                long long as = fk_spos;
                fk_spos = fk_sym_end(fk_spos);
                fk_bd_push(as, fk_spos - as, na);
                if (na > fk_maxslot) {
                    fk_maxslot = na;
                }
                na = na + 1;
            }
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            if (idx >= 0 && idx < FK_FN_CAP) {
                fk_fnar[idx] = na;
            }
            /* arity known before body -> self-recursive calls read it */
            long long body = fk_sparse();
            fk_sskip();
            if (fk_spos < fk_slen && fk_srctext[fk_spos] == FK_CH_RPAREN) {
                fk_spos = fk_spos + 1;
            }
            if (fk_maxslot > 0) {
                body = fk_smknode(111, fk_smklit(fk_maxslot), body, 0);
            }
            if (idx >= 0 && idx < FK_FN_CAP) {
                fk_fn[idx] = body;
            }
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
            fk_root = val3;
            return;
        }
    }
    fk_root = fk_sparse();
}
/* ══ IN-PROCESS SELF-JIT (proof-of-concept, integer-arithmetic + self-recursion) ══ When a --src
 * function is hot AND its whole body is in the lowerable family (literal / arg-slot / add / sub /
 * mul / le / eq / if / PURE-SELF-recursion), the running kernel lowers ITS node tree to x86-64
 * BYTES here, installs them executable via the existing fk_native_call HAL door, and dispatches the
 * call natively — the recursion runs entirely in native code (its own asm `call`), so the whole
 * recursive computation crystallizes, bit-identical and ~order-of-magnitude faster than the walk.
 * HONEST SCOPE: this C lowerer is a PROOF-OF-CONCEPT of the in-process fusion (Lane A's install
 * door + a real tree->bytes lowerer in one running process). The DESTINATION is Lane B's Form
 * emitter (model/form-asm-x64.fk / fkc-nat-expr) run in-process — the same recipe that proves
 * four-way lowering to asm bytes — NOT this C twin. This proves the wire end-to-end for one
 * op-family; it deliberately does NOT cover form-eval's hot string/list/cons ops, and it bails
 * (installs nothing) on any tag outside the family, so a non-lowerable function always falls back
 * to the tree-walker, byte-identical. ABI we emit: long long fn(long long *args) — args ptr in RCX
 * (Win64) / RDI (SysV); args[k] = tagged value of slot k; result tagged in RAX. RBP holds the args
 * ptr through the body. Recursion builds a fresh args array on the native stack and `call rel32`s
 * to fn's own entry (offset 0), so each frame is independent — runs on fkwu's 256MB thread stack
 * (FORM_KERNEL_STACK_MB), the same stack the walker recurses on. */
/* ── GENERAL primitive + inter-fn CARRIERS (the JIT calls these, not per-op asm) ── The reframe:
 * "string/float/list ops" are NOT special JIT features — they are RECIPES over primitives. The JIT
 * only needs (1) any-kind literals (handled by emitting the interned tagged word) and (2) the
 * ability to emit a CALL to a kind-correct carrier. Then ALL recipes lower as recipes; the
 * value-correctness lives in ONE place (the same computation fk_walk runs), so a lowered float add
 * can NEVER drift from the walker. fk_jprim2(tag,a,b) / fk_jprim1(tag,a) take ALREADY-EVALUATED
 * tagged words and return the tagged result — bit-identical to fk_walk's tag case. CORRECTNESS
 * NOTE: add/sub/mul/ le/eq are float-aware here (via fk_num/fk_fbox), so lowering them as carrier
 * calls is correct for float operands (the #59 int-inline was WRONG for floats — fchk would return
 * an integer-add answer). The int-inline survives only behind a provably-int fast path. */
static long long fk_isf(long long v);
static double fk_num(long long v);
static long long fk_fbox(double d);
static long long fk_walk_body(long long i, long long fp);
static long long fk_keyeq(long long a, long long b);
static long long fk_jprim2(long long tag, long long a, long long b) {
    if (tag == 3) {
        if (fk_isf(a) || fk_isf(b)) {
            return fk_fbox(fk_num(a) + fk_num(b));
        }
        return a + b;
    }
    if (tag == 4) {
        if (fk_isf(a) || fk_isf(b)) {
            return fk_fbox(fk_num(a) - fk_num(b));
        }
        return a - b;
    }
    if (tag == 42) {
        if (fk_isf(a) || fk_isf(b)) {
            return fk_fbox(fk_num(a) * fk_num(b));
        }
        return ((a >> 1) * (b >> 1)) << 1;
    }
    if (tag == 5) {
        return (fk_num(a) <= fk_num(b)) ? 2 : 0;
    }
    if (tag == 102) {
        return (fk_num(a) == fk_num(b)) ? 2 : 0;
    }
    if (tag == 103) {
        return (fk_num(a) < fk_num(b)) ? 2 : 0;
    }
    /* lt — mirrors fk_walk tag-103 */
    if (tag == 10) {
        if (fk_isf(a) || fk_isf(b)) {
            return fk_fbox(fk_num(a) / fk_num(b));
        }
        return ((a >> 1) / (b >> 1)) << 1;
    }
    /* div — mirrors fk_walk tag-10 (float-aware) */
    if (tag == 11) {
        if (fk_isf(a) || fk_isf(b)) {
            double x = fk_num(a);
            double y = fk_num(b);
            return fk_fbox(x - y * (double)((long long)(x / y)));
        }
        return ((a >> 1) % (b >> 1)) << 1;
    }
    /* mod — mirrors fk_walk tag-11 (float-aware) */
    if (tag == 27) {
        /* str_concat — mirrors fk_walk's tag-27 exactly */
        long long sa = a >> 1;
        long long sb = b >> 1;
        if (sa < 0 || sa >= fk_sp || sb < 0 || sb >= fk_sp) {
            return 0 - 2;
        }
        long long ln = fk_sl[sa] + fk_sl[sb];
        while (fk_sbp + ln > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = realloc(fk_sb, fk_scap_b);
            fk_sb_check();
        }
        long long off = fk_sbp;
        long long k = 0;
        while (k < fk_sl[sa]) {
            fk_sb[off + k] = fk_sb[fk_so[sa] + k];
            k = k + 1;
        }
        long long m = 0;
        while (m < fk_sl[sb]) {
            fk_sb[off + fk_sl[sa] + m] = fk_sb[fk_so[sb] + m];
            m = m + 1;
        }
        return fk_sintern(off, ln) << 1;
    }
    if (tag == 26) {
        return fk_keyeq(a >> 1, b >> 1) ? 2 : 0;
    }
    /* str_eq — mirrors fk_walk tag-26 */
    if (tag == 28) {
        /* str_byte_at — mirrors fk_walk tag-28 */
        long long sa = a >> 1;
        long long k = b >> 1;
        if (sa < 0 || sa >= fk_sp || k < 0 || k >= fk_sl[sa]) {
            return 0 - 2;
        }
        return ((long long)(unsigned char)fk_sb[fk_so[sa] + k]) << 1;
    }
    return fk_nothing;
}
static long long fk_jprim3(long long tag, long long a, long long b, long long c) {
    if (tag == 29) {
        /* substring — mirrors fk_walk tag-29 */
        long long sa = a >> 1;
        long long lo = b >> 1;
        long long hi = c >> 1;
        if (sa < 0 || sa >= fk_sp || lo < 0 || hi < lo || hi > fk_sl[sa]) {
            return 0 - 2;
        }
        long long ln = hi - lo;
        while (fk_sbp + ln > fk_scap_b) {
            fk_scap_b = fk_scap_b * 2;
            fk_sb = realloc(fk_sb, fk_scap_b);
            fk_sb_check();
        }
        long long j = 0;
        while (j < ln) {
            fk_sb[fk_sbp + j] = fk_sb[fk_so[sa] + lo + j];
            j = j + 1;
        }
        return fk_sintern(fk_sbp, ln) << 1;
    }
    return fk_nothing;
}
static long long fk_jprim1(long long tag, long long a) {
    if (tag == 25) {
        long long sa = a >> 1;
        if (sa < 0 || sa >= fk_sp) {
            return 0;
        }
        return fk_sl[sa] << 1;
    }
    /* str_len */
    if (tag == 54) {
        return ((long long)fk_num(a)) << 1;
    }
    /* float_to_int */
    if (tag == 53) {
        /* str_to_float — mirrors fk_walk's tag-53 exactly */
        long long sa = a >> 1;
        if (sa < 0 || sa >= fk_sp) {
            return fk_fbox(0.0);
        }
        char tmp[128];
        long long n = fk_sl[sa];
        if (n > 126) {
            n = 126;
        }
        long long j = 0;
        while (j < n) {
            tmp[j] = fk_sb[fk_so[sa] + j];
            j = j + 1;
        }
        tmp[n] = 0;
        return fk_fbox(strtod(tmp, 0));
    }
    return fk_nothing;
}
/* ── LIST/CONS carriers (tags 18-23) — pure value ops over the pair arena, taking ALREADY-EVALUATED
 * tagged words and returning the tagged result. Each mirrors fk_walk's tag case EXACTLY (same arena
 * guards, same melt-on-cons, same chain walk) so a lowered cons/head/tail/len/nth can NEVER drift
 * from the walker. cons (19) is the one that can allocate: it pushes h,t onto fk_vs (as the walker
 * does via fk_vp) BEFORE the melt check so the GC sees them as roots and relocates them, then reads
 * the relocated words back — bit-identical to the walker's tag-19 path. The JIT itself holds no
 * live pairs across this call (its intermediates are in registers/machine-stack), so fk_vs is the
 * correct, only root set, exactly as in the walker. */
static void fk_arena(void);
static void fk_melt(void);
static void fk_vp(long long v);
static long long fk_jlist2(long long tag, long long a, long long b) {
    if (tag == 19) {
        /* cons h t — mirrors fk_walk tag-19 exactly (fk_vp roots + melt + grow guard) */
        fk_vp(a);
        fk_vp(b);
        if (fk_cap == 0) {
            fk_arena();
        }
        if (fk_hp * 100 >= fk_cap * 90) {
            fk_melt();
        }
        if (fk_hp + 1 >= fk_cap) {
            fk_vsp = fk_vsp - 2;
            return 1;
        }
        fk_hp = fk_hp + 1;
        fk_hh[fk_hp] = fk_vs[fk_vsp - 2];
        fk_ht[fk_hp] = fk_vs[fk_vsp - 1];
        fk_vsp = fk_vsp - 2;
        return (fk_hp << 1) | 1;
    }
    if (tag == 23) {
        /* nth x k — mirrors fk_walk tag-23 (x evaluated, k evaluated) */
        long long p = a >> 1;
        long long k23 = b >> 1;
        while (p >= 1 && p <= fk_hp && k23 > 0) {
            p = fk_ht[p] >> 1;
            k23 = k23 - 1;
        }
        if (p < 1 || p > fk_hp) {
            return 1;
        }
        return fk_hh[p];
    }
    return fk_nothing;
}
static long long fk_jlist1(long long tag, long long a) {
    if (tag == 20) {
        long long p = a >> 1;
        if (p < 1 || p > fk_hp) {
            return 1;
        }
        return fk_hh[p];
    }
    /* head */
    if (tag == 21) {
        long long p = a >> 1;
        if (p < 1 || p > fk_hp) {
            return 1;
        }
        return fk_ht[p];
    }
    /* tail */
    if (tag == 22) {
        long long p = a >> 1;
        long long n = 0;
        while (p >= 1 && p <= fk_hp) {
            n = n + 1;
            p = fk_ht[p] >> 1;
        }
        return n << 1;
    }
    /* len */
    return fk_nothing;
}
/* ── ensure `callee` has an installed native; return its frame-slot count via *frame, or 0 if it
 * cannot crystallize. Heat-gated and tried-once so a non-lowering callee is marked and never
 * re-lowered (falls through to the walker). Shared by fk_jcall (the --src inter-fn site) and
 * fk_feval_try_native (the --feval trampoline) — one lower+install path, one cache (fk_src_nat /
 * fk_nat_exec / fk_nat_tried / fk_fheat). Lowering also sets fk_jit_frame as a side-effect; we
 * capture and stash it per-callee in fk_src_nat_frame so the direct-dispatch caller knows the frame
 * size without re-lowering. */
static long long fk_src_nat_frame[FK_FN_CAP];
/* eager=1 (the --src inter-fn site): being CALLED from JITed code is itself the heat signal —
 * crystallize on first request so a mutual-recursion pair flips native↔native immediately and does
 * not stay in the walker after one deopt. eager=0 (the --feval trampoline): heat-gate as before so
 * cold fns don't pay lowering cost. */
static fk_natfn fk_ensure_native_ex(long long callee, long long *frame, int eager) {
    if (callee < 0 || callee >= FK_FN_CAP) {
        return 0;
    }
    if (fk_nat_exec[callee] != 0) {
        if (frame) {
            *frame = fk_src_nat_frame[callee];
        }
        return fk_nat_exec[callee];
    }
    if (fk_src_nat[callee] == 0) {
        if (fk_nat_tried[callee]) {
            return 0;
        }
        if (!eager) {
            fk_fheat[callee] = fk_fheat[callee] + 1;
            if (fk_fheat[callee] < fk_feval_hot) {
                return 0;
            }
        }
        fk_nat_tried[callee] = 1;
        long long n = fk_jit_lower(callee);
        if (n <= 0) {
            return 0;
        }
        long long fr = fk_jit_frame;
        unsigned char *img = malloc(n);
        if (img == 0) {
            return 0;
        }
        long long ci = 0;
        while (ci < n) {
            img[ci] = fk_jb[ci];
            ci = ci + 1;
        }
        fk_src_nat[callee] = img;
        fk_src_nat_len[callee] = n;
        fk_src_nat_frame[callee] = fr;
        fk_njit = fk_njit + 1;
        if (fk_conf("FK_JIT_WITNESS")) {
            printf("[jit] fn%lld crystallized: %lld bytes, njit=%lld (direct dispatch ready)\n",
                   callee, n, fk_njit);
        }
    }
    if (fk_nat_exec[callee] == 0) {
        fk_nat_exec[callee] = fk_nat_install(fk_src_nat[callee], fk_src_nat_len[callee]);
        if (fk_nat_exec[callee] == 0) {
            return 0;
        }
    }
    if (frame) {
        *frame = fk_src_nat_frame[callee];
    }
    return fk_nat_exec[callee];
}
static fk_natfn fk_ensure_native(long long callee, long long *frame) {
    return fk_ensure_native_ex(callee, frame, 0);
}
/* inter-fn CALL carrier: dispatch fn `callee` with `argc` args. The lowered call to another fn
 * routes here, so a JITed fn can call ANY other fn correctly. DIRECT NATIVE→NATIVE dispatch (the
 * speed lever): if the callee already has an installed native AND the call arity matches the arity
 * that native was lowered for, set up the rooted args frame at fk_vs[fp..] and JUMP STRAIGHT to the
 * native entry — no fk_walk_body bounce. The args (live tagged values, possibly cons) are spilled
 * into fk_vs[fp..fp+argc) and fk_vsp is raised over the WHOLE frame (args + let-locals), so a
 * compacting fk_melt triggered inside the callee scans these slots as roots and RELOCATES the cons
 * pointers the native stores there (GC-correct, exactly as fk_feval_try_native does). On arity
 * mismatch or no-native → fall through to fk_walk_body (the walker stays source of truth; deopt is
 * always safe). */
/* ── NATIVE TAIL-CALL TRAMPOLINE ────────────────────────────────────────────────────────── A
 * direct native→native CALL recurses on the C machine stack: fine for TREE recursion (fib depth =
 * tree height, small) and bounded LINEAR recursion, but a deep TAIL chain (ev/od 20M deep) would
 * blow the 256 MB stack. The walker solves this with a trampoline (constant stack); the native path
 * needs its own. The lowerer emits a TAIL-position inter-fn call NOT as a call but as: write the
 * new args into the current rbp frame IN PLACE, record the next callee in fk_tail_callee, and
 * RETURN the fk_tailcall sentinel. The C driver fk_jtramp loops on that sentinel — dispatching the
 * next native over the SAME fp frame — so a mutual-recursion chain runs native↔native end-to-end in
 * CONSTANT stack, no walker bounce. fk_tailcall is a reserved ODD-NEGATIVE sentinel in a band no
 * tagged value occupies: ints are even (v<<1); floats are <= fk_fbase-2 (~-9e18); nodes/records are
 * small-magnitude negatives; cons are positive-odd; fnvals sit in the -8e18±16384 band; nothing is
 * -8.999e18. -7.5e18-1 is odd, far above the float floor, far below node magnitudes, and outside
 * the fnval band — so it can never collide with a real result. */
static const long long fk_tailcall = -7500000000000000001LL;
static long long fk_tail_callee = -1;
/* gate: emit the tail-trampoline sentinel form for tail inter-fn calls. ON for the --src JIT path
 * (where native↔native tail-chaining is proven correct + bounded under melt). OFF for the --feval
 * interpreter path, which keeps the #75 wire (tail inter-fn calls lower as non-tail fk_jcall →
 * walker bounce): correct + bounded for the meta-evaluator's heavy env-consing, which the native
 * chain does not yet root register-resident cons across. (Honest scope: the register-spill rung
 * named in the #75 receipt is what would let --feval chain natively too.) */
static long long fk_lower_tail_tramp = 0;
/* recorded by the native tail site just before it returns the sentinel; read by the trampoline. */
static void fk_jtail_set(long long callee) {
    fk_tail_callee = callee;
}
/* drive a native call at frame `fp` (args already in fk_vs[fp..fp+argc)); loop while the native
 * returns the tail sentinel, dispatching the recorded next callee over the SAME frame. argc is the
 * fixed chain arity; a tail target whose declared arity differs, or that can't crystallize, deopts
 * to the walker (which itself trampolines) and ends the native chain — always correct. */
static long long fk_jtramp(long long callee, long long fp, long long argc) {
    long long save_vsp = fk_vsp;
    for (;;) {
        /* out-of-range callee is a hard "no such function" -- checked and returned BEFORE the
         * arity check below, which itself reads fk_fnar[callee] and would be its own out-of-bounds
         * access on an invalid callee. A prior version folded both into one `||` chain whose
         * fallback path (deopt to fk_walk_body(fk_fn[callee], fp)) relied on callee being in range
         * even when it wasn't -- this keeps the mismatched-arity deopt (callee valid, wrong arity)
         * but no longer falls through to fk_fn[callee] on a callee that was never valid. */
        if (callee < 0 || callee >= FK_FN_CAP) {
            fk_vsp = save_vsp;
            return fk_nothing;
        }
        if (fk_fnar[callee] != argc) {
            long long r = fk_walk_body(fk_fn[callee], fp);
            fk_vsp = save_vsp;
            return r;
        }
        long long frame = 0;
        fk_natfn nf = fk_ensure_native_ex(callee, &frame, 1);
        /* eager: a call FROM jit IS the heat */
        if (nf == 0) {
            long long r = fk_walk_body(fk_fn[callee], fp);
            fk_vsp = save_vsp;
            return r;
        }
        long long fr = frame;
        if (fr < argc) {
            fr = argc;
        }

        /* HEADROOM: this native may end in a TAIL inter-fn call that writes up-to-6 args into
         * [rbp+0..6) (= fk_vs[fp..fp+6)) BEFORE the next iteration recomputes the frame. If this
         * fn's own frame is smaller than the tail target's arity, those writes would land above the
         * rooted region. Reserve at least the max lowered arity so every in-place tail rewrite
         * stays inside the melt-scanned, non-overlapping frame. (Generous reserve is always safe:
         * sub-calls take fp2=fk_vsp above it; melt scans it; cost is a few vsp slots per chain.) */
        if (fr < 6) {
            fr = 6;
        }
        long long z = argc;
        while (z < fr) {
            if (fp + z < FK_VALUE_STACK_CAP) {
                fk_vs[fp + z] = 0;
            }
            z = z + 1;
        }
        long long need = fp + fr;
        if (need > fk_vsp && need < FK_VALUE_STACK_CAP) {
            fk_vsp = need;
        }
        fk_tail_callee = -1;
        long long r = nf(&fk_vs[fp]);
        if (r == fk_tailcall) {
            callee = fk_tail_callee;
            continue;
        }
        /* args already rewritten into fk_vs[fp..] */
        fk_vsp = save_vsp;
        return r;
    }
}
/* inter-fn CALL carrier (NON-tail call site). Runs its own nested trampoline so a tail-call
 * sentinel raised inside the callee's chain is absorbed here and never escapes past this frame. The
 * args (live tagged values, possibly cons) are spilled into fk_vs[fp..fp+argc); fk_jtramp raises
 * fk_vsp over the whole frame so a compacting fk_melt scans these slots as roots and RELOCATES the
 * cons pointers the native stores there (GC-correct, as fk_feval_try_native). */
static long long fk_jcall(long long callee, long long argc, const long long *args) {
    if (callee < 0 || callee >= FK_FN_CAP) {
        return fk_nothing;
    }
    long long fp = fk_vsp;
    long long k = 0;
    while (k < argc) {
        fk_vs[fk_vsp] = args[k];
        fk_vsp = fk_vsp + 1;
        k = k + 1;
    }

    /* Direct native↔native dispatch (via the trampoline) is engaged only on the --src JIT path
     * (fk_lower_tail_tramp). On the --feval path the inter-fn wire stays the #75 walker bounce —
     * correct + bounded for the meta-evaluator's env-consing, which the native chain does not yet
     * root register-resident cons across. */
    long long r =
        fk_lower_tail_tramp ? fk_jtramp(callee, fp, argc) : fk_walk_body(fk_fn[callee], fp);
    fk_vsp = fp;
    return r;
}
/* fk_jb, fk_jbp, and fk_jit_frame are already declared earlier in the file,
 * near fk_jtramp -- this used to re-declare all three here too. */
static long long fk_jit_self;
/* fn index being lowered: self-calls must target it */
static int fk_jit_ok;
/* cleared to 0 by emit on any unsupported shape */
static long long fk_jit_entry;
/* byte offset of the post-prologue entry (TCO jmp target) */
static void fk_jb1(unsigned char x) {
    /* fk_jbp counts past capacity even when the write itself is dropped, so the
     * "did this function's code fit" check downstream (fk_jbp > FK_JIT_CODE_BUF_CAP)
     * still sees the true logical size and can fall back to non-JIT execution. */
    if (fk_jbp < FK_JIT_CODE_BUF_CAP) {
        fk_jb[fk_jbp] = x;
    }
    fk_jbp = fk_jbp + 1;
}
static void fk_jb4(int x) {
    fk_jb1(x & 0xff);
    fk_jb1((x >> 8) & 0xff);
    fk_jb1((x >> 16) & 0xff);
    fk_jb1((x >> 24) & 0xff);
}
static void fk_jb8(long long x) {
    int k = 0;
    while (k < 8) {
        fk_jb1((x >> (8 * k)) & 0xff);
        k = k + 1;
    }
}
static void fk_jpatch4(long long at, int v) {
    fk_jb[at] = v & 0xff;
    fk_jb[at + 1] = (v >> 8) & 0xff;
    fk_jb[at + 2] = (v >> 16) & 0xff;
    fk_jb[at + 3] = (v >> 24) & 0xff;
}
static void fk_jemit(long long i, int tail);
static void fk_jbin(long long i) {
    fk_jemit(fk_node[i][1], 0);
    fk_jb1(0x50);
    /* eval left -> rax ; push rax */
    fk_jemit(fk_node[i][2], 0);
    fk_jb1(0x48);
    fk_jb1(0x89);
    fk_jb1(0xC1);
    /* eval right -> rax ; mov rcx,rax */
    fk_jb1(0x58);
    /* pop rax (left) -> rax=left, rcx=right */
}
/* ── emit a CALL to a C carrier at absolute address `fn`, with `nargs` (0..3) args already pushed
 * on the machine stack (last-pushed = argN-1, so they pop in order). Robust stack realignment
 * (works at any incoming alignment): save rsp, align to 16, pad + shadow, pop the staged args into
 * the ABI arg registers, call, restore rsp. Result (tagged word) is left in rax. Win64 ABI: args in
 * rcx,rdx,r8 (+32B shadow); SysV: rdi,rsi,rdx (no shadow). The carrier preserves rbp
 * (callee-saved), so the args pointer survives the call. */
static void fk_jcarrier(void *fn, int nargs) {
/* operands are on the stack: caller pushed arg_{n-1} first ... arg0 last, so arg0 = [rsp+0], arg1 =
 * [rsp+8], ... Read them into the ABI arg registers, then realign + call, then restore rsp to ABOVE
 * the consumed args (rsp + nargs*8). */
#if defined(_WIN32)
    if (nargs >= 1) {
        fk_jb1(0x48);
        fk_jb1(0x8B);
        fk_jb1(0x0C);
        fk_jb1(0x24);
    }
    /* mov rcx,[rsp] */
    if (nargs >= 2) {
        fk_jb1(0x48);
        fk_jb1(0x8B);
        fk_jb1(0x54);
        fk_jb1(0x24);
        fk_jb1(8);
    }
    /* mov rdx,[rsp+8] */
    if (nargs >= 3) {
        fk_jb1(0x4C);
        fk_jb1(0x8B);
        fk_jb1(0x44);
        fk_jb1(0x24);
        fk_jb1(16);
    }
    /* mov r8,[rsp+16] */
    if (nargs >= 4) {
        fk_jb1(0x4C);
        fk_jb1(0x8B);
        fk_jb1(0x4C);
        fk_jb1(0x24);
        fk_jb1(24);
    }
/* mov r9,[rsp+24] */
#else
    if (nargs >= 1) {
        fk_jb1(0x48);
        fk_jb1(0x8B);
        fk_jb1(0x3C);
        fk_jb1(0x24);
    }
    /* mov rdi,[rsp] */
    if (nargs >= 2) {
        fk_jb1(0x48);
        fk_jb1(0x8B);
        fk_jb1(0x74);
        fk_jb1(0x24);
        fk_jb1(8);
    }
    /* mov rsi,[rsp+8] */
    if (nargs >= 3) {
        fk_jb1(0x48);
        fk_jb1(0x8B);
        fk_jb1(0x54);
        fk_jb1(0x24);
        fk_jb1(16);
    }
    /* mov rdx,[rsp+16] */
    if (nargs >= 4) {
        fk_jb1(0x48);
        fk_jb1(0x8B);
        fk_jb1(0x4C);
        fk_jb1(0x24);
        fk_jb1(24);
    }
/* mov rcx,[rsp+24] */
#endif

    /* compute the post-consume rsp into r11 (caller-saved, but we use it only here): */
    fk_jb1(0x4C);
    fk_jb1(0x8D);
    fk_jb1(0x5C);
    fk_jb1(0x24);
    fk_jb1((unsigned char)(nargs * 8));
    /* lea r11,[rsp+nargs*8] */
    fk_jb1(0x48);
    fk_jb1(0x83);
    fk_jb1(0xE4);
    fk_jb1(0xF0);
    /* and rsp,-16 (align down) */
    fk_jb1(0x41);
    fk_jb1(0x53);
    /* push r11 (save restore-target) */
    fk_jb1(0x41);
    fk_jb1(0x53);
/* push r11 (pad -> rsp 16-aligned) */
#if defined(_WIN32)
    fk_jb1(0x48);
    fk_jb1(0x83);
    fk_jb1(0xEC);
    fk_jb1(32);
/* sub rsp,32 (Win64 shadow; keeps 16-align) */
#endif
    fk_jb1(0x48);
    fk_jb1(0xB8);
    fk_jb8((long long)(unsigned long long)fn);
    /* mov rax,&fn */
    fk_jb1(0xFF);
    fk_jb1(0xD0);
/* call rax (result -> rax) */
#if defined(_WIN32)
    fk_jb1(0x48);
    fk_jb1(0x83);
    fk_jb1(0xC4);
    fk_jb1(32);
/* add rsp,32 */
#endif
    fk_jb1(0x48);
    fk_jb1(0x83);
    fk_jb1(0xC4);
    fk_jb1(8);
    /* add rsp,8 (discard pad) */
    fk_jb1(0x5C);
    /* pop rsp (rsp <- saved restore-target) */
}
static long long fk_jprim2(long long tag, long long a, long long b);
static long long fk_jprim1(long long tag, long long a);
static long long fk_jprim3(long long tag, long long a, long long b, long long c);
static long long fk_jlist2(long long tag, long long a, long long b);
static long long fk_jlist1(long long tag, long long a);
static long long fk_jcall(long long callee, long long argc, const long long *args);
/* tail self-call (sum-shaped): compute all new args into temporaries (machine stack), write them
 * into the rbp args array IN PLACE, then jmp to the post-prologue entry. Constant stack — the
 * native twin of fk_walk_body's trampoline. */
static void fk_jemit(long long i, int tail) {
    long long t = fk_node[i][0];
    if (t == 111) {
        fk_jemit(fk_node[i][2], tail);
        return;
    }
    /* reserve: pass tail through to the body */
    if (t == 1) {
        fk_jb1(0x48);
        fk_jb1(0xB8);
        fk_jb8(fk_node[i][1] << 1);
        return;
    }
    /* int lit -> mov rax,imm64(tagged) */
    if (t == 24) {
        fk_jb1(0x48);
        fk_jb1(0xB8);
        fk_jb8(fk_node[i][1] << 1);
        return;
    }
    /* STRING lit: tagged word = poolidx<<1 (known at lower-time, interned at parse) */
    if (t == 2) {
        fk_jb1(0x48);
        fk_jb1(0x8B);
        fk_jb1(0x85);
        fk_jb4(0);
        return;
    }
    /* slot 0 -> mov rax,[rbp+0] */
    if (t == 110) {
        long long li = fk_node[i][1];
        if (fk_node[li][0] != 1) {
            fk_jit_ok = 0;
            return;
        }
        /* slot index must be a literal */
        long long slot = fk_node[li][1];
        fk_jb1(0x48);
        fk_jb1(0x8B);
        fk_jb1(0x85);
        fk_jb4((int)(slot * 8));
        return;
        /* mov rax,[rbp+slot*8] */
    }
    if (t == 3 || t == 4 || t == 42 || t == 5 || t == 102) {
        /* float-aware arith/cmp. The #59 int-inline is WRONG for float operands, so we GUARD at
         * runtime: if EITHER operand is a boxed float (tagged word <= fk_fbase-2, a huge negative),
         * call the kind-correct carrier fk_jprim2 (bit-identical to fk_walk); else run the fast
         * int-inline. This keeps fac/sum/fib native-fast (provably int at runtime) AND makes
         * 0.5+0.25 correct — the float-correctness gate. */
        fk_jbin(i);
        /* rax=left, rcx=right */

        /* threshold = fk_fbase - 2 (the float-band ceiling) */
        fk_jb1(0x49);
        fk_jb1(0xB9);
        fk_jb8(-9000000000000000000LL - 2);
        /* mov r9,fk_fbase-2 */
        fk_jb1(0x4C);
        fk_jb1(0x39);
        fk_jb1(0xC8);
        /* cmp rax,r9 */
        fk_jb1(0x0F);
        fk_jb1(0x8E);
        long long jf1 = fk_jbp;
        fk_jb4(0);
        /* jle FLOAT (left is float) */
        fk_jb1(0x4C);
        fk_jb1(0x39);
        fk_jb1(0xC9);
        /* cmp rcx,r9 */
        fk_jb1(0x0F);
        fk_jb1(0x8E);
        long long jf2 = fk_jbp;
        fk_jb4(0);
        /* jle FLOAT (right is float) */

        /* ── INT fast path (rax=left, rcx=right) ── */
        if (t == 3) {
            fk_jb1(0x48);
            fk_jb1(0x01);
            fk_jb1(0xC8);
        }
        /* add rax,rcx */
        else if (t == 4) {
            fk_jb1(0x48);
            fk_jb1(0x29);
            fk_jb1(0xC8);
        }
        /* sub rax,rcx */
        else if (t == 42) {
            fk_jb1(0x48);
            fk_jb1(0xD1);
            fk_jb1(0xF8);
            fk_jb1(0x48);
            fk_jb1(0xD1);
            fk_jb1(0xF9);
            fk_jb1(0x48);
            fk_jb1(0x0F);
            fk_jb1(0xAF);
            fk_jb1(0xC1);
            fk_jb1(0x48);
            fk_jb1(0xD1);
            fk_jb1(0xE0);
        }
        /* mul */
        else {
            /* le (5) / eq (102) */
            fk_jb1(0x48);
            fk_jb1(0xC7);
            fk_jb1(0xC2);
            fk_jb4(2);
            fk_jb1(0x49);
            fk_jb1(0xC7);
            fk_jb1(0xC0);
            fk_jb4(0);
            fk_jb1(0x48);
            fk_jb1(0x39);
            fk_jb1(0xC8);
            if (t == 5) {
                fk_jb1(0x4C);
                fk_jb1(0x0F);
                fk_jb1(0x4E);
                fk_jb1(0xC2);
            } else {
                fk_jb1(0x4C);
                fk_jb1(0x0F);
                fk_jb1(0x44);
                fk_jb1(0xC2);
            }
            fk_jb1(0x4C);
            fk_jb1(0x89);
            fk_jb1(0xC0);
        }
        fk_jb1(0xE9);
        long long jdone = fk_jbp;
        fk_jb4(0);
        /* jmp DONE */

        /* ── FLOAT path: call fk_jprim2(tag, left, right) ── */
        long long fp_lbl = fk_jbp;
        fk_jpatch4(jf1, (int)(fp_lbl - (jf1 + 4)));
        fk_jpatch4(jf2, (int)(fp_lbl - (jf2 + 4)));
        fk_jb1(0x51);
        /* push rcx (arg2=right) */
        fk_jb1(0x50);
        /* push rax (arg1=left) */
        fk_jb1(0x48);
        fk_jb1(0xC7);
        fk_jb1(0xC0);
        fk_jb4((int)t);
        /* mov rax,tag */
        fk_jb1(0x50);
        /* push rax (arg0=tag) */
        fk_jcarrier((void *)fk_jprim2, 3);
        /* result in rax */
        fk_jpatch4(jdone, (int)(fk_jbp - (jdone + 4)));
        /* DONE */
        return;
    }
    if (t == 10 || t == 11 || t == 103) {
        /* div / mod / lt: 2-arg carrier fk_jprim2(tag,a,b), float-aware (mirrors fk_walk) */
        fk_jemit(fk_node[i][1], 0);
        fk_jb1(0x50);
        /* a -> push */
        fk_jemit(fk_node[i][2], 0);
        fk_jb1(0x50);
        /* b -> push (top) */

        /* stack top->down: b,a. fk_jprim2 args: arg0=tag, arg1=a, arg2=b. Re-stage. */
        fk_jb1(0x59);
        /* pop rcx (b) */
        fk_jb1(0x58);
        /* pop rax (a) */
        fk_jb1(0x51);
        /* push rcx (arg2=b) */
        fk_jb1(0x50);
        /* push rax (arg1=a) */
        fk_jb1(0x48);
        fk_jb1(0xC7);
        fk_jb1(0xC0);
        fk_jb4((int)t);
        /* mov rax,tag */
        fk_jb1(0x50);
        /* push rax (arg0=tag) */
        fk_jcarrier((void *)fk_jprim2, 3);
        return;
    }
    if (t == 25 || t == 54 || t == 53) {
        /* str_len / float_to_int / str_to_float: 1-arg carrier */
        fk_jemit(fk_node[i][1], 0);
        /* arg -> rax */
        fk_jb1(0x50);
        /* push rax (arg1=val) */
        fk_jb1(0x48);
        fk_jb1(0xC7);
        fk_jb1(0xC0);
        fk_jb4((int)t);
        /* mov rax,tag */
        fk_jb1(0x50);
        /* push rax (arg0=tag) */
        fk_jcarrier((void *)fk_jprim1, 2);
        /* result in rax */
        return;
    }
    if (t == 27 || t == 26 || t == 28) {
        /* str_concat / str_eq / str_byte_at: 2-arg carrier */
        fk_jemit(fk_node[i][1], 0);
        fk_jb1(0x50);
        /* left -> push */
        fk_jemit(fk_node[i][2], 0);
        fk_jb1(0x50);
        /* right -> push (top) */

        /* fk_jprim2(tag,a,b): arg0=tag@[rsp], arg1=a@[rsp+8], arg2=b@[rsp+16]. Stack top->down is
         * now right,left. Re-stage to tag,left,right. */
        fk_jb1(0x59);
        /* pop rcx (right) */
        fk_jb1(0x58);
        /* pop rax (left) */
        fk_jb1(0x51);
        /* push rcx (arg2=right=b) */
        fk_jb1(0x50);
        /* push rax (arg1=left=a) */
        fk_jb1(0x48);
        fk_jb1(0xC7);
        fk_jb1(0xC0);
        fk_jb4((int)t);
        /* mov rax,tag */
        fk_jb1(0x50);
        /* push rax (arg0=tag) */
        fk_jcarrier((void *)fk_jprim2, 3);
        return;
    }
    if (t == 29) {
        /* substring: 3-arg carrier fk_jprim3(tag,a,b,c) */
        fk_jemit(fk_node[i][1], 0);
        fk_jb1(0x50);
        /* a -> push */
        fk_jemit(fk_node[i][2], 0);
        fk_jb1(0x50);
        /* b -> push */
        fk_jemit(fk_node[i][3], 0);
        fk_jb1(0x50);
        /* c -> push (top) */

        /* stack top->down: c,b,a. fk_jprim3 args: arg0=tag, arg1=a, arg2=b, arg3=c. Re-stage. */
        fk_jb1(0x41);
        fk_jb1(0x5A);
        /* pop r10 (c) */
        fk_jb1(0x59);
        /* pop rcx (b) */
        fk_jb1(0x58);
        /* pop rax (a) */
        fk_jb1(0x41);
        fk_jb1(0x52);
        /* push r10 (arg3=c) */
        fk_jb1(0x51);
        /* push rcx (arg2=b) */
        fk_jb1(0x50);
        /* push rax (arg1=a) */
        fk_jb1(0x48);
        fk_jb1(0xC7);
        fk_jb1(0xC0);
        fk_jb4(29);
        /* mov rax,29 */
        fk_jb1(0x50);
        /* push rax (arg0=tag) */
        fk_jcarrier((void *)fk_jprim3, 4);
        return;
    }
    if (t == 18) {
        /* empty: nil value = 1 (no carrier needed) */
        fk_jb1(0x48);
        fk_jb1(0xC7);
        fk_jb1(0xC0);
        fk_jb4(1);
        /* mov rax,1 */
        return;
    }
    if (t == 20 || t == 21 || t == 22) {
        /* head/tail/len: 1-arg list carrier fk_jlist1(tag,a) */
        fk_jemit(fk_node[i][1], 0);
        /* arg -> rax */
        fk_jb1(0x50);
        /* push rax (arg1=val) */
        fk_jb1(0x48);
        fk_jb1(0xC7);
        fk_jb1(0xC0);
        fk_jb4((int)t);
        /* mov rax,tag */
        fk_jb1(0x50);
        /* push rax (arg0=tag) */
        fk_jcarrier((void *)fk_jlist1, 2);
        return;
    }
    if (t == 19 || t == 23) {
        /* cons/nth: 2-arg list carrier fk_jlist2(tag,a,b) */
        fk_jemit(fk_node[i][1], 0);
        fk_jb1(0x50);
        /* a -> push */
        fk_jemit(fk_node[i][2], 0);
        fk_jb1(0x50);
        /* b -> push (top) */

        /* stack top->down: b,a. fk_jlist2 args: arg0=tag, arg1=a, arg2=b. Re-stage. */
        fk_jb1(0x59);
        /* pop rcx (b) */
        fk_jb1(0x58);
        /* pop rax (a) */
        fk_jb1(0x51);
        /* push rcx (arg2=b) */
        fk_jb1(0x50);
        /* push rax (arg1=a) */
        fk_jb1(0x48);
        fk_jb1(0xC7);
        fk_jb1(0xC0);
        fk_jb4((int)t);
        /* mov rax,tag */
        fk_jb1(0x50);
        /* push rax (arg0=tag) */
        fk_jcarrier((void *)fk_jlist2, 3);
        return;
    }
    if (t == 109) {
        /* let slot val body: store val into rbp[slot], eval body */
        long long li = fk_node[i][1];
        if (fk_node[li][0] != 1) {
            fk_jit_ok = 0;
            return;
        }
        /* slot index must be a literal (it always is) */
        long long slot = fk_node[li][1];
        fk_jemit(fk_node[i][2], 0);
        /* val -> rax (never tail) */
        fk_jb1(0x48);
        fk_jb1(0x89);
        fk_jb1(0x85);
        fk_jb4((int)(slot * 8));
        /* mov [rbp+slot*8],rax */
        fk_jemit(fk_node[i][3], tail);
        /* body — tail position preserved */
        return;
    }
    if (t == 6) {
        /* if test then else */
        fk_jemit(fk_node[i][1], 0);
        /* test (never tail) */
        fk_jb1(0x48);
        fk_jb1(0x85);
        fk_jb1(0xC0);
        /* test rax,rax */
        fk_jb1(0x0F);
        fk_jb1(0x84);
        long long jz = fk_jbp;
        fk_jb4(0);
        /* jz else (patch) */
        fk_jemit(fk_node[i][2], tail);
        /* then — tail position preserved */
        fk_jb1(0xE9);
        long long je = fk_jbp;
        fk_jb4(0);
        /* jmp end (skip else). harmless dead code if then ended in a tail-jmp. */
        long long elsep = fk_jbp;
        fk_jpatch4(jz, (int)(elsep - (jz + 4)));
        fk_jemit(fk_node[i][3], tail);
        /* else — tail position preserved */
        fk_jpatch4(je, (int)(fk_jbp - (je + 4)));
        /* end = here (the pop rbp; ret follows) */
        return;
    }
    if (t == 7 || t == 12 || t == 240 || t == 241) {
        /* fn call: SELF-recursion native; OTHER-fn via carrier */
        long long callee = (t == 7) ? fk_jit_self : fk_node[i][1];
        long long argc;
        long long an[6];
        {
            long long zi = 0;
            while (zi < 6) {
                an[zi] = -1;
                zi = zi + 1;
            }
        }
        if (t == 241) {
            /* variadic call: args in a 242-cons chain */
            long long cell = fk_node[i][2];
            long long cnt = 0;
            while (cell >= 0 && fk_node[cell][0] == 242) {
                if (cnt < 6) {
                    an[cnt] = fk_node[cell][1];
                }
                cnt = cnt + 1;
                cell = fk_node[cell][2];
            }
            if (cnt > 6) {
                if (fk_conf("FK_JIT_WITNESS")) {
                    printf("[jit-bail] call arity %lld > 6 at node %lld\n", cnt, i);
                }
                fk_jit_ok = 0;
                return;
            }
            /* lowers arity 0..6 */
            argc = cnt;
        } else {
            argc = (t == 240) ? 2 : 1;
            an[0] = (t == 7) ? fk_node[i][1] : fk_node[i][2];
            an[1] = (t == 240) ? fk_node[i][3] : -1;
        }
        if (callee != fk_jit_self) {
            if (tail && argc <= 6 && fk_lower_tail_tramp) {
                /* ── TAIL inter-fn call → native trampoline form (the deep-mutual-recursion lever).
                 * Instead of a recursive fk_jcall (which grows the C stack), rewrite the new args
                 * INTO the current rbp frame IN PLACE (exactly like the tail SELF-call), record the
                 * next callee via fk_jtail_set, and RETURN the fk_tailcall sentinel. The enclosing
                 * fk_jtramp loops on that sentinel over the SAME frame → constant stack,
                 * native↔native end-to-end. */
                {
                    long long k = 0;
                    while (k < argc) {
                        fk_jemit(an[k], 0);
                        fk_jb1(0x50);
                        k = k + 1;
                    }
                }
                /* push arg0..argN-1 (top=argN-1) */
                {
                    long long k = argc;
                    while (k > 0) {
                        k = k - 1;
                        fk_jb1(0x58);
                        /* pop rax (arg k) */
                        fk_jb1(0x48);
                        fk_jb1(0x89);
                        fk_jb1(0x45);
                        fk_jb1((unsigned char)(k * 8));
                    }
                }
                /* mov [rbp+k*8],rax */
                fk_jb1(0x48);
                fk_jb1(0xC7);
                fk_jb1(0xC0);
                fk_jb4((int)callee);
                fk_jb1(0x50);
                /* mov rax,callee ; push (arg0 to carrier) */
                fk_jcarrier((void *)fk_jtail_set, 1);
                /* fk_jtail_set(callee); clobbers rax */
                fk_jb1(0x48);
                fk_jb1(0xB8);
                fk_jb8(fk_tailcall);
                /* mov rax,fk_tailcall sentinel (the body's tail value) */
                return;
            }

            /* ── NON-TAIL inter-function call: emit a call to fk_jcall(callee, argc, argsptr). The
             * carrier dispatches the callee (native via its own trampoline, or walker on deopt), so
             * a JITed fn can call ANY other fn correctly — for ANY arity 0..6. We build the
             * evaluated-args array on the machine stack (arg0 at the lowest address) and pass its
             * pointer; fk_jcarrier's `and rsp,-16` only moves rsp DOWN, leaving this array intact
             * above it. */
            long long k = argc;
            while (k > 0) {
                k = k - 1;
                fk_jemit(an[k], 0);
                fk_jb1(0x50);
            }
            /* push argN-1 ... arg0 (arg0 ends on top = lowest addr) */

            /* args array now at [rsp .. rsp+argc*8); arg0=[rsp]. Capture ptr, then stage carrier
             * args. */
            if (argc == 0) {
                fk_jb1(0x48);
                fk_jb1(0x89);
                fk_jb1(0xE0);
            }
            /* mov rax,rsp (ptr; empty array, unread) */
            else {
                fk_jb1(0x48);
                fk_jb1(0x89);
                fk_jb1(0xE0);
            }
            /* mov rax,rsp (argsptr) */
            fk_jb1(0x50);
            /* push rax (arg2=argsptr, staged last carrier-arg slot) */
            fk_jb1(0x48);
            fk_jb1(0xC7);
            fk_jb1(0xC0);
            fk_jb4((int)argc);
            fk_jb1(0x50);
            /* mov rax,argc; push (arg1) */
            fk_jb1(0x48);
            fk_jb1(0xC7);
            fk_jb1(0xC0);
            fk_jb4((int)callee);
            fk_jb1(0x50);
            /* mov rax,callee; push (arg0) */

            /* stack top->down: callee, argc, argsptr, [arg0..argN-1]. fk_jcarrier reads 3 carrier
             * args from [rsp],[rsp+8],[rsp+16] and restores rsp to rsp+3*8 — exactly past the 3
             * staged carrier args, leaving the argc*8 args region to be cleaned below. */
            fk_jcarrier((void *)fk_jcall, 3);
            /* result in rax */
            if (argc > 0) {
                fk_jb1(0x48);
                fk_jb1(0x83);
                fk_jb1(0xC4);
                fk_jb1((unsigned char)(argc * 8));
            }
            /* add rsp,argc*8 (drop args array) */
            return;
        }

        /* ── SELF-recursion (callee == self): native call/jmp, as in the #59 POC. Arity 1..3. */
        if (argc < 1) {
            if (fk_conf("FK_JIT_WITNESS")) {
                printf("[jit-bail] 0-arg self-recursion at node %lld\n", i);
            }
            fk_jit_ok = 0;
            return;
        }
        /* 0-arg self-recursion not handled here */

        /* evaluate new args into temporaries (machine stack), pushed arg0 first ... argN-1 last */
        {
            long long k = 0;
            while (k < argc) {
                fk_jemit(an[k], 0);
                fk_jb1(0x50);
                k = k + 1;
            }
        }
        if (tail) {
            /* TAIL self-call: write new args into the rbp array IN PLACE, jmp entry. Constant stack
             * — the native twin of the walker's trampoline; sum(1000000) runs flat. Stack top =
             * argN-1. */
            long long k = argc;
            while (k > 0) {
                k = k - 1;
                fk_jb1(0x58);
                /* pop rax (arg k) */
                fk_jb1(0x48);
                fk_jb1(0x89);
                fk_jb1(0x45);
                fk_jb1((unsigned char)(k * 8));
                /* mov [rbp+k*8],rax */
            }
            fk_jb1(0xE9);
            long long js = fk_jbp;
            fk_jb4(0);
            /* jmp entry */
            fk_jpatch4(js, (int)(fk_jit_entry - (js + 4)));
            return;
        }

        /* NON-tail self-call (e.g. fac's (mul n (fac …))): real native recursion. The temporaries
         * (pushed arg0 first ... argN-1 last) are in REVERSE array order on the stack (top=argN-1,
         * deepest=arg0). Reserve a FRAME*8 args region BELOW them (frame = args + let-locals, so
         * the callee's let stores have room), copy each temp into its args[k] slot (let-slots stay
         * scratch), pass rcx = args ptr, native call to offset 0. */
        {
            long long fr = fk_jit_frame;
            if (fr < argc) {
                fr = argc;
            }
            fk_jb1(0x48);
            fk_jb1(0x81);
            fk_jb1(0xEC);
            fk_jb4((int)(fr * 8));
            /* sub rsp,frame*8 (args[] region below the temps) */

            /* temps now sit above the region: temp(argN-1) at [rsp+fr*8+0], ... arg0 at
             * [rsp+fr*8+(argc-1)*8]. Copy temp -> args[k]. */
            {
                long long k = 0;
                while (k < argc) {
                    long long src = fr * 8 + ((argc - 1 - k) * 8);
                    /* temp holding arg k */
                    fk_jb1(0x48);
                    fk_jb1(0x8B);
                    fk_jb1(0x84);
                    fk_jb1(0x24);
                    fk_jb4((int)src);
                    /* mov rax,[rsp+src] */
                    fk_jb1(0x48);
                    fk_jb1(0x89);
                    fk_jb1(0x84);
                    fk_jb1(0x24);
                    fk_jb4((int)(k * 8));
                    /* mov [rsp+k*8],rax */
                    k = k + 1;
                }
            }
            fk_jb1(0x48);
            fk_jb1(0x89);
            fk_jb1(0xE1);
/* mov rcx,rsp (args ptr) */
#if defined(_WIN32)
            fk_jb1(0x48);
            fk_jb1(0x83);
            fk_jb1(0xEC);
            fk_jb1(32);
/* sub rsp,32 (Win64 shadow) */
#endif
            fk_jb1(0xE8);
            long long cs = fk_jbp;
            fk_jb4(0);
            /* call rel32 -> offset 0 (full prologue sets rbp) */
            fk_jpatch4(cs, (int)(0 - (cs + 4)));
#if defined(_WIN32)
            fk_jb1(0x48);
            fk_jb1(0x83);
            fk_jb1(0xC4);
            fk_jb1(32);
/* add rsp,32 (undo shadow) */
#endif
            fk_jb1(0x48);
            fk_jb1(0x81);
            fk_jb1(0xC4);
            fk_jb4((int)(fr * 8 + argc * 8));
            /* add rsp,frame*8+temps (drop both) */
        }
        return;
    }
    if (fk_conf("FK_JIT_WITNESS")) {
        printf("[jit-bail] unsupported tag %lld at node %lld\n", t, i);
    }
    fk_jit_ok = 0;
    /* any other tag: not in the lowerable family — bail */
}
/* fk_jit_frame (declared above): number of frame slots fn f needs (args + let-locals). The args
 * array the ENTRY and every native self-call build must be this many longs, else a let store (mov
 * [rbp+slot*8]) or a slot read writes/reads past the array. Captured from the body's reserve
 * wrapper (tag 111, slot-count literal) when present; else = arity. */
/* lower fn f's body into fk_jb; returns length if the whole tree is in-family, else 0. */
static long long fk_jit_lower(long long f) {
    /* every current call site already validates f before calling in, but fk_fn[f]
     * below was read unconditionally while the fk_fnar[f] read two lines down was
     * already guarded -- check once, up front, so both reads share one invariant. */
    if (f < 0 || f >= FK_FN_CAP) {
        return 0;
    }
    fk_jbp = 0;
    fk_jit_ok = 1;
    fk_jit_self = f;

    /* frame size = max(arity, maxslot+1). maxslot lives in the reserve wrapper (tag 111). */
    {
        long long body = fk_fn[f];
        long long fr = fk_fnar[f];
        if (body >= 0 && fk_node[body][0] == 111) {
            long long li = fk_node[body][1];
            if (li >= 0 && fk_node[li][0] == 1) {
                long long ms = fk_node[li][1] + 1;
                if (ms > fr) {
                    fr = ms;
                }
            }
        }
        if (fr < 1) {
            fr = 1;
        }
        fk_jit_frame = fr;
    }
    fk_jb1(0x55);
/* push rbp */
#if defined(_WIN32)
    fk_jb1(0x48);
    fk_jb1(0x89);
    fk_jb1(0xCD);
/* mov rbp,rcx (args ptr) */
#else
    fk_jb1(0x48);
    fk_jb1(0x89);
    fk_jb1(0xFD);
/* mov rbp,rdi (args ptr) */
#endif
    fk_jit_entry = fk_jbp;
    /* TCO jmp target: rbp already = args ptr */
    fk_jemit(fk_fn[f], 1);
    /* body in TAIL position -> rax */
    fk_jb1(0x5D);
    /* pop rbp */
    fk_jb1(0xC3);
    /* ret */
    if (fk_jit_ok == 0 || fk_jbp > FK_JIT_CODE_BUF_CAP) {
        return 0;
    }
    return fk_jbp;
}
/* install fk_jb[0..n) executable and call it with an args array (tagged values). */
static long long fk_native_call_args(const unsigned char *code, long long n, long long *args) {
#if defined(_WIN32)
    void *mem = VirtualAlloc(0, (unsigned long long)n, 0x3000, 0x04);
    if (mem == 0) {
        return fk_nothing;
    }
    long long k = 0;
    while (k < n) {
        ((unsigned char *)mem)[k] = code[k];
        k = k + 1;
    }
    unsigned int old = 0;
    VirtualProtect(mem, (unsigned long long)n, 0x20, &old);
#else
#if defined(__x86_64__) || defined(__amd64__)
    void *mem = mmap(0, (unsigned long)n, 0x3, 0x1002, -1, 0);
    if (mem == (void *)-1) {
        return fk_nothing;
    }
    long long k = 0;
    while (k < n) {
        ((unsigned char *)mem)[k] = code[k];
        k = k + 1;
    }
    if (mprotect(mem, (unsigned long)n, 0x5) != 0) {
        return fk_nothing;
    }
    long long (*fn)(long long *) = (long long (*)(long long *))mem;
    return fn(args);
#else
    (void)code;
    (void)n;
    (void)args;
    return fk_nothing;
#endif
#endif
#if defined(_WIN32)
    long long (*fn)(long long *) = (long long (*)(long long *))mem;
    return fn(args);
#endif
}
/* install a crystallized image to an executable page ONCE; the caller caches the returned pointer
 * (no per-call VirtualAlloc). Returns 0 on failure. fk_natfn typedef'd earlier. */
static fk_natfn fk_nat_install(const unsigned char *code, long long n) {
#if defined(_WIN32)
    void *mem = VirtualAlloc(0, (unsigned long long)n, 0x3000, 0x04);
    if (mem == 0) {
        return 0;
    }
    long long k = 0;
    while (k < n) {
        ((unsigned char *)mem)[k] = code[k];
        k = k + 1;
    }
    unsigned int old = 0;
    VirtualProtect(mem, (unsigned long long)n, 0x20, &old);
#else
#if defined(__x86_64__) || defined(__amd64__)
    void *mem = mmap(0, (unsigned long)n, 0x3, 0x1002, -1, 0);
    if (mem == (void *)-1) {
        return 0;
    }
    long long k = 0;
    while (k < n) {
        ((unsigned char *)mem)[k] = code[k];
        k = k + 1;
    }
    if (mprotect(mem, (unsigned long)n, 0x5) != 0) {
        return 0;
    }
    return (fk_natfn)mem;
#else
    (void)code;
    (void)n;
    return 0;
#endif
#endif
#if defined(_WIN32)
    return (fk_natfn)mem;
#endif
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
#define FK_SRC_DEP_CAP 128
#define FK_SRC_HASH_CAP 16384
static char fk_src_dep_path[FK_SRC_DEP_CAP][4096];
static long long fk_src_dep_mtime[FK_SRC_DEP_CAP];
static long long fk_src_dep_size[FK_SRC_DEP_CAP];
static long long fk_src_dep_parent[FK_SRC_DEP_CAP];
static long long fk_src_dep_end[FK_SRC_DEP_CAP];
static long long fk_src_dep_count;
static char fk_src_root_path[4096];
static char fk_src_root_text[FK_SOURCE_TEXT_CAP];
static long long fk_src_root_len;

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
    long long pre_n = 0;
    long long s2 = 0;
    while (pre_n == 0 && owner_path[s2] != 0) {
        if (owner_path[s2] == FK_CH_SLASH &&
            ((owner_path[s2 + 1] == 'f' && owner_path[s2 + 2] == 'o' && owner_path[s2 + 3] == 'r' &&
              owner_path[s2 + 4] == 'm' && owner_path[s2 + 5] == FK_CH_SLASH) ||
             (owner_path[s2 + 1] == 'l' && owner_path[s2 + 2] == 'e' && owner_path[s2 + 3] == 'a' &&
              owner_path[s2 + 4] == 'r' && owner_path[s2 + 5] == 'n' && owner_path[s2 + 6] == FK_CH_SLASH))) {
            pre_n = s2 + 1;
        }
        s2 = s2 + 1;
    }
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
    if (!fk_source_hash_append(out, cap, &pos, "fk-unit-v1")) {
        return 0;
    }
    while (i < end && i < fk_src_dep_count) {
        if (!fk_source_hash_append(out, cap, &pos, "|") ||
            !fk_source_hash_append(out, cap, &pos, fk_src_dep_path[i]) ||
            !fk_source_hash_append(out, cap, &pos, "@") ||
            !fk_source_hash_append_ll(out, cap, &pos, fk_src_dep_mtime[i]) ||
            !fk_source_hash_append(out, cap, &pos, ":") ||
            !fk_source_hash_append_ll(out, cap, &pos, fk_src_dep_size[i])) {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}
static int fk_src_unit_hash(char *out, long long cap) {
    return fk_src_unit_hash_range(0, fk_src_dep_count, out, cap);
}
static int fk_src_line_is_bare_import_fk(const char *text, long long line_start, long long line_end);
static int fk_src_append_text(const char *path, const char *text, long long n) {
    long long line_start = 0;
    if (fk_slen + n + 2 >= FK_SOURCE_TEXT_CAP) {
        fk_diag_path("error", path, "combined .fk dependency unit exceeds FK_SOURCE_TEXT_CAP");
        return 0;
    }
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
    char dep_path[4096];
    if (!fk_path_resolve_fk_dep(owner_path, text + start, n, dep_path, 4096)) {
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
                        if (!fk_src_prelude_fk_token(text, start, tn)) {
                            break;
                        }
                        char dep_path[4096];
                        if (!fk_path_resolve_fk_dep(owner_path, text + start, tn, dep_path, 4096)) {
                            fk_diag_path("error", owner_path, "prelude path exceeds buffer");
                            return 0;
                        }
                        if (!fk_src_collect_file(dep_path, owner_idx)) {
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
    long long got = fk_read_all_bounded(fd, fk_buf, FK_PARSE_BUF_CAP - 1);
    close(fd);
    if (got < 0) {
        if (got == -2) {
            fk_diag_path("error", path, "dependency source exceeds FK_PARSE_BUF_CAP");
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
    if (fk_src_dep_count >= FK_SRC_DEP_CAP) {
        free(owned);
        fk_diag_path("error", path, "too many .fk dependencies");
        return 0;
    }
    long long idx = fk_src_dep_count;
    fk_cstr_copy(fk_src_dep_path[fk_src_dep_count], path, 4096);
    fk_src_dep_mtime[fk_src_dep_count] = mtime;
    fk_src_dep_size[fk_src_dep_count] = size;
    fk_src_dep_parent[fk_src_dep_count] = parent_idx;
    fk_src_dep_end[fk_src_dep_count] = fk_src_dep_count + 1;
    fk_src_dep_count = fk_src_dep_count + 1;
    if (fk_cstr_eq(path, fk_src_root_path)) {
        if (got + 1 > FK_SOURCE_TEXT_CAP) {
            free(owned);
            fk_diag_path("error", path, "root source exceeds FK_SOURCE_TEXT_CAP");
            return 0;
        }
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
    if (!fk_src_append_text(path, owned, got)) {
        free(owned);
        return 0;
    }
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
    fk_cstr_copy(fk_src_root_path, root_path, 4096);
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
static int fk_fkb_write_u8(int fd, long long v) {
    unsigned char b = (unsigned char)(v & 255);
    return fk_write_all_raw(fd, &b, 1);
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
    return fk_write_all_raw(fd, b, 4);
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
    return fk_fkb_write_u32(fd, n) && fk_write_all_raw(fd, s, (unsigned long)n);
}
static int fk_fkb_write_bytes(int fd, const char *s, long long n) {
    return fk_fkb_write_u32(fd, n) && fk_write_all_raw(fd, s, (unsigned long)n);
}
static int fk_fkb_write_srctext_slice(int fd, long long start, long long n) {
    if (start < 0 || n < 0 || start + n > fk_slen) {
        return 0;
    }
    return fk_fkb_write_bytes(fd, fk_srctext + start, n);
}
static long long fk_src_symbol_id_for_fn(long long fnidx) {
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
    long long i = 0;
    while (i < fk_fntop) {
        long long fi = fk_fnidx[i];
        if (fi >= 0 && fi < FK_FN_CAP && fk_fn[fi] == node) {
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
    /* compile-errors records fk_nerr at image-write time, placed right after
     * the version line so readers find it in the first bytes; a cached run
     * replays this count as its exit truth (absent line reads as 0) */
    int hn = sprintf(line, "program-image-sym-lens-v1\ncompile-errors %lld\nsource ", fk_nerr);
    if (!fk_write_all_raw(fd, line, (unsigned long)hn) ||
        !fk_write_all_raw(fd, src_path, (unsigned long)fk_path_len(src_path)) ||
        !fk_write_all_raw(fd, "\nfkb ", 5) ||
        !fk_write_all_raw(fd, fkb_path, (unsigned long)fk_path_len(fkb_path)) ||
        !fk_write_all_raw(fd, "\nsource-hash ", 13) ||
        !fk_write_all_raw(fd, source_hash, (unsigned long)fk_path_len(source_hash)) ||
        !fk_write_all_raw(fd, "\n", 1)) {
        close(fd);
        return 0;
    }
    long long dep_i = 0;
    while (dep_i < fk_src_dep_count) {
        int n = sprintf(line, "dependency %lld mtime %lld size %lld path ", dep_i,
                        fk_src_dep_mtime[dep_i], fk_src_dep_size[dep_i]);
        if (!fk_write_all_raw(fd, line, (unsigned long)n) ||
            !fk_write_all_raw(fd, fk_src_dep_path[dep_i],
                              (unsigned long)fk_path_len(fk_src_dep_path[dep_i])) ||
            !fk_write_all_raw(fd, "\n", 1)) {
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
        if (!fk_write_all_raw(fd, line, (unsigned long)n) ||
            !fk_write_all_raw(fd, fk_srctext + name_s, (unsigned long)name_n) ||
            !fk_write_all_raw(fd, "\n", 1)) {
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
            long long target = (dep_fn >= 0 && dep_fn < FK_FN_CAP) ? fk_fn[dep_fn] : -1;
            int n = sprintf(line, "node %lld defines %lld depends %lld target %lld\n", node,
                            defined, dep_sym, target);
            if (!fk_write_all_raw(fd, line, (unsigned long)n)) {
                close(fd);
                return 0;
            }
        }
        node = node + 1;
    }
    close(fd);
    return 1;
}
static int fk_src_write_fkb(const char *src_path, const char *fkb_path, const char *sym_path,
                            long long source_mtime, const char *source_hash) {
#if defined(_WIN32)
    int fd = open(fkb_path, O_WRONLY | O_CREAT | O_TRUNC | 0x8000, 0666);
#else
    int fd = open(fkb_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
#endif
    if (fd < 0) {
        return 0;
    }
    fk_fkb_write_overflow = 0;
    int ok = 1;
    ok = ok && fk_write_all_raw(fd, "FKPIFB1", 7);
    ok = ok && fk_fkb_write_u8(fd, 0);
    ok = ok && fk_fkb_write_u32(fd, 4);
    ok = ok && fk_fkb_write_cstr(fd, src_path);
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
    while (ok && i < fk_sp) {
        ok = fk_fkb_write_bytes(fd, fk_sb + fk_so[i], fk_sl[i]);
        i = i + 1;
    }
    ok = ok && fk_fkb_write_signed(fd, fk_fntop);
    i = 0;
    while (ok && i < fk_fntop) {
        long long fnidx = fk_fnidx[i];
        long long arity = (fnidx >= 0 && fnidx < FK_FN_CAP) ? fk_fnar[fnidx] : 0;
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
            long long target = (dep_fn >= 0 && dep_fn < FK_FN_CAP) ? fk_fn[dep_fn] : -1;
            ok = fk_fkb_write_signed(fd, dep_sym) && fk_fkb_write_signed(fd, target);
        }
        i = i + 1;
    }
    close(fd);
    if (!ok) {
        /* a partial image looks fresh by mtime and poisons the next run's
         * load ("truncated artifact") -- leave no half-written artifact */
        unlink(fkb_path);
        return 0;
    }
    if (!fk_src_write_sym_text(sym_path, src_path, fkb_path, source_hash)) {
        unlink(fkb_path);
        unlink(sym_path);
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
        fk_scap_s = fk_scap_s * 2;
        fk_so = realloc(fk_so, fk_scap_s * 8);
        fk_sl = realloc(fk_sl, fk_scap_s * 8);
        fk_snext = realloc(fk_snext, fk_scap_s * 8);
        if (fk_so == 0 || fk_sl == 0 || fk_snext == 0) {
            fk_die("fk_fkb: out of memory growing string table");
        }
    }
    while (fk_sbp + n > fk_scap_b) {
        fk_scap_b = fk_scap_b * 2;
        fk_sb = realloc(fk_sb, fk_scap_b);
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
    if (tag == 6 || tag == 79 || tag == 109) {
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
    if ((tag == 240 || tag == 241) && field >= 2) {
        return value + node_base;
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
    if (fk_slen + n + 4 >= FK_SOURCE_TEXT_CAP) {
        return 0;
    }
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
    long long got = fk_read_all_bounded(fd, fk_buf, FK_PARSE_BUF_CAP);
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
    if (version < 4) {
        return 0;
    }
    /* Every identity read must execute unconditionally: these advance the
     * decode stream. A short-circuit here (the old `ok && read(...)` shape)
     * skipped the hash read after a src-path mismatch and desynced every
     * later read into "truncated string" -- the wrong-CWD reproduction. */
    int src_path_matches = fk_fkb_read_string_matches_cstr(expected_src_path);
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
    if (sealed != 1 || !source_identity_ok) {
        fk_diag_path("warning", fkb_path,
                     "stale .fkb (stored source identity does not match, e.g. written from a "
                     "different working directory); rebuilding from source");
        return 0;
    }
    long long nf = fk_fkb_read_signed();
    if (nf < 1 || fk_defn_next + nf - 1 > FK_FN_CAP) {
        return 0;
    }
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
    if (nr < 0 || fk_node_count + nr > FK_AST_NODE_CAP) {
        free(fn_roots);
        return 0;
    }
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
        if (old_fnidx > 0 && fk_fntop < FK_TOP_FN_SYM_CAP) {
            long long new_fnidx = fk_fkb_remap_fn(old_fnidx, fn_base);
            fk_fnsym_s[fk_fntop] = name_s;
            fk_fnsym_n[fk_fntop] = name_n;
            fk_fnidx[fk_fntop] = new_fnidx;
            if (new_fnidx >= 0 && new_fnidx < FK_FN_CAP) {
                fk_fnar[new_fnidx] = arity;
            }
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
    return 1;
}
static void fk_fkb_skip_symbol_image(long long version) {
    long long symbol_count = fk_fkb_read_signed();
    long long i = 0;
    while (!fk_fkb_bad && i < symbol_count) {
        (void)fk_fkb_read_signed();
        if (version >= 3) {
            (void)fk_fkb_read_signed();
            (void)fk_fkb_read_signed();
        }
        fk_fkb_skip_string();
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
    long long got = fk_read_all_bounded(fd, fk_buf, FK_PARSE_BUF_CAP);
    close(fd);
    if (got < 0) {
        fk_fkb_begin(0);
        fk_fkb_mark_bad("artifact exceeds FK_PARSE_BUF_CAP or is unreadable");
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
    if (version == 2 || version == 3) {
        /* pre-v4 lane width: superseded, not corrupt -- invalidate so the
         * caller recompiles from source and overwrites with a v4 artifact */
        fk_fkb_mark_bad("pre-v4 artifact lane; superseded");
        return 0;
    }
    if (version != 4) {
        fk_fkb_mark_bad("unsupported version");
        return 0;
    }
    /* Identity reads execute unconditionally -- they advance the decode
     * stream; short-circuiting them desyncs every later read (see
     * fk_src_import_fkb_image). Mismatch stays a soft "rebuild" verdict. */
    int source_identity_ok = 1;
    if (expected_src_path != 0) {
        if (!fk_fkb_read_string_matches_cstr(expected_src_path)) {
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
    if (nf < 0 || nf > FK_FN_CAP) {
        fk_fkb_mark_bad("function count exceeds capacity");
        return 0;
    }
    fk_fn_count = nf;
    long long i = 0;
    while (!fk_fkb_bad && i < nf) {
        fk_fn[i] = fk_fkb_read_signed();
        i = i + 1;
    }
    long long nr = fk_fkb_read_signed();
    if (nr < 0 || nr > FK_AST_NODE_CAP) {
        fk_fkb_mark_bad("node count exceeds capacity");
        return 0;
    }
    fk_node_count = nr;
    i = 0;
    while (!fk_fkb_bad && i < nr) {
        fk_node[i][0] = fk_fkb_read_signed();
        fk_node[i][1] = fk_fkb_read_signed();
        fk_node[i][2] = fk_fkb_read_signed();
        fk_node[i][3] = fk_fkb_read_signed();
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
    fk_fkb_skip_symbol_image(version);
    if (fk_fkb_bad) {
        return 0;
    }
    if (fk_fkb_pos != fk_fkb_len) {
        fk_fkb_mark_bad("trailing bytes");
        return 0;
    }
    fk_defn_next = fk_fn_count;
    fk_fntop = 0;
    fk_const_top = 0;
    fk_root = fk_fn_count > 0 ? fk_fn[0] : -1;
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
    fk_pv_root(fk_fn[0], fk_walk(fk_fn[0], 0));
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
static void fk_src_reset_compile_state(void) {
    fk_arg_n = 0;
    fk_fname_n = 0;
    fk_fn_count = 1;
    fk_node_count = 0;
    fk_ast_full = 0;
    fk_bd_top = 0;
    fk_maxslot = 0;
    fk_nerr = 0;
    fk_nwarn = 0;
    fk_src_truncated = 0;
    fk_string_table_reset();
    fk_fntop = 0;
    fk_const_top = 0;
    fk_defn_next = 1;
    fk_root = -1;
}
static void fk_src_compile_current_unit(const char *path, const char *fkb_path,
                                        const char *sym_path, long long unit_mtime,
                                        const char *source_hash) {
    fk_spos = 0;
    fk_srctext[fk_slen] = 0;
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
    char compile_path[4096];
    fk_cstr_copy(compile_path, path, 4096);
    long long saved_dep_count = fk_src_dep_count;
    char saved_root_path[4096];
    long long saved_root_len = fk_src_root_len;
    char *saved_root_text = malloc(FK_SOURCE_TEXT_CAP);
    char *saved_srctext = malloc(FK_SOURCE_TEXT_CAP);
    char (*saved_dep_path)[4096] = malloc(sizeof(fk_src_dep_path));
    long long *saved_dep_mtime = malloc(sizeof(fk_src_dep_mtime));
    long long *saved_dep_size = malloc(sizeof(fk_src_dep_size));
    long long *saved_dep_parent = malloc(sizeof(fk_src_dep_parent));
    long long *saved_dep_end = malloc(sizeof(fk_src_dep_end));
    if (saved_root_text == 0 || saved_srctext == 0 || saved_dep_path == 0 ||
        saved_dep_mtime == 0 || saved_dep_size == 0 || saved_dep_parent == 0 ||
        saved_dep_end == 0) {
        fk_die("fk_import_compile: out of memory saving source unit");
    }
    fk_cstr_copy(saved_root_path, fk_src_root_path, 4096);
    long long saved_slen = fk_slen;
    long long i = 0;
    while (i < FK_SOURCE_TEXT_CAP) {
        saved_root_text[i] = fk_src_root_text[i];
        saved_srctext[i] = fk_srctext[i];
        i = i + 1;
    }
    i = 0;
    while (i < FK_SRC_DEP_CAP) {
        fk_cstr_copy(saved_dep_path[i], fk_src_dep_path[i], 4096);
        saved_dep_mtime[i] = fk_src_dep_mtime[i];
        saved_dep_size[i] = fk_src_dep_size[i];
        saved_dep_parent[i] = fk_src_dep_parent[i];
        saved_dep_end[i] = fk_src_dep_end[i];
        i = i + 1;
    }
    char source_hash[FK_SRC_HASH_CAP];
    char fkb_path[4096];
    char sym_path[4096];
    long long unit_mtime = 0;
    int ok = 0;
    if (fk_path_replace_ext(compile_path, ".fkb", fkb_path, 4096) &&
        fk_path_replace_ext(compile_path, ".sym", sym_path, 4096) &&
        fk_src_load_unit(compile_path, source_hash, FK_SRC_HASH_CAP, &unit_mtime)) {
        fk_src_reset_compile_state();
        fk_src_compile_current_unit(compile_path, fkb_path, sym_path, unit_mtime, source_hash);
        ok = 1;
    }
    fk_src_dep_count = saved_dep_count;
    fk_cstr_copy(fk_src_root_path, saved_root_path, 4096);
    fk_src_root_len = saved_root_len;
    fk_slen = saved_slen;
    i = 0;
    while (i < FK_SOURCE_TEXT_CAP) {
        fk_src_root_text[i] = saved_root_text[i];
        fk_srctext[i] = saved_srctext[i];
        i = i + 1;
    }
    i = 0;
    while (i < FK_SRC_DEP_CAP) {
        fk_cstr_copy(fk_src_dep_path[i], saved_dep_path[i], 4096);
        fk_src_dep_mtime[i] = saved_dep_mtime[i];
        fk_src_dep_size[i] = saved_dep_size[i];
        fk_src_dep_parent[i] = saved_dep_parent[i];
        fk_src_dep_end[i] = saved_dep_end[i];
        i = i + 1;
    }
    free(saved_root_text);
    free(saved_srctext);
    free(saved_dep_path);
    free(saved_dep_mtime);
    free(saved_dep_size);
    free(saved_dep_parent);
    free(saved_dep_end);
    return ok;
}
static int fk_src_try_import_fkb_images(const char *root_path) {
    long long direct_count = 0;
    long long i = 1;
    while (i < fk_src_dep_count) {
        if (fk_src_dep_parent[i] == 0) {
            direct_count = direct_count + 1;
        }
        i = i + 1;
    }
    if (direct_count == 0) {
        return 0;
    }
    i = 1;
    while (i < fk_src_dep_count) {
        if (fk_src_dep_parent[i] == 0) {
            char dep_fkb_path[4096];
            long long dep_end = fk_src_dep_end[i];
            long long dep_mtime = fk_src_unit_mtime_range(i, dep_end);
            if (!fk_path_replace_ext(fk_src_dep_path[i], ".fkb", dep_fkb_path, 4096)) {
                return 0;
            }
            if (fk_path_mtime_raw(dep_fkb_path) < dep_mtime ||
                fk_src_fkb_version_raw(dep_fkb_path) < 4) {
                if (!fk_src_compile_artifact_only(fk_src_dep_path[i])) {
                    return 0;
                }
            }
        }
        i = i + 1;
    }
    fk_src_reset_compile_state();
    fk_slen = 0;
    fk_srctext[0] = 0;
    i = 1;
    while (i < fk_src_dep_count) {
        if (fk_src_dep_parent[i] == 0) {
            char dep_fkb_path[4096];
            char dep_hash[FK_SRC_HASH_CAP];
            long long dep_end = fk_src_dep_end[i];
            long long dep_mtime = fk_src_unit_mtime_range(i, dep_end);
            if (!fk_path_replace_ext(fk_src_dep_path[i], ".fkb", dep_fkb_path, 4096)) {
                return 0;
            }
            if (!fk_src_unit_hash_range(i, dep_end, dep_hash, FK_SRC_HASH_CAP)) {
                return 0;
            }
            {
                /* a dep image compiled with recovered errors is degraded truth;
                 * importing it would bake the degradation invisibly into this
                 * run -- refuse (unknown record counts as degraded), so the
                 * caller falls back to the flat compile where the full chain
                 * resolves */
                char dep_sym_path[4096];
                if (!fk_path_replace_ext(fk_src_dep_path[i], ".sym", dep_sym_path, 4096) ||
                    fk_src_sym_recorded_errors(dep_sym_path) != 0) {
                    return 0;
                }
            }
            if (!fk_src_import_fkb_image(dep_fkb_path, fk_src_dep_path[i], dep_hash, dep_mtime)) {
                return 0;
            }
            if (fk_conf("FK_IMPORT_TRACE")) {
                fk_diag_path("trace", dep_fkb_path, "loaded import .fkb");
            }
        }
        i = i + 1;
    }
    if (!fk_src_append_text(root_path, fk_src_root_text, fk_src_root_len)) {
        return 0;
    }
    return 1;
}
/* installed native body + length per fn, for --src crystallization. fk_src_nat
 * and fk_src_nat_len are already declared earlier in the file, near
 * fk_nat_exec -- this used to re-declare both here too (a harmless duplicate
 * under C's tentative-definition rules, but redundant); root-caused rather than
 * left, since the earlier declaration already covers this use. */
static int fk_run_src(const char *path, long long arg) {
    char fkb_path[4096];
    char sym_path[4096];
    char dylib_path[4096];
    char expected_source_hash[FK_SRC_HASH_CAP];
    long long unit_mtime = 0;
    if (!fk_path_replace_ext(path, ".fkb", fkb_path, 4096) ||
        !fk_path_replace_ext(path, ".sym", sym_path, 4096) ||
        !fk_path_replace_ext(path, ".dylib", dylib_path, 4096)) {
        fk_die("fk_run_src: artifact path exceeds buffer");
    }
    if (!fk_src_load_unit(path, expected_source_hash, FK_SRC_HASH_CAP, &unit_mtime)) {
        return 2;
    }
    long long fkb_mtime = fk_path_mtime_raw(fkb_path);
    long long dylib_mtime = fk_path_mtime_raw(dylib_path);
    if (dylib_mtime >= unit_mtime) {
        if (fk_run_dylib_artifact(dylib_path, arg, 0)) {
            return 0;
        }
    } else if (dylib_mtime > 0) {
        fk_diag_path("warning", dylib_path, "stale .dylib ignored");
    }
    long long recorded = fk_src_sym_recorded_errors(sym_path);
    if (fkb_mtime >= unit_mtime && recorded < 0) {
        /* an image without its error record is an incomplete cache; rebuild
         * rather than guess (older lenses, or a lens deleted out from under
         * the image) */
        fk_diag_path("warning", sym_path, "sym lens lacks a compile-error record; rebuilding");
    } else if (fkb_mtime >= unit_mtime) {
        if (fk_src_load_fkb_checked(fkb_path, path, expected_source_hash, unit_mtime)) {
            /* the compile carried errors when this image was written; the
             * cache must not launder them -- replay the tally as exit truth */
            if (recorded > 0) {
                fk_diag_path("warning", sym_path,
                        "cached image was compiled with errors; fix source and rerun to clear");
            }
            int rc = fk_run_loaded_program_image(arg);
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
    if (!import_images_loaded) {
        if (!fk_src_load_unit(path, expected_source_hash, FK_SRC_HASH_CAP, &unit_mtime)) {
            return 2;
        }
        fk_src_reset_compile_state();
    }
    fk_src_compile_current_unit(path, fkb_path, sym_path, unit_mtime, expected_source_hash);
    if (fk_path_mtime_raw(dylib_path) < unit_mtime) {
        fk_diag_path("warning", dylib_path,
                "native .dylib emission is not installed in this checkout; emitted .fkb/.sym");
    }
    fk_vs[0] = arg << 1;
    fk_vsp = 1;
    /* ── PARSE DONE, EXECUTION BEGINS ── gcc-style tally, then the two-phase gate:
     * an amputated source is a hard error -- surface the prefix's diagnostics but
     * REFUSE to run (nonzero), never silently execute the truncated program. Any
     * OTHER compile error still recovers INTO a runnable (if degraded) program and
     * runs, carrying a nonzero EXIT via fk_nerr at the final return. */
    fk_diag_flush();
    if (fk_src_truncated) {
        return 1;
    }
    if (fk_nerr == 0 && fk_conf("FK_JIT_SCAN")) {
        long long fi = 1;
        long long ok = 0;
        long long bail = 0;
        while (fi < fk_defn_next) {
            long long n = fk_jit_lower(fi);
            if (n > 0) {
                ok = ok + 1;
                if (fk_conf("FK_JIT_SCAN_V")) {
                    printf("[scan] fn%lld LOWERS (%lld bytes)\n", fi, n);
                }
            } else {
                bail = bail + 1;
                if (fk_conf("FK_JIT_SCAN_V")) {
                    printf("[scan] fn%lld BAILS\n", fi);
                }
            }
            fi = fi + 1;
        }
        printf("[scan] lowered=%lld bailed=%lld total=%lld\n", ok, bail, ok + bail);
    }
    if (fk_nerr == 0) {
        char *je = fk_conf("FK_JIT");
        long long want = (je && je[0] && je[0] != 48) ? 1 : 0;
        if (want) {
            fk_lower_tail_tramp = 1;
        }
        long long root = fk_fn[0];
        long long rt = fk_node[root][0];
        if (want && (rt == 12 || rt == 240 || rt == 241)) {
            long long callee = fk_node[root][1];
            if (callee >= 0 && callee < FK_FN_CAP) {
                long long aargs[4096];
                {
                    long long zi = 0;
                    while (zi < 4096) {
                        aargs[zi] = 0;
                        zi = zi + 1;
                    }
                }
                long long ac = 0;
                int aok = 1;
                if (rt == 12) {
                    ac = 1;
                    aargs[0] = fk_walk(fk_node[root][2], 0);
                } else if (rt == 240) {
                    ac = 2;
                    aargs[0] = fk_walk(fk_node[root][2], 0);
                    aargs[1] = fk_walk(fk_node[root][3], 0);
                } else {
                    long long cell = fk_node[root][2];
                    while (cell >= 0 && fk_node[cell][0] == 242) {
                        if (ac < 6) {
                            aargs[ac] = fk_walk(fk_node[cell][1], 0);
                        }
                        ac = ac + 1;
                        cell = fk_node[cell][2];
                    }
                    if (ac > 6) {
                        aok = 0;
                    }
                }
                long long n = aok ? fk_jit_lower(callee) : 0;
                if (n > 0) {
                    unsigned char *img = malloc(n);
                    if (img == 0) {
                        fk_pv_root(fk_fn[0], fk_walk(fk_fn[0], 0));
                        return fk_nerr > 0 ? 1 : 0;
                    }
                    long long ci = 0;
                    while (ci < n) {
                        img[ci] = fk_jb[ci];
                        ci = ci + 1;
                    }
                    fk_src_nat[callee] = img;
                    fk_src_nat_len[callee] = n;
                    fk_src_nat_frame[callee] = fk_jit_frame;
                    fk_nat_tried[callee] = 1;
                    fk_njit = fk_njit + 1;
                    if (fk_conf("FK_JIT_WITNESS")) {
                        printf(
                            "[jit] fn%lld crystallized in-process: %lld bytes, njit=%lld (native dispatch)\n",
                            callee, n, fk_njit);
                    }
                    fk_nat_exec[callee] = fk_nat_install(img, n);
                    long long rv;
                    if (fk_nat_exec[callee] != 0 && ac == fk_fnar[callee]) {
                        long long ai = 0;
                        while (ai < ac) {
                            if (ai < FK_VALUE_STACK_CAP) {
                                fk_vs[ai] = aargs[ai];
                            }
                            ai = ai + 1;
                        }
                        rv = fk_jtramp(callee, 0, ac);
                    } else if (fk_nat_exec[callee] != 0) {
                        long long fr = fk_jit_frame;
                        if (fr < ac) {
                            fr = ac;
                        }
                        long long ai = 0;
                        while (ai < fr) {
                            if (ai < FK_VALUE_STACK_CAP) {
                                fk_vs[ai] = (ai < ac) ? aargs[ai] : 0;
                            }
                            ai = ai + 1;
                        }
                        long long save_vsp = fk_vsp;
                        if (fr > fk_vsp && fr < FK_VALUE_STACK_CAP) {
                            fk_vsp = fr;
                        }
                        rv = fk_nat_exec[callee](&fk_vs[0]);
                        fk_vsp = save_vsp;
                    } else {
                        rv = fk_native_call_args(img, n, aargs);
                    }
                    fk_pv(rv);
                    return 0;
                }
            }
        }
    }
    fk_pv_root(fk_fn[0], fk_walk(fk_fn[0], 0));
    return fk_nerr > 0 ? 1 : 0;
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
            fk_die("fk_run_feval: form-eval source exceeds FK_SOURCE_TEXT_CAP");
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
    fk_ast_full = 0;
    fk_bd_top = 0;
    fk_maxslot = 0;
    fk_nerr = 0;
    fk_nwarn = 0;
    fk_src_truncated = 0;
    fk_sinit();
    fk_fntop = 0;
    fk_const_top = 0;
    fk_defn_next = 1;
    fk_root = -1;
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
    {
        char *je = fk_conf("FK_JIT");
        fk_feval_jit_on = (je && je[0] && je[0] != 48) ? 1 : 0;
        char *jh = fk_conf("FK_JIT_HOT");
        if (jh && jh[0]) {
            long long h = atoi(jh);
            if (h > 0) {
                fk_feval_hot = h;
            }
        }
        long long zi = 0;
        while (zi < FK_FN_CAP) {
            fk_fheat[zi] = 0;
            fk_nat_tried[zi] = 0;
            fk_nat_exec[zi] = 0;
            fk_src_nat[zi] = 0;
            zi = zi + 1;
        }
    }
    fk_vs[0] = 0;
    fk_vsp = 1;
    /* ── PARSE DONE, EXECUTION BEGINS ── gcc-style tally (twin of fk_run_src). */
    fk_diag_flush();
    long long rv = fk_walk(fk_fn[0], 0);
    fk_pv(rv);
    /* print the meta-eval result by value-kind (int / float / nothing) */
    return fk_nerr > 0 ? 1 : 0;
}
static int fk_run(int argc, char **argv) {
    char fk_stack_here;
    fk_stack_base = &fk_stack_here;
    if (argc < 2) {
        return 1;
    }
    if (argc >= 3 && argv[1][0] == FK_CH_DASH && argv[1][1] == FK_CH_DASH &&
        argv[1][2] == FK_CH_LOWER_F && argv[1][3] == FK_CH_LOWER_E) {
        return fk_run_feval(argv[2]);
    }
    if (argc >= 3 && argv[1][0] == FK_CH_DASH && argv[1][1] == FK_CH_DASH) {
        return fk_run_src(argv[2], argc > 3 ? atoi(argv[3]) : 0);
    }
    if (fk_path_has_suffix(argv[1], ".fk")) {
        return fk_run_src(argv[1], argc > 2 ? atoi(argv[2]) : 0);
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
        char direct_sym_path[4096];
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
        int fkb_rc = fk_run_loaded_program_image(argc > 2 ? atoi(argv[2]) : 0);
        return recorded > 0 && fkb_rc == 0 ? 1 : fkb_rc;
    }
    if (fk_path_has_suffix(argv[1], ".dylib")) {
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
        return fk_run(argc, argv);
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
        return fk_run(argc, argv);
    }
    pthread_join(th, 0);
    return fk_run_ret;
}
#endif
