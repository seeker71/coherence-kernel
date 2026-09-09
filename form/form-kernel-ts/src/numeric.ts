// numeric.ts — generic numeric leaf interface, format-recipe driven.
//
// A numeric value is a NodeID whose level=BASIC, type=RBasicNumeric,
// and inst encodes:
//   - the format-recipe handle (small int, looked up in the kernel's
//     format-table) in the upper 16 bits
//   - the inline-value-handle in the lower 16 bits (for values that
//     fit; larger values become a single child carrying encoded bits)
//
// This is Tier 1 — format-recipe-driven. Tier 0 (existing INT, FP64
// trivials) remains as a fast-path alias for hot formats; nothing
// here forces removing them.

import { Kernel, Level, RBasic, type NodeID } from "./kernel.ts";
import {
  applyArith,
  canonicalize,
  type ArithOp,
  type FormatLibrary,
  type FormatRecipe,
  type Numberish,
} from "./formats.ts";

// RBasic.NUMERIC — a new well-known RBasic category for format-recipe-
// driven numeric leaves. Distinct from RBasic.MATH (operations); this
// is for VALUES carrying a format identity.
export const RBasicNumeric = RBasic.NUMERIC;

// Format-handle assignment. The kernel keeps a sequential table of
// format-recipes; each registered format gets an integer handle.
// Cross-kernel agreement requires the handle order to be deterministic
// per the canonical bootstrap library.
export class FormatTable {
  private byHandle: FormatRecipe[] = [];
  private byNodeID = new Map<string, number>();

  register(fmt: FormatRecipe): number {
    const key = `${fmt.nodeID.pkg}.${fmt.nodeID.level}.${fmt.nodeID.type}.${fmt.nodeID.inst}`;
    const existing = this.byNodeID.get(key);
    if (existing !== undefined) return existing;
    const h = this.byHandle.length;
    this.byHandle.push(fmt);
    this.byNodeID.set(key, h);
    return h;
  }

  get(h: number): FormatRecipe | undefined {
    return this.byHandle[h];
  }

  // Proof execution uses the existing arithmetic interpreter. Native
  // specialization belongs to the Form compiler running on fkwu.
  handler(h: number, op: ArithOp): (a: Numberish, b: Numberish) => Numberish {
    const fmt = this.byHandle[h];
    if (!fmt) throw new Error(`unknown format-handle ${h}`);
    return (a, b) => applyArith(fmt, op, a, b);
  }
}

// Encode a numeric leaf NodeID. For values that fit in 16 bits, inline;
// otherwise the value lives as a single child trivial.
export function internNumeric(
  k: Kernel,
  formats: FormatTable,
  fmt: FormatRecipe,
  rawValue: Numberish,
): NodeID {
  const handle = formats.register(fmt);
  const canonical = canonicalize(fmt, rawValue);

  // For inline-fitting integer-like values in formats ≤16 bits with
  // simple integer encoding, encode value into inst's lower 16 bits.
  // For larger / float values, route through a child trivial.
  if (
    fmt.bits <= 16 &&
    fmt.arithmeticHint === "native-int" &&
    typeof canonical === "number" &&
    Number.isInteger(canonical) &&
    canonical >= -32768 && canonical <= 32767
  ) {
    const inst = ((handle & 0xffff) << 16) | (canonical & 0xffff);
    return {
      pkg: 1,
      level: Level.BASIC,
      type: RBasicNumeric,
      inst,
    };
  }

  // General path: composite with one child carrying the encoded value
  // through the substrate's number/string overflow mechanisms.
  const valueRecipe = encodeOverflowValue(k, fmt, canonical);
  return k.intern(
    { pkg: 1, level: Level.BASIC, type: RBasicNumeric, inst: handle },
    [valueRecipe],
  );
}

// Encode a value into a substrate leaf that the kernel can decode
// without knowing the format. For doubles, route through the f64
// overflow table; for bigints, through i64. The encoding is opaque
// to the substrate — the format-recipe's storage-hint tells the
// decoder how to interpret it.
function encodeOverflowValue(
  k: Kernel,
  fmt: FormatRecipe,
  v: Numberish,
): NodeID {
  if (typeof v === "bigint") return k.internTrivialInt64(v);
  // Use the f64 overflow table for any float-shaped value.
  return k.internTrivialFloat64(v);
}

export function decodeNumeric(
  k: Kernel,
  formats: FormatTable,
  n: NodeID,
): { fmt: FormatRecipe; value: Numberish } {
  if (n.type !== RBasicNumeric) {
    throw new Error(`decodeNumeric: not a numeric NodeID: ${n.type}`);
  }
  // Inline encoding? Lower 16 bits are the value, upper 16 are the
  // format-handle.
  // Detection: inline values carry a 16-bit value range; we use a flag
  // bit by convention — but for v0 we differentiate by whether the
  // NodeID is composite (has children in byID) or not.
  const recipe = k.byID.get(`${n.pkg}.${n.level}.${n.type}.${n.inst}`);
  if (!recipe) {
    // Inline path
    const handle = (n.inst >>> 16) & 0xffff;
    const u16 = n.inst & 0xffff;
    const value = u16 & 0x8000 ? u16 - 0x10000 : u16;
    const fmt = formats.get(handle);
    if (!fmt) throw new Error(`inline numeric: unknown handle ${handle}`);
    return { fmt, value };
  }
  // Composite path
  const handle = n.inst;
  const fmt = formats.get(handle);
  if (!fmt) throw new Error(`numeric: unknown format-handle ${handle}`);
  const valueChild = recipe.children[0];
  if (!valueChild) throw new Error("numeric composite: no value child");
  // Decode the value child through the kernel's trivial decoder.
  const tv = k.trivialValue(valueChild);
  let value: Numberish;
  if (tv.kind === "f64" || tv.kind === "f32") value = tv.float;
  else if (tv.kind === "i64" || tv.kind === "u64") value = tv.bigint;
  else if (
    tv.kind === "int" ||
    tv.kind === "i8" || tv.kind === "i16" ||
    tv.kind === "u8" || tv.kind === "u16" || tv.kind === "u32"
  )
    value = tv.int;
  else throw new Error(`numeric composite: unsupported value kind ${tv.kind}`);
  return { fmt, value };
}

// applyMath — invoke arithmetic through the format-handler cache.
// This is the Pass 1 hot-path optimization. After Pass 2, the
// compiler emits direct calls bypassing this dispatcher for known
// formats; Pass 1 path remains for the walker and unknown formats.
export function applyMath(
  formats: FormatTable,
  fmt: FormatRecipe,
  op: ArithOp,
  a: Numberish,
  b: Numberish,
): Numberish {
  const handle = formats.register(fmt);
  return formats.handler(handle, op)(a, b);
}

// Format-library helper: register all canonical formats into the table
// so they can be referenced by handle.
export function registerFormats(table: FormatTable, lib: FormatLibrary): void {
  for (const fmt of Object.values(lib)) {
    table.register(fmt);
  }
}
