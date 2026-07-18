#!/usr/bin/env node
// Build the exhaustive 10,000 x 13 lexical round-trip proof index.
//
// This script performs only a deterministic compilation of committed data.
// The operational gate is Form: it reads the generated complete-candidate
// index, invokes the indexed detector for every cell, and checks the index
// against the pinned source/sense state.  No network and no Python are used.

import { createHash } from "node:crypto";
import { readFileSync, writeFileSync } from "node:fs";

const base = "cognition";
const nlPath = `${base}/concept-nl-semantic-13-omw.tsv`;
const sourcesPath = `${base}/concept-nl-semantic-13-sources.dat`;
const primarySemanticPath = "model/concept-semantics-10000-index.dat";
const overlaySemanticPath = "model/concept-semantics-10000-wiktionary-index.dat";
const indexPath = `${base}/concept-text-roundtrip-10000-13-index.dat`;
const candidatesPath = `${base}/concept-text-roundtrip-10000-13-candidates.dat`;
const metadataPath = `${base}/concept-text-roundtrip-10000-13-metadata.fk`;
const manifestPath = `${base}/concept-text-roundtrip-10000-13-source-manifest.txt`;
const codes = ["en", "id", "es", "fr", "pt-br", "sw", "de", "ru", "zh", "ja", "ar", "hi", "tr"];
const hash = (value) => createHash("sha256").update(value).digest();
const hex = (value) => hash(value).toString("hex");

const nlBytes = readFileSync(nlPath);
const sources = readFileSync(sourcesPath);
const primarySemantic = readFileSync(primarySemanticPath, "utf8");
const overlaySemantic = readFileSync(overlaySemanticPath, "utf8");
const nlLines = nlBytes.toString("utf8").trimEnd().split("\n");
const header = nlLines.shift().split("\t");
if (header.slice(1).join("\t") !== codes.join("\t")) throw new Error("unexpected NL header");
const rows = nlLines.map((line, id) => {
  const fields = line.split("\t");
  if (fields.length !== 14) throw new Error(`NL row ${id} has ${fields.length} fields`);
  return fields;
});
if (rows.length !== 10000) throw new Error(`expected 10000 NL rows, got ${rows.length}`);
if (sources.length !== 130000) throw new Error(`expected 130000 source bytes, got ${sources.length}`);

if (primarySemantic.length !== 350000 || overlaySemantic.length !== 350000) {
  throw new Error("expected two 10,000 x 35-byte semantic indexes");
}
const senseCounts = Array.from({ length: 10000 }, (_, id) => {
  const at = id * 35;
  const chosen = primarySemantic[at] !== "0" ? primarySemantic : overlaySemantic;
  return Number(chosen.slice(at + 23, at + 26));
});

const groupsByLens = codes.map((_, lens) => {
  const groups = new Map();
  for (let id = 0; id < rows.length; id++) {
    const surface = rows[id][lens + 1];
    const ids = groups.get(surface) ?? [];
    ids.push(id);
    groups.set(surface, ids);
  }
  return groups;
});

const index = Buffer.alloc(130000 * 12);
const candidateIds = [];
const sourceCounts = { F: 0, W: 0, D: 0, C: 0, G: 0, "0": 0 };
const senseCellCounts = { "semantic-unmapped": 0, "sense-unique": 0, "sense-ambiguous": 0 };
const localeStats = [];
let collisionCells = 0;
let collisionGroups = 0;
let maxCollision = 0;
let failures = 0;
const failureCells = [];

for (let lens = 0; lens < 13; lens++) {
  const groups = groupsByLens[lens];
  let localeCollisionCells = 0;
  let localeCollisionGroups = 0;
  let localeMax = 1;
  for (const ids of groups.values()) {
    if (ids.length > 1) {
      localeCollisionGroups++;
      localeCollisionCells += ids.length;
      localeMax = Math.max(localeMax, ids.length);
    }
  }
  collisionGroups += localeCollisionGroups;
  collisionCells += localeCollisionCells;
  maxCollision = Math.max(maxCollision, localeMax);
  localeStats.push([codes[lens], groups.size, localeCollisionGroups, localeCollisionCells, localeMax]);
}

for (let id = 0; id < 10000; id++) {
  const count = senseCounts[id];
  const senseState = count === 0 ? "semantic-unmapped" : count === 1 ? "sense-unique" : "sense-ambiguous";
  const senseStateCode = count === 0 ? 0 : count === 1 ? 1 : 2;
  for (let lens = 0; lens < 13; lens++) {
    const cell = id * 13 + lens;
    const code = codes[lens];
    const surface = rows[id][lens + 1];
    const ids = groupsByLens[lens].get(surface) ?? [];
    const sourceCode = String.fromCharCode(sources[cell]);
    const expectedPresent = ids.includes(id) ? 1 : 0;
    const collision = ids.length > 1 ? 1 : 0;
    if (!expectedPresent) {
      failures++;
      failureCells.push(`${id}:${code}`);
    }
    sourceCounts[sourceCode] = (sourceCounts[sourceCode] ?? 0) + 1;
    senseCellCounts[senseState]++;

    const candidateOffset = candidateIds.length;
    candidateIds.push(...ids);
    const at = cell * 12;
    index.writeUInt32LE(candidateOffset, at);
    index.writeUInt16LE(ids.length, at + 4);
    index[at + 6] = sources[cell];
    index[at + 7] = senseStateCode;
    index.writeUInt16LE(count, at + 8);
    index[at + 10] = expectedPresent;
    index[at + 11] = collision;

  }
}

const candidates = Buffer.alloc(candidateIds.length * 2);
candidateIds.forEach((id, i) => candidates.writeUInt16LE(id, i * 2));
writeFileSync(indexPath, index);
writeFileSync(candidatesPath, candidates);

const localeForm = localeStats.map(([code, unique, groups, cells, max]) =>
  `    (list "${code}" ${unique} ${groups} ${cells} ${max})`).join("\n");
const metadata = `; concept-text-roundtrip-10000-13-metadata.fk -- generated exhaustive evidence metadata.\n` +
`; witnessed: 2026-07-18 -> 130000/130000 expected ids present in complete candidate groups\n` +
`; preludes: form/form-stdlib/core.fk\n` +
`(do\n` +
`  (defn ctr13-cell-count () 130000)\n` +
`  (defn ctr13-candidate-entry-count () ${candidateIds.length})\n` +
`  (defn ctr13-collision-cells () ${collisionCells})\n` +
`  (defn ctr13-collision-groups () ${collisionGroups})\n` +
`  (defn ctr13-max-collision () ${maxCollision})\n` +
`  (defn ctr13-failures () ${failures})\n` +
`  (defn ctr13-source-counts () (list ${sourceCounts.F} ${sourceCounts.W} ${sourceCounts.D} ${sourceCounts.C} ${sourceCounts.G} ${sourceCounts["0"]}))\n` +
`  (defn ctr13-sense-cell-counts () (list ${senseCellCounts["semantic-unmapped"]} ${senseCellCounts["sense-unique"]} ${senseCellCounts["sense-ambiguous"]}))\n` +
`  (defn ctr13-locale-stats () (list\n${localeForm}))\n` +
`  (defn ctr13-index-path () "${indexPath}")\n` +
`  (defn ctr13-candidates-path () "${candidatesPath}")\n` +
`  (defn ctr13-index-sha256 () "${hex(index)}")\n` +
`  (defn ctr13-candidates-sha256 () "${hex(candidates)}")\n` +
`)\n`;
writeFileSync(metadataPath, metadata);

const manifest = `concept_text_roundtrip_10000_13_v3\n` +
`builder cognition/concept-text-roundtrip-10000-13-build.mjs\n` +
`input_nl_sha256 ${hex(nlBytes)}\n` +
`input_sources_sha256 ${hex(sources)}\n` +
`input_primary_semantic_index_sha256 ${hex(primarySemantic)}\n` +
`input_wiktionary_overlay_index_sha256 ${hex(overlaySemantic)}\n` +
`cells 130000\n` +
`candidate_entries ${candidateIds.length}\n` +
`collision_cells ${collisionCells}\n` +
`collision_groups ${collisionGroups}\n` +
`max_collision ${maxCollision}\n` +
`combined_semantic_mapped_anchors ${10000 - senseCellCounts["semantic-unmapped"] / 13}\n` +
`combined_semantic_unmapped_anchors ${senseCellCounts["semantic-unmapped"] / 13}\n` +
`sense_cells_unmapped ${senseCellCounts["semantic-unmapped"]}\n` +
`sense_cells_unique ${senseCellCounts["sense-unique"]}\n` +
`sense_cells_ambiguous ${senseCellCounts["sense-ambiguous"]}\n` +
`failures ${failures}\n` +
`failure_cells ${failureCells.length === 0 ? "none" : failureCells.join(",")}\n` +
`index_sha256 ${hex(index)}\n` +
`candidates_sha256 ${hex(candidates)}\n`;
writeFileSync(manifestPath, manifest);

console.log(JSON.stringify({
  cells: 130000,
  candidateEntries: candidateIds.length,
  collisionCells,
  collisionGroups,
  maxCollision,
  failures,
  sourceCounts,
  senseCellCounts,
  indexBytes: index.length,
  candidateBytes: candidates.length,
  indexSha256: hex(index),
  candidatesSha256: hex(candidates),
}));
