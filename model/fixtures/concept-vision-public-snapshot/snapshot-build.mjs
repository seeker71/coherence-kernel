#!/usr/bin/env node

// Freeze the selected Wikimedia Commons API rows beside the exact downloaded
// derivatives. This acquisition tool never participates in visual inference.

import { createHash } from "node:crypto";
import { readFile, stat, writeFile } from "node:fs/promises";

const apiUrl = "https://commons.wikimedia.org/w/api.php?action=query&generator=categorymembers&gcmtitle=Category%3AQuality_images&gcmnamespace=6&gcmtype=file&gcmlimit=50&prop=imageinfo&iiprop=url%7Cextmetadata%7Csize&iiurlwidth=640&format=json&formatversion=2";
const selected = [
  [84899909, "built-heritage", 752, "building"],
  [60293875, "museum-object", 8497, "vase"],
  [61782879, "road-transport", 248, "car"],
  [75979460, "environmental-action", 2365, "material"],
  [79089522, "mountain-landscape", 1595, "mountain"],
  [87204571, "cultural-performance", 4515, "jewelry"],
  [101147153, "signage", 4598, "document"],
  [102895898, "urban-art", 998, "art"],
  [109394566, "maritime-wreck", 883, "boat"],
  [114888449, "public-sculpture", 4032, "statue"],
  [120426408, "ocean-transit", 1941, "ocean"],
  [121147384, "harbor-infrastructure", 5585, "dock"],
  [150009219, "archaeology", 1520, "bridge"],
  [150073277, "insect-wildlife", 5547, "butterfly"],
  [166501599, "urban-high-rise", 1020, "apartment"],
  [166527966, "recreation", 8249, "playground"],
  [166556664, "sound-sculpture", 1215, "bell"],
  [166825595, "bamboo-forest", 1874, "forest"],
  [172876577, "residential-architecture", 1795, "roof"],
  [173857725, "civic-access", 296, "door"],
  [35328841, "rural-agriculture", 4091, "barn"],
  [92533678, "seascape", 377, "water"],
  [109360375, "material-decay", 4000, "structure"],
  [167585898, "public-landscape", 1776, "path"],
];

const clean = value => String(value ?? "")
  .replace(/<style[\s\S]*?<\/style>/gi, "")
  .replace(/<[^>]+>/g, " ")
  .replace(/&nbsp;|&#160;/g, " ")
  .replace(/&amp;/g, "&")
  .replace(/\s+/g, " ").trim();
const sha256 = data => createHash("sha256").update(data).digest("hex");
const sourcePath = process.argv[2];
const response = sourcePath
  ? await readFile(sourcePath)
  : Buffer.from(await (await fetch(apiUrl, { headers: { "user-agent": "coherence-kernel/1.0 research fixture" } })).arrayBuffer());
const payload = JSON.parse(response);
const byId = new Map(payload.query.pages.map(page => [page.pageid, page]));
const retrievedAt = process.env.SNAPSHOT_RETRIEVED_AT ?? new Date().toISOString();
const authorOverride = new Map([
  [60293875, "Metropolitan Museum of Art Open Access"],
  [109360375, "Y.ssk"],
  [109394566, "Y.ssk"],
]);

const rows = [];
for (let index = 0; index < selected.length; index += 1) {
  const [pageId, domain, targetId, targetLabel] = selected[index];
  const page = byId.get(pageId);
  if (!page) throw new Error(`selected page missing from API snapshot: ${pageId}`);
  const info = page.imageinfo[0];
  const meta = info.extmetadata;
  const local = `${String(index + 1).padStart(4, "0")}.jpg`;
  const bytes = await readFile(new URL(local, import.meta.url));
  const fileStat = await stat(new URL(local, import.meta.url));
  rows.push({
    ordinal: index + 1,
    local,
    domain,
    targetId,
    targetLabel,
    pageId,
    title: page.title,
    sourcePage: `https://commons.wikimedia.org/wiki/${encodeURIComponent(page.title.replace(/ /g, "_"))}`,
    author: authorOverride.get(pageId) ?? clean(meta.Artist?.value || meta.Credit?.value),
    license: clean(meta.LicenseShortName?.value),
    licenseUrl: clean(meta.LicenseUrl?.value),
    derivativeUrl: info.thumburl,
    pixels: `${info.thumbwidth}x${info.thumbheight}`,
    bytes: fileStat.size,
    sha256: sha256(bytes),
  });
}

const snapshot = {
  retrievedAt,
  apiUrl,
  apiResponseSha256: sha256(response),
  apiResponseBytes: response.length,
  category: "Quality images",
  categoryRowsReturned: payload.query.pages.length,
  selectionRule: "24 frozen page IDs selected only after content-only classifier sweep; metadata never entered inference",
  selected: rows,
};
await writeFile(new URL("SOURCE-SNAPSHOT.json", import.meta.url), `${JSON.stringify(snapshot, null, 2)}\n`);
const header = "ordinal\tlocal\tdomain\ttarget-id\ttarget-label\tpage-id\tcommons-file-page\tauthor\tlicense\tlicense-url\tretrieved-at\tsha256\tbytes\tpixels\tderivative-url\n";
const tsv = rows.map(row => [row.ordinal, row.local, row.domain, row.targetId, row.targetLabel,
  row.pageId, row.sourcePage, row.author, row.license, row.licenseUrl, retrievedAt,
  row.sha256, row.bytes, row.pixels, row.derivativeUrl].join("\t")).join("\n");
await writeFile(new URL("PROVENANCE.tsv", import.meta.url), `${header}${tsv}\n`);
console.log(`frozen ${rows.length} verified Wikimedia Commons photographs from ${payload.query.pages.length}-row API snapshot`);
