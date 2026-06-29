// form-walker-rust — the minimal Rust proof-walker.
//
// Extracted faithfully from form-kernel-rust (the full ~19.5K-line kernel that
// already agrees four-way with fkwu). This carries ONLY the proof-witness core:
// an INDEPENDENT lexer (`tokenize_sexp` + `unescape`), an INDEPENDENT parser
// (`read_sexp` + `build_verb`), and a pure-compute evaluator (`walk`). Dropped:
// the NodeID content-addressing substrate, JIT/dylib/asm lowering, server,
// host-io / file / socket / metal, GGUF/model, formats, and all tests/benches.
//
// WHY a SEPARATE walker earns its keep: the value of a foreign witness is that
// its lexer and evaluator are its OWN, not shared with the runtime. A shared
// parse/semantic bug (the scientific-notation `1e-05` float bug — which Rust's
// OWN lexer was also wrong about until fixed) surfaces here as a divergence
// rather than slipping through every path at once. So the lexer below is the
// CURRENT fixed version, copied faithfully — not rewritten from memory.
//
// Pure-op surface covered (and nothing more):
//   literals: integer, float (incl. scientific notation), string, true/false, ()
//   build-verbs: do seq let if defn params  add sub mul div mod
//                eq ne lt le gt ge  and or not
//   natives:    head tail cons empty list nth len  str_concat str_eq
//
// CLI parity with the full kernel's default source path: `form-walker-rust
// a.fk b.fk ...` concatenates the files with '\n', evaluates, and prints the
// final value's display. `--expr "<src>"` evaluates a single expression.

use std::collections::HashMap;
use std::env;
use std::fs;
use std::rc::Rc;

// ---------------------------------------------------------------------------
// Value — the runtime value the evaluator produces. The pure-compute subset of
// the full kernel's Value: no Record (mutable objects), no Nid (substrate
// passthrough), no Closure-over-NodeID. A closure here carries the AST body.
// ---------------------------------------------------------------------------
#[derive(Clone)]
enum Value {
    Null,
    Int(i64),
    Float(f64),
    Str(Rc<str>),
    Bool(bool),
    List(Rc<Vec<Value>>),
    Closure(Rc<Closure>),
}

struct Closure {
    name: String,
    params: Vec<String>,
    body: Rc<Node>,
    // Captured definition environment, chained for lexical scope.
    env: Env,
}

impl Value {
    // Faithful copy of the full kernel's Value::display.
    fn display(&self) -> String {
        match self {
            Value::Null => "null".to_string(),
            Value::Int(n) => n.to_string(),
            Value::Float(f) => format_float(*f),
            Value::Str(s) => s.to_string(),
            Value::Bool(b) => {
                if *b {
                    "true".to_string()
                } else {
                    "false".to_string()
                }
            }
            Value::List(xs) => {
                let parts: Vec<String> = xs.iter().map(|x| x.display()).collect();
                format!("[{}]", parts.join(", "))
            }
            Value::Closure(c) => format!("<closure #{}>", c.name),
        }
    }

    fn as_int(&self) -> i64 {
        match self {
            Value::Int(n) => *n,
            Value::Float(f) => *f as i64,
            Value::Bool(b) => {
                if *b {
                    1
                } else {
                    0
                }
            }
            _ => panic!("as_int: not a number"),
        }
    }

    fn as_float(&self) -> f64 {
        match self {
            Value::Float(f) => *f,
            Value::Int(n) => *n as f64,
            Value::Bool(b) => {
                if *b {
                    1.0
                } else {
                    0.0
                }
            }
            _ => panic!("as_float: not a number"),
        }
    }

    fn as_bool(&self) -> bool {
        match self {
            Value::Bool(b) => *b,
            Value::Int(n) => *n != 0,
            Value::Float(f) => *f != 0.0,
            Value::Null => false,
            _ => true,
        }
    }
}

// Faithful copy of the full kernel's format_float — Rust's default {} for an
// f64, with NaN/Infinity normalized to the three-way-agreed spellings.
fn format_float(f: f64) -> String {
    if f.is_nan() {
        return "NaN".to_string();
    }
    if f.is_infinite() {
        return if f > 0.0 {
            "Infinity".to_string()
        } else {
            "-Infinity".to_string()
        };
    }
    format!("{}", f)
}

// A comparison/logic answer acknowledges with the 0/1 integer states
// (axiom-1) — faithful to the full kernel's bool_int, so the result flows
// straight into arithmetic.
fn bool_int(b: bool) -> Value {
    Value::Int(b as i64)
}

// ---------------------------------------------------------------------------
// AST — the parser's output. The full kernel interns into content-addressed
// NodeIDs; the witness needs only a direct tree. The node shapes mirror the
// full kernel's typed categories (MATH/COMPARE/LOGIC/COND/BLOCK/FNDEF/FNCALL/
// IDENT/LIST) one-for-one so `build_verb`'s dispatch maps across faithfully.
// ---------------------------------------------------------------------------
enum Node {
    Int(i64),
    Float(f64),
    Str(String),
    Bool(bool),
    Null,
    Ident(String),
    // (op a b) for the binary math/compare verbs.
    Math(OpTag, Rc<Node>, Rc<Node>), // op carried as a small tag
    Compare(OpTag, Rc<Node>, Rc<Node>),
    Logic(OpTag, Vec<Rc<Node>>),
    // (if c t) / (if c t e)
    If(Rc<Node>, Rc<Node>, Option<Rc<Node>>),
    // (do ...) / (seq ...)
    Block(Vec<Rc<Node>>),
    // (let name value)
    Let(String, Rc<Node>),
    // (defn name (params...) body)
    Fndef(String, Vec<String>, Rc<Node>),
    // (name args...) — native or user closure call
    Fncall(String, Vec<Rc<Node>>),
}

// Small fixed-size op tag — avoids a String per node, keeps the dispatch
// exactly the named operations the full kernel's RMATH_/RCMP_/RLOG_ enums name.
type OpTag = u8;
const OP_PLUS: u8 = 1;
const OP_MINUS: u8 = 2;
const OP_MUL: u8 = 3;
const OP_DIV: u8 = 4;
const OP_MOD: u8 = 5;
const CMP_EQ: u8 = 1;
const CMP_NE: u8 = 2;
const CMP_LT: u8 = 3;
const CMP_LE: u8 = 4;
const CMP_GT: u8 = 5;
const CMP_GE: u8 = 6;
const LOG_AND: u8 = 1;
const LOG_OR: u8 = 2;
const LOG_NOT: u8 = 3;

// ---------------------------------------------------------------------------
// Lexer — copied VERBATIM from form-kernel-rust's tokenize_sexp + unescape.
// This is the load-bearing independent witness: the scientific-notation float
// bug lived precisely in this scan. Faithfulness here is the whole point — the
// float scan consumes an exponent with OR without a fractional part (Python's
// repr emits e.g. 1e-05 with no decimal point), and a bare 'e' stays a separate
// symbol token.
// ---------------------------------------------------------------------------
#[derive(Clone)]
struct SexpTok {
    kind: &'static str,
    value: String,
    line: u32,
    col: u32,
}

fn tokenize_sexp(src: &str) -> Vec<SexpTok> {
    let bytes = src.as_bytes();
    let mut toks = Vec::with_capacity(64);
    let mut i = 0;
    let mut line: u32 = 1;
    let mut col: u32 = 1;
    while i < bytes.len() {
        let c = bytes[i];
        let (sline, scol) = (line, col);
        match c {
            b'\n' => {
                i += 1;
                line += 1;
                col = 1;
            }
            b' ' | b'\t' | b'\r' => {
                i += 1;
                col += 1;
            }
            b';' => {
                while i < bytes.len() && bytes[i] != b'\n' {
                    i += 1;
                }
                // newline handled by outer loop
            }
            b'(' => {
                toks.push(SexpTok {
                    kind: "LPAREN",
                    value: "(".into(),
                    line: sline,
                    col: scol,
                });
                i += 1;
                col += 1;
            }
            b')' => {
                toks.push(SexpTok {
                    kind: "RPAREN",
                    value: ")".into(),
                    line: sline,
                    col: scol,
                });
                i += 1;
                col += 1;
            }
            b'"' => {
                i += 1;
                col += 1;
                let start = i;
                while i < bytes.len() && bytes[i] != b'"' {
                    if bytes[i] == b'\\' && i + 1 < bytes.len() {
                        i += 2;
                        col += 2;
                        continue;
                    }
                    if bytes[i] == b'\n' {
                        line += 1;
                        col = 1;
                    } else {
                        col += 1;
                    }
                    i += 1;
                }
                let raw = &src[start..i];
                toks.push(SexpTok {
                    kind: "STRING",
                    value: unescape(raw),
                    line: sline,
                    col: scol,
                });
                if i < bytes.len() {
                    i += 1;
                    col += 1;
                }
            }
            b'0'..=b'9' => {
                let start = i;
                while i < bytes.len() && bytes[i].is_ascii_digit() {
                    i += 1;
                }
                // Float: digits '.' digits, and/or a scientific exponent. The dot
                // must be followed by a digit so `(.foo bar)` and bare integers stay
                // legible. The exponent is consumed with OR without a fractional part —
                // Python's repr emits e.g. 1e-05 with no decimal point. Sibling-parity:
                // Go/TS readers parse the same shape.
                let mut is_float = false;
                if i + 1 < bytes.len() && bytes[i] == b'.' && bytes[i + 1].is_ascii_digit() {
                    is_float = true;
                    i += 1; // consume '.'
                    while i < bytes.len() && bytes[i].is_ascii_digit() {
                        i += 1;
                    }
                }
                // Optional exponent: [eE][+-]?[0-9]+ (only when a digit follows, so a
                // bare 'e' stays a separate symbol token).
                if i < bytes.len() && (bytes[i] == b'e' || bytes[i] == b'E') {
                    let mut k = i + 1;
                    if k < bytes.len() && (bytes[k] == b'+' || bytes[k] == b'-') {
                        k += 1;
                    }
                    if k < bytes.len() && bytes[k].is_ascii_digit() {
                        is_float = true;
                        i = k + 1;
                        while i < bytes.len() && bytes[i].is_ascii_digit() {
                            i += 1;
                        }
                    }
                }
                if is_float {
                    toks.push(SexpTok {
                        kind: "FLOAT",
                        value: src[start..i].to_string(),
                        line: sline,
                        col: scol,
                    });
                } else {
                    toks.push(SexpTok {
                        kind: "INT",
                        value: src[start..i].to_string(),
                        line: sline,
                        col: scol,
                    });
                }
                col += (i - start) as u32;
            }
            b'-' if i + 1 < bytes.len() && bytes[i + 1].is_ascii_digit() => {
                let start = i;
                i += 1;
                while i < bytes.len() && bytes[i].is_ascii_digit() {
                    i += 1;
                }
                let mut is_float = false;
                if i + 1 < bytes.len() && bytes[i] == b'.' && bytes[i + 1].is_ascii_digit() {
                    is_float = true;
                    i += 1;
                    while i < bytes.len() && bytes[i].is_ascii_digit() {
                        i += 1;
                    }
                }
                // exponent with OR without a fractional part (e.g. -2e-15)
                if i < bytes.len() && (bytes[i] == b'e' || bytes[i] == b'E') {
                    let mut k = i + 1;
                    if k < bytes.len() && (bytes[k] == b'+' || bytes[k] == b'-') {
                        k += 1;
                    }
                    if k < bytes.len() && bytes[k].is_ascii_digit() {
                        is_float = true;
                        i = k + 1;
                        while i < bytes.len() && bytes[i].is_ascii_digit() {
                            i += 1;
                        }
                    }
                }
                if is_float {
                    toks.push(SexpTok {
                        kind: "FLOAT",
                        value: src[start..i].to_string(),
                        line: sline,
                        col: scol,
                    });
                } else {
                    toks.push(SexpTok {
                        kind: "INT",
                        value: src[start..i].to_string(),
                        line: sline,
                        col: scol,
                    });
                }
                col += (i - start) as u32;
            }
            _ => {
                let start = i;
                while i < bytes.len() {
                    let cc = bytes[i];
                    if cc == b' '
                        || cc == b'\t'
                        || cc == b'\n'
                        || cc == b'\r'
                        || cc == b'('
                        || cc == b')'
                        || cc == b'"'
                        || cc == b';'
                    {
                        break;
                    }
                    i += 1;
                }
                toks.push(SexpTok {
                    kind: "IDENT",
                    value: src[start..i].to_string(),
                    line: sline,
                    col: scol,
                });
                col += (i - start) as u32;
            }
        }
    }
    toks
}

// Copied verbatim from the full kernel: verbatim runs copy as &str slices so
// multibyte chars survive intact.
fn unescape(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    let bytes = s.as_bytes();
    let mut i = 0;
    let mut run = 0;
    while i < bytes.len() {
        if bytes[i] == b'\\' && i + 1 < bytes.len() {
            out.push_str(&s[run..i]);
            match bytes[i + 1] {
                b'n' => out.push('\n'),
                b't' => out.push('\t'),
                b'r' => out.push('\r'),
                b'\\' => out.push('\\'),
                b'"' => out.push('"'),
                c => out.push(c as char),
            }
            i += 2;
            run = i;
            continue;
        }
        i += 1;
    }
    out.push_str(&s[run..]);
    out
}

// ---------------------------------------------------------------------------
// Parser — read_sexp + build_verb, faithful to the full kernel. The full kernel
// interns into NodeIDs and maps verbs to typed categories; here we build the
// AST directly. The verb→shape mapping (and the (let ...) / (defn ...)
// repackaging, the if/if-else arity split, true/false literals, () → Null)
// is the same dispatch.
// ---------------------------------------------------------------------------
fn read_sexp(toks: &[SexpTok], i: usize) -> (Rc<Node>, usize) {
    if i >= toks.len() {
        panic!("parse error: unexpected end of input (expected an expression)");
    }
    let t = &toks[i];
    match t.kind {
        "INT" => {
            let n: i64 = t.value.parse().unwrap();
            (Rc::new(Node::Int(n)), i + 1)
        }
        "FLOAT" => {
            let f: f64 = t.value.parse().unwrap_or_else(|e| {
                panic!(
                    "parse error: bad float literal {:?} at line {}, col {}: {}",
                    t.value, t.line, t.col, e
                )
            });
            (Rc::new(Node::Float(f)), i + 1)
        }
        "STRING" => (Rc::new(Node::Str(t.value.clone())), i + 1),
        "IDENT" => {
            if t.value == "true" {
                return (Rc::new(Node::Bool(true)), i + 1);
            }
            if t.value == "false" {
                return (Rc::new(Node::Bool(false)), i + 1);
            }
            (Rc::new(Node::Ident(t.value.clone())), i + 1)
        }
        "RPAREN" => {
            panic!(
                "parse error at line {} col {}: unmatched `)` (no `(` to close)",
                t.line, t.col
            );
        }
        "LPAREN" => {
            let (open_line, open_col) = (t.line, t.col);
            let mut j = i + 1;
            if j >= toks.len() {
                panic!(
                    "parse error: unclosed `(` opened at line {} col {} (reached end of input)",
                    open_line, open_col
                );
            }
            // `()` — the empty form — is the Null trivial.
            if toks[j].kind == "RPAREN" {
                return (Rc::new(Node::Null), j + 1);
            }
            if toks[j].kind != "IDENT" {
                panic!("parse error at line {} col {}: expected verb after `(` opened at line {} col {}, got {} {:?}",
                    toks[j].line, toks[j].col, open_line, open_col, toks[j].kind, toks[j].value);
            }
            let verb = toks[j].value.clone();
            j += 1;
            let mut args = Vec::new();
            loop {
                if j >= toks.len() {
                    panic!("parse error: unclosed `(` opened at line {} col {} in `({} ...)` (reached end of input)",
                        open_line, open_col, verb);
                }
                if toks[j].kind == "RPAREN" {
                    j += 1;
                    break;
                }
                let (arg, nj) = read_sexp(toks, j);
                args.push(arg);
                j = nj;
            }
            (build_verb(&verb, args), j)
        }
        _ => panic!(
            "parse error at line {} col {}: unexpected token {} {:?}",
            t.line, t.col, t.kind, t.value
        ),
    }
}

// build_verb — faithful to the full kernel's surface-verb lowering. The verbs
// here lower into TYPED nodes (BLOCK/COND/MATH/COMPARE/LOGIC/FNDEF) rather than
// the FNCALL fallback, exactly as the full kernel's BUILD_VERBS set does.
fn build_verb(verb: &str, args: Vec<Rc<Node>>) -> Rc<Node> {
    match verb {
        "do" | "seq" => Rc::new(Node::Block(args)),
        "let" => {
            // (let <ident> <value>) — args[0] is an Ident node; take its name.
            let name = ident_name(&args[0]);
            Rc::new(Node::Let(name, args[1].clone()))
        }
        "if" => {
            if args.len() == 2 {
                Rc::new(Node::If(args[0].clone(), args[1].clone(), None))
            } else {
                Rc::new(Node::If(
                    args[0].clone(),
                    args[1].clone(),
                    Some(args[2].clone()),
                ))
            }
        }
        "add" => Rc::new(Node::Math(OP_PLUS, args[0].clone(), args[1].clone())),
        "sub" => Rc::new(Node::Math(OP_MINUS, args[0].clone(), args[1].clone())),
        "mul" => Rc::new(Node::Math(OP_MUL, args[0].clone(), args[1].clone())),
        "div" => Rc::new(Node::Math(OP_DIV, args[0].clone(), args[1].clone())),
        "mod" => Rc::new(Node::Math(OP_MOD, args[0].clone(), args[1].clone())),
        "eq" => Rc::new(Node::Compare(CMP_EQ, args[0].clone(), args[1].clone())),
        "ne" => Rc::new(Node::Compare(CMP_NE, args[0].clone(), args[1].clone())),
        "lt" => Rc::new(Node::Compare(CMP_LT, args[0].clone(), args[1].clone())),
        "le" => Rc::new(Node::Compare(CMP_LE, args[0].clone(), args[1].clone())),
        "gt" => Rc::new(Node::Compare(CMP_GT, args[0].clone(), args[1].clone())),
        "ge" => Rc::new(Node::Compare(CMP_GE, args[0].clone(), args[1].clone())),
        "and" => Rc::new(Node::Logic(LOG_AND, args)),
        "or" => Rc::new(Node::Logic(LOG_OR, args)),
        "not" => Rc::new(Node::Logic(LOG_NOT, args)),
        "defn" => {
            // (defn <name> (<params>...) <body>). The params form `(a b c)` is
            // itself a parenthesized s-expr, so the full kernel reads it through
            // the same build_verb path: its head `a` is the "verb", b/c its
            // "args". The full kernel's defn then takes `children(params)` as the
            // param idents — i.e. head-name + arg-names. We reconstruct exactly
            // that param list here. `()` (Null) → no params; `(params a b)` →
            // Block. Faithful to the full kernel's repackaging.
            let name = ident_name(&args[0]);
            let params: Vec<String> = match &*args[1] {
                Node::Null => Vec::new(),
                Node::Block(ps) => ps.iter().map(|p| ident_name(p)).collect(),
                Node::Fncall(head, rest) => {
                    let mut ps = vec![head.clone()];
                    ps.extend(rest.iter().map(|p| ident_name(p)));
                    ps
                }
                Node::Ident(s) => vec![s.clone()],
                _ => panic!("defn: malformed params for {}", name),
            };
            Rc::new(Node::Fndef(name, params, args[2].clone()))
        }
        "params" => Rc::new(Node::Block(args)),
        _ => Rc::new(Node::Fncall(verb.to_string(), args)),
    }
}

fn ident_name(n: &Rc<Node>) -> String {
    match &**n {
        Node::Ident(s) => s.clone(),
        Node::Str(s) => s.clone(),
        _ => panic!("expected identifier"),
    }
}

// ---------------------------------------------------------------------------
// Environment — lexical frames chained by parent, mirroring the full kernel's
// Arena frames (parent-linked, name→value). Rc<RefCell> gives the shared
// mutable frame identity a closure captures.
// ---------------------------------------------------------------------------
use std::cell::RefCell;

type Env = Rc<RefCell<Frame>>;

struct Frame {
    vars: HashMap<String, Value>,
    parent: Option<Env>,
}

fn new_frame(parent: Option<Env>) -> Env {
    Rc::new(RefCell::new(Frame {
        vars: HashMap::new(),
        parent,
    }))
}

fn env_lookup(env: &Env, name: &str) -> Option<Value> {
    let f = env.borrow();
    if let Some(v) = f.vars.get(name) {
        return Some(v.clone());
    }
    match &f.parent {
        Some(p) => env_lookup(p, name),
        None => None,
    }
}

fn env_bind(env: &Env, name: String, v: Value) {
    env.borrow_mut().vars.insert(name, v);
}

// ---------------------------------------------------------------------------
// Evaluator — faithful to the full kernel's `walk`. Same arm semantics: width
// promotion (float on either operand → float result), comparisons/logic
// returning 0/1 ints, if/let/do, defn-as-closure, fncall (native first, then
// user closure). The full kernel does TCO via a loop; recursion here is fine
// for the witness's pure-op surface and keeps the dispatch legible.
// ---------------------------------------------------------------------------
fn walk(n: &Rc<Node>, env: &Env) -> Value {
    match &**n {
        Node::Int(v) => Value::Int(*v),
        Node::Float(v) => Value::Float(*v),
        Node::Str(s) => Value::Str(Rc::from(s.as_str())),
        Node::Bool(b) => Value::Bool(*b),
        Node::Null => Value::Null,
        Node::Ident(name) => {
            env_lookup(env, name).unwrap_or_else(|| panic!("unbound: {}", name))
        }
        Node::Math(op, a, b) => {
            let lv = walk(a, env);
            let rv = walk(b, env);
            // Width promotion: if either operand is Float, the result is Float
            // (matches Python int+float→float). Pure int/int stays i64.
            if matches!(lv, Value::Float(_)) || matches!(rv, Value::Float(_)) {
                let l = lv.as_float();
                let r = rv.as_float();
                Value::Float(match *op {
                    OP_PLUS => l + r,
                    OP_MINUS => l - r,
                    OP_MUL => l * r,
                    OP_DIV => l / r,
                    OP_MOD => l - (l / r).floor() * r,
                    _ => unreachable!(),
                })
            } else {
                let l = lv.as_int();
                let r = rv.as_int();
                Value::Int(match *op {
                    OP_PLUS => l + r,
                    OP_MINUS => l - r,
                    OP_MUL => l * r,
                    OP_DIV => l / r,
                    OP_MOD => l % r,
                    _ => unreachable!(),
                })
            }
        }
        Node::Compare(op, a, b) => {
            let lv = walk(a, env);
            let rv = walk(b, env);
            if matches!(lv, Value::Float(_)) || matches!(rv, Value::Float(_)) {
                let l = lv.as_float();
                let r = rv.as_float();
                bool_int(match *op {
                    CMP_EQ => l == r,
                    CMP_NE => l != r,
                    CMP_LT => l < r,
                    CMP_LE => l <= r,
                    CMP_GT => l > r,
                    CMP_GE => l >= r,
                    _ => unreachable!(),
                })
            } else {
                let l = lv.as_int();
                let r = rv.as_int();
                bool_int(match *op {
                    CMP_EQ => l == r,
                    CMP_NE => l != r,
                    CMP_LT => l < r,
                    CMP_LE => l <= r,
                    CMP_GT => l > r,
                    CMP_GE => l >= r,
                    _ => unreachable!(),
                })
            }
        }
        Node::Logic(op, args) => match *op {
            LOG_AND => {
                if !walk(&args[0], env).as_bool() {
                    bool_int(false)
                } else {
                    bool_int(walk(&args[1], env).as_bool())
                }
            }
            LOG_OR => {
                if walk(&args[0], env).as_bool() {
                    bool_int(true)
                } else {
                    bool_int(walk(&args[1], env).as_bool())
                }
            }
            LOG_NOT => bool_int(!walk(&args[0], env).as_bool()),
            _ => unreachable!(),
        },
        Node::If(c, t, e) => {
            if walk(c, env).as_bool() {
                walk(t, env)
            } else if let Some(else_n) = e {
                walk(else_n, env)
            } else {
                Value::Null
            }
        }
        Node::Block(kids) => {
            if kids.is_empty() {
                return Value::Null;
            }
            let last = kids.len() - 1;
            for c in &kids[..last] {
                walk(c, env);
            }
            walk(&kids[last], env)
        }
        Node::Let(name, value) => {
            let v = walk(value, env);
            env_bind(env, name.clone(), v.clone());
            v
        }
        Node::Fndef(name, params, body) => {
            let cl = Rc::new(Closure {
                name: name.clone(),
                params: params.clone(),
                body: body.clone(),
                env: env.clone(),
            });
            env_bind(env, name.clone(), Value::Closure(cl.clone()));
            Value::Closure(cl)
        }
        Node::Fncall(name, args) => {
            // Native takes priority unless the user shadowed it with a binding —
            // faithful to the full kernel's dispatch order.
            if env_lookup(env, name).is_none() {
                if let Some(v) = call_native(name, args, env) {
                    return v;
                }
            }
            let callee = env_lookup(env, name)
                .unwrap_or_else(|| panic!("unbound function: {}", name));
            let cl = match callee {
                Value::Closure(c) => c,
                _ => panic!("not callable: {}", name),
            };
            if args.len() != cl.params.len() {
                panic!("{} wants {} args, got {}", name, cl.params.len(), args.len());
            }
            // Evaluate args in CALLER's env, bind in a fresh call frame chained
            // to the closure's definition env.
            let call_frame = new_frame(Some(cl.env.clone()));
            for (i, p) in cl.params.iter().enumerate() {
                let arg = walk(&args[i], env);
                env_bind(&call_frame, p.clone(), arg);
            }
            walk(&cl.body, &call_frame)
        }
    }
}

// call_native — the pure-op native surface: list ops + string ops. Returns
// None when the name is not a native (so the closure path takes over). Each
// handler is faithful to the full kernel's register_native body.
fn call_native(name: &str, arg_nodes: &[Rc<Node>], env: &Env) -> Option<Value> {
    // Arity is checked inside each handler the same way the full kernel's
    // natives index args[..]; evaluate eagerly in caller env first.
    let args: Vec<Value> = arg_nodes.iter().map(|a| walk(a, env)).collect();
    match name {
        "list" => Some(Value::List(Rc::new(args))),
        "empty" => Some(Value::List(Rc::new(vec![]))),
        "cons" => {
            let mut out = vec![args[0].clone()];
            if let Value::List(rest) = &args[1] {
                out.extend(rest.iter().cloned());
            }
            Some(Value::List(Rc::new(out)))
        }
        "head" => Some(if let Value::List(xs) = &args[0] {
            xs.first().cloned().unwrap_or(Value::Null)
        } else {
            Value::Null
        }),
        "tail" => Some(if let Value::List(xs) = &args[0] {
            Value::List(Rc::new(if xs.is_empty() {
                vec![]
            } else {
                xs[1..].to_vec()
            }))
        } else {
            Value::Null
        }),
        "str_concat" => {
            let s = format!("{}{}", str_of(&args[0]), str_of(&args[1]));
            Some(Value::Str(Rc::from(s.as_str())))
        }
        "str_eq" => Some(bool_int(str_of(&args[0]) == str_of(&args[1]))),
        // nth / len — pure list accessors in the cons/head/tail family. Faithful
        // to the full kernel's natives. They sit just past the named surface but
        // are the same pure list shape and are what a real manifest band
        // (value-execution, verdict 7) folds over; kept minimal: no dict tag.
        "nth" => Some(if let Value::List(xs) = &args[0] {
            let i = args[1].as_int();
            if i < 0 || (i as usize) >= xs.len() {
                Value::Null
            } else {
                xs[i as usize].clone()
            }
        } else {
            Value::Null
        }),
        "len" => Some(match &args[0] {
            Value::List(xs) => {
                // Dict-aware: a "__dict__"-tagged list reports pair count,
                // matching the full kernel's len. A plain list reports its
                // length.
                if let Some(Value::Str(s)) = xs.first() {
                    if &**s == "__dict__" {
                        return Some(Value::Int(((xs.len() - 1) / 2) as i64));
                    }
                }
                Value::Int(xs.len() as i64)
            }
            Value::Str(s) => Value::Int(s.len() as i64),
            _ => Value::Int(0),
        }),
        _ => None,
    }
}

fn str_of(v: &Value) -> String {
    match v {
        Value::Str(s) => s.to_string(),
        other => other.display(),
    }
}

// ---------------------------------------------------------------------------
// run_source — faithful to the full kernel: if there is more than one top-level
// form, wrap the whole source in `(do ...)` so the last form's value is the
// program's value.
// ---------------------------------------------------------------------------
fn count_top_level(toks: &[SexpTok]) -> usize {
    let mut depth = 0;
    let mut count = 0;
    for t in toks {
        match t.kind {
            "LPAREN" => {
                if depth == 0 {
                    count += 1;
                }
                depth += 1;
            }
            "RPAREN" => {
                depth -= 1;
            }
            _ => {
                if depth == 0 {
                    count += 1;
                }
            }
        }
    }
    count
}

fn run_source(src: &str) -> Value {
    let toks = tokenize_sexp(src);
    let wrapped: String;
    let toks = if count_top_level(&toks) == 1 {
        toks
    } else {
        wrapped = format!("(do {})", src);
        tokenize_sexp(&wrapped)
    };
    let (root, _) = read_sexp(&toks, 0);
    let env = new_frame(None);
    walk(&root, &env)
}

fn main() {
    let args: Vec<String> = env::args().skip(1).collect();
    if args.is_empty() {
        eprintln!("form-walker-rust: usage: form-walker-rust <file.fk> [more.fk ...]");
        eprintln!("                  form-walker-rust --expr \"<source>\"");
        std::process::exit(2);
    }
    let src = if args[0] == "--expr" {
        if args.len() < 2 {
            eprintln!("--expr requires an argument");
            std::process::exit(2);
        }
        args[1].clone()
    } else {
        let mut parts = Vec::with_capacity(args.len());
        for path in &args {
            match fs::read_to_string(path) {
                Ok(s) => parts.push(s),
                Err(e) => {
                    eprintln!("read {}: {}", path, e);
                    std::process::exit(1);
                }
            }
        }
        parts.join("\n")
    };
    let result = run_source(&src);
    println!("{}", result.display());
}
