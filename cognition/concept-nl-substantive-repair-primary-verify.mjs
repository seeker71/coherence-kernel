#!/usr/bin/env node
// Exact no-network verification that the substantive repair is the primary
// 13-NL truth surface, while every non-repaired cell remains byte-stable.
import { createHash } from "node:crypto";
import { readFileSync } from "node:fs";

const sha = (value) => createHash("sha256").update(value).digest("hex");
const table = readFileSync("cognition/concept-nl-semantic-13-omw.tsv", "utf8");
const rows = table.trimEnd().split("\n").slice(1).map((line) => line.split("\t"));
const sources = readFileSync("cognition/concept-nl-semantic-13-sources.dat");
const overlayText = readFileSync("cognition/concept-10000-substantive-repair-111-nl.tsv", "utf8");
const overlay = overlayText.trimEnd().split("\n").map((line) => line.split("\t"));
const overlaySources = readFileSync("cognition/concept-10000-substantive-repair-111-nl-sources.dat");
const ranked = readFileSync("model/concept-10000-ranked.dat");
if (rows.length !== 10000 || sources.length !== 130000 || overlay.length !== 111)
  throw new Error("carrier shape mismatch");

const repaired = new Set(overlay.map((row) => Number(row[0])));
let repairedCells = 0;
overlay.forEach((repair, slot) => {
  const id = Number(repair[0]);
  if (rows[id].join("\t") !== repair.slice(1).join("\t"))
    throw new Error(`primary/repair mismatch at id ${id}`);
  for (let lens = 0; lens < 13; lens++) {
    if (sources[id * 13 + lens] !== overlaySources[slot * 13 + lens])
      throw new Error(`source mismatch at id ${id}, lens ${lens}`);
    repairedCells++;
  }
});

for (let id = 0; id < 10000; id++) {
  const rankedLabel = ranked.subarray(id * 30, id * 30 + 20).toString("utf8").replace(/ +$/, "");
  if (rows[id][1] !== rankedLabel) throw new Error(`primary English/ranked mismatch at id ${id}`);
}

const unaffectedHash = createHash("sha256");
let unaffectedCells = 0;
for (let id = 0; id < 10000; id++) if (!repaired.has(id)) {
  unaffectedHash.update(`${id}\t${rows[id].join("\t")}\t`);
  unaffectedHash.update(sources.subarray(id * 13, id * 13 + 13));
  unaffectedHash.update("\n");
  unaffectedCells += 13;
}
const stableUnaffected = unaffectedHash.digest("hex");
if (stableUnaffected !== "14443ca02c45549f845facc53b2f5d2bed09e382c0cd791afce13f1d89bba85c")
  throw new Error(`unaffected carrier drift: ${stableUnaffected}`);

const examples = new Map(overlay.map((row) => [Number(row[0]), row]));
for (const [id, en, field, label] of [
  [104, "welcoming", 3, "menyambut"],
  [5019, "damp", 2, "damp"],
  [9965, "plum", 11, "梅"],
]) {
  const row = examples.get(id);
  if (!row || row[2] !== en || row[field] !== label)
    throw new Error(`canonical example mismatch at id ${id}`);
}

const metadata = readFileSync("cognition/concept-nl-semantic-13-metadata.fk", "utf8");
if (!metadata.includes(sha(table)) || !metadata.includes(sha(sources)) ||
    !metadata.includes(sha(overlayText)) || !metadata.includes(sha(overlaySources)))
  throw new Error("primary metadata does not bind all canonical carriers");
const manifest = readFileSync("cognition/concept-nl-semantic-13-source-manifest.txt", "utf8");
for (const [bytes, path] of [[Buffer.from(table), "concept-nl-semantic-13-omw.tsv"],
  [sources, "concept-nl-semantic-13-sources.dat"], [ranked, "model/concept-10000-ranked.dat"]])
  if (!manifest.includes(`${sha(bytes)}  ${path}`)) throw new Error(`manifest mismatch: ${path}`);

process.stdout.write(`verified primary canonical surface: ${unaffectedCells} unchanged + ${repairedCells} repaired = ${unaffectedCells + repairedCells} cells; 10000/10000 English labels aligned; examples welcoming/damp/plum\n`);
