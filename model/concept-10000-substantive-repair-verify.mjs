#!/usr/bin/env node
// Exact byte/carrier verification for all 111 stable-ID migrations. No network.
import { createHash } from "node:crypto";
import { readFile } from "node:fs/promises";
import { resolve } from "node:path";

const root = resolve(new URL("..", import.meta.url).pathname);
const read = (path, encoding) => readFile(resolve(root, path), encoding);
const sha256 = (bytes) => createHash("sha256").update(bytes).digest("hex");
const migration = (await read("model/concept-10000-substantive-repair-migration.tsv", "utf8"))
  .trimEnd().split("\n").slice(1).map((line) => {
    const f = line.split("\t");
    return { id: Number(f[0]), old: f[1], label: f[2], rank: Number(f[3]), frequency: Number(f[4]),
      page: Number(f[5]), revision: Number(f[6]), timestamp: f[7], section: f[8], sha: f[9], definition: f[10] };
  });
if (migration.length !== 111 || new Set(migration.map((r) => r.id)).size !== 111 ||
    new Set(migration.map((r) => r.label)).size !== 111 || new Set(migration.map((r) => r.old)).size !== 111)
  throw new Error("migration cardinality/uniqueness failed");
if (migration.some((r) => r.rank <= 10000 || r.frequency <= 0 || r.page <= 0 || r.revision <= 0 ||
    r.timestamp.length !== 20 || r.sha.length !== 64 || !r.section || !r.definition))
  throw new Error("migration provenance/meaning field failed");
if (migration.some((r, i) => i && r.rank <= migration[i - 1].rank))
  throw new Error("source ranks are not strictly increasing");

const ranked = await read("model/concept-10000-ranked.dat");
const lexical = await read("model/concept-10000-lexical-index.dat");
const aliases = await read("model/concept-10000-substantive-repair-alias-index.dat");
const semantics = await read("model/concept-10000-substantive-repair-semantic-index.dat");
if (ranked.length !== 300000 || lexical.length !== 260000 || aliases.length !== 2886 || semantics.length !== 150000)
  throw new Error("fixed carrier width failed");
const canonicalAt = (id) => ranked.subarray(id * 30, id * 30 + 20).toString("utf8").replace(/ +$/, "");
if (migration.some((r) => canonicalAt(r.id) !== r.label)) throw new Error("canonical stable-ID projection failed");
const aliasRows = Array.from({ length: 111 }, (_, slot) => ({
  label: aliases.subarray(slot * 26, slot * 26 + 20).toString("utf8").replace(/ +$/, ""),
  id: Number(aliases.subarray(slot * 26 + 21, slot * 26 + 25).toString("ascii")),
}));
const migrationByOld = new Map(migration.map((r) => [r.old, r.id]));
if (aliasRows.some((r) => migrationByOld.get(r.label) !== r.id)) throw new Error("legacy alias route failed");
if (aliasRows.some((r, i) => i && Buffer.compare(Buffer.from(aliasRows[i - 1].label), Buffer.from(r.label)) >= 0))
  throw new Error("alias index is not strictly byte-sorted");
let semanticRows = 0;
for (let id = 0; id < 10000; id++) if (semantics[id * 15] === 49) semanticRows++;
if (semanticRows !== 111 || migration.some((r) => semantics[r.id * 15] !== 49))
  throw new Error("semantic repair coverage failed");

const nlRows = (await read("cognition/concept-10000-substantive-repair-111-nl.tsv", "utf8")).trimEnd().split("\n");
const nlSources = await read("cognition/concept-10000-substantive-repair-111-nl-sources.dat", "utf8");
if (nlRows.length !== 111 || nlRows.some((line) => line.split("\t").length !== 15 || line.split("\t").some((v) => !v)))
  throw new Error("13-NL row shape/nonempty coverage failed");
if (nlSources.length !== 1443 || nlSources !== Array.from({ length: 111 }, () => `F${"G".repeat(12)}`).join(""))
  throw new Error("13-NL provenance matrix failed");

const manifest = (await read("model/concept-10000-substantive-repair-source-manifest.txt", "utf8")).trimEnd().split("\n");
let verifiedFiles = 0;
for (const line of manifest) {
  const [path, size, hash] = line.split("\t");
  if (!size || !hash) continue;
  const bytes = await read(path);
  if (bytes.length !== Number(size) || sha256(bytes) !== hash) throw new Error(`manifest mismatch: ${path}`);
  verifiedFiles++;
}
if (verifiedFiles !== 11) throw new Error(`manifest file count ${verifiedFiles}`);
process.stdout.write("verified: 111 stable IDs, 111 aliases, 111 pinned meanings, 1,443 attributed NL cells, 11 exact file hashes\n");
