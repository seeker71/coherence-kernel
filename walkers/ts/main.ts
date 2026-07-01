// Minimal TypeScript proof-walker — independent parse + pure-compute eval.
//
// One of four sibling walkers (Go, Rust, TypeScript, fkwu). Its keep is ONE
// thing: an INDEPENDENT lexer + evaluator that witnesses the same recipe
// computing the same value four ways — and so catches a shared parse/semantic
// bug fkwu's own paths would miss (the scientific-notation `1e-05` case lives
// in tokenize() below). The walkers never carry a feature; fkwu owns the
// native path. This file holds the smallest core that preserves four-way
// agreement over the pure-op surface and nothing more.
//
// Pure-op surface (faithfully copied from form-kernel-ts/src/{reader,kernel}.ts;
// NOT rewritten from memory — exact intern shapes, exact walk dispatch):
//   • int + string literals (incl. scientific-notation floats)
//   • add sub mul div (+ mod, typed-width variants the reader emits)
//   • eq ne lt le gt ge   • if let do   • defn + user calls (closures)
//   • head tail cons nil(=null/empty) not and or   • str_concat str_eq
//   • the BMF s-expression parse and the RBasic op dispatch
//   • plus list/nth/len and node_eq/value_eq/bp so a real four-way manifest
//     band (eq-shape-band.fk → 524287) can verify, not just an ad-hoc band.
//
// Run:  node --import tsx walkers/ts/main.ts core.fk band.fk
//   or: tsx walkers/ts/main.ts file.fk ...
// Files are concatenated (preludes first); the joined source evaluates to one
// value, rendered bare (no quotes) for byte-comparison across kernels.
//
// Dropped vs the full kernel: JIT/wasm/asm, server, host-io/file/socket/metal,
// GGUF/model, formats, generated tables, the higher-architecture recipe
// modules (blanket/project/generative/proof/vector/parallel/…), and all tests.

import { readFileSync, existsSync } from "node:fs";

// ===========================================================================
// Substrate — NodeID + Recipe + intern table   (from kernel.ts)
// ===========================================================================

export interface NodeID {
  readonly pkg: number;
  readonly level: number;
  readonly type: number;
  readonly inst: number;
}

export const Level = {
  TRIVIAL: 1,
  BASIC: 2,
} as const;

// RBasic — aligned with api/app/services/substrate/category.py and the Go/Rust
// kernels. Only the slots the pure-op surface dispatches on are kept; the
// higher-architecture slots passed through as nodeid in the full kernel are
// not reachable here.
export const RBasic = {
  UNDEFINED: 0,
  WITNESS: 6,
  BLOCK: 9,
  CALL: 10,
  COND: 11,
  MATH: 12,
  COMPARE: 13,
  LOGIC: 14,
  ACCESS: 15,
  MATCH: 19,
  METHOD: 27,
  FNDEF: 31,
  FNCALL: 32,
  IDENT: 33,
  LIST: 34,
  ALIAS: 75,
} as const;

// Triv — trivial RTypes. INT keeps slot 1 (aliased to INT32).
export const Triv = {
  INT: 1,
  STRING: 2,
  BOOL: 3,
  NULL: 4,
  INT32: 1,
  INT64: 5, // overflow table
  FLOAT32: 6, // inline (IEEE 754 bits reinterpret)
  FLOAT64: 7, // overflow table
  INT8: 8,
  INT16: 9,
  UINT8: 10,
  UINT16: 11,
  UINT32: 12,
  UINT64: 13,
} as const;

// MATH instance encoding — width-aware. inst = (width << 4) | op.
export const RMathWidth = {
  I32: 0,
  I8: 1,
  I16: 2,
  I64: 3,
  U8: 4,
  U16: 5,
  U32: 6,
  U64: 7,
  F32: 8,
  F64: 9,
} as const;

export const RMath = { PLUS: 1, MINUS: 2, MUL: 3, DIV: 4, MOD: 5 } as const;

export function mathInst(width: number, op: number): number {
  return ((width & 0xf) << 4) | (op & 0xf);
}
export function mathWidth(inst: number): number {
  return (inst >> 4) & 0xf;
}
export function mathOp(inst: number): number {
  return inst & 0xf;
}
export const RCmp = { EQ: 1, NE: 2, LT: 3, LE: 4, GT: 5, GE: 6 } as const;
export const RLogic = { AND: 1, OR: 2, NOT: 3 } as const;
export const RCond = { IF_THEN: 1, IF_THEN_ELSE: 2 } as const;
export const RBlock = { DO: 1, SEQUENCE: 2, LET: 3 } as const;
export const RMatch = { SWITCH: 1 } as const;

export type NameID = number;

interface Recipe {
  readonly category: NodeID;
  readonly children: readonly NodeID[];
}

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

// Blueprint category helpers — the Form category a native expresses.
export function catCall(): NodeID {
  return { pkg: 1, level: Level.BASIC, type: RBasic.CALL, inst: 1 };
}
export function catWitness(): NodeID {
  return { pkg: 1, level: Level.BASIC, type: RBasic.WITNESS, inst: 1 };
}
export function catAccess(): NodeID {
  return { pkg: 1, level: Level.BASIC, type: RBasic.ACCESS, inst: 1 };
}
export function catMethod(): NodeID {
  return { pkg: 1, level: Level.BASIC, type: RBasic.METHOD, inst: 1 };
}
export function catListNat(): NodeID {
  return { pkg: 1, level: Level.BASIC, type: RBasic.LIST, inst: 1 };
}
export function catCompareEq(): NodeID {
  return { pkg: 1, level: Level.BASIC, type: RBasic.COMPARE, inst: RCmp.EQ };
}
export function catUndefined(): NodeID {
  return { pkg: 1, level: Level.BASIC, type: RBasic.UNDEFINED, inst: 0 };
}

// BP_TABLE — minimal subset exercised by the verification band. The full
// kernel generates the whole table from the ontology; here only the names the
// pure-op surface's verification touches need to resolve. `bp` fails loud on an
// unknown name (sibling parity: Go/Rust panic), so this is honest, not a
// silent fallback.
const BP_TABLE: Record<string, [number, number, number, number]> = {
  add: [1, 2, 12, 1],
  mul: [1, 2, 12, 3],
};

export type NativeFn = (k: Kernel, args: Value[]) => Value;

export interface NativeEntry {
  readonly name: NameID;
  readonly category: NodeID;
  readonly fn: NativeFn;
}

export class Kernel {
  private byKey = new Map<string, NodeID>();
  byID = new Map<string, Recipe>();
  private nextInst = 1;

  strs: string[] = [];
  private strIdx = new Map<string, NameID>();

  private i64s: bigint[] = [];
  private i64Idx = new Map<bigint, number>();
  private u64s: bigint[] = [];
  private u64Idx = new Map<bigint, number>();
  private f64s: number[] = [];
  private f64Idx = new Map<string, number>();

  natives = new Map<NameID, NativeEntry>();

  constructor() {
    this.registerNatives();
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
    this.byKey.set(k, nid);
    this.byID.set(nodeKey(nid), { category, children });
    return nid;
  }

  internTrivialInt(n: number): NodeID {
    const inst = (n | 0) >>> 0;
    return { pkg: 1, level: Level.TRIVIAL, type: Triv.INT, inst };
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

  internTrivialUint64(n: bigint): NodeID {
    if (n < 0n) throw new Error(`uint64: negative value ${n}`);
    const existing = this.u64Idx.get(n);
    if (existing !== undefined) {
      return { pkg: 1, level: Level.TRIVIAL, type: Triv.UINT64, inst: existing };
    }
    const idx = this.u64s.length;
    this.u64s.push(n);
    this.u64Idx.set(n, idx);
    return { pkg: 1, level: Level.TRIVIAL, type: Triv.UINT64, inst: idx };
  }

  internTrivialFloat64(f: number): NodeID {
    let canonical = f;
    if (Number.isNaN(f)) {
      canonical = NaN;
    } else if (f === 0 && 1 / f === -Infinity) {
      canonical = 0;
    }
    const buf = new ArrayBuffer(8);
    new Float64Array(buf)[0] = canonical;
    const lo = new Uint32Array(buf)[0]!;
    const hi = new Uint32Array(buf)[1]!;
    const key = `${hi.toString(16)}_${lo.toString(16)}`;
    const existing = this.f64Idx.get(key);
    if (existing !== undefined) {
      return { pkg: 1, level: Level.TRIVIAL, type: Triv.FLOAT64, inst: existing };
    }
    const idx = this.f64s.length;
    this.f64s.push(canonical);
    this.f64Idx.set(key, idx);
    return { pkg: 1, level: Level.TRIVIAL, type: Triv.FLOAT64, inst: idx };
  }

  decodeInt64(inst: number): bigint {
    const v = this.i64s[inst];
    if (v === undefined) throw new Error(`int64: bad index ${inst}`);
    return v;
  }
  decodeUint64(inst: number): bigint {
    const v = this.u64s[inst];
    if (v === undefined) throw new Error(`uint64: bad index ${inst}`);
    return v;
  }
  decodeFloat64(inst: number): number {
    const v = this.f64s[inst];
    if (v === undefined) throw new Error(`float64: bad index ${inst}`);
    return v;
  }

  internName(s: string): NameID {
    const existing = this.strIdx.get(s);
    if (existing !== undefined) return existing;
    const idx = this.strs.length;
    this.strs.push(s);
    this.strIdx.set(s, idx);
    return idx;
  }

  category(n: NodeID): NodeID {
    if (n.level === Level.TRIVIAL) return n;
    const r = this.byID.get(nodeKey(n));
    return r ? r.category : n;
  }

  children(n: NodeID): readonly NodeID[] {
    const r = this.byID.get(nodeKey(n));
    return r ? r.children : [];
  }

  trivialValue(n: NodeID): Value {
    if (n.level !== Level.TRIVIAL) {
      throw new Error(`trivialValue: ${nodeKey(n)} is composite`);
    }
    switch (n.type) {
      case Triv.INT32: {
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
        return { kind: "bool", bool: n.inst !== 0 };
      case Triv.NULL:
        return { kind: "null" };
      case Triv.INT64:
        return { kind: "i64", bigint: this.decodeInt64(n.inst) };
      case Triv.UINT64:
        return { kind: "u64", bigint: this.decodeUint64(n.inst) };
      case Triv.FLOAT64:
        return { kind: "f64", float: this.decodeFloat64(n.inst) };
      default:
        throw new Error(`trivialValue: unknown trivial type ${n.type}`);
    }
  }

  identID(n: NodeID): NameID {
    if (n.level === Level.TRIVIAL && n.type === Triv.STRING) {
      return n.inst;
    }
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

  render(v: Value): string {
    switch (v.kind) {
      case "null":
        return "null";
      case "int":
        return String(v.int);
      case "i64":
      case "u64":
        return String(v.bigint);
      case "f32":
      case "f64":
        return String(v.float);
      case "str":
        // Bare, not JSON-quoted — the Go (Value.String) and Rust
        // (Value::display) siblings render strings without quotes, and band
        // outputs are byte-compared across kernels.
        return v.str;
      case "bool":
        return v.bool ? "true" : "false";
      case "list":
        return "[" + v.list.map((x) => this.render(x)).join(", ") + "]";
      case "closure":
        return "<closure>";
      case "nodeid":
        return `@${nodeKey(v.nodeid)}`;
    }
  }

  private registerNative(name: string, category: NodeID, fn: NativeFn): void {
    const id = this.internName(name);
    this.natives.set(id, { name: id, category, fn });
  }

  // The pure-op native surface — strings, lists, and the substrate equality /
  // blueprint doors the verification band exercises. Faithful bodies.
  private registerNatives(): void {
    this.registerNative("str_concat", catMethod(), (_k, args) => ({
      kind: "str",
      str: argStr(args, 0) + argStr(args, 1),
    }));
    this.registerNative("str_eq", catCompareEq(), (_k, args) =>
      boolInt(argStr(args, 0) === argStr(args, 1)),
    );
    // str_len / str_byte_at / byte_to_str — the minimal string "narrow
    // waist": measure, decompose (one raw byte, 0-255), construct (the exact
    // dual). Everything else string-shaped is Form-native on top of these
    // three plus str_concat; see receipts/2026-07-01-narrow-waist-string-cleanup.md.
    //
    // Byte scope, named honestly: implemented via latin1 (each JS UTF-16
    // code unit 0-255 IS the raw byte, losslessly, both directions —
    // verified round-tripping byte 233 through byte_to_str then
    // str_byte_at). Source string literals are read as proper UTF-8
    // (readFileSync(p, "utf8")), so any literal outside the Latin-1 range
    // (a real multi-byte character, not just an accented one) will NOT
    // byte-count identically to fkwu's raw-byte view here — a real, bounded
    // gap, not silently papered over. Every test this walker actually needs
    // to pass today is plain ASCII, where this is exact.
    this.registerNative("str_len", catAccess(), (_k, args) => ({
      kind: "int",
      int: Buffer.from(argStr(args, 0), "latin1").length,
    }));
    this.registerNative("str_byte_at", catAccess(), (_k, args) => {
      const buf = Buffer.from(argStr(args, 0), "latin1");
      const i = argInt(args, 1);
      const v = i < 0 || i >= buf.length ? -1 : buf[i]; // -1 OOB: matches fkwu exactly (verified)
      return { kind: "int", int: v };
    });
    this.registerNative("byte_to_str", catMethod(), (_k, args) => ({
      kind: "str",
      str: Buffer.from([argInt(args, 0) & 0xff]).toString("latin1"),
    }));
    // List ops
    this.registerNative("list", catListNat(), (_k, args) => ({
      kind: "list",
      list: args.slice(),
    }));
    this.registerNative("cons", catListNat(), (_k, args) => {
      const head = args[0] ?? { kind: "null" };
      const tail = argList(args, 1);
      return { kind: "list", list: [head, ...tail] };
    });
    this.registerNative("head", catListNat(), (_k, args) => {
      const lst = argList(args, 0);
      return lst[0] ?? { kind: "null" };
    });
    this.registerNative("tail", catListNat(), (_k, args) => ({
      kind: "list",
      list: argList(args, 0).slice(1),
    }));
    this.registerNative("len", catAccess(), (_k, args) => {
      const v = args[0];
      if (v?.kind === "list") return { kind: "int", int: v.list.length };
      if (v?.kind === "str") return { kind: "int", int: v.str.length };
      return { kind: "int", int: 0 };
    });
    this.registerNative("nth", catAccess(), (_k, args) => {
      const lst = argList(args, 0);
      const i = argInt(args, 1);
      return lst[i] ?? { kind: "null" };
    });
    this.registerNative("empty", catListNat(), () => ({ kind: "list", list: [] }));
    // bp — Blueprint name → NodeID, looked up in BP_TABLE. Unknown name fails
    // loud (sibling parity: Go/Rust panic) — the substrate never invents a
    // NodeID for an unknown name.
    this.registerNative("bp", catWitness(), (_k, args) => {
      const name = argStr(args, 0);
      const entry = BP_TABLE[name];
      if (entry === undefined) {
        throw new Error(
          `bp: unregistered blueprint name ${JSON.stringify(name)} — ` +
            `the substrate never invents a NodeID for an unknown name.`,
        );
      }
      const [pkg, level, type, inst] = entry;
      return { kind: "nodeid", nodeid: { pkg, level, type, inst } };
    });
    this.registerNative("node_eq", catCompareEq(), (_k, args) => {
      const a = argNodeID(args, 0);
      const b = argNodeID(args, 1);
      const equal =
        a.pkg === b.pkg &&
        a.level === b.level &&
        a.type === b.type &&
        a.inst === b.inst;
      return boolInt(equal);
    });
    this.registerNative("value_eq", catCompareEq(), (_k, args) => {
      const a = args[0]!;
      const b = args[1]!;
      return boolInt(valueEqual(a, b));
    });
  }
}

// ===========================================================================
// Values — runtime tagged values   (pure-op subset from kernel.ts)
// ===========================================================================

export type Value =
  | { kind: "null" }
  | { kind: "int"; int: number }
  | { kind: "i64"; bigint: bigint }
  | { kind: "u64"; bigint: bigint }
  | { kind: "f32"; float: number }
  | { kind: "f64"; float: number }
  | { kind: "str"; str: string }
  | { kind: "bool"; bool: boolean }
  | { kind: "list"; list: Value[] }
  | { kind: "closure"; closure: Closure }
  | { kind: "nodeid"; nodeid: NodeID };

export interface Closure {
  readonly name: NameID;
  readonly params: readonly NameID[];
  readonly body: NodeID;
  readonly env: Frame;
}

// Frame — scope primitive.
export class Frame {
  readonly parent: Frame | null;
  private readonly keys: NameID[] = [];
  private readonly vals: Value[] = [];

  constructor(parent: Frame | null = null) {
    this.parent = parent;
  }

  bind(name: NameID, value: Value): void {
    const idx = this.keys.indexOf(name);
    if (idx >= 0) {
      this.vals[idx] = value;
      return;
    }
    this.keys.push(name);
    this.vals.push(value);
  }

  lookup(name: NameID): Value | undefined {
    let frame: Frame | null = this;
    while (frame !== null) {
      const idx = frame.keys.indexOf(name);
      if (idx >= 0) return frame.vals[idx];
      frame = frame.parent;
    }
    return undefined;
  }
}

// arg helpers — faithful coercion at the native door (axiom-1: true IS 1).
function argInt(args: Value[], i: number): number {
  const v = args[i];
  if (!v) throw new Error(`arg ${i}: missing`);
  if (v.kind === "bool") return v.bool ? 1 : 0;
  if (v.kind === "int") return v.int;
  if (v.kind === "i64" || v.kind === "u64") return Number(v.bigint);
  throw new Error(`arg ${i}: expected int-like, got ${v.kind}`);
}
function argStr(args: Value[], i: number): string {
  const v = args[i];
  if (v?.kind !== "str") throw new Error(`arg ${i}: expected str`);
  return v.str;
}
function argList(args: Value[], i: number): Value[] {
  const v = args[i];
  if (v?.kind !== "list") throw new Error(`arg ${i}: expected list`);
  return v.list;
}
function argNodeID(args: Value[], i: number): NodeID {
  const v = args[i];
  if (v?.kind !== "nodeid") throw new Error(`arg ${i}: expected nodeid`);
  return v.nodeid;
}

// ===========================================================================
// Reader — `.fk` text → recipe tree   (verbatim from reader.ts)
//
// The independent lexer. The scientific-notation float case (line marked
// below) is exactly the shared bug the four-walker witness exists to catch:
// `1e-05` must tokenize as a float, not stop at `e`.
// ===========================================================================

interface Token {
  kind: "lparen" | "rparen" | "int" | "float" | "str" | "ident";
  text: string;
  pos: number;
}

function tokenize(src: string): Token[] {
  const toks: Token[] = [];
  let i = 0;
  while (i < src.length) {
    const c = src[i];
    if (c === undefined) break;
    if (c === " " || c === "\t" || c === "\n" || c === "\r") {
      i++;
      continue;
    }
    if (c === ";") {
      while (i < src.length && src[i] !== "\n") i++;
      continue;
    }
    if (c === "(") {
      toks.push({ kind: "lparen", text: "(", pos: i });
      i++;
      continue;
    }
    if (c === ")") {
      toks.push({ kind: "rparen", text: ")", pos: i });
      i++;
      continue;
    }
    if (c === '"' || c === "'") {
      const quote = c;
      const start = i;
      i++;
      let s = "";
      while (i < src.length && src[i] !== quote) {
        if (src[i] === "\\" && i + 1 < src.length) {
          const next = src[i + 1];
          if (next === "n") s += "\n";
          else if (next === "r") s += "\r";
          else if (next === "t") s += "\t";
          else if (next === "\\") s += "\\";
          else if (next === '"') s += '"';
          else if (next === "'") s += "'";
          else s += next ?? "";
          i += 2;
          continue;
        }
        s += src[i];
        i++;
      }
      if (src[i] !== quote) throw new Error(`unterminated string at ${start}`);
      i++;
      toks.push({ kind: "str", text: s, pos: start });
      continue;
    }
    const start = i;
    while (i < src.length) {
      const ch = src[i];
      if (ch === undefined) break;
      if (
        ch === " " ||
        ch === "\t" ||
        ch === "\n" ||
        ch === "\r" ||
        ch === "(" ||
        ch === ")" ||
        ch === ";"
      )
        break;
      i++;
    }
    const text = src.slice(start, i);
    if (/^-?\d+$/.test(text)) {
      toks.push({ kind: "int", text, pos: start });
    } else if (/^-?\d+\.\d+(e-?\d+)?$/i.test(text) || /^-?\d+e-?\d+$/i.test(text)) {
      // ← the scientific-notation float case. `1e-05` / `6.66e-15` are floats;
      //   an independent lexer that stopped at `e` (no decimal point) would
      //   diverge here — the bug this four-walker witness exists to catch.
      toks.push({ kind: "float", text, pos: start });
    } else {
      toks.push({ kind: "ident", text, pos: start });
    }
  }
  return toks;
}

interface ParseState {
  toks: Token[];
  i: number;
}

function peek(s: ParseState): Token | undefined {
  return s.toks[s.i];
}

function consume(s: ParseState): Token {
  const t = s.toks[s.i];
  if (t === undefined) throw new Error("unexpected end of input");
  s.i++;
  return t;
}

export function readAll(k: Kernel, src: string): NodeID {
  const s: ParseState = { toks: tokenize(src), i: 0 };
  const forms: NodeID[] = [];
  while (s.i < s.toks.length) {
    forms.push(readOne(k, s));
  }
  if (forms.length === 0) return k.internTrivialNull();
  if (forms.length === 1) return forms[0]!;
  return k.intern(
    { pkg: 1, level: Level.BASIC, type: RBasic.BLOCK, inst: RBlock.DO },
    forms,
  );
}

function readOne(k: Kernel, s: ParseState): NodeID {
  const t = consume(s);
  if (t.kind === "int") {
    const big = BigInt(t.text);
    if (big >= -2147483648n && big <= 2147483647n) {
      return k.internTrivialInt(Number(big));
    }
    return k.internTrivialInt64(big);
  }
  if (t.kind === "float") {
    return k.internTrivialFloat64(parseFloat(t.text));
  }
  if (t.kind === "str") {
    return k.internString(t.text);
  }
  if (t.kind === "ident") {
    if (t.text === "true") return k.internTrivialBool(true);
    if (t.text === "false") return k.internTrivialBool(false);
    if (t.text === "null") return k.internTrivialNull();
    return k.intern(
      { pkg: 1, level: Level.BASIC, type: RBasic.IDENT, inst: 1 },
      [k.internString(t.text)],
    );
  }
  if (t.kind === "lparen") {
    return readList(k, s);
  }
  throw new Error(`unexpected token ${t.kind} at ${t.pos}`);
}

function readList(k: Kernel, s: ParseState): NodeID {
  const head = peek(s);
  if (head === undefined) throw new Error("unterminated list");
  if (head.kind === "rparen") {
    consume(s);
    return k.internTrivialNull();
  }
  if (head.kind === "ident") {
    const verb = head.text;
    if (verb === "let") {
      consume(s);
      return readLet(k, s);
    }
    if (verb === "defn") {
      consume(s);
      return readDefn(k, s);
    }
    if (verb === "if") {
      consume(s);
      const kids = readChildrenUntilRparen(k, s);
      if (kids.length === 2) {
        return k.intern(
          { pkg: 1, level: Level.BASIC, type: RBasic.COND, inst: RCond.IF_THEN },
          kids,
        );
      }
      if (kids.length === 3) {
        return k.intern(
          { pkg: 1, level: Level.BASIC, type: RBasic.COND, inst: RCond.IF_THEN_ELSE },
          kids,
        );
      }
      throw new Error("if: need 2 or 3 args");
    }
    consume(s);
    const kids = readChildrenUntilRparen(k, s);
    return buildVerb(k, verb, kids);
  }
  const callee = readOne(k, s);
  const args = readChildrenUntilRparen(k, s);
  return k.intern(
    { pkg: 1, level: Level.BASIC, type: RBasic.FNCALL, inst: 1 },
    [callee, ...args],
  );
}

function readChildrenUntilRparen(k: Kernel, s: ParseState): NodeID[] {
  const out: NodeID[] = [];
  while (true) {
    const t = peek(s);
    if (t === undefined) throw new Error("unterminated list");
    if (t.kind === "rparen") {
      consume(s);
      return out;
    }
    out.push(readOne(k, s));
  }
}

// (let <name> <value>)
function readLet(k: Kernel, s: ParseState): NodeID {
  const nameTok = consume(s);
  if (nameTok.kind !== "ident") throw new Error("let: name must be identifier");
  const value = readOne(k, s);
  const close = consume(s);
  if (close.kind !== "rparen") throw new Error("let: expected )");
  const nameTrivial: NodeID = {
    pkg: 1,
    level: Level.TRIVIAL,
    type: Triv.STRING,
    inst: k.internName(nameTok.text),
  };
  return k.intern(
    { pkg: 1, level: Level.BASIC, type: RBasic.BLOCK, inst: RBlock.LET },
    [nameTrivial, value],
  );
}

// (defn <name> (<params>...) <body>) — pure-op shape (untyped, inst=1). The
// typed/parametric :tparams/:ret surface of the full reader is "more" and not
// part of the pure-op witness; an unsupported annotation would land as an
// ident param and be caught honestly.
function readDefn(k: Kernel, s: ParseState): NodeID {
  const nameTok = consume(s);
  if (nameTok.kind !== "ident") throw new Error("defn: name must be identifier");
  const lparen = consume(s);
  if (lparen.kind !== "lparen") throw new Error("defn: expected ( for params");
  const paramTrivials: NodeID[] = [];
  while (true) {
    const t = peek(s);
    if (t === undefined) throw new Error("defn: unterminated param list");
    if (t.kind === "rparen") {
      consume(s);
      break;
    }
    if (t.kind !== "ident") throw new Error("defn: params must be identifiers");
    consume(s);
    paramTrivials.push({
      pkg: 1,
      level: Level.TRIVIAL,
      type: Triv.STRING,
      inst: k.internName(t.text),
    });
  }
  const body = readOne(k, s);
  const close = consume(s);
  if (close.kind !== "rparen") throw new Error("defn: expected )");
  const nameTrivial: NodeID = {
    pkg: 1,
    level: Level.TRIVIAL,
    type: Triv.STRING,
    inst: k.internName(nameTok.text),
  };
  const paramsBlock = k.intern(
    { pkg: 1, level: Level.BASIC, type: RBasic.BLOCK, inst: RBlock.SEQUENCE },
    paramTrivials,
  );
  return k.intern(
    { pkg: 1, level: Level.BASIC, type: RBasic.FNDEF, inst: 1 },
    [nameTrivial, paramsBlock, body],
  );
}

// buildVerb — map a surface verb to its RBasic recipe. Matches Go/Rust/TS
// buildVerb exactly so the same source produces the same NodeIDs. Restricted
// to the pure-op surface verbs.
function buildVerb(k: Kernel, verb: string, args: NodeID[]): NodeID {
  switch (verb) {
    case "do":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.BLOCK, inst: RBlock.DO },
        args,
      );
    case "seq":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.BLOCK, inst: RBlock.SEQUENCE },
        args,
      );
    case "add":
    case "+":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.MATH, inst: RMath.PLUS },
        args,
      );
    case "sub":
    case "-":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.MATH, inst: RMath.MINUS },
        args,
      );
    case "mul":
    case "*":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.MATH, inst: RMath.MUL },
        args,
      );
    case "div":
    case "/":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.MATH, inst: RMath.DIV },
        args,
      );
    case "mod":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.MATH, inst: RMath.MOD },
        args,
      );
    case "addf":
    case "+.":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.MATH, inst: mathInst(RMathWidth.F64, RMath.PLUS) },
        args,
      );
    case "subf":
    case "-.":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.MATH, inst: mathInst(RMathWidth.F64, RMath.MINUS) },
        args,
      );
    case "mulf":
    case "*.":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.MATH, inst: mathInst(RMathWidth.F64, RMath.MUL) },
        args,
      );
    case "divf":
    case "/.":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.MATH, inst: mathInst(RMathWidth.F64, RMath.DIV) },
        args,
      );
    case "eq":
    case "==":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.COMPARE, inst: RCmp.EQ },
        args,
      );
    case "ne":
    case "!=":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.COMPARE, inst: RCmp.NE },
        args,
      );
    case "lt":
    case "<":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.COMPARE, inst: RCmp.LT },
        args,
      );
    case "le":
    case "<=":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.COMPARE, inst: RCmp.LE },
        args,
      );
    case "gt":
    case ">":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.COMPARE, inst: RCmp.GT },
        args,
      );
    case "ge":
    case ">=":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.COMPARE, inst: RCmp.GE },
        args,
      );
    case "and":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.LOGIC, inst: RLogic.AND },
        args,
      );
    case "or":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.LOGIC, inst: RLogic.OR },
        args,
      );
    case "not":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.LOGIC, inst: RLogic.NOT },
        args,
      );
    case "list":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.LIST, inst: 1 },
        args,
      );
    case "params":
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.BLOCK, inst: RBlock.SEQUENCE },
        args,
      );
    default: {
      // Function call: bare-string-trivial callee, then args.
      const nameTrivial: NodeID = {
        pkg: 1,
        level: Level.TRIVIAL,
        type: Triv.STRING,
        inst: k.internName(verb),
      };
      return k.intern(
        { pkg: 1, level: Level.BASIC, type: RBasic.FNCALL, inst: 1 },
        [nameTrivial, ...args],
      );
    }
  }
}

// ===========================================================================
// Walker — recipe → value   (the pure dispatch, verbatim from kernel.ts)
// ===========================================================================

export function walk(k: Kernel, node: NodeID, frame: Frame): Value {
  if (node.level === Level.TRIVIAL) {
    return k.trivialValue(node);
  }
  const cat = k.category(node);
  const kids = k.children(node);

  switch (cat.type) {
    case RBasic.IDENT: {
      const id = k.identID(node);
      const v = frame.lookup(id);
      if (v !== undefined) return v;
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
    case RBasic.COND:
      return walkCond(k, cat.inst, kids, frame);
    case RBasic.BLOCK:
      return walkBlock(k, cat.inst, kids, frame);
    case RBasic.FNDEF:
      return walkFnDef(k, kids, frame);
    case RBasic.FNCALL:
      return walkFnCall(k, kids, frame);
    case RBasic.LIST: {
      const items = kids.map((c) => walk(k, c, frame));
      return { kind: "list", list: items };
    }
    case RBasic.ALIAS:
      if (kids.length >= 2) return { kind: "nodeid", nodeid: kids[1]! };
      return { kind: "nodeid", nodeid: node };
    default:
      throw new Error(`walk: unsupported RBasic type ${cat.type}`);
  }
}

function expectInt(v: Value, op: string): number {
  if (v.kind === "bool") return v.bool ? 1 : 0;
  if (v.kind === "i64" || v.kind === "u64") return Number(v.bigint);
  if (v.kind !== "int") throw new Error(`${op}: expected int-like, got ${v.kind}`);
  return v.int;
}

function expectFloat(v: Value, op: string): number {
  if (v.kind === "bool") return v.bool ? 1 : 0;
  if (v.kind === "f32" || v.kind === "f64") return v.float;
  if (v.kind === "int") return v.int;
  if (v.kind === "i64" || v.kind === "u64") return Number(v.bigint);
  throw new Error(`${op}: expected number-like, got ${v.kind}`);
}

function expectBigInt(v: Value, op: string): bigint {
  if (v.kind === "bool") return v.bool ? 1n : 0n;
  if (v.kind === "i64" || v.kind === "u64") return v.bigint;
  if (v.kind === "int") return BigInt(v.int);
  throw new Error(`${op}: expected integer-like, got ${v.kind}`);
}

function walkMath(
  k: Kernel,
  inst: number,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  if (kids.length < 2) throw new Error("math: need at least 2 args");
  const width = mathWidth(inst);
  const op = mathOp(inst);

  if (width === RMathWidth.F64) {
    let acc = expectFloat(walk(k, kids[0]!, frame), "math.f64");
    for (let i = 1; i < kids.length; i++) {
      const x = expectFloat(walk(k, kids[i]!, frame), "math.f64");
      switch (op) {
        case RMath.PLUS: acc = acc + x; break;
        case RMath.MINUS: acc = acc - x; break;
        case RMath.MUL: acc = acc * x; break;
        case RMath.DIV: acc = acc / x; break;
        case RMath.MOD: acc = acc - Math.floor(acc / x) * x; break;
        default: throw new Error(`math.f64: unknown op ${op}`);
      }
    }
    return { kind: "f64", float: acc };
  }

  if (width === RMathWidth.F32) {
    let acc = expectFloat(walk(k, kids[0]!, frame), "math.f32");
    for (let i = 1; i < kids.length; i++) {
      const x = expectFloat(walk(k, kids[i]!, frame), "math.f32");
      switch (op) {
        case RMath.PLUS: acc = Math.fround(acc + x); break;
        case RMath.MINUS: acc = Math.fround(acc - x); break;
        case RMath.MUL: acc = Math.fround(acc * x); break;
        case RMath.DIV: acc = Math.fround(acc / x); break;
        case RMath.MOD: acc = Math.fround(acc - Math.floor(acc / x) * x); break;
        default: throw new Error(`math.f32: unknown op ${op}`);
      }
    }
    return { kind: "f32", float: acc };
  }

  if (width === RMathWidth.I64 || width === RMathWidth.U64) {
    let acc = expectBigInt(walk(k, kids[0]!, frame), "math.i64");
    for (let i = 1; i < kids.length; i++) {
      const x = expectBigInt(walk(k, kids[i]!, frame), "math.i64");
      switch (op) {
        case RMath.PLUS: acc = acc + x; break;
        case RMath.MINUS: acc = acc - x; break;
        case RMath.MUL: acc = acc * x; break;
        case RMath.DIV:
          if (x === 0n) throw new Error("division by zero");
          acc = acc / x;
          break;
        case RMath.MOD:
          if (x === 0n) throw new Error("modulo by zero");
          acc = acc % x;
          break;
        default: throw new Error(`math.i64: unknown op ${op}`);
      }
    }
    return width === RMathWidth.I64
      ? { kind: "i64", bigint: acc }
      : { kind: "u64", bigint: acc };
  }

  // Default integer path — the bare-width op carries Python's arbitrary-
  // precision integer semantics (exact to 2^53 in a JS number, matching Go/Rust
  // int64 folds across that range), with float promotion when any operand walks
  // to a float (int+float→float, like the Rust/Go MATH arms and Python).
  const vals = kids.map((kid) => walk(k, kid!, frame));
  if (vals.some((v) => v.kind === "f32" || v.kind === "f64")) {
    let facc = expectFloat(vals[0]!, "math.f64");
    for (let i = 1; i < vals.length; i++) {
      const x = expectFloat(vals[i]!, "math.f64");
      switch (op) {
        case RMath.PLUS: facc = facc + x; break;
        case RMath.MINUS: facc = facc - x; break;
        case RMath.MUL: facc = facc * x; break;
        case RMath.DIV: facc = facc / x; break;
        case RMath.MOD: facc = facc - Math.floor(facc / x) * x; break;
        default: throw new Error(`math.f64: unknown op ${op}`);
      }
    }
    return { kind: "f64", float: facc };
  }
  let acc = expectInt(vals[0]!, "math.int");
  for (let i = 1; i < vals.length; i++) {
    const x = expectInt(vals[i]!, "math.int");
    switch (op) {
      case RMath.PLUS: acc = acc + x; break;
      case RMath.MINUS: acc = acc - x; break;
      case RMath.MUL: acc = acc * x; break;
      case RMath.DIV:
        if (x === 0) throw new Error("division by zero");
        acc = Math.trunc(acc / x);
        break;
      case RMath.MOD:
        if (x === 0) throw new Error("modulo by zero");
        acc = acc - Math.trunc(acc / x) * x;
        break;
      default: throw new Error(`math.int: unknown op ${op}`);
    }
  }
  return { kind: "int", int: acc };
}

// boolInt — truth answers join axiom-1's 0/1 integer states so they flow
// directly into arithmetic.
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

  let r: boolean;
  if (av.kind === "f32" || av.kind === "f64" || bv.kind === "f32" || bv.kind === "f64") {
    const a = av.kind === "bool" ? (av.bool ? 1 : 0) : expectFloat(av, "compare");
    const b = bv.kind === "bool" ? (bv.bool ? 1 : 0) : expectFloat(bv, "compare");
    switch (op) {
      case RCmp.EQ: r = a === b; break;
      case RCmp.NE: r = a !== b; break;
      case RCmp.LT: r = a < b; break;
      case RCmp.LE: r = a <= b; break;
      case RCmp.GT: r = a > b; break;
      case RCmp.GE: r = a >= b; break;
      default: throw new Error(`compare: unknown op ${op}`);
    }
  } else if (av.kind === "i64" || av.kind === "u64" || bv.kind === "i64" || bv.kind === "u64") {
    const a = av.kind === "bool" ? (av.bool ? 1n : 0n) : expectBigInt(av, "compare");
    const b = bv.kind === "bool" ? (bv.bool ? 1n : 0n) : expectBigInt(bv, "compare");
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
    const a = av.kind === "bool" ? (av.bool ? 1 : 0) : expectInt(av, "compare");
    const b = bv.kind === "bool" ? (bv.bool ? 1 : 0) : expectInt(bv, "compare");
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

function valueEqual(a: Value, b: Value): boolean {
  const aNum = isNumericValue(a);
  const bNum = isNumericValue(b);
  if (aNum && bNum) {
    if (a.kind === "i64" || a.kind === "u64" || b.kind === "i64" || b.kind === "u64") {
      return numericToBig(a) === numericToBig(b);
    }
    return numericToNum(a) === numericToNum(b);
  }
  if (a.kind !== b.kind) return false;
  switch (a.kind) {
    case "null":
      return true;
    case "str":
      return a.str === (b as { str: string }).str;
    case "bool":
      return a.bool === (b as { bool: boolean }).bool;
    case "list": {
      const bl = (b as { list: Value[] }).list;
      return a.list.length === bl.length && a.list.every((item, idx) => valueEqual(item, bl[idx]!));
    }
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

function isNumericValue(
  v: Value,
): v is
  | { kind: "int"; int: number }
  | { kind: "i64"; bigint: bigint }
  | { kind: "u64"; bigint: bigint }
  | { kind: "f32"; float: number }
  | { kind: "f64"; float: number } {
  return (
    v.kind === "int" ||
    v.kind === "i64" ||
    v.kind === "u64" ||
    v.kind === "f32" ||
    v.kind === "f64"
  );
}

function numericToNum(v: Value): number {
  if (v.kind === "f32" || v.kind === "f64") return v.float;
  if (v.kind === "i64" || v.kind === "u64") return Number(v.bigint);
  if (v.kind === "int") return v.int;
  throw new Error(`numericToNum: ${v.kind} is not numeric`);
}

function numericToBig(v: Value): bigint {
  if (v.kind === "i64" || v.kind === "u64") return v.bigint;
  if (v.kind === "f32" || v.kind === "f64") return BigInt(Math.trunc(v.float));
  if (v.kind === "int") return BigInt(v.int);
  throw new Error(`numericToBig: ${v.kind} is not numeric`);
}

function truthy(v: Value): boolean {
  switch (v.kind) {
    case "bool":
      return v.bool;
    case "null":
      return false;
    case "int":
      return v.int !== 0;
    case "i64":
    case "u64":
      return v.bigint !== 0n;
    case "f32":
    case "f64":
      return v.float !== 0 && !isNaN(v.float);
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

function walkCond(
  k: Kernel,
  op: number,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  if (op === RCond.IF_THEN) {
    if (kids.length !== 2) throw new Error("if: need 2 args");
    const c = walk(k, kids[0]!, frame);
    return truthy(c) ? walk(k, kids[1]!, frame) : { kind: "null" };
  }
  if (kids.length !== 3) throw new Error("if/else: need 3 args");
  const c = walk(k, kids[0]!, frame);
  return truthy(c) ? walk(k, kids[1]!, frame) : walk(k, kids[2]!, frame);
}

function walkBlock(
  k: Kernel,
  op: number,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
  if (op === RBlock.LET) {
    if (kids.length !== 2) throw new Error("let: need 2 args (name, value)");
    const name = kids[0]!;
    if (name.level !== Level.TRIVIAL || name.type !== Triv.STRING) {
      throw new Error("let: name must be a string trivial");
    }
    const value = walk(k, kids[1]!, frame);
    frame.bind(name.inst, value);
    return value;
  }
  let result: Value = { kind: "null" };
  for (const c of kids) {
    result = walk(k, c, frame);
  }
  return result;
}

// FNDEF children: [name-trivial, params-SEQUENCE-of-name-trivials, body]
function walkFnDef(k: Kernel, kids: readonly NodeID[], frame: Frame): Value {
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

  const closure: Closure = { name: nameID, params, body, env: frame };
  const value: Value = { kind: "closure", closure };
  frame.bind(nameID, value);
  return value;
}

// FNCALL children: [callee, arg0, arg1, ...]
function walkFnCall(k: Kernel, kids: readonly NodeID[], frame: Frame): Value {
  if (kids.length < 1) throw new Error("call: need callee");
  const calleeNode = kids[0]!;

  let calleeName: NameID | null = null;
  if (calleeNode.level === Level.TRIVIAL && calleeNode.type === Triv.STRING) {
    calleeName = calleeNode.inst;
  } else if (calleeNode.level === Level.BASIC && calleeNode.type === RBasic.IDENT) {
    calleeName = k.identID(calleeNode);
  }

  if (calleeName !== null) {
    const rawName = calleeName;
    // Native dispatch
    const ne = k.natives.get(rawName);
    if (ne !== undefined) {
      const args: Value[] = [];
      for (let i = 1; i < kids.length; i++) {
        args.push(walk(k, kids[i]!, frame));
      }
      return ne.fn(k, args);
    }
    // Closure via frame
    const v = frame.lookup(rawName);
    if (v === undefined) {
      throw new Error(`call: unbound ${k.nameStr(rawName)}`);
    }
    if (v.kind !== "closure") {
      throw new Error(`call: ${k.nameStr(rawName)} is not a closure (got ${v.kind})`);
    }
    return invokeClosure(k, v.closure, kids, frame);
  }

  const calleeVal = walk(k, calleeNode, frame);
  if (calleeVal.kind !== "closure") {
    throw new Error(`call: callee is not a closure (got ${calleeVal.kind})`);
  }
  return invokeClosure(k, calleeVal.closure, kids, frame);
}

function invokeClosure(
  k: Kernel,
  closure: Closure,
  kids: readonly NodeID[],
  frame: Frame,
): Value {
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
  return walk(k, closure.body, callFrame);
}

// ===========================================================================
// CLI — concatenate the .fk file list, evaluate, render one value.
// ===========================================================================

function main(): void {
  const paths = process.argv.slice(2);
  if (paths.length === 0) {
    console.error("usage: node --import tsx main.ts <file.fk> [<file.fk> ...]");
    process.exit(2);
  }
  const missing = paths.filter((p) => !existsSync(p));
  if (missing.length > 0) {
    for (const p of missing) console.error(`input file not found: ${p}`);
    process.exit(2);
  }
  const src = paths.map((p) => readFileSync(p, "utf8")).join("\n");
  const k = new Kernel();
  const frame = new Frame(null);
  const node = readAll(k, src);
  const value = walk(k, node, frame);
  console.log(k.render(value));
}

main();
