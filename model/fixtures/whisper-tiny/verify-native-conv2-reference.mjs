#!/usr/bin/env node
// Independent Node witness for released Whisper-tiny conv1 -> GELU -> conv2 -> GELU.
// No Form output is imported: PCM parsing, Slaney filters, direct DFTs, tensor
// indexing, and all 535,296 learned parameters are evaluated here independently.

import fs from "node:fs";
import crypto from "node:crypto";
import { spawnSync } from "node:child_process";

const root = "model/fixtures/whisper-tiny";
const read = (name) => fs.readFileSync(`${root}/${name}`);
const wav = read("lingua-libre-book-16k.wav");
const w1 = read("encoder-conv1-weight.f32"), b1 = read("encoder-conv1-bias.f32");
const w2 = read("encoder-conv2-weight.f32"), b2 = read("encoder-conv2-bias.f32");
const sha256 = (bytes) => crypto.createHash("sha256").update(bytes).digest("hex");
const fixtures = [
  [wav, "1166acadc40e8d60baa82c6321ba3445fda5305a46539c3d1a0cc43e425de523"],
  [w1, "bb6642598e3efd8ea1fe81605f864342bb174604cba8dee5c23aa223fc126ecb"],
  [b1, "a8deb23b8cb5d0a88ffa398c9951ef92a3e47d44b32412dcb40b01895ec4772f"],
  [w2, "3b38df5c53ddbe1e9a38fdebb02d0d59b3ed3a4626409499bf1c4ea9ef2dc8d4"],
  [b2, "76fb23900c7e77f0c0f1938404ba9c3d1ca569115abb62daa8d9cb3ac08192b3"],
];
for (const [bytes, hash] of fixtures) if (sha256(bytes) !== hash) throw new Error("fixture SHA-256 mismatch");

const dataOffset = wav.indexOf(Buffer.from("data")) + 8;
const sample = (i) => wav.readInt16LE(dataOffset + i * 2) / 32768;
const hann = (n) => 0.5 - 0.5 * Math.cos(2 * Math.PI * n / 400);
function hzToMel(hz) {
  const fsp = 200 / 3, minLogHz = 1000, minLogMel = minLogHz / fsp, logstep = Math.log(6.4) / 27;
  return hz >= minLogHz ? minLogMel + Math.log(hz / minLogHz) / logstep : hz / fsp;
}
function melToHz(mel) {
  const fsp = 200 / 3, minLogMel = 15, logstep = Math.log(6.4) / 27;
  return mel >= minLogMel ? 1000 * Math.exp(logstep * (mel - minLogMel)) : fsp * mel;
}
const lo = hzToMel(0), hi = hzToMel(8000);
const edges = Array.from({length: 82}, (_, i) => melToHz(lo + (hi - lo) * i / 81));
const fftHz = Array.from({length: 201}, (_, i) => i * 40);
const bank = Array.from({length: 80}, (_, m) => {
  const lower = edges[m], center = edges[m + 1], upper = edges[m + 2], norm = 2 / (upper - lower);
  return fftHz.map((hz) => Math.max(0, Math.min((hz - lower) / (center - lower), (upper - hz) / (upper - center))) * norm)
    .map((x) => Number(x.toFixed(8)));
});
function column(start) {
  const frame = Array.from({length: 400}, (_, n) => sample(start + n) * hann(n));
  const powers = Array.from({length: 201}, (_, k) => {
    let re = 0, im = 0;
    for (let n = 0; n < 400; n += 1) {
      const phase = 2 * Math.PI * k * n / 400;
      re += frame[n] * Math.cos(phase); im -= frame[n] * Math.sin(phase);
    }
    return re * re + im * im;
  });
  return bank.map((row) => Math.log10(Math.max(1e-10, row.reduce((sum, x, k) => sum + x * powers[k], 0))));
}
const cols = Array.from({length: 6}, (_, i) => column(4160 + i * 160));
const peak = Math.max(...cols.flat());
const normalized = cols.map((col) => col.map((x) => (Math.max(x, peak - 8) + 4) / 4));

// Ruby's libm-backed Math.erf is intentionally independent from Form's A&S
// approximation. Values cross the process boundary as little-endian f64.
const geluLibm = (values) => {
  const input = Buffer.alloc(values.length * 8);
  values.forEach((x, i) => input.writeDoubleLE(x, i * 8));
  const code = "STDOUT.write(STDIN.read.unpack('E*').map{|x| 0.5*x*(1.0+Math.erf(x/Math.sqrt(2.0)))}.pack('E*'))";
  const run = spawnSync("ruby", ["-e", code], {input, maxBuffer: 1024 * 1024});
  if (run.status !== 0 || run.stdout.length !== input.length)
    throw new Error(`Ruby libm GELU failed: ${run.stderr.toString()}`);
  return values.map((_, i) => run.stdout.readDoubleLE(i * 8));
};
const conv1Raw = (start) => Array.from({length: 384}, (_, out) => {
  let value = b1.readFloatLE(out * 4);
  for (let input = 0; input < 80; input += 1) for (let time = 0; time < 3; time += 1)
    value += w1.readFloatLE(((out * 80 + input) * 3 + time) * 4) * normalized[start + time][input];
  return value;
});
const conv1Activated = geluLibm(Array.from({length: 4}, (_, i) => conv1Raw(i)).flat());
const conv1Tokens = Array.from({length: 4}, (_, i) => conv1Activated.slice(i * 384, (i + 1) * 384));
const conv2Raw = Array.from({length: 384}, (_, out) => {
  let value = b2.readFloatLE(out * 4);
  for (let input = 0; input < 384; input += 1) for (let time = 0; time < 3; time += 1)
    value += w2.readFloatLE(((out * 384 + input) * 3 + time) * 4) * conv1Tokens[time + 1][input];
  return value;
});
const token = geluLibm(conv2Raw);
const positions = [0, 1, 2, 3, 95, 191, 287, 383];
const fingerprint = [...positions.map((i) => token[i]), token.reduce((a, x) => a + x, 0), token.reduce((a, x) => a + Math.abs(x), 0)];
console.log(JSON.stringify({
  implementation: "independent-node-direct-dft-slaney-convolutions+ruby-libm-erf-gelu",
  source: "Lingua-Libre-M92036254-human-book", dataOffset, melShape: [6, 80], peak,
  conv1WeightShape: [384, 80, 3], conv2WeightShape: [384, 384, 3],
  learnedParameters: 535296,
  operations: {dftBins: 1206, conv1MultiplyAdds: 368640, conv2MultiplyAdds: 442368},
  fingerprint,
}, null, 2));
