// S-expression reader — plain Form text → recipe tree, read as fkwu reads it.
//
// Verb names (`add`, `sub`, `mul`, `eq`, `le`, ...) intern to specific
// RBasic recipes; everything else is a function call.

import {
  Kernel,
  Level,
  RBasic,
  RBlock,
  RCmp,
  RCond,
  RLogic,
  RMath,
  Triv,
  type NodeID,
} from "./kernel.ts";

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
    // A string is double-quoted; \n \t \r \" \\ are its escapes, and any other
    // backslash stands for itself, as fkwu's fk_smkstr reads it.
    if (c === '"') {
      const start = i;
      i++;
      let s = "";
      while (i < src.length && src[i] !== '"') {
        if (src[i] === "\\" && i + 1 < src.length) {
          const next = src[i + 1];
          const esc = next === "n" ? "\n" : next === "t" ? "\t" : next === "r" ? "\r" : next === '"' || next === "\\" ? next : "";
          if (esc !== "") {
            s += esc;
            i += 2;
            continue;
          }
        }
        s += src[i];
        i++;
      }
      if (src[i] !== '"') throw new Error(`unterminated string at ${start}`);
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
    // fkwu's number leaf: digits, then a "." with digits or a signed exponent makes it a float
    if (/^-?\d+$/.test(text)) {
      toks.push({ kind: "int", text, pos: start });
    } else if (/^-?\d+(\.\d*)?([eE][+-]?\d+)?$/.test(text)) {
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
  // attribute — when set, every parenthesized form is recorded with the
  // file:line:col of its opening paren so fatal diagnostics can name the
  // Form source line (sibling to the Go/Rust readers).
  attribute: ((node: NodeID, pos: number) => void) | null;
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

export function readForm(k: Kernel, src: string): NodeID {
  const s: ParseState = { toks: tokenize(src), i: 0, attribute: makeAttributor(k, src) };
  const node = readOne(k, s);
  if (s.i !== s.toks.length) {
    const t = s.toks[s.i];
    throw new Error(`extra tokens after expression at ${t?.pos}`);
  }
  return node;
}

// stopAtStrayParen — a `)` with no `(` open ends the reading before anything
// runs, as fkwu's whole-source balance does. The stop names the file, line and
// column, and voices one organ-health reading.
function stopAtStrayParen(k: Kernel, src: string, toks: Token[]): void {
  let depth = 0;
  for (const t of toks) {
    if (t.kind === "lparen") depth++;
    else if (t.kind === "rparen" && depth > 0) depth--;
    else if (t.kind === "rparen") {
      let line = 1;
      let lineStart = 0;
      for (let i = 0; i < t.pos; i++) {
        if (src[i] === "\n") {
          line++;
          lineStart = i + 1;
        }
      }
      const col = t.pos - lineStart + 1;
      const owner = k.resolveReadingLine(line);
      const place = owner === null ? `line ${line} col ${col}` : `${owner.file}:${owner.line}:${col}`;
      const detail = "[unbalanced-source] stray ')' closes a form that was never opened -- refusing to run";
      k.voiceOrgan("reader", "unbalanced-source", "balanced", "compile-error", "source-diagnostics", detail, ["revise"], {
        kernel: "ts",
        path: owner?.file ?? "",
        line: owner?.line ?? line,
        col,
      });
      throw new Error(`parse error at ${place}: ${detail}`);
    }
  }
}

export function readAll(k: Kernel, src: string): NodeID {
  const s: ParseState = { toks: tokenize(src), i: 0, attribute: makeAttributor(k, src) };
  stopAtStrayParen(k, src, s.toks);
  const forms: NodeID[] = [];
  while (s.i < s.toks.length) {
    forms.push(readOne(k, s));
  }
  if (forms.length === 0) return k.internTrivialNull();
  if (forms.length === 1) {
    k.markUnitRoot(forms[0]!, false);
    return forms[0]!;
  }
  const wrapper = k.intern(
    { pkg: 1, level: Level.BASIC, type: RBasic.BLOCK, inst: RBlock.DO },
    forms,
  );
  k.markUnitRoot(wrapper, true);
  return wrapper;
}

// makeAttributor — byte position → (file, line, col) recorder. Line starts
// are precomputed once per read; the kernel's readingFiles line map (set by
// the CLI when loading multiple files) translates global lines back to the
// original file. Returns null when no line map is active.
function makeAttributor(
  k: Kernel,
  src: string,
): ((node: NodeID, pos: number) => void) | null {
  if (k.readingFiles.length === 0) return null;
  const lineStarts: number[] = [0];
  for (let i = 0; i < src.length; i++) {
    if (src[i] === "\n") lineStarts.push(i + 1);
  }
  return (node, pos) => {
    // Binary search: last line start at or before pos.
    let lo = 0;
    let hi = lineStarts.length - 1;
    while (lo < hi) {
      const mid = (lo + hi + 1) >> 1;
      if (lineStarts[mid]! <= pos) lo = mid;
      else hi = mid - 1;
    }
    const globalLine = lo + 1;
    const col = pos - lineStarts[lo]! + 1;
    const owner = k.resolveReadingLine(globalLine);
    if (owner !== null) {
      k.attributeSource(node, owner.file, owner.line, col);
    }
  };
}

function readOne(k: Kernel, s: ParseState): NodeID {
  const t = consume(s);
  if (t.kind === "int") {
    // Read exactly via BigInt, wrapped to the 63-bit integer word (law 1) as fkwu's
    // literal reader wraps it. The int32 range interns inline, wider through INT64.
    const big = BigInt.asIntN(63, BigInt(t.text));
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
    // Bare identifier: wrap in IDENT recipe; the walker resolves through frame.
    return k.intern(
      { pkg: 1, level: Level.BASIC, type: RBasic.IDENT, inst: 1 },
      [k.internString(t.text)],
    );
  }
  if (t.kind === "lparen") {
    const node = readList(k, s);
    if (s.attribute !== null && node.level !== Level.TRIVIAL) {
      s.attribute(node, t.pos);
    }
    return node;
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
  // Special forms with non-uniform child shapes (let, defn) need to peek
  // at the verb before reading children.
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
          {
            pkg: 1,
            level: Level.BASIC,
            type: RBasic.COND,
            inst: RCond.IF_THEN_ELSE,
          },
          kids,
        );
      }
      throw new Error("if: need 2 or 3 args");
    }
    // Verb forms: consume the verb, read remaining children, dispatch
    // through buildVerb.
    consume(s);
    const kids = readChildrenUntilRparen(k, s);
    return buildVerb(k, verb, kids);
  }
  // (expr expr...) with no leading ident — function call where first item
  // is the callee expression
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

// (let <name> <value>) — interns name as a bare string trivial so the
// walker reads NameID directly from the inst slot (no IDENT recipe).
function readLet(k: Kernel, s: ParseState): NodeID {
  const nameTok = consume(s);
  if (nameTok.kind !== "ident")
    throw new Error("let: name must be identifier");
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

// (defn <name> (<params>...) <body>) — the name and each param are bare
// string trivials, so the walker reads a NameID from the inst slot.
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

// VERBS — the verbs fkwu answers as an op or a rewrite row, each with its RBasic category.
const VERBS: ReadonlyMap<string, NodeID> = new Map(
  ([
    ["do", RBasic.BLOCK, RBlock.DO],
    ["add", RBasic.MATH, RMath.PLUS],
    ["sub", RBasic.MATH, RMath.MINUS],
    ["mul", RBasic.MATH, RMath.MUL],
    ["div", RBasic.MATH, RMath.DIV],
    ["mod", RBasic.MATH, RMath.MOD],
    ["eq", RBasic.COMPARE, RCmp.EQ],
    ["ne", RBasic.COMPARE, RCmp.NE],
    ["lt", RBasic.COMPARE, RCmp.LT],
    ["le", RBasic.COMPARE, RCmp.LE],
    ["gt", RBasic.COMPARE, RCmp.GT],
    ["ge", RBasic.COMPARE, RCmp.GE],
    ["and", RBasic.LOGIC, RLogic.AND],
    ["or", RBasic.LOGIC, RLogic.OR],
    ["not", RBasic.LOGIC, RLogic.NOT],
    ["list", RBasic.LIST, 1],
  ] as const).map(([verb, type, inst]) => [verb, { pkg: 1, level: Level.BASIC, type, inst }]),
);

// buildVerb — a verb's recipe, or a call: a bare-string-trivial callee, then the args.
function buildVerb(k: Kernel, verb: string, args: NodeID[]): NodeID {
  const category = VERBS.get(verb);
  if (category !== undefined) return k.intern(category, args);
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
