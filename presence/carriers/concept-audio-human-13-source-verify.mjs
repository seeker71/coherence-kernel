#!/usr/bin/env node
// Verify the committed human-speech snapshot against live Commons metadata.
// This does not download or transcribe audio; the companion shell carrier does.

import { createHash } from "node:crypto";
import { readFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "../..");
const manifestPath = resolve(
  repoRoot,
  "presence/fixtures/concept-audio-human-13-source.tsv",
);
const manifestBytes = readFileSync(manifestPath);
const lines = manifestBytes.toString("utf8").trimEnd().split("\n");
const header = lines[0].split("\t");
const rows = lines.slice(1).map((line) => {
  const values = line.split("\t");
  return Object.fromEntries(header.map((name, index) => [name, values[index]]));
});

const api = "https://commons.wikimedia.org/w/api.php";
const userAgent =
  "coherence-kernel-human-audio/0.1 (public corpus provenance verification)";
const delay = (milliseconds) =>
  new Promise((resolveDelay) => setTimeout(resolveDelay, milliseconds));

async function getJson(parameters) {
  const url = new URL(api);
  for (const [name, value] of Object.entries(parameters)) {
    url.searchParams.set(name, value);
  }
  url.searchParams.set("origin-audit", Date.now().toString());
  let lastStatus = "not-run";
  for (let attempt = 0; attempt < 6; attempt += 1) {
    const response = await fetch(url, { headers: { "user-agent": userAgent } });
    lastStatus = `${response.status} ${response.statusText}`;
    if (response.ok) return response.json();
    await delay(1000 * (attempt + 1));
  }
  throw new Error(`Commons API unavailable after retries: ${lastStatus}`);
}

const titles = rows.map((row) => {
  const filename = decodeURIComponent(new URL(row.raw_url).pathname.split("/").at(-1));
  return `File:${filename.replaceAll("_", " ")}`;
});
const pageData = await getJson({
  action: "query",
  format: "json",
  titles: titles.join("|"),
  prop: "imageinfo|pageprops|revisions",
  iiprop: "url|sha1|mime|size|user|timestamp|extmetadata",
  rvprop: "content",
  rvslots: "main",
});
const entityData = await getJson({
  action: "wbgetentities",
  format: "json",
  ids: rows.map((row) => row.media_id).join("|"),
});

const pageByMediaId = new Map(
  Object.values(pageData.query?.pages ?? {}).map((page) => [`M${page.pageid}`, page]),
);
const licenseNames = new Map([
  ["Q6938433", "CC0"],
  ["Q18199165", "CC BY-SA 4.0"],
]);
const mismatches = [];
const mismatch = (row, field, expected, actual) =>
  mismatches.push({ index: row.index, field, expected, actual });

for (const row of rows) {
  const page = pageByMediaId.get(row.media_id);
  const entity = entityData.entities?.[row.media_id];
  if (!page || !entity) {
    mismatch(row, "media_id", row.media_id, "missing-from-live-api");
    continue;
  }
  const image = page.imageinfo?.[0] ?? {};
  const statements = entity.statements ?? {};
  const statementValue = (property) =>
    statements[property]?.[0]?.mainsnak?.datavalue?.value;
  const speakerStatement = statements.P10894?.[0];
  const speaker = speakerStatement?.qualifiers?.P2093?.[0]?.datavalue?.value;
  const speakerId = speakerStatement?.qualifiers?.P10369?.[0]?.datavalue?.value;
  const wikitext = page.revisions?.[0]?.slots?.main?.["*"] ?? "";
  const transcription = statementValue("P9533")?.text ?? page.pageprops?.defaultsort;
  const language =
    statementValue("P407")?.id ??
    /languageId\s*=\s*(Q\d+)/i.exec(wikitext)?.[1];
  const license = licenseNames.get(statementValue("P275")?.id);
  const recorded = statementValue("P10135")?.time?.slice(1, 11);

  const checks = [
    ["source_transcription", row.source_transcription, transcription],
    ["speaker", row.speaker, speaker],
    ["speaker_id", row.speaker_id, speakerId],
    ["language_q", row.language_q, language],
    ["recorded", row.recorded, recorded],
    ["license", row.license, license],
    ["api_sha1", row.api_sha1, image.sha1],
    ["api_bytes", row.api_bytes, String(image.size)],
    ["api_timestamp", row.api_timestamp, image.timestamp],
    ["raw_url", row.raw_url, image.url],
  ];
  for (const [field, expected, actual] of checks) {
    if (expected !== actual) mismatch(row, field, expected, actual ?? "missing");
  }
}

const manifestSha256 = createHash("sha256").update(manifestBytes).digest("hex");
console.log(
  `source-rows=${rows.length} live-pages=${pageByMediaId.size} ` +
    `live-entities=${Object.keys(entityData.entities ?? {}).length} ` +
    `mismatches=${mismatches.length} retrieved=${new Date().toISOString()} ` +
    `manifest-sha256=${manifestSha256}`,
);
for (const item of mismatches) console.log(JSON.stringify(item));
if (rows.length !== 13 || mismatches.length !== 0) process.exit(1);
