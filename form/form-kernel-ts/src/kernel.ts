// form-kernel-ts — vertical-slice host for Form-on-top.
//
// Executes Form recipe trees and binary artifacts. The CLI carries the
// source-to-recipe adapter for current tests; this module holds the
// substrate, walker, host primitives, and binary artifact loader.
//
//   • Substrate          — NodeID + content-addressed intern table
//   • Walker             — 9 RBasic dispatch arms (matches form-kernel-go/rust)
//   • Frames + closures  — scope, lookup, capture
//   • Native primitives  — strings, lists, file I/O, substrate-write surface
//   • Binary loader      — Form artifact bytes → recipe tree
//
// Category/NodeID values come from form/category-contract.json; every host
// projection consumes that machine-readable authority. Cross-kernel NodeID
// agreement is the conformance contract.

import { BP_TABLE } from "./bp_table.ts";
import { FKWU_RESERVED_HEADS } from "./reserved-heads.ts";
import { byteHost, bstrToBytes, bytesToBstr, isWide } from "./byte-host.ts";
import CATEGORY_CONTRACT from "../../category-contract.json" with { type: "json" };
import {
  EMPTY_KERNEL_HOST,
  type KernelHost,
  type KernelHttpResult,
  type KernelPgAnswer,
  type KernelPgOperation,
  type KernelSocketOperation,
} from "./host.ts";

export type { KernelHost } from "./host.ts";

const UTF8_ENCODER = new TextEncoder();
const UTF8_STRICT = new TextDecoder("utf-8", { fatal: true });
const KH_TAG_HEADER_TS = 43001;
// The record-construction clock kernel_stat 164 reads: every record_new this process has run,
// counted where the record is made. fkwu answers the same key from its arm counter for tag 64
// (record_new). Nothing lowers it.
let recordConstructions = 0;
// This kernel's birth on the wall clock: word 2 (start-ms) of the page kernel_live answers for
// this process. KERNEL_LIVE_MAGIC is word 0, fkwu's live-page magic.
const HOST_BIRTH_UNIX_MS = Date.now();
const KERNEL_LIVE_MAGIC = 0x464b4c4956;

// text the kernel made itself (a unit above 255) as UTF-8; a Form string is already bytes
function utf8Encode(text: string): Uint8Array {
  return UTF8_ENCODER.encode(text);
}

// The one float rendering, byte for byte fkwu's fk_fmt_float_js and Go's core.FormatFloatJS
// (strconv.FormatFloat(f, 'g', -1, 64) with NaN and Infinity spelled out): the shortest digits that
// round-trip, written with an exponent when the decimal exponent is below -4 or at least 6 (the
// exponent signed and at least two digits), and in fixed notation otherwise; -0 keeps its sign.
// toExponential() without an argument yields those shortest digits; only the layout is written here.
function formatFloat(f: number): string {
  if (Number.isNaN(f)) return "NaN";
  if (f === Infinity) return "Infinity";
  if (f === -Infinity) return "-Infinity";
  const sign = f < 0 || Object.is(f, -0) ? "-" : "";
  const sci = Math.abs(f).toExponential();
  const at = sci.indexOf("e");
  const digits = sci.slice(0, at).replace(".", "");
  const exp = Number(sci.slice(at + 1));
  if (exp < -4 || exp >= 6) {
    const rest = digits.length > 1 ? `.${digits.slice(1)}` : "";
    return `${sign}${digits.charAt(0)}${rest}e${exp < 0 ? "-" : "+"}${String(Math.abs(exp)).padStart(2, "0")}`;
  }
  if (exp < 0) return `${sign}0.${"0".repeat(-exp - 1)}${digits}`;
  const point = exp + 1;
  if (digits.length <= point) return `${sign}${digits}${"0".repeat(point - digits.length)}`;
  return `${sign}${digits.slice(0, point)}.${digits.slice(point)}`;
}

function asciiBytes(text: string): Uint8Array {
  const bytes = new Uint8Array(text.length);
  for (let i = 0; i < text.length; i++) bytes[i] = text.charCodeAt(i) & 0x7f;
  return bytes;
}

function byteSubarrayIndex(
  haystack: Uint8Array,
  needle: Uint8Array,
  from: number,
): number {
  if (needle.length === 0) return Math.min(Math.max(from, 0), haystack.length);
  const last = haystack.length - needle.length;
  outer: for (let i = Math.max(from, 0); i <= last; i++) {
    for (let j = 0; j < needle.length; j++) {
      if (haystack[i + j] !== needle[j]) continue outer;
    }
    return i;
  }
  return -1;
}

function callSocket(host: KernelHost, operation: KernelSocketOperation): number | string {
  const socketCall = host.socketCall;
  if (socketCall === undefined) throw new Error(`${operation.op}: host carrier unavailable`);
  return socketCall(operation);
}

function socketNumber(host: KernelHost, operation: KernelSocketOperation): number {
  const result = callSocket(host, operation);
  return typeof result === "number" ? result : -1;
}

// ---------------------------------------------------------------------------
// Substrate — NodeID + Recipe + intern table
// ---------------------------------------------------------------------------

// NodeID — the 4-tuple identity. Registered substrate ids use pkg=1.
// Runtime-interned composites use pkg=0 so temporary recipe ids cannot collide
// with registered/basic categories across an execution context boundary.
// Trivials encode their value in `inst`.
//
// Packed into a single number for hot-path map keys: (pkg << 24) | (level
// << 16) | (type << 8) | inst is too small; we use BigInt for the full
// 4×u32 range. But Map<NodeID-as-object, _> has structural-equality
// problems in JS — Maps use reference equality. The kernel keeps two
// projections of every NodeID: an object (for ergonomic access) and a
// canonical string key (`pkg.level.type.inst`) for Map.
export interface NodeID {
  readonly pkg: number;
  readonly level: number;
  readonly type: number;
  readonly inst: number;
}

export const Level = Object.freeze(CATEGORY_CONTRACT.level);

// RBasic — loaded from the canonical machine-readable category contract.
export const RBasic = Object.freeze(CATEGORY_CONTRACT.r_basic);

// Triv — trivial RTypes, numbered in form/category-contract.json. INT (= INT32) encodes
// inline in NodeID.inst, INT64 and FLOAT64 through per-kernel overflow tables, FLOAT32
// inline as its bits.
export const Triv = Object.freeze(CATEGORY_CONTRACT.triv);

// MATH instance: the op (PLUS/MINUS/MUL/DIV/MOD); the operands' kinds choose the fold.
export const RMath = Object.freeze(CATEGORY_CONTRACT.instances.math);
export const RCmp = Object.freeze(CATEGORY_CONTRACT.instances.compare);
export const RLogic = Object.freeze(CATEGORY_CONTRACT.instances.logic);
export const RCond = Object.freeze(CATEGORY_CONTRACT.instances.cond);
export const RBlock = Object.freeze(CATEGORY_CONTRACT.instances.block);
export const RMatch = Object.freeze(CATEGORY_CONTRACT.instances.match);

// NameID — interned identifier handle. The same number used to encode a
// name trivial's NodeID instance is what every runtime name-lookup
// compares. String comparison happens once at parse time, never in the
// hot path.
export type NameID = number;

// Recipe — composite storage. Trivials are NOT stored; their NodeID carries
// the value.
interface Recipe {
  readonly category: NodeID;
  readonly children: readonly NodeID[];
}

const NO_CHILDREN: readonly NodeID[] = [];

// Stable, content-addressed hash key for a recipe. Same shape ⇒ same key.
function recipeKey(category: NodeID, children: readonly NodeID[]): string {
  let k = `C|${category.pkg}.${category.level}.${category.type}.${category.inst}`;
  for (const c of children) {
    k += `|${c.pkg}.${c.level}.${c.type}.${c.inst}`;
  }
  return k;
}

export function nodeKey(n: NodeID): string {
  return `${n.pkg}.${n.level}.${n.type}.${n.inst}`;
}

function sourceInventorySkipSet(value: Value): Set<string> {
  const skip = new Set<string>();
  if (value.kind !== "list") return skip;
  for (const item of value.list) {
    if (item.kind === "str" && item.str !== "") skip.add(item.str);
  }
  return skip;
}

function sourceInventoryRow(relPath: string, loc: number): Value {
  return {
    kind: "list",
    list: [
      { kind: "str", str: relPath },
      { kind: "int", int: loc },
    ],
  };
}

export type NativeFn = (k: Kernel, args: Value[]) => Value;

interface SwitchArm {
  pattern: NodeID;
  body: NodeID;
}

interface SwitchTable {
  cases: Map<string, NodeID>;
  dynamicArms: SwitchArm[];
  defaultBody?: NodeID;
}

// The category an imported leaf from another scope is re-interned under.
const UNDEFINED_CATEGORY: NodeID = { pkg: 1, level: Level.BASIC, type: RBasic.UNDEFINED, inst: 0 };

// Host effects are injected at construction. The Node worker-backed socket
// and HTTP carriers live in node-host.ts and never enter the browser graph.
export class Kernel {
  // Composite recipes — keyed by content (recipeKey) for intern dedup,
  // and by NodeID (nodeKey) for walker access.
  private byKey = new Map<string, NodeID>();
  byID = new Map<string, Recipe>();
  // the same rows by the NodeID object intern handed out: the walker's lookup, no key built
  private readonly recipeByNode = new Map<NodeID, Recipe>();
  private nextInst = 1; // next instance number for composites
  private sourceAttr = new Map<string, { file: NameID; line: number; col: number }>();
  // formStack — the Form-level call chain currently live (closures and
  // native names, innermost last). Pushed at dispatch, popped on the
  // success path only — after a throw the frames that were live at the
  // crash remain for the top-level catch to surface; a closure is named
  // only then (formStackLabels).
  formStack: (string | Closure)[] = [];
  // stopSeq — stops an attempt has caught, the id of each stop line
  stopSeq = 0;
  // readingFiles — line map for the source currently being read:
  // (file name, first global line) per concatenated part. When non-empty,
  // the reader attributes every parenthesized form so fatal diagnostics
  // can name the Form source line.
  readingFiles: { file: string; startLine: number }[] = [];

  // resolveReadingLine — map a global line in the concatenated read buffer
  // back to (file, line within that file).
  resolveReadingLine(globalLine: number): { file: string; line: number } | null {
    let owner: { file: string; line: number } | null = null;
    for (const part of this.readingFiles) {
      if (part.startLine <= globalLine) {
        owner = { file: part.file, line: globalLine - part.startLine + 1 };
      } else {
        break;
      }
    }
    return owner;
  }

  // attributeSource — record a node's authoring site (first writer wins —
  // content-addressing means a shape interned from two sites keeps its
  // first authoring site).
  attributeSource(node: NodeID, file: string, line: number, col: number): void {
    const key = nodeKey(node);
    if (!this.sourceAttr.has(key)) {
      this.sourceAttr.set(key, { file: this.internName(file), line, col });
    }
  }

  // formStackLabels — the live Form call chain as labels, innermost last.
  formStackLabels(): string[] {
    return this.formStack.map((f) => (typeof f === "string" ? f : this.formFrameLabel(f.name, f.body)));
  }

  // formStackDisplay — the live Form call chain, innermost first, capped.
  formStackDisplay(max: number): string {
    const labels = this.formStackLabels();
    const total = labels.length;
    if (total === 0) return "";
    let out = labels.slice(Math.max(0, total - max)).reverse().join(" < ");
    if (total > max) out += ` … (+${total - max} more)`;
    return out;
  }

  // formFrameLabel — a closure frame's display label: the function name,
  // plus file:line:col when the body recipe carries source attribution.
  private formFrameLabel(name: NameID, body: NodeID): string {
    let label = this.nameStr(name);
    const loc = this.sourceAttr.get(nodeKey(body));
    if (loc !== undefined) {
      label = `${label}@${this.nameStr(loc.file)}:${loc.line}:${loc.col}`;
    }
    return label;
  }
  private importSeq = 1;
  private framebufferRoots: NodeID[] = [];
  // unitRoots — the unit roots the reader built, by how walkUnit reads them:
  // UNIT_DO for a source whose one form is a (do ...), UNIT_WRAPPER for the
  // implicit do that holds several top-level forms.
  readonly unitRoots = new Map<string, number>();
  // unitView — the unit version: a later unit let that rebinds a name raises
  // it, and a closure defined at the unit level reads the unit as of its own.
  unitView = 1;
  // closuresCreated — closures made so far; a do binds a let in a fresh frame
  // when one was made since the do's scope last opened.
  closuresCreated = 0;
  // voiced — the organ-health readings this process has already spoken.
  private readonly voiced = new Set<string>();
  // listCopies — list elements cons has copied in this run.
  private listCopies = 0;

  // String table — substrate strings + identifier names share this table.
  // A name's NodeID.inst is its index into `strs`.
  strs: string[] = [];
  private strIdx = new Map<string, NameID>();

  // Overflow tables for 64-bit numerics. Each is content-addressed by
  // value: `intern_int64(42)` returns the same inst every call.
  //
  // Float canonicalization on intern:
  //   - NaN bit patterns collapse to the quiet-NaN canonical
  //   - -0.0 and +0.0 share an entry (canonical +0.0)
  //   - +Inf and -Inf keep distinct identity
  private i64s: bigint[] = [];
  private i64Idx = new Map<bigint, number>();
  private f64s: number[] = [];
  private f64Idx = new Map<string, number>(); // keyed by IEEE bit pattern as hex

  // Natives — the native function each NameID calls.
  natives = new Map<NameID, NativeFn>();
  // methods — the blueprint method table (BML/NUMS reference: methods live on
  // the blueprint/type, shared by all instances, name-dispatched). Keyed by
  // `${nodeKey(blueprint)}:${nameID}` → the method's Closure.
  methods = new Map<string, Closure>();

  // SWITCH recipe cache — source-level BML/Form `match` lowers to
  // RBasic.MATCH/RMatch.SWITCH. Literal arms are direct NodeID→body edges,
  // keyed by the substrate identity of the scrutinee value. The cache key is
  // the match recipe's own content-addressed NodeID.
  switchTables = new Map<string, SwitchTable>();

  readonly host: KernelHost;
  // the last failure of the pg_* and config_* doors, as pg_last_error reads it
  private pgLastError = "";

  // the call heads the walker asks by NameID: attempt, and the heads fkwu reserves
  readonly attemptName: NameID;
  readonly reservedHeads: ReadonlySet<NameID>;

  constructor(host: KernelHost = EMPTY_KERNEL_HOST) {
    // strings are bytes inside the kernel; text meets them only at the host (byte-host.ts)
    this.host = byteHost(host);
    this.attemptName = this.internName("attempt");
    this.reservedHeads = new Set([...FKWU_RESERVED_HEADS].map((name) => this.internName(name)));
    this.registerNatives();
  }

  shutdown(): void {
    this.host.shutdown?.();
  }

  // intern — content-addressed insertion. Same shape ⇒ same NodeID.
  intern(category: NodeID, children: readonly NodeID[]): NodeID {
    const k = recipeKey(category, children);
    const existing = this.byKey.get(k);
    if (existing) return existing;
    const nid: NodeID = {
      pkg: 0,
      level: category.level,
      type: category.type,
      inst: this.nextInst++,
    };
    const recipe: Recipe = { category, children };
    this.byKey.set(k, nid);
    this.byID.set(nodeKey(nid), recipe);
    this.recipeByNode.set(nid, recipe);
    return nid;
  }

  nextImportScope(): number {
    return this.importSeq++;
  }

  // markUnitRoot — the reader names each do it hands back as a unit root:
  // the implicit do around several top-level forms (wrapper), or the one
  // top-level (do ...) of a source.
  markUnitRoot(root: NodeID, wrapper: boolean): void {
    if (blockKind(this, root) === BLOCK_KIND_DO) {
      this.unitRoots.set(nodeKey(root), wrapper ? UNIT_WRAPPER : UNIT_DO);
    }
  }

  // voiceOrgan — the kernel says it when its reading of a form departs from
  // the language, or a list operation outgrows its budget: one
  // organ-health-v1 reading on stderr, the line the native process runner
  // reads live (form/form-stdlib/organ-health.bml). A reading speaks once per
  // process; its need stays open while the kernel holds no remedy.
  voiceOrgan(
    flow: string,
    aspect: string,
    expected: string,
    observed: string,
    resource: string,
    detail: string,
    offers: string[] = ["continue"],
    evidence: { [key: string]: string | number } = { kernel: "ts" },
  ): void {
    const key = `${flow}\u0000${aspect}\u0000${observed}`;
    if (this.voiced.has(key)) return;
    this.voiced.add(key);
    const now = Date.now();
    const row = {
      schema: "organ-health-v1",
      id: `form-kernel-ts-${now}-${this.voiced.size}:${flow}:${aspect}`,
      organ: "form-kernel-ts",
      flow,
      aspect,
      stage: "observe",
      expected,
      observed,
      health: 0,
      surprise: 1,
      needs: [{ resource, detail }],
      offers,
      selected: "",
      evidence,
      observed_at_ms: now,
      at_ms: now,
    };
    this.host.writeStderr?.(`form-organ health ${JSON.stringify(row)}\n`);
  }

  // noteListCopy — a cons copies a list when it branches off a view that is not its
  // buffer's longest, or meets a list built as an array; past the budget the kernel
  // says so once.
  noteListCopy(n: number): void {
    const before = this.listCopies;
    this.listCopies = before + n;
    if (before < LIST_COPY_BUDGET && this.listCopies >= LIST_COPY_BUDGET) {
      this.voiceOrgan(
        "list",
        "copy-budget",
        `cons copies at most ${LIST_COPY_BUDGET} list elements in one run`,
        `copied ${this.listCopies} list elements`,
        "shared-tail-list",
        "cons branches off shorter views of one list many times; each branch copies the cells it keeps",
      );
    }
  }

  remapImportedLeaf(scope: number, nid: NodeID): NodeID {
    if (nid.pkg !== 0) return nid;
    return this.intern(UNDEFINED_CATEGORY, [
      this.internTrivialInt(scope),
      this.internTrivialInt(nid.level),
      this.internTrivialInt(nid.type),
      this.internTrivialInt(nid.inst),
    ]);
  }

  // internTrivialInt — inline while the value fits the 32-bit inst slot, the INT64 table past
  // it; both decode back to the one integer kind.
  internTrivialInt(n: number): NodeID {
    if (n >= -2147483648 && n <= 2147483647) {
      const inst = (n | 0) >>> 0;
      return { pkg: 1, level: Level.TRIVIAL, type: Triv.INT, inst };
    }
    return this.internTrivialInt64(BigInt(Math.trunc(n)));
  }

  internString(s: string): NodeID {
    const idx = this.internName(s);
    return { pkg: 1, level: Level.TRIVIAL, type: Triv.STRING, inst: idx };
  }

  internTrivialBool(b: boolean): NodeID {
    return { pkg: 1, level: Level.TRIVIAL, type: Triv.BOOL, inst: b ? 1 : 0 };
  }

  internTrivialNull(): NodeID {
    return { pkg: 1, level: Level.TRIVIAL, type: Triv.NULL, inst: 0 };
  }

  internTrivialFloat32(f: number): NodeID {
    // Reinterpret f32 bits as u32, store inline.
    const buf = new ArrayBuffer(4);
    new Float32Array(buf)[0] = f;
    const inst = new Uint32Array(buf)[0]!;
    return { pkg: 1, level: Level.TRIVIAL, type: Triv.FLOAT32, inst };
  }

  // ---- Typed numerics — overflow tables (64-bit) ----

  internTrivialInt64(n: bigint): NodeID {
    const existing = this.i64Idx.get(n);
    if (existing !== undefined) {
      return { pkg: 1, level: Level.TRIVIAL, type: Triv.INT64, inst: existing };
    }
    const idx = this.i64s.length;
    this.i64s.push(n);
    this.i64Idx.set(n, idx);
    return { pkg: 1, level: Level.TRIVIAL, type: Triv.INT64, inst: idx };
  }

  internTrivialFloat64(f: number): NodeID {
    // Float canonicalization for content-addressing:
    //   - all NaN bit patterns → one canonical quiet NaN
    //   - -0.0 → +0.0
    let canonical = f;
    if (Number.isNaN(f)) {
      canonical = NaN; // JS NaN is canonical-quiet already
    } else if (f === 0 && 1 / f === -Infinity) {
      canonical = 0;
    }
    // Key the index by the IEEE 754 bit pattern so equal-bits ⇒ same index.
    const buf = new ArrayBuffer(8);
    new Float64Array(buf)[0] = canonical;
    const lo = new Uint32Array(buf)[0]!;
    const hi = new Uint32Array(buf)[1]!;
    const key = `${hi.toString(16)}_${lo.toString(16)}`;
    const existing = this.f64Idx.get(key);
    if (existing !== undefined) {
      return {
        pkg: 1,
        level: Level.TRIVIAL,
        type: Triv.FLOAT64,
        inst: existing,
      };
    }
    const idx = this.f64s.length;
    this.f64s.push(canonical);
    this.f64Idx.set(key, idx);
    return { pkg: 1, level: Level.TRIVIAL, type: Triv.FLOAT64, inst: idx };
  }

  // ---- Decoders for the overflow tables ----

  decodeInt64(inst: number): bigint {
    const v = this.i64s[inst];
    if (v === undefined) throw new Error(`int64: bad index ${inst}`);
    return v;
  }

  decodeFloat64(inst: number): number {
    const v = this.f64s[inst];
    if (v === undefined) throw new Error(`float64: bad index ${inst}`);
    return v;
  }

  decodeFloat32(inst: number): number {
    const buf = new ArrayBuffer(4);
    new Uint32Array(buf)[0] = inst;
    return new Float32Array(buf)[0]!;
  }

  internName(s: string): NameID {
    const existing = this.strIdx.get(s);
    if (existing !== undefined) return existing;
    const idx = this.strs.length;
    this.strs.push(s);
    this.strIdx.set(s, idx);
    return idx;
  }

  // category — the recipe row answers first. A composite sits at its
  // category's level, so one interned over a trivial-level category is at
  // level 1 too; only a NodeID with no row answers itself.
  category(n: NodeID): NodeID {
    const r = this.recipeAt(n);
    return r ? r.category : n;
  }

  children(n: NodeID): readonly NodeID[] {
    const r = this.recipeAt(n);
    return r ? r.children : NO_CHILDREN;
  }

  // recipeAt — a composite's row: the NodeID intern handed out answers by identity, any
  // other spelling of the same coordinates (make_nodeid, a decoded artifact) by its key.
  recipeAt(n: NodeID): Recipe | undefined {
    return this.recipeByNode.get(n) ?? this.byID.get(nodeKey(n));
  }

  trivialValue(n: NodeID): Value {
    if (n.level !== Level.TRIVIAL) {
      throw new Error(`trivialValue: ${nodeKey(n)} is composite`);
    }
    switch (n.type) {
      case Triv.INT32: {
        // (same slot as Triv.INT)
        const u = n.inst >>> 0;
        const i = u > 0x7fffffff ? u - 0x100000000 : u;
        return { kind: "int", int: i };
      }
      case Triv.STRING: {
        const s = this.strs[n.inst];
        if (s === undefined) {
          throw new Error(`trivialValue: string index ${n.inst} out of range`);
        }
        return { kind: "str", str: s };
      }
      case Triv.BOOL:
        return boolInt(n.inst !== 0);
      case Triv.NULL:
        return { kind: "null" };
      case Triv.INT64:
        return intOrWide(this.decodeInt64(n.inst));
      case Triv.FLOAT32:
        return { kind: "f64", float: this.decodeFloat32(n.inst) };
      case Triv.FLOAT64:
        return { kind: "f64", float: this.decodeFloat64(n.inst) };
      default:
        throw new Error(`trivialValue: unknown trivial type ${n.type}`);
    }
  }

  identID(n: NodeID): NameID {
    // Bare string trivial — the NameID IS the inst.
    if (n.level === Level.TRIVIAL && n.type === Triv.STRING) {
      return n.inst;
    }
    // IDENT recipe wrapping a string trivial.
    const kids = this.children(n);
    if (
      kids.length === 1 &&
      kids[0] !== undefined &&
      kids[0].level === Level.TRIVIAL &&
      kids[0].type === Triv.STRING
    ) {
      return kids[0].inst;
    }
    throw new Error(`identID: ${nodeKey(n)} is not an identifier shape`);
  }

  nameStr(id: NameID): string {
    const s = this.strs[id];
    if (s === undefined) {
      throw new Error(`nameStr: NameID ${id} out of range`);
    }
    return s;
  }

  // render — a value as print and the CLI write it: an absence reads "nothing" at any depth.
  render(v: Value): string {
    return this.renderValue(v, "nothing");
  }

  // renderValue — the one rendering (law 9); `nothing` spells an absence inside a list, where
  // fkwu's value_str writes "null" and its print "nothing". Strings are bare, records "<record>".
  renderValue(v: Value, nothing: string): string {
    switch (v.kind) {
      case "null":
        return nothing;
      case "int":
        return String(v.int);
      case "i64":
        return String(v.bigint);
      case "f64":
        return formatFloat(v.float);
      case "str":
        return v.str;
      case "list":
        return "[" + v.list.map((x) => this.renderValue(x, nothing)).join(", ") + "]";
      case "closure":
        return "<closure>";
      case "nodeid":
        return `@${nodeKey(v.nodeid)}`;
      case "ctor":
        return `${v.ctor_name}(${v.args.map((a) => this.renderValue(a, nothing)).join(", ")})`;
      case "record":
        return "<record>";
    }
  }

  // -------------------------------------------------------------------------
  // Native primitives — registered once; called by FNCALL when the callee
  // identifier resolves to a NameID in the natives map.
  // -------------------------------------------------------------------------

  // A read-side door's path, resolved the way every kernel resolves it (Host.resolveReadPath).
  // Doors that create or change a file pass their path as given.
  private hostReadPath(path: string): string {
    return this.host.resolveReadPath?.(path) ?? path;
  }

  private registerNative(name: string, fn: NativeFn): void {
    this.natives.set(this.internName(name), fn);
  }

  private registerNatives(): void {
    // print writes its values and one newline and answers 0, as fkwu's print does
    this.registerNative("print", (_k, args) => {
      const parts = args.map((a) => this.render(a));
      this.host.writeStdout?.(parts.join(" ") + "\n");
      return { kind: "int", int: 0 };
    });
    // String ops
    // a string is bytes, one code unit each (byte-host.ts): its length is its byte count
    this.registerNative("str_len", (_k, args) => ({
      kind: "int",
      int: argStr(args, 0).length,
    }));
    // substring — BYTES, CLAMPED, NEVER DIES. The one meaning, four ways,
    // laid 2026-09-07 (substring-one-meaning-band.fk, drift gate).
    //
    //   substring(s, start, end) is the bytes of s from max(start,0) up to
    //   min(end, str_len(s)); empty when that range is empty or reversed;
    //   empty when s is not a string. It refuses nothing and it FLOORS
    //   nothing.
    //
    // WHY BYTES. The body indexes bytes everywhere — str_byte_at is the
    // narrow waist, and str_find, split-on, trim, the frame readers and the
    // row walkers all compute byte offsets. Flooring both ends to character
    // starts hands those same indices a SHORTER, SHIFTED window in silence,
    // which silently re-cut every Persian, Chinese, Japanese and Hebrew row
    // in form-stdlib/locale-rows.
    //
    // WHY CLAMPED. fkwu and the core.fk recipe both clamped from birth. A
    // throw here turned an ordinary out-of-range index into a dead process on
    // three arms and an empty string on the fourth.
    //
    // A string is bytes here as on fkwu (byte-host.ts), so every cut, one that
    // severs a multi-byte character included, is the same bytes fkwu holds.
    this.registerNative("substring", (_k, args) => {
      const v = args[0];
      if (v?.kind !== "str") return { kind: "str", str: "" };
      const n = v.str.length;
      const rawStart = args[1] ? argInt(args, 1) : 0;
      const rawEnd = args[2] ? argInt(args, 2) : 0;
      const a = rawStart < 0 ? 0 : rawStart > n ? n : rawStart;
      const b = rawEnd < 0 ? 0 : rawEnd > n ? n : rawEnd;
      if (b <= a) return { kind: "str", str: "" };
      return { kind: "str", str: v.str.slice(a, b) };
    });
    this.registerNative("str_concat", (_k, args) => ({
      kind: "str",
      str: argStr(args, 0) + argStr(args, 1),
    }));
    this.registerNative("form_error", (_k, args) => {
      throw new Error(argStr(args, 0));
    });
    const valueKindNative = (_k: Kernel, args: Value[]): Value => ({
      kind: "str",
      str: valueKindName(args[0] ?? { kind: "null" }),
    });
    this.registerNative("value_kind", valueKindNative);
    // core.fk's float_to_str still asks the kebab spelling, as fkwu's rewrite row allows
    this.registerNative("value-kind", valueKindNative);
    // nothing / nothing? — the axiom-1 third value and the one question that sees it,
    // native as on fkwu (tags 137/138): never-was is neither 0 nor empty.
    this.registerNative("nothing", () => ({ kind: "null" }));
    this.registerNative("nothing?", (_k, args) =>
      ({ kind: "int", int: args[0]?.kind === "null" ? 1 : 0 }));
    // float_leaf is fkwu's own door (tag 201) under its rewrite rows; called by hand it answers nothing
    this.registerNative("float_leaf", () => ({ kind: "null" }));
    // --- struct/object primitive (BML reference, rung 2) ----------------
    // A Record is the kernel's first MUTABLE value: a struct/object with
    // identity. Every language's class/struct compiles onto these natives.
    // Blueprint NodeID tags the type; fields are a name→value map.
    //
    // record_new — (record_new blueprint k1 v1 k2 v2 ...) → record.
    // A blueprint of 0 builds a record with no blueprint (null): its fields
    // work as on any record, record_blueprint reads back 0, and no method
    // dispatches on it. fkwu keeps the blueprint operand verbatim, and 0 is
    // the body's most common record shape (a plain field map).
    this.registerNative("record_new", (k, args) => {
      recordConstructions += 1;
      const a0 = args[0];
      const owner = a0?.kind === "record" ? a0.record : undefined;
      const bp =
        owner !== undefined || (a0?.kind === "int" && a0.int === 0) ? null : argNodeID(args, 0);
      const rec: Record = { blueprint: bp, fields: [] };
      if (owner !== undefined) rec.blueprintRecord = owner;
      let i = 1;
      while (i + 1 < args.length) {
        recordSet(rec, k.internName(argStr(args, i)), args[i + 1]!);
        i += 2;
      }
      return { kind: "record", record: rec };
    });
    // record_get — (record_get rec "field") → value, or 0 when the record
    // carries no such field: fkwu's answer, which the body reads as eq(v, 0).
    this.registerNative("record_get", (k, args) => {
      const r = args[0]!;
      if (r.kind !== "record") throw new Error("record_get: not a record");
      const v = recordGet(r.record, k.internName(argStr(args, 1)));
      return v ?? { kind: "int", int: 0 };
    });
    // record_set — (record_set rec "field" value) → the record (mutated in
    // place; shared identity means all holders see it). BML's `self.x = v`.
    this.registerNative("record_set", (k, args) => {
      const r = args[0]!;
      if (r.kind !== "record") throw new Error("record_set: not a record");
      recordSet(r.record, k.internName(argStr(args, 1)), args[2]!);
      return r;
    });
    // record_has — (record_has rec "field") → bool.
    this.registerNative("record_has", (k, args) => {
      const r = args[0]!;
      if (r.kind !== "record") return boolInt(false);
      const v = recordGet(r.record, k.internName(argStr(args, 1)));
      return boolInt(v !== undefined);
    });
    // record_blueprint — (record_blueprint rec) → the blueprint NodeID.
    this.registerNative("record_blueprint", (_k, args) => {
      const r = args[0]!;
      if (r.kind !== "record") throw new Error("record_blueprint: not a record");
      if (r.record.blueprintRecord !== undefined) {
        return { kind: "record", record: r.record.blueprintRecord };
      }
      if (r.record.blueprint === null) return { kind: "int", int: 0 };
      return { kind: "nodeid", nodeid: r.record.blueprint };
    });
    // record? — (record? v) → bool type predicate.
    this.registerNative("record?", (_k, args) => boolInt(args[0]!.kind === "record"));
    // record_keys — (record_keys rec) → list of field-name strings, in
    // insertion order. Lets Form enumerate a record used as a hash map
    // (e.g. cell-log-store.fk's keydir for compaction).
    this.registerNative("record_keys", (k, args) => {
      const r = args[0]!;
      if (r.kind !== "record") return { kind: "list", list: [] };
      return {
        kind: "list",
        list: r.record.fields.map((f) => ({ kind: "str", str: k.strs[f.name]! }) as Value),
      };
    });
    // --- methods on the blueprint (BML/NUMS reference, rung 2b) ----------
    // Methods live on the blueprint/type, shared by all records of that type,
    // name-dispatched. The keystone that makes a Record a real object.
    //
    // method_define — (method_define blueprint "name" closure) → blueprint.
    this.registerNative("method_define", (k, args) => {
      const cl = args[2]!;
      if (cl.kind !== "closure") {
        throw new Error("method_define: third arg must be a closure");
      }
      const bp = argNodeID(args, 0);
      const key = `${nodeKey(bp)}:${k.internName(argStr(args, 1))}`;
      k.methods.set(key, cl.closure);
      return args[0]!;
    });
    // method_has — (method_has record-or-blueprint "name") → bool.
    this.registerNative("method_has", (k, args) => {
      const a0 = args[0]!;
      let bp: NodeID;
      if (a0.kind === "record") {
        if (a0.record.blueprint === null) return boolInt(false);
        bp = a0.record.blueprint;
      } else if (a0.kind === "nodeid") bp = a0.nodeid;
      else return boolInt(false);
      const key = `${nodeKey(bp)}:${k.internName(argStr(args, 1))}`;
      return boolInt(k.methods.has(key));
    });
    // method_invoke — (method_invoke record "name" arg1 ...) → value.
    // Dispatches by the record's blueprint; the method's FIRST param is the
    // receiver (Python `self`), remaining params bind to call args.
    this.registerNative("method_invoke", (k, args) => {
      const a0 = args[0]!;
      if (a0.kind !== "record") {
        throw new Error("method_invoke: first arg must be a record");
      }
      const bp = a0.record.blueprint;
      if (bp === null) {
        throw new Error(
          `method_invoke: no method '${argStr(args, 1)}' on a record with no blueprint (record_new 0)`,
        );
      }
      const key = `${nodeKey(bp)}:${k.internName(argStr(args, 1))}`;
      const cl = k.methods.get(key);
      if (!cl) {
        throw new Error(
          `method_invoke: no method '${argStr(args, 1)}' on blueprint @${nodeKey(bp)}`,
        );
      }
      const callArgs = args.slice(2);
      if (cl.params.length === 0) {
        throw new Error(
          `method '${argStr(args, 1)}' must declare a receiver param (self)`,
        );
      }
      if (callArgs.length !== cl.params.length - 1) {
        throw new Error(
          `method '${argStr(args, 1)}' wants ${cl.params.length - 1} args, got ${callArgs.length}`,
        );
      }
      const callFrame = new Frame(cl.env);
      callFrame.bind(cl.params[0]!, a0); // receiver
      for (let i = 0; i < callArgs.length; i++) {
        callFrame.bind(cl.params[i + 1]!, callArgs[i]!);
      }
      return walk(k, cl.body, callFrame);
    });
    // str_find — JS-level substring search starting at index `from`.
    // (str_find s needle from) → int (index or -1). Whole search in this
    // JS String.indexOf call; no Form callback per byte, no Form recursion.
    this.registerNative("str_find", (_k, args) => {
      const s = argStr(args, 0);
      const needle = argStr(args, 1);
      const bytes = bstrToBytes(s);
      const needleBytes = bstrToBytes(needle);
      const rawFrom = Math.max(0, argInt(args, 2));
      // A start past the end finds nothing — including the empty needle, which
      // this arm otherwise reported as found AT the clamped end. Sibling parity
      // with the Go kernel's `if from > len(s) { return -1 }` and with rust and
      // fkwu: measured 2026-09-07, str_find(h, "", 99) read 14 here and -1 on
      // the other three, and str_find("", "", 3) read 0 here and -1 there. The
      // band that would have caught it (core-str-find-equivalence-band, bit 16)
      // could not run on this arm at all until substring stopped dying.
      if (rawFrom > bytes.length) return { kind: "int", int: -1 };
      // BYTES, CLAMPED, NEVER DIES — the one meaning, held by all four arms since
      // 2026-09-08 (form-stdlib/tests/str-find-one-meaning-band.fk, 8191 four
      // ways, a drift gate). Until then `rawFrom` was snapped UP to the next UTF-8
      // character start, in sibling parity with Go and Rust. It is a no-op for any
      // needle this arm can hold — a valid needle never begins on a continuation
      // byte — and where a needle IS a byte fragment (which fkwu and Go can hold
      // and this arm cannot) it skipped real matches. The scan already walks the
      // encoded bytes, so there is nothing here that needs a boundary.
      const from = rawFrom;
      const idx = byteSubarrayIndex(bytes, needleBytes, from);
      // `kind: "int"` carries a JS Number — using BigInt here would
      // poison downstream arithmetic with "Cannot mix BigInt and other
      // types" when callers do plain int math on the result.
      return { kind: "int", int: idx };
    });
    // scan_run — return the end-index where a contiguous run of bytes
    // matching `class_code` ends (exclusive). Sibling parity with Go +
    // Rust scan_run. Generic per-byte loop in JS avoids the walker
    // dispatch a pure-Form recursion would pay per character.
    // Class codes: 0=ws, 1=digit, 2=alpha, 3=identifier-char,
    //              4=non-quote-non-escape, 5=non-newline,
    //              6=json-string-safe (code unit >= 0x20, not quote/backslash).
    this.registerNative("scan_run", (_k, args) => {
      const bytes = bstrToBytes(argStr(args, 0));
      const from = Math.max(0, argInt(args, 1));
      const cls = argInt(args, 2);
      const n = bytes.length;
      let end = Math.min(from, n);
      switch (cls) {
        case 0: { // whitespace
          while (end < n) {
            const c = bytes[end]!;
            if (c !== 32 && c !== 9 && c !== 10 && c !== 13) break;
            end++;
          }
          break;
        }
        case 1: { // ascii digit
          while (end < n) {
            const c = bytes[end]!;
            if (c < 48 || c > 57) break;
            end++;
          }
          break;
        }
        case 2: { // ascii alpha
          while (end < n) {
            const c = bytes[end]!;
            if (!((c >= 97 && c <= 122) || (c >= 65 && c <= 90))) break;
            end++;
          }
          break;
        }
        case 3: { // identifier char
          while (end < n) {
            const c = bytes[end]!;
            const isAlnum = (c >= 97 && c <= 122) || (c >= 65 && c <= 90) || (c >= 48 && c <= 57);
            if (!(isAlnum || c === 95 || c === 45)) break;
            end++;
          }
          break;
        }
        case 4: { // non-quote-non-escape
          while (end < n) {
            const c = bytes[end]!;
            if (c === 34 || c === 92) break;
            end++;
          }
          break;
        }
        case 5: { // non-newline
          while (end < n) {
            if (bytes[end]! === 10) break;
            end++;
          }
          break;
        }
        case 6: { // json-string-safe
          while (end < n) {
            const c = bytes[end]!;
            if (c < 0x20 || c === 34 || c === 92) break;
            end++;
          }
          break;
        }
        default:
          throw new Error(`scan_run: unknown class_code ${cls} (valid: 0-6)`);
      }
      return { kind: "int", int: end };
    });
    // str_eq OBSERVES the axiom-1 absence instead of refusing it, mirroring the fkwu
    // arm exactly (probed 2026-09-04: nothing equals nothing, and equals neither ""
    // nor any other string, so the emptymask distinction between never-was and empty
    // survives the comparison). A comparison asks a question ABOUT two values; a
    // length MEASURES one, which is why str_len and str_byte_at still refuse an
    // absence out loud. A walk that meets a file which left between the listing
    // and the read answers here as fkwu does ("not a model"), so this witness
    // stays in step with the primary kernel.
    this.registerNative("str_eq", (_k, args) =>
      args[0]?.kind === "null" || args[1]?.kind === "null"
        ? boolInt(args[0]?.kind === "null" && args[1]?.kind === "null")
        : boolInt(argStr(args, 0) === argStr(args, 1)),
    );
    // value_str — a value as text (law 9): nothing as "", anything else as renderValue writes it.
    this.registerNative("value_str", (_k, args) => {
      const v = args[0];
      return { kind: "str", str: v === undefined || v.kind === "null" ? "" : this.renderValue(v, "null") };
    });
    // str_to_float reads one grammar (law 7): leading whitespace, the longest decimal prefix,
    // no hex, no inf/nan; text with no decimal prefix reads 0.0.
    this.registerNative("str_to_float", (_k, args) => {
      const m = FLOAT_PREFIX.exec(argStr(args, 0));
      return { kind: "f64", float: m === null ? 0 : Number(m[1]) };
    });
    // float_to_int truncates toward zero; NaN, a value outside the integer range or a
    // non-number stops (law 6).
    this.registerNative("float_to_int", (_k, args) => {
      const v = args[0];
      if (v?.kind === "int" || v?.kind === "i64") return v;
      if (v?.kind !== "f64") throw new Error(`float_to_int: only a number truncates, got ${v?.kind ?? "nothing"}`);
      const t = Math.trunc(v.float);
      if (!(t >= -INT_LIMIT && t < INT_LIMIT)) throw new Error(`float_to_int: ${formatFloat(v.float)} has no integer`);
      return intOrWide(BigInt(t));
    });
    // str_byte_at: the i-th BYTE of the string (0-255), -1 out of range (law 8).
    this.registerNative("str_byte_at", (_k, args) => {
      const s = argStr(args, 0);
      const i = argInt(args, 1);
      return { kind: "int", int: i < 0 || i >= s.length ? -1 : s.charCodeAt(i) & 0xff };
    });
    this.registerNative("byte_to_str", (_k, args) => {
      const b = argInt(args, 0);
      return { kind: "str", str: b >= 0 && b <= 255 ? String.fromCharCode(b) : "" };
    });
    // input_byte — this kernel stages no input: every index is outside it and reads 0.
    this.registerNative("input_byte", () => ({ kind: "int", int: 0 }));
    // List ops: cons, head, tail, nth and len run on SharedList in constant time.
    this.registerNative("list", (_k, args) => new SharedList(args.reverse(), args.length));
    this.registerNative("cons", (k, args) => {
      const head = args[0] ?? { kind: "null" };
      // nothing is not a list: consing onto it is a stop, as on fkwu
      if (args[1]?.kind === "null") throw new Error("cons: nothing is not a list -- ask nothing? before consing");
      const xs = args[1];
      if (xs?.kind !== "list") throw new Error(`cons: expected list, got ${xs?.kind ?? "absent"}`);
      const tail = sharedList(k, xs);
      // the longest view of a buffer grows it in place; any other view copies its own cells
      if (tail.buf.length === tail.n) {
        tail.buf.push(head);
        return new SharedList(tail.buf, tail.n + 1);
      }
      k.noteListCopy(tail.n);
      const buf = tail.buf.slice(0, tail.n);
      buf.push(head);
      return new SharedList(buf, tail.n + 1);
    });
    // A receiver that is not a list answers null, as nth does; the tail of a list is a list.
    this.registerNative("head", (_k, args) => {
      const xs = args[0];
      return xs?.kind === "list" ? listAt(xs, 0) : { kind: "null" };
    });
    this.registerNative("tail", (k, args) => {
      const xs = args[0];
      if (xs?.kind !== "list") return { kind: "null" };
      const shared = sharedList(k, xs);
      return new SharedList(shared.buf, Math.max(0, shared.n - 1));
    });
    // len is the honest cell count: a list's cells, a string's bytes.
    this.registerNative("len", (_k, args) => {
      const v = args[0];
      if (v?.kind === "list") return { kind: "int", int: listLength(v) };
      if (v?.kind === "str") return { kind: "int", int: v.str.length };
      // nothing is not an empty collection: its length is a stop, as on fkwu
      if (v?.kind === "null") throw new Error("len: nothing has no length -- ask nothing? before measuring");
      return { kind: "int", int: 0 };
    });
    // A receiver that is not a list answers null, as Go and Rust answer.
    this.registerNative("nth", (_k, args) => {
      const xs = args[0];
      return xs?.kind === "list" ? listAt(xs, argInt(args, 1)) : { kind: "null" };
    });
    this.registerNative("empty", () => new SharedList([], 0));
    // _get — polymorphic subscript over a "__dict__"-tagged pair list (dict[k]), a record
    // alist (string key), a list index or a string byte.
    this.registerNative("_get", (_k, args) => {
      const v = args[0]!;
      const idx = args[1]!;
      if (v.kind === "list" && v.list[0]?.kind === "str" && v.list[0].str === "__dict__") {
        const xs = v.list;
        for (let i = 1; i + 1 < xs.length; i += 2) {
          const key = xs[i]!;
          if (
            (key.kind === "str" && idx.kind === "str" && key.str === idx.str) ||
            (key.kind === "int" && idx.kind === "int" && key.int === idx.int)
          ) {
            return xs[i + 1]!;
          }
        }
        return { kind: "null" };
      }
      // String key on an untagged list → record-field read. A Python class
      // instance is a flat alist (list "__class__" "Counter" "n" 3 …).
      // Mirrors the Rust _get (Value::List, Value::Str) arm: walk pairs from
      // slot 0, match the string key, return the following value; an absent
      // field throws (Rust panics) rather than falling into the index path.
      if (v.kind === "list" && idx.kind === "str") {
        for (let i = 0; i + 1 < v.list.length; i += 2) {
          const kk = v.list[i]!;
          if (kk.kind === "str" && kk.str === idx.str) return v.list[i + 1]!;
        }
        throw new Error(`_get: no field '${idx.str}' on record`);
      }
      if (v.kind === "list") {
        const i = idx.kind === "int" ? idx.int : 0;
        if (i < 0 || i >= v.list.length) return { kind: "null" };
        return v.list[i]!;
      }
      if (v.kind === "str") {
        const i = idx.kind === "int" ? idx.int : 0;
        if (i < 0 || i >= v.str.length) return { kind: "str", str: "" };
        return { kind: "str", str: v.str[i] ?? "" };
      }
      return { kind: "null" };
    });
    // --- Substrate read primitives — kernel reaches the REST surface ----
    // Sibling-parity with the Go/Rust http_get carrier. The walker remains
    // synchronous through a worker-backed Node HTTP client; no shell/curl
    // projection participates in the data lane.
    //
    // http_get(url, headers?, timeout_ms?) → __dict__:
    // status_code, body, error, duration_ms, headers.
    this.registerNative("http_get", (_k, args) => {
      const url = argStr(args, 0);
      const headers: globalThis.Record<string, string[]> = {};
      if (args[1]?.kind === "list") {
        for (const row of args[1].list) {
          if (row.kind !== "list" || row.list.length !== 3) continue;
          const [tag, name, value] = row.list;
          if (
            tag?.kind !== "int" ||
            tag.int !== KH_TAG_HEADER_TS ||
            name?.kind !== "str" ||
            value?.kind !== "str" ||
            name.str.trim() === ""
          ) {
            continue;
          }
          const key = name.str.trim();
          headers[key] = [...(headers[key] ?? []), value.str];
        }
      }
      const timeoutMs = args[2] ? Math.min(Math.max(argInt(args, 2), 1), 60000) : 30000;
      const httpGet = this.host.httpGet;
      if (httpGet === undefined) throw new Error("http_get: host carrier unavailable");
      const result: KernelHttpResult = httpGet({ url, headers, timeoutMs });
      const headerRows: Value[] = [];
      if (Array.isArray(result.headers)) {
        for (const row of result.headers) {
          if (!Array.isArray(row) || row.length !== 3) continue;
          const [tag, name, value] = row;
          if (tag !== KH_TAG_HEADER_TS || typeof name !== "string" || typeof value !== "string") continue;
          headerRows.push({
            kind: "list",
            list: [
              { kind: "int", int: KH_TAG_HEADER_TS },
              { kind: "str", str: name },
              { kind: "str", str: value },
            ],
          });
        }
      }
      return {
        kind: "list",
        list: [
          { kind: "str", str: "__dict__" },
          { kind: "str", str: "status_code" },
          { kind: "int", int: typeof result.statusCode === "number" ? result.statusCode : 0 },
          { kind: "str", str: "body" },
          { kind: "str", str: typeof result.body === "string" ? result.body : "" },
          { kind: "str", str: "error" },
          { kind: "str", str: typeof result.error === "string" ? result.error : "" },
          { kind: "str", str: "duration_ms" },
          { kind: "int", int: typeof result.durationMs === "number" ? result.durationMs : 0 },
          { kind: "str", str: "headers" },
          { kind: "list", list: headerRows },
        ],
      };
    });
    // math_pi — the circle constant; fkwu answers it as a float_leaf rewrite row.
    this.registerNative("math_pi", () => ({ kind: "f64", float: Math.PI }));
    // math_sqrt is IEEE fsqrt, correctly rounded (law 4); the rest are the host's libm.
    this.registerNative("math_sqrt", (_k, args) => {
      return { kind: "f64", float: Math.sqrt(argFloat(args, 0)) };
    });
    // math.pow — always returns float, matching CPython's behaviour.
    // (CPython's `math.pow(2, 3)` returns `8.0`, not `8`. The built-in
    // `pow()` would return int for int arguments; we don't expose that.)
    this.registerNative("math_pow", (_k, args) => {
      return {
        kind: "f64",
        float: Math.pow(argFloat(args, 0), argFloat(args, 1)),
      };
    });
    this.registerNative("math_log", (_k, args) => {
      return { kind: "f64", float: Math.log(argFloat(args, 0)) };
    });
    this.registerNative("math_exp", (_k, args) => {
      return { kind: "f64", float: Math.exp(argFloat(args, 0)) };
    });
    // round_ndigits(x, n) — CPython `round(x, n)` for floats, EXACTLY.
    // The Python adapter lowers `round(x, n)` → `(round_ndigits x n)`. Rounds
    // the exact decimal value of the double half-to-even at n fractional
    // places (n >= 0), matching CPython bit-for-bit. Sibling-parity with the
    // Rust + Go kernels. See roundNdigitsDecimal above.
    this.registerNative("round_ndigits", (_k, args) => {
      return {
        kind: "f64",
        float: roundNdigitsDecimal(argFloat(args, 0), argInt(args, 1)),
      };
    });
    // File I/O
    const readFileTextNative = (_k: Kernel, args: Value[]): Value => {
      try {
        const read = this.host.readTextFile;
        if (read === undefined) return { kind: "null" };
        return { kind: "str", str: read(this.hostReadPath(argStr(args, 0))) };
      } catch {
        return { kind: "null" };
      }
    };
    this.registerNative("host_file_read_text", readFileTextNative);
    this.registerNative("read_file", readFileTextNative);
    // Byte-level host file read — returns a list of ints (0-255), one per byte.
    this.registerNative("read_file_bytes", (_k, args) => {
      try {
        const read = this.host.readBinaryFile;
        if (read === undefined) return { kind: "null" };
        const buf = read(this.hostReadPath(argStr(args, 0)));
        const out: Value[] = new Array(buf.length);
        for (let i = 0; i < buf.length; i++) {
          out[i] = { kind: "int", int: buf[i]! };
        }
        return { kind: "list", list: out };
      } catch {
        return { kind: "null" };
      }
    });
    // source_inventory(root, suffix, skip-dir-names) — generic source
    // inventory primitive. Returns rows of [relative-path, line-count].
    // Form owns classification and aggregation; the kernel only exposes
    // filesystem walking and text line counts as primitive observation.
    this.registerNative("source_inventory", (_k, args) => {
      try {
        const inventory = this.host.sourceInventory;
        if (inventory === undefined) return { kind: "null" };
        const root = this.hostReadPath(argStr(args, 0));
        const suffix = argStr(args, 1);
        const skip = sourceInventorySkipSet(args[2] ?? { kind: "null" });
        const rows = inventory(root, suffix, skip).map((entry) =>
          sourceInventoryRow(entry.path, entry.lines),
        );
        return { kind: "list", list: rows };
      } catch {
        return { kind: "null" };
      }
    });
    // random_bytes(n) — open the doorway. Reads n bytes from
    // /dev/urandom every call. Different per invocation, per kernel
    // invocation. lc-divergence-is-the-doorway: this native intentionally
    // violates sibling parity when invoked — the divergence is the
    // substrate's signal of live field-touch.
    this.registerNative("random_bytes", (_k, args) => {
      const n = argInt(args, 0);
      if (n <= 0) return { kind: "list", list: [] };
      try {
        const randomBytes = this.host.randomBytes;
        if (randomBytes === undefined) return { kind: "null" };
        const buf = randomBytes(n);
        if (buf.length !== n) return { kind: "null" };
        const out: Value[] = new Array(n);
        for (let i = 0; i < n; i++) {
          out[i] = { kind: "int", int: buf[i]! };
        }
        return { kind: "list", list: out };
      } catch {
        return { kind: "null" };
      }
    });
    // ---- bitwise primitives -----------------------------------
    // True kernel primitives — cannot be expressed in pure Form
    // without exponential cost. band/bor/bxor combine the whole int64
    // word, as Go's and Rust's operators do (bitwiseInt); the _u32 doors
    // narrow to a 32-bit word so SHA-256-style recipes compose round
    // functions over machine words.
    this.registerNative("band", (_k, args) =>
      bitwiseInt(args, (a, b) => a & b, (a, b) => a & b),
    );
    this.registerNative("bor", (_k, args) =>
      bitwiseInt(args, (a, b) => a | b, (a, b) => a | b),
    );
    this.registerNative("bxor", (_k, args) =>
      bitwiseInt(args, (a, b) => a ^ b, (a, b) => a ^ b),
    );
    this.registerNative("bnot_u32", (_k, args) => ({
      kind: "int",
      int: ~argInt(args, 0) >>> 0,
    }));
    this.registerNative("shl_u32", (_k, args) => ({
      kind: "int",
      int: (argInt(args, 0) << (argInt(args, 1) & 31)) >>> 0,
    }));
    this.registerNative("shr_u32", (_k, args) => ({
      kind: "int",
      int: argInt(args, 0) >>> (argInt(args, 1) & 31),
    }));
    this.registerNative("rotr_u32", (_k, args) => {
      const a = argInt(args, 0) >>> 0;
      const n = argInt(args, 1) & 31;
      return { kind: "int", int: ((a >>> n) | (a << (32 - n))) >>> 0 };
    });
    // add_u32: modular 32-bit addition — SHA-256's round constants
    // and message schedule both require this discipline.
    this.registerNative("add_u32", (_k, args) => ({
      kind: "int",
      int: (argInt(args, 0) + argInt(args, 1)) >>> 0,
    }));
    // recipe_to_bytes nid → list-of-bytes (or null on error).
    //   Serializes a Recipe subtree to the .fkb wire format as a byte
    //   list — usable over any byte channel without a file detour.
    this.registerNative("recipe_to_bytes", (k, args) => {
      const nid = argNodeID(args, 0);
      const bytes = serializeRecipeArtifact(k, nid);
      const out: Value[] = new Array(bytes.length);
      for (let i = 0; i < bytes.length; i++) {
        out[i] = { kind: "int", int: bytes[i]! };
      }
      return { kind: "list", list: out };
    });
    // bytes_to_recipe bytes-list → nid (or null on parse error).
    this.registerNative("bytes_to_recipe", (k, args) => {
      const a0 = args[0];
      if (!a0 || a0.kind !== "list") return { kind: "null" };
      const bytes = new Uint8Array(a0.list.length);
      for (let i = 0; i < a0.list.length; i++) {
        const v = a0.list[i];
        bytes[i] = v && v.kind === "int" ? v.int & 0xff : 0;
      }
      try {
        const nid = deserializeRecipeArtifact(k, bytes);
        return { kind: "nodeid", nodeid: nid };
      } catch {
        return { kind: "null" };
      }
    });
    this.registerNative("read_form_binary", (k, args) => {
      try {
        const read = this.host.readBinaryFile;
        if (read === undefined) return { kind: "null" };
        return {
          kind: "nodeid",
          nodeid: deserializeRecipeArtifact(k, read(this.hostReadPath(argStr(args, 0)))),
        };
      } catch {
        return { kind: "null" };
      }
    });
    this.registerNative("write_form_binary", (k, args) => {
      try {
        const write = this.host.writeBinaryFile;
        if (write === undefined) return { kind: "int", int: -1 };
        const bytes = serializeRecipeArtifact(k, argNodeID(args, 1));
        write(argStr(args, 0), bytes);
        return { kind: "int", int: bytes.length };
      } catch {
        return { kind: "int", int: -1 };
      }
    });
    const fileSizeNative = (_k: Kernel, args: Value[]): Value => {
      try {
        const fileSize = this.host.fileSize;
        return { kind: "int", int: fileSize?.(this.hostReadPath(argStr(args, 0))) ?? -1 };
      } catch {
        return { kind: "int", int: -1 };
      }
    };
    this.registerNative("host_file_size", fileSizeNative);
    this.registerNative("file_size", fileSizeNative);
    // file_mtime — modification time in unix seconds; -1 if missing.
    // Sibling parity with Go + Rust file_mtime; powers Form-side cache
    // layers that regenerate .fkb projections when source files drift.
    const fileMtimeNative = (_k: Kernel, args: Value[]): Value => {
      try {
        const fileMtime = this.host.fileMtimeSeconds;
        return { kind: "int", int: fileMtime?.(this.hostReadPath(argStr(args, 0))) ?? -1 };
      } catch {
        return { kind: "int", int: -1 };
      }
    };
    this.registerNative("host_file_mtime", fileMtimeNative);
    this.registerNative("file_mtime", fileMtimeNative);
    // A slice that could not be read answers nothing; "" is a read of zero bytes.
    const readFileSliceNative = (_k: Kernel, args: Value[]): Value => {
      const offset = argInt(args, 1);
      const length = argInt(args, 2);
      if (length <= 0) return { kind: "str", str: "" };
      if (offset < 0) return { kind: "null" };
      const read = this.host.readBinarySlice;
      if (read === undefined) return { kind: "null" };
      try {
        // the slice's own bytes: a string is bytes (byte-host.ts), binary included
        return { kind: "str", str: bytesToBstr(read(this.hostReadPath(argStr(args, 0)), offset, length)) };
      } catch {
        return { kind: "null" };
      }
    };
    this.registerNative("host_file_read_slice", readFileSliceNative);
    this.registerNative("read_file_slice", readFileSliceNative);

    // --- Filesystem CRUD natives — real directories + files ----------
    // Sibling parity across Go/Rust/TS. Predicates return 1/0; mutations
    // return 0 on success, -1 on error; fs_list returns a name-string list
    // (sorted for cross-kernel parity) or null on error.
    const fsExistsNative = (_k: Kernel, args: Value[]): Value => {
      try {
        return {
          kind: "int",
          int: this.host.pathExists?.(this.hostReadPath(argStr(args, 0))) === true ? 1 : 0,
        };
      } catch {
        return { kind: "int", int: 0 };
      }
    };
    this.registerNative("host_path_exists", fsExistsNative);
    this.registerNative("fs_exists", fsExistsNative);
    const fsIsDirNative = (_k: Kernel, args: Value[]): Value => {
      try {
        return {
          kind: "int",
          int: this.host.pathIsDirectory?.(this.hostReadPath(argStr(args, 0))) === true ? 1 : 0,
        };
      } catch {
        return { kind: "int", int: 0 };
      }
    };
    this.registerNative("host_path_is_dir", fsIsDirNative);
    this.registerNative("fs_is_dir", fsIsDirNative);
    // One atomic mkdir, as fkwu's tag 56: 1 when this call created the directory, 0 when it
    // already stood or could not be made — the answer a lock directory reads.
    const fsMkdirNative = (_k: Kernel, args: Value[]): Value => {
      try {
        const mkdir = this.host.makeDirectory;
        if (mkdir === undefined) return { kind: "int", int: 0 };
        mkdir(argStr(args, 0));
        return { kind: "int", int: 1 };
      } catch {
        return { kind: "int", int: 0 };
      }
    };
    this.registerNative("host_dir_mkdir", fsMkdirNative);
    this.registerNative("fs_mkdir", fsMkdirNative);
    const fsRmdirNative = (_k: Kernel, args: Value[]): Value => {
      try {
        const path = argStr(args, 0);
        const remove = this.host.removeDirectory;
        if (remove === undefined || this.host.pathIsDirectory?.(path) !== true) {
          return { kind: "int", int: -1 };
        }
        remove(path);
        return { kind: "int", int: 0 };
      } catch {
        return { kind: "int", int: -1 };
      }
    };
    this.registerNative("host_dir_rmdir", fsRmdirNative);
    this.registerNative("fs_rmdir", fsRmdirNative);
    const fsRemoveNative = (_k: Kernel, args: Value[]): Value => {
      try {
        const path = argStr(args, 0);
        const remove = this.host.removePath;
        if (remove === undefined || this.host.pathIsDirectory?.(path) === true) {
          return { kind: "int", int: -1 };
        }
        remove(path);
        return { kind: "int", int: 0 };
      } catch {
        return { kind: "int", int: -1 };
      }
    };
    this.registerNative("host_path_remove", fsRemoveNative);
    this.registerNative("fs_remove", fsRemoveNative);
    const fsRenameNative = (_k: Kernel, args: Value[]): Value => {
      try {
        const rename = this.host.renamePath;
        if (rename === undefined) return { kind: "int", int: -1 };
        rename(argStr(args, 0), argStr(args, 1));
        return { kind: "int", int: 0 };
      } catch {
        return { kind: "int", int: -1 };
      }
    };
    this.registerNative("host_path_rename", fsRenameNative);
    this.registerNative("fs_rename", fsRenameNative);
    const fsListNative = (_k: Kernel, args: Value[]): Value => {
      try {
        // sort by name for cross-kernel parity (Go's os.ReadDir is
        // name-sorted; Rust/Node are OS-arbitrary).
        const list = this.host.listDirectory;
        if (list === undefined) return { kind: "null" };
        const names = [...list(this.hostReadPath(argStr(args, 0)))].sort();
        return { kind: "list", list: names.map((n) => ({ kind: "str", str: n }) as Value) };
      } catch {
        return { kind: "null" };
      }
    };
    this.registerNative("host_dir_list", fsListNative);
    this.registerNative("fs_list", fsListNative);

    // write_file_bytes — sibling of read_file_bytes; writes a byte list.
    // Sibling-parity with form-kernel-go + form-kernel-rust. Values out of
    // 0..255 truncate per Go's `byte(v.Int)` and Rust's `as u8`.
    this.registerNative("write_file_bytes", (_k, args) => {
      try {
        const path = argStr(args, 0);
        const list = argList(args, 1);
        const buf = new Uint8Array(list.length);
        for (let i = 0; i < list.length; i++) {
          buf[i] = argInt(list, i) & 0xff;
        }
        const write = this.host.writeBinaryFile;
        if (write === undefined) return { kind: "int", int: -1 };
        write(path, buf);
        return { kind: "int", int: buf.length };
      } catch {
        return { kind: "int", int: -1 };
      }
    });
    // file_append_bytes path bytes-list → new-file-size | -1. Atomic append
    // (O_APPEND) — the missing primitive for a log-structured store. Unlike
    // write_file_bytes (truncates), this appends at end-of-file and returns
    // the new total size. Creates the file if absent.
    const fileAppendBytesNative = (_k: Kernel, args: Value[]): Value => {
      try {
        const path = argStr(args, 0);
        const list = argList(args, 1);
        const buf = new Uint8Array(list.length);
        for (let i = 0; i < list.length; i++) {
          buf[i] = argInt(list, i) & 0xff;
        }
        const append = this.host.appendBinaryFile;
        if (append === undefined) return { kind: "int", int: -1 };
        return { kind: "int", int: append(path, buf) };
      } catch {
        return { kind: "int", int: -1 };
      }
    };
    this.registerNative("host_file_append_bytes", fileAppendBytesNative);
    this.registerNative("file_append_bytes", fileAppendBytesNative);
    // Host text output. Byte codecs still use write_file_bytes in kernels
    // that expose it; text compilers do not need to materialize byte lists.
    const writeFileTextNative = (_k: Kernel, args: Value[]): Value => {
      try {
        const text = argStr(args, 1);
        const write = this.host.writeTextFile;
        if (write === undefined) return { kind: "int", int: -1 };
        write(argStr(args, 0), text);
        return { kind: "int", int: isWide(text) ? utf8Encode(text).length : text.length };
      } catch {
        return { kind: "int", int: -1 };
      }
    };
    this.registerNative("host_file_write_text", writeFileTextNative);
    this.registerNative("write_file", writeFileTextNative);
    this.registerNative("write_file_text", writeFileTextNative);

    // --- Socket natives — L1 physical layer for inter-cell IO ---------
    // Sibling parity with form-kernel-go + form-kernel-rust: REAL TCP. The
    // synchronous worker-thread shim (see socketCall above) gives the TS
    // kernel blocking listen/accept/connect/send/recv/close identical in
    // surface and behavior to the Go (net.Listen/Dial) and Rust (std::net)
    // kernels. Handle = int (≥0 success, -1 error); socket_recv answers the
    // received string, or nothing on close or error. The worker spawns lazily
    // on first socket use, so non-socket programs pay nothing.
    this.registerNative("socket_listen", (_k, args) => ({
      kind: "int",
      int: socketNumber(this.host, { op: "listen", port: argInt(args, 0) }),
    }));
    // (socket_port listener-handle) → bound TCP port | -1 — sibling of the
    // Go/Rust native; reports an ephemeral (port 0) listener's OS-assigned
    // port for single-process loopback.
    this.registerNative("socket_port", (_k, args) => ({
      kind: "int",
      int: socketNumber(this.host, { op: "port", h: argInt(args, 0) }),
    }));
    this.registerNative("socket_accept", (_k, args) => ({
      kind: "int",
      int: socketNumber(this.host, { op: "accept", h: argInt(args, 0) }),
    }));
    this.registerNative("socket_connect", (_k, args) => ({
      kind: "int",
      int: socketNumber(this.host, {
        op: "connect",
        host: argStr(args, 0),
        port: argInt(args, 1),
      }),
    }));
    this.registerNative("socket_send", (_k, args) => ({
      kind: "int",
      int: socketNumber(this.host, {
        op: "send",
        h: argInt(args, 0),
        text: argStr(args, 1),
      }),
    }));
    this.registerNative("socket_recv", (_k, args) => {
      const max = argInt(args, 1);
      if (max <= 0) return { kind: "str", str: "" };
      const received = callSocket(this.host, {
        op: "recv",
        h: argInt(args, 0),
        max,
      });
      return typeof received === "string" ? { kind: "str", str: received } : { kind: "null" };
    });
    this.registerNative("socket_close", (_k, args) => {
      const h = argInt(args, 0);
      if (h < 0) return { kind: "int", int: -1 };
      return { kind: "int", int: socketNumber(this.host, { op: "close", h }) };
    });

    // ---- the storage port: pg_* and config_* ---------------------------
    // Siblings to the Go (pgx) and Rust (postgres) natives. The host's pg carrier speaks the wire
    // protocol; a cell reads as Go reads it (pgCell) and pg_query writes it as Go's formValueString
    // does. pg_last_error names the last failure of these doors, "" after a success.
    const pgAnswer = (error: string, value: Value): Value => {
      this.pgLastError = error;
      return value;
    };
    const pgCall = (operation: KernelPgOperation): KernelPgAnswer => {
      const call = this.host.pgCall;
      if (call === undefined) throw new Error("pg: host carrier unavailable");
      return call(operation);
    };
    // a parameter travels as text, as the Go native hands pgx its value
    const pgParam = (v: Value): string | null => (v.kind === "null" ? null : this.renderValue(v, "null"));
    // one SQL run on a handle; params, when a list, travel by Parse/Bind/Execute
    const pgRun = (op: string, handle: Value | undefined, sql: string, params: Value | undefined): KernelPgAnswer => {
      const answer = pgCall({
        op: "run",
        h: handle === undefined ? -1 : argInt([handle], 0),
        sql,
        params: params?.kind === "list" ? params.list.map(pgParam) : null,
      });
      return answer.error === "unknown connection handle" ? { error: `${op}: unknown connection handle` } : answer;
    };
    this.registerNative("pg_last_error", () => ({ kind: "str", str: this.pgLastError }));
    this.registerNative("pg_connect", (_k, args) => {
      const url = argStr(args, 0).trim();
      if (!url.startsWith("postgres://") && !url.startsWith("postgresql://")) {
        return pgAnswer("pg_connect: database.url is not a PostgreSQL URL", { kind: "int", int: -1 });
      }
      const answer = pgCall({ op: "connect", ...pgDsn(url) });
      if (answer.error !== undefined) return pgAnswer(answer.error, { kind: "int", int: -1 });
      return pgAnswer("", { kind: "int", int: answer.handle ?? -1 });
    });
    this.registerNative("pg_ping", (_k, args) => {
      const answer = pgRun("pg_ping", args[0], "SELECT 1", undefined);
      return pgAnswer(answer.error ?? "", boolInt(answer.error === undefined));
    });
    this.registerNative("pg_close", (_k, args) => {
      const answer = pgCall({ op: "close", h: argInt(args, 0) });
      return { kind: "int", int: answer.error === undefined ? 0 : -1 };
    });
    this.registerNative("pg_exec", (_k, args) => {
      const answer = pgRun("pg_exec", args[0], argStr(args, 1), args[2]);
      if (answer.error !== undefined) return pgAnswer(answer.error, { kind: "int", int: -1 });
      return pgAnswer("", intOrWide(pgTagCount(answer.tag ?? "")));
    });
    this.registerNative("pg_query_rows", (_k, args) => {
      const answer = pgRun("pg_query_rows", args[0], argStr(args, 1), args[2]);
      if (answer.error !== undefined) return pgAnswer(answer.error, { kind: "list", list: [] });
      const fields = answer.fields ?? [];
      const rows = (answer.rows ?? []).map((row): Value => ({
        kind: "list",
        list: [
          { kind: "str", str: "__dict__" },
          ...row.flatMap((cell, i): Value[] => [
            { kind: "str", str: fields[i]?.name ?? "" },
            pgCell(fields[i]?.oid ?? 25, cell),
          ]),
        ],
      }));
      return pgAnswer("", { kind: "list", list: rows });
    });
    this.registerNative("config_database_url", () => {
      try {
        const url = kernelConfigDatabaseUrl(kernelConfigLoad(this.host));
        if (url.length > 0) return pgAnswer("", { kind: "str", str: url });
        return pgAnswer("database.url is not configured", { kind: "str", str: "" });
      } catch (error) {
        return pgAnswer(error instanceof Error ? error.message : String(error), { kind: "str", str: "" });
      }
    });
    this.registerNative("config_value_or", (_k, args) => {
      const fallback = args[1] ?? { kind: "null" };
      try {
        return kernelConfigValue(kernelConfigLookup(kernelConfigLoad(this.host), argStr(args, 0)), fallback);
      } catch (error) {
        return pgAnswer(error instanceof Error ? error.message : String(error), fallback);
      }
    });

    // Substrate write surface — all attributed as WITNESS.
    this.registerNative("make_nodeid", (_k, args) => ({
      kind: "nodeid",
      nodeid: {
        pkg: argInt(args, 0),
        level: argInt(args, 1),
        type: argInt(args, 2),
        inst: argInt(args, 3),
      },
    }));
    // bp — Blueprint name → NodeID, looked up in the generated BP_TABLE.
    // Unknown name resolves to the undefined node (1,2,0,0).
    this.registerNative("bp", (_k, args) => {
      const name = argStr(args, 0);
      const entry = BP_TABLE[name];
      if (entry === undefined) {
        // Fail loud — never invent a NodeID for an unknown name. The old silent
        // fallback to [1,2,0,0] collapsed every unregistered name onto one
        // NodeID, so distinct blueprints collided invisibly. An unregistered
        // name is a missing registration, not a valid shape. Sibling parity:
        // Go panics, Rust panics.
        throw new Error(
          `bp: unregistered blueprint name ${JSON.stringify(name)} — register it: ` +
            `add its row to form/form-stdlib/blueprint-registry.json and carry the same coordinates into bp_table.ts. ` +
            `The substrate never invents a NodeID for an unknown name.`,
        );
      }
      const [pkg, level, type, inst] = entry;
      return { kind: "nodeid", nodeid: { pkg, level, type, inst } };
    });
    this.registerNative("intern_trivial_int", (k, args) => ({
      kind: "nodeid",
      nodeid: k.internTrivialInt(argInt(args, 0)),
    }));
    this.registerNative("intern_trivial_string", (k, args) => ({
      kind: "nodeid",
      nodeid: k.internString(argStr(args, 0)),
    }));
    this.registerNative("intern_trivial_bool", (k, args) => ({
      kind: "nodeid",
      nodeid: k.internTrivialBool(truthy(args[0]!)),
    }));
    // intern_trivial_float — content-address an IEEE-754 f64 into the overflow
    // table and return its trivial NodeID. The string argument is the float's
    // source text (e.g. "0.5"); a parse failure lands on +0.0 so the witness is
    // total like str_to_int. Sibling of intern_trivial_int / intern_trivial_string;
    // exposes the existing internTrivialFloat64 to Form code so the python-bmf
    // float-literal lift can build a PY-BMF-FLOAT leaf.
    this.registerNative("intern_trivial_float", (k, args) => ({
      kind: "nodeid",
      nodeid: k.internTrivialFloat64(Number(argStr(args, 0)) || 0),
    }));
    this.registerNative("float_value", (k, args) => {
      const n = argNodeID(args, 0);
      if (n.type === Triv.FLOAT32) return { kind: "f64", float: k.decodeFloat32(n.inst) };
      if (n.type === Triv.FLOAT64) return { kind: "f64", float: k.decodeFloat64(n.inst) };
      throw new Error("float_value expects a float NodeID");
    });
    this.registerNative("intern_node", (k, args) => {
      const cat = argNodeID(args, 0);
      const kids = argList(args, 1).map((v) => {
        if (v.kind !== "nodeid")
          throw new Error("intern_node: children must be nodeids");
        return v.nodeid;
      });
      return { kind: "nodeid", nodeid: k.intern(cat, kids) };
    });
    // fb_record — native provenance primitive (tag 128). Records source
    // attribution for an already-interned node, retains it as a framebuffer
    // root, and returns the same node. The packed coordinate is
    // line<<16|col, matching Go, Rust, and the fkwu fourth arm.
    this.registerNative("fb_record", (k, args) => {
      const nid = argNodeID(args, 0);
      const file = k.internName(argStr(args, 1));
      const packed = argInt(args, 2);
      k.sourceAttr.set(nodeKey(nid), {
        file,
        line: packed >>> 16,
        col: packed & 0xffff,
      });
      k.framebufferRoots.push(nid);
      return { kind: "nodeid", nodeid: nid };
    });
    this.registerNative("node_category", (k, args) => ({
      kind: "nodeid",
      nodeid: k.category(argNodeID(args, 0)),
    }));
    this.registerNative("node_children", (k, args) => {
      const kids = k.children(argNodeID(args, 0));
      return {
        kind: "list",
        list: kids.map((c) => ({ kind: "nodeid", nodeid: c } as Value)),
      };
    });
    this.registerNative("node_value", (k, args) =>
      k.trivialValue(argNodeID(args, 0)),
    );
    this.registerNative("node_pkg", (_k, args) => ({
      kind: "int",
      int: argNodeID(args, 0).pkg,
    }));
    this.registerNative("node_level", (_k, args) => ({
      kind: "int",
      int: argNodeID(args, 0).level,
    }));
    this.registerNative("node_type", (_k, args) => ({
      kind: "int",
      int: argNodeID(args, 0).type,
    }));
    this.registerNative("node_inst", (_k, args) => ({
      kind: "int",
      int: argNodeID(args, 0).inst,
    }));
    this.registerNative("node_source", (k, args) => {
      const loc = k.sourceAttr.get(nodeKey(argNodeID(args, 0)));
      if (!loc) return { kind: "list", list: [] };
      return {
        kind: "list",
        list: [
          { kind: "str", str: k.strs[loc.file] ?? "" },
          { kind: "int", int: loc.line },
          { kind: "int", int: loc.col },
        ],
      };
    });
    this.registerNative("framebuffer-events", (k, _args) => ({
      kind: "list",
      list: k.framebufferRoots
        .filter((nid) => k.sourceAttr.has(nodeKey(nid)))
        .map((nid) => ({ kind: "nodeid", nodeid: nid }) as Value),
    }));
    this.registerNative("framebuffer-event-rows", (k, _args) => {
      const rows = k.framebufferRoots
        .filter((nid) => k.sourceAttr.has(nodeKey(nid)))
        .map((nid) => {
          const loc = k.sourceAttr.get(nodeKey(nid))!;
          const children = k.children(nid);
          const seqNode = children[0];
          const seq =
            seqNode !== undefined &&
            seqNode.level === Level.TRIVIAL &&
            seqNode.type === Triv.INT
              ? k.trivialValue(seqNode)
              : { kind: "int", int: 0 } as Value;
          return {
            seq: seq.kind === "int" ? seq.int : 0,
            nid,
            loc,
            children,
          };
        })
        .sort((a, b) => a.seq - b.seq || nodeKey(a.nid).localeCompare(nodeKey(b.nid)));
      return {
        kind: "list",
        list: rows.map((row) => ({
          kind: "list",
          list: [
            { kind: "int", int: row.seq },
            { kind: "str", str: k.strs[row.loc.file] ?? "" },
            { kind: "int", int: row.loc.line },
            { kind: "int", int: row.loc.col },
            { kind: "str", str: nodeKey(row.nid) },
            {
              kind: "list",
              list: row.children.map((child) => ({ kind: "str", str: nodeKey(child) }) as Value),
            },
            {
              kind: "list",
              list: row.children.map((child) => ({
                kind: "str",
                str: child.level === Level.TRIVIAL ? k.render(k.trivialValue(child)) : nodeKey(child),
              }) as Value),
            },
          ],
        }) as Value),
      };
    });
    this.registerNative("framebuffer-counts", (k, _args) => {
      const counts = new Map<string, { file: string; line: number; col: number; count: number }>();
      for (const nid of k.framebufferRoots) {
        const loc = k.sourceAttr.get(nodeKey(nid));
        if (!loc) continue;
        const file = k.strs[loc.file] ?? "";
        const key = `${file}\0${loc.line}\0${loc.col}`;
        const row = counts.get(key) ?? { file, line: loc.line, col: loc.col, count: 0 };
        row.count += 1;
        counts.set(key, row);
      }
      const rows = Array.from(counts.values()).sort((a, b) => {
        if (a.count !== b.count) return b.count - a.count;
        const fileOrder = a.file.localeCompare(b.file);
        if (fileOrder !== 0) return fileOrder;
        if (a.line !== b.line) return a.line - b.line;
        return a.col - b.col;
      });
      return {
        kind: "list",
        list: rows.map((row) => ({
          kind: "list",
          list: [
            { kind: "str", str: row.file },
            { kind: "int", int: row.line },
            { kind: "int", int: row.col },
            { kind: "int", int: row.count },
          ],
        }) as Value),
      };
    });
    this.registerNative("framebuffer-clear", (k, _args) => {
      k.sourceAttr.clear();
      k.framebufferRoots = [];
      return { kind: "null" };
    });
    // value_eq — content identity within a kind, cross-kind 0 (valueEqual); node_eq is the
    // same door on fkwu (tag 80), so it answers the same on any two values.
    const valueEqNative = (_k: Kernel, args: Value[]): Value => boolInt(valueEqual(args[0]!, args[1]!));
    this.registerNative("value_eq", valueEqNative);
    this.registerNative("node_eq", valueEqNative);
    this.registerNative("make_float32", (k, args) => ({
      kind: "nodeid",
      nodeid: k.internTrivialFloat32(argFloat(args, 0)),
    }));
    this.registerNative("make_float64", (k, args) => ({
      kind: "nodeid",
      nodeid: k.internTrivialFloat64(argFloat(args, 0)),
    }));

    // `now_unix_ms` — current wall-clock as a millisecond unix timestamp.
    // External effect (reads the host clock) so it's catCall. Sibling
    // parity holds on shape, NOT on value: every kernel returns an int,
    // every kernel's int is > a recent past epoch — but the exact
    // milliseconds diverge between invocations. Bands check shape only.
    this.registerNative("now_unix_ms", (_k, _args) => ({
      kind: "int",
      int: Date.now(),
    }));

    // `temp_dir` — the host's scratch directory: TMPDIR when the carrier
    // names one, /tmp otherwise (no trailing slash). External read (host
    // env) so it's catCall. The door that lets a band's scratch files land
    // in per-leg space: validate.sh points each sibling kernel at its own
    // TMPDIR, so concurrent legs never share a scratch path. Sibling
    // parity holds on shape, NOT on value — each leg's dir differs by
    // design; bands fold the path into effects, never into the verdict.
    const tempDirNative = (_k: Kernel, _args: Value[]): Value => ({
      kind: "str",
      str: (this.host.tempDirectory?.() ?? "/tmp").replace(/\/+$/, "") || "/tmp",
    });
    this.registerNative("host_temp_dir", tempDirNative);
    this.registerNative("temp_dir", tempDirNative);
    // host_pid, host_monotonic_ms, host_cwd — this process's id, a monotonic millisecond clock and
    // its working directory: the doors fkwu carries as tags 160, 182 and 29. Parity holds on shape,
    // not value: each leg is its own process, and only differences between two clock readings mean
    // anything (fkwu counts from boot, the siblings from their own start).
    this.registerNative("host_pid", (_k, _args) => {
      const pid = this.host.processId?.();
      return pid === undefined ? { kind: "null" } : { kind: "int", int: pid };
    });
    this.registerNative("host_monotonic_ms", (_k, _args) => {
      const ms = this.host.monotonicMs?.();
      return ms === undefined ? { kind: "null" } : { kind: "int", int: Math.floor(ms) };
    });
    this.registerNative("host_cwd", (_k, _args) => {
      const dir = this.host.workingDirectory?.();
      return dir === undefined ? { kind: "null" } : { kind: "str", str: dir };
    });
    // kernel_stat, kernel_live, print_str — the doors fkwu carries as tags 127, 163 and 115.
    //
    // kernel_stat key reads fkwu's self-measurement key space. Key 164 is fkwu's arm counter for
    // tag 64 (record_new): the record-construction clock, every record this process has made,
    // which compaction never lowers. This kernel counts the same event where it happens. A key
    // this kernel does not measure answers nothing, never a zero it did not read.
    this.registerNative("kernel_stat", (_k, args) => {
      const key = args[0];
      return key?.kind === "int" && key.int === 164
        ? { kind: "int", int: recordConstructions }
        : { kind: "null" };
    });
    // kernel_live pid answers that kernel's live page in fkwu's word order: 0 the page magic,
    // 1 the pid, 2 the start-ms. This kernel holds those three words of its own page; fkwu's
    // further words count fkwu's own tissue and are not claimed here. Any other pid, or a host
    // with no process id, answers the empty list, fkwu's answer where no page can be read.
    this.registerNative("kernel_live", (_k, args) => {
      const pid = this.host.processId?.();
      const asked = args[0];
      if (pid === undefined || asked?.kind !== "int" || asked.int !== pid) {
        return { kind: "list", list: [] };
      }
      return {
        kind: "list",
        list: [
          { kind: "int", int: KERNEL_LIVE_MAGIC },
          { kind: "int", int: pid },
          { kind: "int", int: HOST_BIRTH_UNIX_MS },
        ],
      };
    });
    // print_str s writes the string's bytes and one newline to stdout and answers 0, as fkwu
    // does. A value that is not a string writes the newline alone.
    this.registerNative("print_str", (_k, args) => {
      const s = args[0];
      this.host.writeStdout?.((s?.kind === "str" ? s.str : "") + "\n");
      return { kind: "int", int: 0 };
    });

    // `unix_ms_to_iso_utc` — render a millisecond instant as the
    // second-resolution ISO UTC string the Go carrier emits.
    this.registerNative("unix_ms_to_iso_utc", (_k, args) => ({
      kind: "str",
      str: `${new Date(argInt(args, 0)).toISOString().slice(0, 19)}Z`,
    }));

    // ── volatile cells — the RAM organ as a kernel resource ──
    // In-process volatile KV with update timestamps, mirroring the Go
    // carrier (server.go registerHostIONatives): put returns 1, get
    // returns the stored value or null, scan_since returns (key value
    // updated_ms) triples for one namespace, and prune_before returns the
    // count of cells released.
    this.registerNative("volatile_cell_put", (_k, args) => {
      volatileCells.set(volatileCoord(argStr(args, 0), argStr(args, 1)), {
        updatedMs: Date.now(),
        value: args[2] ?? { kind: "null" },
      });
      return { kind: "int", int: 1 };
    });
    this.registerNative("volatile_cell_get", (_k, args) => {
      const cell = volatileCells.get(
        volatileCoord(argStr(args, 0), argStr(args, 1)),
      );
      return cell ? cell.value : { kind: "null" };
    });
    this.registerNative("volatile_cell_scan_since", (_k, args) => {
      const prefix = `${argStr(args, 0)}\x00`;
      const since = argInt(args, 1);
      const out: Value[] = [];
      for (const [coord, cell] of volatileCells) {
        if (!coord.startsWith(prefix) || cell.updatedMs < since) continue;
        out.push({
          kind: "list",
          list: [
            { kind: "str", str: coord.slice(prefix.length) },
            cell.value,
            { kind: "int", int: cell.updatedMs },
          ],
        });
      }
      return { kind: "list", list: out };
    });
    this.registerNative("volatile_cell_prune_before", (_k, args) => {
      const prefix = `${argStr(args, 0)}\x00`;
      const before = argInt(args, 1);
      let pruned = 0;
      for (const [coord, cell] of volatileCells) {
        if (coord.startsWith(prefix) && cell.updatedMs < before) {
          volatileCells.delete(coord);
          pruned += 1;
        }
      }
      return { kind: "int", int: pruned };
    });
  }
}

// volatile cells — process-lifetime RAM organ shared by every kernel
// instance in this process, like Go's goVolatileCells global.
const volatileCells = new Map<string, { updatedMs: number; value: Value }>();
function volatileCoord(namespace: string, key: string): string {
  return `${namespace}\x00${key}`;
}

// argN helpers — typed extraction with friendly errors.
function argInt(args: Value[], i: number): number {
  const v = args[i];
  if (!v) throw new Error(`arg ${i}: missing`);
  if (v.kind === "int") return v.int;
  if (v.kind === "i64") return Number(v.bigint);
  throw new Error(`arg ${i}: expected int-like, got ${v.kind}`);
}
function argFloat(args: Value[], i: number): number {
  const v = args[i];
  if (!v) throw new Error(`arg ${i}: missing`);
  if (v.kind === "f64") return v.float;
  if (v.kind === "int") return v.int;
  if (v.kind === "i64") return Number(v.bigint);
  throw new Error(`arg ${i}: expected number, got ${v.kind}`);
}

// exactFixedDecimal — the EXACT decimal expansion of |x| as a fixed-point
// string "ipart.fpart", for any finite double. JS `toFixed` caps at 100
// fractional places, which is too few for the full expansion (a subnormal
// needs up to 1074), so we reconstruct it from the IEEE mantissa via BigInt:
// a finite double equals mantissa * 2^e2; for e2 < 0 that is
// mantissa * 5^(-e2) / 10^(-e2), an exact terminating decimal. This matches,
// digit-for-digit, the Rust kernel's format!("{:.1074}") and the Go kernel's
// strconv.FormatFloat('f', 1074) — the three exact expansions are identical.
function exactFixedDecimal(ax: number): { ipart: string; fpart: string } {
  // ax is non-negative and finite.
  const buf = new ArrayBuffer(8);
  const dv = new DataView(buf);
  dv.setFloat64(0, ax);
  const hi = dv.getUint32(0);
  const lo = dv.getUint32(4);
  const expBits = (hi >>> 20) & 0x7ff;
  const mantHi = hi & 0xfffff;
  let mant = (BigInt(mantHi) << 32n) | BigInt(lo >>> 0);
  let e2: number;
  if (expBits === 0) {
    // subnormal (or zero)
    e2 = -1074;
  } else {
    mant |= 1n << 52n;
    e2 = expBits - 1075;
  }
  if (mant === 0n) {
    return { ipart: "0", fpart: "" };
  }
  if (e2 >= 0) {
    const intVal = mant << BigInt(e2);
    return { ipart: intVal.toString(), fpart: "" };
  }
  const k = -e2;
  const scaled = mant * 5n ** BigInt(k); // value * 10^k
  let s = scaled.toString();
  if (s.length <= k) {
    s = "0".repeat(k - s.length + 1) + s;
  }
  const split = s.length - k;
  return { ipart: s.slice(0, split), fpart: s.slice(split) };
}

// roundNdigitsDecimal — CPython `round(x, n)` for a finite double, n >= 0.
// Rounds the EXACT decimal value of the double half-to-even at n fractional
// places, then parses back to the nearest double. The naive f64 paths
// (floor(x*10^n+0.5)/10^n; banker's on the scaled f64) diverge because the
// *10^n reintroduces representation error; rounding on the exact decimal
// avoids it. Verified bit-for-bit against CPython on 6.6M cases with ZERO
// divergences. Sibling-parity with the Rust + Go kernels.
function roundNdigitsDecimal(x: number, n: number): number {
  if (Number.isNaN(x) || !Number.isFinite(x)) return x;
  const neg = x < 0 || Object.is(x, -0);
  const ax = Math.abs(x);
  const { ipart, fpart } = exactFixedDecimal(ax);
  const digits = (ipart + fpart).split("");
  const point = ipart.length;
  const keep = point + n;
  if (keep < 0) {
    return neg ? -0 : 0;
  }
  while (digits.length < keep) digits.push("0");
  const keptSlice = digits.slice(0, keep);
  const rest = digits.slice(keep);
  let kept = keptSlice.length === 0 ? "0" : keptSlice.join("");
  let roundUp = false;
  if (rest.length > 0) {
    const first = rest[0]!;
    if (first > "5") roundUp = true;
    else if (first < "5") roundUp = false;
    else {
      const tailNonzero = rest.slice(1).some((d) => d !== "0");
      if (tailNonzero) roundUp = true;
      else roundUp = (kept.charCodeAt(kept.length - 1) - 48) % 2 === 1;
    }
  }
  if (roundUp) kept = addOneDecimal(kept);
  const dec = composeScaledDecimal(kept, n, neg);
  const out = Number(dec);
  if (out === 0 && neg) return -0;
  return out;
}

// addOneDecimal — increment a non-negative decimal digit string by 1,
// propagating carry (may grow by one leading digit).
function addOneDecimal(s: string): string {
  const b = s.split("");
  let i = b.length;
  for (;;) {
    if (i === 0) {
      b.unshift("1");
      break;
    }
    i--;
    if (b[i] === "9") b[i] = "0";
    else {
      b[i] = String.fromCharCode(b[i]!.charCodeAt(0) + 1);
      break;
    }
  }
  return b.join("");
}

// composeScaledDecimal — render integer string `kept` scaled by 10^-n as a
// decimal literal with the given sign. n >= 0.
function composeScaledDecimal(kept: string, n: number, neg: boolean): string {
  let body: string;
  if (n === 0) {
    body = kept;
  } else {
    let si = kept;
    if (si.length <= n) si = "0".repeat(n - si.length + 1) + si;
    const split = si.length - n;
    body = si.slice(0, split) + "." + si.slice(split);
  }
  return neg ? "-" + body : body;
}

function argStr(args: Value[], i: number): string {
  const v = args[i];
  if (v?.kind !== "str") throw new Error(`arg ${i}: expected str, got ${v?.kind ?? "absent"}`);
  return v.str;
}
function argList(args: Value[], i: number): Value[] {
  const v = args[i];
  if (v?.kind !== "list") throw new Error(`arg ${i}: expected list, got ${v?.kind ?? "absent"}`);
  return v.list;
}
function argNodeID(args: Value[], i: number): NodeID {
  const v = args[i];
  if (v?.kind !== "nodeid") throw new Error(`arg ${i}: expected nodeid, got ${v?.kind ?? "absent"}`);
  return v.nodeid;
}

// The integer range (law 1): 63-bit two's complement, [-2^62, 2^62), on every kernel.
const INT_BITS = 63;
const INT_LIMIT = 2 ** 62;

// str_to_float's grammar (law 7): C-locale leading whitespace, then the longest decimal prefix.
const FLOAT_PREFIX = /^[ \t\n\v\f\r]*([+-]?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?)/;

// intOrWide — an integer as the plain int kind when a double holds it exactly, the i64 kind
// (a bigint) past 2^53.
function intOrWide(total: bigint): Value {
  const n = Number(total);
  return Number.isSafeInteger(n)
    ? { kind: "int", int: n }
    : { kind: "i64", bigint: total };
}

// bitwiseInt — band/bor/bxor over the integer word. A safe integer splits into a signed
// high half and an unsigned low 32-bit half, each inside JS's 32-bit operators; an operand
// past 2^53 takes BigInt.
function bitwiseInt(
  args: Value[],
  word: (a: number, b: number) => number,
  wide: (a: bigint, b: bigint) => bigint,
): Value {
  const x = args[0];
  const y = args[1];
  if (x?.kind !== "i64" && y?.kind !== "i64") {
    const a = argInt(args, 0);
    const b = argInt(args, 1);
    if (Number.isSafeInteger(a) && Number.isSafeInteger(b)) {
      const aHi = Math.floor(a / 0x100000000);
      const bHi = Math.floor(b / 0x100000000);
      const lo = word(a - aHi * 0x100000000, b - bHi * 0x100000000) >>> 0;
      return { kind: "int", int: word(aHi, bHi) * 0x100000000 + lo };
    }
  }
  return intOrWide(BigInt.asIntN(INT_BITS, wide(expectBigInt(x!, "bitwise"), expectBigInt(y!, "bitwise"))));
}

// pgDsn — postgres://user[:password]@host[:port][/database][?options], read as the pg floor reads it:
// the last "@" ends the user part, %XX escapes decode in user, password and database, the host is
// localhost and the port 5432 when absent, and the database is the user's name when none is given.
function pgDsn(url: string): { host: string; port: number; user: string; password: string; database: string } {
  const rest = url.slice(url.startsWith("postgresql://") ? 13 : 11);
  const q = rest.indexOf("?");
  const main = q < 0 ? rest : rest.slice(0, q);
  const at = main.lastIndexOf("@");
  const userinfo = at < 0 ? "" : main.slice(0, at);
  const place = main.slice(at + 1);
  const slash = place.indexOf("/");
  const hostport = slash < 0 ? place : place.slice(0, slash);
  const database = slash < 0 ? "" : pgUnescape(place.slice(slash + 1));
  const colon = userinfo.indexOf(":");
  const user = pgUnescape(colon < 0 ? userinfo : userinfo.slice(0, colon));
  const password = colon < 0 ? "" : pgUnescape(userinfo.slice(colon + 1));
  const bracket = hostport.startsWith("[") ? hostport.indexOf("]") : -1;
  const portColon = bracket >= 0 ? hostport.indexOf(":", bracket) : hostport.indexOf(":");
  const host = bracket >= 0 ? hostport.slice(1, bracket) : portColon < 0 ? hostport : hostport.slice(0, portColon);
  return {
    host: host.length === 0 ? "localhost" : host,
    port: portColon < 0 ? 5432 : Number.parseInt(hostport.slice(portColon + 1), 10) || 0,
    user,
    password,
    database: database.length === 0 ? user : database,
  };
}

function pgUnescape(s: string): string {
  try {
    return decodeURIComponent(s);
  } catch {
    return s;
  }
}

// pgCell — a cell as the Go native reads it (database/sql over pgx, dbCellToForm): NULL as null,
// bool and the integer types as ints, float4 widened from its single-precision value, float8 as a float,
// timestamps and dates as RFC3339 in UTC, and every other type as the server's text, bytea as its hex.
function pgCell(oid: number, text: string | null): Value {
  if (text === null) return { kind: "null" };
  switch (oid) {
    case 16:
      return boolInt(text === "t");
    case 20:
    case 21:
    case 23:
      return intOrWide(BigInt(text));
    case 700:
      return { kind: "f64", float: Math.fround(Number(text)) };
    case 701:
      return { kind: "f64", float: Number(text) };
    case 1082:
      return { kind: "str", str: `${text}T00:00:00Z` };
    case 1114:
      return { kind: "str", str: pgRfc3339(text) };
    case 1184:
      return { kind: "str", str: pgRfc3339(text.endsWith("+00") ? text.slice(0, -3) : text) };
    default:
      return { kind: "str", str: text };
  }
}

// the server's "2026-09-12 08:00:00.5" in UTC as RFC3339: "2026-09-12T08:00:00.5Z"
function pgRfc3339(text: string): string {
  const at = text.indexOf(" ");
  return `${at < 0 ? text : `${text.slice(0, at)}T${text.slice(at + 1)}`}Z`;
}

// the row count a command tag carries: its last word when that is a number, 0 otherwise
function pgTagCount(tag: string): bigint {
  const word = tag.slice(tag.lastIndexOf(" ") + 1);
  return /^[0-9]+$/.test(word) ? BigInt(word) : 0n;
}

// a JSON object read from a config layer
type ConfigObject = { [key: string]: unknown };

// kernelConfigLoad — the kernel config as the Go and Rust natives merge it: api/config/api.json in the
// nearest directory at or above the working one that holds it, then ~/.coherence-network/config.json
// laid over it by a deep merge, with ~/.coherence-network/keys.json kept under "keys". A missing layer
// is skipped; a present layer that does not read as a JSON object is an error.
function kernelConfigLoad(host: KernelHost): ConfigObject {
  const merged: ConfigObject = {};
  const root = kernelConfigRoot(host, host.workingDirectory?.() ?? "");
  if (root.length > 0) kernelConfigLayer(host, merged, `${root}/api/config/api.json`);
  const home = host.homeDirectory?.() ?? "";
  if (home.length > 0) {
    kernelConfigLayer(host, merged, `${home}/.coherence-network/config.json`);
    kernelConfigKeys(host, merged, `${home}/.coherence-network/keys.json`);
  }
  return merged;
}

function kernelConfigRoot(host: KernelHost, dir: string): string {
  for (let at = dir; at.length > 0; ) {
    if (host.pathExists?.(`${at}/api/config/api.json`) === true) return at;
    const cut = at.lastIndexOf("/");
    at = cut > 0 ? at.slice(0, cut) : at.length > 1 ? "/" : "";
  }
  return "";
}

function isConfigObject(v: unknown): v is ConfigObject {
  return typeof v === "object" && v !== null && !Array.isArray(v);
}

function kernelConfigLayer(host: KernelHost, merged: ConfigObject, path: string): void {
  if (host.pathExists?.(path) !== true || host.readTextFile === undefined) return;
  const layer: unknown = JSON.parse(host.readTextFile(path));
  if (!isConfigObject(layer)) throw new Error(`${path} must contain a JSON object`);
  kernelConfigMerge(merged, layer);
}

// an object laid over an object merges key by key; any other value replaces what stood
function kernelConfigMerge(dst: ConfigObject, src: ConfigObject): void {
  for (const [key, value] of Object.entries(src)) {
    const current = dst[key];
    if (isConfigObject(value) && isConfigObject(current)) kernelConfigMerge(current, value);
    else dst[key] = value;
  }
}

// keys.json stands under "keys", and its GitHub token fills github_token where none is configured
function kernelConfigKeys(host: KernelHost, merged: ConfigObject, path: string): void {
  let keys: unknown;
  try {
    keys = JSON.parse(host.readTextFile?.(path) ?? "");
  } catch {
    return;
  }
  if (!isConfigObject(keys)) return;
  merged.keys = keys;
  const github = keys.github;
  const candidates = [
    isConfigObject(github) ? github.token : undefined,
    isConfigObject(github) ? github.api_token : undefined,
    keys.github_token,
  ];
  const token = candidates.find((c): c is string => typeof c === "string" && c.trim().length > 0)?.trim();
  const current = merged.github_token;
  if (token !== undefined && (typeof current !== "string" || current.trim().length === 0)) merged.github_token = token;
}

function kernelConfigDatabaseUrl(config: ConfigObject): string {
  const database = config.database;
  const url = isConfigObject(database) && typeof database.url === "string" ? database.url.trim() : "";
  if (url.length > 0) return url;
  return typeof config.database_url === "string" ? config.database_url.trim() : "";
}

// a dotted path through the config's objects; undefined where a part is empty or missing
function kernelConfigLookup(config: ConfigObject, path: string): unknown {
  let current: unknown = config;
  for (const part of path.split(".")) {
    if (part.length === 0 || !isConfigObject(current) || !Object.prototype.hasOwnProperty.call(current, part)) {
      return undefined;
    }
    current = current[part];
  }
  return current;
}

// a config value as the Go native answers it — text, a bool or an integral number as an int and any other
// as a float — with a missing or null value answering the fallback, and an object or list its JSON text
// as the Rust native writes it
function kernelConfigValue(value: unknown, fallback: Value): Value {
  if (value === undefined || value === null) return fallback;
  if (typeof value === "string") return { kind: "str", str: value };
  if (typeof value === "boolean") return boolInt(value);
  if (typeof value === "number") {
    return Number.isSafeInteger(value) ? { kind: "int", int: value } : { kind: "f64", float: value };
  }
  return { kind: "str", str: JSON.stringify(value) };
}

function valueKindName(v: Value): string {
  switch (v.kind) {
    case "null":
      return "null";
    case "int":
    case "i64":
      return "int";
    case "f64":
      return "float";
    case "str":
      return "string";
    case "list":
      return "list";
    case "closure":
      return "closure";
    case "nodeid":
      return "node_id";
    case "record":
      return "record";
    case "ctor":
      return "constructor";
    default:
      return "unknown";
  }
}

// ---------------------------------------------------------------------------
// Values — runtime tagged values
// ---------------------------------------------------------------------------

// SharedList — a list whose cells stand reversed in buf, the head at buf[n-1]: tail is the
// same buffer one shorter, and cons onto the longest view of a buffer pushes in place, so
// cons, head, tail, nth and len take constant time. `list` reads it forward, once.
export class SharedList {
  readonly kind = "list" as const;
  private forward: Value[] | undefined;
  constructor(
    readonly buf: Value[],
    readonly n: number,
  ) {}
  get list(): Value[] {
    if (this.forward === undefined) {
      const out: Value[] = new Array(this.n);
      for (let i = 0; i < this.n; i++) out[i] = this.buf[this.n - 1 - i]!;
      this.forward = out;
    }
    return this.forward;
  }
}

// sharedList — a list value as a SharedList; a list built as an array is copied once.
function sharedList(k: Kernel, v: { kind: "list"; list: Value[] }): SharedList {
  if (v instanceof SharedList) return v;
  const xs = v.list;
  k.noteListCopy(xs.length);
  return new SharedList(xs.slice().reverse(), xs.length);
}

function listLength(v: { kind: "list"; list: Value[] }): number {
  return v instanceof SharedList ? v.n : v.list.length;
}

// listAt — cell i of a list, nothing outside it.
function listAt(v: { kind: "list"; list: Value[] }, i: number): Value {
  if (v instanceof SharedList) return i >= 0 && i < v.n ? v.buf[v.n - 1 - i]! : { kind: "null" };
  return v.list[i] ?? { kind: "null" };
}

// An integer is one kind (law 1) held two ways: a JS number while a double holds it
// exactly ("int"), a bigint past 2^53 ("i64"). A float is a double.
export type Value =
  | { kind: "null" }
  | { kind: "int"; int: number }
  | { kind: "i64"; bigint: bigint }
  | { kind: "f64"; float: number }
  | { kind: "str"; str: string }
  | { kind: "list"; list: Value[] }
  | { kind: "closure"; closure: Closure }
  | { kind: "nodeid"; nodeid: NodeID }
  | { kind: "record"; record: Record } // mutable struct/object (BML rung 2)
  | { // #21 — INDUCTIVE-typed value (constructor application result)
      kind: "ctor";
      inductive: NodeID;
      ctor_name: string;
      ctor_index: number;
      args: Value[];
    };

// Record — a mutable struct/object with identity (BML reference, rung 2).
// The first mutable Value the kernel carries; required for `self.x = v`. A
// JS object reference gives shared mutable identity — two bindings to the same
// record see each other's mutations (object semantics, not value-copy).
// blueprint tags the record's type (class / method-table NodeID); fields is
// an ordered name→value map.
export interface Record {
  // null for a record built as (record_new 0 ...): fields, but no type and no
  // method table; record_blueprint reads back 0 (fkwu keeps the operand verbatim).
  blueprint: NodeID | null;
  // a record given as the blueprint, kept verbatim as fkwu keeps the operand
  // (native-recipe-record.bml's owner); no method table.
  blueprintRecord?: Record;
  fields: { name: NameID; val: Value }[];
}

export function recordGet(r: Record, name: NameID): Value | undefined {
  for (let i = r.fields.length - 1; i >= 0; i--) {
    if (r.fields[i]!.name === name) return r.fields[i]!.val;
  }
  return undefined;
}

export function recordSet(r: Record, name: NameID, val: Value): void {
  for (const f of r.fields) {
    if (f.name === name) {
      f.val = val;
      return;
    }
  }
  r.fields.push({ name, val });
}

export interface Closure {
  readonly name: NameID;
  readonly params: readonly NameID[];
  readonly body: NodeID;
  readonly env: Frame;
}

// ---------------------------------------------------------------------------
// Frame — scope primitive
// ---------------------------------------------------------------------------

// LIST_COPY_BUDGET — the list elements cons may copy in one run before the
// kernel voices the cost (Kernel.noteListCopy).
const LIST_COPY_BUDGET = 2 ** 28;

// bindingAtView — what a rebound unit name held at a unit version: the latest
// binding made at or before it, else the first (a read that came before the
// name's first let reads that let, as fkwu's forward hold does).
function bindingAtView(h: { v: number; val: Value }[], view: number): Value {
  let out = h[0]!.val;
  for (const e of h) {
    if (e.v <= view) out = e.val;
  }
  return out;
}

export class Frame {
  readonly parent: Frame | null;
  private readonly keys: NameID[] = [];
  private readonly vals: Value[] = [];
  // view — the unit version a closure defined at the unit level was made
  // under; its own empty frame carries it, 0 on every other frame.
  view = 0;
  // history — unit bindings a later unit let rebound, each with the unit
  // version it took effect at (the first at 0).
  history: Map<NameID, { v: number; val: Value }[]> | undefined;

  // index — a frame past FRAME_SCAN bindings (the unit's globals) finds a name by map
  private index: Map<NameID, number> | undefined;

  constructor(parent: Frame | null = null) {
    this.parent = parent;
  }

  private find(name: NameID): number {
    return this.index === undefined ? this.keys.indexOf(name) : (this.index.get(name) ?? -1);
  }

  bind(name: NameID, value: Value): void {
    const idx = this.find(name);
    if (idx >= 0) {
      this.vals[idx] = value;
      return;
    }
    this.keys.push(name);
    this.vals.push(value);
    if (this.index !== undefined) {
      this.index.set(name, this.keys.length - 1);
    } else if (this.keys.length > FRAME_SCAN) {
      this.index = new Map(this.keys.map((key, i) => [key, i]));
    }
  }

  // lookup — the nearest binding. A rebound unit name answers what the unit
  // held at the view of the closure reading it (the nearest view on the way
  // up), and its latest binding where no closure view stands.
  lookup(name: NameID): Value | undefined {
    let frame: Frame | null = this;
    let view = 0;
    while (frame !== null) {
      if (view === 0) view = frame.view;
      const idx = frame.find(name);
      if (idx >= 0) {
        if (view !== 0 && frame.history !== undefined) {
          const h = frame.history.get(name);
          if (h !== undefined) return bindingAtView(h, view);
        }
        return frame.vals[idx];
      }
      frame = frame.parent;
    }
    return undefined;
  }

  hasOwn(name: NameID): boolean {
    return this.find(name) >= 0;
  }

  // rebind — a later unit let: the name takes the new value, and the history
  // keeps what each unit version held.
  rebind(name: NameID, value: Value, version: number): void {
    const idx = this.find(name);
    if (idx < 0) {
      this.bind(name, value);
      return;
    }
    if (this.history === undefined) this.history = new Map();
    let h = this.history.get(name);
    if (h === undefined) {
      h = [{ v: 0, val: this.vals[idx]! }];
      this.history.set(name, h);
    }
    h.push({ v: version, val: value });
    this.vals[idx] = value;
  }

  // hasLocal — a binding of name in any frame but the root, whose bindings are
  // the globals: a parameter or a let the call head would read first.
  hasLocal(name: NameID): boolean {
    let frame: Frame | null = this;
    while (frame !== null && frame.parent !== null) {
      if (frame.find(name) >= 0) return true;
      frame = frame.parent;
    }
    return false;
  }
}

const FRAME_SCAN = 16;

// ---------------------------------------------------------------------------
// Walker — recipe → value
// ---------------------------------------------------------------------------

// attempt — (attempt x) walks x with a recover point standing, as fkwu's fk_attempt does.
// A stop inside x (a throw from a Form-level refusal, a RangeError from deep recursion)
// unwinds here: the Form stack returns to its depth at entry, the stop goes out as one
// organ-health line (aspect stop, backtrack selected), and the attempt answers nothing —
// the value every choice reads as "this option did not land".
function attempt(k: Kernel, x: NodeID, frame: Frame): Value {
  const depth = k.formStack.length;
  try {
    return walk(k, x, frame);
  } catch (e) {
    k.formStack.length = depth;
    k.stopSeq++;
    const now = Date.now();
    const row = {
      schema: "organ-health-v1",
      id: `form-kernel-ts-${k.host.processId?.() ?? 0}:stop:${k.stopSeq}`,
      organ: "form-kernel-ts",
      flow: "walker",
      aspect: "stop",
      stage: "applied",
      expected: "value",
      observed: e instanceof Error ? e.message : String(e),
      health: null,
      surprise: 1,
      needs: [],
      offers: ["backtrack"],
      selected: "backtrack",
      result: { answer: "nothing" },
      observed_at_ms: now,
      at_ms: now,
    };
    k.host.writeStderr?.(`form-organ health ${JSON.stringify(row)}\n`);
    return { kind: "null" };
  }
}

// walk — a recipe to its value. A conditional's taken arm, a do's last form
// and a closure's body are tail positions: the loop takes them in place, so a
// tail-recursive Form loop runs in constant host stack and holds no caller's
// frame alive, as the Go and Rust walkers do. A closure entered in tail
// position replaces this walk's form-stack slot.
export function walk(k: Kernel, node: NodeID, frame: Frame): Value {
  let pushed = false;
  let result: Value;
  for (;;) {
    if (node.level === Level.TRIVIAL) {
      result = k.trivialValue(node);
      break;
    }
    const row = k.recipeAt(node);
    const cat = row === undefined ? node : row.category;
    const kids = row === undefined ? NO_CHILDREN : row.children;
    if (cat.type === RBasic.COND) {
      const arm = condArm(k, cat.inst, kids, frame);
      if (arm === null) {
        result = { kind: "null" };
        break;
      }
      node = arm;
      continue;
    }
    if (cat.type === RBasic.BLOCK) {
      if (cat.inst === RBlock.LET) {
        // A let binds the rest of its own do (blockScope binds a do's own
        // lets). Met anywhere else — an if arm, an argument, a do's last
        // form — nothing follows it: it answers its value, binding no name.
        result = letValue(k, kids, frame);
        break;
      }
      if (kids.length === 0) {
        result = { kind: "null" };
        break;
      }
      frame = blockScope(k, kids, frame);
      node = kids[kids.length - 1]!;
      continue;
    }
    if (cat.type === RBasic.FNCALL) {
      const step = resolveCall(k, kids, frame);
      if (step.closure === undefined) {
        result = step.value;
        break;
      }
      const closure = step.closure;
      frame = closureFrame(k, closure, kids, frame);
      if (pushed) {
        k.formStack[k.formStack.length - 1] = closure;
      } else {
        k.formStack.push(closure);
        pushed = true;
      }
      node = closure.body;
      continue;
    }
    result = walkNode(k, node, cat, kids, frame);
    break;
  }
  if (pushed) k.formStack.pop();
  return result;
}

function walkNode(
  k: Kernel,
  node: NodeID,
  cat: NodeID,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  switch (cat.type) {
    case RBasic.IDENT: {
      const id = k.identID(node);
      const v = frame.lookup(id);
      if (v !== undefined) return v;
      // Identifiers can also resolve to natives (callable values).
      const nat = k.natives.get(id);
      if (nat !== undefined) {
        return {
          kind: "closure",
          closure: { name: id, params: [], body: node, env: frame } as Closure,
        };
      }
      throw new Error(`unbound identifier: ${k.nameStr(id)}`);
    }
    case RBasic.MATH:
      return walkMath(k, cat.inst, kids, frame);
    case RBasic.COMPARE:
      return walkCompare(k, cat.inst, kids, frame);
    case RBasic.LOGIC:
      return walkLogic(k, cat.inst, kids, frame);
    case RBasic.MATCH:
      return cat.inst === RMatch.SWITCH
        ? walkMatchSwitch(k, node, kids, frame)
        : { kind: "nodeid", nodeid: node };
    case RBasic.FNDEF:
      return walkFnDef(k, kids, frame);
    case RBasic.LIST: {
      const items = kids.map((c) => walk(k, c, frame));
      return new SharedList(items.reverse(), items.length);
    }
    case RBasic.INDUCTIVE:
      // INDUCTIVE recipes are type definitions. Walking one yields the
      // NodeID of the type itself.
      return { kind: "nodeid", nodeid: node };
    case RBasic.CONSTRUCTOR:
      return walkConstructor(k, node, kids, frame);
    case RBasic.CHOICE_MATCH:
      return walkChoice(k, node, kids, frame);
    case RBasic.QUOTIENT:
      // QUOTIENT recipes — walking one yields its NodeID so structural
      // reasoning over equivalence-class types can address them.
      return { kind: "nodeid", nodeid: node };
    case RBasic.ALIAS:
    case RBasic.BLANKET:
    case RBasic.PROJECT:
    case RBasic.GENERATIVE:
    case RBasic.PROOF:
    case RBasic.INFERENCE:
    case RBasic.VECTOR:
    case RBasic.TILE:
    case RBasic.PARALLELIZE:
    case RBasic.VECTORIZE:
    case RBasic.TRANSMUTE:
      // Higher-architecture recipes — walking returns the NodeID itself,
      // letting downstream code reason structurally without crashing on
      // recipes whose semantics are interpreted by the Form cells that
      // build them (alias, blanket, project, generative, proof, vector,
      // parallel, transmute).
      return { kind: "nodeid", nodeid: node };
    default:
      throw new Error(`walk: unsupported RBasic type ${cat.type}`);
  }
}

// CONSTRUCTOR recipe shape:
//   children: [inductive-ref, ctor-name-trivial, ctor-index-trivial, args...]
function walkConstructor(
  k: Kernel,
  _node: NodeID,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  if (kids.length < 3) {
    throw new Error("constructor: need 3+ children (inductive, name, index)");
  }
  const inductive = kids[0]!;
  const nameNode = kids[1]!;
  const indexNode = kids[2]!;
  if (nameNode.level !== Level.TRIVIAL || nameNode.type !== Triv.STRING) {
    throw new Error("constructor: name must be a string trivial");
  }
  if (indexNode.level !== Level.TRIVIAL || indexNode.type !== Triv.INT32) {
    throw new Error("constructor: index must be an int trivial");
  }
  const args: Value[] = [];
  for (let i = 3; i < kids.length; i++) {
    args.push(walk(k, kids[i]!, frame));
  }
  const indexVal = k.trivialValue(indexNode);
  return {
    kind: "ctor",
    inductive,
    ctor_name: k.nameStr(nameNode.inst),
    ctor_index: indexVal.kind === "int" ? indexVal.int : 0,
    args,
  };
}

// CHOICE recipe shape:
//   children: [scrutinee, arm0-ctor-name, arm0-body, ...]
function walkChoice(
  k: Kernel,
  _node: NodeID,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  if (kids.length < 1) throw new Error("choice: need scrutinee");
  if ((kids.length - 1) % 2 !== 0) {
    throw new Error("choice: arms must be (name, body) pairs");
  }
  const scrutinee = walk(k, kids[0]!, frame);
  if (scrutinee.kind !== "ctor") {
    throw new Error(`choice: scrutinee must be ctor value (got ${scrutinee.kind})`);
  }
  const armNames: string[] = [];
  const armBodies: NodeID[] = [];
  for (let i = 1; i < kids.length; i += 2) {
    const nameNode = kids[i]!;
    if (nameNode.level !== Level.TRIVIAL || nameNode.type !== Triv.STRING) {
      throw new Error("choice: arm name must be string trivial");
    }
    armNames.push(k.nameStr(nameNode.inst));
    armBodies.push(kids[i + 1]!);
  }
  // Totality check — only when scrutinee carries an inductive ref
  const indRecipe = k.recipeAt(scrutinee.inductive);
  if (indRecipe !== undefined && indRecipe.category.type === RBasic.INDUCTIVE) {
    // Walk inductive's constructors to find missing arms
    const ctorChildren = indRecipe.children.slice(2); // skip name + params
    const ctorNames: string[] = [];
    for (const ctorNid of ctorChildren) {
      const ctorRecipe = k.recipeAt(ctorNid);
      if (ctorRecipe && ctorRecipe.category.type === RBasic.CONSTRUCTOR) {
        const cName = ctorRecipe.children[1];
        if (cName && cName.level === Level.TRIVIAL && cName.type === Triv.STRING) {
          ctorNames.push(k.nameStr(cName.inst));
        }
      }
    }
    const missing = ctorNames.filter((n) => !armNames.includes(n));
    if (missing.length > 0) {
      throw new Error(
        `choice: non-total — missing constructor${missing.length > 1 ? "s" : ""}: ${missing.join(", ")}`,
      );
    }
  }
  // Dispatch
  for (let i = 0; i < armNames.length; i++) {
    if (armNames[i] === scrutinee.ctor_name) {
      const body = armBodies[i]!;
      const bodyRecipe = k.recipeAt(body);
      if (bodyRecipe === undefined) {
        return walk(k, body, frame);
      }
      if (bodyRecipe.category.type === RBasic.FNDEF) {
        const params = k.children(bodyRecipe.children[1]!);
        const armFrame = new Frame(frame);
        for (let j = 0; j < params.length; j++) {
          const p = params[j]!;
          if (p.level !== Level.TRIVIAL || p.type !== Triv.STRING) {
            throw new Error("choice: arm params must be string trivials");
          }
          armFrame.bind(p.inst, scrutinee.args[j] ?? { kind: "null" });
        }
        return walk(k, bodyRecipe.children[2]!, armFrame);
      }
      return walk(k, body, frame);
    }
  }
  throw new Error(`choice: no arm matches constructor ${scrutinee.ctor_name}`);
}

function isSwitchDefaultPattern(k: Kernel, pattern: NodeID): boolean {
  if (pattern.level === Level.TRIVIAL) return false;
  const cat = k.category(pattern);
  return cat.type === RBasic.IDENT && k.nameStr(k.identID(pattern)) === "_";
}

function switchTableFor(k: Kernel, node: NodeID, kids: readonly NodeID[]): SwitchTable {
  const tableKey = nodeKey(node);
  const cached = k.switchTables.get(tableKey);
  if (cached !== undefined) return cached;
  const table: SwitchTable = {
    cases: new Map<string, NodeID>(),
    dynamicArms: [],
  };
  for (let i = 1; i < kids.length; i += 2) {
    const pattern = kids[i]!;
    const body = kids[i + 1]!;
    if (isSwitchDefaultPattern(k, pattern)) {
      table.defaultBody = body;
    } else if (pattern.level === Level.TRIVIAL) {
      // a truth leaf walks to its 0/1 int, so it keys as that int
      const key = pattern.type === Triv.BOOL ? k.internTrivialInt(pattern.inst) : pattern;
      table.cases.set(nodeKey(key), body);
    } else {
      table.dynamicArms.push({ pattern, body });
    }
  }
  k.switchTables.set(tableKey, table);
  return table;
}

function switchKeyFromValue(k: Kernel, value: Value): NodeID | undefined {
  switch (value.kind) {
    case "null":
      return k.internTrivialNull();
    case "int":
      return k.internTrivialInt(value.int);
    case "i64":
      return k.internTrivialInt64(value.bigint);
    case "f64":
      return k.internTrivialFloat64(value.float);
    case "str":
      return k.internString(value.str);
    case "nodeid":
      return value.nodeid;
    default:
      return undefined;
  }
}

function walkMatchSwitch(
  k: Kernel,
  node: NodeID,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  if (kids.length < 1 || (kids.length - 1) % 2 !== 0) {
    throw new Error("match: SWITCH expects scrutinee plus pattern/body pairs");
  }
  const scrutinee = walk(k, kids[0]!, frame);
  const table = switchTableFor(k, node, kids);
  const key = switchKeyFromValue(k, scrutinee);
  if (key !== undefined) {
    const body = table.cases.get(nodeKey(key));
    if (body !== undefined) return walk(k, body, frame);
  }
  for (const arm of table.dynamicArms) {
    if (valueEqual(walk(k, arm.pattern, frame), scrutinee)) return walk(k, arm.body, frame);
  }
  if (table.defaultBody !== undefined) return walk(k, table.defaultBody, frame);
  throw new Error(`match: exhausted without a matching arm for ${k.render(scrutinee)}`);
}

function expectInt(v: Value, op: string): number {
  if (v.kind === "int") return v.int;
  if (v.kind === "i64") return Number(v.bigint);
  throw new Error(`${op}: expected int-like, got ${v.kind}`);
}

function expectFloat(v: Value, op: string): number {
  if (v.kind === "f64") return v.float;
  if (v.kind === "int") return v.int;
  if (v.kind === "i64") return Number(v.bigint);
  throw new Error(`${op}: expected number-like, got ${v.kind}`);
}

function expectBigInt(v: Value, op: string): bigint {
  if (v.kind === "i64") return v.bigint;
  if (v.kind === "int") return BigInt(v.int);
  throw new Error(`${op}: expected integer-like, got ${v.kind}`);
}

// walkMath — add/sub/mul/div/mod over the operands' own kinds: any float operand makes the
// fold a float fold; integers fold exactly and wrap at 63 bits (law 1). Integer div/mod
// truncate toward zero and stop on a zero divisor (law 2); float mod truncates too (law 5).
function walkMath(
  k: Kernel,
  op: number,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  if (kids.length < 2) throw new Error("math: need at least 2 args");
  const vals = kids.map((kid) => walk(k, kid!, frame));
  if (vals.some((v) => v.kind === "f64")) {
    let facc = expectFloat(vals[0]!, "math.f64");
    for (let i = 1; i < vals.length; i++) {
      const x = expectFloat(vals[i]!, "math.f64");
      switch (op) {
        case RMath.PLUS:
          facc = facc + x;
          break;
        case RMath.MINUS:
          facc = facc - x;
          break;
        case RMath.MUL:
          facc = facc * x;
          break;
        case RMath.DIV:
          facc = facc / x;
          break;
        case RMath.MOD:
          facc = facc % x;
          break;
        default:
          throw new Error(`math.f64: unknown op ${op}`);
      }
    }
    return { kind: "f64", float: facc };
  }
  // Integers fold in JS numbers while every step stays within ±(2^53−1), where a double
  // is exact. An operand carried as a bigint, or a step that leaves that range, refolds
  // the whole expression in BigInt (foldWide).
  if (vals.some((v) => v.kind === "i64")) return foldWide(op, vals);
  let acc = expectInt(vals[0]!, "math.int");
  for (let i = 1; i < vals.length; i++) {
    const x = expectInt(vals[i]!, "math.int");
    switch (op) {
      case RMath.PLUS:
        acc = acc + x;
        break;
      case RMath.MINUS:
        acc = acc - x;
        break;
      case RMath.MUL:
        acc = acc * x;
        break;
      case RMath.DIV:
        // Truncate toward zero — matches Go/Rust integer `/` (and Python's
        // int() of the quotient), without int32 wrap. Math.trunc, not `| 0`.
        if (x === 0) throw new Error("division by zero");
        acc = Math.trunc(acc / x);
        break;
      case RMath.MOD:
        if (x === 0) throw new Error("modulo by zero");
        acc = acc - Math.trunc(acc / x) * x;
        break;
      default:
        throw new Error(`math.int: unknown op ${op}`);
    }
    if (!Number.isSafeInteger(acc)) return foldWide(op, vals);
  }
  return { kind: "int", int: acc };
}

// foldWide — the integer fold past 2^53: BigInt steps wrapped to the 63-bit word (law 1).
// BigInt `/` and `%` truncate toward zero. The answer is a plain int again when a number
// holds it exactly.
function foldWide(op: number, vals: readonly Value[]): Value {
  let acc = expectBigInt(vals[0]!, "math.int");
  for (let i = 1; i < vals.length; i++) {
    const x = expectBigInt(vals[i]!, "math.int");
    switch (op) {
      case RMath.PLUS:
        acc = acc + x;
        break;
      case RMath.MINUS:
        acc = acc - x;
        break;
      case RMath.MUL:
        acc = acc * x;
        break;
      case RMath.DIV:
        if (x === 0n) throw new Error("division by zero");
        acc = acc / x;
        break;
      case RMath.MOD:
        if (x === 0n) throw new Error("modulo by zero");
        acc = acc % x;
        break;
      default:
        throw new Error(`math.int: unknown op ${op}`);
    }
    acc = BigInt.asIntN(INT_BITS, acc);
  }
  return intOrWide(acc);
}

// boolInt — the truth family's acknowledgment shape: 0/1 integer states
// (axiom-1) so eq/lt/and/not/node_eq/… answers feed arithmetic on every kernel.
function boolInt(b: boolean): Value {
  return { kind: "int", int: b ? 1 : 0 };
}

function walkCompare(
  k: Kernel,
  op: number,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  if (kids.length !== 2) throw new Error("compare: need exactly 2 args");
  const av = walk(k, kids[0]!, frame);
  const bv = walk(k, kids[1]!, frame);

  // A comparison acknowledges with the 0/1 integer states (axiom-1,
  // core-axioms.form) so its answer flows directly into arithmetic —
  // the shape the compiled lane's JS coercion already implied. Operands
  // meet the same numeric coercion in every lane. Where a non-number takes
  // part, eq and ne answer content identity (valueEqual, axiom-3) and an
  // ordering has no answer to give: it refuses by name, as fkwu's tags 5
  // and 103 do. Sibling to the Go and Rust walkers; proven three-way by
  // tests/eq-shape-band.fk.
  //
  // Width-mixing: if either side is float, compare as float; if either
  // side is bigint, compare as bigint; else as int.
  if (!(cmpNumberKind(av) && cmpNumberKind(bv))) {
    if (op === RCmp.EQ || op === RCmp.NE) {
      const same = valueEqual(av, bv);
      return boolInt(op === RCmp.EQ ? same : !same);
    }
    throw new Error("order: only numbers have an order -- ask value_kind before lt/le/gt/ge");
  }
  let r: boolean;
  if (av.kind === "f64" || bv.kind === "f64") {
    const a = expectFloat(av, "compare");
    const b = expectFloat(bv, "compare");
    switch (op) {
      case RCmp.EQ: r = a === b; break;
      case RCmp.NE: r = a !== b; break;
      case RCmp.LT: r = a < b; break;
      case RCmp.LE: r = a <= b; break;
      case RCmp.GT: r = a > b; break;
      case RCmp.GE: r = a >= b; break;
      default: throw new Error(`compare: unknown op ${op}`);
    }
  } else if (av.kind === "i64" || bv.kind === "i64") {
    const a = expectBigInt(av, "compare");
    const b = expectBigInt(bv, "compare");
    switch (op) {
      case RCmp.EQ: r = a === b; break;
      case RCmp.NE: r = a !== b; break;
      case RCmp.LT: r = a < b; break;
      case RCmp.LE: r = a <= b; break;
      case RCmp.GT: r = a > b; break;
      case RCmp.GE: r = a >= b; break;
      default: throw new Error(`compare: unknown op ${op}`);
    }
  } else {
    const a = expectInt(av, "compare");
    const b = expectInt(bv, "compare");
    switch (op) {
      case RCmp.EQ: r = a === b; break;
      case RCmp.NE: r = a !== b; break;
      case RCmp.LT: r = a < b; break;
      case RCmp.LE: r = a <= b; break;
      case RCmp.GT: r = a > b; break;
      case RCmp.GE: r = a >= b; break;
      default: throw new Error(`compare: unknown op ${op}`);
    }
  }
  return boolInt(r);
}

// cmpNumberKind — the kinds the compare lane coerces: an integer or a float. eq/ne over
// anything else is valueEqual; an ordering over anything else has no answer.
function cmpNumberKind(v: Value): boolean {
  return v.kind === "int" || v.kind === "i64" || v.kind === "f64";
}

// valueEqual — content identity (axiom-3: same composition is the same cell).
// value_eq answers it, and eq/ne answer it wherever a non-number takes part.
// An integer never equals a float, however alike they read, and a NaN is the
// NaN it was built as. Strings meet by text, NodeIDs by coordinates, lists by
// their items, however the lists were built. Records and closures are places
// (record_set writes into one), so a place equals only itself. Sibling to
// fkwu's fk_veq.
function valueEqual(a: Value, b: Value): boolean {
  if ((a.kind === "int" || a.kind === "i64") && (b.kind === "int" || b.kind === "i64")) {
    if (a.kind === "int" && b.kind === "int") return a.int === b.int;
    return expectBigInt(a, "value_eq") === expectBigInt(b, "value_eq");
  }
  if (a.kind === "f64" && b.kind === "f64") {
    return a.float === b.float || (Number.isNaN(a.float) && Number.isNaN(b.float));
  }
  if (a.kind !== b.kind) return false;
  switch (a.kind) {
    case "null":
      return true;
    case "str":
      return a.str === (b as { str: string }).str;
    case "list": {
      // lengths first: (eq xs (empty)), nil?'s question, never walks the cells
      const bl = b as { kind: "list"; list: Value[] };
      const n = listLength(a);
      if (n !== listLength(bl)) return false;
      if (a instanceof SharedList && bl instanceof SharedList && a.buf === bl.buf) return true;
      for (let i = 0; i < n; i++) if (!valueEqual(listAt(a, i), listAt(bl, i))) return false;
      return true;
    }
    case "record":
      return a.record === (b as { record: Record }).record;
    case "closure":
      return a.closure === (b as { closure: Closure }).closure;
    case "nodeid": {
      const bn = (b as { nodeid: NodeID }).nodeid;
      return (
        a.nodeid.pkg === bn.pkg &&
        a.nodeid.level === bn.level &&
        a.nodeid.type === bn.type &&
        a.nodeid.inst === bn.inst
      );
    }
    default:
      return false;
  }
}

// truthy — a branch reads a state (axiom-1): 0 and a float zero are 0,
// nothing is neither 0 nor 1 and refuses, and every other value is 1.
function truthy(v: Value): boolean {
  switch (v.kind) {
    case "null":
      throw new Error("if: nothing is neither 0 nor 1 -- ask nothing? before branching");
    case "int":
      return v.int !== 0;
    case "i64":
      return v.bigint !== 0n;
    case "f64":
      return v.float !== 0;
    default:
      return true;
  }
}

function walkLogic(
  k: Kernel,
  op: number,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  // Logic answers join the comparison family's 0/1 integer states
  // (axiom-1) — truth has one value shape, so (mul (and ...) n) flows
  // exactly like (mul (eq ...) n).
  if (op === RLogic.NOT) {
    if (kids.length !== 1) throw new Error("not: need exactly 1 arg");
    const v = walk(k, kids[0]!, frame);
    return boolInt(!truthy(v));
  }
  if (kids.length < 2) throw new Error("and/or: need at least 2 args");
  for (let i = 0; i < kids.length; i++) {
    const v = walk(k, kids[i]!, frame);
    const b = truthy(v);
    if (op === RLogic.AND && !b) return boolInt(false);
    if (op === RLogic.OR && b) return boolInt(true);
    if (i === kids.length - 1) return boolInt(b);
  }
  return boolInt(op === RLogic.AND);
}

// condArm — the arm a conditional takes, or null when an if without an else
// declines; walk takes the arm in tail position.
function condArm(
  k: Kernel,
  op: number,
  kids: readonly NodeID[],
  frame: Frame,
): NodeID | null {
  if (op === RCond.IF_THEN) {
    if (kids.length !== 2) throw new Error("if: need 2 args");
    return truthy(walk(k, kids[0]!, frame)) ? kids[1]! : null;
  }
  if (kids.length !== 3) throw new Error("if/else: need 3 args");
  return truthy(walk(k, kids[0]!, frame)) ? kids[1]! : kids[2]!;
}

// blockScope — a do or sequence is a lexical scope: its lets and defns bind
// for the rest of this do only. It walks every form but the last and answers
// the frame the last form reads, which walk takes in tail position. The scope
// opens at the first binding form, so a do that binds nothing costs no frame;
// a let binds in a fresh frame when a closure was made since the scope last
// opened, so a closure keeps the bindings it was made under. A unit's
// top-level do reads flat (walkUnit).
function blockScope(k: Kernel, kids: readonly NodeID[], frame: Frame): Frame {
  const last = kids.length - 1;
  const kinds = doKinds(k, kids);
  let scope = frame;
  let mark = k.closuresCreated;
  for (let i = 0; i < last; i++) {
    const c = kids[i]!;
    const kind = kinds[i]!;
    if (kind === BLOCK_KIND_LET) {
      const letKids = k.children(c);
      const value = letValue(k, letKids, scope);
      if (scope === frame || k.closuresCreated !== mark) {
        scope = new Frame(scope);
        mark = k.closuresCreated;
      }
      scope.bind(letKids[0]!.inst, value);
      continue;
    }
    if (kind === BLOCK_KIND_DEFN && scope === frame) {
      scope = new Frame(frame);
      mark = k.closuresCreated;
    }
    walk(k, c, scope);
  }
  if (kinds[last] === BLOCK_KIND_DEFN && scope === frame) scope = new Frame(frame);
  return scope;
}

const BLOCK_KIND_LET = 1;
const BLOCK_KIND_DEFN = 2;
const BLOCK_KIND_DO = 3;
const UNIT_DO = 1;
const UNIT_WRAPPER = 2;

function blockKind(k: Kernel, n: NodeID): number {
  if (n.level === Level.TRIVIAL) return 0;
  const cat = k.category(n);
  if (cat.type === RBasic.BLOCK) {
    return cat.inst === RBlock.LET ? BLOCK_KIND_LET : BLOCK_KIND_DO;
  }
  return cat.type === RBasic.FNDEF ? BLOCK_KIND_DEFN : 0;
}

// doKinds — the binding kind of each form of a do, read once per do recipe:
// the children array a recipe row holds is the same array on every walk.
const DO_KINDS = new WeakMap<readonly NodeID[], Uint8Array>();

function doKinds(k: Kernel, kids: readonly NodeID[]): Uint8Array {
  let kinds = DO_KINDS.get(kids);
  if (kinds === undefined) {
    kinds = new Uint8Array(kids.length);
    for (let i = 0; i < kids.length; i++) kinds[i] = blockKind(k, kids[i]!);
    DO_KINDS.set(kids, kinds);
  }
  return kinds;
}

function letValue(k: Kernel, kids: readonly NodeID[], frame: Frame): Value {
  if (kids.length !== 2) throw new Error("let: need 2 args (name, value)");
  const name = kids[0]!;
  if (name.level !== Level.TRIVIAL || name.type !== Triv.STRING) {
    throw new Error("let: name must be a string trivial");
  }
  return walk(k, kids[1]!, frame);
}

// bindLet -- a unit's let binds the unit's frame: the value reads the frame
// before the name joins it. A later unit let that rebinds a name raises the
// unit version, so a closure defined before it keeps the binding it was made
// under, as fkwu's hold per reference keeps it.
function bindLet(k: Kernel, node: NodeID, frame: Frame): Value {
  const kids = k.children(node);
  const value = letValue(k, kids, frame);
  const name = kids[0]!.inst;
  if (frame.parent === null && frame.hasOwn(name)) {
    k.unitView++;
    frame.rebind(name, value, k.unitView);
  } else {
    frame.bind(name, value);
  }
  return value;
}

// walkUnit -- a unit root read in the unit's own frame. The unit's top-level
// do is its sequence, as fkwu's fk_parse_top reads it: a let there binds the
// unit's frame, where every defn of the unit and every unit loaded after it
// reads it; a defn binds there; a do before the first let or expression is a
// top-level do too; a later do is a lexical scope (walkBlock). The implicit do
// around several top-level forms holds each form at column 0, where every do
// is a top-level do. A root the reader did not name (a deserialized recipe, a
// combined program) keeps every do at its top flat.
export function walkUnit(k: Kernel, node: NodeID, frame: Frame): Value {
  const kind = blockKind(k, node);
  if (kind === BLOCK_KIND_LET) return bindLet(k, node, frame);
  if (kind !== BLOCK_KIND_DO) return walk(k, node, frame);
  const mark = k.unitRoots.get(nodeKey(node));
  if (mark === UNIT_DO) return walkUnitDo(k, node, frame);
  let result: Value = { kind: "null" };
  for (const c of k.children(node)) {
    result =
      mark === UNIT_WRAPPER &&
      blockKind(k, c) === BLOCK_KIND_DO &&
      k.unitRoots.get(nodeKey(c)) !== UNIT_WRAPPER
        ? walkUnitDo(k, c, frame)
        : walkUnit(k, c, frame);
  }
  return result;
}

function walkUnitDo(k: Kernel, node: NodeID, frame: Frame): Value {
  let leading = true;
  let result: Value = { kind: "null" };
  for (const c of k.children(node)) {
    const kind = blockKind(k, c);
    if (kind === BLOCK_KIND_DO) {
      result = leading ? walkUnitDo(k, c, frame) : walk(k, c, frame);
    } else if (kind === BLOCK_KIND_LET) {
      result = bindLet(k, c, frame);
      leading = false;
    } else {
      result = walk(k, c, frame);
      if (kind !== BLOCK_KIND_DEFN) leading = false;
    }
  }
  return result;
}

// FNDEF children:  [name-trivial, params-SEQUENCE-of-name-trivials, body]
// (matches Go kernel's defn shape)
function walkFnDef(
  k: Kernel,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  if (kids.length !== 3) {
    throw new Error("defn: need 3 children (name, params, body)");
  }
  const name = kids[0]!;
  const paramsBlock = kids[1]!;
  const body = kids[2]!;

  if (name.level !== Level.TRIVIAL || name.type !== Triv.STRING) {
    throw new Error("defn: name must be string trivial");
  }
  const nameID = name.inst;

  const paramKids = k.children(paramsBlock);
  const params: NameID[] = paramKids.map((p) => {
    if (p.level !== Level.TRIVIAL || p.type !== Triv.STRING) {
      throw new Error("defn: params must be string trivials");
    }
    return p.inst;
  });

  // A closure defined at the unit level reads the unit as it stands now: its
  // own empty frame carries the unit version (Frame.view).
  let env = frame;
  if (frame.parent === null) {
    env = new Frame(frame);
    env.view = k.unitView;
  }
  k.closuresCreated++;
  const closure: Closure = { name: nameID, params, body, env };
  const value: Value = { kind: "closure", closure };
  frame.bind(nameID, value);
  return value;
}

// FNCALL children: [callee, arg0, arg1, ...]
// Callee is either an IDENT recipe, a bare string trivial, or any expression
// that evaluates to a closure.
// CallStep — a call either answered (a native) or names the closure walk
// enters in tail position.
type CallStep = { value: Value; closure?: undefined } | { closure: Closure; value?: undefined };

function resolveCall(
  k: Kernel,
  kids: readonly NodeID[],
  frame: Frame,
): CallStep {
  if (kids.length < 1) throw new Error("call: need callee");
  const calleeNode = kids[0]!;

  // Fast path: callee is a bare name. Resolve directly through frame or
  // natives without going through walk → IDENT dispatch.
  let calleeName: NameID | null = null;
  if (
    calleeNode.level === Level.TRIVIAL &&
    calleeNode.type === Triv.STRING
  ) {
    calleeName = calleeNode.inst;
  } else if (
    calleeNode.level === Level.BASIC &&
    calleeNode.type === RBasic.IDENT
  ) {
    calleeName = k.identID(calleeNode);
  }

  if (calleeName !== null) {
    const rawName = calleeName;
    // (attempt x): x is walked under a recover point, never before — fkwu's mode 28.
    if (kids.length === 2 && rawName === k.attemptName) {
      return { value: attempt(k, kids[1]!, frame) };
    }
    // Native dispatch. A local binding of the name (a parameter, a let) is
    // nearer than the native unless fkwu reserves the head: the one
    // call-position reading every arm gives.
    const native = k.natives.get(rawName);
    if (native !== undefined && (k.reservedHeads.has(rawName) || !frame.hasLocal(rawName))) {
      const args: Value[] = [];
      for (let i = 1; i < kids.length; i++) {
        args.push(walk(k, kids[i]!, frame));
      }
      k.formStack.push(k.nameStr(rawName));
      const out = native(k, args);
      k.formStack.pop();
      return { value: out };
    }
    const v = frame.lookup(rawName);
    if (v === undefined) {
      throw new Error(`call: unbound ${k.nameStr(rawName)}`);
    }
    if (v.kind !== "closure") {
      throw new Error(
        `call: ${k.nameStr(rawName)} is not a closure (got ${v.kind})`,
      );
    }
    return { closure: v.closure };
  }

  // General path: callee is an expression
  const calleeVal = walk(k, calleeNode, frame);
  if (calleeVal.kind !== "closure") {
    throw new Error(`call: callee is not a closure (got ${calleeVal.kind})`);
  }
  return { closure: calleeVal.closure };
}

// closureFrame — the frame a closure's body reads: its parameters bound to the
// call's arguments, each argument read in the caller's frame.
function closureFrame(
  k: Kernel,
  closure: Closure,
  kids: readonly NodeID[],
  frame: Frame,
): Frame {
  if (kids.length - 1 !== closure.params.length) {
    throw new Error(
      `call: arity mismatch (expected ${closure.params.length}, got ${kids.length - 1})`,
    );
  }
  const callFrame = new Frame(closure.env);
  for (let i = 0; i < closure.params.length; i++) {
    const v = walk(k, kids[i + 1]!, frame);
    callFrame.bind(closure.params[i]!, v);
  }
  return callFrame;
}

const FORM_BINARY_MAGIC_V1 = asciiBytes("FORMBIN1");
const FORM_BINARY_MAGIC = asciiBytes("FORMBIN2");
const FORM_BINARY_LEAF = 0;
const FORM_BINARY_COMPOSITE = 1;
// FLOAT64 carries its VALUE, not its index. A float64 trivial NodeID's `inst`
// is a per-kernel f64s-table index — meaningless in another kernel. So a float
// node serializes as [FORM_BINARY_FLOAT64][8 bytes IEEE-754 little-endian] and
// each kernel re-interns the value on read (fresh local index). The trivial
// float type tag (FLOAT64 = 7 three-way across Rust/Go/TS) never rides the wire
// either: the value travels in bytes, not the index nor the local type-tag, so
// the .fkb stays portable regardless of how each kernel numbers its types.
const FORM_BINARY_FLOAT64 = 2;
// INT64 carries its VALUE, not its index — the same reasoning as FLOAT64. A
// TRIV_INT64 NodeID's `inst` is a per-kernel i64s-table index, so an int64 node
// serializes as [FORM_BINARY_INT64][8 bytes signed little-endian] and each
// kernel re-interns on read. Aligned three-way: tag = 3 across Rust/Go/TS.
const FORM_BINARY_INT64 = 3;
const FORM_BINARY_MAX_BYTES = 64 << 20;
const FORM_BINARY_MAX_STRINGS = 262_144;
const FORM_BINARY_MAX_STRING_BYTES = 32 << 20;
const FORM_BINARY_MAX_CHILDREN = 262_144;
const FORM_BINARY_MAX_NODES = 1_000_000;
const FORM_BINARY_MAX_DEPTH = 256;

interface FormBinaryDecodeBudget {
  nodes: number;
}

function enterFormBinaryNode(budget: FormBinaryDecodeBudget, depth: number): void {
  if (depth > FORM_BINARY_MAX_DEPTH) {
    throw new Error("form binary: maximum node depth exceeded");
  }
  budget.nodes += 1;
  if (budget.nodes > FORM_BINARY_MAX_NODES) {
    throw new Error("form binary: maximum node count exceeded");
  }
}

function pushU32(out: number[], v: number): void {
  const n = v >>> 0;
  out.push((n >>> 24) & 0xff, (n >>> 16) & 0xff, (n >>> 8) & 0xff, n & 0xff);
}

function readU32(bytes: Uint8Array, pos: number): [number, number] {
  if (pos + 4 > bytes.length) throw new Error("form binary: truncated u32");
  const v =
    ((bytes[pos]! << 24) >>> 0) |
    (bytes[pos + 1]! << 16) |
    (bytes[pos + 2]! << 8) |
    bytes[pos + 3]!;
  return [v >>> 0, pos + 4];
}

// pushF64LE / readF64LE — an IEEE-754 f64 as 8 little-endian bytes (the payload
// of a FORM_BINARY_FLOAT64 node). Sibling parity with Rust/Go little-endian.
function pushF64LE(out: number[], f: number): void {
  const view = new DataView(new ArrayBuffer(8));
  view.setFloat64(0, f, true);
  for (let i = 0; i < 8; i++) out.push(view.getUint8(i));
}

function readF64LE(bytes: Uint8Array, pos: number): [number, number] {
  if (pos + 8 > bytes.length) throw new Error("form binary: truncated float64");
  const view = new DataView(new ArrayBuffer(8));
  for (let i = 0; i < 8; i++) view.setUint8(i, bytes[pos + i]!);
  return [view.getFloat64(0, true), pos + 8];
}

// pushI64LE / readI64LE — a signed int64 as 8 little-endian bytes (the payload
// of a FORM_BINARY_INT64 node). Sibling parity with Rust/Go little-endian.
function pushI64LE(out: number[], n: bigint): void {
  const view = new DataView(new ArrayBuffer(8));
  view.setBigInt64(0, n, true);
  for (let i = 0; i < 8; i++) out.push(view.getUint8(i));
}

function readI64LE(bytes: Uint8Array, pos: number): [bigint, number] {
  if (pos + 8 > bytes.length) throw new Error("form binary: truncated int64");
  const view = new DataView(new ArrayBuffer(8));
  for (let i = 0; i < 8; i++) view.setUint8(i, bytes[pos + i]!);
  return [view.getBigInt64(0, true), pos + 8];
}

interface FormBinaryStringTable {
  strings: string[];
  indexes: Map<number, number>;
}

function collectArtifactStrings(k: Kernel, nid: NodeID, table: FormBinaryStringTable): void {
  const recipe = k.recipeAt(nid);
  if (recipe) {
    collectArtifactStrings(k, recipe.category, table);
    for (const child of recipe.children) collectArtifactStrings(k, child, table);
    return;
  }
  if (nid.level === Level.TRIVIAL && nid.type === Triv.STRING && !table.indexes.has(nid.inst)) {
    const value = k.strs[nid.inst];
    if (value === undefined) throw new Error(`form binary: bad string index ${nid.inst}`);
    table.indexes.set(nid.inst, table.strings.length);
    table.strings.push(value);
  }
}

function serializeNodeWithStrings(k: Kernel, nid: NodeID, out: number[], table: FormBinaryStringTable): void {
  const recipe = k.recipeAt(nid);
  if (recipe) {
    pushU32(out, FORM_BINARY_COMPOSITE);
    serializeNodeWithStrings(k, recipe.category, out, table);
    pushU32(out, recipe.children.length);
    for (const child of recipe.children) serializeNodeWithStrings(k, child, out, table);
    return;
  }
  if (nid.level === Level.TRIVIAL && nid.type === Triv.FLOAT64) {
    pushU32(out, FORM_BINARY_FLOAT64);
    pushF64LE(out, k.decodeFloat64(nid.inst));
    return;
  }
  if (nid.level === Level.TRIVIAL && nid.type === Triv.INT64) {
    pushU32(out, FORM_BINARY_INT64);
    pushI64LE(out, k.decodeInt64(nid.inst));
    return;
  }
  pushU32(out, FORM_BINARY_LEAF);
  pushU32(out, nid.pkg);
  pushU32(out, nid.level);
  pushU32(out, nid.type);
  if (nid.level === Level.TRIVIAL && nid.type === Triv.STRING) {
    const local = table.indexes.get(nid.inst);
    if (local === undefined) throw new Error(`form binary: missing local string index ${nid.inst}`);
    pushU32(out, local);
  } else {
    pushU32(out, nid.inst);
  }
}

function deserializeNode(
  k: Kernel,
  bytes: Uint8Array,
  strings: readonly string[],
  pos: number,
  scope: number,
  budget: FormBinaryDecodeBudget,
  depth: number,
): [NodeID, number] {
  enterFormBinaryNode(budget, depth);
  let tag: number;
  [tag, pos] = readU32(bytes, pos);
  if (tag === FORM_BINARY_FLOAT64) {
    let value: number;
    [value, pos] = readF64LE(bytes, pos);
    return [k.internTrivialFloat64(value), pos];
  }
  if (tag === FORM_BINARY_INT64) {
    let value: bigint;
    [value, pos] = readI64LE(bytes, pos);
    return [k.internTrivialInt64(value), pos];
  }
  if (tag === FORM_BINARY_LEAF) {
    let pkg: number;
    let level: number;
    let type: number;
    let inst: number;
    [pkg, pos] = readU32(bytes, pos);
    [level, pos] = readU32(bytes, pos);
    [type, pos] = readU32(bytes, pos);
    [inst, pos] = readU32(bytes, pos);
    if (level === Level.TRIVIAL && type === Triv.STRING) {
      const value = strings[inst];
      if (value === undefined) throw new Error(`form binary: bad string index ${inst}`);
      return [k.internString(value), pos];
    }
    return [k.remapImportedLeaf(scope, { pkg, level, type, inst }), pos];
  }
  if (tag !== FORM_BINARY_COMPOSITE) {
    throw new Error(`form binary: unknown node tag ${tag}`);
  }
  let category: NodeID;
  [category, pos] = deserializeNode(k, bytes, strings, pos, scope, budget, depth + 1);
  let count: number;
  [count, pos] = readU32(bytes, pos);
  if (count > FORM_BINARY_MAX_CHILDREN) {
    throw new Error("form binary: maximum child count exceeded");
  }
  const children: NodeID[] = [];
  for (let i = 0; i < count; i++) {
    let child: NodeID;
    [child, pos] = deserializeNode(k, bytes, strings, pos, scope, budget, depth + 1);
    children.push(child);
  }
  return [k.intern(category, children), pos];
}

function deserializeNodeV1(
  k: Kernel,
  bytes: Uint8Array,
  strings: readonly string[],
  pos: number,
  scope: number,
  budget: FormBinaryDecodeBudget,
  depth: number,
): [NodeID, number] {
  enterFormBinaryNode(budget, depth);
  let pkg: number;
  let level: number;
  let type: number;
  let inst: number;
  let count: number;
  [pkg, pos] = readU32(bytes, pos);
  [level, pos] = readU32(bytes, pos);
  [type, pos] = readU32(bytes, pos);
  [inst, pos] = readU32(bytes, pos);
  [count, pos] = readU32(bytes, pos);
  if (count > FORM_BINARY_MAX_CHILDREN) {
    throw new Error("form binary: maximum child count exceeded");
  }
  if (count === 0) {
    if (level === Level.TRIVIAL && type === Triv.STRING) {
      const value = strings[inst];
      if (value === undefined) throw new Error(`form binary: bad string index ${inst}`);
      return [k.internString(value), pos];
    }
    return [k.remapImportedLeaf(scope, { pkg, level, type, inst }), pos];
  }
  const category =
    level === Level.TRIVIAL && type === Triv.STRING
      ? k.internString(readBinaryString(strings, inst))
      : { pkg, level, type, inst };
  const children: NodeID[] = [];
  for (let i = 0; i < count; i++) {
    let child: NodeID;
    [child, pos] = deserializeNodeV1(k, bytes, strings, pos, scope, budget, depth + 1);
    children.push(child);
  }
  return [k.intern(category, children), pos];
}

function readBinaryString(strings: readonly string[], index: number): string {
  const value = strings[index];
  if (value === undefined) throw new Error(`form binary: bad string index ${index}`);
  return value;
}

export function serializeRecipeArtifact(k: Kernel, root: NodeID): Uint8Array {
  const table: FormBinaryStringTable = { strings: [], indexes: new Map() };
  collectArtifactStrings(k, root, table);
  const out: number[] = Array.from(FORM_BINARY_MAGIC);
  pushU32(out, table.strings.length);
  for (const s of table.strings) {
    const encoded = isWide(s) ? utf8Encode(s) : bstrToBytes(s);
    pushU32(out, encoded.length);
    for (const byte of encoded) out.push(byte);
  }
  serializeNodeWithStrings(k, root, out, table);
  return Uint8Array.from(out);
}

export function deserializeRecipeArtifact(k: Kernel, bytes: Uint8Array): NodeID {
  if (bytes.length > FORM_BINARY_MAX_BYTES) {
    throw new Error("form binary: maximum artifact size exceeded");
  }
  const isV1 = hasMagic(bytes, FORM_BINARY_MAGIC_V1);
  const isV2 = hasMagic(bytes, FORM_BINARY_MAGIC);
  if (!isV1 && !isV2) throw new Error("form binary: bad magic");
  let pos = isV1 ? FORM_BINARY_MAGIC_V1.length : FORM_BINARY_MAGIC.length;
  let stringCount: number;
  [stringCount, pos] = readU32(bytes, pos);
  if (stringCount > FORM_BINARY_MAX_STRINGS) {
    throw new Error("form binary: maximum string count exceeded");
  }
  const strings: string[] = [];
  let totalStringBytes = 0;
  for (let i = 0; i < stringCount; i++) {
    let len: number;
    [len, pos] = readU32(bytes, pos);
    totalStringBytes += len;
    if (totalStringBytes > FORM_BINARY_MAX_STRING_BYTES) {
      throw new Error("form binary: maximum string bytes exceeded");
    }
    if (len > bytes.length - pos) throw new Error("form binary: truncated string");
    // FORMBIN2 strings are UTF-8 text by contract; validated, they are held as their bytes
    const raw = bytes.subarray(pos, pos + len);
    try {
      UTF8_STRICT.decode(raw);
    } catch {
      throw new Error("form binary: invalid utf8");
    }
    strings.push(bytesToBstr(raw));
    pos += len;
  }
  const scope = k.nextImportScope();
  const budget: FormBinaryDecodeBudget = { nodes: 0 };
  const [root, end] = isV1
    ? deserializeNodeV1(k, bytes, strings, pos, scope, budget, 0)
    : deserializeNode(k, bytes, strings, pos, scope, budget, 0);
  if (end !== bytes.length) throw new Error("form binary: trailing bytes");
  return root;
}

function hasMagic(bytes: Uint8Array, magic: Uint8Array): boolean {
  if (bytes.length < magic.length) return false;
  for (let i = 0; i < magic.length; i++) {
    if (bytes[i] !== magic[i]) return false;
  }
  return true;
}
