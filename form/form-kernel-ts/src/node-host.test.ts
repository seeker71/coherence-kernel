import { mkdtempSync, rmSync } from "node:fs";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { Frame, Kernel, walk } from "./kernel.ts";
import { createNodeKernelHost } from "./node-host.ts";
import { readAll } from "./reader.ts";

let passed = 0;
function check(name: string, condition: boolean, detail = ""): void {
  if (!condition) throw new Error(`${name}${detail === "" ? "" : `: ${detail}`}`);
  passed++;
}

const directory = mkdtempSync(join(tmpdir(), "form-kernel-ts-host-"));
const path = join(directory, "carrier.txt");
const formPath = path.split("\\").join("/");
const stdout: string[] = [];
const stderr: string[] = [];
const kernel = new Kernel(
  createNodeKernelHost({
    writeStdout: (text) => stdout.push(text),
    writeStderr: (text) => stderr.push(text),
    tempDirectory: () => directory,
  }),
);

function evaluate(source: string): string {
  const root = readAll(kernel, source);
  return kernel.render(walk(kernel, root, new Frame(null)));
}

try {
  check(
    "filesystem carrier",
    evaluate(`(do (write_file "${formPath}" "host-carrier") (read_file "${formPath}"))`) ===
      "host-carrier",
  );
  check(
    "stdout carrier",
    evaluate(`(do (print "node-output") 1)`) === "1" &&
      stdout.join("") === "node-output\n",
  );
  check("stderr remains empty", stderr.length === 0);
  check("temp directory carrier", evaluate(`(temp_dir)`) === directory);
  check(
    "UTF-8 bytes carrier",
    evaluate(`(string_bytes "Aλ🙂")`) === "[65, 206, 187, 240, 159, 153, 130]",
  );
  check(
    "streaming UTF-8 byte fold",
    evaluate(`(do
      (defn sum-byte (acc byte) (add acc byte))
      (string_byte_fold "Aλ🙂" 0 sum-byte))`) === "1140",
  );
  check(
    "socket worker carrier",
    evaluate(`(do
      (let listener (socket_listen 0))
      (let port (socket_port listener))
      (let client (socket_connect "127.0.0.1" port))
      (let server (socket_accept listener))
      (socket_send client "ping")
      (let message (socket_recv server 4))
      (socket_close client)
      (socket_close server)
      (socket_close listener)
      message)`) === "ping",
  );
} finally {
  kernel.shutdown();
  rmSync(directory, { recursive: true, force: true });
}

console.log(`${passed} passed, 0 failed`);
