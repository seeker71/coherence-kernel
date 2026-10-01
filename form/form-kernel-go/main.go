// form-kernel-go — vertical-slice host for Form-on-top.
//
// Executes Form recipe trees and binary artifacts. It reads plain Form files
// named on argv, joined in order, and walks them as one unit; it follows no
// directive and lowers nothing — fkwu prepares a unit's whole closure
// ("./fkwu --closure <unit> <out>" from the repo root).
//
//   • Substrate          — NodeID + content-addressed intern table
//   • Walker             — all 22 RBasic dispatch arms
//   • Frames + closures  — scope, lookup, capture
//   • Native primitives  — strings, lists, I/O, conversion
//   • Binary loader      — Form artifact bytes → recipe tree
//
// Parsers and grammars belong in Form artifacts above this layer.
//
// Usage:  form-kernel-go <file.fk> [more.fk ...]
//         form-kernel-go --bench
//         form-kernel-go --expr "(add 2 3)"

package main

import (
	"encoding/json"
	"fmt"
	"form-kernel-go/core"
	"hash/fnv"
	"io"
	"math"
	"net"
	"os"
	"os/exec"
	"path/filepath"
	"runtime/debug"
	"sort"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"
	"time"
	"unicode/utf8"
)

// --- core types extracted to package core (so JIT plugins can import them) ---
type NodeID = core.NodeID
type NameID = core.NameID
type ValueKind = core.ValueKind
type Value = core.Value

// nodeIDKey identifies a recipe in observation and dispatch tables.
func nodeIDKey(n NodeID) string {
	return fmt.Sprintf("%d.%d.%d.%d", n.Pkg, n.Level, n.Type, n.Inst)
}

type Record = core.Record
type Closure = core.Closure
type Frame = core.Frame

const (
	VNull    = core.VNull
	VInt     = core.VInt
	VStr     = core.VStr
	VList    = core.VList
	VClosure = core.VClosure
	VNodeID  = core.VNodeID
	VFloat   = core.VFloat
	VRecord  = core.VRecord
)

var (
	NewFrame      = core.NewFrame
	NewCallFrame  = core.NewCallFrame
	formatFloatJS = core.FormatFloatJS
)

// --- Socket natives — L1 physical layer (TCP) ---------------------------
// Connection table. Handles are monotone ints; the kernel never reveals
// the underlying Listener/Conn to Form code — only the handle. -1 always
// means error (sibling parity with Rust/TS). socket_close on -1 is a
// no-op returning -1.
var (
	socketTableMu sync.Mutex
	socketHandles       = map[int64]interface{}{} // listener or conn
	socketNextHnd int64 = 0
)

func socketRegister(v interface{}) int64 {
	socketTableMu.Lock()
	defer socketTableMu.Unlock()
	socketNextHnd++
	h := socketNextHnd
	socketHandles[h] = v
	return h
}

func socketLookup(h int64) interface{} {
	socketTableMu.Lock()
	defer socketTableMu.Unlock()
	return socketHandles[h]
}

func socketDrop(h int64) {
	socketTableMu.Lock()
	defer socketTableMu.Unlock()
	delete(socketHandles, h)
}

// ---------------------------------------------------------------------------
// Substrate — NodeID + Recipe + intern table
// ---------------------------------------------------------------------------

// NodeID — the 4-tuple identity. Registered substrate ids use pkg=1.
// Runtime-interned composites use pkg=0 so temporary recipe ids cannot collide
// with registered/basic categories across an execution context boundary.
// Trivials encode their value in `Inst`.

const (
	LevelTrivial uint32 = 1
	LevelBasic   uint32 = 2

	// RBasic — governed by form/category-contract.json.
	RBasicUndefined uint32 = 0
	RBasicWitness   uint32 = 6 // substrate self-attestation
	RBasicBlock     uint32 = 9
	RBasicCall      uint32 = 10 // invoke external effect (I/O, tool)
	RBasicCond      uint32 = 11
	RBasicMath      uint32 = 12
	RBasicCompare   uint32 = 13
	RBasicLogic     uint32 = 14
	RBasicAccess    uint32 = 15 // read property / field
	RBasicMatch     uint32 = 19 // match/switch by substrate key
	RBasicChoice    uint32 = 20 // choose/fail/stop - speculative branching
	RBasicMethod    uint32 = 27 // transform on a cell-like value
	RBasicTransmute uint32 = 76 // present value through Blueprint without changing identity
	// Kernel-demo additions (extending RBasic for self-hosting needs)
	RBasicFnDef        uint32 = 31
	RBasicFnCall       uint32 = 32
	RBasicIdent        uint32 = 33
	RBasicList         uint32 = 34  // list-literal recipe
	RBasicField        uint32 = 88  // Field Model Form: distributed field value
	RBasicCarrier      uint32 = 89  // sequence / graph / mesh / attention carrier
	RBasicTopology     uint32 = 90  // adjacency and boundary declaration
	RBasicFiber        uint32 = 91  // value shape at each field location
	RBasicRegion       uint32 = 92  // named carrier subset
	RBasicBoundary     uint32 = 93  // membrane / constraint surface
	RBasicNeighborhood uint32 = 94  // local context relation
	RBasicMatchField   uint32 = 95  // region / subgraph / gradient match
	RBasicDelta        uint32 = 96  // snapshot-relative candidate mutation
	RBasicResolve      uint32 = 97  // conflict algebra over deltas
	RBasicCommit       uint32 = 98  // atomic logical-time commit
	RBasicStep         uint32 = 99  // freeze/match/choose/delta/commit
	RBasicLift         uint32 = 100 // data -> field
	RBasicSample       uint32 = 101 // point/region probe
	RBasicObserve      uint32 = 102 // projection + receipt
	RBasicIntervene    uint32 = 103 // consented perturbation
	RBasicResidual     uint32 = 104 // loss / budget remainder
	RBasicReceipt      uint32 = 105 // transparent execution record
	RBasicCost         uint32 = 106 // observer cost ledger
	RBasicConsent      uint32 = 107 // permission surface
	RBasicEvidence     uint32 = 108 // observed/inferred/simulated status

	TrivInt    uint32 = 1
	TrivString uint32 = 2
	TrivBool   uint32 = 3
	TrivNull   uint32 = 4
	// INT64 — signed integer whose magnitude exceeds the 32-bit inst slot.
	// Integer literals fit inline in TrivInt while |n| ≤ 2^31-1; once a
	// literal (hash, address, large counter) crosses the int32 ceiling the
	// inst carries an index into the kernel's `i64s` overflow table, exactly
	// as FLOAT64 does with `f64s`. Both TrivInt and TrivInt64 decode to the
	// same Value{VInt, int64}, so arithmetic stays a single uniform i64 path.
	// Slot 5 is aligned three-way — Rust, Go, and TS all carry INT64 = 5.
	TrivInt64 uint32 = 5
	// FLOAT32 — IEEE 754 32-bit value stored inline; the inst field carries
	// the IEEE bit pattern reinterpreted as u32. No overflow table needed.
	// Matches sibling TS kernel's Triv.FLOAT32 = 6.
	TrivFloat32 uint32 = 6
	// FLOAT64 — IEEE 754 64-bit value; 64 bits exceed the 32-bit inst slot,
	// so the inst carries an index into the kernel's `f64s` overflow table.
	// Canonicalization on intern: NaN bit patterns collapse to qNaN, -0.0
	// collapses to +0.0, ±Inf stay distinct (mirrors Rust + TS sibling
	// kernels). The trivial float type tags are aligned three-way — Rust,
	// Go, and TS all carry FLOAT32 = 6 and FLOAT64 = 7. Cross-kernel parity
	// also rides on .fk source, where the token shape (digits+dot vs digits)
	// names the type, and on the .fkb value-carrying float record.
	TrivFloat64 uint32 = 7
)

// RMath / RCompare / RLogic / RCond / RBlock instance constants
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

// NameID — interned identifier handle. The same uint32 used to encode a
// name trivial's NodeID instance is what every runtime name-lookup
// compares. String comparison happens once at parse time, never in the
// hot path.

// NativeEntry — a native's function plus the Form category it expresses.
// Carries Blueprint attribution into the kernel: when the walker dispatches
// through a native, the trace records the category alongside the FNCALL
// arm, so reasoning about which Form-shapes did the work reaches inside
// the host-language layer. UNDEFINED marks natives whose Form attribution
// hasn't been settled yet — honest, not omitted.
type NativeEntry struct {
	Name     NameID
	Category NodeID
	Fn       NativeFn
}

// armKey — (ty, inst) tuple key for trace dispatch counters. Storing the
// inst alongside ty surfaces typed-numeric distribution — MATH.PLUS_F64
// (inst=0x91) becomes distinguishable from MATH.PLUS_I32 (inst=0x01) in
// the report.
type armKey struct {
	Ty   uint32
	Inst uint32
}

// Trace — per-(arm, inst) dispatch counters. Held inside Kernel so the
// walker can record without threading an extra reference through every
// recursive call. Mirrors the Rust kernel's trace structure for sibling-
// kernel parity.
type Trace struct {
	TotalWalks      uint64
	ArmCounts       map[armKey]uint64 // (cat.Type, cat.Inst) → count
	FnCounts        map[string]uint64
	NativeCounts    map[string]uint64
	ChoiceAttempts  uint64
	ChoiceSuccesses uint64
	ChoiceFailures  uint64
	MatchLookups    uint64
	MatchHits       uint64
	MatchDefaults   uint64
	MatchMisses     uint64
}

func newTrace() *Trace {
	return &Trace{
		ArmCounts:    make(map[armKey]uint64),
		FnCounts:     make(map[string]uint64),
		NativeCounts: make(map[string]uint64),
	}
}

func (t *Trace) record(armTy uint32, armInst uint32) {
	t.TotalWalks++
	t.ArmCounts[armKey{Ty: armTy, Inst: armInst}]++
}

func (t *Trace) recordFn(name string) {
	t.FnCounts[name]++
}

func (t *Trace) recordNative(name string) {
	t.NativeCounts[name]++
}

// armName — label categories in the trace JSON. Walker arms + native
// Blueprint-attribution categories. Mirrors Rust kernel's Trace::arm_name.
func armName(armTy uint32) string {
	switch armTy {
	case RBasicBlock:
		return "BLOCK"
	case RBasicCond:
		return "COND"
	case RBasicMath:
		return "MATH"
	case RBasicCompare:
		return "COMPARE"
	case RBasicLogic:
		return "LOGIC"
	case RBasicMatch:
		return "MATCH"
	case RBasicIdent:
		return "IDENT"
	case RBasicFnDef:
		return "FNDEF"
	case RBasicFnCall:
		return "FNCALL"
	case RBasicList:
		return "LIST"
	case RBasicChoiceMatch:
		return "CHOICE_MATCH"
	case RBasicWitness:
		return "WITNESS"
	case RBasicCall:
		return "CALL"
	case RBasicAccess:
		return "ACCESS"
	case RBasicChoice:
		return "CHOICE"
	case RBasicMethod:
		return "METHOD"
	case RBasicTransmute:
		return "TRANSMUTE"
	case RBasicField:
		return "FIELD"
	case RBasicCarrier:
		return "CARRIER"
	case RBasicTopology:
		return "TOPOLOGY"
	case RBasicFiber:
		return "FIBER"
	case RBasicRegion:
		return "REGION"
	case RBasicBoundary:
		return "BOUNDARY"
	case RBasicNeighborhood:
		return "NEIGHBORHOOD"
	case RBasicMatchField:
		return "MATCH_FIELD"
	case RBasicDelta:
		return "DELTA"
	case RBasicResolve:
		return "RESOLVE"
	case RBasicCommit:
		return "COMMIT"
	case RBasicStep:
		return "STEP"
	case RBasicLift:
		return "LIFT"
	case RBasicSample:
		return "SAMPLE"
	case RBasicObserve:
		return "OBSERVE"
	case RBasicIntervene:
		return "INTERVENE"
	case RBasicResidual:
		return "RESIDUAL"
	case RBasicReceipt:
		return "RECEIPT"
	case RBasicCost:
		return "COST"
	case RBasicConsent:
		return "CONSENT"
	case RBasicEvidence:
		return "EVIDENCE"
	default:
		return "OTHER"
	}
}

// armVariantName — readable label for an (arm_ty, arm_inst) pair.
// Returns "MATH.PLUS", "COMPARE.LE", "BLOCK.LET", etc. For arms without
// a known inst encoding, returns just the bare arm name. Symmetric with
// the Rust/TS variant naming.
func armVariantName(armTy uint32, armInst uint32) string {
	base := armName(armTy)
	var variant string
	switch armTy {
	case RBasicMath:
		switch armInst {
		case RMathPlus:
			variant = "PLUS"
		case RMathMinus:
			variant = "MINUS"
		case RMathMultiply:
			variant = "MUL"
		case RMathDivide:
			variant = "DIV"
		case RMathModulo:
			variant = "MOD"
		}
	case RBasicCompare:
		switch armInst {
		case RCompareEq:
			variant = "EQ"
		case RCompareNe:
			variant = "NE"
		case RCompareLt:
			variant = "LT"
		case RCompareLe:
			variant = "LE"
		case RCompareGt:
			variant = "GT"
		case RCompareGe:
			variant = "GE"
		}
	case RBasicLogic:
		switch armInst {
		case RLogicAnd:
			variant = "AND"
		case RLogicOr:
			variant = "OR"
		case RLogicNot:
			variant = "NOT"
		}
	case RBasicCond:
		switch armInst {
		case RCondIfThen:
			variant = "IF"
		case RCondIfThenElse:
			variant = "IF_ELSE"
		}
	case RBasicBlock:
		switch armInst {
		case RBlockDo:
			variant = "DO"
		case RBlockSequence:
			variant = "SEQ"
		case RBlockLet:
			variant = "LET"
		}
	case RBasicMatch:
		switch armInst {
		case RMatchSwitch:
			variant = "SWITCH"
		}
	case RBasicChoice:
		switch armInst {
		case RChoiceChoose:
			variant = "CHOOSE"
		case RChoiceFail:
			variant = "FAIL"
		case RChoiceStop:
			variant = "STOP"
		}
	}
	if variant == "" {
		return base
	}
	return base + "." + variant
}

func (t *Trace) toJSON() map[string]interface{} {
	type variantRec struct {
		ArmTy      uint32 `json:"arm_ty"`
		ArmInst    uint32 `json:"arm_inst"`
		ArmName    string `json:"arm_name"`
		ArmVariant string `json:"arm_variant_name"`
		Count      uint64 `json:"count"`
	}
	type armRec struct {
		ArmTy   uint32 `json:"arm_ty"`
		ArmName string `json:"arm_name"`
		Count   uint64 `json:"count"`
	}
	type nameRec struct {
		Name  string `json:"name"`
		Count uint64 `json:"count"`
	}

	// Per-(ty, inst) records — preserves typed-numeric distribution.
	variants := make([]variantRec, 0, len(t.ArmCounts))
	for k, c := range t.ArmCounts {
		variants = append(variants, variantRec{
			ArmTy:      k.Ty,
			ArmInst:    k.Inst,
			ArmName:    armName(k.Ty),
			ArmVariant: armVariantName(k.Ty, k.Inst),
			Count:      c,
		})
	}
	sort.Slice(variants, func(i, j int) bool { return variants[i].Count > variants[j].Count })

	// Per-ty aggregate — kept for backward compatibility with consumers
	// that want the coarser shape.
	byTy := make(map[uint32]uint64)
	for k, c := range t.ArmCounts {
		byTy[k.Ty] += c
	}
	arms := make([]armRec, 0, len(byTy))
	for ty, c := range byTy {
		arms = append(arms, armRec{ArmTy: ty, ArmName: armName(ty), Count: c})
	}
	sort.Slice(arms, func(i, j int) bool { return arms[i].Count > arms[j].Count })

	functions := make([]nameRec, 0, len(t.FnCounts))
	for name, c := range t.FnCounts {
		functions = append(functions, nameRec{Name: name, Count: c})
	}
	sort.Slice(functions, func(i, j int) bool { return functions[i].Count > functions[j].Count })

	natives := make([]nameRec, 0, len(t.NativeCounts))
	for name, c := range t.NativeCounts {
		natives = append(natives, nameRec{Name: name, Count: c})
	}
	sort.Slice(natives, func(i, j int) bool { return natives[i].Count > natives[j].Count })

	rate := 0.0
	if t.ChoiceAttempts > 0 {
		rate = float64(t.ChoiceSuccesses) / float64(t.ChoiceAttempts)
	}
	return map[string]interface{}{
		"total_walks":         t.TotalWalks,
		"arms":                arms,     // aggregated by ty (backward-compatible)
		"variants":            variants, // full (ty, inst) granularity
		"functions":           functions,
		"natives":             natives,
		"choice_attempts":     t.ChoiceAttempts,
		"choice_successes":    t.ChoiceSuccesses,
		"choice_failures":     t.ChoiceFailures,
		"choice_success_rate": rate,
		"match_lookups":       t.MatchLookups,
		"match_hits":          t.MatchHits,
		"match_defaults":      t.MatchDefaults,
		"match_misses":        t.MatchMisses,
	}
}

// Kernel — the running substrate.
type Kernel struct {
	stopSeq   int64 // stops an attempt has caught, the id of each stop line
	walkDepth int64 // host walk invocations standing now
	walkWall  int64 // the depth where a walk stops: the recursion needs to be tail or balanced
	byHash    map[uint64]NodeID
	byID      map[NodeID]Recipe
	strs      []string
	strIdx    map[string]NameID
	next      uint32
	// Float64 overflow table — IEEE 754 values don't fit the 32-bit `inst`
	// field, so the FLOAT64 trivial NodeID carries an index into `f64s`.
	// `f64Idx` is keyed by the IEEE bit pattern after canonicalization
	// (NaN → qNaN, -0.0 → +0.0) so the same value parsed twice yields the
	// same NodeID. Mirrors Rust + TS sibling kernels.
	f64s   []float64
	f64Idx map[uint64]uint32
	// Int64 overflow table — the sibling of `f64s` for integers wider than the
	// 32-bit inst slot. `i64Idx` is keyed by the value itself (integers are
	// already canonical) so the same literal interns to the same NodeID.
	i64s    []int64
	i64Idx  map[int64]uint32
	natives map[NameID]NativeEntry
	// methods — the blueprint method table (BML/NUMS reference: methods live
	// on the blueprint/type, shared by all instances, name-dispatched). Keyed
	// by (blueprint, method-name) → the method's Closure.
	methods map[methodKey]*Closure
	// sourceAttr — NodeID → (file_name_id, line, col), written by fb_record and
	// the reader, read by node_source.
	sourceAttr map[NodeID]sourceLoc
	// formStack — the Form-level call chain currently live (closure and
	// native names, innermost last; closure labels carry file:line:col
	// when the body recipe is attributed). Pushed at dispatch, truncated
	// on the walk's success path — so after a panic the frames that were
	// live at the crash are still here for the recover site to surface.
	formStack []formFrame
	// readingFiles — line map for the source currently being read:
	// (file_name_id, first_global_line) per concatenated part. When
	// non-empty, readSexpr attributes every parenthesized form so fatal
	// diagnostics can name the Form source line.
	readingFiles     []readingPart
	importSeq        uint32
	activeRoots      []NodeID
	framebufferRoots []NodeID
	observeRuntime   bool
	observeSeq       uint32
	// Optional tracing — nil for hot-path runs, set for trace subcommand.
	// Per lc-native-kernel-binary's "tracing and observation pattern."
	Trace        *Trace
	switchTables map[NodeID]*switchTable
	// unitRoots — the unit roots the reader built, by how walkUnit reads
	// them: unitDo for a source whose one form is a (do ...), unitWrapper for
	// the implicit do that holds several top-level forms.
	unitRoots map[NodeID]uint8
	// unitView — the unit version: a later unit let that rebinds a name
	// raises it, and a closure defined at the unit level reads the unit as of
	// its own.
	unitView uint64
	// closuresCreated — closures made so far; a do binds a let in a fresh
	// frame when one was made since the do's scope last opened.
	closuresCreated uint64
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

type readingPart struct {
	FileID    NameID
	StartLine uint32
}

type sourceLoc struct {
	FileID NameID
	Line   uint32
	Col    uint32
}

// goWalkStack answers the walker's stack from FORM_KERNEL_STACK_MB, the door every kernel
// reads (default 2048 MB, as TS's worker), raises the goroutine stack ceiling to it, and
// answers the depth wall: the stack over the bytes one Form level costs this walker
// (measured about 8 KB: 1 GB held between 100,000 and 150,000 levels). The wall is a stop,
// catchable by attempt, where the ceiling would be a fatal no recover reaches.
func goWalkStack() int64 {
	mb := int64(2048)
	if s := strings.TrimSpace(os.Getenv("FORM_KERNEL_STACK_MB")); s != "" {
		if v, err := strconv.ParseInt(s, 10, 64); err == nil && v >= 8 {
			mb = v
		}
	}
	debug.SetMaxStack(int(mb << 20))
	return (mb << 20) / 10240
}

var goWalkWall = goWalkStack()

func NewKernel() *Kernel {
	k := &Kernel{
		walkWall:     goWalkWall,
		byHash:       make(map[uint64]NodeID),
		byID:         make(map[NodeID]Recipe),
		strIdx:       make(map[string]NameID),
		sourceAttr:   make(map[NodeID]sourceLoc),
		importSeq:    1,
		unitView:     1,
		next:         1,
		f64Idx:       make(map[uint64]uint32),
		i64Idx:       make(map[int64]uint32),
		natives:      make(map[NameID]NativeEntry),
		methods:      make(map[methodKey]*Closure),
		switchTables: make(map[NodeID]*switchTable),
	}
	k.registerNatives()
	return k
}

// hostMonotonicStart anchors host_monotonic_ms: milliseconds on Go's monotonic clock since this
// kernel started. Only differences between two readings mean anything.
var hostMonotonicStart = time.Now()

// recordConstructions is the record-construction clock kernel_stat 164 reads: every record_new
// this process has run, counted where the record is made. fkwu answers the same key from its arm
// counter for tag 64 (record_new). Nothing lowers it.
var recordConstructions atomic.Int64

// hostBirthUnixMs is this kernel's birth on the wall clock: word 2 (start-ms) of the page
// kernel_live answers for this process. kernelLiveMagic is word 0, fkwu's live-page magic.
var hostBirthUnixMs = time.Now().UnixMilli()

const kernelLiveMagic = 0x464B4C4956

// resolveKernelHostPath answers where a host path names a file for a read-side door, the same way
// on every kernel. A path that stands where the kernel runs names itself. Otherwise the walk tries
// dir/p, dir/form/p and dir/form/form/p from the working directory upward and stops at the checkout
// that holds it, the first directory with a .git entry, so one checkout never reads another's
// files. Doors that create or change a file never walk: they name the path as given.
func resolveKernelHostPath(path string) string {
	if path == "" || filepath.IsAbs(path) {
		return path
	}
	if _, err := os.Stat(path); err == nil {
		return path
	}
	directory, err := os.Getwd()
	if err != nil {
		return path
	}
	for {
		for _, candidate := range []string{
			filepath.Join(directory, path),
			filepath.Join(directory, "form", path),
			filepath.Join(directory, "form", "form", path),
		} {
			if _, statErr := os.Stat(candidate); statErr == nil {
				return candidate
			}
		}
		if _, gitErr := os.Stat(filepath.Join(directory, ".git")); gitErr == nil {
			return path
		}
		parent := filepath.Dir(directory)
		if parent == directory {
			return path
		}
		directory = parent
	}
}

func sourceInventorySkipSet(v Value) map[string]bool {
	skip := map[string]bool{}
	if v.Kind != VList {
		return skip
	}
	for _, item := range v.List {
		if item.Kind == VStr && item.Str != "" {
			skip[item.Str] = true
		}
	}
	return skip
}

func countTextLines(path string) int64 {
	body, err := os.ReadFile(path)
	if err != nil {
		return -1
	}
	if len(body) == 0 {
		return 0
	}
	lines := int64(strings.Count(string(body), "\n"))
	if body[len(body)-1] != '\n' {
		lines++
	}
	return lines
}

func sourceInventoryRow(rel string, loc int64) Value {
	return Value{Kind: VList, List: []Value{
		{Kind: VStr, Str: rel},
		{Kind: VInt, Int: loc},
	}}
}

func (k *Kernel) nextImportScope() uint32 {
	scope := k.importSeq
	k.importSeq++
	return scope
}

func (k *Kernel) remapImportedLeaf(scope uint32, nid NodeID) NodeID {
	if nid.Pkg != 0 {
		return nid
	}
	return k.intern(catUndefined(), []NodeID{
		k.internTrivialInt(int64(scope)),
		k.internTrivialInt(int64(nid.Level)),
		k.internTrivialInt(int64(nid.Type)),
		k.internTrivialInt(int64(nid.Inst)),
	})
}

func hashRecipe(r Recipe) uint64 {
	h := fnv.New64a()
	fmt.Fprintf(h, "C|%d.%d.%d.%d", r.Category.Pkg, r.Category.Level, r.Category.Type, r.Category.Inst)
	for _, c := range r.Children {
		fmt.Fprintf(h, "|%d.%d.%d.%d", c.Pkg, c.Level, c.Type, c.Inst)
	}
	return h.Sum64()
}

// intern — content-addressed insertion. Same shape ⇒ same NodeID.
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
	// Inline while the value fits the 32-bit inst slot; overflow into `i64s`
	// once it crosses the int32 ceiling (mirrors internTrivialFloat64). Both
	// paths decode back to Value{VInt, int64} in trivialValue, so callers and
	// arithmetic never see the storage split.
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

// decodeInt64 — read back the value from the i64 overflow table.
func (k *Kernel) decodeInt64(inst uint32) int64 {
	if int(inst) >= len(k.i64s) {
		panic(fmt.Sprintf("decodeInt64: bad index %d", inst))
	}
	return k.i64s[inst]
}

// internTrivialFloat32 — IEEE 754 32-bit inline encoding. The float's bit
// pattern (cast through math.Float32bits) lives directly in the inst slot;
// no overflow table needed. Two f32 values with the same bit pattern share
// the same NodeID by construction. NaN bit patterns are NOT canonicalized
// here because f32 NaNs are uncommon at the substrate boundary; if needed,
// the caller (or a typed-numeric layer above) collapses them first.
// Sibling parity with TS kernel's internTrivialFloat32.
func (k *Kernel) internTrivialFloat32(f float32) NodeID {
	bits := math.Float32bits(f)
	return NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivFloat32, Inst: bits}
}

// internTrivialFloat64 — content-addressed insertion into the f64 overflow
// table. The trivial NodeID carries the table index in `inst`. Canonicalization
// matches the Rust + TS sibling kernels so the same float value parsed twice
// produces the same NodeID:
//   - any NaN bit pattern collapses to qNaN (0x7ff8000000000000)
//   - -0.0 collapses to +0.0
//   - ±Inf keep distinct identity
func (k *Kernel) internTrivialFloat64(f float64) NodeID {
	var canonical float64
	switch {
	case math.IsNaN(f):
		canonical = math.Float64frombits(0x7ff8000000000000)
	case f == 0.0:
		// IEEE 754: -0.0 == +0.0 by value comparison; collapse both to +0.0
		// so the substrate doesn't carry a redundant duplicate entry.
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

// decodeFloat32 — read back the IEEE bit pattern from the inst slot.
func (k *Kernel) decodeFloat32(inst uint32) float32 {
	return math.Float32frombits(inst)
}

// decodeFloat64 — read back the value from the overflow table.
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

// internName — fast path when the caller already holds the string and
// only needs the NameID (no NodeID wrapper).
func (k *Kernel) internName(s string) NameID {
	if idx, ok := k.strIdx[s]; ok {
		return idx
	}
	idx := NameID(len(k.strs))
	k.strs = append(k.strs, s)
	k.strIdx[s] = idx
	return idx
}

func (k *Kernel) observationActive() bool {
	return k.observeRuntime || k.Trace != nil
}

func (k *Kernel) observeFrame(file string, line uint32, col uint32, children ...NodeID) {
	if !k.observationActive() {
		return
	}
	k.observeSeq++
	kids := make([]NodeID, 0, len(children)+1)
	kids = append(kids, k.internTrivialInt(int64(k.observeSeq)))
	kids = append(kids, children...)
	nid := k.intern(catReceipt(), kids)
	fileID := k.internName(file)
	k.sourceAttr[nid] = sourceLoc{FileID: fileID, Line: line, Col: col}
	k.activeRoots = append(k.activeRoots, nid)
	k.framebufferRoots = append(k.framebufferRoots, nid)
}

func (k *Kernel) observeRecipeDispatch(cat NodeID) {
	k.observeFrame(
		"observe/go/recipe-dispatch",
		cat.Type,
		cat.Inst,
		k.internTrivialInt(int64(cat.Type)),
		k.internTrivialInt(int64(cat.Inst)),
	)
}

func (k *Kernel) observeNamedDispatch(file string, name NameID) {
	k.observeFrame(file, uint32(name), 1, k.internString(k.nameStr(name)))
}

func (k *Kernel) nodeDisplay(n NodeID) string {
	if n.Level != LevelTrivial {
		return nodeIDKey(n)
	}
	return k.trivialValue(n).String()
}

func (k *Kernel) framebufferSourceCounts() []map[string]interface{} {
	type key struct {
		file string
		line uint32
		col  uint32
	}
	counts := map[key]int{}
	for _, nid := range k.framebufferRoots {
		loc, ok := k.sourceAttr[nid]
		if !ok {
			continue
		}
		file := ""
		if int(loc.FileID) < len(k.strs) {
			file = k.strs[loc.FileID]
		}
		counts[key{file: file, line: loc.Line, col: loc.Col}]++
	}
	rows := make([]map[string]interface{}, 0, len(counts))
	for k, count := range counts {
		rows = append(rows, map[string]interface{}{
			"file":  k.file,
			"line":  k.line,
			"col":   k.col,
			"count": count,
		})
	}
	sort.Slice(rows, func(i, j int) bool {
		ci := rows[i]["count"].(int)
		cj := rows[j]["count"].(int)
		if ci != cj {
			return ci > cj
		}
		fi := rows[i]["file"].(string)
		fj := rows[j]["file"].(string)
		if fi != fj {
			return fi < fj
		}
		li := rows[i]["line"].(uint32)
		lj := rows[j]["line"].(uint32)
		if li != lj {
			return li < lj
		}
		return rows[i]["col"].(uint32) < rows[j]["col"].(uint32)
	})
	return rows
}

func (k *Kernel) framebufferEvents() []map[string]interface{} {
	rows := make([]map[string]interface{}, 0, len(k.framebufferRoots))
	for _, nid := range k.framebufferRoots {
		loc, ok := k.sourceAttr[nid]
		if !ok {
			continue
		}
		file := ""
		if int(loc.FileID) < len(k.strs) {
			file = k.strs[loc.FileID]
		}
		kids := k.children(nid)
		seq := int64(0)
		if len(kids) > 0 && kids[0].Level == LevelTrivial && kids[0].Type == TrivInt {
			seq = int64(int32(kids[0].Inst))
		}
		childRows := make([]string, 0, len(kids))
		childValues := make([]string, 0, len(kids))
		for _, child := range kids {
			childRows = append(childRows, nodeIDKey(child))
			childValues = append(childValues, k.nodeDisplay(child))
		}
		rows = append(rows, map[string]interface{}{
			"seq":          seq,
			"file":         file,
			"line":         loc.Line,
			"col":          loc.Col,
			"node":         nodeIDKey(nid),
			"children":     childRows,
			"child_values": childValues,
		})
	}
	sort.Slice(rows, func(i, j int) bool {
		si := rows[i]["seq"].(int64)
		sj := rows[j]["seq"].(int64)
		if si != sj {
			return si < sj
		}
		return rows[i]["node"].(string) < rows[j]["node"].(string)
	})
	return rows
}

// The serve worker's per-request sweep: nodes and trailing strings no live
// value, frame or program root reaches.
func markStringNode(n NodeID, liveStrings map[NameID]bool) {
	if n.Pkg == 1 && n.Level == LevelTrivial && n.Type == TrivString {
		liveStrings[NameID(n.Inst)] = true
	}
}

func (k *Kernel) markNode(n NodeID, liveNodes map[NodeID]bool, liveStrings map[NameID]bool) {
	markStringNode(n, liveStrings)
	if n.Pkg != 0 || liveNodes[n] {
		return
	}
	r, ok := k.byID[n]
	if !ok {
		return
	}
	liveNodes[n] = true
	k.markNode(r.Category, liveNodes, liveStrings)
	for _, child := range r.Children {
		k.markNode(child, liveNodes, liveStrings)
	}
}

func (k *Kernel) markValue(v Value, liveNodes map[NodeID]bool, liveStrings map[NameID]bool, liveFrames map[*Frame]bool) {
	switch v.Kind {
	case VList:
		for _, item := range v.List {
			k.markValue(item, liveNodes, liveStrings, liveFrames)
		}
	case VClosure:
		if v.Cl != nil {
			liveStrings[v.Cl.Name] = true
			k.markNode(v.Cl.Body, liveNodes, liveStrings)
			k.markFrame(v.Cl.Env, liveNodes, liveStrings, liveFrames)
		}
	case VNodeID:
		k.markNode(v.Nid, liveNodes, liveStrings)
	}
}

func (k *Kernel) markFrame(frame *Frame, liveNodes map[NodeID]bool, liveStrings map[NameID]bool, liveFrames map[*Frame]bool) {
	for cur := frame; cur != nil; cur = cur.Parent {
		if liveFrames[cur] {
			return
		}
		liveFrames[cur] = true
		for _, binding := range cur.Bindings {
			liveStrings[binding.Name] = true
			k.markValue(binding.Val, liveNodes, liveStrings, liveFrames)
		}
		for _, h := range cur.History {
			for _, e := range h {
				k.markValue(e.Val, liveNodes, liveStrings, liveFrames)
			}
		}
	}
}

func (k *Kernel) substrateGC(roots []Value, stack *Frame) {
	liveNodes := make(map[NodeID]bool)
	liveStrings := make(map[NameID]bool)
	liveFrames := make(map[*Frame]bool)
	for name := range k.natives {
		liveStrings[name] = true
	}
	for _, loc := range k.sourceAttr {
		liveStrings[loc.FileID] = true
	}
	for _, root := range k.activeRoots {
		k.markNode(root, liveNodes, liveStrings)
	}
	for _, root := range roots {
		k.markValue(root, liveNodes, liveStrings, liveFrames)
	}
	k.markFrame(stack, liveNodes, liveStrings, liveFrames)
	for nid, recipe := range k.byID {
		if nid.Pkg == 0 && !liveNodes[nid] {
			delete(k.byID, nid)
			delete(k.byHash, hashRecipe(recipe))
			delete(k.sourceAttr, nid)
			delete(k.switchTables, nid)
		}
	}
	for len(k.strs) > 0 {
		idx := NameID(len(k.strs) - 1)
		if liveStrings[idx] {
			break
		}
		delete(k.strIdx, k.strs[idx])
		k.strs = k.strs[:len(k.strs)-1]
	}
}

// category — the recipe row answers first. A composite sits at its
// category's level, so one interned over a trivial-level category is at
// level 1 too; only a NodeID with no row answers itself.
func (k *Kernel) category(n NodeID) NodeID {
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

// recipeAt — fold of category + children. The walker's hot path uses this
// to do ONE map lookup per composite step instead of two. For trivials,
// the caller already short-circuited on Level before calling.
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
		return boolInt(n.Inst != 0)
	case TrivNull:
		return Value{Kind: VNull}
	case TrivFloat32:
		return Value{Kind: VFloat, Float: float64(k.decodeFloat32(n.Inst))}
	case TrivFloat64:
		return Value{Kind: VFloat, Float: k.decodeFloat64(n.Inst)}
	}
	panic(fmt.Sprintf("trivialValue: unknown trivial type %d", n.Type))
}

// identID — the NameID this identifier resolves to. No string lookup, no
// comparison; the inst slot IS the NameID.
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

// nameStr — resolve a NameID back to its source-level string. Error
// messages and parse-time only; never in the walker's hot path.
func (k *Kernel) nameStr(id NameID) string { return k.strs[id] }

// resolveReadingLine — map a global line in the concatenated read buffer
// back to (file_name_id, line_within_that_file). Entries are in
// concatenation order; the last entry at or before the line owns it.
func (k *Kernel) resolveReadingLine(globalLine uint32) (NameID, uint32, bool) {
	var fileID NameID
	var local uint32
	found := false
	for _, part := range k.readingFiles {
		if part.StartLine <= globalLine {
			fileID = part.FileID
			local = globalLine - part.StartLine + 1
			found = true
		} else {
			break
		}
	}
	return fileID, local, found
}

// formFrame — one live Form call frame, stored RAW. Until 2026-08-17 the
// stack held rendered strings, and the closure-dispatch site built each one
// eagerly: a sourceAttr map lookup plus an fmt.Sprintf allocation on EVERY
// function call, paid so that an error which almost never happens could print
// a chain. On a BML compile that is hundreds of millions of dispatches; the
// json-codec-bml band's Go leg was sampled live at 49 minutes with
// walkInner→formFrameLabel→fmt.Sprintf standing in the hot tower. The only
// reader of this stack is formStackDisplay, an error path — so frames now
// carry the raw NameID/NodeID and rendering happens where reading happens.
type formFrame struct {
	name    NameID
	body    NodeID
	hasBody bool
}

// renderFormFrame — the display form of one frame, built only at read time.
// Closure frames render through formFrameLabel exactly as the eager path did,
// so the printed chain is byte-identical to what receipts already quote.
func (k *Kernel) renderFormFrame(f formFrame) string {
	if !f.hasBody {
		return k.nameStr(f.name)
	}
	return k.formFrameLabel(f.name, f.body)
}

// formStackDisplay — the live Form call chain, innermost first, capped.
func (k *Kernel) formStackDisplay(max int) string {
	if len(k.formStack) == 0 {
		return ""
	}
	total := len(k.formStack)
	var frames []string
	for i := total - 1; i >= 0 && len(frames) < max; i-- {
		frames = append(frames, k.renderFormFrame(k.formStack[i]))
	}
	out := strings.Join(frames, " < ")
	if total > max {
		out += fmt.Sprintf(" … (+%d more)", total-max)
	}
	return out
}

// formFrameLabel — a closure frame's display label: the function name,
// plus file:line:col when the body recipe carries source attribution.
func (k *Kernel) formFrameLabel(name NameID, body NodeID) string {
	label := k.nameStr(name)
	if loc, ok := k.sourceAttr[body]; ok && int(loc.FileID) < len(k.strs) {
		label = fmt.Sprintf("%s@%s:%d:%d", label, k.strs[loc.FileID], loc.Line, loc.Col)
	}
	return label
}

// ---------------------------------------------------------------------------
// Values — runtime tagged values
// ---------------------------------------------------------------------------

// isDictValue — a "__dict__"-tagged pair list, the row shape pg_query_rows answers.
func isDictValue(v Value) bool {
	return v.Kind == VList &&
		len(v.List) > 0 &&
		v.List[0].Kind == VStr &&
		v.List[0].Str == "__dict__"
}

func scanClassMatch(c byte, class int64) bool {
	alpha := (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
	digit := c >= '0' && c <= '9'
	switch class {
	case 0:
		return c == ' ' || c == '\t' || c == '\n' || c == '\r'
	case 1:
		return digit
	case 2:
		return alpha
	case 3:
		return alpha || digit || c == '_' || c == '-'
	case 4:
		return c != '"' && c != '\\'
	case 5:
		return c != '\n'
	case 6:
		return c >= 0x20 && c != '"' && c != '\\'
	}
	return false
}

// An integer is 63-bit two's complement, [-2^62, 2^62), on every kernel: fkwu
// tags its words, so its arithmetic wraps there and so does this kernel's.
const intLimit63 = float64(1 << 62)

func wrap63(n int64) int64 { return (n << 1) >> 1 }

// intLiteral reads an integer literal the way fkwu's reader does: the digits
// fold in a wrapping word, then the value wraps to 63 bits.
func intLiteral(s string) int64 {
	neg := len(s) > 0 && s[0] == '-'
	var u uint64
	for i := 0; i < len(s); i++ {
		if s[i] >= '0' && s[i] <= '9' {
			u = u*10 + uint64(s[i]-'0')
		}
	}
	n := int64(u)
	if neg {
		n = -n
	}
	return wrap63(n)
}

// decimalPrefixFloat — str_to_float's one grammar: leading whitespace skipped,
// then the longest decimal prefix (sign, digits, one dot, an exponent that has
// digits). No hex, no inf, no nan; text with no digits reads 0.0.
func decimalPrefixFloat(s string) float64 {
	i := 0
	for i < len(s) && (s[i] == ' ' || (s[i] >= '\t' && s[i] <= '\r')) {
		i++
	}
	start := i
	if i < len(s) && (s[i] == '+' || s[i] == '-') {
		i++
	}
	digits := 0
	for i < len(s) && s[i] >= '0' && s[i] <= '9' {
		i++
		digits++
	}
	if i < len(s) && s[i] == '.' {
		i++
		for i < len(s) && s[i] >= '0' && s[i] <= '9' {
			i++
			digits++
		}
	}
	if digits == 0 {
		return 0
	}
	end := i
	if i < len(s) && (s[i] == 'e' || s[i] == 'E') {
		j := i + 1
		if j < len(s) && (s[j] == '+' || s[j] == '-') {
			j++
		}
		if j < len(s) && s[j] >= '0' && s[j] <= '9' {
			for j < len(s) && s[j] >= '0' && s[j] <= '9' {
				j++
			}
			end = j
		}
	}
	f, _ := strconv.ParseFloat(s[start:end], 64)
	return f
}

// argStr — the string lane's checked accessor: any other kind stops, named.
func argStr(args []Value, i int) string {
	if i >= len(args) || args[i].Kind != VStr {
		got := "absent"
		if i < len(args) {
			got = valueKindName(args[i])
		}
		panic(fmt.Sprintf("argStr: arg %d: expected str, got %s", i, got))
	}
	return args[i].Str
}

func valueKindName(v Value) string {
	switch v.Kind {
	case VNull:
		return "null"
	case VInt:
		return "int"
	case VStr:
		return "string"
	case VList:
		return "list"
	case VClosure:
		return "closure"
	case VNodeID:
		return "node_id"
	case VFloat:
		return "float"
	case VRecord:
		return "record"
	default:
		return "unknown"
	}
}

// roundNdigitsDecimal — CPython `round(x, n)` for a finite double, n >= 0.
//
// CPython rounds the TRUE value of the double (a finite dyadic decimal) to n
// fractional digits half-to-even, then takes the nearest double. The naive
// f64 paths (floor(x*10^n+0.5)/10^n; banker's on the scaled f64) both diverge
// because the *10^n reintroduces representation error CPython's decimal path
// avoids. This rounds on the EXACT decimal expansion instead, obtained via
// strconv.FormatFloat('f', 1074): a finite double m*2^e2 (e2<0) equals
// m*5^(-e2)/10^(-e2), a decimal with exactly -e2 (<= 1074) fractional digits,
// so 1074 places is the exact terminating expansion for any double — no
// domain assumption, no pre-rounding at the round position. Verified bit-for-
// bit against CPython on 6.6M cases with ZERO divergences. Sibling-parity
// with the Rust kernel (format!("{:.1074}")) and TS kernel (BigInt
// mantissa*5^k, since JS toFixed caps at 100 places).
func roundNdigitsDecimal(x float64, n int64) float64 {
	if math.IsNaN(x) || math.IsInf(x, 0) {
		return x
	}
	neg := math.Signbit(x)
	ax := math.Abs(x)
	// Exact fixed-point decimal of |x|; 1074 fractional places is the full
	// terminating expansion for any double, so no rounding occurs at the tail.
	s := strconv.FormatFloat(ax, 'f', 1074, 64)
	dot := strings.IndexByte(s, '.')
	ipart := s[:dot]
	fpart := s[dot+1:]
	digits := []byte(ipart + fpart)
	point := int64(len(ipart))
	keep := point + n
	if keep < 0 {
		if neg {
			return math.Copysign(0, -1)
		}
		return 0
	}
	keepI := int(keep)
	if len(digits) < keepI {
		pad := make([]byte, keepI-len(digits))
		for i := range pad {
			pad[i] = '0'
		}
		digits = append(digits, pad...)
	}
	keptSlice := digits[:keepI]
	rest := digits[keepI:]
	var kept string
	if len(keptSlice) == 0 {
		kept = "0"
	} else {
		kept = string(keptSlice)
	}
	roundUp := false
	if len(rest) > 0 {
		first := rest[0]
		if first > '5' {
			roundUp = true
		} else if first < '5' {
			roundUp = false
		} else {
			tailNonzero := false
			for _, d := range rest[1:] {
				if d != '0' {
					tailNonzero = true
					break
				}
			}
			if tailNonzero {
				roundUp = true
			} else {
				lastKept := kept[len(kept)-1]
				roundUp = (lastKept-'0')%2 == 1
			}
		}
	}
	if roundUp {
		kept = addOneDecimal(kept)
	}
	dec := composeScaledDecimal(kept, int(n), neg)
	out, err := strconv.ParseFloat(dec, 64)
	if err != nil {
		out = 0
	}
	if out == 0 && neg {
		return math.Copysign(0, -1)
	}
	return out
}

// addOneDecimal — increment a non-negative decimal digit string by 1,
// propagating carry (may grow by one leading digit).
func addOneDecimal(s string) string {
	b := []byte(s)
	i := len(b)
	for {
		if i == 0 {
			b = append([]byte{'1'}, b...)
			break
		}
		i--
		if b[i] == '9' {
			b[i] = '0'
		} else {
			b[i]++
			break
		}
	}
	return string(b)
}

// composeScaledDecimal — render integer string `kept` scaled by 10^-n as a
// decimal literal with the given sign. n >= 0.
func composeScaledDecimal(kept string, n int, neg bool) string {
	var body string
	if n == 0 {
		body = kept
	} else {
		si := kept
		if len(si) <= n {
			pad := n - len(si) + 1
			si = strings.Repeat("0", pad) + si
		}
		split := len(si) - n
		body = si[:split] + "." + si[split:]
	}
	if neg {
		return "-" + body
	}
	return body
}

type choiceFailSignal struct{}
type choiceStopSignal struct{}

// methodKey — (blueprint, method-name) key for the blueprint method table.
type methodKey struct {
	blueprint NodeID
	name      NameID
}

// ---------------------------------------------------------------------------
// Native functions — what Form-on-top reaches for at the leaves
// ---------------------------------------------------------------------------

type NativeFn func(k *Kernel, args []Value) Value

// registerNative — central registration point. The string name is
// interned once into a NameID; runtime dispatch is u32-keyed. Each
// native carries the Form category it expresses (Blueprint attribution).
// The names are the rows of form/form-stdlib/primitive-registry.fk.
func (k *Kernel) registerNative(name string, category NodeID, fn NativeFn) {
	id := k.internName(name)
	k.natives[id] = NativeEntry{Name: id, Category: category, Fn: fn}
}

func (k *Kernel) registerNatives() {
	// Blueprint attribution discipline (mirrors Rust kernel):
	//   catCall      — invoke external effect (I/O, tool)
	//   catAccess    — read property / field
	//   catMethod    — transform on a cell-like value
	//   catCompare   — equality / ordering
	//   catListNat   — construct/destructure a List
	//   catWitness   — substrate self-attestation (intern, walk, lookup)
	//   catUndefined — honest "no Form category settled yet"

	// print answers 0, as fkwu's tag 239 does.
	k.registerNative("print", catCall(), func(_ *Kernel, args []Value) Value {
		for i, a := range args {
			if i > 0 {
				fmt.Print(" ")
			}
			fmt.Print(a.String())
		}
		fmt.Println()
		return Value{Kind: VInt, Int: 0}
	})
	// String ops
	k.registerNative("str_len", catAccess(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: int64(len(argStr(args, 0)))}
	})
	// substring — BYTES, CLAMPED, NEVER DIES. The one meaning, four ways,
	// laid 2026-09-07 (substring-one-meaning-band.fk, drift gate).
	//
	//   substring(s, start, end) is the bytes of s from max(start,0) up to
	//   min(end, len(s)); empty when that range is empty or reversed; empty
	//   when s is not a string. It refuses nothing and it FLOORS nothing.
	//
	// WHY BYTES. The body indexes bytes everywhere — str_byte_at is the
	// narrow waist, the frame readers, the row walkers, str_find, split-on
	// and trim all compute byte offsets. A cut that floors its ends to
	// character starts returns a SHORTER, SHIFTED window for those same
	// indices and says nothing about it, so every Persian, Chinese, Japanese
	// and Hebrew row in form-stdlib/locale-rows was silently re-cut here.
	// Flooring does keep adjacency among floored indices; what it cannot keep
	// is the content.
	//
	// WHY CLAMPED. fkwu and the core.fk recipe both clamped from birth. A
	// panic here turned an out-of-range index — the ordinary end of a scan —
	// into a dead process on three arms and an empty string on the fourth.
	// Go's string holds arbitrary bytes, so this arm answers every cut
	// exactly, like fkwu. Rust's str and the TS kernel's UTF-16 string cannot
	// hold a severed multi-byte character; those two arms answer the axiom-1
	// absence there rather than a different window. What all four arms hold
	// together is that NO ARM EVER ANSWERS A DIFFERENT NON-EMPTY WINDOW.
	k.registerNative("substring", catAccess(), func(_ *Kernel, args []Value) Value {
		if len(args) < 3 || args[0].Kind != VStr {
			return Value{Kind: VStr, Str: ""}
		}
		s := args[0].Str
		a := args[1].AsInt()
		b := args[2].AsInt()
		if a < 0 {
			a = 0
		}
		if b > int64(len(s)) {
			b = int64(len(s))
		}
		if b <= a {
			return Value{Kind: VStr, Str: ""}
		}
		return Value{Kind: VStr, Str: s[a:b]}
	})
	k.registerNative("str_concat", catMethod(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VStr, Str: argStr(args, 0) + argStr(args, 1)}
	})
	// host-io builtins: the kernel's host-driver layer (host-kernel.form). These give a
	// Form recipe the host effects the agent runner needs — run a process, read/write a
	// file — so the runner is a Form recipe the kernel runs, NOT hand-written emitted C.
	// Output is non-deterministic (host-io standing wall), so these are receipt-validated,
	// not part of the four-way output-identity floor. The kernel already shells out (JIT
	// go-build, plugin load), so this exposes an existing capability, not a new class.
	k.registerNative("host-exec", catMethod(), func(_ *Kernel, args []Value) Value {
		cmd := exec.Command("sh", "-c", argStr(args, 0))
		// A non-empty second arg = the process's stdin, piped in-memory: no temp file,
		// no writable filesystem. The bytes go kernel -> subprocess directly, so a
		// question never spills to disk and a host with no writable /tmp (Android)
		// still escalates. An empty or absent one leaves stdin inherited, as fkwu's
		// fk_host_exec leaves it.
		if len(args) > 1 && argStr(args, 1) != "" {
			cmd.Stdin = strings.NewReader(argStr(args, 1))
		} else {
			cmd.Stdin = os.Stdin
		}
		// The answer is the child's stdout; its stderr passes through to this
		// kernel's own, as on fkwu (a merged stream answered words fkwu never
		// hands back).
		cmd.Stderr = os.Stderr
		out, err := cmd.Output()
		// Launch failure (fork starvation — sh never ran) answers the axiom-1
		// nothing (the same null head-of-empty carries), never "": "" means the
		// command RAN and spoke zero bytes. An *exec.ExitError is a process that
		// DID run and exited nonzero — its output still crosses. Mirrors
		// fk_host_exec in runtime/fkwu-uni.c (witnessed 2026-08-27, PR #542).
		if err != nil {
			if _, ran := err.(*exec.ExitError); !ran {
				return Value{Kind: VNull}
			}
		}
		return Value{Kind: VStr, Str: string(out)}
	})
	k.registerNative("form_error", catWitness(), func(_ *Kernel, args []Value) Value {
		panic(argStr(args, 0))
	})
	// --- struct/object primitive (BML reference, rung 2) -------------------
	// A Record is the kernel's first MUTABLE value: a struct/object with
	// identity. Every language's class/struct compiles onto these natives.
	// Blueprint NodeID tags the type; fields are a name→value map.
	//
	// record_new — (record_new blueprint k1 v1 k2 v2 ...) → Record.
	// A blueprint of 0 builds a record with no blueprint: its fields work as
	// on any record, record_blueprint reads back 0, and no method dispatches
	// on it. fkwu keeps the blueprint operand verbatim, and 0 is the body's
	// most common record shape (a plain field map).
	k.registerNative("record_new", catMethod(), func(k *Kernel, args []Value) Value {
		recordConstructions.Add(1)
		rec := &Record{}
		if args[0].Kind == VInt && args[0].Int == 0 {
			rec.NoBlueprint = true
		} else if args[0].Kind == VRecord {
			rec.BlueprintRec = args[0].Rec
		} else {
			rec.Blueprint = args[0].AsNid()
		}
		i := 1
		for i+1 < len(args) {
			rec.Set(k.internName(args[i].Str), args[i+1])
			i += 2
		}
		return Value{Kind: VRecord, Rec: rec}
	})
	// record_get — (record_get rec "field") → value, or 0 when the record
	// carries no such field: fkwu's answer, which the body reads as eq(v, 0).
	// record_get and record_set on a value that is not a record stop by name, as
	// Rust and TS stop (fkwu: "only a record has fields"); record_has answers 0.
	k.registerNative("record_get", catAccess(), func(k *Kernel, args []Value) Value {
		if args[0].Kind != VRecord {
			panic("record_get: not a record -- only a record has fields; ask record? first")
		}
		v, ok := args[0].Rec.Get(k.internName(argStr(args, 1)))
		if !ok {
			return Value{Kind: VInt, Int: 0}
		}
		return v
	})
	// record_set — (record_set rec "field" value) → the record (mutated in
	// place; shared identity means all holders see it). BML's `self.x = v`.
	k.registerNative("record_set", catMethod(), func(k *Kernel, args []Value) Value {
		if args[0].Kind != VRecord {
			panic("record_set: not a record -- only a record has fields; ask record? first")
		}
		args[0].Rec.Set(k.internName(argStr(args, 1)), args[2])
		return args[0]
	})
	// record_has — (record_has rec "field") → bool; a value that is not a record
	// has no field, so it answers 0, as Rust and TS answer.
	k.registerNative("record_has", catAccess(), func(k *Kernel, args []Value) Value {
		if args[0].Kind != VRecord {
			return boolInt(false)
		}
		_, ok := args[0].Rec.Get(k.internName(argStr(args, 1)))
		return boolInt(ok)
	})
	// record_blueprint — (record_blueprint rec) → the blueprint NodeID, or 0
	// for a record built without one.
	k.registerNative("record_blueprint", catAccess(), func(_ *Kernel, args []Value) Value {
		if args[0].Rec.BlueprintRec != nil {
			return Value{Kind: VRecord, Rec: args[0].Rec.BlueprintRec}
		}
		if args[0].Rec.NoBlueprint {
			return Value{Kind: VInt, Int: 0}
		}
		return Value{Kind: VNodeID, Nid: args[0].Rec.Blueprint}
	})
	// record_keys — (record_keys rec) → list of field-name strings, in
	// insertion order. Lets Form enumerate a record used as a hash map (e.g.
	// the keydir of cell-log-store.fk for compaction).
	k.registerNative("record_keys", catAccess(), func(k *Kernel, args []Value) Value {
		fields := args[0].Rec.Fields
		out := make([]Value, len(fields))
		for i, f := range fields {
			out[i] = Value{Kind: VStr, Str: k.strs[f.Name]}
		}
		return Value{Kind: VList, List: out}
	})
	// record? — (record? v) → bool type predicate.
	k.registerNative("record?", catAccess(), func(_ *Kernel, args []Value) Value {
		return boolInt(args[0].Kind == VRecord)
	})
	// --- methods on the blueprint (BML/NUMS reference, rung 2b) ----------
	// Methods live on the blueprint/type, shared by all records of that type,
	// name-dispatched. The keystone that makes a Record a real object.
	//
	// method_define — (method_define blueprint "name" closure) → blueprint.
	k.registerNative("method_define", catMethod(), func(k *Kernel, args []Value) Value {
		if args[2].Kind != VClosure {
			panic("method_define: third arg must be a closure")
		}
		k.methods[methodKey{args[0].AsNid(), k.internName(argStr(args, 1))}] = args[2].Cl
		return args[0]
	})
	// method_has — (method_has record-or-blueprint "name") → bool.
	k.registerNative("method_has", catAccess(), func(k *Kernel, args []Value) Value {
		var bp NodeID
		switch args[0].Kind {
		case VRecord:
			if args[0].Rec.NoBlueprint {
				return boolInt(false)
			}
			bp = args[0].Rec.Blueprint
		case VNodeID:
			bp = args[0].Nid
		default:
			return boolInt(false)
		}
		_, ok := k.methods[methodKey{bp, k.internName(argStr(args, 1))}]
		return boolInt(ok)
	})
	// method_invoke — (method_invoke record "name" arg1 arg2 ...) → value.
	// Dispatches by the record's blueprint; the method's FIRST param is the
	// receiver (Python `self` convention), remaining params bind to call args.
	k.registerNative("method_invoke", catMethod(), func(k *Kernel, args []Value) Value {
		if args[0].Kind != VRecord {
			panic("method_invoke: first arg must be a record")
		}
		rec := args[0].Rec
		if rec.NoBlueprint {
			panic(fmt.Sprintf("method_invoke: no method '%s' on a record with no blueprint (record_new 0)", args[1].Str))
		}
		key := methodKey{rec.Blueprint, k.internName(argStr(args, 1))}
		cl, ok := k.methods[key]
		if !ok {
			panic(fmt.Sprintf("method_invoke: no method '%s' on blueprint @%d.%d.%d.%d",
				args[1].Str, rec.Blueprint.Pkg, rec.Blueprint.Level,
				rec.Blueprint.Type, rec.Blueprint.Inst))
		}
		callArgs := args[2:]
		if len(cl.Params) == 0 {
			panic(fmt.Sprintf("method '%s' must declare a receiver param (self)", args[1].Str))
		}
		if len(callArgs) != len(cl.Params)-1 {
			panic(fmt.Sprintf("method '%s' wants %d args, got %d",
				args[1].Str, len(cl.Params)-1, len(callArgs)))
		}
		call := NewCallFrame(cl.Env, len(cl.Params))
		call.Bind(cl.Params[0], args[0]) // receiver
		for i, p := range cl.Params[1:] {
			call.Bind(p, callArgs[i])
		}
		return k.walk(cl.Body, call)
	})
	// str_find — Go-level substring search starting at index `from`.
	// Signature: (str_find s needle from) → int (index or -1). The whole
	// search runs in this Go loop (uses strings.Index after slicing); no
	// Form closure dispatch per byte, no Form recursion. This is what
	// `tokenizeSexp` does internally — exposed for Form scanners that
	// would otherwise blow the walker stack with per-character recursion.
	// BYTES, CLAMPED, NEVER DIES — the one meaning, held by all four arms since
	// 2026-09-08 (form-stdlib/tests/str-find-one-meaning-band.fk, 8191 four ways,
	// a drift gate; core.fk's fstr-find is the body's own statement of it).
	// Until that day this snapped `from` UP to the nearest char boundary. For a
	// needle that is well-formed UTF-8 that snap is a no-op — no valid needle can
	// begin on a continuation byte, so skipping continuation bytes can never skip
	// a match. For a needle that IS a byte fragment of a character, which a Go
	// string and fkwu's byte buffer can both hold, it skipped real matches: in
	// "aΩΩb" the second 0xA9 was found at 4 where the byte answer is 2. The
	// comment it carried ("so a find-next loop stepping +1 advances past a
	// multibyte char instead of re-finding it forever") described a hazard that
	// cannot occur: +1 from a match lands on a continuation byte, where no valid
	// needle matches, so the scan advances on its own.
	k.registerNative("str_find", catAccess(), func(_ *Kernel, args []Value) Value {
		s := argStr(args, 0)
		needle := argStr(args, 1)
		from := int(args[2].AsInt())
		if from < 0 {
			from = 0
		}
		if from > len(s) {
			return Value{Kind: VInt, Int: -1}
		}
		idx := strings.Index(s[from:], needle)
		if idx < 0 {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: int64(from + idx)}
	})
	// scan_run s from class — the end index (exclusive) of the run of bytes from
	// max(from,0) that match the class, fkwu's fk_scan_run: 0 whitespace (space tab
	// lf cr), 1 digit, 2 ascii alpha, 3 identifier (alpha digit _ -), 4 not '"' or
	// '\\', 5 not lf, 6 json-safe (>= 0x20, not '"' or '\\'). A non-string or an
	// unknown class matches no byte.
	k.registerNative("scan_run", catAccess(), func(_ *Kernel, args []Value) Value {
		from := args[1].AsInt()
		if from < 0 {
			from = 0
		}
		if args[0].Kind != VStr {
			return Value{Kind: VInt, Int: from}
		}
		s, class, end := args[0].Str, args[2].AsInt(), from
		for end < int64(len(s)) && scanClassMatch(s[end], class) {
			end++
		}
		return Value{Kind: VInt, Int: end}
	})
	// str_eq OBSERVES the axiom-1 absence instead of refusing it, mirroring the fkwu
	// arm exactly (probed 2026-09-04: nothing equals nothing, and equals neither ""
	// nor any other string, so the emptymask distinction between never-was and empty
	// survives the comparison). A comparison asks a question ABOUT two values; a
	// length MEASURES one, which is why str_len and str_byte_at still refuse an
	// absence out loud. A walk that meets a file which left between the listing
	// and the read answers here as fkwu does ("not a model"), so this witness
	// stays in step with the primary kernel.
	k.registerNative("str_eq", catCompare(RCompareEq), func(_ *Kernel, args []Value) Value {
		if len(args) >= 2 && (args[0].Kind == VNull || args[1].Kind == VNull) {
			return boolInt(args[0].Kind == VNull && args[1].Kind == VNull)
		}
		return boolInt(argStr(args, 0) == argStr(args, 1))
	})
	k.registerNative("value_str", catMethod(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VStr, Str: formValueString(args[0])}
	})
	k.registerNative("value_kind", catWitness(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VStr, Str: valueKindName(args[0])}
	})
	// nothing / nothing? — the axiom-1 third value and the one question that sees it,
	// native as on fkwu (tags 137/138): never-was is neither 0 nor empty.
	k.registerNative("nothing", catWitness(), func(_ *Kernel, _ []Value) Value {
		return Value{Kind: VNull}
	})
	k.registerNative("nothing?", catWitness(), func(_ *Kernel, args []Value) Value {
		if len(args) > 0 && args[0].Kind == VNull {
			return Value{Kind: VInt, Int: 1}
		}
		return Value{Kind: VInt, Int: 0}
	})
	k.registerNative("str_to_float", catMethod(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VFloat, Float: decimalPrefixFloat(argStr(args, 0))}
	})
	k.registerNative("float_to_int", catMethod(), func(_ *Kernel, args []Value) Value {
		switch args[0].Kind {
		case VFloat:
			f := args[0].Float
			if f != f || f < -intLimit63 || f >= intLimit63 {
				panic(fmt.Sprintf("float_to_int: %s has no integer -- an int is 63-bit", formatFloatJS(f)))
			}
			return Value{Kind: VInt, Int: int64(f)}
		case VInt:
			return args[0]
		}
		panic("float_to_int: only a number reads as an integer -- ask value_kind first")
	})
	// str_byte_at: the i-th raw byte of the string (0-255); an index outside it answers -1.
	k.registerNative("str_byte_at", catAccess(), func(_ *Kernel, args []Value) Value {
		s := argStr(args, 0)
		i := args[1].AsInt()
		if i < 0 || i >= int64(len(s)) {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: int64(s[i])}
	})
	k.registerNative("byte_to_str", catAccess(), func(_ *Kernel, args []Value) Value {
		if args[0].AsInt() < 0 || args[0].AsInt() > 255 {
			return Value{Kind: VStr, Str: ""}
		}
		// one raw byte, as on fkwu: the exact dual of str_byte_at, never a code point
		return Value{Kind: VStr, Str: string([]byte{byte(args[0].AsInt())})}
	})
	// input_byte — byte i of the staged input, 0 outside it, as fkwu reads
	// its staged buffer. This kernel stages no input, so every byte is 0.
	k.registerNative("input_byte", catAccess(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: 0}
	})
	// List ops
	k.registerNative("list", catListNat(), func(_ *Kernel, args []Value) Value {
		out := make([]Value, len(args))
		copy(out, args)
		return Value{Kind: VList, List: out}
	})
	// cons onto a word that is not a list makes a pair, as fkwu's cons does: the
	// pair keeps its tail word, list readers (len, nth, print, value_str) end at
	// it and read [h], and tail, eq and value_eq see the word. The body leans on
	// it: form-eval's env is a cons chain ended by 0, the stage bus carries
	// (cons stage key).
	k.registerNative("cons", catListNat(), func(_ *Kernel, args []Value) Value {
		// nothing is not a list: consing onto it is a stop, as on fkwu
		if args[1].Kind == VNull {
			panic("cons: nothing is not a list -- ask nothing? before consing")
		}
		if args[1].Kind != VList {
			word := args[1]
			return Value{Kind: VList, List: []Value{args[0]}, Tail: &word}
		}
		out := make([]Value, 0, len(args[1].List)+1)
		out = append(out, args[0])
		out = append(out, args[1].List...)
		return Value{Kind: VList, List: out, Tail: args[1].Tail}
	})
	k.registerNative("head", catListNat(), func(_ *Kernel, args []Value) Value {
		if len(args[0].List) == 0 {
			return Value{Kind: VNull}
		}
		return args[0].List[0]
	})
	// A receiver that is not a list has no tail and answers null, as head and nth answer.
	// The tail of a pair's last cell is the pair's tail word.
	k.registerNative("tail", catListNat(), func(_ *Kernel, args []Value) Value {
		if args[0].Kind != VList {
			return Value{Kind: VNull}
		}
		if len(args[0].List) == 0 {
			return Value{Kind: VList, List: []Value{}}
		}
		if len(args[0].List) == 1 && args[0].Tail != nil {
			return *args[0].Tail
		}
		return Value{Kind: VList, List: args[0].List[1:], Tail: args[0].Tail}
	})
	// len counts cells: a "__dict__" row's marker is a cell like any other.
	k.registerNative("len", catAccess(), func(_ *Kernel, args []Value) Value {
		switch args[0].Kind {
		case VList:
			return Value{Kind: VInt, Int: int64(len(args[0].List))}
		case VStr:
			return Value{Kind: VInt, Int: int64(len(args[0].Str))}
		case VNull:
			// nothing is not an empty collection: its length is a stop, as on fkwu
			panic("len: nothing has no length -- ask nothing? before measuring")
		}
		return Value{Kind: VInt, Int: 0}
	})
	k.registerNative("nth", catAccess(), func(_ *Kernel, args []Value) Value {
		if args[0].Kind != VList {
			return Value{Kind: VNull}
		}
		i := args[1].AsInt()
		if i < 0 || int(i) >= len(args[0].List) {
			return Value{Kind: VNull}
		}
		return args[0].List[i]
	})
	k.registerNative("empty", catListNat(), func(_ *Kernel, _ []Value) Value {
		return Value{Kind: VList, List: []Value{}}
	})
	// _get target key — fkwu's tag 106 (fk_get_value): a "__dict__" row answers the value
	// under a string key; a list answers the element at an int index, a negative index
	// reading the head; every miss and every other target answers 0.
	k.registerNative("_get", catAccess(), func(_ *Kernel, args []Value) Value {
		target, key := args[0], args[1]
		if isDictValue(target) {
			xs := target.List
			for i := 1; i+1 < len(xs); i += 2 {
				if key.Kind == VStr && xs[i].Kind == VStr && xs[i].Str == key.Str {
					return xs[i+1]
				}
			}
			return Value{Kind: VInt, Int: 0}
		}
		if target.Kind == VList && key.Kind == VInt {
			i := key.Int
			if i < 0 {
				i = 0
			}
			if i < int64(len(target.List)) {
				return target.List[i]
			}
		}
		return Value{Kind: VInt, Int: 0}
	})
	// The float natives; math.Sqrt is IEEE fsqrt, correctly rounded as on fkwu.
	k.registerNative("math_sqrt", catMethod(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VFloat, Float: math.Sqrt(args[0].AsFloat())}
	})
	k.registerNative("math_pi", catMethod(), func(_ *Kernel, _ []Value) Value {
		return Value{Kind: VFloat, Float: math.Pi}
	})
	k.registerNative("math_pow", catMethod(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VFloat, Float: math.Pow(args[0].AsFloat(), args[1].AsFloat())}
	})
	k.registerNative("math_log", catMethod(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VFloat, Float: math.Log(args[0].AsFloat())}
	})
	k.registerNative("math_exp", catMethod(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VFloat, Float: math.Exp(args[0].AsFloat())}
	})
	// round_ndigits(x, n) — CPython round(x, n) exactly: the double's exact decimal
	// value rounded half-to-even at n places (roundNdigitsDecimal). A negative n
	// stops by name, as on fkwu: ndigits counts places.
	k.registerNative("round_ndigits", catMethod(), func(_ *Kernel, args []Value) Value {
		n := args[1].AsInt()
		if n < 0 {
			panic("round_ndigits: ndigits is a count of places, an int at or above 0")
		}
		return Value{Kind: VFloat, Float: roundNdigitsDecimal(args[0].AsFloat(), n)}
	})

	// ── Float construction + introspection — sibling-parity with the
	// TS kernel's make_float32/make_float64 + f32/f64 transmute casts.
	// The Rust kernel doesn't currently expose these as natives (it
	// only parses float literals from .fk source); Go matches TS so
	// Form code that constructs floats explicitly has a verb to call.
	//
	// make_float64 — intern a float-valued substrate trivial. Takes an
	// int or float arg; returns a NodeID. Sibling to make_int* /
	// make_uint* — used when Form code wants to hold the substrate
	// identity rather than the value.
	k.registerNative("make_float32", catWitness(), func(k *Kernel, args []Value) Value {
		return Value{Kind: VNodeID, Nid: k.internTrivialFloat32(float32(args[0].AsFloat()))}
	})
	// float_leaf mode x — fkwu's multiplexed door (tag 201); its stage-bus modes have no
	// sibling carrier, so every mode answers nothing here.
	k.registerNative("float_leaf", catWitness(), func(_ *Kernel, _ []Value) Value {
		return Value{Kind: VNull}
	})
	k.registerNative("make_float64", catWitness(), func(k *Kernel, args []Value) Value {
		return Value{Kind: VNodeID, Nid: k.internTrivialFloat64(args[0].AsFloat())}
	})

	// File I/O
	readFileTextNative := func(_ *Kernel, args []Value) Value {
		b, err := os.ReadFile(resolveKernelHostPath(argStr(args, 0)))
		if err != nil {
			return Value{Kind: VNull}
		}
		return Value{Kind: VStr, Str: string(b)}
	}
	k.registerNative("host_file_read_text", catCall(), readFileTextNative)
	k.registerNative("read_file", catCall(), readFileTextNative)
	// Byte-level host file read — returns a list of ints (0-255), one per byte.
	k.registerNative("read_file_bytes", catCall(), func(_ *Kernel, args []Value) Value {
		b, err := os.ReadFile(resolveKernelHostPath(argStr(args, 0)))
		if err != nil {
			return Value{Kind: VNull}
		}
		out := make([]Value, len(b))
		for i, by := range b {
			out[i] = Value{Kind: VInt, Int: int64(by)}
		}
		return Value{Kind: VList, List: out}
	})
	// source_inventory(root, suffix, skip-dir-names) — generic source
	// inventory primitive. Returns rows of [relative-path, line-count].
	// Form owns classification and aggregation; the kernel only exposes
	// filesystem walking and text line counts as primitive observation.
	k.registerNative("source_inventory", catCall(), func(_ *Kernel, args []Value) Value {
		root := resolveKernelHostPath(argStr(args, 0))
		suffix := argStr(args, 1)
		skip := sourceInventorySkipSet(args[2])
		rootAbs, err := filepath.Abs(root)
		if err != nil {
			return Value{Kind: VNull}
		}
		rows := []Value{}
		err = filepath.WalkDir(rootAbs, func(path string, d os.DirEntry, walkErr error) error {
			if walkErr != nil {
				return walkErr
			}
			name := d.Name()
			if d.IsDir() {
				if skip[name] {
					return filepath.SkipDir
				}
				return nil
			}
			if suffix != "" && !strings.HasSuffix(name, suffix) {
				return nil
			}
			rel, err := filepath.Rel(rootAbs, path)
			if err != nil {
				return err
			}
			rows = append(rows, sourceInventoryRow(filepath.ToSlash(rel), countTextLines(path)))
			return nil
		})
		if err != nil {
			return Value{Kind: VNull}
		}
		return Value{Kind: VList, List: rows}
	})
	// random_bytes(n) — open the doorway. Reads n bytes from
	// /dev/urandom every call. Different per invocation, per kernel
	// process. lc-divergence-is-the-doorway: this native intentionally
	// violates sibling parity when invoked — the divergence is the
	// substrate's signal of live field-touch.
	k.registerNative("random_bytes", catCall(), func(_ *Kernel, args []Value) Value {
		n := int(args[0].AsInt())
		if n <= 0 {
			return Value{Kind: VList, List: []Value{}}
		}
		f, err := os.Open("/dev/urandom")
		if err != nil {
			return Value{Kind: VNull}
		}
		defer f.Close()
		buf := make([]byte, n)
		if _, err := io.ReadFull(f, buf); err != nil {
			return Value{Kind: VNull}
		}
		out := make([]Value, n)
		for i, by := range buf {
			out[i] = Value{Kind: VInt, Int: int64(by)}
		}
		return Value{Kind: VList, List: out}
	})
	// ---- bitwise primitives -----------------------------------
	// True kernel primitives — cannot be expressed in pure Form
	// without exponential cost. band/bor/bxor combine the whole int64
	// word; the _u32 doors narrow to a 32-bit word so SHA-256-style
	// recipes compose round functions over machine words.
	k.registerNative("band", catMethod(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: args[0].AsInt() & args[1].AsInt()}
	})
	k.registerNative("bor", catMethod(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: args[0].AsInt() | args[1].AsInt()}
	})
	k.registerNative("bxor", catMethod(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: args[0].AsInt() ^ args[1].AsInt()}
	})
	k.registerNative("bnot_u32", catMethod(), func(_ *Kernel, args []Value) Value {
		a := uint32(args[0].AsInt())
		return Value{Kind: VInt, Int: int64(^a)}
	})
	k.registerNative("shl_u32", catMethod(), func(_ *Kernel, args []Value) Value {
		a := uint32(args[0].AsInt())
		n := uint32(args[1].AsInt()) & 31
		return Value{Kind: VInt, Int: int64(a << n)}
	})
	k.registerNative("shr_u32", catMethod(), func(_ *Kernel, args []Value) Value {
		a := uint32(args[0].AsInt())
		n := uint32(args[1].AsInt()) & 31
		return Value{Kind: VInt, Int: int64(a >> n)}
	})
	k.registerNative("rotr_u32", catMethod(), func(_ *Kernel, args []Value) Value {
		a := uint32(args[0].AsInt())
		n := uint32(args[1].AsInt()) & 31
		return Value{Kind: VInt, Int: int64((a >> n) | (a << (32 - n)))}
	})
	// add_u32: modular 32-bit addition — SHA-256's round constants
	// and message schedule both require this discipline.
	k.registerNative("add_u32", catMethod(), func(_ *Kernel, args []Value) Value {
		a := uint32(args[0].AsInt())
		b := uint32(args[1].AsInt())
		return Value{Kind: VInt, Int: int64(a + b)}
	})
	// recipe_to_bytes nid → list-of-bytes (or null on error).
	//   Serializes a Recipe subtree to the .fkb wire format as a byte
	//   list — usable over any byte channel without a file detour.
	k.registerNative("recipe_to_bytes", catWitness(), func(k *Kernel, args []Value) Value {
		bytes := serializeArtifact(k, args[0].AsNid())
		out := make([]Value, len(bytes))
		for i, b := range bytes {
			out[i] = Value{Kind: VInt, Int: int64(b)}
		}
		return Value{Kind: VList, List: out}
	})
	// bytes_to_recipe bytes-list → nid (or null on parse error).
	k.registerNative("bytes_to_recipe", catWitness(), func(k *Kernel, args []Value) Value {
		if args[0].Kind != VList {
			return Value{Kind: VNull}
		}
		bytes := make([]byte, len(args[0].List))
		for i, v := range args[0].List {
			bytes[i] = byte(v.Int)
		}
		root, err := deserializeArtifact(k, bytes)
		if err != nil {
			return Value{Kind: VNull}
		}
		return Value{Kind: VNodeID, Nid: root}
	})
	// jit_leaf_inram (image, arg) — run a Form-emitted arm64 leaf image
	// (lo-compile-fn's output) in-process via MAP_JIT. Form owns emission;
	// this carrier is available on darwin/arm64+cgo.
	k.registerInRAMJIT()
	// write_form_binary — a recipe to a .fkb artifact on disk (string table +
	// tree), so it crosses kernel invocations; read_form_binary reads it back.
	k.registerNative("write_form_binary", catCall(), func(k *Kernel, args []Value) Value {
		path := argStr(args, 0)
		nid := args[1].AsNid()
		bytes := serializeArtifact(k, nid)
		if err := os.WriteFile(path, bytes, 0644); err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: int64(len(bytes))}
	})
	k.registerNative("read_form_binary", catCall(), func(k *Kernel, args []Value) Value {
		b, err := os.ReadFile(resolveKernelHostPath(argStr(args, 0)))
		if err != nil {
			return Value{Kind: VNull}
		}
		root, err := deserializeArtifact(k, b)
		if err != nil {
			return Value{Kind: VNull}
		}
		return Value{Kind: VNodeID, Nid: root}
	})
	fileSizeNative := func(_ *Kernel, args []Value) Value {
		info, err := os.Stat(resolveKernelHostPath(argStr(args, 0)))
		if err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: info.Size()}
	}
	k.registerNative("host_file_size", catCall(), fileSizeNative)
	k.registerNative("file_size", catCall(), fileSizeNative)
	// file_mtime — modification time in unix seconds; -1 if file missing.
	// Used by Form-side cache layers (form-stdlib/cache.fk) to decide
	// when a .fkb projection of a source file is stale. Generic: any
	// "regenerate cache when source newer" pattern can compose this.
	fileMtimeNative := func(_ *Kernel, args []Value) Value {
		info, err := os.Stat(resolveKernelHostPath(argStr(args, 0)))
		if err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: info.ModTime().Unix()}
	}
	k.registerNative("host_file_mtime", catCall(), fileMtimeNative)
	k.registerNative("file_mtime", catCall(), fileMtimeNative)
	// read_file_slice path off len — the slice's own bytes; "" when len asks for
	// none, and nothing when no byte was measured (a missing file, a negative
	// offset, a read error), in the order fkwu, Rust and TS read them.
	readFileSliceNative := func(_ *Kernel, args []Value) Value {
		offset := args[1].AsInt()
		length := args[2].AsInt()
		if length <= 0 {
			return Value{Kind: VStr, Str: ""}
		}
		if offset < 0 {
			return Value{Kind: VNull}
		}
		f, err := os.Open(resolveKernelHostPath(argStr(args, 0)))
		if err != nil {
			// a file that never was answers nothing, matching read_file
			return Value{Kind: VNull}
		}
		defer f.Close()
		buf := make([]byte, length)
		n, rerr := f.ReadAt(buf, offset)
		// io.EOF with a short count is an honest slice past the end; any other
		// error measured no trustworthy byte — nothing, never a silent
		// truncation handed back as the slice (2026-08-27).
		if rerr != nil && rerr != io.EOF {
			return Value{Kind: VNull}
		}
		return Value{Kind: VStr, Str: string(buf[:n])}
	}
	k.registerNative("host_file_read_slice", catCall(), readFileSliceNative)
	k.registerNative("read_file_slice", catCall(), readFileSliceNative)

	// --- Filesystem CRUD natives — real directories + files ------------
	// Sibling parity across Go/Rust/TS. Paths are strings. Convention:
	// predicates return 1/0; mutations return 0 on success, -1 on error;
	// fs_list returns a VList of name-strings (entries of a directory),
	// or VNull on error. These compose into a real directory tree with
	// file CRUD, the foundation under the file carrier and the substrate
	// file store.
	// (fs_exists path)        → 1 | 0
	// (fs_is_dir path)        → 1 | 0
	// (fs_mkdir path)         → 0 | -1   (mkdir -p; existing dir is success)
	// (fs_rmdir path)         → 0 | -1   (recursive remove of a directory)
	// (fs_remove path)        → 0 | -1   (remove a single file)
	// (fs_rename old new)     → 0 | -1
	// (fs_list path)          → VList of entry-name strings | VNull
	fsExistsNative := func(_ *Kernel, args []Value) Value {
		if _, err := os.Stat(resolveKernelHostPath(argStr(args, 0))); err != nil {
			return Value{Kind: VInt, Int: 0}
		}
		return Value{Kind: VInt, Int: 1}
	}
	k.registerNative("host_path_exists", catCall(), fsExistsNative)
	k.registerNative("fs_exists", catCall(), fsExistsNative)
	fsIsDirNative := func(_ *Kernel, args []Value) Value {
		info, err := os.Stat(resolveKernelHostPath(argStr(args, 0)))
		if err != nil || !info.IsDir() {
			return Value{Kind: VInt, Int: 0}
		}
		return Value{Kind: VInt, Int: 1}
	}
	k.registerNative("host_path_is_dir", catCall(), fsIsDirNative)
	k.registerNative("fs_is_dir", catCall(), fsIsDirNative)
	// One atomic mkdir, as fkwu's tag 56: 1 when this call created the directory, 0 when it
	// already stood or could not be made — the answer a lock directory reads.
	fsMkdirNative := func(_ *Kernel, args []Value) Value {
		if err := os.Mkdir(argStr(args, 0), 0777); err != nil {
			return Value{Kind: VInt, Int: 0}
		}
		return Value{Kind: VInt, Int: 1}
	}
	k.registerNative("host_dir_mkdir", catCall(), fsMkdirNative)
	k.registerNative("fs_mkdir", catCall(), fsMkdirNative)
	fsRmdirNative := func(_ *Kernel, args []Value) Value {
		info, err := os.Stat(argStr(args, 0))
		if err != nil || !info.IsDir() {
			return Value{Kind: VInt, Int: -1}
		}
		if err := os.RemoveAll(argStr(args, 0)); err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: 0}
	}
	k.registerNative("host_dir_rmdir", catCall(), fsRmdirNative)
	k.registerNative("fs_rmdir", catCall(), fsRmdirNative)
	fsRemoveNative := func(_ *Kernel, args []Value) Value {
		info, err := os.Stat(argStr(args, 0))
		if err != nil || info.IsDir() {
			return Value{Kind: VInt, Int: -1}
		}
		if err := os.Remove(argStr(args, 0)); err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: 0}
	}
	k.registerNative("host_path_remove", catCall(), fsRemoveNative)
	k.registerNative("fs_remove", catCall(), fsRemoveNative)
	fsRenameNative := func(_ *Kernel, args []Value) Value {
		if err := os.Rename(argStr(args, 0), argStr(args, 1)); err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: 0}
	}
	k.registerNative("host_path_rename", catCall(), fsRenameNative)
	k.registerNative("fs_rename", catCall(), fsRenameNative)
	fsListNative := func(_ *Kernel, args []Value) Value {
		entries, err := os.ReadDir(resolveKernelHostPath(argStr(args, 0)))
		if err != nil {
			return Value{Kind: VNull}
		}
		out := make([]Value, len(entries))
		for i, e := range entries {
			out[i] = Value{Kind: VStr, Str: e.Name()}
		}
		return Value{Kind: VList, List: out}
	}
	k.registerNative("host_dir_list", catCall(), fsListNative)
	k.registerNative("fs_list", catCall(), fsListNative)

	// --- Socket natives — L1 physical layer for inter-cell IO ----------
	// Sibling parity across Go/Rust/TS. Handle = int (≥ 0 success, -1
	// error). The connection table is package-level (socketHandles).
	// (socket_listen port)             → handle | -1
	// (socket_accept listener-handle)  → conn-handle | -1   (BLOCKS)
	// (socket_connect host port)       → conn-handle | -1
	// (socket_send conn bytes-string)  → bytes-sent | -1
	// (socket_recv conn max-bytes)     → received-string ("" on close)
	// (socket_close handle)            → 0 | -1
	k.registerNative("socket_listen", catCall(), func(_ *Kernel, args []Value) Value {
		port := args[0].AsInt()
		ln, err := net.Listen("tcp", fmt.Sprintf("127.0.0.1:%d", port))
		if err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: socketRegister(ln)}
	})
	// (socket_port listener-handle) → bound TCP port | -1. Lets a listener
	// opened on port 0 (ephemeral) report the OS-assigned port — the basis
	// of single-process loopback (listen 0 → port → connect → accept).
	k.registerNative("socket_port", catCall(), func(_ *Kernel, args []Value) Value {
		v := socketLookup(args[0].AsInt())
		ln, ok := v.(net.Listener)
		if !ok {
			return Value{Kind: VInt, Int: -1}
		}
		ta, ok := ln.Addr().(*net.TCPAddr)
		if !ok {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: int64(ta.Port)}
	})
	k.registerNative("socket_accept", catCall(), func(_ *Kernel, args []Value) Value {
		v := socketLookup(args[0].AsInt())
		ln, ok := v.(net.Listener)
		if !ok {
			return Value{Kind: VInt, Int: -1}
		}
		c, err := ln.Accept()
		if err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: socketRegister(c)}
	})
	k.registerNative("socket_connect", catCall(), func(_ *Kernel, args []Value) Value {
		host := argStr(args, 0)
		port := args[1].AsInt()
		c, err := net.Dial("tcp", net.JoinHostPort(host, strconv.FormatInt(port, 10)))
		if err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: socketRegister(c)}
	})
	k.registerNative("socket_send", catCall(), func(_ *Kernel, args []Value) Value {
		v := socketLookup(args[0].AsInt())
		c, ok := v.(net.Conn)
		if !ok {
			return Value{Kind: VInt, Int: -1}
		}
		n, err := c.Write([]byte(argStr(args, 1)))
		if err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: int64(n)}
	})
	k.registerNative("socket_recv", catCall(), func(_ *Kernel, args []Value) Value {
		v := socketLookup(args[0].AsInt())
		c, ok := v.(net.Conn)
		// A dead handle or a read ERROR answers nothing — reading either as ""
		// let a mid-stream failure pass as end-of-response, handing back a
		// truncated reply as complete. "" is reserved for asked-for-zero and
		// the peer's orderly close; bytes that arrived WITH io.EOF still cross
		// (the old code dropped them). Mirrors fk_socket_recv_native
		// (2026-08-27).
		if !ok {
			return Value{Kind: VNull}
		}
		max := args[1].AsInt()
		if max <= 0 {
			return Value{Kind: VStr, Str: ""}
		}
		buf := make([]byte, max)
		n, err := c.Read(buf)
		if n > 0 {
			return Value{Kind: VStr, Str: string(buf[:n])}
		}
		if err == io.EOF {
			return Value{Kind: VStr, Str: ""}
		}
		return Value{Kind: VNull}
	})
	k.registerNative("socket_close", catCall(), func(_ *Kernel, args []Value) Value {
		h := args[0].AsInt()
		if h < 0 {
			return Value{Kind: VInt, Int: -1}
		}
		v := socketLookup(h)
		if v == nil {
			return Value{Kind: VInt, Int: -1}
		}
		switch x := v.(type) {
		case net.Listener:
			x.Close()
		case net.Conn:
			x.Close()
		}
		socketDrop(h)
		return Value{Kind: VInt, Int: 0}
	})

	// --- Substrate write surface ----------------------------------------
	// All attributed as WITNESS — the substrate attesting to its own
	// structure. Form code holds NodeIDs as values (VNodeID) and uses
	// these natives to construct recipes.

	// make_nodeid — one range law with fkwu's native node word
	// (form-stdlib/bml/native-node-word.bml): pkg < 2^6, level < 2^13,
	// type < 2^12, 0 <= inst < 2^32, all >= 0; the 1.1.1 trivial-int lane takes
	// any 63-bit int and answers what intern_trivial_int answers for it. Outside
	// it the door stops, so no two coordinates ever collapse onto one identity.
	// Not yet one identity with fkwu: an int past int32 is fkwu's @1.1.1.N, and
	// this kernel's INT64 leaf @1.1.5.k (k an index in its own table), so
	// node_type, node_inst and value_str of such a leaf differ from fkwu's.
	k.registerNative("make_nodeid", catWitness(), func(k *Kernel, args []Value) Value {
		p, l, t, i := args[0].AsInt(), args[1].AsInt(), args[2].AsInt(), args[3].AsInt()
		if p == 1 && l == 1 && t == 1 {
			return Value{Kind: VNodeID, Nid: k.internTrivialInt(i)}
		}
		if p < 0 || p >= 1<<6 || l < 0 || l >= 1<<13 || t < 0 || t >= 1<<12 || i < 0 || i >= 1<<32 {
			panic("make_nodeid: coordinate is outside the native 64-bit node identity layout")
		}
		return Value{Kind: VNodeID, Nid: NodeID{Pkg: uint32(p), Level: uint32(l), Type: uint32(t), Inst: uint32(i)}}
	})
	k.registerNative("bp", catWitness(), func(_ *Kernel, args []Value) Value {
		if c, ok := bpTable[argStr(args, 0)]; ok {
			return Value{Kind: VNodeID, Nid: NodeID{Pkg: c[0], Level: c[1], Type: c[2], Inst: c[3]}}
		}
		// Fail loud — never invent a NodeID for an unknown name. The old silent
		// fallback to {1,2,0,0} collapsed EVERY unregistered name onto one
		// NodeID, so distinct blueprints collided invisibly (the bug that bit
		// the Shamballa channel twice). The substrate's promise is that identity
		// is bounded by what is registered; an unregistered name is a missing
		// registration, not a valid shape. Sibling parity: Rust panics, TS throws.
		panic(fmt.Sprintf("bp: unregistered blueprint name %q — register it: "+
			"add its row to form/form-stdlib/blueprint-registry.json and carry the same coordinates into bp_table.go. "+
			"The substrate never invents a NodeID for an unknown name.", args[0].Str))
	})
	k.registerNative("intern_trivial_int", catWitness(), func(k *Kernel, args []Value) Value {
		return Value{Kind: VNodeID, Nid: k.internTrivialInt(args[0].AsInt())}
	})
	k.registerNative("intern_trivial_string", catWitness(), func(k *Kernel, args []Value) Value {
		return Value{Kind: VNodeID, Nid: k.internString(argStr(args, 0))}
	})
	k.registerNative("intern_trivial_bool", catWitness(), func(_ *Kernel, args []Value) Value {
		inst := uint32(0)
		if truthy(args[0]) {
			inst = 1
		}
		return Value{Kind: VNodeID, Nid: NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivBool, Inst: inst}}
	})
	// intern_trivial_float — a float's source text, read by str_to_float's one
	// grammar, as a trivial NodeID in the f64 overflow table.
	k.registerNative("intern_trivial_float", catWitness(), func(k *Kernel, args []Value) Value {
		return Value{Kind: VNodeID, Nid: k.internTrivialFloat64(decimalPrefixFloat(argStr(args, 0)))}
	})

	// float_value — decode a TrivFloat* NodeID back to a VFloat so it can be
	// fed into math_sqrt, math_acos, arithmetic, etc. This is the small bridge
	// that lets host-interned live floats (via intern_trivial_float / make_float64)
	// be used directly in kernel-native numeric code (including the geometry projection
	// on external efficacy-probe vectors).
	k.registerNative("float_value", catMethod(), func(k *Kernel, args []Value) Value {
		if len(args) != 1 {
			panic("float_value expects 1 argument")
		}
		n := args[0]
		if n.Kind != VNodeID {
			panic("float_value expects a NodeID")
		}
		switch n.Nid.Type {
		case TrivFloat32:
			return Value{Kind: VFloat, Float: float64(k.decodeFloat32(n.Nid.Inst))}
		case TrivFloat64:
			return Value{Kind: VFloat, Float: k.decodeFloat64(n.Nid.Inst)}
		default:
			panic("float_value expects a float NodeID")
		}
	})

	k.registerNative("intern_node", catWitness(), func(k *Kernel, args []Value) Value {
		if len(args) != 2 {
			panic(fmt.Sprintf("intern_node: expected 2 args, got %d", len(args)))
		}
		if args[0].Kind != VNodeID {
			panic(fmt.Sprintf("intern_node: category must be NodeID, got %s; stack: %s", args[0].String(), k.formStackDisplay(32)))
		}
		if args[1].Kind != VList {
			panic(fmt.Sprintf("intern_node: children must be list, got %s; stack: %s", args[1].String(), k.formStackDisplay(32)))
		}
		cat := args[0].Nid
		kids := make([]NodeID, len(args[1].List))
		for i, c := range args[1].List {
			if c.Kind != VNodeID {
				panic(fmt.Sprintf("intern_node: child %d must be NodeID, got %s; stack: %s", i, c.String(), k.formStackDisplay(32)))
			}
			kids[i] = c.Nid
		}
		return Value{Kind: VNodeID, Nid: k.intern(cat, kids)}
	})
	k.registerNative("node_category", catWitness(), func(k *Kernel, args []Value) Value {
		return Value{Kind: VNodeID, Nid: k.category(args[0].AsNid())}
	})
	k.registerNative("node_children", catWitness(), func(k *Kernel, args []Value) Value {
		kids := k.children(args[0].AsNid())
		out := make([]Value, len(kids))
		for i, c := range kids {
			out[i] = Value{Kind: VNodeID, Nid: c}
		}
		return Value{Kind: VList, List: out}
	})
	k.registerNative("node_value", catWitness(), func(k *Kernel, args []Value) Value {
		return k.trivialValue(args[0].AsNid())
	})
	k.registerNative("node_pkg", catWitness(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: int64(args[0].AsNid().Pkg)}
	})
	k.registerNative("node_level", catWitness(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: int64(args[0].AsNid().Level)}
	})
	k.registerNative("node_type", catWitness(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: int64(args[0].AsNid().Type)}
	})
	k.registerNative("node_inst", catWitness(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: core.NidInst(args[0].AsNid())}
	})
	// value_eq — content identity (valueEqual). node_eq is fkwu's second spelling of
	// the same tag 80, so it shares the meaning.
	valueEqNative := func(_ *Kernel, args []Value) Value {
		return boolInt(valueEqual(args[0], args[1]))
	}
	k.registerNative("value_eq", catCompare(RCompareEq), valueEqNative)
	k.registerNative("node_eq", catCompare(RCompareEq), valueEqNative)
	// fb_record (nid, file, line<<16|col) records a node's source attribution and
	// answers the node; core.fk's intern_node_at lowers onto it.
	k.registerNative("fb_record", catWitness(), func(k *Kernel, args []Value) Value {
		nid := args[0].AsNid()
		fileNid := k.internString(argStr(args, 1))
		fileID := NameID(fileNid.Inst)
		packed := args[2].AsInt()
		line := uint32(packed >> 16)
		col := uint32(packed & 0xFFFF)
		k.sourceAttr[nid] = sourceLoc{FileID: fileID, Line: line, Col: col}
		k.framebufferRoots = append(k.framebufferRoots, nid)
		return Value{Kind: VNodeID, Nid: nid}
	})
	// node_source — read back a Recipe's source attribution.
	// Returns (list file_string line col) or empty list if none recorded.
	k.registerNative("node_source", catWitness(), func(k *Kernel, args []Value) Value {
		loc, ok := k.sourceAttr[args[0].AsNid()]
		if !ok {
			return Value{Kind: VList, List: []Value{}}
		}
		file := k.strs[loc.FileID]
		return Value{Kind: VList, List: []Value{
			{Kind: VStr, Str: file},
			{Kind: VInt, Int: int64(loc.Line)},
			{Kind: VInt, Int: int64(loc.Col)},
		}}
	})
	// framebuffer-events — every NodeID holding source attribution: the emitter
	// pays one map insert per fb_record, the observer walks this list.
	k.registerNative("framebuffer-events", catWitness(), func(k *Kernel, _ []Value) Value {
		out := make([]Value, 0, len(k.framebufferRoots))
		for _, nid := range k.framebufferRoots {
			if _, ok := k.sourceAttr[nid]; !ok {
				continue
			}
			out = append(out, Value{Kind: VNodeID, Nid: nid})
		}
		return Value{Kind: VList, List: out}
	})
	// framebuffer-event-rows - ordered detail rows over the same framebuffer
	// facts as framebuffer-counts. Detail is useful before a route condenses;
	// counts are the compressed after-JIT view.
	k.registerNative("framebuffer-event-rows", catWitness(), func(k *Kernel, _ []Value) Value {
		rows := k.framebufferEvents()
		out := make([]Value, 0, len(rows))
		for _, row := range rows {
			childVals := []Value{}
			for _, child := range row["children"].([]string) {
				childVals = append(childVals, Value{Kind: VStr, Str: child})
			}
			childValueVals := []Value{}
			for _, child := range row["child_values"].([]string) {
				childValueVals = append(childValueVals, Value{Kind: VStr, Str: child})
			}
			out = append(out, Value{Kind: VList, List: []Value{
				{Kind: VInt, Int: row["seq"].(int64)},
				{Kind: VStr, Str: row["file"].(string)},
				{Kind: VInt, Int: int64(row["line"].(uint32))},
				{Kind: VInt, Int: int64(row["col"].(uint32))},
				{Kind: VStr, Str: row["node"].(string)},
				{Kind: VList, List: childVals},
				{Kind: VList, List: childValueVals},
			}})
		}
		return Value{Kind: VList, List: out}
	})
	// framebuffer-counts - observer-side aggregation over the framebuffer
	// plane. Rows are (file, line, col, count), so repeated recipe dispatch,
	// branch failure, and JIT events stay on the same surface as source
	// attribution instead of becoming a trace-only side channel.
	k.registerNative("framebuffer-counts", catWitness(), func(k *Kernel, _ []Value) Value {
		rows := k.framebufferSourceCounts()
		out := make([]Value, 0, len(rows))
		for _, row := range rows {
			out = append(out, Value{Kind: VList, List: []Value{
				{Kind: VStr, Str: row["file"].(string)},
				{Kind: VInt, Int: int64(row["line"].(uint32))},
				{Kind: VInt, Int: int64(row["col"].(uint32))},
				{Kind: VInt, Int: int64(row["count"].(int))},
			}})
		}
		return Value{Kind: VList, List: out}
	})
	k.registerNative("framebuffer-observe-start", catWitness(), func(k *Kernel, _ []Value) Value {
		k.observeRuntime = true
		return Value{Kind: VNull}
	})
	k.registerNative("framebuffer-observe-stop", catWitness(), func(k *Kernel, _ []Value) Value {
		k.observeRuntime = false
		return Value{Kind: VNull}
	})
	// framebuffer-clear — reset the framebuffer for bounded windows.
	k.registerNative("framebuffer-clear", catWitness(), func(k *Kernel, _ []Value) Value {
		k.sourceAttr = make(map[NodeID]sourceLoc)
		k.observeSeq = 0
		k.framebufferRoots = nil
		return Value{Kind: VNull}
	})
	// write_file_bytes — sibling of read_file_bytes; writes a byte list. A
	// second argument that is not a list of ints answers -1 and leaves the
	// file as it stood, as fkwu (fk_byte_list_ok) and TS answer.
	k.registerNative("write_file_bytes", catCall(), func(_ *Kernel, args []Value) Value {
		bytes, ok := byteListOf(args[1])
		if !ok {
			return Value{Kind: VInt, Int: -1}
		}
		err := os.WriteFile(argStr(args, 0), bytes, 0644)
		if err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: int64(len(bytes))}
	})
	// file_append_bytes path bytes-list → new-file-size | -1. Atomic O_APPEND
	// write — the missing primitive for a log-structured store. Unlike
	// write_file_bytes (which truncates), this seeks to end-of-file under the
	// kernel's append lock so concurrent appends do not clobber, then returns
	// the new total file size. Creates the file if absent. Foundation for
	// cell-log-store.fk (the Bitcask-shape store).
	fileAppendBytesNative := func(_ *Kernel, args []Value) Value {
		bytes, ok := byteListOf(args[1])
		if !ok {
			return Value{Kind: VInt, Int: -1}
		}
		f, err := os.OpenFile(argStr(args, 0), os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
		if err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		defer f.Close()
		if _, err := f.Write(bytes); err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		info, err := f.Stat()
		if err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: info.Size()}
	}
	k.registerNative("host_file_append_bytes", catCall(), fileAppendBytesNative)
	k.registerNative("file_append_bytes", catCall(), fileAppendBytesNative)
	// write_file_text — host text output. Keeps text compilers from
	// materializing byte lists while byte codecs still use write_file_bytes.
	writeFileTextNative := func(_ *Kernel, args []Value) Value {
		bytes := []byte(argStr(args, 1))
		err := os.WriteFile(argStr(args, 0), bytes, 0644)
		if err != nil {
			return Value{Kind: VInt, Int: -1}
		}
		return Value{Kind: VInt, Int: int64(len(bytes))}
	}
	k.registerNative("host_file_write_text", catCall(), writeFileTextNative)
	k.registerNative("write_file", catCall(), writeFileTextNative)
	k.registerNative("write_file_text", catCall(), writeFileTextNative)

	k.registerHostIONatives()

	// `now_unix_ms` — current wall-clock as a millisecond unix timestamp.
	// External effect (reads the host clock) so it's catCall. Sibling
	// parity holds on shape, NOT on value: every kernel returns an int,
	// every kernel's int is > a recent past epoch — but the exact
	// milliseconds diverge between invocations. Bands check shape only.
	k.registerNative("now_unix_ms", catCall(), func(_ *Kernel, _ []Value) Value {
		return Value{Kind: VInt, Int: time.Now().UnixMilli()}
	})

	// `temp_dir` — the host's scratch directory: TMPDIR when the carrier
	// names one, /tmp otherwise (no trailing slash). External read (host
	// env) so it's catCall. The door that lets a band's scratch files land
	// in per-leg space: validate.sh points each sibling kernel at its own
	// TMPDIR, so concurrent legs never share a scratch path. Sibling
	// parity holds on shape, NOT on value — each leg's dir differs by
	// design; bands fold the path into effects, never into the verdict.
	tempDirNative := func(_ *Kernel, _ []Value) Value {
		dir := os.Getenv("TMPDIR")
		if dir == "" {
			dir = "/tmp"
		}
		return Value{Kind: VStr, Str: strings.TrimRight(dir, "/")}
	}
	k.registerNative("host_temp_dir", catCall(), tempDirNative)
	k.registerNative("temp_dir", catCall(), tempDirNative)
	// host_pid, host_monotonic_ms, host_cwd — this process's id, a monotonic millisecond clock and
	// its working directory: the doors fkwu carries as tags 160, 182 and 29. Parity holds on shape,
	// not value: each leg is its own process, and only differences between two clock readings mean
	// anything (fkwu counts from boot, the siblings from their own start).
	k.registerNative("host_pid", catCall(), func(_ *Kernel, _ []Value) Value {
		return Value{Kind: VInt, Int: int64(os.Getpid())}
	})
	k.registerNative("host_monotonic_ms", catCall(), func(_ *Kernel, _ []Value) Value {
		return Value{Kind: VInt, Int: time.Since(hostMonotonicStart).Milliseconds()}
	})
	k.registerNative("host_cwd", catCall(), func(_ *Kernel, _ []Value) Value {
		dir, err := os.Getwd()
		if err != nil {
			return Value{Kind: VNull}
		}
		return Value{Kind: VStr, Str: dir}
	})
	// kernel_stat, kernel_live, print_str — the doors fkwu carries as tags 127, 163 and 115.
	//
	// kernel_stat key reads fkwu's self-measurement key space. Key 164 is fkwu's arm counter for
	// tag 64 (record_new): the record-construction clock, every record this process has made,
	// which compaction never lowers. This kernel counts the same event where it happens. A key
	// this kernel does not measure answers nothing, never a zero it did not read.
	k.registerNative("kernel_stat", catWitness(), func(_ *Kernel, args []Value) Value {
		if len(args) > 0 && args[0].Kind == VInt && args[0].Int == 164 {
			return Value{Kind: VInt, Int: recordConstructions.Load()}
		}
		return Value{Kind: VNull}
	})
	// kernel_live pid answers that kernel's live page in fkwu's word order: 0 the page magic,
	// 1 the pid, 2 the start-ms. This kernel holds those three words of its own page; fkwu's
	// further words count fkwu's own tissue and are not claimed here. Any other pid answers the
	// empty list, fkwu's answer where no page can be read.
	k.registerNative("kernel_live", catWitness(), func(_ *Kernel, args []Value) Value {
		if len(args) == 0 || args[0].Kind != VInt || args[0].Int != int64(os.Getpid()) {
			return Value{Kind: VList, List: []Value{}}
		}
		return Value{Kind: VList, List: []Value{
			{Kind: VInt, Int: kernelLiveMagic},
			{Kind: VInt, Int: args[0].Int},
			{Kind: VInt, Int: hostBirthUnixMs},
		}}
	})
	// print_str s writes the string's bytes and one newline to stdout as one write, and
	// answers 0, as fkwu does. A value that is not a string writes the newline alone.
	k.registerNative("print_str", catCall(), func(_ *Kernel, args []Value) Value {
		s := ""
		if len(args) > 0 && args[0].Kind == VStr {
			s = args[0].Str
		}
		os.Stdout.WriteString(s + "\n")
		return Value{Kind: VInt, Int: 0}
	})
}

// Category constructors for native attribution live further down alongside
// catMath/catCompare/catBlock/etc. The reader-side helpers already cover
// catCompare(inst), catBlock(inst), etc.; the native-attribution helpers
// (catCall, catWitness, catAccess, catMethod, catListNat, catUndefined)
// are defined in the same block to keep them together.

// ---------------------------------------------------------------------------
// Walker — full RBasic dispatch
// ---------------------------------------------------------------------------

// Block kinds walkInner reads off a do's own forms, and how walkUnit reads a
// unit root the reader named.
const (
	blockKindLet  = 1
	blockKindDefn = 2
	blockKindDo   = 3
	unitDo        = uint8(1)
	unitWrapper   = uint8(2)
)

func (k *Kernel) blockKind(n NodeID) int {
	if n.Level == LevelTrivial {
		return 0
	}
	cat := k.recipeAt(n).Category
	switch cat.Type {
	case RBasicBlock:
		if cat.Inst == RBlockLet {
			return blockKindLet
		}
		return blockKindDo
	case RBasicFnDef:
		return blockKindDefn
	}
	return 0
}

// markUnitRoot — the reader names each do it hands back as a unit root: the
// implicit do around several top-level forms (wrapper), or the one top-level
// (do ...) of a source.
func (k *Kernel) markUnitRoot(root NodeID, wrapper bool) {
	if k.blockKind(root) != blockKindDo {
		return
	}
	if k.unitRoots == nil {
		k.unitRoots = make(map[NodeID]uint8)
	}
	if wrapper {
		k.unitRoots[root] = unitWrapper
	} else {
		k.unitRoots[root] = unitDo
	}
}

// observeBlock — the dispatch record walkInner makes, for a block form that
// bindLet or walkUnit reads without passing it through walkInner.
func (k *Kernel) observeBlock(cat NodeID) {
	if k.Trace != nil {
		k.Trace.record(cat.Type, cat.Inst)
	}
	k.observeRecipeDispatch(cat)
}

// bindLet — a unit's let binds the unit's frame: the value reads the frame
// before the name joins it. A later unit let that rebinds a name raises the
// unit version, so a closure defined before it keeps the binding it was made
// under, as fkwu's hold per reference keeps it.
func (k *Kernel) bindLet(n NodeID, env *Frame) Value {
	r := k.recipeAt(n)
	k.observeBlock(r.Category)
	v := k.walk(r.Children[1], env)
	name := k.identID(r.Children[0])
	switch {
	case env.Parent == nil && env.HasOwn(name):
		k.unitView++
		env.Rebind(name, v, k.unitView)
	default:
		env.Bind(name, v)
	}
	return v
}

// walkUnit — a unit root read in the unit's own frame. The unit's top-level
// do is its sequence, as fkwu's fk_parse_top reads it: a let there binds the
// unit's frame, where every defn of the unit and every unit loaded after it
// reads it; a defn binds there; a do before the first let or expression is a
// top-level do too; a later do is a lexical scope (walkInner's block arm). The
// implicit do around several top-level forms holds each form at column 0,
// where every do is a top-level do. A root the reader did not name (a
// deserialized recipe, a combined program) keeps every do at its top flat.
func (k *Kernel) walkUnit(n NodeID, env *Frame) Value {
	kind := k.blockKind(n)
	if kind == blockKindLet {
		return k.bindLet(n, env)
	}
	if kind != blockKindDo {
		return k.walk(n, env)
	}
	mark := k.unitRoots[n]
	if mark == unitDo {
		return k.walkUnitDo(n, env)
	}
	r := k.recipeAt(n)
	k.observeBlock(r.Category)
	v := Value{}
	for _, c := range r.Children {
		if mark == unitWrapper && k.blockKind(c) == blockKindDo && k.unitRoots[c] != unitWrapper {
			v = k.walkUnitDo(c, env)
		} else {
			v = k.walkUnit(c, env)
		}
	}
	return v
}

func (k *Kernel) walkUnitDo(n NodeID, env *Frame) Value {
	r := k.recipeAt(n)
	k.observeBlock(r.Category)
	leading := true
	v := Value{}
	for _, c := range r.Children {
		switch kind := k.blockKind(c); kind {
		case blockKindDo:
			if leading {
				v = k.walkUnitDo(c, env)
			} else {
				v = k.walk(c, env)
			}
		case blockKindLet:
			v = k.bindLet(c, env)
			leading = false
		default:
			v = k.walk(c, env)
			if kind != blockKindDefn {
				leading = false
			}
		}
	}
	return v
}

func (k *Kernel) walk(n NodeID, env *Frame) Value {
	// One Form-stack slot per host walk invocation (see walkInner's closure
	// arm). The truncation runs only on the success path — a panic leaves
	// the live frames in place for the recover site to read.
	depth := len(k.formStack)
	k.walkDepth++
	if k.walkDepth > k.walkWall && k.walkWall > 0 {
		panic("eval too deep -- the recursion needs to be tail or balanced")
	}
	v := k.walkInner(n, env)
	k.walkDepth--
	k.formStack = k.formStack[:depth]
	return v
}

func (k *Kernel) walkInner(n NodeID, env *Frame) Value {
	// Tail-call optimization: a tail-position call — a closure body, a cond
	// branch, or a do-block's last expr — reassigns n/env and loops here
	// instead of recursing, so tail-recursive Form loops (gm-rep-loop,
	// gm-sep-loop, caps-get, find-loop, …) run in CONSTANT stack. This is the
	// "non-stack" shape: genuine data nesting still recurses; iteration does
	// not. Result-transparent — identical values to plain recursion, far less
	// stack (a long member/statement list no longer recurses N-deep), which is
	// what lets the strictest kernel parse the full thesis grammar files.
	// myFrame — index of this walk invocation's Form-stack slot, -1 until
	// a closure is entered. TCO re-entry REPLACES the slot (the tail
	// caller's frame is complete), mirroring the host stack's collapse.
	myFrame := -1
	for {
		if n.Level == LevelTrivial {
			return k.trivialValue(n)
		}
		// One map lookup per composite walk step. cat + kids read off the same
		// recipe row; Go's map returns Recipe by value, but Children is a slice
		// header pointing to the table's backing array — zero-copy access.
		r := k.recipeAt(n)
		cat, kids := r.Category, r.Children

		// Tracing hook: when k.Trace is set, record the arm dispatch. Pure
		// counter increment — no allocation, no IO. Per lc-native-kernel-binary.
		// Records (ty, inst) so typed-numeric distribution stays distinguishable.
		if k.Trace != nil {
			k.Trace.record(cat.Type, cat.Inst)
		}
		k.observeRecipeDispatch(cat)

		switch cat.Type {
		case RBasicMath:
			lv := k.walk(kids[0], env)
			rv := k.walk(kids[1], env)
			// Width promotion: if either operand is Float, the result is
			// Float (matches Python `int + float → float`, and IEEE 754
			// arithmetic on mixed inputs). Pure int/int stays on the
			// fast int path. Mirrors Rust kernel's RB_MATH dispatch.
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
					// truncated, the sign of the dividend, as integer mod is
					return Value{Kind: VFloat, Float: math.Mod(l, r)}
				}
			}
			a := lv.AsInt()
			b := rv.AsInt()
			switch cat.Inst {
			case RMathPlus:
				return Value{Kind: VInt, Int: wrap63(a + b)}
			case RMathMinus:
				return Value{Kind: VInt, Int: wrap63(a - b)}
			case RMathMultiply:
				return Value{Kind: VInt, Int: wrap63(a * b)}
			case RMathDivide:
				return Value{Kind: VInt, Int: wrap63(a / b)}
			case RMathModulo:
				return Value{Kind: VInt, Int: a % b}
			}

		case RBasicCompare:
			lv := k.walk(kids[0], env)
			rv := k.walk(kids[1], env)
			// Same width-promotion rule as math: float on either side forces
			// an IEEE comparison. Pure int/int stays integer. Mirrors Rust.
			// A comparison acknowledges with the 0/1 integer states (axiom-1,
			// core-axioms.form) so its answer flows directly into arithmetic —
			// the same shape every JIT lane already lands at the i64 ABI.
			// Proven three-way by tests/eq-shape-band.fk.
			// Where a non-number takes part, eq and ne answer content identity
			// (valueEqual, axiom-3) and an ordering has no answer to give: it
			// refuses by name, as fkwu's tags 5 and 103 do, rather than reading
			// the operand as an int.
			if !(cmpNumberKind(lv) && cmpNumberKind(rv)) {
				if cat.Inst == RCompareEq || cat.Inst == RCompareNe {
					return boolInt(valueEqual(lv, rv) == (cat.Inst == RCompareEq))
				}
				panic("order: only numbers have an order -- ask value_kind before lt/le/gt/ge")
			}
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
			// Logic consumes truthiness and answers in the comparison
			// family's 0/1 integer states (axiom-1) — truth has one value
			// shape, so (mul (and ...) n) flows on every kernel exactly
			// like (mul (eq ...) n). Mirrors Rust's as_bool and TS truthy
			// on the consuming side.
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
				// A let binds the rest of its own do (the loop below binds a
				// do's own lets). Met anywhere else — an if arm, an argument,
				// a do's last form — nothing follows it, so it answers its
				// value and binds no name.
				return k.walk(kids[1], env)
			}
			if len(kids) == 0 {
				return Value{}
			}
			// DO / SEQUENCE — a lexical scope: its lets and defns bind for
			// the rest of this do only. The scope opens at the first binding
			// form, so a do that binds nothing costs no frame. A unit's
			// top-level do reads flat (walkUnit).
			last := len(kids) - 1
			scope := env
			mark := k.closuresCreated
			for i := 0; i < last; i++ {
				kind := k.blockKind(kids[i])
				if kind == blockKindLet {
					// The value reads the scope as it stands; the name binds
					// in a fresh frame when the scope is not yet open or a
					// closure was made since it opened, so a closure keeps
					// the bindings it was made under.
					r := k.recipeAt(kids[i])
					k.observeBlock(r.Category)
					v := k.walk(r.Children[1], scope)
					if scope == env || k.closuresCreated != mark {
						scope = NewFrame(scope)
						mark = k.closuresCreated
					}
					scope.Bind(k.identID(r.Children[0]), v)
					continue
				}
				if kind == blockKindDefn && scope == env {
					scope = NewFrame(env)
					mark = k.closuresCreated
				}
				k.walk(kids[i], scope)
			}
			if scope == env && k.blockKind(kids[last]) == blockKindDefn {
				scope = NewFrame(env)
			}
			env = scope
			n = kids[last] // TCO: a do/seq block's last expr is in tail position
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
				for i, branch := range kids {
					if k.Trace != nil {
						k.Trace.ChoiceAttempts++
					}
					k.observeFrame("observe/go/choice/attempt", uint32(i+1), uint32(len(kids)), branch)
					value, ok, stopped := k.walkChoiceBranch(branch, env)
					if ok {
						if k.Trace != nil {
							k.Trace.ChoiceSuccesses++
						}
						if stopped {
							k.observeFrame("observe/go/choice/stop", uint32(i+1), uint32(len(kids)), branch)
						} else {
							k.observeFrame("observe/go/choice/success", uint32(i+1), uint32(len(kids)), branch)
						}
						return value
					}
					if k.Trace != nil {
						k.Trace.ChoiceFailures++
					}
					k.observeFrame("observe/go/choice/fail", uint32(i+1), uint32(len(kids)), branch)
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
			// A closure defined at the unit level reads the unit as it stands
			// now: its own empty frame carries the unit version (Frame.View).
			clEnv := env
			if env.Parent == nil {
				clEnv = NewFrame(env)
				clEnv.View = k.unitView
			}
			k.closuresCreated++
			cl := &Closure{Name: name, Params: params, Body: kids[2], Env: clEnv}
			env.Bind(name, Value{Kind: VClosure, Cl: cl})
			return Value{Kind: VClosure, Cl: cl}

		case RBasicFnCall:
			rawName := k.identID(kids[0])
			// (attempt x): x is walked under a recover point, never before — fkwu's mode 28.
			if len(kids) == 2 && k.nameStr(rawName) == "attempt" {
				return k.attempt(kids[1], env)
			}
			// A present native answers its name, as on fkwu and TS: a Form definition of the
			// same name is a fallback for a kernel without the native, never an override of one.
			// A local binding of the name (a parameter, a let) is nearer than the native
			// unless fkwu reserves the head: the one call-position reading every arm gives.
			if ne, ok := k.natives[rawName]; ok && (fkwuReservedHeads[k.nameStr(rawName)] || !env.HasLocal(rawName)) {
				args := make([]Value, len(kids)-1)
				for i := 1; i < len(kids); i++ {
					args[i-1] = k.walk(kids[i], env)
				}
				if k.Trace != nil && ne.Category.Type != RBasicUndefined {
					k.Trace.record(ne.Category.Type, ne.Category.Inst)
				}
				if k.Trace != nil {
					k.Trace.recordNative(k.nameStr(ne.Name))
				}
				k.observeNamedDispatch("observe/go/native-dispatch", ne.Name)
				k.formStack = append(k.formStack, formFrame{name: ne.Name})
				v := ne.Fn(k, args)
				k.formStack = k.formStack[:len(k.formStack)-1]
				return v
			}
			v, ok := env.Lookup(rawName)
			if !ok {
				panic(fmt.Sprintf("walk: unbound function %q", k.nameStr(rawName)))
			}
			if v.Kind != VClosure {
				panic(fmt.Sprintf("walk: %q is not callable", k.nameStr(rawName)))
			}
			cl := v.Cl
			if len(kids)-1 != len(cl.Params) {
				panic(fmt.Sprintf("walk: %q wants %d args, got %d", k.nameStr(rawName), len(cl.Params), len(kids)-1))
			}
			argVals := make([]Value, len(cl.Params))
			for i := 1; i < len(kids); i++ {
				argVals[i-1] = k.walk(kids[i], env)
			}
			call := NewCallFrame(cl.Env, len(cl.Params))
			for i, p := range cl.Params {
				call.Bind(p, argVals[i])
			}
			if k.Trace != nil {
				k.Trace.recordFn(k.nameStr(cl.Name))
			}
			k.observeNamedDispatch("observe/go/function-dispatch", cl.Name)
			if frame := (formFrame{name: cl.Name, body: cl.Body, hasBody: true}); myFrame >= 0 && myFrame < len(k.formStack) {
				k.formStack[myFrame] = frame
			} else {
				myFrame = len(k.formStack)
				k.formStack = append(k.formStack, frame)
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

		// Structural passthrough — categories the walker can't yet execute
		// (CHOICE_MATCH, CONSTRUCTOR, INDUCTIVE, QUOTIENT, ALIAS, BLANKET,
		// PROJECT, GENERATIVE, PROOF, INFERENCE, VECTOR, TILE, PARALLELIZE,
		// VECTORIZE, OBSERVER, TRANSMUTE, ...) intern fine and the trace
		// records their attribution. Walking returns the NodeID itself so
		// downstream structural reasoning continues. Sibling-parity with
		// the Rust + TS kernels.
		return Value{Kind: VNodeID, Nid: n}
	}
}

func (k *Kernel) walkChoiceBranch(branch NodeID, env *Frame) (value Value, ok bool, stopped bool) {
	depth := len(k.formStack)
	defer func() {
		if r := recover(); r != nil {
			// A swallowed branch failure must not leave its frames behind.
			k.formStack = k.formStack[:depth]
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
			// Truth is the 0/1 integer states (axiom-1): a true or false
			// pattern keys as the int it is, the int a comparison answers.
			if pattern.Type == TrivBool {
				table.cases[k.internTrivialInt(int64(pattern.Inst))] = body
				continue
			}
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
	case VNodeID:
		return v.Nid, true
	default:
		return NodeID{}, false
	}
}

// byteListOf — the bytes a byte writer writes: a list whose every cell is an int,
// each masked to its low byte (all four kernels mask). Anything else is no byte
// list (ok false), and the writer answers -1 before it touches the file, as
// fkwu's fk_byte_list_ok refuses.
func byteListOf(v Value) ([]byte, bool) {
	if v.Kind != VList {
		return nil, false
	}
	bytes := make([]byte, len(v.List))
	for i, x := range v.List {
		if x.Kind != VInt {
			return nil, false
		}
		bytes[i] = byte(x.Int)
	}
	return bytes, true
}

// valueEqual — content identity (axiom-3: same composition is the same cell).
// value_eq answers it, and eq/ne answer it wherever a non-number takes part.
// Numbers keep their kind: an int never equals a float here, and a NaN is the
// NaN it was built as. Strings meet by text, NodeIDs by coordinates, lists by
// their items, however the lists were built. Records and closures are places
// (record_set writes into one), so a place equals only itself. Siblings to
// Rust's value_equal, TypeScript's valueEqual and fkwu's fk_veq.
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
		return a.Float == b.Float || (a.Float != a.Float && b.Float != b.Float)
	case VStr:
		return a.Str == b.Str
	case VNodeID:
		return a.Nid == b.Nid
	case VList:
		if len(a.List) != len(b.List) {
			return false
		}
		// a pair meets only a pair with the same tail word, as on fkwu
		if (a.Tail == nil) != (b.Tail == nil) || (a.Tail != nil && !valueEqual(*a.Tail, *b.Tail)) {
			return false
		}
		if len(a.List) == 0 || &a.List[0] == &b.List[0] {
			return true
		}
		for i := range a.List {
			if !valueEqual(a.List[i], b.List[i]) {
				return false
			}
		}
		return true
	case VRecord:
		return a.Rec == b.Rec
	case VClosure:
		return a.Cl == b.Cl
	}
	return false
}

// cmpNumberKind — the kinds the compare lane coerces: ints and floats (truth
// is already their 0/1). eq/ne over anything else is valueEqual; an ordering over
// anything else has no answer.
func cmpNumberKind(v Value) bool {
	return v.Kind == VInt || v.Kind == VFloat
}

func (k *Kernel) walkMatchSwitch(node NodeID, kids []NodeID, env *Frame) Value {
	if len(kids) < 1 || (len(kids)-1)%2 != 0 {
		panic("match: SWITCH expects scrutinee plus pattern/body pairs")
	}
	if k.Trace != nil {
		k.Trace.MatchLookups++
	}
	scrutinee := k.walk(kids[0], env)
	table := k.switchTableFor(node, kids)
	if key, ok := k.switchKeyFromValue(scrutinee); ok {
		if body, found := table.cases[key]; found {
			if k.Trace != nil {
				k.Trace.MatchHits++
			}
			k.observeFrame("observe/go/match/hit", key.Type, key.Inst, node, key, body)
			return k.walk(body, env)
		}
	}
	for _, arm := range table.dynamicArms {
		if valueEqual(k.walk(arm.pattern, env), scrutinee) {
			if k.Trace != nil {
				k.Trace.MatchHits++
			}
			k.observeFrame("observe/go/match/dynamic-hit", 1, uint32(len(table.dynamicArms)), node, arm.pattern, arm.body)
			return k.walk(arm.body, env)
		}
	}
	if table.hasDefault {
		if k.Trace != nil {
			k.Trace.MatchDefaults++
		}
		k.observeFrame("observe/go/match/default", 1, 1, node, table.defaultBody)
		return k.walk(table.defaultBody, env)
	}
	if k.Trace != nil {
		k.Trace.MatchMisses++
	}
	k.observeFrame("observe/go/match/miss", 1, uint32(len(kids)), node)
	panic(fmt.Sprintf("match: exhausted without a matching arm for %s", scrutinee.String()))
}

// truthy — a branch reads a state (axiom-1): 0 and a float zero are 0,
// nothing is neither 0 nor 1 and refuses, and every other value is 1.
func truthy(v Value) bool {
	switch v.Kind {
	case VInt:
		return v.Int != 0
	case VFloat:
		return v.Float != 0
	case VNull:
		panic("if: nothing is neither 0 nor 1 -- ask nothing? before branching")
	}
	return true
}

// boolInt — the truth family's acknowledgment shape: 0/1 integer states
// (axiom-1) so eq/lt/and/not/node_eq/… answers feed arithmetic on every kernel.
func boolInt(b bool) Value {
	if b {
		return Value{Kind: VInt, Int: 1}
	}
	return Value{Kind: VInt, Int: 0}
}

// ---------------------------------------------------------------------------
// S-expression source adapter — text → recipe tree
// ---------------------------------------------------------------------------
//
// Syntax:
//   (verb arg arg ...)      — composite recipe
//   <int>                   — trivial INT
//   "string"                — trivial STRING
//   <ident>                 — identifier reference (RBasicIdent)
//   ; comment to end of line
//
// Verb mapping (recipe builders):
//   do, seq, let
//   if (2-arg or 3-arg)
//   add, sub, mul, div, mod
//   eq, ne, lt, le, gt, ge
//   and, or, not
//   defn (name params-list body)
//   <anything-else>         — FnCall to that name

// sexpToken — source-reader cell. Carries 1-based line/col so parse
// errors can point at the source. Without this, every paren imbalance
// surfaces as an unhelpful "index out of bounds" panic.
type sexpToken struct {
	kind  string // "LPAREN" | "RPAREN" | "INT" | "STRING" | "IDENT"
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
			// Don't advance col; newline handler will reset on \n
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
			// Fractional part: a `.` after the digits makes a float, with or
			// without fraction digits (`1.` reads 1.0), as fkwu's number leaf
			// and the TS reader read it.
			if i < len(src) && src[i] == '.' {
				isFloat = true
				i++ // consume '.'
				for i < len(src) && src[i] >= '0' && src[i] <= '9' {
					i++
				}
			}
			// Exponent: e/E [+/-] one-or-more digits. Accepted on both
			// pure-int mantissa (1e5) and fractional mantissa (1.5e3),
			// matching the TS kernel reader's float regex.
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
				// Any other backslash stands for itself, as fkwu's fk_smkstr
				// reads it: "a\qb" is four bytes.
				out = append(out, '\\', s[i+1])
			}
			i++
			continue
		}
		out = append(out, s[i])
	}
	return string(out)
}

// readSexpr — parse the token stream starting at position i, return the
// recipe NodeID and the next position. Every error path includes line/col
// so paren imbalance points at the source instead of dying with "index
// out of bounds." The source adapter is foreign-syntax-by-necessity;
// its job is to fail informatively when humans miscount.
func (k *Kernel) readSexpr(toks []sexpToken, i int) (NodeID, int) {
	if i >= len(toks) {
		panic("parse error: unexpected end of input (expected an expression)")
	}
	t := toks[i]
	switch t.kind {
	case "INT":
		return k.internTrivialInt(intLiteral(t.value)), i + 1
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
		// Bool literals — true/false are reserved, become trivial values at parse
		// time. Parallel to int/string literals; lets Form predicates read
		// naturally without `(eq 0 0)` constructors.
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
			var arg NodeID
			var ni int
			if verb == "defn" && len(args) == 1 && toks[i].kind == "LPAREN" {
				// A defn's parameter list holds names, not an expression.
				// Read as an expression, its first name became the list's
				// verb: `(params tok i)` built a two-name sequence, and every
				// first parameter spelled like a verb (params, match, fail,
				// list, ...) fell out of the arity. fkwu and TS read names.
				arg, ni = k.readDefnParams(toks, i)
			} else {
				arg, ni = k.readSexpr(toks, i)
			}
			args = append(args, arg)
			i = ni
		}
		node := k.buildVerb(verb, args)
		// Source attribution at read time: every parenthesized form
		// remembers the file:line:col of its opening paren, so a fatal
		// mid-walk can name the Form source line. Content-addressing
		// means a shape interned from two sites keeps its FIRST
		// authoring site.
		if fileID, localLine, ok := k.resolveReadingLine(uint32(openLine)); ok {
			if _, exists := k.sourceAttr[node]; !exists {
				k.sourceAttr[node] = sourceLoc{FileID: fileID, Line: localLine, Col: uint32(openCol)}
			}
		}
		return node, i
	}
	panic(fmt.Sprintf("parse error at line %d col %d: unexpected token %s %q", t.line, t.col, t.kind, t.value))
}

// readDefnParams — read a defn's `(name name ...)` parameter list starting at
// its LPAREN, as names only: each IDENT becomes the bare string trivial the
// defn arm of buildVerb already carries, so the returned sequence is the very
// params block that defn interns. Returns the node and the next position.
func (k *Kernel) readDefnParams(toks []sexpToken, i int) (NodeID, int) {
	openLine, openCol := toks[i].line, toks[i].col
	i++
	names := []NodeID{}
	for {
		if i >= len(toks) {
			panic(fmt.Sprintf("parse error: unclosed defn parameter list opened at line %d col %d (reached end of input)",
				openLine, openCol))
		}
		t := toks[i]
		if t.kind == "RPAREN" {
			return k.intern(catBlock(RBlockSequence), names), i + 1
		}
		if t.kind != "IDENT" {
			panic(fmt.Sprintf("parse error at line %d col %d: defn parameter list opened at line %d col %d holds names only, got %s %q",
				t.line, t.col, openLine, openCol, t.kind, t.value))
		}
		names = append(names, k.internString(t.value))
		i++
	}
}

// attempt walks x with a recover point standing, as fkwu's fk_attempt does. A stop inside x
// (a panic from a Form-level refusal) unwinds here: the Form stack returns to its depth at
// entry, the stop goes out as one organ-health line (aspect stop, backtrack selected), and the
// attempt answers nothing — the value every choice reads as "this option did not land".
func (k *Kernel) attempt(x NodeID, env *Frame) (v Value) {
	depth := len(k.formStack)
	walkDepth := k.walkDepth
	defer func() {
		if r := recover(); r != nil {
			k.formStack = k.formStack[:depth]
			k.walkDepth = walkDepth
			k.stopSeq++
			now := time.Now().UnixMilli()
			row, _ := json.Marshal(map[string]interface{}{
				"schema": "organ-health-v1", "id": fmt.Sprintf("form-kernel-go-%d:stop:%d", os.Getpid(), k.stopSeq),
				"organ": "form-kernel-go", "flow": "walker", "aspect": "stop", "stage": "applied",
				"expected": "value", "observed": fmt.Sprint(r), "health": nil, "surprise": 1,
				"needs": []string{}, "offers": []string{"backtrack"}, "selected": "backtrack",
				"result": map[string]string{"answer": "nothing"}, "observed_at_ms": now, "at_ms": now,
			})
			fmt.Fprintf(os.Stderr, "form-organ health %s\n", row)
			v = Value{Kind: VNull}
		}
	}()
	return k.walk(x, env)
}

// buildVerb — map an S-expression verb to its recipe category + children.
// The single point where the source syntax meets the substrate vocabulary.
func (k *Kernel) buildVerb(verb string, args []NodeID) NodeID {
	switch verb {
	case "do":
		return k.intern(catBlock(RBlockDo), args)
	case "let":
		// (let <ident> <value>) — repackage the identifier wrapper as the
		// bare string trivial so the walker reads NameID directly from `inst`.
		// (let <ident> <value> <body>) binds the name over its body alone and
		// answers the body, as fkwu reads it: it is the do (do (let n v) body),
		// whose own scope ends with the body, so the name leaves with it.
		if len(args) < 2 || len(args) > 3 {
			panic(fmt.Sprintf("parse error: let takes (let name value) or (let name value body), got %d forms", len(args)))
		}
		nameID := k.identID(args[0])
		nameTrivial := NodeID{Pkg: 1, Level: LevelTrivial, Type: TrivString, Inst: uint32(nameID)}
		bind := k.intern(catBlock(RBlockLet), []NodeID{nameTrivial, args[1]})
		if len(args) == 3 {
			return k.intern(catBlock(RBlockDo), []NodeID{bind, args[2]})
		}
		return bind
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
	case "defn":
		// (defn <name> (<params>...) <body>) — names and params get repackaged
		// as bare string trivials so the walker reads NameID via `inst`.
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
	case "list":
		// Canonical list literal. Keep the source recipe identical to the
		// Rust/TypeScript readers instead of routing through a native FNCALL.
		return k.intern(catListNat(), args)
	default:
		// Default: a function call to `verb` with these args
		nameStr := k.internString(verb)
		all := append([]NodeID{nameStr}, args...)
		return k.intern(catFnCall(), all)
	}
}

// Category constructors
func catMath(inst uint32) NodeID {
	return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicMath, Inst: inst}
}
func catCompare(inst uint32) NodeID {
	return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicCompare, Inst: inst}
}
func catLogic(inst uint32) NodeID {
	return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicLogic, Inst: inst}
}
func catCond(inst uint32) NodeID {
	return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicCond, Inst: inst}
}
func catBlock(inst uint32) NodeID {
	return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicBlock, Inst: inst}
}
func catIdent() NodeID  { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicIdent, Inst: 1} }
func catFnDef() NodeID  { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicFnDef, Inst: 1} }
func catFnCall() NodeID { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicFnCall, Inst: 1} }

// Native-attribution category constructors. Each names the Form-shape a
// native expresses; the walker records them in the trace when the native
// fires. Mirrors Rust kernel's cat_call / cat_witness / cat_access /
// cat_method / cat_list_nat / cat_undefined.
func catCall() NodeID      { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicCall, Inst: 1} }
func catWitness() NodeID   { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicWitness, Inst: 1} }
func catAccess() NodeID    { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicAccess, Inst: 1} }
func catMethod() NodeID    { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicMethod, Inst: 1} }
func catListNat() NodeID   { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicList, Inst: 1} }
func catReceipt() NodeID   { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicReceipt, Inst: 1} }
func catUndefined() NodeID { return NodeID{Pkg: 1, Level: LevelBasic, Type: RBasicUndefined, Inst: 0} }

// ---------------------------------------------------------------------------
// Main — entry point
// ---------------------------------------------------------------------------

// stopAtStrayParen — a `)` with no `(` open ends the reading, as fkwu's
// whole-source balance does: wrapped in the implicit do it would close that do
// early and every form after it would silently fall away, so nothing runs. The
// stop names the file, line and column, and voices one organ-health reading.
func (k *Kernel) stopAtStrayParen(toks []sexpToken) {
	depth := 0
	for _, t := range toks {
		if t.kind == "LPAREN" {
			depth++
			continue
		}
		if t.kind != "RPAREN" {
			continue
		}
		if depth > 0 {
			depth--
			continue
		}
		path, line := "", uint32(t.line)
		where := fmt.Sprintf("line %d col %d", t.line, t.col)
		if fileID, local, ok := k.resolveReadingLine(uint32(t.line)); ok {
			path, line = k.nameStr(fileID), local
			where = fmt.Sprintf("%s:%d:%d", path, local, t.col)
		}
		detail := "[unbalanced-source] stray ')' closes a form that was never opened -- refusing to run"
		now := time.Now().UnixMilli()
		row, _ := json.Marshal(map[string]interface{}{
			"schema": "organ-health-v1", "id": fmt.Sprintf("form-kernel-go-%d:reader:unbalanced-source", now),
			"organ": "form-kernel-go", "flow": "reader", "aspect": "unbalanced-source", "stage": "observe",
			"expected": "balanced", "observed": "compile-error", "health": 0, "surprise": 1,
			"needs":  []map[string]string{{"resource": "source-diagnostics", "detail": detail}},
			"offers": []string{"revise"}, "selected": "",
			"evidence":       map[string]interface{}{"kernel": "go", "path": path, "line": line, "col": t.col},
			"observed_at_ms": now, "at_ms": now,
		})
		fmt.Fprintf(os.Stderr, "form-organ health %s\n", row)
		panic(fmt.Sprintf("parse error at %s: %s", where, detail))
	}
}

func readRootFromSource(k *Kernel, src string) NodeID {
	toks := tokenizeSexp(src)
	k.stopAtStrayParen(toks)
	// Wrap multiple top-level forms in an implicit do-block. Counts
	// top-level expressions by paren depth — single expr passes through,
	// multiple get wrapped.
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
	k.markUnitRoot(root, wrapped != src)
	return root
}

func kernelSourceExcerpt(src string, maxLen int) string {
	if len(src) <= maxLen {
		return src
	}
	return src[:maxLen]
}

func kernelSourceLineCount(src string) int {
	if src == "" {
		return 0
	}
	return strings.Count(src, "\n") + 1
}

type kernelCrashDiagnosis struct {
	fatalKind       string
	likelyRootCause string
	avoidance       string
}

func diagnoseKernelPanic(message string) kernelCrashDiagnosis {
	lower := strings.ToLower(message)
	// Reader and name failures come first: their messages quote the offending
	// token, which may itself read "string" or "index".
	switch {
	case strings.HasPrefix(lower, "parse error"):
		return kernelCrashDiagnosis{
			fatalKind:       "source_compile_failure",
			likelyRootCause: "the input is not plain Form: unbalanced text, or a raw BML/section-bearing unit the kernel does not lower",
			avoidance:       "prepare the unit with \"./fkwu --closure <unit> <out>\" from the repo root, then repair the reported source coordinate",
		}
	case strings.Contains(lower, "unbound"):
		return kernelCrashDiagnosis{
			fatalKind:       "name_resolution_error",
			likelyRootCause: "a recipe or route manifest referenced a name its defining unit never brought: the kernel follows no prelude, import or home-index link, or the input is not plain Form",
			avoidance:       "hand the kernel a unit's whole closure: prepare it with \"./fkwu --closure <unit> <out>\" from the repo root",
		}
	case strings.Contains(lower, "as_str") ||
		strings.Contains(lower, "argstr") ||
		strings.Contains(lower, "expected string") ||
		strings.Contains(lower, "string"):
		return kernelCrashDiagnosis{
			fatalKind:       "type_contract_violation",
			likelyRootCause: "a Form/native recipe passed a non-string value to a string-only primitive",
			avoidance:       "guard with value_kind, convert with value_str, or use null-safe JSON constructors before calling string primitives",
		}
	case strings.Contains(lower, "as_int") ||
		strings.Contains(lower, "argint") ||
		strings.Contains(lower, "expected int") ||
		strings.Contains(lower, "wrong primitive kind"):
		return kernelCrashDiagnosis{
			fatalKind:       "type_contract_violation",
			likelyRootCause: "a Form/native recipe passed a value with the wrong primitive kind to a typed host boundary",
			avoidance:       "validate the value kind before the native call, or route through an explicit conversion recipe",
		}
	case strings.Contains(lower, "arity") ||
		strings.Contains(lower, "wants") ||
		strings.Contains(lower, "argument"):
		return kernelCrashDiagnosis{
			fatalKind:       "arity_contract_violation",
			likelyRootCause: "a closure or native was called with a different argument count than its declaration accepts",
			avoidance:       "align the call site with the function signature or add an adapter recipe at the boundary",
		}
	case strings.Contains(lower, "bounds") ||
		strings.Contains(lower, "range") ||
		strings.Contains(lower, "index"):
		return kernelCrashDiagnosis{
			fatalKind:       "bounds_violation",
			likelyRootCause: "a recipe indexed outside the observed collection/string bounds",
			avoidance:       "check length/bounds before indexing or use a boundary-aware recipe that returns an explicit error value",
		}
	default:
		return kernelCrashDiagnosis{
			fatalKind:       "kernel_panic",
			likelyRootCause: "the kernel crossed an unchecked host-language panic boundary",
			avoidance:       "inspect the trace stack and source excerpt, then move the failing boundary into a checked fatal/error return",
		}
	}
}

func kernelFatalHTTPBody(message string, diagnosis kernelCrashDiagnosis, tracePath string) string {
	trace := tracePath
	if trace == "" {
		trace = "trace unavailable"
	}
	return fmt.Sprintf(
		"fatal[%s]: %s\nlikely_root_cause: %s\navoidance: %s\ntrace: %s\n",
		diagnosis.fatalKind,
		message,
		diagnosis.likelyRootCause,
		diagnosis.avoidance,
		trace,
	)
}

func kernelModeFromArgs(args []string) string {
	if len(args) == 0 {
		return "startup"
	}
	switch args[0] {
	case "--expr":
		return "expr"
	case "--emit-binary":
		return "emit-binary"
	case "--binary":
		return "binary"
	case "--bench":
		return "bench"
	case "--numeric-bench":
		return "numeric-bench"
	case "trace":
		return "trace"
	case "serve":
		return "serve"
	default:
		return "source"
	}
}

// formStackInnermostFirst — reverse the live stack for the trace record
// so the first entry answers "where exactly" (sibling to Rust's form_stack).
func formStackInnermostFirst(stack []string) []string {
	out := make([]string, 0, len(stack))
	for i := len(stack) - 1; i >= 0; i-- {
		out = append(out, stack[i])
	}
	return out
}

func writeKernelCrashTrace(args []string, src string, recovered any, formStack []string) string {
	return writeKernelCrashTraceWithContext(args, src, recovered, "", "", formStack)
}

func writeKernelCrashTraceWithContext(args []string, src string, recovered any, sourceLabel string, operation string, formStack []string) string {
	dir := filepath.Join(".cache", "form-kernel-go")
	if err := os.MkdirAll(dir, 0755); err != nil {
		dir = os.TempDir()
	}
	path := filepath.Join(
		dir,
		fmt.Sprintf("crash-%s-%d.json", time.Now().UTC().Format("20060102T150405Z"), os.Getpid()),
	)
	tailStart := len(src) - 2000
	if tailStart < 0 {
		tailStart = 0
	}
	message := fmt.Sprint(recovered)
	diagnosis := diagnoseKernelPanic(message)
	report := map[string]any{
		"when_utc":          time.Now().UTC().Format(time.RFC3339Nano),
		"pid":               os.Getpid(),
		"mode":              kernelModeFromArgs(args),
		"args":              args,
		"fatal_kind":        diagnosis.fatalKind,
		"fatal_message":     message,
		"panic":             message,
		"likely_root_cause": diagnosis.likelyRootCause,
		"avoidance":         diagnosis.avoidance,
		"source_label":      sourceLabel,
		"operation":         operation,
		"source_bytes":      len(src),
		"source_line_count": kernelSourceLineCount(src),
		"source_head":       kernelSourceExcerpt(src, 2000),
		"form_stack":        formStackInnermostFirst(formStack),
		"source_tail":       src[tailStart:],
		"go_stack":          string(debug.Stack()),
	}
	data, err := json.MarshalIndent(report, "", "  ")
	if err != nil {
		return ""
	}
	if err := os.WriteFile(path, append(data, '\n'), 0644); err != nil {
		return ""
	}
	return path
}

// formFilePart is one argv file's place in the joined source: the global line
// its first line lands on.
type formFilePart struct {
	path      string
	startLine uint32
}

// readFormFiles reads plain Form files in argv order as bytes and joins them
// with one newline. A kernel follows no directive and lowers nothing: whoever
// hands a unit over prepares it ("./fkwu --closure <unit> <out>").
func readFormFiles(paths []string) (string, []formFilePart, error) {
	parts := make([]string, 0, len(paths))
	lineMap := make([]formFilePart, 0, len(paths))
	nextLine := uint32(1)
	for _, path := range paths {
		b, err := os.ReadFile(path)
		if err != nil {
			return "", nil, fmt.Errorf("read %s: %v", path, err)
		}
		text := string(b)
		lineMap = append(lineMap, formFilePart{path: path, startLine: nextLine})
		// +1 for the join newline that opens the next file's first line
		nextLine += uint32(strings.Count(text, "\n")) + 1
		parts = append(parts, text)
	}
	return strings.Join(parts, "\n"), lineMap, nil
}

// readFormRoot reads src as one unit, attributing each form to its file's line.
func (k *Kernel) readFormRoot(src string, lineMap []formFilePart) NodeID {
	for _, part := range lineMap {
		k.readingFiles = append(k.readingFiles, readingPart{FileID: k.internName(part.path), StartLine: part.startLine})
	}
	root := readRootFromSource(k, src)
	k.readingFiles = nil
	return root
}

func main() {
	args := os.Args[1:]
	if len(args) == 0 {
		fmt.Fprintln(os.Stderr, "usage: form-kernel-go <file.fk> [more.fk ...] | --binary file.fkb | --emit-binary out.fkb file.fk... | --expr \"...\" | --bench | --numeric-bench | trace ... | serve --port 18080 <routes.fk...>")
		os.Exit(2)
	}

	var src string
	var crashK *Kernel

	// Catch parse-time and walk-time panics and convert them to clean error
	// output. The trace file keeps the host stack and source excerpt for
	// backtracking without making Form authors read a host runtime dump first.
	defer func() {
		if r := recover(); r != nil {
			fmt.Fprintf(os.Stderr, "form-kernel-go: %v\n", r)
			var stack []string
			if crashK != nil {
				// Raw frames render on this crash path, the read site.
				for _, f := range crashK.formStack {
					stack = append(stack, crashK.renderFormFrame(f))
				}
				// The Form-level call chain live at the crash, innermost
				// first — the line that produced the fatal is the innermost
				// attributed frame.
				if display := crashK.formStackDisplay(16); display != "" {
					fmt.Fprintf(os.Stderr, "form-kernel-go: form stack: %s\n", display)
				}
			}
			if tracePath := writeKernelCrashTrace(args, src, r, stack); tracePath != "" {
				fmt.Fprintf(os.Stderr, "form-kernel-go: crash trace: %s\n", tracePath)
			}
			os.Exit(1)
		}
	}()

	if args[0] == "--bench" {
		runBench()
		return
	}

	if args[0] == "--numeric-bench" {
		runNumericBench()
		return
	}

	if args[0] == "trace" {
		os.Exit(cliTrace(args[1:]))
	}

	if args[0] == "serve" {
		os.Exit(cliServe(args[1:]))
	}

	if args[0] == "--binary" {
		if len(args) < 2 {
			fmt.Fprintln(os.Stderr, "--binary requires a path")
			os.Exit(2)
		}
		b, err := os.ReadFile(args[1])
		if err != nil {
			fmt.Fprintf(os.Stderr, "read %s: %v\n", args[1], err)
			os.Exit(1)
		}
		k := NewKernel()
		root, err := deserializeArtifact(k, b)
		if err != nil {
			fmt.Fprintf(os.Stderr, "form-kernel-go: %v\n", err)
			os.Exit(1)
		}
		fmt.Println(k.walkUnit(root, NewFrame(nil)).String())
		return
	}

	files := args
	switch args[0] {
	case "--emit-binary":
		if len(args) < 3 {
			fmt.Fprintln(os.Stderr, "--emit-binary requires an output path and one or more .fk files")
			os.Exit(2)
		}
		files = args[2:]
	case "--expr":
		if len(args) < 2 {
			fmt.Fprintln(os.Stderr, "--expr requires an argument")
			os.Exit(2)
		}
		src, files = args[1], nil
	}
	var lineMap []formFilePart
	if files != nil {
		joined, parts, err := readFormFiles(files)
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
		src, lineMap = joined, parts
	}

	k := NewKernel()
	crashK = k
	root := k.readFormRoot(src, lineMap)
	if args[0] == "--emit-binary" {
		if err := os.WriteFile(args[1], serializeArtifact(k, root), 0644); err != nil {
			fmt.Fprintf(os.Stderr, "write %s: %v\n", args[1], err)
			os.Exit(1)
		}
		return
	}
	fmt.Println(k.walkUnit(root, NewFrame(nil)).String())
}

// cliTrace — run with arm-dispatch tracing enabled. Emits a JSON report
// with the result, elapsed time, and the per-arm dispatch counts including
// native Blueprint attribution. Sibling-parity with the Rust kernel's
// trace subcommand.
func cliTrace(args []string) int {
	if len(args) == 0 {
		fmt.Fprintln(os.Stderr, "usage: form-kernel-go trace [--expr \"...\" | <file.fk>]")
		return 2
	}
	var src string
	var lineMap []formFilePart
	if args[0] == "--expr" {
		if len(args) < 2 {
			fmt.Fprintln(os.Stderr, "--expr requires an argument")
			return 2
		}
		src = args[1]
	} else {
		joined, parts, err := readFormFiles(args)
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			return 1
		}
		src, lineMap = joined, parts
	}

	k := NewKernel()
	k.Trace = newTrace()
	root := k.readFormRoot(src, lineMap)
	start := time.Now()
	result := k.walkUnit(root, NewFrame(nil))
	elapsed := time.Since(start)

	report := map[string]interface{}{
		"result":             result.String(),
		"elapsed_us":         elapsed.Microseconds(),
		"elapsed_human":      elapsed.String(),
		"trace":              k.Trace.toJSON(),
		"framebuffer_counts": k.framebufferSourceCounts(),
		"framebuffer_events": k.framebufferEvents(),
	}
	out, _ := json.MarshalIndent(report, "", "  ")
	fmt.Println(string(out))
	return 0
}

// --- Native implementations — same recursive shape as the Form versions.
// `opaque` is an //go:noinline barrier; wrapping the recursive call's
// argument prevents Go from folding the whole computation when inputs
// are compile-time constants. Without it, pure functions get partially
// folded and the "native" column measures register loads, not work.

//go:noinline
func opaque(n int64) int64 { return n }

func nativeFib(n int64) int64 {
	if n <= 1 {
		return n
	}
	return nativeFib(opaque(n-1)) + nativeFib(opaque(n-2))
}

func nativeFact(n int64) int64 {
	if n <= 1 {
		return 1
	}
	return n * nativeFact(opaque(n-1))
}

func nativeSum(n, acc int64) int64 {
	if n == 0 {
		return acc
	}
	return nativeSum(opaque(n-1), opaque(acc+n))
}

func nativeAck(m, n int64) int64 {
	if m == 0 {
		return n + 1
	}
	if n == 0 {
		return nativeAck(opaque(m-1), 1)
	}
	return nativeAck(opaque(m-1), nativeAck(m, opaque(n-1)))
}

// runBench — three-column output: native compile, kernel walk, overhead.
func runBench() {
	cases := []struct {
		name        string
		src         string
		nativeIters int
		native      func() int64
	}{
		{"fib28",
			`(do (defn fib (n) (if (le n 1) n (add (fib (sub n 1)) (fib (sub n 2))))) (fib 28))`,
			100, func() int64 { return nativeFib(28) }},
		{"fact12",
			`(do (defn fact (n) (if (le n 1) 1 (mul n (fact (sub n 1))))) (fact 12))`,
			500000, func() int64 { return nativeFact(12) }},
		{"sum1000",
			`(do (defn sum (n acc) (if (eq n 0) acc (sum (sub n 1) (add acc n)))) (sum 1000 0))`,
			50000, func() int64 { return nativeSum(1000, 0) }},
		{"ackermann",
			`(do (defn ack (m n) (if (eq m 0) (add n 1) (if (eq n 0) (ack (sub m 1) 1) (ack (sub m 1) (ack m (sub n 1)))))) (ack 3 6))`,
			100, func() int64 { return nativeAck(3, 6) }},
	}

	const kernelIters = 5

	fmt.Printf("%-12s %-12s %-14s %-14s %s\n", "workload", "result", "native", "kernel", "overhead")
	for _, c := range cases {
		// Native timing
		start := time.Now()
		var nativeResult int64
		for i := 0; i < c.nativeIters; i++ {
			nativeResult = c.native()
		}
		nativeDur := time.Since(start) / time.Duration(c.nativeIters)

		// Kernel timing — fresh kernel per case so intern table starts clean
		k := NewKernel()
		toks := tokenizeSexp(c.src)
		root, _ := k.readSexpr(toks, 0)
		env := NewFrame(nil)
		start = time.Now()
		var kernelResult Value
		for i := 0; i < kernelIters; i++ {
			kernelResult = k.walk(root, env)
		}
		kernelDur := time.Since(start) / kernelIters

		overhead := float64(kernelDur) / float64(nativeDur)
		fmt.Printf("%-12s %-12s %-14s %-14s %.0f×\n",
			c.name,
			kernelResult.String(),
			nativeDur,
			kernelDur,
			overhead,
		)
		_ = nativeResult // silence unused-write warning for the loop's last value
	}
}

// ---------------------------------------------------------------------------
// Form binary artifact format helpers
// ---------------------------------------------------------------------------
// Per-node serialization is tagged. Leaves store their local 4-tuple value.
// Composites store the full category node followed by children. That keeps
// temporary, unregistered blueprint/recipe categories scoped to the artifact
// shape instead of treating their context-local NodeID numbers as global.
// Sibling of the Rust/TS binary artifact helpers. Same byte layout.
const (
	formBinaryLeaf      uint32 = 0
	formBinaryComposite uint32 = 1
	// FLOAT64 carries its VALUE, not its index. A float64 trivial NodeID's
	// Inst is a per-kernel f64s-table index — meaningless in another kernel.
	// So a float node serializes as [formBinaryFloat64][8 bytes IEEE-754
	// little-endian] and each kernel re-interns the value on read (fresh
	// local index). The trivial float type tag (FLOAT64 = 7 three-way) never
	// rides the wire either: the value travels in bytes, not the index nor
	// the local type-tag, so the .fkb stays portable regardless of how each
	// kernel numbers its trivial types.
	formBinaryFloat64 uint32 = 2
	// INT64 carries its VALUE, not its index — the same reasoning as FLOAT64.
	// A TrivInt64 NodeID's Inst is a per-kernel i64s-table index, meaningless
	// in another kernel, so an int64 node serializes as [formBinaryInt64][8
	// bytes signed little-endian] and each kernel re-interns on read.
	formBinaryInt64 uint32 = 3

	// The binary door accepts bounded artifacts only. These limits are shared
	// by the Go, Rust, TypeScript, and Python readers so a hostile count or
	// recursive shape cannot turn one sibling into an allocator or stack-crash
	// oracle while the others reject it.
	formBinaryMaxBytes       = 64 << 20
	formBinaryMaxStrings     = 262144
	formBinaryMaxStringBytes = 32 << 20
	formBinaryMaxChildren    = 262144
	formBinaryMaxNodes       = 1000000
	formBinaryMaxDepth       = 256
)

type formBinaryDecodeBudget struct {
	nodes uint64
}

func (b *formBinaryDecodeBudget) enter(depth int) error {
	if depth > formBinaryMaxDepth {
		return fmt.Errorf("form binary: maximum node depth exceeded")
	}
	b.nodes++
	if b.nodes > formBinaryMaxNodes {
		return fmt.Errorf("form binary: maximum node count exceeded")
	}
	return nil
}

func pushU32(bytes []byte, v uint32) []byte {
	return append(bytes, byte(v>>24), byte(v>>16), byte(v>>8), byte(v))
}

func readU32(bytes []byte, pos int) (uint32, int, error) {
	if pos < 0 || pos > len(bytes) || len(bytes)-pos < 4 {
		return 0, pos, fmt.Errorf("form binary: truncated u32")
	}
	v := (uint32(bytes[pos]) << 24) |
		(uint32(bytes[pos+1]) << 16) |
		(uint32(bytes[pos+2]) << 8) |
		uint32(bytes[pos+3])
	return v, pos + 4, nil
}

// pushF64LE — append an IEEE-754 f64 as 8 little-endian bytes (the payload of
// a formBinaryFloat64 node). Sibling parity with Rust/TS little-endian writers.
func pushF64LE(bytes []byte, f float64) []byte {
	bits := math.Float64bits(f)
	return append(bytes,
		byte(bits), byte(bits>>8), byte(bits>>16), byte(bits>>24),
		byte(bits>>32), byte(bits>>40), byte(bits>>48), byte(bits>>56))
}

func readF64LE(bytes []byte, pos int) (float64, int, error) {
	if pos < 0 || pos > len(bytes) || len(bytes)-pos < 8 {
		return 0, pos, fmt.Errorf("form binary: truncated float64")
	}
	bits := uint64(bytes[pos]) |
		uint64(bytes[pos+1])<<8 |
		uint64(bytes[pos+2])<<16 |
		uint64(bytes[pos+3])<<24 |
		uint64(bytes[pos+4])<<32 |
		uint64(bytes[pos+5])<<40 |
		uint64(bytes[pos+6])<<48 |
		uint64(bytes[pos+7])<<56
	return math.Float64frombits(bits), pos + 8, nil
}

// pushI64LE / readI64LE — a signed int64 as 8 little-endian bytes (the payload
// of a formBinaryInt64 node). Sibling parity with Rust/TS little-endian writers.
func pushI64LE(bytes []byte, n int64) []byte {
	u := uint64(n)
	return append(bytes,
		byte(u), byte(u>>8), byte(u>>16), byte(u>>24),
		byte(u>>32), byte(u>>40), byte(u>>48), byte(u>>56))
}

func readI64LE(bytes []byte, pos int) (int64, int, error) {
	if pos < 0 || pos > len(bytes) || len(bytes)-pos < 8 {
		return 0, pos, fmt.Errorf("form binary: truncated int64")
	}
	u := uint64(bytes[pos]) |
		uint64(bytes[pos+1])<<8 |
		uint64(bytes[pos+2])<<16 |
		uint64(bytes[pos+3])<<24 |
		uint64(bytes[pos+4])<<32 |
		uint64(bytes[pos+5])<<40 |
		uint64(bytes[pos+6])<<48 |
		uint64(bytes[pos+7])<<56
	return int64(u), pos + 8, nil
}

type formBinaryStringTable struct {
	strings []string
	indexes map[uint32]uint32
}

func collectArtifactStrings(k *Kernel, nid NodeID, table *formBinaryStringTable) {
	if r, ok := k.byID[nid]; ok {
		collectArtifactStrings(k, r.Category, table)
		for _, c := range r.Children {
			collectArtifactStrings(k, c, table)
		}
		return
	}
	if nid.Level == LevelTrivial && nid.Type == TrivString {
		if int(nid.Inst) >= len(k.strs) {
			panic(fmt.Sprintf("form binary: bad string index %d", nid.Inst))
		}
		if _, ok := table.indexes[nid.Inst]; !ok {
			table.indexes[nid.Inst] = uint32(len(table.strings))
			table.strings = append(table.strings, k.strs[nid.Inst])
		}
	}
}

func serializeNidWithStrings(k *Kernel, nid NodeID, bytes []byte, table *formBinaryStringTable) []byte {
	if r, ok := k.byID[nid]; ok {
		bytes = pushU32(bytes, formBinaryComposite)
		bytes = serializeNidWithStrings(k, r.Category, bytes, table)
		bytes = pushU32(bytes, uint32(len(r.Children)))
		for _, c := range r.Children {
			bytes = serializeNidWithStrings(k, c, bytes, table)
		}
		return bytes
	}
	if nid.Level == LevelTrivial && nid.Type == TrivFloat64 {
		bytes = pushU32(bytes, formBinaryFloat64)
		bytes = pushF64LE(bytes, k.decodeFloat64(nid.Inst))
		return bytes
	}
	if nid.Level == LevelTrivial && nid.Type == TrivInt64 {
		bytes = pushU32(bytes, formBinaryInt64)
		bytes = pushI64LE(bytes, k.decodeInt64(nid.Inst))
		return bytes
	}
	bytes = pushU32(bytes, formBinaryLeaf)
	bytes = pushU32(bytes, nid.Pkg)
	bytes = pushU32(bytes, nid.Level)
	bytes = pushU32(bytes, nid.Type)
	if nid.Level == LevelTrivial && nid.Type == TrivString {
		local, ok := table.indexes[nid.Inst]
		if !ok {
			panic(fmt.Sprintf("form binary: missing local string index %d", nid.Inst))
		}
		bytes = pushU32(bytes, local)
	} else {
		bytes = pushU32(bytes, nid.Inst)
	}
	return bytes
}

func deserializeNidWithStringsV1(k *Kernel, bytes []byte, pos int, stringsTable []string, scope uint32, budget *formBinaryDecodeBudget, depth int) (NodeID, int, error) {
	if err := budget.enter(depth); err != nil {
		return NodeID{}, pos, err
	}
	var pkg, level, ty, inst, count uint32
	var err error
	if pkg, pos, err = readU32(bytes, pos); err != nil {
		return NodeID{}, pos, err
	}
	if level, pos, err = readU32(bytes, pos); err != nil {
		return NodeID{}, pos, err
	}
	if ty, pos, err = readU32(bytes, pos); err != nil {
		return NodeID{}, pos, err
	}
	if inst, pos, err = readU32(bytes, pos); err != nil {
		return NodeID{}, pos, err
	}
	if count, pos, err = readU32(bytes, pos); err != nil {
		return NodeID{}, pos, err
	}
	if count > formBinaryMaxChildren {
		return NodeID{}, pos, fmt.Errorf("form binary: maximum child count exceeded")
	}
	if count == 0 {
		if level == LevelTrivial && ty == TrivString {
			if int(inst) >= len(stringsTable) {
				return NodeID{}, pos, fmt.Errorf("form binary: bad string index %d", inst)
			}
			return k.internString(stringsTable[inst]), pos, nil
		}
		return k.remapImportedLeaf(scope, NodeID{Pkg: pkg, Level: level, Type: ty, Inst: inst}), pos, nil
	}
	category := NodeID{Pkg: pkg, Level: level, Type: ty, Inst: inst}
	if level == LevelTrivial && ty == TrivString {
		if int(inst) >= len(stringsTable) {
			return NodeID{}, pos, fmt.Errorf("form binary: bad string index %d", inst)
		}
		category = k.internString(stringsTable[inst])
	}
	children := make([]NodeID, int(count))
	for i := uint32(0); i < count; i++ {
		var c NodeID
		c, pos, err = deserializeNidWithStringsV1(k, bytes, pos, stringsTable, scope, budget, depth+1)
		if err != nil {
			return NodeID{}, pos, err
		}
		children[i] = c
	}
	return k.intern(category, children), pos, nil
}

var formBinaryMagicV1 = []byte("FORMBIN1")
var formBinaryMagic = []byte("FORMBIN2")

func serializeArtifact(k *Kernel, root NodeID) []byte {
	table := &formBinaryStringTable{indexes: make(map[uint32]uint32)}
	collectArtifactStrings(k, root, table)
	bytes := append([]byte{}, formBinaryMagic...)
	bytes = pushU32(bytes, uint32(len(table.strings)))
	for _, s := range table.strings {
		raw := []byte(s)
		bytes = pushU32(bytes, uint32(len(raw)))
		bytes = append(bytes, raw...)
	}
	return serializeNidWithStrings(k, root, bytes, table)
}

func deserializeArtifact(k *Kernel, bytes []byte) (NodeID, error) {
	if len(bytes) > formBinaryMaxBytes {
		return NodeID{}, fmt.Errorf("form binary: maximum artifact size exceeded")
	}
	v1 := len(bytes) >= len(formBinaryMagicV1) && string(bytes[:len(formBinaryMagicV1)]) == string(formBinaryMagicV1)
	v2 := len(bytes) >= len(formBinaryMagic) && string(bytes[:len(formBinaryMagic)]) == string(formBinaryMagic)
	if !v1 && !v2 {
		return NodeID{}, fmt.Errorf("form binary: bad magic")
	}
	pos := len(formBinaryMagic)
	if v1 {
		pos = len(formBinaryMagicV1)
	}
	stringCount, pos, err := readU32(bytes, pos)
	if err != nil {
		return NodeID{}, err
	}
	if stringCount > formBinaryMaxStrings {
		return NodeID{}, fmt.Errorf("form binary: maximum string count exceeded")
	}
	stringsTable := make([]string, int(stringCount))
	totalStringBytes := uint64(0)
	for i := uint32(0); i < stringCount; i++ {
		n, next, err := readU32(bytes, pos)
		if err != nil {
			return NodeID{}, err
		}
		pos = next
		totalStringBytes += uint64(n)
		if totalStringBytes > formBinaryMaxStringBytes {
			return NodeID{}, fmt.Errorf("form binary: maximum string bytes exceeded")
		}
		if int(n) > len(bytes)-pos {
			return NodeID{}, fmt.Errorf("form binary: truncated string")
		}
		end := pos + int(n)
		raw := bytes[pos:end]
		if !utf8.Valid(raw) {
			return NodeID{}, fmt.Errorf("form binary: invalid utf8")
		}
		stringsTable[i] = string(raw)
		pos = end
	}
	var root NodeID
	var end int
	budget := &formBinaryDecodeBudget{}
	if v1 {
		scope := k.nextImportScope()
		root, end, err = deserializeNidWithStringsV1(k, bytes, pos, stringsTable, scope, budget, 0)
	} else {
		scope := k.nextImportScope()
		root, end, err = deserializeFormbinDepth(k, bytes, pos, stringsTable, scope)
	}
	if err != nil {
		return NodeID{}, err
	}
	if end != len(bytes) {
		return NodeID{}, fmt.Errorf("form binary: trailing bytes")
	}
	return root, nil
}
