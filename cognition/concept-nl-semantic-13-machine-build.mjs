#!/usr/bin/env node
// Fill the non-English 10k concept surface through a live translation carrier.
// No machine label overwrites a lexical-source label; the main build applies
// this cache only to absent cells and marks every such cell G (unreviewed).
import { readFile, writeFile } from "node:fs/promises";
import { resolve } from "node:path";

const rankedPath = resolve(process.argv[2] || "model/concept-10000-ranked.dat");
const outputPath = resolve(process.argv[3] || "cognition/concept-nl-semantic-13-machine.tsv");
const endpoint = "https://translate.googleapis.com/translate_a/single";
const locales = [
  ["id", "id"], ["es", "es"], ["fr", "fr"], ["pt-br", "pt"],
  ["sw", "sw"], ["de", "de"], ["ru", "ru"], ["zh", "zh-CN"],
  ["ja", "ja"], ["ar", "ar"], ["hi", "hi"], ["tr", "tr"],
];

const ranked = await readFile(rankedPath);
if (ranked.length !== 300000) throw new Error(`expected 300000 ranked bytes, got ${ranked.length}`);
const labels = Array.from({ length: 10000 }, (_, id) =>
  ranked.subarray(id * 30, id * 30 + 20).toString("utf8").replace(/ +$/, ""));

function chunksOf(values, maxRows = 100, maxChars = 2200) {
  const chunks = [];
  let chunk = [];
  let chars = 0;
  for (const value of values) {
    if (chunk.length && (chunk.length >= maxRows || chars + value.length + 1 > maxChars)) {
      chunks.push(chunk); chunk = []; chars = 0;
    }
    chunk.push(value); chars += value.length + 1;
  }
  if (chunk.length) chunks.push(chunk);
  return chunks;
}

async function translateChunk(values, target, attempt = 0) {
  const url = new URL(endpoint);
  url.search = new URLSearchParams({ client: "gtx", sl: "en", tl: target, dt: "t", q: values.join("\n") });
  const response = await fetch(url, { headers: { "user-agent": "coherence-kernel-concept-surface/1" } });
  if (!response.ok) {
    if (attempt < 4) {
      await new Promise(resolveDelay => setTimeout(resolveDelay, 400 * (attempt + 1)));
      return translateChunk(values, target, attempt + 1);
    }
    throw new Error(`${target}: HTTP ${response.status}`);
  }
  const payload = await response.json();
  const text = payload[0].map(segment => segment[0] || "").join("").replace(/\r/g, "");
  const translated = text.split("\n");
  if (translated.length === values.length) return translated;
  if (values.length === 1) throw new Error(`${target}: one row became ${translated.length} rows`);
  const middle = Math.floor(values.length / 2);
  return [
    ...await translateChunk(values.slice(0, middle), target, attempt),
    ...await translateChunk(values.slice(middle), target, attempt),
  ];
}

async function translateLocale([lens, target]) {
  const translated = [];
  const chunks = chunksOf(labels);
  for (let index = 0; index < chunks.length; index++) {
    translated.push(...await translateChunk(chunks[index], target));
    if ((index + 1) % 20 === 0 || index + 1 === chunks.length)
      process.stderr.write(`${lens} ${index + 1}/${chunks.length}\n`);
  }
  if (translated.length !== 10000) throw new Error(`${lens}: expected 10000 rows, got ${translated.length}`);
  return translated.map(value => value.replace(/[\t\r\n]+/g, " "));
}

const translatedByLens = new Array(locales.length);
let nextLocale = 0;
async function worker() {
  while (nextLocale < locales.length) {
    const index = nextLocale++;
    translatedByLens[index] = await translateLocale(locales[index]);
  }
}
await Promise.all([worker(), worker(), worker()]);

const lines = [`concept-id\t${locales.map(([lens]) => lens).join("\t")}`];
for (let id = 0; id < 10000; id++)
  lines.push(`${id}\t${translatedByLens.map(column => column[id]).join("\t")}`);
await writeFile(outputPath, `${lines.join("\n")}\n`, "utf8");
process.stdout.write(`built ${outputPath}: 10000 concepts x 12 translated lenses\n`);
