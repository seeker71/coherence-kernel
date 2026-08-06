#!/usr/bin/env node
// Independent JavaScript reference for the Form learned-patch live gate.
// No PyTorch or Python: decode one real frame per scene with ffmpeg, parse BMP
// and the pinned checkpoint records directly, then evaluate Conv2d in f64 over
// the released f32 coefficients in the same [out,in,y,x] order.

import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import crypto from "node:crypto";
import { execFileSync } from "node:child_process";

const root = "model/fixtures/lingbot-map";
const weightRecord = fs.readFileSync(`${root}/checkpoint-data7-record.bin`);
const biasRecord = fs.readFileSync(`${root}/checkpoint-data8-record.bin`);

function sha256(bytes) {
  return crypto.createHash("sha256").update(bytes).digest("hex");
}
if (sha256(weightRecord) !== "2c07e9f1d118d54358dc10eb56b16b8d4b81f3f0da11b2712133f1b8d1b54880") {
  throw new Error("released patch weight record SHA-256 mismatch");
}
if (sha256(biasRecord) !== "1bc851cacd9e6532372dafd9b4a3195ade615843e5c4d5d5bd6fd4bd33df94fe") {
  throw new Error("released patch bias record SHA-256 mismatch");
}

function payload(record, size) {
  if (record.readUInt32LE(0) !== 0x04034b50) throw new Error("not a ZIP local record");
  const nameLength = record.readUInt16LE(26);
  const extraLength = record.readUInt16LE(28);
  const start = 30 + nameLength + extraLength;
  return record.subarray(start, start + size);
}
const weight = payload(weightRecord, 2_408_448);
const bias = payload(biasRecord, 4_096);
if (weight.length !== 2_408_448 || bias.length !== 4_096) {
  throw new Error("released tensor payload length mismatch");
}

const temporary = fs.mkdtempSync(path.join(os.tmpdir(), "lingbot-learned-reference-"));
const scenes = [
  ["loop", 294],
  ["oxford", 392],
  ["university", 294],
  ["courthouse", 294],
];

function decode(name) {
  const source = `${root}/real-life/${name}-24f.mkv`;
  const output = path.join(temporary, `${name}.bmp`);
  execFileSync("ffmpeg", ["-loglevel", "error", "-y", "-i", source, "-frames:v", "1", "-c:v", "bmp", output]);
  return output;
}

function infer(file) {
  const bmp = fs.readFileSync(file);
  const dataOffset = bmp.readUInt32LE(10);
  const width = bmp.readInt32LE(18);
  const signedHeight = bmp.readInt32LE(22);
  const height = Math.abs(signedHeight);
  const rowStride = 4 * Math.floor((3 * width + 3) / 4);
  const patchX = Math.floor(Math.floor(width / 14) / 2);
  const patchY = Math.floor(Math.floor(height / 14) / 2);
  const x0 = patchX * 14;
  const y0 = patchY * 14;
  const mean = [0.485, 0.456, 0.406];
  const std = [0.229, 0.224, 0.225];

  function rgb(x, y) {
    const storageY = signedHeight < 0 ? y : height - 1 - y;
    const i = dataOffset + storageY * rowStride + x * 3;
    return [bmp[i + 2], bmp[i + 1], bmp[i]];
  }

  const token = [];
  for (let outChannel = 0; outChannel < 1024; outChannel += 1) {
    let value = bias.readFloatLE(outChannel * 4);
    for (let i = 0; i < 588; i += 1) {
      const inChannel = Math.floor(i / 196);
      const pixelOffset = i % 196;
      const x = pixelOffset % 14;
      const y = Math.floor(pixelOffset / 14);
      const input = (rgb(x0 + x, y0 + y)[inChannel] / 255 - mean[inChannel]) / std[inChannel];
      value += weight.readFloatLE((outChannel * 588 + i) * 4) * input;
    }
    token.push(value);
  }
  const positions = [0, 1, 2, 3, 255, 511, 767, 1023];
  return {
    width,
    height,
    patchX,
    patchY,
    fingerprint: [...positions.map((i) => token[i]), token.reduce((sum, x) => sum + x, 0)],
    token,
  };
}

const rows = scenes.map(([name, expectedHeight]) => {
  const row = infer(decode(name));
  if (row.width !== 518 || row.height !== expectedHeight) throw new Error(`${name} frame dimensions changed`);
  return { name, ...row };
});
const base = rows[0].token;
const distances = rows.slice(1).map((row) =>
  row.token.reduce((sum, x, i) => sum + Math.abs(x - base[i]), 0));

console.log(JSON.stringify({
  implementation: "independent-node-ieee-f32-bmp-conv2d",
  learnedCoefficients: 602_112,
  outputChannels: 1024,
  rows: rows.map(({ name, width, height, patchX, patchY, fingerprint }) =>
    ({ name, width, height, patchX, patchY, fingerprint })),
  distances,
}, null, 2));
