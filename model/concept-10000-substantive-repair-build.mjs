#!/usr/bin/env node
// Replace the 111 non-lexical OpenSubtitles debris slots with the next-ranked
// source entries that have a revision-pinned substantive English Wiktionary
// definition. Stable IDs remain fixed and every displaced surface is retained
// in a dedicated alias index. No Python and no runtime-seed participation.

import { createHash } from "node:crypto";
import { readFile, writeFile } from "node:fs/promises";
import { resolve } from "node:path";
import { spawnSync } from "node:child_process";

const root = resolve(new URL("..", import.meta.url).pathname);
const sourceUrl = "https://raw.githubusercontent.com/hermitdave/FrequencyWords/525f9b560de45753a5ea01069454e72e9aa541c6/content/2018/en/en_50k.txt";
const sourceSha = "5351ff405b1126ef555791dd4d9798a48e3e9a501a9fc481a9da957752cfb458";
const wikiApi = "https://en.wiktionary.org/w/api.php";
const translateApi = "https://translate.googleapis.com/translate_a/single";
const userAgent = "coherence-kernel/1.0 (https://github.com/seeker71/coherence-kernel; stable substantive concept repair)";
const refresh = process.argv.includes("--refresh");

const rankedPath = resolve(root, "model/concept-10000-ranked.dat");
const lexicalIndexPath = resolve(root, "model/concept-10000-lexical-index.dat");
const holesPath = resolve(root, "model/concept-semantics-10000-wiktionary-lexical-remaining.tsv");
const evidencePath = resolve(root, "model/concept-10000-substantive-repair-evidence.jsonl");
const migrationPath = resolve(root, "model/concept-10000-substantive-repair-migration.tsv");
const aliasPath = resolve(root, "model/concept-10000-substantive-repair-alias-index.dat");
const semanticIndexPath = resolve(root, "model/concept-10000-substantive-repair-semantic-index.dat");
const semanticPayloadPath = resolve(root, "model/concept-10000-substantive-repair-semantic-payload.dat");
const nlPath = resolve(root, "cognition/concept-10000-substantive-repair-111-nl.tsv");
const nlSourcePath = resolve(root, "cognition/concept-10000-substantive-repair-111-nl-sources.dat");
const dataFormPath = resolve(root, "model/concept-10000-substantive-repair-data.fk");
const nlFormPath = resolve(root, "cognition/concept-10000-substantive-repair-111-nl-data.fk");
const manifestPath = resolve(root, "model/concept-10000-substantive-repair-source-manifest.txt");

const locales = [
  ["en", "en"], ["id", "id"], ["es", "es"], ["fr", "fr"],
  ["pt-br", "pt"], ["sw", "sw"], ["de", "de"], ["ru", "ru"],
  ["zh", "zh-CN"], ["ja", "ja"], ["ar", "ar"], ["hi", "hi"], ["tr", "tr"],
];
const allowedSections = new Set([
  "noun", "verb", "adjective", "adverb", "interjection", "preposition",
  "determiner", "conjunction", "pronoun", "numeral", "particle",
]);
const inflectionOnly = /\{\{\s*(?:en-)?(?:third-person|plural of|past of|simple past|present participle|comparative of|superlative of|alternative form of|alternative spelling of|misspelling of|obsolete spelling of|inflection of|form of)\b/i;

function sha256(bytes) {
  return createHash("sha256").update(bytes).digest("hex");
}

function sleep(ms) {
  return new Promise((done) => setTimeout(done, ms));
}

async function fetchRetry(url, options = {}, attempts = 7) {
  let last;
  for (let attempt = 0; attempt < attempts; attempt++) {
    try {
      const response = await fetch(url, { ...options, headers: { "User-Agent": userAgent, ...(options.headers || {}) } });
      if (!response.ok) throw new Error(`HTTP ${response.status}: ${url}`);
      return response;
    } catch (error) {
      last = error;
      await sleep(350 * (attempt + 1));
    }
  }
  throw last;
}

async function wikiQuery(parameters) {
  const url = new URL(wikiApi);
  for (const [key, value] of Object.entries(parameters)) url.searchParams.set(key, value);
  const body = await (await fetchRetry(url)).json();
  if (!body.query) throw new Error(`Wiktionary response lacks query: ${JSON.stringify(body.error || {})}`);
  return body.query;
}

function englishSection(wikitext) {
  const match = wikitext.match(/(?:^|\n)==English==\s*\n([\s\S]*?)(?=\n==[^=]|$)/);
  return match ? match[1] : "";
}

function firstSubstantiveDefinition(english) {
  let section = "";
  for (const line of english.split("\n")) {
    const heading = line.match(/^(={3,6})\s*([^=]+?)\s*\1\s*$/);
    if (heading) {
      const candidate = heading[2].trim();
      section = allowedSections.has(candidate.toLowerCase()) ? candidate : "";
      continue;
    }
    const definition = line.match(/^#(?![#*:])\s*(.+?)\s*$/);
    if (!section || !definition) continue;
    const source = definition[1];
    if (inflectionOnly.test(source)) continue;
    if (source.replace(/\{\{[^}]+\}\}|\[\[|\]\]|[\s.'"(),;:!?-]/g, "").length < 4) continue;
    return { section, definition: source };
  }
  return null;
}

function sourceRows(bytes) {
  return bytes.toString("utf8").trimEnd().split("\n").map((line, index) => {
    const split = line.lastIndexOf(" ");
    return { label: line.slice(0, split), frequency: Number(line.slice(split + 1)), sourceRank: index + 1 };
  });
}

function rankedRows(bytes) {
  if (bytes.length !== 300000) throw new Error(`ranked carrier must be 300000 bytes, got ${bytes.length}`);
  return Array.from({ length: 10000 }, (_, id) => ({
    id,
    label: bytes.subarray(id * 30, id * 30 + 20).toString("utf8").replace(/ +$/, ""),
    frequency: Number(bytes.subarray(id * 30 + 21, id * 30 + 29).toString("ascii")),
  }));
}

function holeRows(text) {
  const rows = text.trimEnd().split("\n").slice(1).map((line) => {
    const [id, legacyLabel] = line.split("\t");
    return { id: Number(id), legacyLabel };
  });
  if (rows.length !== 111 || new Set(rows.map((row) => row.id)).size !== 111)
    throw new Error(`expected 111 distinct repair IDs, got ${rows.length}`);
  return rows;
}

function candidateSyntax(label) {
  return Buffer.byteLength(label) <= 20 && /^[a-z]+(?:-[a-z]+)*$/.test(label);
}

async function selectEvidence(holeList, currentRows) {
  const sourceBytes = Buffer.from(await (await fetchRetry(sourceUrl)).arrayBuffer());
  if (sha256(sourceBytes) !== sourceSha) throw new Error("pinned frequency source SHA-256 mismatch");
  const rights = await wikiQuery({ action: "query", format: "json", formatversion: "2", meta: "siteinfo", siprop: "rightsinfo" });
  const existing = new Set(currentRows.map((row) => row.label));
  const candidates = sourceRows(sourceBytes).slice(10000).filter((row) => candidateSyntax(row.label) && !existing.has(row.label));
  const selected = [];
  let inspected = 0;
  for (let at = 0; selected.length < 111 && at < candidates.length; at += 25) {
    const batch = candidates.slice(at, at + 25);
    const query = await wikiQuery({
      action: "query", format: "json", formatversion: "2", prop: "revisions",
      rvprop: "ids|timestamp|content", rvslots: "main", titles: batch.map((row) => row.label).join("|"),
    });
    const pages = new Map((query.pages || []).map((page) => [page.title.toLowerCase(), page]));
    for (const candidate of batch) {
      inspected++;
      const page = pages.get(candidate.label);
      if (!page || page.missing || page.title !== candidate.label) continue;
      const revision = page.revisions?.[0];
      const english = englishSection(revision?.slots?.main?.content || "");
      const meaning = firstSubstantiveDefinition(english);
      if (!meaning) continue;
      const hole = holeList[selected.length];
      selected.push({
        ...hole, canonicalLabel: candidate.label, sourceRank: candidate.sourceRank,
        frequency: candidate.frequency, pageId: page.pageid, revisionId: revision.revid,
        timestamp: revision.timestamp, englishSha256: sha256(english),
        section: meaning.section, definition: meaning.definition,
      });
      if (selected.length === 111) break;
    }
    process.stderr.write(`substantive selection: ${selected.length}/111 (${inspected} ranked candidates inspected)\n`);
  }
  if (selected.length !== 111) throw new Error(`only ${selected.length} substantive candidates selected`);
  const meta = {
    schema: "coherence-kernel.substantive-concept-repair.v1", sourceUrl, sourceSha256: sourceSha,
    sourceStartRank: 10001, selectionLaw: "next source-ranked unique lowercase alphabetic/hyphen entry <=20 UTF-8 bytes with exact lowercase Wiktionary title and first non-inflection substantive common-POS English definition; proper nouns and form-only definitions excluded",
    wiktionaryApi: wikiApi, rightsText: rights.rightsinfo?.text || "", rightsUrl: rights.rightsinfo?.url || "",
    inspectedCandidates: inspected, selected: selected.length,
  };
  await writeFile(evidencePath, [JSON.stringify({ _meta: meta }), ...selected.map((row) => JSON.stringify(row))].join("\n") + "\n");
  return { meta, rows: selected };
}

async function translateChunk(values, target, attempt = 0) {
  const url = new URL(translateApi);
  url.search = new URLSearchParams({ client: "gtx", sl: "en", tl: target, dt: "t", q: values.join("\n") });
  const payload = await (await fetchRetry(url)).json();
  const text = payload[0].map((segment) => segment[0] || "").join("").replace(/\r/g, "");
  const translated = text.split("\n");
  if (translated.length === values.length) return translated;
  if (values.length === 1) throw new Error(`${target}: translation row cardinality ${translated.length}`);
  const middle = Math.floor(values.length / 2);
  return [...await translateChunk(values.slice(0, middle), target, attempt + 1),
          ...await translateChunk(values.slice(middle), target, attempt + 1)];
}

async function buildTranslations(rows) {
  const columns = [rows.map((row) => row.canonicalLabel)];
  for (const [, target] of locales.slice(1)) {
    const column = [];
    for (let at = 0; at < rows.length; at += 50)
      column.push(...await translateChunk(rows.slice(at, at + 50).map((row) => row.canonicalLabel), target));
    if (column.length !== 111 || column.some((value) => value.trim().length === 0))
      throw new Error(`${target}: translation coverage incomplete`);
    columns.push(column.map((value) => value.replace(/[\t\r\n]+/g, " ")));
    process.stderr.write(`translated substantive repair: ${target} 111/111\n`);
  }
  const lines = rows.map((row, index) => [
    row.id, `wiktionary:${row.pageId}:${row.revisionId}`,
    ...columns.map((column) => column[index]),
  ].join("\t"));
  await writeFile(nlPath, lines.join("\n") + "\n");
  await writeFile(nlSourcePath, rows.map(() => `F${"G".repeat(12)}`).join(""));
}

async function readEvidence() {
  const lines = (await readFile(evidencePath, "utf8")).trimEnd().split("\n").map(JSON.parse);
  if (lines.length !== 112) throw new Error(`repair evidence cardinality is ${lines.length - 1}, expected 111`);
  return { meta: lines[0]._meta, rows: lines.slice(1) };
}

function fixed(value, width, name) {
  const bytes = Buffer.byteLength(value);
  if (bytes > width) throw new Error(`${name} exceeds ${width} bytes: ${value}`);
  return value + " ".repeat(width - bytes);
}

function decimal(value, width, name) {
  const out = String(value).padStart(width, "0");
  if (out.length !== width) throw new Error(`${name} exceeds ${width} digits: ${value}`);
  return out;
}

function lengthField(value, width, name) {
  return decimal(Buffer.byteLength(value), width, `${name} length`) + value;
}

function semanticPayload(row) {
  return decimal(row.pageId, 10, "page") + decimal(row.revisionId, 10, "revision") +
    fixed(row.timestamp, 20, "timestamp") + row.englishSha256 +
    lengthField("exact", 2, "lookup") + lengthField(row.canonicalLabel, 2, "title") +
    lengthField("accepted-substantive-repair", 2, "status") + lengthField(row.section, 2, "section") +
    lengthField(row.definition, 5, "definition") + "001" + lengthField("stable-id-repair", 2, "morphology status");
}

async function buildCarriers(evidence, currentRows) {
  const byId = new Map(evidence.rows.map((row) => [row.id, row]));
  if (byId.size !== 111) throw new Error("repair IDs are not unique");
  const canonical = currentRows.map((row) => {
    const repair = byId.get(row.id);
    return repair ? { ...row, label: repair.canonicalLabel, frequency: repair.frequency } : row;
  });
  if (new Set(canonical.map((row) => row.label)).size !== 10000) throw new Error("canonical labels are not unique after repair");
  const ranked = canonical.map((row) => `${fixed(row.label, 20, "label")}|${decimal(row.frequency, 8, "frequency")};`).join("");
  const lexical = [...canonical].sort((a, b) => Buffer.compare(Buffer.from(a.label), Buffer.from(b.label)))
    .map((row) => `${fixed(row.label, 20, "index label")}|${decimal(row.id, 4, "id")};`).join("");
  const aliases = [...evidence.rows].sort((a, b) => Buffer.compare(Buffer.from(a.legacyLabel), Buffer.from(b.legacyLabel)))
    .map((row) => `${fixed(row.legacyLabel, 20, "alias")}|${decimal(row.id, 4, "alias id")};`).join("");
  if (Buffer.byteLength(ranked) !== 300000 || Buffer.byteLength(lexical) !== 260000 || Buffer.byteLength(aliases) !== 2886)
    throw new Error("fixed carrier width invariant failed");
  await writeFile(rankedPath, ranked);
  await writeFile(lexicalIndexPath, lexical);
  await writeFile(aliasPath, aliases);

  let payload = "";
  let index = "";
  for (let id = 0; id < 10000; id++) {
    const row = byId.get(id);
    if (!row) { index += "0".repeat(15); continue; }
    const encoded = semanticPayload(row);
    index += "11" + decimal(Buffer.byteLength(payload), 8, "payload offset") + decimal(Buffer.byteLength(encoded), 5, "payload length");
    payload += encoded;
  }
  await writeFile(semanticIndexPath, index);
  await writeFile(semanticPayloadPath, payload);

  const migrationHeader = "stable_id\tlegacy_alias\tcanonical_label\tupstream_rank\tfrequency\twiktionary_page\twiktionary_revision\ttimestamp\tsection\tenglish_section_sha256\tdefinition";
  const migrationRows = evidence.rows.map((row) => [row.id, row.legacyLabel, row.canonicalLabel, row.sourceRank,
    row.frequency, row.pageId, row.revisionId, row.timestamp, row.section, row.englishSha256,
    row.definition.replace(/[\t\r\n]+/g, " ")].join("\t"));
  await writeFile(migrationPath, [migrationHeader, ...migrationRows].join("\n") + "\n");
}

async function buildForms(evidence) {
  const ids = evidence.rows.map((row) => row.id).join(" ");
  const ranks = evidence.rows.map((row) => row.sourceRank).join(" ");
  const rankCases = evidence.rows.reduceRight((tail, row) =>
    `(if (eq id ${row.id}) ${row.sourceRank} ${tail})`, "fallback");
  const idCases = evidence.rows.reduceRight((tail, row) =>
    `(if (eq id ${row.id}) 1 ${tail})`, "0");
  const form = `; concept-10000-substantive-repair-data.fk — stable-ID repair carriers.\n; Generated from revision-pinned frequency and Wiktionary evidence; do not hand edit.\n; witnessed: 2026-07-20 -> 111/111 substantive replacements; legacy surfaces retained only in migration evidence and rejected by semantic detection\n; preludes: form/form-stdlib/core.fk\n(do\n  (defn csr111-count () 111)\n  (defn csr111-alias-index-path () "model/concept-10000-substantive-repair-alias-index.dat")\n  (defn csr111-semantic-index-path () "model/concept-10000-substantive-repair-semantic-index.dat")\n  (defn csr111-semantic-payload-path () "model/concept-10000-substantive-repair-semantic-payload.dat")\n  (defn csr111-migration-path () "model/concept-10000-substantive-repair-migration.tsv")\n  (defn csr111-evidence-path () "model/concept-10000-substantive-repair-evidence.jsonl")\n  (defn csr111-ids () (list ${ids}))\n  (defn csr111-source-ranks () (list ${ranks}))\n  (defn csr111-rank-go (ids ranks id fallback)\n    (if (eq (len ids) 0) fallback\n      (if (eq (head ids) id) (head ranks)\n        (csr111-rank-go (tail ids) (tail ranks) id fallback))))\n  (defn csr111-effective-rank (id fallback)\n    (csr111-rank-go (csr111-ids) (csr111-source-ranks) id fallback))\n  (defn csr111-repaired-id-go? (ids id)\n    (if (eq (len ids) 0) 0\n      (if (eq (head ids) id) 1 (csr111-repaired-id-go? (tail ids) id))))\n  (defn csr111-repaired-id? (id) (csr111-repaired-id-go? (csr111-ids) id))\n  0)\n`;
  const optimizedForm = form.replace(
    /  \(defn csr111-rank-go[\s\S]*?  \(defn csr111-repaired-id\? \(id\)[^\n]+\n/,
    `  ; Constant decision ladders avoid list allocation on ordinary lookup.\n` +
    `  (defn csr111-effective-rank (id fallback) ${rankCases})\n` +
    `  (defn csr111-repaired-id? (id) ${idCases})\n`);
  if (optimizedForm === form) throw new Error("failed to optimize generated rank lookup");
  await writeFile(dataFormPath, optimizedForm);

  const nl = await readFile(nlPath);
  const offsets = [0];
  for (let i = 0; i < nl.length; i++) if (nl[i] === 10) offsets.push(i + 1);
  if (offsets.length !== 112) throw new Error(`NL offset cardinality ${offsets.length}, expected 112`);
  const nlForm = `; concept-10000-substantive-repair-111-nl-data.fk — generated 111 x 13 overlay.\n; English is frequency source F; twelve translations are attributed Google carrier G, unreviewed.\n; witnessed: 2026-07-18 -> 1,443/1,443 non-empty attributed cells\n; preludes: form/form-stdlib/core.fk\n(do\n  (defn csr111-nl-data-path () "cognition/concept-10000-substantive-repair-111-nl.tsv")\n  (defn csr111-nl-sources-path () "cognition/concept-10000-substantive-repair-111-nl-sources.dat")\n  (defn csr111-nl-offsets () (list ${offsets.join(" ")}))\n  0)\n`;
  await writeFile(nlFormPath, nlForm);
}

async function writeManifest(evidence) {
  const files = [rankedPath, lexicalIndexPath, aliasPath, evidencePath, migrationPath,
    semanticIndexPath, semanticPayloadPath, nlPath, nlSourcePath, dataFormPath, nlFormPath];
  const lines = [
    "schema=coherence-kernel.substantive-concept-repair-manifest.v1",
    `frequency_source_url=${sourceUrl}`,
    `frequency_source_sha256=${sourceSha}`,
    `frequency_source_commit=525f9b560de45753a5ea01069454e72e9aa541c6`,
    `wiktionary_api=${wikiApi}`,
    `wiktionary_rights=${evidence.meta.rightsText}`,
    `wiktionary_rights_url=${evidence.meta.rightsUrl}`,
    `translation_endpoint=${translateApi}`,
    "translation_state=machine-translated-unreviewed",
    `selection_law=${evidence.meta.selectionLaw}`,
    `inspected_ranked_candidates=${evidence.meta.inspectedCandidates}`,
    "stable_ids_repaired=111", "legacy_aliases=111", "semantic_definitions=111",
    "nl_cells=1443", "nl_frequency_cells=111", "nl_machine_unreviewed_cells=1332",
  ];
  for (const file of files) {
    const bytes = await readFile(file);
    lines.push(`${file.slice(root.length + 1)}\t${bytes.length}\t${sha256(bytes)}`);
  }
  await writeFile(manifestPath, lines.join("\n") + "\n");
}

const holes = holeRows(await readFile(holesPath, "utf8"));
const current = rankedRows(await readFile(rankedPath));
let evidence;
if (refresh) {
  evidence = await selectEvidence(holes, current);
  await buildTranslations(evidence.rows);
} else {
  evidence = await readEvidence();
}
for (let i = 0; i < 111; i++) {
  if (evidence.rows[i].id !== holes[i].id || evidence.rows[i].legacyLabel !== holes[i].legacyLabel)
    throw new Error(`evidence/defect mismatch at repair row ${i}`);
}
await buildCarriers(evidence, current);
await buildForms(evidence);
await writeManifest(evidence);
for (const script of [
  "cognition/concept-nl-substantive-repair-materialize.mjs",
  "cognition/concept-text-roundtrip-10000-13-build.mjs",
  "cognition/concept-human-corpus-13-reindex.mjs",
]) {
  const result = spawnSync(process.execPath, [script], { cwd: root, stdio: "inherit" });
  if (result.status !== 0) throw new Error(`downstream canonical rebuild failed: ${script}`);
}
process.stdout.write(`repaired 111 stable IDs; 111 aliases; 111 definitions; 1,443 attributed NL cells\n`);
