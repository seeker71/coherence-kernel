#!/usr/bin/env node

// Fetch and derive the six public-data task families without Python.
// --refresh performs network reads and rewrites the committed snapshot.
// --verify is offline: it verifies every raw SHA and regenerates both derived
// artifacts byte-for-byte in memory.

import { createHash } from "node:crypto";
import { readFile, writeFile, mkdir } from "node:fs/promises";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const here = dirname(fileURLToPath(import.meta.url));
const snapshotPath = join(here, "derived-task-snapshot.json");
const manifestPath = join(here, "source-manifest.json");
const formPath = join(here, "concept-pl-task-families-snapshot.fk");

const sources = [
  {
    key: "usgs_streamflow",
    file: "raw-usgs-streamflow.json",
    authority: "U.S. Geological Survey National Water Information System",
    url: "https://waterservices.usgs.gov/nwis/dv/?format=json&sites=01646500&startDT=2024-01-01&endDT=2024-01-10&parameterCd=00060&siteStatus=all",
  },
  {
    key: "treasury_debt",
    file: "raw-treasury-debt-to-penny.json",
    authority: "U.S. Department of the Treasury Fiscal Data",
    url: "https://api.fiscaldata.treasury.gov/services/api/fiscal_service/v2/accounting/od/debt_to_penny?filter=record_date:gte:2024-01-02,record_date:lte:2024-01-05&page%5Bsize%5D=100",
  },
  {
    key: "nasa_exoplanet",
    file: "raw-nasa-exoplanet-orbits.json",
    authority: "NASA Exoplanet Archive",
    url: "https://exoplanetarchive.ipac.caltech.edu/TAP/sync?query=select%20top%20100%20pl_name%2Cpl_orbper%2Cpl_orbpererr1%2Cpl_orbpererr2%20from%20ps%20where%20pl_orbper%20is%20not%20null%20and%20pl_orbpererr1%20is%20not%20null%20and%20pl_orbpererr2%20is%20not%20null%20order%20by%20pl_orbper&format=json",
  },
  {
    key: "fda_device_events",
    file: "raw-fda-device-events.json",
    authority: "U.S. Food and Drug Administration openFDA",
    url: "https://api.fda.gov/device/event.json?search=date_received:%5B20240101+TO+20240102%5D&limit=20",
  },
  {
    key: "usgs_earthquakes",
    file: "raw-usgs-earthquakes.json",
    authority: "U.S. Geological Survey Earthquake Hazards Program",
    url: "https://earthquake.usgs.gov/fdsnws/event/1/query?format=geojson&starttime=2024-01-01&endtime=2024-01-02&minmagnitude=2&limit=100&orderby=time-asc",
  },
  {
    key: "nasa_neo",
    file: "raw-nasa-neo.json",
    authority: "NASA Near Earth Object Web Service",
    url: "https://api.nasa.gov/neo/rest/v1/feed?start_date=2024-01-01&end_date=2024-01-03&api_key=DEMO_KEY",
  },
];

const sha256 = (bytes) => createHash("sha256").update(bytes).digest("hex");
const stableJson = (value) => `${JSON.stringify(value, null, 2)}\n`;
const requireValue = (value, message) => {
  if (value === undefined || value === null) throw new Error(message);
  return value;
};
const byKey = (entries) => Object.fromEntries(entries.map((entry) => [entry.key, entry]));

function evaluate(task) {
  const [family, a, b, limit] = [task.familyCode, task.valuesA, task.valuesB, task.limit];
  if (family === 0) return a.filter((value) => value > limit).length;
  if (family === 1) return Math.max(0, a.reduce((sum, value) => sum + value, 0) - limit);
  if (family === 2) return Math.max(0, Math.min(a[1], b[1]) - Math.max(a[0], b[0]));
  if (family === 3) return a.filter((value) => value === 0).length;
  if (family === 4) return a.length - new Set(a).size;
  if (family === 5) {
    let result = 0;
    for (let index = 0; index < a.length; index += 2) {
      result += Math.max(0, a[index + 1] - a[index]);
    }
    return result;
  }
  throw new Error(`unknown family ${family}`);
}

function derive(raw, manifest) {
  const hashes = Object.fromEntries(manifest.sources.map((source) => [source.key, source.rawSha256]));

  const water = JSON.parse(raw.usgs_streamflow.toString("utf8"));
  const waterRows = water.value.timeSeries[0].values[0].value;
  const waterAt = (date) => Number(requireValue(
    waterRows.find((row) => row.dateTime.startsWith(date)), `missing USGS water ${date}`
  ).value);

  const treasury = JSON.parse(raw.treasury_debt.toString("utf8"));
  const treasuryAt = (date) => requireValue(
    treasury.data.find((row) => row.record_date === date), `missing Treasury ${date}`
  );
  const millions = (value) => Math.round(Number(value) / 1_000_000);
  const debt0 = treasuryAt("2024-01-02");
  const debt1 = treasuryAt("2024-01-03");

  const exoplanets = JSON.parse(raw.nasa_exoplanet.toString("utf8"));
  const planetAt = (name, period) => requireValue(
    exoplanets.find((row) => row.pl_name === name && row.pl_orbper === period),
    `missing exoplanet ${name}/${period}`
  );
  const orbitInterval = (row) => [
    Math.round((row.pl_orbper + row.pl_orbpererr2) * 100_000_000),
    Math.round((row.pl_orbper + row.pl_orbpererr1) * 100_000_000),
  ];
  const orbitA = planetAt("Kepler-790 b", 13.73469807);
  const orbitB = planetAt("Kepler-790 b", 13.7347221495);
  const orbitMutation = planetAt("HATS-24 b", 1.3484975);

  const fda = JSON.parse(raw.fda_device_events.toString("utf8"));
  const eventAt = (report) => requireValue(
    fda.results.find((row) => row.report_number === report), `missing FDA report ${report}`
  );
  const completenessFlags = (row) => [
    typeof row.date_of_event === "string" && row.date_of_event.length > 0 ? 1 : 0,
    typeof row.event_type === "string" && row.event_type.length > 0 ? 1 : 0,
    typeof row.device?.[0]?.generic_name === "string" && row.device[0].generic_name.length > 0 ? 1 : 0,
  ];

  const earthquakes = JSON.parse(raw.usgs_earthquakes.toString("utf8"));
  const earthquakeAt = (id) => requireValue(
    earthquakes.features.find((row) => row.id === id), `missing earthquake ${id}`
  );
  const networkCode = { ak: 1, hv: 2, nc: 3, pr: 4, tx: 5, us: 6 };
  const networks = (ids) => ids.map((id) => requireValue(
    networkCode[earthquakeAt(id).properties.net], `unmapped earthquake network ${id}`
  ));

  const neo = JSON.parse(raw.nasa_neo.toString("utf8"));
  const objects = Object.values(neo.near_earth_objects).flat();
  const neoAt = (id) => requireValue(objects.find((row) => row.id === id), `missing NEO ${id}`);
  const diameterPair = (id) => {
    const diameter = neoAt(id).estimated_diameter.meters;
    return [Math.round(diameter.estimated_diameter_min), Math.round(diameter.estimated_diameter_max)];
  };

  const tasks = [
    {
      conceptId: 377,
      familyCode: 0,
      family: "streamflow-threshold",
      valuesA: [waterAt("2024-01-01"), waterAt("2024-01-02"), waterAt("2024-01-03")],
      valuesB: [],
      limit: waterAt("2024-01-07"),
      mutatedValuesA: [waterAt("2024-01-01"), waterAt("2024-01-02"), waterAt("2024-01-09")],
      mutatedValuesB: [],
      mutatedLimit: waterAt("2024-01-07"),
      source: `snapshot:usgs_streamflow:${hashes.usgs_streamflow}`,
      derivation: "NWIS site 01646500 parameter 00060 daily mean discharge; baseline Jan 1-3, threshold Jan 7, mutation substitutes Jan 9 for Jan 3",
    },
    {
      conceptId: 2594,
      familyCode: 1,
      family: "debt-reconciliation",
      valuesA: [millions(debt0.debt_held_public_amt), millions(debt0.intragov_hold_amt)],
      valuesB: [],
      limit: millions(debt0.tot_pub_debt_out_amt),
      mutatedValuesA: [millions(debt1.debt_held_public_amt), millions(debt0.intragov_hold_amt)],
      mutatedValuesB: [],
      mutatedLimit: millions(debt0.tot_pub_debt_out_amt),
      source: `snapshot:treasury_debt:${hashes.treasury_debt}`,
      derivation: "Debt-to-Penny amounts rounded to nearest million; baseline reconciles Jan 2 components to Jan 2 total, mutation substitutes the Jan 3 public-held component",
    },
    {
      conceptId: 2430,
      familyCode: 2,
      family: "orbital-interval-overlap",
      valuesA: orbitInterval(orbitA),
      valuesB: orbitInterval(orbitB),
      limit: 0,
      mutatedValuesA: orbitInterval(orbitMutation),
      mutatedValuesB: orbitInterval(orbitB),
      mutatedLimit: 0,
      source: `snapshot:nasa_exoplanet:${hashes.nasa_exoplanet}`,
      derivation: "Orbital-period lower/upper uncertainty intervals scaled by 1e8 days; baseline uses two Kepler-790 b measurements, mutation substitutes HATS-24 b",
    },
    {
      conceptId: 912,
      familyCode: 3,
      family: "device-record-completeness",
      valuesA: completenessFlags(eventAt("9610595-2024-00002")),
      valuesB: [],
      limit: 0,
      mutatedValuesA: completenessFlags(eventAt("9610595-2024-00001")),
      mutatedValuesB: [],
      mutatedLimit: 0,
      source: `snapshot:fda_device_events:${hashes.fda_device_events}`,
      derivation: "Presence flags for date_of_event, event_type, and first device generic_name; mutation substitutes adjacent report 00001 whose date_of_event is absent",
    },
    {
      conceptId: 5860,
      familyCode: 4,
      family: "seismic-network-repetition",
      valuesA: networks(["pr71435948", "us6000m0w6", "hv73705887", "hv73705892"]),
      valuesB: [],
      limit: 0,
      mutatedValuesA: networks(["pr71435948", "us6000m0w6", "hv73705887", "ak0241oafsc"]),
      mutatedValuesB: [],
      mutatedLimit: 0,
      source: `snapshot:usgs_earthquakes:${hashes.usgs_earthquakes}`,
      derivation: "Network codes ak=1,hv=2,nc=3,pr=4,tx=5,us=6 for named event ids; mutation replaces repeated hv event with named ak event",
    },
    {
      conceptId: 2440,
      familyCode: 5,
      family: "diameter-range-width",
      valuesA: ["2415949", "3160747", "3309828"].flatMap(diameterPair),
      valuesB: [],
      limit: 0,
      mutatedValuesA: ["3457842", "3553062", "3591616"].flatMap(diameterPair),
      mutatedValuesB: [],
      mutatedLimit: 0,
      source: `snapshot:nasa_neo:${hashes.nasa_neo}`,
      derivation: "Rounded NASA estimated minimum/maximum diameter metres for three named object ids; mutation substitutes the next three named ids",
    },
  ].map((task) => ({
    ...task,
    baselineResult: evaluate(task),
    mutatedResult: evaluate({
      ...task,
      valuesA: task.mutatedValuesA,
      valuesB: task.mutatedValuesB,
      limit: task.mutatedLimit,
    }),
  }));

  if (tasks.some((task) => task.baselineResult === task.mutatedResult)) {
    throw new Error("every source-backed mutation must change its family result");
  }
  return {
    schema: "coherence-concept-pl-task-snapshot/v1",
    manifestSha256: sha256(Buffer.from(stableJson(manifest))),
    taskCount: tasks.length,
    tasks,
  };
}

const escapeForm = (value) => value.replaceAll("\\", "\\\\").replaceAll('"', '\\"');
const formList = (values) => `(list ${values.join(" ")})`;
function renderForm(snapshot) {
  const baseline = snapshot.tasks.map((task) =>
    `(list ${task.conceptId} ${task.familyCode} ${formList(task.valuesA)} ${formList(task.valuesB)} ${task.limit} "${escapeForm(task.source)}")`
  );
  const mutated = snapshot.tasks.map((task) =>
    `(list ${task.conceptId} ${task.familyCode} ${formList(task.mutatedValuesA)} ${formList(task.mutatedValuesB)} ${task.mutatedLimit} "${escapeForm(task.source)}")`
  );
  const derivations = snapshot.tasks.map((task) =>
    `(list ${task.conceptId} "${escapeForm(task.family)}" "${escapeForm(task.derivation)}" ${task.baselineResult} ${task.mutatedResult})`
  );
  return `; concept-pl-task-families-snapshot.fk -- generated public-data snapshot.\n; Generated by build.mjs; edit the raw snapshot or builder, never this file.\n; witnessed: 2026-07-18 -> offline raw-SHA verification\n; preludes: form/form-stdlib/core.fk\n(do\n  (defn cptfs-manifest-sha () "${snapshot.manifestSha256}")\n  (defn cptfs-baseline-rows () (list\n    ${baseline.join("\n    ")}))\n  (defn cptfs-mutated-rows () (list\n    ${mutated.join("\n    ")}))\n  (defn cptfs-derivations () (list\n    ${derivations.join("\n    ")}))\n)\n`;
}

async function loadRawFromDisk(manifest) {
  const raw = {};
  for (const source of manifest.sources) {
    const bytes = await readFile(join(here, source.file));
    const actual = sha256(bytes);
    if (actual !== source.rawSha256) {
      throw new Error(`${source.file}: SHA mismatch ${actual} != ${source.rawSha256}`);
    }
    raw[source.key] = bytes;
  }
  return raw;
}

async function refresh() {
  await mkdir(here, { recursive: true });
  const retrievedAt = new Date().toISOString();
  const entries = [];
  for (const source of sources) {
    const response = await fetch(source.url, { headers: { "user-agent": "coherence-kernel-public-snapshot/1" } });
    if (!response.ok) throw new Error(`${source.key}: HTTP ${response.status}`);
    const bytes = Buffer.from(await response.arrayBuffer());
    JSON.parse(bytes.toString("utf8"));
    await writeFile(join(here, source.file), bytes);
    entries.push({
      ...source,
      retrievedAt,
      bytes: bytes.length,
      rawSha256: sha256(bytes),
    });
  }
  const manifest = { schema: "coherence-public-source-manifest/v1", sources: entries };
  const raw = await loadRawFromDisk(manifest);
  const snapshot = derive(raw, manifest);
  await writeFile(manifestPath, stableJson(manifest));
  await writeFile(snapshotPath, stableJson(snapshot));
  await writeFile(formPath, renderForm(snapshot));
  process.stdout.write(`${snapshot.manifestSha256} ${snapshot.taskCount}\n`);
}

async function verify() {
  const manifestBytes = await readFile(manifestPath);
  const manifest = JSON.parse(manifestBytes);
  const raw = await loadRawFromDisk(manifest);
  const snapshot = derive(raw, manifest);
  const expectedSnapshot = await readFile(snapshotPath, "utf8");
  const expectedForm = await readFile(formPath, "utf8");
  if (stableJson(snapshot) !== expectedSnapshot) throw new Error("derived-task-snapshot.json is stale");
  if (renderForm(snapshot) !== expectedForm) throw new Error("concept-pl-task-families-snapshot.fk is stale");
  process.stdout.write(`${snapshot.manifestSha256} ${snapshot.taskCount} verified\n`);
}

const mode = process.argv[2];
if (mode === "--refresh") await refresh();
else if (mode === "--verify") await verify();
else throw new Error("usage: node build.mjs --refresh|--verify");
