package main

import (
	"bytes"
	"context"
	"database/sql"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"net/http"
	"net/url"
	"os"
	"path/filepath"
	"sort"
	"strconv"
	"strings"
	"sync"
	"time"

	"github.com/jackc/pgx/v5/pgconn"
	_ "github.com/jackc/pgx/v5/stdlib"
)

const (
	khTagHeader   int64 = 43001
	khTagRequest  int64 = 43002
	khTagResponse int64 = 43003
	khTagRoute    int64 = 43004
	khTagField    int64 = 43008

	headerFormRouter     = "X-Form-Router"
	headerRouteHow       = "X-Form-Route-How"
	headerRouteWhere     = "X-Form-Route-Where"
	headerRouteWhen      = "X-Form-Route-When"
	headerRouteWho       = "X-Form-Route-Who"
	headerNativeInvite   = "X-Form-Native-Invitation"
	headerNativeState    = "X-Form-Native-Invitation-State"
	headerNativeProtocol = "X-Form-Native-Invitation-Protocol"
	headerNativePath     = "X-Form-Native-Invitation-Selected-Path"
	headerNativeDecline  = "X-Form-Native-Invitation-Decline-Signal"
	headerNativeFallback = "X-Form-Native-Invitation-Decline-Header"
	headerFatalKind      = "X-Form-Fatal-Kind"
	headerCrashTrace     = "X-Form-Crash-Trace"
	routeHowNative       = "native-kernel-go"
	routeHowNativeError  = "native-kernel-error"
	routeHowFanout       = "fanout-python"
	nativeInviteValue    = "offered"
	nativeInviteState    = "native-invitation-offered"
	nativeInviteProtocol = "Form/BML route recipe"
	nativeInviteDecline  = "native_invitation_declined"
	maxDecisionHeaderLen = 240
)

var goKernelConfigPath string
var goKernelStartedAt = time.Now().UTC()
var goExternalHTTPClient = &http.Client{}

type volatileCell struct {
	updatedMS int64
	value     Value
}

type volatileCellTable struct {
	mu    sync.Mutex
	cells map[string]volatileCell
}

type pgHandleTable struct {
	mu      sync.Mutex
	next    int64
	handles map[int64]*sql.DB
	lastErr string
}

var goPgHandles = &pgHandleTable{handles: map[int64]*sql.DB{}}
var goVolatileCells = &volatileCellTable{cells: map[string]volatileCell{}}

func volatileCoord(namespace, key string) string {
	return namespace + "\x00" + key
}

func cloneValue(v Value) Value {
	if v.Kind != VList {
		return v
	}
	out := make([]Value, len(v.List))
	for i, item := range v.List {
		out[i] = cloneValue(item)
	}
	v.List = out
	return v
}

func (t *volatileCellTable) put(namespace, key string, value Value) int64 {
	t.mu.Lock()
	defer t.mu.Unlock()
	t.cells[volatileCoord(namespace, key)] = volatileCell{
		updatedMS: time.Now().UnixMilli(),
		value:     cloneValue(value),
	}
	return 1
}

func (t *volatileCellTable) get(namespace, key string) (volatileCell, bool) {
	t.mu.Lock()
	defer t.mu.Unlock()
	cell, ok := t.cells[volatileCoord(namespace, key)]
	if ok {
		cell.value = cloneValue(cell.value)
	}
	return cell, ok
}

func (t *volatileCellTable) delete(namespace, key string) bool {
	t.mu.Lock()
	defer t.mu.Unlock()
	coord := volatileCoord(namespace, key)
	_, ok := t.cells[coord]
	if ok {
		delete(t.cells, coord)
	}
	return ok
}

func (t *volatileCellTable) scanSince(namespace string, sinceMS int64) []Value {
	prefix := namespace + "\x00"
	t.mu.Lock()
	defer t.mu.Unlock()
	out := []Value{}
	for coord, cell := range t.cells {
		if !strings.HasPrefix(coord, prefix) || cell.updatedMS < sinceMS {
			continue
		}
		out = append(out, Value{Kind: VList, List: []Value{
			{Kind: VStr, Str: strings.TrimPrefix(coord, prefix)},
			cloneValue(cell.value),
			{Kind: VInt, Int: cell.updatedMS},
		}})
	}
	return out
}

func (t *volatileCellTable) pruneBefore(namespace string, beforeMS int64) int64 {
	prefix := namespace + "\x00"
	t.mu.Lock()
	defer t.mu.Unlock()
	var removed int64
	for coord, cell := range t.cells {
		if strings.HasPrefix(coord, prefix) && cell.updatedMS < beforeMS {
			delete(t.cells, coord)
			removed++
		}
	}
	return removed
}

func (t *pgHandleTable) setErr(err error) {
	t.mu.Lock()
	defer t.mu.Unlock()
	if err == nil {
		t.lastErr = ""
	} else {
		t.lastErr = formatPGError(err)
	}
}

func formatPGError(err error) string {
	var pgErr *pgconn.PgError
	if errors.As(err, &pgErr) {
		parts := []string{pgErr.Message}
		if pgErr.Detail != "" {
			parts = append(parts, "detail: "+pgErr.Detail)
		}
		if pgErr.Hint != "" {
			parts = append(parts, "hint: "+pgErr.Hint)
		}
		return strings.Join(parts, " | ")
	}
	return err.Error()
}

func (t *pgHandleTable) register(db *sql.DB) int64 {
	t.mu.Lock()
	defer t.mu.Unlock()
	t.next++
	h := t.next
	t.handles[h] = db
	return h
}

func (t *pgHandleTable) lookup(h int64) (*sql.DB, bool) {
	t.mu.Lock()
	defer t.mu.Unlock()
	db, ok := t.handles[h]
	return db, ok
}

func (t *pgHandleTable) close(h int64) bool {
	t.mu.Lock()
	db, ok := t.handles[h]
	if ok {
		delete(t.handles, h)
	}
	t.mu.Unlock()
	if ok {
		_ = db.Close()
	}
	return ok
}

func (k *Kernel) registerHostIONatives() {
	k.registerNative("volatile_cell_put", catCall(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: goVolatileCells.put(argStr(args, 0), argStr(args, 1), args[2])}
	})
	k.registerNative("volatile_cell_get", catAccess(), func(_ *Kernel, args []Value) Value {
		cell, ok := goVolatileCells.get(argStr(args, 0), argStr(args, 1))
		if !ok {
			return Value{Kind: VNull}
		}
		return cell.value
	})
	k.registerNative("volatile_cell_delete", catCall(), func(_ *Kernel, args []Value) Value {
		if goVolatileCells.delete(argStr(args, 0), argStr(args, 1)) {
			return Value{Kind: VInt, Int: 1}
		}
		return Value{Kind: VInt, Int: 0}
	})
	k.registerNative("volatile_cell_scan_since", catAccess(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VList, List: goVolatileCells.scanSince(argStr(args, 0), args[1].Int)}
	})
	k.registerNative("volatile_cell_prune_before", catCall(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VInt, Int: goVolatileCells.pruneBefore(argStr(args, 0), args[1].Int)}
	})
	k.registerNative("repo_root", catAccess(), func(_ *Kernel, _ []Value) Value {
		root, err := findRepoRoot()
		if err != nil {
			return Value{Kind: VStr, Str: ""}
		}
		return Value{Kind: VStr, Str: root}
	})
	k.registerNative("kernel_runtime_name", catCall(), func(_ *Kernel, _ []Value) Value {
		return Value{Kind: VStr, Str: "form-kernel-go"}
	})
	k.registerNative("kernel_started_unix_ms", catCall(), func(_ *Kernel, _ []Value) Value {
		return Value{Kind: VInt, Int: goKernelStartedAt.UnixMilli()}
	})
	k.registerNative("unix_ms_to_iso_utc", catCall(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VStr, Str: time.UnixMilli(args[0].Int).UTC().Format("2006-01-02T15:04:05Z")}
	})
	k.registerNative("uptime_human", catCall(), func(_ *Kernel, args []Value) Value {
		return Value{Kind: VStr, Str: uptimeHuman(args[0].Int)}
	})
	k.registerNative("config_value_or", catCall(), func(_ *Kernel, args []Value) Value {
		config, err := loadKernelConfig()
		if err != nil {
			goPgHandles.setErr(err)
			return Value{Kind: VStr, Str: argStr(args, 1)}
		}
		value, ok := lookupConfigPath(config, argStr(args, 0))
		if !ok {
			return Value{Kind: VStr, Str: argStr(args, 1)}
		}
		switch v := value.(type) {
		case string:
			return Value{Kind: VStr, Str: v}
		case bool:
			return boolInt(v)
		case float64:
			if v == float64(int64(v)) {
				return Value{Kind: VInt, Int: int64(v)}
			}
			return Value{Kind: VFloat, Float: v}
		case nil:
			return Value{Kind: VStr, Str: argStr(args, 1)}
		default:
			return Value{Kind: VStr, Str: fmt.Sprint(v)}
		}
	})
	k.registerNative("config_database_url", catCall(), func(_ *Kernel, _ []Value) Value {
		url, err := loadConfiguredDatabaseURL()
		if err != nil {
			goPgHandles.setErr(err)
			return Value{Kind: VStr, Str: ""}
		}
		return Value{Kind: VStr, Str: url}
	})
	k.registerNative("http_get", catCall(), func(_ *Kernel, args []Value) Value {
		if len(args) < 1 || args[0].Kind != VStr {
			return formHTTPGetResult(0, nil, "", "http_get: url must be a string", 0)
		}
		headers := http.Header{}
		if len(args) > 1 {
			headers = formHTTPHeaderValues(args[1])
		}
		timeout := 10 * time.Second
		if len(args) > 2 {
			timeout = formHTTPTimeout(args[2], timeout)
		}
		return externalHTTPGetValue(argStr(args, 0), headers, timeout)
	})
	k.registerNative("pg_last_error", catCall(), func(_ *Kernel, _ []Value) Value {
		goPgHandles.mu.Lock()
		defer goPgHandles.mu.Unlock()
		return Value{Kind: VStr, Str: goPgHandles.lastErr}
	})
	k.registerNative("pg_connect", catCall(), func(_ *Kernel, args []Value) Value {
		dsn := strings.TrimSpace(argStr(args, 0))
		if !strings.HasPrefix(dsn, "postgres://") && !strings.HasPrefix(dsn, "postgresql://") {
			err := fmt.Errorf("pg_connect: database.url is not a PostgreSQL URL")
			goPgHandles.setErr(err)
			return Value{Kind: VInt, Int: -1}
		}
		ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
		defer cancel()
		db, err := sql.Open("pgx", dsn)
		if err != nil {
			goPgHandles.setErr(err)
			return Value{Kind: VInt, Int: -1}
		}
		db.SetMaxOpenConns(8)
		db.SetMaxIdleConns(8)
		db.SetConnMaxLifetime(30 * time.Minute)
		if err := db.PingContext(ctx); err != nil {
			_ = db.Close()
			goPgHandles.setErr(err)
			return Value{Kind: VInt, Int: -1}
		}
		goPgHandles.setErr(nil)
		return Value{Kind: VInt, Int: goPgHandles.register(db)}
	})
	k.registerNative("pg_ping", catCall(), func(_ *Kernel, args []Value) Value {
		db, ok := goPgHandles.lookup(args[0].Int)
		if !ok {
			goPgHandles.setErr(errors.New("pg_ping: unknown connection handle"))
			return boolInt(false)
		}
		ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
		defer cancel()
		if err := db.PingContext(ctx); err != nil {
			goPgHandles.setErr(err)
			return boolInt(false)
		}
		goPgHandles.setErr(nil)
		return boolInt(true)
	})
	k.registerNative("pg_close", catCall(), func(_ *Kernel, args []Value) Value {
		if goPgHandles.close(args[0].Int) {
			return Value{Kind: VInt, Int: 0}
		}
		return Value{Kind: VInt, Int: -1}
	})
	k.registerNative("pg_exec", catCall(), func(_ *Kernel, args []Value) Value {
		db, ok := goPgHandles.lookup(args[0].Int)
		if !ok {
			goPgHandles.setErr(errors.New("pg_exec: unknown connection handle"))
			return Value{Kind: VInt, Int: -1}
		}
		ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
		defer cancel()
		res, err := db.ExecContext(ctx, argStr(args, 1), formSQLArgs(args, 2)...)
		if err != nil {
			goPgHandles.setErr(err)
			return Value{Kind: VInt, Int: -1}
		}
		n, _ := res.RowsAffected()
		goPgHandles.setErr(nil)
		return Value{Kind: VInt, Int: n}
	})
	k.registerNative("pg_query", catCall(), func(_ *Kernel, args []Value) Value {
		rows, cancel, err := queryRows(args)
		if err != nil {
			if cancel != nil {
				cancel()
			}
			goPgHandles.setErr(err)
			return Value{Kind: VStr, Str: "ERR"}
		}
		defer cancel()
		defer rows.Close()
		cols, err := rows.Columns()
		if err != nil {
			goPgHandles.setErr(err)
			return Value{Kind: VStr, Str: "ERR"}
		}
		var out strings.Builder
		rowIndex := 0
		for rows.Next() {
			values, err := scanSQLRow(rows, len(cols))
			if err != nil {
				goPgHandles.setErr(err)
				return Value{Kind: VStr, Str: "ERR"}
			}
			if rowIndex > 0 {
				out.WriteByte('\n')
			}
			for i, v := range values {
				if i > 0 {
					out.WriteByte('\t')
				}
				out.WriteString(formValueString(dbCellToForm(v)))
			}
			rowIndex++
		}
		if err := rows.Err(); err != nil {
			goPgHandles.setErr(err)
			return Value{Kind: VStr, Str: "ERR"}
		}
		goPgHandles.setErr(nil)
		return Value{Kind: VStr, Str: out.String()}
	})
	k.registerNative("pg_query_rows", catCall(), func(_ *Kernel, args []Value) Value {
		rows, cancel, err := queryRows(args)
		if err != nil {
			if cancel != nil {
				cancel()
			}
			goPgHandles.setErr(err)
			return Value{Kind: VList, List: []Value{}}
		}
		defer cancel()
		defer rows.Close()
		cols, err := rows.Columns()
		if err != nil {
			goPgHandles.setErr(err)
			return Value{Kind: VList, List: []Value{}}
		}
		out := []Value{}
		for rows.Next() {
			values, err := scanSQLRow(rows, len(cols))
			if err != nil {
				goPgHandles.setErr(err)
				return Value{Kind: VList, List: []Value{}}
			}
			row := []Value{{Kind: VStr, Str: "__dict__"}}
			for i, col := range cols {
				row = append(row, Value{Kind: VStr, Str: col}, dbCellToForm(values[i]))
			}
			out = append(out, Value{Kind: VList, List: row})
		}
		if err := rows.Err(); err != nil {
			goPgHandles.setErr(err)
			return Value{Kind: VList, List: []Value{}}
		}
		goPgHandles.setErr(nil)
		return Value{Kind: VList, List: out}
	})
}

func queryRows(args []Value) (*sql.Rows, context.CancelFunc, error) {
	db, ok := goPgHandles.lookup(args[0].Int)
	if !ok {
		return nil, nil, errors.New("pg_query_rows: unknown connection handle")
	}
	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	rows, err := db.QueryContext(ctx, argStr(args, 1), formSQLArgs(args, 2)...)
	if err != nil {
		cancel()
		return nil, nil, err
	}
	return rows, cancel, nil
}

func formSQLArgs(args []Value, idx int) []any {
	if len(args) <= idx || args[idx].Kind != VList {
		return nil
	}
	out := make([]any, 0, len(args[idx].List))
	for _, v := range args[idx].List {
		switch v.Kind {
		case VInt:
			out = append(out, v.Int)
		case VFloat:
			out = append(out, v.Float)
		case VStr:
			out = append(out, v.Str)
		case VNull:
			out = append(out, nil)
		default:
			out = append(out, formValueString(v))
		}
	}
	return out
}

const maxHTTPGetBodyBytes int64 = 25 << 20

func externalHTTPGetValue(rawURL string, headers http.Header, timeout time.Duration) Value {
	start := time.Now()
	ctx, cancel := context.WithTimeout(context.Background(), timeout)
	defer cancel()

	req, err := http.NewRequestWithContext(ctx, http.MethodGet, rawURL, nil)
	if err != nil {
		return formHTTPGetResult(0, nil, "", err.Error(), time.Since(start).Milliseconds())
	}
	for key, vals := range headers {
		if hopByHopHeader(key) {
			continue
		}
		for _, val := range vals {
			req.Header.Add(key, val)
		}
	}

	resp, err := goExternalHTTPClient.Do(req)
	if err != nil {
		return formHTTPGetResult(0, nil, "", err.Error(), time.Since(start).Milliseconds())
	}
	defer resp.Body.Close()

	bodyBytes, readErr := io.ReadAll(io.LimitReader(resp.Body, maxHTTPGetBodyBytes+1))
	errorText := ""
	if readErr != nil {
		errorText = readErr.Error()
	}
	if int64(len(bodyBytes)) > maxHTTPGetBodyBytes {
		bodyBytes = bodyBytes[:maxHTTPGetBodyBytes]
		errorText = fmt.Sprintf("http_get: response body exceeded %d bytes", maxHTTPGetBodyBytes)
	}
	return formHTTPGetResult(resp.StatusCode, resp.Header, string(bodyBytes), errorText, time.Since(start).Milliseconds())
}

func formHTTPHeaderValues(v Value) http.Header {
	headers := http.Header{}
	if v.Kind != VList {
		return headers
	}
	for _, raw := range v.List {
		if raw.Kind != VList || len(raw.List) != 3 {
			continue
		}
		if raw.List[0].Kind != VInt || raw.List[0].Int != khTagHeader ||
			raw.List[1].Kind != VStr || raw.List[2].Kind != VStr {
			continue
		}
		name := strings.TrimSpace(raw.List[1].Str)
		if name == "" {
			continue
		}
		headers.Add(name, raw.List[2].Str)
	}
	return headers
}

func formHTTPTimeout(v Value, fallback time.Duration) time.Duration {
	var ms int64
	switch v.Kind {
	case VInt:
		ms = v.Int
	case VFloat:
		ms = int64(v.Float)
	default:
		return fallback
	}
	if ms <= 0 {
		return fallback
	}
	timeout := time.Duration(ms) * time.Millisecond
	if timeout > 60*time.Second {
		return 60 * time.Second
	}
	return timeout
}

func formHTTPGetResult(status int, headers http.Header, body, errorText string, durationMS int64) Value {
	return Value{Kind: VList, List: []Value{
		{Kind: VStr, Str: "__dict__"},
		{Kind: VStr, Str: "status_code"},
		{Kind: VInt, Int: int64(status)},
		{Kind: VStr, Str: "body"},
		{Kind: VStr, Str: body},
		{Kind: VStr, Str: "error"},
		{Kind: VStr, Str: errorText},
		{Kind: VStr, Str: "duration_ms"},
		{Kind: VInt, Int: durationMS},
		{Kind: VStr, Str: "headers"},
		{Kind: VList, List: formHTTPHeaderList(headers)},
	}}
}

func formHTTPHeaderList(headers http.Header) []Value {
	if headers == nil {
		return []Value{}
	}
	keys := make([]string, 0, len(headers))
	for key := range headers {
		keys = append(keys, key)
	}
	sort.Strings(keys)
	out := []Value{}
	for _, key := range keys {
		values := append([]string{}, headers[key]...)
		sort.Strings(values)
		for _, value := range values {
			out = append(out, Value{Kind: VList, List: []Value{
				{Kind: VInt, Int: khTagHeader},
				{Kind: VStr, Str: key},
				{Kind: VStr, Str: value},
			}})
		}
	}
	return out
}

func scanSQLRow(rows *sql.Rows, n int) ([]any, error) {
	values := make([]any, n)
	dest := make([]any, n)
	for i := range values {
		dest[i] = &values[i]
	}
	if err := rows.Scan(dest...); err != nil {
		return nil, err
	}
	return values, nil
}

func dbCellToForm(v any) Value {
	switch x := v.(type) {
	case nil:
		return Value{Kind: VNull}
	case int64:
		return Value{Kind: VInt, Int: x}
	case int32:
		return Value{Kind: VInt, Int: int64(x)}
	case int:
		return Value{Kind: VInt, Int: int64(x)}
	case float64:
		return Value{Kind: VFloat, Float: x}
	case float32:
		return Value{Kind: VFloat, Float: float64(x)}
	case bool:
		return boolInt(x)
	case string:
		return Value{Kind: VStr, Str: x}
	case []byte:
		return Value{Kind: VStr, Str: string(x)}
	case time.Time:
		return Value{Kind: VStr, Str: x.UTC().Format(time.RFC3339Nano)}
	default:
		return Value{Kind: VStr, Str: fmt.Sprint(x)}
	}
}

func formValueString(v Value) string {
	switch v.Kind {
	case VStr:
		return v.Str
	case VInt:
		return strconv.FormatInt(v.Int, 10)
	case VFloat:
		return formatFloatJS(v.Float)
	case VNull:
		return ""
	default:
		return v.String()
	}
}

func loadConfiguredDatabaseURL() (string, error) {
	config, err := loadKernelConfig()
	if err != nil {
		return "", err
	}
	if db, ok := config["database"].(map[string]any); ok {
		if url, ok := db["url"].(string); ok && strings.TrimSpace(url) != "" {
			return strings.TrimSpace(url), nil
		}
	}
	if url, ok := config["database_url"].(string); ok && strings.TrimSpace(url) != "" {
		return strings.TrimSpace(url), nil
	}
	return "", errors.New("database.url is not configured")
}

func lookupConfigPath(config map[string]any, path string) (any, bool) {
	current := any(config)
	for _, part := range strings.Split(path, ".") {
		if part == "" {
			return nil, false
		}
		obj, ok := current.(map[string]any)
		if !ok {
			return nil, false
		}
		current, ok = obj[part]
		if !ok {
			return nil, false
		}
	}
	return current, true
}

func uptimeHuman(seconds int64) string {
	if seconds < 0 {
		seconds = 0
	}
	days := seconds / 86400
	remainder := seconds % 86400
	hours := remainder / 3600
	remainder %= 3600
	minutes := remainder / 60
	secs := remainder % 60
	if days > 0 {
		return fmt.Sprintf("%dd %dh %dm %ds", days, hours, minutes, secs)
	}
	if hours > 0 {
		return fmt.Sprintf("%dh %dm %ds", hours, minutes, secs)
	}
	if minutes > 0 {
		return fmt.Sprintf("%dm %ds", minutes, secs)
	}
	return fmt.Sprintf("%ds", secs)
}

func loadKernelConfig() (map[string]any, error) {
	merged := map[string]any{}
	// An explicit --config is authoritative and standalone. Never require or
	// silently mix a consumer repository's api/config/api.json into it.
	if overlay := strings.TrimSpace(goKernelConfigPath); overlay != "" {
		if err := mergeConfigFile(merged, overlay); err != nil {
			return nil, fmt.Errorf("explicit kernel config %s: %w", overlay, err)
		}
		return merged, nil
	}
	// Without --config, repository and home files are optional discovery
	// layers. Missing files yield an empty config; malformed present files fail.
	if root, err := findRepoRoot(); err == nil {
		base := filepath.Join(root, "api", "config", "api.json")
		if err := mergeConfigFile(merged, base); err != nil && !os.IsNotExist(err) {
			return nil, err
		}
	}
	if home, err := os.UserHomeDir(); err == nil {
		overlay := filepath.Join(home, ".coherence-network", "config.json")
		if err := mergeConfigFile(merged, overlay); err != nil && !os.IsNotExist(err) {
			return nil, err
		}
		mergeKernelKeys(merged, filepath.Join(home, ".coherence-network", "keys.json"))
	}
	return merged, nil
}

func mergeKernelKeys(dst map[string]any, path string) {
	bytes, err := os.ReadFile(path)
	if err != nil {
		return
	}
	var keys map[string]any
	if err := json.Unmarshal(bytes, &keys); err != nil {
		return
	}
	dst["keys"] = keys
	if token := githubTokenFromKeys(keys); token != "" {
		if current, ok := dst["github_token"].(string); !ok || strings.TrimSpace(current) == "" {
			dst["github_token"] = token
		}
	}
}

func githubTokenFromKeys(keys map[string]any) string {
	if github, ok := keys["github"].(map[string]any); ok {
		for _, key := range []string{"token", "api_token"} {
			if value, ok := github[key].(string); ok && strings.TrimSpace(value) != "" {
				return strings.TrimSpace(value)
			}
		}
	}
	if value, ok := keys["github_token"].(string); ok && strings.TrimSpace(value) != "" {
		return strings.TrimSpace(value)
	}
	return ""
}

func mergeConfigFile(dst map[string]any, path string) error {
	bytes, err := os.ReadFile(path)
	if err != nil {
		return err
	}
	var src map[string]any
	if err := json.Unmarshal(bytes, &src); err != nil {
		return err
	}
	deepMerge(dst, src)
	return nil
}

func deepMerge(dst, src map[string]any) {
	for k, v := range src {
		if sv, ok := v.(map[string]any); ok {
			if dv, ok := dst[k].(map[string]any); ok {
				deepMerge(dv, sv)
				continue
			}
		}
		dst[k] = v
	}
}

func findRepoRoot() (string, error) {
	wd, err := os.Getwd()
	if err != nil {
		return "", err
	}
	for {
		if _, err := os.Stat(filepath.Join(wd, "api", "config", "api.json")); err == nil {
			return wd, nil
		}
		next := filepath.Dir(wd)
		if next == wd {
			break
		}
		wd = next
	}
	return "", errors.New("could not find repo root containing api/config/api.json")
}

type goRoute struct {
	name           string
	method         string
	path           string
	handlerName    string
	requiredHeader string
	pressureBudget int64
	handler        *Closure
	typedRequest   bool
}

type goServeProgram struct {
	source   string
	lineMap  []formFilePart
	upstream *url.URL
	client   *http.Client
	pool     sync.Pool
}

type goServeWorker struct {
	k       *Kernel
	env     *Frame
	routes  []goRoute
	program *goServeProgram
}

func cliServe(args []string) int {
	port := 18080
	var upstream *url.URL
	files := []string{}
	for i := 0; i < len(args); i++ {
		switch args[i] {
		case "--port":
			i++
			if i >= len(args) {
				fmt.Fprintln(os.Stderr, "serve --port requires a value")
				return 2
			}
			n, err := strconv.Atoi(args[i])
			if err != nil || n <= 0 {
				fmt.Fprintf(os.Stderr, "invalid port %q\n", args[i])
				return 2
			}
			port = n
		case "--config":
			i++
			if i >= len(args) {
				fmt.Fprintln(os.Stderr, "serve --config requires a path")
				return 2
			}
			goKernelConfigPath = args[i]
		case "--upstream":
			i++
			if i >= len(args) {
				fmt.Fprintln(os.Stderr, "serve --upstream requires a value")
				return 2
			}
			parsed, err := url.Parse(args[i])
			if err != nil || parsed.Scheme == "" || parsed.Host == "" {
				fmt.Fprintf(os.Stderr, "invalid upstream %q\n", args[i])
				return 2
			}
			upstream = parsed
		default:
			files = append(files, args[i])
		}
	}
	if len(files) == 0 {
		fmt.Fprintln(os.Stderr, "usage: form-kernel-go serve --port 18080 [--config path] [--upstream https://api.example] <routes.fk...>")
		return 2
	}
	source, lineMap, err := readFormFiles(files)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		return 1
	}
	program := &goServeProgram{
		source:   source,
		lineMap:  lineMap,
		upstream: upstream,
		client: &http.Client{
			Timeout: 30 * time.Second,
		},
	}
	program.pool.New = func() any {
		worker, err := buildGoServeWorker(program)
		if err != nil {
			panic(err)
		}
		return worker
	}
	first, err := buildGoServeWorker(program)
	if err != nil {
		fmt.Fprintf(os.Stderr, "serve route load: %v\n", err)
		return 1
	}
	program.pool.Put(first)
	mux := http.NewServeMux()
	mux.Handle("/", program)
	addr := fmt.Sprintf("127.0.0.1:%d", port)
	fmt.Fprintf(os.Stderr, "form-kernel-go serve listening on http://%s\n", addr)
	server := &http.Server{
		Addr:              addr,
		Handler:           mux,
		ReadHeaderTimeout: 5 * time.Second,
	}
	if err := server.ListenAndServe(); err != nil && !errors.Is(err, http.ErrServerClosed) {
		fmt.Fprintf(os.Stderr, "serve: %v\n", err)
		return 1
	}
	return 0
}

func buildGoServeWorker(program *goServeProgram) (*goServeWorker, error) {
	k := NewKernel()
	root := k.readFormRoot(program.source, program.lineMap)
	env := NewFrame(nil)
	k.activeRoots = []NodeID{root}
	_ = k.walkUnit(root, env)
	routes, err := buildGoRoutes(k, env)
	if err != nil {
		return nil, err
	}
	return &goServeWorker{k: k, env: env, routes: routes, program: program}, nil
}

func buildGoRoutes(k *Kernel, env *Frame) ([]goRoute, error) {
	routesID := NameID(k.internString("routes").Inst)
	routesValue, ok := env.Lookup(routesID)
	if !ok || routesValue.Kind != VList {
		return nil, errors.New("route program must bind a top-level routes list")
	}
	out := make([]goRoute, 0, len(routesValue.List))
	for _, row := range routesValue.List {
		route, err := buildGoRoute(k, env, row)
		if err != nil {
			return nil, err
		}
		out = append(out, route)
	}
	return out, nil
}

func buildGoRoute(k *Kernel, env *Frame, row Value) (goRoute, error) {
	if row.Kind != VList {
		return goRoute{}, errors.New("each route must be a kh-route row or compatibility path/handler row")
	}
	if len(row.List) == 8 && row.List[0].Kind == VInt && row.List[0].Int == khTagRoute {
		return buildKHRoute(k, env, row.List)
	}
	if len(row.List) == 2 {
		if row.List[0].Kind != VStr || row.List[1].Kind != VClosure {
			return goRoute{}, errors.New("compatibility route row must be (list path handler)")
		}
		return goRoute{
			name:           row.List[0].Str,
			method:         "ANY",
			path:           row.List[0].Str,
			handlerName:    k.nameStr(row.List[1].Cl.Name),
			pressureBudget: 40,
			handler:        row.List[1].Cl,
		}, nil
	}
	if len(row.List) == 3 {
		if row.List[0].Kind != VStr || row.List[1].Kind != VStr || row.List[2].Kind != VClosure {
			return goRoute{}, errors.New("compatibility route row must be (list method path handler)")
		}
		return goRoute{
			name:           row.List[1].Str,
			method:         row.List[0].Str,
			path:           row.List[1].Str,
			handlerName:    k.nameStr(row.List[2].Cl.Name),
			pressureBudget: 40,
			handler:        row.List[2].Cl,
		}, nil
	}
	return goRoute{}, errors.New("each route must be a kh-route row or compatibility path/handler row")
}

func buildKHRoute(k *Kernel, env *Frame, fields []Value) (goRoute, error) {
	name, err := routeString(fields[1], "name")
	if err != nil {
		return goRoute{}, err
	}
	method, err := routeString(fields[2], "method")
	if err != nil {
		return goRoute{}, err
	}
	path, err := routeString(fields[3], "pattern")
	if err != nil {
		return goRoute{}, err
	}
	handlerName, err := routeString(fields[5], "handler")
	if err != nil {
		return goRoute{}, err
	}
	requiredHeader, err := routeString(fields[6], "required_header")
	if err != nil {
		return goRoute{}, err
	}
	pressureBudget, err := routeInt(fields[7], "pressure_budget")
	if err != nil {
		return goRoute{}, err
	}
	handler, err := routeHandler(k, env, name, handlerName)
	if err != nil {
		return goRoute{}, err
	}
	if name == "" || handlerName == "" {
		return goRoute{}, errors.New("kh-route name and handler must not be empty")
	}
	if path == "" || !strings.HasPrefix(path, "/") {
		return goRoute{}, fmt.Errorf("kh-route %s pattern must start with /", name)
	}
	if !validRouteMethod(method) {
		return goRoute{}, fmt.Errorf("kh-route %s method must be GET, POST, PUT, PATCH, DELETE, or ANY", name)
	}
	if pressureBudget < 0 {
		return goRoute{}, fmt.Errorf("kh-route %s pressure_budget must be non-negative", name)
	}
	return goRoute{
		name:           name,
		method:         method,
		path:           path,
		handlerName:    handlerName,
		requiredHeader: requiredHeader,
		pressureBudget: pressureBudget,
		handler:        handler,
		typedRequest:   true,
	}, nil
}

func routeString(v Value, field string) (string, error) {
	if v.Kind != VStr {
		return "", fmt.Errorf("kh-route %s must be a string", field)
	}
	return v.Str, nil
}

func routeInt(v Value, field string) (int64, error) {
	if v.Kind != VInt {
		return 0, fmt.Errorf("kh-route %s must be an integer", field)
	}
	return v.Int, nil
}

func routeHandler(k *Kernel, env *Frame, routeName, handlerName string) (*Closure, error) {
	handlerID := NameID(k.internString(handlerName).Inst)
	v, ok := env.Lookup(handlerID)
	if !ok {
		return nil, fmt.Errorf("kh-route %s handler %s is not bound", routeName, handlerName)
	}
	if v.Kind != VClosure {
		return nil, fmt.Errorf("kh-route %s handler %s must resolve to a closure", routeName, handlerName)
	}
	return v.Cl, nil
}

func validRouteMethod(method string) bool {
	switch method {
	case "GET", "POST", "PUT", "PATCH", "DELETE", "ANY":
		return true
	default:
		return false
	}
}

func (p *goServeProgram) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	worker := p.pool.Get().(*goServeWorker)
	defer p.pool.Put(worker)
	worker.serve(w, r)
}

func (wkr *goServeWorker) serve(w http.ResponseWriter, r *http.Request) {
	var activeRoute *goRoute
	defer func() {
		if recovered := recover(); recovered != nil {
			message := fmt.Sprint(recovered)
			diagnosis := diagnoseKernelPanic(message)
			source, sourceLabel := wkr.crashSource()
			where := "route:unknown request:" + r.URL.EscapedPath()
			if activeRoute != nil {
				where = nativeRouteWhere(activeRoute, r)
			}
			operation := fmt.Sprintf("request=%s %s %s", r.Method, r.URL.RequestURI(), where)
			// Frames are stored raw and rendered only here, on the crash path.
			formStack := make([]string, 0, len(wkr.k.formStack))
			for _, f := range wkr.k.formStack {
				formStack = append(formStack, wkr.k.renderFormFrame(f))
			}
			// The pooled worker serves the next request with a clean stack.
			wkr.k.formStack = wkr.k.formStack[:0]
			tracePath := writeKernelCrashTraceWithContext(
				[]string{"serve", r.Method, r.URL.RequestURI()},
				source,
				recovered,
				sourceLabel,
				operation,
				formStack,
			)
			setRouteDecisionHeaders(
				w.Header(),
				routeHowNativeError,
				where,
				routeDecisionWho(r),
				routeDecisionWhen(),
			)
			w.Header().Set(headerFatalKind, sanitizeDecisionHeader(diagnosis.fatalKind))
			if tracePath != "" {
				w.Header().Set(headerCrashTrace, sanitizeDecisionHeader(tracePath))
			}
			w.Header().Set("Content-Type", "text/plain; charset=utf-8")
			w.WriteHeader(http.StatusInternalServerError)
			_, _ = w.Write([]byte(kernelFatalHTTPBody(message, diagnosis, tracePath)))
		}
	}()
	route := wkr.match(r)
	if route == nil {
		if wkr.program != nil && wkr.program.upstream != nil {
			wkr.fanout(w, r)
			return
		}
		http.NotFound(w, r)
		return
	}
	activeRoute = route
	requestValue, err := requestValue(route, r)
	if err != nil {
		http.Error(w, fmt.Sprintf("native request read failed: %v\n", err), http.StatusBadRequest)
		return
	}
	call := NewCallFrame(route.handler.Env, len(route.handler.Params))
	if len(route.handler.Params) == 1 {
		call.Bind(route.handler.Params[0], requestValue)
	} else if len(route.handler.Params) != 0 {
		http.Error(w, "native handler must accept zero or one request argument\n", http.StatusInternalServerError)
		return
	}
	result := wkr.k.walk(route.handler.Body, call)
	status, headers, body, ok := formHTTPResponse(result)
	if !ok {
		status = http.StatusOK
		body = formValueString(result)
		headers = http.Header{}
	}
	for key, vals := range headers {
		for _, val := range vals {
			w.Header().Add(key, val)
		}
	}
	setRouteDecisionHeaders(
		w.Header(),
		routeHowNative,
		nativeRouteWhere(route, r),
		routeDecisionWho(r),
		routeDecisionWhen(),
	)
	w.WriteHeader(status)
	_, _ = w.Write([]byte(body))
	wkr.k.substrateGC([]Value{result}, wkr.env)
}

func (wkr *goServeWorker) crashSource() (string, string) {
	if wkr == nil || wkr.program == nil {
		return "", "form-kernel-go serve"
	}
	return wkr.program.source, "go serve source manifest"
}

func (wkr *goServeWorker) fanout(w http.ResponseWriter, r *http.Request) {
	target := upstreamURL(wkr.program.upstream, r.URL)
	body, err := io.ReadAll(io.LimitReader(r.Body, 10<<20))
	if err != nil {
		http.Error(w, fmt.Sprintf("fanout request read failed: %v\n", err), http.StatusBadRequest)
		return
	}
	req, err := http.NewRequestWithContext(r.Context(), r.Method, target, bytes.NewReader(body))
	if err != nil {
		http.Error(w, fmt.Sprintf("fanout request build failed: %v\n", err), http.StatusBadGateway)
		return
	}
	copyForwardHeaders(req.Header, r.Header)
	when := routeDecisionWhen()
	who := routeDecisionWho(r)
	where := fanoutRouteWhere(target)
	setRouteDecisionHeaders(req.Header, routeHowFanout, where, who, when)
	setFanoutNativeInvitationHeaders(req.Header)
	req.Header.Set("X-Forwarded-Host", r.Host)
	req.Header.Set("X-Forwarded-Proto", "http")
	resp, err := wkr.program.client.Do(req)
	if err != nil {
		http.Error(w, fmt.Sprintf("fanout upstream failed: %v\n", err), http.StatusBadGateway)
		return
	}
	defer resp.Body.Close()
	copyResponseHeaders(w.Header(), resp.Header)
	setRouteDecisionHeaders(
		w.Header(),
		routeHowFanout,
		where,
		who,
		when,
	)
	setFanoutNativeInvitationHeaders(w.Header())
	w.WriteHeader(resp.StatusCode)
	_, _ = io.Copy(w, io.LimitReader(resp.Body, 25<<20))
}

func setRouteDecisionHeaders(headers http.Header, how, where, who, when string) {
	headers.Set(headerFormRouter, sanitizeDecisionHeader(how))
	headers.Set(headerRouteHow, sanitizeDecisionHeader(how))
	headers.Set(headerRouteWhere, sanitizeDecisionHeader(where))
	headers.Set(headerRouteWho, sanitizeDecisionHeader(who))
	headers.Set(headerRouteWhen, sanitizeDecisionHeader(when))
}

func setFanoutNativeInvitationHeaders(headers http.Header) {
	headers.Set(headerNativeInvite, nativeInviteValue)
	headers.Set(headerNativeState, nativeInviteState)
	headers.Set(headerNativeProtocol, nativeInviteProtocol)
	headers.Set(headerNativePath, routeHowFanout)
	headers.Set(headerNativeDecline, nativeInviteDecline)
	headers.Set(headerNativeFallback, "X-Form-Python-Fallback")
}

func routeDecisionWhen() string {
	return time.Now().UTC().Format(time.RFC3339Nano)
}

func routeDecisionWho(r *http.Request) string {
	for _, name := range []string{"X-Coherence-Agent", "X-Codex-Agent", "X-Actor", "User-Agent"} {
		if value := r.Header.Get(name); value != "" {
			return value
		}
	}
	if r.RemoteAddr != "" {
		return r.RemoteAddr
	}
	return "unknown"
}

func nativeRouteWhere(route *goRoute, r *http.Request) string {
	return "route:" + route.name + " pattern:" + route.path + " request:" + r.URL.EscapedPath()
}

func fanoutRouteWhere(target string) string {
	parsed, err := url.Parse(target)
	if err != nil {
		return "upstream:unknown"
	}
	parsed.RawQuery = ""
	parsed.Fragment = ""
	return "upstream:" + parsed.String()
}

func sanitizeDecisionHeader(value string) string {
	value = strings.Map(func(r rune) rune {
		if r == '\r' || r == '\n' || r == '\t' {
			return ' '
		}
		if r < 0x20 || r == 0x7f {
			return -1
		}
		return r
	}, value)
	value = strings.Join(strings.Fields(value), " ")
	if value == "" {
		return "unknown"
	}
	if len(value) > maxDecisionHeaderLen {
		return value[:maxDecisionHeaderLen]
	}
	return value
}

func upstreamURL(base *url.URL, requestURL *url.URL) string {
	out := *base
	basePath := strings.TrimRight(base.EscapedPath(), "/")
	reqPath := requestURL.EscapedPath()
	if reqPath == "" {
		reqPath = "/"
	}
	if basePath == "" {
		out.Path = reqPath
	} else {
		out.Path = basePath + "/" + strings.TrimLeft(reqPath, "/")
	}
	out.RawQuery = requestURL.RawQuery
	out.Fragment = ""
	return out.String()
}

func copyForwardHeaders(dst, src http.Header) {
	for key, vals := range src {
		if hopByHopHeader(key) || routerOwnedHeader(key) {
			continue
		}
		for _, val := range vals {
			dst.Add(key, val)
		}
	}
}

func copyResponseHeaders(dst, src http.Header) {
	for key, vals := range src {
		if hopByHopHeader(key) || routerOwnedHeader(key) || strings.EqualFold(key, "Content-Length") {
			continue
		}
		for _, val := range vals {
			dst.Add(key, val)
		}
	}
}

func routerOwnedHeader(key string) bool {
	return strings.EqualFold(key, headerFormRouter) ||
		strings.EqualFold(key, headerRouteHow) ||
		strings.EqualFold(key, headerRouteWhere) ||
		strings.EqualFold(key, headerRouteWhen) ||
		strings.EqualFold(key, headerRouteWho) ||
		strings.EqualFold(key, headerNativeInvite) ||
		strings.EqualFold(key, headerNativeState) ||
		strings.EqualFold(key, headerNativeProtocol) ||
		strings.EqualFold(key, headerNativePath) ||
		strings.EqualFold(key, headerNativeDecline) ||
		strings.EqualFold(key, headerNativeFallback)
}

func hopByHopHeader(key string) bool {
	switch strings.ToLower(key) {
	case "connection", "keep-alive", "proxy-authenticate", "proxy-authorization",
		"te", "trailer", "transfer-encoding", "upgrade":
		return true
	default:
		return false
	}
}

func (wkr *goServeWorker) match(r *http.Request) *goRoute {
	for i := range wkr.routes {
		route := &wkr.routes[i]
		if !routeMethodMatches(route.method, r.Method) {
			continue
		}
		if !routePathMatches(route.path, r.URL.Path) {
			continue
		}
		if route.requiredHeader != "" && r.Header.Get(route.requiredHeader) == "" {
			continue
		}
		if route.pressureBudget >= 0 {
			return route
		}
	}
	return nil
}

func routeMethodMatches(routeMethod, requestMethod string) bool {
	return routeMethod == "ANY" || routeMethod == requestMethod
}

func routePathMatches(pattern, path string) bool {
	if strings.HasSuffix(pattern, "*") {
		return routeWildcardPathMatches(pattern, path)
	}
	if strings.Contains(pattern, "{") || strings.Contains(pattern, "/:") {
		return routeTemplatePathMatches(pattern, path)
	}
	return pattern == path
}

func routeWildcardPathMatches(pattern, path string) bool {
	prefix := strings.TrimSuffix(pattern, "*")
	if !strings.HasPrefix(path, prefix) {
		return false
	}
	if strings.HasSuffix(prefix, "/") {
		return true
	}
	remainder := strings.TrimPrefix(path, prefix)
	return remainder != "" && !strings.Contains(remainder, "/")
}

func routeTemplatePathMatches(pattern, path string) bool {
	patternParts := strings.Split(strings.Trim(pattern, "/"), "/")
	pathParts := strings.Split(strings.Trim(path, "/"), "/")
	if len(patternParts) != len(pathParts) {
		return false
	}
	for i, patternPart := range patternParts {
		if routeTemplateSegment(patternPart) {
			if pathParts[i] == "" {
				return false
			}
			continue
		}
		if patternPart != pathParts[i] {
			return false
		}
	}
	return true
}

func routeTemplateSegment(segment string) bool {
	if strings.HasPrefix(segment, ":") && len(segment) > 1 {
		return true
	}
	return strings.HasPrefix(segment, "{") && strings.HasSuffix(segment, "}") && len(segment) > 2
}

func requestValue(route *goRoute, r *http.Request) (Value, error) {
	if route.typedRequest {
		return khRequestValue(r)
	}
	return requestAlistValue(r), nil
}

func khRequestValue(r *http.Request) (Value, error) {
	body, err := io.ReadAll(io.LimitReader(r.Body, 10<<20))
	if err != nil {
		return Value{}, err
	}
	headers := []Value{}
	for key, values := range r.Header {
		for _, value := range values {
			headers = append(headers, Value{Kind: VList, List: []Value{
				{Kind: VInt, Int: khTagHeader},
				{Kind: VStr, Str: key},
				{Kind: VStr, Str: value},
			}})
		}
	}
	query := []Value{}
	for key, values := range r.URL.Query() {
		for _, value := range values {
			query = append(query, Value{Kind: VList, List: []Value{
				{Kind: VInt, Int: khTagField},
				{Kind: VStr, Str: key},
				{Kind: VStr, Str: value},
			}})
		}
	}
	return Value{Kind: VList, List: []Value{
		{Kind: VInt, Int: khTagRequest},
		{Kind: VStr, Str: r.Method},
		{Kind: VStr, Str: r.URL.Path},
		{Kind: VList, List: headers},
		{Kind: VList, List: query},
		{Kind: VStr, Str: string(body)},
	}}, nil
}

func requestAlistValue(r *http.Request) Value {
	pairs := []Value{
		pairValue("__method__", r.Method),
		pairValue("__path__", r.URL.Path),
	}
	for key, values := range r.URL.Query() {
		for _, value := range values {
			pairs = append(pairs, pairValue(key, value))
		}
	}
	return Value{Kind: VList, List: pairs}
}

func pairValue(key, value string) Value {
	return Value{Kind: VList, List: []Value{
		{Kind: VStr, Str: key},
		{Kind: VStr, Str: value},
	}}
}

func formHTTPResponse(v Value) (int, http.Header, string, bool) {
	if v.Kind != VList || len(v.List) != 4 || v.List[0].Kind != VInt || v.List[0].Int != khTagResponse {
		return 0, nil, "", false
	}
	status := int(v.List[1].Int)
	headers := http.Header{}
	if v.List[2].Kind == VList {
		for _, raw := range v.List[2].List {
			if raw.Kind != VList || len(raw.List) != 3 {
				continue
			}
			if raw.List[0].Kind == VInt && raw.List[0].Int == khTagHeader &&
				raw.List[1].Kind == VStr && raw.List[2].Kind == VStr {
				headers.Add(raw.List[1].Str, raw.List[2].Str)
			}
		}
	}
	body := formValueString(v.List[3])
	return status, headers, body, true
}
