package main

import (
	"math"
	"os"
	"testing"
)

// The laws docs/kernel-interface.md pins on the pure core, read through this
// kernel's own reader and walker. Each expectation is the answer fkwu gives,
// or, where fkwu has not reached the law yet, the law itself.

// evalForm walks src as one unit and answers its value, or stopped when the walk stops.
func evalForm(src string) (v Value, stopped bool) {
	defer func() {
		if r := recover(); r != nil {
			stopped = true
		}
	}()
	k := NewKernel()
	return k.walkUnit(readRootFromSource(k, src), NewFrame(nil)), false
}

func wantInt(t *testing.T, src string, want int64) {
	t.Helper()
	v, stopped := evalForm(src)
	if stopped || v.Kind != VInt || v.Int != want {
		t.Errorf("%s: got %v (stopped=%v), want %d", src, v, stopped, want)
	}
}

func wantFloat(t *testing.T, src string, want float64) {
	t.Helper()
	v, stopped := evalForm(src)
	if stopped || v.Kind != VFloat || math.Float64bits(v.Float) != math.Float64bits(want) {
		t.Errorf("%s: got %v (stopped=%v), want %v", src, v, stopped, want)
	}
}

func wantStop(t *testing.T, src string) {
	t.Helper()
	if v, stopped := evalForm(src); !stopped {
		t.Errorf("%s: answered %v, want a stop", src, v)
	}
}

func wantStr(t *testing.T, src string, want string) {
	t.Helper()
	v, stopped := evalForm(src)
	if stopped || v.Kind != VStr || v.Str != want {
		t.Errorf("%s: got %v (stopped=%v), want %q", src, v, stopped, want)
	}
}

// Law 1: integers are 63-bit two's complement, literals included.
func TestIntegersAre63Bit(t *testing.T) {
	wantInt(t, "(add 4611686018427387903 1)", -4611686018427387904)
	wantInt(t, "(mul 3037000499 3037000499)", -5928526807)
	wantInt(t, "(sub 0 4611686018427387904)", -4611686018427387904)
	wantInt(t, "(div -4611686018427387904 -1)", -4611686018427387904)
	wantInt(t, "9223372036854775807", -1)
	wantInt(t, "99999999999999999999", -1457092405402533889)
	wantInt(t, "-4611686018427387905", 4611686018427387903)
}

// Law 2: integer div and mod by zero stop.
func TestIntegerDivideByZeroStops(t *testing.T) {
	wantStop(t, "(div 7 0)")
	wantStop(t, "(mod 7 0)")
}

// Law 5: float mod truncates, the sign of the dividend, as integer mod does.
func TestFloatModTruncates(t *testing.T) {
	wantFloat(t, "(mod (sub 0.0 7.5) 2.0)", -1.5)
	wantFloat(t, "(mod 7.5 (sub 0.0 2.0))", 1.5)
	wantInt(t, "(mod (sub 0 7) 2)", -1)
}

// Law 6: float_to_int truncates; NaN, out of range and a non-number stop.
func TestFloatToIntStopsWithoutAnInteger(t *testing.T) {
	wantInt(t, "(float_to_int 3.9)", 3)
	wantInt(t, "(float_to_int (sub 0.0 2.9))", -2)
	wantInt(t, "(float_to_int 7)", 7)
	wantStop(t, "(float_to_int (div 0.0 0.0))")
	wantStop(t, "(float_to_int 10000000000000000000.0)")
	wantStop(t, "(float_to_int 4611686018427387904.0)")
	wantStop(t, `(float_to_int "x")`)
}

// Law 7: str_to_float reads one grammar.
func TestStrToFloatReadsOneGrammar(t *testing.T) {
	rows := []struct {
		text string
		want float64
	}{
		{"3.5abc", 3.5}, {" 1.5", 1.5}, {"\t\n+2.5E-1", 0.25}, {"0x10", 0}, {"inf", 0}, {"nan", 0},
		{"1e", 1}, {"1e+3x", 1000}, {".5", 0.5}, {"5.", 5}, {"-", 0}, {".", 0}, {"abc", 0},
		{"1e400", math.Inf(1)},
	}
	for _, r := range rows {
		if got := decimalPrefixFloat(r.text); math.Float64bits(got) != math.Float64bits(r.want) {
			t.Errorf("decimalPrefixFloat(%q) = %v, want %v", r.text, got, r.want)
		}
	}
	wantFloat(t, `(str_to_float "3.5abc")`, 3.5)
	wantStop(t, "(str_to_float 5)")
}

// Law 8: str_byte_at outside the string answers -1.
func TestStrByteAtOutsideAnswersMinusOne(t *testing.T) {
	wantInt(t, `(str_byte_at "abc" 5)`, -1)
	wantInt(t, `(str_byte_at "abc" (sub 0 1))`, -1)
	wantInt(t, `(str_byte_at "abc" 1)`, 98)
	wantStop(t, `(str_find 5 "a" 0)`)
}

// Law 9: one rendering, records and closures included.
func TestValueStrRendersOneWay(t *testing.T) {
	wantStr(t, `(value_str (record_new 0 "x" 1))`, "<record>")
	wantStr(t, `(do (defn f (x) x) (value_str f))`, "<closure>")
	wantStr(t, `(value_str (list 1 (nothing) "a" 2.5))`, "[1, null, a, 2.5]")
	wantStr(t, `(value_str (nothing))`, "")
	wantStr(t, `(value_str (make_nodeid 1 2 12 1))`, "@1.2.12.1")
	if s := (Value{Kind: VList, List: []Value{{Kind: VInt, Int: 1}, {Kind: VNull}}}).String(); s != "[1, nothing]" {
		t.Errorf("print rendering of a list holding nothing: %q", s)
	}
	// print names a record and a closure by kind, the words value_str uses
	if s := (Value{Kind: VRecord, Rec: &Record{NoBlueprint: true}}).String(); s != "<record>" {
		t.Errorf("print rendering of a record: %q", s)
	}
	if s := (Value{Kind: VClosure, Cl: &Closure{Name: 7}}).String(); s != "<closure>" {
		t.Errorf("print rendering of a closure: %q", s)
	}
}

// The identity constructor keeps fkwu's one range law (native-node-word.bml):
// outside the 64-bit layout it stops, so no two coordinates share an identity.
func TestMakeNodeidRangeLaw(t *testing.T) {
	wantStr(t, `(value_str (make_nodeid 1 2 12 4294967295))`, "@1.2.12.4294967295")
	wantStr(t, `(value_str (make_nodeid 63 8191 4095 0))`, "@63.8191.4095.0")
	wantStop(t, "(make_nodeid 1 2 12 4294967296)")
	wantStop(t, "(make_nodeid 1 2 4096 1)")
	wantStop(t, "(make_nodeid 1 8192 12 1)")
	wantStop(t, "(make_nodeid 64 2 12 1)")
	wantStop(t, "(make_nodeid 1 2 12 (sub 0 1))")
	wantStop(t, "(make_nodeid (sub 0 1) 2 12 1)")
	// the trivial-int lane is the int's own leaf, wide ints included
	wantInt(t, "(eq (make_nodeid 1 1 1 5000000000) (intern_trivial_int 5000000000))", 1)
	wantInt(t, "(node_value (make_nodeid 1 1 1 4611686018427387903))", 4611686018427387903)
}

// The reader reads as fkwu's does: any other backslash stands for itself, and a
// `.` after the digits makes a float with or without fraction digits.
func TestReaderEscapesAndBareDot(t *testing.T) {
	wantInt(t, `(str_len "a\qb")`, 4)
	wantInt(t, `(str_byte_at "a\qb" 1)`, 92)
	wantInt(t, `(str_len "a\x41b")`, 6)
	wantInt(t, `(str_len "a\tb")`, 3)
	wantFloat(t, "5.", 5)
	wantFloat(t, "(add 1. 1)", 2)
}

// A float is not an index or a word: an integer door stops on it, as TS's argInt does.
func TestFloatIsNotAnIndex(t *testing.T) {
	wantStop(t, `(str_byte_at "abc" 1.9)`)
	wantStop(t, "(nth (list 4 5 6) 1.9)")
	wantStop(t, "(byte_to_str 65.7)")
	wantStop(t, "(bxor 1.5 0)")
	wantInt(t, "(float_to_int 1.9)", 1)
}

// Nothing is never a counterfeit: a dead handle or a slice of a file that never was
// answers nothing, an EOF-short slice stays its honest bytes, and the length of
// nothing stops.
func TestAbsenceAnswersNothing(t *testing.T) {
	k := NewKernel()
	call := func(name string, args ...Value) Value {
		return k.natives[k.internName(name)].Fn(k, args)
	}
	if v := call("socket_recv", Value{Kind: VInt, Int: -1}, Value{Kind: VInt, Int: 16}); v.Kind != VNull {
		t.Fatalf("dead-handle recv must answer nothing: kind=%v", v.Kind)
	}
	if v := call("read_file_slice", Value{Kind: VStr, Str: "/nonexistent-pw-probe"}, Value{Kind: VInt, Int: 0}, Value{Kind: VInt, Int: 4}); v.Kind != VNull {
		t.Fatalf("slice of never-was must answer nothing: kind=%v", v.Kind)
	}
	f, err := os.CreateTemp(t.TempDir(), "pw-*.txt")
	if err != nil {
		t.Fatal(err)
	}
	if _, err := f.WriteString("ab"); err != nil {
		t.Fatal(err)
	}
	f.Close()
	if v := call("read_file_slice", Value{Kind: VStr, Str: f.Name()}, Value{Kind: VInt, Int: 0}, Value{Kind: VInt, Int: 8}); v.Kind != VStr || v.Str != "ab" {
		t.Fatalf("EOF-short slice stays honest bytes: kind=%v str=%q", v.Kind, v.Str)
	}
	// a negative offset measures no byte: nothing, for a file that is and one that never was
	if v := call("read_file_slice", Value{Kind: VStr, Str: f.Name()}, Value{Kind: VInt, Int: -5}, Value{Kind: VInt, Int: 1}); v.Kind != VNull {
		t.Fatalf("negative offset must answer nothing: kind=%v str=%q", v.Kind, v.Str)
	}
	if v := call("read_file_slice", Value{Kind: VStr, Str: "/nonexistent-pw-probe"}, Value{Kind: VInt, Int: -1}, Value{Kind: VInt, Int: 4}); v.Kind != VNull {
		t.Fatalf("negative offset into never-was must answer nothing: kind=%v", v.Kind)
	}
	// a slice that asks for no byte is the empty read, as on fkwu, Rust and TS
	if v := call("read_file_slice", Value{Kind: VStr, Str: f.Name()}, Value{Kind: VInt, Int: 1}, Value{Kind: VInt, Int: 0}); v.Kind != VStr || v.Str != "" {
		t.Fatalf("a zero-length slice is the empty read: kind=%v str=%q", v.Kind, v.Str)
	}
	wantStop(t, "(str_len (nothing))")
	wantStop(t, "(len (nothing))")
}

// fkwu's doors the siblings share by name: print answers 0, node_eq is value_eq,
// every float_leaf mode answers nothing.
func TestSharedDoorsAnswerAsFkwu(t *testing.T) {
	wantInt(t, `(print "law-probe")`, 0)
	wantInt(t, `(node_eq "a" "a")`, 1)
	wantInt(t, `(node_eq 3 3)`, 1)
	if v, stopped := evalForm("(float_leaf 9 0)"); stopped || v.Kind != VNull {
		t.Errorf("float_leaf: got %v (stopped=%v), want nothing", v, stopped)
	}
	// _get is fkwu's tag 106: every miss answers 0, a negative index reads the head
	wantInt(t, `(_get (list 4 5) 1)`, 5)
	wantInt(t, `(_get (list 4 5) (sub 0 3))`, 4)
	wantInt(t, `(_get (list 4 5) 9)`, 0)
	wantInt(t, `(_get (list "__dict__" "k" 10) "k")`, 10)
	wantInt(t, `(_get (list "__dict__" "k" 10) "z")`, 0)
	wantInt(t, `(_get (record_new 0 "k" 1) "k")`, 0)
	wantInt(t, `(_get "abc" 1)`, 0)
}

// A released native leaves no native behind: its home in Form answers the name.
func TestReleasedNativesAreGone(t *testing.T) {
	k := NewKernel()
	for _, name := range []string{
		"char_at", "ord", "int_to_str", "str_to_int", "sum", "abs", "range", "intern_node_at",
		"string_byte_fold", "form_table_text", "pow", "min", "max", "math_acos", "print_float",
		"pair_angle", "dominant_band_delta", "string_bytes", "seeded_bytes", "sum_bytes_list",
		"serialize-recipe", "deserialize-recipe", "field_blueprint", "field_evidence",
		"substrate_mark", "substrate_counts", "substrate_release", "substrate_gc", "register_jit",
		"unregister_jit", "jit_aliased?", "_iter", "_in", "_dict_new", "_dict_get", "_dict_set",
		"_dict_has", "_dict_keys", "_dict_values", "_len", "form-error", "_plus", "_list_append",
		"str_line_at", "str_ascii_prefix", "host-read", "host-write", "file_byte_at",
		"framebuffer-observe-active?", "trace", "dylib_call", "value-kind",
	} {
		if _, ok := k.natives[k.internName(name)]; ok {
			t.Errorf("%s is still a native", name)
		}
	}
	if v, stopped := evalForm("(sum (list 1 2))"); !stopped {
		t.Errorf("sum without its core.fk home answered %v; the home must carry it", v)
	}
}

// (let NAME VALUE BODY) binds NAME over BODY alone and answers BODY, as fkwu
// reads it; the name leaves with the body.
func TestLetWithABody(t *testing.T) {
	wantInt(t, "(add 1 (let h 4 (mul h h)))", 17)
	wantInt(t, "(do (defn f (a) (add (let a 10 (add a 1)) a)) (f 5))", 16)
	wantInt(t, "(do (defn g (a) (let a (add a 1) (let a (mul a 2) a))) (g 5))", 12)
	wantInt(t, "(do (defn c (n acc) (if (eq n 0) acc (let m (sub n 1) (c m (add acc 1))))) (c 5000 0))", 5000)
	wantInt(t, "(add 1 (let q 4))", 5)
	wantStop(t, "(add 1 (let q 4 5 6))")
}

// cons onto a word that is not a list makes a pair, as fkwu's cons does: list
// readers end at the tail word and read [h]; tail, eq and value_eq see it.
func TestConsOntoAWordIsAPair(t *testing.T) {
	wantInt(t, "(tail (cons 7 60))", 60)
	wantInt(t, "(len (cons 7 60))", 1)
	wantInt(t, "(nth (cons 7 60) 0)", 7)
	wantStr(t, "(value_str (cons 1 (cons 2 0)))", "[1, 2]")
	wantInt(t, "(tail (tail (cons 1 (cons 2 0))))", 0)
	wantInt(t, "(eq (cons 7 60) (list 7))", 0)
	wantInt(t, "(value_eq (cons 7 60) (cons 7 60))", 1)
	wantInt(t, "(value_eq (cons 7 60) (cons 7 61))", 0)
	wantStr(t, `(tail (cons 7 "s"))`, "s")
	wantStop(t, "(cons 7 (nothing))")
}

// record_has asks whether a value has a field: a value that is not a record has
// none. record_get and record_set on it stop by name.
func TestRecordDoorsOnANonRecord(t *testing.T) {
	wantInt(t, `(record_has 0 "a")`, 0)
	wantInt(t, `(record_has (list) "a")`, 0)
	wantInt(t, `(record_has (record_new 0 "a" 1) "a")`, 1)
	wantStop(t, `(record_get 0 "a")`)
	wantStop(t, `(record_set 0 "a" 1)`)
}

// A byte writer takes a list of ints: a string, a number or a list holding a
// non-int answers -1 and leaves the file as it stood, as fkwu and TS answer.
func TestByteWritersRefuseANonByteList(t *testing.T) {
	path := t.TempDir() + "/bytes"
	if err := os.WriteFile(path, []byte("precious"), 0644); err != nil {
		t.Fatal(err)
	}
	wantInt(t, `(write_file_bytes "`+path+`" "abc")`, -1)
	wantInt(t, `(write_file_bytes "`+path+`" (list 65 1.5))`, -1)
	wantInt(t, `(file_append_bytes "`+path+`" (list 1.5))`, -1)
	wantInt(t, `(file_append_bytes "`+path+`" 7)`, -1)
	if got, _ := os.ReadFile(path); string(got) != "precious" {
		t.Errorf("a refused write touched the file: %q", got)
	}
	wantInt(t, `(write_file_bytes "`+path+`" (list 65 322))`, 2)
	if got, _ := os.ReadFile(path); string(got) != "AB" {
		t.Errorf("an int writes its low byte: %q", got)
	}
}

// round_ndigits counts places: a negative count stops by name, as on fkwu.
func TestRoundNdigitsCountsPlaces(t *testing.T) {
	wantStop(t, "(round_ndigits 1.25 -1)")
	wantFloat(t, "(round_ndigits 2.675 2)", 2.67)
}
