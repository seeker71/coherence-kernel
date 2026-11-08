/* The Form runtime reads one source file and offers its forms.
 * A word is nothing, an integer, a real, text, a list, a function, or a node.
 * Definitions are bound before any form runs, so a later definition is visible.
 * A prelude line names sources whose definitions join this program.
 * A form.bml section lowers its definitions into the same forms.
 * Retained owners root their words; temporary values are collected between
 * interpreter steps. Collections grow with the data.
 */

#include <ctype.h>
#include <dirent.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <sys/socket.h>
#include "fkwu-optable.h"
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
#ifdef __APPLE__
#include <libproc.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach-o/dyld.h>
#include <pthread.h>
#include <sys/event.h>
#include <sys/sysctl.h>
#elif defined(__linux__)
#include <poll.h>
#include <sys/syscall.h>
#endif

static void fk_die(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}
#include "fkwu-node-word.h"

extern char **environ;

static const size_t EMPTY_COUNT = 0;
static const size_t FIRST_INDEX = 0;
static const size_t ONE_ELEMENT = 1;
static const size_t FIRST_RESERVATION = 1;
static const size_t GROWTH_FACTOR = 2;
static const size_t TEXT_TERMINATOR_SIZE = 1;
static const int STRING_COMPARE_EQUAL = 0;
static const int SYSTEM_CALL_OK = 0;
static const int DECIMAL_RADIX = 10;
static const int PROGRAM_ARGUMENT_COUNT = 2;
static const int SOURCE_PATH_ARGUMENT = 1;
static const long long WORD_FALSE = 0;
static const long long WORD_TRUE = 1;
static const double REAL_ZERO = 0.0;
static const uint64_t NANOSECONDS_PER_SECOND = 1000000000ULL;

typedef uint64_t NodeIdentity;

static const char COMMENT_INTRODUCER = ';';
static const char CALL_OPEN = '(';
static const char CALL_CLOSE = ')';
static const char TEXT_DELIMITER = '"';
static const char TEXT_TERMINATOR = '\0';
static const char NEWLINE = '\n';
static const char TAB = '\t';
static const char RETURN_MARK = '\r';
static const char ESCAPE_MARK = '\\';
static const char DECIMAL_POINT = '.';
static const char SIGN_PLUS = '+';
static const char SIGN_MINUS = '-';
static const char EXPONENT_LOWER = 'e';
static const char EXPONENT_UPPER = 'E';
static const char DIGIT_ZERO = '0';
static const char DIGIT_NINE = '9';

static const char *const NAME_DEFN = "defn";
static const char *const NAME_TRUE = "true";
static const char *const NAME_FALSE = "false";
static const char *const NAME_NOTHING = "nothing";
static const char *const KIND_NAME_NOTHING = "null";
static const char *const KIND_NAME_INTEGER = "int";
static const char *const KIND_NAME_REAL = "float";
static const char *const KIND_NAME_TEXT = "string";
static const char *const KIND_NAME_LIST = "list";
static const char *const KIND_NAME_FUNCTION = "closure";
static const char *const KIND_NAME_NODE = "node_id";
static const char *const KIND_NAME_RECORD = "record";
static const char *const LIST_OPEN = "[";
static const char *const LIST_SEPARATOR = ", ";
static const char *const LIST_CLOSE = "]";
static const char *const LINE_ENDING = "\n";
static const char *const SOURCE_OPEN_MODE = "rb";
static const char *const INTEGER_FORMAT = "%lld";
static const char *const ERROR_FORMAT = "fkwu: %s at byte %zu\n";
static const char *const ERROR_FORMAT_PATH = "fkwu: %s at byte %zu in %s\n";
static const char *const ERROR_BARE = "fkwu: %s\n";
static const char LINE_COMMENT_MARK = '/';
static const char PATH_SEPARATOR = '/';
static const char SECTION_OPEN = '{';
static const char SECTION_CLOSE = '}';
static const char INDEX_OPEN = '[';
static const char INDEX_CLOSE = ']';
static const char STATEMENT_END = ';';
static const char NAME_MARK = '=';
static const char ARGUMENT_SEPARATOR = ',';
static const char TERNARY_MARK = '?';
static const char PAIR_MARK = ':';
static const char NEGATION_MARK = '!';
static const char PRODUCT_MARK = '*';
static const char REMAINDER_MARK = '%';
static const char DISJUNCTION_MARK = '|';
static const char ARROW_MARK = '>';
static const char *const USAGE_TEXT = "usage: fkwu SOURCE\n";
static const char *const FORM_ROOT = "form";
static const char *const PRELUDE_WORD = "preludes:";
static const char *const SECTION_WORD = "section";
static const char *const DEF_WORD = "def";
static const char *const IMPORT_WORD = "import";
static const char *const FORM_BML_SECTION = "form.bml";
static const char *const UNBOUND_PREFIX = "an unbound name was offered: ";
static const char *const COALESCE_WORD = "??";
static const char *const FAIL_NAME = "fail";
static const char *const STOP_NAME = "stop";
static const char *const TRY_WORD = "try";
static const char *const NULL_DEVICE = "/dev/null";
static const char *const TERMINAL_INPUT_DOOR = "terminal-input";
static const long long TERMINAL_INPUT_READ = 2;
static const mode_t DIRECTORY_MODE = 0755;
static const mode_t TEXT_FILE_MODE = 0644;
static const size_t GIFT_HEADER_BYTES = 16;
static const size_t GIFT_SEQUENCE_OFFSET = 0;
static const size_t GIFT_LENGTH_OFFSET = 8;
static const mode_t GIFT_OPEN_MODE = 0600;
static const long long GIFT_READ_BUDGET_MS = 1;
static const char *const ROSTER_OBJECT = "/fg-roster";
static const size_t NODE_REFERENCE_BYTES = 64;
static const char *const TEMP_DIRECTORY = "/tmp";
static const char *const METAL_ABSENT_STATUS = "metal=not-up\nmetal_owner=fkwu-form-cli\nmetal_carrier=dynamic\nmetal_loaded=false\nmetal_door=handle\n";
static const char *const SHELL_PATH = "/bin/sh";
static const char *const SHELL_COMMAND = "-c";
static const unsigned U32_MASK = 0xffffffffu;
static const unsigned U32_BITS = 32;
static const int TERMINAL_COLUMNS = 0;
static const int TERMINAL_ROWS = 1;
static const mode_t NULL_DEVICE_MODE = 0;
static const long long SOURCE_LINE_STRIDE = 65536;
static const long long SCAN_SPACE = 0;
static const long long SCAN_DIGITS = 1;
static const long long SCAN_ALPHA = 2;
static const long long SCAN_NAME = 3;
static const long long SCAN_STRING = 4;
static const long long SCAN_LINE = 5;
static const long long SCAN_JSON_SAFE = 6;
static const long long MILLISECONDS_PER_SECOND = 1000;
static const long long MICROSECONDS_PER_SECOND = 1000000;
static const long long NANOSECONDS_PER_MILLISECOND = 1000000;
static const size_t SLEEP_REST_FIELDS = 4;
static const size_t SLEEP_WOKE = 1;

enum {
    BOOLEAN_NODE_FALSE = 1,
    BOOLEAN_NODE_TRUE = 2
};

typedef enum {
    KIND_NOTHING,
    KIND_INTEGER,
    KIND_REAL,
    KIND_TEXT,
    KIND_LIST,
    KIND_FUNCTION,
    KIND_NODE,
    KIND_RECORD
} ValueKind;

typedef enum {
    FORM_INTEGER,
    FORM_REAL,
    FORM_TEXT,
    FORM_SYMBOL,
    FORM_GROUP
} FormKind;

typedef enum {
    OP_IF,
    OP_DO,
    OP_AND,
    OP_LIST,
    OP_ADD,
    OP_SUB,
    OP_EQ,
    OP_LT,
    OP_GE,
    OP_STR_EQ,
    OP_STR_CONCAT,
    OP_VALUE_KIND,
    OP_NOTHING,
    OP_READ_LINE,
    OP_FILE_MTIME,
    OP_CELL_PATH,
    OP_METHOD_INVOKE,
    OP_KERNEL_AST,
    OP_FIELD,
    OP_INTERN_BOOL,
    OP_KERNEL_STAT,
    OP_FB_RECORD,
    OP_FB_EVENTS,
    OP_FB_CLEAR,
    OP_GT,
    OP_LE,
    OP_NTH,
    OP_SLICE,
    OP_PLUS,
    OP_LET,
    OP_OR,
    OP_LAMBDA,
    OP_NOT,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_HEAD,
    OP_TAIL,
    OP_CONS,
    OP_PRINT,
    OP_COALESCE,
    OP_TRY,
    OP_ATTEMPT,
    OP_FORM_ERROR,
    OP_FAIL,
    OP_CHOICE
} OperationId;

typedef enum { FIXED_ARITY, VARIADIC } ArityPolicy;
typedef enum { LEAVE_ARGUMENTS, EVALUATE_ARGUMENTS } EvaluationPolicy;

enum {
    CALL_HEAD = 0,
    CALL_ARGUMENTS = 1,
    DEFN_NAME = 1,
    DEFN_PARAMETERS = 2,
    DEFN_BODY = 3,
    DEFN_PART_COUNT = 4,
    IF_TEST = 0,
    IF_CONSEQUENT = 1,
    IF_ALTERNATIVE = 2,
    IF_ARGUMENT_COUNT = 3,
    IF_STAGE_TEST = 0,
    IF_STAGE_BRANCH = 1,
    TRY_STAGE_BODY = 1,
    TRY_STAGE_HANDLER = 2,
    LEFT_OPERAND = 0,
    RIGHT_OPERAND = 1,
    SLICE_END = 2,
    SOLE_OPERAND = 0,
    ARITY_NONE = 0,
    ARITY_ONE = 1,
    ARITY_TWO = 2,
    ARITY_THREE = 3,
    ARITY_FOUR = 4,
    ARITY_FIVE = 5,
    NODE_FIELD_PACKAGE = 0,
    NODE_FIELD_LEVEL = 1,
    NODE_FIELD_TYPE = 2,
    NODE_FIELD_INSTANCE = 3,
    RECORD_WORD = 0,
    RECORD_PATH = 1,
    RECORD_POSITION = 2,
    STAT_ROOTS_SINCE_CLEAR = 9,
    STAT_REJECTED = 11,
    STAT_ENTERED = 12,
    STAT_ACCEPTED = 13,
    STAT_LAST_INDEX = 14,
    STAT_GIFT_FRAMES = 41,
    STAT_GIFT_BYTES = 42,
    STAT_JIT_BOXES = 45,
    STAT_JIT_UNBOXES = 46,
    STAT_JIT_NATIVE = 47,
    STAT_JIT_CRYSTALLIZED = 48,
    STAT_JIT_UNROLL = 49,
    STAT_JIT_LOOP_ITERATIONS = 50,
    STAT_NODES_MINTED = 53,
    STAT_FLOAT_CAPACITY = 56,
    STAT_FIELD_FLOATS = 57,
    STAT_NODES_GAS = 58,
    STAT_NODES_WATER = 59,
    STAT_NODES_ICE = 60,
    STAT_NODE_LOGICAL_BYTES = 61,
    STAT_NODE_PRIVATE_BYTES = 62,
    STAT_ROOTS_STANDING = 54,
    STAT_ROOT_CAPACITY = 55,
    STAT_NODE_IDENTITY_BYTES = 63,
    STAT_DISPATCHES = 0,
    STAT_DISPATCH_DISTINCT = 1,
    STAT_HOTTEST_TAG = 2,
    STAT_HOTTEST_COUNT = 3,
    STAT_CELLS = 4,
    STAT_STRINGS = 5,
    STAT_CONS = 6,
    STAT_STACK = 7,
    STAT_FLOATS = 8,
    STAT_RECIPES = 43,
    LIVE_PID = 1,
    LIVE_START_MS = 2,
    LIVE_SEQUENCE = 3,
    LIVE_DISPATCHES = 4,
    LIVE_HEAT = 5,
    LIVE_NODES = 6,
    LIVE_STRINGS = 7,
    LIVE_CONS = 8,
    LIVE_FUNCTIONS = 9,
    LIVE_GIFT_FRAMES = 10,
    LIVE_GIFT_BYTES = 11,
    LIVE_NODE_CAP = 12,
    LIVE_HEAP_CAP = 13,
    LIVE_STACK = 14,
    LIVE_FLOATS = 15,
    LIVE_HOTTEST_TAG = 16,
    LIVE_HOTTEST_COUNT = 17,
    LIVE_CPU_US = 18,
    LIVE_ALIVE = 19,
    LIVE_DISTINCT = 20,
    LIVE_MELTS = 21,
    LIVE_STORE_SHARED = 22,
    LIVE_FIELD_VERSION = 28,
    LIVE_FIELD_DEVICE = 29,
    LIVE_FIELD_INODE = 30,
    LIVE_MINTS = 33,
    LIVE_PAGE_WORDS = 34,
    LIVE_NAME_ROOM = 32,
    STACK_TRACE_DEPTH = 12,
    INVALID_DESCRIPTOR = -1
};

typedef struct Value Value;
typedef struct Form Form;
typedef struct Name Name;
typedef struct Binding Binding;
typedef struct Operation Operation;
typedef struct Frame Frame;
typedef struct Record Record;
typedef struct Cell Cell;
typedef struct List List;
typedef struct CellProgram CellProgram;

typedef Value (*OperationOffer)(Value *arguments, size_t argument_count);

struct Name {
    const char *bytes;
    size_t length;
};

struct Value {
    ValueKind kind;
    long long integer;
    double real;
    char *text;
    size_t text_length;
    Value *items;
    size_t item_count;
    Form *body;
    Name *parameters;
    size_t parameter_count;
    Binding *captures;
    size_t capture_count;
    Record *record;
    Cell *cell;
    List *list;
    CellProgram *program;
    size_t native_operation;
    // A borrowed value retains its physical generation independently of the
    // frame or parent value from which it was selected.
    Record *owner;
};

struct List {
    Value head;
    List *tail;
    Value *dotted_tail;
    Value *flat;
    size_t flat_count;
    List *indexed;
    size_t lead;
};

typedef enum { CELL_TEXT, CELL_INTEGER, CELL_COMPOSITE, CELL_REAL } CellKind;

struct Cell {
    fk_node_id category;
    Value category_value;
    CellKind kind;
    long long integer;
    double real;
    char *text;
    size_t text_length;
    Value children_value;
    char *source;
    size_t source_length;
    long long source_line;
    long long source_column;
    uint64_t hash;
    long long field_ref;
    size_t jit_slot;
    uint64_t jit_generation;
    int jit_released;
    Cell *jit_next_released;
};

struct Record {
    Value blueprint;
    Value *keys;
    Value *values;
    size_t count;
    size_t capacity;
    size_t *text_slots;
    size_t text_capacity;
    size_t text_indexed;
};

typedef enum { HEAP_TEXT, HEAP_LIST, HEAP_RECORD, HEAP_PARAMETERS, HEAP_CAPTURES, HEAP_NATIVE_CELL } HeapKind;
static void heap_track(void *pointer, HeapKind kind, size_t count);
static void heap_prepare(void);
static void *heap_allocate(size_t count, size_t width, HeapKind kind, size_t elements);
static void heap_forget(void *pointer);
static void heap_collect(Value result);
static int heap_collection_due;
static size_t heap_text_allocations, heap_list_allocations, heap_collection_epoch;

struct Form {
    FormKind kind;
    size_t syntax_id;
    /* A call form's resolved head (operation and function slots), valid while
     * head_epoch equals global_epoch; zero never matches. */
    uint64_t head_epoch;
    size_t head_operation, head_function;
    Value syntax_view;
    int syntax_view_ready;
    const char *source;
    size_t source_length;
    size_t offset;
    const char *path;
    long long integer;
    double real;
    char *text;
    size_t text_length;
    Form *annotation;
    Form **parts;
    size_t part_count;
    size_t part_capacity;
    int is_bound;
    int unresolved;
    uint64_t entered_count;
    uint64_t minted_count;
    uint64_t heat_census;
};

struct Binding {
    const char *bytes;
    size_t length;
    Value value;
    size_t previous_function;
    size_t name_offset;
};

struct Operation {
    const char *spelling;
    size_t arity;
    ArityPolicy arity_policy;
    EvaluationPolicy evaluation;
    OperationId id;
    long long tag;
    OperationOffer offer;
    size_t calls;
    int answers;
};

typedef enum {
    FRAME_SEQUENCE,
    FRAME_DO,
    FRAME_AND,
    FRAME_IF,
    FRAME_EAGER,
    FRAME_FUNCTION,
    FRAME_LET,
    FRAME_OR,
    FRAME_LAMBDA,
    FRAME_COALESCE,
    FRAME_TRY,
    FRAME_FAIL,
    FRAME_APPLY,
    FRAME_CHOICE
} FrameKind;

struct Frame {
    FrameKind kind;
    Form *form;
    size_t next_part;
    size_t stage;
    int has_incoming;
    Value incoming;
    Value accumulator;
    int has_accumulator;
    Value *arguments;
    size_t argument_count;
    const Operation *operation;
    Value callee;
    size_t local_base;
    const char *prior_failure;
    int prior_failure_reported;
    size_t stage_base;
    CellProgram *previous_program;
    int restore_program;
};

static const char *source_bytes;
static const char *entry_source_bytes;
static size_t entry_source_length;
static size_t source_length;
static size_t source_offset;
static const char *source_path;
static const unsigned char *staged_input;
static size_t staged_input_length;

static Operation *operations;
static size_t operation_count;
static size_t operation_capacity;
static Binding *functions;
static size_t function_count;
static size_t function_capacity;
static Binding *locals;
static size_t local_count;
static size_t local_capacity;
static Binding *globals;
static size_t global_count;
/* Form owns shared identity. Retain its admitted entry independently of the
 * currently selected cell closure; this bridge disappears with the seed. */
static Value native_field_entry;
static size_t global_capacity;
static char **loaded_units;
static size_t loaded_count, loaded_capacity;

static Frame *frames;
static size_t frame_count;
static const char *failure_message;
static int failure_reported;
static jmp_buf *evaluation_recovery;
static size_t frame_capacity;
static Value finished;
static int result_ready;

typedef struct CellCall CellCall;
struct CellCall {
    CellCall *parent;
    Frame *suspended_frames;
    size_t suspended_frame_count;
    Binding *suspended_locals;
    size_t suspended_local_count;
    Value suspended_finished;
    CellProgram *program;
    jmp_buf recovery;
    Value argument;
    const char *path;
    char *input;
    size_t input_length, input_offset;
    FILE *out, *err;
    char *out_bytes, *err_bytes;
    size_t out_length, err_length;
    int capture, checking, parsing, errors, resident, execution_complete;
    Value diagnostics, image_units, publication, image_argument, path_value;
    size_t source_units, source_functions;
    const char *stop_kind, *message, *recipe, *unit;
    unsigned long long deadline;
    unsigned deadline_steps;
};
static CellCall *active_cell;
static Value offer_cell_run(Value *arguments, size_t argument_count);
static Value offer_cell_call(Value *arguments, size_t argument_count);
static Value offer_cell_input(Value *arguments, size_t argument_count);
static void cell_stop(const char *kind, const char *message);
static FILE *runtime_output(void) { return active_cell != NULL ? active_cell->out : stdout; }
static FILE *runtime_error(void) { return active_cell != NULL ? active_cell->err : stderr; }

typedef struct {
    Value word;
    char *path;
    size_t path_length;
    long long position;
} FrameRecord;

static FrameRecord *frame_records;
static size_t frame_record_count;
static size_t frame_record_capacity;
static size_t frame_since_clear;
static size_t frame_entered;
static size_t frame_accepted;
static size_t frame_rejected;
static size_t frame_last_index;

static size_t stage_depth;
static void stage_unwind(size_t depth, long long phase, const char *why);
static Value stage_door(long long mode, Value value);
static Value jit_cpu_run(Value request);
static int value_proper_list(Value value);
static Value audio_capture_bytes(Value milliseconds);
static Value audio_speaker_leaf(long long mode, Value value);
static Value native_posix_operation(Value request);
static Value offer_kernel_live(Value *arguments, size_t argument_count);
static jmp_buf *parser_choice;
typedef struct {
    Value blueprint;
    char *name;
    size_t name_length;
    Value function;
} MethodDefinition;

static MethodDefinition *method_definitions;
static size_t method_definition_count;
static size_t method_definition_capacity;
typedef struct { int descriptor; uint64_t deadline; CellCall *request_owner; } SocketHandle;
static SocketHandle *socket_descriptors;
static size_t socket_descriptor_count;
static size_t socket_descriptor_capacity;
typedef struct { int descriptor; CellCall *owner; int close_error; } FileLease;
static FileLease *file_leases;
static size_t file_lease_count, file_lease_capacity;

static void note_stack(void) {
    size_t shown = EMPTY_COUNT;
    for (size_t index = frame_count; index > FIRST_INDEX && shown < STACK_TRACE_DEPTH; ) {
        index -= ONE_ELEMENT;
        Form *form = frames[index].form;
        if (form == NULL || form->kind != FORM_GROUP || form->part_count == EMPTY_COUNT) continue;
        Form *head = form->parts[CALL_HEAD];
        if (head->kind != FORM_SYMBOL || head->text == NULL) continue;
        fprintf(runtime_error(), "  %.*s at byte %zu in %s\n", (int)head->text_length, head->text, form->offset, form->path != NULL ? form->path : "<native>");
        shown += ONE_ELEMENT;
    }
}

static void note_failure_owner(const char *reason) {
    for (size_t i = frame_count; i > 0; --i) {
        Form *form = frames[i - 1].form;
        if (form == NULL || form->kind != FORM_GROUP || form->part_count == 0) continue;
        Form *head = form->parts[0];
        if (head->kind != FORM_SYMBOL) continue;
        fprintf(runtime_error(), "%s -- in %.*s (%s)\n", reason, (int)head->text_length, head->text, form->path != NULL ? form->path : "<native>");
        return;
    }
}

static void fail(const char *reason) {
    if (evaluation_recovery != NULL) {
        failure_message = strdup(reason);
        if (failure_message == NULL) { fputs("fkwu: out of memory retaining failure\n", stderr); exit(EXIT_FAILURE); }
        failure_reported = 1;
        fprintf(runtime_error(), ERROR_BARE, reason);
        longjmp(*evaluation_recovery, 1);
    }
    fprintf(runtime_error(), ERROR_BARE, reason);
    if (active_cell != NULL) cell_stop(active_cell->parsing ? "unrunnable" : "stop", reason);
    note_stack();
    note_failure_owner(reason);
    exit(EXIT_FAILURE);
}

static void fail_at(size_t offset, const char *reason) {
    if (parser_choice != NULL) longjmp(*parser_choice, 1);
    if (evaluation_recovery != NULL) fail(reason);
    if (active_cell != NULL) cell_stop("unrunnable", reason);
    if (source_path != NULL) fprintf(stderr, ERROR_FORMAT_PATH, reason, offset, source_path);
    else fprintf(stderr, ERROR_FORMAT, reason, offset);
    note_stack();
    exit(EXIT_FAILURE);
}

static char *message_with_name(const char *prefix, const char *bytes, size_t length) {
    size_t prefix_length = strlen(prefix);
    if (prefix_length > SIZE_MAX - length - TEXT_TERMINATOR_SIZE) fail("a message exceeds addressable memory");
    char *message = malloc(prefix_length + length + TEXT_TERMINATOR_SIZE);
    if (message == NULL) fail("out of memory");
    memcpy(message, prefix, prefix_length);
    if (length != EMPTY_COUNT) memcpy(message + prefix_length, bytes, length);
    message[prefix_length + length] = TEXT_TERMINATOR;
    return message;
}

static size_t checked_add(size_t left, size_t right) {
    if (left > SIZE_MAX - right) fail("a count exceeds addressable memory");
    return left + right;
}

static size_t grown_capacity(size_t capacity, size_t needed) {
    size_t next = capacity == EMPTY_COUNT ? needed : capacity;
    if (next == EMPTY_COUNT) next = FIRST_RESERVATION;
    while (next < needed) {
        if (next > SIZE_MAX / GROWTH_FACTOR) fail("a collection exceeds addressable memory");
        next *= GROWTH_FACTOR;
    }
    return next;
}

static void *grow_list(void *items, size_t *capacity, size_t item_size, size_t needed) {
    if (needed <= *capacity) return items;
    size_t next = grown_capacity(*capacity, needed);
    if (item_size != EMPTY_COUNT && next > SIZE_MAX / item_size) fail("a collection exceeds addressable memory");
    void *grown = realloc(items, next * item_size);
    if (grown == NULL) fail("out of memory");
    *capacity = next;
    return grown;
}

static void *allocate_zeroed(size_t count, size_t item_size) {
    if (count == EMPTY_COUNT) return NULL;
    if (item_size != EMPTY_COUNT && count > SIZE_MAX / item_size) fail("a collection exceeds addressable memory");
    void *memory = calloc(count, item_size);
    if (memory == NULL) fail("out of memory");
    return memory;
}

static int bytes_equal(const char *left, size_t left_length, const char *right, size_t right_length) {
    if (left_length != right_length) return 0;
    if (left_length == EMPTY_COUNT) return 1;
    return memcmp(left, right, left_length) == STRING_COMPARE_EQUAL;
}

static int bytes_equal_cstr(const char *left, size_t left_length, const char *right) {
    return bytes_equal(left, left_length, right, strlen(right));
}

static char *copy_bytes(const char *bytes, size_t length) {
    if (length > SIZE_MAX - TEXT_TERMINATOR_SIZE) fail("text exceeds addressable memory");
    char *copy = malloc(length + TEXT_TERMINATOR_SIZE);
    if (copy == NULL) fail("out of memory");
    if (length != EMPTY_COUNT) memcpy(copy, bytes, length);
    copy[length] = TEXT_TERMINATOR;
    return copy;
}

static Value nothing_value(void) {
    Value value = {
        .kind = KIND_NOTHING,
        .integer = WORD_FALSE,
        .real = REAL_ZERO,
        .text = NULL,
        .text_length = EMPTY_COUNT,
        .items = NULL,
        .item_count = EMPTY_COUNT,
        .body = NULL,
        .parameters = NULL,
        .parameter_count = EMPTY_COUNT,
        .captures = NULL,
        .capture_count = EMPTY_COUNT
    };
    return value;
}

static Value integer_value(long long integer) {
    Value value = nothing_value();
    value.kind = KIND_INTEGER;
    value.integer = integer;
    return value;
}

/* Form arithmetic is signed 63-bit. Values decoded from the int64 wire keep
 * their full payload until an arithmetic operation is requested. */
static long long form_integer_wrap(uint64_t bits) {
    bits &= UINT64_C(0x7fffffffffffffff);
    return (bits & UINT64_C(0x4000000000000000))
        ? (long long)(bits & UINT64_C(0x3fffffffffffffff)) - INT64_C(4611686018427387904)
        : (long long)bits;
}

static long long decimal_integer(const char *text, size_t length) {
    size_t at = length && (text[0] == '-' || text[0] == '+') ? 1 : 0;
    uint64_t bits = 0;
    for (; at < length; ++at) bits = bits * 10 + (unsigned char)text[at] - '0';
    return form_integer_wrap(length && text[0] == '-' ? UINT64_C(0) - bits : bits);
}

static Value real_value(double real) {
    Value value = nothing_value();
    value.kind = KIND_REAL;
    value.real = real;
    return value;
}

static Value text_value(const char *bytes, size_t length) {
    Value value = nothing_value();
    value.kind = KIND_TEXT;
    value.text = heap_allocate(checked_add(length,1),1,HEAP_TEXT,length);
    if (length != 0) memcpy(value.text,bytes,length);
    value.text_length = length;
    return value;
}

static const size_t LIST_DIRECT = 64;

static Value inherit_owner(Value value, Value source) {
    if (value.owner == NULL) value.owner = source.owner;
    return value;
}

static Value list_at(Value list, size_t index) {
    List *cell = list.list;
    if (cell == NULL || index >= list.item_count) return nothing_value();
    if (cell->flat != NULL && cell->flat_count == list.item_count && index < cell->flat_count) return inherit_owner(cell->flat[index],list);
    if (cell->indexed != NULL && index >= cell->lead) {
        size_t within = index - cell->lead;
        if (within < cell->indexed->flat_count) return inherit_owner(cell->indexed->flat[within],list);
    }
    if (list.item_count > LIST_DIRECT && index >= LIST_DIRECT && cell->flat == NULL) {
        Value *flat = allocate_zeroed(list.item_count, sizeof *flat);
        List *cursor = cell;
        for (size_t step = FIRST_INDEX; step < list.item_count; step = checked_add(step, ONE_ELEMENT)) {
            if (cursor == NULL) break;
            flat[step] = cursor->head;
            cursor = cursor->tail;
        }
        cell->flat = flat;
        cell->flat_count = list.item_count;
        cell->indexed = NULL;
        cell->lead = EMPTY_COUNT;
        return inherit_owner(flat[index],list);
    }
    for (size_t step = FIRST_INDEX; step < index; step = checked_add(step, ONE_ELEMENT)) cell = cell->tail;
    return inherit_owner(cell->head,list);
}

static Value node_value(long long identity) {
    Value value = nothing_value();
    value.kind = KIND_NODE;
    value.integer = identity;
    return value;
}

static int values_equal(Value left, Value right);
static int values_identical(Value left, Value right);
static void expect_kind(Value value, ValueKind kind, const char *reason);
static Value list_from(Value *items, size_t item_count);
static const char *kind_name(ValueKind kind);
static Value boolean_word(int predicate);

static const uint64_t VALUE_HASH_OFFSET = 14695981039346656037ULL;
static const uint64_t VALUE_HASH_PRIME = 1099511628211ULL;
static const size_t CELL_BUCKET_START = 1024;

typedef struct CellBucket CellBucket;
struct CellBucket {
    Cell *cell;
    CellBucket *next;
};

static Cell **cells;
static size_t cell_count;
static size_t cell_capacity;
static CellBucket **cell_buckets;
static size_t cell_bucket_count;
static size_t cell_bucket_fill;
static size_t node_mint_events;
static int field_context_ready;
static long long field_device, field_inode;
static size_t field_nodes, field_strings, field_string_bytes, field_pairs;

static uint64_t hash_mix(uint64_t hash, uint64_t word) {
    return (hash ^ word) * VALUE_HASH_PRIME;
}

static uint64_t hash_bytes(uint64_t hash, const char *bytes, size_t length) {
    for (size_t index = FIRST_INDEX; index < length; index = checked_add(index, ONE_ELEMENT)) {
        hash = hash_mix(hash, (unsigned char)bytes[index]);
    }
    return hash;
}

static uint64_t hash_value(Value value) {
    uint64_t hash = hash_mix(VALUE_HASH_OFFSET, (uint64_t)value.kind);
    if (value.kind == KIND_INTEGER || value.kind == KIND_NODE) hash = hash_mix(hash, (uint64_t)value.integer);
    if (value.kind == KIND_TEXT) hash = hash_bytes(hash, value.text, value.text_length);
    if (value.kind == KIND_REAL) {
        uint64_t bits = 0;
        memcpy(&bits, &value.real, sizeof bits);
        hash = hash_mix(hash, bits);
    }
    if (value.kind == KIND_LIST) {
        hash = hash_mix(hash, (uint64_t)value.item_count);
        for (List *cell = value.list; cell != NULL; cell = cell->tail) {
            hash = hash_mix(hash, hash_value(cell->head));
            if (cell->dotted_tail != NULL) hash = hash_mix(hash_mix(hash,KIND_LIST),hash_value(*cell->dotted_tail));
        }
    }
    return hash;
}

static void cell_rehash(size_t count) {
    CellBucket **buckets = allocate_zeroed(count, sizeof *buckets);
    for (size_t index = FIRST_INDEX; index < cell_bucket_count; index = checked_add(index, ONE_ELEMENT)) {
        CellBucket *slot = cell_buckets[index];
        while (slot != NULL) {
            CellBucket *next = slot->next;
            size_t place = (size_t)(slot->cell->hash & (uint64_t)(count - ONE_ELEMENT));
            slot->next = buckets[place];
            buckets[place] = slot;
            slot = next;
        }
    }
    free(cell_buckets);
    cell_buckets = buckets;
    cell_bucket_count = count;
}

static void cell_index(Cell *cell) {
    if (cell_bucket_count == EMPTY_COUNT) cell_rehash(CELL_BUCKET_START);
    if (cell_bucket_fill >= cell_bucket_count) cell_rehash(cell_bucket_count * GROWTH_FACTOR);
    size_t place = (size_t)(cell->hash & (uint64_t)(cell_bucket_count - ONE_ELEMENT));
    CellBucket *slot = allocate_zeroed(ONE_ELEMENT, sizeof *slot);
    slot->cell = cell;
    slot->next = cell_buckets[place];
    cell_buckets[place] = slot;
    cell_bucket_fill = checked_add(cell_bucket_fill, ONE_ELEMENT);
}

static Cell *cell_hold(void) {
    if (cell_count == cell_capacity) {
        size_t capacity = cell_capacity == EMPTY_COUNT ? FIRST_RESERVATION : cell_capacity;
        if (cell_capacity != EMPTY_COUNT && capacity > SIZE_MAX / GROWTH_FACTOR) fail("the node table exceeds addressable memory");
        if (cell_capacity != EMPTY_COUNT) capacity *= GROWTH_FACTOR;
        Cell **grown = allocate_zeroed(capacity, sizeof *grown);
        if (cell_count != EMPTY_COUNT) memcpy(grown, cells, cell_count * sizeof *grown);
        free(cells);
        cells = grown;
        cell_capacity = capacity;
    }
    Cell *cell = allocate_zeroed(ONE_ELEMENT, sizeof *cell);
    cells[cell_count] = cell;
    cell_count = checked_add(cell_count, ONE_ELEMENT);
    return cell;
}

static Value node_from_cell(Cell *cell) {
    Value value = node_value((long long)cell->category);
    value.cell = cell;
    return value;
}

static fk_node_id node_word(Value node, const char *reason) {
    expect_kind(node, KIND_NODE, reason);
    if (node.cell != NULL) return node.cell->category;
    return (fk_node_id)node.integer;
}

static Value node_field(Value node, int field, const char *reason) {
    if (node.kind == KIND_NOTHING) return nothing_value();
    if (node.kind != KIND_NODE) {
        char detail[80];
        snprintf(detail, sizeof detail, "%s %lld", kind_name(node.kind), node.integer);
        fail(message_with_name("a node field expects a node, received ", detail, strlen(detail)));
    }
    (void)reason;
    return integer_value(fk_nid_get(node_word(node, reason), field));
}


static CellBucket *cell_chain(uint64_t hash) {
    if (cell_bucket_count == EMPTY_COUNT) return NULL;
    return cell_buckets[(size_t)(hash & (uint64_t)(cell_bucket_count - ONE_ELEMENT))];
}

static Value offer_make_nodeid(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[NODE_FIELD_PACKAGE], KIND_INTEGER, "make_nodeid expects a package");
    expect_kind(arguments[NODE_FIELD_LEVEL], KIND_INTEGER, "make_nodeid expects a level");
    expect_kind(arguments[NODE_FIELD_TYPE], KIND_INTEGER, "make_nodeid expects a type");
    expect_kind(arguments[NODE_FIELD_INSTANCE], KIND_INTEGER, "make_nodeid expects an instance");
    if (!fk_nid_fits(arguments[0].integer, arguments[1].integer, arguments[2].integer, arguments[3].integer))
        fail("coordinate is outside the native 64-bit node identity layout");
    fk_node_id identity = fk_nid_make(arguments[NODE_FIELD_PACKAGE].integer, arguments[NODE_FIELD_LEVEL].integer, arguments[NODE_FIELD_TYPE].integer, arguments[NODE_FIELD_INSTANCE].integer);
    return node_value((long long)identity);
}

static Value offer_node_pkg(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return node_field(arguments[SOLE_OPERAND], NODE_FIELD_PACKAGE, "node_pkg expects a node");
}

static Value offer_node_level(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return node_field(arguments[SOLE_OPERAND], NODE_FIELD_LEVEL, "node_level expects a node");
}

static Value offer_node_type(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return node_field(arguments[SOLE_OPERAND], NODE_FIELD_TYPE, "node_type expects a node");
}

static Value offer_node_inst(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return node_field(arguments[SOLE_OPERAND], NODE_FIELD_INSTANCE, "node_inst expects a node");
}

static Value immediate_float32(double number) {
    float narrow = (float)number;
    uint32_t bits;
    memcpy(&bits, &narrow, sizeof bits);
    if (isnan(narrow)) bits = UINT32_C(0x7fc00000);
    else if (narrow == 0.0f) bits = 0;
    return node_value((long long)fk_nid_make(1, 1, 6, bits));
}

static double decimal_prefix(Value text) {
    if (text.kind != KIND_TEXT) return 0.0;
    size_t i = 0;
    while (i < text.text_length && isspace((unsigned char)text.text[i])) ++i;
    size_t start = i;
    if (i < text.text_length && (text.text[i] == '+' || text.text[i] == '-')) ++i;
    size_t digits = 0;
    while (i < text.text_length && isdigit((unsigned char)text.text[i])) { ++i; ++digits; }
    if (i < text.text_length && text.text[i] == '.') {
        ++i;
        while (i < text.text_length && isdigit((unsigned char)text.text[i])) { ++i; ++digits; }
    }
    if (digits == 0) return 0.0;
    if (i < text.text_length && (text.text[i] == 'e' || text.text[i] == 'E')) {
        size_t exponent = i++;
        if (i < text.text_length && (text.text[i] == '+' || text.text[i] == '-')) ++i;
        size_t exponent_digits = i;
        while (i < text.text_length && isdigit((unsigned char)text.text[i])) ++i;
        if (i == exponent_digits) i = exponent;
    }
    char *number = copy_bytes(text.text + start, i - start);
    double result = strtod(number, NULL);
    free(number);
    return result;
}

static Value offer_str_to_float(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_TEXT, "str_to_float expects text");
    return real_value(decimal_prefix(arguments[0]));
}

static Value offer_make_float32(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind == KIND_REAL) return immediate_float32(arguments[0].real);
    expect_kind(arguments[0], KIND_INTEGER, "make_float32 expects a number");
    return immediate_float32((double)arguments[0].integer);
}

static Value offer_float_value(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind == KIND_NODE && fk_nid_get((fk_node_id)arguments[0].integer, 1) == 1 && fk_nid_get((fk_node_id)arguments[0].integer, 2) == 6) {
        uint32_t bits = (uint32_t)fk_nid_get((fk_node_id)arguments[0].integer, 3);
        float value;
        memcpy(&value, &bits, sizeof value);
        return real_value((double)value);
    }
    return arguments[0].kind == KIND_NODE && arguments[0].cell != NULL && arguments[0].cell->kind == CELL_REAL
        ? real_value(arguments[0].cell->real) : nothing_value();
}

static Value offer_float_to_int(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind == KIND_INTEGER) return arguments[0];
    expect_kind(arguments[0], KIND_REAL, "float_to_int expects a number");
    double number = arguments[0].real;
    if (!isfinite(number) || number < -4611686018427387904.0 || number >= 4611686018427387904.0) fail("float_to_int is outside the Form signed integer range");
    return integer_value((long long)number);
}

static Value offer_node_children(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_NODE, "node_children expects a node");
    Cell *cell = arguments[SOLE_OPERAND].cell;
    return cell != NULL && cell->kind == CELL_COMPOSITE ? inherit_owner(cell->children_value,arguments[SOLE_OPERAND]) : list_from(NULL,EMPTY_COUNT);
}

static Value offer_node_category(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[SOLE_OPERAND].kind != KIND_NODE) {
        fail(message_with_name("node_category expects a node, received ", kind_name(arguments[SOLE_OPERAND].kind), strlen(kind_name(arguments[SOLE_OPERAND].kind))));
    }
    if (arguments[SOLE_OPERAND].cell == NULL || arguments[SOLE_OPERAND].cell->kind != CELL_COMPOSITE) return arguments[SOLE_OPERAND];
    return inherit_owner(arguments[SOLE_OPERAND].cell->category_value,arguments[SOLE_OPERAND]);
}

static Value offer_node_payload(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    expect_kind(arguments[SOLE_OPERAND], KIND_NODE, "node_value expects a node");
    Cell *cell = arguments[SOLE_OPERAND].cell;
    if (cell == NULL) {
        fk_node_id coordinate = (fk_node_id)arguments[SOLE_OPERAND].integer;
        if (fk_nid_get(coordinate, 0) == 1 && fk_nid_get(coordinate, 1) == 1 &&
            (fk_nid_get(coordinate, 2) == 1 || fk_nid_get(coordinate, 2) == 3)) return integer_value(fk_nid_get(coordinate, 3));
        if (fk_nid_get(coordinate, 1) == 1 && fk_nid_get(coordinate, 2) == 6) return offer_float_value(arguments, 1);
        return nothing_value();
    }
    if (cell->kind == CELL_TEXT) return text_value(cell->text, cell->text_length);
    if (cell->kind == CELL_INTEGER) return integer_value(cell->integer);
    if (cell->kind == CELL_REAL) return real_value(cell->real);
    return nothing_value();
}

static Value offer_node_eq(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (left.kind != KIND_NODE || right.kind != KIND_NODE) return boolean_word(0);
    return boolean_word(values_equal(left, right));
}

static Value offer_node_source(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_RECORD) {
        for (size_t i = 0; i < frame_record_count; ++i) {
            if (!values_equal(frame_records[i].word, arguments[SOLE_OPERAND])) continue;
            Value fields[3] = {text_value(frame_records[i].path, frame_records[i].path_length),
                integer_value(frame_records[i].position / SOURCE_LINE_STRIDE),
                integer_value(frame_records[i].position % SOURCE_LINE_STRIDE)};
            return list_from(fields, 3);
        }
        return list_from(NULL, EMPTY_COUNT);
    }
    if (arguments[SOLE_OPERAND].kind != KIND_NODE) {
        fail(message_with_name("node_source expects a node, received ", kind_name(arguments[SOLE_OPERAND].kind), strlen(kind_name(arguments[SOLE_OPERAND].kind))));
    }
    Cell *cell = arguments[SOLE_OPERAND].cell;
    if (cell == NULL || cell->source == NULL) return list_from(NULL, EMPTY_COUNT);
    Value *fields = allocate_zeroed(ARITY_THREE, sizeof *fields);
    fields[NODE_FIELD_PACKAGE] = text_value(cell->source, cell->source_length);
    fields[NODE_FIELD_LEVEL] = integer_value(cell->source_line);
    fields[NODE_FIELD_TYPE] = integer_value(cell->source_column);
    Value result = list_from(fields, ARITY_THREE); free(fields); return result;
}

/* Form has already chosen and committed the identity. This terminal carrier
 * only attaches a local view; it never interns, chooses a row, or calls Form. */
static int field_context(Value identity, int bind) {
    if (!value_proper_list(identity) || identity.item_count!=2 || list_at(identity,0).kind!=KIND_INTEGER || list_at(identity,1).kind!=KIND_INTEGER) return 0;
    long long device=list_at(identity,0).integer, inode=list_at(identity,1).integer;
    if (field_context_ready && (device!=field_device || inode!=field_inode)) fail("native field context changed; existing values retain their store owner");
    if (!field_context_ready && bind) { field_device=device; field_inode=inode; field_context_ready=1; }
    return field_context_ready;
}
static uint64_t field_view_hash(fk_node_id identity) { return hash_mix(VALUE_HASH_OFFSET,(uint64_t)identity); }
static Cell *field_view(fk_node_id identity) {
    uint64_t hash=field_view_hash(identity);
    for (CellBucket *slot=cell_chain(hash); slot!=NULL; slot=slot->next)
        if (slot->cell->field_ref>0 && slot->cell->category==identity) return slot->cell;
    return NULL;
}
static void field_view_source(Cell *cell,Value source) {
    if (!value_proper_list(source) || (source.item_count!=0 && source.item_count!=3)) fail("native field source view refused");
    if (source.item_count==0) return;
    Value path=list_at(source,0), line=list_at(source,1), column=list_at(source,2);
    if (path.kind!=KIND_TEXT || line.kind!=KIND_INTEGER || column.kind!=KIND_INTEGER) fail("native field source view types refused");
    if (cell->source!=NULL) {
        if (!bytes_equal(cell->source,cell->source_length,path.text,path.text_length) || cell->source_line!=line.integer || cell->source_column!=column.integer) fail("native field source changed after attachment");
        return;
    }
    cell->source=copy_bytes(path.text,path.text_length); cell->source_length=path.text_length;
    cell->source_line=line.integer; cell->source_column=column.integer;
}
/* Weak capabilities: Form chooses admission and identity, while GC retires a
 * map whose result was never delivered or whose caller has released it. */
typedef struct { Record *record; long long device, inode; } FieldMap;
static FieldMap *field_maps;
static size_t field_map_count, field_map_capacity;
static long long field_published_pid;
static Value record_value(Record *record);
static Value offer_fb_record(Value *arguments, size_t argument_count);
static Value gift_field_leaf(Value request);
static Value native_field_leaf(Value request) {
    if (!value_proper_list(request) || request.item_count<3 || list_at(request,0).kind!=KIND_TEXT) return nothing_value();
    Value command=list_at(request,0), identity=list_at(request,1);
    if (command.text_length>=5 && memcmp(command.text,"gift-",5)==0) return gift_field_leaf(request);
    if (bytes_equal_cstr(command.text,command.text_length,"live")) return offer_kernel_live(&identity,1);
    if (bytes_equal_cstr(command.text,command.text_length,"map-open")) {
        if (request.item_count!=3 || !field_context(identity,0)) return nothing_value();
        size_t slot=0; while (slot<field_map_count && field_maps[slot].record!=NULL) ++slot;
        if (slot==field_map_count) field_maps=grow_list(field_maps,&field_map_capacity,sizeof *field_maps,field_map_count+1);
        Value handle=record_value(heap_allocate(1,sizeof(Record),HEAP_RECORD,1));
        field_maps[slot]=(FieldMap){handle.record,field_device,field_inode};
        if (slot==field_map_count) ++field_map_count;
        return handle;
    }
    if (bytes_equal_cstr(command.text,command.text_length,"map-held") || bytes_equal_cstr(command.text,command.text_length,"map-release")) {
        Value handle=list_at(request,2);
        if (handle.kind!=KIND_RECORD) return integer_value(0);
        for (size_t i=0;i<field_map_count;++i) if (field_maps[i].record==handle.record) {
            if (bytes_equal_cstr(command.text,command.text_length,"map-release")) { field_maps[i].record=NULL; return integer_value(1); }
            return integer_value(value_proper_list(identity) && identity.item_count==2 && list_at(identity,0).kind==KIND_INTEGER && list_at(identity,1).kind==KIND_INTEGER && list_at(identity,0).integer==field_maps[i].device && list_at(identity,1).integer==field_maps[i].inode);
        }
        return integer_value(0);
    }
    if (bytes_equal_cstr(command.text,command.text_length,"map-count")) {
        size_t live=0; for (size_t i=0;i<field_map_count;++i) live+=field_maps[i].record!=NULL;
        return integer_value((long long)live);
    }
    if (bytes_equal_cstr(command.text,command.text_length,"frame-record")) {
        Value args=list_at(request,2);
        if (!value_proper_list(args) || args.item_count!=3) return nothing_value();
        Value fields[]={list_at(args,0),list_at(args,1),list_at(args,2)};
        return offer_fb_record(fields,3);
    }
    if (bytes_equal_cstr(command.text,command.text_length,"published")) {
        if (request.item_count!=4) return nothing_value();
        Value counts=list_at(request,2), minted=list_at(request,3);
        if (!value_proper_list(counts) || counts.item_count!=4 || minted.kind!=KIND_INTEGER || minted.integer<0) return nothing_value();
        for (size_t i=0;i<4;++i) if (list_at(counts,i).kind!=KIND_INTEGER || list_at(counts,i).integer<0 || (uint64_t)list_at(counts,i).integer>SIZE_MAX) return nothing_value();
        if ((uint64_t)minted.integer>SIZE_MAX-node_mint_events || !field_context(identity,1)) return nothing_value();
        int changed=field_published_pid!=(long long)getpid() || field_nodes!=(size_t)list_at(counts,0).integer || field_strings!=(size_t)list_at(counts,1).integer || field_pairs!=(size_t)list_at(counts,3).integer || minted.integer!=0;
        field_nodes=(size_t)list_at(counts,0).integer; field_strings=(size_t)list_at(counts,1).integer;
        field_string_bytes=(size_t)list_at(counts,2).integer; field_pairs=(size_t)list_at(counts,3).integer;
        node_mint_events+=(size_t)minted.integer;
        for (size_t i=minted.integer>0 ? frame_count : 0;i>0;--i) if (frames[i-1].kind==FRAME_FUNCTION && frames[i-1].callee.body!=NULL) {
            frames[i-1].callee.body->minted_count+=(size_t)minted.integer; break;
        }
        if (changed) {
            field_published_pid=0;
            Value pid=integer_value((long long)getpid()); Value page=offer_kernel_live(&pid,1);
            if (page.kind!=KIND_LIST || page.item_count!=LIVE_PAGE_WORDS) return nothing_value();
            field_published_pid=(long long)getpid();
        }
        return integer_value(1);
    }
    if (bytes_equal_cstr(command.text,command.text_length,"view")) {
        Value node=list_at(request,2);
        if ((request.item_count!=3 && request.item_count!=4) || node.kind!=KIND_NODE || !field_context(identity,0)) return nothing_value();
        Cell *cell=field_view((fk_node_id)node.integer);
        if (cell!=NULL && request.item_count==4) field_view_source(cell,list_at(request,3));
        return cell==NULL ? nothing_value() : node_from_cell(cell);
    }
    if (!bytes_equal_cstr(command.text,command.text_length,"attach") || request.item_count!=9) return nothing_value();
    Value ref=list_at(request,2), node=list_at(request,3), kind=list_at(request,4), payload=list_at(request,5);
    Value category=list_at(request,6), children=list_at(request,7), source=list_at(request,8);
    if (ref.kind!=KIND_INTEGER || ref.integer<=0 || ref.integer>UINT32_MAX || node.kind!=KIND_NODE || kind.kind!=KIND_TEXT) return nothing_value();
    CellKind type;
    if (bytes_equal_cstr(kind.text,kind.text_length,"string") && payload.kind==KIND_TEXT) type=CELL_TEXT;
    else if (bytes_equal_cstr(kind.text,kind.text_length,"integer") && payload.kind==KIND_INTEGER) type=CELL_INTEGER;
    else if (bytes_equal_cstr(kind.text,kind.text_length,"float64") && payload.kind==KIND_REAL) type=CELL_REAL;
    else if (bytes_equal_cstr(kind.text,kind.text_length,"composite") && category.kind==KIND_NODE && children.kind==KIND_LIST) type=CELL_COMPOSITE;
    else return nothing_value();
    fk_node_id expected=type==CELL_TEXT ? fk_nid_make(1,1,2,ref.integer) : type==CELL_REAL ? fk_nid_make(1,1,7,ref.integer) : type==CELL_COMPOSITE ? fk_nid_make(0,2,0,ref.integer) : fk_nid_fits(1,1,1,payload.integer) ? fk_nid_make(1,1,1,payload.integer) : fk_nid_make(1,1,5,ref.integer);
    if ((fk_node_id)node.integer!=expected || !value_proper_list(source) || (source.item_count!=0 && source.item_count!=3)) return nothing_value();
    if (source.item_count==3 && (list_at(source,0).kind!=KIND_TEXT || list_at(source,1).kind!=KIND_INTEGER || list_at(source,2).kind!=KIND_INTEGER)) return nothing_value();
    if (!field_context(identity,0)) return nothing_value();
    Cell *existing=field_view((fk_node_id)node.integer);
    if (existing!=NULL) {
        if (existing->field_ref!=ref.integer || existing->kind!=type) fail("native field view identity disagrees with its committed row");
        int same=type==CELL_TEXT ? bytes_equal(existing->text,existing->text_length,payload.text,payload.text_length)
            : type==CELL_INTEGER ? existing->integer==payload.integer
            : type==CELL_REAL ? memcmp(&existing->real,&payload.real,sizeof payload.real)==0
            : values_identical(existing->category_value,category) && values_identical(existing->children_value,children);
        if (!same) fail("native field immutable view disagrees with its committed row");
        field_view_source(existing,source); return node_from_cell(existing);
    }
    Cell *cell=cell_hold();
    cell->category=(fk_node_id)node.integer; cell->kind=type; cell->field_ref=ref.integer;
    cell->hash=field_view_hash(cell->category);
    if (type==CELL_TEXT) { cell->text=copy_bytes(payload.text,payload.text_length); cell->text_length=payload.text_length; }
    else if (type==CELL_INTEGER) cell->integer=payload.integer;
    else if (type==CELL_REAL) cell->real=payload.real;
    else { cell->category_value=category; cell->children_value=children; }
    field_view_source(cell,source); cell_index(cell); return node_from_cell(cell);
}

static Value boolean_word(int predicate) {
    return integer_value(predicate ? WORD_TRUE : WORD_FALSE);
}

static int integer_is_nonzero(Value value) {
    if (value.kind != KIND_INTEGER) fail("a branch expects a present integer truth value");
    return value.integer != WORD_FALSE;
}

static int is_digit_char(char character) {
    return character >= DIGIT_ZERO && character <= DIGIT_NINE;
}

static const char *kind_name(ValueKind kind) {
    if (kind == KIND_NOTHING) return KIND_NAME_NOTHING;
    if (kind == KIND_INTEGER) return KIND_NAME_INTEGER;
    if (kind == KIND_REAL) return KIND_NAME_REAL;
    if (kind == KIND_TEXT) return KIND_NAME_TEXT;
    if (kind == KIND_LIST) return KIND_NAME_LIST;
    if (kind == KIND_FUNCTION) return KIND_NAME_FUNCTION;
    if (kind == KIND_NODE) return KIND_NAME_NODE;
    if (kind == KIND_RECORD) return KIND_NAME_RECORD;
    fail("a word has an unknown kind");
    return KIND_NAME_NOTHING;
}

static void expect_kind(Value value, ValueKind kind, const char *reason) {
    if (value.kind != kind) fail(reason);
}

static int is_number(Value value) {
    return value.kind == KIND_INTEGER || value.kind == KIND_REAL;
}

static double number_as_real(Value value) {
    if (value.kind == KIND_REAL) return value.real;
    return (double)value.integer;
}

static long long checked_integer_add(long long left, long long right) {
    return form_integer_wrap((uint64_t)left + (uint64_t)right);
}

static long long checked_integer_sub(long long left, long long right) {
    return form_integer_wrap((uint64_t)left - (uint64_t)right);
}

static long long checked_integer_mul(long long left, long long right) {
    return form_integer_wrap((uint64_t)left * (uint64_t)right);
}

static long long checked_integer_div(long long left, long long right) {
    if (right == WORD_FALSE) fail("div expects a nonzero divisor");
    if (left == LLONG_MIN && right == -WORD_TRUE) return form_integer_wrap((uint64_t)left);
    return form_integer_wrap((uint64_t)(left / right));
}

static long long checked_integer_mod(long long left, long long right) {
    if (right == WORD_FALSE) fail("mod expects a nonzero divisor");
    if (left == LLONG_MIN && right == -WORD_TRUE) return WORD_FALSE;
    return form_integer_wrap((uint64_t)(left % right));
}

static Form **syntax_forms;
static size_t syntax_count, syntax_capacity;

static Form *new_form(FormKind kind, size_t offset) {
    Form *form = allocate_zeroed(ONE_ELEMENT, sizeof *form);
    form->kind = kind;
    form->offset = offset;
    form->path = source_path;
    form->source = source_bytes;
    form->source_length = source_length;
    syntax_forms = grow_list(syntax_forms, &syntax_capacity, sizeof *syntax_forms, syntax_count + 1);
    form->syntax_id = syntax_count + 1;
    syntax_forms[syntax_count++] = form;
    return form;
}

static void group_add(Form *group, Form *child) {
    size_t needed = checked_add(group->part_count, ONE_ELEMENT);
    group->parts = grow_list(group->parts, &group->part_capacity, sizeof *group->parts, needed);
    group->parts[group->part_count] = child;
    group->part_count = needed;
}

static int form_is_symbol(Form *form, const char *spelling) {
    return form != NULL && form->kind == FORM_SYMBOL && bytes_equal_cstr(form->text, form->text_length, spelling);
}

static size_t call_argument_count(Form *call) {
    if (call->part_count == EMPTY_COUNT) return EMPTY_COUNT;
    return call->part_count - CALL_ARGUMENTS;
}

static Form *call_argument(Form *call, size_t index) {
    return call->parts[checked_add(CALL_ARGUMENTS, index)];
}

static void skip_space_and_comments(void) {
    while (source_offset < source_length) {
        unsigned char character = (unsigned char)source_bytes[source_offset];
        if (isspace(character)) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            continue;
        }
        if (source_bytes[source_offset] == LINE_COMMENT_MARK) {
            size_t next = checked_add(source_offset, ONE_ELEMENT);
            if (next < source_length && source_bytes[next] == LINE_COMMENT_MARK) {
                source_offset = checked_add(next, ONE_ELEMENT);
                while (source_offset < source_length && source_bytes[source_offset] != NEWLINE) {
                    source_offset = checked_add(source_offset, ONE_ELEMENT);
                }
                continue;
            }
        }
        if (source_bytes[source_offset] != COMMENT_INTRODUCER) return;
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        while (source_offset < source_length && source_bytes[source_offset] != NEWLINE) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
        }
    }
}

static void skip_bml(void) {
    while (source_offset < source_length) {
        unsigned char character = (unsigned char)source_bytes[source_offset];
        if (isspace(character)) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            continue;
        }
        if (character == ';') {
            size_t line = source_offset;
            while (line > 0 && source_bytes[line - 1] != '\n' && isspace((unsigned char)source_bytes[line - 1])) --line;
            if (line == 0 || source_bytes[line - 1] == '\n') {
                while (source_offset < source_length && source_bytes[source_offset] != '\n') ++source_offset;
                continue;
            }
        }
        if (source_bytes[source_offset] != LINE_COMMENT_MARK) return;
        size_t next = checked_add(source_offset, ONE_ELEMENT);
        if (next < source_length && source_bytes[next] == '*') {
            size_t opening = source_offset;
            source_offset = next + ONE_ELEMENT;
            while (source_offset + ONE_ELEMENT < source_length &&
                   !(source_bytes[source_offset] == '*' && source_bytes[source_offset + ONE_ELEMENT] == '/')) ++source_offset;
            if (source_offset + ONE_ELEMENT >= source_length) fail_at(opening, "a block comment was not closed");
            source_offset += ARITY_TWO;
            continue;
        }
        if (next >= source_length || source_bytes[next] != LINE_COMMENT_MARK) return;
        source_offset = checked_add(next, ONE_ELEMENT);
        while (source_offset < source_length && source_bytes[source_offset] != NEWLINE) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
        }
    }
}

static int token_is_number(const char *text, size_t length, Form *form) {
    size_t index = FIRST_INDEX;
    if (index < length && (text[index] == SIGN_PLUS || text[index] == SIGN_MINUS)) {
        index = checked_add(index, ONE_ELEMENT);
    }
    size_t digits_before = EMPTY_COUNT;
    while (index < length && is_digit_char(text[index])) {
        index = checked_add(index, ONE_ELEMENT);
        digits_before = checked_add(digits_before, ONE_ELEMENT);
    }
    int real = 0;
    if (index < length && text[index] == DECIMAL_POINT) {
        real = 1;
        index = checked_add(index, ONE_ELEMENT);
        size_t digits_after = EMPTY_COUNT;
        while (index < length && is_digit_char(text[index])) {
            index = checked_add(index, ONE_ELEMENT);
            digits_after = checked_add(digits_after, ONE_ELEMENT);
        }
        if (checked_add(digits_before, digits_after) == EMPTY_COUNT) return 0;
    } else if (digits_before == EMPTY_COUNT) {
        return 0;
    }
    if (index < length && (text[index] == EXPONENT_LOWER || text[index] == EXPONENT_UPPER)) {
        real = 1;
        index = checked_add(index, ONE_ELEMENT);
        if (index < length && (text[index] == SIGN_PLUS || text[index] == SIGN_MINUS)) {
            index = checked_add(index, ONE_ELEMENT);
        }
        size_t exponent_digits = EMPTY_COUNT;
        while (index < length && is_digit_char(text[index])) {
            index = checked_add(index, ONE_ELEMENT);
            exponent_digits = checked_add(exponent_digits, ONE_ELEMENT);
        }
        if (exponent_digits == EMPTY_COUNT) return 0;
    }
    if (index != length) return 0;
    errno = 0;
    if (!real) {
        form->kind = FORM_INTEGER;
        form->integer = decimal_integer(text, length);
        return 1;
    }
    double number = strtod(text, NULL);
    form->kind = FORM_REAL;
    form->real = number;
    return 1;
}

static char escaped_character(char mark, size_t offset) {
    if (mark == TEXT_DELIMITER || mark == ESCAPE_MARK || mark == LINE_COMMENT_MARK) return mark;
    if (mark == 'n') return NEWLINE;
    if (mark == 't') return TAB;
    if (mark == 'r') return RETURN_MARK;
    fail_at(offset, "a text escape was not recognized");
    return mark;
}

static Form *parse_text(void) {
    size_t opening = source_offset;
    source_offset = checked_add(source_offset, ONE_ELEMENT);
    char *text = NULL;
    size_t length = EMPTY_COUNT;
    size_t capacity = EMPTY_COUNT;
    while (source_offset < source_length && source_bytes[source_offset] != TEXT_DELIMITER) {
        char character = source_bytes[source_offset];
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        if (character == ESCAPE_MARK) {
            if (source_offset >= source_length) fail_at(opening, "a text escape was not finished");
            character = escaped_character(source_bytes[source_offset], source_offset);
            source_offset = checked_add(source_offset, ONE_ELEMENT);
        }
        size_t needed = checked_add(length, ONE_ELEMENT);
        text = grow_list(text, &capacity, sizeof *text, needed);
        text[length] = character;
        length = needed;
    }
    if (source_offset >= source_length) fail_at(opening, "a text value was not closed");
    source_offset = checked_add(source_offset, ONE_ELEMENT);
    size_t terminated = checked_add(length, TEXT_TERMINATOR_SIZE);
    text = grow_list(text, &capacity, sizeof *text, terminated);
    text[length] = TEXT_TERMINATOR;
    Form *form = new_form(FORM_TEXT, opening);
    form->text = text;
    form->text_length = length;
    return form;
}

static Form *parse_atom(void) {
    size_t start = source_offset;
    while (source_offset < source_length) {
        char character = source_bytes[source_offset];
        unsigned char classified = (unsigned char)character;
        if (isspace(classified) || character == CALL_OPEN || character == CALL_CLOSE || character == COMMENT_INTRODUCER) break;
        source_offset = checked_add(source_offset, ONE_ELEMENT);
    }
    if (source_offset == start) fail_at(start, "the reader did not advance");
    size_t length = source_offset - start;
    char *text = copy_bytes(source_bytes + start, length);
    Form *form = new_form(FORM_SYMBOL, start);
    form->text = text;
    form->text_length = length;
    if (token_is_number(text, length, form)) return form;
    return form;
}

static int at_source_section(void);
static Form *parse_source_section(void);

static Form **parse_source_span(size_t *form_count, int one) {
    typedef struct {
        Form *group;
        size_t open_offset;
    } OpenGroup;
    OpenGroup *open_groups = NULL;
    size_t open_count = EMPTY_COUNT;
    size_t open_capacity = EMPTY_COUNT;
    Form **top_forms = NULL;
    size_t top_count = EMPTY_COUNT;
    size_t top_capacity = EMPTY_COUNT;
    while (1) {
        if (one && open_count == 0 && top_count != 0) break;
        skip_space_and_comments();
        if (source_offset >= source_length) {
            if (open_count != EMPTY_COUNT) fail_at(open_groups[open_count - ONE_ELEMENT].open_offset, "a call was not closed");
            break;
        }
        char character = source_bytes[source_offset];
        if (character == CALL_CLOSE) {
            if (open_count == EMPTY_COUNT) fail_at(source_offset, "a closing delimiter has no call");
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            open_count -= ONE_ELEMENT;
            Form *finished = open_groups[open_count].group;
            if (open_count == EMPTY_COUNT) {
                size_t needed = checked_add(top_count, ONE_ELEMENT);
                top_forms = grow_list(top_forms, &top_capacity, sizeof *top_forms, needed);
                top_forms[top_count] = finished;
                top_count = needed;
            } else {
                group_add(open_groups[open_count - ONE_ELEMENT].group, finished);
            }
            continue;
        }
        if (character == CALL_OPEN) {
            size_t opening = source_offset;
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            size_t needed = checked_add(open_count, ONE_ELEMENT);
            open_groups = grow_list(open_groups, &open_capacity, sizeof *open_groups, needed);
            open_groups[open_count].group = new_form(FORM_GROUP, opening);
            open_groups[open_count].open_offset = opening;
            open_count = needed;
            continue;
        }
        Form *atom = at_source_section() ? parse_source_section() : character == TEXT_DELIMITER ? parse_text() : parse_atom();
        if (open_count == EMPTY_COUNT) {
            size_t needed = checked_add(top_count, ONE_ELEMENT);
            top_forms = grow_list(top_forms, &top_capacity, sizeof *top_forms, needed);
            top_forms[top_count] = atom;
            top_count = needed;
        } else {
            group_add(open_groups[open_count - ONE_ELEMENT].group, atom);
        }
    }
    *form_count = top_count;
    free(open_groups);
    return top_forms;
}

static char *read_source(const char *path, size_t *length) {
    FILE *file = fopen(path, SOURCE_OPEN_MODE);
    if (file == NULL) fail("the source file could not be opened");
    char *buffer = NULL;
    size_t capacity = EMPTY_COUNT;
    size_t filled = EMPTY_COUNT;
    while (1) {
        size_t needed = checked_add(filled, ONE_ELEMENT);
        size_t next = grown_capacity(capacity, needed);
        if (next > SIZE_MAX / sizeof *buffer) fail("source exceeds addressable memory");
        char *grown = realloc(buffer, next);
        if (grown == NULL) fail("out of memory");
        buffer = grown;
        size_t request = next - filled;
        size_t got = fread(buffer + filled, sizeof *buffer, request, file);
        filled = checked_add(filled, got);
        capacity = next;
        if (got < request) {
            if (ferror(file)) fail("the source file could not be read");
            break;
        }
    }
    fclose(file);
    if (memchr(buffer, TEXT_TERMINATOR, filled) != NULL) fail("source contains a zero byte");
    if (filled > SIZE_MAX - TEXT_TERMINATOR_SIZE) fail("source exceeds addressable memory");
    char *text = realloc(buffer, filled + TEXT_TERMINATOR_SIZE);
    if (text == NULL) fail("out of memory");
    text[filled] = TEXT_TERMINATOR;
    *length = filled;
    return text;
}

typedef struct {
    const char *bytes;
    size_t length, entry;
    uint64_t hash;
} NameSlot;

typedef struct {
    NameSlot *slots;
    size_t capacity, count;
} NameIndex;

static NameIndex operation_names, function_names, global_names;

typedef struct {
    char *from, *relative, *path;
    CellProgram *program;
} CellImport;
typedef struct { char *from, *relative, *path; int after_bind; } CellLoad;
typedef struct { char *path; const char *bytes; size_t length, serial; Form **forms; size_t form_count; Value binding_checks; int binding_checked; size_t initialization_order; } SourceView;
static size_t source_initialization_order;
static SourceView *source_views;
static size_t source_view_count, source_view_capacity;

struct CellProgram {
    CellProgram *next;
    CellProgram *closure_owner;
    size_t serial;
    char *path;
    const char *bytes;
    size_t length;
    Form **forms;
    size_t form_count;
    Binding *functions, *globals;
    size_t function_count, function_capacity, global_count, global_capacity, library_globals;
    NameIndex function_names, global_names;
    char **units;
    size_t unit_count, unit_capacity;
    uint64_t *digests;
    CellImport *imports;
    size_t import_count, import_capacity;
    CellLoad *loads;
    size_t load_count, load_capacity;
    int bindings_ready, disk_admitted;
    Value publication, image_diagnostics;
    SourceView *sources;
    size_t source_count, source_capacity;
    Value answer;
    Value diagnostics, diagnostic_text;
    int errors;
    int binding_refusal;
    int admitted, busy, library, initialized;
    void *loading_owner, *initializing_owner, *admission_owner;
    const char *initialization_stop, *initialization_message, *initialization_recipe, *initialization_unit;
};
static CellProgram main_program;
static CellProgram *current_program = &main_program, *cell_programs;
static size_t program_serial;
static char builder_identity[96];

static size_t program_id(CellProgram *program) {
    if (program == NULL) return 0;
    if (program->serial == 0) program->serial = ++program_serial;
    return program->serial;
}

static void source_view(CellProgram *program, const char *path, const char *bytes, size_t length) {
    for (size_t i = 0; i < program->source_count; ++i) if (program->sources[i].bytes == bytes) return;
    SourceView *view = NULL;
    for (size_t i = 0; i < source_view_count; ++i) if (source_views[i].bytes == bytes) { view = &source_views[i]; break; }
    if (view == NULL) {
        char *canonical = realpath(path, NULL);
        source_views = grow_list(source_views, &source_view_capacity, sizeof *source_views, source_view_count + 1);
        view = &source_views[source_view_count++];
        *view = (SourceView){canonical != NULL ? canonical : strdup(path), bytes, length, source_view_count};
    }
    program->sources = grow_list(program->sources, &program->source_capacity, sizeof *program->sources, program->source_count + 1);
    program->sources[program->source_count++] = *view;
}

static const char *runtime_executable;

/* Bootstrap-only location carrier. Form owns source admission once its home
 * has loaded; this lookup leaves with the initial loader's move into Form. */
static char *executing_image_path(void) {
#if defined(__APPLE__)
    uint32_t size = 0;
    _NSGetExecutablePath(NULL, &size);
    char *path = allocate_zeroed(size, 1);
    if (_NSGetExecutablePath(path, &size) != 0) { free(path); return NULL; }
    char *canonical = realpath(path, NULL);
    if (canonical != NULL) { free(path); return canonical; }
    return path;
#elif defined(__linux__)
    return realpath("/proc/self/exe", NULL);
#else
    return runtime_executable == NULL ? NULL : realpath(runtime_executable, NULL);
#endif
}

static void admit_builder(void) {
#if defined(__APPLE__)
    const struct mach_header *image = _dyld_get_image_header(0);
    const struct load_command *command = (const void *)((const char *)image + (image->magic == MH_MAGIC_64 ? sizeof(struct mach_header_64) : sizeof(struct mach_header)));
    const unsigned char *uuid = NULL;
    uint64_t fingerprint = 0;
    for (uint32_t i = 0; i < image->ncmds; ++i) {
        if (command->cmd == LC_UUID) uuid = ((const struct uuid_command *)command)->uuid;
        if (command->cmd == LC_SEGMENT_64) {
            const struct segment_command_64 *segment = (const void *)command;
            if (strcmp(segment->segname, "__TEXT") == 0) fingerprint = hash_bytes(1469598103934665603ULL, (const void *)(segment->vmaddr + _dyld_get_image_vmaddr_slide(0)), segment->filesize);
        }
        command = (const void *)((const char *)command + command->cmdsize);
    }
    if (uuid == NULL || fingerprint == 0) fail("loaded executable identity is unavailable");
    size_t at = (size_t)snprintf(builder_identity, sizeof builder_identity, "fkwu-image-v1 ");
    for (size_t i = 0; i < 16; ++i) at += (size_t)snprintf(builder_identity + at, sizeof builder_identity - at, "%02x", uuid[i]);
    snprintf(builder_identity + at, sizeof builder_identity - at, " %016llx", (unsigned long long)fingerprint);
#else
#if defined(__linux__)
    /* This handle stays on the loaded inode after replacement or unlink. */
    FILE *image = fopen("/proc/self/exe", "rb");
#else
    char *path = executing_image_path();
    if (path == NULL) fail("executing image location is unavailable");
    FILE *image = fopen(path, "rb");
    free(path);
#endif
    if (image == NULL) fail("executing image identity is unavailable");
    char bytes[65536]; size_t got, count = 0; uint64_t fingerprint = VALUE_HASH_OFFSET;
    while ((got = fread(bytes, 1, sizeof bytes, image)) != 0) { fingerprint = hash_bytes(fingerprint, bytes, got); count += got; }
    int valid = !ferror(image) && count != 0;
    if (fclose(image) != 0 || !valid) fail("executing image identity is incomplete");
    snprintf(builder_identity, sizeof builder_identity, "fkwu-image-v1 %016llx/%zu", (unsigned long long)fingerprint, count);
#endif
}

static void capture_program(CellProgram *program) {
    program->functions = functions; program->function_count = function_count; program->function_capacity = function_capacity;
    program->globals = globals; program->global_count = global_count; program->global_capacity = global_capacity;
    program->function_names = function_names; program->global_names = global_names;
    program->units = loaded_units; program->unit_count = loaded_count; program->unit_capacity = loaded_capacity;
}

typedef struct { const char *text; size_t length; size_t index; uint64_t epoch; } GlobalHit;
enum { GLOBAL_HIT_SLOTS = 8192 };
static GlobalHit global_hits[GLOBAL_HIT_SLOTS];
static uint64_t global_epoch = 1;

static void select_program(CellProgram *program) {
    if (program == current_program) return;
    ++global_epoch;
    capture_program(current_program);
    functions = program->functions; function_count = program->function_count; function_capacity = program->function_capacity;
    globals = program->globals; global_count = program->global_count; global_capacity = program->global_capacity;
    function_names = program->function_names; global_names = program->global_names;
    loaded_units = program->units; loaded_count = program->unit_count; loaded_capacity = program->unit_capacity;
    current_program = program;
}

static Value count_word(size_t count);
static Value list_from(Value *items, size_t item_count);
static const Operation *find_operation_text(const char *bytes, size_t length);
static char *try_resolve_unit(const char *from_file, const char *relative);

/* Parser-object transport only. Form owns lexical capture analysis, numbering,
 * dependency windows and static projection; captured values stay private. */
static Value syntax_callable(Value function, Value name, size_t offset) {
    if (function.kind != KIND_FUNCTION || function.body == NULL || function.native_operation != 0) return nothing_value();
    Value *parameters = allocate_zeroed(function.parameter_count, sizeof *parameters);
    Value *captures = allocate_zeroed(function.capture_count, sizeof *captures);
    for (size_t i = 0; i < function.parameter_count; ++i) parameters[i] = text_value(function.parameters[i].bytes, function.parameters[i].length);
    for (size_t i = 0; i < function.capture_count; ++i) captures[i] = text_value(function.captures[i].bytes, function.captures[i].length);
    Value fields[] = {name, list_from(parameters, function.parameter_count), count_word(function.body->syntax_id), function, count_word(offset), count_word(program_id(function.program)), list_from(captures, function.capture_count)};
    Value result = list_from(fields, 7); free(parameters); free(captures); return result;
}

static Value kernel_syntax(Value *arguments) {
    if (arguments[0].kind != KIND_INTEGER || arguments[0].integer != (long long)getpid()) return nothing_value();
    Value query = arguments[1];
    if (query.kind != KIND_LIST || query.item_count == 0) return nothing_value();
    Value command = list_at(query, 0), requested = list_at(query, 1);
    if (command.kind != KIND_TEXT) return nothing_value();
    capture_program(current_program);
    CellProgram *root = active_cell != NULL ? active_cell->program : &main_program;
    program_id(root);
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-builder")) return text_value(builder_identity, strlen(builder_identity));
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-resolve")) {
        Value relative = list_at(query, 2);
        if (requested.kind != KIND_TEXT || relative.kind != KIND_TEXT) return nothing_value();
        char *path = try_resolve_unit(requested.text, relative.text);
        if (path == NULL) return nothing_value();
        Value value = text_value(path, strlen(path)); free(path); return value;
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-cache-units")) {
        CellProgram **members = NULL; size_t count = 0, capacity = 0;
        members = grow_list(members, &capacity, sizeof *members, 1); members[count++] = root;
        for (size_t i = 0; i < count; ++i) for (size_t j = 0; j < members[i]->import_count; ++j) {
            CellProgram *child = members[i]->imports[j].program; int found = child == NULL;
            for (size_t k = 0; k < count; ++k) if (members[k] == child) found = 1;
            if (!found) { members = grow_list(members, &capacity, sizeof *members, count + 1); members[count++] = child; }
        }
        Value *rows = allocate_zeroed(count, sizeof *rows);
        for (size_t i = 0; i < count; ++i) {
            CellProgram *member = members[i];
            Value *forms = allocate_zeroed(member->form_count, sizeof *forms);
            for (size_t j = 0; j < member->form_count; ++j) forms[j] = count_word(member->forms[j]->syntax_id);
            Value *loads = allocate_zeroed(member->load_count, sizeof *loads);
            for (size_t j = 0; j < member->load_count; ++j) {
                CellLoad load = member->loads[j];
                Value row[] = {text_value(load.from, strlen(load.from)), text_value(load.relative, strlen(load.relative)), text_value(load.path, strlen(load.path)), integer_value(load.after_bind)};
                loads[j] = list_from(row, 4);
            }
            Value row[] = {text_value(member->path, strlen(member->path)), text_value(member->bytes, member->length), list_from(forms, member->form_count), list_from(loads, member->load_count)};
            rows[i] = list_from(row, 4); free(forms); free(loads);
        }
        Value result = list_from(rows, count); free(rows); free(members); return result;
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-owner")) {
        CellProgram *owner = root;
        if (requested.kind == KIND_FUNCTION && requested.program != NULL) owner = requested.program;
        else if (requested.kind == KIND_INTEGER && requested.integer > 0 && (size_t)requested.integer <= root->function_count) owner = root->functions[requested.integer - 1].value.program;
        for (CellProgram *member = owner; member != NULL; member = member->closure_owner) if (member->errors != 0) return nothing_value();
        return count_word(program_id(owner));
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-program")) {
        if (requested.kind != KIND_INTEGER || requested.integer <= 0) return nothing_value();
        CellProgram *program = main_program.serial == (size_t)requested.integer ? &main_program : NULL;
        for (CellProgram *item = cell_programs; program == NULL && item != NULL; item = item->next) if (item->serial == (size_t)requested.integer) program = item;
        if (program == NULL) return nothing_value();
        Value *sources = allocate_zeroed(program->source_count, sizeof *sources);
        for (size_t i = 0; i < program->source_count; ++i) {
            SourceView view = source_views[program->sources[i].serial - 1];
            Value *roots = allocate_zeroed(view.form_count, sizeof *roots);
            for (size_t j = 0; j < view.form_count; ++j) roots[j] = count_word(view.forms[j]->syntax_id);
            Value row[] = {text_value(view.path, strlen(view.path)), text_value(view.bytes, view.length), count_word(view.serial), list_from(roots, view.form_count), count_word(view.initialization_order)};
            sources[i] = list_from(row, 5); free(roots);
        }
        Value *bindings = allocate_zeroed(program->function_count, sizeof *bindings);
        for (size_t i = 0; i < program->function_count; ++i) {
            Binding binding = program->functions[i];
            Value name = text_value(binding.bytes, binding.length);
            Value descriptor = syntax_callable(binding.value, name, binding.name_offset);
            Value row[] = {name, count_word(program_id(binding.value.program)), count_word(binding.value.body->offset), count_word(binding.value.parameter_count), count_word(binding.value.body->syntax_id), list_at(descriptor, 1), count_word(binding.name_offset), binding.value};
            bindings[i] = list_from(row, 8);
        }
        Value *roots = allocate_zeroed(program->form_count, sizeof *roots), *imports = allocate_zeroed(program->import_count, sizeof *imports);
        for (size_t i = 0; i < program->form_count; ++i) roots[i] = count_word(program->forms[i]->syntax_id);
        for (size_t i = 0; i < program->import_count; ++i) {
            CellImport item = program->imports[i];
            Value edge[] = {text_value(item.from, strlen(item.from)), text_value(item.relative, strlen(item.relative)), text_value(item.path, strlen(item.path)), count_word(program_id(item.program))};
            imports[i] = list_from(edge, 4);
        }
        size_t source_owner = 0;
        for (size_t i = 0; i < program->source_count; ++i) if (program->sources[i].bytes == program->bytes) source_owner = program->sources[i].serial;
        Value row[] = {count_word(program->serial), text_value(program->path != NULL ? program->path : "", program->path != NULL ? strlen(program->path) : 0), list_from(sources, program->source_count), list_from(bindings, program->function_count), count_word(program_id(program->closure_owner)), list_from(roots, program->form_count), list_from(imports, program->import_count), count_word(source_owner)};
        free(sources); free(bindings); free(roots); free(imports); return list_from(row, 8);
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-header")) {
        Value fields[] = {count_word(root->serial), count_word(root->function_count), count_word(syntax_count)};
        return list_from(fields, 3);
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-roots")) {
        Value *roots = allocate_zeroed(root->form_count, sizeof *roots);
        for (size_t i = 0; i < root->form_count; ++i) roots[i] = count_word(root->forms[i]->syntax_id);
        Value result = list_from(roots, root->form_count); free(roots); return result;
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-all-roots")) {
        Value *units = allocate_zeroed(root->source_count, sizeof *units);
        for (size_t i = 0; i < root->source_count; ++i) {
            SourceView view = source_views[root->sources[i].serial - 1];
            Value *forms = allocate_zeroed(view.form_count, sizeof *forms);
            for (size_t j = 0; j < view.form_count; ++j) forms[j] = count_word(view.forms[j]->syntax_id);
            Value row[] = {text_value(view.path, strlen(view.path)), list_from(forms, view.form_count), view.binding_checked ? view.binding_checks : nothing_value()};
            units[i] = list_from(row, 3); free(forms);
        }
        Value value = list_from(units, root->source_count); free(units); return value;
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-index")) {
        if (requested.kind != KIND_FUNCTION || requested.capture_count != 0 || requested.native_operation != 0) return nothing_value();
        for (size_t i = 0; i < root->function_count; ++i) {
            Value function = root->functions[i].value;
            if (function.body == requested.body && function.program == requested.program) return count_word(i + 1);
        }
        return nothing_value();
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-heat")) {
        static uint64_t census;
        uint64_t counted = ++census, emitted = ++census;
        size_t count = 0;
        for (CellProgram *program = &main_program; program != NULL; program = program == &main_program ? cell_programs : program->next) {
            for (size_t i = 0; i < program->function_count; ++i) {
                Form *body = program->functions[i].value.body;
                if (body == NULL || body->entered_count == 0 || body->heat_census == counted) continue;
                body->heat_census = counted; count += 1;
            }
        }
        Value *rows = allocate_zeroed(count, sizeof *rows); size_t at = 0;
        for (CellProgram *program = &main_program; program != NULL; program = program == &main_program ? cell_programs : program->next) {
            for (size_t i = 0; i < program->function_count; ++i) {
                Binding binding = program->functions[i]; Form *body = binding.value.body;
                if (body == NULL || body->entered_count == 0 || body->heat_census == emitted) continue;
                body->heat_census = emitted;
                size_t line = 1, column = 1;
                for (size_t j = 0; j < binding.name_offset && j < body->source_length; ++j) {
                    if (body->source[j] == '\n') { line += 1; column = 1; } else column += 1;
                }
                const char *path = body->path != NULL ? body->path : "";
                /* Reals are inline, so no float-pool boxes/unboxes exist.
                 * Form bodies are interpreted; native-body status/folds are
                 * zero. Fresh node births are separately attributed. */
                Value fields[] = {count_word(body->entered_count), text_value(binding.bytes, binding.length), text_value(path, strlen(path)), count_word(line), count_word(column), integer_value(0), integer_value(0), integer_value(0), count_word(body->minted_count), integer_value(0)};
                rows[at++] = list_from(fields, 10);
            }
        }
        Value result = list_from(rows, at); free(rows); return result;
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-function") && requested.kind == KIND_FUNCTION)
        return syntax_callable(requested, text_value("", 0), requested.body != NULL ? requested.body->offset : 0);
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-function") && requested.kind == KIND_INTEGER && requested.integer >= 0 && (size_t)requested.integer < root->function_count) {
        Binding binding = root->functions[requested.integer];
        return syntax_callable(binding.value, text_value(binding.bytes, binding.length), binding.name_offset);
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-operation") && requested.kind == KIND_TEXT) {
        const Operation *operation = find_operation_text(requested.text, requested.text_length);
        if (operation == NULL) return nothing_value();
        Value rewrite = list_from(NULL, 0);
        for (size_t i = 0; i < sizeof(fk_rwtab) / sizeof(fk_rwtab[0]); ++i) {
            if (!bytes_equal_cstr(requested.text, requested.text_length, fk_rwtab[i].name)) continue;
            size_t count = (size_t)fk_rwtab[i].nprog;
            Value *words = allocate_zeroed(count, sizeof *words);
            for (size_t j = 0; j < count; ++j) words[j] = integer_value(fk_rwtab[i].prog[j]);
            rewrite = list_from(words, count); free(words); break;
        }
        int delegated = operation->id == OP_FIELD;
        const char *source = delegated && native_field_entry.body != NULL ? native_field_entry.body->path : NULL;
        Value fields[] = {integer_value(operation->tag), count_word(operation->arity), integer_value(operation->arity_policy == VARIADIC), integer_value(operation->evaluation == LEAVE_ARGUMENTS), rewrite,
            text_value(delegated ? "Form" : "native", delegated ? 4 : 6), source == NULL ? nothing_value() : text_value(source, strlen(source))};
        return list_from(fields, 7);
    }
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-declared-operation") && requested.kind == KIND_TEXT) {
        if (find_operation_text(requested.text, requested.text_length) != NULL) return integer_value(1);
        for (long long i = 0; i < fk_optab_n; ++i) if (bytes_equal_cstr(requested.text, requested.text_length, fk_optab[i].name)) return integer_value(1);
        for (size_t i = 0; i < sizeof(fk_rwtab) / sizeof(fk_rwtab[0]); ++i) if (bytes_equal_cstr(requested.text, requested.text_length, fk_rwtab[i].name)) return integer_value(1);
        return integer_value(0);
    }
    if (requested.kind != KIND_INTEGER || requested.integer <= 0 || (size_t)requested.integer > syntax_count) return nothing_value();
    Form *form = syntax_forms[requested.integer - 1];
    if (bytes_equal_cstr(command.text, command.text_length, "syntax-source")) return text_value(form->source, form->source_length);
    if (!bytes_equal_cstr(command.text, command.text_length, "syntax-node")) return nothing_value();
    if (form->syntax_view_ready) return form->syntax_view;
    const char *kind = form->kind == FORM_INTEGER ? "int" : form->kind == FORM_REAL ? "float" : form->kind == FORM_TEXT ? "text" : form->kind == FORM_SYMBOL ? "name" : "group";
    Value value = form->kind == FORM_INTEGER ? integer_value(form->integer) : form->kind == FORM_REAL ? real_value(form->real) :
        (form->kind == FORM_TEXT || form->kind == FORM_SYMBOL) ? text_value(form->text, form->text_length) : nothing_value();
    Value *children = allocate_zeroed(form->part_count, sizeof *children);
    for (size_t i = 0; i < form->part_count; ++i) children[i] = count_word(form->parts[i]->syntax_id);
    Value fields[] = {text_value(kind, strlen(kind)), value, list_from(children, form->part_count), text_value(form->path != NULL ? form->path : "", form->path != NULL ? strlen(form->path) : 0), count_word(form->offset), form->annotation != NULL ? count_word(form->annotation->syntax_id) : nothing_value()};
    free(children);
    form->syntax_view = list_from(fields, 6); form->syntax_view_ready = 1;
    return form->syntax_view;
}

static size_t name_index_find(NameIndex *index, const char *bytes, size_t length) {
    if (index->capacity == 0) return SIZE_MAX;
    uint64_t hash = hash_bytes(VALUE_HASH_OFFSET, bytes, length);
    size_t slot = (size_t)hash & (index->capacity - 1);
    while (index->slots[slot].entry != 0) {
        NameSlot row = index->slots[slot];
        if (row.hash == hash && bytes_equal(row.bytes, row.length, bytes, length)) return row.entry - 1;
        slot = (slot + 1) & (index->capacity - 1);
    }
    return SIZE_MAX;
}

static void name_index_set(NameIndex *index, const char *bytes, size_t length, size_t entry) {
    if (index->count >= index->capacity / 2) {
        size_t capacity = index->capacity == 0 ? 2 : checked_add(index->capacity, index->capacity);
        NameSlot *slots = allocate_zeroed(capacity, sizeof *slots);
        for (size_t i = 0; i < index->capacity; ++i) if (index->slots[i].entry != 0) {
            size_t slot = (size_t)index->slots[i].hash & (capacity - 1);
            while (slots[slot].entry != 0) slot = (slot + 1) & (capacity - 1);
            slots[slot] = index->slots[i];
        }
        free(index->slots);
        index->slots = slots;
        index->capacity = capacity;
    }
    uint64_t hash = hash_bytes(VALUE_HASH_OFFSET, bytes, length);
    size_t slot = (size_t)hash & (index->capacity - 1);
    while (index->slots[slot].entry != 0) {
        NameSlot row = index->slots[slot];
        if (row.hash == hash && bytes_equal(row.bytes, row.length, bytes, length)) break;
        slot = (slot + 1) & (index->capacity - 1);
    }
    if (index->slots[slot].entry == 0) ++index->count;
    index->slots[slot] = (NameSlot){bytes, length, checked_add(entry, 1), hash};
}

static void register_operation(const char *spelling, size_t arity, ArityPolicy arity_policy,
                               EvaluationPolicy evaluation, OperationId id, OperationOffer offer) {
    size_t needed = checked_add(operation_count, ONE_ELEMENT);
    operations = grow_list(operations, &operation_capacity, sizeof *operations, needed);
    operations[operation_count].spelling = spelling;
    operations[operation_count].arity = arity;
    operations[operation_count].arity_policy = arity_policy;
    operations[operation_count].evaluation = evaluation;
    operations[operation_count].id = id;
    operations[operation_count].tag = 0;
    for (long long row = 0; row < fk_optab_n; ++row) {
        if (strcmp(spelling, fk_optab[row].name) == 0) { operations[operation_count].tag = fk_optab[row].tag; break; }
    }
    operations[operation_count].offer = offer;
    operations[operation_count].calls = EMPTY_COUNT;
    operations[operation_count].answers = 0;
    name_index_set(&operation_names, spelling, strlen(spelling), operation_count);
    operation_count = needed;
}

static void operation_answers(const char *spelling) {
    size_t index = name_index_find(&operation_names, spelling, strlen(spelling));
    if (index != SIZE_MAX) operations[index].answers = 1;
}

static const Operation *find_operation_text(const char *bytes, size_t length) {
    size_t index = name_index_find(&operation_names, bytes, length);
    return index == SIZE_MAX ? NULL : &operations[index];
}

static Binding *find_function_text(const char *bytes, size_t length) {
    size_t index = name_index_find(&function_names, bytes, length);
    return index == SIZE_MAX ? NULL : &functions[index];
}

static Binding *find_function_owned(const char *bytes, size_t length, const char *path) {
    size_t first = name_index_find(&function_names, bytes, length);
    for (size_t i = first; path != NULL && i != SIZE_MAX; ) {
        Binding *binding = &functions[i];
        Form *body = binding->value.body;
        if (body != NULL && body->path != NULL && strcmp(path, body->path) == 0) return binding;
        i = binding->previous_function == 0 ? SIZE_MAX : binding->previous_function - 1;
    }
    if (first != SIZE_MAX) return &functions[first];
    for (CellProgram *owner = current_program->closure_owner; owner != NULL; owner = owner->closure_owner) {
        size_t found = name_index_find(&owner->function_names, bytes, length);
        if (found != SIZE_MAX) return &owner->functions[found];
    }
    return NULL;
}

static void bind_definition(Form *form) {
    if (form->part_count != DEFN_PART_COUNT) fail_at(form->offset, "a definition needs a name, parameters, and a body");
    Form *name = form->parts[DEFN_NAME];
    Form *parameters = form->parts[DEFN_PARAMETERS];
    if (name->kind != FORM_SYMBOL) fail_at(name->offset, "a definition name must be a symbol");
    const Operation *reserved = find_operation_text(name->text, name->text_length);
    if (reserved != NULL && reserved->evaluation == LEAVE_ARGUMENTS) {
        fail_at(name->offset, message_with_name("a definition uses a reserved operation name: ", name->text, name->text_length));
    }
    if (parameters->kind != FORM_GROUP) fail_at(parameters->offset, "a parameter list must be a group");
    for (size_t index = FIRST_INDEX; index < parameters->part_count; index = checked_add(index, ONE_ELEMENT)) {
        Form *parameter = parameters->parts[index];
        if (parameter->kind != FORM_SYMBOL) fail_at(parameter->offset, "a parameter must be a symbol");
        for (size_t earlier = FIRST_INDEX; earlier < index; earlier = checked_add(earlier, ONE_ELEMENT)) {
            Form *previous = parameters->parts[earlier];
            if (bytes_equal(previous->text, previous->text_length, parameter->text, parameter->text_length)) {
                fail_at(parameter->offset, "a parameter name is repeated");
            }
        }
    }
    Value function = nothing_value();
    function.kind = KIND_FUNCTION;
    function.program = current_program;
    function.body = form->parts[DEFN_BODY];
    function.parameter_count = parameters->part_count;
    function.parameters = heap_allocate(parameters->part_count,sizeof *function.parameters,HEAP_PARAMETERS,parameters->part_count);
    for (size_t index = FIRST_INDEX; index < parameters->part_count; index = checked_add(index, ONE_ELEMENT)) {
        function.parameters[index].bytes = parameters->parts[index]->text;
        function.parameters[index].length = parameters->parts[index]->text_length;
    }
    Binding *existing = find_function_text(name->text, name->text_length);
    if (existing != NULL && existing->value.body != NULL && existing->value.body->path != NULL &&
        form->path != NULL && strcmp(existing->value.body->path, form->path) == 0) {
        existing->value = function;
        existing->name_offset = name->offset;
        form->is_bound = 1;
        return;
    }
    size_t needed = checked_add(function_count, ONE_ELEMENT);
    functions = grow_list(functions, &function_capacity, sizeof *functions, needed);
    functions[function_count].bytes = name->text;
    functions[function_count].length = name->text_length;
    functions[function_count].value = function;
    functions[function_count].name_offset = name->offset;
    functions[function_count].previous_function = name_index_find(&function_names, name->text, name->text_length) + 1;
    name_index_set(&function_names, name->text, name->text_length, function_count);
    function_count = needed;
    ++global_epoch;
    form->is_bound = 1;
}

static void bind_definitions(Form **forms, size_t form_count) {
    Form **pending = NULL;
    size_t pending_count = EMPTY_COUNT;
    size_t pending_capacity = EMPTY_COUNT;
    for (size_t index = form_count; index > FIRST_INDEX; ) {
        index -= ONE_ELEMENT;
        size_t needed = checked_add(pending_count, ONE_ELEMENT);
        pending = grow_list(pending, &pending_capacity, sizeof *pending, needed);
        pending[pending_count] = forms[index];
        pending_count = needed;
    }
    while (pending_count != EMPTY_COUNT) {
        pending_count -= ONE_ELEMENT;
        Form *form = pending[pending_count];
        if (form->kind != FORM_GROUP) continue;
        if (form->part_count != EMPTY_COUNT && form_is_symbol(form->parts[CALL_HEAD], NAME_DEFN)) {
            bind_definition(form);
            continue;
        }
        for (size_t index = form->part_count; index > FIRST_INDEX; ) {
            index -= ONE_ELEMENT;
            size_t needed = checked_add(pending_count, ONE_ELEMENT);
            pending = grow_list(pending, &pending_capacity, sizeof *pending, needed);
            pending[pending_count] = form->parts[index];
            pending_count = needed;
        }
    }
}

static int values_equal(Value left, Value right);

static Value offer_add(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("add expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return integer_value(checked_integer_add(left.integer, right.integer));
    double sum = number_as_real(left) + number_as_real(right);
    return real_value(sum);
}

static Value offer_sub(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("sub expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return integer_value(checked_integer_sub(left.integer, right.integer));
    double difference = number_as_real(left) - number_as_real(right);
    return real_value(difference);
}

static Value offer_eq(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[0], right = arguments[1];
    if (is_number(left) && is_number(right) && left.kind != right.kind) {
        Value integer = left.kind == KIND_INTEGER ? left : right;
        double real = left.kind == KIND_REAL ? left.real : right.real;
        return boolean_word(isfinite(real) && real >= (double)LLONG_MIN && real < -(double)LLONG_MIN &&
            trunc(real) == real && (long long)real == integer.integer);
    }
    return boolean_word(values_equal(arguments[LEFT_OPERAND], arguments[RIGHT_OPERAND]));
}

static Value offer_value_eq(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return boolean_word(values_equal(arguments[0], arguments[1]));
}

static Value offer_ne(Value *arguments, size_t argument_count) {
    return boolean_word(!offer_eq(arguments, argument_count).integer);
}

static Value offer_math_sqrt(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[0])) fail("math_sqrt expects a number");
    return real_value(sqrt(number_as_real(arguments[0])));
}

static Value offer_math_exp(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[0])) fail("math_exp expects a number");
    return real_value(exp(number_as_real(arguments[0])));
}

static Value offer_math_log(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[0])) fail("math_log expects a number");
    return real_value(log(number_as_real(arguments[0])));
}

static Value offer_math_pow(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[0]) || !is_number(arguments[1])) fail("math_pow expects two numbers");
    return real_value(pow(number_as_real(arguments[0]), number_as_real(arguments[1])));
}

static Value offer_lt(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("lt expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return boolean_word(left.integer < right.integer);
    return boolean_word(number_as_real(left) < number_as_real(right));
}

static Value offer_ge(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("ge expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return boolean_word(left.integer >= right.integer);
    return boolean_word(number_as_real(left) >= number_as_real(right));
}

static Value offer_str_concat(Value *arguments, size_t argument_count);
static Value list_from(Value *items, size_t item_count);
static Value count_word(size_t count);

static Value offer_gt(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("gt expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return boolean_word(left.integer > right.integer);
    return boolean_word(number_as_real(left) > number_as_real(right));
}

static Value offer_le(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("le expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return boolean_word(left.integer <= right.integer);
    return boolean_word(number_as_real(left) <= number_as_real(right));
}

static Value offer_nth(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "nth expects an integer index");
    if (arguments[LEFT_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[LEFT_OPERAND].kind == KIND_TEXT) {
        expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "nth expects an integer index");
        if (arguments[RIGHT_OPERAND].integer < WORD_FALSE) return nothing_value();
        size_t index = (size_t)arguments[RIGHT_OPERAND].integer;
        if (index >= arguments[LEFT_OPERAND].text_length) return nothing_value();
        return text_value(arguments[LEFT_OPERAND].text + index, ONE_ELEMENT);
    }
    if (arguments[LEFT_OPERAND].kind != KIND_LIST) fail(message_with_name("nth expects a list, received ", kind_name(arguments[LEFT_OPERAND].kind), strlen(kind_name(arguments[LEFT_OPERAND].kind))));
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "nth expects an integer index");
    if (arguments[RIGHT_OPERAND].integer < WORD_FALSE) return nothing_value();
    size_t index = (size_t)arguments[RIGHT_OPERAND].integer;
    if (index >= arguments[LEFT_OPERAND].item_count) return nothing_value();
    return list_at(arguments[LEFT_OPERAND], index);
}

static Value offer_slice(Value *arguments, size_t argument_count) {
    if (argument_count != ARITY_TWO && argument_count != ARITY_THREE) fail("slice expects a list and a start");
    if (arguments[LEFT_OPERAND].kind == KIND_NOTHING || arguments[RIGHT_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[LEFT_OPERAND].kind != KIND_LIST) fail(message_with_name("slice expects a list, received ", kind_name(arguments[LEFT_OPERAND].kind), strlen(kind_name(arguments[LEFT_OPERAND].kind))));
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "slice expects an integer start");
    Value list = arguments[LEFT_OPERAND];
    size_t end = list.item_count;
    size_t start = EMPTY_COUNT;
    if (arguments[RIGHT_OPERAND].integer > WORD_FALSE) {
        start = (size_t)arguments[RIGHT_OPERAND].integer;
        if (start > end) start = end;
    }
    if (argument_count == ARITY_THREE) {
        expect_kind(arguments[SLICE_END], KIND_INTEGER, "slice expects an integer end");
        if (arguments[SLICE_END].integer <= WORD_FALSE) end = EMPTY_COUNT;
        else {
            size_t requested = (size_t)arguments[SLICE_END].integer;
            if (requested < end) end = requested;
        }
    }
    if (start > end) start = end;
    if (end == list.item_count) {
        List *cell = list.list;
        for (size_t index = FIRST_INDEX; index < start; index = checked_add(index, ONE_ELEMENT)) cell = cell->tail;
        list.list = cell;
        list.item_count = end - start;
        return list;
    }
    size_t count = end - start;
    Value *items = allocate_zeroed(count, sizeof *items);
    List *cell = list.list;
    for (size_t index = FIRST_INDEX; index < start; index = checked_add(index, ONE_ELEMENT)) cell = cell->tail;
    for (size_t index = FIRST_INDEX; index < count; index = checked_add(index, ONE_ELEMENT)) {
        items[index] = cell->head;
        cell = cell->tail;
    }
    Value sliced = list_from(items, count);
    free(items);
    return inherit_owner(sliced,list);
}

static Value offer_plus(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (left.kind == KIND_TEXT && right.kind == KIND_TEXT) return offer_str_concat(arguments, argument_count);
    if (!is_number(left) || !is_number(right)) fail("plus expects two numbers or two texts");
    return offer_add(arguments, argument_count);
}

static Value offer_len(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value value = arguments[SOLE_OPERAND];
    if (value.kind == KIND_LIST) return count_word(value.item_count);
    if (value.kind == KIND_TEXT) return count_word(value.text_length);
    if (value.kind == KIND_NOTHING) fail("nothing has no length");
    fail("len expects a list or text");
    return nothing_value();
}

static Value offer_empty(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    return list_from(NULL, EMPTY_COUNT);
}

static Value integer_text(long long integer) {
    unsigned long long magnitude = integer < WORD_FALSE ? (unsigned long long)(-(integer + WORD_TRUE)) + ONE_ELEMENT : (unsigned long long)integer;
    size_t digits = ONE_ELEMENT;
    unsigned long long scale = magnitude;
    while (scale >= (unsigned long long)DECIMAL_RADIX) {
        scale /= (unsigned long long)DECIMAL_RADIX;
        digits = checked_add(digits, ONE_ELEMENT);
    }
    size_t length = digits;
    if (integer < WORD_FALSE) length = checked_add(length, ONE_ELEMENT);
    char *text = heap_allocate(checked_add(length,TEXT_TERMINATOR_SIZE),1,HEAP_TEXT,length);
    text[length] = TEXT_TERMINATOR;
    size_t cursor = length;
    if (magnitude == 0) text[FIRST_INDEX] = DIGIT_ZERO;
    while (magnitude != 0) {
        cursor -= ONE_ELEMENT;
        text[cursor] = (char)(DIGIT_ZERO + (magnitude % (unsigned long long)DECIMAL_RADIX));
        magnitude /= (unsigned long long)DECIMAL_RADIX;
    }
    if (integer < WORD_FALSE) text[FIRST_INDEX] = SIGN_MINUS;
    Value value = nothing_value();
    value.kind = KIND_TEXT;
    value.text = text;
    value.text_length = length;
    return value;
}

static Value offer_value_str(Value *arguments, size_t argument_count);
static void print_value_to(Value root, FILE *output);

static Value offer_str(Value *arguments, size_t argument_count) {
    return offer_value_str(arguments, argument_count);
}

static Value offer_print_str(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "print_str expects text");
    if (active_cell != NULL && !active_cell->capture) return integer_value(0);
    if (arguments[SOLE_OPERAND].text_length != EMPTY_COUNT) {
        fwrite(arguments[SOLE_OPERAND].text, sizeof(char), arguments[SOLE_OPERAND].text_length, runtime_output());
    }
    fputc(NEWLINE, runtime_output());
    fflush(runtime_output());
    return integer_value(0);
}

static void print_value(Value root);

static Value offer_not(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return boolean_word(!integer_is_nonzero(arguments[SOLE_OPERAND]));
}

static Value offer_mul(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("mul expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return integer_value(checked_integer_mul(left.integer, right.integer));
    double product = number_as_real(left) * number_as_real(right);
    return real_value(product);
}

static Value offer_div(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("div expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return integer_value(checked_integer_div(left.integer, right.integer));
    double divisor = number_as_real(right);
    double quotient = number_as_real(left) / divisor;
    return real_value(quotient);
}

static Value offer_mod(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("mod expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return integer_value(checked_integer_mod(left.integer, right.integer));
    double divisor = number_as_real(right);
    double remainder = fmod(number_as_real(left), divisor);
    return real_value(remainder);
}

static Value offer_head(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[SOLE_OPERAND].kind == KIND_TEXT) {
        if (arguments[SOLE_OPERAND].text_length == EMPTY_COUNT) return nothing_value();
        return text_value(arguments[SOLE_OPERAND].text, ONE_ELEMENT);
    }
    if (arguments[SOLE_OPERAND].kind != KIND_LIST) fail(message_with_name("head expects a list, received ", kind_name(arguments[SOLE_OPERAND].kind), strlen(kind_name(arguments[SOLE_OPERAND].kind))));
    if (arguments[SOLE_OPERAND].item_count == EMPTY_COUNT) return nothing_value();
    return inherit_owner(arguments[SOLE_OPERAND].list->head,arguments[SOLE_OPERAND]);
}

static Value offer_tail(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[SOLE_OPERAND].kind == KIND_TEXT) {
        if (arguments[SOLE_OPERAND].text_length == EMPTY_COUNT) return text_value("", EMPTY_COUNT);
        return text_value(arguments[SOLE_OPERAND].text + ONE_ELEMENT, arguments[SOLE_OPERAND].text_length - ONE_ELEMENT);
    }
    if (arguments[SOLE_OPERAND].kind != KIND_LIST) fail(message_with_name("tail expects a list, received ", kind_name(arguments[SOLE_OPERAND].kind), strlen(kind_name(arguments[SOLE_OPERAND].kind))));
    if (arguments[SOLE_OPERAND].item_count == EMPTY_COUNT) return list_from(NULL, EMPTY_COUNT);
    Value tail = arguments[SOLE_OPERAND];
    if (tail.list->dotted_tail != NULL) return inherit_owner(*tail.list->dotted_tail,tail);
    tail.list = tail.list->tail;
    tail.item_count -= ONE_ELEMENT;
    return tail;
}

static Value offer_cons(Value *arguments, size_t argument_count) {
    if (arguments[1].kind == KIND_NOTHING) fail("cons onto nothing has no list");
    (void)argument_count;
    if (arguments[RIGHT_OPERAND].kind != KIND_LIST) {
        Value pair = list_from(arguments, 1);
        pair.list->dotted_tail = allocate_zeroed(1, sizeof(Value));
        *pair.list->dotted_tail = arguments[RIGHT_OPERAND];
        return pair;
    }
    List *cell = heap_allocate(ONE_ELEMENT,sizeof *cell,HEAP_LIST,1);
    cell->head = arguments[LEFT_OPERAND];
    cell->tail = arguments[RIGHT_OPERAND].list;
    if (cell->tail != NULL && cell->tail->flat != NULL && cell->tail->flat_count == arguments[RIGHT_OPERAND].item_count) {
        cell->indexed = cell->tail;
        cell->lead = ONE_ELEMENT;
    } else if (cell->tail != NULL && cell->tail->indexed != NULL) {
        cell->indexed = cell->tail->indexed;
        cell->lead = checked_add(cell->tail->lead, ONE_ELEMENT);
    }
    Value list = nothing_value();
    list.kind = KIND_LIST;
    list.list = cell;
    list.item_count = checked_add(arguments[RIGHT_OPERAND].item_count, ONE_ELEMENT);
    list.owner = arguments[RIGHT_OPERAND].owner;
    return list;
}

static void record_index_text(Record *record) {
    if (record->text_indexed == record->count) return;
    if (record->text_capacity == 0 || record->count > record->text_capacity / 2) {
        size_t capacity = record->text_capacity ? record->text_capacity : 8;
        while (record->count > capacity / 2) capacity = checked_add(capacity, capacity);
        size_t *slots = allocate_zeroed(capacity, sizeof *record->text_slots);
        free(record->text_slots);
        record->text_slots = slots;
        record->text_capacity = capacity;
        record->text_indexed = 0;
    }
    for (; record->text_indexed < record->count; ++record->text_indexed) {
        Value key = record->keys[record->text_indexed];
        if (key.kind != KIND_TEXT) continue;
        size_t slot = hash_bytes(VALUE_HASH_OFFSET, key.text, key.text_length) & (record->text_capacity - 1);
        while (record->text_slots[slot]) slot = (slot + 1) & (record->text_capacity - 1);
        record->text_slots[slot] = record->text_indexed + 1;
    }
}

static int record_find(Record *record, Value key, size_t *index) {
    // Immutable text keys admit a physical index. Other values retain their
    // exact equality semantics, including structural keys and signed zero.
    if (key.kind == KIND_TEXT) {
        record_index_text(record);
        if (record->text_capacity == 0) return 0;
        size_t slot = hash_bytes(VALUE_HASH_OFFSET, key.text, key.text_length) & (record->text_capacity - 1);
        while (record->text_slots[slot]) {
            size_t found = record->text_slots[slot] - 1;
            if (values_equal(record->keys[found], key)) { *index = found; return 1; }
            slot = (slot + 1) & (record->text_capacity - 1);
        }
        return 0;
    }
    for (size_t cursor = FIRST_INDEX; cursor < record->count; cursor = checked_add(cursor, ONE_ELEMENT)) {
        if (!values_equal(record->keys[cursor], key)) continue;
        *index = cursor;
        return 1;
    }
    return 0;
}

static Value record_value(Record *record) {
    heap_track(record, HEAP_RECORD, 1);
    Value value = nothing_value();
    value.kind = KIND_RECORD;
    value.record = record;
    return value;
}

static Value offer_record_set(Value *arguments, size_t argument_count);

static Value offer_record_keys(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_RECORD, "record_keys expects a record");
    Record *record = arguments[0].record;
    return inherit_owner(list_from(record->keys, record->count),arguments[0]);
}

static Value offer_record_new(Value *arguments, size_t argument_count) {
    if (argument_count == EMPTY_COUNT) fail("record_new expects a blueprint or a size");
    size_t fields = argument_count - ONE_ELEMENT;
    if ((fields % ARITY_TWO) != EMPTY_COUNT) fail("record_new expects key and value pairs");
    Value created = record_value(heap_allocate(ONE_ELEMENT,sizeof(Record),HEAP_RECORD,1));
    created.record->blueprint = argument_count == 1 && arguments[0].kind == KIND_INTEGER ? nothing_value() : arguments[0];
    for (size_t index = ONE_ELEMENT; index < argument_count; index = checked_add(index, ARITY_TWO)) {
        Value triple[] = {created, arguments[index], arguments[checked_add(index, ONE_ELEMENT)]};
        created = offer_record_set(triple, ARITY_THREE);
    }
    return created;
}

static Value offer_record_has(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_RECORD, "record_has expects a record");
    size_t index = FIRST_INDEX;
    return boolean_word(record_find(arguments[LEFT_OPERAND].record, arguments[RIGHT_OPERAND], &index));
}

static Value offer_record_get(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_RECORD, "record_get expects a record");
    size_t index = FIRST_INDEX;
    if (!record_find(arguments[LEFT_OPERAND].record, arguments[RIGHT_OPERAND], &index)) return nothing_value();
    return inherit_owner(arguments[LEFT_OPERAND].record->values[index],arguments[LEFT_OPERAND]);
}

static Value offer_record_set(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_RECORD, "record_set expects a record");
    Record *record = arguments[LEFT_OPERAND].record;
    Value key = arguments[RIGHT_OPERAND];
    Value stored = arguments[SLICE_END];
    size_t index = FIRST_INDEX;
    if (record_find(record, key, &index)) {
        record->values[index] = stored;
        return arguments[LEFT_OPERAND];
    }
    size_t needed = checked_add(record->count, ONE_ELEMENT);
    size_t key_capacity = record->capacity;
    size_t value_capacity = record->capacity;
    record->keys = grow_list(record->keys, &key_capacity, sizeof *record->keys, needed);
    record->values = grow_list(record->values, &value_capacity, sizeof *record->values, needed);
    record->capacity = key_capacity;
    record->keys[record->count] = key;
    record->values[record->count] = stored;
    record->count = needed;
    return arguments[LEFT_OPERAND];
}

static ssize_t find_method(Value blueprint, Value name) {
    if (name.kind != KIND_TEXT) return -1;
    for (size_t index = FIRST_INDEX; index < method_definition_count; index = checked_add(index, ONE_ELEMENT)) {
        MethodDefinition *method = method_definitions + index;
        if (!values_equal(method->blueprint, blueprint)) continue;
        if (!bytes_equal(method->name, method->name_length, name.text, name.text_length)) continue;
        return (ssize_t)index;
    }
    return -1;
}

static Value receiver_blueprint(Value receiver) {
    if (receiver.kind == KIND_RECORD) return inherit_owner(receiver.record->blueprint,receiver);
    return receiver;
}

static Value offer_method_define(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[RIGHT_OPERAND], KIND_TEXT, "method_define expects a method name");
    expect_kind(arguments[SLICE_END], KIND_FUNCTION, "method_define expects a function value");
    Value blueprint = arguments[LEFT_OPERAND];
    Value name = arguments[RIGHT_OPERAND];
    ssize_t existing = find_method(blueprint, name);
    if (existing >= 0) {
        method_definitions[existing].function = arguments[SLICE_END];
        return blueprint;
    }
    size_t needed = checked_add(method_definition_count, ONE_ELEMENT);
    method_definitions = grow_list(method_definitions, &method_definition_capacity, sizeof *method_definitions, needed);
    MethodDefinition *entry = method_definitions + method_definition_count;
    entry->blueprint = blueprint;
    entry->name = copy_bytes(name.text, name.text_length);
    entry->name_length = name.text_length;
    entry->function = arguments[SLICE_END];
    method_definition_count = needed;
    return blueprint;
}

static Value offer_method_has(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value blueprint = receiver_blueprint(arguments[LEFT_OPERAND]);
    return boolean_word(find_method(blueprint, arguments[RIGHT_OPERAND]) >= 0);
}

static Value offer_record_type(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return boolean_word(arguments[0].kind == KIND_RECORD);
}

static Value offer_record_blueprint(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_RECORD, "record_blueprint expects a record");
    return inherit_owner(arguments[0].record->blueprint,arguments[0]);
}

static Value offer_range(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_INTEGER, "range expects an integer start");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "range expects an integer end");
    long long start = arguments[LEFT_OPERAND].integer;
    long long end = arguments[RIGHT_OPERAND].integer;
    if (end <= start) return list_from(NULL, EMPTY_COUNT);
    long long span = checked_integer_sub(end, start);
    Value *items = allocate_zeroed((size_t)span, sizeof *items);
    for (long long step = WORD_FALSE; step < span; step = checked_integer_add(step, WORD_TRUE)) {
        items[step] = integer_value(checked_integer_add(start, step));
    }
    Value result = list_from(items, (size_t)span); free(items); return result;
}

static Value offer_byte_to_str(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[SOLE_OPERAND].kind != KIND_INTEGER) {
        fail(message_with_name("byte_to_str expects an integer, received ", kind_name(arguments[SOLE_OPERAND].kind), strlen(kind_name(arguments[SOLE_OPERAND].kind))));
    }
    long long byte = arguments[SOLE_OPERAND].integer;
    if (byte < WORD_FALSE || byte > UCHAR_MAX) fail("byte_to_str expects a byte");
    char text = (char)byte;
    return text_value(&text, ONE_ELEMENT);
}

static int scan_continues(unsigned char byte, long long mode) {
    if (mode == SCAN_SPACE) return byte == ' ' || byte == '\t' || byte == '\n' || byte == '\r';
    if (mode == SCAN_DIGITS) return byte >= '0' && byte <= '9';
    if (mode == SCAN_ALPHA) return (byte >= 'A' && byte <= 'Z') || (byte >= 'a' && byte <= 'z');
    if (mode == SCAN_NAME) return (byte >= 'A' && byte <= 'Z') || (byte >= 'a' && byte <= 'z') || (byte >= '0' && byte <= '9') || byte == '_' || byte == '-';
    if (mode == SCAN_STRING) return byte != '"' && byte != '\\';
    if (mode == SCAN_LINE) return byte != '\n';
    if (mode == SCAN_JSON_SAFE) return byte >= ' ' && byte != '"' && byte != '\\';
    return 0;
}

static Value offer_scan_run(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "scan_run expects a start");
    expect_kind(arguments[SLICE_END], KIND_INTEGER, "scan_run expects a mode");
    size_t index = arguments[RIGHT_OPERAND].integer < 0 ? 0 : (size_t)arguments[RIGHT_OPERAND].integer;
    if (arguments[SOLE_OPERAND].kind != KIND_TEXT) return count_word(index);
    size_t length = arguments[SOLE_OPERAND].text_length;
    const char *text = arguments[SOLE_OPERAND].text;
    long long mode = arguments[SLICE_END].integer;
    while (index < length && scan_continues((unsigned char)text[index], mode)) index = checked_add(index, ONE_ELEMENT);
    return count_word(index);
}

static size_t clamp_text_index(Value bound, size_t length, size_t when_absent) {
    if (bound.kind == KIND_NOTHING) return when_absent;
    expect_kind(bound, KIND_INTEGER, "substring expects an index");
    if (bound.integer <= WORD_FALSE) return FIRST_INDEX;
    if ((unsigned long long)bound.integer >= length) return length;
    return (size_t)bound.integer;
}

static Value offer_substring(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "substring expects an integer start");
    expect_kind(arguments[SLICE_END], KIND_INTEGER, "substring expects an integer end");
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[SOLE_OPERAND].kind != KIND_TEXT) return text_value("", 0);
    size_t length = arguments[SOLE_OPERAND].text_length;
    size_t start = clamp_text_index(arguments[RIGHT_OPERAND], length, FIRST_INDEX);
    size_t end = clamp_text_index(arguments[SLICE_END], length, length);
    if (end < start) return text_value("", EMPTY_COUNT);
    return text_value(arguments[SOLE_OPERAND].text + start, end - start);
}

static Value offer_char_at(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[LEFT_OPERAND].kind == KIND_NOTHING || arguments[RIGHT_OPERAND].kind == KIND_NOTHING) return nothing_value();
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "char_at expects text");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "char_at expects an index");
    if (arguments[RIGHT_OPERAND].integer < WORD_FALSE) return nothing_value();
    size_t index = (size_t)arguments[RIGHT_OPERAND].integer;
    if (index >= arguments[LEFT_OPERAND].text_length) return nothing_value();
    return text_value(arguments[LEFT_OPERAND].text + index, ONE_ELEMENT);
}

static Value offer_ord(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "ord expects text");
    if (arguments[SOLE_OPERAND].text_length == EMPTY_COUNT) return nothing_value();
    return integer_value((long long)(unsigned char)arguments[SOLE_OPERAND].text[FIRST_INDEX]);
}

static Value offer_str_find(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "str_find expects text");
    expect_kind(arguments[RIGHT_OPERAND], KIND_TEXT, "str_find expects a needle");
    expect_kind(arguments[SLICE_END], KIND_INTEGER, "str_find expects a start");
    long long requested = arguments[SLICE_END].integer < WORD_FALSE ? WORD_FALSE : arguments[SLICE_END].integer;
    size_t start = (size_t)requested;
    size_t haystack = arguments[SOLE_OPERAND].text_length;
    size_t needle = arguments[RIGHT_OPERAND].text_length;
    if (start > haystack || needle > haystack - start) return integer_value(-WORD_TRUE);
    for (size_t index = start; index <= haystack - needle; index = checked_add(index, ONE_ELEMENT)) {
        if (bytes_equal(arguments[SOLE_OPERAND].text + index, needle, arguments[RIGHT_OPERAND].text, needle)) return count_word(index);
        if (index == haystack - needle) break;
    }
    return integer_value(-WORD_TRUE);
}

static Value offer_str_to_int(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "str_to_int expects text");
    const char *text = arguments[SOLE_OPERAND].text;
    size_t length = arguments[SOLE_OPERAND].text_length;
    size_t index = FIRST_INDEX;
    if (length != EMPTY_COUNT && text[FIRST_INDEX] == '-') {
        index = ONE_ELEMENT;
    }
    if (index >= length) return nothing_value();
    for (; index < length; index = checked_add(index, ONE_ELEMENT)) {
        unsigned char byte = (unsigned char)text[index];
        if (byte < '0' || byte > '9') return nothing_value();
    }
    return integer_value(decimal_integer(text, length));
}

static Value offer_str_len(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "str_len expects text");
    return count_word(arguments[SOLE_OPERAND].text_length);
}

static Value offer_str_byte_at(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "str_byte_at expects text");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "str_byte_at expects an integer index");
    if (arguments[RIGHT_OPERAND].integer < WORD_FALSE) return integer_value(-1);
    size_t index = (size_t)arguments[RIGHT_OPERAND].integer;
    if (index >= arguments[LEFT_OPERAND].text_length) return integer_value(-1);
    return integer_value((unsigned char)arguments[LEFT_OPERAND].text[index]);
}

static Value offer_value_str(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value value = arguments[SOLE_OPERAND];
    if (value.kind == KIND_TEXT) return value;
    if (value.kind == KIND_INTEGER) return integer_text(value.integer);
    if (value.kind == KIND_NOTHING) return text_value("", 0);
    char *bytes = NULL;
    size_t length = 0;
    FILE *output = open_memstream(&bytes, &length);
    if (output == NULL) return nothing_value();
    print_value_to(value, output);
    if (fclose(output) != 0) { free(bytes); return nothing_value(); }
    Value text = text_value(bytes, length);
    free(bytes);
    return text;
}

static int terminal_input_request(Value request) {
    Value door = list_at(request, FIRST_INDEX);
    return request.kind == KIND_LIST && request.item_count >= ARITY_TWO && door.kind == KIND_TEXT
        && bytes_equal_cstr(door.text, door.text_length, TERMINAL_INPUT_DOOR);
}

static Value offer_terminal_input(Value request) {
    Value operation = list_at(request, ONE_ELEMENT);
    expect_kind(operation, KIND_INTEGER, "terminal input expects an operation");
    if (operation.integer == TERMINAL_INPUT_READ) return nothing_value();
    return integer_value(WORD_FALSE);
}

static long long sleep_milliseconds(Value request) {
    if (request.kind == KIND_INTEGER) return request.integer;
    Value duration = list_at(request, FIRST_INDEX);
    if (request.kind == KIND_LIST && request.item_count != EMPTY_COUNT && duration.kind == KIND_INTEGER) return duration.integer;
    fail("host_sleep_ms expects an integer duration");
    return WORD_FALSE;
}

static Value deadline_rest(void) {
    Value *fields = allocate_zeroed(SLEEP_REST_FIELDS, sizeof *fields);
    fields[FIRST_INDEX] = integer_value(WORD_FALSE);
    fields[SLEEP_WOKE] = integer_value(-WORD_TRUE);
    fields[ARITY_TWO] = integer_value(WORD_FALSE);
    fields[ARITY_THREE] = list_from(NULL, EMPTY_COUNT);
    Value result = list_from(fields, SLEEP_REST_FIELDS); free(fields); return result;
}

static uint64_t monotonic_ns(void);
static void cell_stop(const char *kind, const char *message);
static Value offer_host_sleep_ms(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value request = arguments[SOLE_OPERAND];
    if (terminal_input_request(request)) return offer_terminal_input(request);
    long long milliseconds = sleep_milliseconds(request);
    if (milliseconds < WORD_FALSE) fail("host_sleep_ms expects a nonnegative duration");
    uint64_t until = monotonic_ns() / 1000000 + (uint64_t)milliseconds;
    if (active_cell != NULL && active_cell->deadline != 0 && active_cell->deadline < until) until = active_cell->deadline;
    /* Relative poll uses the kernel wait directly, including after a native
     * fork; nanosleep on Darwin may retain a parent's invalid clock port. */
    for (;;) {
        uint64_t now = monotonic_ns() / 1000000;
        if (now >= until) break;
        uint64_t left = until - now;
        if (poll(NULL, 0, left > INT_MAX ? INT_MAX : (int)left) < 0 && errno != EINTR) fail("host_sleep_ms could not wait");
    }
    if (active_cell != NULL && active_cell->deadline != 0 && monotonic_ns() / 1000000 >= active_cell->deadline) cell_stop("deadline", "cell execution deadline reached while waiting");
    if (request.kind == KIND_LIST) return deadline_rest();
    return integer_value(milliseconds);
}

static uint64_t monotonic_ns(void);

static Value offer_host_monotonic_ms(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    return integer_value((long long)(monotonic_ns() / NANOSECONDS_PER_MILLISECOND));
}

static Value offer_now_unix_ms(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    struct timespec now;
    if (clock_gettime(CLOCK_REALTIME, &now) != SYSTEM_CALL_OK) fail("now_unix_ms could not read the clock");
    long long milliseconds = (long long)now.tv_sec * MILLISECONDS_PER_SECOND + (long long)now.tv_nsec / NANOSECONDS_PER_MILLISECOND;
    return integer_value(milliseconds);
}

static Value offer_host_getenv(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind != KIND_TEXT || arguments[SOLE_OPERAND].text_length==0 || memchr(arguments[SOLE_OPERAND].text,0,arguments[SOLE_OPERAND].text_length)!=NULL || memchr(arguments[SOLE_OPERAND].text,'=',arguments[SOLE_OPERAND].text_length)!=NULL) return integer_value(-2);
    const char *value = getenv(arguments[SOLE_OPERAND].text);
    if (value == NULL) return nothing_value();
    return text_value(value, strlen(value));
}

static pid_t integer_pid(Value value, const char *reason) {
    (void)reason;
    if (value.kind != KIND_INTEGER || value.integer <= 0 || value.integer > INT_MAX) return 0;
    return (pid_t)value.integer;
}

#ifdef __APPLE__
static int process_table_row(pid_t pid, struct kinfo_proc *row) {
    int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, pid};
    size_t length = sizeof *row;
    return sysctl(mib, 4, row, &length, NULL, 0) == 0 && length == sizeof *row;
}
#endif

static Value offer_host_alive(Value *arguments, size_t argument_count) {
    (void)argument_count;
    pid_t pid = integer_pid(arguments[SOLE_OPERAND], "host_alive expects a process");
    if (pid == 0) return integer_value(-1);
#ifdef __APPLE__
    struct kinfo_proc row;
    if (process_table_row(pid, &row) && row.kp_proc.p_stat == SZOMB) return integer_value(0);
#endif
    if (kill(pid,0)==SYSTEM_CALL_OK || errno==EPERM) return integer_value(1);
    return errno==ESRCH ? integer_value(0) : nothing_value();
}

static int process_proven_ended(Value pid) {
    Value observed=offer_host_alive(&pid,1);
    return observed.kind==KIND_INTEGER && observed.integer==0;
}

static Value offer_host_kill(Value *arguments, size_t argument_count) {
    (void)argument_count;
    pid_t pid = integer_pid(arguments[SOLE_OPERAND], "host_kill expects a process");
    if (pid == 0) return integer_value(-1);
    if (kill(pid, SIGTERM) == SYSTEM_CALL_OK) return integer_value(WORD_FALSE);
    return integer_value(-WORD_TRUE);
}

static Value offer_host_signal(Value *arguments, size_t argument_count) {
    (void)argument_count;
    pid_t pid = integer_pid(arguments[0], "host_signal expects a process");
    if (pid == 0 || arguments[1].kind != KIND_INTEGER || arguments[1].integer < 1 || arguments[1].integer > 31) return integer_value(-2);
    return integer_value(kill(pid, (int)arguments[1].integer) == 0 ? 0 : -1);
}

static long long monotonic_milliseconds(void);

static Value process_exit_value(int status) {
    return integer_value(WIFEXITED(status) ? WEXITSTATUS(status) : WIFSIGNALED(status) ? 128 + WTERMSIG(status) : -1);
}

static Value offer_host_wait(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value request = arguments[0];
    int timed = request.kind == KIND_LIST;
    Value duration = timed ? list_at(request, 1) : nothing_value();
    if (timed && (request.item_count != 2 || duration.kind != KIND_INTEGER)) return integer_value(-1);
    pid_t pid = integer_pid(timed ? list_at(request, 0) : request, "host_wait expects a process");
    if (pid == 0) return integer_value(-1);
    int status = 0;
    pid_t waited;
    do { waited = waitpid(pid, &status, timed ? WNOHANG : 0); } while (waited < 0 && errno == EINTR);
    if (waited == pid) return process_exit_value(status);
    if (waited < 0) return integer_value(-1);
    if (duration.integer <= 0) return nothing_value();
    long long start = monotonic_milliseconds();
    long long end = duration.integer > LLONG_MAX - start ? LLONG_MAX : start + duration.integer;
    int event_fd;
#ifdef __APPLE__
    event_fd = kqueue();
    if (event_fd < 0) return integer_value(-2);
    struct kevent event;
    EV_SET(&event, (uintptr_t)pid, EVFILT_PROC, EV_ADD | EV_ENABLE | EV_ONESHOT, NOTE_EXIT, 0, NULL);
    if (kevent(event_fd, &event, 1, NULL, 0, NULL) < 0) {
        close(event_fd);
        do { waited = waitpid(pid, &status, WNOHANG); } while (waited < 0 && errno == EINTR);
        return waited == pid ? process_exit_value(status) : integer_value(waited < 0 ? -1 : -2);
    }
#elif defined(__linux__) && defined(SYS_pidfd_open)
    event_fd = (int)syscall(SYS_pidfd_open, pid, 0);
    if (event_fd < 0) return integer_value(errno == ESRCH ? -1 : -2);
#else
    return integer_value(-2);
#endif
#if defined(__APPLE__) || (defined(__linux__) && defined(SYS_pidfd_open))
    Value result = nothing_value();
    for (;;) {
        do { waited = waitpid(pid, &status, WNOHANG); } while (waited < 0 && errno == EINTR);
        if (waited != 0) { result = waited == pid ? process_exit_value(status) : integer_value(-1); break; }
        long long remaining = end - monotonic_milliseconds();
        if (remaining <= 0) break;
#ifdef __APPLE__
        struct timespec timeout = {(time_t)(remaining / 1000), (long)(remaining % 1000) * 1000000};
        int ready = kevent(event_fd, NULL, 0, &event, 1, &timeout);
#else
        struct pollfd event = {event_fd, POLLIN, 0};
        int ready = poll(&event, 1, remaining > INT_MAX ? INT_MAX : (int)remaining);
#endif
        if (ready < 0 && errno != EINTR) { result = integer_value(-2); break; }
    }
    close(event_fd);
    return result;
#endif
}

static Value terminal_extent(int axis) {
    struct winsize window;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window) != SYSTEM_CALL_OK) return nothing_value();
    unsigned short cells = axis == TERMINAL_COLUMNS ? window.ws_col : window.ws_row;
    if (cells == 0) return nothing_value();
    return integer_value((long long)cells);
}

static unsigned unsigned_word(Value value, const char *reason) {
    expect_kind(value, KIND_INTEGER, reason);
    return (unsigned)((unsigned long long)value.integer & U32_MASK);
}

static Value offer_add_u32(Value *arguments, size_t argument_count) {
    (void)argument_count;
    unsigned sum = unsigned_word(arguments[LEFT_OPERAND], "add_u32 expects an integer") + unsigned_word(arguments[RIGHT_OPERAND], "add_u32 expects an integer");
    return integer_value((long long)sum);
}

static Value offer_band(Value *arguments, size_t argument_count) {
    (void)argument_count;
    unsigned bits = unsigned_word(arguments[LEFT_OPERAND], "band expects an integer") & unsigned_word(arguments[RIGHT_OPERAND], "band expects an integer");
    return integer_value((long long)bits);
}

static Value offer_shr_u32(Value *arguments, size_t argument_count) {
    (void)argument_count;
    unsigned bits = unsigned_word(arguments[LEFT_OPERAND], "shr_u32 expects an integer");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "shr_u32 expects a shift");
    if (arguments[RIGHT_OPERAND].integer < WORD_FALSE) fail("shr_u32 expects a non-negative shift");
    if ((unsigned long long)arguments[RIGHT_OPERAND].integer >= U32_BITS) return integer_value(WORD_FALSE);
    return integer_value((long long)(bits >> (unsigned)arguments[RIGHT_OPERAND].integer));
}

static Value offer_bor(Value *arguments, size_t argument_count) {
    (void)argument_count;
    unsigned bits = unsigned_word(arguments[LEFT_OPERAND], "bor expects an integer") | unsigned_word(arguments[RIGHT_OPERAND], "bor expects an integer");
    return integer_value((long long)bits);
}

static Value offer_bxor(Value *arguments, size_t argument_count) {
    (void)argument_count;
    unsigned bits = unsigned_word(arguments[LEFT_OPERAND], "bxor expects an integer") ^ unsigned_word(arguments[RIGHT_OPERAND], "bxor expects an integer");
    return integer_value((long long)bits);
}

static Value offer_bnot_u32(Value *arguments, size_t argument_count) {
    (void)argument_count;
    unsigned bits = ~unsigned_word(arguments[SOLE_OPERAND], "bnot_u32 expects an integer");
    return integer_value((long long)bits);
}

static Value offer_shl_u32(Value *arguments, size_t argument_count) {
    (void)argument_count;
    unsigned bits = unsigned_word(arguments[LEFT_OPERAND], "shl_u32 expects an integer");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "shl_u32 expects a shift");
    if (arguments[RIGHT_OPERAND].integer < WORD_FALSE) fail("shl_u32 expects a non-negative shift");
    if ((unsigned long long)arguments[RIGHT_OPERAND].integer >= U32_BITS) return integer_value(WORD_FALSE);
    return integer_value((long long)(bits << (unsigned)arguments[RIGHT_OPERAND].integer));
}

static Value offer_rotr_u32(Value *arguments, size_t argument_count) {
    (void)argument_count;
    unsigned bits = unsigned_word(arguments[LEFT_OPERAND], "rotr_u32 expects an integer");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "rotr_u32 expects a rotation");
    if (arguments[RIGHT_OPERAND].integer < WORD_FALSE) fail("rotr_u32 expects a non-negative rotation");
    unsigned count = (unsigned)(arguments[RIGHT_OPERAND].integer % (long long)U32_BITS);
    unsigned turned = count == 0 ? bits : (bits >> count) | (bits << (U32_BITS - count));
    return integer_value((long long)turned);
}

typedef struct GiftMapping {
    char *name;
    void *address;
    size_t bytes;
    int descriptor;
    int held;
    int writable;
    long long allowance;
    int lifetime;
    size_t write_limit;
    void *extra_address;
    size_t extra_bytes;
    int release_error;
} GiftMapping;

static GiftMapping *gift_mappings;
static size_t gift_mapping_count;
static size_t gift_mapping_capacity;
static int gift_map_error;
static long long process_born_ms;
static uint64_t live_sequence;

typedef struct { long long generation; Record *owner; Value root; } NativeGiftView;
static NativeGiftView *native_gift_views;
static size_t native_gift_view_count, native_gift_view_capacity;

typedef struct RosterMapping {
    void *address;
    size_t bytes;
    int descriptor;
    int open;
} RosterMapping;

static RosterMapping gift_roster;

static long long monotonic_milliseconds(void) {
    return (long long)(monotonic_ns() / NANOSECONDS_PER_MILLISECOND);
}

static long long unix_milliseconds(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_REALTIME, &now) != SYSTEM_CALL_OK) fail("the clock could not be read");
    return (long long)now.tv_sec * MILLISECONDS_PER_SECOND + (long long)now.tv_nsec / NANOSECONDS_PER_MILLISECOND;
}

static uint64_t shared_load(void *word) {
    return __atomic_load_n((uint64_t *)word, __ATOMIC_ACQUIRE);
}

static void shared_store(void *word, uint64_t value) {
    __atomic_store_n((uint64_t *)word, value, __ATOMIC_RELEASE);
}

static int gift_budget_spent(long long started) {
    return monotonic_milliseconds() - started >= GIFT_READ_BUDGET_MS;
}

static size_t host_page_bytes(void) {
    long page = sysconf(_SC_PAGESIZE);
    if (page <= SYSTEM_CALL_OK) fail("the host page is unavailable");
    return (size_t)page;
}

static void gift_reserve(void) {
    if (gift_mapping_count == gift_mapping_capacity) {
        size_t capacity = gift_mapping_capacity == EMPTY_COUNT ? FIRST_RESERVATION : gift_mapping_capacity;
        if (gift_mapping_capacity != EMPTY_COUNT && capacity > SIZE_MAX / GROWTH_FACTOR) fail("a gift mapping exceeds addressable memory");
        if (gift_mapping_capacity != EMPTY_COUNT) capacity *= GROWTH_FACTOR;
        GiftMapping *grown = allocate_zeroed(capacity, sizeof *grown);
        if (gift_mapping_count != EMPTY_COUNT) memcpy(grown, gift_mappings, gift_mapping_count * sizeof *grown);
        free(gift_mappings);
        gift_mappings = grown;
        gift_mapping_capacity = capacity;
    }
}

static size_t gift_hold(char *name, void *address, size_t bytes, int descriptor, int writable, size_t write_limit) {
    gift_reserve();
    gift_mappings[gift_mapping_count].name = name;
    gift_mappings[gift_mapping_count].address = address;
    gift_mappings[gift_mapping_count].bytes = bytes;
    gift_mappings[gift_mapping_count].descriptor = descriptor;
    gift_mappings[gift_mapping_count].held = 1;
    gift_mappings[gift_mapping_count].writable = writable;
    gift_mappings[gift_mapping_count].allowance = (long long)(bytes - GIFT_HEADER_BYTES);
    gift_mappings[gift_mapping_count].lifetime = 0;
    gift_mappings[gift_mapping_count].write_limit = write_limit;
    size_t handle = gift_mapping_count;
    gift_mapping_count = checked_add(gift_mapping_count, ONE_ELEMENT);
    return handle;
}

static GiftMapping *gift_mapping(Value handle) {
    if (handle.kind != KIND_INTEGER || handle.integer < WORD_FALSE) return NULL;
    if ((unsigned long long)handle.integer >= gift_mapping_count) return NULL;
    GiftMapping *mapping = &gift_mappings[(size_t)handle.integer];
    if (!mapping->held || mapping->address==NULL) return NULL;
    return mapping;
}

static int gift_refresh(GiftMapping *mapping) {
    if (mapping==NULL || mapping->extra_address!=NULL) return 0;
    struct stat state;
    if (fstat(mapping->descriptor,&state)!=0 || state.st_size<0 || (uint64_t)state.st_size>SIZE_MAX || (size_t)state.st_size<mapping->bytes) return 0;
    size_t bytes=(size_t)state.st_size;
    if (bytes==mapping->bytes) return 1;
    void *grown=mmap(NULL,bytes,mapping->writable ? PROT_READ|PROT_WRITE : PROT_READ,MAP_SHARED,mapping->descriptor,0);
    if (grown==MAP_FAILED) return 0;
    if (munmap(mapping->address,mapping->bytes)!=0) {
        mapping->release_error=errno;
        if (munmap(grown,bytes)!=0) { mapping->extra_address=grown; mapping->extra_bytes=bytes; }
        return 0;
    }
    mapping->address=grown; mapping->bytes=bytes; return 1;
}

static Value gift_map(char *name, int create, size_t extent) {
    gift_map_error=0;
    int descriptor = shm_open(name, create==1 ? O_RDWR | O_CREAT : create==2 ? O_RDWR : O_RDONLY, GIFT_OPEN_MODE);
    if (descriptor < SYSTEM_CALL_OK) {
        gift_map_error=errno;
        free(name);
        return nothing_value();
    }
    struct stat info;
    if (fstat(descriptor, &info) != SYSTEM_CALL_OK) {
        gift_map_error=errno;
        close(descriptor);
        free(name);
        return nothing_value();
    }
    size_t bytes = (size_t)info.st_size;
    size_t needed = checked_add(GIFT_HEADER_BYTES, extent);
    if (create==1 && bytes < needed) {
        // Grow the same object. Replacing its name would strand existing
        // receivers on a permanently stale mapping.
        if (ftruncate(descriptor, (off_t)needed) != SYSTEM_CALL_OK) {
            gift_map_error=errno;
            close(descriptor);
            free(name);
            return nothing_value();
        }
        bytes = needed;
    }
    if (bytes < GIFT_HEADER_BYTES) {
        gift_map_error=EINVAL;
        close(descriptor);
        free(name);
        return nothing_value();
    }
    void *address = mmap(NULL, bytes, create ? PROT_READ | PROT_WRITE : PROT_READ, MAP_SHARED, descriptor, 0);
    if (address == MAP_FAILED) {
        gift_map_error=errno;
        close(descriptor);
        free(name);
        return nothing_value();
    }
    return integer_value((long long)gift_hold(name, address, bytes, descriptor, create!=0, create==2 ? bytes-GIFT_HEADER_BYTES : extent));
}

static long long gift_extent(Value request) {
    if (request.kind == KIND_INTEGER) return request.integer;
    Value extent = list_at(request, FIRST_INDEX);
    if (request.kind == KIND_LIST && request.item_count != EMPTY_COUNT && extent.kind == KIND_INTEGER) return extent.integer;
    fail("shm_offer expects a byte extent");
    return WORD_FALSE;
}

static Value offer_shm_offer(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "shm_offer expects a name");
    if (memchr(arguments[LEFT_OPERAND].text,0,arguments[LEFT_OPERAND].text_length)!=NULL) return nothing_value();
    Value request=arguments[RIGHT_OPERAND];
    if (request.kind==KIND_LIST && (!value_proper_list(request) || (request.item_count!=1 && request.item_count!=2) || (request.item_count==2 && (list_at(request,1).kind!=KIND_INTEGER || list_at(request,1).integer<0 || list_at(request,1).integer>1)))) return nothing_value();
    long long extent = gift_extent(arguments[RIGHT_OPERAND]);
    if (extent < WORD_FALSE) return nothing_value();
    if ((unsigned long long)extent > SIZE_MAX - GIFT_HEADER_BYTES) fail("shm_offer extent exceeds addressable memory");
    gift_reserve();
    char *name = copy_bytes(arguments[LEFT_OPERAND].text, arguments[LEFT_OPERAND].text_length);
    // Both byte and native values use the Form-owned growable reservation.
    // The POSIX object is a stable descriptor; existing readers never need a
    // replacement mapping when a caller admits a larger payload.
    Value handle=gift_map(name,1,NODE_REFERENCE_BYTES);
    GiftMapping *mapping=gift_mapping(handle);
    if (mapping!=NULL) {
        mapping->allowance=extent;
        Value lifetime=list_at(arguments[RIGHT_OPERAND],1);
        mapping->lifetime=lifetime.kind==KIND_INTEGER && lifetime.integer==1;
    }
    return handle;
}

static Value offer_shm_receive(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "shm_receive expects a name");
    if (memchr(arguments[SOLE_OPERAND].text,0,arguments[SOLE_OPERAND].text_length)!=NULL) return nothing_value();
    gift_reserve();
    char *name = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    return gift_map(name, 0, EMPTY_COUNT);
}

static Value offer_shm_read(Value *arguments, size_t argument_count) {
    (void)argument_count;
    GiftMapping *mapping = gift_mapping(arguments[SOLE_OPERAND]);
    if (!gift_refresh(mapping) || mapping->bytes < GIFT_HEADER_BYTES) return nothing_value();
    long long started = monotonic_milliseconds();
    for (;;) {
        uint64_t first = shared_load((char *)mapping->address + GIFT_SEQUENCE_OFFSET);
        if ((first & 1ULL) == 0) {
            if (first == 0) {
                if (shared_load((char *)mapping->address + GIFT_SEQUENCE_OFFSET) == 0) return nothing_value();
            } else {
                uint64_t length = shared_load((char *)mapping->address + GIFT_LENGTH_OFFSET);
                if (length <= mapping->bytes - GIFT_HEADER_BYTES) {
                    char *copy = copy_bytes((char *)mapping->address + GIFT_HEADER_BYTES, (size_t)length);
                    if (shared_load((char *)mapping->address + GIFT_SEQUENCE_OFFSET) == first) {
                        Value text = text_value(copy, (size_t)length);
                        free(copy);
                        return text;
                    }
                    free(copy);
                }
            }
        }
        if (gift_budget_spent(started)) return nothing_value();
    }
}

static Value gift_publish_compare(GiftMapping *mapping, const void *payload, size_t length, Value prior, Value prior_sequence) {
    if (mapping == NULL || !mapping->writable || mapping->bytes < GIFT_HEADER_BYTES || length > mapping->write_limit || length > mapping->bytes - GIFT_HEADER_BYTES) return nothing_value();
    void *sequence = (char *)mapping->address + GIFT_SEQUENCE_OFFSET;
    uint64_t even=shared_load(sequence), expected=even;
    if (prior_sequence.kind!=KIND_NOTHING && (prior_sequence.kind!=KIND_INTEGER || prior_sequence.integer<0 || (uint64_t)prior_sequence.integer!=even)) return nothing_value();
    // Contention is an absent offer. Elapsed time cannot prove another writer
    // died and must never authorize clearing its publication ownership.
    if ((even&1) || !__atomic_compare_exchange_n((uint64_t *)sequence,&expected,even+1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)) return nothing_value();
    if (prior.kind!=KIND_NOTHING && (prior.kind!=KIND_TEXT || shared_load((char *)mapping->address+GIFT_LENGTH_OFFSET)!=prior.text_length || prior.text_length>mapping->bytes-GIFT_HEADER_BYTES || memcmp((char *)mapping->address+GIFT_HEADER_BYTES,prior.text,prior.text_length)!=0)) {
        shared_store(sequence,even); return nothing_value();
    }
    shared_store((char *)mapping->address + GIFT_LENGTH_OFFSET, (uint64_t)length);
    if (length != EMPTY_COUNT) memcpy((char *)mapping->address + GIFT_HEADER_BYTES, payload, length);
    shared_store(sequence, even + 2);
    return integer_value((long long)(even + 2));
}

static Value gift_field_leaf(Value request) {
    Value command=list_at(request,0), handle=list_at(request,1);
    if (bytes_equal_cstr(command.text,command.text_length,"gift-open")) {
        if (handle.kind!=KIND_TEXT || memchr(handle.text,0,handle.text_length)!=NULL) return nothing_value();
        gift_reserve();
        Value writable=list_at(request,2);
        Value opened=gift_map(copy_bytes(handle.text,handle.text_length),writable.kind==KIND_INTEGER && writable.integer==1 ? 2 : 0,0);
        Value result[]={integer_value(opened.kind==KIND_INTEGER ? 1 : gift_map_error==ENOENT ? 0 : -1),opened,integer_value(gift_map_error)};
        return list_from(result,3);
    }
    if (bytes_equal_cstr(command.text,command.text_length,"gift-release-status")) {
        if (handle.kind!=KIND_INTEGER || handle.integer<0 || (uint64_t)handle.integer>=gift_mapping_count) return nothing_value();
        GiftMapping *held=&gift_mappings[handle.integer];
        Value status[]={integer_value(held->held),integer_value(held->address!=NULL),integer_value(held->descriptor>=0),integer_value(held->extra_address!=NULL),integer_value(held->release_error)};
        return list_from(status,5);
    }
    if (bytes_equal_cstr(command.text,command.text_length,"gift-holds")) {
        Value *ids=allocate_zeroed(native_gift_view_count,sizeof *ids); size_t count=0;
        for (size_t i=0;i<native_gift_view_count;++i) if (native_gift_views[i].owner!=NULL) ids[count++]=integer_value(native_gift_views[i].generation);
        Value result=list_from(ids,count); free(ids); return result;
    }
    if (bytes_equal_cstr(command.text,command.text_length,"gift-view-info")) {
        if (handle.owner==NULL || handle.kind!=KIND_NODE || fk_nid_get((fk_node_id)handle.integer,0)!=63) return nothing_value();
        for (size_t i=0;i<native_gift_view_count;++i) if (native_gift_views[i].owner==handle.owner && native_gift_views[i].root.integer==handle.integer) return integer_value(native_gift_views[i].generation);
        return nothing_value();
    }
    if (bytes_equal_cstr(command.text,command.text_length,"gift-held") || bytes_equal_cstr(command.text,command.text_length,"gift-view")) {
        if (!field_context_ready || handle.kind!=KIND_INTEGER || handle.integer<=0) return nothing_value();
        for (size_t i=0;i<native_gift_view_count;++i) if (native_gift_views[i].owner!=NULL && native_gift_views[i].generation==handle.integer) {
            Value held=native_gift_views[i].root;
            if (bytes_equal_cstr(command.text,command.text_length,"gift-view") && (request.item_count!=5 || !values_identical(list_at(request,2),held) || !values_identical(list_at(request,3),held.cell->category_value) || !values_identical(list_at(request,4),held.cell->children_value))) fail("owned gift view disagrees with its admitted generation");
            return held;
        }
        if (!bytes_equal_cstr(command.text,command.text_length,"gift-view") || request.item_count!=5) return nothing_value();
        Value node=list_at(request,2), category=list_at(request,3), children=list_at(request,4);
        if (node.kind!=KIND_NODE || fk_nid_get((fk_node_id)node.integer,0)!=63 || category.kind!=KIND_NODE || children.kind!=KIND_LIST) return nothing_value();
        size_t slot=0; while (slot<native_gift_view_count && native_gift_views[slot].owner!=NULL) ++slot;
        if (slot==native_gift_view_count) native_gift_views=grow_list(native_gift_views,&native_gift_view_capacity,sizeof *native_gift_views,native_gift_view_count+1);
        Value owner=record_value(heap_allocate(1,sizeof(Record),HEAP_RECORD,1));
        Cell *cell=heap_allocate(1,sizeof *cell,HEAP_NATIVE_CELL,1);
        cell->category=(fk_node_id)node.integer; cell->kind=CELL_COMPOSITE;
        cell->category_value=category; cell->children_value=children;
        Value root=node_from_cell(cell); root.owner=owner.record;
        owner.record->blueprint=integer_value(0);
        owner.record->keys=allocate_zeroed(1,sizeof(Value)); owner.record->values=allocate_zeroed(1,sizeof(Value));
        owner.record->keys[0]=text_value("root",4); owner.record->values[0]=root;
        owner.record->count=owner.record->capacity=1;
        native_gift_views[slot]=(NativeGiftView){handle.integer,owner.record,root};
        if (slot==native_gift_view_count) ++native_gift_view_count;
        return root;
    }
    GiftMapping *mapping=gift_mapping(handle);
    if (mapping==NULL) return nothing_value();
    if (bytes_equal_cstr(command.text,command.text_length,"gift-unlink")) {
        if (!mapping->writable) return nothing_value();
        return shm_unlink(mapping->name)==0 ? integer_value(1) : nothing_value();
    }
    if (bytes_equal_cstr(command.text,command.text_length,"gift-meta")) {
        struct stat state;
        if (fstat(mapping->descriptor,&state)!=0) return nothing_value();
        Value fields[]={integer_value((long long)state.st_dev),integer_value((long long)state.st_ino),text_value(mapping->name,strlen(mapping->name)),integer_value(mapping->allowance),integer_value(mapping->lifetime),integer_value(mapping->writable)};
        return list_from(fields,6);
    }
    if (bytes_equal_cstr(command.text,command.text_length,"gift-read")) return offer_shm_read(&handle,1);
    if (bytes_equal_cstr(command.text,command.text_length,"gift-publish") && (request.item_count==4 || request.item_count==5)) {
        Value payload=list_at(request,2);
        if (payload.kind!=KIND_TEXT) return nothing_value();
        return gift_publish_compare(mapping,payload.text,payload.text_length,list_at(request,3),request.item_count==5 ? list_at(request,4) : nothing_value());
    }
    if (bytes_equal_cstr(command.text,command.text_length,"gift-restore") && request.item_count==5) {
        Value payload=list_at(request,2), observed=list_at(request,3);
        if (!mapping->writable || payload.kind!=KIND_TEXT || observed.kind!=KIND_INTEGER || observed.integer<=0 || !(observed.integer&1) || payload.text_length>mapping->bytes-GIFT_HEADER_BYTES) return nothing_value();
        // Form selects committed named references. This physical compare stays
        // beside the terminal write: zero device/inode and an odd sequence do
        // not identify a Darwin shared-memory object after name replacement.
        Value references=list_at(request,4);
        uint64_t length=shared_load((char *)mapping->address+GIFT_LENGTH_OFFSET);
        if (!value_proper_list(references) || length!=NODE_REFERENCE_BYTES || length>mapping->bytes-GIFT_HEADER_BYTES) return nothing_value();
        int matches=0;
        for (size_t i=0;i<references.item_count;++i) {
            Value reference=list_at(references,i);
            if (reference.kind==KIND_TEXT && reference.text_length==NODE_REFERENCE_BYTES && memcmp((char *)mapping->address+GIFT_HEADER_BYTES,reference.text,reference.text_length)==0) matches=1;
        }
        if (!matches) return nothing_value();
        uint64_t odd=(uint64_t)observed.integer;
        void *sequence=(char *)mapping->address+GIFT_SEQUENCE_OFFSET;
        // Form holds the store lease and supplies the committed attempt of a
        // proven ended writer. Repeating interrupted restoration is idempotent.
        if (!__atomic_compare_exchange_n((uint64_t *)sequence,&odd,odd,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)) return nothing_value();
        shared_store((char *)mapping->address+GIFT_LENGTH_OFFSET,payload.text_length);
        if (payload.text_length) memcpy((char *)mapping->address+GIFT_HEADER_BYTES,payload.text,payload.text_length);
        shared_store(sequence,odd+1);
        return integer_value((long long)(odd+1));
    }
    return nothing_value();
}

static Value offer_shm_seq(Value *arguments, size_t argument_count) {
    (void)argument_count;
    GiftMapping *mapping = gift_mapping(arguments[SOLE_OPERAND]);
    if (mapping == NULL || mapping->bytes < GIFT_HEADER_BYTES) return integer_value(-WORD_TRUE);
    long long started = monotonic_milliseconds();
    for (;;) {
        uint64_t sequence = shared_load((char *)mapping->address + GIFT_SEQUENCE_OFFSET);
        if ((sequence & 1ULL) == 0) return integer_value((long long)sequence);
        if (gift_budget_spent(started)) return integer_value((long long)sequence);
    }
}

static Value offer_shm_release(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value handle=arguments[SOLE_OPERAND];
    if (handle.kind!=KIND_INTEGER || handle.integer<0 || (uint64_t)handle.integer>=gift_mapping_count) return integer_value(0);
    GiftMapping *mapping=&gift_mappings[handle.integer];
    if (!mapping->held) return integer_value(0);
    int error=0;
    if (mapping->extra_address!=NULL) {
        if (munmap(mapping->extra_address,mapping->extra_bytes)==0) mapping->extra_address=NULL;
        else error=errno;
    }
    if (mapping->address!=NULL) {
        if (munmap(mapping->address,mapping->bytes)==0) mapping->address=NULL;
        else error=errno;
    }
    if (mapping->address==NULL && mapping->extra_address==NULL && mapping->descriptor>=0) {
        if (close(mapping->descriptor)==0) mapping->descriptor=-1;
        else {
            error=errno;
#if defined(__linux__)
            mapping->descriptor=-1;
#else
            if (error==EBADF) mapping->descriptor=-1;
#endif
        }
    }
    if (error!=0) mapping->release_error=error;
    if (mapping->address!=NULL || mapping->extra_address!=NULL || mapping->descriptor>=0) return nothing_value();
    free(mapping->name);
    mapping->name = NULL;
    mapping->held = 0;
    return error==0 ? integer_value(1) : nothing_value();
}

/* /fg-roster is the standing publisher wire: 64 header bytes followed by
 * 128-byte slots, each a terminated 120-byte name and int64 millisecond stamp.
 * Keep that shared ABI; a private variable-length layout corrupts live peers. */
static void roster_open(int create) {
    if (gift_roster.open) return;
    int descriptor = shm_open(ROSTER_OBJECT, create ? O_RDWR | O_CREAT : O_RDWR, GIFT_OPEN_MODE);
    if (descriptor < 0) return;
    struct stat state;
    if (fstat(descriptor, &state) != 0) { close(descriptor); return; }
    size_t bytes = state.st_size >= 0 ? (size_t)state.st_size : 0;
    if (create && bytes < 65536) {
        if (ftruncate(descriptor, 65536) != 0) { close(descriptor); return; }
        bytes = 65536;
    }
    if (bytes < 64 + 128) { close(descriptor); return; }
    void *address = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
    if (address == MAP_FAILED) { close(descriptor); return; }
    gift_roster.descriptor = descriptor; gift_roster.address = address;
    gift_roster.bytes = bytes; gift_roster.open = 1;
}

static Value offer_gift_roster_names(Value *arguments, size_t argument_count) {
    (void)arguments; (void)argument_count;
    roster_open(0);
    if (!gift_roster.open) return list_from(NULL, 0);
    size_t slots = (gift_roster.bytes - 64) / 128, count = 0;
    Value *rows = allocate_zeroed(slots, sizeof *rows);
    for (size_t i = 0; i < slots; ++i) {
        char *slot = (char *)gift_roster.address + 64 + i * 128;
        size_t length = strnlen(slot, 120);
        if (length == 0 || length >= 120) continue;
        long long spoke;
        memcpy(&spoke, slot + 120, sizeof spoke);
        Value pair[] = {text_value(slot, length), integer_value(spoke)};
        rows[count++] = list_from(pair, 2);
    }
    Value result = list_from(rows, count); free(rows); return result;
}

static Value offer_gift_roster_register(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value name = arguments[0];
    if (name.kind != KIND_TEXT || name.text_length == 0 || name.text_length >= 120 || memchr(name.text, 0, name.text_length) != NULL) return integer_value(-1);
    roster_open(1);
    if (!gift_roster.open) return integer_value(-1);
    size_t slots = (gift_roster.bytes - 64) / 128, selected = SIZE_MAX, vacant = SIZE_MAX, oldest = SIZE_MAX;
    long long oldest_at = LLONG_MAX;
    for (size_t i = 0; i < slots; ++i) {
        char *slot = (char *)gift_roster.address + 64 + i * 128;
        size_t length = strnlen(slot, 120);
        if (length == 0) { if (vacant == SIZE_MAX) vacant = i; }
        else if (length < 120 && bytes_equal(slot, length, name.text, name.text_length)) { selected = i; break; }
        else {
            long long spoke; memcpy(&spoke, slot + 120, sizeof spoke);
            if (oldest == SIZE_MAX || spoke < oldest_at) { oldest = i; oldest_at = spoke; }
        }
    }
    if (selected == SIZE_MAX) selected = vacant != SIZE_MAX ? vacant : oldest;
    if (selected != SIZE_MAX) {
        char *slot = (char *)gift_roster.address + 64 + selected * 128;
        memcpy(slot, name.text, name.text_length); slot[name.text_length] = 0;
        long long spoke = unix_milliseconds(); memcpy(slot + 120, &spoke, sizeof spoke);
    }
    return integer_value(selected != SIZE_MAX ? (long long)selected : -1);
}

static Value offer_terminal_cols(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    return terminal_extent(TERMINAL_COLUMNS);
}

static Value offer_terminal_lines(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    return terminal_extent(TERMINAL_ROWS);
}

static Value offer_host_file_write_text(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "host_file_write_text expects a path");
    expect_kind(arguments[RIGHT_OPERAND], KIND_TEXT, "host_file_write_text expects text");
    char *path = copy_bytes(arguments[LEFT_OPERAND].text, arguments[LEFT_OPERAND].text_length);
    int descriptor = open(path, O_WRONLY | O_CREAT | O_TRUNC, TEXT_FILE_MODE);
    free(path);
    if (descriptor < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    const char *bytes = arguments[RIGHT_OPERAND].text;
    size_t length = arguments[RIGHT_OPERAND].text_length;
    size_t written = EMPTY_COUNT;
    while (written < length) {
        ssize_t step = write(descriptor, bytes + written, length - written);
        if (step < SYSTEM_CALL_OK) {
            if (errno == EINTR) continue;
            close(descriptor);
            return integer_value(-WORD_TRUE);
        }
        written = checked_add(written, (size_t)step);
    }
    if (close(descriptor) != SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    return integer_value((long long)written);
}

static Value offer_file_append_bytes(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "file_append_bytes expects a path");
    if (arguments[RIGHT_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[RIGHT_OPERAND].kind != KIND_LIST) {
        fail(message_with_name("file_append_bytes expects bytes, received ", kind_name(arguments[RIGHT_OPERAND].kind), strlen(kind_name(arguments[RIGHT_OPERAND].kind))));
    }
    size_t length = arguments[RIGHT_OPERAND].item_count;
    unsigned char *bytes = allocate_zeroed(length == EMPTY_COUNT ? ONE_ELEMENT : length, sizeof *bytes);
    List *cursor = arguments[RIGHT_OPERAND].list;
    for (size_t index = FIRST_INDEX; index < length; index = checked_add(index, ONE_ELEMENT)) {
        Value byte = cursor->head;
        cursor = cursor->tail;
        if (byte.kind != KIND_INTEGER || byte.integer < WORD_FALSE || byte.integer > UCHAR_MAX) {
            free(bytes);
            fail("file_append_bytes expects a byte");
        }
        bytes[index] = (unsigned char)byte.integer;
    }
    char *path = copy_bytes(arguments[LEFT_OPERAND].text, arguments[LEFT_OPERAND].text_length);
    int descriptor = open(path, O_WRONLY | O_CREAT | O_APPEND, TEXT_FILE_MODE);
    free(path);
    if (descriptor < SYSTEM_CALL_OK) {
        free(bytes);
        return integer_value(-WORD_TRUE);
    }
    size_t written = EMPTY_COUNT;
    while (written < length) {
        ssize_t step = write(descriptor, bytes + written, length - written);
        if (step < SYSTEM_CALL_OK) {
            if (errno == EINTR) continue;
            close(descriptor);
            free(bytes);
            return integer_value(-WORD_TRUE);
        }
        written = checked_add(written, (size_t)step);
    }
    free(bytes);
    struct stat state;
    int measured = fstat(descriptor, &state) == 0;
    if (close(descriptor) != SYSTEM_CALL_OK || !measured) return integer_value(-WORD_TRUE);
    return integer_value((long long)state.st_size);
}

static Value offer_file_mtime(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value path = arguments[0];
    if (path.kind != KIND_TEXT || memchr(path.text, 0, path.text_length) != NULL) return integer_value(-1);
    struct stat observed;
    return stat(path.text, &observed) == 0 ? integer_value((long long)observed.st_mtime) : integer_value(-1);
}

static Value offer_host_utimes(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value path = arguments[0], stamp = arguments[1];
    if (path.kind != KIND_TEXT || path.text_length == 0 || stamp.kind != KIND_INTEGER || memchr(path.text, 0, path.text_length) != NULL) return integer_value(-2);
    long long seconds = stamp.integer / 1000, remainder = stamp.integer % 1000;
    if (remainder < 0) { --seconds; remainder += 1000; }
    struct timespec times[2] = {{(time_t)seconds, (long)(remainder * 1000000)}, {(time_t)seconds, (long)(remainder * 1000000)}};
    return integer_value(utimensat(AT_FDCWD, path.text, times, 0) == 0 ? 0 : errno == ENAMETOOLONG ? -2 : -1);
}

static Value offer_host_chmod(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value path = arguments[0], mode = arguments[1];
    if (path.kind != KIND_TEXT || path.text_length == 0 || mode.kind != KIND_INTEGER || mode.integer < 0 || mode.integer > 07777 ||
        memchr(path.text, 0, path.text_length) != NULL) return integer_value(-2);
    return integer_value(chmod(path.text, (mode_t)mode.integer) == 0 ? 0 : errno == ENAMETOOLONG ? -2 : -1);
}

static int host_path_argument(Value path) {
    return path.kind == KIND_TEXT && path.text_length > 0 &&
        memchr(path.text, 0, path.text_length) == NULL;
}

static Value offer_host_link(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!host_path_argument(arguments[0]) || !host_path_argument(arguments[1])) return integer_value(-2);
    return integer_value(link(arguments[0].text, arguments[1].text) == 0 ? 0 : errno == ENAMETOOLONG ? -2 : -1);
}

static Value offer_host_file_mode(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!host_path_argument(arguments[0])) return integer_value(-2);
    struct stat observed;
    return stat(arguments[0].text, &observed) == 0 ? integer_value(observed.st_mode & 0xffff) : integer_value(errno == ENAMETOOLONG ? -2 : -1);
}

static Value offer_read_file_slice(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "read_file_slice expects a path");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "read_file_slice expects a start");
    expect_kind(arguments[SLICE_END], KIND_INTEGER, "read_file_slice expects a length");
    if (arguments[RIGHT_OPERAND].integer < WORD_FALSE || arguments[SLICE_END].integer < WORD_FALSE) return nothing_value();
    if (arguments[SLICE_END].integer == WORD_FALSE) return text_value("", EMPTY_COUNT);
    if ((unsigned long long)arguments[SLICE_END].integer > SIZE_MAX) fail("read_file_slice length exceeds addressable memory");
    char *path = copy_bytes(arguments[LEFT_OPERAND].text, arguments[LEFT_OPERAND].text_length);
    int descriptor = open(path, O_RDONLY);
    free(path);
    if (descriptor < SYSTEM_CALL_OK) return nothing_value();
    size_t length = (size_t)arguments[SLICE_END].integer;
    char *bytes = allocate_zeroed(checked_add(length, TEXT_TERMINATOR_SIZE), sizeof *bytes);
    ssize_t filled = pread(descriptor, bytes, length, (off_t)arguments[RIGHT_OPERAND].integer);
    close(descriptor);
    if (filled <= SYSTEM_CALL_OK) {
        free(bytes);
        return nothing_value();
    }
    Value text = text_value(bytes, (size_t)filled);
    free(bytes);
    return text;
}

static Value offer_host_file_read_text(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "host_file_read_text expects a path");
    char *path = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    int descriptor = open(path, O_RDONLY);
    free(path);
    if (descriptor < SYSTEM_CALL_OK) return nothing_value();
    struct stat info;
    if (fstat(descriptor, &info) != SYSTEM_CALL_OK || info.st_size < SYSTEM_CALL_OK) {
        close(descriptor);
        return nothing_value();
    }
    size_t length = (size_t)info.st_size;
    char *bytes = allocate_zeroed(checked_add(length, TEXT_TERMINATOR_SIZE), sizeof *bytes);
    size_t filled = EMPTY_COUNT;
    while (filled < length) {
        ssize_t step = read(descriptor, bytes + filled, length - filled);
        if (step < SYSTEM_CALL_OK) {
            if (errno == EINTR) continue;
            free(bytes);
            close(descriptor);
            return nothing_value();
        }
        if (step == SYSTEM_CALL_OK) break;
        filled = checked_add(filled, (size_t)step);
    }
    close(descriptor);
    Value text = text_value(bytes, filled);
    free(bytes);
    return text;
}

static Value offer_file_size(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "file_size expects a path");
    char *path = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    struct stat info;
    int found = stat(path, &info);
    free(path);
    /* An extent is a regular file's; a pipe, device or directory has none. */
    if (found != SYSTEM_CALL_OK || !S_ISREG(info.st_mode)) return integer_value(-WORD_TRUE);
    return integer_value((long long)info.st_size);
}

static Value offer_path_is_dir(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_TEXT, "host_path_is_dir expects a path");
    struct stat info;
    return boolean_word(stat(arguments[0].text, &info) == 0 && S_ISDIR(info.st_mode));
}

static Value offer_path_exists(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind != KIND_TEXT || memchr(arguments[0].text, 0, arguments[0].text_length) != NULL) return integer_value(0);
    /* Presence is a stat: nothing is opened, so a pipe never waits and an unreadable file is still present. */
    struct stat info;
    return integer_value(stat(arguments[0].text, &info) == 0 ? 1 : 0);
}

static Value offer_file_identity(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind != KIND_TEXT || arguments[0].text_length == 0 || memchr(arguments[0].text, 0, arguments[0].text_length) != NULL) return integer_value(-2);
    struct stat info;
    if (stat(arguments[0].text, &info) != 0) return errno == ENAMETOOLONG ? integer_value(-2) : nothing_value();
#ifdef __APPLE__
    long long modified = (long long)info.st_mtimespec.tv_sec * 1000 + info.st_mtimespec.tv_nsec / 1000000;
    long long born = (long long)info.st_birthtimespec.tv_sec * 1000 + info.st_birthtimespec.tv_nsec / 1000000;
#else
    long long modified = (long long)info.st_mtim.tv_sec * 1000 + info.st_mtim.tv_nsec / 1000000;
    long long born = 0;
#endif
    Value fields[] = {integer_value(info.st_size), integer_value(modified), integer_value(info.st_ino), integer_value(born)};
    return list_from(fields, 4);
}

static int compare_path_names(const void *left, const void *right) {
    return strcmp(((const Value *)left)->text, ((const Value *)right)->text);
}

static Value offer_dir_list(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_TEXT, "host_dir_list expects a path");
    DIR *dir = opendir(arguments[0].text);
    if (dir == NULL) return nothing_value();
    Value *names = NULL;
    size_t count = 0, capacity = 0;
    struct dirent *entry;
    int error = 0;
    for (;;) {
        errno = 0;
        entry = readdir(dir);
        if (entry == NULL) { error = errno; break; }
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        names = grow_list(names, &capacity, sizeof *names, checked_add(count, 1));
        names[count++] = text_value(entry->d_name, strlen(entry->d_name));
    }
    closedir(dir);
    if (error != 0) { free(names); return nothing_value(); }
    if (count > 1) qsort(names, count, sizeof *names, compare_path_names);
    Value result = list_from(names, count);
    free(names);
    return result;
}

static Value offer_mkdir_mode(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind != KIND_TEXT || arguments[0].text_length == 0 || arguments[1].kind != KIND_INTEGER ||
        memchr(arguments[0].text, 0, arguments[0].text_length) != NULL ||
        arguments[1].integer < 0 || arguments[1].integer > 07777) return integer_value(-2);
    if (mkdir(arguments[0].text, (mode_t)arguments[1].integer) != 0) return integer_value(errno == ENAMETOOLONG ? -2 : 0);
    chmod(arguments[0].text, (mode_t)arguments[1].integer);
    return integer_value(1);
}

static int remove_tree_at(int parent, const char *name) {
    struct stat info;
    if (fstatat(parent, name, &info, AT_SYMLINK_NOFOLLOW) != 0) return errno == ENOENT ? 0 : -1;
    if (!S_ISDIR(info.st_mode)) return unlinkat(parent, name, 0);
    int fd = openat(parent, name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW);
    if (fd < 0) return -1;
    DIR *dir = fdopendir(fd);
    if (dir == NULL) { close(fd); return -1; }
    Value *names = NULL;
    size_t count = 0, capacity = 0;
    int error = 0;
    for (;;) {
        errno = 0;
        struct dirent *entry = readdir(dir);
        if (entry == NULL) { error = errno; break; }
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        names = grow_list(names, &capacity, sizeof *names, checked_add(count, 1));
        names[count++] = text_value(entry->d_name, strlen(entry->d_name));
    }
    for (size_t i = 0; i < count; ++i) if (remove_tree_at(fd, names[i].text) != 0) error = 1;
    free(names);
    closedir(dir);
    return error != 0 ? -1 : unlinkat(parent, name, AT_REMOVEDIR);
}

static Value offer_dir_rmdir(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind != KIND_TEXT || memchr(arguments[0].text, 0, arguments[0].text_length) != NULL) return integer_value(-1);
    struct stat info;
    if (lstat(arguments[0].text, &info) != 0 || !S_ISDIR(info.st_mode)) return integer_value(-1);
    return integer_value(remove_tree_at(AT_FDCWD, arguments[0].text));
}

static Value offer_path_remove(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind != KIND_TEXT || memchr(arguments[0].text, 0, arguments[0].text_length) != NULL) return integer_value(-1);
    return integer_value(unlink(arguments[0].text));
}

typedef struct {
    int published, matched, released, error;
    const char *reason;
    size_t leases[3], lease_count;
} Publication;

static int file_lease_close(size_t index);
/* Publication has three possible descriptors: the two parent directories and
   the compared file. Reserve their unwind ownership before opening any of them.
   Form owns retry policy through the same native-posix leases as other I/O. */
static int publication_close(Publication *result, int fd) {
    size_t index=file_lease_count++;
    file_leases[index]=(FileLease){fd,active_cell};
    int closed=file_lease_close(index);
    if (!closed) { result->leases[result->lease_count++]=index+1; result->released=0; }
    return closed;
}

static int publication_parent(const char *path, const char **name) {
    size_t end = strlen(path);
    while (end > 1 && path[end - 1] == '/') --end;
    size_t cut = end;
    while (cut > 0 && path[cut - 1] != '/') --cut;
    char *parent = cut == 0 ? copy_bytes(".", 1) : copy_bytes(path, cut == 1 ? 1 : cut - 1);
    *name = path + cut;
#ifdef O_SEARCH
    int fd = open(parent, O_SEARCH | O_NONBLOCK);
#else
    int fd = open(parent, O_RDONLY | O_DIRECTORY | O_NONBLOCK);
#endif
    free(parent);
    return fd;
}

static int publication_matches(int parent, const char *name, Value expected, Publication *result) {
    int fd = openat(parent, name, O_RDONLY | O_NONBLOCK);
    if (fd < 0) return -1;
    struct stat info;
    int stated = fstat(fd, &info), saved = errno;
    if (stated != 0 || !S_ISREG(info.st_mode)) {
        int closed = publication_close(result,fd);
        if (stated != 0 && closed) errno = saved;
        return stated != 0 || !closed ? -1 : -2;
    }
    char bytes[16384];
    size_t at = 0;
    int match;
    for (;;) {
        ssize_t got = read(fd, bytes, sizeof bytes);
        if (got < 0 && errno == EINTR) continue;
        if (got < 0) { match = -1; break; }
        if (got == 0) { match = at == expected.text_length; break; }
        if ((size_t)got > expected.text_length - at || memcmp(bytes, expected.text + at, (size_t)got) != 0) { match = 0; break; }
        at += (size_t)got;
    }
    saved = errno;
    if (!publication_close(result,fd)) return -1;
    errno = saved;
    return match;
}

static Publication publish_path(const char *from, const char *to, Value expected) {
    Publication result = {0, 0, 1, 0, "publication-unavailable"};
    file_leases=grow_list(file_leases,&file_lease_capacity,sizeof *file_leases,file_lease_count+3);
    const char *from_name, *to_name;
    int lock = publication_parent(to, &to_name);
    if (lock < 0) { result.reason = "publication-directory-unavailable"; result.error = errno; return result; }
    int source = publication_parent(from, &from_name);
    if (source < 0) {
        result.reason = "publication-stage-directory-unavailable"; result.error = errno;
        publication_close(&result,lock); return result;
    }
    int locked;
    do { locked = flock(lock, LOCK_EX); } while (locked != 0 && errno == EINTR);
    if (locked != 0) {
        result.reason = "publication-lock-unavailable"; result.error = errno;
        publication_close(&result,source); publication_close(&result,lock); return result;
    }
    int match;
    if (expected.kind == KIND_NOTHING) {
        struct stat info;
        int present = fstatat(lock, to_name, &info, AT_SYMLINK_NOFOLLOW);
        match = present == 0 ? 0 : errno == ENOENT ? 1 : -1;
    } else match = publication_matches(lock, to_name, expected, &result);
    result.matched = match == 1;
    if (match == -2) result.reason = "destination-not-regular";
    else if (match < 0) { result.reason = "destination-read-failed"; result.error = errno; }
    else if (match == 0) result.reason = "destination-changed";
    else {
        result.published = renameat(source, from_name, lock, to_name) == 0;
        result.error = result.published ? 0 : errno;
        result.reason = result.published ? "published" : "rename-failed";
    }
    int comparison_error = result.error;
    flock(lock, LOCK_UN);
    int source_closed = publication_close(&result,source), source_error = errno;
    int closed = publication_close(&result,lock);
    if (!result.released) {
        result.error = !closed ? errno : !source_closed ? source_error : comparison_error;
        result.reason = "publication-release-unconfirmed";
    }
    return result;
}

static Value offer_path_rename(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value from = arguments[0], target = arguments[1], expected = nothing_value();
    int guarded = target.kind == KIND_RECORD;
    if (guarded) {
        Value get[] = {target, text_value("expected", 8)};
        if (!offer_record_has(get, 2).integer) return nothing_value();
        expected = offer_record_get(get, 2);
        if (expected.kind != KIND_NOTHING && expected.kind != KIND_TEXT) return nothing_value();
        get[1] = text_value("path", 4);
        target = offer_record_get(get, 2);
    }
    if (from.kind != KIND_TEXT || target.kind != KIND_TEXT || from.text_length == 0 || target.text_length == 0 ||
        memchr(from.text, 0, from.text_length) != NULL || memchr(target.text, 0, target.text_length) != NULL) return nothing_value();
    if (!guarded) return integer_value(rename(from.text, target.text) == 0 ? 0 : -1);
    Publication result = publish_path(from.text, target.text, expected);
    Value leases[3];
    for (size_t i=0;i<result.lease_count;++i) leases[i]=integer_value((long long)result.leases[i]);
    Value fields[] = {integer_value(0), text_value("published", 9), integer_value(result.published),
        text_value("matched", 7), integer_value(result.matched), text_value("released", 8), integer_value(result.released),
        text_value("os_error", 8), integer_value(result.error), text_value("reason", 6), text_value(result.reason, strlen(result.reason)),
        text_value("release_leases",14),list_from(leases,result.lease_count)};
    return offer_record_new(fields, sizeof fields / sizeof *fields);
}

#ifdef __APPLE__
/* libproc sizes are observations, not a fixed process/descriptor allowance. */
static void *process_rows(int pid, int *bytes) {
    int needed = pid == 0 ? proc_listpids(PROC_ALL_PIDS, 0, NULL, 0)
                          : proc_pidinfo(pid, PROC_PIDLISTFDS, 0, NULL, 0);
    if (needed <= 0) return NULL;
    void *rows = NULL;
    for (;;) {
        rows = realloc(rows, (size_t)needed);
        if (rows == NULL) fail("out of memory reading process ownership");
        int got = pid == 0 ? proc_listpids(PROC_ALL_PIDS, 0, rows, needed)
                          : proc_pidinfo(pid, PROC_PIDLISTFDS, 0, rows, needed);
        if (got <= 0) { free(rows); return NULL; }
        if (got < needed) { *bytes = got; return rows; }
        if (needed > INT_MAX / 2) { free(rows); return NULL; }
        needed *= 2;
    }
}

static int compare_pid_values(const void *left, const void *right) {
    long long a = ((const Value *)left)->integer, b = ((const Value *)right)->integer;
    return (a > b) - (a < b);
}
#endif

static Value offer_file_holders(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind != KIND_TEXT || arguments[0].text_length == 0 || memchr(arguments[0].text, 0, arguments[0].text_length) != NULL) return integer_value(-2);
#ifdef __APPLE__
    struct stat info;
    if (stat(arguments[0].text, &info) != 0) return errno == ENAMETOOLONG ? integer_value(-2) : nothing_value();
    int bytes = 0;
    int *pids = process_rows(0, &bytes);
    if (pids == NULL) return nothing_value();
    Value *holders = NULL;
    size_t count = 0, capacity = 0;
    for (size_t i = 0; i < (size_t)bytes / sizeof *pids; ++i) {
        if (pids[i] <= 0) continue;
        int fd_bytes = 0;
        struct proc_fdinfo *fds = process_rows(pids[i], &fd_bytes);
        if (fds == NULL) continue;
        int held = 0;
        for (size_t j = 0; !held && j < (size_t)fd_bytes / sizeof *fds; ++j) {
            if (fds[j].proc_fdtype != PROX_FDTYPE_VNODE) continue;
            struct vnode_fdinfowithpath vnode;
            int got = proc_pidfdinfo(pids[i], fds[j].proc_fd, PROC_PIDFDVNODEPATHINFO, &vnode, sizeof vnode);
            if (got != sizeof vnode) continue;
            held = vnode.pvip.vip_vi.vi_stat.vst_dev == (uint32_t)info.st_dev &&
                   vnode.pvip.vip_vi.vi_stat.vst_ino == (uint64_t)info.st_ino;
        }
        free(fds);
        if (!held) continue;
        holders = grow_list(holders, &capacity, sizeof *holders, checked_add(count, 1));
        holders[count++] = integer_value(pids[i]);
    }
    free(pids);
    if (count > 1) qsort(holders, count, sizeof *holders, compare_pid_values);
    Value result = list_from(holders, count);
    free(holders);
    return result;
#else
    return nothing_value();
#endif
}

static Value offer_vm_stat(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
#ifdef __APPLE__
    vm_statistics64_data_t vm;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    mach_port_t host = mach_host_self();
    kern_return_t status = host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm, &count);
    mach_port_deallocate(mach_task_self(), host);
    uint64_t memory = 0;
    size_t size = sizeof memory;
    long page = sysconf(_SC_PAGESIZE);
    if (status != KERN_SUCCESS || page <= 0 || sysctlbyname("hw.memsize", &memory, &size, NULL, 0) != 0) return list_from(NULL, 0);
    uint64_t readings[] = {(uint64_t)page, memory, vm.free_count, vm.active_count, vm.inactive_count,
        vm.wire_count, vm.speculative_count, vm.purgeable_count, vm.compressor_page_count,
        vm.pageins, vm.pageouts, vm.compressions, vm.decompressions, vm.swapins, vm.swapouts,
        vm.external_page_count, vm.internal_page_count, vm.total_uncompressed_pages_in_compressor};
    Value values[sizeof readings / sizeof *readings];
    for (size_t i = 0; i < sizeof readings / sizeof *readings; ++i) values[i] = integer_value((long long)readings[i]);
    return list_from(values, sizeof values / sizeof *values);
#else
    return list_from(NULL, 0);
#endif
}

static Value offer_host_process(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind != KIND_INTEGER || arguments[0].integer <= 0 || arguments[0].integer > INT_MAX) return integer_value(-1);
    pid_t pid = (pid_t)arguments[0].integer;
    Value row[] = {integer_value(pid), integer_value(0), text_value("", 0), text_value("", 0), list_from(NULL, 0), integer_value(0), integer_value(7), integer_value(0)};
#ifdef __APPLE__
    struct proc_bsdinfo info;
    int got = proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &info, sizeof info);
    if (got != sizeof info) {
        if (kill(pid, 0) != 0 && errno != EPERM) return integer_value(-1);
        struct kinfo_proc process;
        if (process_table_row(pid, &process)) {
            row[1] = integer_value(process.kp_eproc.e_ppid);
            row[5] = integer_value((long long)process.kp_proc.p_starttime.tv_sec * 1000 + process.kp_proc.p_starttime.tv_usec / 1000);
            row[7] = integer_value(process.kp_proc.p_stat == SZOMB ? 3 : process.kp_proc.p_stat == SSTOP ? 2 : 1);
        }
    } else {
        row[1] = integer_value(info.pbi_ppid);
        row[5] = integer_value((long long)info.pbi_start_tvsec * 1000 + info.pbi_start_tvusec / 1000);
        row[7] = integer_value(info.pbi_status == SZOMB ? 3 : info.pbi_status == SSTOP ? 2 : 1);
    }
    char path[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(pid, path, sizeof path) > 0) { row[2] = text_value(path, strlen(path)); row[6].integer &= ~1; }
    struct proc_vnodepathinfo vnode;
    if (proc_pidinfo(pid, PROC_PIDVNODEPATHINFO, 0, &vnode, sizeof vnode) == sizeof vnode) {
        row[3] = text_value(vnode.pvi_cdir.vip_path, strlen(vnode.pvi_cdir.vip_path)); row[6].integer &= ~2;
    }
    int mib[] = {CTL_KERN, KERN_PROCARGS2, pid};
    size_t length = 0;
    if (sysctl(mib, 3, NULL, &length, NULL, 0) == 0 && length >= sizeof(int)) {
        char *bytes = allocate_zeroed(length, 1);
        if (sysctl(mib, 3, bytes, &length, NULL, 0) == 0 && length >= sizeof(int)) {
            int argc;
            memcpy(&argc, bytes, sizeof argc);
            size_t at = sizeof argc;
            while (at < length && bytes[at] != 0) ++at;
            while (at < length && bytes[at] == 0) ++at;
            Value *argv = NULL;
            size_t count = 0, capacity = 0;
            while (argc > 0 && count < (size_t)argc && at < length) {
                size_t start = at;
                while (at < length && bytes[at] != 0) ++at;
                if (at == length) break;
                argv = grow_list(argv, &capacity, sizeof *argv, checked_add(count, 1));
                argv[count++] = text_value(bytes + start, at - start);
                ++at;
            }
            if (argc >= 0 && count == (size_t)argc) { row[4] = list_from(argv, count); row[6].integer &= ~4; }
            free(argv);
        }
        free(bytes);
    }
#else
    if (kill(pid, 0) != 0 && errno != EPERM) return integer_value(-1);
#endif
    return list_from(row, sizeof row / sizeof *row);
}

static long long cpu_microseconds(void);

static Value offer_host_processes(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value name = arguments[0];
    if (name.kind != KIND_TEXT || memchr(name.text, 0, name.text_length) != NULL) return nothing_value();
    size_t length = name.text_length;
    int prefix = length > 0 && name.text[length - 1] == '*';
    if (prefix) --length;
    if (length == 0) return list_from(NULL, 0);
#ifdef __APPLE__
    int bytes = 0;
    int *pids = process_rows(0, &bytes);
    if (pids == NULL) return prefix ? nothing_value() : list_from(NULL, 0);
    Value *rows = NULL;
    size_t count = 0, capacity = 0;
    mach_timebase_info_data_t timebase;
    if (mach_timebase_info(&timebase) != KERN_SUCCESS || timebase.denom == 0) { free(pids); return nothing_value(); }
    time_t now = time(NULL);
    for (size_t i = 0; i < (size_t)bytes / sizeof *pids; ++i) {
        pid_t pid = pids[i];
        if (pid <= 0) continue;
        struct kinfo_proc process;
        int have_process = 0;
        char command[PROC_PIDPATHINFO_MAXSIZE] = {0};
        if (proc_name(pid, command, sizeof command) <= 0) {
            if (!prefix) continue;
            have_process = process_table_row(pid, &process);
            if (!have_process) {
                if (kill(pid, 0) != 0 && errno == ESRCH) continue;
                Value identifier = integer_value(pid);
                rows = grow_list(rows, &capacity, sizeof *rows, checked_add(count, 1));
                rows[count++] = list_from(&identifier, 1);
                continue;
            }
            if (process.kp_eproc.e_ucred.cr_uid != getuid()) continue;
            snprintf(command, sizeof command, "%s", process.kp_proc.p_comm);
        }
        size_t command_length = strlen(command);
        if (command_length < length || memcmp(command, name.text, length) != 0 || (!prefix && command_length != length)) continue;
        Value row[] = {integer_value(pid), nothing_value(), nothing_value(), nothing_value(), nothing_value(), nothing_value()};
        struct proc_taskinfo task;
        if (proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &task, sizeof task) == sizeof task) {
            if (task.pti_resident_size <= LLONG_MAX) row[1] = integer_value((long long)task.pti_resident_size);
            __uint128_t micros = ((__uint128_t)task.pti_total_user + task.pti_total_system) * timebase.numer / timebase.denom / 1000;
            if (micros <= LLONG_MAX) row[2] = integer_value((long long)micros);
        } else if (!prefix) continue;
        struct proc_bsdinfo info;
        long long started = 0;
        if (proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &info, sizeof info) == sizeof info) {
            started = (long long)info.pbi_start_tvsec;
            row[4] = integer_value(info.pbi_nice);
            row[5] = integer_value(info.pbi_ppid);
        } else if (prefix && (have_process || process_table_row(pid, &process))) {
            started = process.kp_proc.p_starttime.tv_sec;
            row[4] = integer_value(process.kp_proc.p_nice);
            row[5] = integer_value(process.kp_eproc.e_ppid);
        }
        if (started > 0 && now >= started) row[3] = integer_value(now - started);
        rows = grow_list(rows, &capacity, sizeof *rows, checked_add(count, 1));
        rows[count++] = list_from(row, sizeof row / sizeof *row);
    }
    free(pids);
    Value result = list_from(rows, count);
    free(rows);
    return result;
#else
    return nothing_value();
#endif
}

static Value offer_host_cpu_us(Value *arguments, size_t argument_count) {
    (void)arguments; (void)argument_count;
    return integer_value(cpu_microseconds());
}

static Value offer_host_pid(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    return integer_value((long long)getpid());
}

static Value offer_temp_dir(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    const char *directory = getenv("TMPDIR");
    if (directory == NULL || directory[0] == TEXT_TERMINATOR) directory = TEMP_DIRECTORY;
    size_t length = strlen(directory);
    if (length > ONE_ELEMENT && directory[length - ONE_ELEMENT] == PATH_SEPARATOR) length -= ONE_ELEMENT;
    return text_value(directory, length);
}

static Value offer_host_cwd(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    size_t capacity = host_page_bytes();
    for (;;) {
        char *buffer = allocate_zeroed(capacity, sizeof *buffer);
        if (getcwd(buffer, capacity) != NULL) {
            Value text = text_value(buffer, strlen(buffer));
            free(buffer);
            return text;
        }
        free(buffer);
        if (errno != ERANGE) return nothing_value();
        if (capacity > SIZE_MAX / GROWTH_FACTOR) fail("the working directory exceeds addressable memory");
        capacity *= GROWTH_FACTOR;
    }
}

static Value offer_host_dir_mkdir(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "host_dir_mkdir expects a path");
    char *path = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    int made = mkdir(path, DIRECTORY_MODE);
    free(path);
    return boolean_word(made == 0);
}

static Value offer_host_spawn_at(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value command = arguments[0], redirects = arguments[1];
    if (command.kind != KIND_LIST || command.item_count == 0 || redirects.kind != KIND_LIST) return integer_value(-1);
    char **argv = allocate_zeroed(checked_add(command.item_count, 1), sizeof *argv);
    for (size_t i = 0; i < command.item_count; ++i) {
        Value word = list_at(command, i);
        if (word.kind != KIND_TEXT || memchr(word.text, 0, word.text_length) != NULL || (i == 0 && word.text_length == 0)) { free(argv); return integer_value(-1); }
        argv[i] = word.text;
    }
    const char *paths[4] = {NULL, NULL, NULL, NULL};
    for (size_t i = 0; i < 4 && i < redirects.item_count; ++i) {
        Value path = list_at(redirects, i);
        if (path.kind == KIND_NOTHING) continue;
        if (path.kind != KIND_TEXT || memchr(path.text, 0, path.text_length) != NULL) { free(argv); return integer_value(-1); }
        if (path.text_length != 0) paths[i] = path.text;
    }
    int descriptors[3] = {-1, -1, -1};
    int result = -2;
    for (size_t i = 0; i < 3; ++i) {
        if (paths[i] == NULL) continue;
        if (i == 2 && paths[1] != NULL && strcmp(paths[1], paths[2]) == 0) descriptors[2] = descriptors[1];
        else descriptors[i] = open(paths[i], i == 0 ? O_RDONLY : O_WRONLY | O_CREAT | O_APPEND, 0666);
        if (descriptors[i] < 0) goto cleanup;
    }
    posix_spawn_file_actions_t actions;
    result = -3;
    if (posix_spawn_file_actions_init(&actions) != 0) goto cleanup;
    int error = 0;
    for (size_t i = 0; i < 3 && error == 0; ++i) if (descriptors[i] >= 0) error = posix_spawn_file_actions_adddup2(&actions, descriptors[i], (int)i);
    for (size_t i = 0; i < 3 && error == 0; ++i) if (descriptors[i] > STDERR_FILENO && !(i == 2 && descriptors[2] == descriptors[1])) error = posix_spawn_file_actions_addclose(&actions, descriptors[i]);
    if (error == 0 && paths[3] != NULL) {
#if defined(__APPLE__) && __MAC_OS_X_VERSION_MIN_REQUIRED >= 260000
        error = posix_spawn_file_actions_addchdir(&actions, paths[3]);
#else
        error = posix_spawn_file_actions_addchdir_np(&actions, paths[3]);
#endif
    }
    pid_t pid = 0;
    if (error == 0) {
        error = posix_spawnp(&pid, argv[0], &actions, NULL, argv, environ);
        result = error == 0 ? (int)pid : (error == EAGAIN || error == ENOMEM ? -3 : -4);
    }
    posix_spawn_file_actions_destroy(&actions);
cleanup:
    for (size_t i = 0; i < 3; ++i) if (descriptors[i] >= 0 && !(i == 2 && descriptors[2] == descriptors[1])) close(descriptors[i]);
    free(argv);
    return integer_value(result);
}

static Value offer_host_spawn_quiet(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_LIST, "host_spawn_quiet expects a command list");
    Value command = arguments[SOLE_OPERAND];
    if (command.item_count == EMPTY_COUNT) fail("host_spawn_quiet expects a program");
    char **argv = allocate_zeroed(checked_add(command.item_count, ONE_ELEMENT), sizeof *argv);
    List *cursor = command.list;
    for (size_t index = FIRST_INDEX; index < command.item_count; index = checked_add(index, ONE_ELEMENT)) {
        expect_kind(cursor->head, KIND_TEXT, "host_spawn_quiet expects text arguments");
        argv[index] = cursor->head.text;
        cursor = cursor->tail;
    }
    posix_spawn_file_actions_t actions;
    if (posix_spawn_file_actions_init(&actions) != SYSTEM_CALL_OK) fail("host_spawn_quiet could not prepare output");
    if (posix_spawn_file_actions_addopen(&actions, STDOUT_FILENO, NULL_DEVICE, O_WRONLY, NULL_DEVICE_MODE) != SYSTEM_CALL_OK
        || posix_spawn_file_actions_addopen(&actions, STDERR_FILENO, NULL_DEVICE, O_WRONLY, NULL_DEVICE_MODE) != SYSTEM_CALL_OK) {
        posix_spawn_file_actions_destroy(&actions);
        fail("host_spawn_quiet could not prepare output");
    }
    pid_t pid = 0;
    int started = posix_spawnp(&pid, argv[FIRST_INDEX], &actions, NULL, argv, environ);
    posix_spawn_file_actions_destroy(&actions);
    free(argv);
    if (started != SYSTEM_CALL_OK) fail("host_spawn_quiet could not start");
    return integer_value((long long)pid);
}

static char *read_descriptor(int descriptor, size_t *length) {
    size_t capacity = host_page_bytes();
    char *buffer = allocate_zeroed(capacity, sizeof *buffer);
    size_t filled = EMPTY_COUNT;
    for (;;) {
        if (filled == capacity) {
            if (capacity > SIZE_MAX / GROWTH_FACTOR) fail("host-exec output exceeds addressable memory");
            size_t grown = capacity * GROWTH_FACTOR;
            char *next = allocate_zeroed(grown, sizeof *next);
            memcpy(next, buffer, filled);
            free(buffer);
            buffer = next;
            capacity = grown;
        }
        ssize_t step = read(descriptor, buffer + filled, capacity - filled);
        if (step < SYSTEM_CALL_OK) {
            if (errno == EINTR) continue;
            free(buffer);
            return NULL;
        }
        if (step == SYSTEM_CALL_OK) {
            *length = filled;
            return buffer;
        }
        filled = checked_add(filled, (size_t)step);
    }
}

static Value host_argv_child(Value command, int capture) {
    if (command.kind != KIND_LIST || command.item_count == 0) return nothing_value();
    char **argv = allocate_zeroed(command.item_count + 1, sizeof *argv);
    List *cursor = command.list;
    for (size_t i = 0; i < command.item_count; ++i) {
        if (cursor == NULL || cursor->dotted_tail != NULL || cursor->head.kind != KIND_TEXT || memchr(cursor->head.text, 0, cursor->head.text_length) != NULL || (i == 0 && cursor->head.text_length == 0)) { free(argv); return nothing_value(); }
        argv[i] = cursor->head.text; cursor = cursor->tail;
    }
    int output[2] = {-1, -1};
    if (capture && pipe(output) != 0) { free(argv); return nothing_value(); }
    pid_t pid = fork();
    if (pid == 0) {
        if (capture) {
            close(output[0]);
            if (dup2(output[1], STDOUT_FILENO) < 0) _exit(127);
            if (output[1] != STDOUT_FILENO) close(output[1]);
        }
        execvp(argv[0], argv); _exit(127);
    }
    free(argv);
    if (pid < 0) { if (capture) { close(output[0]); close(output[1]); } return nothing_value(); }
    if (!capture) return integer_value(pid);
    close(output[1]);
    size_t length = 0; char *bytes = read_descriptor(output[0], &length);
    close(output[0]);
    int status = 0; pid_t reaped;
    do { reaped = waitpid(pid, &status, 0); } while (reaped < 0 && errno == EINTR);
    if (bytes == NULL || reaped != pid) { free(bytes); return nothing_value(); }
    Value value = text_value(bytes, length); free(bytes); return value;
}

static Value offer_host_spawn(Value *arguments, size_t argument_count) {
    (void)argument_count; return host_argv_child(arguments[0], 0);
}

static Value offer_host_capture(Value *arguments, size_t argument_count) {
    (void)argument_count; return host_argv_child(arguments[0], 1);
}

static Value offer_host_exec(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "host-exec expects a command");
    expect_kind(arguments[RIGHT_OPERAND], KIND_TEXT, "host-exec expects input");
    int inbound[2];
    int outbound[2];
    if (pipe(inbound) != SYSTEM_CALL_OK || pipe(outbound) != SYSTEM_CALL_OK) return nothing_value();
    posix_spawn_file_actions_t actions;
    if (posix_spawn_file_actions_init(&actions) != SYSTEM_CALL_OK) {
        close(inbound[0]); close(inbound[1]); close(outbound[0]); close(outbound[1]);
        return nothing_value();
    }
    if (posix_spawn_file_actions_adddup2(&actions, inbound[0], STDIN_FILENO) != SYSTEM_CALL_OK
        || posix_spawn_file_actions_adddup2(&actions, outbound[1], STDOUT_FILENO) != SYSTEM_CALL_OK
        || posix_spawn_file_actions_addclose(&actions, inbound[0]) != SYSTEM_CALL_OK
        || posix_spawn_file_actions_addclose(&actions, inbound[1]) != SYSTEM_CALL_OK
        || posix_spawn_file_actions_addclose(&actions, outbound[0]) != SYSTEM_CALL_OK
        || posix_spawn_file_actions_addclose(&actions, outbound[1]) != SYSTEM_CALL_OK) {
        posix_spawn_file_actions_destroy(&actions);
        close(inbound[0]); close(inbound[1]); close(outbound[0]); close(outbound[1]);
        return nothing_value();
    }
    char *command = copy_bytes(arguments[LEFT_OPERAND].text, arguments[LEFT_OPERAND].text_length);
    char *argv[] = {(char *)SHELL_PATH, (char *)SHELL_COMMAND, command, NULL};
    pid_t pid = 0;
    int started = posix_spawn(&pid, SHELL_PATH, &actions, NULL, argv, environ);
    posix_spawn_file_actions_destroy(&actions);
    close(inbound[0]);
    close(outbound[1]);
    if (started != SYSTEM_CALL_OK) {
        close(inbound[1]);
        close(outbound[0]);
        free(command);
        return nothing_value();
    }
    size_t sent = EMPTY_COUNT;
    size_t input_length = arguments[RIGHT_OPERAND].text_length;
    while (sent < input_length) {
        ssize_t step = write(inbound[1], arguments[RIGHT_OPERAND].text + sent, input_length - sent);
        if (step < SYSTEM_CALL_OK) {
            if (errno == EINTR) continue;
            break;
        }
        sent = checked_add(sent, (size_t)step);
    }
    close(inbound[1]);
    size_t length = EMPTY_COUNT;
    char *output = read_descriptor(outbound[0], &length);
    close(outbound[0]);
    int status = 0;
    waitpid(pid, &status, 0);
    free(command);
    if (output == NULL) return nothing_value();
    Value text = text_value(output, length);
    free(output);
    return text;
}

static Value offer_print(Value *arguments, size_t argument_count) {
    if (active_cell != NULL && !active_cell->capture) return integer_value(0);
    for (size_t i = 0; i < argument_count; ++i) {
        if (i != 0) fputc(' ', runtime_output());
        print_value(arguments[i]);
    }
    fputc(NEWLINE, runtime_output());
    return integer_value(0);
}

static Value offer_str_eq(Value *arguments, size_t argument_count) {
    (void)argument_count;
    for (size_t i = 0; i < ARITY_TWO; ++i)
        if (arguments[i].kind != KIND_TEXT && arguments[i].kind != KIND_NOTHING)
            fail("str_eq expects text or nothing");
    if (arguments[LEFT_OPERAND].kind == KIND_NOTHING || arguments[RIGHT_OPERAND].kind == KIND_NOTHING)
        return boolean_word(arguments[LEFT_OPERAND].kind == arguments[RIGHT_OPERAND].kind);
    return boolean_word(bytes_equal(arguments[LEFT_OPERAND].text, arguments[LEFT_OPERAND].text_length,
                                    arguments[RIGHT_OPERAND].text, arguments[RIGHT_OPERAND].text_length));
}

static Value offer_str_concat(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[LEFT_OPERAND].kind == KIND_NOTHING || arguments[RIGHT_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[LEFT_OPERAND].kind != KIND_TEXT || arguments[RIGHT_OPERAND].kind != KIND_TEXT) {
        const char *left_kind = kind_name(arguments[LEFT_OPERAND].kind);
        const char *right_kind = kind_name(arguments[RIGHT_OPERAND].kind);
        char detail[64];
        snprintf(detail, sizeof detail, "%s and %s", left_kind, right_kind);
        fail(message_with_name("str_concat expects text, received ", detail, strlen(detail)));
    }
    size_t left_length = arguments[LEFT_OPERAND].text_length;
    size_t right_length = arguments[RIGHT_OPERAND].text_length;
    size_t length = checked_add(left_length, right_length);
    char *text = heap_allocate(checked_add(length,TEXT_TERMINATOR_SIZE),1,HEAP_TEXT,length);
    if (left_length != EMPTY_COUNT) memcpy(text, arguments[LEFT_OPERAND].text, left_length);
    if (right_length != EMPTY_COUNT) memcpy(text + left_length, arguments[RIGHT_OPERAND].text, right_length);
    text[length] = TEXT_TERMINATOR;
    Value value = nothing_value();
    value.kind = KIND_TEXT;
    value.text = text;
    value.text_length = length;
    return value;
}

static Value offer_value_kind(Value *arguments, size_t argument_count) {
    (void)argument_count;
    const char *name = kind_name(arguments[SOLE_OPERAND].kind);
    Value value = nothing_value();
    value.kind = KIND_TEXT; value.text = (char *)name; value.text_length = strlen(name);
    return value;
}

/* Form owns FORMBIN framing and node traversal. These two scalar modes only
 * transport representation bits; no C serializer or second wire authority. */
static Value offer_float_leaf(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind != KIND_INTEGER) return nothing_value();
    long long mode = arguments[0].integer;
    Value value = arguments[1];
    if (mode == 58) return jit_cpu_run(value);
    if (mode == 59) return audio_capture_bytes(value);
    if (mode == 60) return native_posix_operation(value);
    if (mode == 41) {
        if (value.kind != KIND_INTEGER) return integer_value(-2);
        long long seconds=value.integer/1000;
        if (value.integer%1000<0) --seconds;
        time_t instant=(time_t)seconds;
        struct tm local;
        if ((long long)instant!=seconds || localtime_r(&instant,&local)==NULL) return integer_value(-1);
        const char *zone=local.tm_zone==NULL ? "" : local.tm_zone;
        Value fields[]={integer_value(local.tm_year+1900),integer_value(local.tm_mon+1),integer_value(local.tm_mday),integer_value(local.tm_hour),integer_value(local.tm_min),integer_value(local.tm_sec),integer_value(local.tm_wday),integer_value(local.tm_gmtoff),text_value(zone,strlen(zone))};
        return list_from(fields,9);
    }
    if (mode == 42) {
        struct utsname host;
        return uname(&host)==0 ? text_value(host.sysname,strlen(host.sysname)) : nothing_value();
    }
    if (mode == 61) return native_field_leaf(value);
    if (mode >= 10 && mode <= 16) return audio_speaker_leaf(mode, value);
    if (mode >= 29 && mode <= 32) return stage_door(mode, value);
    if (mode == 0) return offer_float_value(&value, 1);
    if (mode == 1) return offer_make_float32(&value, 1);
    if (mode == 3) return real_value(3.14159265358979323846264338327950288);
    if (mode == 4) return offer_value_kind(&value, 1);
    if ((mode != 34 && mode != 35) || !value_proper_list(value)) return nothing_value();
    int scalar_only = mode == 35 && value.item_count == 3 && list_at(value,2).kind == KIND_TEXT &&
        bytes_equal_cstr(list_at(value,2).text,list_at(value,2).text_length,"value");
    if (value.item_count != 2 && !scalar_only) return nothing_value();
    Value tag = list_at(value, 0), payload = list_at(value, 1);
    if (tag.kind != KIND_INTEGER || (tag.integer != 2 && tag.integer != 3)) return nothing_value();
    uint64_t bits = 0;
    if (mode == 34) {
        if (tag.integer == 3) {
            if (payload.kind != KIND_INTEGER) return nothing_value();
            memcpy(&bits, &payload.integer, sizeof bits);
        } else {
            if (payload.kind != KIND_REAL) return nothing_value();
            memcpy(&bits, &payload.real, sizeof bits);
        }
        char bytes[8];
        for (size_t i = 0; i < 8; ++i) bytes[i] = (char)(bits >> (i * 8));
        return text_value(bytes, 8);
    }
    if (payload.kind != KIND_TEXT || payload.text_length != 8) return nothing_value();
    for (size_t i = 0; i < 8; ++i) bits |= (uint64_t)(unsigned char)payload.text[i] << (i * 8);
    if (tag.integer == 3) {
        long long number;
        memcpy(&number, &bits, sizeof number);
        Value scalar = integer_value(number);
        return scalar_only ? scalar : nothing_value();
    }
    double number;
    memcpy(&number, &bits, sizeof number);
    return scalar_only ? real_value(number) : nothing_value();
}

static Value offer_nothing_value(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    return nothing_value();
}

static Value offer_nothing(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return boolean_word(arguments[SOLE_OPERAND].kind == KIND_NOTHING);
}

static Value offer_file_open(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value path = arguments[0];
    if (path.kind != KIND_TEXT || memchr(path.text, 0, path.text_length) != NULL) return integer_value(-1);
    return integer_value(open(path.text, O_RDONLY));
}

static Value offer_file_read(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind != KIND_INTEGER || arguments[0].integer < 0 || arguments[0].integer > INT_MAX ||
        arguments[1].kind != KIND_INTEGER || arguments[1].integer <= 0) return text_value("", 0);
    size_t count = (size_t)arguments[1].integer;
    if (count > SSIZE_MAX) return text_value("", 0);
    if (active_cell != NULL && arguments[0].integer == 0) {
        size_t remaining = active_cell->input_length - active_cell->input_offset;
        if (count > remaining) count = remaining;
        Value result = text_value(active_cell->input + active_cell->input_offset, count);
        active_cell->input_offset += count;
        return result;
    }
    char *bytes = allocate_zeroed(count, 1);
    ssize_t got;
    do { got = read((int)arguments[0].integer, bytes, count); } while (got < 0 && errno == EINTR);
    Value result = text_value(bytes, got > 0 ? (size_t)got : 0);
    free(bytes);
    return result;
}

static Value offer_file_close(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value descriptor = arguments[0];
    if (descriptor.kind != KIND_INTEGER || descriptor.integer < 0 || descriptor.integer > INT_MAX) return integer_value(-1);
    return integer_value(close((int)descriptor.integer));
}

static Value offer_read_line(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    if (active_cell != NULL) {
        if (active_cell->input_offset == active_cell->input_length) return nothing_value();
        size_t from = active_cell->input_offset;
        while (active_cell->input_offset < active_cell->input_length && active_cell->input[active_cell->input_offset] != '\n') ++active_cell->input_offset;
        size_t end = active_cell->input_offset;
        if (end < active_cell->input_length) ++active_cell->input_offset;
        return text_value(active_cell->input + from, end - from);
    }
    char *line = NULL;
    size_t capacity = EMPTY_COUNT;
    ssize_t got = getline(&line, &capacity, stdin);
    if (got < SYSTEM_CALL_OK) {
        int end_of_input = feof(stdin);
        int read_error = ferror(stdin);
        free(line);
        if (end_of_input && !read_error) return nothing_value();
        fail("read_line could not read");
    }
    size_t length = (size_t)got;
    if (length > EMPTY_COUNT && line[length - ONE_ELEMENT] == NEWLINE) length -= ONE_ELEMENT;
    Value value = text_value(line, length);
    free(line);
    return value;
}

static Value offer_fs_truncate(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value path = arguments[0], length = arguments[1];
    if (path.kind != KIND_TEXT || memchr(path.text, 0, path.text_length) != NULL ||
        length.kind != KIND_INTEGER || length.integer < 0 || (long long)(off_t)length.integer != length.integer) return nothing_value();
    int fd = open(path.text, O_WRONLY | O_CREAT, 0666);
    if (fd < 0) return nothing_value();
    int changed = ftruncate(fd, (off_t)length.integer);
    off_t extent = changed == 0 ? lseek(fd, 0, SEEK_END) : -1;
    int released = close(fd);
    return extent >= 0 && released == 0 ? integer_value((long long)extent) : nothing_value();
}

static Value offer_cell_path(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    // Imports keep the arriving call's owner; the parser cursor names the
    // current unit, not a new entry. The native cell owner carries this identity.
    const char *root = active_cell != NULL ? active_cell->path : main_program.path;
    return text_value(root, strlen(root));
}

static Value offer_self_source(Value *arguments, size_t argument_count) {
    (void)arguments; (void)argument_count;
    return text_value(entry_source_bytes != NULL ? entry_source_bytes : "", entry_source_length);
}

static Value offer_realpath(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value path = arguments[0];
    if (path.kind != KIND_TEXT || path.text_length == 0 || memchr(path.text, 0, path.text_length) != NULL) return integer_value(-2);
    char *resolved = realpath(path.text, NULL);
    if (resolved == NULL) return errno == ENAMETOOLONG ? integer_value(-2) : nothing_value();
    Value result = text_value(resolved, strlen(resolved));
    free(resolved);
    return result;
}

static Value offer_intern_bool(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return node_value((long long)fk_nid_make(1, 1, 3, integer_is_nonzero(arguments[SOLE_OPERAND]) ? 1 : 0));
}

static Value count_word(size_t count) {
    if (count > (size_t)LLONG_MAX) fail("a counter exceeds the native integer range");
    return integer_value((long long)count);
}

static Value list_from(Value *items, size_t item_count) {
    Value value = nothing_value();
    value.kind = KIND_LIST;
    value.item_count = item_count;
    List *tail = NULL;
    for (size_t index = item_count; index > FIRST_INDEX; ) {
        index -= ONE_ELEMENT;
        List *cell = heap_allocate(ONE_ELEMENT,sizeof *cell,HEAP_LIST,1);
        cell->head = items[index];
        cell->tail = tail;
        tail = cell;
    }
    value.list = tail;
    return value;
}

static Value offer_fb_record(Value *arguments, size_t argument_count) {
    (void)argument_count;
    frame_entered = checked_add(frame_entered, ONE_ELEMENT);
    frame_last_index = frame_entered;
    if (arguments[RECORD_WORD].kind != KIND_NODE && arguments[RECORD_WORD].kind != KIND_RECORD) {
        frame_rejected = checked_add(frame_rejected, ONE_ELEMENT);
        return arguments[RECORD_WORD];
    }
    expect_kind(arguments[RECORD_PATH], KIND_TEXT, "fb_record expects a path");
    expect_kind(arguments[RECORD_POSITION], KIND_INTEGER, "fb_record expects an integer position");
    size_t needed = checked_add(frame_record_count, ONE_ELEMENT);
    frame_records = grow_list(frame_records, &frame_record_capacity, sizeof *frame_records, needed);
    frame_records[frame_record_count].word = arguments[RECORD_WORD];
    frame_records[frame_record_count].path = arguments[RECORD_PATH].text;
    frame_records[frame_record_count].path_length = arguments[RECORD_PATH].text_length;
    frame_records[frame_record_count].position = arguments[RECORD_POSITION].integer;
    frame_record_count = needed;
    frame_since_clear = checked_add(frame_since_clear, ONE_ELEMENT);
    frame_accepted = checked_add(frame_accepted, ONE_ELEMENT);
    Cell *cell = arguments[RECORD_WORD].kind == KIND_NODE ? arguments[RECORD_WORD].cell : NULL;
    if (cell != NULL && cell->source == NULL) {
        cell->source = copy_bytes(arguments[RECORD_PATH].text, arguments[RECORD_PATH].text_length);
        cell->source_length = arguments[RECORD_PATH].text_length;
        cell->source_line = arguments[RECORD_POSITION].integer / SOURCE_LINE_STRIDE;
        cell->source_column = arguments[RECORD_POSITION].integer % SOURCE_LINE_STRIDE;
    }
    return arguments[RECORD_WORD];
}

static Value offer_fb_events(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    Value *records = allocate_zeroed(frame_record_count, sizeof *records);
    for (size_t index = FIRST_INDEX; index < frame_record_count; index = checked_add(index, ONE_ELEMENT)) {
        records[index] = frame_records[index].word;
    }
    Value result = list_from(records, frame_record_count); free(records); return result;
}

static Value offer_fb_clear(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    Value cleared = count_word(frame_record_count);
    frame_record_count = EMPTY_COUNT;
    frame_since_clear = EMPTY_COUNT;
    return cleared;
}

static size_t align_to_word(size_t bytes) {
    size_t word = sizeof(uint64_t);
    size_t mask = word - ONE_ELEMENT;
    if (bytes > SIZE_MAX - mask) fail("a shared record exceeds addressable memory");
    return (bytes + mask) & ~mask;
}

static uint64_t monotonic_ns(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != SYSTEM_CALL_OK) fail("the clock could not be read");
    return (uint64_t)now.tv_sec * NANOSECONDS_PER_SECOND + (uint64_t)now.tv_nsec;
}

static size_t cover_pages(size_t bytes) {
    long page = sysconf(_SC_PAGESIZE);
    if (page <= SYSTEM_CALL_OK) fail("the page size could not be read");
    size_t page_bytes = (size_t)page;
    if (bytes == EMPTY_COUNT) return page_bytes;
    size_t slack = page_bytes - ONE_ELEMENT;
    if (bytes > SIZE_MAX - slack) fail("a shared object exceeds addressable memory");
    size_t rounded = bytes + slack;
    return rounded - (rounded % page_bytes);
}

/* Existing /fg-bus1 stage protocol. Nesting grows with actual opens; the ring
 * and name table are the shared versioned layout, with cursors for continuation. */
#define STAGE_SLOTS 65536LL
#define STAGE_NAMES 16384LL
#define STAGE_NAME_OFFSET (4096 + (8LL << 20))
#define STAGE_BYTES (STAGE_NAME_OFFSET + (2LL << 20))
#define STAGE_WORD(k) (((long long *)stage_bus)[2 + (k)])
static char *stage_bus;
static int stage_tried;
static long long stage_pid, stage_body;
typedef struct { long long index, stage, key, time; } StageOpen;
static StageOpen *stage_stack;
static size_t stage_capacity;
static long long *stage_row(long long index) { return (long long *)(stage_bus + 4096) + (index & (STAGE_SLOTS - 1)) * 16; }
static long long *stage_name_entry(long long id, long long probe) { return (long long *)(stage_bus + STAGE_NAME_OFFSET) + ((id + probe) & (STAGE_NAMES - 1)) * 16; }
static long long stage_us(void) { return (long long)(monotonic_ns() / 1000); }
static long long stage_name(const char *bytes, size_t length) {
    long long id = (long long)(hash_bytes(VALUE_HASH_OFFSET, bytes, length) & ((1ULL << 62) - 1));
    if (id == 0) id = 1;
    for (long long probe = 0; probe < 32; ++probe) {
        long long *entry = stage_name_entry(id, probe), expected = 0;
        if (__atomic_compare_exchange_n(entry, &expected, id, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
            memcpy(entry + 2, bytes, length < 112 ? length : 112);
            __atomic_store_n(entry + 1, (long long)length | (1LL << 61) | (length > 112 ? 1LL << 62 : 0), __ATOMIC_RELEASE);
            __atomic_fetch_add(&STAGE_WORD(5), 1, __ATOMIC_RELAXED);
            return id;
        }
        if (expected == id) return id;
    }
    __atomic_fetch_add(&STAGE_WORD(8), 1, __ATOMIC_RELAXED);
    return id;
}
static void stage_exit(void) { stage_unwind(0, 3, "exit"); }
static int stage_ready(void) {
    if (stage_tried) return stage_bus != NULL;
    stage_tried = 1;
    int fd = shm_open("/fg-bus1", O_RDWR | O_CREAT, 0600);
    if (fd < 0) return 0;
    struct stat info;
    if (fstat(fd, &info) != 0 || (info.st_size < STAGE_BYTES && ftruncate(fd, STAGE_BYTES) != 0)) { close(fd); return 0; }
    void *mapped = mmap(NULL, STAGE_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    int closed = close(fd);
    if (mapped == MAP_FAILED) return 0;
    if (closed != 0) { munmap(mapped, STAGE_BYTES); return 0; }
    stage_bus = mapped;
    long long zero = 0;
    if (__atomic_load_n(&STAGE_WORD(0), __ATOMIC_ACQUIRE) == 0) {
        STAGE_WORD(1) = 1; STAGE_WORD(2) = STAGE_SLOTS; STAGE_WORD(4) = STAGE_NAMES; STAGE_WORD(7) = 1;
        __atomic_compare_exchange_n(&STAGE_WORD(6), &zero, stage_us(), 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
        zero = 0;
        __atomic_compare_exchange_n(&STAGE_WORD(0), &zero, 0x4642555331LL, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
    }
    if (__atomic_load_n(&STAGE_WORD(0), __ATOMIC_ACQUIRE) != 0x4642555331LL ||
        STAGE_WORD(1) != 1 || STAGE_WORD(2) != STAGE_SLOTS || STAGE_WORD(4) != STAGE_NAMES) {
        munmap(stage_bus, STAGE_BYTES); stage_bus = NULL; return 0;
    }
    stage_pid = getpid();
    char *cwd = getcwd(NULL, 0);
    stage_body = cwd != NULL ? stage_name(cwd, strlen(cwd)) : 0;
    free(cwd);
    Value owner = integer_value(stage_pid);
    offer_kernel_live(&owner, 1);
    atexit(stage_exit);
    return 1;
}
static long long stage_put(long long *row) {
    long long index = __atomic_fetch_add(&STAGE_WORD(3), 1, __ATOMIC_ACQ_REL);
    long long *slot = stage_row(index);
    __atomic_store_n(slot, 0, __ATOMIC_RELAXED);
    __atomic_thread_fence(__ATOMIC_RELEASE);
    if (row[1] == 0) { row[1] = stage_pid; row[10] = stage_body; }
    for (size_t i = 1; i < 12; ++i) slot[i] = row[i];
    __atomic_store_n(slot, index + 1, __ATOMIC_RELEASE);
    __atomic_fetch_add((long long *)stage_bus, 2, __ATOMIC_RELEASE);
    return index;
}
static long long stage_close_at(size_t depth, long long phase, long long outcome, long long amount, long long digest) {
    long long row[12] = {0};
    StageOpen open = stage_stack[depth];
    row[2] = stage_us(); row[3] = open.stage; row[4] = open.key; row[5] = phase | ((long long)depth << 8);
    row[6] = open.index; row[7] = row[2] - open.time; row[8] = outcome; row[9] = amount; row[11] = digest;
    stage_depth = depth;
    return stage_put(row);
}
static void stage_unwind(size_t depth, long long phase, const char *why) {
    if (stage_bus == NULL || stage_depth <= depth) return;
    long long outcome = why != NULL ? stage_name(why, strlen(why)) : 0;
    while (stage_depth > depth) stage_close_at(stage_depth - 1, phase, outcome, 0, 0);
}
static void stage_lastbreath(long long pid) {
    if (!stage_ready()) return;
    long long head = __atomic_load_n(&STAGE_WORD(3), __ATOMIC_ACQUIRE), pending = 0, outcome = 0;
    for (long long i = head; i > 0 && i > head - STAGE_SLOTS; ) {
        long long index = --i, *slot = stage_row(index);
        if (__atomic_load_n(slot, __ATOMIC_ACQUIRE) != index + 1 || slot[1] != pid) continue;
        if ((slot[5] & 255) != 1 || pending > 0) { pending += (slot[5] & 255) != 1 ? 1 : -1; continue; }
        long long row[12] = {0, pid, stage_us(), slot[3], slot[4], 4 | (slot[5] & ~255LL), index, 0, 0, 0, slot[10], 0};
        row[7] = row[2] - slot[2];
        row[8] = outcome != 0 ? outcome : (outcome = stage_name("lastbreath", 10));
        stage_put(row);
    }
}
static int stage_value_word(Value value, int named, long long *out) {
    if (value.kind == KIND_INTEGER) { *out = value.integer; return 1; }
    if (value.kind != KIND_TEXT) return 0;
    *out = named ? stage_name(value.text, value.text_length) : (long long)(hash_bytes(VALUE_HASH_OFFSET, value.text, value.text_length) & ((1ULL << 62) - 1));
    return 1;
}
static Value stage_door(long long mode, Value value) {
    if (!stage_ready()) return nothing_value();
    if (mode == 32) {
        if (value.kind != KIND_INTEGER) return nothing_value();
        for (long long probe = 0; probe < 32; ++probe) {
            long long *entry = stage_name_entry(value.integer, probe);
            long long id = __atomic_load_n(entry, __ATOMIC_ACQUIRE), flags = __atomic_load_n(entry + 1, __ATOMIC_ACQUIRE);
            if (id == 0 || (id == value.integer && flags == 0)) break;
            size_t length = (size_t)(flags & ((1LL << 61) - 1));
            if (id == value.integer) return text_value((char *)(entry + 2), length < 112 ? length : 112);
        }
        return nothing_value();
    }
    if (value.kind != KIND_LIST || value.item_count == 0) return nothing_value();
    Value head = offer_head(&value, 1), tail = offer_tail(&value, 1);
    long long row[12] = {0};
    if (mode == 29) {
        if (head.kind != KIND_TEXT || !stage_value_word(head, 1, &row[3]) || !stage_value_word(tail, 1, &row[4])) return nothing_value();
        row[2] = stage_us(); row[5] = 1 | ((long long)stage_depth << 8); row[6] = stage_depth != 0 ? stage_stack[stage_depth - 1].index : -1;
        stage_stack = grow_list(stage_stack, &stage_capacity, sizeof *stage_stack, checked_add(stage_depth, 1));
        long long index = stage_put(row);
        stage_stack[stage_depth++] = (StageOpen){index, row[3], row[4], row[2]};
        return integer_value(index);
    }
    if (mode == 30) {
        Value outcome = tail, amount = integer_value(0), digest = integer_value(0);
        if (tail.kind == KIND_LIST) {
            if (tail.item_count != 3) return nothing_value();
            outcome = list_at(tail, 0); amount = list_at(tail, 1); digest = list_at(tail, 2);
        }
        size_t depth = stage_depth;
        while (depth > 0 && stage_stack[depth - 1].index != head.integer) --depth;
        if (head.kind != KIND_INTEGER || depth == 0 || outcome.kind != KIND_TEXT || amount.kind != KIND_INTEGER ||
            !stage_value_word(outcome, 1, &row[8]) || !stage_value_word(digest, 0, &row[11])) return nothing_value();
        stage_unwind(depth, 3, NULL);
        return integer_value(stage_close_at(depth - 1, 2, row[8], amount.integer, row[11]));
    }
    if (mode != 31 || head.kind != KIND_INTEGER || tail.kind != KIND_INTEGER) return nothing_value();
    long long next = __atomic_load_n(&STAGE_WORD(3), __ATOMIC_ACQUIRE), now = stage_us(), index = head.integer, lapped = 0, holes = 0;
    if (index < 0) index = next;
    else if (index < next - STAGE_SLOTS) { lapped = next - STAGE_SLOTS - index; index = next - STAGE_SLOTS; }
    Value *rows = NULL;
    size_t count = 4, capacity = 0;
    rows = grow_list(rows, &capacity, sizeof *rows, count);
    while (index < next && count - 4 < 4096) {
        long long *slot = stage_row(index), sequence = __atomic_load_n(slot, __ATOMIC_ACQUIRE);
        for (size_t i = 1; i < 12; ++i) row[i] = slot[i];
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        if (sequence == index + 1 && __atomic_load_n(slot, __ATOMIC_RELAXED) == sequence) {
            long long fields[] = {index, row[1], row[2], row[3], row[4], row[5] & 255, row[5] >> 8, row[6], row[7], row[8], row[9], row[10], row[11]};
            Value values[13];
            for (size_t i = 0; i < 13; ++i) values[i] = integer_value(fields[i]);
            rows = grow_list(rows, &capacity, sizeof *rows, count + 1);
            rows[count++] = list_from(values, 13);
        } else if (sequence >= index + 1 || index + STAGE_SLOTS < __atomic_load_n(&STAGE_WORD(3), __ATOMIC_ACQUIRE)) ++lapped;
        else if (index <= tail.integer || next - index > 1024) ++holes;
        else break;
        ++index;
    }
    rows[0] = integer_value(index); rows[1] = integer_value(lapped); rows[2] = integer_value(holes); rows[3] = integer_value(now);
    Value result = list_from(rows, count);
    free(rows);
    return result;
}

typedef struct DispatchCensus {
    size_t total;
    size_t distinct;
    size_t hottest_tag;
    size_t hottest_count;
} DispatchCensus;

static DispatchCensus dispatch_census(void) {
    DispatchCensus census = {EMPTY_COUNT, EMPTY_COUNT, EMPTY_COUNT, EMPTY_COUNT};
    for (size_t index = FIRST_INDEX; index < operation_count; index = checked_add(index, ONE_ELEMENT)) {
        size_t calls = operations[index].calls;
        if (calls == EMPTY_COUNT) continue;
        census.total = checked_add(census.total, calls);
        census.distinct = checked_add(census.distinct, ONE_ELEMENT);
        if (calls > census.hottest_count) {
            census.hottest_count = calls;
            census.hottest_tag = (size_t)operations[index].tag;
        }
    }
    return census;
}

static void gift_held(size_t *frames, size_t *bytes) {
    *frames = EMPTY_COUNT;
    *bytes = EMPTY_COUNT;
    for (size_t index = FIRST_INDEX; index < gift_mapping_count; index = checked_add(index, ONE_ELEMENT)) {
        if (!gift_mappings[index].held) continue;
        *frames = checked_add(*frames, ONE_ELEMENT);
        if (gift_mappings[index].address!=NULL) *bytes = checked_add(*bytes,gift_mappings[index].bytes);
        if (gift_mappings[index].extra_address!=NULL) *bytes = checked_add(*bytes,gift_mappings[index].extra_bytes);
    }
}

static size_t node_private_bytes(void) {
    size_t bytes = cell_capacity * sizeof(Cell *);
    for (size_t index = FIRST_INDEX; index < cell_count; index = checked_add(index, ONE_ELEMENT)) {
        Cell *cell = cells[index];
        bytes = checked_add(bytes, sizeof *cell);
        if (cell->text != NULL) bytes = checked_add(bytes, checked_add(cell->text_length, TEXT_TERMINATOR_SIZE));
        if (cell->source != NULL) bytes = checked_add(bytes, checked_add(cell->source_length, TEXT_TERMINATOR_SIZE));
    }
    return bytes;
}

static long long cpu_microseconds(void) {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) != SYSTEM_CALL_OK) return WORD_FALSE;
    long long user = (long long)usage.ru_utime.tv_sec * MICROSECONDS_PER_SECOND + (long long)usage.ru_utime.tv_usec;
    long long system = (long long)usage.ru_stime.tv_sec * MICROSECONDS_PER_SECOND + (long long)usage.ru_stime.tv_usec;
    return user + system;
}

static int live_page_name(char *name, size_t room, long long pid) {
    if (pid < WORD_FALSE) return WORD_FALSE;
    int wrote = snprintf(name, room, "/fg7-k%lld", pid);
    return wrote > WORD_FALSE && (size_t)wrote < room;
}

enum { LIVE_WIRE_HEADER = 4, LIVE_WIRE_VERSION = 7 };
static const uint64_t LIVE_WIRE_MAGIC = UINT64_C(0x464b57554c495645);
static int64_t last_live_words[LIVE_PAGE_WORDS];
static int publish_live_page(long long pid, const int64_t *words);
static void release_live_page(void) {
    if (last_live_words[LIVE_PID] != (int64_t)getpid()) return;
    last_live_words[LIVE_ALIVE] = 0;
    publish_live_page((long long)getpid(), last_live_words);
}

static int publish_live_page(long long pid, const int64_t *words) {
    char name[LIVE_NAME_ROOM];
    if (pid != (long long)getpid() || !live_page_name(name, sizeof name, pid)) return 0;
    static int release_armed = WORD_FALSE;
    if (!release_armed) {
        atexit(release_live_page);
        release_armed = WORD_TRUE;
    }
    size_t bytes = (LIVE_WIRE_HEADER + LIVE_PAGE_WORDS) * sizeof(uint64_t);
    int descriptor = shm_open(name, O_RDWR | O_CREAT, 0600);
    if (descriptor < SYSTEM_CALL_OK) { fprintf(runtime_error(),"native live page open failed: errno=%d\n",errno); return 0; }
    struct stat standing = {0};
    if (fstat(descriptor, &standing) != SYSTEM_CALL_OK || standing.st_uid != getuid() || (standing.st_mode & 0077) != 0) {
        fprintf(runtime_error(),"native live page metadata refused: errno=%d uid=%lu mode=%lo\n",errno,(unsigned long)standing.st_uid,(unsigned long)standing.st_mode);
        close(descriptor);
        return 0;
    }
    // Darwin reports the page-rounded backing extent. The versioned header
    // carries the logical word count; host padding is not another wire body.
    if (standing.st_size < (off_t)bytes && (standing.st_size != 0 || ftruncate(descriptor, (off_t)bytes) != 0)) {
        fprintf(runtime_error(),"native live page extent refused: size=%lld expected=%zu errno=%d\n",(long long)standing.st_size,bytes,errno);
        close(descriptor); return 0;
    }
    uint64_t *mapped = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
    close(descriptor);
    if (mapped == MAP_FAILED) { fprintf(runtime_error(),"native live page mapping failed: errno=%d\n",errno); return 0; }
    // This page has exactly one writer: its named process. A replacement PID
    // owns a new birth, and never interprets the previous snapshot as its own.
    uint64_t sequence = __atomic_load_n(mapped + 3, __ATOMIC_ACQUIRE);
    sequence = (sequence | 1) + 2;
    __atomic_store_n(mapped + 3, sequence, __ATOMIC_RELEASE);
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    __atomic_store_n(mapped, LIVE_WIRE_MAGIC, __ATOMIC_RELAXED);
    __atomic_store_n(mapped + 1, LIVE_WIRE_VERSION, __ATOMIC_RELAXED);
    __atomic_store_n(mapped + 2, LIVE_PAGE_WORDS, __ATOMIC_RELAXED);
    for (size_t i = 0; i < LIVE_PAGE_WORDS; ++i) __atomic_store_n(mapped + LIVE_WIRE_HEADER + i, (uint64_t)words[i], __ATOMIC_RELAXED);
    __atomic_store_n(mapped + 3, sequence + 1, __ATOMIC_RELEASE);
    int released = munmap(mapped, bytes) == 0;
    if (released) memcpy(last_live_words, words, sizeof last_live_words);
    return released;
}

static int read_live_page(long long pid, int64_t *words) {
    char name[LIVE_NAME_ROOM];
    if (!live_page_name(name, sizeof name, pid)) return WORD_FALSE;
    int descriptor = shm_open(name, O_RDONLY, GIFT_OPEN_MODE);
    if (descriptor < SYSTEM_CALL_OK) return WORD_FALSE;
    struct stat standing;
    size_t bytes = (LIVE_WIRE_HEADER + LIVE_PAGE_WORDS) * sizeof(uint64_t);
    if (fstat(descriptor, &standing) != SYSTEM_CALL_OK || standing.st_uid != getuid() || (standing.st_mode & 0077) != 0 || standing.st_size < (off_t)bytes) {
        close(descriptor);
        return WORD_FALSE;
    }
    uint64_t *mapped = mmap(NULL, bytes, PROT_READ, MAP_SHARED, descriptor, 0);
    close(descriptor);
    if (mapped == MAP_FAILED) return WORD_FALSE;
    uint64_t first = __atomic_load_n(mapped + 3, __ATOMIC_ACQUIRE);
    int valid = first != 0 && !(first & 1) && __atomic_load_n(mapped, __ATOMIC_RELAXED) == LIVE_WIRE_MAGIC &&
        __atomic_load_n(mapped + 1, __ATOMIC_RELAXED) == LIVE_WIRE_VERSION && __atomic_load_n(mapped + 2, __ATOMIC_RELAXED) == LIVE_PAGE_WORDS;
    if (valid) for (size_t i = 0; i < LIVE_PAGE_WORDS; ++i) words[i] = (int64_t)__atomic_load_n(mapped + LIVE_WIRE_HEADER + i, __ATOMIC_RELAXED);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
    valid = valid && first == __atomic_load_n(mapped + 3, __ATOMIC_ACQUIRE);
    int released = munmap(mapped, bytes) == 0;
    return valid && released && words[LIVE_PID] == pid && words[LIVE_START_MS] > 0 && (words[LIVE_ALIVE] == 0 || words[LIVE_ALIVE] == 1);
}

static void fill_live_words(int64_t *words) {
    memset(words, EMPTY_COUNT, LIVE_PAGE_WORDS * sizeof *words);
    static pid_t process_owner;
    if (process_owner != 0 && process_owner != getpid()) {
        process_born_ms = unix_milliseconds(); node_mint_events = 0; live_sequence = 0;
    }
    process_owner = getpid();
    if (process_born_ms == WORD_FALSE) process_born_ms = unix_milliseconds();
    DispatchCensus census = dispatch_census();
    size_t frames = EMPTY_COUNT;
    size_t gift_bytes = EMPTY_COUNT;
    gift_held(&frames, &gift_bytes);
    live_sequence = checked_add((size_t)live_sequence, ONE_ELEMENT);
    words[LIVE_PID] = (int64_t)getpid();
    words[LIVE_START_MS] = process_born_ms;
    words[LIVE_SEQUENCE] = (int64_t)live_sequence;
    words[LIVE_DISPATCHES] = (int64_t)census.total;
    words[LIVE_NODES] = (int64_t)(field_context_ready ? field_nodes : cell_count);
    words[LIVE_STRINGS] = (int64_t)heap_text_allocations;
    words[LIVE_CONS] = (int64_t)heap_list_allocations;
    words[LIVE_MELTS] = (int64_t)heap_collection_epoch;
    words[LIVE_FUNCTIONS] = (int64_t)function_count;
    words[LIVE_GIFT_FRAMES] = (int64_t)frames;
    words[LIVE_GIFT_BYTES] = (int64_t)gift_bytes;
    words[LIVE_NODE_CAP] = (int64_t)cell_capacity;
    words[LIVE_STACK] = (int64_t)frame_count;
    words[LIVE_HOTTEST_TAG] = (int64_t)census.hottest_tag;
    words[LIVE_HOTTEST_COUNT] = (int64_t)census.hottest_count;
    words[LIVE_CPU_US] = cpu_microseconds();
    words[LIVE_ALIVE] = WORD_TRUE;
    words[LIVE_DISTINCT] = (int64_t)census.distinct;
    words[LIVE_STORE_SHARED] = field_context_ready ? 2 : 0;
    words[LIVE_FIELD_VERSION] = LIVE_WIRE_VERSION;
    words[LIVE_FIELD_DEVICE] = field_context_ready ? field_device : 0;
    words[LIVE_FIELD_INODE] = field_context_ready ? field_inode : 0;
    words[LIVE_MINTS] = (int64_t)node_mint_events;
}

static Value live_list(const int64_t *words) {
    Value *items = allocate_zeroed(LIVE_PAGE_WORDS, sizeof *items);
    for (size_t index = FIRST_INDEX; index < LIVE_PAGE_WORDS; index = checked_add(index, ONE_ELEMENT)) {
        items[index] = integer_value((long long)words[index]);
    }
    Value result = list_from(items, LIVE_PAGE_WORDS); free(items); return result;
}

static size_t native_execution_count;
static uint64_t cpu_image_runs, cpu_image_us;

static Value offer_kernel_stat(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "kernel_stat expects an integer key");
    long long key = arguments[SOLE_OPERAND].integer;
    if (key >= 100) {
        size_t calls = 0;
        for (size_t index = 0; index < operation_count; ++index) {
            if (operations[index].tag == key - 100) calls = checked_add(calls, operations[index].calls);
        }
        return count_word(calls);
    }
    if (key == STAT_ROOTS_SINCE_CLEAR) return count_word(frame_since_clear);
    if (key == STAT_REJECTED) return count_word(frame_rejected);
    if (key == STAT_ENTERED) return count_word(frame_entered);
    if (key == STAT_ACCEPTED) return count_word(frame_accepted);
    if (key == STAT_LAST_INDEX) return count_word(frame_last_index);
    if (key == STAT_GIFT_FRAMES || key == STAT_GIFT_BYTES) {
        size_t frames = EMPTY_COUNT;
        size_t bytes = EMPTY_COUNT;
        gift_held(&frames, &bytes);
        return count_word(key == STAT_GIFT_FRAMES ? frames : bytes);
    }
    if (key == STAT_ROOTS_STANDING) return count_word(frame_record_count);
    if (key == STAT_ROOT_CAPACITY) return count_word(frame_record_capacity);
    if (key == STAT_NODE_IDENTITY_BYTES) return integer_value((long long)sizeof(NodeIdentity));
    if (key == STAT_DISPATCHES || key == STAT_DISPATCH_DISTINCT || key == STAT_HOTTEST_TAG || key == STAT_HOTTEST_COUNT) {
        DispatchCensus census = dispatch_census();
        if (key == STAT_DISPATCHES) return count_word(census.total);
        if (key == STAT_DISPATCH_DISTINCT) return count_word(census.distinct);
        if (key == STAT_HOTTEST_TAG) return count_word(census.hottest_tag);
        return count_word(census.hottest_count);
    }
    if (key == STAT_CELLS) return count_word(field_context_ready ? field_nodes : cell_count);
    if (key == STAT_NODES_MINTED) return count_word(node_mint_events);
    if (key == STAT_NODES_GAS) return count_word(field_context_ready ? 0 : cell_count);
    if (key == STAT_NODES_ICE) return count_word(field_context_ready ? field_nodes : 0);
    if (field_context_ready && key==70) return count_word(field_string_bytes);
    if (field_context_ready && key==71) return count_word(field_strings);
    if (field_context_ready && key==72) return count_word(field_pairs);
    if (key == STAT_STRINGS) return count_word(heap_text_allocations);
    if (key == STAT_CONS) return count_word(heap_list_allocations);
    if (key == 69) return count_word(heap_collection_epoch);
    if (key == STAT_JIT_NATIVE) return count_word(native_execution_count);
    if (key == 77) return count_word(cpu_image_runs);
    if (key == 78) return count_word(cpu_image_us);
    if (key == STAT_CONS || key == STAT_FLOATS || key == STAT_JIT_BOXES || key == STAT_JIT_UNBOXES || key == STAT_JIT_NATIVE || key == STAT_JIT_CRYSTALLIZED || key == STAT_JIT_UNROLL || key == STAT_JIT_LOOP_ITERATIONS || key == STAT_FLOAT_CAPACITY || key == STAT_FIELD_FLOATS || key == STAT_NODES_WATER || key == STAT_NODES_ICE || key == STAT_NODE_LOGICAL_BYTES) return integer_value(WORD_FALSE);
    if (key == STAT_STACK) return count_word(frame_count);
    if (key == STAT_RECIPES) return count_word(function_count);
    if (key == STAT_NODE_PRIVATE_BYTES) return count_word(node_private_bytes());
    return nothing_value();
}

/* Temporary physical carriers: policy and supervision remain in the Form
 * host owner; these expose the platform's actual answer without a shell. */
static Value offer_isatty(Value *arguments, size_t argument_count) {
    (void)arguments; (void)argument_count;
    return boolean_word(isatty(STDIN_FILENO) != 0);
}

static Value offer_input_byte(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_INTEGER, "input_byte expects an integer offset");
    long long offset = arguments[0].integer;
    return integer_value(offset < 0 || (unsigned long long)offset >= staged_input_length ? 0 : staged_input[offset]);
}

static Value offer_host_nice(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_INTEGER, "host_nice expects an integer priority");
    if (arguments[0].integer < INT_MIN || arguments[0].integer > INT_MAX) return integer_value(-1);
    return integer_value(setpriority(PRIO_PROCESS, 0, (int)arguments[0].integer));
}

/* These legacy surfaces have no carrier on the POSIX seed. Preserve their
 * typed unavailable outcomes, not a hardware observation or fabricated result.
 * Actual Darwin audio, host metrics and Metal operations have separate owners. */
static Value offer_carrier_unavailable(Value *arguments, size_t argument_count) {
    (void)arguments; (void)argument_count; return integer_value(-1);
}
static Value offer_carrier_no_devices(Value *arguments, size_t argument_count) {
    (void)arguments; (void)argument_count; return integer_value(0);
}
static Value offer_carrier_no_text(Value *arguments, size_t argument_count) {
    (void)arguments; (void)argument_count; return text_value("", 0);
}
static Value offer_carrier_no_samples(Value *arguments, size_t argument_count) {
    (void)arguments; (void)argument_count; return list_from(NULL, 0);
}

static Value offer_kernel_live(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "kernel_live expects a pid");
    long long pid = arguments[SOLE_OPERAND].integer;
    int64_t words[LIVE_PAGE_WORDS];
    if (pid == (long long)getpid()) {
        fill_live_words(words);
        if (!publish_live_page(pid, words)) return list_from(NULL, EMPTY_COUNT);
        return live_list(words);
    }
    if (!read_live_page(pid, words)) return list_from(NULL, EMPTY_COUNT);
    return live_list(words);
}

static int kernel_page_state(Value pid) {
    if (pid.kind != KIND_INTEGER || pid.integer <= 0 || pid.integer > INT_MAX) return 0;
    char name[LIVE_NAME_ROOM];
    if (!live_page_name(name, sizeof name, pid.integer)) return 0;
    int fd = shm_open(name, O_RDONLY, GIFT_OPEN_MODE);
    if (fd < 0) return errno == ENOENT ? 0 : -1;
    int gone = process_proven_ended(pid);
    int64_t words[LIVE_PAGE_WORDS];
    if (close(fd) != 0 || !read_live_page(pid.integer, words)) return -1;
    if (gone && process_proven_ended(pid)) return 1;
    return 2;
}

static Value offer_kernel_page_ended(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return integer_value(kernel_page_state(arguments[0]));
}

static Value offer_kernel_page_bury(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value pid = arguments[0];
    if (kernel_page_state(pid) != 1) return integer_value(0);
    char name[LIVE_NAME_ROOM];
    if (!live_page_name(name, sizeof name, pid.integer)) return integer_value(0);
    int first = shm_open(name, O_RDONLY, GIFT_OPEN_MODE);
    if (first < 0) return integer_value(0);
    struct stat original, current;
    int observed = fstat(first, &original) == 0 && kernel_page_state(pid) == 1;
    int latest = observed ? shm_open(name, O_RDONLY, GIFT_OPEN_MODE) : -1;
    int same = latest >= 0 && fstat(latest, &current) == 0 && original.st_dev == current.st_dev && original.st_ino == current.st_ino;
    int buried = same && process_proven_ended(pid) && shm_unlink(name) == 0;
    int released_latest = latest < 0 || close(latest) == 0;
    int released_first = close(first) == 0;
    if (buried) stage_lastbreath(pid.integer);
    return integer_value(buried && released_latest && released_first);
}

/* The carrier owns GPU objects; this bootstrap bridge only transports Values.
 * Form's metal-carrier.bml / metal-jit.bml own admission and execution policy. */
static struct {
    void *handle;
    long long (*matvec_fixture)(char *, long long);
    long long (*matvec_report)(char *, long long);
    long long (*matvec_f32)(const char *, long long, const char *, long long, const char *, long long, char *, long long);
    long long (*gpu_utilization)(void);
    long long (*disk_stat)(long long *);
    long long (*pipeline)(const char *, long long, const char *, long long);
    long long (*buf_alloc)(long long);
    long long (*buf_from_file)(const char *, long long, long long, long long);
    long long (*buf_write)(long long, long long, const char *, long long);
    long long (*buf_fill)(long long, long long, const char *, long long, long long, long long);
    long long (*enqueue)(long long, const char *, long long, long long);
    long long (*sync)(void);
    long long (*buf_read)(long long, long long, long long, char *, long long);
    long long (*status)(char *, long long);
    long long (*batch_concurrent)(void);
    long long (*buf_free)(long long);
    long long (*submit)(void);
    long long (*live)(long long *);
    long long (*fence_wait)(long long);
    long long (*deadline)(long long);
    long long (*audio_available)(void);
    long long (*audio_stream_start)(long long);
    long long (*audio_stream_held)(void);
    long long (*audio_stream_stop)(void);
    const char *(*audio_stream_read)(long long, long long, long long *);
    const char *(*audio_capture)(long long, long long *);
    const char *(*audio_loopback)(const char *, long long, long long, long long, long long, long long *, long long *, long long *);
    long long (*audio_play)(const char *, long long, long long, long long);
    long long (*audio_output_start)(long long, long long);
    long long (*audio_output_held)(void);
    long long (*audio_output_write)(const char *, long long, long long);
    long long (*audio_output_stop)(long long);
} metal;

static int metal_ready(void) {
    if (metal.handle != NULL) return 1;
    const char *selected = getenv("FKWU_METAL_CARRIER");
    char *path = NULL;
    if (selected == NULL || *selected == 0) {
        char *executable = executing_image_path();
        if (executable == NULL) return 0;
        char *slash = strrchr(executable, '/');
        size_t prefix = slash == NULL ? 0 : (size_t)(slash - executable) + 1;
        const char *suffix = "form/native/metal/fk-metal-carrier.dylib";
        path = allocate_zeroed(checked_add(checked_add(prefix, strlen(suffix)), 1), 1);
        memcpy(path, executable, prefix);
        memcpy(path + prefix, suffix, strlen(suffix));
        free(executable);
        selected = path;
    }
    void *handle = dlopen(selected, RTLD_NOW | RTLD_LOCAL);
    free(path);
    if (handle == NULL) return 0;
#define LOAD_METAL(name) do { metal.name = (__typeof__(metal.name))dlsym(handle, "fk_metal_" #name "_external"); if (metal.name == NULL) { dlclose(handle); return 0; } } while (0)
    LOAD_METAL(pipeline); LOAD_METAL(buf_alloc); LOAD_METAL(buf_from_file);
    LOAD_METAL(buf_write); LOAD_METAL(enqueue); LOAD_METAL(sync); LOAD_METAL(buf_read);
    LOAD_METAL(status); LOAD_METAL(batch_concurrent); LOAD_METAL(buf_free);
    LOAD_METAL(submit); LOAD_METAL(live); LOAD_METAL(fence_wait); LOAD_METAL(deadline);
#undef LOAD_METAL
    metal.buf_fill = (__typeof__(metal.buf_fill))dlsym(handle, "fk_metal_buf_fill_external");
    metal.matvec_fixture = (__typeof__(metal.matvec_fixture))dlsym(handle, "fk_metal_matvec_fixture_external");
    metal.matvec_report = (__typeof__(metal.matvec_report))dlsym(handle, "fk_metal_matvec_report_external");
    metal.matvec_f32 = (__typeof__(metal.matvec_f32))dlsym(handle, "fk_metal_matvec_f32_external");
    metal.gpu_utilization = (__typeof__(metal.gpu_utilization))dlsym(handle, "fk_host_gpu_utilization");
    metal.disk_stat = (__typeof__(metal.disk_stat))dlsym(handle, "fk_host_disk_stat");
#define LOAD_AUDIO(field, name) metal.field = (__typeof__(metal.field))dlsym(handle, "fk_audio_" name "_external")
    LOAD_AUDIO(audio_available,"available"); LOAD_AUDIO(audio_stream_start,"stream_start");
    LOAD_AUDIO(audio_stream_held,"stream_held");
    LOAD_AUDIO(audio_stream_stop,"stream_stop"); LOAD_AUDIO(audio_stream_read,"stream_read");
    LOAD_AUDIO(audio_capture,"capture"); LOAD_AUDIO(audio_play,"play");
    LOAD_AUDIO(audio_loopback,"loopback");
    LOAD_AUDIO(audio_output_start,"output_start"); LOAD_AUDIO(audio_output_write,"output_write");
    LOAD_AUDIO(audio_output_held,"output_held");
    LOAD_AUDIO(audio_output_stop,"output_stop");
#undef LOAD_AUDIO
    metal.handle = handle;
    return 1;
}

static Value metal_count(long long count) { return integer_value(count < 0 ? 0 : count); }

/* AudioQueue lifetime stays in the optional carrier. Form owns PCM statistics,
 * WAV framing and reports; this bridge only copies completed physical readings.
 * Mode 59 is raw private capture for that Form owner, never retained to disk. */
static long long audio_wait(long long requested) {
    if (active_cell == NULL || active_cell->deadline == 0) return requested;
    uint64_t now = monotonic_ns() / 1000000;
    long long left = now >= active_cell->deadline ? 0 : (long long)(active_cell->deadline - now);
    return requested < 0 || left < requested ? left : requested;
}
static int audio_expired(void) { return active_cell != NULL && active_cell->deadline != 0 && monotonic_ns() / 1000000 >= active_cell->deadline; }
static void audio_deadline(void) {
    if (audio_expired())
        cell_stop("deadline", "cell execution deadline reached while awaiting audio");
}
static int audio_available(void) { return metal_ready() && metal.audio_available != NULL && metal.audio_available() > 0; }
static Value offer_mic_count(Value *a, size_t n) { (void)a; (void)n; return integer_value(audio_available()); }
static Value offer_mic_name(Value *a, size_t n) {
    (void)n; const char *name = a[0].kind == KIND_INTEGER && a[0].integer == 0 && audio_available() ? "coreaudio-default" : "";
    return text_value(name, strlen(name));
}
static Value offer_mic_health(Value *a, size_t n) {
    (void)n; return integer_value(a[0].kind == KIND_INTEGER && a[0].integer == 0 && audio_available() ? 1 : -1);
}
static Value offer_mic_start(Value *a, size_t n) {
    (void)a; (void)n; audio_deadline();
    if (!metal_ready() || metal.audio_stream_start == NULL || metal.audio_stream_stop == NULL || metal.audio_stream_held == NULL) return integer_value(-1);
    int already = metal.audio_stream_held() != 0;
    long long status = metal.audio_stream_start(audio_wait(-1));
    if (!already && status == 0 && audio_expired()) {
        long long release = metal.audio_stream_stop();
        if (release != 0 && metal.audio_stream_held()) fprintf(runtime_error(),"audio: timed-out input admission retains incomplete release\n");
    }
    audio_deadline(); return integer_value(status);
}
static Value offer_mic_stop(Value *a, size_t n) {
    (void)a; (void)n;
    long long status = metal_ready() && metal.audio_stream_stop != NULL ? metal.audio_stream_stop() : 0;
    return integer_value(status);
}
static Value offer_mic_read(Value *a, size_t n) {
    (void)n;
    if (a[0].kind != KIND_INTEGER || a[0].integer < 0 || a[1].kind != KIND_INTEGER || a[1].integer < 0) return nothing_value();
    if (!metal_ready() || metal.audio_stream_read == NULL) return text_value("", 0);
    long long length = -1;
    const char *bytes = metal.audio_stream_read(a[0].integer, audio_wait(a[1].integer), &length);
    Value result = bytes == NULL || length < 0 ? nothing_value() : text_value(bytes, (size_t)length);
    audio_deadline(); return result;
}
static Value audio_capture_bytes(Value milliseconds) {
    if (milliseconds.kind == KIND_LIST && milliseconds.item_count == 3 && value_proper_list(milliseconds)) {
        Value pcm = list_at(milliseconds,0), rate = list_at(milliseconds,1), tail = list_at(milliseconds,2);
        if (pcm.kind != KIND_TEXT || pcm.text_length%2 != 0 || rate.kind != KIND_INTEGER || rate.integer<4000 || rate.integer>192000 || tail.kind != KIND_INTEGER || tail.integer<0) return nothing_value();
        audio_deadline();
        if (!metal_ready() || metal.audio_loopback == NULL) return nothing_value();
        long long played=-1, captured=-1, released=0;
        const char *bytes=metal.audio_loopback(pcm.text,(long long)pcm.text_length,rate.integer,tail.integer,audio_wait(-1),&played,&captured,&released);
        Value fields[]={integer_value(played),bytes == NULL || captured<0 ? nothing_value() : text_value(bytes,(size_t)captured)};
        Value result=played<0 || !released ? nothing_value() : list_from(fields,2);
        if (!released) fprintf(runtime_error(),"audio: loopback played %lld samples; private input release incomplete\n",played);
        audio_deadline(); return result;
    }
    if (milliseconds.kind != KIND_INTEGER || milliseconds.integer < 0) return nothing_value();
    if (milliseconds.integer == 0) return text_value("", 0);
    audio_deadline();
    if (!metal_ready() || metal.audio_capture == NULL) return nothing_value();
    long long length = -1;
    const char *bytes = metal.audio_capture(audio_wait(milliseconds.integer), &length);
    Value result = bytes == NULL || length < 0 ? nothing_value() : text_value(bytes, (size_t)length);
    // The private capture has settled its queue before the evaluator may unwind.
    audio_deadline(); return result;
}
static Value audio_speaker_leaf(long long mode, Value value) {
    if (mode == 10) return integer_value(audio_available());
    if (mode == 11) return offer_mic_name(&value, 1);
    if (mode == 12) return offer_mic_health(&value, 1);
    if (mode == 16) {
        long long status = metal_ready() && metal.audio_output_stop != NULL ? metal.audio_output_stop(audio_wait(-1)) : 0;
        audio_deadline(); return integer_value(status);
    }
    if (mode == 15) {
        if (value.kind != KIND_TEXT || value.text_length % 2 != 0) return integer_value(-2);
        if (value.text_length == 0) return integer_value(0);
        audio_deadline();
        long long status = metal_ready() && metal.audio_output_write != NULL ? metal.audio_output_write(value.text, (long long)value.text_length, audio_wait(-1)) : -1;
        audio_deadline(); return integer_value(status);
    }
    Value pcm = nothing_value(), rate = value;
    if (mode == 13) {
        if (value.kind != KIND_LIST || value.list == NULL) return integer_value(-2);
        pcm = offer_head(&value, 1); rate = offer_tail(&value, 1);
    }
    if (rate.kind != KIND_INTEGER || rate.integer < 4000 || rate.integer > 192000) return integer_value(-3);
    if (mode == 13 && (pcm.kind != KIND_TEXT || pcm.text_length % 2 != 0)) return integer_value(-2);
    if (mode == 13 && pcm.text_length == 0) return integer_value(0);
    audio_deadline();
    if (!metal_ready()) return integer_value(-1);
    int already = mode == 14 && metal.audio_output_held != NULL ? metal.audio_output_held() != 0 : 0;
    if (mode == 14 && (metal.audio_output_held == NULL || metal.audio_output_stop == NULL)) return integer_value(-1);
    long long status = mode == 14 ? (metal.audio_output_start == NULL ? -1 : metal.audio_output_start(rate.integer,audio_wait(-1))) :
        (metal.audio_play == NULL ? -1 : metal.audio_play(pcm.text, (long long)pcm.text_length, rate.integer, audio_wait(-1)));
    if (mode == 14 && !already && status == 0 && audio_expired()) {
        long long release = metal.audio_output_stop(0);
        if (release != 0 && metal.audio_output_held()) fprintf(runtime_error(),"audio: timed-out output admission retains incomplete release\n");
    }
    audio_deadline(); return integer_value(status);
}

static long long host_gpu_last_us, host_gpu_integrated_us;
static long long host_gpu_last_level = -1;
static Value offer_host_gpu_utilization(Value *a, size_t n) {
    (void)a; (void)n;
    long long now = (long long)(monotonic_ns() / 1000);
    // This historical reading integrates sampled machine utilization; it is
    // not a Metal command-buffer timer or a per-process inference measurement.
    if (host_gpu_last_us == 0 || now - host_gpu_last_us >= 50000) {
        host_gpu_last_level = metal_ready() && metal.gpu_utilization != NULL ? metal.gpu_utilization() : -1;
        if (host_gpu_last_us != 0 && now > host_gpu_last_us && host_gpu_last_level >= 0) {
            host_gpu_integrated_us += (now - host_gpu_last_us) * host_gpu_last_level / 100;
        }
        host_gpu_last_us = now;
    }
    return integer_value(host_gpu_last_level);
}

static Value offer_host_gpu_busy_us(Value *a, size_t n) {
    offer_host_gpu_utilization(a, n);
    return integer_value(host_gpu_integrated_us);
}

static Value offer_host_cpu_busy_us(Value *a, size_t n) {
    (void)a; (void)n;
#ifdef __APPLE__
    host_cpu_load_info_data_t reading;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    mach_port_t host = mach_host_self();
    kern_return_t result = host_statistics(host, HOST_CPU_LOAD_INFO, (host_info_t)&reading, &count);
    mach_port_deallocate(mach_task_self(), host);
    long ticks = sysconf(_SC_CLK_TCK);
    if (result != KERN_SUCCESS || ticks <= 0) return integer_value(-1);
    unsigned long long busy = (unsigned long long)reading.cpu_ticks[CPU_STATE_USER] + reading.cpu_ticks[CPU_STATE_SYSTEM] + reading.cpu_ticks[CPU_STATE_NICE];
    return integer_value((long long)(busy * 1000000 / (unsigned long long)ticks));
#else
    return integer_value(-1);
#endif
}

static Value offer_host_load_avg(Value *a, size_t n) {
    (void)a; (void)n;
    double loads[3];
    if (getloadavg(loads, 3) != 3) return list_from(NULL, 0);
    Value values[3];
    for (size_t i = 0; i < 3; ++i) values[i] = integer_value((long long)(loads[i] * 1000));
    return list_from(values, 3);
}

static Value offer_host_volume(Value *a, size_t n) {
    (void)n;
    if (a[0].kind != KIND_TEXT || memchr(a[0].text, 0, a[0].text_length) != NULL) return nothing_value();
    struct statvfs reading;
    if (statvfs(a[0].text, &reading) != 0) return nothing_value();
    unsigned long long scale = reading.f_frsize, blocks[3] = {reading.f_blocks, reading.f_bfree, reading.f_bavail};
    if (scale == 0) return nothing_value();
    Value values[3];
    for (size_t i = 0; i < 3; ++i) {
        if (blocks[i] > (unsigned long long)LLONG_MAX / scale) return nothing_value();
        values[i] = integer_value((long long)(blocks[i] * scale));
    }
    return list_from(values, 3);
}

static Value offer_host_disk_stat(Value *a, size_t n) {
    (void)a; (void)n;
    long long counters[4] = {0};
    if (!metal_ready() || metal.disk_stat == NULL || metal.disk_stat(counters) < 0) return list_from(NULL, 0);
    Value values[4];
    for (size_t i = 0; i < 4; ++i) values[i] = integer_value(counters[i]);
    return list_from(values, 4);
}

static Value offer_metal_matvec(Value *a, size_t n) {
    const int fixture = n == 0;
    const char *failure = fixture ? "FAIL fkwu-form-cli-metal-direct carrier result\n" : "FAIL fkwu-form-cli-metal-matvec-f32 carrier result\n";
    if (!fixture && (a[0].kind != KIND_TEXT || a[1].kind != KIND_TEXT || a[2].kind != KIND_TEXT)) {
        const char *invalid = "FAIL fkwu-form-cli-metal-matvec-f32 invalid string input\n";
        return text_value(invalid, strlen(invalid));
    }
    if (!metal_ready()) {
        const char *unavailable = "SKIP fkwu-form-cli-metal: dynamic carrier unavailable\nmetal_owner=fkwu-form-cli\nmetal_carrier=dynamic\nmetal_loaded=false\n";
        return text_value(unavailable, strlen(unavailable));
    }
    if (fixture ? metal.matvec_fixture == NULL : metal.matvec_f32 == NULL) {
        const char *missing = "FAIL fkwu-form-cli-metal: loaded carrier lacks matvec capability\nmetal_loaded=true\n";
        return text_value(missing, strlen(missing));
    }
    // Execute once. A newer carrier retains its complete report, so a larger
    // readback preserves completed effects instead of repeating GPU work.
    size_t capacity = 4096;
    char *bytes = allocate_zeroed(capacity, 1);
    long long got = fixture ? metal.matvec_fixture(bytes, (long long)capacity)
        : metal.matvec_f32(a[0].text, (long long)a[0].text_length, a[1].text, (long long)a[1].text_length,
            a[2].text, (long long)a[2].text_length, bytes, (long long)capacity);
    if (got < 0 || (unsigned long long)got > capacity) { free(bytes); return text_value(failure, strlen(failure)); }
    if (metal.matvec_report != NULL) {
        long long required = metal.matvec_report(NULL, 0);
        if (required < got || required < 0 || (unsigned long long)required > SIZE_MAX) { free(bytes); return text_value(failure, strlen(failure)); }
        if ((unsigned long long)required > capacity) {
            free(bytes); capacity = (size_t)required; bytes = allocate_zeroed(capacity, 1);
        }
        if (metal.matvec_report(bytes, required) != required) { free(bytes); return text_value(failure, strlen(failure)); }
        got = required;
    } else if ((size_t)got == capacity) {
        free(bytes);
        const char *truncated = "FAIL fkwu-form-cli-metal: completed report exceeded legacy carrier buffer; rebuild carrier\n";
        return text_value(truncated, strlen(truncated));
    }
    Value result = text_value(bytes, (size_t)got); free(bytes); return result;
}

static Value offer_metal_pipeline(Value *a, size_t n) {
    (void)n;
    if (a[0].kind != KIND_TEXT || a[1].kind != KIND_TEXT || !metal_ready()) return integer_value(0);
    return metal_count(metal.pipeline(a[0].text, (long long)a[0].text_length, a[1].text, (long long)a[1].text_length));
}

/* The physical leaf may receive [payload, record, list-key]. Form owns that
 * list and every release decision. Reserve its link before allocating: the
 * next evaluator deadline must never see an unowned physical handle. This
 * bridge leaves with the corresponding alloc/map/free leaves as the seed shrinks. */
typedef struct { Record *owner; size_t slot; Value handles; List *link; } MetalRetention;
static int metal_retention(Value *payload, MetalRetention *held) {
    memset(held, 0, sizeof *held);
    if (payload->kind != KIND_LIST) return 1;
    if (payload->item_count != 3) return 0;
    Value owner = list_at(*payload, 1), key = list_at(*payload, 2);
    if (owner.kind != KIND_RECORD || !record_find(owner.record, key, &held->slot)) return 0;
    held->handles = owner.record->values[held->slot];
    if (held->handles.kind != KIND_LIST) return 0;
    for (List *item = held->handles.list; item != NULL; item = item->tail) if (item->dotted_tail != NULL) return 0;
    Value pair[] = {integer_value(0), held->handles};
    held->handles = offer_cons(pair, 2); held->link = held->handles.list;
    held->owner = owner.record; *payload = list_at(*payload, 0); return 1;
}
static Value metal_retained(MetalRetention *held, long long handle) {
    Value result = metal_count(handle);
    if (handle > 0 && held->owner != NULL) {
        held->link->head = result; held->handles.list = held->link;
        held->owner->values[held->slot] = held->handles;
    }
    return result;
}
static Value offer_metal_buf_alloc(Value *a, size_t n) {
    (void)n; Value bytes = a[0]; MetalRetention held;
    if (!metal_retention(&bytes, &held) || bytes.kind != KIND_INTEGER || !metal_ready()) return integer_value(0);
    return metal_retained(&held, metal.buf_alloc(bytes.integer));
}

static Value offer_metal_buf_from_file(Value *a, size_t n) {
    (void)n; Value path = a[0]; MetalRetention held;
    if (!metal_retention(&path, &held) || path.kind != KIND_TEXT || a[1].kind != KIND_INTEGER || a[2].kind != KIND_INTEGER || !metal_ready()) return integer_value(0);
    return metal_retained(&held, metal.buf_from_file(path.text, (long long)path.text_length, a[1].integer, a[2].integer));
}

static Value offer_metal_buf_write(Value *a, size_t n) {
    (void)n;
    if (a[0].kind != KIND_INTEGER || a[1].kind != KIND_INTEGER) return integer_value(0);
    char *packed = NULL;
    const char *bytes;
    size_t length;
    if (a[2].kind == KIND_TEXT) { bytes = a[2].text; length = a[2].text_length; }
    else if (a[2].kind == KIND_LIST) {
        if (a[2].item_count > SIZE_MAX / sizeof(uint32_t)) return integer_value(0);
        length = a[2].item_count * sizeof(uint32_t);
        packed = allocate_zeroed(length, 1);
        for (size_t i = 0; i < a[2].item_count; ++i) {
            Value value = list_at(a[2], i);
            uint32_t word;
            if (value.kind == KIND_INTEGER) word = (uint32_t)value.integer;
            else if (value.kind == KIND_REAL) { float number = (float)value.real; memcpy(&word, &number, sizeof word); }
            else { free(packed); return integer_value(0); }
            for (size_t byte = 0; byte < sizeof word; ++byte) packed[i * sizeof word + byte] = (char)(word >> (8 * byte));
        }
        bytes = packed;
    } else return integer_value(0);
    long long written = metal_ready() ? metal.buf_write(a[0].integer, a[1].integer, bytes, (long long)length) : 0;
    free(packed);
    return metal_count(written);
}

static Value offer_metal_buf_fill(Value *a, size_t n) {
    (void)n;
    if (a[0].kind != KIND_INTEGER || a[1].kind != KIND_INTEGER || a[2].kind != KIND_LIST || a[2].item_count != 3 || !metal_ready() || metal.buf_fill == NULL) return integer_value(-1);
    Value path = list_at(a[2], 0), offset = list_at(a[2], 1), length = list_at(a[2], 2);
    if (path.kind != KIND_TEXT || offset.kind != KIND_INTEGER || length.kind != KIND_INTEGER) return integer_value(-1);
    return integer_value(metal.buf_fill(a[0].integer, a[1].integer, path.text, (long long)path.text_length, offset.integer, length.integer));
}

static Value offer_metal_enqueue(Value *a, size_t n) {
    (void)n;
    if (a[0].kind != KIND_INTEGER || a[1].kind != KIND_TEXT || a[2].kind != KIND_INTEGER || !metal_ready()) return integer_value(0);
    return metal_count(metal.enqueue(a[0].integer, a[1].text, (long long)a[1].text_length, a[2].integer));
}

static Value offer_metal_buf_read(Value *a, size_t n) {
    (void)n;
    if (a[0].kind != KIND_INTEGER || a[1].kind != KIND_INTEGER || a[2].kind != KIND_INTEGER || a[2].integer < 0 || !metal_ready()) return text_value("", 0);
    size_t length = (size_t)a[2].integer;
    char *bytes = allocate_zeroed(checked_add(length, 1), 1);
    long long got = metal.buf_read(a[0].integer, a[1].integer, a[2].integer, bytes, a[2].integer);
    Value result = got == a[2].integer ? text_value(bytes, length) : text_value("", 0);
    free(bytes);
    return result;
}

static Value offer_metal_sync(Value *a, size_t n) { (void)a; (void)n; return metal_ready() ? metal_count(metal.sync()) : integer_value(0); }
static Value offer_metal_batch_concurrent(Value *a, size_t n) { (void)a; (void)n; return metal_ready() ? metal_count(metal.batch_concurrent()) : integer_value(0); }
static Value offer_metal_submit(Value *a, size_t n) { (void)a; (void)n; return metal_ready() ? metal_count(metal.submit()) : integer_value(0); }
static Value offer_metal_buf_free(Value *a, size_t n) {
    (void)n; Value handle = a[0], remaining = {0}; Record *owner = NULL; size_t slot = 0;
    if (handle.kind == KIND_LIST) {
        if (handle.item_count != 3) return integer_value(0);
        Value record = list_at(handle, 1), key = list_at(handle, 2);
        handle = list_at(handle, 0);
        if (handle.kind != KIND_INTEGER || record.kind != KIND_RECORD || !record_find(record.record, key, &slot)) return integer_value(0);
        Value held = record.record->values[slot];
        if (held.kind != KIND_LIST) return integer_value(0);
        Value *items = allocate_zeroed(held.item_count, sizeof(Value)); size_t count = 0; int found = 0;
        for (List *item = held.list; item != NULL; item = item->tail) {
            if (item->dotted_tail != NULL) { free(items); return integer_value(0); }
            if (item->head.kind == KIND_INTEGER && item->head.integer == handle.integer) found = 1;
            else items[count++] = item->head;
        }
        if (!found) { free(items); return integer_value(0); }
        remaining = list_from(items, count); free(items); owner = record.record;
    }
    if (handle.kind != KIND_INTEGER || !metal_ready()) return integer_value(0);
    long long freed = metal.buf_free(handle.integer);
    /* The release and its Form ownership change precede the next deadline poll. */
    if (freed == 1 && owner != NULL) owner->values[slot] = remaining;
    return metal_count(freed);
}
static Value offer_metal_fence_wait(Value *a, size_t n) { (void)n; return a[0].kind != KIND_INTEGER ? integer_value(-1) : metal_ready() ? integer_value(metal.fence_wait(a[0].integer)) : integer_value(0); }
static Value offer_metal_deadline(Value *a, size_t n) { (void)n; return a[0].kind == KIND_INTEGER && metal_ready() ? integer_value(metal.deadline(a[0].integer)) : integer_value(-2); }

static Value offer_metal_status(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    if (!metal_ready()) return text_value(METAL_ABSENT_STATUS, strlen(METAL_ABSENT_STATUS));
    size_t capacity = host_page_bytes();
    for (;;) {
        char *bytes = allocate_zeroed(capacity, 1);
        long long got = metal.status(bytes, (long long)capacity);
        if (got >= 0 && (size_t)got < capacity) {
            Value result = text_value(bytes, (size_t)got);
            free(bytes); return result;
        }
        free(bytes);
        if (got < 0) return text_value("", 0);
        capacity = checked_add(capacity, capacity);
    }
}

static Value offer_metal_live(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    if (!metal_ready()) return list_from(NULL, 0);
    long long readings[20] = {0};
    if (metal.live(readings) <= 0) return list_from(NULL, 0);
    Value values[20];
    for (size_t i = 0; i < 20; ++i) values[i] = integer_value(readings[i]);
    return list_from(values, 20);
}

static Value offer_list(Value *arguments, size_t argument_count) {
    return list_from(arguments, argument_count);
}

static int list_to_bytes(Value list, unsigned char **bytes, size_t *length) {
    if (list.kind == KIND_NOTHING) { *bytes = NULL; *length = 0; return 1; }
    if (list.kind != KIND_LIST) return 0;
    *length = list.item_count;
    *bytes = allocate_zeroed(*length == 0 ? 1 : *length, 1);
    List *cursor = list.list;
    for (size_t i = 0; i < *length; ++i) {
        if (cursor == NULL || cursor->head.kind != KIND_INTEGER || cursor->head.integer < 0 || cursor->head.integer > 255) {
            free(*bytes); *bytes = NULL; return 0;
        }
        (*bytes)[i] = (unsigned char)cursor->head.integer;
        cursor = cursor->tail;
    }
    return 1;
}

static int socket_put(int descriptor) {
    size_t needed = checked_add(socket_descriptor_count, ONE_ELEMENT);
    socket_descriptors = grow_list(socket_descriptors, &socket_descriptor_capacity, sizeof *socket_descriptors, needed);
    socket_descriptors[socket_descriptor_count] = (SocketHandle){descriptor, 0};
    socket_descriptor_count = needed;
    return (int)socket_descriptor_count;
}

static int socket_fetch(long long handle) {
    if (handle <= WORD_FALSE) return INVALID_DESCRIPTOR;
    if ((unsigned long long)handle > socket_descriptor_count) return INVALID_DESCRIPTOR;
    int descriptor = socket_descriptors[handle - ONE_ELEMENT].descriptor;
    return descriptor;
}

static int socket_release(long long handle) {
    if (handle <= WORD_FALSE) return 0;
    if ((unsigned long long)handle > socket_descriptor_count) return 0;
    socket_descriptors[handle - ONE_ELEMENT] = (SocketHandle){INVALID_DESCRIPTOR, 0};
    return 1;
}
static uint64_t socket_deadline(uint64_t offered);
static int socket_wait(int descriptor, short events, uint64_t deadline);
static Value offer_socket_listen(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "socket_listen expects a port");
    if (arguments[0].integer < 0 || arguments[0].integer > 65535) return integer_value(-1);
    int descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    int flags = fcntl(descriptor, F_GETFL, 0);
    if (flags < 0 || fcntl(descriptor, F_SETFL, flags | O_NONBLOCK) < 0) { close(descriptor); return integer_value(-1); }
    int yes = 1;
    setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
    struct sockaddr_in address;
    memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons((uint16_t)arguments[SOLE_OPERAND].integer);
    if (bind(descriptor, (struct sockaddr *)&address, sizeof address) != SYSTEM_CALL_OK
        || listen(descriptor, 16) != SYSTEM_CALL_OK) {
        close(descriptor);
        return integer_value(-WORD_TRUE);
    }
    return integer_value((long long)socket_put(descriptor));
}

static Value offer_socket_port(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "socket_port expects a handle");
    int descriptor = socket_fetch(arguments[SOLE_OPERAND].integer);
    if (descriptor < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    struct sockaddr_in address;
    socklen_t length = sizeof address;
    if (getsockname(descriptor, (struct sockaddr *)&address, &length) != SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    return integer_value((long long)ntohs(address.sin_port));
}

static Value offer_socket_accept(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "socket_accept expects a handle");
    int descriptor = socket_fetch(arguments[SOLE_OPERAND].integer);
    if (descriptor < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    uint64_t deadline = socket_deadline(socket_descriptors[arguments[0].integer - 1].deadline);
    int accepted;
    for (;;) {
        if (!socket_wait(descriptor, POLLIN, deadline)) return integer_value(-1);
        accepted = accept(descriptor, NULL, NULL);
        if (accepted >= 0) break;
        if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) return integer_value(-1);
    }
    int flags = fcntl(accepted, F_GETFL, 0);
    if (flags < 0 || fcntl(accepted, F_SETFL, flags | O_NONBLOCK) < 0) { close(accepted); return integer_value(-1); }
    int handle = socket_put(accepted); socket_descriptors[handle - 1].deadline = socket_descriptors[arguments[0].integer - 1].deadline; return integer_value(handle);
}

static void socket_quiet(int descriptor) {
#ifdef SO_NOSIGPIPE
    int yes = 1;
    setsockopt(descriptor, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof yes);
#else
    (void)descriptor;
#endif
}

static uint64_t socket_deadline(uint64_t offered) {
    if (active_cell != NULL && active_cell->deadline != 0 && (offered == 0 || active_cell->deadline < offered)) return active_cell->deadline;
    return offered;
}

static int socket_wait(int descriptor, short events, uint64_t deadline) {
    for (;;) {
        uint64_t now = monotonic_ns() / 1000000;
        if (deadline != 0 && now >= deadline) { errno = ETIMEDOUT; return 0; }
        uint64_t left = deadline == 0 ? 0 : deadline - now;
        int timeout = deadline == 0 ? -1 : left > INT_MAX ? INT_MAX : (int)left;
        struct pollfd waiting = {descriptor, events, 0};
        int result = poll(&waiting, 1, timeout);
        if (result > 0) return !(waiting.revents & POLLNVAL);
        if (result == 0) { errno = ETIMEDOUT; return 0; }
        if (errno != EINTR) return 0;
    }
}

/* The waiting caller may leave DNS on its deadline. The detached resolver
 * then owns and releases the copied input/result; it never enters Form. */
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t changed;
    char *host, *port;
    struct addrinfo *addresses;
    int done, abandoned, error;
} SocketResolution;

static void socket_resolution_free(SocketResolution *job) {
    if (job->addresses != NULL) freeaddrinfo(job->addresses);
    free(job->host); free(job->port);
    pthread_cond_destroy(&job->changed); pthread_mutex_destroy(&job->mutex); free(job);
}

static void *socket_resolve_worker(void *opaque) {
    SocketResolution *job = opaque;
    struct addrinfo hints = {0}; hints.ai_family = AF_UNSPEC; hints.ai_socktype = SOCK_STREAM;
    job->error = getaddrinfo(job->host, job->port, &hints, &job->addresses);
    pthread_mutex_lock(&job->mutex);
    if (job->abandoned) { pthread_mutex_unlock(&job->mutex); socket_resolution_free(job); return NULL; }
    job->done = 1; pthread_cond_signal(&job->changed); pthread_mutex_unlock(&job->mutex); return NULL;
}

static struct addrinfo *socket_resolve(const char *host, const char *port, uint64_t deadline) {
    if (deadline == 0) {
        struct addrinfo hints = {0}, *addresses = NULL;
        hints.ai_family = AF_UNSPEC; hints.ai_socktype = SOCK_STREAM;
        return getaddrinfo(host, port, &hints, &addresses) == 0 ? addresses : NULL;
    }
    if (monotonic_ns() / 1000000 >= deadline) { errno = ETIMEDOUT; return NULL; }
    SocketResolution *job = calloc(1, sizeof *job);
    if (job == NULL) return NULL;
    job->host = strdup(host); job->port = strdup(port);
    if (job->host == NULL || job->port == NULL) { free(job->host); free(job->port); free(job); return NULL; }
    if (pthread_mutex_init(&job->mutex, NULL) != 0) { free(job->host); free(job->port); free(job); return NULL; }
    pthread_condattr_t attributes; pthread_condattr_init(&attributes);
#ifdef __linux__
    pthread_condattr_setclock(&attributes, CLOCK_MONOTONIC);
#endif
    int initialized = pthread_cond_init(&job->changed, &attributes); pthread_condattr_destroy(&attributes);
    if (initialized != 0) { pthread_mutex_destroy(&job->mutex); free(job->host); free(job->port); free(job); return NULL; }
    pthread_attr_t detached; pthread_attr_init(&detached); pthread_attr_setdetachstate(&detached, PTHREAD_CREATE_DETACHED);
    pthread_t worker; int started = pthread_create(&worker, &detached, socket_resolve_worker, job); pthread_attr_destroy(&detached);
    if (started != 0) { socket_resolution_free(job); return NULL; }
    pthread_mutex_lock(&job->mutex);
    while (!job->done) {
        uint64_t now = monotonic_ns() / 1000000;
        if (now >= deadline) { job->abandoned = 1; pthread_mutex_unlock(&job->mutex); errno = ETIMEDOUT; return NULL; }
#ifdef __APPLE__
        uint64_t left = deadline - now;
        struct timespec until = {(time_t)(left / 1000), (long)(left % 1000) * 1000000};
        int result = pthread_cond_timedwait_relative_np(&job->changed, &job->mutex, &until);
#else
        struct timespec until = {(time_t)(deadline / 1000), (long)(deadline % 1000) * 1000000};
        int result = pthread_cond_timedwait(&job->changed, &job->mutex, &until);
#endif
        if (result != 0 && result != ETIMEDOUT && result != EINTR) { job->abandoned = 1; pthread_mutex_unlock(&job->mutex); return NULL; }
    }
    struct addrinfo *addresses = job->error == 0 ? job->addresses : NULL;
    if (addresses != NULL) job->addresses = NULL;
    pthread_mutex_unlock(&job->mutex); socket_resolution_free(job); return addresses;
}

static int socket_connect_descriptor(const char *host, const char *port, uint64_t deadline) {
    struct addrinfo *addresses = socket_resolve(host, port, deadline);
    if (addresses == NULL) return -1;
    int descriptor = -1;
    for (struct addrinfo *address = addresses; address != NULL; address = address->ai_next) {
        descriptor = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
        if (descriptor < 0) continue;
        socket_quiet(descriptor);
        int flags = fcntl(descriptor, F_GETFL, 0);
        int result = flags < 0 || fcntl(descriptor, F_SETFL, flags | O_NONBLOCK) < 0 ? -1 : connect(descriptor, address->ai_addr, address->ai_addrlen);
        if (result == 0) break;
        if ((errno == EINPROGRESS || errno == EINTR) && socket_wait(descriptor, POLLOUT, deadline)) {
            int error = 0; socklen_t size = sizeof error;
            if (getsockopt(descriptor, SOL_SOCKET, SO_ERROR, &error, &size) == 0 && error == 0) break;
        }
        close(descriptor); descriptor = -1;
    }
    freeaddrinfo(addresses);
    return descriptor;
}

static Value offer_socket_connect(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_TEXT, "socket_connect expects a host");
    Value offered = arguments[1], end = nothing_value();
    int request_owned = offered.kind == KIND_LIST && offered.item_count == 3 && list_at(offered, 2).kind == KIND_TEXT && bytes_equal_cstr(list_at(offered, 2).text, list_at(offered, 2).text_length, "request");
    if (offered.kind == KIND_LIST && (offered.item_count == 2 || request_owned)) { end = list_at(offered, 1); offered = list_at(offered, 0); }
    if (offered.kind != KIND_INTEGER || offered.integer < 0 || offered.integer > 65535 || (end.kind != KIND_NOTHING && (end.kind != KIND_INTEGER || end.integer < 0)) || memchr(arguments[0].text, 0, arguments[0].text_length) != NULL) return integer_value(-1);
    uint64_t deadline = socket_deadline(end.kind == KIND_INTEGER ? (uint64_t)end.integer : 0);
    char port[6]; snprintf(port, sizeof port, "%lld", offered.integer);
    int descriptor = socket_connect_descriptor(arguments[0].text, port, deadline);
    if (descriptor < 0) return integer_value(-1);
    int handle = socket_put(descriptor); socket_descriptors[handle - 1].deadline = end.kind == KIND_INTEGER ? (uint64_t)end.integer : 0;
    socket_descriptors[handle - 1].request_owner = request_owned ? active_cell : NULL; return integer_value(handle);
}

static ssize_t socket_send_bytes(int descriptor, const void *bytes, size_t length, uint64_t deadline) {
    socket_quiet(descriptor);
    int flags = 0;
#ifdef MSG_NOSIGNAL
    flags = MSG_NOSIGNAL;
#endif
    ssize_t sent;
    for (;;) {
        if (!socket_wait(descriptor, POLLOUT, deadline)) return -1;
        sent = send(descriptor, bytes, length, flags);
        if (sent >= 0 || (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)) break;
    }
    return sent;
}

static Value offer_socket_send(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_INTEGER, "socket_send expects a handle");
    int descriptor = socket_fetch(arguments[LEFT_OPERAND].integer);
    if (descriptor < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    uint64_t deadline = socket_deadline(socket_descriptors[arguments[0].integer - 1].deadline);
    if (arguments[RIGHT_OPERAND].kind == KIND_TEXT) {
        ssize_t sent = socket_send_bytes(descriptor, arguments[RIGHT_OPERAND].text, arguments[RIGHT_OPERAND].text_length, deadline);
        return integer_value(sent < SYSTEM_CALL_OK ? -WORD_TRUE : (long long)sent);
    }
    unsigned char *bytes = NULL;
    size_t length = EMPTY_COUNT;
    if (!list_to_bytes(arguments[RIGHT_OPERAND], &bytes, &length)) return integer_value(-WORD_TRUE);
    ssize_t sent = socket_send_bytes(descriptor, bytes, length, deadline);
    free(bytes);
    return integer_value(sent < SYSTEM_CALL_OK ? -WORD_TRUE : (long long)sent);
}

static Value offer_socket_recv(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_INTEGER, "socket_recv expects a handle");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "socket_recv expects a max byte count");
    int descriptor = socket_fetch(arguments[LEFT_OPERAND].integer);
    if (descriptor < SYSTEM_CALL_OK) return nothing_value();
    long long max = arguments[RIGHT_OPERAND].integer;
    if (max < WORD_FALSE) return nothing_value();
    size_t budget = (size_t)max;
    unsigned char *bytes = allocate_zeroed(budget == EMPTY_COUNT ? ONE_ELEMENT : budget, sizeof *bytes);
    ssize_t got;
    uint64_t deadline = socket_deadline(socket_descriptors[arguments[0].integer - 1].deadline);
    for (;;) {
        if (!socket_wait(descriptor, POLLIN, deadline)) { got = -1; break; }
        got = recv(descriptor, bytes, budget, 0);
        if (got >= 0 || (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)) break;
    }
    if (got < SYSTEM_CALL_OK) {
        free(bytes);
        return nothing_value();
    }
    Value value = text_value((char *)bytes, (size_t)got);
    free(bytes);
    return value;
}

static Value offer_socket_close(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "socket_close expects a handle");
    int descriptor = socket_fetch(arguments[SOLE_OPERAND].integer);
    if (descriptor < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    int closed = close(descriptor);
    socket_release(arguments[SOLE_OPERAND].integer);
    return integer_value(closed == SYSTEM_CALL_OK ? WORD_FALSE : -WORD_TRUE);
}

/* Optional TLS is a verified physical byte exchange. HTTP meaning stays in
 * Form. Each exchange settles SSL, context and descriptor before returning. */
static struct {
    void *library;
    const void *(*method)(void);
    void *(*ctx_new)(const void *);
    void (*ctx_free)(void *);
    void (*ctx_verify)(void *, int, void *);
    int (*ctx_paths)(void *);
    void *(*new_ssl)(void *);
    void (*free_ssl)(void *);
    int (*set_fd)(void *, int);
    long (*ctrl)(void *, int, long, void *);
    int (*set_host)(void *, const char *);
    int (*connect_ssl)(void *);
    int (*write_ssl)(void *, const void *, int);
    int (*read_ssl)(void *, void *, int);
    int (*error)(const void *, int);
    long (*verified)(const void *);
    int (*shutdown_ssl)(void *);
    void (*clear_error)(void);
} tls;

static int tls_ready(void) {
    if (tls.library != NULL) return 1;
    const char *paths[] = {"/opt/homebrew/opt/openssl@3/lib/libssl.3.dylib", "/opt/homebrew/opt/openssl@3/lib/libssl.dylib", "libssl.so.3", "libssl.dylib"};
    for (size_t i = 0; i < sizeof paths / sizeof *paths; ++i) {
        void *library = dlopen(paths[i], RTLD_NOW | RTLD_LOCAL);
        if (library == NULL) continue;
#define TLS_SYMBOL(field, name) do { tls.field = (__typeof__(tls.field))dlsym(library, name); if (tls.field == NULL) goto tls_missing; } while (0)
        TLS_SYMBOL(method, "TLS_client_method"); TLS_SYMBOL(ctx_new, "SSL_CTX_new"); TLS_SYMBOL(ctx_free, "SSL_CTX_free");
        TLS_SYMBOL(ctx_verify, "SSL_CTX_set_verify"); TLS_SYMBOL(ctx_paths, "SSL_CTX_set_default_verify_paths");
        TLS_SYMBOL(new_ssl, "SSL_new"); TLS_SYMBOL(free_ssl, "SSL_free"); TLS_SYMBOL(set_fd, "SSL_set_fd");
        TLS_SYMBOL(ctrl, "SSL_ctrl"); TLS_SYMBOL(set_host, "SSL_set1_host"); TLS_SYMBOL(connect_ssl, "SSL_connect");
        TLS_SYMBOL(write_ssl, "SSL_write"); TLS_SYMBOL(read_ssl, "SSL_read"); TLS_SYMBOL(error, "SSL_get_error");
        TLS_SYMBOL(verified, "SSL_get_verify_result"); TLS_SYMBOL(shutdown_ssl, "SSL_shutdown");
        TLS_SYMBOL(clear_error, "ERR_clear_error");
#undef TLS_SYMBOL
        tls.library = library; return 1;
tls_missing:
        dlclose(library);
    }
    return 0;
}

static int tls_retry(void *connection, int result, int saved_errno, int descriptor, uint64_t deadline) {
    int error = tls.error(connection, result);
    if (error == 2 || error == 3) return socket_wait(descriptor, error == 2 ? POLLIN : POLLOUT, deadline);
    return error == 5 && saved_errno == EINTR && (deadline == 0 || monotonic_ns() / 1000000 < deadline);
}

static Value offer_tls_request(Value *a, size_t n) {
    (void)n;
    Value port = a[1], end = nothing_value();
    if (port.kind == KIND_LIST && port.item_count == 2) { end = list_at(port, 1); port = list_at(port, 0); }
    if (a[0].kind != KIND_TEXT || port.kind != KIND_TEXT || a[2].kind != KIND_TEXT || (end.kind != KIND_NOTHING && (end.kind != KIND_INTEGER || end.integer < 0))) return nothing_value();
    if (a[0].text_length == 0 || port.text_length == 0 || memchr(a[0].text, 0, a[0].text_length) || memchr(port.text, 0, port.text_length)) return nothing_value();
    if (!tls_ready()) return nothing_value();
    uint64_t deadline = socket_deadline(end.kind == KIND_INTEGER ? (uint64_t)end.integer : 0);
    int descriptor = socket_connect_descriptor(a[0].text, port.text, deadline);
    if (descriptor < 0) return nothing_value();
#ifdef SO_NOSIGPIPE
    int quiet = 1;
    if (setsockopt(descriptor, SOL_SOCKET, SO_NOSIGPIPE, &quiet, sizeof quiet) != 0) { close(descriptor); return nothing_value(); }
#else
    sigset_t blocked, previous, pending;
    sigemptyset(&blocked); sigaddset(&blocked, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &blocked, &previous) != 0) { close(descriptor); return nothing_value(); }
    sigpending(&pending); int already_pending = sigismember(&pending, SIGPIPE);
#endif
    void *context = NULL, *connection = NULL;
    char *bytes = NULL; size_t length = 0, capacity = 0;
    int complete = 0, pipe_error = 0; const char *why = "context creation failed";
    context = tls.ctx_new(tls.method());
    if (context == NULL) goto tls_done;
    tls.ctx_verify(context, 1, NULL);
    why = "trust roots unavailable";
    if (tls.ctx_paths(context) != 1) goto tls_done;
    why = "connection creation failed";
    connection = tls.new_ssl(context);
    if (connection == NULL) goto tls_done;
    why = "hostname verification setup failed";
    if (tls.ctrl(connection, 55, 0, a[0].text) != 1 || tls.set_host(connection, a[0].text) != 1 || tls.set_fd(connection, descriptor) != 1) goto tls_done;
    why = "verified handshake failed";
    for (;;) {
        if (deadline != 0 && monotonic_ns() / 1000000 >= deadline) goto tls_done;
        tls.clear_error(); errno = 0;
        int result = tls.connect_ssl(connection), saved_errno = errno;
        if (saved_errno == EPIPE) pipe_error = 1;
        if (result == 1) break;
        if (!tls_retry(connection, result, saved_errno, descriptor, deadline)) goto tls_done;
    }
    if (tls.verified(connection) != 0) goto tls_done;
    for (size_t sent = 0; sent < a[2].text_length;) {
        why = "write failed or caller deadline reached";
        if (deadline != 0 && monotonic_ns() / 1000000 >= deadline) goto tls_done;
        size_t left = a[2].text_length - sent;
        int amount = left > INT_MAX ? INT_MAX : (int)left;
        tls.clear_error(); errno = 0;
        int wrote = tls.write_ssl(connection, a[2].text + sent, amount);
        int saved_errno = errno;
        if (saved_errno == EPIPE) pipe_error = 1;
        if (wrote > 0) { sent += (size_t)wrote; continue; }
        if (!tls_retry(connection, wrote, saved_errno, descriptor, deadline)) goto tls_done;
    }
    for (;;) {
        why = "read failed or caller deadline reached";
        if (deadline != 0 && monotonic_ns() / 1000000 >= deadline) goto tls_done;
        char chunk[8192]; tls.clear_error(); errno = 0;
        int got = tls.read_ssl(connection, chunk, sizeof chunk);
        int saved_errno = errno;
        if (saved_errno == EPIPE) pipe_error = 1;
        if (got > 0) {
            if ((size_t)got > SIZE_MAX - length) { why = "response extent overflow"; goto tls_done; }
            size_t needed = length + (size_t)got;
            if (needed > capacity) {
                size_t grown = capacity == 0 ? sizeof chunk : capacity > SIZE_MAX / 2 ? SIZE_MAX : capacity * 2;
                if (grown < needed) grown = needed;
                char *next = realloc(bytes, grown);
                if (next == NULL) { why = "response allocation failed"; goto tls_done; }
                bytes = next; capacity = grown;
            }
            memcpy(bytes + length, chunk, (size_t)got); length += (size_t)got; continue;
        }
        int error = tls.error(connection, got);
        if (error == 2 || error == 3) { if (socket_wait(descriptor, error == 2 ? POLLIN : POLLOUT, deadline)) continue; goto tls_done; }
        if (error == 5 && saved_errno == EINTR) continue;
        if (error == 6) break;
        why = "read ended without a verified TLS close"; goto tls_done;
    }
    why = "TLS release incomplete";
    for (;;) {
        if (deadline != 0 && monotonic_ns() / 1000000 >= deadline) goto tls_done;
        tls.clear_error(); errno = 0;
        int result = tls.shutdown_ssl(connection), saved_errno = errno;
        if (saved_errno == EPIPE) pipe_error = 1;
        if (result == 1) { complete = 1; break; }
        if (!tls_retry(connection, result, saved_errno, descriptor, deadline)) goto tls_done;
    }
tls_done:
    if (connection != NULL) tls.free_ssl(connection);
    if (context != NULL) tls.ctx_free(context);
    if (close(descriptor) != 0) { complete = 0; why = "socket release incomplete"; }
#ifndef SO_NOSIGPIPE
    if (pipe_error && !already_pending) { struct timespec zero = {0,0}; while (sigtimedwait(&blocked, NULL, &zero) < 0 && errno == EINTR) {} }
    if (pthread_sigmask(SIG_SETMASK, &previous, NULL) != 0) { complete = 0; why = "signal ownership restoration failed"; }
#endif
    if (!complete) { free(bytes); fprintf(runtime_error(), "tls_request: %s\n", why); return nothing_value(); }
    Value result = text_value(bytes == NULL ? "" : bytes, length); free(bytes); return result;
}

typedef struct {
    unsigned char *bytes;
    size_t length, span, residents;
    void *memory;
    uint64_t generation;
    int process_retained;
} NativePage;
static NativePage *native_pages;
static size_t native_page_count, native_page_capacity;
static Cell *jit_released_cells;

/* One executable-page owner for integer leaves and Form's buffer ABI. Images
 * and their meaning stay in Form; this carrier owns only host memory. */
static NativePage *jit_admit_bytes(const unsigned char *bytes, size_t length, size_t *selected) {
#if (defined(__APPLE__) && defined(__aarch64__)) || (defined(__linux__) && defined(__x86_64__))
    if (length == 0) return NULL;
    size_t slot = SIZE_MAX, reusable = SIZE_MAX;
    for (size_t i = 0; i < native_page_count; ++i) {
        NativePage *page = &native_pages[i];
        if (page->memory != NULL && page->length == length && memcmp(page->bytes, bytes, length) == 0) { *selected = i; return page; }
        if (page->generation == UINT64_MAX) continue;
        if (page->memory == NULL && slot == SIZE_MAX) slot = i;
        if (page->residents == 0 && !page->process_retained && reusable == SIZE_MAX) reusable = i;
    }
    if (slot == SIZE_MAX && native_page_count == native_page_capacity) slot = reusable;
    if (slot == SIZE_MAX) {
        if (native_page_count == SIZE_MAX / sizeof *native_pages) return NULL;
        size_t needed = native_page_count + 1;
        if (needed > native_page_capacity) {
            size_t grown = native_page_capacity == 0 ? 1 : native_page_capacity > SIZE_MAX / 2 / sizeof *native_pages ? needed : native_page_capacity * 2;
            NativePage *next = realloc(native_pages, grown * sizeof *next);
            if (next == NULL) return NULL;
            native_pages = next; native_page_capacity = grown;
        }
        slot = native_page_count;
        native_pages[slot] = (NativePage){0};
    }
    size_t unit = host_page_bytes();
    if (length > SIZE_MAX - (unit - 1)) return NULL;
    size_t span = ((length + unit - 1) / unit) * unit;
    unsigned char *copy = malloc(length);
    if (copy == NULL) return NULL;
#ifdef __APPLE__
    void *memory = mmap(NULL, span, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON | MAP_JIT, -1, 0);
    if (memory == MAP_FAILED) { free(copy); return NULL; }
    pthread_jit_write_protect_np(0); memcpy(memory, bytes, length); pthread_jit_write_protect_np(1);
#else
    void *memory = mmap(NULL, span, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED) { free(copy); return NULL; }
    memcpy(memory, bytes, length);
    if (mprotect(memory, span, PROT_READ | PROT_EXEC) != 0) { munmap(memory, span); free(copy); return NULL; }
#endif
    __builtin___clear_cache(memory, (char *)memory + length); memcpy(copy, bytes, length);
    NativePage *page = &native_pages[slot];
    if (page->memory != NULL && munmap(page->memory, page->span) != 0) { munmap(memory, span); free(copy); return NULL; }
    free(page->bytes);
    *page = (NativePage){copy, length, span, 0, memory, page->generation + 1, 0};
    if (slot == native_page_count) native_page_count += 1;
    *selected = slot; return page;
#else
    (void)bytes; (void)length; (void)selected; return NULL;
#endif
}

// Transient file leases are the unwind hook for the Form storage owner. Native
// POSIX recipes do all reads, writes, framing and publication on the returned
// descriptor. Private journal leases own a lock; ordinary regular-file handles
// retain the same unwind ownership without imposing journal permissions.
static int file_lease_close(size_t index) {
    if (index >= file_lease_count) return 1;
    if (file_leases[index].descriptor < 0) return file_leases[index].close_error == 0;
    int fd = file_leases[index].descriptor;
    if (close(fd) == 0 || errno == EBADF) { file_leases[index] = (FileLease){-1,NULL}; return 1; }
    file_leases[index].close_error = errno;
#ifdef __linux__
    // Linux releases the descriptor before reporting close errors. Retrying
    // could close a new owner's reused descriptor; retain the error, not fd.
    file_leases[index].descriptor = -1;
#endif
    return 0;
}
static Value native_posix_operation(Value request) {
    if (!value_proper_list(request) || request.item_count < 2) return nothing_value();
    Value operation = list_at(request,0), argument = list_at(request,1);
    if (operation.kind != KIND_TEXT) return nothing_value();
    if (bytes_equal_cstr(operation.text,operation.text_length,"fork")) {
        // An in-process fork must pass through libc: its atfork bookkeeping
        // includes the Darwin PID cache. Form keeps the continuation policy.
        // Active scoped leases cannot become two independently writable owners.
        for (size_t i=0;i<file_lease_count;++i) if (file_leases[i].descriptor>=0) return integer_value(-EBUSY);
        for (size_t i=0;i<native_gift_view_count;++i) if (native_gift_views[i].owner!=NULL) return integer_value(-EBUSY);
        pid_t child=fork();
        if (child<0) return integer_value(-errno);
        if (child==0) {
            process_born_ms=unix_milliseconds(); node_mint_events=0; live_sequence=0;
            memset(last_live_words,0,sizeof last_live_words); field_published_pid=0;
        }
        return integer_value((long long)child);
    }
    if (bytes_equal_cstr(operation.text,operation.text_length,"close") || bytes_equal_cstr(operation.text,operation.text_length,"held") || bytes_equal_cstr(operation.text,operation.text_length,"status") || bytes_equal_cstr(operation.text,operation.text_length,"identity") || bytes_equal_cstr(operation.text,operation.text_length,"stamp") || bytes_equal_cstr(operation.text,operation.text_length,"mode")) {
        if (request.item_count != 2 || argument.kind != KIND_INTEGER || argument.integer <= 0 || (uint64_t)argument.integer > file_lease_count) return nothing_value();
        size_t index = (size_t)argument.integer-1;
        if (bytes_equal_cstr(operation.text,operation.text_length,"identity") || bytes_equal_cstr(operation.text,operation.text_length,"stamp") || bytes_equal_cstr(operation.text,operation.text_length,"mode")) {
            struct stat state;
            if (file_leases[index].descriptor < 0 || fstat(file_leases[index].descriptor,&state) != 0) return nothing_value();
            if (bytes_equal_cstr(operation.text,operation.text_length,"mode")) return integer_value((long long)state.st_mode);
#ifdef __APPLE__
            struct timespec modified=state.st_mtimespec, changed=state.st_ctimespec;
#else
            struct timespec modified=state.st_mtim, changed=state.st_ctim;
#endif
            Value fields[] = {integer_value((long long)state.st_dev),integer_value((long long)state.st_ino),integer_value((long long)state.st_size),integer_value(modified.tv_sec),integer_value(modified.tv_nsec),integer_value(changed.tv_sec),integer_value(changed.tv_nsec)};
            return list_from(fields,bytes_equal_cstr(operation.text,operation.text_length,"identity") ? 2 : 7);
        }
        if (bytes_equal_cstr(operation.text,operation.text_length,"status")) {
            Value fields[] = {integer_value(file_leases[index].descriptor >= 0),integer_value(file_leases[index].close_error)};
            return list_from(fields,2);
        }
        return integer_value(bytes_equal_cstr(operation.text,operation.text_length,"held") ? file_leases[index].descriptor >= 0 : file_lease_close(index));
    }
    int regular = bytes_equal_cstr(operation.text,operation.text_length,"regular-open");
    if ((!regular && !bytes_equal_cstr(operation.text,operation.text_length,"open")) || request.item_count != 3 || argument.kind != KIND_TEXT || argument.text_length == 0 || memchr(argument.text,0,argument.text_length) != NULL) return nothing_value();
    Value writing = list_at(request,2);
    if (writing.kind != KIND_INTEGER || (writing.integer != 0 && writing.integer != 1)) return nothing_value();
    if (active_cell != NULL && active_cell->deadline != 0 && monotonic_ns()/1000000 >= active_cell->deadline) cell_stop("deadline","file lease admission reached the cell deadline");
    // Reserve the ownership slot before obtaining a descriptor. Allocation can
    // fail through a catchable Form choice and must not strand an untracked fd.
    file_leases = grow_list(file_leases,&file_lease_capacity,sizeof *file_leases,file_lease_count+1);
    // A FIFO must not block before the regular-file check can refuse it.
    int flags=O_CLOEXEC|O_NONBLOCK|(regular ? 0 : O_NOFOLLOW)|(writing.integer ? (regular ? O_WRONLY : O_RDWR)|O_CREAT : O_RDONLY);
    int fd = open(argument.text,flags,regular ? 0666 : 0600);
    if (fd < 0) return regular ? integer_value(-errno) : nothing_value();
    size_t index = file_lease_count++;
    file_leases[index] = (FileLease){fd,active_cell};
    struct stat state;
    if (fstat(fd,&state) != 0 || !S_ISREG(state.st_mode) || (!regular && (state.st_uid != getuid() || (state.st_mode&0077) != 0))) {
        if (!file_lease_close(index)) fail("file descriptor release incomplete after regular-file admission failed");
        return nothing_value();
    }
    for (;!regular;) {
        if (flock(fd,(writing.integer ? LOCK_EX : LOCK_SH)|LOCK_NB) == 0) break;
        int error = errno;
        if (error != EINTR && error != EWOULDBLOCK && error != EAGAIN) {
            if (!file_lease_close(index)) fprintf(runtime_error(),"file lease %zu: release incomplete after lock admission failed\n",index+1);
            return nothing_value();
        }
        for (size_t held=0; error!=EINTR && held<index; ++held) {
            struct stat owner;
            if (file_leases[held].descriptor >= 0 && fstat(file_leases[held].descriptor,&owner)==0 && owner.st_dev==state.st_dev && owner.st_ino==state.st_ino) {
                file_lease_close(index);
                fail("file lease would wait on its own process; reuse or release the held owner");
            }
        }
        if (active_cell != NULL && active_cell->deadline != 0 && monotonic_ns()/1000000 >= active_cell->deadline) {
            file_lease_close(index); cell_stop("deadline","file lease remained held by another owner until the cell deadline");
        }
        // This only yields between actual lock refusals; it is not an attempt
        // count or wall-time allowance. The caller's deadline is authoritative.
        poll(NULL,0,1);
    }
    Value fields[] = {integer_value((long long)index+1),integer_value(fd)};
    return list_from(fields,2);
}

static int value_proper_list(Value value) {
    if (value.kind != KIND_LIST) return 0;
    List *cursor = value.list;
    for (size_t i = 0; i < value.item_count; ++i) {
        if (cursor == NULL || cursor->dotted_tail != NULL) return 0;
        cursor = cursor->tail;
    }
    return cursor == NULL;
}

static Value jit_cpu_run(Value request) {
    if (!value_proper_list(request) || request.item_count != 3) return nothing_value();
    Value image = list_at(request, 0), buffers = list_at(request, 1), constants = list_at(request, 2);
#if defined(__linux__) && defined(__x86_64__)
    if (!value_proper_list(image) || image.item_count != 2) return nothing_value();
    Value target = list_at(image, 0);
    if (target.kind != KIND_TEXT || !bytes_equal_cstr(target.text, target.text_length, "linux-x86_64")) return nothing_value();
    image = list_at(image, 1);
#elif !defined(__APPLE__) || !defined(__aarch64__)
    return nothing_value();
#endif
    if (image.kind != KIND_TEXT || image.text_length == 0 || !value_proper_list(buffers) || !value_proper_list(constants)) return nothing_value();
#ifdef __APPLE__
    if (image.text_length % 4 != 0) return nothing_value();
#endif
    size_t count = buffers.item_count;
    void **pointers = calloc(count == 0 ? 1 : count, sizeof *pointers);
    void **owned = calloc(count == 0 ? 1 : count, sizeof *owned);
    void **maps = calloc(count == 0 ? 1 : count, sizeof *maps);
    size_t *lengths = calloc(count == 0 ? 1 : count, sizeof *lengths);
    uint32_t *words = calloc(constants.item_count == 0 ? 1 : constants.item_count, sizeof *words);
    Value *answers = calloc(count == 0 ? 1 : count, sizeof *answers);
    size_t answered = 0; int ok = pointers && owned && maps && lengths && words && answers;
    for (size_t i = 0; ok && i < constants.item_count; ++i) {
        Value word = list_at(constants, i);
        if (word.kind != KIND_INTEGER) ok = 0; else words[i] = (uint32_t)word.integer;
    }
    for (size_t i = 0; ok && i < count; ++i) {
        Value buffer = list_at(buffers, i);
        if (buffer.kind == KIND_LIST && !value_proper_list(buffer)) { ok = 0; break; }
        if (buffer.kind == KIND_TEXT) pointers[i] = buffer.text;
        else if (buffer.kind == KIND_LIST && buffer.item_count == 1) {
            Value text = list_at(buffer, 0);
            if (text.kind != KIND_TEXT) { ok = 0; break; }
            lengths[i] = text.text_length; owned[i] = malloc(text.text_length == 0 ? 1 : text.text_length);
            if (owned[i] == NULL) { ok = 0; break; }
            memcpy(owned[i], text.text, text.text_length); pointers[i] = owned[i];
        } else if (buffer.kind == KIND_LIST && buffer.item_count == 4) {
            Value tag = list_at(buffer, 0), path = list_at(buffer, 1), offset = list_at(buffer, 2), size = list_at(buffer, 3);
            if (tag.kind != KIND_TEXT || !bytes_equal_cstr(tag.text, tag.text_length, "file") || path.kind != KIND_TEXT || memchr(path.text, 0, path.text_length) || offset.kind != KIND_INTEGER || size.kind != KIND_INTEGER || offset.integer < 0 || size.integer <= 0) { ok = 0; break; }
            int fd = open(path.text, O_RDONLY); struct stat extent;
            if (fd < 0) { ok = 0; break; }
            if (fstat(fd, &extent) != 0 || offset.integer > extent.st_size || size.integer > extent.st_size - offset.integer) { close(fd); ok = 0; break; }
            size_t page = host_page_bytes(), edge = (size_t)((uint64_t)offset.integer % page);
            if ((uint64_t)size.integer > SIZE_MAX - edge) { close(fd); ok = 0; break; }
            lengths[i] = (size_t)size.integer + edge;
            void *mapping = mmap(NULL, lengths[i], PROT_READ, MAP_PRIVATE, fd, (off_t)(offset.integer - edge));
            int closed = close(fd) == 0;
            if (mapping == MAP_FAILED) { ok = 0; break; }
            maps[i] = mapping; pointers[i] = (char *)mapping + edge;
            if (!closed) ok = 0;
        } else ok = 0;
    }
    if (ok) {
        size_t slot = 0; NativePage *page = jit_admit_bytes((unsigned char *)image.text, image.text_length, &slot);
        if (page == NULL) ok = 0;
        else {
            page->process_retained = 1;
            void (*call)(void **, const uint32_t *, uint64_t) = (void *)page->memory;
            uint64_t began = monotonic_ns(); call(pointers, words, 1);
            cpu_image_runs += 1; cpu_image_us += (monotonic_ns() - began) / 1000;
            for (size_t i = 0; i < count; ++i) if (owned[i] != NULL) answers[answered++] = text_value(owned[i], lengths[i]);
        }
    }
    for (size_t i = 0; i < count; ++i) {
        if (owned != NULL) free(owned[i]);
        if (maps != NULL && maps[i] != NULL && munmap(maps[i], lengths[i]) != 0) ok = 0;
    }
    Value result = ok ? list_from(answers, answered) : nothing_value();
    free(pointers); free(owned); free(maps); free(lengths); free(words); free(answers); return result;
}

static void jit_cell_begin(void) {
    while (jit_released_cells != NULL) {
        Cell *cell = jit_released_cells;
        jit_released_cells = cell->jit_next_released;
        cell->jit_released = 0; cell->jit_slot = 0; cell->jit_next_released = NULL;
    }
}

static void jit_release_identity(Cell *owner) {
    owner->jit_released = 1; owner->jit_slot = 0;
    owner->jit_next_released = jit_released_cells; jit_released_cells = owner;
}

static int jit_arguments(Value value, long long arguments[8]) {
    size_t count = 0, length = value.kind == KIND_LIST ? value.item_count : 1;
    List *cursor = value.list;
    for (size_t i = 0; i < length; ++i) {
        Value item = value;
        if (value.kind == KIND_LIST) {
            if (cursor == NULL || cursor->dotted_tail != NULL) return 0;
            item = cursor->head; cursor = cursor->tail;
        }
        if (item.kind == KIND_INTEGER && count < 8) arguments[count++] = item.integer;
        else if (item.kind == KIND_TEXT && count <= 6) {
            arguments[count++] = (long long)(intptr_t)item.text;
            arguments[count++] = (long long)item.text_length;
        } else return 0;
    }
    return count != 0;
}

static Value offer_jit_leaf_inram(Value *arguments, size_t argument_count) {
    (void)argument_count;
#if defined(__APPLE__) && defined(__aarch64__)
    Value request = arguments[0], image = request;
    Cell *owner = NULL;
    long long action = 0;
    if (request.kind == KIND_LIST && request.item_count == 3) {
        Value mode = list_at(request, 0), identity = list_at(request, 1);
        if (mode.kind == KIND_INTEGER && identity.kind == KIND_NODE) {
            if (identity.cell == NULL) return nothing_value();
            owner = identity.cell; action = mode.integer; image = list_at(request, 2);
        }
    }
    NativePage *page = NULL;
    if (owner != NULL) {
        if (owner->jit_released) return action == 1 ? integer_value(0) : nothing_value();
        if (owner->jit_slot > 0 && owner->jit_slot <= native_page_count) {
            page = &native_pages[owner->jit_slot - 1];
            if (page->memory == NULL || page->generation != owner->jit_generation) page = NULL;
        }
        if (action == 1) {
            if (page == NULL) { jit_release_identity(owner); return integer_value(0); }
            if (page->residents == 0) return nothing_value();
            if (page->residents == 1 && !page->process_retained && munmap(page->memory, page->span) != 0) return nothing_value();
            page->residents -= 1;
            if (page->residents == 0 && !page->process_retained) { free(page->bytes); page->bytes = NULL; page->memory = NULL; }
            jit_release_identity(owner);
            return integer_value(1);
        }
        if (action != 0) return nothing_value();
    }
    long long values[8] = {0};
    if (!jit_arguments(arguments[1], values)) return nothing_value();
    if (page == NULL) {
        if (image.kind != KIND_LIST || image.item_count == 0 || image.item_count % 4 != 0) return nothing_value();
        size_t length = image.item_count;
        unsigned char *bytes = allocate_zeroed(length, 1);
        List *cursor = image.list;
        for (size_t i = 0; i < length; ++i) {
            if (cursor == NULL || cursor->head.kind != KIND_INTEGER || cursor->head.integer < 0 || cursor->head.integer > 255 || cursor->dotted_tail != NULL) { free(bytes); return nothing_value(); }
            bytes[i] = (unsigned char)cursor->head.integer; cursor = cursor->tail;
        }
        size_t slot = SIZE_MAX;
        page = jit_admit_bytes(bytes, length, &slot); free(bytes);
        if (page == NULL) return nothing_value();
        if (owner != NULL) { owner->jit_slot = slot + 1; owner->jit_generation = page->generation; page->residents += 1; }
    }
    long long (*call)(long long,long long,long long,long long,long long,long long,long long,long long) = (void *)page->memory;
    native_execution_count += 1;
    long long result = call(values[0],values[1],values[2],values[3],values[4],values[5],values[6],values[7]);
    __asm__ volatile("" : "+r"(result) : : "x19","x20","x21","x22","x23","x24","x25","x26","x27","x28");
    return integer_value(form_integer_wrap((uint64_t)result));
#else
    (void)arguments;
    return nothing_value();
#endif
}

static void register_builtin_operations(void) {
    register_operation("cell_run", ARITY_FOUR, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_cell_run);
    register_operation("cell_call", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_cell_call);
    register_operation("cell_input", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_cell_input);
    register_operation("socket_listen", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_listen);
    register_operation("socket_port", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_port);
    register_operation("socket_accept", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_accept);
    register_operation("socket_connect", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_connect);
    register_operation("socket_send", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_send);
    register_operation("socket_recv", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_recv);
    register_operation("socket_close", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_close);
    register_operation("tls_request", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_tls_request);
    register_operation("if", ARITY_THREE, FIXED_ARITY, LEAVE_ARGUMENTS, OP_IF, NULL);
    register_operation("do", ARITY_NONE, VARIADIC, LEAVE_ARGUMENTS, OP_DO, NULL);
    register_operation("and", ARITY_NONE, VARIADIC, LEAVE_ARGUMENTS, OP_AND, NULL);
    register_operation("list", ARITY_NONE, VARIADIC, EVALUATE_ARGUMENTS, OP_LIST, offer_list);
    register_operation("add", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_ADD, offer_add);
    register_operation("sub", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_SUB, offer_sub);
    register_operation("eq", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_EQ, offer_eq);
    register_operation("ne", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_EQ, offer_ne);
    register_operation("value_eq", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_EQ, offer_value_eq);
    register_operation("math_sqrt", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_math_sqrt);
    register_operation("math_exp", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_math_exp);
    register_operation("math_log", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_math_log);
    register_operation("math_pow", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_math_pow);
    register_operation("lt", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_LT, offer_lt);
    register_operation("ge", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_GE, offer_ge);
    register_operation("str_eq", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_STR_EQ, offer_str_eq);
    register_operation("str_concat", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_STR_CONCAT, offer_str_concat);
    register_operation("value_kind", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_VALUE_KIND, offer_value_kind);
    register_operation("float_leaf", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_float_leaf);
    register_operation("make_float32", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_make_float32);
    register_operation("nothing?", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_NOTHING, offer_nothing);
    register_operation(NAME_NOTHING, ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_NOTHING, offer_nothing_value);
    register_operation("stop", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_NOTHING, offer_nothing_value);
    register_operation("read_line", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_READ_LINE, offer_read_line);
    register_operation("file_mtime", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FILE_MTIME, offer_file_mtime);
    register_operation("cell_path", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_CELL_PATH, offer_cell_path);
    register_operation("self_source", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_self_source);
    register_operation("host_realpath", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_realpath);
    register_operation("fs_truncate", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_fs_truncate);
    register_operation("intern_trivial_bool", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_INTERN_BOOL, offer_intern_bool);
    register_operation("kernel_stat", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_stat);
    register_operation("kernel_live_pids", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("kernel_roster_adopt", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("kernel_roster_forget", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("kernel_live", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_live);
    register_operation("kernel_page_ended", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_kernel_page_ended);
    register_operation("kernel_page_bury", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_kernel_page_bury);
    register_operation("fb_record", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FB_RECORD, offer_fb_record);
    register_operation("framebuffer-events", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FB_EVENTS, offer_fb_events);
    register_operation("framebuffer-clear", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FB_CLEAR, offer_fb_clear);
    register_operation("gt", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_GT, offer_gt);
    register_operation("le", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_LE, offer_le);
    register_operation("nth", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_NTH, offer_nth);
    operation_answers("nth");
    register_operation("slice", ARITY_NONE, VARIADIC, EVALUATE_ARGUMENTS, OP_SLICE, offer_slice);
    register_operation("+", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PLUS, offer_plus);
    register_operation("let", ARITY_TWO, VARIADIC, LEAVE_ARGUMENTS, OP_LET, NULL);
    register_operation("len", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PLUS, offer_len);
    register_operation("empty", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PLUS, offer_empty);
    register_operation("str", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PLUS, offer_str);
    register_operation("print_str", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PLUS, offer_print_str);
    register_operation("or", ARITY_NONE, VARIADIC, LEAVE_ARGUMENTS, OP_OR, NULL);
    register_operation("lambda", ARITY_TWO, FIXED_ARITY, LEAVE_ARGUMENTS, OP_LAMBDA, NULL);
    register_operation("not", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_NOT, offer_not);
    register_operation("mul", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_MUL, offer_mul);
    register_operation("div", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_DIV, offer_div);
    register_operation("mod", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_MOD, offer_mod);
    register_operation("head", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_HEAD, offer_head);
    register_operation("tail", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_TAIL, offer_tail);
    register_operation("cons", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_CONS, offer_cons);
    register_operation("print", ARITY_NONE, VARIADIC, EVALUATE_ARGUMENTS, OP_PRINT, offer_print);
    register_operation("record_new", ARITY_NONE, VARIADIC, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_new);
    register_operation("record_has", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_has);
    register_operation("record_get", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_get);
    register_operation("record_set", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_set);
    register_operation("record_keys", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_keys);
    register_operation("record?", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_type);
    register_operation("record_blueprint", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_blueprint);
    register_operation("method_define", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_method_define);
    register_operation("method_has", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_method_has);
    register_operation("method_invoke", ARITY_TWO, VARIADIC, EVALUATE_ARGUMENTS, OP_METHOD_INVOKE, NULL);
    register_operation("kernel_ast", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_AST, NULL);
    register_operation("jit_leaf_inram", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_jit_leaf_inram);
    register_operation("byte_to_str", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_byte_to_str);
    register_operation("str_len", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_str_len);
    register_operation("substring", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_substring);
    operation_answers("substring");
    register_operation("scan_run", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_scan_run);
    register_operation("char_at", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_char_at);
    register_operation("ord", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_ord);
    register_operation("str_find", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_str_find);
    operation_answers("str_find");
    register_operation("str_to_int", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_str_to_int);
    register_operation("str_byte_at", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_str_byte_at);
    register_operation("value_str", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_value_str);
    register_operation("range", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_range);
    register_operation("host_sleep_ms", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_sleep_ms);
    register_operation("host_monotonic_ms", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_monotonic_ms);
    register_operation("now_unix_ms", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_now_unix_ms);
    register_operation("host_getenv", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_getenv);
    register_operation("host_alive", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_alive);
    register_operation("host_kill", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_kill);
    register_operation("host_wait", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_wait);
    register_operation("host_signal", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_signal);
    register_operation("host_dir_mkdir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_dir_mkdir);
    register_operation("fs_mkdir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_dir_mkdir);
    register_operation("host_dir_rmdir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_dir_rmdir);
    register_operation("fs_rmdir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_dir_rmdir);
    register_operation("host_path_remove", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_path_remove);
    register_operation("fs_remove", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_path_remove);
    register_operation("host_path_rename", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_path_rename);
    register_operation("fs_rename", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_path_rename);
    register_operation("host_mkdir_mode", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_mkdir_mode);
    register_operation("host_dir_list", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_dir_list);
    register_operation("fs_list", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_dir_list);
    register_operation("host_file_holders", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_holders);
    register_operation("host_file_identity", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_identity);
    register_operation("host_vm_stat", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_vm_stat);
    register_operation("host_process", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_process);
    register_operation("host_processes", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_processes);
    register_operation("host_cpu_us", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_cpu_us);
    register_operation("isatty", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_isatty);
    register_operation("input_byte", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_input_byte);
    register_operation("host_nice", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_nice);
    register_operation("sense_mic_count", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_mic_count);
    register_operation("sense_mic_name", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_mic_name);
    register_operation("sense_mic_health", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_mic_health);
    register_operation("sense_mic_stream_start", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_mic_start);
    register_operation("sense_mic_stream_read", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_mic_read);
    register_operation("sense_mic_stream_stop", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_mic_stop);
    register_operation("sense_cam_count", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_no_devices);
    register_operation("sense_cam_name", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_no_text);
    register_operation("sense_cam_health", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("sense_cam_grab", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("sense_cam_luma", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_no_samples);
    register_operation("sense_audio_loopback", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_no_samples);
    register_operation("sense_wifi_ssid", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_no_text);
    register_operation("sense_wifi_signal", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("sense_bt_present", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("sense_bt_count", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("sense_power", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("sense_mem", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("sense_publish", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("mesh_serve", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("mesh_announce", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("mesh_discover", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("mesh_registry", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("mesh_roster", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("api_health", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("mesh_register", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("mesh_detect", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("cuda_matvec", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_unavailable);
    register_operation("cuda_matvec_f32", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_carrier_no_samples);
    register_operation("host_gpu_utilization", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_gpu_utilization);
    register_operation("host_gpu_busy_us", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_gpu_busy_us);
    register_operation("host_cpu_busy_us", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_cpu_busy_us);
    register_operation("host_load_avg", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_load_avg);
    register_operation("host_volume", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_volume);
    register_operation("host_disk_stat", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_disk_stat);
    register_operation("metal_matvec_fixture", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_matvec);
    register_operation("metal_matvec_f32", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_matvec);
    register_operation("host_pid", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_pid);
    register_operation("temp_dir", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_temp_dir);
    register_operation("host_temp_dir", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_temp_dir);
    register_operation("host_cwd", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_cwd);
    register_operation("host_file_write_text", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_write_text);
    register_operation("write_file_text", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_write_text);
    register_operation("write_file", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_write_text);
    register_operation("file_append_bytes", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_append_bytes);
    register_operation("host_file_append_bytes", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_append_bytes);
    register_operation("file_size", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_size);
    register_operation("host_file_size", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_size);
    register_operation("host_file_mtime", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_mtime);
    register_operation("host_utimes", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_utimes);
    register_operation("host_chmod", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_chmod);
    register_operation("host_link", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_link);
    register_operation("host_file_mode", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_mode);
    register_operation("file_mtime", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_mtime);
    register_operation("host_path_is_dir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_path_is_dir);
    register_operation("fs_is_dir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_path_is_dir);
    register_operation("host_path_exists", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_path_exists);
    register_operation("fs_exists", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_path_exists);
    register_operation("host_file_read_text", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_read_text);
    register_operation("read_file", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_read_text);
    register_operation("file_open", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_open);
    register_operation("file_read", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_read);
    register_operation("file_close", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_close);
    register_operation("read_file_slice", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_read_file_slice);
    register_operation("host_file_read_slice", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_read_file_slice);
    register_operation("terminal_cols", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_terminal_cols);
    register_operation("shm_offer", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shm_offer);
    register_operation("shm_receive", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shm_receive);
    register_operation("shm_read", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("shm_write", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("shm_seq", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shm_seq);
    register_operation("shm_release", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shm_release);
    register_operation("node_category", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_category);
    register_operation("node_eq", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_eq);
    register_operation("node_value", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_payload);
    register_operation("node_gift_read", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("node_gift_write", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("make_nodeid", ARITY_FOUR, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_make_nodeid);
    register_operation("node_pkg", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_pkg);
    register_operation("node_level", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_level);
    register_operation("node_type", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_type);
    register_operation("node_inst", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_inst);
    register_operation("intern_trivial_string", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    /* bp carries a symbolic category; fol-bp resolves the admitted coordinate registry. */
    operation_answers("bp");
    register_operation("intern_trivial_int", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("intern_trivial_float", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("make_float64", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("float_value", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_float_value);
    register_operation("float_to_int", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_float_to_int);
    register_operation("str_to_float", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_str_to_float);
    register_operation("intern_node", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("intern_node_at", ARITY_FIVE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("node_children", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_children);
    register_operation("node_at", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("cell_map", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("cell_field", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("cell_value", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("cell_ref", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("cell_unmap", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FIELD, NULL);
    register_operation("node_source", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_source);
    register_operation("host-exec", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_exec);
    register_operation("gift_roster_names", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_gift_roster_names);
    register_operation("gift_roster_register", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_gift_roster_register);
    register_operation("add_u32", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_add_u32);
    register_operation("band", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_band);
    register_operation("shr_u32", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shr_u32);
    register_operation("shl_u32", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shl_u32);
    register_operation("rotr_u32", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_rotr_u32);
    register_operation("bor", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_bor);
    register_operation("bxor", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_bxor);
    register_operation("bnot_u32", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_bnot_u32);
    register_operation("terminal_lines", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_terminal_lines);
    register_operation("metal_status", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_status);
    register_operation("metal_live", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_live);
    register_operation("metal_pipeline", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_pipeline);
    register_operation("metal_buf_alloc", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_buf_alloc);
    register_operation("metal_buf_from_file", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_buf_from_file);
    register_operation("metal_buf_write", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_buf_write);
    register_operation("metal_buf_fill", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_buf_fill);
    register_operation("metal_enqueue", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_enqueue);
    register_operation("metal_buf_read", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_buf_read);
    register_operation("metal_sync", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_sync);
    register_operation("metal_batch_concurrent", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_batch_concurrent);
    register_operation("metal_submit", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_submit);
    register_operation("metal_buf_free", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_buf_free);
    register_operation("metal_fence_wait", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_fence_wait);
    register_operation("metal_deadline", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_metal_deadline);
    register_operation("host_spawn_quiet", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_spawn_quiet);
    register_operation("host_spawn", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_spawn);
    register_operation("host_capture", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_capture);
    register_operation("host_spawn_at", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_spawn_at);
    register_operation(COALESCE_WORD, ARITY_TWO, FIXED_ARITY, LEAVE_ARGUMENTS, OP_COALESCE, NULL);
    register_operation(TRY_WORD, ARITY_TWO, FIXED_ARITY, LEAVE_ARGUMENTS, OP_TRY, NULL);
    register_operation("attempt", ARITY_ONE, FIXED_ARITY, LEAVE_ARGUMENTS, OP_ATTEMPT, NULL);
    register_operation("form_error", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FORM_ERROR, NULL);
    register_operation(FAIL_NAME, ARITY_NONE, FIXED_ARITY, LEAVE_ARGUMENTS, OP_FAIL, NULL);
    register_operation("choice", ARITY_NONE, VARIADIC, LEAVE_ARGUMENTS, OP_CHOICE, NULL);
}

typedef struct {
    Value left;
    Value right;
    List *left_cursor;
    List *right_cursor;
    size_t index;
    int entered;
} EqualPair;

static int atom_equal(Value left, Value right) {
    if (left.kind != right.kind) return 0;
    if (left.kind == KIND_FUNCTION) return left.body == right.body && left.captures == right.captures && left.program == right.program && left.native_operation == right.native_operation;
    if (left.kind == KIND_RECORD) return left.record == right.record;
    if (left.kind == KIND_NOTHING) return 1;
    if (left.kind == KIND_INTEGER) return left.integer == right.integer;
    if (left.kind == KIND_NODE) return left.integer == right.integer;
    if (left.kind == KIND_REAL) return left.real == right.real;
    if (left.kind == KIND_TEXT) return bytes_equal(left.text, left.text_length, right.text, right.text_length);
    return 0;
}

static int atom_identical(Value left,Value right,int strict) {
    return strict && left.kind==KIND_REAL && right.kind==KIND_REAL ? memcmp(&left.real,&right.real,sizeof left.real)==0 : atom_equal(left,right);
}
static int values_compare(Value left, Value right,int strict) {
    if (left.kind != KIND_LIST && right.kind != KIND_LIST) return atom_identical(left,right,strict);
    EqualPair *pairs = NULL;
    size_t pair_count = EMPTY_COUNT;
    size_t pair_capacity = EMPTY_COUNT;
    size_t needed = checked_add(pair_count, ONE_ELEMENT);
    pairs = grow_list(pairs, &pair_capacity, sizeof *pairs, needed);
    pairs[pair_count].left = left;
    pairs[pair_count].right = right;
    pairs[pair_count].left_cursor = NULL;
    pairs[pair_count].right_cursor = NULL;
    pairs[pair_count].index = FIRST_INDEX;
    pairs[pair_count].entered = 0;
    pair_count = needed;
    int same = 1;
    while (pair_count != EMPTY_COUNT) {
        EqualPair pair = pairs[pair_count - ONE_ELEMENT];
        if (pair.left.kind == KIND_LIST || pair.right.kind == KIND_LIST) {
            if (pair.left.kind != KIND_LIST || pair.right.kind != KIND_LIST || pair.left.item_count != pair.right.item_count) {
                same = 0;
                break;
            }
            if (!pair.entered) {
                pair.entered = 1;
                pair.index = FIRST_INDEX;
                pair.left_cursor = pair.left.list;
                pair.right_cursor = pair.right.list;
            }
            if (pair.index == pair.left.item_count) {
                pair_count -= ONE_ELEMENT;
                continue;
            }
            Value child_left = pair.left_cursor->head;
            Value child_right = pair.right_cursor->head;
            if (pair.left_cursor->dotted_tail != NULL || pair.right_cursor->dotted_tail != NULL) {
                // Once the last head is compared, compare the actual terminal
                // values. Proper termination is the empty list, never an
                // implicit match for an arbitrary dotted tail.
                pair.left = pair.left_cursor->dotted_tail != NULL ? *pair.left_cursor->dotted_tail : list_from(NULL,0);
                pair.right = pair.right_cursor->dotted_tail != NULL ? *pair.right_cursor->dotted_tail : list_from(NULL,0);
                pair.entered = 0;
            } else {
                pair.left_cursor = pair.left_cursor->tail;
                pair.right_cursor = pair.right_cursor->tail;
                pair.index = checked_add(pair.index, ONE_ELEMENT);
            }
            pairs[pair_count - ONE_ELEMENT] = pair;
            needed = checked_add(pair_count, ONE_ELEMENT);
            pairs = grow_list(pairs, &pair_capacity, sizeof *pairs, needed);
            pairs[pair_count].left = child_left;
            pairs[pair_count].right = child_right;
            pairs[pair_count].left_cursor = NULL;
            pairs[pair_count].right_cursor = NULL;
            pairs[pair_count].index = FIRST_INDEX;
            pairs[pair_count].entered = 0;
            pair_count = needed;
            continue;
        }
        if (!atom_identical(pair.left, pair.right,strict)) {
            same = 0;
            break;
        }
        pair_count -= ONE_ELEMENT;
    }
    free(pairs);
    return same;
}
static int values_equal(Value left,Value right) { return values_compare(left,right,0); }
static int values_identical(Value left,Value right) { return values_compare(left,right,1); }

static void deliver(Value value) {
    if (frame_count == EMPTY_COUNT) {
        finished = value;
        result_ready = 1;
        return;
    }
    frames[frame_count - ONE_ELEMENT].has_incoming = 1;
    frames[frame_count - ONE_ELEMENT].incoming = value;
}

static Frame *append_frame(void) {
    size_t needed = checked_add(frame_count, ONE_ELEMENT);
    frames = grow_list(frames, &frame_capacity, sizeof *frames, needed);
    frames[frame_count] = (Frame){0};
    frames[frame_count].stage_base = stage_depth;
    frame_count = needed;
    return &frames[frame_count - ONE_ELEMENT];
}

static void pop_frame(void) {
    Frame *frame = &frames[frame_count - ONE_ELEMENT];
    if (frame->kind == FRAME_LET && frame->stage == 1) local_count = frame->local_base;
    if (frame->restore_program) select_program(frame->previous_program);
    free(frame->arguments);
    frame->arguments = NULL;
    frame_count -= ONE_ELEMENT;
}

static int find_value_binding(const char *bytes, size_t length, Value *out) {
    size_t base = FIRST_INDEX;
    for (size_t index = frame_count; index > FIRST_INDEX; ) {
        index -= ONE_ELEMENT;
        if (frames[index].kind == FRAME_FUNCTION) {
            base = frames[index].local_base;
            break;
        }
    }
    for (size_t index = local_count; index > base; ) {
        index -= ONE_ELEMENT;
        if (bytes_equal(locals[index].bytes, locals[index].length, bytes, length)) {
            *out = locals[index].value;
            return 1;
        }
    }
    for (size_t index = frame_count; index > FIRST_INDEX; ) {
        index -= ONE_ELEMENT;
        if (frames[index].kind != FRAME_FUNCTION) continue;
        Value callee = frames[index].callee;
        for (size_t capture = callee.capture_count; capture > FIRST_INDEX; ) {
            capture -= ONE_ELEMENT;
            if (bytes_equal(callee.captures[capture].bytes, callee.captures[capture].length, bytes, length)) {
                *out = callee.captures[capture].value;
                return 1;
            }
        }
        break;
    }
    if (bytes_equal_cstr(bytes, length, NAME_TRUE)) {
        *out = integer_value(WORD_TRUE);
        return 1;
    }
    if (bytes_equal_cstr(bytes, length, NAME_FALSE)) {
        *out = integer_value(WORD_FALSE);
        return 1;
    }
    /* A symbol's text pointer is stable for its form, so its global slot is
     * cached until any write to the global table (global_epoch) moves it. */
    GlobalHit *hit = &global_hits[(((uintptr_t)bytes >> 3) ^ length) & (GLOBAL_HIT_SLOTS - 1)];
    size_t global;
    if (hit->text == bytes && hit->length == length && hit->epoch == global_epoch &&
        (hit->index == SIZE_MAX || bytes_equal(globals[hit->index].bytes, globals[hit->index].length, bytes, length))) global = hit->index;
    else {
        global = name_index_find(&global_names, bytes, length);
        *hit = (GlobalHit){bytes, length, global, global_epoch};
    }
    if (global != SIZE_MAX) {
        *out = globals[global].value;
        return 1;
    }
    for (CellProgram *owner = current_program->closure_owner; owner != NULL; owner = owner->closure_owner) {
        size_t found = name_index_find(&owner->global_names, bytes, length);
        if (found != SIZE_MAX) { *out = owner->globals[found].value; return 1; }
    }
    return 0;
}

static Value value_from_atom(Form *form) {
    if (form->unresolved) return nothing_value();
    if (form->kind == FORM_INTEGER) return integer_value(form->integer);
    if (form->kind == FORM_REAL) return real_value(form->real);
    if (form->kind == FORM_TEXT) {
        /* A literal's bytes are parser-owned, terminated and never freed or
         * written, so the value borrows them: no allocation, nothing to collect. */
        Value literal = nothing_value();
        literal.kind = KIND_TEXT; literal.text = form->text; literal.text_length = form->text_length;
        return literal;
    }
    if (form->kind == FORM_SYMBOL) {
        Value bound;
        if (find_value_binding(form->text, form->text_length, &bound)) return bound;
        const Operation *field = find_operation_text(form->text, form->text_length);
        if (field != NULL && field->id == OP_FIELD) {
            Value native = nothing_value(); native.kind = KIND_FUNCTION;
            native.parameter_count = field->arity; native.native_operation = (size_t)(field - operations) + 1;
            return native;
        }
        Binding *function = find_function_owned(form->text, form->text_length, form->path);
        if (function != NULL) return function->value;
        /* form-bml.fk's intrinsic aliases, after caller/source shadowing. */
        if (form_is_symbol(form, "empty")) return list_from(NULL, 0);
        if (form_is_symbol(form, NAME_NOTHING) || form_is_symbol(form, FAIL_NAME) || form_is_symbol(form, STOP_NAME)) return nothing_value();
        const Operation *operation = find_operation_text(form->text, form->text_length);
        if (operation != NULL && operation->evaluation == EVALUATE_ARGUMENTS) {
            Value native = nothing_value();
            native.kind = KIND_FUNCTION;
            native.parameter_count = operation->arity;
            native.native_operation = (size_t)(operation - operations) + 1;
            return native;
        }
        fail_at(form->offset, message_with_name(UNBOUND_PREFIX, form->text, form->text_length));
    }
    fail_at(form->offset, "a form has an unknown shape");
    return nothing_value();
}

static void offer_form(Form *form);
static void append_local(const char *bytes, size_t length, Value value);
static Value closure_value(Form *parameters, Form *body, size_t offset);

static int function_accepts(Value callee, size_t count) {
    if (callee.native_operation != 0) {
        const Operation *operation = &operations[callee.native_operation - 1];
        return operation->arity_policy == VARIADIC ? count >= operation->arity : count == operation->arity;
    }
    return count == callee.parameter_count;
}

static void begin_function_call(Form *form, Value callee) {
    size_t argument_count = call_argument_count(form);
    if (!function_accepts(callee, argument_count)) fail_at(form->offset, "a function received the wrong number of arguments");
    Frame *frame = append_frame();
    frame->kind = FRAME_EAGER;
    frame->form = form;
    frame->argument_count = argument_count;
    frame->arguments = allocate_zeroed(argument_count, sizeof *frame->arguments);
    frame->callee = callee;
}

static void begin_call(Form *form) {
    if (form->unresolved) { deliver(nothing_value()); return; }
    if (form->part_count == EMPTY_COUNT) fail_at(form->offset, "a call needs a head");
    Form *head = form->parts[CALL_HEAD];
    if (head->kind != FORM_SYMBOL) {
        Frame *frame = append_frame();
        frame->kind = FRAME_APPLY;
        frame->form = form;
        return;
    }
    if (form_is_symbol(head, NAME_DEFN)) {
        if (form->is_bound) {
            deliver(nothing_value());
            return;
        }
        if (form->part_count != DEFN_PART_COUNT || form->parts[DEFN_NAME]->kind != FORM_SYMBOL) fail_at(form->offset, "a local definition needs a name, parameters, and a body");
        Form *name = form->parts[DEFN_NAME];
        Value function = closure_value(form->parts[DEFN_PARAMETERS], form->parts[DEFN_BODY], form->offset);
        size_t captured = function.capture_count;
        Binding *captures = heap_allocate(checked_add(captured,1),sizeof *captures,HEAP_CAPTURES,checked_add(captured,1));
        if (captured != 0) memcpy(captures, function.captures, captured * sizeof *captures);
        heap_forget(function.captures); free(function.captures);
        function.captures = captures;
        function.capture_count = captured + 1;
        captures[captured] = (Binding){name->text, name->text_length, function};
        append_local(name->text, name->text_length, function);
        deliver(function);
        return;
    }
    size_t argument_count = call_argument_count(form);
    Value bound;
    if (find_value_binding(head->text, head->text_length, &bound) && bound.kind == KIND_FUNCTION) {
        begin_function_call(form, bound);
        return;
    }
    const Operation *operation;
    Binding *function;
    if (form->head_epoch == global_epoch) {
        operation = form->head_operation == SIZE_MAX ? NULL : &operations[form->head_operation];
        function = form->head_function == SIZE_MAX ? NULL : &functions[form->head_function];
    } else {
        operation = find_operation_text(head->text, head->text_length);
        function = operation == NULL ? find_function_owned(head->text, head->text_length, form->path) : NULL;
        /* Only a slot in this program's own table is cached; a closure owner's is looked up each time. */
        int own = function == NULL || (function >= functions && function < functions + function_count);
        if (own) {
            form->head_operation = operation == NULL ? SIZE_MAX : (size_t)(operation - operations);
            form->head_function = function == NULL ? SIZE_MAX : (size_t)(function - functions);
            form->head_epoch = global_epoch;
        }
    }
    if (function != NULL) {
        begin_function_call(form, function->value);
        return;
    }
    if (operation != NULL) {
        if (operation->arity_policy == FIXED_ARITY && argument_count != operation->arity) {
            fail_at(form->offset, "an operation received the wrong number of arguments");
        }
        Frame *frame = append_frame();
        frame->form = form;
        frame->argument_count = argument_count;
        frame->operation = operation;
        if (operation->id == OP_IF) frame->kind = FRAME_IF;
        else if (operation->id == OP_DO) frame->kind = FRAME_DO;
        else if (operation->id == OP_AND) frame->kind = FRAME_AND;
        else if (operation->id == OP_LET) frame->kind = FRAME_LET;
        else if (operation->id == OP_OR) frame->kind = FRAME_OR;
        else if (operation->id == OP_LAMBDA) frame->kind = FRAME_LAMBDA;
        else if (operation->id == OP_COALESCE) frame->kind = FRAME_COALESCE;
        else if (operation->id == OP_TRY || operation->id == OP_ATTEMPT) frame->kind = FRAME_TRY;
        else if (operation->id == OP_FAIL) frame->kind = FRAME_FAIL;
        else if (operation->id == OP_CHOICE) frame->kind = FRAME_CHOICE;
        else {
            frame->kind = FRAME_EAGER;
            frame->arguments = allocate_zeroed(argument_count, sizeof *frame->arguments);
        }
        return;
    }
    fail_at(head->offset, message_with_name(UNBOUND_PREFIX, head->text, head->text_length));
}

static void offer_form(Form *form) {
    if (form->kind != FORM_GROUP) {
        deliver(value_from_atom(form));
        return;
    }
    begin_call(form);
}

static void append_local(const char *bytes, size_t length, Value value) {
    size_t needed = checked_add(local_count, ONE_ELEMENT);
    locals = grow_list(locals, &local_capacity, sizeof *locals, needed);
    locals[local_count].bytes = bytes;
    locals[local_count].length = length;
    locals[local_count].value = value;
    local_count = needed;
}

static void step_series(size_t index, int arguments_only) {
    Frame frame = frames[index];
    size_t count = arguments_only ? call_argument_count(frame.form) : frame.form->part_count;
    if (frame.has_incoming) {
        frame.accumulator = frame.incoming;
        frame.has_accumulator = 1;
        frame.has_incoming = 0;
        frame.next_part = checked_add(frame.next_part, ONE_ELEMENT);
        frames[index] = frame;
        return;
    }
    if (frame.next_part >= count) {
        Value result = frame.has_accumulator ? frame.accumulator : nothing_value();
        pop_frame();
        deliver(result);
        return;
    }
    Form *child = arguments_only ? call_argument(frame.form, frame.next_part) : frame.form->parts[frame.next_part];
    frames[index] = frame;
    offer_form(child);
}

static void step_and(size_t index) {
    Frame frame = frames[index];
    if (frame.has_incoming) {
        int truth = integer_is_nonzero(frame.incoming);
        frame.has_incoming = 0;
        if (!truth) {
            pop_frame();
            deliver(integer_value(WORD_FALSE));
            return;
        }
        frame.next_part = checked_add(frame.next_part, ONE_ELEMENT);
        frames[index] = frame;
        return;
    }
    if (frame.next_part >= call_argument_count(frame.form)) {
        pop_frame();
        deliver(integer_value(WORD_TRUE));
        return;
    }
    Form *child = call_argument(frame.form, frame.next_part);
    frames[index] = frame;
    offer_form(child);
}

static void step_if(size_t index) {
    Frame frame = frames[index];
    if (frame.has_incoming && frame.stage == IF_STAGE_TEST) {
        Form *branch = integer_is_nonzero(frame.incoming)
            ? call_argument(frame.form, IF_CONSEQUENT)
            : call_argument(frame.form, IF_ALTERNATIVE);
        frame.has_incoming = 0;
        frame.stage = IF_STAGE_BRANCH;
        frames[index] = frame;
        offer_form(branch);
        return;
    }
    if (frame.has_incoming && frame.stage == IF_STAGE_BRANCH) {
        Value result = frame.incoming;
        pop_frame();
        deliver(result);
        return;
    }
    if (frame.stage != IF_STAGE_TEST) fail("an if branch was not offered");
    Form *test = call_argument(frame.form, IF_TEST);
    frames[index] = frame;
    offer_form(test);
}

static int tail_result_forwarded(Frame *frame) {
    if (frame->has_incoming) return 0;
    if (frame->kind == FRAME_FUNCTION) return 1;
    if (frame->kind == FRAME_IF || frame->kind == FRAME_COALESCE) return frame->stage == IF_STAGE_BRANCH;
    if (frame->kind == FRAME_SEQUENCE) return frame->next_part + 1 == frame->form->part_count;
    if (frame->kind == FRAME_DO) return frame->next_part + 1 == call_argument_count(frame->form);
    if (frame->kind == FRAME_LET) return frame->stage == 1 && frame->next_part + 1 == call_argument_count(frame->form);
    return 0;
}

static size_t retire_tail_caller(size_t index) {
    size_t target = index;
    while (target > 0 && tail_result_forwarded(&frames[target - 1])) {
        --target;
        if (frames[target].kind == FRAME_FUNCTION) {
            // Arguments and escaped closures have already been evaluated. Keep
            // their ownership while normal pops restore the outward program.
            Frame call = frames[index]; frames[index].arguments = NULL;
            size_t base = frames[target].local_base;
            while (frame_count > target) pop_frame();
            local_count = base;
            Frame *next = append_frame();
            next->form = call.form; next->callee = call.callee;
            next->arguments = call.arguments; next->argument_count = call.argument_count;
            return frame_count - 1;
        }
    }
    return index;
}

static void run_function_body(size_t index) {
    Frame frame = frames[index];
    if (frame.callee.native_operation != 0) {
        frame.operation = &operations[frame.callee.native_operation - 1];
        frame.kind = FRAME_EAGER; frame.next_part = frame.argument_count; frame.has_incoming = 0;
        frames[index] = frame;
        return;
    }
    index = retire_tail_caller(index);
    frame = frames[index];
    CellProgram *owner = frame.callee.program != NULL ? frame.callee.program : &main_program;
    if (owner != current_program) {
        frame.previous_program = current_program;
        frame.restore_program = 1;
        select_program(owner);
    }
    size_t base = local_count;
    for (size_t parameter = FIRST_INDEX; parameter < frame.callee.parameter_count; parameter = checked_add(parameter, ONE_ELEMENT)) {
        append_local(frame.callee.parameters[parameter].bytes, frame.callee.parameters[parameter].length, frame.arguments[parameter]);
    }
    frame.local_base = base;
    frame.kind = FRAME_FUNCTION;
    frame.has_incoming = 0;
    frames[index] = frame;
    frame.callee.body->entered_count += 1;
    offer_form(frame.callee.body);
}

static void step_fail(size_t index);

/* A value view can remain readable after its caller moves on. Only an absent
 * stored view needs the Form owner; immediate coordinates stay physical. */
static int field_view_needed(Value value) {
    if (value.kind != KIND_NODE || value.cell != NULL) return 0;
    fk_node_id id = (fk_node_id)value.integer;
    long long package = fk_nid_get(id, 0), level = fk_nid_get(id, 1), type = fk_nid_get(id, 2);
    return package == 0 || (package == 1 && level == 1 && (type == 2 || type == 5 || type == 7));
}

/* Reads the frame in place; the 552-byte copy is taken only when the call is
 * actually redirected to the field owner. */
static int enter_field_owner(size_t index, const Frame *view) {
    const Operation *op = view->operation; const Value *arg = view->arguments;
    const char *name = op->id == OP_FIELD ? op->spelling : NULL;
    Value args = nothing_value();
    if (op->offer == offer_kernel_live && arg[0].kind == KIND_INTEGER && arg[0].integer == (long long)getpid()) name = "kernel_live";
    if (op->offer == offer_kernel_stat && arg[0].kind == KIND_INTEGER) {
        long long key = arg[0].integer;
        if (key == 4 || key == 58 || key == 59 || key == 60 || key == 70 || key == 71 || key == 72 || (key>=79 && key<=86)) name = "kernel_stat";
    }
    if ((op->offer == offer_node_payload || op->offer == offer_node_category ||
         op->offer == offer_node_children || op->offer == offer_node_source ||
         op->offer == offer_float_value) && field_view_needed(arg[0])) name = op->spelling;
    if (op->offer == offer_node_source && arg[0].kind == KIND_NODE &&
        ((arg[0].cell != NULL && arg[0].cell->field_ref != 0) ||
         (arg[0].cell == NULL && fk_nid_get((fk_node_id)arg[0].integer,0)!=63))) name = "node_source";
    if (op->offer == offer_fb_record && arg[0].kind == KIND_NODE &&
        (arg[0].cell == NULL || arg[0].cell->field_ref != 0)) name = "fb_record";
    if (op->offer == offer_float_leaf && arg[0].kind == KIND_INTEGER) {
        long long mode = arg[0].integer;
        Value value = arg[1];
        if (mode == 2) { name = "make_float64"; args = list_from(&value, 1); }
        else if (mode == 0 && field_view_needed(value)) { name = "float_value"; args = list_from(&value, 1); }
        else if (mode == 35 && value_proper_list(value) && value.item_count == 2) {
            Value packet[] = {list_at(value,0), list_at(value,1), text_value("value",5)};
            Value decode[] = {integer_value(35), list_from(packet,3)};
            Value scalar = offer_float_leaf(decode,2);
            if (scalar.kind != KIND_NOTHING) {
                name = scalar.kind == KIND_INTEGER ? "intern_trivial_int" : "make_float64";
                args = list_from(&scalar,1);
            }
        }
    }
    if (name == NULL) return 0;
    Frame frame = frames[index];
    if (native_field_entry.kind != KIND_FUNCTION) fail("native field Form owner is not admitted");
    if (args.kind == KIND_NOTHING) args = list_from(frame.arguments, frame.argument_count);
    Value operation_name = text_value(name, strlen(name));
    Value *previous = frame.arguments;
    Value *replacement = allocate_zeroed(2, sizeof *replacement);
    replacement[0] = operation_name; replacement[1] = args;
    frame.arguments = replacement;
    frame.argument_count = 2; frame.callee = native_field_entry; frame.operation = NULL;
    frames[index] = frame;
    free(previous);
    run_function_body(index);
    return 1;
}

static void step_eager(size_t index) {
    /* The hot paths read and write the frame in place. The pointer is used only
     * before any call that can append a frame and so move the frame array. */
    Frame *live = &frames[index];
    if (live->has_incoming) {
        live->arguments[live->next_part] = live->incoming;
        live->has_incoming = 0;
        live->next_part = checked_add(live->next_part, ONE_ELEMENT);
        return;
    }
    if (live->next_part < live->argument_count) {
        Form *child = call_argument(live->form, live->next_part);
        offer_form(child);
        return;
    }
    if (live->operation == NULL) {
        run_function_body(index);
        return;
    }
    operations[(size_t)(live->operation - operations)].calls = checked_add(live->operation->calls, ONE_ELEMENT);
    if (enter_field_owner(index, live)) return;
    live = &frames[index];
    if (live->operation->id != OP_KERNEL_AST && live->operation->id != OP_METHOD_INVOKE && live->operation->id != OP_FORM_ERROR) {
        const Operation *operation = live->operation;
        Value result = operation->offer(live->arguments, live->argument_count);
        pop_frame();
        deliver(result);
        return;
    }
    Frame frame = frames[index];
    {
        if (frame.operation->id == OP_KERNEL_AST) {
            Value query = frame.arguments[1];
            Value command = query.kind == KIND_LIST ? list_at(query, 0) : nothing_value();
            if (command.kind == KIND_TEXT && command.text_length >= 7 && memcmp(command.text, "syntax-", 7) == 0) {
                Value value = kernel_syntax(frame.arguments);
                pop_frame(); deliver(value); return;
            }
            Binding *adapter = find_function_text("kas-query", 9);
            if (adapter == NULL) fail("kernel_ast: Form source view is unavailable");
            frame.callee = adapter->value;
            frame.operation = NULL;
            frames[index] = frame;
            run_function_body(index);
            return;
        }
        if (frame.operation->id == OP_METHOD_INVOKE) {
            if (frame.argument_count < 2) fail("method_invoke expects a receiver and name");
            expect_kind(frame.arguments[0], KIND_RECORD, "method_invoke expects a record receiver");
            ssize_t method = find_method(frame.arguments[0].record->blueprint, frame.arguments[1]);
            if (method < 0) fail("method_invoke could not find a method under that name");
            frame.callee = method_definitions[method].function;
            frame.argument_count -= 1;
            if (frame.argument_count != frame.callee.parameter_count) fail("a function received the wrong number of arguments");
            memmove(frame.arguments + 1, frame.arguments + 2, (frame.argument_count - 1) * sizeof *frame.arguments);
            frame.operation = NULL;
            frames[index] = frame;
            run_function_body(index);
            return;
        }
        if (frame.operation->id == OP_FORM_ERROR) {
            expect_kind(frame.arguments[0], KIND_TEXT, "form_error expects a message");
            failure_message = copy_bytes(frame.arguments[0].text, frame.arguments[0].text_length);
            failure_reported = 0;
            step_fail(index);
            return;
        }
    }
}

static void bind_global(const char *bytes, size_t length, Value value) {
    size_t needed = checked_add(global_count, ONE_ELEMENT);
    globals = grow_list(globals, &global_capacity, sizeof *globals, needed);
    globals[global_count].bytes = bytes;
    globals[global_count].length = length;
    globals[global_count].value = value;
    name_index_set(&global_names, bytes, length, global_count);
    global_count = needed;
    ++global_epoch;
}

static int inside_function(void) {
    for (size_t index = frame_count; index > FIRST_INDEX; ) {
        index -= ONE_ELEMENT;
        if (frames[index].kind == FRAME_FUNCTION) return 1;
    }
    return 0;
}

static void step_or(size_t index) {
    Frame frame = frames[index];
    if (frame.has_incoming) {
        int truth = integer_is_nonzero(frame.incoming);
        frame.has_incoming = 0;
        if (truth) {
            pop_frame();
            deliver(integer_value(WORD_TRUE));
            return;
        }
        frame.next_part = checked_add(frame.next_part, ONE_ELEMENT);
        frames[index] = frame;
        return;
    }
    if (frame.next_part >= call_argument_count(frame.form)) {
        pop_frame();
        deliver(integer_value(WORD_FALSE));
        return;
    }
    Form *child = call_argument(frame.form, frame.next_part);
    frames[index] = frame;
    offer_form(child);
}

static Value closure_value(Form *parameters, Form *body, size_t offset) {
    if (parameters->kind != FORM_GROUP) fail_at(parameters->offset, "a lambda parameter list must be a group");
    Value function = nothing_value();
    function.kind = KIND_FUNCTION;
    function.program = current_program;
    function.body = body;
    function.parameter_count = parameters->part_count;
    function.parameters = heap_allocate(parameters->part_count,sizeof *function.parameters,HEAP_PARAMETERS,parameters->part_count);
    for (size_t parameter = FIRST_INDEX; parameter < parameters->part_count; parameter = checked_add(parameter, ONE_ELEMENT)) {
        Form *name = parameters->parts[parameter];
        if (name->kind != FORM_SYMBOL) fail_at(name->offset, "a lambda parameter must be a symbol");
        for (size_t earlier = 0; earlier < parameter; ++earlier) {
            Form *previous = parameters->parts[earlier];
            if (bytes_equal(previous->text, previous->text_length, name->text, name->text_length)) fail_at(name->offset, "a parameter name is repeated");
        }
        function.parameters[parameter].bytes = name->text;
        function.parameters[parameter].length = name->text_length;
    }
    size_t base = FIRST_INDEX;
    Binding *outer = NULL;
    size_t outer_count = EMPTY_COUNT;
    for (size_t cursor = frame_count; cursor > FIRST_INDEX; ) {
        cursor -= ONE_ELEMENT;
        if (frames[cursor].kind != FRAME_FUNCTION) continue;
        base = frames[cursor].local_base;
        outer = frames[cursor].callee.captures;
        outer_count = frames[cursor].callee.capture_count;
        break;
    }
    if (base > local_count) fail_at(offset, "a lambda looked above its locals");
    size_t visible = local_count - base;
    size_t total = checked_add(outer_count, visible);
    if (total != EMPTY_COUNT) {
        Binding *captures = heap_allocate(total,sizeof *captures,HEAP_CAPTURES,total);
        if (outer_count != EMPTY_COUNT) memcpy(captures, outer, outer_count * sizeof *captures);
        if (visible != EMPTY_COUNT) memcpy(captures + outer_count, locals + base, visible * sizeof *captures);
        function.captures = captures;
        function.capture_count = total;
    }
    return function;
}

static void step_lambda(size_t index) {
    Frame frame = frames[index];
    if (call_argument_count(frame.form) != ARITY_TWO) fail_at(frame.form->offset, "a lambda names parameters and a body");
    Value function = closure_value(call_argument(frame.form, LEFT_OPERAND), call_argument(frame.form, RIGHT_OPERAND), frame.form->offset);
    pop_frame();
    deliver(function);
}

static void step_coalesce(size_t index) {
    Frame frame = frames[index];
    if (!frame.has_incoming) {
        Form *child = frame.stage == IF_STAGE_TEST
            ? call_argument(frame.form, LEFT_OPERAND)
            : call_argument(frame.form, RIGHT_OPERAND);
        frames[index] = frame;
        offer_form(child);
        return;
    }
    if (frame.stage == IF_STAGE_TEST && frame.incoming.kind == KIND_NOTHING) {
        frame.has_incoming = 0;
        frame.stage = IF_STAGE_BRANCH;
        frames[index] = frame;
        offer_form(call_argument(frame.form, RIGHT_OPERAND));
        return;
    }
    Value value = frame.incoming;
    pop_frame();
    deliver(value);
}

static void step_choice(size_t index) {
    Frame frame = frames[index];
    if (!frame.has_accumulator) {
        frame.local_base = local_count;
        frame.prior_failure = failure_message;
        frame.prior_failure_reported = failure_reported;
        frame.has_accumulator = 1;
        frames[index] = frame;
    }
    if (frame.has_incoming) {
        Value value = frame.incoming;
        frame.has_incoming = 0;
        if (value.kind != KIND_NOTHING) {
            local_count = frame.local_base;
            failure_message = frame.prior_failure;
            failure_reported = frame.prior_failure_reported;
            pop_frame();
            deliver(value);
            return;
        }
        frame.stage = checked_add(frame.stage, ONE_ELEMENT);
        frames[index] = frame;
    }
    if (frame.stage >= call_argument_count(frame.form)) {
        local_count = frame.local_base;
        failure_message = frame.prior_failure;
        failure_reported = frame.prior_failure_reported;
        pop_frame();
        deliver(nothing_value());
        return;
    }
    Form *child = call_argument(frame.form, frame.stage);
    failure_message = frame.prior_failure;
    failure_reported = frame.prior_failure_reported;
    frames[index] = frame;
    offer_form(child);
}

static void step_try(size_t index) {
    Frame frame = frames[index];
    if (frame.stage == IF_STAGE_TEST) {
        frame.local_base = local_count;
        frame.prior_failure = failure_message;
        frame.prior_failure_reported = failure_reported;
        frame.stage = TRY_STAGE_BODY;
        frames[index] = frame;
        offer_form(call_argument(frame.form, LEFT_OPERAND));
        return;
    }
    if (frame.has_incoming) {
        Value value = frame.incoming;
        local_count = frame.local_base;
        failure_message = frame.prior_failure;
        failure_reported = frame.prior_failure_reported;
        if (value.kind == KIND_NOTHING && frame.stage == TRY_STAGE_BODY &&
            (frame.operation == NULL || frame.operation->id != OP_ATTEMPT)) {
            frame.has_incoming = 0;
            frame.stage = TRY_STAGE_HANDLER;
            stage_unwind(frame.stage_base, 3, "nothing");
            frames[index] = frame;
            return;
        }
        pop_frame();
        deliver(value);
        return;
    }
    if (frame.stage == TRY_STAGE_HANDLER) {
        if (frame.operation != NULL && frame.operation->id == OP_ATTEMPT) {
            local_count = frame.local_base;
            failure_message = frame.prior_failure;
            failure_reported = frame.prior_failure_reported;
            pop_frame();
            deliver(nothing_value());
            return;
        }
        frames[index] = frame;
        offer_form(call_argument(frame.form, RIGHT_OPERAND));
        return;
    }
    fail("a try did not offer a result");
}

static void step_fail(size_t index) {
    (void)index;
    if (failure_message != NULL && !failure_reported) {
        fprintf(runtime_error(), ERROR_BARE, failure_message);
        failure_reported = 1;
    }
    for (size_t cursor = frame_count; cursor > FIRST_INDEX; ) {
        cursor -= ONE_ELEMENT;
        if (frames[cursor].kind == FRAME_CHOICE) {
            frames[cursor].has_incoming = 0;
            frames[cursor].stage = checked_add(frames[cursor].stage, ONE_ELEMENT);
            while (frame_count > cursor + 1) pop_frame();
            local_count = frames[cursor].local_base;
            stage_unwind(frames[cursor].stage_base, 3, failure_message);
            return;
        }
        if (frames[cursor].kind != FRAME_TRY || frames[cursor].stage == TRY_STAGE_HANDLER) continue;
        frames[cursor].has_incoming = 0;
        frames[cursor].stage = TRY_STAGE_HANDLER;
        while (frame_count > cursor + 1) pop_frame();
        local_count = frames[cursor].local_base;
        stage_unwind(frames[cursor].stage_base, 3, failure_message);
        return;
    }
    evaluation_recovery = NULL;
    if (active_cell != NULL) cell_stop("stop", failure_message != NULL ? failure_message : "a failure was not caught");
    if (failure_reported) { note_stack(); note_failure_owner(failure_message != NULL ? failure_message : "a failure was not caught"); exit(EXIT_FAILURE); }
    fail(failure_message != NULL ? failure_message : "a failure was not caught");
}

static void step_apply(size_t index) {
    Frame frame = frames[index];
    if (frame.has_incoming) {
        if (frame.stage == IF_STAGE_TEST) {
            if (frame.incoming.kind != KIND_FUNCTION) fail_at(frame.form->parts[CALL_HEAD]->offset, "a call head is not a function");
            size_t argument_count = call_argument_count(frame.form);
            if (!function_accepts(frame.incoming, argument_count)) fail_at(frame.form->offset, "a function received the wrong number of arguments");
            frame.callee = frame.incoming;
            frame.argument_count = argument_count;
            frame.arguments = allocate_zeroed(argument_count, sizeof *frame.arguments);
        } else {
            frame.arguments[frame.stage - ONE_ELEMENT] = frame.incoming;
        }
        frame.has_incoming = 0;
        frame.stage = checked_add(frame.stage, ONE_ELEMENT);
        frames[index] = frame;
        if (frame.stage >= frame.form->part_count) run_function_body(index);
        return;
    }
    Form *child = frame.form->parts[frame.stage];
    frames[index] = frame;
    offer_form(child);
}

static void step_let(size_t index) {
    Frame frame = frames[index];
    size_t count = call_argument_count(frame.form);
    if (count < ARITY_TWO) fail_at(frame.form->offset, "let expects a name, value and optional body");
    if (frame.stage == 1 && frame.has_incoming) {
        if (++frame.next_part < count) {
            frame.has_incoming = 0;
            frames[index] = frame;
            offer_form(call_argument(frame.form, frame.next_part));
            return;
        }
        Value result = frame.incoming;
        pop_frame();
        deliver(result);
        return;
    }
    if (!frame.has_incoming) {
        frames[index] = frame;
        offer_form(call_argument(frame.form, RIGHT_OPERAND));
        return;
    }
    Form *name = call_argument(frame.form, LEFT_OPERAND);
    if (name->kind != FORM_SYMBOL) fail_at(name->offset, "a let name must be a symbol");
    if (count > ARITY_TWO) {
        frame.local_base = local_count;
        append_local(name->text, name->text_length, frame.incoming);
        frame.stage = 1;
        frame.next_part = 2;
        frame.has_incoming = 0;
        frames[index] = frame;
        offer_form(call_argument(frame.form, 2));
        return;
    }
    if (inside_function()) append_local(name->text, name->text_length, frame.incoming);
    else bind_global(name->text, name->text_length, frame.incoming);
    Value value = frame.incoming;
    pop_frame();
    deliver(value);
}

static void step_function(size_t index) {
    Frame frame = frames[index];
    if (!frame.has_incoming) fail("a function body was not offered");
    Value result = frame.incoming;
    local_count = frame.local_base;
    pop_frame();
    deliver(result);
}

static void drive(Form *program) {
    jmp_buf recovery;
    jmp_buf *previous_recovery = evaluation_recovery;
    Frame *frame = append_frame();
    frame->kind = FRAME_SEQUENCE;
    frame->form = program;
    if (setjmp(recovery) != 0) {
        evaluation_recovery = NULL;
        step_fail(frame_count - 1);
    }
    evaluation_recovery = &recovery;
    while (frame_count != EMPTY_COUNT) {
        // Native constructors have returned and every evaluator value is now
        // visible in frames/owners. Long Form operations can release temporary
        // values here without waiting for their enclosing cell to finish.
        if (heap_collection_due) heap_collect(nothing_value());
        // Preserve the native walker's documented 16384-step polling cadence.
        // This samples the caller's deadline; it never limits the work allowed.
        // Public timestamps stay in CLOCK_MONOTONIC across process renewals.
        if (active_cell != NULL && active_cell->deadline != 0 &&
            (active_cell->deadline_steps++ & 16383u) == 0 &&
            monotonic_ns() / NANOSECONDS_PER_MILLISECOND >= active_cell->deadline)
            cell_stop("deadline", "cell deadline reached");
        size_t index = frame_count - ONE_ELEMENT;
        FrameKind kind = frames[index].kind;
        if (kind == FRAME_SEQUENCE) step_series(index, 0);
        else if (kind == FRAME_DO) step_series(index, 1);
        else if (kind == FRAME_AND) step_and(index);
        else if (kind == FRAME_IF) step_if(index);
        else if (kind == FRAME_OR) step_or(index);
        else if (kind == FRAME_EAGER) step_eager(index);
        else if (kind == FRAME_FUNCTION) step_function(index);
        else if (kind == FRAME_LET) step_let(index);
        else if (kind == FRAME_LAMBDA) step_lambda(index);
        else if (kind == FRAME_COALESCE) step_coalesce(index);
        else if (kind == FRAME_TRY) step_try(index);
        else if (kind == FRAME_CHOICE) step_choice(index);
        else if (kind == FRAME_FAIL) step_fail(index);
        else if (kind == FRAME_APPLY) step_apply(index);
        else fail("an evaluation frame has an unknown kind");
    }
    if (!result_ready) fail("the program produced no result");
    evaluation_recovery = previous_recovery;
}

typedef struct {
    Value value;
    size_t index;
    int opened;
} PrintItem;

static void print_real(double value, FILE *output) {
    if (isnan(value)) { fputs("NaN", output); return; }
    if (isinf(value)) { fputs(signbit(value) ? "-Infinity" : "Infinity", output); return; }
    char text[64];
    int precision = 1;
    for (; precision <= DBL_DECIMAL_DIG; ++precision) {
        snprintf(text, sizeof text, "%.*g", precision, value);
        double restored = strtod(text, NULL);
        if (memcmp(&restored, &value, sizeof value) == 0) break;
    }
    char *exponent = strchr(text, 'e');
    if (exponent != NULL && atoi(exponent + 1) >= -6 && atoi(exponent + 1) < 21) {
        int places = precision - 1 - atoi(exponent + 1);
        snprintf(text, sizeof text, "%.*f", places > 0 ? places : 0, value);
        exponent = NULL;
    }
    if (exponent != NULL) {
        char *digits = exponent + 1, *write = digits;
        if (*digits == '-' || *digits == '+') *write++ = *digits++;
        while (*digits == '0' && digits[1]) ++digits;
        memmove(write, digits, strlen(digits) + 1);
    }
    fputs(text, output);
}

static void print_atom(Value value, FILE *output) {
    if (value.kind == KIND_NOTHING) { fputs("null", output); return; }
    if (value.kind == KIND_NODE) {
        fk_node_id id = node_word(value, "node rendering expects a node");
        fprintf(output, "@%lld.%lld.%lld.%lld", fk_nid_get(id, 0), fk_nid_get(id, 1), fk_nid_get(id, 2), fk_nid_get(id, 3));
        return;
    }
    if (value.kind == KIND_INTEGER) {
        fprintf(output, INTEGER_FORMAT, value.integer);
        return;
    }
    if (value.kind == KIND_REAL) {
        print_real(value.real, output);
        return;
    }
    if (value.kind == KIND_TEXT) {
        fwrite(value.text, sizeof *value.text, value.text_length, output);
        return;
    }
    fprintf(output, "<%s>", kind_name(value.kind));
}

static void print_value_to(Value root, FILE *output) {
    if (root.kind == KIND_NOTHING) return;
    PrintItem *items = NULL;
    size_t item_count = EMPTY_COUNT;
    size_t item_capacity = EMPTY_COUNT;
    size_t needed = checked_add(item_count, ONE_ELEMENT);
    items = grow_list(items, &item_capacity, sizeof *items, needed);
    items[item_count].value = root;
    items[item_count].index = FIRST_INDEX;
    items[item_count].opened = 0;
    item_count = needed;
    while (item_count != EMPTY_COUNT) {
        PrintItem item = items[item_count - ONE_ELEMENT];
        if (item.value.kind != KIND_LIST) {
            print_atom(item.value, output);
            item_count -= ONE_ELEMENT;
            continue;
        }
        if (!item.opened) {
            fputs(LIST_OPEN, output);
            item.opened = 1;
            item.index = FIRST_INDEX;
            items[item_count - ONE_ELEMENT] = item;
            if (item.value.item_count == EMPTY_COUNT) {
                fputs(LIST_CLOSE, output);
                item_count -= ONE_ELEMENT;
            }
            continue;
        }
        if (item.index == item.value.item_count) {
            fputs(LIST_CLOSE, output);
            item_count -= ONE_ELEMENT;
            continue;
        }
        if (item.index > FIRST_INDEX) fputs(LIST_SEPARATOR, output);
        Value child = list_at(item.value, item.index);
        item.index = checked_add(item.index, ONE_ELEMENT);
        items[item_count - ONE_ELEMENT] = item;
        needed = checked_add(item_count, ONE_ELEMENT);
        items = grow_list(items, &item_capacity, sizeof *items, needed);
        items[item_count].value = child;
        items[item_count].index = FIRST_INDEX;
        items[item_count].opened = 0;
        item_count = needed;
    }
    free(items);
}

static void print_value(Value root) {
    print_value_to(root, runtime_output());
}

typedef struct {
    const char *bytes;
    size_t length;
    size_t offset;
} SourceCursor;

static int file_is_source(const char *path) {
    struct stat info;
    if (stat(path, &info) != SYSTEM_CALL_OK) return 0;
    return S_ISREG(info.st_mode);
}

static char *join_path(const char *left, const char *right) {
    size_t left_length = strlen(left);
    size_t right_length = strlen(right);
    int need_slash = left_length != EMPTY_COUNT && left[left_length - ONE_ELEMENT] != PATH_SEPARATOR;
    size_t slash = need_slash ? ONE_ELEMENT : EMPTY_COUNT;
    if (left_length > SIZE_MAX - slash || left_length + slash > SIZE_MAX - right_length) fail("a path exceeds addressable memory");
    size_t total = left_length + slash + right_length;
    char *joined = malloc(checked_add(total, TEXT_TERMINATOR_SIZE));
    if (joined == NULL) fail("out of memory");
    if (left_length != EMPTY_COUNT) memcpy(joined, left, left_length);
    if (need_slash) joined[left_length] = PATH_SEPARATOR;
    if (right_length != EMPTY_COUNT) memcpy(joined + left_length + slash, right, right_length);
    joined[total] = TEXT_TERMINATOR;
    return joined;
}

static char *directory_of(const char *path) {
    const char *slash = strrchr(path, PATH_SEPARATOR);
    if (slash == NULL || slash == path) return copy_bytes("", EMPTY_COUNT);
    return copy_bytes(path, (size_t)(slash - path));
}

static int is_path_char(unsigned char character) {
    if (isalnum(character)) return 1;
    return character == PATH_SEPARATOR || character == DECIMAL_POINT || character == '_' || character == SIGN_MINUS || character == SIGN_PLUS;
}

static int token_is_none(const char *bytes, size_t length) {
    return bytes_equal_cstr(bytes, length, "NONE");
}

static int same_path(const char *left, const char *right) {
    return strcmp(left, right) == STRING_COMPARE_EQUAL;
}

static char *try_resolve_local_unit(const char *from_file, const char *relative) {
    if (file_is_source(relative)) return copy_bytes(relative, strlen(relative));
    char *under_form = join_path(FORM_ROOT, relative);
    if (file_is_source(under_form)) return under_form;
    free(under_form);
    char *directory = copy_bytes(from_file, strlen(from_file));
    while (directory[FIRST_INDEX] != TEXT_TERMINATOR) {
        char *parent = directory_of(directory);
        free(directory);
        if (parent[FIRST_INDEX] == TEXT_TERMINATOR || same_path(parent, from_file)) {
            free(parent);
            break;
        }
        char *beside = join_path(parent, relative);
        if (file_is_source(beside)) {
            free(parent);
            return beside;
        }
        free(beside);
        char *root = join_path(parent, FORM_ROOT);
        char *nested = join_path(root, relative);
        free(root);
        if (file_is_source(nested)) {
            free(parent);
            return nested;
        }
        free(nested);
        directory = parent;
    }
    /* Preserve every lexical/local selection before extending a bare or
     * relative entry's search above cwd. Form source admission carries this
     * policy as the bootstrap resolver moves into the native walker. */
    if (from_file[0] != PATH_SEPARATOR) {
        char *cwd = getcwd(NULL, 0);
        if (cwd == NULL) return NULL;
        char *absolute = join_path(cwd, from_file); free(cwd);
        char *resolved = try_resolve_local_unit(absolute, relative); free(absolute);
        return resolved;
    }
    return NULL;
}

static char *try_resolve_unit(const char *from_file, const char *relative) {
    char *local = try_resolve_local_unit(from_file, relative);
    if (local != NULL) return local;
    /* Scratch callers keep their own sources first. Only a missing library
     * falls back to the executing body's home, never a second runtime. */
    char *executable = executing_image_path();
    if (executable == NULL) return NULL;
    char *resolved = try_resolve_local_unit(executable, relative);
    free(executable);
    return resolved;
}

static char *resolve_unit(const char *from_file, const char *relative) {
    char *path = try_resolve_unit(from_file, relative);
    if (path == NULL) fail(message_with_name("a source could not be opened: ", relative, strlen(relative)));
    return path;
}

static int unit_is_loaded(const char *path) {
    for (size_t index = FIRST_INDEX; index < loaded_count; index = checked_add(index, ONE_ELEMENT)) {
        if (same_path(loaded_units[index], path)) return 1;
    }
    return 0;
}

static void remember_unit(char *path) {
    size_t needed = checked_add(loaded_count, ONE_ELEMENT);
    loaded_units = grow_list(loaded_units, &loaded_capacity, sizeof *loaded_units, needed);
    loaded_units[loaded_count] = path;
    loaded_count = needed;
}

static SourceCursor capture_source(void) {
    SourceCursor cursor;
    cursor.bytes = source_bytes;
    cursor.length = source_length;
    cursor.offset = source_offset;
    return cursor;
}

static void restore_source(SourceCursor cursor) {
    source_bytes = cursor.bytes;
    source_length = cursor.length;
    source_offset = cursor.offset;
}

static int is_symbol_char(char character) {
    unsigned char classified = (unsigned char)character;
    if (isalnum(classified)) return 1;
    return character == '_' || character == SIGN_MINUS || character == TERNARY_MARK || character == '!';
}

static int at_keyword(const char *word) {
    size_t length = strlen(word);
    if (source_offset > source_length || source_length - source_offset < length) return 0;
    if (memcmp(source_bytes + source_offset, word, length) != STRING_COMPARE_EQUAL) return 0;
    size_t next = source_offset + length;
    if (next < source_length && is_symbol_char(source_bytes[next])) return 0;
    return 1;
}

static int take_keyword(const char *word) {
    skip_bml();
    if (!at_keyword(word)) return 0;
    source_offset = checked_add(source_offset, strlen(word));
    return 1;
}

static int take_declaration_keyword(const char *word, int pattern) {
    size_t saved = source_offset;
    if (!take_keyword(word)) return 0;
    skip_bml();
    if (source_offset < source_length &&
        ((is_symbol_char(source_bytes[source_offset]) && !is_digit_char(source_bytes[source_offset])) ||
         (pattern && source_bytes[source_offset] == INDEX_OPEN))) return 1;
    source_offset = saved;
    return 0;
}

static void expect_character(char expected, const char *reason) {
    skip_bml();
    if (source_offset >= source_length || source_bytes[source_offset] != expected) fail_at(source_offset, reason);
    source_offset = checked_add(source_offset, ONE_ELEMENT);
}

static Form *form_integer(long long integer, size_t offset) {
    Form *form = new_form(FORM_INTEGER, offset);
    form->integer = integer;
    return form;
}

static Form *form_symbol(const char *spelling, size_t offset) {
    Form *form = new_form(FORM_SYMBOL, offset);
    form->text = copy_bytes(spelling, strlen(spelling));
    form->text_length = strlen(spelling);
    return form;
}

static Form *form_text(const char *bytes, size_t length, size_t offset) {
    Form *form = new_form(FORM_TEXT, offset);
    form->text = copy_bytes(bytes, length);
    form->text_length = length;
    return form;
}

static Form *symbol_text(Form *name) {
    return form_text(name->text, name->text_length, name->offset);
}

static Form *form_call(const char *name, Form **arguments, size_t argument_count, size_t offset) {
    Form *call = new_form(FORM_GROUP, offset);
    group_add(call, form_symbol(name, offset));
    for (size_t index = FIRST_INDEX; index < argument_count; index = checked_add(index, ONE_ELEMENT)) {
        group_add(call, arguments[index]);
    }
    return call;
}

static Form *pipe_call(Form *input, Form *stage) {
    if (stage->kind == FORM_GROUP && stage->part_count != EMPTY_COUNT) {
        group_add(stage, input);
        return stage;
    }
    if (stage->kind == FORM_SYMBOL) {
        Form *call = new_form(FORM_GROUP, stage->offset);
        group_add(call, stage);
        group_add(call, input);
        return call;
    }
    fail_at(stage->offset, "a pipe stage must be a call");
    return stage;
}

static Form *form_call_head(Form *head, Form **arguments, size_t argument_count) {
    Form *call = new_form(FORM_GROUP, head->offset);
    group_add(call, head);
    for (size_t index = FIRST_INDEX; index < argument_count; index = checked_add(index, ONE_ELEMENT)) {
        group_add(call, arguments[index]);
    }
    return call;
}

static void append_form(Form ***forms, size_t *count, size_t *capacity, Form *form) {
    size_t needed = checked_add(*count, ONE_ELEMENT);
    *forms = grow_list(*forms, capacity, sizeof **forms, needed);
    (*forms)[*count] = form;
    *count = needed;
}

static Form *parse_bml_expression(void);
static void parse_bml_statements(Form ***forms, size_t *count, size_t *capacity);

static Form *read_bml_symbol(void) {
    skip_bml();
    size_t start = source_offset;
    if (start >= source_length || !is_symbol_char(source_bytes[start]) || is_digit_char(source_bytes[start])) {
        fail_at(start, "a name was expected");
    }
    while (source_offset < source_length && is_symbol_char(source_bytes[source_offset])) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
    }
    size_t length = source_offset - start;
    Form *form = new_form(FORM_SYMBOL, start);
    form->text = copy_bytes(source_bytes + start, length);
    form->text_length = length;
    return form;
}

static Form *read_bml_number(void) {
    size_t start = source_offset;
    while (source_offset < source_length && is_digit_char(source_bytes[source_offset])) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
    }
    if (source_offset < source_length && source_bytes[source_offset] == DECIMAL_POINT) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        while (source_offset < source_length && is_digit_char(source_bytes[source_offset])) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
        }
    }
    if (source_offset < source_length && (source_bytes[source_offset] == EXPONENT_LOWER || source_bytes[source_offset] == EXPONENT_UPPER)) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        if (source_offset < source_length && (source_bytes[source_offset] == SIGN_PLUS || source_bytes[source_offset] == SIGN_MINUS)) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
        }
        size_t digits = source_offset;
        while (source_offset < source_length && is_digit_char(source_bytes[source_offset])) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
        }
        if (source_offset == digits) fail_at(start, "a numeric exponent needs digits");
    }
    size_t length = source_offset - start;
    char *text = copy_bytes(source_bytes + start, length);
    Form *form = new_form(FORM_SYMBOL, start);
    if (!token_is_number(text, length, form)) fail_at(start, "a number was not readable");
    return form;
}

static const char *take_compare_operator(void) {
    skip_bml();
    if (source_offset >= source_length) return NULL;
    char character = source_bytes[source_offset];
    char following = TEXT_TERMINATOR;
    size_t next = checked_add(source_offset, ONE_ELEMENT);
    if (next < source_length) following = source_bytes[next];
    if (character == NAME_MARK && following == NAME_MARK) {
        source_offset = checked_add(next, ONE_ELEMENT);
        return "eq";
    }
    if (character == '>' && following == NAME_MARK) {
        source_offset = checked_add(next, ONE_ELEMENT);
        return "ge";
    }
    if (character == '<' && following == NAME_MARK) {
        source_offset = checked_add(next, ONE_ELEMENT);
        return "le";
    }
    if (character == '>') {
        source_offset = next;
        return "gt";
    }
    if (character == '<') {
        source_offset = next;
        return "lt";
    }
    return NULL;
}

static const char *take_sum_operator(void) {
    skip_bml();
    if (source_offset >= source_length) return NULL;
    char character = source_bytes[source_offset];
    if (character == SIGN_PLUS) {
        size_t next = checked_add(source_offset, ONE_ELEMENT);
        if (next < source_length && source_bytes[next] == SIGN_PLUS) {
            source_offset = checked_add(next, ONE_ELEMENT);
            return "str_concat";
        }
        source_offset = next;
        return "+";
    }
    if (character == SIGN_MINUS) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        return "sub";
    }
    return NULL;
}

static Form *parse_bml_list(void) {
    size_t opening = source_offset;
    expect_character(INDEX_OPEN, "a list was not opened");
    Form **items = NULL;
    size_t item_count = EMPTY_COUNT;
    size_t item_capacity = EMPTY_COUNT;
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == INDEX_CLOSE) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        return form_call("list", NULL, EMPTY_COUNT, opening);
    }
    while (1) {
        append_form(&items, &item_count, &item_capacity, parse_bml_expression());
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == INDEX_CLOSE) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            break;
        }
        expect_character(ARGUMENT_SEPARATOR, "a list value was not separated");
    }
    return form_call("list", items, item_count, opening);
}

static Form *parse_bml_arguments(void) {
    expect_character(CALL_OPEN, "a call was not opened");
    Form *group = new_form(FORM_GROUP, source_offset);
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == CALL_CLOSE) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        return group;
    }
    while (1) {
        group_add(group, parse_bml_expression());
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == CALL_CLOSE) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            break;
        }
        expect_character(ARGUMENT_SEPARATOR, "a call argument was not separated");
    }
    return group;
}

static Form *parse_bml_postfix(void);
static void skip_type_arguments(void);
static int take_arrow(void);

static Form *parse_bml_blueprint_type(void) {
    Form *name = read_bml_symbol();
    Form *types = form_call("list", NULL, 0, name->offset);
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == '<') {
        ++source_offset;
        skip_bml();
        while (source_offset < source_length && source_bytes[source_offset] != '>') {
            group_add(types, parse_bml_blueprint_type());
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == '>') break;
            expect_character(ARGUMENT_SEPARATOR, "a blueprint type was not separated");
        }
        expect_character('>', "a blueprint type was not closed");
    }
    Form *args[] = {symbol_text(name), types};
    return form_call("bl-ty", args, 2, name->offset);
}

static Form *parse_bml_blueprint_fields(int declaration) {
    size_t opening = source_offset;
    expect_character(SECTION_OPEN, "blueprint fields were not opened");
    Form *fields = form_call("list", NULL, 0, opening);
    skip_bml();
    while (source_offset < source_length && source_bytes[source_offset] != SECTION_CLOSE) {
        Form *field = read_bml_symbol();
        expect_character(PAIR_MARK, "a blueprint field needs its value");
        Form *value = declaration ? parse_bml_blueprint_type() : parse_bml_expression();
        Form *pair[] = {symbol_text(field), value};
        group_add(fields, form_call("list", pair, 2, field->offset));
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == SECTION_CLOSE) break;
        expect_character(ARGUMENT_SEPARATOR, "a blueprint field was not separated");
        skip_bml();
    }
    expect_character(SECTION_CLOSE, "blueprint fields were not closed");
    return fields;
}

static int at_bml_blueprint_literal(void) {
    size_t saved = source_offset;
    skip_bml();
    int present = 0;
    if (source_offset < source_length && source_bytes[source_offset] == SECTION_OPEN) {
        ++source_offset;
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == SECTION_CLOSE) present = 1;
        else if (source_offset < source_length && is_symbol_char(source_bytes[source_offset]) && !is_digit_char(source_bytes[source_offset])) {
            read_bml_symbol();
            skip_bml();
            present = source_offset < source_length && source_bytes[source_offset] == PAIR_MARK;
        }
    }
    source_offset = saved;
    return present;
}

static Form *parse_parenthesized_lambda(void) {
    size_t saved = source_offset;
    if (source_offset >= source_length || source_bytes[source_offset] != CALL_OPEN) return NULL;
    source_offset = checked_add(source_offset, ONE_ELEMENT);
    Form *parameters = new_form(FORM_GROUP, saved);
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == CALL_CLOSE) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        if (!take_arrow()) {
            source_offset = saved;
            return NULL;
        }
        return parameters;
    }
    while (1) {
        skip_bml();
        if (source_offset >= source_length || !is_symbol_char(source_bytes[source_offset]) || is_digit_char(source_bytes[source_offset])) {
            source_offset = saved;
            return NULL;
        }
        group_add(parameters, read_bml_symbol());
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == PAIR_MARK) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            skip_bml();
            if (source_offset >= source_length || !is_symbol_char(source_bytes[source_offset]) || is_digit_char(source_bytes[source_offset])) {
                source_offset = saved;
                return NULL;
            }
            read_bml_symbol();
            skip_type_arguments();
            skip_bml();
        }
        if (source_offset < source_length && source_bytes[source_offset] == CALL_CLOSE) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            if (!take_arrow()) {
                source_offset = saved;
                return NULL;
            }
            return parameters;
        }
        if (source_offset >= source_length || source_bytes[source_offset] != ARGUMENT_SEPARATOR) {
            source_offset = saved;
            return NULL;
        }
        source_offset = checked_add(source_offset, ONE_ELEMENT);
    }
}

static Form *parse_bml_primary(void) {
    skip_bml();
    if (source_offset >= source_length) fail_at(source_offset, "an expression ended early");
    char character = source_bytes[source_offset];
    if (character == TEXT_DELIMITER) return parse_text();
    if (is_digit_char(character)) return read_bml_number();
    if (character == NEGATION_MARK) {
        size_t next = checked_add(source_offset, ONE_ELEMENT);
        if (next >= source_length || source_bytes[next] != NAME_MARK) {
            size_t mark = source_offset;
            source_offset = next;
            Form *inner = parse_bml_postfix();
            Form *arguments[] = {inner};
            return form_call("not", arguments, ARITY_ONE, mark);
        }
    }
    if (character == SIGN_MINUS) {
        size_t mark = source_offset;
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        skip_bml();
        if (source_offset < source_length && (is_digit_char(source_bytes[source_offset]) || source_bytes[source_offset] == DECIMAL_POINT)) {
            Form *number = read_bml_number();
            if (number->kind == FORM_INTEGER) number->integer = checked_integer_sub(WORD_FALSE, number->integer);
            else {
                number->real = -number->real;
            }
            return number;
        }
        Form *inner = parse_bml_postfix();
        Form *arguments[] = {form_integer(WORD_FALSE, mark), inner};
        return form_call("sub", arguments, ARITY_TWO, mark);
    }
    if (character == INDEX_OPEN) return parse_bml_list();
    if (character == SECTION_OPEN) {
        size_t mark = source_offset;
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        Form **statements = NULL;
        size_t statement_count = EMPTY_COUNT;
        size_t statement_capacity = EMPTY_COUNT;
        parse_bml_statements(&statements, &statement_count, &statement_capacity);
        return form_call("do", statements, statement_count, mark);
    }
    if (character == CALL_OPEN) {
        Form *lambda_parameters = parse_parenthesized_lambda();
        if (lambda_parameters != NULL) {
            Form *body = parse_bml_expression();
            Form *lambda_arguments[] = {lambda_parameters, body};
            return form_call("lambda", lambda_arguments, ARITY_TWO, lambda_parameters->offset);
        }
        size_t saved = source_offset;
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        skip_bml();
        if (source_offset < source_length && is_symbol_char(source_bytes[source_offset]) && !is_digit_char(source_bytes[source_offset])) {
            size_t name_at = source_offset;
            while (source_offset < source_length && is_symbol_char(source_bytes[source_offset])) {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
            }
            size_t name_length = source_offset - name_at;
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == CALL_CLOSE && isupper((unsigned char)source_bytes[name_at])) {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
                char *type_name = copy_bytes(source_bytes + name_at, name_length);
                Form *inner = parse_bml_postfix();
                inner->annotation = form_symbol(type_name, name_at);
                free(type_name);
                return inner;
            }
            (void)name_length;
        }
        source_offset = saved;
        expect_character(CALL_OPEN, "a group was not opened");
        Form *inner = parse_bml_expression();
        expect_character(CALL_CLOSE, "a group was not closed");
        return inner;
    }
    if (is_symbol_char(character) && !is_digit_char(character)) {
        Form *name = read_bml_symbol();
        if (!at_bml_blueprint_literal()) return name;
        Form *args[] = {symbol_text(name), parse_bml_blueprint_fields(0)};
        return form_call("bl-make", args, 2, name->offset);
    }
    fail_at(source_offset, "an expression was not recognized");
    return NULL;
}

static Form *parse_bml_postfix(void) {
    Form *expression = parse_bml_primary();
    while (1) {
        skip_bml();
        if (source_offset >= source_length) break;
        char character = source_bytes[source_offset];
        if (character == CALL_OPEN) {
            Form *arguments = parse_bml_arguments();
            if (form_is_symbol(expression, "offer") && arguments->part_count == 2) expression = form_symbol("oac-offer", expression->offset);
            if (form_is_symbol(expression, "choice") && arguments->part_count == 2) expression = form_symbol("oac-choice", expression->offset);
            expression = form_call_head(expression, arguments->parts, arguments->part_count);
            continue;
        }
        if (character == DECIMAL_POINT) {
            size_t range_next = checked_add(source_offset, ONE_ELEMENT);
            if (range_next < source_length && source_bytes[range_next] == DECIMAL_POINT) break;
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            Form *method = read_bml_symbol();
            Form **arguments = NULL;
            size_t argument_count = EMPTY_COUNT;
            size_t argument_capacity = EMPTY_COUNT;
            append_form(&arguments, &argument_count, &argument_capacity, expression);
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == CALL_OPEN) {
                Form *rest = parse_bml_arguments();
                for (size_t index = FIRST_INDEX; index < rest->part_count; index = checked_add(index, ONE_ELEMENT)) {
                    append_form(&arguments, &argument_count, &argument_capacity, rest->parts[index]);
                }
            }
            expression = argument_count == 1 && form_is_symbol(method, "str")
                ? form_call("value_str", arguments, argument_count, method->offset)
                : argument_count == 2 && form_is_symbol(method, "join")
                ? form_call("bml-value-join", arguments, argument_count, method->offset)
                : form_call_head(method, arguments, argument_count);
            continue;
        }
        if (character != INDEX_OPEN) break;
        size_t opening = source_offset;
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        skip_bml();
        Form *start = source_offset < source_length && source_bytes[source_offset] == PAIR_MARK
            ? form_integer(0, opening) : parse_bml_expression();
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == PAIR_MARK) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == INDEX_CLOSE) {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
                Form *arguments[] = {expression, start, form_call("nothing", NULL, 0, opening)};
                expression = form_call("bml-value-slice", arguments, ARITY_THREE, opening);
                continue;
            }
            Form *end = parse_bml_expression();
            expect_character(INDEX_CLOSE, "an index was not closed");
            Form *arguments[] = {expression, start, end};
            expression = form_call("bml-value-slice", arguments, ARITY_THREE, opening);
            continue;
        }
        expect_character(INDEX_CLOSE, "an index was not closed");
        Form *arguments[] = {expression, start};
        expression = form_call("bml-value-index", arguments, ARITY_TWO, opening);
    }
    return expression;
}

static Form *parse_bml_product(void) {
    Form *expression = parse_bml_postfix();
    while (1) {
        skip_bml();
        if (source_offset >= source_length) break;
        char character = source_bytes[source_offset];
        const char *operation = NULL;
        if (character == PRODUCT_MARK) operation = "mul";
        else if (character == REMAINDER_MARK) operation = "mod";
        else if (character == LINE_COMMENT_MARK) {
            size_t next = checked_add(source_offset, ONE_ELEMENT);
            if (next < source_length && source_bytes[next] == LINE_COMMENT_MARK) break;
            operation = "div";
        }
        if (operation == NULL) break;
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        Form *right = parse_bml_postfix();
        Form *arguments[] = {expression, right};
        expression = form_call(operation, arguments, ARITY_TWO, expression->offset);
    }
    return expression;
}

static Form *parse_bml_sum(void) {
    Form *expression = parse_bml_product();
    while (1) {
        size_t saved = source_offset;
        const char *operation = take_sum_operator();
        if (operation == NULL) {
            source_offset = saved;
            break;
        }
        Form *right = parse_bml_product();
        Form *arguments[] = {expression, right};
        Form *annotation = expression->annotation;
        if (strcmp(operation, "+") == 0) operation = form_is_symbol(annotation, "String") ? "str_concat" : form_is_symbol(annotation, "Comparable") ? "add" : "bml-value-add";
        expression = form_call(operation, arguments, ARITY_TWO, expression->offset);
        expression->annotation = annotation;
    }
    return expression;
}

static Form *parse_bml_range(void) {
    Form *expression = parse_bml_sum();
    while (1) {
        skip_bml();
        size_t next = checked_add(source_offset, ONE_ELEMENT);
        if (source_offset >= source_length || next >= source_length || source_bytes[source_offset] != DECIMAL_POINT || source_bytes[next] != DECIMAL_POINT) break;
        source_offset = checked_add(next, ONE_ELEMENT);
        Form *end = parse_bml_sum();
        Form *arguments[] = {expression, end};
        expression = form_call("range", arguments, ARITY_TWO, expression->offset);
    }
    return expression;
}

static int at_not_equal(void) {
    skip_bml();
    size_t next = checked_add(source_offset, ONE_ELEMENT);
    if (source_offset >= source_length || next >= source_length) return 0;
    return source_bytes[source_offset] == NEGATION_MARK && source_bytes[next] == NAME_MARK;
}

static Form *parse_bml_comparison(void) {
    Form *expression = parse_bml_range();
    while (1) {
        if (at_not_equal()) {
            source_offset = checked_add(checked_add(source_offset, ONE_ELEMENT), ONE_ELEMENT);
            Form *right = parse_bml_range();
            Form *equal_arguments[] = {expression, right};
            const char *equality = form_is_symbol(expression->annotation, "String") ? "str_eq" : form_is_symbol(expression->annotation, "Comparable") ? "eq" : form_is_symbol(expression->annotation, "Value") ? "value_eq" : "bml-value-equal";
            Form *equal = form_call(equality, equal_arguments, ARITY_TWO, expression->offset);
            Form *negation[] = {equal};
            expression = form_call("not", negation, ARITY_ONE, expression->offset);
            continue;
        }
        size_t saved = source_offset;
        const char *operation = take_compare_operator();
        if (operation == NULL) {
            source_offset = saved;
            break;
        }
        if (strcmp(operation, "eq") == 0) operation = form_is_symbol(expression->annotation, "String") ? "str_eq" : form_is_symbol(expression->annotation, "Comparable") ? "eq" : form_is_symbol(expression->annotation, "Value") ? "value_eq" : "bml-value-equal";
        Form *right = parse_bml_range();
        Form *arguments[] = {expression, right};
        expression = form_call(operation, arguments, ARITY_TWO, expression->offset);
    }
    return expression;
}

static Form *parse_bml_conjunction(void) {
    Form *expression = parse_bml_comparison();
    while (1) {
        skip_bml();
        size_t next = checked_add(source_offset, ONE_ELEMENT);
        if (source_offset >= source_length || next >= source_length || source_bytes[source_offset] != '&' || source_bytes[next] != '&') break;
        source_offset = checked_add(next, ONE_ELEMENT);
        Form *right = parse_bml_comparison();
        Form *arguments[] = {expression, right};
        expression = form_call("and", arguments, ARITY_TWO, expression->offset);
    }
    return expression;
}

static Form *parse_bml_disjunction(void) {
    Form *expression = parse_bml_conjunction();
    while (1) {
        skip_bml();
        size_t next = checked_add(source_offset, ONE_ELEMENT);
        if (source_offset >= source_length || next >= source_length || source_bytes[source_offset] != DISJUNCTION_MARK || source_bytes[next] != DISJUNCTION_MARK) break;
        source_offset = checked_add(next, ONE_ELEMENT);
        Form *right = parse_bml_conjunction();
        Form *arguments[] = {expression, right};
        expression = form_call("or", arguments, ARITY_TWO, expression->offset);
    }
    return expression;
}

static int take_arrow(void) {
    skip_bml();
    size_t next = checked_add(source_offset, ONE_ELEMENT);
    if (source_offset >= source_length || next >= source_length) return 0;
    if (source_bytes[source_offset] != NAME_MARK || source_bytes[next] != ARROW_MARK) return 0;
    source_offset = checked_add(next, ONE_ELEMENT);
    return 1;
}

static Form *match_subject_name(size_t offset) {
    char spelling[48];
    if (snprintf(spelling, sizeof spelling, "match-at-%zu", offset) < SYSTEM_CALL_OK) fail("a match name could not be formed");
    return form_symbol(spelling, offset);
}

static int form_is_wildcard(Form *form) {
    return form->kind == FORM_SYMBOL && bytes_equal_cstr(form->text, form->text_length, "_");
}

static Form *parse_bml_match(size_t opening) {
    skip_bml();
    /* Keywords remain ordinary names unless the complete control shape starts.
     * The Form grammar owns this choice; this bootstrap mirrors its fallback. */
    if (source_offset >= source_length || strchr(",;)]}=?:><+*/%&|", source_bytes[source_offset]) != NULL) {
        source_offset = opening;
        return NULL;
    }
    if (take_keyword("then") || take_keyword("else")) {
        source_offset = opening;
        return NULL;
    }
    jmp_buf subject_choice;
    jmp_buf *outer_choice = parser_choice;
    parser_choice = &subject_choice;
    if (setjmp(subject_choice)) {
        parser_choice = outer_choice;
        source_offset = opening;
        return NULL;
    }
    Form *subject = parse_bml_expression();
    parser_choice = outer_choice;
    skip_bml();
    if (source_offset >= source_length || source_bytes[source_offset] != SECTION_OPEN) {
        source_offset = opening;
        return NULL;
    }
    source_offset = checked_add(source_offset, ONE_ELEMENT);
    Form *name = match_subject_name(opening);
    Form *chain = form_symbol(NAME_NOTHING, opening);
    Form **arms = NULL;
    size_t arm_count = EMPTY_COUNT;
    size_t arm_capacity = EMPTY_COUNT;
    while (1) {
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == SECTION_CLOSE) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            break;
        }
        Form *patterns = new_form(FORM_GROUP, source_offset);
        while (1) {
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == TEXT_DELIMITER) group_add(patterns, parse_text());
            else if (source_offset < source_length && is_digit_char(source_bytes[source_offset])) group_add(patterns, read_bml_number());
            else group_add(patterns, read_bml_symbol());
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == '|') {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
                continue;
            }
            break;
        }
        skip_bml();
        size_t arrow = checked_add(source_offset, ONE_ELEMENT);
        if (source_offset >= source_length || arrow >= source_length || source_bytes[source_offset] != NAME_MARK || source_bytes[arrow] != '>') {
            fail_at(source_offset, "a match arm is missing its result");
        }
        source_offset = checked_add(arrow, ONE_ELEMENT);
        Form *result = parse_bml_expression();
        append_form(&arms, &arm_count, &arm_capacity, patterns);
        append_form(&arms, &arm_count, &arm_capacity, result);
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == ARGUMENT_SEPARATOR) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
        }
    }
    for (size_t index = arm_count; index > FIRST_INDEX; ) {
        index -= ARITY_TWO;
        Form *patterns = arms[index];
        Form *result = arms[checked_add(index, ONE_ELEMENT)];
        int open = 0;
        for (size_t pattern = FIRST_INDEX; pattern < patterns->part_count; pattern = checked_add(pattern, ONE_ELEMENT)) {
            if (form_is_wildcard(patterns->parts[pattern])) open = 1;
        }
        Form *test = form_symbol(NAME_TRUE, opening);
        if (!open) {
            test = form_symbol(NAME_FALSE, opening);
            for (size_t pattern = patterns->part_count; pattern > FIRST_INDEX; ) {
                pattern -= ONE_ELEMENT;
                Form *eq_arguments[] = {name, patterns->parts[pattern]};
                Form *eq = form_call("eq", eq_arguments, ARITY_TWO, opening);
                Form *if_arguments[] = {eq, form_symbol(NAME_TRUE, opening), test};
                test = form_call("if", if_arguments, ARITY_THREE, opening);
            }
        }
        Form *if_arguments[] = {test, result, chain};
        chain = form_call("if", if_arguments, ARITY_THREE, opening);
    }
    Form *binding_arguments[] = {name, subject};
    Form *binding = form_call("let", binding_arguments, ARITY_TWO, opening);
    Form *sequence[] = {binding, chain};
    return form_call("do", sequence, ARITY_TWO, opening);
}

static Form *parse_bml_conditional(size_t opening) {
    Form *test = parse_bml_expression();
    if (!take_keyword("then")) fail_at(source_offset, "an if is missing then");
    Form *consequent = parse_bml_expression();
    if (!take_keyword("else")) fail_at(source_offset, "an if is missing else");
    Form *alternative = parse_bml_expression();
    Form *arguments[] = {test, consequent, alternative};
    return form_call("if", arguments, ARITY_THREE, opening);
}

static Form *parse_bml_expression(void) {
    skip_bml();
    size_t opening = source_offset;
    if (take_keyword("if")) return parse_bml_conditional(opening);
    if (take_keyword("match")) {
        Form *matched = parse_bml_match(opening);
        if (matched != NULL) return matched;
    }
    if (take_keyword("choice")) {
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == SECTION_OPEN) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            Form **options = NULL;
            size_t option_count = EMPTY_COUNT;
            size_t option_capacity = EMPTY_COUNT;
            parse_bml_statements(&options, &option_count, &option_capacity);
            return form_call("choice", options, option_count, opening);
        }
        source_offset = opening;
    }
    if (take_keyword(TRY_WORD)) {
        Form *body = parse_bml_expression();
        if (!take_keyword("catch") && !take_keyword("else")) fail_at(source_offset, "a try is missing catch or else");
        Form *handler = parse_bml_expression();
        Form *try_arguments[] = {body, handler};
        return form_call(TRY_WORD, try_arguments, ARITY_TWO, opening);
    }
    Form *condition = parse_bml_disjunction();
    while (1) {
        skip_bml();
        size_t pipe_next = checked_add(source_offset, ONE_ELEMENT);
        if (source_offset >= source_length || pipe_next >= source_length || source_bytes[source_offset] != DISJUNCTION_MARK || source_bytes[pipe_next] != ARROW_MARK) break;
        source_offset = checked_add(pipe_next, ONE_ELEMENT);
        condition = pipe_call(condition, parse_bml_postfix());
    }
    if (take_arrow()) {
        if (condition->kind != FORM_SYMBOL) fail_at(condition->offset, "a lambda names one parameter");
        Form *parameters = new_form(FORM_GROUP, condition->offset);
        group_add(parameters, condition);
        Form *body = parse_bml_expression();
        Form *arguments[] = {parameters, body};
        return form_call("lambda", arguments, ARITY_TWO, opening);
    }
    skip_bml();
    size_t coalesce_next = checked_add(source_offset, ONE_ELEMENT);
    if (source_offset < source_length && coalesce_next < source_length
        && source_bytes[source_offset] == TERNARY_MARK && source_bytes[coalesce_next] == TERNARY_MARK) {
        source_offset = checked_add(coalesce_next, ONE_ELEMENT);
        Form *alternative = parse_bml_expression();
        Form *coalesce_arguments[] = {condition, alternative};
        return form_call(COALESCE_WORD, coalesce_arguments, ARITY_TWO, opening);
    }
    if (source_offset >= source_length || source_bytes[source_offset] != TERNARY_MARK) return condition;
    source_offset = checked_add(source_offset, ONE_ELEMENT);
    Form *consequent = parse_bml_expression();
    expect_character(PAIR_MARK, "a conditional is missing its other branch");
    Form *alternative = parse_bml_expression();
    Form *arguments[] = {condition, consequent, alternative};
    return form_call("if", arguments, ARITY_THREE, condition->offset);
}

static void parse_bml_statements(Form ***forms, size_t *count, size_t *capacity);

static Form *definition_form(Form *name, Form *parameters, Form *body) {
    Form *definition = new_form(FORM_GROUP, name->offset);
    group_add(definition, form_symbol(NAME_DEFN, name->offset));
    group_add(definition, name);
    group_add(definition, parameters);
    group_add(definition, body);
    return definition;
}

static void skip_type_arguments(void) {
    skip_bml();
    if (source_offset >= source_length || source_bytes[source_offset] != '<') return;
    size_t depth = ONE_ELEMENT;
    source_offset = checked_add(source_offset, ONE_ELEMENT);
    while (depth != EMPTY_COUNT) {
        if (source_offset >= source_length) fail_at(source_offset, "a type argument was not closed");
        char character = source_bytes[source_offset];
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        if (character == '<') depth = checked_add(depth, ONE_ELEMENT);
        if (character == '>') depth -= ONE_ELEMENT;
    }
}

static Form *parse_bml_type(void) {
    Form *name = read_bml_symbol();
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == '<') {
        size_t start = name->offset;
        skip_type_arguments();
        return form_text(source_bytes + start, source_offset - start, start);
    }
    return symbol_text(name);
}

static void parse_bml_annotation(Form *name) {
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == PAIR_MARK) {
        ++source_offset;
        name->annotation = parse_bml_type();
    }
}

static Form *parse_bml_type_list(void) {
    Form *types = form_call("list", NULL, 0, source_offset);
    skip_bml();
    if (source_offset >= source_length || source_bytes[source_offset] != '<') return types;
    ++source_offset;
    for (;;) {
        group_add(types, parse_bml_type());
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == '>') { ++source_offset; return types; }
        expect_character(ARGUMENT_SEPARATOR, "a type argument was not separated");
    }
}

static void parse_bml_attributes(void) {
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == INDEX_OPEN) parse_bml_list();
}

static void skip_member(void) {
    size_t depth = EMPTY_COUNT;
    while (source_offset < source_length) {
        skip_bml();
        if (source_offset >= source_length) fail_at(source_offset, "a member was not finished");
        char character = source_bytes[source_offset];
        if (character == TEXT_DELIMITER) {
            parse_text();
            continue;
        }
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        if (character == CALL_OPEN || character == SECTION_OPEN || character == INDEX_OPEN) depth = checked_add(depth, ONE_ELEMENT);
        if (character == CALL_CLOSE || character == SECTION_CLOSE || character == INDEX_CLOSE) {
            if (depth != EMPTY_COUNT) depth -= ONE_ELEMENT;
        }
        if (character == STATEMENT_END && depth == EMPTY_COUNT) return;
    }
    fail_at(source_offset, "a member was not finished");
}

static Form *parse_bml_parameters(void) {
    size_t opening = source_offset;
    expect_character(CALL_OPEN, "a parameter list was not opened");
    Form *group = new_form(FORM_GROUP, opening);
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == CALL_CLOSE) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        return group;
    }
    while (1) {
        Form *parameter = read_bml_symbol();
        parse_bml_annotation(parameter);
        group_add(group, parameter);
        if (source_offset < source_length && source_bytes[source_offset] == CALL_CLOSE) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            break;
        }
        expect_character(ARGUMENT_SEPARATOR, "a parameter was not separated");
    }
    return group;
}

static Form *parse_bml_definition(void) {
    Form *name = read_bml_symbol();
    Form *parameters = parse_bml_parameters();
    parse_bml_annotation(name);
    skip_bml();
    Form *body;
    if (source_offset < source_length && source_bytes[source_offset] == SECTION_OPEN) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        Form **statements = NULL;
        size_t statement_count = EMPTY_COUNT;
        size_t statement_capacity = EMPTY_COUNT;
        parse_bml_statements(&statements, &statement_count, &statement_capacity);
        body = form_call("do", statements, statement_count, name->offset);
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
        }
    } else {
        expect_character(NAME_MARK, "a definition is missing its body");
        body = parse_bml_expression();
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) ++source_offset;
    }
    return definition_form(name, parameters, body);
}

static Form *parse_pattern_binding(size_t opening) {
    Form *pattern = parse_bml_list();
    expect_character(NAME_MARK, "a binding is missing its value");
    Form *value = parse_bml_expression();
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) ++source_offset;
    Form *holder = match_subject_name(opening);
    Form **statements = NULL;
    size_t statement_count = EMPTY_COUNT;
    size_t statement_capacity = EMPTY_COUNT;
    Form *holder_arguments[] = {holder, value};
    append_form(&statements, &statement_count, &statement_capacity, form_call("let", holder_arguments, ARITY_TWO, opening));
    for (size_t index = CALL_ARGUMENTS; index < pattern->part_count; index = checked_add(index, ONE_ELEMENT)) {
        Form *name = pattern->parts[index];
        if (name->kind != FORM_SYMBOL) fail_at(name->offset, "a pattern name must be a symbol");
        Form *nth_arguments[] = {holder, form_integer((long long)(index - CALL_ARGUMENTS), name->offset)};
        Form *read_arguments[] = {name, form_call("nth", nth_arguments, ARITY_TWO, name->offset)};
        append_form(&statements, &statement_count, &statement_capacity, form_call("let", read_arguments, ARITY_TWO, name->offset));
    }
    return form_call("do", statements, statement_count, opening);
}

static Form *parse_bml_binding(void) {
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == INDEX_OPEN) return parse_pattern_binding(source_offset);
    Form *name = read_bml_symbol();
    parse_bml_annotation(name);
    expect_character(NAME_MARK, "a binding is missing its value");
    Form *value = parse_bml_expression();
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) ++source_offset;
    Form *arguments[] = {name, value};
    return form_call("let", arguments, ARITY_TWO, name->offset);
}

/* Transitional source carrier: construct the existing Form language values.
 * language-template.fk and form-bml-lower.fk own their meaning and shrink path. */
static void parse_bml_class(Form ***forms, size_t *count, size_t *capacity, int is_template) {
    Form *name = read_bml_symbol();
    Form *types = parse_bml_type_list();
    parse_bml_attributes();
    Form *base = NULL;
    skip_bml();
    if (!is_template && source_offset < source_length && source_bytes[source_offset] == PAIR_MARK) {
        ++source_offset;
        base = read_bml_symbol();
        types = parse_bml_type_list();
    }
    Form *members = form_call("list", NULL, 0, name->offset);
    Form *names[4];
    for (size_t i = 0; i < 4; ++i) names[i] = form_call("list", NULL, 0, name->offset);
    skip_bml();
    int template_line = is_template && source_offset < source_length && source_bytes[source_offset] == STATEMENT_END;
    if (template_line) ++source_offset;
    else {
        expect_character(SECTION_OPEN, "a class was not opened");
        for (;;) {
            skip_bml();
            if (source_offset >= source_length) fail_at(name->offset, "a class was not closed");
            if (source_bytes[source_offset] == SECTION_CLOSE) { ++source_offset; break; }
            if (is_template) {
                if (!take_keyword("member")) fail_at(source_offset, "a template expects a member");
                Form *member = read_bml_symbol();
                expect_character(PAIR_MARK, "a template member needs its type");
                Form *type = parse_bml_type();
                expect_character(STATEMENT_END, "a template member was not finished");
                Form *args[] = {symbol_text(member), type};
                group_add(members, form_call("language-template-member", args, 2, member->offset));
                continue;
            }
            Form *method;
            size_t kind = 0;
            if (take_keyword(DEF_WORD)) method = parse_bml_definition();
            else {
                if (base != NULL) fail_at(source_offset, "a language class expects a method");
                if (take_keyword("field")) kind = 1;
                else if (take_keyword("ref")) kind = 2;
                else if (take_keyword("thought")) kind = 3;
                else fail_at(source_offset, "an abstraction expects a member");
                Form *member = read_bml_symbol();
                parse_bml_annotation(member);
                expect_character(NAME_MARK, "a member needs its value");
                Form *body = parse_bml_expression();
                expect_character(STATEMENT_END, "a member was not finished");
                method = definition_form(member, new_form(FORM_GROUP, member->offset), body);
            }
            Form *member = method->parts[DEFN_NAME];
            group_add(names[kind], symbol_text(member));
            if (base != NULL) {
                size_t length = checked_add(checked_add(name->text_length, 1), member->text_length);
                char *qualified = allocate_zeroed(length + 1, 1);
                memcpy(qualified, name->text, name->text_length);
                qualified[name->text_length] = '_';
                memcpy(qualified + name->text_length + 1, member->text, member->text_length);
                Form *qualified_name = form_symbol(qualified, member->offset);
                qualified_name->annotation = member->annotation;
                method->parts[DEFN_NAME] = qualified_name;
                free(qualified);
                Form *args[] = {symbol_text(member), qualified_name};
                group_add(members, form_call("language-class-member-method", args, 2, member->offset));
            }
            append_form(forms, count, capacity, method);
        }
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) ++source_offset;
    }
    Form *value;
    if (is_template) {
        Form *args[] = {symbol_text(name), types, members};
        value = form_call(template_line ? "language-template" : "language-template-with-members", args, template_line ? 2 : 3, name->offset);
    } else if (base != NULL) {
        Form *args[] = {symbol_text(name), base, types, members};
        value = form_call("language-class", args, 4, name->offset);
    } else {
        Form *args[] = {form_text("abstraction", 11, name->offset), symbol_text(name), types, names[0], names[1], names[2], names[3]};
        value = form_call("list", args, 7, name->offset);
        append_form(forms, count, capacity, definition_form(name, new_form(FORM_GROUP, name->offset), value));
        return;
    }
    Form *args[] = {name, value};
    append_form(forms, count, capacity, form_call("let", args, 2, name->offset));
}

static void parse_bml_statements(Form ***forms, size_t *count, size_t *capacity) {
    while (1) {
        skip_bml();
        if (source_offset >= source_length) fail_at(source_offset, "a section was not closed");
        if (source_bytes[source_offset] == SECTION_CLOSE) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            return;
        }
        if (take_keyword("defer")) {
            Form *deferred = parse_bml_expression();
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) ++source_offset;
            Form **rest = NULL;
            size_t rest_count = EMPTY_COUNT;
            size_t rest_capacity = EMPTY_COUNT;
            parse_bml_statements(&rest, &rest_count, &rest_capacity);
            Form *body = form_call("do", rest, rest_count, deferred->offset);
            // Match fbl-defer's Form lowering: a completed absence is still a
            // successful body. Only failure selects cleanup-and-propagation.
            Form *held_body_arguments[] = {body};
            Form *held_body = form_call("list",held_body_arguments,ARITY_ONE,deferred->offset);
            Form *propagate = form_call(FAIL_NAME, NULL, EMPTY_COUNT, deferred->offset);
            Form *catch_statements[] = {deferred, propagate};
            Form *handler = form_call("do", catch_statements, ARITY_TWO, deferred->offset);
            Form *try_arguments[] = {held_body, handler};
            Form *attempt = form_call(TRY_WORD, try_arguments, ARITY_TWO, deferred->offset);
            Form *holder = match_subject_name(deferred->offset);
            Form *binding_arguments[] = {holder, attempt};
            Form *binding = form_call("let", binding_arguments, ARITY_TWO, deferred->offset);
            Form *unpack_arguments[] = {holder};
            Form *unpacked = form_call("head",unpack_arguments,ARITY_ONE,deferred->offset);
            Form *sequence[] = {binding, deferred, unpacked};
            append_form(forms, count, capacity, form_call("do", sequence, ARITY_THREE, deferred->offset));
            return;
        }
        if (take_declaration_keyword("let", 1) || take_declaration_keyword("ref", 0) || take_declaration_keyword("thought", 0)) {
            append_form(forms, count, capacity, parse_bml_binding());
            continue;
        }
        if (take_declaration_keyword("blueprint", 0)) {
            Form *name = read_bml_symbol();
            Form *args[] = {symbol_text(name), parse_bml_blueprint_fields(1)};
            append_form(forms, count, capacity, form_call("bl-declare", args, 2, name->offset));
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) ++source_offset;
            continue;
        }
        if (take_declaration_keyword(DEF_WORD, 0)) {
            append_form(forms, count, capacity, parse_bml_definition());
            continue;
        }
        if (take_declaration_keyword("field", 0)) {
            Form *name = read_bml_symbol();
            parse_bml_annotation(name);
            expect_character(NAME_MARK, "a field is missing its value");
            Form *body = parse_bml_expression();
            expect_character(STATEMENT_END, "a field was not finished");
            append_form(forms, count, capacity, definition_form(name, new_form(FORM_GROUP, name->offset), body));
            continue;
        }
        if (take_declaration_keyword("member", 0)) {
            skip_member();
            continue;
        }
        if (take_declaration_keyword("class", 0)) {
            parse_bml_class(forms, count, capacity, 0);
            continue;
        }
        if (take_declaration_keyword("template", 0)) {
            parse_bml_class(forms, count, capacity, 1);
            continue;
        }
        if (source_bytes[source_offset] == SECTION_OPEN) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            Form **statements = NULL;
            size_t statement_count = EMPTY_COUNT;
            size_t statement_capacity = EMPTY_COUNT;
            parse_bml_statements(&statements, &statement_count, &statement_capacity);
            append_form(forms, count, capacity, form_call("do", statements, statement_count, source_offset));
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
            }
            continue;
        }
        append_form(forms, count, capacity, parse_bml_expression());
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == SECTION_CLOSE) continue;
        expect_character(STATEMENT_END, "an expression was not finished");
    }
}

static char *read_import_path(void) {
    skip_bml();
    size_t start = source_offset;
    while (source_offset < source_length) {
        char character = source_bytes[source_offset];
        unsigned char classified = (unsigned char)character;
        if (isspace(classified) || character == STATEMENT_END) break;
        source_offset = checked_add(source_offset, ONE_ELEMENT);
    }
    if (source_offset == start) fail_at(start, "an import is missing its path");
    return copy_bytes(source_bytes + start, source_offset - start);
}

static void load_unit(const char *from_file, const char *relative);
static void drive_forms(Form **forms, size_t form_count);

/* The catalog owns home names; this seed only links their source units. The
 * native source linker can take this admission step without changing recipes. */
typedef struct { char *name; size_t length; char *unit; } HomeName;
static HomeName *home_names;
static size_t home_count;
static int homes_read;

static void link_home_names(Form **forms, size_t count, const char *from_file) {
    if (!homes_read) {
        homes_read = 1;
        char *catalog = resolve_unit(from_file, "form-stdlib/home-index.txt");
        size_t length = 0;
        char *bytes = read_source(catalog, &length);
        size_t capacity = 0, at = 0;
        while (at < length) {
            size_t end = at;
            while (end < length && bytes[end] != '\n') ++end;
            while (at < end && isspace((unsigned char)bytes[at])) ++at;
            if (at < end && bytes[at] != '#') {
                size_t name = at;
                while (at < end && !isspace((unsigned char)bytes[at])) ++at;
                size_t name_length = at - name;
                while (at < end && isspace((unsigned char)bytes[at])) ++at;
                size_t unit = at;
                while (at < end && !isspace((unsigned char)bytes[at])) ++at;
                if (name_length != 0 && at > unit) {
                    home_names = grow_list(home_names, &capacity, sizeof *home_names, checked_add(home_count, 1));
                    home_names[home_count++] = (HomeName){copy_bytes(bytes + name, name_length), name_length, copy_bytes(bytes + unit, at - unit)};
                }
            }
            at = end + 1;
        }
        free(bytes);
        free(catalog);
    }
    Form **pending = NULL;
    size_t used = 0, capacity = 0;
    for (size_t i = 0; i < count; ++i) append_form(&pending, &used, &capacity, forms[i]);
    while (used != 0) {
        Form *form = pending[--used];
        if (form->kind == FORM_SYMBOL && find_function_text(form->text, form->text_length) == NULL &&
            (find_operation_text(form->text, form->text_length) == NULL ||
                (bytes_equal_cstr(form->text, form->text_length, "kernel_ast") && find_function_text("kas-query", 9) == NULL) ||
                (current_program == &main_program && (bytes_equal_cstr(form->text, form->text_length, "cell_run") || bytes_equal_cstr(form->text, form->text_length, "cell_call")) && find_function_text("pifbd-parsed-cell", 17) == NULL))) {
            for (size_t i = 0; i < home_count; ++i) {
                if (bytes_equal(form->text, form->text_length, home_names[i].name, home_names[i].length)) {
                    load_unit(from_file, home_names[i].unit);
                    break;
                }
            }
        }
        for (size_t i = 0; i < form->part_count; ++i) append_form(&pending, &used, &capacity, form->parts[i]);
    }
    free(pending);
}

static void load_preludes(const char *bytes, size_t length, const char *from_file) {
    size_t line_start = FIRST_INDEX;
    for (size_t index = FIRST_INDEX; index <= length; index = checked_add(index, ONE_ELEMENT)) {
        int at_end = index == length || bytes[index] == NEWLINE;
        if (!at_end) continue;
        size_t cursor = line_start;
        while (cursor < index && isspace((unsigned char)bytes[cursor])) cursor = checked_add(cursor, ONE_ELEMENT);
        if (cursor < index && bytes[cursor] == COMMENT_INTRODUCER) cursor = checked_add(cursor, ONE_ELEMENT);
        else if (cursor < index && bytes[cursor] == LINE_COMMENT_MARK && checked_add(cursor, ONE_ELEMENT) < index && bytes[cursor + ONE_ELEMENT] == LINE_COMMENT_MARK) {
            cursor = checked_add(cursor, ARITY_TWO);
        } else {
            line_start = checked_add(index, ONE_ELEMENT);
            continue;
        }
        while (cursor < index && isspace((unsigned char)bytes[cursor])) cursor = checked_add(cursor, ONE_ELEMENT);
        size_t mark_length = strlen(PRELUDE_WORD);
        if (cursor < index && index - cursor >= mark_length && memcmp(bytes + cursor, PRELUDE_WORD, mark_length) == STRING_COMPARE_EQUAL) {
            cursor = checked_add(cursor, mark_length);
            while (cursor < index) {
                while (cursor < index && (isspace((unsigned char)bytes[cursor]) || bytes[cursor] == ',')) cursor = checked_add(cursor, ONE_ELEMENT);
                if (cursor >= index) break;
                size_t start = cursor;
                while (cursor < index && !isspace((unsigned char)bytes[cursor]) && bytes[cursor] != ',') cursor = checked_add(cursor, ONE_ELEMENT);
                size_t token_length = cursor - start;
                int path = token_length != EMPTY_COUNT;
                for (size_t mark = FIRST_INDEX; path && mark < token_length; mark = checked_add(mark, ONE_ELEMENT)) {
                    if (!is_path_char((unsigned char)bytes[start + mark])) path = 0;
                }
                if (!path || token_is_none(bytes + start, token_length)) break;
                char *relative = copy_bytes(bytes + start, token_length);
                load_unit(from_file, relative);
                free(relative);
            }
        }
        line_start = checked_add(index, ONE_ELEMENT);
    }
}

static int at_source_section(void) {
    if (!at_keyword(SECTION_WORD)) return 0;
    size_t after = source_offset + strlen(SECTION_WORD);
    while (after < source_length && isspace((unsigned char)source_bytes[after])) ++after;
    return after < source_length && source_bytes[after] == INDEX_OPEN;
}

static Form *parse_source_section(void) {
    size_t opening = source_offset;
    if (!take_keyword(SECTION_WORD)) fail_at(opening, "a section was expected");
    expect_character(INDEX_OPEN, "a section name was not opened");
    size_t name_at = source_offset;
    while (source_offset < source_length && source_bytes[source_offset] != INDEX_CLOSE) ++source_offset;
    size_t name_length = source_offset - name_at;
    expect_character(INDEX_CLOSE, "a section name was not closed");
    int executable = name_length == strlen(FORM_BML_SECTION) && memcmp(source_bytes + name_at, FORM_BML_SECTION, name_length) == 0;
    expect_character(SECTION_OPEN, "a section was not opened");
    Form **forms = NULL;
    size_t count = 0, capacity = 0;
    if (executable) parse_bml_statements(&forms, &count, &capacity);
    else {
        size_t depth = 1;
        while (depth != 0) {
            skip_bml();
            if (source_offset >= source_length) fail_at(opening, "a section was not closed");
            char c = source_bytes[source_offset];
            if (c == TEXT_DELIMITER) { parse_text(); continue; }
            ++source_offset;
            if (c == SECTION_OPEN) ++depth;
            if (c == SECTION_CLOSE) --depth;
        }
    }
    return form_call("do", forms, count, opening);
}

/* One top-level reader admits imports, plain forms and sections in order.
 * Import-only library units have no section to use as a language discriminator.
 * This shared source admission moves with the parser into the native walker. */
static Form **parse_source(size_t *form_count) {
    Form **forms = NULL;
    size_t count = EMPTY_COUNT;
    size_t capacity = EMPTY_COUNT;
    while (1) {
        skip_space_and_comments();
        if (source_offset >= source_length) break;
        if (take_keyword(IMPORT_WORD)) {
            char *relative = read_import_path();
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) ++source_offset;
            load_unit(source_path, relative);
            free(relative);
            continue;
        }
        if (!at_source_section()) {
            size_t span_count = 0;
            Form **span = parse_source_span(&span_count, 1);
            for (size_t i = 0; i < span_count; ++i) append_form(&forms, &count, &capacity, span[i]);
            free(span);
            continue;
        }
        Form *section = parse_source_section();
        for (size_t i = CALL_ARGUMENTS; i < section->part_count; ++i) append_form(&forms, &count, &capacity, section->parts[i]);
    }
    *form_count = count;
    return forms;
}

static Form **parse_current_source(size_t *form_count) {
    source_view(current_program, source_path, source_bytes, source_length);
    const char *admitted = source_bytes;
    Form **forms = parse_source(form_count);
    for (size_t i = 0; i < source_view_count; ++i) if (source_views[i].bytes == admitted) {
        source_views[i].forms = forms; source_views[i].form_count = *form_count; break;
    }
    return forms;
}

static uint64_t cell_source_digest(const char *path);
static int cell_program_fresh(CellProgram *program);

static void cell_import(CellProgram *owner, const char *from, const char *relative, const char *path, CellProgram *program) {
    for (size_t i = 0; i < owner->import_count; ++i) {
        CellImport *item = &owner->imports[i];
        if (same_path(item->from, from) && same_path(item->relative, relative)) return;
    }
    owner->imports = grow_list(owner->imports, &owner->import_capacity, sizeof *owner->imports, owner->import_count + 1);
    owner->imports[owner->import_count++] = (CellImport){strdup(from), strdup(relative), strdup(path), program};
}

static void cell_merge_library(CellProgram *library, int with_globals) {
    for (size_t i = 0; i < library->source_count; ++i) {
        SourceView view = library->sources[i]; source_view(current_program, view.path, view.bytes, view.length);
    }
    for (size_t i = 0; i < library->unit_count; ++i) {
        if (!unit_is_loaded(library->units[i])) remember_unit(strdup(library->units[i]));
    }
    for (size_t i = 0; i < library->import_count; ++i) {
        CellImport item = library->imports[i];
        cell_import(current_program, item.from, item.relative, item.path, item.program);
    }
    for (size_t i = 0; i < library->function_count; ++i) {
        Binding binding = library->functions[i];
        int present = 0;
        for (size_t j = 0; j < function_count; ++j) {
            if (functions[j].value.body == binding.value.body && functions[j].value.program == binding.value.program) { present = 1; break; }
        }
        if (present) continue;
        functions = grow_list(functions, &function_capacity, sizeof *functions, function_count + 1);
        functions[function_count] = binding;
        functions[function_count].previous_function = name_index_find(&function_names, binding.bytes, binding.length) + 1;
        name_index_set(&function_names, binding.bytes, binding.length, function_count++);
        ++global_epoch;
    }
    if (with_globals) for (size_t i = 0; i < library->global_count; ++i) {
        Binding binding = library->globals[i];
        if (name_index_find(&library->global_names, binding.bytes, binding.length) != i) continue;
        if (name_index_find(&global_names, binding.bytes, binding.length) == SIZE_MAX) bind_global(binding.bytes, binding.length, binding.value);
    }
}

static void cell_seal_program(CellProgram *program) {
    capture_program(program);
    program->digests = allocate_zeroed(loaded_count, sizeof *program->digests);
    for (size_t i = 0; i < loaded_count; ++i) {
        CellProgram *parsed = same_path(program->path, loaded_units[i]) ? program : NULL;
        for (size_t j = 0; parsed == NULL && j < program->import_count; ++j) {
            CellProgram *dependency = program->imports[j].program;
            if (dependency != NULL && same_path(dependency->path, loaded_units[i])) parsed = dependency;
        }
        if (parsed != NULL && parsed->bytes != NULL) {
            uint64_t digest = hash_bytes(VALUE_HASH_OFFSET, parsed->bytes, parsed->length);
            program->digests[i] = digest != 0 ? digest : 1;
        }
    }
    program->admitted = 1;
}

static Value cell_cached_source(const char *path) {
    if (active_cell == NULL || active_cell->image_units.kind != KIND_LIST) return nothing_value();
    for (size_t i = 0; i < active_cell->image_units.item_count; ++i) {
        Value unit = list_at(active_cell->image_units, i), name = list_at(unit, 0);
        if (name.kind == KIND_TEXT && same_path(name.text, path)) return unit;
    }
    return nothing_value();
}

/* Only parser-object construction is physical. The Form image owner validates
 * the immutable source packet and replay order before this boundary is called. */
static Form *cell_cached_form(Value row) {
    Value kind = list_at(row, 0), value = list_at(row, 1), children = list_at(row, 2), offset = list_at(row, 3), annotation = list_at(row, 4), path = list_at(row, 5);
    expect_kind(kind, KIND_TEXT, "cached syntax kind is absent");
    expect_kind(children, KIND_LIST, "cached syntax children are absent");
    expect_kind(offset, KIND_INTEGER, "cached syntax position is absent");
    expect_kind(path, KIND_TEXT, "cached syntax path is absent");
    FormKind tag = bytes_equal_cstr(kind.text, kind.text_length, "int") ? FORM_INTEGER : bytes_equal_cstr(kind.text, kind.text_length, "float") ? FORM_REAL : bytes_equal_cstr(kind.text, kind.text_length, "text") ? FORM_TEXT : bytes_equal_cstr(kind.text, kind.text_length, "name") ? FORM_SYMBOL : FORM_GROUP;
    Form *form = new_form(tag, (size_t)offset.integer);
    form->path = copy_bytes(path.text, path.text_length);
    if (tag == FORM_INTEGER) { expect_kind(value, KIND_INTEGER, "cached integer is absent"); form->integer = value.integer; }
    else if (tag == FORM_REAL) { expect_kind(value, KIND_REAL, "cached real is absent"); form->real = value.real; }
    else if (tag == FORM_TEXT || tag == FORM_SYMBOL) { expect_kind(value, KIND_TEXT, "cached text is absent"); form->text = copy_bytes(value.text, value.text_length); form->text_length = value.text_length; }
    for (size_t i = 0; i < children.item_count; ++i) append_form(&form->parts, &form->part_count, &form->part_capacity, cell_cached_form(list_at(children, i)));
    if (annotation.kind != KIND_NOTHING) form->annotation = cell_cached_form(annotation);
    return form;
}

static void cell_read_parsed(CellProgram *program, Value unit) {
    Value bytes = list_at(unit, 1), forms = list_at(unit, 2), loads = list_at(unit, 3);
    expect_kind(bytes, KIND_TEXT, "cached source is absent"); expect_kind(forms, KIND_LIST, "cached roots are absent"); expect_kind(loads, KIND_LIST, "cached imports are absent");
    source_path = program->path; source_bytes = copy_bytes(bytes.text, bytes.text_length); source_length = bytes.text_length; source_offset = 0;
    program->bytes = source_bytes; program->length = source_length;
    source_view(program, source_path, source_bytes, source_length);
    size_t capacity = 0;
    for (size_t i = 0; i < forms.item_count; ++i) append_form(&program->forms, &program->form_count, &capacity, cell_cached_form(list_at(forms, i)));
    for (size_t i = 0; i < source_view_count; ++i) if (source_views[i].bytes == program->bytes) { source_views[i].forms = program->forms; source_views[i].form_count = program->form_count; break; }
    for (size_t i = 0; i < loads.item_count; ++i) {
            Value row = list_at(loads, i), after = list_at(row, 3);
            if (after.kind == KIND_INTEGER && after.integer == 0) { Value from = list_at(row, 0), relative = list_at(row, 1); load_unit(from.text, relative.text); }
    }
    bind_definitions(program->forms, program->form_count); program->bindings_ready = 1;
    link_home_names(program->forms, program->form_count, program->path);
}

static void load_unit(const char *from_file, const char *relative) {
    char *path = resolve_unit(from_file, relative);
    if (active_cell != NULL) {
        CellProgram *owner = current_program;
        owner->loads = grow_list(owner->loads, &owner->load_capacity, sizeof *owner->loads, owner->load_count + 1);
        owner->loads[owner->load_count++] = (CellLoad){strdup(from_file), strdup(relative), strdup(path), owner->bindings_ready};
    }
    if (unit_is_loaded(path)) {
        free(path);
        return;
    }
    if (active_cell != NULL) {
        CellProgram *owner = current_program, *library = NULL;
        SourceCursor saved = capture_source();
        const char *saved_path = source_path;
        for (CellProgram *candidate = cell_programs; candidate != NULL; candidate = candidate->next) {
            /* A completed cell can become an import without replaying its
             * effects. Promotion pins that owner against later root resets;
             * Form retains the admission graph and initializer meaning. */
            if (!candidate->library && candidate->initialized == 1 &&
                same_path(candidate->path, path) && cell_program_fresh(candidate)) candidate->library = 1;
            if (candidate->library && same_path(candidate->path, path) &&
                ((candidate->busy && candidate->loading_owner == active_cell) || candidate->admission_owner == active_cell || cell_program_fresh(candidate))) { library = candidate; break; }
        }
        if (library == NULL) {
            library = allocate_zeroed(1, sizeof *library);
            library->path = path; library->library = 1; library->busy = 1;
            library->closure_owner = owner;
            library->loading_owner = active_cell;
            library->admission_owner = active_cell;
            library->next = cell_programs; cell_programs = library;
            select_program(library);
            remember_unit(strdup(path));
            Value cached = cell_cached_source(path);
            if (cached.kind != KIND_NOTHING) cell_read_parsed(library, cached);
            else {
            source_path = path;
            source_bytes = read_source(path, &source_length);
            library->bytes = source_bytes; library->length = source_length;
            source_offset = 0;
            load_preludes(source_bytes, source_length, path);
            library->forms = parse_current_source(&library->form_count);
            bind_definitions(library->forms, library->form_count);
            library->bindings_ready = 1;
            link_home_names(library->forms, library->form_count, path);
            active_cell->source_units += 1;
            for (size_t i = 0; i < function_count; ++i) if (functions[i].value.program == library) active_cell->source_functions += 1;
            }
            cell_seal_program(library);
            library->admitted = 0;
            library->busy = 0;
            library->loading_owner = NULL;
            select_program(owner);
        }
        cell_import(owner, from_file, relative, path, library);
        cell_merge_library(library, 0);
        source_path = saved_path; restore_source(saved);
        return;
    }
    remember_unit(path);
    size_t length = EMPTY_COUNT;
    char *bytes = read_source(path, &length);
    load_preludes(bytes, length, path);
    SourceCursor saved = capture_source();
    const char *saved_path = source_path;
    source_bytes = bytes;
    source_length = length;
    source_offset = FIRST_INDEX;
    source_path = path;
    size_t form_count = EMPTY_COUNT;
    Form **forms = parse_current_source(&form_count);
    bind_definitions(forms, form_count);
    link_home_names(forms, form_count, path);
    if (form_count != EMPTY_COUNT && (active_cell == NULL || !active_cell->checking)) drive_forms(forms, form_count);
    // Bootstrap homes can include the requested entry itself. Retain that
    // admission so main neither hides it as already loaded nor executes its
    // initializers a second time.
    if (same_path(path,main_program.path)) {
        main_program.forms=forms; main_program.form_count=form_count;
        main_program.answer=finished;
    }
    source_path = saved_path;
    restore_source(saved);
}

static Value cell_initialize_imports(CellProgram *program) {
    Value answer = nothing_value();
    for (size_t i = 0; i < program->import_count; ++i) {
        CellProgram *library = program->imports[i].program;
        if (library == NULL || library == program) continue;
        select_program(library);
        if (library->initialized == -2) {
            active_cell->stop_kind = library->initialization_stop;
            active_cell->message = library->initialization_message;
            active_cell->recipe = library->initialization_recipe;
            active_cell->unit = library->initialization_unit;
            longjmp(active_cell->recovery, 1);
        }
        if (library->initialized == 0) {
            library->initialized = -1;
            library->initializing_owner = active_cell;
            answer = cell_initialize_imports(library);
            if (library->form_count != 0) { drive_forms(library->forms, library->form_count); answer = finished; }
            library->answer = answer;
            library->initialized = 1;
            library->initializing_owner = NULL;
        }
        answer = library->answer;
        capture_program(library);
        select_program(program);
        cell_merge_library(library, 1);
    }
    return answer;
}

static void drive_forms(Form **forms, size_t form_count) {
    /* Retain the actual unit entry order, including direct-main imports. This
     * is an execution observation; Form decides its projection and validity. */
    const char *initializing_path = NULL;
    for (size_t i = 0; i < source_view_count; ++i) {
        SourceView *view = &source_views[i];
        if (forms != NULL && form_count != 0 && view->forms == forms && view->form_count == form_count) {
            initializing_path = view->path;
            if (view->initialization_order == 0)
                view->initialization_order = ++source_initialization_order;
        }
    }
    /* This bootstrap boundary carries the existing stage protocol; its owner
     * moves with unit initialization into the Form source loader. Cell recovery
     * and process exit already unwind an interrupted stage without claiming it
     * completed. Foreign observers need the actual unit, not just stack depth. */
    Value stage = nothing_value();
    if (initializing_path != NULL) {
        Value entering[] = {text_value("source-initialization", 21), text_value(initializing_path, strlen(initializing_path))};
        stage = stage_door(29, offer_cons(entering, 2));
    }
    Form program = {0};
    program.kind = FORM_GROUP; program.parts = forms; program.part_count = form_count;
    drive(&program);
    if (stage.kind == KIND_INTEGER) {
        Value settled[] = {stage, text_value("complete", 8), count_word(form_count), integer_value(0)};
        stage_door(30, list_from(settled, 4));
    }
}

/* Native values cross cell boundaries without copying identity. Reclamation is
 * nonmoving, at a return safepoint with every suspended evaluator made visible.
 * Parser/source storage and canonical nodes retain their separate owners. */
typedef struct {
    void *pointer;
    HeapKind kind;
    size_t count;
    int marked;
} HeapItem;
static HeapItem *heap_items;
static size_t heap_count, heap_capacity;
enum { HEAP_INDEX_START = 1024 };
static size_t heap_collection_target = HEAP_INDEX_START;
static size_t *heap_index;
static size_t heap_index_capacity;
static size_t *heap_pending;
static size_t heap_pending_count, heap_pending_capacity;

static size_t heap_hash(void *pointer) {
    uintptr_t word = (uintptr_t)pointer >> 3;
    word ^= word >> 17; word *= (uintptr_t)0x9e3779b97f4a7c15ULL;
    return (size_t)(word ^ (word >> 31));
}

static void heap_reindex(size_t capacity) {
    if (capacity == heap_index_capacity) memset(heap_index, 0, capacity * sizeof *heap_index);
    else {
        size_t *replacement = allocate_zeroed(capacity, sizeof *replacement);
        free(heap_index); heap_index = replacement;
    }
    heap_index_capacity = capacity;
    for (size_t i = 0; i < heap_count; ++i) if (heap_items[i].pointer != NULL) {
        size_t slot = heap_hash(heap_items[i].pointer) & (capacity - 1);
        while (heap_index[slot] != 0) slot = (slot + 1) & (capacity - 1);
        heap_index[slot] = i + 1;
    }
}

static size_t heap_find(void *pointer) {
    if (pointer == NULL || heap_index_capacity == 0) return SIZE_MAX;
    size_t slot = heap_hash(pointer) & (heap_index_capacity - 1);
    while (heap_index[slot] != 0) {
        size_t index = heap_index[slot] - 1;
        if (heap_items[index].pointer == pointer) return index;
        slot = (slot + 1) & (heap_index_capacity - 1);
    }
    return SIZE_MAX;
}

static void heap_prepare(void) {
    if (heap_index_capacity == 0) heap_reindex(HEAP_INDEX_START);
    else if (heap_count + 1 >= heap_index_capacity / 2) heap_reindex(checked_add(heap_index_capacity, heap_index_capacity));
    heap_items = grow_list(heap_items, &heap_capacity, sizeof *heap_items, heap_count + 1);
}

static void *heap_allocate(size_t count, size_t width, HeapKind kind, size_t elements) {
    // Bookkeeping admission precedes obtaining the allocation it must own.
    heap_prepare();
    void *pointer=allocate_zeroed(count,width);
    heap_track(pointer,kind,elements);
    return pointer;
}

static void heap_track(void *pointer, HeapKind kind, size_t count) {
    if (pointer == NULL || heap_find(pointer) != SIZE_MAX) return;
    heap_prepare();
    size_t index = heap_count++;
    heap_items[index] = (HeapItem){pointer, kind, count, 0};
    if (kind == HEAP_TEXT) ++heap_text_allocations;
    if (kind == HEAP_LIST) ++heap_list_allocations;
    size_t slot = heap_hash(pointer) & (heap_index_capacity - 1);
    while (heap_index[slot] != 0) slot = (slot + 1) & (heap_index_capacity - 1);
    heap_index[slot] = index + 1;
    if (heap_count >= heap_collection_target) heap_collection_due = 1;
}

static void heap_forget(void *pointer) {
    size_t index = heap_find(pointer);
    if (index != SIZE_MAX) {
        if (heap_items[index].kind == HEAP_TEXT) --heap_text_allocations;
        if (heap_items[index].kind == HEAP_LIST) --heap_list_allocations;
        heap_items[index].pointer = NULL;
    }
}

static void heap_mark_pointer(void *pointer) {
    size_t index = heap_find(pointer);
    if (index == SIZE_MAX || heap_items[index].marked) return;
    heap_items[index].marked = 1;
    if (heap_items[index].kind == HEAP_TEXT || heap_items[index].kind == HEAP_PARAMETERS) return;
    heap_pending = grow_list(heap_pending, &heap_pending_capacity, sizeof *heap_pending, heap_pending_count + 1);
    heap_pending[heap_pending_count++] = index;
}

static void heap_mark_value(Value value) {
    heap_mark_pointer(value.owner);
    if (value.kind == KIND_TEXT) heap_mark_pointer(value.text);
    else if (value.kind == KIND_LIST) heap_mark_pointer(value.list);
    else if (value.kind == KIND_RECORD) heap_mark_pointer(value.record);
    else if (value.kind == KIND_FUNCTION) { heap_mark_pointer(value.parameters); heap_mark_pointer(value.captures); }
    else if (value.kind == KIND_NODE) heap_mark_pointer(value.cell);
}

static void heap_mark_bindings(Binding *bindings, size_t count) {
    for (size_t i = 0; i < count; ++i) { heap_mark_pointer((void *)bindings[i].bytes); heap_mark_value(bindings[i].value); }
}

static void heap_mark_frames(Frame *stack, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        Frame *frame = &stack[i];
        heap_mark_value(frame->incoming); heap_mark_value(frame->accumulator); heap_mark_value(frame->callee);
        if (frame->arguments != NULL) for (size_t j = 0; j < frame->argument_count; ++j) heap_mark_value(frame->arguments[j]);
    }
}

static void heap_mark_program(CellProgram *program) {
    heap_mark_bindings(program->functions, program->function_count);
    heap_mark_bindings(program->globals, program->global_count);
    heap_mark_value(program->answer); heap_mark_value(program->diagnostics); heap_mark_value(program->diagnostic_text);
    heap_mark_value(program->publication); heap_mark_value(program->image_diagnostics);
}

static void heap_collect(Value result) {
    capture_program(current_program);
    heap_mark_value(result); heap_mark_value(finished); heap_mark_value(native_field_entry);
    heap_mark_frames(frames, frame_count); heap_mark_bindings(locals, local_count);
    heap_mark_program(&main_program);
    for (CellProgram *program = cell_programs; program != NULL; program = program->next) heap_mark_program(program);
    for (CellCall *call = active_cell; call != NULL; call = call->parent) {
        heap_mark_value(call->argument); heap_mark_value(call->diagnostics); heap_mark_value(call->suspended_finished);
        heap_mark_value(call->image_units); heap_mark_value(call->publication);
        heap_mark_value(call->image_argument); heap_mark_value(call->path_value);
        heap_mark_frames(call->suspended_frames, call->suspended_frame_count);
        heap_mark_bindings(call->suspended_locals, call->suspended_local_count);
    }
    for (size_t i = 0; i < syntax_count; ++i) if (syntax_forms[i]->syntax_view_ready) heap_mark_value(syntax_forms[i]->syntax_view);
    for (size_t i = 0; i < source_view_count; ++i) if (source_views[i].binding_checked) heap_mark_value(source_views[i].binding_checks);
    for (size_t i = 0; i < cell_count; ++i) {
        Cell *cell = cells[i]; heap_mark_value(cell->category_value);
        heap_mark_value(cell->children_value);
    }
    for (size_t i = 0; i < frame_record_count; ++i) { heap_mark_value(frame_records[i].word); heap_mark_pointer((void *)frame_records[i].path); }
    for (size_t i = 0; i < method_definition_count; ++i) { heap_mark_value(method_definitions[i].blueprint); heap_mark_value(method_definitions[i].function); }
    while (heap_pending_count != 0) {
        HeapItem item = heap_items[heap_pending[--heap_pending_count]];
        if (item.kind == HEAP_LIST) {
            List *list = item.pointer; heap_mark_value(list->head);
            heap_mark_pointer(list->tail); heap_mark_pointer(list->indexed);
            if (list->dotted_tail != NULL) heap_mark_value(*list->dotted_tail);
            // flat is only a derived copy of these immutable linked heads.
            // Their ownership travels through head/tail once, including shared
            // suffixes; revisiting every flat suffix makes marking quadratic.
        } else if (item.kind == HEAP_RECORD) {
            Record *record = item.pointer; heap_mark_value(record->blueprint);
            for (size_t i = 0; i < record->count; ++i) { heap_mark_value(record->keys[i]); heap_mark_value(record->values[i]); }
        } else if (item.kind == HEAP_CAPTURES) heap_mark_bindings(item.pointer, item.count);
        else if (item.kind == HEAP_NATIVE_CELL) {
            Cell *cell=item.pointer; heap_mark_value(cell->category_value); heap_mark_value(cell->children_value);
        }
    }
    for (size_t i=0;i<field_map_count;++i) if (field_maps[i].record!=NULL) {
        size_t item=heap_find(field_maps[i].record);
        if (item==SIZE_MAX || !heap_items[item].marked) field_maps[i].record=NULL;
    }
    for (size_t i=0;i<native_gift_view_count;++i) if (native_gift_views[i].owner!=NULL) {
        size_t item=heap_find(native_gift_views[i].owner);
        if (item==SIZE_MAX || !heap_items[item].marked) {
            native_gift_views[i].owner=NULL; native_gift_views[i].root=nothing_value();
        }
    }
    size_t kept = 0;
    for (size_t i = 0; i < heap_count; ++i) {
        HeapItem item = heap_items[i];
        if (item.pointer == NULL) continue;
        if (item.marked) { item.marked = 0; heap_items[kept++] = item; continue; }
        if (item.kind == HEAP_TEXT) --heap_text_allocations;
        if (item.kind == HEAP_LIST) { --heap_list_allocations; List *list = item.pointer; free(list->flat); free(list->dotted_tail); }
        else if (item.kind == HEAP_RECORD) { Record *record = item.pointer; free(record->keys); free(record->values); free(record->text_slots); }
        free(item.pointer);
    }
    heap_count = kept;
    heap_reindex(heap_index_capacity);
    // This schedules care from measured live allocation growth; it admits no
    // less work and imposes no task, context or generation allowance.
    heap_collection_target = kept >= HEAP_INDEX_START / 2 ? checked_add(kept, kept) : HEAP_INDEX_START;
    heap_collection_due = 0;
    ++heap_collection_epoch;
}

/* Form owns the lexical decision and its health text. This carrier retains the
 * result with the parsed incarnation and applies only the named syntax sites. */
/* The binding check's verdict image. A clean verdict (no rows) depends only on the
 * exact sources the checker saw and on this runtime's build, so it is admitted once
 * and replayed by digest after that: a temp-dir marker named by the digest of every
 * source view (path and bytes, in order) and the runtime's own source stamp. A verdict
 * with rows is never imaged; those sources are checked again on every start. */
static int check_image_path(char *out, size_t capacity) {
    static const char build[] = __TIMESTAMP__ " " __DATE__ " " __TIME__;
    uint64_t key = hash_bytes(VALUE_HASH_OFFSET, build, sizeof build - 1);
    size_t total = 0;
    for (size_t i = 0; i < source_view_count; ++i) {
        SourceView *view = &source_views[i];
        if (view->path != NULL) key = hash_bytes(hash_mix(key, 0x2f), view->path, strlen(view->path));
        key = hash_bytes(hash_mix(key, (uint64_t)view->length), view->bytes, view->length);
        total += view->length;
    }
    const char *directory = getenv("TMPDIR");
    if (directory == NULL || directory[0] == TEXT_TERMINATOR) directory = TEMP_DIRECTORY;
    size_t length = strlen(directory);
    while (length > 1 && directory[length - 1] == PATH_SEPARATOR) --length;
    int wrote = snprintf(out, capacity, "%.*s/fkwu-check-%016llx-%zx-%zu", (int)length, directory, (unsigned long long)key, total, source_view_count);
    return wrote > 0 && (size_t)wrote < capacity;
}

/* Per-unit clean verdicts. A unit is clean when every name it uses is known; the
 * known set is every function and top-level let name of the checked program plus
 * this build's operations. Its marker is keyed by that name set's digest, the
 * unit's path and bytes, and the build, so removing a name anywhere rechecks every
 * unit, and editing one unit rechecks only it. A unit with rows is never imaged. */
typedef struct { const char *bytes; size_t length; } CheckName;
static CheckName *check_names; static size_t check_name_count, check_name_capacity;
static void check_name_add(const char *bytes, size_t length) {
    check_names = grow_list(check_names, &check_name_capacity, sizeof *check_names, check_name_count + 1);
    check_names[check_name_count++] = (CheckName){bytes, length};
}
static void check_name_lets(Form *form) {
    if (form == NULL || form->kind != FORM_GROUP || form->part_count == 0 || form->parts[0]->kind != FORM_SYMBOL) return;
    Form *head = form->parts[0];
    if (bytes_equal_cstr(head->text, head->text_length, "do")) { for (size_t i = 1; i < form->part_count; ++i) check_name_lets(form->parts[i]); }
    else if (bytes_equal_cstr(head->text, head->text_length, "let") && form->part_count == 3 && form->parts[1]->kind == FORM_SYMBOL)
        check_name_add(form->parts[1]->text, form->parts[1]->text_length);
}
static int check_name_order(const void *a, const void *b) {
    const CheckName *x = a, *y = b; size_t n = x->length < y->length ? x->length : y->length;
    int c = memcmp(x->bytes, y->bytes, n);
    return c != 0 ? c : (x->length > y->length) - (x->length < y->length);
}
static uint64_t check_names_digest(CellProgram *root) {
    check_name_count = 0;
    for (size_t i = 0; i < root->function_count; ++i) check_name_add(root->functions[i].bytes, root->functions[i].length);
    for (size_t i = 0; i < root->source_count; ++i) {
        SourceView *view = &source_views[root->sources[i].serial - 1];
        for (size_t j = 0; j < view->form_count; ++j) check_name_lets(view->forms[j]);
    }
    if (check_name_count > 1) qsort(check_names, check_name_count, sizeof *check_names, check_name_order);
    static const char build[] = __TIMESTAMP__ " " __DATE__ " " __TIME__;
    uint64_t key = hash_bytes(VALUE_HASH_OFFSET, build, sizeof build - 1);
    for (size_t i = 0; i < check_name_count; ++i) {
        if (i > 0 && check_name_order(&check_names[i - 1], &check_names[i]) == 0) continue;
        key = hash_bytes(hash_mix(key, 0x0a), check_names[i].bytes, check_names[i].length);
    }
    return key;
}
static int check_unit_path(char *out, size_t capacity, uint64_t names, SourceView *view) {
    uint64_t key = hash_mix(names, (uint64_t)view->length);
    if (view->path != NULL) key = hash_bytes(hash_mix(key, 0x2f), view->path, strlen(view->path));
    key = hash_bytes(key, view->bytes, view->length);
    const char *directory = getenv("TMPDIR");
    if (directory == NULL || directory[0] == TEXT_TERMINATOR) directory = TEMP_DIRECTORY;
    size_t length = strlen(directory);
    while (length > 1 && directory[length - 1] == PATH_SEPARATOR) --length;
    int wrote = snprintf(out, capacity, "%.*s/fkwu-unit-%016llx-%zx", (int)length, directory, (unsigned long long)key, view->length);
    return wrote > 0 && (size_t)wrote < capacity;
}

static void check_program_bindings(CellProgram *program) {
    CellProgram *saved = current_program;
    capture_program(saved);
    select_program(&main_program);
    char image[4096];
    int imaged = check_image_path(image, sizeof image);
    struct stat seen;
    int admitted = imaged && stat(image, &seen) == 0 && S_ISREG(seen.st_mode);
    CellProgram *root = active_cell != NULL ? active_cell->program : &main_program;
    uint64_t names = admitted ? 0 : check_names_digest(root);
    unsigned char *carried = admitted ? NULL : allocate_zeroed(program->source_count + 1, 1);
    if (!admitted) for (size_t i = 0; i < program->source_count; ++i) {
        SourceView *view = &source_views[program->sources[i].serial - 1];
        char unit[4096];
        if (view->binding_checked || !check_unit_path(unit, sizeof unit, names, view)) continue;
        if (stat(unit, &seen) == 0 && S_ISREG(seen.st_mode)) {
            view->binding_checks = list_from(NULL, 0); view->binding_checked = 1; carried[i] = 1;
        }
    }
    Value rows;
    if (admitted) rows = list_from(NULL, 0);
    else {
        Form head = {.kind = FORM_SYMBOL, .text = "kas-check", .text_length = 9};
        Form pid = {.kind = FORM_INTEGER, .integer = (long long)getpid()};
        Form *parts[] = {&head, &pid};
        Form invoke = {.kind = FORM_GROUP, .parts = parts, .part_count = 2};
        Form *forms[] = {&invoke}; drive_forms(forms, 1);
        rows = finished;
        if (imaged && rows.kind == KIND_LIST && rows.item_count == 0) {
            int marker = open(image, O_WRONLY | O_CREAT | O_EXCL, TEXT_FILE_MODE);
            if (marker >= 0) close(marker);
        }
    }
    expect_kind(rows, KIND_LIST, "source binding observation must answer rows");
    Value *diagnostics = allocate_zeroed(rows.item_count, sizeof *diagnostics);
    size_t text_length = 0;
    for (size_t i = 0; i < rows.item_count; ++i) {
        Value line = list_at(list_at(rows, i), 2);
        expect_kind(line, KIND_TEXT, "source binding observation must retain text");
        text_length = checked_add(text_length, line.text_length);
    }
    char *text = allocate_zeroed(text_length + 1, 1); size_t at = 0;
    for (size_t i = 0; i < rows.item_count; ++i) {
        Value row = list_at(rows, i), id = list_at(row, 0), line = list_at(row, 2);
        if (id.kind != KIND_INTEGER || id.integer <= 0 || (size_t)id.integer > syntax_count || line.kind != KIND_TEXT) fail("source binding observation has an invalid site");
        syntax_forms[id.integer - 1]->unresolved = 1;
        Value refuse = list_at(row, 3);
        if (refuse.kind == KIND_INTEGER && refuse.integer != 0) program->binding_refusal = 1;
        diagnostics[i] = list_at(row, 1);
        memcpy(text + at, line.text, line.text_length); at += line.text_length;
    }
    program->diagnostics = list_from(diagnostics, rows.item_count);
    program->diagnostic_text = text_value(text, text_length);
    program->errors = rows.item_count;
    for (size_t i = 0; i < program->source_count; ++i) {
        SourceView *view = &source_views[program->sources[i].serial - 1];
        if (view->binding_checked) continue;
        Value *checks = allocate_zeroed(rows.item_count, sizeof *checks); size_t count = 0;
        for (size_t j = 0; j < rows.item_count; ++j) {
            Value row = list_at(rows, j), id = list_at(row, 0);
            if (syntax_forms[id.integer - 1]->source == view->bytes) checks[count++] = row;
        }
        view->binding_checks = list_from(checks, count); view->binding_checked = 1; free(checks);
    }
    if (carried != NULL) {
        for (size_t i = 0; i < program->source_count; ++i) {
            SourceView *view = &source_views[program->sources[i].serial - 1];
            char unit[4096];
            if (carried[i] || !view->binding_checked || view->binding_checks.kind != KIND_LIST || view->binding_checks.item_count != 0) continue;
            if (!check_unit_path(unit, sizeof unit, names, view)) continue;
            int marker = open(unit, O_WRONLY | O_CREAT | O_EXCL, TEXT_FILE_MODE);
            if (marker >= 0) close(marker);
        }
        free(carried);
    }
    for (CellProgram *dependency = cell_programs; dependency != NULL; dependency = dependency->next) {
        if (active_cell == NULL || dependency->admission_owner != active_cell) continue;
        dependency->errors = 0; dependency->binding_refusal = 0;
        for (size_t i = 0; i < dependency->source_count; ++i) {
            SourceView *view = &source_views[dependency->sources[i].serial - 1];
            dependency->errors += view->binding_checks.item_count;
            for (size_t j = 0; j < view->binding_checks.item_count; ++j) {
                Value refuse = list_at(list_at(view->binding_checks, j), 3);
                if (refuse.kind == KIND_INTEGER && refuse.integer != 0) dependency->binding_refusal = 1;
            }
        }
    }
    free(diagnostics); free(text);
    select_program(saved);
    finished = nothing_value(); result_ready = 0;
}

static Value cell_record_field(Value record, const char *key, Value value) {
    Value fields[] = {record, text_value(key, strlen(key)), value};
    return offer_record_set(fields, 3);
}

static Value cell_result(CellCall *call, Value value, long long elapsed, long long cpu) {
    Value result = record_value(heap_allocate(1,sizeof(Record),HEAP_RECORD,1));
    Value stopped = nothing_value(), diagnostics = call->diagnostics.kind == KIND_LIST ? call->diagnostics : list_from(NULL, 0);
    if (call->stop_kind != NULL) {
        stopped = record_value(heap_allocate(1,sizeof(Record),HEAP_RECORD,1));
        cell_record_field(stopped, "kind", text_value(call->stop_kind, strlen(call->stop_kind)));
        cell_record_field(stopped, "message", text_value(call->message, strlen(call->message)));
        cell_record_field(stopped, "recipe", text_value(call->recipe != NULL ? call->recipe : "", call->recipe != NULL ? strlen(call->recipe) : 0));
        cell_record_field(stopped, "unit", text_value(call->unit != NULL ? call->unit : call->path, strlen(call->unit != NULL ? call->unit : call->path)));
        Value diagnostic = record_value(heap_allocate(1,sizeof(Record),HEAP_RECORD,1));
        cell_record_field(diagnostic, "organ", text_value("fkwu", 4));
        cell_record_field(diagnostic, "aspect", text_value("cell", 4));
        cell_record_field(diagnostic, "stage", text_value(call->parsing ? "compile" : "execute", call->parsing ? 7 : 7));
        cell_record_field(diagnostic, "observed", text_value(call->stop_kind, strlen(call->stop_kind)));
        cell_record_field(diagnostic, "detail", text_value(call->message, strlen(call->message)));
        cell_record_field(diagnostic, "path", text_value(call->path, strlen(call->path)));
        cell_record_field(diagnostic, "health", integer_value(0));
        if (diagnostics.item_count == 0) diagnostics = list_from(&diagnostic, 1);
    }
    cell_record_field(result, "value", stopped.kind == KIND_NOTHING ? value : nothing_value());
    cell_record_field(result, "execution_complete", integer_value(call->execution_complete));
    cell_record_field(result, "completed_value", stopped.kind != KIND_NOTHING && call->execution_complete ? value : nothing_value());
    cell_record_field(result, "out", text_value(call->out_bytes != NULL ? call->out_bytes : "", call->out_length));
    cell_record_field(result, "err", text_value(call->err_bytes != NULL ? call->err_bytes : "", call->err_length));
    cell_record_field(result, "diag", diagnostics);
    cell_record_field(result, "stopped", stopped);
    cell_record_field(result, "publication", call->publication);
    cell_record_field(result, "errors", integer_value(call->errors));
    cell_record_field(result, "resident", integer_value(call->resident));
    cell_record_field(result, "source_units", count_word(call->source_units));
    cell_record_field(result, "source_functions", count_word(call->source_functions));
    /* Process-local reflection handle, including compile-only admissions. */
    if (call->program != NULL) cell_record_field(result, "source_owner", count_word(program_id(call->program)));
    cell_record_field(result, "ms", integer_value(elapsed));
    cell_record_field(result, "cpu_us", integer_value(cpu));
    return result;
}

static void cell_stop(const char *kind, const char *message) {
    CellCall *call = active_cell;
    call->stop_kind = kind;
    call->message = strdup(message);
    if (strcmp(kind, "unrunnable") == 0) call->errors += 1;
    for (size_t i = frame_count; i > 0; --i) {
        Form *form = frames[i - 1].form;
        if (form == NULL || form->kind != FORM_GROUP || form->part_count == 0) continue;
        Form *head = form->parts[0];
        if (head->kind != FORM_SYMBOL) continue;
        call->recipe = copy_bytes(head->text, head->text_length);
        call->unit = form->path;
        break;
    }
    longjmp(call->recovery, 1);
}

static Value offer_cell_input(Value *arguments, size_t argument_count) {
    (void)arguments; (void)argument_count;
    return active_cell != NULL ? active_cell->argument : nothing_value();
}

static uint64_t cell_source_digest(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) return 0;
    char bytes[65536];
    size_t got;
    uint64_t digest = VALUE_HASH_OFFSET;
    while ((got = fread(bytes, 1, sizeof bytes, file)) != 0) digest = hash_bytes(digest, bytes, got);
    int read_ok = !ferror(file), closed = fclose(file) == 0;
    return read_ok && closed ? (digest != 0 ? digest : 1) : 0;
}

static int cell_program_fresh(CellProgram *program) {
    if (!program->admitted || program->busy) return 0;
    for (size_t i = 0; i < program->unit_count; ++i) {
        uint64_t current = cell_source_digest(program->units[i]);
        if (current == 0 || current != program->digests[i]) { program->admitted = 0; return 0; }
    }
    for (size_t i = 0; i < program->import_count; ++i) {
        CellImport *item = &program->imports[i];
        char *selected = try_resolve_unit(item->from, item->relative);
        int same = selected != NULL && same_path(selected, item->path);
        free(selected);
        if (!same) { program->admitted = 0; return 0; }
    }
    for (size_t i = 0; i < program->load_count; ++i) {
        CellLoad item = program->loads[i];
        char *selected = try_resolve_unit(item.from, item.relative);
        int same = selected != NULL && same_path(selected, item.path); free(selected);
        if (!same) { program->admitted = 0; return 0; }
    }
    return 1;
}

static Value cell_get(Value record, const char *name) {
    Value args[] = {record, text_value(name, strlen(name))}; return offer_record_get(args, 2);
}

static Value cell_image_operation(const char *operation, Value value) {
    CellProgram *saved = current_program;
    Value argument = active_cell->argument;
    active_cell->image_argument = argument;
    Value request[] = {text_value(operation, strlen(operation)), value};
    active_cell->argument = list_from(request, 2);
    select_program(&main_program);
    Form name = {.kind = FORM_SYMBOL, .text = "pifbd-parsed-cell", .text_length = 17};
    Form input_name = {.kind = FORM_SYMBOL, .text = "cell_input", .text_length = 10};
    Form *input_parts[] = {&input_name}; Form input = {.kind = FORM_GROUP, .parts = input_parts, .part_count = 1};
    Form *parts[] = {&name, &input}; Form invoke = {.kind = FORM_GROUP, .parts = parts, .part_count = 2};
    Form *forms[] = {&invoke}; drive_forms(forms, 1);
    Value result = finished;
    active_cell->argument = argument; active_cell->image_argument = nothing_value(); select_program(saved);
    finished = nothing_value(); result_ready = 0; return result;
}

/* A child cell has its own parser, names, input and recovery. Native values and
 * effects stay in this process. Admission caching belongs to this same owner. */
static Value cell_execute(Value path, Value argument, Value deadline, Value input, int capture) {
    CellCall *call = allocate_zeroed(1, sizeof *call);
    call->parent = active_cell;
    call->capture = capture;
    call->path = path.kind == KIND_TEXT ? path.text : "";
    call->path_value = path;
    call->argument = argument;
    call->input = copy_bytes("", 0);
    int check = deadline.kind == KIND_TEXT && bytes_equal_cstr(deadline.text, deadline.text_length, "check");
    int valid = path.kind == KIND_TEXT && path.text_length > 0 && memchr(path.text, 0, path.text_length) == NULL &&
        (deadline.kind == KIND_NOTHING || deadline.kind == KIND_INTEGER || check) &&
        (input.kind == KIND_NOTHING || input.kind == KIND_LIST);
    size_t input_length = 0;
    for (List *line = input.kind == KIND_LIST ? input.list : NULL; valid && line != NULL; line = line->tail) {
        if (line->head.kind != KIND_TEXT) valid = 0;
        else input_length = checked_add(input_length, checked_add(line->head.text_length, 1));
    }
    if (!valid) {
        call->stop_kind = "refused"; call->message = "cell requires a path, deadline and input lines";
        return cell_result(call, nothing_value(), 0, 0);
    }
    call->checking = check;
    call->input = allocate_zeroed(input_length + 1, 1);
    call->input_length = input_length;
    size_t cursor = 0;
    for (List *line = input.kind == KIND_LIST ? input.list : NULL; line != NULL; line = line->tail) {
        memcpy(call->input + cursor, line->head.text, line->head.text_length);
        cursor += line->head.text_length;
        call->input[cursor++] = '\n';
    }
    call->out = capture ? open_memstream(&call->out_bytes, &call->out_length) : fopen("/dev/null", "w");
    call->err = capture ? open_memstream(&call->err_bytes, &call->err_length) : fopen("/dev/null", "w");
    if (call->out == NULL || call->err == NULL) {
        if (call->out != NULL) fclose(call->out);
        if (call->err != NULL) fclose(call->err);
        call->stop_kind = "refused"; call->message = "cell capture unavailable";
        return cell_result(call, nothing_value(), 0, 0);
    }
    unsigned long long began = monotonic_ns() / 1000000;
    long long began_cpu = cpu_microseconds();
    CellProgram *program = NULL;
    for (CellProgram *candidate = cell_programs; !check && candidate != NULL; candidate = candidate->next) {
        if (!candidate->library && strcmp(candidate->path, path.text) == 0 && cell_program_fresh(candidate)) { program = candidate; break; }
    }
    call->resident = program != NULL;
    if (program == NULL) {
        program = allocate_zeroed(1, sizeof *program);
        program->path = copy_bytes(path.text, path.text_length);
        program->next = cell_programs; cell_programs = program;
    }
    program->initialized = 0;
    program->busy += 1;
    call->program = program;
    if (deadline.kind == KIND_INTEGER && deadline.integer > 0) call->deadline = began + (unsigned long long)deadline.integer;
    if (active_cell != NULL && active_cell->deadline != 0 && (call->deadline == 0 || active_cell->deadline < call->deadline)) call->deadline = active_cell->deadline;
    SourceCursor saved_source = capture_source();
    const char *saved_path = source_path, *saved_entry = entry_source_bytes;
    size_t saved_entry_length = entry_source_length;
    Frame *saved_frames = frames;
    size_t saved_frame_count = frame_count, saved_frame_capacity = frame_capacity;
    Binding *saved_locals = locals;
    size_t saved_local_count = local_count, saved_local_capacity = local_capacity;
    CellProgram *saved_program = current_program;
    Value saved_finished = finished;
    int saved_result_ready = result_ready, saved_reported = failure_reported;
    const char *saved_failure = failure_message;
    jmp_buf *saved_recovery = evaluation_recovery, *saved_choice = parser_choice;
    size_t saved_stage_depth = stage_depth;
    call->suspended_frames = saved_frames; call->suspended_frame_count = saved_frame_count;
    call->suspended_locals = saved_locals; call->suspended_local_count = saved_local_count;
    call->suspended_finished = saved_finished;
    frames = NULL; frame_count = frame_capacity = 0;
    locals = NULL; local_count = local_capacity = 0;
    select_program(program);
    finished = nothing_value(); result_ready = 0;
    failure_message = NULL; failure_reported = 0; evaluation_recovery = NULL; parser_choice = NULL;
    active_cell = call;
    if (setjmp(call->recovery) == 0) {
        call->parsing = 1;
        source_path = program->path;
        if (!call->resident) {
            if (!check) {
                Value reading = cell_image_operation("read", path);
                call->image_units = cell_get(reading, "units");
                program->image_diagnostics = cell_get(reading, "diag");
                Value status = cell_get(reading, "status");
                program->disk_admitted = status.kind == KIND_TEXT && bytes_equal_cstr(status.text, status.text_length, "admitted");
            }
            Value cached = cell_cached_source(program->path);
            if (cached.kind != KIND_NOTHING) {
                remember_unit(copy_bytes(program->path, strlen(program->path)));
                cell_read_parsed(program, cached);
                entry_source_bytes = program->bytes; entry_source_length = program->length;
            } else {
            source_bytes = read_source(source_path, &source_length);
            program->bytes = source_bytes; program->length = source_length;
            entry_source_bytes = source_bytes; entry_source_length = source_length;
            source_offset = 0;
            remember_unit(copy_bytes(source_path, strlen(source_path)));
            load_preludes(source_bytes, source_length, source_path);
            program->forms = parse_current_source(&program->form_count);
            bind_definitions(program->forms, program->form_count);
            program->bindings_ready = 1;
            link_home_names(program->forms, program->form_count, source_path);
            call->source_units += 1;
            for (size_t i = 0; i < function_count; ++i) if (functions[i].value.program == program) call->source_functions += 1;
            }
            cell_seal_program(program);
            /* Admission has copied source bytes, syntax and load paths into
             * the program. The decoded image is no longer an execution owner.
             * Keep this lifetime with Form's source-admission owner as that
             * remaining bootstrap seam moves into the native walker. */
            call->image_units = nothing_value();
            check_program_bindings(program);
            if (call->source_units != 0) program->disk_admitted = 0;
            program->admitted = !check;
            for (CellProgram *dependency = cell_programs; dependency != NULL; dependency = dependency->next) {
                if (dependency->admission_owner == call) { dependency->admitted = !check; dependency->admission_owner = NULL; }
            }
        } else {
            source_bytes = program->bytes; source_length = program->length; source_offset = 0;
            entry_source_bytes = source_bytes; entry_source_length = source_length;
            global_count = program->library_globals;
            free(global_names.slots); global_names = (NameIndex){0};
            for (size_t i = 0; i < global_count; ++i) name_index_set(&global_names, globals[i].bytes, globals[i].length, i);
            ++global_epoch;
        }
        call->diagnostics = program->diagnostics;
        call->errors = program->errors;
        if (program->diagnostic_text.kind == KIND_TEXT) fwrite(program->diagnostic_text.text, 1, program->diagnostic_text.text_length, call->err);
        if (program->binding_refusal) {
            call->stop_kind = "unrunnable"; call->message = "source contains an unresolved value binding";
            longjmp(call->recovery, 1);
        }
        call->parsing = 0;
        if (!check) {
            if (program->errors == 0 && !program->disk_admitted) {
                program->publication = cell_image_operation(program->publication.kind == KIND_RECORD ? "pending" : "publish", program->publication.kind == KIND_RECORD ? program->publication : path);
                call->publication = program->publication;
                Value status = cell_get(program->publication, "status");
                if (status.kind != KIND_TEXT || !bytes_equal_cstr(status.text, status.text_length, "image-published")) cell_stop("image-publication", "parsed source image publication needs care");
                program->disk_admitted = 1;
                if (program->image_diagnostics.kind == KIND_LIST && program->image_diagnostics.item_count != 0) {
                    Value healed = cell_image_operation("healed", path);
                    Value *rows = allocate_zeroed(program->image_diagnostics.item_count + 1, sizeof *rows);
                    for (size_t i = 0; i < program->image_diagnostics.item_count; ++i) rows[i] = list_at(program->image_diagnostics, i);
                    rows[program->image_diagnostics.item_count] = healed;
                    program->image_diagnostics = list_from(rows, program->image_diagnostics.item_count + 1); free(rows);
                }
            }
            if (program->image_diagnostics.kind == KIND_LIST && program->image_diagnostics.item_count != 0) {
                size_t count = call->diagnostics.item_count + program->image_diagnostics.item_count;
                Value *rows = allocate_zeroed(count, sizeof *rows); size_t at = 0;
                for (size_t i = 0; i < call->diagnostics.item_count; ++i) rows[at++] = list_at(call->diagnostics, i);
                for (size_t i = 0; i < program->image_diagnostics.item_count; ++i) rows[at++] = list_at(program->image_diagnostics, i);
                call->diagnostics = list_from(rows, count); free(rows);
            }
            jit_cell_begin();
            Value dependency_answer = cell_initialize_imports(program);
            program->library_globals = global_count;
            if (program->form_count != 0) drive_forms(program->forms, program->form_count);
            else finished = dependency_answer;
            call->execution_complete = 1;
        }
    }
    Value answer = finished;
    // A physical call may finish between evaluator polling points. Preserve its
    // returned value/completed effects, while recording the exhausted caller
    // allowance at the executing boundary before projecting the cell result.
    if (call->stop_kind == NULL && call->deadline != 0 && monotonic_ns()/1000000 >= call->deadline) {
        call->stop_kind = "deadline";
        call->message = "cell returned after its execution deadline";
    }
    for (CellProgram *owned = cell_programs; owned != NULL; owned = owned->next) {
        if (owned->admission_owner == call) { owned->admitted = 0; owned->admission_owner = NULL; }
        if (owned->loading_owner == call) { owned->busy = 0; owned->loading_owner = NULL; }
        if (owned->initializing_owner == call) {
            owned->initialized = -2; owned->initializing_owner = NULL;
            owned->initialization_stop = call->stop_kind;
            owned->initialization_message = call->message;
            owned->initialization_recipe = call->recipe;
            owned->initialization_unit = call->unit;
        }
    }
    stage_unwind(saved_stage_depth, 3, call->stop_kind != NULL ? call->stop_kind : "cell-return");
    /* Request sockets cannot escape an interrupted HTTP exchange. Explicitly
     * offered socket handles have no request owner and keep their lifetime. */
    for (size_t i = 0; i < socket_descriptor_count; ++i) {
        if (socket_descriptors[i].request_owner != call) continue;
        int released = close(socket_descriptors[i].descriptor) == 0;
        socket_descriptors[i] = (SocketHandle){INVALID_DESCRIPTOR, 0, NULL};
        if (!released) {
            call->errors += 1;
            if (call->stop_kind == NULL) { call->stop_kind = "socket-release"; call->message = "request socket release incomplete"; }
        }
    }
    for (size_t i = 0; i < file_lease_count; ++i) {
        if (file_leases[i].owner != call) continue;
        if (!file_lease_close(i)) {
            // A failed release keeps its handle available for local care after
            // this call's frame is gone; never retain a dangling owner pointer.
            file_leases[i].owner = NULL;
            call->errors += 1;
            if (call->stop_kind == NULL) { call->stop_kind="file-release"; call->message="file lease release incomplete"; }
        }
    }
    while (frame_count > 0) pop_frame();
    free(frames); free(locals);
    frames = saved_frames; frame_count = saved_frame_count; frame_capacity = saved_frame_capacity;
    locals = saved_locals; local_count = saved_local_count; local_capacity = saved_local_capacity;
    if (call->execution_complete && call->errors == 0 &&
        (call->stop_kind == NULL || strcmp(call->stop_kind, "deadline") == 0)) {
        program->answer = answer;
        program->initialized = 1;
    }
    select_program(saved_program);
    program->busy -= 1;
    finished = saved_finished; result_ready = saved_result_ready;
    failure_message = saved_failure; failure_reported = saved_reported; evaluation_recovery = saved_recovery; parser_choice = saved_choice;
    restore_source(saved_source); source_path = saved_path; entry_source_bytes = saved_entry; entry_source_length = saved_entry_length;
    active_cell = call->parent;
    fclose(call->out); fclose(call->err);
    Value result = cell_result(call, answer, (long long)(monotonic_ns() / 1000000 - began), cpu_microseconds() - began_cpu);
    free(call->input); free(call->out_bytes); free(call->err_bytes); free(call);
    heap_collect(result);
    return result;
}

static Value offer_cell_run(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return cell_execute(arguments[0], arguments[1], arguments[2], arguments[3], 1);
}

static Value offer_cell_call(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return cell_execute(arguments[0], arguments[1], arguments[2], nothing_value(), 0);
}

int main(int argc, char **argv) {
    setvbuf(stdin, NULL, _IONBF, 0);
    runtime_executable = argv[0];
    admit_builder();
    process_born_ms = unix_milliseconds();
    if (argc < PROGRAM_ARGUMENT_COUNT) {
        fputs(USAGE_TEXT, stderr);
        return EXIT_FAILURE;
    }
    register_builtin_operations();
    staged_input = (const unsigned char *)(argc > PROGRAM_ARGUMENT_COUNT ? argv[PROGRAM_ARGUMENT_COUNT] : "");
    staged_input_length = strlen((const char *)staged_input);
    source_path = argv[SOURCE_PATH_ARGUMENT];
    main_program.path = strdup(source_path);
    source_bytes = read_source(source_path, &source_length);
    entry_source_bytes = source_bytes;
    entry_source_length = source_length;
    main_program.bytes = source_bytes; main_program.length = source_length;
    Form field_name = {0};
    field_name.kind = FORM_SYMBOL; field_name.text = "nfs-entry"; field_name.text_length = 9;
    Form *field_names[] = {&field_name};
    link_home_names(field_names, 1, source_path);
    Binding *field_home = find_function_text("nfs-entry", 9);
    if (field_home == NULL) fail("native field Form owner is unavailable");
    native_field_entry = field_home->value;
    int entry_admitted=main_program.forms!=NULL;
    size_t form_count=main_program.form_count;
    Form **forms=main_program.forms;
    if (!entry_admitted) {
        remember_unit(copy_bytes(source_path, strlen(source_path)));
        load_preludes(source_bytes, source_length, source_path);
        source_offset = FIRST_INDEX;
        forms = parse_current_source(&form_count);
        bind_definitions(forms, form_count);
        link_home_names(forms, form_count, source_path);
        main_program.forms = forms; main_program.form_count = form_count;
    }
    load_unit(source_path, "form-stdlib/bml/kernel-ast.bml");
    capture_program(&main_program);
    check_program_bindings(&main_program);
    if (main_program.diagnostic_text.kind == KIND_TEXT) fwrite(main_program.diagnostic_text.text, 1, main_program.diagnostic_text.text_length, stderr);
    if (main_program.binding_refusal) return EXIT_FAILURE;
    if (entry_admitted) finished=main_program.answer;
    else drive_forms(forms, form_count);
    print_value(finished);
    fputs(LINE_ENDING, stdout);
    if (fflush(stdout) != SYSTEM_CALL_OK || ferror(stdout)) fail("the result could not be written");
    return main_program.errors != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
