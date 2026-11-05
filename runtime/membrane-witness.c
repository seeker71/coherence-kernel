/* The membrane floor. Same readings as the carrier: image, offer, leaf, fault. */
#include "membrane-boot.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>

static int fails;

static void check(const char *name, int ok) {
    printf("%s=%s\n", name, ok ? "1" : "0");
    if (!ok) fails++;
}

static int canon_eq(Machine *m, uint32_t a, uint32_t b) {
    const uint8_t *pa;
    const uint8_t *pb;
    uint32_t na = cell_canon(m, a, &pa);
    uint32_t nb = cell_canon(m, b, &pb);
    return na && na == nb && memcmp(pa, pb, na) == 0;
}

static int payload_has(Machine *m, uint32_t slot, const char *s) {
    const uint8_t *p;
    uint32_t n = cell_payload(m, slot, &p);
    size_t len = strlen(s);
    uint32_t i, c;
    if (p && n >= len) {
        for (i = 0; i + len <= n; i++)
            if (memcmp(p + i, s, len) == 0) return 1;
    }
    for (c = 0; c < cell_nchild(m, slot); c++)
        if (payload_has(m, cell_child(m, slot, c), s)) return 1;
    return 0;
}

static int frame_has_phase(Machine *m, int64_t phase) {
    uint32_t i;
    for (i = 0; i < frame_count(m); i++) {
        int64_t v = -1;
        uint32_t row = frame_row(m, i);
        if (cell_i64(m, cell_child(m, row, 0), &v) && v == phase) return 1;
    }
    return 0;
}

static void fault_child(int arch) {
    Machine *m = machine_open();
    uint8_t code[16];
    uint32_t n = 0;
    uint32_t bytes, port;
    uint32_t args[2];
    if (arch == 1) {
        uint32_t w0 = 0xD4200000u;
        uint32_t w1 = 0xD65F03C0u;
        memcpy(code, &w0, 4);
        memcpy(code + 4, &w1, 4);
        n = 8;
    } else if (arch == 2) {
        uint8_t x[] = {0x8b, 0x04, 0x25, 0x00, 0x00, 0x00, 0x00, 0xc3};
        memcpy(code, x, sizeof x);
        n = sizeof x;
    } else _exit(3);
    bytes = make_bytes(m, code, n);
    port = make_port(m, DIR_EFFERENT, SHAPE_BYTES, "cpu.leaf");
    args[0] = bytes;
    args[1] = make_scalar(m, 0);
    offer(m, port, args, 2);
    _exit(2);
}

int gf_membrane_witness(void) {
    Machine *m = machine_open();
    HostFacts facts;
    uint8_t code[16];
    uint32_t ncode = 0;
    uint32_t leaf, name, exp, channel;
    uint8_t *image = 0;
    size_t image_n = 0;
    Machine *thawed;
    Outcome boot, ran;
    int64_t observed = -1;
    uint32_t ack_slot;
    const uint32_t branch_code[] = {
        0xa9be7bfd, 0xa90153f3, 0xaa0003f3, 0xaa0203f4,
        0xf9400269, 0xf100053f, 0x54000061, 0xf9400660,
        0x14000002, 0xf9400a60, 0x91006261, 0xd2800042,
        0xd63f0280, 0xa94153f3, 0xa8c27bfd, 0xd65f03c0
    };

    machine_facts(m, &facts);
    printf("page=%ld\narch=%s\ndepth_limit=%llu\nentropy_present=%d\ngpu_present=%d\n",
           facts.page_size, boot_arch_name(facts.arch),
           (unsigned long long)facts.max_depth, facts.entropy_present, facts.gpu_present);
    check("nothing_is_not_zero", word_nothing() != word_state(0) && word_class(word_nothing()) == CLASS_NOTHING);
    check("state_zero_is_the_word_zero", word_state(0) == 0 && word_class(word_state(0)) == CLASS_STATE);
    check("state_rejects_other", word_class(word_state(2)) == CLASS_NOTHING);

    if (facts.arch == 1) {
        uint32_t w0 = 0x0B010000u, w1 = 0xD65F03C0u;
        memcpy(code, &w0, 4);
        memcpy(code + 4, &w1, 4);
        ncode = 8;
    } else if (facts.arch == 2) {
        uint8_t x[] = {0x01, 0xfe, 0x89, 0xf8, 0xc3};
        memcpy(code, x, sizeof x);
        ncode = sizeof x;
    }
    check("arch_known", ncode > 0);

    {
        uint32_t seven = make_scalar(m, 7);
        uint32_t eight = make_scalar(m, 8);
        uint32_t held = cell_compose(m, KIND_TEXT, &seven, 1, "kept", 4);
        const uint8_t *before;
        uint32_t bn = cell_canon(m, seven, &before);
        uint8_t saved[256];
        if (bn && bn < sizeof saved) memcpy(saved, before, bn);
        cell_pin(m, held);
        machine_release_unpinned(m);
        check("old_survives", cell_live(m, seven) && canon_eq(m, seven, seven) && cell_live(m, eight) == 0);
        cell_unpin(m, held);
        machine_release_unpinned(m);
        check("composted", cell_live(m, seven) == 0);
        {
            uint32_t again = make_scalar(m, 7);
            const uint8_t *after;
            uint32_t an = cell_canon(m, again, &after);
            check("remint_same_canon", bn && an == bn && memcmp(saved, after, bn) == 0);
        }
    }

    leaf = make_leaf(m, 0, code, ncode);
    name = make_text(m, "add");
    exp = make_export(m, name, leaf);
    channel = make_channel(m, "boot", &exp, 1);
    machine_set_channel(m, channel);
    check("freeze", freeze(m, &image, &image_n));
    printf("image_bytes=%zu\n", image_n);

    thawed = machine_open();
    check("thaw", thaw(thawed, image, image_n));
    boot = offer(thawed, machine_channel(thawed), 0, 0);
    printf("publish_class=%d\npublish_ack=%llu\n",
           boot.arrived ? word_class(boot.ack) : -1, (unsigned long long)boot.ack);
    check("publish_does_not_enter", boot.arrived && boot.ack == word_state(1));
    {
        uint32_t export_slot = cell_child(thawed, machine_channel(thawed), 0);
        uint32_t args[2];
        args[0] = make_scalar(thawed, 40);
        args[1] = make_scalar(thawed, 2);
        ran = offer(thawed, export_slot, args, 2);
        ack_slot = (ran.arrived && word_class(ran.ack) == CLASS_NODE) ? (uint32_t)word_payload(ran.ack) : 0;
        cell_i64(thawed, ack_slot, &observed);
    }
    printf("ack_class=%d\nobserved=%lld\n", ran.arrived ? word_class(ran.ack) : -1, (long long)observed);
    check("answer_is_node", ran.arrived && word_class(ran.ack) == CLASS_NODE);
    check("observed_42", observed == 42);

    {
        uint8_t *bad = malloc(image_n);
        Machine *refused = machine_open();
        uint32_t before = cell_live_count(refused);
        memcpy(bad, image, image_n);
        bad[image_n - 1] ^= 0x5a;
        check("tamper_refused", thaw(refused, bad, image_n) == 0 && cell_live_count(refused) == before);
        free(bad);
        machine_close(refused);
    }

    {
        uint32_t video = make_port(thawed, DIR_AFFERENT, SHAPE_PIXEL, "video");
        uint32_t wrong = make_port(thawed, DIR_AFFERENT, SHAPE_BYTES, "clock.monotonic");
        uint32_t clk = make_port(thawed, DIR_AFFERENT, SHAPE_SCALAR, "clock.monotonic");
        Outcome miss = offer(thawed, video, 0, 0);
        Outcome badp = offer(thawed, wrong, 0, 0);
        Outcome reading = offer(thawed, clk, 0, 0);
        uint32_t node = reading.arrived && word_class(reading.ack) == CLASS_NODE ? (uint32_t)word_payload(reading.ack) : 0;
        const uint8_t *dom = 0;
        uint32_t dn = 0;
        int64_t ticks = 0;
        printf("missing_class=%d\n", miss.arrived ? word_class(miss.ack) : -1);
        check("missing_is_nothing", miss.arrived && word_class(miss.ack) == CLASS_NOTHING && miss.ack != word_state(0));
        check("shape_mismatch_is_membrane_zero", badp.arrived && badp.ack == word_state(0) && frame_has_phase(thawed, 4));
        if (cell_kind(thawed, node) == KIND_READING) {
            dn = cell_payload(thawed, cell_child(thawed, node, 1), &dom);
            cell_i64(thawed, cell_child(thawed, node, 0), &ticks);
        }
        check("clock_domain", dn == 12 && dom && memcmp(dom, "monotonic-ns", 12) == 0 && ticks > 0);
    }

    {
        uint32_t path = make_text(thawed, "/tmp/greenfield-file");
        uint32_t body = make_bytes(thawed, "SECRET-PAYLOAD", 14);
        uint32_t wargs[2];
        uint32_t write_p = make_port(thawed, DIR_EFFERENT, SHAPE_BYTES, "file.write");
        uint32_t read_p = make_port(thawed, DIR_AFFERENT, SHAPE_BYTES, "file.read");
        Outcome w, r;
        const uint8_t *got = 0;
        uint32_t gn;
        wargs[0] = path;
        wargs[1] = body;
        w = offer(thawed, write_p, wargs, 2);
        r = offer(thawed, read_p, &path, 1);
        gn = (r.arrived && word_class(r.ack) == CLASS_NODE) ? cell_payload(thawed, (uint32_t)word_payload(r.ack), &got) : 0;
        check("file_durable", w.arrived && w.ack == word_state(1) && gn == 14 && got && memcmp(got, "SECRET-PAYLOAD", 14) == 0);
        unlink("/tmp/greenfield-file");
    }

    {
        uint32_t ent = make_port(thawed, DIR_AFFERENT, SHAPE_BYTES, "entropy");
        uint32_t n = make_scalar(thawed, 16);
        Outcome e = offer(thawed, ent, &n, 1);
        const uint8_t *ep = 0;
        uint32_t elen = 0;
        if (e.arrived && word_class(e.ack) == CLASS_NODE) elen = cell_payload(thawed, (uint32_t)word_payload(e.ack), &ep);
        if (facts.entropy_present) check("entropy_bytes", elen == 16);
        else check("entropy_absent", e.arrived && word_class(e.ack) == CLASS_NOTHING);
    }

    {
        uint32_t waitp = make_port(thawed, DIR_AFFERENT, SHAPE_SCALAR, "event.wait");
        uint32_t sigp = make_port(thawed, DIR_EFFERENT, SHAPE_BOOL, "event.signal");
        uint32_t zero = make_scalar(thawed, 0);
        uint32_t later = make_scalar(thawed, 1000000000ll);
        Outcome silence = offer(thawed, waitp, &zero, 1);
        Outcome open1 = offer(thawed, waitp, &later, 1);
        Outcome open2 = offer(thawed, waitp, &later, 1);
        Outcome poke = offer(thawed, sigp, 0, 0);
        Outcome woke = offer(thawed, waitp, &later, 1);
        check("timeout_is_nothing", silence.arrived && word_class(silence.ack) == CLASS_NOTHING);
        check("held_is_not_an_ack", open1.arrived == 0 && open2.arrived == 0);
        check("event_woke", poke.arrived && poke.ack == word_state(1) && woke.arrived && woke.ack == word_state(1));
    }

    {
        uint32_t gpu = make_port(thawed, DIR_AFFERENT, SHAPE_BOOL, "gpu.admit");
        Outcome g = offer(thawed, gpu, 0, 0);
        if (facts.gpu_present) check("gpu_admitted", g.arrived && g.ack == word_state(1));
        else check("gpu_absent", g.arrived && word_class(g.ack) == CLASS_NOTHING);
    }

    {
        uint32_t bin = make_text(thawed, "/usr/bin/true");
        uint32_t proc = make_port(thawed, DIR_EFFERENT, SHAPE_TEXT, "process");
        Outcome p = offer(thawed, proc, &bin, 1);
        int64_t status = -1;
        uint32_t node = p.arrived && word_class(p.ack) == CLASS_NODE ? (uint32_t)word_payload(p.ack) : 0;
        cell_i64(thawed, node, &status);
        check("process_exit_is_node", p.arrived && word_class(p.ack) == CLASS_NODE && status == 0);
    }

    {
        uint32_t eq = make_port(thawed, DIR_EFFERENT, SHAPE_BOOL, "identity.eq");
        uint32_t a = make_scalar(thawed, 3);
        uint32_t b = make_scalar(thawed, 3);
        uint32_t c = make_scalar(thawed, 4);
        uint32_t same[2], diff[2];
        Outcome eq1, eq0;
        same[0] = a;
        same[1] = b;
        diff[0] = a;
        diff[1] = c;
        eq1 = offer(thawed, eq, same, 2);
        eq0 = offer(thawed, eq, diff, 2);
        check("eq_same", eq1.arrived && eq1.ack == word_state(1) && a == b);
        check("eq_different", eq0.arrived && eq0.ack == word_state(0));
    }

    if (facts.arch == 1) {
        uint32_t probe = make_scalar(thawed, 1);
        uint32_t write_p = make_port(thawed, DIR_EFFERENT, SHAPE_BYTES, "file.write");
        uint32_t yes_path = make_text(thawed, "/tmp/greenfield-arm-yes");
        uint32_t no_path = make_text(thawed, "/tmp/greenfield-arm-no");
        uint32_t yes_body = make_bytes(thawed, "yes", 3);
        uint32_t no_body = make_bytes(thawed, "no", 2);
        uint32_t branch = make_leaf(thawed, 1, branch_code, sizeof branch_code);
        uint32_t args[5];
        Outcome br;
        args[0] = probe;
        args[1] = write_p;
        args[2] = write_p;
        args[3] = yes_path;
        args[4] = yes_body;
        (void)no_path;
        (void)no_body;
        br = offer(thawed, branch, args, 5);
        check("branch_one_arm", br.arrived && br.ack == word_state(1) &&
              access("/tmp/greenfield-arm-yes", R_OK) == 0 &&
              access("/tmp/greenfield-arm-no", R_OK) != 0);
        unlink("/tmp/greenfield-arm-yes");
    } else check("branch_one_arm", 0);

    {
        int secret = 0;
        uint32_t fi;
        for (fi = 0; fi < frame_count(thawed); fi++)
            if (payload_has(thawed, frame_row(thawed, fi), "SECRET-PAYLOAD")) secret = 1;
        check("frame_has_rows", frame_count(thawed) > 0);
        check("frame_keeps_text_out", secret == 0);
    }

    {
        uint32_t old_ch = machine_channel(thawed);
        const uint8_t *oldc;
        uint32_t oldn = cell_canon(thawed, old_ch, &oldc);
        uint8_t saved[4096];
        uint32_t msg = make_text(thawed, "more");
        Outcome ext;
        uint32_t born;
        if (oldn < sizeof saved) memcpy(saved, oldc, oldn);
        ext = offer(thawed, old_ch, &msg, 1);
        born = ext.arrived && word_class(ext.ack) == CLASS_NODE ? (uint32_t)word_payload(ext.ack) : 0;
        check("channel_mints",
              cell_live(thawed, old_ch) && oldn && cell_canon(thawed, old_ch, &oldc) == oldn &&
              memcmp(oldc, saved, oldn) == 0 && born != old_ch &&
              cell_nchild(thawed, born) == cell_nchild(thawed, old_ch) + 1);
    }

    {
        int pipes[2];
        pid_t child;
        int status = 0;
        int saw = -1;
        check("fault_pipe", pipe(pipes) == 0);
        child = fork();
        if (child == 0) {
            close(pipes[0]);
            fault_arm(pipes[1]);
            fault_child(facts.arch);
        }
        if (child > 0) {
            close(pipes[1]);
            waitpid(child, &status, 0);
            if (read(pipes[0], &saw, sizeof saw) != (ssize_t)sizeof saw) saw = -1;
            close(pipes[0]);
            printf("fault_term=%d\nfault_evidence=%d\n",
                   WIFSIGNALED(status) ? WTERMSIG(status) : 0, saw);
            check("fault_preserves_signal", WIFSIGNALED(status) && WTERMSIG(status) == SIGTRAP && saw == SIGTRAP);
        }
    }

    free(image);
    machine_close(m);
    machine_close(thawed);
    printf("fails=%d\n", fails);
    return fails ? 1 : 0;
}

/* The old bootstrap is cc of runtime/fkwu-uni.c, then that fkwu builds
 * form-cli. This carrier runs those same programs and compares the readings. */
static int spawn_tail(char *const argv[], const char *input, int capture_err,
                      char *tail, size_t cap, int *code) {
    int inpipe[2];
    int outpipe[2];
    pid_t pid;
    char buf[4096];
    size_t kept = 0;
    int status = 0;
    if (cap == 0) return -1;
    tail[0] = 0;
    if (pipe(inpipe) != 0 || pipe(outpipe) != 0) return -1;
    pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        dup2(inpipe[0], 0);
        dup2(outpipe[1], 1);
        if (capture_err) dup2(outpipe[1], 2);
        close(inpipe[0]); close(inpipe[1]);
        close(outpipe[0]); close(outpipe[1]);
        execvp(argv[0], argv);
        _exit(127);
    }
    close(inpipe[0]);
    close(outpipe[1]);
    if (input != 0) {
        size_t len = strlen(input);
        size_t wrote = 0;
        while (wrote < len) {
            ssize_t n = write(inpipe[1], input + wrote, len - wrote);
            if (n < 0) break;
            wrote += (size_t)n;
        }
    }
    close(inpipe[1]);
    for (;;) {
        ssize_t n = read(outpipe[0], buf, sizeof buf);
        if (n <= 0) break;
        if ((size_t)n >= cap - 1) {
            memcpy(tail, buf + (size_t)n - (cap - 1), cap - 1);
            kept = cap - 1;
        } else if (kept + (size_t)n < cap) {
            memcpy(tail + kept, buf, (size_t)n);
            kept += (size_t)n;
        } else {
            size_t drop = kept + (size_t)n - (cap - 1);
            memmove(tail, tail + drop, kept - drop);
            kept -= drop;
            memcpy(tail + kept, buf, (size_t)n);
            kept += (size_t)n;
        }
    }
    tail[kept] = 0;
    close(outpipe[0]);
    if (waitpid(pid, &status, 0) < 0) return -1;
    *code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    return 0;
}

static void trim_end(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ')) {
        s[--n] = 0;
    }
}

static int file_executable(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode) && access(path, X_OK) == 0 && st.st_size > 0;
}

int gf_produce(void) {
    char fkwu_out[] = "/tmp/sema-greenfield-boot/fkwu";
    char cli_out[] = "/tmp/sema-greenfield-boot/form-cli";
    char cli_old[] = "/tmp/sema-greenfield-boot/form-cli-standing";
    char ground_new[256];
    char ground_old[256];
    char tail[2048];
    char *cc_argv[6];
    char *ground_new_argv[3];
    char *ground_old_argv[3];
    char *cli_new_argv[3];
    char *cli_old_argv[3];
    const char *cli_new_in = "{\"out\":\"/tmp/sema-greenfield-boot/form-cli\"}\n";
    const char *cli_old_in = "{\"out\":\"/tmp/sema-greenfield-boot/form-cli-standing\"}\n";
    int code = -1;
    int bad = 0;
    int born;
    if (mkdir("/tmp/sema-greenfield-boot", 0755) != 0 && errno != EEXIST) {
        printf("produce-dir=0\nproduce-fails=1\n");
        return 1;
    }
    unlink(fkwu_out);
    cc_argv[0] = "cc";
    cc_argv[1] = "-O2";
    cc_argv[2] = "-o";
    cc_argv[3] = fkwu_out;
    cc_argv[4] = "runtime/fkwu-uni.c";
    cc_argv[5] = 0;
    born = spawn_tail(cc_argv, 0, 1, tail, sizeof tail, &code) == 0 && code == 0 && file_executable(fkwu_out);
    printf("produced-fkwu=%d\n", born);
    if (!born) {
        printf("%s\nproduce-fails=1\n", tail);
        return 1;
    }
    ground_new_argv[0] = fkwu_out;
    ground_new_argv[1] = "bootstrap/ground.fk";
    ground_new_argv[2] = 0;
    ground_old_argv[0] = "./fkwu";
    ground_old_argv[1] = "bootstrap/ground.fk";
    ground_old_argv[2] = 0;
    if (spawn_tail(ground_new_argv, "", 0, ground_new, sizeof ground_new, &code) != 0 || code != 0) bad++;
    trim_end(ground_new);
    if (spawn_tail(ground_old_argv, "", 0, ground_old, sizeof ground_old, &code) != 0 || code != 0) bad++;
    trim_end(ground_old);
    printf("ground-produced=%s\nground-standing=%s\nground-same=%d\n",
           ground_new, ground_old, strcmp(ground_new, ground_old) == 0 && strcmp(ground_new, "42") == 0);
    if (strcmp(ground_new, ground_old) != 0 || strcmp(ground_new, "42") != 0) bad++;
    if (bad) {
        printf("produce-fails=%d\n", bad);
        return 1;
    }
    unlink(cli_out);
    unlink(cli_old);
    cli_new_argv[0] = fkwu_out;
    cli_new_argv[1] = "gate/form-cli-build-run.bml";
    cli_new_argv[2] = 0;
    cli_old_argv[0] = "./fkwu";
    cli_old_argv[1] = "gate/form-cli-build-run.bml";
    cli_old_argv[2] = 0;
    {
        int new_ok = spawn_tail(cli_new_argv, cli_new_in, 1, tail, sizeof tail, &code) == 0
            && code == 0 && file_executable(cli_out);
        int old_ok = spawn_tail(cli_old_argv, cli_old_in, 1, tail, sizeof tail, &code) == 0
            && code == 0 && file_executable(cli_old);
        printf("form-cli-produced=%d\nform-cli-standing=%d\n", new_ok, old_ok);
        if (!new_ok || !old_ok) {
            printf("%s\n", tail);
            bad++;
        }
    }
    printf("produce-fails=%d\n", bad);
    return bad ? 1 : 0;
}

int main(int argc, char **argv) {
    int floor_rc;
    setvbuf(stdout, 0, _IOLBF, 0);
    if (argc == 2 && strcmp(argv[1], "--produce") == 0) {
        floor_rc = gf_membrane_witness();
        if (floor_rc != 0) return floor_rc;
        return gf_produce();
    }
    return gf_membrane_witness();
}
