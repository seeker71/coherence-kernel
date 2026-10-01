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
		"framebuffer-observe-active?", "trace", "dylib_call",
	} {
		if _, ok := k.natives[k.internName(name)]; ok {
			t.Errorf("%s is still a native", name)
		}
	}
	if v, stopped := evalForm("(sum (list 1 2))"); !stopped {
		t.Errorf("sum without its core.fk home answered %v; the home must carry it", v)
	}
}
