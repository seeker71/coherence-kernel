// walkers/go/main.go — the minimal Go proof-walker.
//
// One job: be an INDEPENDENT witness. Its own lexer + its own tree-walking
// evaluator read a `.fk` source (preludes concatenated) and compute a value,
// so a shared parse/semantic bug fkwu's own paths would miss has a second pair
// of eyes. This is the entire reason a foreign walker earns its keep — the
// scientific-notation `1e-05` float bug and the int64-width literal bug were
// both caught exactly this way: one kernel's lexer disagreeing with three.
//
// It is the smallest INDEPENDENT parse+eval core extracted from the full Go
// kernel (form/form-kernel-go), preserving the EXACT lexer + evaluator
// semantics. Functions are copied verbatim from the origin, NOT rewritten — a
// byte-for-byte-equivalent lexer is what makes it an honest witness. Dropped on
// purpose (these are "more", never the witness): the JIT (crystallize-on-heat),
// the HTTP server, host-io / file / socket / metal, the GGUF/model code, the
// .fkb binary codec, source-attribution provenance, trace counters, observe
// hooks, and every *_test.go. fkwu owns the native path; this only confirms a
// recipe computes the same value.
//
// Pure-op surface covered: integer + int64 + float + string + bool literals;
// add sub mul div mod; eq ne lt le gt ge; if let do seq; defn + user calls
// (tail-call optimized, like the origin); and/or/not; head tail cons list nth
// empty; str_concat str_eq str_len str_find substring char_at int_to_str;
// value_eq; match (switch). The BMF s-expression parse (the lexer), the
// content-addressed intern, and the blueprint/op dispatch.
//
// Usage: walker file.fk [more.fk ...]   — prints the evaluated root value.
package main

import (
	"fmt"
	"hash/fnv"
	"math"
	"os"
	"strconv"
	"strings"
	"unicode/utf8"
)

// ---------------------------------------------------------------------------
// Substrate value/recipe type cluster (copied from form-kernel-go/core).
// Pure data + pure methods, no *Kernel coupling.
// ---------------------------------------------------------------------------

// NodeID — a substrate coordinate (package, level, type, instance).
type NodeID struct{ Pkg, Level, Type, Inst uint32 }

// NameID — interned-string handle.
type NameID uint32

// ValueKind — the runtime value tag.
type ValueKind int

const (
	VNull ValueKind = iota
	VInt
	VStr
	VBool
	VList
	VClosure
	VNodeID
	VFloat
)

// Closure — a Form function value: params, body recipe, captured frame.
type Closure struct {
	Name   NameID
	Params []NameID
	Body   NodeID
	Env    *Frame
}

// binding — one (name, value) of a Frame.
type binding struct {
	Name NameID
	Val  Value
}

// Frame — a lexical environment: bindings + parent link.
type Frame struct {
	Parent   *Frame
	Bindings []binding
}

func NewFrame(parent *Frame) *Frame { return &Frame{Parent: parent} }

func NewCallFrame(parent *Frame, arity int) *Frame {
	return &Frame{Parent: parent, Bindings: make([]binding, 0, arity)}
}

func (f *Frame) Bind(name NameID, v Value) {
	for i := range f.Bindings {
		if f.Bindings[i].Name == name {
			f.Bindings[i].Val = v
			return
		}
	}
	f.Bindings = append(f.Bindings, binding{name, v})
}

func (f *Frame) Lookup(name NameID) (Value, bool) {
	for cur := f; cur != nil; cur = cur.Parent {
		for i := range cur.Bindings {
			if cur.Bindings[i].Name == name {
				return cur.Bindings[i].Val, true
			}
		}
	}
	return Value{}, false
}

// Value — runtime tagged union.
type Value struct {
	Kind  ValueKind
	Int   int64
	Float float64
	Str   string
	Bool  bool
	List  []Value
	Cl    *Closure
	Nid   NodeID
}

func (v Value) String() string {
	switch v.Kind {
	case VNull:
		return "null"
	case VInt:
		return strconv.FormatInt(v.Int, 10)
	case VFloat:
		return FormatFloatJS(v.Float)
	case VStr:
		return v.Str
	case VBool:
		if v.Bool {
			return "true"
		}
		return "false"
	case VList:
		parts := make([]string, len(v.List))
		for i, x := range v.List {
			parts[i] = x.String()
		}
		return "[" + strings.Join(parts, ", ") + "]"
	case VClosure:
		return "<closure #" + strconv.FormatUint(uint64(v.Cl.Name), 10) + ">"
	case VNodeID:
		return fmt.Sprintf("@%d.%d.%d.%d", v.Nid.Pkg, v.Nid.Level, v.Nid.Type, v.Nid.Inst)
	}
	return "?"
}

func (v Value) AsFloat() float64 {
	switch v.Kind {
	case VFloat:
		return v.Float
	case VInt:
		return float64(v.Int)
	case VBool:
		if v.Bool {
			return 1.0
		}
		return 0.0
	}
	panic(fmt.Sprintf("AsFloat: %v", v))
}

func (v Value) AsInt() int64 {
	switch v.Kind {
	case VInt:
		return v.Int
	case VFloat:
		return int64(v.Float)
	case VBool:
		if v.Bool {
			return 1
		}
		return 0
	}
	panic(fmt.Sprintf("as_int: %v", v))
}

// FormatFloatJS — JS String(number) semantics: shortest round-trippable form.
func FormatFloatJS(f float64) string {
	if math.IsNaN(f) {
		return "NaN"
	}
	if math.IsInf(f, 1) {
		return "Infinity"
	}
	if math.IsInf(f, -1) {
		return "-Infinity"
	}
	return strconv.FormatFloat(f, 'g', -1, 64)
}

// ---------------------------------------------------------------------------
// Substrate vocabulary — category/trivial type tags (copied from main.go).
// ---------------------------------------------------------------------------

const (
	LevelTrivial uint32 = 1
	LevelBasic   uint32 = 2

	RBasicBlock   uint32 = 9
	RBasicCond    uint32 = 11
	RBasicMath    uint32 = 12
	RBasicCompare uint32 = 13
	RBasicLogic   uint32 = 14
	RBasicMatch   uint32 = 19
	RBasicChoice  uint32 = 20
	RBasicFnDef   uint32 = 31
	RBasicFnCall  uint32 = 32
	RBasicIdent   uint32 = 33
	RBasicList    uint32 = 34

	TrivInt     uint32 = 1
	TrivString  uint32 = 2
	TrivBool    uint32 = 3
	TrivNull    uint32 = 4
	TrivInt64   uint32 = 5
	TrivFloat32 uint32 = 6
	TrivFloat64 uint32 = 7
)

const (
	RMathPlus     uint32 = 1
	RMathMinus    uint32 = 2
	RMathMultiply uint32 = 3
	RMathDivide   uint32 = 4
	RMathModulo   uint32 = 5

	RCompareEq uint32 = 1
	RCompareNe uint32 = 2
	RCompareLt uint32 = 3
	RCompareLe uint32 = 4
	RCompareGt uint32 = 5
	RCompareGe uint32 = 6

	RLogicAnd uint32 = 1
	RLogicOr  uint32 = 2
	RLogicNot uint32 = 3

	RCondIfThen     uint32 = 1
	RCondIfThenElse uint32 = 2

	RBlockDo       uint32 = 1
	RBlockSequence uint32 = 2
	RBlockLet      uint32 = 3

	RMatchSwitch uint32 = 1

	RChoiceChoose uint32 = 1
	RChoiceFail   uint32 = 2
	RChoiceStop   uint32 = 3
)

// Recipe — composite storage. Trivials are NOT stored; their NodeID carries
// the value.
type Recipe struct {
	Category NodeID
	Children []NodeID
}

// ---------------------------------------------------------------------------
// Kernel — minimal: the intern tables + natives + switch-table cache the
// kept functions touch. JIT/trace/observe/source-attr fields all dropped.
// ---------------------------------------------------------------------------

type NativeFn func(k *Kernel, args []Value) Value

type NativeEntry struct {
	Name NameID
	Fn   NativeFn
}

type switchTable struct {
	cases       map[NodeID]NodeID
	dynamicArms []switchArm
	defaultBody NodeID
	hasDefault  bool
}

type switchArm struct {
	pattern NodeID
	body    NodeID
}

type choiceFailSignal struct{}
type choiceStopSignal struct{}

type Kernel struct {
	byHash map[uint64]NodeID
	byID   map[NodeID]Recipe
	strs   []string
	strIdx map[string]NameID
	next   uint32

	f64s   []float64
	f64Idx map[uint64]uint32
	i64s   []int64
	i64Idx map[int64]uint32

	natives      map[NameID]NativeEntry
	switchTables map[NodeID]*switchTable
}

func NewKernel() *Kernel {
	k := &Kernel{
		byHash:       map[uint64]NodeID{},
		byID:         map[NodeID]Recipe{},
		strIdx:       map[string]NameID{},
		next:         1,
		f64Idx:       map[uint64]uint32{},
		i64Idx:       map[int64]uint32{},
		natives:      map[NameID]NativeEntry{},
		switchTables: map[NodeID]*switchTable{},
	}
	k.registerNatives()
	return k
}

// ---------------------------------------------------------------------------
// Intern + decode (copied verbatim from main.go).
// ---------------------------------------------------------------------------

func hashRecipe(r Recipe) uint64 {
	h := fnv.New64a()
	fmt.Fprintf(h, "C|%d.%d.%d.%d", r.Category.Pkg, r.Category.Level, r.Category.Type, r.Category.Inst)
	for _, c := range r.Children {
		fmt.Fprintf(h, "|%d.%d.%d.%d", c.Pkg, c.Level, c.Type, c.Inst)
	}
	return h.Sum64()
}

func (k *Kernel) intern(category NodeID, children []NodeID) NodeID {
	r := Recipe{Category: category, Children: children}
	h := hashRecipe(r)
	if nid, ok := k.byHash[h]; ok {
		return nid
	}
	nid := NodeID{Pkg: 0, Level: category.Level, Type: category.Type, Inst: k.next}
	k.next++
	k.byHash[h] = nid
	k.byID[nid] = r
	return nid
}

func (k *Kernel) internTrivialInt(n int64) NodeID {
	if n >= math.MinInt32 && n <= math.MaxInt32 {
		return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivInt, Inst: uint32(int32(n))}
	}
	if idx, ok := k.i64Idx[n]; ok {
		return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivInt64, Inst: idx}
	}
	idx := uint32(len(k.i64s))
	k.i64s = append(k.i64s, n)
	k.i64Idx[n] = idx
	return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivInt64, Inst: idx}
}

func (k *Kernel) decodeInt64(inst uint32) int64 {
	if int(inst) >= len(k.i64s) {
		panic(fmt.Sprintf("decodeInt64: bad index %d", inst))
	}
	return k.i64s[inst]
}

func (k *Kernel) internTrivialFloat64(f float64) NodeID {
	var canonical float64
	switch {
	case math.IsNaN(f):
		canonical = math.Float64frombits(0x7ff8000000000000)
	case f == 0.0:
		canonical = 0.0
	default:
		canonical = f
	}
	bits := math.Float64bits(canonical)
	if idx, ok := k.f64Idx[bits]; ok {
		return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivFloat64, Inst: idx}
	}
	idx := uint32(len(k.f64s))
	k.f64s = append(k.f64s, canonical)
	k.f64Idx[bits] = idx
	return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivFloat64, Inst: idx}
}

func (k *Kernel) decodeFloat32(inst uint32) float32 {
	return math.Float32frombits(inst)
}

func (k *Kernel) decodeFloat64(inst uint32) float64 {
	if int(inst) >= len(k.f64s) {
		panic(fmt.Sprintf("decodeFloat64: bad index %d", inst))
	}
	return k.f64s[inst]
}

func (k *Kernel) internString(s string) NodeID {
	if idx, ok := k.strIdx[s]; ok {
		return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivString, Inst: uint32(idx)}
	}
	idx := NameID(len(k.strs))
	k.strs = append(k.strs, s)
	k.strIdx[s] = idx
	return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivString, Inst: uint32(idx)}
}

func (k *Kernel) internName(s string) NameID {
	if idx, ok := k.strIdx[s]; ok {
		return idx
	}
	idx := NameID(len(k.strs))
	k.strs = append(k.strs, s)
	k.strIdx[s] = idx
	return idx
}

// ---------------------------------------------------------------------------
// Recipe access + trivial decode + identifier resolution (copied verbatim).
// ---------------------------------------------------------------------------

func (k *Kernel) category(n NodeID) NodeID {
	if n.Level == LevelTrivial {
		return n
	}
	if r, ok := k.byID[n]; ok {
		return r.Category
	}
	return n
}

func (k *Kernel) children(n NodeID) []NodeID {
	if r, ok := k.byID[n]; ok {
		return r.Children
	}
	return nil
}

func (k *Kernel) recipeAt(n NodeID) Recipe { return k.byID[n] }

func (k *Kernel) trivialValue(n NodeID) Value {
	if n.Level != LevelTrivial {
		panic(fmt.Sprintf("trivialValue: %v is composite", n))
	}
	switch n.Type {
	case TrivInt:
		return Value{Kind: VInt, Int: int64(int32(n.Inst))}
	case TrivInt64:
		return Value{Kind: VInt, Int: k.decodeInt64(n.Inst)}
	case TrivString:
		return Value{Kind: VStr, Str: k.strs[n.Inst]}
	case TrivBool:
		return Value{Kind: VBool, Bool: n.Inst != 0}
	case TrivNull:
		return Value{Kind: VNull}
	case TrivFloat32:
		return Value{Kind: VFloat, Float: float64(k.decodeFloat32(n.Inst))}
	case TrivFloat64:
		return Value{Kind: VFloat, Float: k.decodeFloat64(n.Inst)}
	}
	panic(fmt.Sprintf("trivialValue: unknown trivial type %d", n.Type))
}

func (k *Kernel) identID(n NodeID) NameID {
	if n.Level == LevelTrivial && n.Type == TrivString {
		return NameID(n.Inst)
	}
	kids := k.children(n)
	if len(kids) == 1 && kids[0].Level == LevelTrivial && kids[0].Type == TrivString {
		return NameID(kids[0].Inst)
	}
	panic(fmt.Sprintf("identID: %v is not an identifier shape", n))
}

func (k *Kernel) nameStr(id NameID) string { return k.strs[id] }

// ---------------------------------------------------------------------------
// Evaluator — walk / walkInner (copied from main.go, JIT/observe/trace arms
// stripped; the pure-recipe dispatch + tail-call optimization preserved).
// ---------------------------------------------------------------------------

func (k *Kernel) walk(n NodeID, env *Frame) Value {
	return k.walkInner(n, env)
}

func (k *Kernel) walkInner(n NodeID, env *Frame) Value {
	for {
		if n.Level == LevelTrivial {
			return k.trivialValue(n)
		}
		r := k.recipeAt(n)
		cat, kids := r.Category, r.Children

		switch cat.Type {
		case RBasicMath:
			lv := k.walk(kids[0], env)
			rv := k.walk(kids[1], env)
			if lv.Kind == VFloat || rv.Kind == VFloat {
				l := lv.AsFloat()
				r := rv.AsFloat()
				switch cat.Inst {
				case RMathPlus:
					return Value{Kind: VFloat, Float: l + r}
				case RMathMinus:
					return Value{Kind: VFloat, Float: l - r}
				case RMathMultiply:
					return Value{Kind: VFloat, Float: l * r}
				case RMathDivide:
					return Value{Kind: VFloat, Float: l / r}
				case RMathModulo:
					return Value{Kind: VFloat, Float: l - math.Floor(l/r)*r}
				}
			}
			a := lv.AsInt()
			b := rv.AsInt()
			switch cat.Inst {
			case RMathPlus:
				return Value{Kind: VInt, Int: a + b}
			case RMathMinus:
				return Value{Kind: VInt, Int: a - b}
			case RMathMultiply:
				return Value{Kind: VInt, Int: a * b}
			case RMathDivide:
				return Value{Kind: VInt, Int: a / b}
			case RMathModulo:
				return Value{Kind: VInt, Int: a % b}
			}

		case RBasicCompare:
			lv := k.walk(kids[0], env)
			rv := k.walk(kids[1], env)
			if lv.Kind == VFloat || rv.Kind == VFloat {
				l := lv.AsFloat()
				r := rv.AsFloat()
				switch cat.Inst {
				case RCompareEq:
					return boolInt(l == r)
				case RCompareNe:
					return boolInt(l != r)
				case RCompareLt:
					return boolInt(l < r)
				case RCompareLe:
					return boolInt(l <= r)
				case RCompareGt:
					return boolInt(l > r)
				case RCompareGe:
					return boolInt(l >= r)
				}
			}
			a := lv.AsInt()
			b := rv.AsInt()
			switch cat.Inst {
			case RCompareEq:
				return boolInt(a == b)
			case RCompareNe:
				return boolInt(a != b)
			case RCompareLt:
				return boolInt(a < b)
			case RCompareLe:
				return boolInt(a <= b)
			case RCompareGt:
				return boolInt(a > b)
			case RCompareGe:
				return boolInt(a >= b)
			}

		case RBasicLogic:
			switch cat.Inst {
			case RLogicAnd:
				if !truthy(k.walk(kids[0], env)) {
					return boolInt(false)
				}
				return boolInt(truthy(k.walk(kids[1], env)))
			case RLogicOr:
				if truthy(k.walk(kids[0], env)) {
					return boolInt(true)
				}
				return boolInt(truthy(k.walk(kids[1], env)))
			case RLogicNot:
				return boolInt(!truthy(k.walk(kids[0], env)))
			}

		case RBasicCond:
			cond := k.walk(kids[0], env)
			if truthy(cond) {
				n = kids[1] // TCO: taken branch is in tail position
				continue
			}
			if cat.Inst == RCondIfThenElse && len(kids) >= 3 {
				n = kids[2] // TCO: else branch is in tail position
				continue
			}
			return Value{Kind: VNull}

		case RBasicBlock:
			if cat.Inst == RBlockLet {
				name := k.identID(kids[0])
				v := k.walk(kids[1], env)
				env.Bind(name, v)
				return v
			}
			if len(kids) == 0 {
				return Value{}
			}
			for i := 0; i < len(kids)-1; i++ {
				k.walk(kids[i], env)
			}
			n = kids[len(kids)-1] // TCO: a do/seq block's last expr is in tail position
			continue

		case RBasicMatch:
			if cat.Inst == RMatchSwitch {
				return k.walkMatchSwitch(n, kids, env)
			}
			return Value{Kind: VNodeID, Nid: n}

		case RBasicChoice:
			switch cat.Inst {
			case RChoiceFail:
				panic(choiceFailSignal{})
			case RChoiceStop:
				panic(choiceStopSignal{})
			case RChoiceChoose:
				for _, branch := range kids {
					value, ok, _ := k.walkChoiceBranch(branch, env)
					if ok {
						return value
					}
				}
				panic(choiceFailSignal{})
			}
			return Value{Kind: VNull}

		case RBasicIdent:
			id := k.identID(n)
			if v, ok := env.Lookup(id); ok {
				return v
			}
			panic(fmt.Sprintf("walk: unbound identifier %q", k.nameStr(id)))

		case RBasicFnDef:
			name := k.identID(kids[0])
			paramKids := k.children(kids[1])
			params := make([]NameID, len(paramKids))
			for i, p := range paramKids {
				params[i] = NameID(p.Inst)
			}
			cl := &Closure{Name: name, Params: params, Body: kids[2], Env: env}
			env.Bind(name, Value{Kind: VClosure, Cl: cl})
			return Value{Kind: VClosure, Cl: cl}

		case RBasicFnCall:
			name := k.identID(kids[0])
			// Native takes priority unless user shadowed with a closure.
			if ne, ok := k.natives[name]; ok {
				if _, hasUserBinding := env.Lookup(name); !hasUserBinding {
					args := make([]Value, len(kids)-1)
					for i := 1; i < len(kids); i++ {
						args[i-1] = k.walk(kids[i], env)
					}
					return ne.Fn(k, args)
				}
			}
			v, ok := env.Lookup(name)
			if !ok {
				panic(fmt.Sprintf("walk: unbound function %q", k.nameStr(name)))
			}
			if v.Kind != VClosure {
				panic(fmt.Sprintf("walk: %q is not callable", k.nameStr(name)))
			}
			cl := v.Cl
			if len(kids)-1 != len(cl.Params) {
				panic(fmt.Sprintf("walk: %q wants %d args, got %d", k.nameStr(name), len(cl.Params), len(kids)-1))
			}
			argVals := make([]Value, len(cl.Params))
			for i := 1; i < len(kids); i++ {
				argVals[i-1] = k.walk(kids[i], env)
			}
			call := NewCallFrame(cl.Env, len(cl.Params))
			for i, p := range cl.Params {
				call.Bind(p, argVals[i])
			}
			n = cl.Body // TCO: closure body is in tail position.
			env = call
			continue

		case RBasicList:
			out := make([]Value, len(kids))
			for i, c := range kids {
				out[i] = k.walk(c, env)
			}
			return Value{Kind: VList, List: out}
		}

		// Structural passthrough — categories the walker can't execute intern
		// fine; walking returns the NodeID so downstream reasoning continues.
		return Value{Kind: VNodeID, Nid: n}
	}
}

func (k *Kernel) walkChoiceBranch(branch NodeID, env *Frame) (value Value, ok bool, stopped bool) {
	defer func() {
		if r := recover(); r != nil {
			switch r.(type) {
			case choiceFailSignal:
				value = Value{Kind: VNull}
				ok = false
				stopped = false
			case choiceStopSignal:
				value = Value{Kind: VNull}
				ok = true
				stopped = true
			default:
				panic(r)
			}
		}
	}()
	return k.walk(branch, env), true, false
}

// ---------------------------------------------------------------------------
// match/switch (copied verbatim from main.go, trace/observe stripped).
// ---------------------------------------------------------------------------

func (k *Kernel) switchTableFor(node NodeID, kids []NodeID) *switchTable {
	if table, ok := k.switchTables[node]; ok {
		return table
	}
	if len(kids) < 1 || (len(kids)-1)%2 != 0 {
		panic("match: SWITCH expects scrutinee plus pattern/body pairs")
	}
	table := &switchTable{cases: make(map[NodeID]NodeID)}
	for i := 1; i < len(kids); i += 2 {
		pattern := kids[i]
		body := kids[i+1]
		if k.isSwitchDefaultPattern(pattern) {
			table.defaultBody = body
			table.hasDefault = true
			continue
		}
		if pattern.Level == LevelTrivial {
			table.cases[pattern] = body
			continue
		}
		table.dynamicArms = append(table.dynamicArms, switchArm{pattern: pattern, body: body})
	}
	k.switchTables[node] = table
	return table
}

func (k *Kernel) isSwitchDefaultPattern(pattern NodeID) bool {
	if pattern.Level != LevelTrivial {
		cat := k.category(pattern)
		if cat.Type == RBasicIdent {
			return k.nameStr(k.identID(pattern)) == "_"
		}
	}
	return false
}

func (k *Kernel) switchKeyFromValue(v Value) (NodeID, bool) {
	switch v.Kind {
	case VNull:
		return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivNull, Inst: 0}, true
	case VInt:
		return k.internTrivialInt(v.Int), true
	case VFloat:
		return k.internTrivialFloat64(v.Float), true
	case VStr:
		return k.internString(v.Str), true
	case VBool:
		if v.Bool {
			return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivBool, Inst: 1}, true
		}
		return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivBool, Inst: 0}, true
	case VNodeID:
		return v.Nid, true
	default:
		return NodeID{}, false
	}
}

func valueEqual(a, b Value) bool {
	if a.Kind != b.Kind {
		return false
	}
	switch a.Kind {
	case VNull:
		return true
	case VInt:
		return a.Int == b.Int
	case VFloat:
		return a.Float == b.Float
	case VStr:
		return a.Str == b.Str
	case VBool:
		return a.Bool == b.Bool
	case VNodeID:
		return a.Nid == b.Nid
	case VList:
		if len(a.List) != len(b.List) {
			return false
		}
		for i := range a.List {
			if !valueEqual(a.List[i], b.List[i]) {
				return false
			}
		}
		return true
	default:
		return false
	}
}

func (k *Kernel) walkMatchSwitch(node NodeID, kids []NodeID, env *Frame) Value {
	if len(kids) < 1 || (len(kids)-1)%2 != 0 {
		panic("match: SWITCH expects scrutinee plus pattern/body pairs")
	}
	scrutinee := k.walk(kids[0], env)
	table := k.switchTableFor(node, kids)
	if key, ok := k.switchKeyFromValue(scrutinee); ok {
		if body, found := table.cases[key]; found {
			return k.walk(body, env)
		}
	}
	for _, arm := range table.dynamicArms {
		if valueEqual(k.walk(arm.pattern, env), scrutinee) {
			return k.walk(arm.body, env)
		}
	}
	if table.hasDefault {
		return k.walk(table.defaultBody, env)
	}
	panic(fmt.Sprintf("match: exhausted without a matching arm for %s", scrutinee.String()))
}

func truthy(v Value) bool {
	switch v.Kind {
	case VBool:
		return v.Bool
	case VInt:
		return v.Int != 0
	case VNull:
		return false
	}
	return true
}

func boolInt(b bool) Value {
	if b {
		return Value{Kind: VInt, Int: 1}
	}
	return Value{Kind: VInt, Int: 0}
}

// ---------------------------------------------------------------------------
// S-expression lexer + reader — the INDEPENDENT parse front-end. This is the
// load-bearing witness surface (it caught the 1e-05 float bug and the int64
// width bug). Copied verbatim from main.go; do not "improve".
// ---------------------------------------------------------------------------

type sexpToken struct {
	kind  string // "LPAREN" | "RPAREN" | "INT" | "FLOAT" | "STRING" | "IDENT"
	value string
	line  int
	col   int
}

func tokenizeSexp(src string) []sexpToken {
	tokens := make([]sexpToken, 0, 64)
	line, col := 1, 1
	advance := func(n int) { col += n }
	newline := func() { line++; col = 1 }
	i := 0
	for i < len(src) {
		c := src[i]
		if c == '\n' {
			i++
			newline()
			continue
		}
		if c == ' ' || c == '\t' || c == '\r' {
			i++
			advance(1)
			continue
		}
		if c == ';' {
			for i < len(src) && src[i] != '\n' {
				i++
			}
			continue
		}
		startLine, startCol := line, col
		if c == '(' {
			tokens = append(tokens, sexpToken{"LPAREN", "(", startLine, startCol})
			i++
			advance(1)
			continue
		}
		if c == ')' {
			tokens = append(tokens, sexpToken{"RPAREN", ")", startLine, startCol})
			i++
			advance(1)
			continue
		}
		if c == '"' {
			i++
			advance(1)
			start := i
			for i < len(src) && src[i] != '"' {
				if src[i] == '\\' && i+1 < len(src) {
					i += 2
					advance(2)
					continue
				}
				if src[i] == '\n' {
					newline()
				} else {
					advance(1)
				}
				i++
			}
			tokens = append(tokens, sexpToken{"STRING", unescapeStr(src[start:i]), startLine, startCol})
			if i < len(src) {
				i++
				advance(1)
			}
			continue
		}
		if (c >= '0' && c <= '9') || (c == '-' && i+1 < len(src) && src[i+1] >= '0' && src[i+1] <= '9') {
			start := i
			if c == '-' {
				i++
			}
			for i < len(src) && src[i] >= '0' && src[i] <= '9' {
				i++
			}
			isFloat := false
			// Fractional part: `.` followed by at least one digit.
			if i < len(src) && src[i] == '.' && i+1 < len(src) && src[i+1] >= '0' && src[i+1] <= '9' {
				isFloat = true
				i++ // consume '.'
				for i < len(src) && src[i] >= '0' && src[i] <= '9' {
					i++
				}
			}
			// Exponent: e/E [+/-] one-or-more digits. Accepted on both pure-int
			// mantissa (1e5) and fractional mantissa (1.5e3). THE scientific-
			// notation case (1e-05) the foreign witness exists to keep honest.
			if i < len(src) && (src[i] == 'e' || src[i] == 'E') {
				j := i + 1
				if j < len(src) && (src[j] == '+' || src[j] == '-') {
					j++
				}
				if j < len(src) && src[j] >= '0' && src[j] <= '9' {
					isFloat = true
					i = j
					for i < len(src) && src[i] >= '0' && src[i] <= '9' {
						i++
					}
				}
			}
			if isFloat {
				tokens = append(tokens, sexpToken{"FLOAT", src[start:i], startLine, startCol})
			} else {
				tokens = append(tokens, sexpToken{"INT", src[start:i], startLine, startCol})
			}
			advance(i - start)
			continue
		}
		// Identifier — any non-whitespace, non-paren, non-quote
		start := i
		for i < len(src) && src[i] != ' ' && src[i] != '\t' && src[i] != '\n' &&
			src[i] != '\r' && src[i] != '(' && src[i] != ')' && src[i] != '"' && src[i] != ';' {
			i++
		}
		tokens = append(tokens, sexpToken{"IDENT", src[start:i], startLine, startCol})
		advance(i - start)
	}
	return tokens
}

func unescapeStr(s string) string {
	out := make([]byte, 0, len(s))
	for i := 0; i < len(s); i++ {
		if s[i] == '\\' && i+1 < len(s) {
			switch s[i+1] {
			case 'n':
				out = append(out, '\n')
			case 't':
				out = append(out, '\t')
			case 'r':
				out = append(out, '\r')
			case '\\':
				out = append(out, '\\')
			case '"':
				out = append(out, '"')
			default:
				out = append(out, s[i+1])
			}
			i++
			continue
		}
		out = append(out, s[i])
	}
	return string(out)
}

func (k *Kernel) readSexpr(toks []sexpToken, i int) (NodeID, int) {
	if i >= len(toks) {
		panic("parse error: unexpected end of input (expected an expression)")
	}
	t := toks[i]
	switch t.kind {
	case "INT":
		n, _ := strconv.ParseInt(t.value, 10, 64)
		return k.internTrivialInt(n), i + 1
	case "FLOAT":
		f, err := strconv.ParseFloat(t.value, 64)
		if err != nil {
			panic(fmt.Sprintf("parse error: bad float literal %q at line %d col %d: %v",
				t.value, t.line, t.col, err))
		}
		return k.internTrivialFloat64(f), i + 1
	case "STRING":
		return k.internString(t.value), i + 1
	case "IDENT":
		if t.value == "true" {
			return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivBool, Inst: 1}, i + 1
		}
		if t.value == "false" {
			return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivBool, Inst: 0}, i + 1
		}
		return k.intern(catIdent(), []NodeID{k.internString(t.value)}), i + 1
	case "RPAREN":
		panic(fmt.Sprintf("parse error at line %d col %d: unmatched `)` (no `(` to close)", t.line, t.col))
	case "LPAREN":
		openLine, openCol := t.line, t.col
		i++
		if i >= len(toks) {
			panic(fmt.Sprintf("parse error: unclosed `(` opened at line %d col %d (reached end of input)", openLine, openCol))
		}
		if toks[i].kind == "RPAREN" {
			return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivNull, Inst: 0}, i + 1
		}
		if toks[i].kind != "IDENT" {
			panic(fmt.Sprintf("parse error at line %d col %d: expected verb after `(` opened at line %d col %d, got %s %q",
				toks[i].line, toks[i].col, openLine, openCol, toks[i].kind, toks[i].value))
		}
		verb := toks[i].value
		i++
		args := []NodeID{}
		for {
			if i >= len(toks) {
				panic(fmt.Sprintf("parse error: unclosed `(` opened at line %d col %d in `(%s ...)` (reached end of input)",
					openLine, openCol, verb))
			}
			if toks[i].kind == "RPAREN" {
				i++
				break
			}
			arg, ni := k.readSexpr(toks, i)
			args = append(args, arg)
			i = ni
		}
		node := k.buildVerb(verb, args)
		return node, i
	}
	panic(fmt.Sprintf("parse error at line %d col %d: unexpected token %s %q", t.line, t.col, t.kind, t.value))
}

func (k *Kernel) buildVerb(verb string, args []NodeID) NodeID {
	switch verb {
	case "do":
		return k.intern(catBlock(RBlockDo), args)
	case "seq":
		return k.intern(catBlock(RBlockSequence), args)
	case "let":
		nameID := k.identID(args[0])
		nameTrivial := NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivString, Inst: uint32(nameID)}
		return k.intern(catBlock(RBlockLet), []NodeID{nameTrivial, args[1]})
	case "if":
		if len(args) == 2 {
			return k.intern(catCond(RCondIfThen), args)
		}
		return k.intern(catCond(RCondIfThenElse), args)
	case "add":
		return k.intern(catMath(RMathPlus), args)
	case "sub":
		return k.intern(catMath(RMathMinus), args)
	case "mul":
		return k.intern(catMath(RMathMultiply), args)
	case "div":
		return k.intern(catMath(RMathDivide), args)
	case "mod":
		return k.intern(catMath(RMathModulo), args)
	case "eq":
		return k.intern(catCompare(RCompareEq), args)
	case "ne":
		return k.intern(catCompare(RCompareNe), args)
	case "lt":
		return k.intern(catCompare(RCompareLt), args)
	case "le":
		return k.intern(catCompare(RCompareLe), args)
	case "gt":
		return k.intern(catCompare(RCompareGt), args)
	case "ge":
		return k.intern(catCompare(RCompareGe), args)
	case "and":
		return k.intern(catLogic(RLogicAnd), args)
	case "or":
		return k.intern(catLogic(RLogicOr), args)
	case "not":
		return k.intern(catLogic(RLogicNot), args)
	case "match":
		return k.intern(catMatch(RMatchSwitch), args)
	case "choose":
		return k.intern(catChoice(RChoiceChoose), args)
	case "fail":
		return k.intern(catChoice(RChoiceFail), args)
	case "stop":
		return k.intern(catChoice(RChoiceStop), args)
	case "defn":
		toTriv := func(id NameID) NodeID {
			return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivString, Inst: uint32(id)}
		}
		nameTrivial := toTriv(k.identID(args[0]))
		paramKids := k.children(args[1])
		pnames := make([]NodeID, len(paramKids))
		for i, p := range paramKids {
			pnames[i] = toTriv(k.identID(p))
		}
		paramsBlock := k.intern(catBlock(RBlockSequence), pnames)
		return k.intern(catFnDef(), []NodeID{nameTrivial, paramsBlock, args[2]})
	case "params":
		return k.intern(catBlock(RBlockSequence), args)
	default:
		nameStr := k.internString(verb)
		all := append([]NodeID{nameStr}, args...)
		return k.intern(catFnCall(), all)
	}
}

// Category constructors (copied verbatim).
func catMath(inst uint32) NodeID    { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicMath, Inst: inst} }
func catCompare(inst uint32) NodeID { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicCompare, Inst: inst} }
func catLogic(inst uint32) NodeID   { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicLogic, Inst: inst} }
func catCond(inst uint32) NodeID    { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicCond, Inst: inst} }
func catBlock(inst uint32) NodeID   { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicBlock, Inst: inst} }
func catMatch(inst uint32) NodeID   { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicMatch, Inst: inst} }
func catChoice(inst uint32) NodeID  { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicChoice, Inst: inst} }
func catIdent() NodeID              { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicIdent, Inst: 1} }
func catFnDef() NodeID              { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicFnDef, Inst: 1} }
func catFnCall() NodeID             { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicFnCall, Inst: 1} }

// readRootFromSource — wrap multiple top-level forms in an implicit do-block.
// Copied verbatim from main.go.
func readRootFromSource(k *Kernel, src string) NodeID {
	toks := tokenizeSexp(src)
	wrapped := "(do " + src + ")"
	if len(toks) > 0 && toks[0].kind == "LPAREN" {
		depth := 0
		topLevelCount := 0
		for _, t := range toks {
			if t.kind == "LPAREN" {
				if depth == 0 {
					topLevelCount++
				}
				depth++
			} else if t.kind == "RPAREN" {
				depth--
			} else if depth == 0 {
				topLevelCount++
			}
		}
		if topLevelCount == 1 {
			wrapped = src
		}
	}
	toks = tokenizeSexp(wrapped)
	root, _ := k.readSexpr(toks, 0)
	return root
}

// ---------------------------------------------------------------------------
// Natives — the pure-op handlers exercised by the four-way bands. Copied
// verbatim from main.go's registerNatives (Blueprint-attribution categories
// dropped — they only fed the trace, which this walker omits).
// ---------------------------------------------------------------------------

func floorCharBoundary(s string, i int) int {
	if i > len(s) {
		i = len(s)
	}
	for i > 0 && i < len(s) && !utf8.RuneStart(s[i]) {
		i--
	}
	return i
}

func ceilCharBoundary(s string, i int) int {
	if i >= len(s) {
		return len(s)
	}
	for i < len(s) && !utf8.RuneStart(s[i]) {
		i++
	}
	return i
}

func (k *Kernel) registerNative(name string, fn NativeFn) {
	id := k.internName(name)
	k.natives[id] = NativeEntry{Name: id, Fn: fn}
}

func (k *Kernel) registerNatives() {
	// list / cons / head / tail / nth / empty — list family.
	k.registerNative("list", func(_ *Kernel, args []Value) Value {
		out := make([]Value, len(args))
		copy(out, args)
		return Value{Kind: VList, List: out}
	})
	k.registerNative("cons", func(_ *Kernel, args []Value) Value {
		out := make([]Value, 0, len(args[1].List)+1)
		out = append(out, args[0])
		out = append(out, args[1].List...)
		return Value{Kind: VList, List: out}
	})
	k.registerNative("head", func(_ *Kernel, args []Value) Value {
		if len(args[0].List) == 0 {
			return Value{Kind: VNull}
		}
		return args[0].List[0]
	})
	k.registerNative("tail", func(_ *Kernel, args []Value) Value {
		if len(args[0].List) == 0 {
			return Value{Kind: VList, List: []Value{}}
		}
		return Value{Kind: VList, List: args[0].List[1:]}
	})
	k.registerNative("nth", func(_ *Kernel, args []Value) Value {
		if args[0].Kind != VList {
			return Value{Kind: VNull}
		}
		i := args[1].AsInt()
		if i < 0 || int(i) >= len(args[0].List) {
			return Value{Kind: VNull}
		}
		return args[0].List[i]
	})
	k.registerNative("empty", func(_ *Kernel, _ []Value) Value {
		return Value{Kind: VList, List: []Value{}}
	})
	k.registerNative("len", func(_ *Kernel, args []Value) Value {
		switch args[0].Kind {
		case VList:
			return Value{Kind: VInt, Int: int64(len(args[0].List))}
		case VStr:
			return Value{Kind: VInt, Int: int64(len(args[0].Str))}
		}
		return Value{Kind: VInt, Int: 0}
	})

	// String family.
	k.registerNative("str_concat", func(_ *Kernel, args []Value) Value {
		return Value{Kind: VStr, Str: args[0].Str + args[1].Str}
	})
	k.registerNative("str_eq", func(_ *Kernel, args []Value) Value {
		return boolInt(args[0].Str == args[1].Str)
	})
	k.registerNative("str_len", func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: int64(len(args[0].Str))}
	})
	k.registerNative("str_find", func(_ *Kernel, args []Value) Value {
		s := args[0].Str
		needle := args[1].Str
		from := int(args[2].AsInt())
		if from < 0 {
			from = 0
		}
		if from > len(s) {
			return Value{Kind: VInt, Int: -1}
		}
		from = ceilCharBoundary(s, from)
		idx := strings.Index(s[from:], needle)
		if idx < 0 {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: int64(from + idx)}
	})
	k.registerNative("substring", func(_ *Kernel, args []Value) Value {
		s := args[0].Str
		a := args[1].AsInt()
		b := args[2].AsInt()
		if a < 0 || b < a || b > int64(len(s)) {
			panic(fmt.Sprintf("substring: bounds out of range start=%d end=%d len=%d", a, b, len(s)))
		}
		return Value{Kind: VStr, Str: s[floorCharBoundary(s, int(a)):floorCharBoundary(s, int(b))]}
	})
	k.registerNative("char_at", func(_ *Kernel, args []Value) Value {
		s := args[0].Str
		i := args[1].AsInt()
		if i < 0 || i >= int64(len(s)) {
			panic(fmt.Sprintf("char_at: bounds out of range index=%d len=%d", i, len(s)))
		}
		if !utf8.RuneStart(s[i]) {
			return Value{Kind: VStr, Str: ""}
		}
		r, _ := utf8.DecodeRuneInString(s[i:])
		return Value{Kind: VStr, Str: string(r)}
	})
	k.registerNative("int_to_str", func(_ *Kernel, args []Value) Value {
		v := args[0]
		switch v.Kind {
		case VStr:
			return Value{Kind: VStr, Str: v.Str}
		case VBool:
			if v.Bool {
				return Value{Kind: VStr, Str: "true"}
			}
			return Value{Kind: VStr, Str: "false"}
		case VNull:
			return Value{Kind: VStr, Str: "null"}
		}
		return Value{Kind: VStr, Str: v.String()}
	})

	// value_eq — structural value equality across kinds.
	k.registerNative("value_eq", func(k *Kernel, args []Value) Value {
		return boolInt(valueEqual(args[0], args[1]))
	})
}

// ---------------------------------------------------------------------------
// Entry — read concatenated .fk source, evaluate, print the root value.
// ---------------------------------------------------------------------------

func main() {
	args := os.Args[1:]
	if len(args) == 0 {
		fmt.Fprintln(os.Stderr, "usage: walker <file.fk> [more.fk ...]")
		os.Exit(2)
	}
	var parts []string
	for _, path := range args {
		b, err := os.ReadFile(path)
		if err != nil {
			fmt.Fprintf(os.Stderr, "read %s: %v\n", path, err)
			os.Exit(1)
		}
		parts = append(parts, string(b))
	}
	src := strings.Join(parts, "\n")

	defer func() {
		if r := recover(); r != nil {
			fmt.Fprintf(os.Stderr, "walker: %v\n", r)
			os.Exit(1)
		}
	}()

	k := NewKernel()
	root := readRootFromSource(k, src)
	env := NewFrame(nil)
	result := k.walk(root, env)
	fmt.Println(result.String())
}
