#!/usr/bin/env node
// Reindex the committed attributed Tatoeba rows against the canonical primary
// 10k x 13 surface. Source rows/provenance stay fixed; only detector evidence
// and role contracts are recomputed. No network and no Python.
import { createHash } from "node:crypto";
import { readFileSync, writeFileSync } from "node:fs";

const dataPath = "cognition/fixtures/human-corpus-13/tatoeba-human-sentences.tsv";
const manifestPath = "cognition/fixtures/human-corpus-13/ARCHIVES.tsv";
const offsetsPath = "cognition/concept-human-corpus-13-offsets.fk";
const metadataPath = "cognition/concept-human-corpus-13-metadata.fk";
const digest = (value) => createHash("sha256").update(value).digest("hex");
const asciiFold = (text) => text.replace(/[A-Z]/g, (c) => c.toLowerCase());
const wordChar = (char) => {
  if (!char) return false;
  const n = char.charCodeAt(0);
  return (n >= 48 && n <= 57) || (n >= 65 && n <= 90) ||
    (n >= 97 && n <= 122) || n >= 128;
};
function addPattern(nodes, pattern, value) {
  let at = 0;
  for (const char of pattern) {
    if (!nodes[at].next.has(char)) {
      nodes[at].next.set(char, nodes.length);
      nodes.push({ next: new Map(), fail: 0, out: [] });
    }
    at = nodes[at].next.get(char);
  }
  nodes[at].out.push(value);
}
function buildMatcher(labels, unsegmented) {
  const nodes = [{ next: new Map(), fail: 0, out: [] }];
  for (const [surface, ids] of labels) addPattern(nodes, asciiFold(surface), [surface, ids]);
  const queue = [];
  for (const child of nodes[0].next.values()) queue.push(child);
  for (let q = 0; q < queue.length; q++) {
    const at = queue[q];
    for (const [char, child] of nodes[at].next) {
      queue.push(child);
      let fail = nodes[at].fail;
      while (fail && !nodes[fail].next.has(char)) fail = nodes[fail].fail;
      if (nodes[fail].next.has(char)) fail = nodes[fail].next.get(char);
      nodes[child].fail = fail;
      nodes[child].out.push(...nodes[fail].out);
    }
  }
  return (sentence) => {
    const folded = asciiFold(sentence);
    let at = 0, offset = 0;
    const ids = new Set(), surfaces = new Map();
    for (const char of folded) {
      while (at && !nodes[at].next.has(char)) at = nodes[at].fail;
      if (nodes[at].next.has(char)) at = nodes[at].next.get(char);
      offset += char.length;
      for (const [surface, surfaceIds] of nodes[at].out) {
        const width = asciiFold(surface).length;
        const start = offset - width;
        if (!unsegmented && (wordChar(folded[start - 1]) || wordChar(folded[offset]))) continue;
        surfaceIds.forEach((id) => ids.add(id));
        if (!surfaces.has(surface)) surfaces.set(surface, surfaceIds);
      }
    }
    return { ids: [...ids].sort((a, b) => a - b), surfaces };
  };
}

const nl = readFileSync("cognition/concept-nl-semantic-13-omw.tsv", "utf8").trimEnd().split("\n");
const locales = nl.shift().split("\t").slice(1);
const labels = new Map(locales.map((code) => [code, new Map()]));
nl.forEach((line, id) => line.split("\t").slice(1).forEach((surface, lens) => {
  const map = labels.get(locales[lens]);
  if (!map.has(surface)) map.set(surface, []);
  map.get(surface).push(id);
}));
const matchers = new Map(locales.map((code) =>
  [code, buildMatcher(labels.get(code), code === "zh" || code === "ja")]));

const input = readFileSync(dataPath, "utf8").trimEnd().split("\n");
const header = input.shift();
const rows = input.map((line, index) => {
  const fields = line.split("\t");
  if (fields.length !== 17) throw new Error(`snapshot row ${index} has ${fields.length} fields`);
  const found = matchers.get(fields[0])(fields[16]);
  let role = fields[8], domain = fields[9], expected = Number(fields[10]), surface = fields[11];
  const surfaceFor = (id) => [...found.surfaces].find(([, ids]) => ids.includes(id))?.[0] || "";
  const collision = [...found.surfaces].find(([, ids]) => ids.length > 1);
  if (role === "domain" && !found.ids.includes(expected))
    throw new Error(`domain expected id ${expected} absent at row ${index}`);
  if (found.ids.length === 0) {
    role = "negative"; domain = "none"; expected = -1; surface = "";
  } else if (role === "ambiguity" && collision) {
    domain = "collision"; expected = collision[1][0]; surface = collision[0];
  } else if (role !== "domain") {
    role = "open"; domain = "open-lexical"; expected = found.ids[0]; surface = surfaceFor(expected);
  } else {
    surface = surfaceFor(expected);
  }
  fields[8] = role; fields[9] = domain; fields[10] = String(expected); fields[11] = surface;
  fields[12] = String(found.ids.length); fields[13] = found.ids.join(",");
  return { fields, locale: fields[0], role, author: fields[3], ids: found.ids };
});

const snapshot = `${header}\n${rows.map((row) => row.fields.join("\t")).join("\n")}\n`;
const manifest = readFileSync(manifestPath);
const offsets = [Buffer.byteLength(`${header}\n`)];
rows.forEach((row) => offsets.push(offsets.at(-1) + Buffer.byteLength(`${row.fields.join("\t")}\n`)));
const offsetsText = `; concept-human-corpus-13-offsets.fk -- generated byte index over selected source rows.\n` +
  `; witnessed: 2026-07-18 -> ${rows.length} attributed rows indexed against canonical primary NL\n` +
  `; preludes: form/form-stdlib/core.fk\n(do\n  (defn hcnl13-offsets () (list\n` +
  `${offsets.map((n) => `    ${n}`).join("\n")}))\n)\n`;
const count = (role) => rows.filter((row) => row.role === role).length;
const unique = new Set(rows.flatMap((row) => row.ids));
const stats = locales.map((locale) => {
  const selected = rows.filter((row) => row.locale === locale);
  return [locale, selected.length, ...["domain", "open", "ambiguity", "negative"].map((role) =>
    selected.filter((row) => row.role === role).length), new Set(selected.map((row) => row.author)).size];
});
const metadata = `; concept-human-corpus-13-metadata.fk -- exact bounded public-corpus evidence.\n` +
`; Source: Tatoeba detailed exports, retrieved 2026-07-18; CC BY 2.0 FR.\n` +
`; Human-contributed does not imply native-speaker or human-reviewed; reviewed count is zero.\n` +
`; witnessed: 2026-07-18 -> ${rows.length} source rows, ${unique.size} detected concepts, ${count("negative")} retained negative rows\n` +
`; preludes: form/form-stdlib/core.fk\n(do\n` +
`  (defn hcnl13-row-count () ${rows.length})\n  (defn hcnl13-locale-count () 13)\n` +
`  (defn hcnl13-domain-count () ${count("domain")})\n  (defn hcnl13-open-count () ${count("open")})\n` +
`  (defn hcnl13-ambiguity-count () ${count("ambiguity")})\n  (defn hcnl13-negative-count () ${count("negative")})\n` +
`  (defn hcnl13-detection-count () ${rows.reduce((n, row) => n + row.ids.length, 0)})\n` +
`  (defn hcnl13-unique-concept-count () ${unique.size})\n  (defn hcnl13-attributed-count () ${rows.length})\n` +
`  (defn hcnl13-human-reviewed-count () 0)\n  (defn hcnl13-data-path () "${dataPath}")\n` +
`  (defn hcnl13-manifest-path () "${manifestPath}")\n  (defn hcnl13-data-sha256 () "${digest(snapshot)}")\n` +
`  (defn hcnl13-manifest-sha256 () "${digest(manifest)}")\n` +
`  (defn hcnl13-hash-valid? (path expected)\n    (if (str_eq (host-exec (str_concat "/usr/bin/shasum -a 256 "\n      (str_concat path (str_concat " | /usr/bin/grep -q '^"\n        (str_concat expected "  ' && printf 1")))) "") "1") 1 0))\n` +
`  (defn hcnl13-artifacts-valid? ()\n    (and (eq (hcnl13-hash-valid? (hcnl13-data-path) (hcnl13-data-sha256)) 1)\n         (eq (hcnl13-hash-valid? (hcnl13-manifest-path) (hcnl13-manifest-sha256)) 1)))\n` +
`  (defn hcnl13-locale-stats () (list\n${stats.map((row) => `    (list "${row[0]}" ${row.slice(1).join(" ")})`).join("\n")}))\n)\n`;

writeFileSync(dataPath, snapshot);
writeFileSync(offsetsPath, offsetsText);
writeFileSync(metadataPath, metadata);
process.stdout.write(`${JSON.stringify({ rows: rows.length, detections: rows.reduce((n, row) => n + row.ids.length, 0),
  uniqueConcepts: unique.size, roles: { domain: count("domain"), open: count("open"), ambiguity: count("ambiguity"), negative: count("negative") },
  snapshotSha256: digest(snapshot) })}\n`);
