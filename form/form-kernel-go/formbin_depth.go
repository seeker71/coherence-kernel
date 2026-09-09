package main

// The proof carrier marshals bytes/NodeIDs and measures its host. Traversal,
// continuations, depth attention and slice decisions live in formbin-depth.bml.
import (
	"bufio"
	"encoding/json"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strconv"
	"strings"
	"time"
)

func formbinPipeLine(reader *bufio.Reader) (string, error) {
	line, err := reader.ReadString('\n')
	return strings.TrimSuffix(line, "\n"), err
}

func formbinPipeInts(reader *bufio.Reader, count int) ([]int64, error) {
	values := make([]int64, count)
	for i := range values {
		line, err := formbinPipeLine(reader)
		if err != nil {
			return nil, fmt.Errorf("form binary: native pipe: %w", err)
		}
		values[i], err = strconv.ParseInt(line, 10, 64)
		if err != nil {
			return nil, fmt.Errorf("form binary: native pipe integer: %w", err)
		}
	}
	return values, nil
}

func deserializeFormbinDepth(k *Kernel, body []byte, start int, table []string, scope uint32) (rootNode NodeID, end int, decodeErr error) {
	started := time.Now()
	trace, err := os.CreateTemp("", "formbin-depth-*.jsonl")
	if err != nil {
		return NodeID{}, start, fmt.Errorf("form binary: attention trace: %w", err)
	}
	defer trace.Close()
	fmt.Fprintf(os.Stderr, "form binary: adaptive depth evidence=%s\n", trace.Name())
	encoder := json.NewEncoder(trace)
	completed := false
	defer func() {
		phase, reason := "interrupted", "decode did not complete"
		if completed {
			phase, reason = "complete", ""
		} else if decodeErr != nil {
			phase, reason = "error", decodeErr.Error()
		}
		if err := encoder.Encode(map[string]any{"schema": "formbin-depth-attention-v2", "phase": phase, "engine": "fkwu", "cursor": end, "input_bytes": len(body), "decoder_elapsed_ms": time.Since(started).Milliseconds(), "error": reason}); err != nil && decodeErr == nil {
			decodeErr = fmt.Errorf("form binary: final attention trace: %w", err)
		}
	}()
	if err := encoder.Encode(map[string]any{"schema": "formbin-depth-attention-v2", "phase": "starting", "engine": "fkwu", "cursor": start, "input_bytes": len(body), "started_unix_ms": started.UnixMilli()}); err != nil {
		return NodeID{}, start, err
	}
	cwd, err := os.Getwd()
	if err != nil {
		return NodeID{}, start, err
	}
	door, err := resolveFormImport(filepath.Join(cwd, ".formbin-reader"), "observe/formbin-depth-native-run.fk")
	if err != nil {
		if executable, e := os.Executable(); e == nil {
			door, err = resolveFormImport(executable, "observe/formbin-depth-native-run.fk")
		}
	}
	if err != nil {
		return NodeID{}, start, err
	}
	input, err := os.CreateTemp("", "formbin-native-*.fkb")
	if err != nil {
		return NodeID{}, start, err
	}
	defer os.Remove(input.Name())
	if _, err = input.Write(body); err != nil {
		input.Close()
		return NodeID{}, start, err
	}
	if err = input.Close(); err != nil {
		return NodeID{}, start, err
	}
	sourceReady := time.Now()
	root := filepath.Dir(filepath.Dir(door))
	command := exec.Command(filepath.Join(root, "fkwu"), door)
	command.Dir = root
	command.Stderr = os.Stderr
	output, err := command.StdoutPipe()
	if err != nil {
		return NodeID{}, start, err
	}
	feedback, err := command.StdinPipe()
	if err != nil {
		return NodeID{}, start, err
	}
	if err = command.Start(); err != nil {
		return NodeID{}, start, err
	}
	finished := false
	defer func() {
		feedback.Close()
		if !finished {
			command.Process.Kill()
			command.Wait()
		}
	}()
	reader := bufio.NewReader(output)
	exchange := time.Now().UnixMilli()
	if _, err = fmt.Fprintf(feedback, "%s\n%d\n%d\n", input.Name(), start, exchange); err != nil {
		return NodeID{}, start, err
	}
	values := make([]NodeID, 0)
	initial, err := formbinPipeInts(reader, 3)
	if err != nil {
		return NodeID{}, start, err
	}
	quantum, watermark := initial[0], initial[1]
	setupMS := time.Since(started).Milliseconds()
	var before, after runtime.MemStats
	for round := 0; ; round++ {
		roundStarted := time.Now()
		runtime.ReadMemStats(&before)
		began := time.Now()
		state, err := formbinPipeInts(reader, 4)
		if err != nil {
			return NodeID{}, start, err
		}
		outcome, err := formbinPipeLine(reader)
		if err != nil {
			return NodeID{}, start, err
		}
		depthTime, err := formbinPipeInts(reader, 2)
		if err != nil {
			return NodeID{}, start, err
		}
		pos, mode := int(state[0]), state[3]
		var carriedErr error
		rows := [][3]int64{}
		for {
			line, err := formbinPipeLine(reader)
			if err != nil {
				return NodeID{}, pos, err
			}
			if line == "" {
				break
			}
			fields := strings.Fields(line)
			if len(fields) != 3 {
				return NodeID{}, pos, fmt.Errorf("form binary: native row width %d", len(fields))
			}
			var row [3]int64
			for i, field := range fields {
				row[i], err = strconv.ParseInt(field, 10, 64)
				if err != nil {
					return NodeID{}, pos, err
				}
			}
			rows = append(rows, row)
		}
		for rowIndex := len(rows) - 1; rowIndex >= 0; rowIndex-- {
			row := rows[rowIndex]
			tag, at, count := row[0], int(row[1]), int(row[2])
			var node NodeID
			switch tag {
			case 1:
				base := len(values) - count - 1
				if base < 0 {
					carriedErr = fmt.Errorf("form binary: native traversal missing operands")
					break
				}
				node = k.intern(values[base], append([]NodeID(nil), values[base+1:]...))
				values = values[:base]
			case 2:
				value, _, e := readF64LE(body, at+4)
				carriedErr = e
				node = k.internTrivialFloat64(value)
			case 3:
				value, _, e := readI64LE(body, at+4)
				carriedErr = e
				node = k.internTrivialInt(value)
			case 0:
				pkg, p, e := readU32(body, at+4)
				carriedErr = e
				level, p, e := readU32(body, p)
				if e != nil {
					carriedErr = e
				}
				ty, p, e := readU32(body, p)
				if e != nil {
					carriedErr = e
				}
				inst, _, e := readU32(body, p)
				if e != nil {
					carriedErr = e
				}
				if level == LevelTrivial && ty == TrivString {
					if int(inst) >= len(table) {
						carriedErr = fmt.Errorf("form binary: bad string index %d", inst)
					} else {
						node = k.internString(table[inst])
					}
				} else {
					node = k.remapImportedLeaf(scope, NodeID{Pkg: pkg, Level: level, Type: ty, Inst: inst})
				}
			default:
				carriedErr = fmt.Errorf("form binary: native traversal unknown carrier tag %d", tag)
			}
			if carriedErr != nil {
				break
			}
			values = append(values, node)
		}
		elapsed := time.Since(began).Milliseconds()
		runtime.ReadMemStats(&after)
		controlStarted := time.Now()
		if _, err := fmt.Fprintf(feedback, "%d\n%d\n", elapsed, after.NumGC-before.NumGC); err != nil {
			return NodeID{}, pos, err
		}
		decision, err := formbinPipeInts(reader, 4)
		controlUS := time.Since(controlStarted).Microseconds()
		if err != nil {
			return NodeID{}, pos, err
		}
		action := decision[1]
		if carriedErr != nil {
			outcome = carriedErr.Error()
		}
		applied := mode < 2 && carriedErr == nil
		if applied {
			if action == 1 {
				runtime.Gosched()
			}
			if _, err := fmt.Fprintln(feedback, "continue"); err != nil {
				return NodeID{}, pos, err
			}
		}
		event := map[string]any{"schema": "formbin-depth-attention-v2", "round": round, "cursor": pos, "input_bytes": len(body), "nodes": state[1], "depth": depthTime[0], "peak_depth": state[2], "elapsed_ms": elapsed, "carrier_heap_alloc_bytes": after.HeapAlloc, "carrier_stack_inuse_bytes": after.StackInuse, "carrier_allocated_bytes": after.TotalAlloc - before.TotalAlloc, "carrier_gc_cycles": after.NumGC - before.NumGC, "quantum": quantum, "watermark": watermark, "attention": decision[0], "offered": []string{"continue", "yield-smaller-slice", "grow-slice"}, "selected": action, "next_quantum": decision[3], "next_watermark": decision[2], "mode": mode, "error": outcome}
		event["applied"] = applied
		event["phase"] = "slice"
		event["exchange"] = filepath.Base(trace.Name())
		event["framebuffer_exchange"] = exchange
		event["carrier_gc_pause_ns"] = after.PauseTotalNs - before.PauseTotalNs
		event["native_slice_ms"] = depthTime[1]
		event["native_pid"] = command.Process.Pid
		event["engine"] = "fkwu"
		event["control_us"] = controlUS
		event["round_before_trace_us"] = time.Since(roundStarted).Microseconds()
		event["decoder_elapsed_ms"] = time.Since(started).Milliseconds()
		if round == 0 {
			event["source_prepare_ms"] = sourceReady.Sub(started).Milliseconds()
			event["setup_ms"] = setupMS
		}
		if err := encoder.Encode(event); err != nil {
			return NodeID{}, pos, fmt.Errorf("form binary: attention trace write: %w", err)
		}
		if carriedErr != nil {
			return NodeID{}, pos, carriedErr
		}
		if mode == 3 {
			return NodeID{}, pos, fmt.Errorf("%s", outcome)
		}
		if mode == 2 {
			if len(values) != 1 {
				return NodeID{}, pos, fmt.Errorf("form binary: native traversal root count %d", len(values))
			}
			err := command.Wait()
			finished = true
			if err != nil {
				return NodeID{}, pos, fmt.Errorf("form binary: native traversal exit: %w", err)
			}
			completed = true
			return values[0], pos, nil
		}
		quantum, watermark = decision[3], decision[2]
	}
}
