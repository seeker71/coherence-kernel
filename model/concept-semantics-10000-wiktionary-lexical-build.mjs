#!/usr/bin/env node
// Revision-pinned Wiktionary lexical evidence for the remaining semantic holes.
//
// The morphology overlay intentionally rejects pronouns, proper names, lexical
// definitions, and ambiguous inflections because it is a WordNet projection.
// This independent lane does not force those rows into WordNet. It carries the
// first substantive definition exactly as Wiktionary wrote it, with the page
// revision and the SHA-256 of the complete English section. Capitalized lookup
// is allowed only when the exact lower-case title has no English definition;
// this recovers source-defined proper names without inventing a meaning.

import { createHash } from "node:crypto";
import { existsSync, readFileSync, writeFileSync } from "node:fs";
import { basename, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repo = resolve(fileURLToPath(new URL("..", import.meta.url)));
const morphologyEvidencePath = join(repo, "model/concept-semantics-10000-wiktionary-evidence.jsonl");
const evidencePath = join(repo, "model/concept-semantics-10000-wiktionary-lexical-evidence.jsonl");
const indexPath = join(repo, "model/concept-semantics-10000-wiktionary-lexical-index.dat");
const payloadPath = join(repo, "model/concept-semantics-10000-wiktionary-lexical-payload.dat");
const blocksPath = join(repo, "model/concept-semantics-10000-wiktionary-lexical-blocks.dat");
const statsPath = join(repo, "model/concept-semantics-10000-wiktionary-lexical-stats.txt");
const remainingPath = join(repo, "model/concept-semantics-10000-wiktionary-lexical-remaining.tsv");
const manifestPath = join(repo, "model/concept-semantics-10000-wiktionary-lexical-source-manifest.txt");
const api = "https://en.wiktionary.org/w/api.php";
const userAgent = "coherence-kernel/1.0 (https://github.com/seeker71/coherence-kernel; attributed lexical projection)";
const refresh = process.argv.includes("--refresh");

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
  for (let attempt = 0; attempt < 7; attempt++) {
    try {
      const response = await fetch(url, { headers: { "User-Agent": userAgent } });
      if (!response.ok) throw new Error(`HTTP ${response.status}`);
      const body = await response.json();
      if (body.error) throw new Error(`${body.error.code}: ${body.error.info}`);
      if (!body.query) throw new Error("response has no query object");
      return body.query;
    } catch (error) {
      last = error;
      await sleep(400 * (attempt + 1));
    }
  }
  throw last;
}

function englishSection(wikitext) {
  const match = wikitext.match(/(?:^|\n)==English==\s*\n([\s\S]*?)(?=\n==[^=]|$)/);
  return match ? match[1] : "";
}

function substantiveDefinitions(english) {
  const rows = [];
  let section = "";
  for (const line of english.split("\n")) {
    const heading = line.match(/^(={3,6})\s*([^=]+?)\s*\1\s*$/);
    if (heading) {
      const title = heading[2].trim();
      const lower = title.toLowerCase();
      if (/^(etymology|pronunciation|audio|hyphenation|rhymes|homophones|usage notes|references|further reading|anagrams|see also|alternative forms|descendants|derived terms|related terms|translations|quotations)$/.test(lower)) {
        if (heading[1].length <= 4) section = "";
      } else if (heading[1].length <= 5) section = title;
      continue;
    }
    const definition = line.match(/^#(?![#*:])\s*(.+?)\s*$/);
    if (section && definition) rows.push({ section, source: definition[1] });
  }
  return rows;
}

function capitalized(label) {
  if (!/^\p{Ll}/u.test(label)) return label;
  return label[0].toLocaleUpperCase("en-US") + label.slice(1);
}

function morphologyMisses() {
  const lines = readFileSync(morphologyEvidencePath, "utf8").trimEnd().split("\n").map(JSON.parse);
  const rows = lines.slice(1).filter((row) => row.status !== "accepted");
  if (rows.length !== 1556) throw new Error(`expected 1556 combined misses, observed ${rows.length}`);
  return rows.map(({ id, label, status }) => ({ id, label, morphologyStatus: status }));
}

function pageMap(query) {
  const pages = new Map();
  for (const page of query.pages || []) pages.set(page.title.toLocaleLowerCase("en-US"), page);
  return pages;
}

function sourceRow(anchor, page, lookup) {
  if (!page || page.missing) return null;
  const revision = page.revisions?.[0];
  const wikitext = revision?.slots?.main?.content || "";
  const english = englishSection(wikitext);
  const definitions = substantiveDefinitions(english);
  if (definitions.length === 0) return null;
  return {
    ...anchor,
    status: "accepted-lexical",
    lookup,
    title: page.title,
    pageId: page.pageid,
    revisionId: revision.revid,
    timestamp: revision.timestamp,
    englishSha256: sha256(english),
    section: definitions[0].section,
    definition: definitions[0].source,
    definitionCount: definitions.length,
  };
}

async function queryPages(titles) {
  return apiQuery({
    action: "query", format: "json", formatversion: "2", prop: "revisions",
    rvprop: "ids|timestamp|content", rvslots: "main", titles: titles.join("|"),
  });
}

async function refreshEvidence(anchors) {
  const rights = await apiQuery({
    action: "query", format: "json", formatversion: "2", meta: "siteinfo", siprop: "rightsinfo",
  });
  const rows = [];
  for (let at = 0; at < anchors.length; at += 25) {
    const batch = anchors.slice(at, at + 25);
    const exactQuery = await queryPages(batch.map((row) => row.label));
    const exactPages = pageMap(exactQuery);
    const pending = [];
    for (const anchor of batch) {
      const exact = sourceRow(anchor, exactPages.get(anchor.label.toLocaleLowerCase("en-US")), "exact");
      if (exact) rows.push(exact);
      else pending.push(anchor);
    }
    if (pending.length > 0) {
      const capitalQuery = await queryPages(pending.map((row) => capitalized(row.label)));
      const capitalPages = pageMap(capitalQuery);
      for (const anchor of pending) {
        const title = capitalized(anchor.label);
        const found = sourceRow(anchor, capitalPages.get(title.toLocaleLowerCase("en-US")), "capitalized");
        rows.push(found || {
          ...anchor,
          status: "no-english-definition",
          lookup: title === anchor.label ? "exact" : "exact+capitalized",
          title,
          pageId: 0,
          revisionId: 0,
          timestamp: "",
          englishSha256: sha256(""),
          section: "",
          definition: "",
          definitionCount: 0,
        });
      }
    }
    process.stderr.write(`wiktionary lexical ${Math.min(at + batch.length, anchors.length)}/${anchors.length}\n`);
  }
  rows.sort((a, b) => a.id - b.id);
  const meta = {
    schema: "coherence-kernel.wiktionary-lexical-evidence.v1",
    api,
    rightsText: rights.rightsinfo?.text || "",
    rightsUrl: rights.rightsinfo?.url || "",
    userAgent,
    sourceFloor: "first substantive definition; exact title then capitalized proper-name fallback",
    combinedMisses: anchors.length,
  };
  const body = [JSON.stringify({ _meta: meta }), ...rows.map((row) => JSON.stringify(row))].join("\n") + "\n";
  writeFileSync(evidencePath, body);
  return { meta, rows };
}

function readEvidence() {
  const lines = readFileSync(evidencePath, "utf8").trimEnd().split("\n").map(JSON.parse);
  return { meta: lines[0]._meta, rows: lines.slice(1) };
}

function field(value, width, label) {
  const bytes = Buffer.byteLength(value);
  if (bytes >= 10 ** width) throw new Error(`${label} exceeds ${width}-digit byte length`);
  return String(bytes).padStart(width, "0") + value;
}

function build(evidence) {
  const byId = new Map(evidence.rows.map((row) => [row.id, row]));
  let index = "";
  let payload = "";
  let accepted = 0;
  let exact = 0;
  let capitalizedCount = 0;
  const blocks = Array.from({ length: 100 }, () => ({ exact: 0, capitalized: 0, rejected: 0, valid: 0 }));
  const sections = new Map();
  const morphologyStatuses = new Map();
  for (let id = 0; id < 10000; id++) {
    const row = byId.get(id);
    if (!row) {
      index += "0".repeat(15);
      continue;
    }
    morphologyStatuses.set(row.morphologyStatus, (morphologyStatuses.get(row.morphologyStatus) || 0) + 1);
    const rowPayload =
      String(row.pageId).padStart(10, "0") + String(row.revisionId).padStart(10, "0") +
      String(row.timestamp || "").padEnd(20, " ") + row.englishSha256 +
      field(row.lookup, 2, "lookup") + field(row.title, 2, "title") +
      field(row.status, 2, "status") + field(row.section, 2, "section") +
      field(row.definition, 5, "definition") + String(row.definitionCount).padStart(3, "0") +
      field(row.morphologyStatus, 2, "morphology status");
    const offset = Buffer.byteLength(payload);
    const length = Buffer.byteLength(rowPayload);
    const acceptedRow = row.status === "accepted-lexical";
    const valid = acceptedRow
      ? row.pageId > 0 && row.revisionId > 0 && row.timestamp.length === 20 &&
        row.englishSha256.length === 64 && row.definition.length > 0
      : row.status === "no-english-definition" && row.definition.length === 0;
    if (!valid) throw new Error(`invalid source row ${id}`);
    const state = acceptedRow ? (row.lookup === "exact" ? "1" : "2") : "3";
    index += state + "1" +
      String(offset).padStart(8, "0") + String(length).padStart(5, "0");
    const block = blocks[Math.floor(id / 100)];
    if (state === "1") block.exact++;
    else if (state === "2") block.capitalized++;
    else block.rejected++;
    block.valid++;
    payload += rowPayload;
    if (row.status === "accepted-lexical") {
      accepted++;
      if (row.lookup === "exact") exact++;
      else capitalizedCount++;
      sections.set(row.section, (sections.get(row.section) || 0) + 1);
    }
  }
  if (Buffer.byteLength(index) !== 150000) throw new Error("lexical index width mismatch");
  const unresolved = evidence.rows.length - accepted;
  const stats = [
    `input_combined_misses=${evidence.rows.length}`,
    `accepted_lexical=${accepted}`,
    `accepted_exact_title=${exact}`,
    `accepted_capitalized_title=${capitalizedCount}`,
    `remaining_semantic_holes=${unresolved}`,
    `total_semantically_represented=${8444 + accepted}`,
    `payload_bytes=${Buffer.byteLength(payload)}`,
    ...[...sections].sort((a, b) => b[1] - a[1] || a[0].localeCompare(b[0])).map(([section, count]) => `section_${section.toLowerCase().replace(/[^a-z0-9]+/g, "_")}=${count}`),
    ...[...morphologyStatuses].sort().map(([status, count]) => `input_${status.replace(/-/g, "_")}=${count}`),
  ].join("\n") + "\n";
  writeFileSync(indexPath, index);
  writeFileSync(payloadPath, payload);
  const blockBytes = blocks.map((block) =>
    String(block.exact).padStart(3, "0") + String(block.capitalized).padStart(3, "0") +
    String(block.rejected).padStart(3, "0") + String(block.valid).padStart(3, "0")).join("");
  if (Buffer.byteLength(blockBytes) !== 1200) throw new Error("lexical block-summary width mismatch");
  writeFileSync(blocksPath, blockBytes);
  writeFileSync(statsPath, stats);
  const remaining = ["id\tanchor\tmorphology_status\tlookup\tsource_status",
    ...evidence.rows.filter((row) => row.status !== "accepted-lexical").map((row) =>
      `${row.id}\t${row.label}\t${row.morphologyStatus}\t${row.lookup}\t${row.status}`),
  ].join("\n") + "\n";
  writeFileSync(remainingPath, remaining);
  const finalEvidence = readFileSync(evidencePath);
  const manifest = [
    "Wiktionary lexical semantics source manifest",
    "witnessed: 2026-07-18",
    "",
    `wiktionary_api ${api}`,
    `wiktionary_rights ${evidence.meta.rightsText}`,
    `wiktionary_rights_url ${evidence.meta.rightsUrl}`,
    "acceptance_law first substantive English definition at exact title; if absent, first substantive English definition at capitalized title",
    "sense_law source-listed first definition is carried verbatim as a lexical surface meaning; no WordNet synset or context disambiguation is claimed",
    "rejection_law no substantive English definition at either source title remains an explicit hole",
    "",
    `evidence ${basename(evidencePath)}`,
    `evidence_sha256 ${sha256(finalEvidence)}`,
    `index ${basename(indexPath)}`,
    `index_sha256 ${sha256(index)}`,
    `payload ${basename(payloadPath)}`,
    `payload_sha256 ${sha256(payload)}`,
    `blocks ${basename(blocksPath)}`,
    `blocks_sha256 ${sha256(blockBytes)}`,
    `stats ${basename(statsPath)}`,
    `stats_sha256 ${sha256(stats)}`,
    `remaining ${basename(remainingPath)}`,
    `remaining_sha256 ${sha256(remaining)}`,
  ].join("\n") + "\n";
  writeFileSync(manifestPath, manifest);
  return stats;
}

const misses = morphologyMisses();
const evidence = refresh || !existsSync(evidencePath) ? await refreshEvidence(misses) : readEvidence();
if (evidence.rows.length !== misses.length) throw new Error(`evidence cardinality ${evidence.rows.length} != ${misses.length}`);
process.stdout.write(build(evidence));
