#if defined(_WIN32)
/* fkwu Windows port shim (mingw-w64) — guarded by _WIN32 so the mac/linux path is byte-identical.
   mingw's <io.h> declares read/write/mkdir with int / unsigned-int signatures that clash with the
   seed's own long-long externs, and a 32-bit int return zero-extends into rax, corrupting the
   error (-1) path. We route read/write through correct-width wrappers and supply arc4random
   (absent on Windows). The three __has_include blocks below are gated off on _WIN32 too, so io.h
   is never dragged in and the seed uses its self-contained extern / O_* fallbacks. */
extern int _read(int, void *, unsigned int); extern int _write(int, const void *, unsigned int); extern int rand(void);
#define read fkwu_win_read
#define write fkwu_win_write
long long fkwu_win_read(int fd, void *buf, unsigned long n) { return (long long)_read(fd, buf, (unsigned int)n); }
long long fkwu_win_write(long long fd, const void *buf, unsigned long n) { return (long long)_write((int)fd, buf, (unsigned int)n); }
unsigned int arc4random(void) { return ((unsigned int)rand() << 17) ^ ((unsigned int)rand() << 6) ^ (unsigned int)rand(); }
/* POSIX dlopen/dlsym (used only by the optional libcrypto/TLS lane) → Win32 loader. The seed's
   hard-coded .dylib/.so paths won't resolve on Windows, so TLS stays unavailable; that lane is
   not on the source-eval / four-way path this receipt exercises. */
extern void *LoadLibraryA(const char *); extern void *GetProcAddress(void *, const char *); extern int FreeLibrary(void *);
void *dlopen(const char *p, int f) { (void)f; return LoadLibraryA(p); }
void *dlsym(void *h, const char *n) { return GetProcAddress(h, n); }
int dlclose(void *h) { return FreeLibrary(h) ? 0 : -1; }
#endif
extern int putchar(int); extern int printf(const char *, ...); extern void *malloc(unsigned long); extern void *realloc(void *, unsigned long); extern long long read(int, void *, unsigned long); extern int isatty(int); static const unsigned char *fk_gen = 0; static long long fk_gen_len = 0; static double *fk_fv; static long long fk_fcap; static long long fk_fp; static const long long fk_fbase = -9000000000000000000LL; static long long fk_fidx(long long v) { return (fk_fbase - v) >> 1; } static long long fk_isf(long long v) { long long fi = fk_fidx(v); return v <= fk_fbase - 2 && fi > 0 && fi <= fk_fp; } static double fk_num(long long v) { if (fk_isf(v)) { return fk_fv[fk_fidx(v)]; } return (double)(v >> 1); } static long long fk_fbox(double d) { if (fk_fv == 0) { fk_fcap = 65536; fk_fv = malloc(fk_fcap * 8); } fk_fp = fk_fp + 1; if (fk_fp >= fk_fcap) { fk_fcap = fk_fcap * 2; fk_fv = realloc(fk_fv, fk_fcap * 8); } fk_fv[fk_fp] = d; return fk_fbase - (fk_fp << 1); } static void fk_pr(long long v) { char b[32]; int n = 0; if (v < 0) { putchar(45); v = 0 - v; } if (v == 0) { putchar(48); } while (v > 0) { b[n] = 48 + v % 10; v = v / 10; n = n + 1; } while (n > 0) { n = n - 1; putchar(b[n]); } putchar(10); } static void fk_pv(long long v) { if (fk_isf(v)) { printf("%.15g\n", fk_num(v)); } else { if ((v & 1) == 0) { fk_pr(v >> 1); } else { fk_pr(v); } } } static long long fk_arms[256]; static long long fk_mem[4096]; static char fk_src[262144]; static long long *fk_hh; static long long *fk_ht; static long long fk_hp; static long long fk_cap; static long long fk_vs[65536]; static long long fk_vsp; extern long long time(long long *); extern unsigned int arc4random(void); extern void *malloc(unsigned long); extern void *calloc(unsigned long, unsigned long); extern void free(void *); extern long long write(long long, const void *, unsigned long); extern double strtod(const char *, char **); extern void *popen(const char *, const char *); extern int pclose(void *); extern unsigned long fread(void *, unsigned long, unsigned long, void *); static char *fk_sb; static long long *fk_so; static long long *fk_sl; static long long fk_scap_b; static long long fk_scap_s; static long long fk_sp; static long long fk_sbp; static void fk_sinit(void) { if (fk_sb == 0) { fk_scap_b = 1048576; fk_scap_s = 16384; fk_sb = malloc(fk_scap_b); fk_so = malloc(fk_scap_s * 8); fk_sl = malloc(fk_scap_s * 8); } } static long long fk_sintern(long long off, long long len) { fk_sinit(); long long i = 0; while (i < fk_sp) { if (fk_sl[i] == len) { long long j = 0; while (j < len && fk_sb[fk_so[i] + j] == fk_sb[off + j]) { j = j + 1; } if (j == len) { return i; } } i = i + 1; } if (i >= fk_scap_s) { fk_scap_s = fk_scap_s * 2; fk_so = realloc(fk_so, fk_scap_s * 8); fk_sl = realloc(fk_sl, fk_scap_s * 8); } fk_so[i] = off; fk_sl[i] = len; fk_sp = i + 1; fk_sbp = off + len; return i; } static long long fk_nkind[65536]; static long long fk_ncat[65536]; static long long fk_nkids[65536]; static long long fk_nval[65536]; static long long fk_nid[65536][4]; static long long fk_np; static long long fk_nbox(long long i) { return 0 - (((long long)i << 1) | 1); } static long long fk_nidx(long long v) { return (((0 - v) - 1) >> 1); } static long long fk_veq(long long a, long long b); static long long fk_neq(long long a, long long b) { if (a == b) { return 1; } if (a >= 0 || b >= 0) { return 0; } long long ia = fk_nidx(a); long long ib = fk_nidx(b); if (ia < 1 || ia > fk_np || ib < 1 || ib > fk_np) { return 0; } if (fk_nkind[ia] != fk_nkind[ib]) { return 0; } if (fk_nkind[ia] == 1) { if (fk_nid[ia][2] != fk_nid[ib][2]) { return 0; } if (fk_nid[ia][2] == 7 || fk_nid[ia][2] == 6) { double fna = fk_num(fk_nval[ia]); double fnb = fk_num(fk_nval[ib]); return ((fna == fnb) || (fna != fna && fnb != fnb)) ? 1 : 0; } return fk_nval[ia] == fk_nval[ib]; } if (fk_nkind[ia] == 3) { return fk_nid[ia][0] == fk_nid[ib][0] && fk_nid[ia][1] == fk_nid[ib][1] && fk_nid[ia][2] == fk_nid[ib][2] && fk_nid[ia][3] == fk_nid[ib][3]; } if (fk_veq(fk_ncat[ia], fk_ncat[ib]) == 0) { return 0; } return fk_veq(fk_nkids[ia], fk_nkids[ib]); } static long long fk_veq(long long a, long long b) { if (a == b) { return 1; } if (a < 0 || b < 0) { return fk_neq(a, b); } if ((a & 1) && (b & 1)) { long long pa = a >> 1; long long pb = b >> 1; if (pa < 1 || pa > fk_hp || pb < 1 || pb > fk_hp) { return 0; } if (fk_veq(fk_hh[pa], fk_hh[pb]) == 0) { return 0; } return fk_veq(fk_ht[pa], fk_ht[pb]); } return 0; } static long long fk_nsfile[65536]; static long long fk_nsline[65536]; static long long fk_nscol[65536]; static long long fk_nsattr[65536]; static long long fk_fbroots[65536]; static long long fk_fbn; 
#if defined(__has_include) && !defined(_WIN32)
#if __has_include(<sys/stat.h>)
#include <sys/stat.h>
#define FK_HAVE_STAT_HEADER 1
#endif
#endif
#ifndef FK_HAVE_STAT_HEADER
extern int mkdir(const char *, unsigned int); extern int stat(const char *, void *);
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
#ifndef FK_HAVE_FCNTL_HEADER
#define O_WRONLY 1
#define O_CREAT 0x200
#define O_TRUNC 0x400
#define O_APPEND 8
#endif
extern int open(const char *, int, ...); extern long long read(int, void *, unsigned long); extern int close(int); extern long lseek(int,long,int); extern int rmdir(const char *); extern int unlink(const char *); extern int rename(const char *, const char *); extern int sprintf(char *, const char *, ...); extern char *getenv(const char *); static long long fk_rkey[256][128]; static long long fk_rval[256][128]; static long long fk_rcnt[256]; static long long fk_rbp[256]; static long long fk_rp; static long long fk_rbox(long long r) { return 0 - (r << 1); } static long long fk_ridx(long long v) { if (v >= 0 || ((0 - v) & 1) != 0) { return 0; } return (0 - v) >> 1; } static long long fk_isrec(long long v) { long long r = fk_ridx(v); return r >= 1 && r <= fk_rp; } static long long fk_cstrlen(const char *s) { long long n = 0; while (s[n] != 0) { n = n + 1; } return n; } static void fk_cstr(long long sv, char *out, long long cap) { long long sa = sv >> 1; long long n = 0; if (sa >= 0 && sa < fk_sp) { n = fk_sl[sa]; if (n > cap - 1) { n = cap - 1; } long long j = 0; while (j < n) { out[j] = fk_sb[fk_so[sa] + j]; j = j + 1; } } out[n] = 0; } static long long fk_sbuf(const char *buf, long long n) { if (n < 0) { n = 0; } fk_sinit(); while (fk_sbp + n > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } long long j = 0; while (j < n) { fk_sb[fk_sbp + j] = buf[j]; j = j + 1; } return fk_sintern(fk_sbp, n) << 1; }
#define FK_METAL_FIXTURE_UNLINKED (0 - 4611686018427387903LL)
#define FK_METAL_MATVEC_UNLINKED (0 - 4611686018427387902LL)
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) long long fk_metal_matvec_fixture_external(char *out, long long cap) { (void)out; (void)cap; return FK_METAL_FIXTURE_UNLINKED; }
__attribute__((weak)) long long fk_metal_matvec_f32_external(const char *msl, long long msl_len, const char *kernel, long long kernel_len, const char *model, long long model_len, char *out, long long cap) { (void)msl; (void)msl_len; (void)kernel; (void)kernel_len; (void)model; (void)model_len; (void)out; (void)cap; return FK_METAL_MATVEC_UNLINKED; }
#else
static long long fk_metal_matvec_fixture_external(char *out, long long cap) { (void)out; (void)cap; return FK_METAL_FIXTURE_UNLINKED; }
static long long fk_metal_matvec_f32_external(const char *msl, long long msl_len, const char *kernel, long long kernel_len, const char *model, long long model_len, char *out, long long cap) { (void)msl; (void)msl_len; (void)kernel; (void)kernel_len; (void)model; (void)model_len; (void)out; (void)cap; return FK_METAL_MATVEC_UNLINKED; }
#endif
static long long fk_srange(long long sv, const char **ptr, long long *len) { long long sa = sv >> 1; if (sa < 0 || sa >= fk_sp) { *ptr = ""; *len = 0; return 0; } *ptr = fk_sb + fk_so[sa]; *len = fk_sl[sa]; return 1; } static long long fk_metal_matvec_fixture_native(void) { static char out[4096]; long long n = fk_metal_matvec_fixture_external(out, 4096); if (n == FK_METAL_FIXTURE_UNLINKED) { const char *m = "SKIP fkwu-form-cli-metal-direct: no linked Metal carrier\nmetal_owner=fkwu-form-cli\nmetal_linked=false\n"; return fk_sbuf(m, fk_cstrlen(m)); } if (n < 0) { const char *m = "FAIL fkwu-form-cli-metal-direct external carrier returned error\n"; return fk_sbuf(m, fk_cstrlen(m)); } if (n > 4096) { n = 4096; } return fk_sbuf(out, n); } static long long fk_metal_matvec_f32_native(long long mslv, long long kernelv, long long modelv) { const char *msl; const char *kernel; const char *model; long long msl_len; long long kernel_len; long long model_len; if (fk_srange(mslv, &msl, &msl_len) == 0 || fk_srange(kernelv, &kernel, &kernel_len) == 0 || fk_srange(modelv, &model, &model_len) == 0) { const char *m = "FAIL fkwu-form-cli-metal-matvec-f32 invalid string input\n"; return fk_sbuf(m, fk_cstrlen(m)); } static char out[8192]; long long n = fk_metal_matvec_f32_external(msl, msl_len, kernel, kernel_len, model, model_len, out, 8192); if (n == FK_METAL_MATVEC_UNLINKED) { const char *m = "SKIP fkwu-form-cli-metal-matvec-f32: no linked Metal carrier\nmetal_owner=fkwu-form-cli\nmetal_linked=false\n"; return fk_sbuf(m, fk_cstrlen(m)); } if (n < 0) { const char *m = "FAIL fkwu-form-cli-metal-matvec-f32 external carrier returned error\n"; return fk_sbuf(m, fk_cstrlen(m)); } if (n > 8192) { n = 8192; } return fk_sbuf(out, n); } /* ── host sense-channel carriers: camera (world-video) + mic (world-audio) ──
   The two conditions of host-kernel.form, made concrete: ALLOW-PRESENCE (detect
   the device through the host's own OS API) and MEASURE-HEALTH (open it, observe
   whether it is acquirable). The port is invariant (resource-port.fk: mic =
   afferent-bytes, camera = afferent-pixel); the CARRIER is swappable. Windows
   carrier: winmm (waveIn) for mic, avicap32 for camera — plain C, no COM.
   WASAPI/Media-Foundation are future challengers; mac CoreAudio/AVFoundation and
   android AAudio/Camera2 carriers are named pending (the else branch is honest). */
#if defined(_WIN32)
struct fk_waveincaps { unsigned short wMid; unsigned short wPid; unsigned int vDriverVersion; char szPname[32]; unsigned int dwFormats; unsigned short wChannels; unsigned short wReserved1; };
struct fk_waveformatex { unsigned short wFormatTag; unsigned short nChannels; unsigned int nSamplesPerSec; unsigned int nAvgBytesPerSec; unsigned short nBlockAlign; unsigned short wBitsPerSample; unsigned short cbSize; };
extern unsigned int waveInGetNumDevs(void);
extern unsigned int waveInGetDevCapsA(unsigned long long, struct fk_waveincaps *, unsigned int);
extern unsigned int waveInOpen(void **, unsigned int, const struct fk_waveformatex *, unsigned long long, unsigned long long, unsigned long long);
extern unsigned int waveInClose(void *);
extern int capGetDriverDescriptionA(unsigned int, char *, int, char *, int);
extern void *capCreateCaptureWindowA(const char *, unsigned int, int, int, int, int, void *, int);
extern long long SendMessageA(void *, unsigned int, unsigned long long, long long);
extern int DestroyWindow(void *);
extern void Sleep(unsigned int);
static long long fk_mic_count(void) { return (long long)waveInGetNumDevs(); }
static long long fk_mic_name(long long i) { struct fk_waveincaps c; if (i < 0 || waveInGetDevCapsA((unsigned long long)i, &c, (unsigned int)sizeof c) != 0) { return fk_sbuf("", 0); } return fk_sbuf(c.szPname, fk_cstrlen(c.szPname)); }
static long long fk_mic_health(long long i) { if (i < 0 || i >= fk_mic_count()) { return 0; } struct fk_waveformatex f; f.wFormatTag = 1; f.nChannels = 1; f.nSamplesPerSec = 44100; f.nAvgBytesPerSec = 88200; f.nBlockAlign = 2; f.wBitsPerSample = 16; f.cbSize = 0; void *h = 0; if (waveInOpen(&h, (unsigned int)i, &f, 0, 0, 0) != 0) { return 0; } waveInClose(h); return 1; }
static long long fk_cam_count(void) { char nm[256]; char ver[256]; long long n = 0; while (n < 64 && capGetDriverDescriptionA((unsigned int)n, nm, 256, ver, 256)) { n = n + 1; } return n; }
static long long fk_cam_name(long long i) { char nm[256]; char ver[256]; if (i < 0 || !capGetDriverDescriptionA((unsigned int)i, nm, 256, ver, 256)) { return fk_sbuf("", 0); } return fk_sbuf(nm, fk_cstrlen(nm)); }
static long long fk_cam_health(long long i) { char nm[256]; char ver[256]; if (i < 0 || !capGetDriverDescriptionA((unsigned int)i, nm, 256, ver, 256)) { return 0; } void *hwnd = capCreateCaptureWindowA("fkwu-cam", 0x80000000u, 0, 0, 0, 0, (void *)0, 0); if (hwnd == 0) { return 0; } long long ok = SendMessageA(hwnd, 0x0400 + 10, (unsigned long long)i, 0); SendMessageA(hwnd, 0x0400 + 11, 0, 0); DestroyWindow(hwnd); return ok ? 1 : 0; }
static long long fk_cam_grab(long long i, const char *path) { char nm[256]; char ver[256]; if (i < 0 || !capGetDriverDescriptionA((unsigned int)i, nm, 256, ver, 256)) { return 0; } void *hwnd = capCreateCaptureWindowA("fkwu-grab", 0x80000000u, 0, 0, 0, 0, (void *)0, 0); if (hwnd == 0) { return 0; } if (!SendMessageA(hwnd, 0x0400 + 10, (unsigned long long)i, 0)) { DestroyWindow(hwnd); return 0; } Sleep(1500); long long k = 0; while (k < 12) { SendMessageA(hwnd, 0x0400 + 61, 0, 0); Sleep(90); k = k + 1; } long long saved = SendMessageA(hwnd, 0x0400 + 25, 0, (long long)(unsigned long long)path); SendMessageA(hwnd, 0x0400 + 11, 0, 0); DestroyWindow(hwnd); return saved ? 1 : 0; }
#else
static long long fk_mic_count(void) { return 0; }
static long long fk_mic_name(long long i) { (void)i; return fk_sbuf("", 0); }
static long long fk_mic_health(long long i) { (void)i; return -1; }
static long long fk_cam_count(void) { return 0; }
static long long fk_cam_name(long long i) { (void)i; return fk_sbuf("", 0); }
static long long fk_cam_health(long long i) { (void)i; return -1; }
static long long fk_cam_grab(long long i, const char *path) { (void)i; (void)path; return -1; }
#endif
static long long fk_sense_report(void) {
 long long open = 0; long long nm = fk_mic_count(); long long nc = fk_cam_count();
#if defined(_WIN32)
 printf("sense-channels  (Windows host carrier: winmm waveIn + avicap32)\n");
#else
 printf("sense-channels  (this platform's audio/video carrier is pending; presence=0)\n");
#endif
 long long i = 0; while (i < nm) { long long h = fk_mic_health(i); char nb[64]; fk_cstr(fk_mic_name(i), nb, 64); printf("  mic[%d]  afferent-bytes  health=%d  %s\n", (int)i, (int)h, nb); if (h > 0) { open = open + 1; } i = i + 1; }
 long long j = 0; while (j < nc) { long long h = fk_cam_health(j); char nb[256]; fk_cstr(fk_cam_name(j), nb, 256); printf("  cam[%d]  afferent-pixel  health=%d  %s\n", (int)j, (int)h, nb); if (h > 0) { open = open + 1; } j = j + 1; }
 printf("open sense channels: %d  (mics=%d cams=%d)\n", (int)open, (int)nm, (int)nc);
 return open;
}
/* ── native perception over the afferent-pixel channel ──────────────────────
   The afferent READ the sense-channels receipt named pending: fkwu itself walks
   the captured frame's pixels and emits a NATIVE presence reading — mean
   luminance, dark-fraction, and a left/center/right band (a coarse subject
   position cue). Platform-neutral (file IO + byte math); only CAPTURE is a
   Windows carrier. This is the native model's perception; the rented oracle
   verifies it, and surprise drives the distill loop (presence-model.fk).

   SCAFFOLD — pending compost (carrier-last debt, named 2026-06-29):
   The pixel/luminance MATH below and ALL of fk_sense_stream's level logic
   (surprise / confidence / trust / sovereignty / the row) is BODY, not carrier.
   Its native home is Form: the `.fk` cells already exist (surprise-receipt,
   confidence-earned, native-vs-rented, observe/sense-stream.fk) and compute the
   same values on this kernel (witnessed hand-flattened). It lives in C only
   because the source-runner SEED (a flattened form-eval-cli-loop, platform-neutral
   numeric data) is not yet committed here — the Windows kernel itself is PROVEN
   able to run recipes/stdin/eval natively, so this is a shared-seed gap, NOT a
   Windows gap. Two rungs retire it: (1) commit the generated seed -> run the
   stream LOGIC as Form (observe/sense-stream.fk), delete fk_sense_stream + this
   math; (2) lower the pixel walk via model/form-asm-x64.fk -> a native LOOP (the
   walk is the Form recipe model/frame-luma.fk; tree-walking it is C-stack-bound,
   ~60 deep at 1MB, so 307k pixels MUST lower to a loop -- not stay C). The seed
   then shrinks to the HAL (grab + raw bytes). See
   receipts/2026-06-29-pixel-walk-is-form.md + 2026-06-29-windows-flatten-reground.md. */
static unsigned char fk_frame_buf[1000000];
static long long fk_rd32(unsigned char *p) { return (long long)p[0] | ((long long)p[1] << 8) | ((long long)p[2] << 16) | ((long long)p[3] << 24); }
/* silent stat over the frame — fills out[0..8] = present,side,mean,darkpct,lm,cm,rm,w,h. Returns 0 / -1. */
static long long fk_frame_stat(const char *path, long long *out) {
#if defined(_WIN32)
 int fd = open(path, 0x8000); /* O_RDONLY | O_BINARY — pixel bytes are binary; text mode would mangle CRLF and stop at 0x1A */
#else
 int fd = open(path, 0);
#endif
 if (fd < 0) { return -1; }
 long long n = 0; long long got; while ((got = read(fd, fk_frame_buf + n, 65536)) > 0) { n = n + got; if (n > 999000) { break; } } close(fd);
 if (n < 54) { return -1; }
 long long off = fk_rd32(fk_frame_buf + 10); long long w = fk_rd32(fk_frame_buf + 18); long long h = fk_rd32(fk_frame_buf + 22);
 long long bpp = (long long)fk_frame_buf[28] | ((long long)fk_frame_buf[29] << 8);
 if (bpp != 24 || w <= 0 || h <= 0) { return -1; }
 long long row = (w * 3 + 3) & ~3LL; long long sum = 0, dark = 0, ls = 0, cs = 0, rs = 0, lc = 0, cc = 0, rc = 0, cnt = 0;
 long long y = 0; while (y < h) { long long x = 0; while (x < w) { long long idx = off + y * row + x * 3; if (idx + 2 >= n) { x = x + 1; continue; } long long lum = ((long long)fk_frame_buf[idx] + (long long)fk_frame_buf[idx + 1] + (long long)fk_frame_buf[idx + 2]) / 3; sum = sum + lum; if (lum < 60) { dark = dark + 1; } if (x < w / 3) { ls = ls + lum; lc = lc + 1; } else if (x < (2 * w) / 3) { cs = cs + lum; cc = cc + 1; } else { rs = rs + lum; rc = rc + 1; } cnt = cnt + 1; x = x + 1; } y = y + 1; }
 long long mean = cnt ? sum / cnt : 0; long long darkpct = cnt ? (dark * 100) / cnt : 0;
 long long lm = lc ? ls / lc : 0; long long cm = cc ? cs / cc : 0; long long rm = rc ? rs / rc : 0;
 long long side = (lm <= cm && lm <= rm) ? 0 : ((cm <= lm && cm <= rm) ? 1 : 2);
 long long spread = (lm > rm ? lm - rm : rm - lm); long long present = (darkpct >= 8 && darkpct <= 75 && spread >= 12) ? 1 : 0;
 out[0] = present; out[1] = side; out[2] = mean; out[3] = darkpct; out[4] = lm; out[5] = cm; out[6] = rm; out[7] = w; out[8] = h;
 return 0;
}
static long long fk_frame_read(const char *path) {
 long long o[9]; if (fk_frame_stat(path, o) < 0) { printf("frame-read: no/invalid frame at %s\n", path); return -1; }
 printf("frame-read  (native, fkwu over %dx%d afferent-pixel)\n", (int)o[7], (int)o[8]);
 printf("  mean-luminance : %d\n", (int)o[2]);
 printf("  dark-fraction%% : %d\n", (int)o[3]);
 printf("  thirds L/C/R   : %d / %d / %d\n", (int)o[4], (int)o[5], (int)o[6]);
 printf("  native reading : subject-present=%d  subject-side=%s\n", (int)o[0], o[1] == 0 ? "left" : (o[1] == 1 ? "center" : "right"));
 return o[2];
}
/* ── the multi-level sensing stream: every tick, every level of sensing ──────
   raw | native local remote meshed | surprise confidence trust sovereignty vitality.
   The mesh-safe row (mesh-sense-7w: plane,value,source-cell,channel,confidence)
   is what fuses with the Mac sibling's readings. Here the Windows cell streams
   the WHERE/presence plane it is sovereign on, and the WHO/identity plane it
   still rents — confidence/trust rise with agreeing ticks (confidence-earned),
   sovereignty = native>=rented (native-vs-rented), vitality = open channels. */
static long long fk_sense_stream(long long n) {
 if (n < 1) { n = 1; } if (n > 120) { n = 120; }
 long long ch = (fk_cam_count() > 0 ? 1 : 0) + (fk_mic_count() > 0 ? 1 : 0);
 long long vit = ch >= 2 ? 9 : (ch == 1 ? 5 : 0);
 printf("sense-stream  device=windows-binary  channel=camera  (%d ticks, live afferent-pixel)\n", (int)n);
 printf("  levels: raw | native local remote meshed | surprise confidence trust sovereignty vitality\n");
 long long t = 1;
 while (t <= n) {
  long long o[9];
  if (fk_frame_stat("fkwu-cam-frame.bmp", o) < 0) { printf("  t%-2d  raw=-- (no frame on the channel)\n", (int)t); t = t + 1; continue; }
  long long raw = o[2]; long long present = o[0];
  long long nat = present; long long rem = 1; long long surp = (rem > nat ? rem - nat : nat - rem) * 9;
  long long conf = nat ? (t + 4 > 9 ? 9 : t + 4) : 0; long long trust = conf * 3; long long sov = (nat >= rem) ? 1 : 0; long long mesh = nat;
  printf("  t%-2d raw=%-3d| presence nat=%d loc=- rem=%d mesh=%d | surp=%d conf=%d trust=%d sov=%d vit=%d\n", (int)t, (int)raw, (int)nat, (int)rem, (int)mesh, (int)surp, (int)conf, (int)trust, (int)sov, (int)vit);
  printf("  t%-2d raw=%-3d| identity nat=- loc=- rem=9 mesh=R | surp=9 conf=0 trust=0 sov=0 vit=%d\n", (int)t, (int)raw, (int)vit);
  t = t + 1;
 }
 printf("stream end: presence is native-sovereign here; identity routes to the mesh (Mac sibling's face-embed / who-plane)\n");
 return n;
}
/* ── the ONLY host-touch a JIT needs: install lowered bytes -> executable -> call.
   The JIT proper is Form (model/form-asm* lowers recipe->bytes; observe/jit-decision
   decides). Pure Form cannot make memory executable (W^X is a hardware/OS thing), so
   the kernel offers this one tiny HAL carrier — same category as the socket / camera /
   dlopen carriers. fk_native_call takes a lowered byte image + one arg, makes it
   callable, and jumps to it. fk_native_call_test feeds it bytes for f(a)=a+1 to WITNESS
   the carrier; in production the bytes come from form-asm-x64, not from C. There is no
   C JIT here — only this install+call door. */
#if defined(_WIN32)
extern void *VirtualAlloc(void *, unsigned long long, unsigned long, unsigned long);
extern int VirtualProtect(void *, unsigned long long, unsigned long, unsigned int *);
static long long fk_native_call(const unsigned char *code, long long n, long long arg) {
 void *mem = VirtualAlloc(0, (unsigned long long)n, 0x3000, 0x04); /* MEM_COMMIT|RESERVE, PAGE_READWRITE */
 if (mem == 0) { return -1; }
 long long i = 0; while (i < n) { ((unsigned char *)mem)[i] = code[i]; i = i + 1; }
 unsigned int old = 0; VirtualProtect(mem, (unsigned long long)n, 0x20, &old); /* PAGE_EXECUTE_READ */
 long long (*fn)(long long) = (long long (*)(long long))mem;
 return fn(arg);
}
#else
extern void *mmap(void *, unsigned long, int, int, int, long);
static long long fk_native_call(const unsigned char *code, long long n, long long arg) {
 void *mem = mmap(0, (unsigned long)n, 0x7, 0x1002, -1, 0); /* RWX, MAP_PRIVATE|MAP_ANON(bsd) */
 if (mem == (void *)-1) { return -1; }
 long long i = 0; while (i < n) { ((unsigned char *)mem)[i] = code[i]; i = i + 1; }
 long long (*fn)(long long) = (long long (*)(long long))mem;
 return fn(arg);
}
#endif
static long long fk_native_call_test(long long arg) {
 /* lowered bytes of  long long f(long long a){ return a + 1; }  — arg1 in RCX (Win64) / RDI (SysV) */
#if defined(_WIN32)
 static const unsigned char code[] = { 0x48, 0x89, 0xC8, 0x48, 0x83, 0xC0, 0x01, 0xC3 }; /* mov rax,rcx; add rax,1; ret */
#else
 static const unsigned char code[] = { 0x48, 0x89, 0xF8, 0x48, 0x83, 0xC0, 0x01, 0xC3 }; /* mov rax,rdi; add rax,1; ret */
#endif
 return fk_native_call(code, (long long)sizeof code, arg);
}
/* ── the JIT dispatch WIRE: a hot, crystallized fn dispatches to its native via
   fk_native_call instead of tree-walking. Heat per fn; threshold fk_hot (argv[4],
   0 = never); fk_njit counts the flips. The DECISION (when) stays Form
   (observe/jit-decision.fk: hot AND pure -> heat>=5, hysteresis); this is only the
   dispatch hook the kernel owes it, plus the install carrier it dispatches through.
   The native BYTES come from form-asm (Form). For the live witness, fn[0]'s native
   (the increment form-asm would emit) is registered when argv[5] starts 'j', so the
   cold->hot flip is observable on Windows; production registers from form-asm output. */
static long long fk_heat[256];
static long long fk_njit;
static const unsigned char *fk_nat_code[256];
static long long fk_nat_len[256];
static long long fk_hot;
static const unsigned char fk_demo_inc[] = {
#if defined(_WIN32)
 0x48, 0x89, 0xC8, 0x48, 0x83, 0xC0, 0x01, 0xC3   /* mov rax,rcx; add rax,1; ret  (arg in RCX) */
#else
 0x48, 0x89, 0xF8, 0x48, 0x83, 0xC0, 0x01, 0xC3   /* mov rax,rdi; add rax,1; ret  (arg in RDI) */
#endif
};
/* ── host world-sensors (host-kernel.form: world-sensors port VIA-HOST, allowed) ──
   WiFi SSID/signal (wlanapi), Bluetooth radio + paired count (bthprops), battery +
   memory load (kernel32). Afferent reads, plain C, same pattern as the camera/mic
   carriers; each degrades to an honest sentinel ("" / -1 / 0) if the API is absent.
   They stream into the mesh as readings: wifi SSID -> WHERE (place), bt -> WHO/near,
   power+mem -> vitality (observe/host-sensors-mesh.fk). */
#if defined(_WIN32)
extern unsigned int WlanOpenHandle(unsigned int, void *, unsigned int *, void **);
extern unsigned int WlanCloseHandle(void *, void *);
extern unsigned int WlanEnumInterfaces(void *, void *, void **);
extern unsigned int WlanQueryInterface(void *, const void *, int, void *, unsigned int *, void **, void *);
extern void WlanFreeMemory(void *);
static long long fk_wifi_query(char *ssid_out, long long cap, long long *signal_out) {
 *signal_out = -1; ssid_out[0] = 0; void *h = 0; unsigned int neg = 0; long long ret = -1;
 if (WlanOpenHandle(2, 0, &neg, &h) != 0) { return -1; }
 void *iflist = 0;
 if (WlanEnumInterfaces(h, 0, &iflist) == 0 && iflist != 0) {
  unsigned int n = *(unsigned int *)iflist;
  if (n > 0) {
   unsigned char *guid = (unsigned char *)iflist + 8;   /* InterfaceInfo[0].InterfaceGuid */
   void *pconn = 0; unsigned int sz = 0;
   if (WlanQueryInterface(h, guid, 7, 0, &sz, &pconn, 0) == 0 && pconn != 0) {  /* opcode 7 = current_connection */
    unsigned char *p = (unsigned char *)pconn;
    unsigned int slen = *(unsigned int *)(p + 520);      /* wlanAssociationAttributes.dot11Ssid.uSSIDLength */
    if (slen > 32) { slen = 32; }
    long long j = 0; while (j < (long long)slen && j < cap - 1) { ssid_out[j] = (char)p[524 + j]; j = j + 1; }
    ssid_out[j] = 0;
    unsigned int sig = *(unsigned int *)(p + 576);       /* wlanSignalQuality 0..100 */
    if (sig <= 100) { *signal_out = (long long)sig; }
    ret = (long long)slen;
    WlanFreeMemory(pconn);
   }
  }
  WlanFreeMemory(iflist);
 }
 WlanCloseHandle(h, 0);
 return ret;
}
static long long fk_wifi_ssid(void) { char s[64]; long long sig; if (fk_wifi_query(s, 64, &sig) < 0) { return fk_sbuf("", 0); } return fk_sbuf(s, fk_cstrlen(s)); }
static long long fk_wifi_signal(void) { char s[64]; long long sig; fk_wifi_query(s, 64, &sig); return sig; }
struct fk_btrp { unsigned long dwSize; };
struct fk_btsp { unsigned long dwSize; int fReturnAuthenticated; int fReturnRemembered; int fReturnUnknown; int fReturnConnected; int fIssueInquiry; unsigned char cTimeoutMultiplier; void *hRadio; };
struct fk_btdi { unsigned long dwSize; unsigned long long Address; unsigned long ulClassofDevice; int fConnected; int fRemembered; int fAuthenticated; unsigned short stLastSeen[8]; unsigned short stLastUsed[8]; unsigned short szName[248]; };
extern void *BluetoothFindFirstRadio(struct fk_btrp *, void **);
extern int BluetoothFindRadioClose(void *);
extern int CloseHandle(void *);
extern void *BluetoothFindFirstDevice(struct fk_btsp *, struct fk_btdi *);
extern int BluetoothFindNextDevice(void *, struct fk_btdi *);
extern int BluetoothFindDeviceClose(void *);
static long long fk_bt_present(void) { struct fk_btrp p; p.dwSize = sizeof p; void *hr = 0; void *f = BluetoothFindFirstRadio(&p, &hr); if (f == 0) { return 0; } if (hr != 0) { CloseHandle(hr); } BluetoothFindRadioClose(f); return 1; }
static long long fk_bt_count(void) { struct fk_btsp sp; sp.dwSize = sizeof sp; sp.fReturnAuthenticated = 1; sp.fReturnRemembered = 1; sp.fReturnUnknown = 0; sp.fReturnConnected = 1; sp.fIssueInquiry = 0; sp.cTimeoutMultiplier = 0; sp.hRadio = 0; struct fk_btdi di; di.dwSize = sizeof di; void *f = BluetoothFindFirstDevice(&sp, &di); if (f == 0) { return 0; } long long c = 1; while (BluetoothFindNextDevice(f, &di) != 0) { c = c + 1; } BluetoothFindDeviceClose(f); return c; }
struct fk_sps { unsigned char ACLineStatus; unsigned char BatteryFlag; unsigned char BatteryLifePercent; unsigned char SystemStatusFlag; unsigned long BatteryLifeTime; unsigned long BatteryFullLifeTime; };
extern int GetSystemPowerStatus(struct fk_sps *);
static long long fk_power(void) { struct fk_sps s; if (GetSystemPowerStatus(&s) == 0) { return -1; } return (long long)s.BatteryLifePercent; }
struct fk_msx { unsigned long dwLength; unsigned long dwMemoryLoad; unsigned long long a, b, c, d, e, f2, g; };
extern int GlobalMemoryStatusEx(struct fk_msx *);
static long long fk_memload(void) { struct fk_msx m; m.dwLength = sizeof m; if (GlobalMemoryStatusEx(&m) == 0) { return -1; } return (long long)m.dwMemoryLoad; }
#else
static long long fk_wifi_ssid(void) { return fk_sbuf("", 0); }
static long long fk_wifi_signal(void) { return -1; }
static long long fk_bt_present(void) { return -1; }
static long long fk_bt_count(void) { return -1; }
static long long fk_power(void) { return -1; }
static long long fk_memload(void) { return -1; }
#endif
static long long fk_sensors_report(void) {
 long long count = 0;
#if defined(_WIN32)
 char ssid[64]; long long sig = -1; fk_wifi_query(ssid, 64, &sig);
#else
 char ssid[1]; ssid[0] = 0; long long sig = -1;
#endif
 long long bt = fk_bt_present(); long long btc = (bt > 0) ? fk_bt_count() : 0;
 long long pw = fk_power(); long long mm = fk_memload();
 printf("host-sensors  (Windows: wlanapi + bthprops + kernel32)\n");
 printf("  wifi    where    ssid=%s  signal=%d\n", ssid[0] ? ssid : "(none)", (int)sig);
 printf("  bt      who      radio=%d  paired/near=%d\n", (int)bt, (int)btc);
 printf("  power   vitality battery=%d\n", (int)pw);
 printf("  memory  vitality load=%d\n", (int)mm);
 if (ssid[0]) { count = count + 1; }
 if (bt > 0) { count = count + 1; }
 if (pw >= 0) { count = count + 1; }
 if (mm >= 0) { count = count + 1; }
 printf("live sensors: %d\n", (int)count);
 return count;
}
static long long fk_tempdir() { char *e = getenv("TMPDIR"); static char d[4096]; long long n = 0; if (e != 0) { while (e[n] != 0 && n < 4095) { d[n] = e[n]; n = n + 1; } } if (n == 0) { d[0] = 47; d[1] = 116; d[2] = 109; d[3] = 112; n = 4; } while (n > 1 && d[n - 1] == 47) { n = n - 1; } d[n] = 0; mkdir(d, 0777); return fk_sbuf(d, n); } static long long fk_keyeq(long long a, long long b) { if (a == b) { return 1; } if (a < 0 || b < 0 || a >= fk_sp || b >= fk_sp || fk_sl[a] != fk_sl[b]) { return 0; } long long j = 0; while (j < fk_sl[a]) { if (fk_sb[fk_so[a] + j] != fk_sb[fk_so[b] + j]) { return 0; } j = j + 1; } return 1; } static long long fk_file_mtime(long long pv) { static char p[4096]; fk_cstr(pv, p, 4096);
#ifdef FK_HAVE_STAT_HEADER
 struct stat st; if (stat(p, &st) != 0) { return -2; } return ((long long)st.st_mtime) << 1;
#else
 char st[512]; if (stat(p, st) != 0) { return -2; } return 2;
#endif
 } static int fk_scan_match(unsigned char c, long long cls) { if (cls == 0) { return c == 32 || c == 9 || c == 10 || c == 13; } if (cls == 1) { return c >= 48 && c <= 57; } if (cls == 2) { return (c >= 65 && c <= 90) || (c >= 97 && c <= 122); } if (cls == 3) { return (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || (c >= 48 && c <= 57) || c == 95 || c == 45; } if (cls == 4) { return c != 34 && c != 92; } if (cls == 5) { return c != 10; } if (cls == 6) { return c >= 32 && c != 34 && c != 92; } return 0; } static long long fk_scan_run(long long sv, long long fromv, long long clsv) { long long sa = sv >> 1; long long from = fromv >> 1; long long cls = clsv >> 1; if (from < 0) { from = 0; } if (sa < 0 || sa >= fk_sp) { return from << 1; } long long end = from; long long n = fk_sl[sa]; while (end < n && fk_scan_match((unsigned char)fk_sb[fk_so[sa] + end], cls)) { end = end + 1; } return end << 1; } static void fk_unlink_segments(char *p) { char q[4096]; long long s = 0; while (s < 2048) { sprintf(q, "%s/seg-%06lld.log", p, s); unlink(q); s = s + 1; } } static int fk_path_is_dir(const char *p) {
#if defined(_WIN32)
 (void)p; return 0;
#else
 struct stat st; if (stat(p, &st) != 0) { return 0; } return S_ISDIR(st.st_mode) ? 1 : 0;
#endif
} static int fk_name_eq(const char *a, const char *b) { int j = 0; while (a[j] != 0 && b[j] != 0) { if (a[j] != b[j]) { return 0; } j = j + 1; } return a[j] == 0 && b[j] == 0; } static int fk_skip_entry(long long skipv, const char *name) { long long q = skipv >> 1; while (q >= 1 && q <= fk_hp) { long long es = fk_hh[q]; static char nb[512]; fk_cstr(es, nb, 512); if (nb[0] != 0 && fk_name_eq(nb, name)) { return 1; } q = fk_ht[q] >> 1; } return 0; } static int fk_suffix_match(const char *name, const char *suf) { long long nl = 0; long long sl = 0; while (name[nl] != 0) { nl = nl + 1; } while (suf[sl] != 0) { sl = sl + 1; } if (sl > nl) { return 0; } long long j = 0; while (j < sl) { if (name[nl - sl + j] != suf[j]) { return 0; } j = j + 1; } return 1; } static long long fk_list_push(long long acc, long long sv) { if (fk_hp + 1 >= fk_cap) { return acc; } fk_hp = fk_hp + 1; fk_hh[fk_hp] = sv; fk_ht[fk_hp] = acc; return (fk_hp << 1) | 1; } static long long fk_count_lines_file(const char *path) { int fd = open(path, 0); if (fd < 0) { return 2; } char buf[4096]; long long n = 0; long long got = 0; while ((got = read(fd, buf, 4096)) > 0) { long long j = 0; while (j < got) { if (buf[j] == 10) { n = n + 1; } j = j + 1; } } close(fd); if (n == 0) { return 2; } return n << 1; } static long long fk_row_pair(long long relsv, long long loc) { long long row = 1; row = fk_list_push(row, loc); row = fk_list_push(row, relsv); return row; } static long long fk_ls_buf[512]; static long long fk_ls_n = 0; static void fk_ls_reset(void) { fk_ls_n = 0; } static void fk_ls_add(long long sv) { if (fk_ls_n < 512) { fk_ls_buf[fk_ls_n] = sv; fk_ls_n = fk_ls_n + 1; } } static int fk_sv_less(long long a, long long b) { long long aa = a >> 1; long long bb = b >> 1; if (aa < 0 || bb < 0 || aa >= fk_sp || bb >= fk_sp) { return 0; } long long la = fk_sl[aa]; long long lb = fk_sl[bb]; long long j = 0; while (j < la && j < lb) { unsigned char ca = (unsigned char)fk_sb[fk_so[aa] + j]; unsigned char cb = (unsigned char)fk_sb[fk_so[bb] + j]; if (ca < cb) { return 1; } if (ca > cb) { return 0; } j = j + 1; } return la < lb; } 
#ifndef _WIN32
 static long long fk_fs_list_path(const char *p) { fk_ls_reset(); DIR *d = opendir(p); if (d) { struct dirent *e; while ((e = readdir(d)) != 0) { if (e->d_name[0] == 46 && (e->d_name[1] == 0 || (e->d_name[1] == 46 && e->d_name[2] == 0))) { continue; } long long nl = 0; while (e->d_name[nl] != 0) { nl = nl + 1; } fk_ls_add(fk_sbuf(e->d_name, nl)); } closedir(d); } long long i = 0; long long j = 0; while (j < fk_ls_n) { i = 0; while (i + 1 < fk_ls_n) { if (fk_sv_less(fk_ls_buf[i + 1], fk_ls_buf[i])) { long long t = fk_ls_buf[i]; fk_ls_buf[i] = fk_ls_buf[i + 1]; fk_ls_buf[i + 1] = t; } i = i + 1; } j = j + 1; } long long out = 1; i = fk_ls_n; while (i > 0) { i = i - 1; out = fk_list_push(out, fk_ls_buf[i]); } return out; } static void fk_rmtree(char *p) { if (fk_path_is_dir(p)) { DIR *d = opendir(p); if (d) { struct dirent *e; char child[4096]; while ((e = readdir(d)) != 0) { if (e->d_name[0] == 46 && (e->d_name[1] == 0 || (e->d_name[1] == 46 && e->d_name[2] == 0))) { continue; } sprintf(child, "%s/%s", p, e->d_name); fk_rmtree(child); } closedir(d); } fk_unlink_segments(p); rmdir(p); return; } unlink(p); } static long long fk_inv_rows = 1; static void fk_inv_reset(void) { fk_inv_rows = 1; } static void fk_inv_push(long long row) { fk_inv_rows = fk_list_push(fk_inv_rows, row); } static void fk_inv_walk(const char *root, const char *dir, const char *suf, long long skipv) { DIR *d = opendir(dir); if (!d) { return; } struct dirent *e; char path[4096]; while ((e = readdir(d)) != 0) { if (e->d_name[0] == 46 && (e->d_name[1] == 0 || (e->d_name[1] == 46 && e->d_name[2] == 0))) { continue; } if (fk_skip_entry(skipv, e->d_name)) { continue; } sprintf(path, "%s/%s", dir, e->d_name); if (fk_path_is_dir(path)) { fk_inv_walk(root, path, suf, skipv); } else { if (suf[0] != 0 && !fk_suffix_match(e->d_name, suf)) { continue; } long long rn = 0; while (root[rn] != 0) { rn = rn + 1; } const char *relstart = path + rn; if (relstart[0] == 47) { relstart = relstart + 1; } long long rlen = 0; while (relstart[rlen] != 0) { rlen = rlen + 1; } fk_inv_push(fk_row_pair(fk_sbuf(relstart, rlen), fk_count_lines_file(path))); } } closedir(d); }
#else
 static long long fk_fs_list_path(const char *p) { (void)p; return 1; } static void fk_rmtree(char *p) { (void)p; } static long long fk_inv_rows = 1; static void fk_inv_reset(void) { fk_inv_rows = 1; } static void fk_inv_push(long long row) { fk_inv_rows = fk_list_push(fk_inv_rows, row); } static void fk_inv_walk(const char *root, const char *dir, const char *suf, long long skipv) { (void)root; (void)dir; (void)suf; (void)skipv; }
#endif
 
#if defined(_WIN32)
typedef unsigned long long fk_os_socket_t;
struct fk_wsadata { unsigned short wVersion; unsigned short wHighVersion; char szDescription[257]; char szSystemStatus[129]; unsigned short iMaxSockets; unsigned short iMaxUdpDg; char *lpVendorInfo; };
extern int WSAStartup(unsigned short, struct fk_wsadata *);
extern fk_os_socket_t socket(int,int,int);
extern int bind(fk_os_socket_t,const void*,int);
extern int listen(fk_os_socket_t,int);
extern fk_os_socket_t accept(fk_os_socket_t,void*,void*);
extern int connect(fk_os_socket_t,const void*,int);
extern int getsockname(fk_os_socket_t,void*,int*);
extern int setsockopt(fk_os_socket_t,int,int,const char*,int);
extern int closesocket(fk_os_socket_t);
extern int recv(fk_os_socket_t,char*,int,int);
extern int send(fk_os_socket_t,const char*,int,int);
#define FK_INVALID_SOCKET ((fk_os_socket_t)(~0ULL))
#define FK_SOL_SOCKET_NATIVE 65535
#define FK_SO_REUSEADDR_NATIVE 4
static void fk_sock_boot(void) { static int ready = 0; if (ready == 0) { struct fk_wsadata w; if (WSAStartup(0x0202, &w) == 0) { ready = 1; } } }
static int fk_os_socket_ok(fk_os_socket_t s) { return s != FK_INVALID_SOCKET; }
static int fk_os_close_socket(fk_os_socket_t s) { return closesocket(s); }
static long long fk_os_recv_socket(fk_os_socket_t s, void *buf, unsigned long n) { if (n > 2147483647UL) { n = 2147483647UL; } return (long long)recv(s, (char *)buf, (int)n, 0); }
static long long fk_os_send_socket(fk_os_socket_t s, const void *buf, unsigned long n) { if (n > 2147483647UL) { n = 2147483647UL; } return (long long)send(s, (const char *)buf, (int)n, 0); }
static int fk_os_setsockopt_reuse(fk_os_socket_t s, int *yes) { return setsockopt(s, FK_SOL_SOCKET_NATIVE, FK_SO_REUSEADDR_NATIVE, (const char *)yes, 4); }
#else
typedef int fk_os_socket_t;
extern int socket(int,int,int);
extern int bind(int,const void*,unsigned int);
extern int listen(int,int);
extern long accept(int,void*,void*);
extern int connect(int,const void*,unsigned int);
extern int getsockname(int,void*,unsigned int*);
extern int setsockopt(int,int,int,const void*,unsigned int);
extern long long recv(int,void*,unsigned long,int);
extern long long send(int,const void*,unsigned long,int);
#define FK_INVALID_SOCKET (-1)
#if defined(__linux__)
#define FK_SOL_SOCKET_NATIVE 1
#define FK_SO_REUSEADDR_NATIVE 2
#else
#define FK_SOL_SOCKET_NATIVE 65535
#define FK_SO_REUSEADDR_NATIVE 4
#endif
static void fk_sock_boot(void) { }
static int fk_os_socket_ok(fk_os_socket_t s) { return s >= 0; }
static int fk_os_close_socket(fk_os_socket_t s) { return close(s); }
static long long fk_os_recv_socket(fk_os_socket_t s, void *buf, unsigned long n) { return recv(s, buf, n, 0); }
static long long fk_os_send_socket(fk_os_socket_t s, const void *buf, unsigned long n) { return send(s, buf, n, 0); }
static int fk_os_setsockopt_reuse(fk_os_socket_t s, int *yes) { return setsockopt(s, FK_SOL_SOCKET_NATIVE, FK_SO_REUSEADDR_NATIVE, yes, 4); }
#endif
#if defined(_WIN32)
struct addrinfo { int ai_flags; int ai_family; int ai_socktype; int ai_protocol; unsigned long long ai_addrlen; char *ai_canonname; void *ai_addr; struct addrinfo *ai_next; };
#elif defined(__linux__)
struct addrinfo { int ai_flags; int ai_family; int ai_socktype; int ai_protocol; unsigned int ai_addrlen; void *ai_addr; char *ai_canonname; struct addrinfo *ai_next; };
#else
struct addrinfo { int ai_flags; int ai_family; int ai_socktype; int ai_protocol; unsigned int ai_addrlen; char *ai_canonname; void *ai_addr; struct addrinfo *ai_next; };
#endif
extern int getaddrinfo(const char *, const char *, const struct addrinfo *, struct addrinfo **); extern void freeaddrinfo(struct addrinfo *);
struct fk_sockaddr4 {
#if defined(__APPLE__)
 unsigned char len; unsigned char fam;
#else
 unsigned short fam;
#endif
 unsigned char p[2]; unsigned int addr; unsigned char z[8];
};
static void fk_sockaddr4_set(struct fk_sockaddr4 *a, long long port, unsigned int addr) {
#if defined(__APPLE__)
 a->len = 16; a->fam = 2;
#else
 a->fam = 2;
#endif
 a->p[0] = (unsigned char)((port >> 8) & 255); a->p[1] = (unsigned char)(port & 255); a->addr = addr; int z = 0; while (z < 8) { a->z[z] = 0; z = z + 1; }
}
static fk_os_socket_t fk_sock_raw[1024]; static int fk_sock_kind[1024];
static long long fk_sock_alloc(fk_os_socket_t s, int kind) { long long h = 1; while (h < 1024) { if (fk_sock_kind[h] == 0) { fk_sock_raw[h] = s; fk_sock_kind[h] = kind; return h; } h = h + 1; } fk_os_close_socket(s); return -1; }
static fk_os_socket_t fk_sock_lookup(long long h, int kind) { if (h < 1 || h >= 1024 || fk_sock_kind[h] == 0) { return FK_INVALID_SOCKET; } if (kind != 0 && fk_sock_kind[h] != kind) { return FK_INVALID_SOCKET; } return fk_sock_raw[h]; }
static long long fk_socket_listen_native(long long port) { fk_sock_boot(); fk_os_socket_t s = socket(2, 1, 0); if (!fk_os_socket_ok(s)) { return -1; } int yes = 1; fk_os_setsockopt_reuse(s, &yes); struct fk_sockaddr4 a; fk_sockaddr4_set(&a, port, 0); if (bind(s, &a, 16) < 0) { fk_os_close_socket(s); return -1; } if (listen(s, 16) < 0) { fk_os_close_socket(s); return -1; } return fk_sock_alloc(s, 1); }
static long long fk_socket_port_native(long long h) { fk_os_socket_t s = fk_sock_lookup(h, 1); if (!fk_os_socket_ok(s)) { return -1; } struct fk_sockaddr4 a; int n = 16; if (getsockname(s, &a, &n) < 0) { return -1; } return (((long long)a.p[0]) << 8) + (long long)a.p[1]; }
static long long fk_socket_accept_native(long long h) { fk_os_socket_t s = fk_sock_lookup(h, 1); if (!fk_os_socket_ok(s)) { return -1; } fk_sock_boot(); fk_os_socket_t c = (fk_os_socket_t)accept(s, 0, 0); if (!fk_os_socket_ok(c)) { return -1; } return fk_sock_alloc(c, 2); }
static long long fk_socket_connect_native(long long hostv, long long portv) { fk_sock_boot(); char host[512]; char port[32]; fk_cstr(hostv, host, 512); sprintf(port, "%lld", portv); struct addrinfo hints; hints.ai_flags = 0; hints.ai_family = 0; hints.ai_socktype = 1; hints.ai_protocol = 0; hints.ai_addrlen = 0; hints.ai_canonname = 0; hints.ai_addr = 0; hints.ai_next = 0; struct addrinfo *res = 0; if (getaddrinfo(host, port, &hints, &res) != 0 || res == 0) { return -1; } fk_os_socket_t s = FK_INVALID_SOCKET; struct addrinfo *rp = res; while (rp != 0) { s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); if (fk_os_socket_ok(s)) { if (connect(s, rp->ai_addr, (unsigned int)rp->ai_addrlen) == 0) { break; } fk_os_close_socket(s); s = FK_INVALID_SOCKET; } rp = rp->ai_next; } freeaddrinfo(res); if (!fk_os_socket_ok(s)) { return -1; } return fk_sock_alloc(s, 2); }
static long long fk_socket_send_native(long long h, long long sv) { fk_os_socket_t s = fk_sock_lookup(h, 2); long long sa = sv >> 1; if (!fk_os_socket_ok(s) || sa < 0 || sa >= fk_sp) { return -1; } return fk_os_send_socket(s, fk_sb + fk_so[sa], (unsigned long)fk_sl[sa]); }
static long long fk_socket_recv_native(long long h, long long maxn) { fk_os_socket_t s = fk_sock_lookup(h, 2); if (!fk_os_socket_ok(s) || maxn <= 0) { return fk_sbuf("", 0); } static char tmp[65536]; if (maxn > 65536) { maxn = 65536; } long long got = fk_os_recv_socket(s, tmp, (unsigned long)maxn); if (got <= 0) { return fk_sbuf("", 0); } return fk_sbuf(tmp, got); }
static long long fk_socket_close_native(long long h) { fk_os_socket_t s = fk_sock_lookup(h, 0); if (!fk_os_socket_ok(s)) { return -1; } fk_sock_kind[h] = 0; if (fk_os_close_socket(s) == 0) { return 0; } return -1; }
/* ── live MESH transport (host-kernel.form world-net port): the Windows cell streams its
   live senses over TCP into the mesh; a mesh endpoint receives them. The socket move IS a
   host carrier (like camera/mic); the readings are the mesh-safe rows. sense_publish(port)
   connects 127.0.0.1:port and sends the live readings; mesh_serve(port) listens/accepts/
   recvs/prints one message (the receiver / relay tap). Point the host at the Mac's
   field-relay to make it cross-device; loopback witnesses it. */
#if defined(_WIN32)
static long long fk_sense_publish(long long port) {
 static char buf[4096]; int n = 0;
 long long mic = fk_mic_count(); long long cam = fk_cam_count();
 char ssid[64]; long long sig = -1; fk_wifi_query(ssid, 64, &sig);
 long long bt = fk_bt_present(); long long pw = fk_power(); long long mm = fk_memload();
 n = n + sprintf(buf + n, "cell=windows-binary\n");
 n = n + sprintf(buf + n, "reading present  cam=%d mic=%d\n", (int)cam, (int)mic);
 n = n + sprintf(buf + n, "reading where    wifi=%s sig=%d bt=%d\n", ssid[0] ? ssid : "-", (int)sig, (int)bt);
 n = n + sprintf(buf + n, "reading vitality battery=%d mem=%d\n", (int)pw, (int)mm);
 /* relay host: env MESH_RELAY=a.b.c.d (the Mac's field-relay), default 127.0.0.1 — cross-device. */
 unsigned int addr = 0x0100007f; char *rl = getenv("MESH_RELAY");
 if (rl != 0) { unsigned int o0 = 0, o1 = 0, o2 = 0, o3 = 0; long long k = 0; unsigned int *cur = &o0; int part = 0;
  while (rl[k] != 0) { char ch = rl[k]; if (ch >= 48 && ch <= 57) { *cur = (*cur) * 10 + (unsigned int)(ch - 48); } else if (ch == 46 && part < 3) { part = part + 1; cur = (part == 1) ? &o1 : (part == 2) ? &o2 : &o3; } k = k + 1; }
  if (part == 3) { addr = (o0 & 255) | ((o1 & 255) << 8) | ((o2 & 255) << 16) | ((o3 & 255) << 24); } }
 fk_sock_boot();
 fk_os_socket_t s = socket(2, 1, 0);
 if (!fk_os_socket_ok(s)) { return -1; }
 struct fk_sockaddr4 a; fk_sockaddr4_set(&a, port, addr); /* MESH_RELAY or 127.0.0.1 */
 if (connect(s, &a, 16) != 0) { fk_os_close_socket(s); return -2; }
 long long sent = fk_os_send_socket(s, buf, (unsigned long)n);
 fk_os_close_socket(s);
 return sent;
}
static long long fk_mesh_serve(long long port) {
 fk_sock_boot();
 fk_os_socket_t ls = socket(2, 1, 0);
 if (!fk_os_socket_ok(ls)) { return -1; }
 int yes = 1; fk_os_setsockopt_reuse(ls, &yes);
 struct fk_sockaddr4 a; fk_sockaddr4_set(&a, port, 0);
 if (bind(ls, &a, 16) != 0) { fk_os_close_socket(ls); return -2; }
 if (listen(ls, 1) != 0) { fk_os_close_socket(ls); return -3; }
 fk_os_socket_t cs = accept(ls, 0, 0);
 if (!fk_os_socket_ok(cs)) { fk_os_close_socket(ls); return -4; }
 static char rbuf[8192]; long long got = fk_os_recv_socket(cs, rbuf, 8191);
 if (got > 0) { long long j = 0; while (j < got) { putchar((int)(unsigned char)rbuf[j]); j = j + 1; } }
 fk_os_close_socket(cs); fk_os_close_socket(ls);
 return got;
}
/* ── mesh AUTO-DISCOVERY (no copy-pasted peer address): announce presence + readings by
   UDP BROADCAST to the LAN; discover peers by listening for theirs. The cell JOINS the
   mesh over whatever channel is available — broadcast on 255.255.255.255:port — and finds
   the others, rather than being handed an IP. mesh_announce(port) broadcasts; mesh_discover
   (port) receives one peer's announce. This supersedes the MESH_RELAY env (a hand-config). */
extern int sendto(fk_os_socket_t, const char *, int, int, const void *, int);
extern int recvfrom(fk_os_socket_t, char *, int, int, void *, int *);
static long long fk_mesh_announce(long long port) {
 static char buf[4096]; int n = 0;
 long long mic = fk_mic_count(); long long cam = fk_cam_count();
 char ssid[64]; long long sig = -1; fk_wifi_query(ssid, 64, &sig);
 long long bt = fk_bt_present(); long long pw = fk_power(); long long mm = fk_memload();
 n = n + sprintf(buf + n, "cell=windows-binary\n");
 n = n + sprintf(buf + n, "reading present  cam=%d mic=%d\n", (int)cam, (int)mic);
 n = n + sprintf(buf + n, "reading where    wifi=%s sig=%d bt=%d\n", ssid[0] ? ssid : "-", (int)sig, (int)bt);
 n = n + sprintf(buf + n, "reading vitality battery=%d mem=%d\n", (int)pw, (int)mm);
 fk_sock_boot();
 fk_os_socket_t s = socket(2, 2, 0); /* AF_INET, SOCK_DGRAM */
 if (!fk_os_socket_ok(s)) { return -1; }
 int yes = 1; setsockopt(s, 65535, 32, (const char *)&yes, 4); /* SOL_SOCKET, SO_BROADCAST */
 struct fk_sockaddr4 a; fk_sockaddr4_set(&a, port, 0xffffffff); /* 255.255.255.255 — LAN broadcast, NO peer address */
 long long sent = sendto(s, buf, (int)n, 0, &a, 16);
 fk_os_close_socket(s);
 return sent;
}
static long long fk_mesh_discover(long long port) {
 fk_sock_boot();
 fk_os_socket_t s = socket(2, 2, 0); /* DGRAM */
 if (!fk_os_socket_ok(s)) { return -1; }
 int yes = 1; fk_os_setsockopt_reuse(s, &yes);
 struct fk_sockaddr4 a; fk_sockaddr4_set(&a, port, 0); /* INADDR_ANY — listen for any peer's broadcast */
 if (bind(s, &a, 16) != 0) { fk_os_close_socket(s); return -2; }
 static char rbuf[8192]; struct fk_sockaddr4 from; int fromlen = 16;
 long long got = recvfrom(s, rbuf, 8191, 0, &from, &fromlen);
 if (got > 0) { long long j = 0; while (j < got) { putchar((int)(unsigned char)rbuf[j]); j = j + 1; } }
 fk_os_close_socket(s);
 return got;
}
/* ── mesh RENDEZVOUS SERVER (the new-repo core, proven on the kernel's own carriers):
   PUBLIC ACCESS via a listening socket, PERSISTENCE via an append-only registry file,
   DISCOVERY via the roster read-back. mesh_registry(port) accepts one registration, appends
   it to mesh-cells.txt, acks; mesh_roster() reads the persisted registry. This is the server's
   CARRIER layer; its routing/registry LOGIC is Form (comes home as .fk at the cursor seed). */
static long long fk_mesh_registry(long long port) {
 fk_sock_boot();
 fk_os_socket_t ls = socket(2, 1, 0);
 if (!fk_os_socket_ok(ls)) { return -1; }
 int yes = 1; fk_os_setsockopt_reuse(ls, &yes);
 struct fk_sockaddr4 a; fk_sockaddr4_set(&a, port, 0);
 if (bind(ls, &a, 16) != 0) { fk_os_close_socket(ls); return -2; }
 if (listen(ls, 4) != 0) { fk_os_close_socket(ls); return -3; }
 fk_os_socket_t cs = accept(ls, 0, 0);
 if (!fk_os_socket_ok(cs)) { fk_os_close_socket(ls); return -4; }
 static char rbuf[8192]; long long got = fk_os_recv_socket(cs, rbuf, 8191);
 if (got > 0) {
  int fd = open("mesh-cells.txt", 1 | 0x100 | 0x8 | 0x8000, 0666); /* O_WRONLY|O_CREAT|O_APPEND|O_BINARY — append-only registry */
  if (fd >= 0) { write(fd, rbuf, (unsigned long)got); write(fd, "\n---\n", 5); close(fd); }
  fk_os_send_socket(cs, "registered\n", 11);
 }
 fk_os_close_socket(cs); fk_os_close_socket(ls);
 return got;
}
static long long fk_mesh_roster(void) {
 int fd = open("mesh-cells.txt", 0x8000); /* O_RDONLY|O_BINARY — read the persisted registry */
 if (fd < 0) { return 0; }
 static char buf[65536]; long long n = 0; long long g;
 while ((g = read(fd, buf + n, 8192)) > 0) { n = n + g; if (n > 60000) { break; } }
 close(fd);
 long long j = 0; while (j < n) { putchar((int)(unsigned char)buf[j]); j = j + 1; }
 return n;
}
#else
static long long fk_sense_publish(long long port) { (void)port; return -1; }
static long long fk_mesh_serve(long long port) { (void)port; return -1; }
static long long fk_mesh_announce(long long port) { (void)port; return -1; }
static long long fk_mesh_discover(long long port) { (void)port; return -1; }
static long long fk_mesh_registry(long long port) { (void)port; return -1; }
static long long fk_mesh_roster(void) { return -1; }
#endif
/* ── PUBLIC-API proxy channel (cross-network rendezvous): register the cell + detect peers
   through https://api.coherencycoin.com over Windows-native TLS (WinHTTP — the kernel's
   libcrypto TLS is unavailable on Windows). LAN broadcast (mesh_announce/discover) joins
   same-network cells; this proxy joins cells on DIFFERENT networks via the public API. */
#if defined(_WIN32)
extern void *WinHttpOpen(const unsigned short *, unsigned long, const unsigned short *, const unsigned short *, unsigned long);
extern void *WinHttpConnect(void *, const unsigned short *, unsigned short, unsigned long);
extern void *WinHttpOpenRequest(void *, const unsigned short *, const unsigned short *, const unsigned short *, const unsigned short *, const unsigned short **, unsigned long);
extern int WinHttpSendRequest(void *, const unsigned short *, unsigned long, void *, unsigned long, unsigned long, unsigned long long);
extern int WinHttpReceiveResponse(void *, void *);
extern int WinHttpQueryDataAvailable(void *, unsigned long *);
extern int WinHttpReadData(void *, void *, unsigned long, unsigned long *);
extern int WinHttpCloseHandle(void *);
static void fk_widen(const char *s, unsigned short *w, long long cap) { long long i = 0; while (s[i] != 0 && i < cap - 1) { w[i] = (unsigned short)(unsigned char)s[i]; i = i + 1; } w[i] = 0; }
static long long fk_https(const char *path, const char *method, const char *body, long long blen, char *out, long long cap) {
 static unsigned short wa[16], wh[64], wp[512], wm[8], wct[64];
 fk_widen("fkwu", wa, 16); fk_widen("api.coherencycoin.com", wh, 64); fk_widen(path, wp, 512); fk_widen(method, wm, 8);
 void *hS = WinHttpOpen(wa, 0, 0, 0, 0); /* DEFAULT_PROXY */
 if (hS == 0) { return -1; }
 void *hC = WinHttpConnect(hS, wh, 443, 0);
 if (hC == 0) { WinHttpCloseHandle(hS); return -2; }
 void *hR = WinHttpOpenRequest(hC, wm, wp, 0, 0, 0, 0x00800000); /* WINHTTP_FLAG_SECURE */
 if (hR == 0) { WinHttpCloseHandle(hC); WinHttpCloseHandle(hS); return -3; }
 const unsigned short *hdr = 0; unsigned long hdrlen = 0;
 if (blen > 0) { fk_widen("Content-Type: application/json\r\n", wct, 64); hdr = wct; hdrlen = 0xFFFFFFFFu; }
 long long rc = -4;
 if (WinHttpSendRequest(hR, hdr, hdrlen, (void *)body, (unsigned long)blen, (unsigned long)blen, 0) && WinHttpReceiveResponse(hR, 0)) {
  long long total = 0; unsigned long avail = 0;
  while (WinHttpQueryDataAvailable(hR, &avail) && avail > 0) {
   if (total + (long long)avail > cap - 1) { avail = (unsigned long)(cap - 1 - total); }
   if (avail == 0) { break; }
   unsigned long got = 0;
   if (!WinHttpReadData(hR, out + total, avail, &got) || got == 0) { break; }
   total = total + (long long)got;
  }
  out[total] = 0; rc = total;
 }
 WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS);
 return rc;
}
static long long fk_api_health(void) { static char out[8192]; long long n = fk_https("/api/health", "GET", 0, 0, out, 8192); if (n > 0) { long long j = 0; while (j < n) { putchar((int)(unsigned char)out[j]); j = j + 1; } putchar(10); } return n; }
static long long fk_mesh_register(void) {
 static char body[4096]; int n;
 long long mic = fk_mic_count(); long long cam = fk_cam_count();
 char ssid[64]; long long sig = -1; fk_wifi_query(ssid, 64, &sig);
 long long pw = fk_power(); long long mm = fk_memload();
 n = sprintf(body, "{\"cell\":\"windows-binary\",\"present\":{\"cam\":%d,\"mic\":%d},\"where\":{\"wifi\":\"%s\",\"sig\":%d},\"vitality\":{\"battery\":%d,\"mem\":%d}}", (int)cam, (int)mic, ssid[0] ? ssid : "-", (int)sig, (int)pw, (int)mm);
 static char out[8192]; long long r = fk_https("/api/mesh/register", "POST", body, n, out, 8192);
 if (r > 0) { long long j = 0; while (j < r) { putchar((int)(unsigned char)out[j]); j = j + 1; } putchar(10); }
 return r;
}
static long long fk_mesh_detect(void) { static char out[16384]; long long n = fk_https("/api/mesh/cells", "GET", 0, 0, out, 16384); if (n > 0) { long long j = 0; while (j < n) { putchar((int)(unsigned char)out[j]); j = j + 1; } putchar(10); } return n; }
#else
static long long fk_api_health(void) { return -1; }
static long long fk_mesh_register(void) { return -1; }
static long long fk_mesh_detect(void) { return -1; }
#endif
static int fk_sock_getaddrinfo(const char *h, const char *p, const struct addrinfo *i, struct addrinfo **r) { fk_sock_boot(); return getaddrinfo(h, p, i, r); }
static int fk_sock_socket(int af, int ty, int pr) { fk_sock_boot(); fk_os_socket_t s = socket(af, ty, pr); if (!fk_os_socket_ok(s)) { return -1; } return (int)s; }
static int fk_sock_connect(int fd, const void *a, unsigned int n) { fk_sock_boot(); return connect((fk_os_socket_t)(unsigned int)fd, a, n); }
static int fk_sock_close(int fd) { return fk_os_close_socket((fk_os_socket_t)(unsigned int)fd); }
static long long fk_sock_read(int fd, void *buf, unsigned long n) { return fk_os_recv_socket((fk_os_socket_t)(unsigned int)fd, buf, n); }
static long long fk_sock_write(int fd, const void *buf, unsigned long n) { return fk_os_send_socket((fk_os_socket_t)(unsigned int)fd, buf, n); }
struct timeval { long tv_sec; int tv_usec; }; extern int gettimeofday(struct timeval *, void *); static long long fk_now_ms(void) { struct timeval tv; if (gettimeofday(&tv, 0) != 0) { return 0; } return ((long long)tv.tv_sec * 1000LL) + ((long long)tv.tv_usec / 1000LL); } static long long fk_elapsed_ms(long long start) { long long end = fk_now_ms(); if (start <= 0 || end <= start) { return 1; } return end - start; }
 static void fk_arena(void); static long long fk_cons_val(long long h, long long t) { if (fk_cap == 0) { fk_arena(); } if (fk_hp + 1 >= fk_cap) { return 1; } fk_hp = fk_hp + 1; fk_hh[fk_hp] = h; fk_ht[fk_hp] = t; return (fk_hp << 1) | 1; } static long long fk_http_dict(long long status, long long body, long long err) { long long d = 1; d = fk_cons_val(1, d); d = fk_cons_val(fk_sbuf("headers", 7), d); d = fk_cons_val(0, d); d = fk_cons_val(fk_sbuf("duration_ms", 11), d); d = fk_cons_val(err, d); d = fk_cons_val(fk_sbuf("error", 5), d); d = fk_cons_val(body, d); d = fk_cons_val(fk_sbuf("body", 4), d); d = fk_cons_val(status << 1, d); d = fk_cons_val(fk_sbuf("status_code", 11), d); d = fk_cons_val(fk_sbuf("__dict__", 8), d); return d; } static int fk_starts(const char *s, const char *p) { long long i = 0; while (p[i] != 0) { if (s[i] != p[i]) { return 0; } i = i + 1; } return 1; } static long long fk_http_status(const char *buf, long long n) { long long i = 0; while (i < n && buf[i] != 32) { i = i + 1; } while (i < n && buf[i] == 32) { i = i + 1; } long long v = 0; while (i < n && buf[i] >= 48 && buf[i] <= 57) { v = v * 10 + (buf[i] - 48); i = i + 1; } return v; } static long long fk_http_body_offset(const char *buf, long long n) { long long i = 0; while (i + 3 < n) { if (buf[i] == 13 && buf[i + 1] == 10 && buf[i + 2] == 13 && buf[i + 3] == 10) { return i + 4; } i = i + 1; } i = 0; while (i + 1 < n) { if (buf[i] == 10 && buf[i + 1] == 10) { return i + 2; } i = i + 1; } return n; } static long long fk_http_headers(const char *, long long, long long); static long long fk_http_dict_with_headers(long long, long long, long long, long long, long long); static long long fk_http_append_request_headers(char *, long long, long long, long long); static long long fk_http_get_plain(long long urlv, long long headersv, long long timeoutv) { (void)timeoutv; long long started = fk_now_ms(); char url[2048]; char host[512]; char path[1536]; char port[16]; fk_cstr(urlv, url, 2048); if (!fk_starts(url, "http://")) { return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: fkwu floor supports http:// only", 41)); } long long p = 7; long long h = 0; while (url[p] != 0 && url[p] != 47 && url[p] != 58 && h < 511) { host[h] = url[p]; h = h + 1; p = p + 1; } host[h] = 0; port[0] = 56; port[1] = 48; port[2] = 0; if (url[p] == 58) { p = p + 1; long long pi = 0; while (url[p] >= 48 && url[p] <= 57 && pi < 15) { port[pi] = url[p]; pi = pi + 1; p = p + 1; } port[pi] = 0; } long long q = 0; if (url[p] == 47) { while (url[p] != 0 && q < 1535) { path[q] = url[p]; q = q + 1; p = p + 1; } } else { path[q] = 47; q = 1; } path[q] = 0; if (h == 0) { return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: empty host", 20)); } struct addrinfo hints; hints.ai_flags = 0; hints.ai_family = 0; hints.ai_socktype = 1; hints.ai_protocol = 0; hints.ai_addrlen = 0; hints.ai_canonname = 0; hints.ai_addr = 0; hints.ai_next = 0; struct addrinfo *res = 0; if (getaddrinfo(host, port, &hints, &res) != 0 || res == 0) { return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: resolve failed", 24)); } int fd = -1; struct addrinfo *rp = res; while (rp != 0) { fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); if (fd >= 0) { if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) { break; } close(fd); fd = -1; } rp = rp->ai_next; } freeaddrinfo(res); if (fd < 0) { return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: connect failed", 24)); } char req[4096]; long long rn = sprintf(req, "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n", path, host); rn = fk_http_append_request_headers(req, rn, 4096, headersv); if (rn + 2 < 4096) { req[rn] = 13; req[rn + 1] = 10; rn = rn + 2; } long long wr = 0; while (wr < rn) { long long nwr = write(fd, req + wr, rn - wr); if (nwr <= 0) { close(fd); return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: write failed", 22)); } wr = wr + nwr; } static char resp[65536]; long long total = 0; while (total < 65535) { long long got = read(fd, resp + total, 65535 - total); if (got <= 0) { break; } total = total + got; } close(fd); resp[total] = 0; long long status = fk_http_status(resp, total); long long bo = fk_http_body_offset(resp, total); if (bo > total) { bo = total; } return fk_http_dict_with_headers(status, fk_http_headers(resp, total, bo), fk_sbuf(resp + bo, total - bo), fk_sbuf("", 0), fk_elapsed_ms(started)); } static int fk_http_lit_eq_ci(const char *buf, long long n, const char *lit) { long long i = 0; while (i < n && lit[i] != 0) { char a = buf[i]; char b = lit[i]; if (a >= 65 && a <= 90) { a = a + 32; } if (b >= 65 && b <= 90) { b = b + 32; } if (a != b) { return 0; } i = i + 1; } return i == n && lit[i] == 0; } static int fk_http_header_name_ok(const char *buf, long long n) { if (n <= 0 || n > 128) { return 0; } long long i = 0; while (i < n) { unsigned char c = (unsigned char)buf[i]; if (!((c >= 65 && c <= 90) || (c >= 97 && c <= 122) || (c >= 48 && c <= 57) || c == 45)) { return 0; } i = i + 1; } if (fk_http_lit_eq_ci(buf, n, "host") || fk_http_lit_eq_ci(buf, n, "connection") || fk_http_lit_eq_ci(buf, n, "content-length") || fk_http_lit_eq_ci(buf, n, "transfer-encoding") || fk_http_lit_eq_ci(buf, n, "proxy-connection") || fk_http_lit_eq_ci(buf, n, "keep-alive") || fk_http_lit_eq_ci(buf, n, "upgrade") || fk_http_lit_eq_ci(buf, n, "te") || fk_http_lit_eq_ci(buf, n, "trailer")) { return 0; } return 1; } static int fk_http_header_value_ok(const char *buf, long long n) { if (n < 0 || n > 1024) { return 0; } long long i = 0; while (i < n) { unsigned char c = (unsigned char)buf[i]; if (c == 0 || c == 10 || c == 13 || c == 127) { return 0; } if (c < 32 && c != 9) { return 0; } i = i + 1; } return 1; } static long long fk_http_append_bytes(char *out, long long pos, long long cap, const char *buf, long long n) { long long i = 0; while (i < n && pos + 1 < cap) { out[pos] = buf[i]; pos = pos + 1; i = i + 1; } return pos; } static long long fk_http_append_request_headers(char *req, long long rn, long long cap, long long headersv) { long long q = headersv >> 1; long long count = 0; while (q >= 1 && q <= fk_hp && count < 64) { long long row = fk_hh[q]; if ((row & 1) != 0) { long long rp = row >> 1; if (rp >= 1 && rp <= fk_hp && ((fk_hh[rp] >> 1) == 43001)) { long long np = fk_ht[rp] >> 1; if (np >= 1 && np <= fk_hp) { long long namev = fk_hh[np]; long long vp = fk_ht[np] >> 1; if (vp >= 1 && vp <= fk_hp) { long long valuev = fk_hh[vp]; long long ns = namev >> 1; long long vs = valuev >> 1; if ((namev & 1) == 0 && (valuev & 1) == 0 && ns >= 0 && ns < fk_sp && vs >= 0 && vs < fk_sp) { const char *name = fk_sb + fk_so[ns]; const char *value = fk_sb + fk_so[vs]; long long nl = fk_sl[ns]; long long vl = fk_sl[vs]; if (fk_http_header_name_ok(name, nl) && fk_http_header_value_ok(value, vl) && rn + nl + vl + 4 < cap) { rn = fk_http_append_bytes(req, rn, cap, name, nl); rn = fk_http_append_bytes(req, rn, cap, ": ", 2); rn = fk_http_append_bytes(req, rn, cap, value, vl); rn = fk_http_append_bytes(req, rn, cap, "\r\n", 2); count = count + 1; } } } } } } q = fk_ht[q] >> 1; } return rn; } static long long fk_http_header_row(const char *name, long long nl, const char *value, long long vl) { long long row = 1; row = fk_cons_val(fk_sbuf(value, vl), row); row = fk_cons_val(fk_sbuf(name, nl), row); row = fk_cons_val(43001LL << 1, row); return row; } static long long fk_http_headers(const char *buf, long long n, long long bo) { long long out = 1; long long end = bo; if (end > n) { end = n; } long long i = 0; while (i < end && buf[i] != 10) { i = i + 1; } if (i < end && buf[i] == 10) { i = i + 1; } long long count = 0; while (i < end && count < 128) { if (buf[i] == 13 || buf[i] == 10) { break; } long long ls = i; while (i < end && buf[i] != 10) { i = i + 1; } long long le = i; if (le > ls && buf[le - 1] == 10) { le = le - 1; } if (le > ls && buf[le - 1] == 13) { le = le - 1; } long long colon = ls; while (colon < le && buf[colon] != 58) { colon = colon + 1; } if (colon < le && colon > ls) { long long ns = ls; long long ne = colon; while (ne > ns && (buf[ne - 1] == 32 || buf[ne - 1] == 9)) { ne = ne - 1; } long long vs = colon + 1; while (vs < le && (buf[vs] == 32 || buf[vs] == 9)) { vs = vs + 1; } long long ve = le; while (ve > vs && (buf[ve - 1] == 32 || buf[ve - 1] == 9)) { ve = ve - 1; } if (ne > ns) { out = fk_cons_val(fk_http_header_row(buf + ns, ne - ns, buf + vs, ve - vs), out); count = count + 1; } } if (i < end && buf[i] == 10) { i = i + 1; } } return out; } static long long fk_http_dict_with_headers(long long status, long long headers, long long body, long long err, long long duration) { long long d = 1; d = fk_cons_val(headers, d); d = fk_cons_val(fk_sbuf("headers", 7), d); d = fk_cons_val(duration << 1, d); d = fk_cons_val(fk_sbuf("duration_ms", 11), d); d = fk_cons_val(err, d); d = fk_cons_val(fk_sbuf("error", 5), d); d = fk_cons_val(body, d); d = fk_cons_val(fk_sbuf("body", 4), d); d = fk_cons_val(status << 1, d); d = fk_cons_val(fk_sbuf("status_code", 11), d); d = fk_cons_val(fk_sbuf("__dict__", 8), d); return d; } static long long fk_host_exec(long long cmdv, long long inputv) { (void)inputv; char cmd[8192]; fk_cstr(cmdv, cmd, 8192); void *fp = popen(cmd, "r"); if (fp == 0) { return fk_sbuf("", 0); } static char hbuf[262144]; long long total = 0; while (total < 262143) { unsigned long got = fread(hbuf + total, 1, (unsigned long)(262143 - total), fp); if (got == 0) { break; } total = total + (long long)got; } pclose(fp); return fk_sbuf(hbuf, total); } static long long fk_sock_request(long long hostv, long long portv, long long reqv) { char host[512]; char port[16]; fk_cstr(hostv, host, 512); fk_cstr(portv, port, 16); long long rsa = reqv >> 1; long long rlen = (rsa >= 0 && rsa < fk_sp) ? fk_sl[rsa] : 0; struct addrinfo hints; hints.ai_flags = 0; hints.ai_family = 0; hints.ai_socktype = 1; hints.ai_protocol = 0; hints.ai_addrlen = 0; hints.ai_canonname = 0; hints.ai_addr = 0; hints.ai_next = 0; struct addrinfo *res = 0; if (getaddrinfo(host, port, &hints, &res) != 0 || res == 0) { return fk_sbuf("", 0); } int fd = -1; struct addrinfo *rp = res; while (rp != 0) { fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); if (fd >= 0) { if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) { break; } close(fd); fd = -1; } rp = rp->ai_next; } freeaddrinfo(res); if (fd < 0) { return fk_sbuf("", 0); } const char *rptr = fk_sb + fk_so[rsa]; long long wr = 0; while (wr < rlen) { long long nwr = write(fd, rptr + wr, rlen - wr); if (nwr <= 0) { break; } wr = wr + nwr; } static char resp[65536]; long long total = 0; while (total < 65535) { long long got = read(fd, resp + total, 65535 - total); if (got <= 0) { break; } total = total + got; } close(fd); return fk_sbuf(resp, total); } static long long fk_is_dict_value(long long v) { if ((v & 1) == 0) { return 0; } long long p = v >> 1; if (p < 1 || p > fk_hp) { return 0; } long long marker = fk_sbuf("__dict__", 8); long long h = fk_hh[p]; if ((h & 1) != 0) { return 0; } return fk_keyeq(h >> 1, marker >> 1); } static long long fk_get_value(long long target, long long key) { if (fk_is_dict_value(target)) { long long p = fk_ht[target >> 1] >> 1; long long ks = key >> 1; while (p >= 1 && p <= fk_hp) { long long k = fk_hh[p]; long long vp = fk_ht[p] >> 1; if (vp < 1 || vp > fk_hp) { return 0; } if ((k & 1) == 0 && (key & 1) == 0 && fk_keyeq(k >> 1, ks)) { return fk_hh[vp]; } p = fk_ht[vp] >> 1; } return 0; } if ((target & 1) != 0) { long long want = key >> 1; long long p = target >> 1; while (p >= 1 && p <= fk_hp && want > 0) { p = fk_ht[p] >> 1; want = want - 1; } if (p >= 1 && p <= fk_hp) { return fk_hh[p]; } } return 0; } extern void *dlopen(const char *, int); extern void *dlsym(void *, const char *); typedef const void *(*fk_tls_method_fn)(void); typedef void *(*fk_ctx_new_fn)(const void *); typedef void (*fk_ctx_free_fn)(void *); typedef void *(*fk_ssl_new_fn)(void *); typedef void (*fk_ssl_free_fn)(void *); typedef int (*fk_ssl_set_fd_fn)(void *, int); typedef long (*fk_ssl_ctrl_fn)(void *, int, long, void *); typedef int (*fk_ssl_set1_host_fn)(void *, const char *); typedef int (*fk_ssl_connect_fn)(void *); typedef int (*fk_ssl_write_fn)(void *, const void *, int); typedef int (*fk_ssl_read_fn)(void *, void *, int); typedef long (*fk_ssl_verify_result_fn)(const void *); typedef void (*fk_ctx_set_verify_fn)(void *, int, void *); typedef int (*fk_ctx_default_paths_fn)(void *); static void *fk_ssl_lib(void) { static void *h = 0; if (h != 0) { return h; } dlopen("/opt/homebrew/opt/openssl@3/lib/libcrypto.3.dylib", 2); dlopen("/opt/homebrew/opt/openssl@3/lib/libcrypto.dylib", 2); dlopen("libcrypto.so.3", 2); h = dlopen("/opt/homebrew/opt/openssl@3/lib/libssl.3.dylib", 2); if (h == 0) { h = dlopen("/opt/homebrew/opt/openssl@3/lib/libssl.dylib", 2); } if (h == 0) { h = dlopen("libssl.so.3", 2); } if (h == 0) { h = dlopen("libssl.dylib", 2); } return h; } static long long fk_parse_url(const char *url, const char *scheme, long long start, char *host, char *path, char *port, const char *default_port) { if (!fk_starts(url, scheme)) { return 0; } long long p = start; long long h = 0; while (url[p] != 0 && url[p] != 47 && url[p] != 58 && h < 511) { host[h] = url[p]; h = h + 1; p = p + 1; } host[h] = 0; long long pi = 0; while (default_port[pi] != 0 && pi < 15) { port[pi] = default_port[pi]; pi = pi + 1; } port[pi] = 0; if (url[p] == 58) { p = p + 1; pi = 0; while (url[p] >= 48 && url[p] <= 57 && pi < 15) { port[pi] = url[p]; pi = pi + 1; p = p + 1; } port[pi] = 0; } long long q = 0; if (url[p] == 47) { while (url[p] != 0 && q < 1535) { path[q] = url[p]; q = q + 1; p = p + 1; } } else { path[0] = 47; q = 1; } path[q] = 0; return h > 0; } static int fk_tcp_connect(const char *host, const char *port) { struct addrinfo hints; hints.ai_flags = 0; hints.ai_family = 0; hints.ai_socktype = 1; hints.ai_protocol = 0; hints.ai_addrlen = 0; hints.ai_canonname = 0; hints.ai_addr = 0; hints.ai_next = 0; struct addrinfo *res = 0; if (getaddrinfo(host, port, &hints, &res) != 0 || res == 0) { return -1; } int fd = -1; struct addrinfo *rp = res; while (rp != 0) { fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); if (fd >= 0) { if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) { break; } close(fd); fd = -1; } rp = rp->ai_next; } freeaddrinfo(res); return fd; } static long long fk_https_get_ssl(long long urlv, long long headersv, long long timeoutv) { (void)timeoutv; long long started = fk_now_ms(); char url[2048]; char host[512]; char path[1536]; char port[16]; fk_cstr(urlv, url, 2048); if (!fk_parse_url(url, "https://", 8, host, path, port, "443")) { return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: bad https url", 23)); } void *lib = fk_ssl_lib(); if (lib == 0) { return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: libssl missing", 24)); } fk_tls_method_fn TLS_client_method = (fk_tls_method_fn)dlsym(lib, "TLS_client_method"); fk_ctx_new_fn SSL_CTX_new = (fk_ctx_new_fn)dlsym(lib, "SSL_CTX_new"); fk_ctx_free_fn SSL_CTX_free = (fk_ctx_free_fn)dlsym(lib, "SSL_CTX_free"); fk_ssl_new_fn SSL_new = (fk_ssl_new_fn)dlsym(lib, "SSL_new"); fk_ssl_free_fn SSL_free = (fk_ssl_free_fn)dlsym(lib, "SSL_free"); fk_ssl_set_fd_fn SSL_set_fd = (fk_ssl_set_fd_fn)dlsym(lib, "SSL_set_fd"); fk_ssl_ctrl_fn SSL_ctrl = (fk_ssl_ctrl_fn)dlsym(lib, "SSL_ctrl"); fk_ssl_set1_host_fn SSL_set1_host = (fk_ssl_set1_host_fn)dlsym(lib, "SSL_set1_host"); fk_ssl_connect_fn SSL_connect = (fk_ssl_connect_fn)dlsym(lib, "SSL_connect"); fk_ssl_write_fn SSL_write = (fk_ssl_write_fn)dlsym(lib, "SSL_write"); fk_ssl_read_fn SSL_read = (fk_ssl_read_fn)dlsym(lib, "SSL_read"); fk_ssl_verify_result_fn SSL_get_verify_result = (fk_ssl_verify_result_fn)dlsym(lib, "SSL_get_verify_result"); fk_ctx_set_verify_fn SSL_CTX_set_verify = (fk_ctx_set_verify_fn)dlsym(lib, "SSL_CTX_set_verify"); fk_ctx_default_paths_fn SSL_CTX_set_default_verify_paths = (fk_ctx_default_paths_fn)dlsym(lib, "SSL_CTX_set_default_verify_paths"); if (TLS_client_method == 0 || SSL_CTX_new == 0 || SSL_CTX_free == 0 || SSL_new == 0 || SSL_free == 0 || SSL_set_fd == 0 || SSL_ctrl == 0 || SSL_set1_host == 0 || SSL_connect == 0 || SSL_write == 0 || SSL_read == 0 || SSL_get_verify_result == 0 || SSL_CTX_set_verify == 0 || SSL_CTX_set_default_verify_paths == 0) { return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: ssl symbol missing", 28)); } int fd = fk_tcp_connect(host, port); if (fd < 0) { return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: connect failed", 24)); } void *ctx = SSL_CTX_new(TLS_client_method()); if (ctx == 0) { close(fd); return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: ctx failed", 20)); } SSL_CTX_set_verify(ctx, 1, 0); SSL_CTX_set_default_verify_paths(ctx); void *ssl = SSL_new(ctx); if (ssl == 0) { SSL_CTX_free(ctx); close(fd); return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: ssl failed", 20)); } SSL_ctrl(ssl, 55, 0, host); if (SSL_set1_host(ssl, host) != 1) { SSL_free(ssl); SSL_CTX_free(ctx); close(fd); return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: host verify setup failed", 34)); } if (SSL_set_fd(ssl, fd) != 1 || SSL_connect(ssl) != 1) { SSL_free(ssl); SSL_CTX_free(ctx); close(fd); return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: tls connect failed", 28)); } if (SSL_get_verify_result(ssl) != 0) { SSL_free(ssl); SSL_CTX_free(ctx); close(fd); return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: tls verify failed", 27)); } char req[4096]; long long rn = sprintf(req, "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n", path, host); rn = fk_http_append_request_headers(req, rn, 4096, headersv); if (rn + 2 < 4096) { req[rn] = 13; req[rn + 1] = 10; rn = rn + 2; } long long wr = 0; while (wr < rn) { int nwr = SSL_write(ssl, req + wr, (int)(rn - wr)); if (nwr <= 0) { SSL_free(ssl); SSL_CTX_free(ctx); close(fd); return fk_http_dict(0, fk_sbuf("", 0), fk_sbuf("http_get: tls write failed", 26)); } wr = wr + nwr; } static char resp[65536]; long long total = 0; while (total < 65535) { int got = SSL_read(ssl, resp + total, (int)(65535 - total)); if (got <= 0) { break; } total = total + got; } SSL_free(ssl); SSL_CTX_free(ctx); close(fd); resp[total] = 0; long long status = fk_http_status(resp, total); long long bo = fk_http_body_offset(resp, total); if (bo > total) { bo = total; } return fk_http_dict_with_headers(status, fk_http_headers(resp, total, bo), fk_sbuf(resp + bo, total - bo), fk_sbuf("", 0), fk_elapsed_ms(started)); } static long long fk_http_get_native(long long urlv, long long headersv, long long timeoutv) { char url[2048]; fk_cstr(urlv, url, 2048); if (fk_starts(url, "https://")) { return fk_https_get_ssl(urlv, headersv, timeoutv); } return fk_http_get_plain(urlv, headersv, timeoutv); } static long long fk_tls_request(long long hostv, long long portv, long long reqv) { char host[512]; char port[16]; fk_cstr(hostv, host, 512); fk_cstr(portv, port, 16); long long rsa = reqv >> 1; long long rlen = (rsa >= 0 && rsa < fk_sp) ? fk_sl[rsa] : 0; void *lib = fk_ssl_lib(); if (lib == 0) { return fk_sbuf("", 0); } fk_tls_method_fn TLS_client_method = (fk_tls_method_fn)dlsym(lib, "TLS_client_method"); fk_ctx_new_fn SSL_CTX_new = (fk_ctx_new_fn)dlsym(lib, "SSL_CTX_new"); fk_ctx_free_fn SSL_CTX_free = (fk_ctx_free_fn)dlsym(lib, "SSL_CTX_free"); fk_ssl_new_fn SSL_new = (fk_ssl_new_fn)dlsym(lib, "SSL_new"); fk_ssl_free_fn SSL_free = (fk_ssl_free_fn)dlsym(lib, "SSL_free"); fk_ssl_set_fd_fn SSL_set_fd = (fk_ssl_set_fd_fn)dlsym(lib, "SSL_set_fd"); fk_ssl_ctrl_fn SSL_ctrl = (fk_ssl_ctrl_fn)dlsym(lib, "SSL_ctrl"); fk_ssl_set1_host_fn SSL_set1_host = (fk_ssl_set1_host_fn)dlsym(lib, "SSL_set1_host"); fk_ssl_connect_fn SSL_connect = (fk_ssl_connect_fn)dlsym(lib, "SSL_connect"); fk_ssl_write_fn SSL_write = (fk_ssl_write_fn)dlsym(lib, "SSL_write"); fk_ssl_read_fn SSL_read = (fk_ssl_read_fn)dlsym(lib, "SSL_read"); fk_ssl_verify_result_fn SSL_get_verify_result = (fk_ssl_verify_result_fn)dlsym(lib, "SSL_get_verify_result"); fk_ctx_set_verify_fn SSL_CTX_set_verify = (fk_ctx_set_verify_fn)dlsym(lib, "SSL_CTX_set_verify"); fk_ctx_default_paths_fn SSL_CTX_set_default_verify_paths = (fk_ctx_default_paths_fn)dlsym(lib, "SSL_CTX_set_default_verify_paths"); if (TLS_client_method == 0 || SSL_CTX_new == 0 || SSL_CTX_free == 0 || SSL_new == 0 || SSL_free == 0 || SSL_set_fd == 0 || SSL_ctrl == 0 || SSL_set1_host == 0 || SSL_connect == 0 || SSL_write == 0 || SSL_read == 0 || SSL_get_verify_result == 0 || SSL_CTX_set_verify == 0 || SSL_CTX_set_default_verify_paths == 0) { return fk_sbuf("", 0); } int fd = fk_tcp_connect(host, port); if (fd < 0) { return fk_sbuf("", 0); } void *ctx = SSL_CTX_new(TLS_client_method()); if (ctx == 0) { close(fd); return fk_sbuf("", 0); } SSL_CTX_set_verify(ctx, 1, 0); SSL_CTX_set_default_verify_paths(ctx); void *ssl = SSL_new(ctx); if (ssl == 0) { SSL_CTX_free(ctx); close(fd); return fk_sbuf("", 0); } SSL_ctrl(ssl, 55, 0, host); if (SSL_set1_host(ssl, host) != 1) { SSL_free(ssl); SSL_CTX_free(ctx); close(fd); return fk_sbuf("", 0); } if (SSL_set_fd(ssl, fd) != 1 || SSL_connect(ssl) != 1) { SSL_free(ssl); SSL_CTX_free(ctx); close(fd); return fk_sbuf("", 0); } if (SSL_get_verify_result(ssl) != 0) { SSL_free(ssl); SSL_CTX_free(ctx); close(fd); return fk_sbuf("", 0); } const char *rptr = fk_sb + fk_so[rsa]; long long wr = 0; while (wr < rlen) { int nwr = SSL_write(ssl, rptr + wr, (int)(rlen - wr)); if (nwr <= 0) { SSL_free(ssl); SSL_CTX_free(ctx); close(fd); return fk_sbuf("", 0); } wr = wr + nwr; } static char resp[65536]; long long total = 0; while (total < 65535) { int got = SSL_read(ssl, resp + total, (int)(65535 - total)); if (got <= 0) { break; } total = total + got; } SSL_free(ssl); SSL_CTX_free(ctx); close(fd); return fk_sbuf(resp, total); } static double fk_sqrt_d(double x) { if (x <= 0.0) { return 0.0; } double g = x >= 1.0 ? x : 1.0; long long i = 0; while (i < 32) { g = 0.5 * (g + x / g); i = i + 1; } return g; } static double fk_exp_d(double x) { double ln2 = 0.6931471805599453; long long n = (long long)(x / ln2); double r = x - ((double)n) * ln2; while (r > 0.34657359027997264) { r = r - ln2; n = n + 1; } while (r < -0.34657359027997264) { r = r + ln2; n = n - 1; } double term = 1.0; double sum = 1.0; long long k = 1; while (k <= 28) { term = term * r / (double)k; sum = sum + term; k = k + 1; } while (n > 0) { sum = sum * 2.0; n = n - 1; } while (n < 0) { sum = sum * 0.5; n = n + 1; } return sum; } static double fk_log_d(double x) { if (x <= 0.0) { return 0.0; } double ln2 = 0.6931471805599453; long long e = 0; while (x >= 2.0) { x = x * 0.5; e = e + 1; } while (x < 1.0) { x = x * 2.0; e = e - 1; } double z = (x - 1.0) / (x + 1.0); double z2 = z * z; double zp = z; double acc = 0.0; long long k = 0; while (k < 32) { acc = acc + zp / (double)(2 * k + 1); zp = zp * z2; k = k + 1; } return 2.0 * acc + ((double)e) * ln2; } static double fk_dot_list(long long av, long long bv) { long long pa = av >> 1; long long pb = bv >> 1; double acc = 0.0; while (pa >= 1 && pa <= fk_hp && pb >= 1 && pb <= fk_hp) { acc = acc + fk_num(fk_hh[pa]) * fk_num(fk_hh[pb]); pa = fk_ht[pa] >> 1; pb = fk_ht[pb] >> 1; } return acc; } static double fk_mag_list(long long av) { long long pa = av >> 1; double acc = 0.0; while (pa >= 1 && pa <= fk_hp) { double x = fk_num(fk_hh[pa]); acc = acc + x * x; pa = fk_ht[pa] >> 1; } return fk_sqrt_d(acc); } static void fk_arena(void) { fk_cap = 4096; fk_hh = malloc(fk_cap * 8); fk_ht = malloc(fk_cap * 8); fk_hh[0] = 1; fk_ht[0] = 1; } static long long *fk_fw; static long long *fk_nh; static long long *fk_nt; static long long fk_nhp; static void fk_mw(long long v) { char b[32]; long long n = 0; if (v == 0) { b[0] = 48; n = 1; } while (v > 0) { b[n] = 48 + v % 10; v = v / 10; n = n + 1; } while (n > 0) { n = n - 1; write(2, b + n, 1); } } static void fk_mc(long long c) { char b = c; write(2, &b, 1); } static long long fk_mlive(long long b) { if ((b & 1) == 0) { return 0; } long long p = b >> 1; if (p < 1 || p > fk_hp) { return 0; } if (fk_fw[p] != 0) { return 0; } fk_fw[p] = 0 - 1; return 1 + fk_mlive(fk_ht[p]) + fk_mlive(fk_hh[p]); } static long long fk_mcopy(long long b) { if ((b & 1) == 0) { return b; } long long p = b >> 1; if (p < 1 || p > fk_hp) { return b; } if (fk_fw[p] > 0) { return (fk_fw[p] << 1) | 1; } long long t2 = fk_mcopy(fk_ht[p]); long long h2 = fk_mcopy(fk_hh[p]); fk_nhp = fk_nhp + 1; fk_nh[fk_nhp] = h2; fk_nt[fk_nhp] = t2; fk_fw[p] = fk_nhp; return (fk_nhp << 1) | 1; } static void fk_melt(void) { fk_fw = calloc(fk_hp + 1, 8); if (fk_fw == 0) { return; } long long nlive = 0; long long k = 0; while (k < fk_vsp) { nlive = nlive + fk_mlive(fk_vs[k]); k = k + 1; } k = 0; while (k < 4096) { nlive = nlive + fk_mlive(fk_mem[k]); k = k + 1; } k = 1; while (k <= fk_np) { nlive = nlive + fk_mlive(fk_ncat[k]); nlive = nlive + fk_mlive(fk_nkids[k]); nlive = nlive + fk_mlive(fk_nval[k]); k = k + 1; } long long ncap = fk_cap; if (nlive * 2 > fk_cap) { ncap = fk_cap * 2; } fk_nh = malloc(ncap * 8); fk_nt = malloc(ncap * 8); if (fk_nh == 0 || fk_nt == 0) { free(fk_nh); free(fk_nt); free(fk_fw); return; } fk_nhp = 0; fk_nh[0] = 1; fk_nt[0] = 1; k = 0; while (k < fk_vsp) { fk_vs[k] = fk_mcopy(fk_vs[k]); k = k + 1; } k = 0; while (k < 4096) { fk_mem[k] = fk_mcopy(fk_mem[k]); k = k + 1; } k = 1; while (k <= fk_np) { fk_ncat[k] = fk_mcopy(fk_ncat[k]); fk_nkids[k] = fk_mcopy(fk_nkids[k]); fk_nval[k] = fk_mcopy(fk_nval[k]); k = k + 1; } free(fk_hh); free(fk_ht); free(fk_fw); fk_hh = fk_nh; fk_ht = fk_nt; fk_hp = fk_nhp; fk_cap = ncap; } extern void _exit(int); static void fk_vp(long long v) { if (fk_vsp >= 65536) { fk_mc(118); fk_mc(115); fk_mc(33); fk_mc(10); _exit(9); } fk_vs[fk_vsp] = v; fk_vsp = fk_vsp + 1; } static long long fk_fn_count; static long long fk_node_count; static long long fk_fn[2048]; static long long fk_node[65536][4]; static char fk_buf[1048576]; static long long fk_pos; extern int open(const char *, int, ...); extern long long read(int, void *, unsigned long); static long long fk_next() { long long sg = 1; while (fk_buf[fk_pos] != 0) { if (fk_buf[fk_pos] == 45 && fk_buf[fk_pos + 1] >= 48 && fk_buf[fk_pos + 1] <= 57) { sg = 0 - 1; fk_pos = fk_pos + 1; break; } if (fk_buf[fk_pos] >= 48) { if (fk_buf[fk_pos] <= 57) { break; } } fk_pos = fk_pos + 1; } long long v = 0; while (fk_buf[fk_pos] >= 48 && fk_buf[fk_pos] <= 57) { v = v * 10 + (fk_buf[fk_pos] - 48); fk_pos = fk_pos + 1; } return sg * v; } static long long fk_str_root_depth(long long i, long long d) { if (d > 64 || i < 0 || i >= fk_node_count) { return 0; } long long t = fk_node[i][0]; if (t == 24 || t == 27 || t == 29 || t == 32 || t == 33 || t == 62 || t == 63 || t == 125) { return 1; } if (t == 6) { if (fk_str_root_depth(fk_node[i][2], d + 1) && fk_str_root_depth(fk_node[i][3], d + 1)) { return 1; } return 0; } if (t == 12) { long long f = fk_node[i][1]; if (f >= 0 && f < fk_fn_count) { return fk_str_root_depth(fk_fn[f], d + 1); } return 0; } if (t == 69) { return fk_str_root_depth(fk_node[i][2], d + 1); } if (t == 109) { return fk_str_root_depth(fk_node[i][3], d + 1); } if (t == 111) { return fk_str_root_depth(fk_node[i][2], d + 1); } return 0; } static void fk_psv(long long v) { long long sa = v >> 1; if (sa >= 0 && sa < fk_sp) { long long j = 0; while (j < fk_sl[sa]) { putchar((int)(unsigned char)fk_sb[fk_so[sa] + j]); j = j + 1; } putchar(10); } else { fk_pv(v); } } static void fk_pv_root(long long root, long long v) { if (fk_str_root_depth(root, 0)) { fk_psv(v); } else { fk_pv(v); } } static long long fk_walk(long long i, long long fp) { long long t = fk_node[i][0]; fk_arms[t] = fk_arms[t] + 1; if (t == 1) { return fk_node[i][1] << 1; } if (t == 2) { return fk_vs[fp]; } if (t == 3) { long long a3 = fk_walk(fk_node[i][1], fp); long long b3 = fk_walk(fk_node[i][2], fp); if (fk_isf(a3) || fk_isf(b3)) { return fk_fbox(fk_num(a3) + fk_num(b3)); } return a3 + b3; } if (t == 4) { long long a4 = fk_walk(fk_node[i][1], fp); long long b4 = fk_walk(fk_node[i][2], fp); if (fk_isf(a4) || fk_isf(b4)) { return fk_fbox(fk_num(a4) - fk_num(b4)); } return a4 - b4; } if (t == 5) { long long a5 = fk_walk(fk_node[i][1], fp); long long b5 = fk_walk(fk_node[i][2], fp); if (fk_num(a5) <= fk_num(b5)) { return 2; } return 0; } if (t == 6) { if (fk_walk(fk_node[i][1], fp) == 0) { return fk_walk(fk_node[i][3], fp); } return fk_walk(fk_node[i][2], fp); } if (t == 7) { long long v7 = fk_walk(fk_node[i][1], fp); fk_vp(v7); long long r7 = fk_walk(fk_fn[0], fk_vsp - 1); fk_vsp = fk_vsp - 1; return r7; } if (t == 8) { return fk_node[fk_walk(fk_node[i][1], fp) >> 1][fk_walk(fk_node[i][2], fp) >> 1] << 1; } if (t == 9) { putchar((int)(fk_walk(fk_node[i][1], fp) >> 1)); return 0; } if (t == 10) { long long a10 = fk_walk(fk_node[i][1], fp); long long b10 = fk_walk(fk_node[i][2], fp); if (fk_isf(a10) || fk_isf(b10)) { return fk_fbox(fk_num(a10) / fk_num(b10)); } return ((a10 >> 1) / (b10 >> 1)) << 1; } if (t == 11) { long long a11 = fk_walk(fk_node[i][1], fp); long long b11 = fk_walk(fk_node[i][2], fp); if (fk_isf(a11) || fk_isf(b11)) { double x11 = fk_num(a11); double y11 = fk_num(b11); return fk_fbox(x11 - y11 * (double)((long long)(x11 / y11))); } return ((a11 >> 1) % (b11 >> 1)) << 1; } if (t == 12) { long long v12 = fk_walk(fk_node[i][2], fp); fk_vp(v12); long long r12 = fk_walk(fk_fn[fk_node[i][1]], fk_vsp - 1); fk_vsp = fk_vsp - 1; return r12; } if (t == 13) { long long mi = fk_walk(fk_node[i][1], fp) >> 1; long long mv = fk_walk(fk_node[i][2], fp); fk_mem[mi & 4095] = mv; return mv; } if (t == 14) { return fk_mem[(fk_walk(fk_node[i][1], fp) >> 1) & 4095]; } if (t == 15) { return time(0) << 1; } if (t == 16) { return ((long long)arc4random()) << 1; } if (t == 17) { return ((long long)fk_src[(fk_walk(fk_node[i][1], fp) >> 1) & 262143]) << 1; } if (t == 18) { return 1; } if (t == 19) { long long h19 = fk_walk(fk_node[i][1], fp); fk_vp(h19); long long t19 = fk_walk(fk_node[i][2], fp); fk_vp(t19); if (fk_cap == 0) { fk_arena(); } if (fk_hp * 100 >= fk_cap * 90) { fk_melt(); } if (fk_hp + 1 >= fk_cap) { fk_vsp = fk_vsp - 2; return 1; } fk_hp = fk_hp + 1; fk_hh[fk_hp] = fk_vs[fk_vsp - 2]; fk_ht[fk_hp] = fk_vs[fk_vsp - 1]; fk_vsp = fk_vsp - 2; return (fk_hp << 1) | 1; } if (t == 20) { long long p = fk_walk(fk_node[i][1], fp) >> 1; if (p < 1 || p > fk_hp) { return 1; } return fk_hh[p]; } if (t == 21) { long long p = fk_walk(fk_node[i][1], fp) >> 1; if (p < 1 || p > fk_hp) { return 1; } return fk_ht[p]; } if (t == 22) { long long p = fk_walk(fk_node[i][1], fp) >> 1; long long n = 0; while (p >= 1 && p <= fk_hp) { n = n + 1; p = fk_ht[p] >> 1; } return n << 1; } if (t == 23) { long long x23 = fk_walk(fk_node[i][1], fp); fk_vp(x23); long long k23 = fk_walk(fk_node[i][2], fp) >> 1; fk_vsp = fk_vsp - 1; long long p = fk_vs[fk_vsp] >> 1; while (p >= 1 && p <= fk_hp && k23 > 0) { p = fk_ht[p] >> 1; k23 = k23 - 1; } if (p < 1 || p > fk_hp) { return 1; } return fk_hh[p]; } if (t == 44) { long long fv44 = fk_walk(fk_node[i][1], fp); fk_vp(fv44); long long av44 = fk_walk(fk_node[i][2], fp); fk_vp(av44); long long p44 = fk_vs[fk_vsp - 2] >> 1; if (p44 < 1 || p44 > fk_hp) { fk_vsp = fk_vsp - 2; return 0; } long long f44 = fk_hh[p44] >> 1; long long p44t = fk_ht[p44] >> 1; if (p44t < 1 || p44t > fk_hp) { fk_vsp = fk_vsp - 2; return 0; } long long a44 = fk_hh[p44t] >> 1; long long caps44 = fk_ht[p44t]; long long args44 = fk_vs[fk_vsp - 1]; long long rev44 = 1; long long cc44 = caps44 >> 1; while (cc44 >= 1 && cc44 <= fk_hp) { fk_hp = fk_hp + 1; fk_hh[fk_hp] = fk_hh[cc44]; fk_ht[fk_hp] = rev44; rev44 = (fk_hp << 1) | 1; cc44 = fk_ht[cc44] >> 1; } long long comb44 = args44; long long rr44 = rev44 >> 1; while (rr44 >= 1 && rr44 <= fk_hp) { fk_hp = fk_hp + 1; fk_hh[fk_hp] = fk_hh[rr44]; fk_ht[fk_hp] = comb44; comb44 = (fk_hp << 1) | 1; rr44 = fk_ht[rr44] >> 1; } long long carg44 = 1; if (a44 == 0) { carg44 = 1; } else { if (a44 == 1) { long long pa44 = comb44 >> 1; if (pa44 < 1 || pa44 > fk_hp) { fk_vsp = fk_vsp - 2; return 1; } carg44 = fk_hh[pa44]; } else { carg44 = comb44; } } fk_vsp = fk_vsp - 2; fk_vp(carg44); long long r44 = fk_walk(fk_fn[f44], fk_vsp - 1); fk_vsp = fk_vsp - 1; return r44; } if (t == 24) { return fk_node[i][1] << 1; } if (t == 25) { long long sa = fk_walk(fk_node[i][1], fp) >> 1; if (sa < 0 || sa >= fk_sp) { return 0; } return fk_sl[sa] << 1; } if (t == 26) { long long sa26 = fk_walk(fk_node[i][1], fp) >> 1; long long sb26 = fk_walk(fk_node[i][2], fp) >> 1; if (fk_keyeq(sa26, sb26)) { return 2; } return 0; } if (t == 27) { long long sa = fk_walk(fk_node[i][1], fp) >> 1; long long sb = fk_walk(fk_node[i][2], fp) >> 1; if (sa < 0 || sa >= fk_sp || sb < 0 || sb >= fk_sp) { return 0 - 2; } long long ln = fk_sl[sa] + fk_sl[sb]; while (fk_sbp + ln > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } long long j = 0; while (j < fk_sl[sa]) { fk_sb[fk_sbp + j] = fk_sb[fk_so[sa] + j]; j = j + 1; } j = 0; while (j < fk_sl[sb]) { fk_sb[fk_sbp + fk_sl[sa] + j] = fk_sb[fk_so[sb] + j]; j = j + 1; } return fk_sintern(fk_sbp, ln) << 1; } if (t == 28) { long long sa = fk_walk(fk_node[i][1], fp) >> 1; long long k = fk_walk(fk_node[i][2], fp) >> 1; if (sa < 0 || sa >= fk_sp || k < 0 || k >= fk_sl[sa]) { return 0 - 2; } return ((long long)(unsigned char)fk_sb[fk_so[sa] + k]) << 1; } if (t == 29) { long long sa = fk_walk(fk_node[i][1], fp) >> 1; long long a = fk_walk(fk_node[i][2], fp) >> 1; long long b = fk_walk(fk_node[i][3], fp) >> 1; if (sa < 0 || sa >= fk_sp || a < 0 || b < a || b > fk_sl[sa]) { return 0 - 2; } long long ln = b - a; while (fk_sbp + ln > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } long long j2 = 0; while (j2 < ln) { fk_sb[fk_sbp + j2] = fk_sb[fk_so[sa] + a + j2]; j2 = j2 + 1; } return fk_sintern(fk_sbp, ln) << 1; } if (t == 30) { long long sa = fk_walk(fk_node[i][1], fp) >> 1; long long sb = fk_walk(fk_node[i][2], fp) >> 1; long long from = fk_walk(fk_node[i][3], fp) >> 1; if (sa < 0 || sa >= fk_sp || sb < 0 || sb >= fk_sp) { return 0 - 2; } if (from < 0) { from = 0; } if (from > fk_sl[sa]) { return 0 - 2; } long long ln = fk_sl[sb]; if (ln == 0) { return from << 1; } long long lim = fk_sl[sa] - ln; long long pos = from; while (pos <= lim) { long long j3 = 0; while (j3 < ln && fk_sb[fk_so[sa] + pos + j3] == fk_sb[fk_so[sb] + j3]) { j3 = j3 + 1; } if (j3 == ln) { return pos << 1; } pos = pos + 1; } return 0 - 2; } if (t == 31) { long long sa = fk_walk(fk_node[i][1], fp) >> 1; if (sa < 0 || sa >= fk_sp) { return 0; } long long off = fk_so[sa]; long long n = fk_sl[sa]; long long sign = 1; long long j4 = 0; while (j4 < n && (fk_sb[off + j4] == 32 || fk_sb[off + j4] == 9 || fk_sb[off + j4] == 10 || fk_sb[off + j4] == 13)) { j4 = j4 + 1; } if (j4 < n && fk_sb[off + j4] == 45) { sign = 0 - 1; j4 = j4 + 1; } long long v = 0; while (j4 < n) { char c = fk_sb[off + j4]; if (c < 48 || c > 57) { break; } v = v * 10 + (c - 48); j4 = j4 + 1; } return (sign * v) << 1; } if (t == 32) { char tmp[64]; long long vr = fk_walk(fk_node[i][1], fp); long long n = 0; if (vr == (0 - 9223372036854775807LL)) { tmp[0] = 116; tmp[1] = 114; tmp[2] = 117; tmp[3] = 101; n = 4; } else if (vr == (0 - 9223372036854775805LL)) { tmp[0] = 102; tmp[1] = 97; tmp[2] = 108; tmp[3] = 115; tmp[4] = 101; n = 5; } else if (fk_isf(vr)) { n = sprintf(tmp, "%.15g", fk_num(vr)); } else { long long v = vr >> 1; long long u = v; if (v == 0) { tmp[n] = 48; n = 1; } else { if (v < 0) { tmp[n] = 45; n = n + 1; u = 0 - v; } char digs[32]; long long dn = 0; while (u > 0) { digs[dn] = 48 + (u % 10); u = u / 10; dn = dn + 1; } while (dn > 0) { dn = dn - 1; tmp[n] = digs[dn]; n = n + 1; } } } while (fk_sbp + n > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } long long j5 = 0; while (j5 < n) { fk_sb[fk_sbp + j5] = tmp[j5]; j5 = j5 + 1; } return fk_sintern(fk_sbp, n) << 1; } if (t == 33) { long long b = fk_walk(fk_node[i][1], fp) >> 1; if (b < 0 || b > 255) { return fk_sintern(fk_sbp, 0) << 1; } while (fk_sbp + 1 > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } fk_sb[fk_sbp] = (char)b; return fk_sintern(fk_sbp, 1) << 1; } if (t == 34) { return (((fk_walk(fk_node[i][1], fp) >> 1) & (fk_walk(fk_node[i][2], fp) >> 1)) << 1); } if (t == 35) { return (((fk_walk(fk_node[i][1], fp) >> 1) | (fk_walk(fk_node[i][2], fp) >> 1)) << 1); } if (t == 36) { return (((fk_walk(fk_node[i][1], fp) >> 1) ^ (fk_walk(fk_node[i][2], fp) >> 1)) << 1); } if (t == 37) { unsigned int x = (unsigned int)(fk_walk(fk_node[i][1], fp) >> 1); long long n = (fk_walk(fk_node[i][2], fp) >> 1) & 31; return ((long long)(unsigned int)(x << n)) << 1; } if (t == 38) { unsigned int x = (unsigned int)(fk_walk(fk_node[i][1], fp) >> 1); long long n = (fk_walk(fk_node[i][2], fp) >> 1) & 31; return ((long long)(x >> n)) << 1; } if (t == 39) { unsigned long long x = (unsigned int)(fk_walk(fk_node[i][1], fp) >> 1); long long n = (fk_walk(fk_node[i][2], fp) >> 1) & 31; return ((long long)(unsigned int)((x >> n) | (x << (32 - n)))) << 1; } if (t == 40) { unsigned int x = (unsigned int)(fk_walk(fk_node[i][1], fp) >> 1); unsigned int y = (unsigned int)(fk_walk(fk_node[i][2], fp) >> 1); return ((long long)(unsigned int)(x + y)) << 1; } if (t == 41) { unsigned int x = (unsigned int)(fk_walk(fk_node[i][1], fp) >> 1); return ((long long)(unsigned int)(~x)) << 1; } if (t == 42) { long long a42 = fk_walk(fk_node[i][1], fp); long long b42 = fk_walk(fk_node[i][2], fp); if (fk_isf(a42) || fk_isf(b42)) { return fk_fbox(fk_num(a42) * fk_num(b42)); } return ((a42 >> 1) * (b42 >> 1)) << 1; } if (t == 43) { long long iv43 = fk_walk(fk_node[i][1], fp); long long ix43 = 1; while (ix43 <= fk_np) { if (fk_nkind[ix43] == 1 && fk_nid[ix43][2] == 1 && fk_nval[ix43] == iv43) { return fk_nbox(ix43); } ix43 = ix43 + 1; } if (fk_np + 1 >= 65536) { return 0; } fk_np = fk_np + 1; fk_nkind[fk_np] = 1; fk_nval[fk_np] = iv43; fk_nkids[fk_np] = 1; fk_ncat[fk_np] = 0; fk_nid[fk_np][0] = 1; fk_nid[fk_np][1] = 1; fk_nid[fk_np][2] = 1; fk_nid[fk_np][3] = iv43 >> 1; return fk_nbox(fk_np); } if (t == 45) { return fk_walk(fk_node[i][1], fp); } if (t == 46) { long long sv46 = fk_walk(fk_node[i][1], fp); long long sa46 = sv46 >> 1; long long ix46 = 1; while (ix46 <= fk_np) { if (fk_nkind[ix46] == 1 && fk_nid[ix46][2] == 2 && fk_nval[ix46] == sv46) { return fk_nbox(ix46); } ix46 = ix46 + 1; } if (sa46 < 0 || sa46 >= fk_sp || fk_np + 1 >= 65536) { return 0; } fk_np = fk_np + 1; fk_nkind[fk_np] = 1; fk_nval[fk_np] = sv46; fk_nkids[fk_np] = 1; fk_ncat[fk_np] = 0; fk_nid[fk_np][0] = 1; fk_nid[fk_np][1] = 1; fk_nid[fk_np][2] = 2; fk_nid[fk_np][3] = sa46; return fk_nbox(fk_np); } if (t == 47) { long long cat47 = fk_walk(fk_node[i][1], fp); long long kids47 = fk_walk(fk_node[i][2], fp); long long ix47 = 1; while (ix47 <= fk_np) { if (fk_nkind[ix47] == 2 && fk_veq(fk_ncat[ix47], cat47) != 0 && fk_veq(fk_nkids[ix47], kids47) != 0) { return fk_nbox(ix47); } ix47 = ix47 + 1; } if (fk_np + 1 >= 65536) { return 0; } fk_np = fk_np + 1; fk_nkind[fk_np] = 2; fk_ncat[fk_np] = cat47; fk_nkids[fk_np] = kids47; fk_nval[fk_np] = 0; fk_nid[fk_np][0] = 0; fk_nid[fk_np][1] = 0; fk_nid[fk_np][2] = 0; fk_nid[fk_np][3] = fk_np; if (cat47 < 0) { long long ci47 = fk_nidx(cat47); if (ci47 >= 1 && ci47 <= fk_np) { fk_nid[fk_np][1] = fk_nid[ci47][1]; fk_nid[fk_np][2] = fk_nid[ci47][2]; } } return fk_nbox(fk_np); } if (t == 48) { long long nv48 = fk_walk(fk_node[i][1], fp); if (nv48 >= 0) { return 1; } long long ni48 = fk_nidx(nv48); if (ni48 < 1 || ni48 > fk_np) { return 1; } if (fk_nkind[ni48] == 2) { return fk_nkids[ni48]; } return 1; } if (t == 49) { long long nv49 = fk_walk(fk_node[i][1], fp); if (nv49 >= 0) { return 0; } long long ni49 = fk_nidx(nv49); if (ni49 < 1 || ni49 > fk_np) { return 0; } if (fk_nkind[ni49] == 1) { return fk_nval[ni49]; } return 0; } if (t == 80) { if (fk_veq(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp)) != 0) { return 2; } return 0; } if (t == 91) { long long xs91 = fk_walk(fk_node[i][1], fp); long long q91 = xs91 >> 1; long long p91 = 0; long long l91 = 0; long long ty91 = 0; long long in91 = 0; if (q91 >= 1 && q91 <= fk_hp) { p91 = fk_hh[q91] >> 1; q91 = fk_ht[q91] >> 1; } if (q91 >= 1 && q91 <= fk_hp) { l91 = fk_hh[q91] >> 1; q91 = fk_ht[q91] >> 1; } if (q91 >= 1 && q91 <= fk_hp) { ty91 = fk_hh[q91] >> 1; q91 = fk_ht[q91] >> 1; } if (q91 >= 1 && q91 <= fk_hp) { in91 = fk_hh[q91] >> 1; } if (fk_np + 1 >= 65536) { return 0; } fk_np = fk_np + 1; fk_nkind[fk_np] = 3; fk_ncat[fk_np] = 0; fk_nkids[fk_np] = 1; fk_nval[fk_np] = 0; fk_nid[fk_np][0] = p91; fk_nid[fk_np][1] = l91; fk_nid[fk_np][2] = ty91; fk_nid[fk_np][3] = in91; return fk_nbox(fk_np); } if (t == 92) { long long nv92 = fk_walk(fk_node[i][1], fp); if (nv92 >= 0) { return nv92; } long long ni92 = fk_nidx(nv92); if (ni92 < 1 || ni92 > fk_np) { return nv92; } if (fk_nkind[ni92] == 2) { return fk_ncat[ni92]; } return nv92; } if (t == 93) { long long nv93 = fk_walk(fk_node[i][1], fp); if (nv93 >= 0) { return 0; } long long ni93 = fk_nidx(nv93); if (ni93 < 1 || ni93 > fk_np) { return 0; } return fk_nid[ni93][2] << 1; } if (t == 94) { long long nv94 = fk_walk(fk_node[i][1], fp); if (nv94 >= 0) { return 0; } long long ni94 = fk_nidx(nv94); if (ni94 < 1 || ni94 > fk_np) { return 0; } return fk_nid[ni94][3] << 1; } if (t == 95) { long long nv95 = fk_walk(fk_node[i][1], fp); if (nv95 >= 0) { return 0; } long long ni95 = fk_nidx(nv95); if (ni95 < 1 || ni95 > fk_np) { return 0; } return fk_nid[ni95][0] << 1; } if (t == 96) { long long nv96 = fk_walk(fk_node[i][1], fp); if (nv96 >= 0) { return 0; } long long ni96 = fk_nidx(nv96); if (ni96 < 1 || ni96 > fk_np) { return 0; } return fk_nid[ni96][1] << 1; } if (t == 112) { long long bv112 = fk_walk(fk_node[i][1], fp); long long se112 = (bv112 != 0) ? (0 - 9223372036854775807LL) : (0 - 9223372036854775805LL); long long ix112 = 1; while (ix112 <= fk_np) { if (fk_nkind[ix112] == 1 && fk_nid[ix112][2] == 3 && fk_nval[ix112] == se112) { return fk_nbox(ix112); } ix112 = ix112 + 1; } if (fk_np + 1 >= 65536) { return 0; } fk_np = fk_np + 1; fk_nkind[fk_np] = 1; fk_nval[fk_np] = se112; fk_nkids[fk_np] = 1; fk_ncat[fk_np] = 0; fk_nid[fk_np][0] = 1; fk_nid[fk_np][1] = 1; fk_nid[fk_np][2] = 3; fk_nid[fk_np][3] = (bv112 != 0) ? 1 : 0; return fk_nbox(fk_np); } if (t == 113) { long long sa113 = fk_walk(fk_node[i][1], fp) >> 1; double fd113 = 0.0; if (sa113 >= 0 && sa113 < fk_sp) { char tb113[128]; long long n113 = fk_sl[sa113]; if (n113 > 126) { n113 = 126; } long long jj113 = 0; while (jj113 < n113) { tb113[jj113] = fk_sb[fk_so[sa113] + jj113]; jj113 = jj113 + 1; } tb113[n113] = 0; fd113 = strtod(tb113, 0); } long long fb113 = fk_fbox(fd113); if (fk_np + 1 >= 65536) { return 0; } fk_np = fk_np + 1; fk_nkind[fk_np] = 1; fk_nval[fk_np] = fb113; fk_nkids[fk_np] = 1; fk_ncat[fk_np] = 0; fk_nid[fk_np][0] = 1; fk_nid[fk_np][1] = 1; fk_nid[fk_np][2] = 7; fk_nid[fk_np][3] = 0; return fk_nbox(fk_np); } if (t == 50) { long long sa = fk_node[i][1]; if (sa < 0 || sa >= fk_sp) { return 0; } char tmp[128]; long long n = fk_sl[sa]; if (n > 126) { n = 126; } long long j = 0; while (j < n) { tmp[j] = fk_sb[fk_so[sa] + j]; j = j + 1; } tmp[n] = 0; return fk_fbox(strtod(tmp, 0)); } if (t == 51) { double d = fk_num(fk_walk(fk_node[i][1], fp)); long long q = (long long)d; if (d < (double)q) { q = q - 1; } return q << 1; } if (t == 52) { double x = fk_num(fk_walk(fk_node[i][1], fp)); long long nd = fk_walk(fk_node[i][2], fp) >> 1; double sc = 1.0; while (nd > 0) { sc = sc * 10.0; nd = nd - 1; } double y = x * sc; double ay = y < 0.0 ? 0.0 - y : y; long long base = (long long)ay; double frac = ay - (double)base; long long qa = base; if (frac > 0.5) { qa = base + 1; } else if (frac == 0.5 && (base & 1)) { qa = base + 1; } long long q = y < 0.0 ? 0 - qa : qa; return fk_fbox(((double)q) / sc); } if (t == 53) { long long sa = fk_walk(fk_node[i][1], fp) >> 1; if (sa < 0 || sa >= fk_sp) { return fk_fbox(0.0); } char tmp[128]; long long n = fk_sl[sa]; if (n > 126) { n = 126; } long long j = 0; while (j < n) { tmp[j] = fk_sb[fk_so[sa] + j]; j = j + 1; } tmp[n] = 0; return fk_fbox(strtod(tmp, 0)); } if (t == 54) { return ((long long)fk_num(fk_walk(fk_node[i][1], fp))) << 1; } if (t == 81) { return fk_fbox(fk_sqrt_d(fk_num(fk_walk(fk_node[i][1], fp)))); } if (t == 82) { return ((long long)fk_num(fk_walk(fk_node[i][1], fp))) << 1; } if (t == 83) { fk_walk(fk_node[i][1], fp); return 0; } if (t == 84) { long long a84 = fk_walk(fk_node[i][1], fp); fk_vp(a84); long long b84 = fk_walk(fk_node[i][2], fp); fk_vsp = fk_vsp - 1; return fk_fbox(fk_dot_list(fk_vs[fk_vsp], b84)); } if (t == 85) { return fk_fbox(fk_mag_list(fk_walk(fk_node[i][1], fp))); } if (t == 86) { long long a86 = fk_walk(fk_node[i][1], fp); fk_vp(a86); long long b86 = fk_walk(fk_node[i][2], fp); fk_vsp = fk_vsp - 1; double ma86 = fk_mag_list(fk_vs[fk_vsp]); double mb86 = fk_mag_list(b86); if (ma86 == 0.0 || mb86 == 0.0) { return fk_fbox(0.0); } return fk_fbox(fk_dot_list(fk_vs[fk_vsp], b86) / (ma86 * mb86)); } if (t == 87) { double d87 = fk_num(fk_walk(fk_node[i][1], fp)); double a87 = d87 < 0.0 ? 0.0 - d87 : d87; long long q87 = (long long)(a87 + 0.5); if (d87 < 0.0) { q87 = 0 - q87; } return q87 << 1; } if (t == 88) { double d88 = fk_num(fk_walk(fk_node[i][1], fp)); long long q88 = (long long)d88; if (d88 > (double)q88) { q88 = q88 + 1; } return q88 << 1; } if (t == 89) { return fk_fbox(fk_exp_d(fk_num(fk_walk(fk_node[i][1], fp)))); } if (t == 90) { return fk_fbox(fk_log_d(fk_num(fk_walk(fk_node[i][1], fp)))); } if (t == 55) { static char p[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096); fk_unlink_segments(p); return rmdir(p) << 1; } if (t == 56) { static char p[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096); int rc = mkdir(p, 0777); return (rc < 0 ? 0 : 1) << 1; } if (t == 57) { static char p[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096); int fd = open(p, 0); if (fd < 0) { return 0; } close(fd); return 2; } if (t == 58) { static char p[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096); return unlink(p) << 1; } if (t == 59) { static char a[4096]; static char b[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), a, 4096); fk_cstr(fk_walk(fk_node[i][2], fp), b, 4096); return rename(a, b) << 1; } if (t == 60) { static char p[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096); int fd = open(p, 0); if (fd < 0) { return -2; } long n = lseek(fd, 0, 2); close(fd); return ((long long)n) << 1; } if (t == 61) { static char p[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096); long long xs = fk_walk(fk_node[i][2], fp); int fd = open(p, O_WRONLY | O_CREAT | O_APPEND, 0666); if (fd < 0) { return -2; } static char tmp[8192]; long long n = 0; long long q = xs >> 1; while (q >= 1 && q <= fk_hp && n < 8192) { tmp[n] = (char)(fk_hh[q] >> 1); n = n + 1; q = fk_ht[q] >> 1; } long long wr = write(fd, tmp, n); long long total = lseek(fd, 0, 2); close(fd); if (wr < 0 || total < 0) { return -2; } return total << 1; } if (t == 62) { static char p[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096); long long off = fk_walk(fk_node[i][2], fp) >> 1; long long len = fk_walk(fk_node[i][3], fp) >> 1; if (len <= 0) { return fk_sbuf("", 0); } int fd = open(p, 0); if (fd < 0) { return fk_sbuf("", 0); } lseek(fd, off, 0); fk_sinit(); while (fk_sbp + len > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } long long got = read(fd, fk_sb + fk_sbp, len); close(fd); if (got < 0) { got = 0; } return fk_sintern(fk_sbp, got) << 1; } if (t == 63) { static char p[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096); int fd = open(p, 0); if (fd < 0) { return fk_sbuf("", 0); } fk_sinit(); long long base = fk_sbp; long long total = 0; for (;;) { while (base + total + 65536 > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } long long got = read(fd, fk_sb + base + total, 65536); if (got <= 0) { break; } total = total + got; } close(fd); return fk_sintern(base, total) << 1; } if (t == 64) { long long xs64 = fk_walk(fk_node[i][1], fp); fk_rp = fk_rp + 1; if (fk_rp >= 256) { fk_rp = 255; } fk_rcnt[fk_rp] = 0; fk_rbp[fk_rp] = 0; long long q64 = xs64 >> 1; while (q64 >= 1 && q64 <= fk_hp) { long long e64 = fk_hh[q64]; long long ep64 = e64 >> 1; if (ep64 >= 1 && ep64 <= fk_hp) { long long k64 = fk_hh[ep64] >> 1; long long tp64 = fk_ht[ep64] >> 1; long long v64 = 0; if (tp64 >= 1 && tp64 <= fk_hp) { v64 = fk_hh[tp64]; } if (k64 == -1) { fk_rbp[fk_rp] = v64; } else if (fk_rcnt[fk_rp] < 128) { fk_rkey[fk_rp][fk_rcnt[fk_rp]] = k64; fk_rval[fk_rp][fk_rcnt[fk_rp]] = v64; fk_rcnt[fk_rp] = fk_rcnt[fk_rp] + 1; } } q64 = fk_ht[q64] >> 1; } return fk_rbox(fk_rp); } if (t == 65) { long long r = fk_ridx(fk_walk(fk_node[i][1], fp)); long long key = fk_walk(fk_node[i][2], fp) >> 1; if (r < 1 || r >= 256) { return 0; } long long j = 0; while (j < fk_rcnt[r]) { if (fk_keyeq(fk_rkey[r][j], key)) { return fk_rval[r][j]; } j = j + 1; } return 0; } if (t == 66) { long long rec = fk_walk(fk_node[i][1], fp); long long r = fk_ridx(rec); long long key = fk_walk(fk_node[i][2], fp) >> 1; long long val = fk_walk(fk_node[i][3], fp); if (r < 1 || r >= 256) { return 0; } long long j = 0; while (j < fk_rcnt[r]) { if (fk_keyeq(fk_rkey[r][j], key)) { fk_rval[r][j] = val; return rec; } j = j + 1; } if (fk_rcnt[r] < 128) { fk_rkey[r][fk_rcnt[r]] = key; fk_rval[r][fk_rcnt[r]] = val; fk_rcnt[r] = fk_rcnt[r] + 1; } return rec; } if (t == 67) { long long r = fk_ridx(fk_walk(fk_node[i][1], fp)); long long key = fk_walk(fk_node[i][2], fp) >> 1; if (r < 1 || r >= 256) { return 0; } long long j = 0; while (j < fk_rcnt[r]) { if (fk_keyeq(fk_rkey[r][j], key)) { return 2; } j = j + 1; } return 0; } if (t == 68) { if (fk_isrec(fk_walk(fk_node[i][1], fp))) { return 2; } return 0; } if (t == 99) { long long r = fk_ridx(fk_walk(fk_node[i][1], fp)); long long out = 1; if (r < 1 || r >= 256) { return out; } long long j = fk_rcnt[r]; while (j > 0) { j = j - 1; if (fk_hp + 1 >= fk_cap) { return out; } fk_hp = fk_hp + 1; fk_hh[fk_hp] = fk_rkey[r][j] << 1; fk_ht[fk_hp] = out; out = (fk_hp << 1) | 1; } return out; } if (t == 101) { return fk_tempdir(); } if (t == 104) { static char p[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p, 4096); long long sv104 = fk_walk(fk_node[i][2], fp); long long sa104 = sv104 >> 1; if (sa104 < 0 || sa104 >= fk_sp) { return -2; } long long n104 = fk_sl[sa104]; long long base104 = fk_so[sa104]; int fd104 = open(p, O_WRONLY | O_CREAT | O_TRUNC, 0666); if (fd104 < 0) { return -2; } long long wr104 = 0; while (wr104 < n104) { long long w104 = write(fd104, fk_sb + base104 + wr104, n104 - wr104); if (w104 <= 0) { break; } wr104 = wr104 + w104; } close(fd104); if (wr104 < 0) { return -2; } return wr104 << 1; } if (t == 105) { return fk_http_get_native(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp), fk_walk(fk_node[i][3], fp)); } if (t == 118) { return fk_sock_request(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp), fk_walk(fk_node[i][3], fp)); } if (t == 119) { return fk_tls_request(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp), fk_walk(fk_node[i][3], fp)); } if (t == 136) { return fk_host_exec(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp)); } if (t == 106) { long long base106 = fk_walk(fk_node[i][1], fp); fk_vp(base106); long long key106 = fk_walk(fk_node[i][2], fp); fk_vsp = fk_vsp - 1; return fk_get_value(fk_vs[fk_vsp], key106); } if (t == 107) { return fk_file_mtime(fk_walk(fk_node[i][1], fp)); } if (t == 108) { long long s108 = fk_walk(fk_node[i][1], fp); fk_vp(s108); long long f108 = fk_walk(fk_node[i][2], fp); fk_vp(f108); long long c108 = fk_walk(fk_node[i][3], fp); fk_vsp = fk_vsp - 2; return fk_scan_run(fk_vs[fk_vsp], fk_vs[fk_vsp + 1], c108); } if (t == 132) {
#ifdef FK_HAVE_DIRENT_HEADER
 if (fk_cap == 0) { fk_arena(); } static char fkl_p[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), fkl_p, 4096); void *fkl_d = opendir(fkl_p); if (fkl_d == 0) { return 1; } static char fkl_nb[1048576]; static long long fkl_no[16384]; static long long fkl_nl[16384]; long long fkl_nc = 0; long long fkl_bp = 0; while (1) { struct dirent *fkl_de = readdir(fkl_d); if (fkl_de == 0) { break; } char *fkl_nm = fkl_de->d_name; if (fkl_nm[0] == 46 && fkl_nm[1] == 0) { continue; } if (fkl_nm[0] == 46 && fkl_nm[1] == 46 && fkl_nm[2] == 0) { continue; } long long fkl_L = 0; while (fkl_nm[fkl_L] != 0) { fkl_L = fkl_L + 1; } if (fkl_nc >= 16384) { break; } if (fkl_bp + fkl_L + 1 > 1048576) { break; } fkl_no[fkl_nc] = fkl_bp; fkl_nl[fkl_nc] = fkl_L; long long fkl_j = 0; while (fkl_j < fkl_L) { fkl_nb[fkl_bp] = fkl_nm[fkl_j]; fkl_bp = fkl_bp + 1; fkl_j = fkl_j + 1; } fkl_nb[fkl_bp] = 0; fkl_bp = fkl_bp + 1; fkl_nc = fkl_nc + 1; } closedir(fkl_d); static long long fkl_ix[16384]; long long fkl_k = 0; while (fkl_k < fkl_nc) { fkl_ix[fkl_k] = fkl_k; fkl_k = fkl_k + 1; } long long fkl_a = 1; while (fkl_a < fkl_nc) { long long fkl_key = fkl_ix[fkl_a]; long long fkl_b = fkl_a - 1; while (fkl_b >= 0) { char *fkl_x = fkl_nb + fkl_no[fkl_ix[fkl_b]]; char *fkl_y = fkl_nb + fkl_no[fkl_key]; long long fkl_c = 0; while (fkl_x[fkl_c] != 0 && fkl_x[fkl_c] == fkl_y[fkl_c]) { fkl_c = fkl_c + 1; } if (((unsigned char)fkl_x[fkl_c]) <= ((unsigned char)fkl_y[fkl_c])) { break; } fkl_ix[fkl_b + 1] = fkl_ix[fkl_b]; fkl_b = fkl_b - 1; } fkl_ix[fkl_b + 1] = fkl_key; fkl_a = fkl_a + 1; } long long fkl_out = 1; long long fkl_m = fkl_nc; while (fkl_m > 0) { fkl_m = fkl_m - 1; long long fkl_si = fkl_ix[fkl_m]; long long fkl_sv = fk_sbuf(fkl_nb + fkl_no[fkl_si], fkl_nl[fkl_si]); if (fk_hp + 1 >= fk_cap) { return fkl_out; } fk_hp = fk_hp + 1; fk_hh[fk_hp] = fkl_sv; fk_ht[fk_hp] = fkl_out; fkl_out = (fk_hp << 1) | 1; } return fkl_out;
#else
 return 1;
#endif
    } if (t == 133) { static char p70[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p70, 4096); int fd70 = open(p70, 0); return ((long long)fd70) << 1; } if (t == 134) { long long fd71 = fk_walk(fk_node[i][1], fp) >> 1; long long max71 = fk_walk(fk_node[i][2], fp) >> 1; if (fd71 < 0 || max71 <= 0) { return fk_sbuf("", 0); } fk_sinit(); while (fk_sbp + max71 > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } long long got71 = read((int)fd71, fk_sb + fk_sbp, max71); if (got71 <= 0) { return fk_sbuf("", 0); } return fk_sintern(fk_sbp, got71) << 1; } if (t == 135) { long long fd72 = fk_walk(fk_node[i][1], fp) >> 1; if (fd72 < 0) { return -2; } return ((long long)close((int)fd72)) << 1; } if (t == 203) { return fk_metal_matvec_fixture_native(); } if (t == 204) { long long m204 = fk_walk(fk_node[i][1], fp); fk_vp(m204); long long k204 = fk_walk(fk_node[i][2], fp); fk_vp(k204); long long b204 = fk_walk(fk_node[i][3], fp); fk_vsp = fk_vsp - 2; return fk_metal_matvec_f32_native(fk_vs[fk_vsp], fk_vs[fk_vsp + 1], b204); } if (t == 205) { return fk_mic_count() << 1; } if (t == 206) { return fk_cam_count() << 1; } if (t == 207) { return fk_mic_name(fk_walk(fk_node[i][1], fp) >> 1); } if (t == 208) { return fk_cam_name(fk_walk(fk_node[i][1], fp) >> 1); } if (t == 209) { return fk_mic_health(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 210) { return fk_cam_health(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 211) { return fk_sense_report() << 1; } if (t == 212) { return fk_cam_grab(fk_walk(fk_node[i][1], fp) >> 1, "fkwu-cam-frame.bmp") << 1; } if (t == 213) { return fk_frame_read("fkwu-cam-frame.bmp") << 1; } if (t == 214) { return fk_sense_stream(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 215) { return fk_native_call_test(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 216) { return fk_wifi_ssid(); } if (t == 217) { return fk_wifi_signal() << 1; } if (t == 218) { return fk_bt_present() << 1; } if (t == 219) { return fk_bt_count() << 1; } if (t == 220) { return fk_power() << 1; } if (t == 221) { return fk_memload() << 1; } if (t == 222) { return fk_sensors_report() << 1; } if (t == 223) { return fk_sense_publish(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 224) { return fk_mesh_serve(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 225) { return fk_mesh_announce(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 226) { return fk_mesh_discover(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 227) { return fk_api_health() << 1; } if (t == 228) { return fk_mesh_register() << 1; } if (t == 229) { return fk_mesh_detect() << 1; } if (t == 230) { return fk_mesh_registry(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 231) { return fk_mesh_roster() << 1; }
#ifndef _WIN32
 if (t == 200) { static char p200[4096]; fk_cstr(fk_walk(fk_node[i][1], fp), p200, 4096); return fk_path_is_dir(p200) ? 2 : 0; } if (t == 202) { static char r202[4096]; static char s202[256]; fk_cstr(fk_walk(fk_node[i][1], fp), r202, 4096); fk_cstr(fk_walk(fk_node[i][2], fp), s202, 256); fk_inv_reset(); fk_inv_walk(r202, r202, s202, fk_walk(fk_node[i][3], fp)); return fk_inv_rows; }
#else
 if (t == 200) { return 0; } if (t == 202) { return 1; }
#endif
 if (t == 120) { return fk_socket_listen_native(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 121) { return fk_socket_port_native(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 122) { return fk_socket_accept_native(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 123) { long long host123 = fk_walk(fk_node[i][1], fp); fk_vp(host123); long long port123 = fk_walk(fk_node[i][2], fp) >> 1; fk_vsp = fk_vsp - 1; return fk_socket_connect_native(fk_vs[fk_vsp], port123) << 1; } if (t == 124) { long long h124 = fk_walk(fk_node[i][1], fp) >> 1; long long bytes124 = fk_walk(fk_node[i][2], fp); return fk_socket_send_native(h124, bytes124) << 1; } if (t == 125) { long long h125 = fk_walk(fk_node[i][1], fp) >> 1; long long max125 = fk_walk(fk_node[i][2], fp) >> 1; return fk_socket_recv_native(h125, max125); } if (t == 126) { return fk_socket_close_native(fk_walk(fk_node[i][1], fp) >> 1) << 1; } if (t == 97) { long long r = fk_ridx(fk_walk(fk_node[i][1], fp)); long long key = fk_walk(fk_node[i][2], fp) >> 1; if (r < 1 || r >= 256) { return 0; } long long j = 0; while (j < fk_rcnt[r]) { if (fk_keyeq(fk_rkey[r][j], key)) { return fk_rval[r][j]; } j = j + 1; } return 0; } if (t == 98) { long long rec = fk_walk(fk_node[i][1], fp); long long r = fk_ridx(rec); long long key = fk_walk(fk_node[i][2], fp) >> 1; long long val = fk_walk(fk_node[i][3], fp); if (r < 1 || r >= 256) { return 0; } long long j = 0; while (j < fk_rcnt[r]) { if (fk_keyeq(fk_rkey[r][j], key)) { fk_rval[r][j] = val; return 0; } j = j + 1; } if (fk_rcnt[r] < 128) { fk_rkey[r][fk_rcnt[r]] = key; fk_rval[r][fk_rcnt[r]] = val; fk_rcnt[r] = fk_rcnt[r] + 1; } return 0; } if (t == 100) { long long r = fk_ridx(fk_walk(fk_node[i][1], fp)); if (r < 1 || r >= 256) { return 0; } return fk_rbp[r]; } if (t == 70) { long long a70 = fk_walk(fk_node[i][1], fp); long long b70 = fk_walk(fk_node[i][2], fp); if (a70 != 0 && b70 != 0) { return 2; } return 0; } if (t == 71) { long long a71 = fk_walk(fk_node[i][1], fp); long long b71 = fk_walk(fk_node[i][2], fp); if (a71 != 0 || b71 != 0) { return 2; } return 0; } if (t == 72) { if (fk_walk(fk_node[i][1], fp) == 0) { return 2; } return 0; } if (t == 73) { return fk_node[i][1] << 1; } if (t == 74) { return fk_walk(fk_node[i][1], fp); } if (t == 75) { return fk_walk(fk_node[i][1], fp); } if (t == 76) { return fk_walk(fk_node[i][1], fp); } if (t == 77) { return fk_walk(fk_node[i][2], fp); } if (t == 78) { return fk_walk(fk_node[i][1], fp); } if (t == 79) { if (fk_walk(fk_node[i][1], fp) == 0) { return fk_walk(fk_node[i][3], fp); } return fk_walk(fk_node[i][2], fp); } if (t == 127) { long long ks_k = fk_walk(fk_node[i][1], fp) >> 1; long long ks_n = 256; if (ks_k == 0) { long long ks_s = 0; long long ks_u = 1; while (ks_u < ks_n) { ks_s = ks_s + fk_arms[ks_u]; ks_u = ks_u + 1; } return ks_s << 1; } if (ks_k == 1) { long long ks_d = 0; long long ks_u = 1; while (ks_u < ks_n) { if (fk_arms[ks_u] > 0) { ks_d = ks_d + 1; } ks_u = ks_u + 1; } return ks_d << 1; } if (ks_k == 2) { long long ks_bt = 0; long long ks_bc = 0; long long ks_u = 1; while (ks_u < ks_n) { if (fk_arms[ks_u] > ks_bc) { ks_bc = fk_arms[ks_u]; ks_bt = ks_u; } ks_u = ks_u + 1; } return ks_bt << 1; } if (ks_k == 3) { long long ks_bc = 0; long long ks_u = 1; while (ks_u < ks_n) { if (fk_arms[ks_u] > ks_bc) { ks_bc = fk_arms[ks_u]; } ks_u = ks_u + 1; } return ks_bc << 1; } if (ks_k == 4) { return fk_np << 1; } if (ks_k == 5) { return fk_sp << 1; } if (ks_k == 6) { return fk_hp << 1; } if (ks_k == 7) { return fk_vsp << 1; } if (ks_k == 8) { return fk_fp << 1; } if (ks_k >= 100 && ks_k < 100 + ks_n) { return fk_arms[ks_k - 100] << 1; } return 0; } if (t == 128) { long long fr_nv = fk_walk(fk_node[i][1], fp); long long fr_fv = fk_walk(fk_node[i][2], fp); long long fr_pk = fk_walk(fk_node[i][3], fp) >> 1; long long fr_ni = fk_nidx(fr_nv); if (fr_ni >= 1 && fr_ni <= fk_np) { fk_nsfile[fr_ni] = fr_fv; fk_nsline[fr_ni] = fr_pk >> 16; fk_nscol[fr_ni] = fr_pk & 65535; fk_nsattr[fr_ni] = 1; if (fk_fbn < 65536) { fk_fbroots[fk_fbn] = fr_nv; fk_fbn = fk_fbn + 1; } } return fr_nv; } if (t == 129) { if (fk_cap == 0) { fk_arena(); } if ((fk_hp + fk_fbn + 4) * 100 >= fk_cap * 90) { fk_melt(); } long long fe_r = 1; long long fe_i = fk_fbn; while (fe_i > 0) { fe_i = fe_i - 1; if (fk_hp + 1 < fk_cap) { fk_hp = fk_hp + 1; fk_hh[fk_hp] = fk_fbroots[fe_i]; fk_ht[fk_hp] = fe_r; fe_r = (fk_hp << 1) | 1; } } return fe_r; } if (t == 130) { long long ns_nv = fk_walk(fk_node[i][1], fp); long long ns_ni = fk_nidx(ns_nv); if (ns_ni < 1 || ns_ni > fk_np || fk_nsattr[ns_ni] == 0) { return 1; } if (fk_cap == 0) { fk_arena(); } if ((fk_hp + 6) * 100 >= fk_cap * 90) { fk_melt(); } long long ns_c = 1; fk_hp = fk_hp + 1; fk_hh[fk_hp] = (fk_nscol[ns_ni] << 1); fk_ht[fk_hp] = ns_c; ns_c = (fk_hp << 1) | 1; fk_hp = fk_hp + 1; fk_hh[fk_hp] = (fk_nsline[ns_ni] << 1); fk_ht[fk_hp] = ns_c; ns_c = (fk_hp << 1) | 1; fk_hp = fk_hp + 1; fk_hh[fk_hp] = fk_nsfile[ns_ni]; fk_ht[fk_hp] = ns_c; ns_c = (fk_hp << 1) | 1; return ns_c; } if (t == 131) { long long fc_i = 1; while (fc_i <= fk_np) { fk_nsattr[fc_i] = 0; fc_i = fc_i + 1; } fk_fbn = 0; return 0; } if (t == 69) { fk_walk(fk_node[i][1], fp); return fk_walk(fk_node[i][2], fp); } if (t == 102) { long long ae = fk_walk(fk_node[i][1], fp); long long be = fk_walk(fk_node[i][2], fp); if (fk_num(ae) == fk_num(be)) { return 2; } return 0; } if (t == 103) { long long al = fk_walk(fk_node[i][1], fp); long long bl = fk_walk(fk_node[i][2], fp); if (fk_num(al) < fk_num(bl)) { return 2; } return 0; } if (t == 109) { fk_vs[fp + (fk_walk(fk_node[i][1], fp) >> 1)] = fk_walk(fk_node[i][2], fp); return fk_walk(fk_node[i][3], fp); } if (t == 110) { return fk_vs[fp + (fk_walk(fk_node[i][1], fp) >> 1)]; } if (t == 111) { long long k111 = fk_walk(fk_node[i][1], fp) >> 1; long long sv111 = fk_vsp; long long need111 = fp + 1 + k111; while (fk_vsp < need111) { fk_vs[fk_vsp] = 0; fk_vsp = fk_vsp + 1; } long long r111 = fk_walk(fk_node[i][2], fp); fk_vsp = sv111; return r111; } if (t == 114) { char rbuf[8192]; long long rn = 0; while (rn < 8191) { char rc; long long rg = read(0, &rc, 1); if (rg <= 0) { if (rn == 0) { return 0 - 2; } break; } if (rc == 10) { break; } rbuf[rn] = rc; rn = rn + 1; } while (fk_sbp + rn > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } long long rj = 0; while (rj < rn) { fk_sb[fk_sbp + rj] = rbuf[rj]; rj = rj + 1; } return fk_sintern(fk_sbp, rn) << 1; } if (t == 115) { long long psv = fk_walk(fk_node[i][1], fp); long long psa = psv >> 1; if (psa >= 0 && psa < fk_sp) { long long pj = 0; while (pj < fk_sl[psa]) { putchar((int)(unsigned char)fk_sb[fk_so[psa] + pj]); pj = pj + 1; } } putchar(10); return 0; } if (t == 116) { if (isatty(0)) { return 2; } return 0; } if (t == 117) { if (fk_gen_len <= 0) { return fk_sintern(fk_sbp, 0) << 1; } while (fk_sbp + fk_gen_len > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } long long gj = 0; while (gj < fk_gen_len) { fk_sb[fk_sbp + gj] = (char)fk_gen[gj]; gj = gj + 1; } return fk_sintern(fk_sbp, fk_gen_len) << 1; } return 0; } /* ── SEED bootstrap stone 1: run Form SOURCE directly (the README's "minimal flatten
   baked into runtime/fkwu-uni.c"). A recursive-descent s-expr parser turns source into
   the SAME node format fk_run loads, then fk_walk runs it. This is the ONE-TIME bootstrap
   seed (like the cc command) whose telos is to flatten the FORM flattener (form-flatten.fk)
   so it self-hosts, then RETIRE. It is NOT body logic that stays in C.

   BOUNDED — do not grow this into a full C flattener (that is the carrier-last inversion).
   The flattener IS Form (form-flatten.fk; flatten/README.md "the real flatten body"); the
   source-runner IS Form (grammars/form-eval.fk runs source off the BMF cursor). Both run
   native via the JIT. This parser exists ONLY to break the keystone circularity once: parse
   just enough to flatten form-flatten.fk, hand off, and stop. The cleaner unlock is the
   PLATFORM-NEUTRAL seed (form-flatten.fk flattened once, on the Mac) — committing that data
   lets Form flatten Form here with no C parser at all. Stones 1-3 (literals/ops/if, defn+
   recursion, do+let) prove the circularity is breakable on Windows; they are the bootstrap,
   not the destination. Witnessed: fkwu --src run a .fk file. */
static char fk_srctext[262144];
static long long fk_spos;
static long long fk_slen;
static int fk_sws(char c) { return c == 32 || c == 9 || c == 10 || c == 13; }
static void fk_sskip(void) { while (fk_spos < fk_slen) { char c = fk_srctext[fk_spos]; if (fk_sws(c)) { fk_spos = fk_spos + 1; } else if (c == 59) { while (fk_spos < fk_slen && fk_srctext[fk_spos] != 10) { fk_spos = fk_spos + 1; } } else { break; } } }
static int fk_sym_eq(long long s, long long n, const char *w) { long long i = 0; while (w[i] != 0) { if (i >= n || fk_srctext[s + i] != w[i]) { return 0; } i = i + 1; } return i == n; }
static long long fk_arg_s, fk_arg_n, fk_fname_s, fk_fname_n;   /* stone 2: the defn's single arg + fn name (offset,len in srctext) */
static int fk_sym_eq2(long long s1, long long n1, long long s2, long long n2) { if (n1 != n2) { return 0; } long long i = 0; while (i < n1) { if (fk_srctext[s1 + i] != fk_srctext[s2 + i]) { return 0; } i = i + 1; } return 1; }
static long long fk_sym_end(long long s) { while (s < fk_slen) { char d = fk_srctext[s]; if (fk_sws(d) || d == 40 || d == 41) { break; } s = s + 1; } return s; }
static long long fk_optag(long long s, long long n) { if (fk_sym_eq(s, n, "add")) { return 3; } if (fk_sym_eq(s, n, "sub")) { return 4; } if (fk_sym_eq(s, n, "mul")) { return 42; } if (fk_sym_eq(s, n, "div")) { return 10; } if (fk_sym_eq(s, n, "mod")) { return 11; } if (fk_sym_eq(s, n, "le")) { return 5; } if (fk_sym_eq(s, n, "eq")) { return 102; } if (fk_sym_eq(s, n, "if")) { return 6; } return -1; }
static long long fk_smknode(long long t0, long long c1, long long c2, long long c3) { long long k = fk_node_count; fk_node_count = fk_node_count + 1; fk_node[k][0] = t0; fk_node[k][1] = c1; fk_node[k][2] = c2; fk_node[k][3] = c3; return k; }
static long long fk_smklit(long long v) { return fk_smknode(1, v, 0, 0); }
/* stone 3: a binding stack maps a name -> a FRAME SLOT (the arg is slot 0; each let takes the next slot).
   A bare bound name lowers to tag 110 (read fk_vs[fp+slot]); a let lowers to tag 109 (store then body);
   a function reserves fk_maxslot slots (tag 111). Over-reserve is safe (form-flatten over-reserves too). */
static long long fk_bd_s[128], fk_bd_n[128], fk_bd_off[128], fk_bd_top, fk_maxslot;
static long long fk_bd_lookup(long long s, long long n) { long long i = fk_bd_top; while (i > 0) { i = i - 1; if (fk_sym_eq2(s, n, fk_bd_s[i], fk_bd_n[i])) { return fk_bd_off[i]; } } return -1; }
static void fk_bd_push(long long s, long long n, long long off) { if (fk_bd_top < 128) { fk_bd_s[fk_bd_top] = s; fk_bd_n[fk_bd_top] = n; fk_bd_off[fk_bd_top] = off; fk_bd_top = fk_bd_top + 1; } }
static void fk_bd_pop(void) { if (fk_bd_top > 0) { fk_bd_top = fk_bd_top - 1; } }
static long long fk_parse_do(void);
/* stone 4: a function table. Each top-level (defn name ...) gets its own fn-index (>=1); a call to a
   registered name lowers to tag 12 (call-by-index, single-arg). A non-defn top form is the root (fn[0]). */
static long long fk_fnsym_s[256], fk_fnsym_n[256], fk_fnidx[256], fk_fntop, fk_defn_next, fk_root;
static long long fk_fn_lookup(long long s, long long n) { long long i = 0; while (i < fk_fntop) { if (fk_sym_eq2(s, n, fk_fnsym_s[i], fk_fnsym_n[i])) { return fk_fnidx[i]; } i = i + 1; } return -1; }
static long long fk_sparse(void) {
 fk_sskip();
 if (fk_spos >= fk_slen) { return 0; }
 char c = fk_srctext[fk_spos];
 if (c == 40) {
  fk_spos = fk_spos + 1; fk_sskip();
  long long s = fk_spos; fk_spos = fk_sym_end(fk_spos); long long hn = fk_spos - s;
  /* (defn name (arg) body): arg -> slot 0; body becomes fn[0], wrapped in a reserve over its lets. */
  if (fk_sym_eq(s, hn, "defn")) {
   fk_sskip(); long long ns2 = fk_spos; fk_spos = fk_sym_end(fk_spos); fk_fname_s = ns2; fk_fname_n = fk_spos - ns2;
   fk_sskip(); if (fk_spos < fk_slen && fk_srctext[fk_spos] == 40) { fk_spos = fk_spos + 1; }
   fk_sskip(); long long as2 = fk_spos; fk_spos = fk_sym_end(fk_spos); long long alen = fk_spos - as2;
   fk_sskip(); if (fk_spos < fk_slen && fk_srctext[fk_spos] == 41) { fk_spos = fk_spos + 1; }
   fk_bd_top = 0; fk_maxslot = 0; fk_bd_push(as2, alen, 0);
   long long body = fk_sparse();
   fk_sskip(); if (fk_spos < fk_slen && fk_srctext[fk_spos] == 41) { fk_spos = fk_spos + 1; }
   if (fk_maxslot > 0) { body = fk_smknode(111, fk_smklit(fk_maxslot), body, 0); }
   return body;
  }
  if (fk_sym_eq(s, hn, "do")) { return fk_parse_do(); }
  /* (let name val body): a 3-element standalone let — store val at the next slot, eval body in scope. */
  if (fk_sym_eq(s, hn, "let")) {
   fk_sskip(); long long ns = fk_spos; fk_spos = fk_sym_end(fk_spos); long long nlen = fk_spos - ns;
   long long val = fk_sparse();
   long long slot = fk_maxslot + 1; fk_maxslot = slot; fk_bd_push(ns, nlen, slot);
   long long body = fk_sparse(); fk_bd_pop();
   fk_sskip(); if (fk_spos < fk_slen && fk_srctext[fk_spos] == 41) { fk_spos = fk_spos + 1; }
   return fk_smknode(109, fk_smklit(slot), val, body);
  }
  long long tag = fk_optag(s, hn);
  if (tag >= 0) {
   long long ar = (tag == 6) ? 3 : 2;
   long long c1 = 0, c2 = 0, c3 = 0;
   if (ar >= 1) { c1 = fk_sparse(); }
   if (ar >= 2) { c2 = fk_sparse(); }
   if (ar >= 3) { c3 = fk_sparse(); }
   fk_sskip(); if (fk_spos < fk_slen && fk_srctext[fk_spos] == 41) { fk_spos = fk_spos + 1; }
   return fk_smknode(tag, c1, c2, c3);
  }
  /* call: any registered function name (INCLUDING self — each defn is registered before its body is
     parsed) -> tag 12 (call fk_fn[idx] with one arg). This replaces the old fn[0]-only self-call (tag 7),
     which was wrong once there is more than one function. A 0-arg call parses a dummy 0 off the immediate
     ) — the callee reads no slot, so it is inert. */
  long long fidx = fk_fn_lookup(s, hn);
  if (fidx >= 0) {
   long long c1 = fk_sparse();
   fk_sskip(); if (fk_spos < fk_slen && fk_srctext[fk_spos] == 41) { fk_spos = fk_spos + 1; }
   return fk_smknode(12, fidx, c1, 0);
  }
  while (fk_spos < fk_slen && fk_srctext[fk_spos] != 41) { fk_spos = fk_spos + 1; } if (fk_spos < fk_slen) { fk_spos = fk_spos + 1; } return fk_smklit(0);
 }
 if ((c >= 48 && c <= 57) || (c == 45 && fk_spos + 1 < fk_slen && fk_srctext[fk_spos + 1] >= 48 && fk_srctext[fk_spos + 1] <= 57)) {
  long long neg = 0; if (c == 45) { neg = 1; fk_spos = fk_spos + 1; }
  long long v = 0; while (fk_spos < fk_slen) { char d = fk_srctext[fk_spos]; if (d < 48 || d > 57) { break; } v = v * 10 + (d - 48); fk_spos = fk_spos + 1; }
  if (neg) { v = 0 - v; }
  return fk_smklit(v);
 }
 /* a bare symbol: a bound name -> tag 110 (read its frame slot); else an honest 0. */
 long long s = fk_spos; fk_spos = fk_sym_end(fk_spos);
 long long off = fk_bd_lookup(s, fk_spos - s);
 if (off >= 0) { return fk_smknode(110, fk_smklit(off), 0, 0); }
 return fk_smklit(0);
}
/* (do f1 f2 .. fn): sequence forms (tag 69 = eval-first/return-rest). A do-let `(let name val)` binds
   `name` to the next slot for the REST of the do (the common bind-the-rest pattern). */
static long long fk_parse_do(void) {
 fk_sskip();
 if (fk_spos >= fk_slen || fk_srctext[fk_spos] == 41) { if (fk_spos < fk_slen) { fk_spos = fk_spos + 1; } return fk_smklit(0); }
 if (fk_srctext[fk_spos] == 40) {
  long long p = fk_spos + 1; while (p < fk_slen && fk_sws(fk_srctext[p])) { p = p + 1; }
  long long he = fk_sym_end(p);
  if (fk_sym_eq(p, he - p, "let")) {
   fk_spos = he; fk_sskip();
   long long ns = fk_spos; fk_spos = fk_sym_end(fk_spos); long long nlen = fk_spos - ns;
   long long val = fk_sparse();
   fk_sskip(); if (fk_spos < fk_slen && fk_srctext[fk_spos] == 41) { fk_spos = fk_spos + 1; }
   long long slot = fk_maxslot + 1; fk_maxslot = slot; fk_bd_push(ns, nlen, slot);
   long long rest = fk_parse_do(); fk_bd_pop();
   return fk_smknode(109, fk_smklit(slot), val, rest);
  }
 }
 long long node = fk_sparse();
 fk_sskip();
 if (fk_spos >= fk_slen || fk_srctext[fk_spos] == 41) { if (fk_spos < fk_slen) { fk_spos = fk_spos + 1; } return node; }
 long long rest = fk_parse_do();
 return fk_smknode(69, node, rest, 0);
}
extern int atoi(const char *);
/* one top-level form: (do ...) is transparent (its inner forms are top-level too); (defn ...) registers
   a function at its own index; anything else is the root expression. Multi-arg defns push each arg name
   to slots 0..k-1 (callable single-arg via tag 12 today; multi-arg calls are the next stone). */
static void fk_parse_top(void) {
 fk_sskip();
 if (fk_spos >= fk_slen) { return; }
 if (fk_srctext[fk_spos] == 40) {
  long long p = fk_spos + 1; while (p < fk_slen && fk_sws(fk_srctext[p])) { p = p + 1; }
  long long he = fk_sym_end(p);
  if (fk_sym_eq(p, he - p, "do")) {
   fk_spos = he;
   while (1) { fk_sskip(); if (fk_spos >= fk_slen || fk_srctext[fk_spos] == 41) { break; } fk_parse_top(); }
   if (fk_spos < fk_slen && fk_srctext[fk_spos] == 41) { fk_spos = fk_spos + 1; }
   return;
  }
  if (fk_sym_eq(p, he - p, "defn")) {
   fk_spos = he; fk_sskip();
   long long ns2 = fk_spos; fk_spos = fk_sym_end(fk_spos); long long nlen2 = fk_spos - ns2;
   long long idx = fk_defn_next; fk_defn_next = fk_defn_next + 1;
   if (fk_fntop < 256) { fk_fnsym_s[fk_fntop] = ns2; fk_fnsym_n[fk_fntop] = nlen2; fk_fnidx[fk_fntop] = idx; fk_fntop = fk_fntop + 1; }
   fk_fname_s = ns2; fk_fname_n = nlen2;
   fk_sskip(); if (fk_spos < fk_slen && fk_srctext[fk_spos] == 40) { fk_spos = fk_spos + 1; }
   fk_bd_top = 0; fk_maxslot = 0; long long na = 0;
   while (1) { fk_sskip(); if (fk_spos >= fk_slen || fk_srctext[fk_spos] == 41) { break; } long long as = fk_spos; fk_spos = fk_sym_end(fk_spos); fk_bd_push(as, fk_spos - as, na); if (na > fk_maxslot) { fk_maxslot = na; } na = na + 1; }
   if (fk_spos < fk_slen && fk_srctext[fk_spos] == 41) { fk_spos = fk_spos + 1; }
   long long body = fk_sparse();
   fk_sskip(); if (fk_spos < fk_slen && fk_srctext[fk_spos] == 41) { fk_spos = fk_spos + 1; }
   if (fk_maxslot > 0) { body = fk_smknode(111, fk_smklit(fk_maxslot), body, 0); }
   if (idx >= 0 && idx < 4096) { fk_fn[idx] = body; }
   return;
  }
 }
 fk_root = fk_sparse();
}
static int fk_run_src(const char *path, long long arg) {
#if defined(_WIN32)
 int fd = open(path, 0x8000);
#else
 int fd = open(path, 0);
#endif
 if (fd < 0) { return 2; }
 long long g = read(fd, fk_srctext, 262143); close(fd); if (g < 0) { return 3; }
 fk_slen = g; fk_spos = 0; fk_srctext[g] = 0;
 fk_arg_n = 0; fk_fname_n = 0; fk_node_count = 0; fk_bd_top = 0; fk_maxslot = 0;
 fk_fntop = 0; fk_defn_next = 1; fk_root = -1;
 while (1) { fk_sskip(); if (fk_spos >= fk_slen) { break; } fk_parse_top(); }
 if (fk_root >= 0) { fk_fn[0] = fk_root; }
 else if (fk_defn_next > 1) { fk_fn[0] = fk_fn[fk_defn_next - 1]; } /* single/last defn, staged-arg driven (stones 1-2) */
 else { fk_fn[0] = fk_smklit(0); }
 fk_fn_count = fk_defn_next;
 fk_vs[0] = arg << 1; fk_vsp = 1;
 fk_pv_root(fk_fn[0], fk_walk(fk_fn[0], 0));
 return 0;
}
static int fk_run(int argc, char **argv) { if (argc < 2) { return 1; } if (argc >= 3 && argv[1][0] == 45 && argv[1][1] == 45) { return fk_run_src(argv[2], argc > 3 ? atoi(argv[3]) : 0); } int fd = open(argv[1], 0); if (fd < 0) { return 2; } long long got = read(fd, fk_buf, 1048575); if (got < 0) { return 3; } fk_buf[got] = 0; long long nf = fk_next(); fk_fn_count = nf; long long k = 0; while (k < nf) { fk_fn[k] = fk_next(); k = k + 1; } long long nr = fk_next(); fk_node_count = nr; long long r = 0; while (r < nr) { fk_node[r][0] = fk_next(); fk_node[r][1] = fk_next(); fk_node[r][2] = fk_next(); fk_node[r][3] = fk_next(); r = r + 1; } long long ns = fk_next(); fk_sinit(); long long si = 0; while (si < ns) { long long sl = fk_next(); if (fk_sp >= fk_scap_s) { fk_scap_s = fk_scap_s * 2; fk_so = realloc(fk_so, fk_scap_s * 8); fk_sl = realloc(fk_sl, fk_scap_s * 8); } while (fk_sbp + sl > fk_scap_b) { fk_scap_b = fk_scap_b * 2; fk_sb = realloc(fk_sb, fk_scap_b); } fk_so[fk_sp] = fk_sbp; fk_sl[fk_sp] = sl; long long bj = 0; while (bj < sl) { fk_sb[fk_sbp] = (char)fk_next(); fk_sbp = fk_sbp + 1; bj = bj + 1; } fk_sp = fk_sp + 1; si = si + 1; } long long a = 0; if (argc > 2) { a = atoi(argv[2]) << 1; } if (argc > 3) { int sfd = open(argv[3], 0); if (sfd >= 0) { long long sg = read(sfd, fk_src, 262143); if (sg >= 0) { fk_src[sg] = 0; } } } fk_vs[0] = a; fk_vsp = 1; if (argc > 4) { fk_hot = atoi(argv[4]); } if (argc > 5 && argv[5][0] == 106) { fk_nat_code[0] = fk_demo_inc; fk_nat_len[0] = 8; } long long rootv; fk_heat[0] = fk_heat[0] + 1; if (fk_hot > 0 && fk_heat[0] >= fk_hot && fk_nat_code[0] != 0) { fk_njit = fk_njit + 1; rootv = fk_native_call(fk_nat_code[0], fk_nat_len[0], fk_vs[0] >> 1) << 1; } else { rootv = fk_walk(fk_fn[0], 0); } fk_pv_root(fk_fn[0], rootv); long long t = 1; while (t <= 255) { fk_pr(fk_arms[t]); t = t + 1; } fk_pr(fk_njit); return 0; }
#if defined(_WIN32)
int main(int argc, char **argv) { return fk_run(argc, argv); }
#else
extern char *getenv(const char *); typedef void *fk_pthread_t; typedef struct { long fk_pa_sig; char fk_pa_opaque[64]; } fk_pthread_attr_t; extern int pthread_attr_init(fk_pthread_attr_t *); extern int pthread_attr_setstacksize(fk_pthread_attr_t *, unsigned long); extern int pthread_create(fk_pthread_t *, const fk_pthread_attr_t *, void *(*)(void *), void *); extern int pthread_join(fk_pthread_t, void **); static int fk_run_argc; static char **fk_run_argv; static int fk_run_ret; static void *fk_run_thunk(void *p) { (void)p; fk_run_ret = fk_run(fk_run_argc, fk_run_argv); return 0; } int main(int argc, char **argv) { fk_run_argc = argc; fk_run_argv = argv; unsigned long mb = 256; char *e = getenv("FORM_KERNEL_STACK_MB"); if (e) { int v = atoi(e); if (v > 0) { mb = (unsigned long)v; } } fk_pthread_attr_t at; pthread_attr_init(&at); pthread_attr_setstacksize(&at, mb * 1024UL * 1024UL); fk_pthread_t th; if (pthread_create(&th, &at, fk_run_thunk, 0) != 0) { return fk_run(argc, argv); } pthread_join(th, 0); return fk_run_ret; }
#endif

