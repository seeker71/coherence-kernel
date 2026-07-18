#!/usr/bin/env node
// Independent implementation of the complete Whisper-tiny conv1 witness.
// It parses raw PCM and f32 files with Node, generates the Slaney bank from its
// published formula, performs direct DFTs, then evaluates all 92,544 parameters.

import fs from "node:fs";
import crypto from "node:crypto";

const root = "model/fixtures/whisper-tiny";
const wav = fs.readFileSync(`${root}/lingua-libre-book-16k.wav`);
const weight = fs.readFileSync(`${root}/encoder-conv1-weight.f32`);
const bias = fs.readFileSync(`${root}/encoder-conv1-bias.f32`);
const sha256 = (bytes) => crypto.createHash("sha256").update(bytes).digest("hex");
const expected = new Map([
  [wav, "1166acadc40e8d60baa82c6321ba3445fda5305a46539c3d1a0cc43e425de523"],
  [weight, "bb6642598e3efd8ea1fe81605f864342bb174604cba8dee5c23aa223fc126ecb"],
  [bias, "a8deb23b8cb5d0a88ffa398c9951ef92a3e47d44b32412dcb40b01895ec4772f"],
]);
for (const [bytes, hash] of expected) if (sha256(bytes) !== hash) throw new Error("fixture SHA-256 mismatch");

const dataOffset = wav.indexOf(Buffer.from("data")) + 8;
const sample = (i) => wav.readInt16LE(dataOffset + i * 2) / 32768;
const hann = (n) => 0.5 - 0.5 * Math.cos(2 * Math.PI * n / 400);

function hzToMel(hz) {
  const fsp = 200 / 3;
  const minLogHz = 1000;
  const minLogMel = minLogHz / fsp;
  const logstep = Math.log(6.4) / 27;
  return hz >= minLogHz ? minLogMel + Math.log(hz / minLogHz) / logstep : hz / fsp;
}
function melToHz(mel) {
  const fsp = 200 / 3;
  const minLogMel = 15;
  const logstep = Math.log(6.4) / 27;
  return mel >= minLogMel ? 1000 * Math.exp(logstep * (mel - minLogMel)) : fsp * mel;
}
const lo = hzToMel(0), hi = hzToMel(8000);
const edges = Array.from({length: 82}, (_, i) => melToHz(lo + (hi - lo) * i / 81));
const fftHz = Array.from({length: 201}, (_, i) => i * 16000 / 400);
const bank = Array.from({length: 80}, (_, m) => {
  const lower = edges[m], center = edges[m + 1], upper = edges[m + 2];
  const norm = 2 / (upper - lower);
  return fftHz.map((hz) => Math.max(0, Math.min((hz - lower) / (center - lower), (upper - hz) / (upper - center))) * norm)
    .map((x) => Number(x.toFixed(8)));
});
function column(start) {
  const frame = Array.from({length: 400}, (_, n) => sample(start + n) * hann(n));
  const powers = Array.from({length: 201}, (_, k) => {
    let re = 0, im = 0;
    for (let n = 0; n < 400; n += 1) {
      const phase = 2 * Math.PI * k * n / 400;
      re += frame[n] * Math.cos(phase);
      im -= frame[n] * Math.sin(phase);
    }
    return re * re + im * im;
  });
  return bank.map((row) => Math.log10(Math.max(1e-10,
    row.reduce((sum, x, k) => sum + x * powers[k], 0))));
}
const tile = [column(4160), column(4320), column(4480)];
const peak = Math.max(...tile.flat());
const normalized = tile.map((col) => col.map((x) => (Math.max(x, peak - 8) + 4) / 4));
const token = Array.from({length: 384}, (_, out) => {
  let value = bias.readFloatLE(out * 4);
  for (let input = 0; input < 80; input += 1) for (let time = 0; time < 3; time += 1) {
    value += weight.readFloatLE(((out * 80 + input) * 3 + time) * 4) * normalized[time][input];
  }
  return value;
});
const positions = [0, 1, 2, 3, 95, 191, 287, 383];
const fingerprint = [
  ...positions.map((i) => token[i]),
  token.reduce((sum, x) => sum + x, 0),
  token.reduce((sum, x) => sum + Math.abs(x), 0),
];
console.log(JSON.stringify({
  implementation: "independent-node-direct-dft-slaney-conv1d",
  source: "Lingua-Libre-M92036254-human-book",
  dataOffset,
  melShape: [3, 80],
  peak,
  weightShape: [384, 80, 3],
  biasShape: [384],
  learnedParameters: 92544,
  operations: {dftBins: 603, convMultiplyAdds: 92160},
  fingerprint,
}, null, 2));
