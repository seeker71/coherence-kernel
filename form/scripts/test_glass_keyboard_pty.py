#!/usr/bin/env python3
"""POSIX PTY acceptance test; Python is test equipment, never a Glass carrier.

Runs the production native terminal door and Form decoder/controller in a
bounded child. No existing terminal, sensor fleet or shared Glass data is touched.
"""
import os
import select
import signal
import subprocess
import termios
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DOOR = "form/form-stdlib/tests/form-glass-input-pty-run.fk"


class Session:
    def __init__(self):
        self.master, self.slave = os.openpty()
        self.saved = termios.tcgetattr(self.slave)
        self.child = subprocess.Popen(
            [str(ROOT / "fkwu"), DOOR], cwd=ROOT, stdin=self.slave,
            stdout=self.slave, stderr=self.slave, start_new_session=True,
        )
        self.buffer = b""
        self.until(b"[INPUT-READY, 1]", 90)
        direct = termios.tcgetattr(self.slave)
        assert not direct[3] & (termios.ICANON | termios.ECHO), direct

    def until(self, marker, timeout=2):
        deadline = time.monotonic() + timeout
        while marker not in self.buffer:
            assert time.monotonic() < deadline, (marker, self.buffer[-2000:])
            ready, _, _ = select.select([self.master], [], [], 0.05)
            if ready:
                self.buffer += os.read(self.master, 65536)
            assert self.child.poll() is None or marker in self.buffer, self.buffer[-2000:]
        before, self.buffer = self.buffer.split(marker, 1)
        return before

    def key(self, data, expected):
        self.buffer = b""
        started = time.monotonic()
        os.write(self.master, data)
        self.until(expected)
        return round((time.monotonic() - started) * 1000, 2)

    def restored(self):
        actual = termios.tcgetattr(self.slave)
        expected = list(self.saved)
        # Darwin marks PENDIN when canonical input resumes. It is kernel queue
        # state (sys/termios.h), not a user setting; every other bit, speed and
        # control character must match exactly, including ICANON/ECHO/ISIG.
        pending = getattr(termios, "PENDIN", 0)
        actual[3] &= ~pending
        expected[3] &= ~pending
        assert actual == expected, [(i, a, b) for i, (a, b) in enumerate(zip(expected, actual)) if a != b]

    def close(self):
        if self.child.poll() is None:
            self.child.terminate()
            self.child.wait(timeout=5)
        try:
            self.restored()
        finally:
            os.close(self.master)
            os.close(self.slave)


def main():
    session = Session()
    try:
        timings = []
        timings.append(session.key(b"s", b"[KEY-STATE, meaning, all,"))
        timings.append(session.key(b"0", b"[KEY-STATE, meaning, none,"))
        timings.append(session.key(b"2", b"[KEY-STATE, meaning, py,"))
        timings.append(session.key(b"1", b"[KEY-STATE, meaning, go+py,"))
        timings.append(session.key(b"1", b"[KEY-STATE, meaning, py,"))
        # A five-second host wait is interrupted by terminal input, not a newline.
        assert b"-2]" in session.buffer, session.buffer
        for key, view in [(b"h", b"help"), (b"a", b"atlas"), (b"t", b"raster"),
                          (b"o", b"overview"), (b"m", b"memory"), (b"f", b"flow"),
                          (b"j", b"recipes"), (b"k", b"kernel"), (b"v", b"events"),
                          (b"n", b"channels")]:
            timings.append(session.key(key, b"[KEY-STATE, " + view + b","))
        session.key(b"\x1b[200~q0sh\x1b[201~", b"[KEY-STATE, channels, py,")
        assert session.child.poll() is None, "paste executed quit"
        session.key(b"\x1b", b"[KEY-STATE, channels,")
        session.key(b"[", b"[KEY-STATE, channels,")
        session.key(b"B", b"sample.")
        session.key(b"e", b", 1, ,")
        session.key(b"q", b"RESTORED")
        assert session.child.wait(timeout=5) == 0
        session.restored()
        print(f"PASS direct keys before newline: {len(timings)} transitions; max={max(timings)} ms")
        print("PASS zero/multiselect, split arrow, bracketed paste, inspect/evidence, clean quit")
    finally:
        session.close()

    for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGHUP):
        session = Session()
        try:
            session.child.send_signal(sig)
            session.child.wait(timeout=5)
            session.restored()
            print(f"PASS terminal restoration on {sig.name}")
        finally:
            session.close()

    session = Session()
    try:
        session.child.send_signal(signal.SIGTSTP)
        _, status = os.waitpid(session.child.pid, os.WUNTRACED)
        assert os.WIFSTOPPED(status), status
        session.restored()
        session.child.send_signal(signal.SIGCONT)
        session.key(b"s", b"[KEY-STATE, meaning, all,")
        session.key(b"q", b"RESTORED")
        session.child.wait(timeout=5)
        session.restored()
        print("PASS suspension/resume and subsequent direct input")
    finally:
        session.close()

    result = subprocess.run([str(ROOT / "fkwu"), DOOR], cwd=ROOT,
                            input=b"q", stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=15)
    assert result.returncode == 0 and b"NON-TTY" in result.stdout, result
    print("PASS non-TTY input remains untouched/unavailable")


if __name__ == "__main__":
    main()
