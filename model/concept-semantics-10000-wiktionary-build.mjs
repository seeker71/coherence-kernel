#!/usr/bin/env node
// Revision-pinned Wiktionary morphology evidence for the explicit WordNet misses.
//
// This builder never invents a gloss. It accepts only an explicit English form
// template naming one unambiguous base lemma, then carries that lemma's real
// Princeton WordNet 3.1 sense, gloss, polysemy and pointer relations.

import { createHash } from "node:crypto";
import { execFileSync } from "node:child_process";
import {
  existsSync, mkdtempSync, readFileSync, rmSync, writeFileSync,
} from "node:fs";
import { tmpdir } from "node:os";
import { basename, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repo = resolve(fileURLToPath(new URL("..", import.meta.url)));
const rankedPath = join(repo, "model/concept-10000-ranked.dat");
const primaryIndexPath = join(repo, "model/concept-semantics-10000-index.dat");
const evidencePath = join(repo, "model/concept-semantics-10000-wiktionary-evidence.jsonl");
const overlayIndexPath = join(repo, "model/concept-semantics-10000-wiktionary-index.dat");
const overlayPayloadPath = join(repo, "model/concept-semantics-10000-wiktionary-payload.dat");
const provenanceIndexPath = join(repo, "model/concept-semantics-10000-wiktionary-provenance-index.dat");
const provenancePayloadPath = join(repo, "model/concept-semantics-10000-wiktionary-provenance-payload.dat");
const statsPath = join(repo, "model/concept-semantics-10000-wiktionary-stats.txt");
const manifestPath = join(repo, "model/concept-semantics-10000-wiktionary-source-manifest.txt");
const wordnetUrl = "https://wordnetcode.princeton.edu/wn3.1.dict.tar.gz";
const wordnetSha256 = "3f7d8be8ef6ecc7167d39b10d66954ec734280b5bdcd57f7d9eafe429d11c22a";
const api = "https://en.wiktionary.org/w/api.php";
const userAgent = "coherence-kernel/1.0 (https://github.com/seeker71/coherence-kernel; attributed semantic projection)";
const args = new Set(process.argv.slice(2));

function sha256(bytes) {
  return createHash("sha256").update(bytes).digest("hex");
}

function sleep(ms) {
  return new Promise((done) => setTimeout(done, ms));
}

async function apiQuery(parameters) {
  const url = new URL(api);
  for (const [key, value] of Object.entries(parameters)) url.searchParams.set(key, value);
  let last;
  for (let attempt = 0; attempt < 6; attempt++) {
    try {
      const response = await fetch(url, { headers: { "User-Agent": userAgent } });
      if (!response.ok) throw new Error(`HTTP ${response.status}`);
      const body = await response.json();
      if (body.error) throw new Error(`${body.error.code}: ${body.error.info}`);
      if (!body.query) throw new Error("response has no query object");
      return body.query;
    } catch (error) {
      last = error;
      await sleep(300 * (attempt + 1));
    }
  }
  throw last;
}

function labelsAndMisses() {
  const ranked = readFileSync(rankedPath);
  const index = readFileSync(primaryIndexPath, "utf8");
  if (ranked.byteLength !== 300000 || index.length !== 350000) throw new Error("canonical table width mismatch");
  const rows = [];
  for (let id = 0; id < 10000; id++) {
    if (index[id * 35] !== "0") continue;
    rows.push({ id, label: ranked.subarray(id * 30, id * 30 + 20).toString("utf8").replace(/ +$/, "") });
  }
  if (rows.length !== 2629) throw new Error(`expected 2629 primary misses, observed ${rows.length}`);
  return rows;
}

function englishSection(wikitext) {
  const match = wikitext.match(/(?:^|\n)==English==\s*\n([\s\S]*?)(?=\n==[^=]|$)/);
  return match ? match[1] : "";
}

function outerTemplates(text) {
  const found = [];
  for (let i = 0; i + 1 < text.length; i++) {
    if (text.slice(i, i + 2) !== "{{") continue;
    const start = i;
    let depth = 1;
    i += 2;
    while (i + 1 < text.length && depth > 0) {
      const pair = text.slice(i, i + 2);
      if (pair === "{{") { depth++; i += 2; continue; }
      if (pair === "}}") { depth--; i += 2; continue; }
      i++;
    }
    if (depth === 0) found.push(text.slice(start, i));
    i--;
  }
  return found;
}

function templateParts(raw) {
  const body = raw.slice(2, -2);
  const parts = [];
  let current = "";
  let braces = 0;
  let brackets = 0;
  for (let i = 0; i < body.length; i++) {
    const pair = body.slice(i, i + 2);
    if (pair === "{{") { braces++; current += pair; i++; continue; }
    if (pair === "}}" && braces > 0) { braces--; current += pair; i++; continue; }
    if (pair === "[[") { brackets++; current += pair; i++; continue; }
    if (pair === "]]" && brackets > 0) { brackets--; current += pair; i++; continue; }
    if (body[i] === "|" && braces === 0 && brackets === 0) {
      parts.push(current.trim()); current = ""; continue;
    }
    current += body[i];
  }
  parts.push(current.trim());
  return parts;
}

const formTemplates = new Set([
  "plural of", "en-plural of", "singular of",
  "inflection of", "infl of", "infl-of", "en-inflection of",
  "verb form of", "noun form of", "adjective form of", "adverb form of",
  "simple past of", "past of", "past participle of", "present participle of",
  "third-person singular of", "third person singular of", "en-third-person singular of",
  "comparative of", "superlative of",
]);

function cleanBase(value) {
  return value
    .replace(/^\[\[|\]\]$/g, "")
    .replace(/\[\[([^\]|]+)\|([^\]]+)\]\]/g, "$2")
    .replace(/\[\[([^\]]+)\]\]/g, "$1")
    .replace(/#.*$/, "")
    .replace(/ /g, "_")
    .trim().toLowerCase();
}

function formFromTemplate(raw, pos) {
  const parts = templateParts(raw);
  const name = parts.shift().toLowerCase().replace(/_/g, " ").replace(/\s+/g, " ");
  if (!formTemplates.has(name)) return null;
  const positional = parts.filter((part) => !/^[A-Za-z][A-Za-z0-9_-]*=/.test(part));
  let base;
  if (name === "en-plural of" || name === "en-third-person singular of") base = positional[0];
  else base = positional[0] === "en" ? positional[1] : positional[0];
  base = cleanBase(base || "");
  if (!base || base === "-") return null;
  return { template: name, base, pos };
}

const sectionPos = new Map([
  ["noun", "n"], ["noun form", "n"], ["proper noun", "n"],
  ["verb", "v"], ["verb form", "v"],
  ["adjective", "a"], ["adjective form", "a"],
  ["adverb", "r"], ["adverb form", "r"],
]);
const otherPos = new Set([
  "article", "circumfix", "classifier", "conjunction", "contraction", "counter",
  "determiner", "diacritical mark", "ideophone", "interjection", "letter",
  "numeral", "particle", "phrase", "postposition", "prefix", "preposition",
  "prepositional phrase", "pronoun", "proverb", "punctuation mark", "suffix", "symbol",
]);

function parseEnglish(english) {
  const sections = [];
  let active = null;
  for (const line of english.split("\n")) {
    const heading = line.match(/^(={3,6})\s*([^=]+?)\s*\1\s*$/);
    if (heading) {
      const title = heading[2].trim().toLowerCase();
      if (sectionPos.has(title) || otherPos.has(title)) {
        active = { title, pos: sectionPos.get(title) || "", definitions: [] };
        sections.push(active);
      } else if (active && heading[1].length <= 5 && !/^(usage notes|synonyms|antonyms|derived terms|related terms|translations|quotations)$/.test(title)) {
        active = null;
      }
      continue;
    }
    const definition = line.match(/^#(?![#*:])\s*(.+?)\s*$/);
    if (active && definition) active.definitions.push(definition[1]);
  }
  const substantive = sections.filter((section) => section.definitions.length > 0);
  const candidates = [];
  for (const section of substantive) {
    section.formDefinitions = 0;
    section.lexicalDefinitions = 0;
    for (const definition of section.definitions) {
      const templates = outerTemplates(definition);
      const forms = templates
        .map((raw) => formFromTemplate(raw, section.pos))
        .filter(Boolean);
      let residual = definition;
      for (const raw of templates) residual = residual.replace(raw, "");
      residual = residual.replace(/<!--[\s\S]*?-->/g, "").replace(/[\s.,;:()\[\]'"!?—–-]/g, "");
      if (forms.length > 0 && !/[\p{L}\p{N}]/u.test(residual)) {
        section.formDefinitions++;
        candidates.push(...forms);
      } else section.lexicalDefinitions++;
    }
  }
  const first = substantive[0];
  let status = "no-substantive-entry";
  if (first && !first.pos) status = "non-wordnet-pos-first";
  else if (first && first.lexicalDefinitions > 0) status = "lexical-or-mixed-first";
  else if (first && substantive.some((section) => section.lexicalDefinitions > 0)) status = "lexical-ambiguity";
  else if (first && candidates.length === 0) status = "no-explicit-form";
  else if (first) {
    const bases = [...new Set(candidates.map((candidate) => candidate.base))];
    status = bases.length === 1 ? "source-eligible" : "ambiguous-bases";
  }
  return {
    status,
    firstPos: first?.title || "",
    sections: substantive.map((section) => ({
      title: section.title,
      pos: section.pos,
      definitions: section.definitions.length,
      formDefinitions: section.formDefinitions,
      lexicalDefinitions: section.lexicalDefinitions,
    })),
    candidates,
  };
}

async function refreshEvidence(misses) {
  const rights = await apiQuery({
    action: "query", format: "json", formatversion: "2", meta: "siteinfo", siprop: "rightsinfo",
  });
  const rows = [];
  for (let at = 0; at < misses.length; at += 25) {
    const batch = misses.slice(at, at + 25);
    const query = await apiQuery({
      action: "query", format: "json", formatversion: "2", prop: "revisions",
      rvprop: "ids|timestamp|content", rvslots: "main", titles: batch.map((row) => row.label).join("|"),
    });
    const byTitle = new Map();
    for (const page of query.pages || []) byTitle.set(page.title.toLowerCase(), page);
    const normalized = new Map((query.normalized || []).map((row) => [row.from.toLowerCase(), row.to.toLowerCase()]));
    for (const miss of batch) {
      const wanted = normalized.get(miss.label.toLowerCase()) || miss.label.toLowerCase();
      const page = byTitle.get(wanted);
      if (!page || page.missing) {
        rows.push({ ...miss, status: "wiktionary-page-missing", pageId: 0, revisionId: 0, timestamp: "", title: page?.title || miss.label, englishSha256: sha256("") , firstPos: "", sections: [], candidates: [] });
        continue;
      }
      const revision = page.revisions?.[0];
      const wikitext = revision?.slots?.main?.content || "";
      const english = englishSection(wikitext);
      if (!english) {
        rows.push({ ...miss, status: "english-section-missing", pageId: page.pageid, revisionId: revision?.revid || 0, timestamp: revision?.timestamp || "", title: page.title, englishSha256: sha256(""), firstPos: "", sections: [], candidates: [] });
        continue;
      }
      rows.push({
        ...miss,
        ...parseEnglish(english),
        pageId: page.pageid,
        revisionId: revision.revid,
        timestamp: revision.timestamp,
        title: page.title,
        englishSha256: sha256(english),
      });
    }
    if (at % 250 === 0) process.stderr.write(`wiktionary ${at}/${misses.length}\n`);
  }
  const meta = {
    schema: "coherence-kernel.wiktionary-morphology-evidence.v1",
    api,
    rightsText: rights.rightsinfo?.text || "",
    rightsUrl: rights.rightsinfo?.url || "",
    userAgent,
    primaryMisses: misses.length,
  };
  const body = [JSON.stringify({ _meta: meta }), ...rows.map((row) => JSON.stringify(row))].join("\n") + "\n";
  writeFileSync(evidencePath, body);
  return { meta, rows };
}

function readEvidence() {
  const lines = readFileSync(evidencePath, "utf8").trimEnd().split("\n").map(JSON.parse);
  return { meta: lines[0]._meta, rows: lines.slice(1) };
}

async function acquireWordNet() {
  const explicit = process.argv.find((arg) => arg.startsWith("--wordnet-dir="));
  if (explicit) return resolve(explicit.slice("--wordnet-dir=".length));
  const root = mkdtempSync(join(tmpdir(), "coherence-wordnet-"));
  const archive = join(root, "wn3.1.dict.tar.gz");
  const response = await fetch(wordnetUrl, { headers: { "User-Agent": userAgent } });
  if (!response.ok) throw new Error(`WordNet download HTTP ${response.status}`);
  const bytes = Buffer.from(await response.arrayBuffer());
  if (sha256(bytes) !== wordnetSha256) throw new Error("WordNet archive SHA-256 mismatch");
  writeFileSync(archive, bytes);
  execFileSync("tar", ["-xzf", archive, "-C", root]);
  const candidates = [join(root, "dict"), join(root, "WordNet-3.1", "dict")];
  const dict = candidates.find((path) => existsSync(join(path, "index.noun")));
  if (!dict) throw new Error("WordNet dict directory not found after extraction");
  process.on("exit", () => rmSync(root, { recursive: true, force: true }));
  return dict;
}

function loadWordNet(dict) {
  const entries = new Map();
  const data = new Map();
  const tags = new Map();
  for (const line of readFileSync(join(dict, "index.sense"), "utf8").split("\n")) {
    const fields = line.trim().split(/\s+/);
    if (fields.length < 4 || fields[2] !== "1") continue;
    const [lemma, code] = fields[0].split("%");
    const pos = code[0] === "1" ? "n" : code[0] === "2" ? "v" : (code[0] === "3" || code[0] === "5") ? "a" : "r";
    tags.set(`${lemma}\0${pos}\0${fields[1]}`, Number(fields[3]));
  }
  for (const [name, pos] of [["noun", "n"], ["verb", "v"], ["adj", "a"], ["adv", "r"]]) {
    for (const line of readFileSync(join(dict, `index.${name}`), "utf8").split("\n")) {
      if (line.startsWith("  ")) continue;
      const fields = line.trim().split(/\s+/);
      if (fields.length < 7) continue;
      const pointerCount = Number(fields[3]);
      const senseCount = Number(fields[4 + pointerCount]);
      const offset = fields[7 + pointerCount - 1];
      entries.set(`${fields[0]}\0${pos}`, { lemma: fields[0], pos, offset, senseCount, tagCount: tags.get(`${fields[0]}\0${pos}\0${offset}`) || 0 });
    }
    for (const line of readFileSync(join(dict, `data.${name}`), "utf8").split("\n")) {
      if (!/^\d{8} /.test(line)) continue;
      const split = line.indexOf(" | ");
      const head = (split >= 0 ? line.slice(0, split) : line).trim().split(/\s+/);
      const gloss = split >= 0 ? line.slice(split + 3).replace(/[ \r]+$/, "") : "";
      const offset = head[0];
      const type = head[2];
      const wordCount = Number.parseInt(head[3], 16);
      const pointerAt = 4 + wordCount * 2;
      const pointerCount = Number(head[pointerAt]);
      let relations = "";
      for (let i = 0; i < pointerCount; i++) {
        const at = pointerAt + 1 + i * 4;
        relations += head[at].padEnd(2, " ") + head[at + 2] + head[at + 1];
      }
      data.set(`${pos}\0${offset}`, { type, gloss, relations, relationCount: pointerCount });
    }
  }
  return { entries, data };
}

const posPriority = new Map([["n", 0], ["v", 1], ["a", 2], ["r", 3]]);

function project(evidence, wordnet) {
  const byId = new Map(evidence.rows.map((row) => [row.id, row]));
  let payload = "";
  let index = "";
  const accepted = [];
  const reasonCounts = new Map();
  for (let id = 0; id < 10000; id++) {
    const row = byId.get(id);
    if (!row) { index += "0".repeat(35); continue; }
    if (row.status === "accepted") row.status = "source-eligible";
    if (row.status !== "source-eligible") {
      reasonCounts.set(row.status, (reasonCounts.get(row.status) || 0) + 1);
      index += "0".repeat(35); continue;
    }
    const base = row.candidates[0].base;
    const allowed = new Set(row.candidates.map((candidate) => candidate.pos).filter(Boolean));
    const candidates = [...allowed].map((pos) => wordnet.entries.get(`${base}\0${pos}`)).filter(Boolean);
    candidates.sort((a, b) => b.tagCount - a.tagCount || posPriority.get(a.pos) - posPriority.get(b.pos));
    if (candidates.length === 0) {
      row.status = "wordnet-base-pos-missing";
      reasonCounts.set(row.status, (reasonCounts.get(row.status) || 0) + 1);
      index += "0".repeat(35); continue;
    }
    const best = candidates[0];
    const semantic = wordnet.data.get(`${best.pos}\0${best.offset}`);
    const allLemmaEntries = ["n", "v", "a", "r"].map((pos) => wordnet.entries.get(`${base}\0${pos}`)).filter(Boolean);
    const senseCount = allLemmaEntries.reduce((sum, entry) => sum + entry.senseCount, 0);
    const lemma = base;
    const rowPayload = String(Buffer.byteLength(lemma)).padStart(2, "0") + lemma +
      String(Buffer.byteLength(semantic.gloss)).padStart(4, "0") + semantic.gloss + semantic.relations;
    const offset = Buffer.byteLength(payload);
    const length = Buffer.byteLength(rowPayload);
    const indexRow = `3${String(offset).padStart(8, "0")}${String(length).padStart(5, "0")}${semantic.type}${best.offset}${String(senseCount).padStart(3, "0")}${allLemmaEntries.length}${String(best.tagCount).padStart(5, "0")}${String(semantic.relationCount).padStart(3, "0")}`;
    if (Buffer.byteLength(indexRow) !== 35) throw new Error(`bad index width at ${id}`);
    index += indexRow;
    payload += rowPayload;
    row.status = "accepted";
    row.selected = { base, pos: best.pos, synset: semantic.type + best.offset, gloss: semantic.gloss, senseCount, posCount: allLemmaEntries.length, tagCount: best.tagCount, relationCount: semantic.relationCount };
    accepted.push(row);
  }
  if (Buffer.byteLength(index) !== 350000) throw new Error("overlay index width mismatch");
  const finalEvidence = [JSON.stringify({ _meta: evidence.meta }), ...evidence.rows.map((row) => JSON.stringify(row))].join("\n") + "\n";
  let provenanceIndex = "";
  let provenancePayload = "";
  for (let id = 0; id < 10000; id++) {
    const row = byId.get(id);
    if (!row) { provenanceIndex += "0".repeat(13); continue; }
    const candidates = row.candidates.map((candidate) =>
      String(Buffer.byteLength(candidate.template)).padStart(2, "0") + candidate.template +
      String(Buffer.byteLength(candidate.base)).padStart(2, "0") + candidate.base + candidate.pos).join("");
    const sourcePayload =
      String(row.pageId).padStart(10, "0") + String(row.revisionId).padStart(10, "0") +
      String(row.timestamp || "").padEnd(20, " ") + row.englishSha256 +
      String(Buffer.byteLength(row.title)).padStart(2, "0") + row.title +
      String(Buffer.byteLength(row.status)).padStart(2, "0") + row.status +
      String(row.candidates.length).padStart(2, "0") + candidates;
    provenanceIndex += String(Buffer.byteLength(provenancePayload)).padStart(8, "0") + String(Buffer.byteLength(sourcePayload)).padStart(5, "0");
    provenancePayload += sourcePayload;
  }
  writeFileSync(evidencePath, finalEvidence);
  writeFileSync(overlayIndexPath, index);
  writeFileSync(overlayPayloadPath, payload);
  writeFileSync(provenanceIndexPath, provenanceIndex);
  writeFileSync(provenancePayloadPath, provenancePayload);
  const stats = [
    `primary_misses=2629`,
    `accepted=${accepted.length}`,
    `combined_mapped=${7371 + accepted.length}`,
    `combined_misses=${2629 - accepted.length}`,
    `overlay_payload_bytes=${Buffer.byteLength(payload)}`,
    ...[...reasonCounts].sort(([a], [b]) => a.localeCompare(b)).map(([reason, count]) => `rejected_${reason.replace(/-/g, "_")}=${count}`),
  ].join("\n") + "\n";
  writeFileSync(statsPath, stats);
  const manifest = [
    "Wiktionary morphology overlay source manifest",
    "witnessed: 2026-07-18",
    "",
    `wiktionary_api ${api}`,
    `wiktionary_rights ${evidence.meta.rightsText}`,
    `wiktionary_rights_url ${evidence.meta.rightsUrl}`,
    `evidence ${basename(evidencePath)}`,
    `evidence_sha256 ${sha256(finalEvidence)}`,
    "evidence_revision_law one pageid/revid/timestamp and English-section SHA-256 per primary miss",
    "acceptance_law first substantive English POS is WordNet-compatible; every substantive definition is an explicit form template; exactly one base lemma; compatible WordNet POS exists",
    "",
    `wordnet_archive_url ${wordnetUrl}`,
    `wordnet_archive_sha256 ${wordnetSha256}`,
    "wordnet_license model/concept-semantics-10000-WORDNET-LICENSE.txt",
    `overlay_index ${basename(overlayIndexPath)}`,
    `overlay_index_sha256 ${sha256(index)}`,
    `overlay_payload ${basename(overlayPayloadPath)}`,
    `overlay_payload_sha256 ${sha256(payload)}`,
    `provenance_index ${basename(provenanceIndexPath)}`,
    `provenance_index_sha256 ${sha256(provenanceIndex)}`,
    `provenance_payload ${basename(provenancePayloadPath)}`,
    `provenance_payload_sha256 ${sha256(provenancePayload)}`,
    `stats ${basename(statsPath)}`,
    `stats_sha256 ${sha256(stats)}`,
  ].join("\n") + "\n";
  writeFileSync(manifestPath, manifest);
  return { accepted, stats };
}

const misses = labelsAndMisses();
const evidence = args.has("--refresh") || !existsSync(evidencePath) ? await refreshEvidence(misses) : readEvidence();
if (evidence.rows.length !== misses.length) throw new Error(`evidence cardinality ${evidence.rows.length} != ${misses.length}`);
const dict = await acquireWordNet();
const result = project(evidence, loadWordNet(dict));
process.stdout.write(result.stats);
