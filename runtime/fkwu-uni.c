/* The Form runtime reads one source file and offers its forms.
 * A word is nothing, an integer, a real, text, a list, a function, or a node.
 * Definitions are bound before any form runs, so a later definition is visible.
 * A prelude line names sources whose definitions join this program.
 * A form.bml section lowers its definitions into the same forms.
 * Words live until the process exits. Collections grow with the data.
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdint.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

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
static const uint64_t BUS_MAGIC = 0x4642555332ULL;
static const uint64_t BUS_LAYOUT_VERSION = 1;
static const uint64_t NANOSECONDS_PER_SECOND = 1000000000ULL;
static const mode_t SHARED_OBJECT_MODE = 0600;
static const char *const BUS_FILE_PATH = "/tmp/fg-bus2";

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
static const char *const KIND_NAME_NOTHING = "nothing";
static const char *const KIND_NAME_INTEGER = "int";
static const char *const KIND_NAME_REAL = "float";
static const char *const KIND_NAME_TEXT = "string";
static const char *const KIND_NAME_LIST = "list";
static const char *const KIND_NAME_FUNCTION = "function";
static const char *const KIND_NAME_NODE = "node";
static const char *const KIND_NAME_RECORD = "record";
static const char *const LIST_OPEN = "[";
static const char *const LIST_SEPARATOR = ", ";
static const char *const LIST_CLOSE = "]";
static const char *const LINE_ENDING = "\n";
static const char *const SOURCE_OPEN_MODE = "rb";
static const char *const INTEGER_FORMAT = "%lld";
static const char *const REAL_FORMAT = "%.*g";
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
static const char *const SECTION_TAG = "section [";
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
static const uint64_t ROSTER_MAGIC = 0x46524F5354455231ULL;
static const uint64_t NODE_GIFT_MAGIC = 0x4E4F444547494654ULL;
static const size_t NODE_REFERENCE_BYTES = 64;
static const char *const TEMP_DIRECTORY = "/tmp";
static const char *const METAL_ABSENT_STATUS = "metal=not-up\n";
static const char *const SHELL_PATH = "/bin/sh";
static const char *const SHELL_COMMAND = "-c";
static const size_t ROSTER_SEQUENCE = 0;
static const size_t ROSTER_MAGIC_WORD = 1;
static const size_t ROSTER_COUNT = 2;
static const size_t ROSTER_USED = 3;
static const size_t ROSTER_HEADER_WORDS = 4;
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
static const long long SCAN_NONQUOTE = 4;
static const long long SCAN_LINE = 5;
static const long long SCAN_JSONSAFE = 6;
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
    OP_INTERN_BOOL,
    OP_KERNEL_STAT,
    OP_FB_RECORD,
    OP_FB_EVENTS,
    OP_FB_CLEAR,
    OP_BUS_OPEN,
    OP_BUS_CLOSE,
    OP_BUS_READ,
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
    BUS_STAGE = 0,
    BUS_KEY = 1,
    BUS_INDEX = 0,
    BUS_OUTCOME = 1,
    BUS_CURSOR = 0,
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
    STAT_OWNER_CONSTRUCTION_CLOCK = 164,
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
    LIVE_MINTS = 33,
    LIVE_PAGE_WORDS = 34,
    LIVE_NAME_ROOM = 32,
    STACK_TRACE_DEPTH = 12,
    GIFT_REF_MAGIC = 0,
    GIFT_REF_GENERATION = 1,
    GIFT_REF_PID = 2,
    GIFT_REF_START = 3,
    GIFT_REF_LIFETIME = 4,
    GIFT_REF_WORDS = 8,
    GIFT_CHILD_PAYLOAD = 0,
    GIFT_CHILD_PID = 1,
    GIFT_CHILD_START = 2,
    GIFT_CHILD_GENERATION = 3,
    GIFT_CHILD_LIFETIME = 4,
    GIFT_ROOT_CHILDREN = 5,
    GIFT_LIFETIME_PROCESS = 1,
    GIFT_ROOT_PACKAGE = 2,
    GIFT_ROOT_LEVEL = 1,
    GIFT_ROOT_TYPE = 2,
    GIFT_ROOT_INSTANCE = 1,
    BUS_HEADER_MAGIC = 0,
    BUS_HEADER_LAYOUT = 1,
    BUS_HEADER_COUNT = 2,
    BUS_HEADER_END = 3,
    BUS_HEADER_WORDS = 4,
    BUS_PHASE_OPEN = 1,
    BUS_PHASE_CLOSE = 2,
    BUS_RECORD_FIELDS = 6,
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
};

struct List {
    Value head;
    List *tail;
    Value *flat;
    size_t flat_count;
    List *indexed;
    size_t lead;
};

typedef enum { CELL_TEXT, CELL_INTEGER, CELL_COMPOSITE } CellKind;

struct Cell {
    fk_node_id category;
    CellKind kind;
    long long integer;
    char *text;
    size_t text_length;
    Value *children;
    size_t child_count;
    char *source;
    size_t source_length;
    long long source_line;
    long long source_column;
    uint64_t hash;
};

struct Record {
    Value blueprint;
    Value *keys;
    Value *values;
    size_t count;
    size_t capacity;
};

struct Form {
    FormKind kind;
    size_t offset;
    long long integer;
    double real;
    char *text;
    size_t text_length;
    Form **parts;
    size_t part_count;
    size_t part_capacity;
    int is_bound;
};

struct Binding {
    const char *bytes;
    size_t length;
    Value value;
};

struct Operation {
    const char *spelling;
    size_t arity;
    ArityPolicy arity_policy;
    EvaluationPolicy evaluation;
    OperationId id;
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
};

static const char *source_bytes;
static size_t source_length;
static size_t source_offset;
static const char *source_path;

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
static size_t global_capacity;

static Frame *frames;
static size_t frame_count;
static size_t frame_capacity;
static Value finished;
static int result_ready;

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
static size_t owner_construction_clock;

static int shared_descriptor = INVALID_DESCRIPTOR;
static uint64_t *shared_header;
static size_t shared_mapped_bytes;
static int shared_unavailable;
static size_t shared_maps;
static size_t shared_bytes;

typedef struct {
    Value blueprint;
    char *name;
    size_t name_length;
    Value function;
} MethodDefinition;

static MethodDefinition *method_definitions;
static size_t method_definition_count;
static size_t method_definition_capacity;

static int *socket_descriptors;
static size_t socket_descriptor_count;
static size_t socket_descriptor_capacity;

static void note_stack(void) {
    size_t shown = EMPTY_COUNT;
    for (size_t index = frame_count; index > FIRST_INDEX && shown < STACK_TRACE_DEPTH; ) {
        index -= ONE_ELEMENT;
        Form *form = frames[index].form;
        if (form == NULL || form->kind != FORM_GROUP || form->part_count == EMPTY_COUNT) continue;
        Form *head = form->parts[CALL_HEAD];
        if (head->kind != FORM_SYMBOL || head->text == NULL) continue;
        fprintf(stderr, "  %.*s at byte %zu\n", (int)head->text_length, head->text, form->offset);
        shown += ONE_ELEMENT;
    }
}

static void fail(const char *reason) {
    fprintf(stderr, ERROR_BARE, reason);
    note_stack();
    exit(EXIT_FAILURE);
}

static void fail_at(size_t offset, const char *reason) {
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

static Value real_value(double real) {
    Value value = nothing_value();
    value.kind = KIND_REAL;
    value.real = real;
    return value;
}

static Value text_value(const char *bytes, size_t length) {
    Value value = nothing_value();
    value.kind = KIND_TEXT;
    value.text = copy_bytes(bytes, length);
    value.text_length = length;
    return value;
}

static const size_t LIST_DIRECT = 64;

static Value list_at(Value list, size_t index) {
    List *cell = list.list;
    if (cell == NULL || index >= list.item_count) return nothing_value();
    if (cell->flat != NULL && cell->flat_count == list.item_count && index < cell->flat_count) return cell->flat[index];
    if (cell->indexed != NULL && index >= cell->lead) {
        size_t within = index - cell->lead;
        if (within < cell->indexed->flat_count) return cell->indexed->flat[within];
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
        return flat[index];
    }
    for (size_t step = FIRST_INDEX; step < index; step = checked_add(step, ONE_ELEMENT)) cell = cell->tail;
    return cell->head;
}

static Value node_value(long long identity) {
    Value value = nothing_value();
    value.kind = KIND_NODE;
    value.integer = identity;
    return value;
}

static int values_equal(Value left, Value right);
static int inside_function(void);
static void append_local(const char *bytes, size_t length, Value value);

static Value function_from_parameters(Form *parameters, Form *body, int capture_visible) {
    if (parameters->kind != FORM_GROUP) fail_at(parameters->offset, "a parameter list must be a group");
    Value function = nothing_value();
    function.kind = KIND_FUNCTION;
    function.body = body;
    function.parameter_count = parameters->part_count;
    function.parameters = allocate_zeroed(parameters->part_count, sizeof *function.parameters);
    for (size_t index = FIRST_INDEX; index < parameters->part_count; index = checked_add(index, ONE_ELEMENT)) {
        Form *parameter = parameters->parts[index];
        if (parameter->kind != FORM_SYMBOL) fail_at(parameter->offset, "a parameter must be a symbol");
        for (size_t earlier = FIRST_INDEX; earlier < index; earlier = checked_add(earlier, ONE_ELEMENT)) {
            Form *previous = parameters->parts[earlier];
            if (bytes_equal(previous->text, previous->text_length, parameter->text, parameter->text_length)) {
                fail_at(parameter->offset, "a parameter name is repeated");
            }
        }
        function.parameters[index].bytes = parameter->text;
        function.parameters[index].length = parameter->text_length;
    }
    if (!capture_visible) return function;
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
    if (base > local_count) fail("a function looked above its locals");
    size_t visible = local_count - base;
    size_t total = checked_add(outer_count, visible);
    if (total == EMPTY_COUNT) return function;
    Binding *captures = allocate_zeroed(total, sizeof *captures);
    if (outer_count != EMPTY_COUNT) memcpy(captures, outer, outer_count * sizeof *captures);
    if (visible != EMPTY_COUNT) memcpy(captures + outer_count, locals + base, visible * sizeof *captures);
    function.captures = captures;
    function.capture_count = total;
    return function;
}
static void expect_kind(Value value, ValueKind kind, const char *reason);
static Value list_from(Value *items, size_t item_count);
static const char *kind_name(ValueKind kind);
static Value boolean_word(int predicate);
static int unwind_to_recovery(void);
static Value invoke_function_value(Value function, Value *arguments, size_t argument_count);

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
    if (value.kind == KIND_NODE) hash = hash_mix(hash, (uint64_t)(uintptr_t)value.cell);
    if (value.kind == KIND_TEXT) hash = hash_bytes(hash, value.text, value.text_length);
    if (value.kind == KIND_REAL) {
        uint64_t bits = 0;
        memcpy(&bits, &value.real, sizeof bits);
        hash = hash_mix(hash, bits);
    }
    if (value.kind == KIND_LIST) {
        hash = hash_mix(hash, (uint64_t)value.item_count);
        for (List *cell = value.list; cell != NULL; cell = cell->tail) hash = hash_mix(hash, hash_value(cell->head));
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
    if (node.kind == KIND_REAL) {
        if (field == NODE_FIELD_PACKAGE) return integer_value(1);
        if (field == NODE_FIELD_LEVEL) return integer_value(1);
        if (field == NODE_FIELD_TYPE) return integer_value(7);
        if (field == NODE_FIELD_INSTANCE) return integer_value(0);
    }
    if (node.kind != KIND_NODE) {
        char detail[80];
        snprintf(detail, sizeof detail, "%s %lld", kind_name(node.kind), node.integer);
        fail(message_with_name("a node field expects a node, received ", detail, strlen(detail)));
    }
    (void)reason;
    return integer_value(fk_nid_get(node_word(node, reason), field));
}

static uint64_t text_cell_hash(const char *text, size_t length) {
    return hash_mix(hash_bytes(hash_mix(VALUE_HASH_OFFSET, (uint64_t)CELL_TEXT), text, length), (uint64_t)length);
}

static uint64_t integer_cell_hash(long long integer) {
    return hash_mix(hash_mix(VALUE_HASH_OFFSET, (uint64_t)CELL_INTEGER), (uint64_t)integer);
}

static uint64_t composite_hash(fk_node_id category, Value *children, size_t count) {
    uint64_t hash = hash_mix(hash_mix(VALUE_HASH_OFFSET, (uint64_t)CELL_COMPOSITE), category);
    hash = hash_mix(hash, (uint64_t)count);
    for (size_t index = FIRST_INDEX; index < count; index = checked_add(index, ONE_ELEMENT)) {
        hash = hash_mix(hash, hash_value(children[index]));
    }
    return hash;
}

static CellBucket *cell_chain(uint64_t hash) {
    if (cell_bucket_count == EMPTY_COUNT) return NULL;
    return cell_buckets[(size_t)(hash & (uint64_t)(cell_bucket_count - ONE_ELEMENT))];
}

static Cell *find_text_cell(const char *text, size_t length) {
    uint64_t hash = text_cell_hash(text, length);
    for (CellBucket *slot = cell_chain(hash); slot != NULL; slot = slot->next) {
        Cell *cell = slot->cell;
        if (cell->hash == hash && cell->kind == CELL_TEXT && bytes_equal(cell->text, cell->text_length, text, length)) return cell;
    }
    return NULL;
}

static Cell *find_integer_cell(long long integer) {
    uint64_t hash = integer_cell_hash(integer);
    for (CellBucket *slot = cell_chain(hash); slot != NULL; slot = slot->next) {
        Cell *cell = slot->cell;
        if (cell->hash == hash && cell->kind == CELL_INTEGER && cell->integer == integer) return cell;
    }
    return NULL;
}

static Cell *find_composite(fk_node_id category, Value *children, size_t count, uint64_t hash) {
    for (CellBucket *slot = cell_chain(hash); slot != NULL; slot = slot->next) {
        Cell *cell = slot->cell;
        if (cell->hash != hash || cell->kind != CELL_COMPOSITE || cell->category != category || cell->child_count != count) continue;
        int same = 1;
        for (size_t child = FIRST_INDEX; child < count; child = checked_add(child, ONE_ELEMENT)) {
            if (!values_equal(cell->children[child], children[child])) same = 0;
        }
        if (same) return cell;
    }
    return NULL;
}

static Cell *composite_cell(Value category, Value children) {
    expect_kind(category, KIND_NODE, "intern_node expects a category");
    expect_kind(children, KIND_LIST, "intern_node expects children");
    fk_node_id word = node_word(category, "intern_node expects a category");
    Value *flat = NULL;
    if (children.item_count != EMPTY_COUNT) {
        flat = allocate_zeroed(children.item_count, sizeof *flat);
        List *cursor = children.list;
        for (size_t index = FIRST_INDEX; index < children.item_count; index = checked_add(index, ONE_ELEMENT)) {
            flat[index] = cursor->head;
            cursor = cursor->tail;
        }
    }
    uint64_t hash = composite_hash(word, flat, children.item_count);
    Cell *found = find_composite(word, flat, children.item_count, hash);
    if (found != NULL) {
        free(flat);
        return found;
    }
    Cell *cell = cell_hold();
    cell->kind = CELL_COMPOSITE;
    cell->category = word;
    cell->child_count = children.item_count;
    cell->children = flat;
    cell->hash = hash;
    cell_index(cell);
    return cell;
}

static Value offer_make_nodeid(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[NODE_FIELD_PACKAGE], KIND_INTEGER, "make_nodeid expects a package");
    expect_kind(arguments[NODE_FIELD_LEVEL], KIND_INTEGER, "make_nodeid expects a level");
    expect_kind(arguments[NODE_FIELD_TYPE], KIND_INTEGER, "make_nodeid expects a type");
    expect_kind(arguments[NODE_FIELD_INSTANCE], KIND_INTEGER, "make_nodeid expects an instance");
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

static Value offer_bp(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NODE) return arguments[SOLE_OPERAND];
    return node_value(WORD_FALSE);
}

static Value offer_intern_trivial_string(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "intern_trivial_string expects text");
    Cell *found = find_text_cell(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    if (found == NULL) {
        found = cell_hold();
        found->kind = CELL_TEXT;
        found->text = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
        found->text_length = arguments[SOLE_OPERAND].text_length;
        found->hash = text_cell_hash(found->text, found->text_length);
        cell_index(found);
    }
    if (found->category == 0) found->category = fk_nid_make(1, 1, 2, 0);
    return node_from_cell(found);
}

static Value offer_intern_trivial_int(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "intern_trivial_int expects an integer");
    Cell *found = find_integer_cell(arguments[SOLE_OPERAND].integer);
    if (found == NULL) {
        found = cell_hold();
        found->kind = CELL_INTEGER;
        found->integer = arguments[SOLE_OPERAND].integer;
        found->hash = integer_cell_hash(found->integer);
        cell_index(found);
    }
    if (found->category == 0) found->category = fk_nid_make(1, 1, 1, 0);
    return node_from_cell(found);
}

static Value offer_intern_node(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return node_from_cell(composite_cell(arguments[LEFT_OPERAND], arguments[RIGHT_OPERAND]));
}

static Value offer_node_children(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_REAL) return list_from(NULL, EMPTY_COUNT);
    expect_kind(arguments[SOLE_OPERAND], KIND_NODE, "node_children expects a node");
    Cell *cell = arguments[SOLE_OPERAND].cell;
    if (cell == NULL || cell->child_count == EMPTY_COUNT) return list_from(NULL, EMPTY_COUNT);
    return list_from(cell->children, cell->child_count);
}

static Value offer_node_category(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[SOLE_OPERAND].kind == KIND_REAL) return nothing_value();
    if (arguments[SOLE_OPERAND].kind != KIND_NODE) {
        fail(message_with_name("node_category expects a node, received ", kind_name(arguments[SOLE_OPERAND].kind), strlen(kind_name(arguments[SOLE_OPERAND].kind))));
    }
    if (arguments[SOLE_OPERAND].cell == NULL || arguments[SOLE_OPERAND].cell->kind != CELL_COMPOSITE) return arguments[SOLE_OPERAND];
    return node_value((long long)arguments[SOLE_OPERAND].cell->category);
}

static Value offer_node_payload(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[SOLE_OPERAND].kind == KIND_REAL) return arguments[SOLE_OPERAND];
    expect_kind(arguments[SOLE_OPERAND], KIND_NODE, "node_value expects a node");
    Cell *cell = arguments[SOLE_OPERAND].cell;
    if (cell == NULL) return integer_value(arguments[SOLE_OPERAND].integer);
    if (cell->kind == CELL_TEXT) return text_value(cell->text, cell->text_length);
    if (cell->kind == CELL_INTEGER) return integer_value(cell->integer);
    return nothing_value();
}

static Value offer_node_eq(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (left.kind == KIND_REAL && right.kind == KIND_REAL) return boolean_word(left.real == right.real);
    if (left.kind != KIND_NODE || right.kind != KIND_NODE) return boolean_word(0);
    if (left.cell != NULL && right.cell != NULL) return boolean_word(left.cell == right.cell);
    long long left_word = left.cell != NULL && left.cell->kind == CELL_COMPOSITE ? (long long)left.cell->category : left.integer;
    long long right_word = right.cell != NULL && right.cell->kind == CELL_COMPOSITE ? (long long)right.cell->category : right.integer;
    return boolean_word(left_word == right_word);
}

static Value offer_node_source(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_REAL) return list_from(NULL, EMPTY_COUNT);
    if (arguments[SOLE_OPERAND].kind != KIND_NODE) {
        fail(message_with_name("node_source expects a node, received ", kind_name(arguments[SOLE_OPERAND].kind), strlen(kind_name(arguments[SOLE_OPERAND].kind))));
    }
    Cell *cell = arguments[SOLE_OPERAND].cell;
    if (cell == NULL || cell->source == NULL) return list_from(NULL, EMPTY_COUNT);
    Value *fields = allocate_zeroed(ARITY_THREE, sizeof *fields);
    fields[NODE_FIELD_PACKAGE] = text_value(cell->source, cell->source_length);
    fields[NODE_FIELD_LEVEL] = integer_value(cell->source_line);
    fields[NODE_FIELD_TYPE] = integer_value(cell->source_column);
    return list_from(fields, ARITY_THREE);
}

static Value offer_intern_node_at(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[ARITY_TWO], KIND_TEXT, "intern_node_at expects a source path");
    expect_kind(arguments[ARITY_THREE], KIND_INTEGER, "intern_node_at expects a line");
    expect_kind(arguments[ARITY_FOUR], KIND_INTEGER, "intern_node_at expects a column");
    Cell *cell = composite_cell(arguments[LEFT_OPERAND], arguments[RIGHT_OPERAND]);
    if (cell->source == NULL) {
        cell->source = copy_bytes(arguments[ARITY_TWO].text, arguments[ARITY_TWO].text_length);
        cell->source_length = arguments[ARITY_TWO].text_length;
        cell->source_line = arguments[ARITY_THREE].integer;
        cell->source_column = arguments[ARITY_FOUR].integer;
    }
    return node_from_cell(cell);
}

static Value boolean_word(int predicate) {
    return integer_value(predicate ? WORD_TRUE : WORD_FALSE);
}

static int integer_is_nonzero(Value value) {
    return value.kind == KIND_INTEGER && value.integer != WORD_FALSE;
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

static int text_matches(Value value, const char *literal) {
    return value.kind == KIND_TEXT && bytes_equal(value.text, value.text_length, literal, strlen(literal));
}

static int byte_from_value(Value value, unsigned char *out) {
    if (value.kind != KIND_INTEGER || value.integer < WORD_FALSE || value.integer > UCHAR_MAX) return 0;
    *out = (unsigned char)value.integer;
    return 1;
}

static int list_to_bytes(Value list, unsigned char **bytes, size_t *length) {
    if (list.kind == KIND_NOTHING) {
        *bytes = NULL;
        *length = EMPTY_COUNT;
        return 1;
    }
    if (list.kind != KIND_LIST) return 0;
    *length = list.item_count;
    *bytes = allocate_zeroed(list.item_count == EMPTY_COUNT ? ONE_ELEMENT : list.item_count, sizeof **bytes);
    List *cursor = list.list;
    for (size_t index = FIRST_INDEX; index < list.item_count; index = checked_add(index, ONE_ELEMENT)) {
        unsigned char byte = 0;
        if (cursor == NULL || !byte_from_value(cursor->head, &byte)) return 0;
        (*bytes)[index] = byte;
        cursor = cursor->tail;
    }
    return 1;
}

static Value bytes_to_list(const unsigned char *bytes, size_t length) {
    Value *items = allocate_zeroed(length, sizeof *items);
    for (size_t index = FIRST_INDEX; index < length; index = checked_add(index, ONE_ELEMENT)) {
        items[index] = integer_value((long long)bytes[index]);
    }
    return list_from(items, length);
}

static int path_exists(const char *path) {
    struct stat info;
    return stat(path, &info) == SYSTEM_CALL_OK;
}

static int path_is_dir(const char *path) {
    struct stat info;
    if (stat(path, &info) != SYSTEM_CALL_OK) return 0;
    return S_ISDIR(info.st_mode) ? 1 : 0;
}

static int socket_put(int descriptor) {
    size_t needed = checked_add(socket_descriptor_count, ONE_ELEMENT);
    socket_descriptors = grow_list(socket_descriptors, &socket_descriptor_capacity, sizeof *socket_descriptors, needed);
    socket_descriptors[socket_descriptor_count] = descriptor;
    socket_descriptor_count = needed;
    return (int)socket_descriptor_count;
}

static int socket_fetch(long long handle) {
    if (handle <= WORD_FALSE) return INVALID_DESCRIPTOR;
    if ((unsigned long long)handle > socket_descriptor_count) return INVALID_DESCRIPTOR;
    int descriptor = socket_descriptors[handle - ONE_ELEMENT];
    return descriptor;
}

static int socket_release(long long handle) {
    if (handle <= WORD_FALSE) return 0;
    if ((unsigned long long)handle > socket_descriptor_count) return 0;
    socket_descriptors[handle - ONE_ELEMENT] = INVALID_DESCRIPTOR;
    return 1;
}

static int is_number(Value value) {
    return value.kind == KIND_INTEGER || value.kind == KIND_REAL;
}

static double number_as_real(Value value) {
    if (value.kind == KIND_REAL) return value.real;
    return (double)value.integer;
}

static long long checked_integer_add(long long left, long long right) {
    long long sum = WORD_FALSE;
    if (__builtin_saddll_overflow(left, right, &sum)) fail("add is outside the native integer range");
    return sum;
}

static long long checked_integer_sub(long long left, long long right) {
    long long difference = WORD_FALSE;
    if (__builtin_ssubll_overflow(left, right, &difference)) fail("sub is outside the native integer range");
    return difference;
}

static long long checked_integer_mul(long long left, long long right) {
    long long product = WORD_FALSE;
    if (__builtin_smulll_overflow(left, right, &product)) fail("mul is outside the native integer range");
    return product;
}

static long long checked_integer_div(long long left, long long right) {
    if (right == WORD_FALSE) fail("div expects a nonzero divisor");
    if (left == LLONG_MIN && right == -WORD_TRUE) fail("div is outside the native integer range");
    return left / right;
}

static long long checked_integer_mod(long long left, long long right) {
    if (right == WORD_FALSE) fail("mod expects a nonzero divisor");
    if (left == LLONG_MIN && right == -WORD_TRUE) return WORD_FALSE;
    return left % right;
}

static Form *new_form(FormKind kind, size_t offset) {
    Form *form = allocate_zeroed(ONE_ELEMENT, sizeof *form);
    form->kind = kind;
    form->offset = offset;
    return form;
}

static void group_add(Form *group, Form *child) {
    size_t needed = checked_add(group->part_count, ONE_ELEMENT);
    group->parts = grow_list(group->parts, &group->part_capacity, sizeof *group->parts, needed);
    group->parts[group->part_count] = child;
    group->part_count = needed;
}

static int form_is_symbol(Form *form, const char *spelling) {
    return form->kind == FORM_SYMBOL && bytes_equal_cstr(form->text, form->text_length, spelling);
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
        if (source_bytes[source_offset] == COMMENT_INTRODUCER) {
            int at_line_start = 1;
            size_t cursor = source_offset;
            while (cursor > FIRST_INDEX) {
                char previous = source_bytes[cursor - ONE_ELEMENT];
                if (previous == NEWLINE) break;
                if (!isspace((unsigned char)previous)) {
                    at_line_start = 0;
                    break;
                }
                cursor -= ONE_ELEMENT;
            }
            if (at_line_start) {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
                while (source_offset < source_length && source_bytes[source_offset] != NEWLINE) {
                    source_offset = checked_add(source_offset, ONE_ELEMENT);
                }
                continue;
            }
        }
        if (source_bytes[source_offset] != LINE_COMMENT_MARK) return;
        size_t next = checked_add(source_offset, ONE_ELEMENT);
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
        long long integer = strtoll(text, NULL, DECIMAL_RADIX);
        if (errno == ERANGE) fail_at(form->offset, "an integer is outside the native range");
        form->kind = FORM_INTEGER;
        form->integer = integer;
        return 1;
    }
    double number = strtod(text, NULL);
    if (errno == ERANGE || !isfinite(number)) fail_at(form->offset, "a real is outside the native range");
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

static Form **parse_source(size_t *form_count) {
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
        Form *atom = character == TEXT_DELIMITER ? parse_text() : parse_atom();
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

static void register_operation(const char *spelling, size_t arity, ArityPolicy arity_policy,
                               EvaluationPolicy evaluation, OperationId id, OperationOffer offer) {
    size_t needed = checked_add(operation_count, ONE_ELEMENT);
    operations = grow_list(operations, &operation_capacity, sizeof *operations, needed);
    operations[operation_count].spelling = spelling;
    operations[operation_count].arity = arity;
    operations[operation_count].arity_policy = arity_policy;
    operations[operation_count].evaluation = evaluation;
    operations[operation_count].id = id;
    operations[operation_count].offer = offer;
    operations[operation_count].calls = EMPTY_COUNT;
    operations[operation_count].answers = 0;
    operation_count = needed;
}

static void operation_answers(const char *spelling) {
    for (size_t index = FIRST_INDEX; index < operation_count; index = checked_add(index, ONE_ELEMENT)) {
        if (bytes_equal_cstr(spelling, strlen(spelling), operations[index].spelling)) operations[index].answers = 1;
    }
}

static const Operation *find_operation_text(const char *bytes, size_t length) {
    for (size_t index = FIRST_INDEX; index < operation_count; index = checked_add(index, ONE_ELEMENT)) {
        if (bytes_equal_cstr(bytes, length, operations[index].spelling)) return &operations[index];
    }
    return NULL;
}

static Binding *find_function_text(const char *bytes, size_t length) {
    for (size_t index = FIRST_INDEX; index < function_count; index = checked_add(index, ONE_ELEMENT)) {
        if (bytes_equal(functions[index].bytes, functions[index].length, bytes, length)) return &functions[index];
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
    Value function = function_from_parameters(parameters, form->parts[DEFN_BODY], 0);
    Binding *existing = find_function_text(name->text, name->text_length);
    if (existing != NULL) {
        existing->value = function;
        form->is_bound = 1;
        return;
    }
    size_t needed = checked_add(function_count, ONE_ELEMENT);
    functions = grow_list(functions, &function_capacity, sizeof *functions, needed);
    functions[function_count].bytes = name->text;
    functions[function_count].length = name->text_length;
    functions[function_count].value = function;
    function_count = needed;
    form->is_bound = 1;
}

static void bind_runtime_definition(Form *form) {
    if (form->part_count != DEFN_PART_COUNT) fail_at(form->offset, "a definition needs a name, parameters, and a body");
    Form *name = form->parts[DEFN_NAME];
    Form *parameters = form->parts[DEFN_PARAMETERS];
    if (name->kind != FORM_SYMBOL) fail_at(name->offset, "a definition name must be a symbol");
    const Operation *reserved = find_operation_text(name->text, name->text_length);
    if (reserved != NULL && reserved->evaluation == LEAVE_ARGUMENTS) {
        fail_at(name->offset, message_with_name("a definition uses a reserved operation name: ", name->text, name->text_length));
    }
    Value function = function_from_parameters(parameters, form->parts[DEFN_BODY], inside_function());
    if (inside_function()) append_local(name->text, name->text_length, function);
    else {
        Binding *existing = find_function_text(name->text, name->text_length);
        if (existing != NULL) {
            existing->value = function;
            form->is_bound = 1;
            return;
        }
        size_t needed = checked_add(function_count, ONE_ELEMENT);
        functions = grow_list(functions, &function_capacity, sizeof *functions, needed);
        functions[function_count].bytes = name->text;
        functions[function_count].length = name->text_length;
        functions[function_count].value = function;
        function_count = needed;
        form->is_bound = 1;
    }
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
    if (!isfinite(sum)) fail("add is outside the native real range");
    return real_value(sum);
}

static Value offer_sub(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("sub expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return integer_value(checked_integer_sub(left.integer, right.integer));
    double difference = number_as_real(left) - number_as_real(right);
    if (!isfinite(difference)) fail("sub is outside the native real range");
    return real_value(difference);
}

static Value offer_eq(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return boolean_word(values_equal(arguments[LEFT_OPERAND], arguments[RIGHT_OPERAND]));
}

static Value offer_get(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value carrier = arguments[LEFT_OPERAND];
    Value key = arguments[RIGHT_OPERAND];
    if (carrier.kind == KIND_LIST) {
        if (carrier.item_count >= ONE_ELEMENT && text_matches(carrier.list->head, "__dict__") && key.kind == KIND_TEXT) {
            List *cursor = carrier.list->tail;
            while (cursor != NULL && cursor->tail != NULL) {
                if (cursor->head.kind == KIND_TEXT && bytes_equal(cursor->head.text, cursor->head.text_length, key.text, key.text_length)) {
                    return cursor->tail->head;
                }
                cursor = cursor->tail->tail;
            }
            return integer_value(WORD_FALSE);
        }
        if (key.kind != KIND_INTEGER) return integer_value(WORD_FALSE);
        if (carrier.item_count == EMPTY_COUNT) return integer_value(WORD_FALSE);
        if (key.integer < WORD_FALSE) return carrier.list->head;
        size_t index = (size_t)key.integer;
        if (index >= carrier.item_count) return integer_value(WORD_FALSE);
        return list_at(carrier, index);
    }
    return integer_value(WORD_FALSE);
}

static Value offer_lt(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (left.kind == KIND_NOTHING || right.kind == KIND_NOTHING) return boolean_word(0);
    if (!is_number(left) || !is_number(right)) fail("lt expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return boolean_word(left.integer < right.integer);
    return boolean_word(number_as_real(left) < number_as_real(right));
}

static Value offer_ge(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (left.kind == KIND_NOTHING || right.kind == KIND_NOTHING) return boolean_word(0);
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
    if (left.kind == KIND_NOTHING || right.kind == KIND_NOTHING) return boolean_word(0);
    if (!is_number(left) || !is_number(right)) fail("gt expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return boolean_word(left.integer > right.integer);
    return boolean_word(number_as_real(left) > number_as_real(right));
}

static Value offer_le(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (left.kind == KIND_NOTHING || right.kind == KIND_NOTHING) return boolean_word(0);
    if (!is_number(left) || !is_number(right)) fail("le expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return boolean_word(left.integer <= right.integer);
    return boolean_word(number_as_real(left) <= number_as_real(right));
}

static Value offer_nth(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[LEFT_OPERAND].kind == KIND_NOTHING || arguments[RIGHT_OPERAND].kind == KIND_NOTHING) return nothing_value();
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
    return sliced;
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
    char *text = malloc(checked_add(length, TEXT_TERMINATOR_SIZE));
    if (text == NULL) fail("out of memory");
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

static Value offer_str(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value value = arguments[SOLE_OPERAND];
    if (value.kind == KIND_TEXT) return value;
    if (value.kind == KIND_INTEGER) return integer_text(value.integer);
    if (value.kind == KIND_NOTHING) return text_value("nothing", strlen("nothing"));
    fail("str expects text, an integer, or nothing");
    return nothing_value();
}

static Value offer_print_str(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "print_str expects text");
    if (arguments[SOLE_OPERAND].text_length != EMPTY_COUNT) {
        fwrite(arguments[SOLE_OPERAND].text, sizeof(char), arguments[SOLE_OPERAND].text_length, stdout);
    }
    return nothing_value();
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
    if (!isfinite(product)) fail("mul is outside the native real range");
    return real_value(product);
}

static Value offer_div(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("div expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return integer_value(checked_integer_div(left.integer, right.integer));
    double divisor = number_as_real(right);
    if (divisor == 0.0) fail("div expects a nonzero divisor");
    double quotient = number_as_real(left) / divisor;
    if (!isfinite(quotient)) fail("div is outside the native real range");
    return real_value(quotient);
}

static Value offer_mod(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value left = arguments[LEFT_OPERAND];
    Value right = arguments[RIGHT_OPERAND];
    if (!is_number(left) || !is_number(right)) fail("mod expects two numbers");
    if (left.kind == KIND_INTEGER && right.kind == KIND_INTEGER) return integer_value(checked_integer_mod(left.integer, right.integer));
    double divisor = number_as_real(right);
    if (divisor == 0.0) fail("mod expects a nonzero divisor");
    double remainder = fmod(number_as_real(left), divisor);
    if (!isfinite(remainder)) fail("mod is outside the native real range");
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
    return arguments[SOLE_OPERAND].list->head;
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
    tail.list = tail.list->tail;
    tail.item_count -= ONE_ELEMENT;
    return tail;
}

static Value offer_cons(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[RIGHT_OPERAND], KIND_LIST, "cons expects a list");
    List *cell = allocate_zeroed(ONE_ELEMENT, sizeof *cell);
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
    return list;
}

static int record_find(Record *record, Value key, size_t *index) {
    for (size_t cursor = FIRST_INDEX; cursor < record->count; cursor = checked_add(cursor, ONE_ELEMENT)) {
        if (!values_equal(record->keys[cursor], key)) continue;
        *index = cursor;
        return 1;
    }
    return 0;
}

static Value record_value(Record *record) {
    Value value = nothing_value();
    value.kind = KIND_RECORD;
    value.record = record;
    return value;
}

static Value offer_record_set(Value *arguments, size_t argument_count);

static Value offer_record_new(Value *arguments, size_t argument_count) {
    if (argument_count == EMPTY_COUNT) fail("record_new expects a blueprint or a size");
    owner_construction_clock = checked_add(owner_construction_clock, ONE_ELEMENT);
    size_t field_start = ONE_ELEMENT;
    Record *record = allocate_zeroed(ONE_ELEMENT, sizeof *record);
    if (arguments[FIRST_INDEX].kind == KIND_INTEGER && argument_count == ONE_ELEMENT) {
        record->blueprint = nothing_value();
        return record_value(record);
    }
    record->blueprint = arguments[FIRST_INDEX];
    size_t fields = argument_count - field_start;
    if ((fields % ARITY_TWO) != EMPTY_COUNT) fail("record_new expects key and value pairs");
    Value created = record_value(record);
    for (size_t index = field_start; index < argument_count; index = checked_add(index, ARITY_TWO)) {
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
    return arguments[LEFT_OPERAND].record->values[index];
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

static Value offer_record_type(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return boolean_word(arguments[SOLE_OPERAND].kind == KIND_RECORD);
}

static Value offer_record_blueprint(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_RECORD, "record_blueprint expects a record");
    return arguments[SOLE_OPERAND].record->blueprint;
}

static Value offer_record_keys(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_RECORD, "record_keys expects a record");
    Record *record = arguments[SOLE_OPERAND].record;
    Value *keys = allocate_zeroed(record->count, sizeof *keys);
    for (size_t index = FIRST_INDEX; index < record->count; index = checked_add(index, ONE_ELEMENT)) {
        keys[index] = record->keys[index];
    }
    return list_from(keys, record->count);
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
    if (receiver.kind == KIND_RECORD) return receiver.record->blueprint;
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

static Value offer_method_invoke(Value *arguments, size_t argument_count) {
    if (argument_count < ARITY_TWO) fail("method_invoke expects at least a receiver and name");
    Value receiver = arguments[FIRST_INDEX];
    Value name = arguments[ONE_ELEMENT];
    if (receiver.kind != KIND_RECORD) fail("method_invoke expects a record receiver");
    expect_kind(name, KIND_TEXT, "method_invoke expects a method name");
    ssize_t index = find_method(receiver.record->blueprint, name);
    if (index < 0) fail("method_invoke could not find a method under that name");
    MethodDefinition *method = method_definitions + index;
    size_t invoke_count = checked_add(argument_count - ONE_ELEMENT, ONE_ELEMENT);
    Value *invoke_arguments = allocate_zeroed(invoke_count, sizeof *invoke_arguments);
    invoke_arguments[FIRST_INDEX] = receiver;
    for (size_t argument = ARITY_TWO; argument < argument_count; argument = checked_add(argument, ONE_ELEMENT)) {
        invoke_arguments[argument - ONE_ELEMENT] = arguments[argument];
    }
    return invoke_function_value(method->function, invoke_arguments, invoke_count);
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
    return list_from(items, (size_t)span);
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
    if (mode == SCAN_NONQUOTE) return byte != '"' && byte != '\\';
    if (mode == SCAN_LINE) return byte != '\n';
    if (mode == SCAN_JSONSAFE) return byte >= 32 && byte != '"' && byte != '\\';
    return 0;
}

static Value offer_scan_run(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "scan_run expects text");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "scan_run expects a start");
    expect_kind(arguments[SLICE_END], KIND_INTEGER, "scan_run expects a mode");
    if (arguments[RIGHT_OPERAND].integer < WORD_FALSE) return nothing_value();
    size_t index = (size_t)arguments[RIGHT_OPERAND].integer;
    size_t length = arguments[SOLE_OPERAND].text_length;
    const char *text = arguments[SOLE_OPERAND].text;
    long long mode = arguments[SLICE_END].integer;
    if (mode != SCAN_SPACE && mode != SCAN_DIGITS && mode != SCAN_ALPHA && mode != SCAN_NAME
        && mode != SCAN_NONQUOTE && mode != SCAN_LINE && mode != SCAN_JSONSAFE) return nothing_value();
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
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "substring expects text");
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
    int negative = 0;
    if (length != EMPTY_COUNT && text[FIRST_INDEX] == '-') {
        negative = 1;
        index = ONE_ELEMENT;
    }
    if (index >= length) return nothing_value();
    long long value = WORD_FALSE;
    for (; index < length; index = checked_add(index, ONE_ELEMENT)) {
        unsigned char byte = (unsigned char)text[index];
        if (byte < '0' || byte > '9') return nothing_value();
        if (value > (LLONG_MAX - (byte - '0')) / 10) fail("str_to_int is outside the native range");
        value = value * 10 + (byte - '0');
    }
    return integer_value(negative ? -value : value);
}

static Value offer_str_to_float(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "str_to_float expects text");
    char *text = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    char *end = NULL;
    errno = 0;
    double value = strtod(text, &end);
    if (end == text || errno == ERANGE) {
        free(text);
        return real_value(REAL_ZERO);
    }
    free(text);
    return real_value(value);
}

static Value offer_float_to_int(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_INTEGER) return arguments[SOLE_OPERAND];
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    expect_kind(arguments[SOLE_OPERAND], KIND_REAL, "float_to_int expects a float");
    double value = arguments[SOLE_OPERAND].real;
    if (!isfinite(value) || value > (double)LLONG_MAX || value < (double)LLONG_MIN) fail("float_to_int is outside the native integer range");
    return integer_value((long long)value);
}

static Value offer_make_float32(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[SOLE_OPERAND])) fail("make_float32 expects a number");
    float narrowed = (float)number_as_real(arguments[SOLE_OPERAND]);
    return real_value((double)narrowed);
}

static Value offer_make_float64(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[SOLE_OPERAND])) fail("make_float64 expects a number");
    return real_value(number_as_real(arguments[SOLE_OPERAND]));
}

static Value offer_float_value(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_REAL) return arguments[SOLE_OPERAND];
    if (arguments[SOLE_OPERAND].kind == KIND_INTEGER) return real_value((double)arguments[SOLE_OPERAND].integer);
    if (arguments[SOLE_OPERAND].kind == KIND_NODE && arguments[SOLE_OPERAND].cell != NULL && arguments[SOLE_OPERAND].cell->kind == CELL_INTEGER) {
        return real_value((double)arguments[SOLE_OPERAND].cell->integer);
    }
    return nothing_value();
}

static Value offer_intern_trivial_float(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_TEXT) {
        char *text = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
        char *end = NULL;
        double value = strtod(text, &end);
        free(text);
        if (end == text) return nothing_value();
        return real_value(value);
    }
    if (is_number(arguments[SOLE_OPERAND])) return real_value(number_as_real(arguments[SOLE_OPERAND]));
    return nothing_value();
}

static Value offer_float_leaf(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_INTEGER, "float_leaf expects a mode");
    long long mode = arguments[LEFT_OPERAND].integer;
    if (mode == WORD_FALSE) {
        Value value = arguments[RIGHT_OPERAND];
        if (value.kind == KIND_REAL) return value;
        if (value.kind == KIND_INTEGER) return real_value((double)value.integer);
        return nothing_value();
    }
    if (mode == WORD_TRUE) return offer_make_float32(arguments + RIGHT_OPERAND, ARITY_ONE);
    if (mode == ARITY_TWO) return offer_make_float64(arguments + RIGHT_OPERAND, ARITY_ONE);
    if (mode == ARITY_THREE) return real_value(3.14159265358979323846);
    return nothing_value();
}

static Value offer_math_pi(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    return real_value(3.14159265358979323846);
}

static Value offer_math_sqrt(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[SOLE_OPERAND])) fail("math_sqrt expects a number");
    return real_value(sqrt(number_as_real(arguments[SOLE_OPERAND])));
}

static Value offer_math_exp(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[SOLE_OPERAND])) fail("math_exp expects a number");
    return real_value(exp(number_as_real(arguments[SOLE_OPERAND])));
}

static Value offer_math_log(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[SOLE_OPERAND])) fail("math_log expects a number");
    return real_value(log(number_as_real(arguments[SOLE_OPERAND])));
}

static Value offer_math_pow(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[LEFT_OPERAND]) || !is_number(arguments[RIGHT_OPERAND])) fail("math_pow expects numbers");
    return real_value(pow(number_as_real(arguments[LEFT_OPERAND]), number_as_real(arguments[RIGHT_OPERAND])));
}

static Value offer_round_ndigits(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!is_number(arguments[LEFT_OPERAND])) fail("round_ndigits expects a number");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "round_ndigits expects an integer precision");
    double value = number_as_real(arguments[LEFT_OPERAND]);
    long long digits = arguments[RIGHT_OPERAND].integer;
    if (digits == WORD_FALSE) return real_value(nearbyint(value));
    double factor = pow(10.0, (double)digits);
    if (!isfinite(factor) || factor == REAL_ZERO) return real_value(value);
    return real_value(nearbyint(value * factor) / factor);
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
    if (arguments[RIGHT_OPERAND].integer < WORD_FALSE) return nothing_value();
    size_t index = (size_t)arguments[RIGHT_OPERAND].integer;
    if (index >= arguments[LEFT_OPERAND].text_length) return nothing_value();
    return integer_value((unsigned char)arguments[LEFT_OPERAND].text[index]);
}

static Value offer_value_str(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value value = arguments[SOLE_OPERAND];
    if (value.kind == KIND_TEXT) return value;
    if (value.kind == KIND_INTEGER) return integer_text(value.integer);
    if (value.kind == KIND_NOTHING) return text_value(KIND_NAME_NOTHING, strlen(KIND_NAME_NOTHING));
    const char *name = kind_name(value.kind);
    return text_value(name, strlen(name));
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
    return list_from(fields, SLEEP_REST_FIELDS);
}

static Value offer_host_sleep_ms(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value request = arguments[SOLE_OPERAND];
    if (terminal_input_request(request)) return offer_terminal_input(request);
    long long milliseconds = sleep_milliseconds(request);
    if (milliseconds < WORD_FALSE) fail("host_sleep_ms expects a nonnegative duration");
    struct timespec remaining;
    remaining.tv_sec = (time_t)(milliseconds / MILLISECONDS_PER_SECOND);
    remaining.tv_nsec = (long)((milliseconds % MILLISECONDS_PER_SECOND) * NANOSECONDS_PER_MILLISECOND);
    while (nanosleep(&remaining, &remaining) != SYSTEM_CALL_OK) {
        if (errno != EINTR) fail("host_sleep_ms could not wait");
    }
    if (request.kind == KIND_LIST) return deadline_rest();
    return integer_value(milliseconds);
}

static Value offer_host_monotonic_ms(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != SYSTEM_CALL_OK) fail("host_monotonic_ms could not read the clock");
    long long milliseconds = (long long)now.tv_sec * MILLISECONDS_PER_SECOND + (long long)now.tv_nsec / NANOSECONDS_PER_MILLISECOND;
    return integer_value(milliseconds);
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
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "host_getenv expects text");
    const char *value = getenv(arguments[SOLE_OPERAND].text);
    if (value == NULL) return nothing_value();
    return text_value(value, strlen(value));
}

static pid_t integer_pid(Value value, const char *reason) {
    expect_kind(value, KIND_INTEGER, reason);
    if (value.integer <= WORD_FALSE) return 0;
    return (pid_t)value.integer;
}

static Value offer_host_alive(Value *arguments, size_t argument_count) {
    (void)argument_count;
    pid_t pid = integer_pid(arguments[SOLE_OPERAND], "host_alive expects a process");
    if (pid == 0) return integer_value(WORD_FALSE);
    return boolean_word(kill(pid, 0) == SYSTEM_CALL_OK);
}

static Value offer_host_kill(Value *arguments, size_t argument_count) {
    (void)argument_count;
    pid_t pid = integer_pid(arguments[SOLE_OPERAND], "host_kill expects a process");
    if (pid == 0) return integer_value(WORD_FALSE);
    if (kill(pid, SIGTERM) == SYSTEM_CALL_OK || errno == ESRCH) return integer_value(WORD_FALSE);
    return integer_value(-WORD_TRUE);
}

static Value offer_host_wait(Value *arguments, size_t argument_count) {
    (void)argument_count;
    pid_t pid = integer_pid(arguments[SOLE_OPERAND], "host_wait expects a process");
    if (pid == 0) return integer_value(WORD_FALSE);
    int status = 0;
    if (waitpid(pid, &status, 0) < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    if (WIFEXITED(status)) return integer_value(WEXITSTATUS(status));
    return integer_value(WORD_FALSE);
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
} GiftMapping;

static GiftMapping *gift_mappings;
static size_t gift_mapping_count;
static size_t gift_mapping_capacity;
static long long process_born_ms;
static uint64_t live_sequence;

typedef struct GiftGeneration {
    uint64_t id;
    Value root;
    int held;
} GiftGeneration;

static GiftGeneration *gift_generations;
static size_t gift_generation_count;
static size_t gift_generation_capacity;
static uint64_t gift_generation_next = 1;

typedef struct RosterMapping {
    void *address;
    size_t bytes;
    int descriptor;
    int open;
} RosterMapping;

static RosterMapping gift_roster;

static long long monotonic_milliseconds(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != SYSTEM_CALL_OK) fail("the clock could not be read");
    return (long long)now.tv_sec * MILLISECONDS_PER_SECOND + (long long)now.tv_nsec / NANOSECONDS_PER_MILLISECOND;
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

static size_t gift_hold(char *name, void *address, size_t bytes, int descriptor) {
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
    gift_mappings[gift_mapping_count].name = name;
    gift_mappings[gift_mapping_count].address = address;
    gift_mappings[gift_mapping_count].bytes = bytes;
    gift_mappings[gift_mapping_count].descriptor = descriptor;
    gift_mappings[gift_mapping_count].held = 1;
    size_t handle = gift_mapping_count;
    gift_mapping_count = checked_add(gift_mapping_count, ONE_ELEMENT);
    return handle;
}

static GiftMapping *gift_mapping(Value handle) {
    if (handle.kind != KIND_INTEGER || handle.integer < WORD_FALSE) return NULL;
    if ((unsigned long long)handle.integer >= gift_mapping_count) return NULL;
    GiftMapping *mapping = &gift_mappings[(size_t)handle.integer];
    if (!mapping->held) return NULL;
    return mapping;
}

static Value gift_map(char *name, int create, size_t extent) {
    int descriptor = shm_open(name, create ? O_RDWR | O_CREAT : O_RDWR, GIFT_OPEN_MODE);
    if (descriptor < SYSTEM_CALL_OK) {
        free(name);
        return nothing_value();
    }
    struct stat info;
    if (fstat(descriptor, &info) != SYSTEM_CALL_OK) {
        close(descriptor);
        free(name);
        return nothing_value();
    }
    size_t bytes = (size_t)info.st_size;
    size_t needed = checked_add(GIFT_HEADER_BYTES, extent);
    if (create && bytes < needed) {
        if (bytes != EMPTY_COUNT) {
            close(descriptor);
            if (shm_unlink(name) != SYSTEM_CALL_OK) {
                free(name);
                return nothing_value();
            }
            descriptor = shm_open(name, O_RDWR | O_CREAT | O_EXCL, GIFT_OPEN_MODE);
            if (descriptor < SYSTEM_CALL_OK) {
                free(name);
                return nothing_value();
            }
        }
        if (ftruncate(descriptor, (off_t)needed) != SYSTEM_CALL_OK) {
            close(descriptor);
            free(name);
            return nothing_value();
        }
        bytes = needed;
    }
    if (bytes < GIFT_HEADER_BYTES) {
        close(descriptor);
        free(name);
        return nothing_value();
    }
    void *address = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
    if (address == MAP_FAILED) {
        close(descriptor);
        free(name);
        return nothing_value();
    }
    return integer_value((long long)gift_hold(name, address, bytes, descriptor));
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
    long long extent = gift_extent(arguments[RIGHT_OPERAND]);
    if (extent < WORD_FALSE) return nothing_value();
    if ((unsigned long long)extent > SIZE_MAX - GIFT_HEADER_BYTES) fail("shm_offer extent exceeds addressable memory");
    char *name = copy_bytes(arguments[LEFT_OPERAND].text, arguments[LEFT_OPERAND].text_length);
    return gift_map(name, 1, (size_t)extent);
}

static Value offer_shm_receive(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "shm_receive expects a name");
    char *name = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    return gift_map(name, 0, EMPTY_COUNT);
}

static Value offer_shm_read(Value *arguments, size_t argument_count) {
    (void)argument_count;
    GiftMapping *mapping = gift_mapping(arguments[SOLE_OPERAND]);
    if (mapping == NULL || mapping->bytes < GIFT_HEADER_BYTES) return nothing_value();
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

static Value gift_publish(GiftMapping *mapping, const void *payload, size_t length) {
    if (mapping == NULL || mapping->bytes < GIFT_HEADER_BYTES || length > mapping->bytes - GIFT_HEADER_BYTES) return nothing_value();
    void *sequence = (char *)mapping->address + GIFT_SEQUENCE_OFFSET;
    long long started = monotonic_milliseconds();
    uint64_t even = 0;
    for (;;) {
        uint64_t seen = shared_load(sequence);
        if ((seen & 1ULL) == 0) {
            even = seen;
            if (__atomic_compare_exchange_n((uint64_t *)sequence, &seen, even + 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) break;
        } else if (gift_budget_spent(started)) {
            shared_store(sequence, seen + 1);
            started = monotonic_milliseconds();
        }
    }
    shared_store((char *)mapping->address + GIFT_LENGTH_OFFSET, (uint64_t)length);
    if (length != EMPTY_COUNT) memcpy((char *)mapping->address + GIFT_HEADER_BYTES, payload, length);
    shared_store(sequence, even + 2);
    return integer_value((long long)(even + 2));
}

static Value offer_shm_write(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[RIGHT_OPERAND], KIND_TEXT, "shm_write expects text");
    return gift_publish(gift_mapping(arguments[LEFT_OPERAND]), arguments[RIGHT_OPERAND].text, arguments[RIGHT_OPERAND].text_length);
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
    GiftMapping *mapping = gift_mapping(arguments[SOLE_OPERAND]);
    if (mapping == NULL) return integer_value(WORD_FALSE);
    munmap(mapping->address, mapping->bytes);
    close(mapping->descriptor);
    free(mapping->name);
    mapping->address = NULL;
    mapping->name = NULL;
    mapping->descriptor = -1;
    mapping->held = 0;
    return integer_value(WORD_TRUE);
}

static Value offer_node_gift_read(Value *arguments, size_t argument_count) {
    (void)argument_count;
    GiftMapping *mapping = gift_mapping(arguments[SOLE_OPERAND]);
    if (mapping == NULL || mapping->bytes < GIFT_HEADER_BYTES) return nothing_value();
    long long started = monotonic_milliseconds();
    for (;;) {
        uint64_t first = shared_load((char *)mapping->address + GIFT_SEQUENCE_OFFSET);
        if ((first & 1ULL) == 0) {
            uint64_t length = shared_load((char *)mapping->address + GIFT_LENGTH_OFFSET);
            uint64_t mark = 0;
            int marked = length == NODE_REFERENCE_BYTES && mapping->bytes >= GIFT_HEADER_BYTES + NODE_REFERENCE_BYTES;
            if (marked) memcpy(&mark, (char *)mapping->address + GIFT_HEADER_BYTES, sizeof mark);
            if (shared_load((char *)mapping->address + GIFT_SEQUENCE_OFFSET) == first) {
                if (first == 0 || !marked || mark != NODE_GIFT_MAGIC) return nothing_value();
                uint64_t generation = 0;
                memcpy(&generation, (char *)mapping->address + GIFT_HEADER_BYTES + GIFT_REF_GENERATION * sizeof(uint64_t), sizeof generation);
                for (size_t index = FIRST_INDEX; index < gift_generation_count; index = checked_add(index, ONE_ELEMENT)) {
                    if (gift_generations[index].held && gift_generations[index].id == generation) return gift_generations[index].root;
                }
                fail("a node gift stands and its root is not adopted yet");
            }
        }
        if (gift_budget_spent(started)) return nothing_value();
    }
}

static Value gift_root(Value payload, uint64_t generation) {
    if (process_born_ms == WORD_FALSE) process_born_ms = unix_milliseconds();
    Value *children = allocate_zeroed(GIFT_ROOT_CHILDREN, sizeof *children);
    children[GIFT_CHILD_PAYLOAD] = payload;
    children[GIFT_CHILD_PID] = integer_value((long long)getpid());
    children[GIFT_CHILD_START] = integer_value(process_born_ms);
    children[GIFT_CHILD_GENERATION] = integer_value((long long)generation);
    children[GIFT_CHILD_LIFETIME] = integer_value(GIFT_LIFETIME_PROCESS);
    Value category = node_value((long long)fk_nid_make(GIFT_ROOT_PACKAGE, GIFT_ROOT_LEVEL, GIFT_ROOT_TYPE, GIFT_ROOT_INSTANCE));
    return node_from_cell(composite_cell(category, list_from(children, GIFT_ROOT_CHILDREN)));
}

static Value offer_node_gift_write(Value *arguments, size_t argument_count) {
    (void)argument_count;
    GiftMapping *mapping = gift_mapping(arguments[LEFT_OPERAND]);
    if (mapping == NULL || arguments[RIGHT_OPERAND].kind == KIND_FUNCTION) return nothing_value();
    uint64_t generation = gift_generation_next;
    gift_generation_next = checked_add((size_t)gift_generation_next, ONE_ELEMENT);
    size_t slot = gift_generation_count;
    size_t needed = checked_add(slot, ONE_ELEMENT);
    gift_generations = grow_list(gift_generations, &gift_generation_capacity, sizeof *gift_generations, needed);
    gift_generations[slot].id = generation;
    gift_generations[slot].root = gift_root(arguments[RIGHT_OPERAND], generation);
    gift_generations[slot].held = WORD_TRUE;
    gift_generation_count = needed;
    uint64_t reference[GIFT_REF_WORDS] = {EMPTY_COUNT};
    reference[GIFT_REF_MAGIC] = NODE_GIFT_MAGIC;
    reference[GIFT_REF_GENERATION] = generation;
    reference[GIFT_REF_PID] = (uint64_t)getpid();
    reference[GIFT_REF_START] = (uint64_t)process_born_ms;
    reference[GIFT_REF_LIFETIME] = GIFT_LIFETIME_PROCESS;
    return gift_publish(mapping, reference, NODE_REFERENCE_BYTES);
}

static size_t roster_stride(size_t name_length) {
    size_t raw = sizeof(int64_t) + sizeof(uint32_t) + name_length;
    size_t word = sizeof(uint64_t);
    return (raw + (word - ONE_ELEMENT)) & ~(word - ONE_ELEMENT);
}

static char *roster_entries(void) {
    return (char *)gift_roster.address + ROSTER_HEADER_WORDS * sizeof(uint64_t);
}

static void roster_map_bytes(size_t bytes) {
    void *address = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, gift_roster.descriptor, 0);
    if (address == MAP_FAILED) fail("the gift roster could not be mapped");
    if (gift_roster.address != NULL) munmap(gift_roster.address, gift_roster.bytes);
    gift_roster.address = address;
    gift_roster.bytes = bytes;
}

static void roster_open(int create) {
    if (gift_roster.open) {
        struct stat info;
        if (fstat(gift_roster.descriptor, &info) != SYSTEM_CALL_OK) fail("the gift roster could not be read");
        if ((size_t)info.st_size > gift_roster.bytes) roster_map_bytes((size_t)info.st_size);
        return;
    }
    int descriptor = shm_open(ROSTER_OBJECT, create ? O_RDWR | O_CREAT : O_RDWR, GIFT_OPEN_MODE);
    if (descriptor < SYSTEM_CALL_OK) return;
    struct stat info;
    if (fstat(descriptor, &info) != SYSTEM_CALL_OK) {
        close(descriptor);
        fail("the gift roster could not be read");
    }
    size_t bytes = (size_t)info.st_size;
    if (create && bytes < host_page_bytes()) {
        bytes = host_page_bytes();
        if (ftruncate(descriptor, (off_t)bytes) != SYSTEM_CALL_OK) {
            close(descriptor);
            fail("the gift roster could not be created");
        }
    }
    if (bytes < ROSTER_HEADER_WORDS * sizeof(uint64_t)) {
        close(descriptor);
        return;
    }
    gift_roster.descriptor = descriptor;
    gift_roster.open = 1;
    roster_map_bytes(bytes);
}

static void roster_grow(size_t needed) {
    size_t bytes = gift_roster.bytes == EMPTY_COUNT ? host_page_bytes() : gift_roster.bytes;
    while (bytes < needed) {
        if (bytes > SIZE_MAX / GROWTH_FACTOR) fail("the gift roster exceeds addressable memory");
        bytes *= GROWTH_FACTOR;
    }
    if (ftruncate(gift_roster.descriptor, (off_t)bytes) != SYSTEM_CALL_OK) fail("the gift roster could not grow");
    roster_map_bytes(bytes);
}

static int roster_snapshot(char **entries, uint64_t *count, uint64_t *used) {
    long long started = monotonic_milliseconds();
    size_t header = ROSTER_HEADER_WORDS * sizeof(uint64_t);
    for (;;) {
        uint64_t *words = gift_roster.address;
        uint64_t first = shared_load(&words[ROSTER_SEQUENCE]);
        if ((first & 1ULL) == 0) {
            uint64_t magic = words[ROSTER_MAGIC_WORD];
            uint64_t seen_count = words[ROSTER_COUNT];
            uint64_t seen_used = words[ROSTER_USED];
            if (magic == 0 && seen_count == 0 && seen_used == 0 && shared_load(&words[ROSTER_SEQUENCE]) == first) {
                *entries = NULL;
                *count = 0;
                *used = 0;
                return 1;
            }
            if (magic != ROSTER_MAGIC) fail("the gift roster has an unread layout");
            if (header + seen_used <= gift_roster.bytes) {
                char *copy = seen_used == 0 ? NULL : copy_bytes(roster_entries(), (size_t)seen_used);
                if (shared_load(&words[ROSTER_SEQUENCE]) == first) {
                    *entries = copy;
                    *count = seen_count;
                    *used = seen_used;
                    return 1;
                }
                free(copy);
            }
        }
        if (gift_budget_spent(started)) return 0;
    }
}

static int roster_find(const char *entries, uint64_t count, uint64_t used, const char *name, size_t name_length, uint64_t *index, size_t *offset) {
    size_t cursor = EMPTY_COUNT;
    for (uint64_t slot = 0; slot < count; slot = slot + 1) {
        if (cursor + sizeof(int64_t) + sizeof(uint32_t) > used) fail("the gift roster is not readable");
        uint32_t stored = 0;
        memcpy(&stored, entries + cursor + sizeof(int64_t), sizeof stored);
        size_t stride = roster_stride(stored);
        if (cursor + stride > used) fail("the gift roster is not readable");
        if (bytes_equal(entries + cursor + sizeof(int64_t) + sizeof(uint32_t), stored, name, name_length)) {
            *index = slot;
            *offset = cursor;
            return 1;
        }
        cursor += stride;
    }
    return 0;
}

static uint64_t roster_claim(void) {
    long long started = monotonic_milliseconds();
    void *sequence = (uint64_t *)gift_roster.address + ROSTER_SEQUENCE;
    for (;;) {
        uint64_t seen = shared_load(sequence);
        if ((seen & 1ULL) == 0) {
            uint64_t even = seen;
            if (__atomic_compare_exchange_n((uint64_t *)sequence, &seen, even + 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) return even;
        } else if (gift_budget_spent(started)) {
            shared_store(sequence, (seen + 1) & ~1ULL);
            started = monotonic_milliseconds();
        }
    }
}

static Value offer_gift_roster_names(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    roster_open(0);
    if (!gift_roster.open) return list_from(NULL, EMPTY_COUNT);
    char *entries = NULL;
    uint64_t count = 0;
    uint64_t used = 0;
    if (!roster_snapshot(&entries, &count, &used)) fail("the gift roster did not settle");
    if (count == 0) return list_from(NULL, EMPTY_COUNT);
    Value *rows = allocate_zeroed((size_t)count, sizeof *rows);
    size_t cursor = EMPTY_COUNT;
    for (uint64_t slot = 0; slot < count; slot = slot + 1) {
        if (cursor + sizeof(int64_t) + sizeof(uint32_t) > used) fail("the gift roster is not readable");
        int64_t spoke = 0;
        uint32_t stored = 0;
        memcpy(&spoke, entries + cursor, sizeof spoke);
        memcpy(&stored, entries + cursor + sizeof(int64_t), sizeof stored);
        size_t stride = roster_stride(stored);
        if (cursor + stride > used) fail("the gift roster is not readable");
        Value *pair = allocate_zeroed(ARITY_TWO, sizeof *pair);
        pair[FIRST_INDEX] = text_value(entries + cursor + sizeof(int64_t) + sizeof(uint32_t), stored);
        pair[RIGHT_OPERAND] = integer_value((long long)spoke);
        rows[slot] = list_from(pair, ARITY_TWO);
        cursor += stride;
    }
    free(entries);
    return list_from(rows, (size_t)count);
}

static Value offer_gift_roster_register(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "gift_roster_register expects a name");
    roster_open(1);
    if (!gift_roster.open) return nothing_value();
    const char *name = arguments[SOLE_OPERAND].text;
    size_t name_length = arguments[SOLE_OPERAND].text_length;
    for (;;) {
        char *entries = NULL;
        uint64_t count = 0;
        uint64_t used = 0;
        if (!roster_snapshot(&entries, &count, &used)) fail("the gift roster did not settle");
        uint64_t found = 0;
        size_t found_at = EMPTY_COUNT;
        int present = roster_find(entries, count, used, name, name_length, &found, &found_at);
        size_t header = ROSTER_HEADER_WORDS * sizeof(uint64_t);
        size_t stride = roster_stride(name_length);
        if (!present && header + (size_t)used + stride > gift_roster.bytes) {
            free(entries);
            roster_grow(header + (size_t)used + stride);
            continue;
        }
        uint64_t even = roster_claim();
        uint64_t *words = gift_roster.address;
        if (words[ROSTER_MAGIC_WORD] == 0) {
            words[ROSTER_MAGIC_WORD] = ROSTER_MAGIC;
            words[ROSTER_COUNT] = 0;
            words[ROSTER_USED] = 0;
            count = 0;
            used = 0;
            present = 0;
        }
        if (words[ROSTER_MAGIC_WORD] != ROSTER_MAGIC) {
            shared_store((uint64_t *)gift_roster.address + ROSTER_SEQUENCE, even + 2);
            free(entries);
            fail("the gift roster has an unread layout");
        }
        if (present && (words[ROSTER_COUNT] != count || words[ROSTER_USED] != used)) present = 0;
        if (!present) {
            present = roster_find(roster_entries(), words[ROSTER_COUNT], words[ROSTER_USED], name, name_length, &found, &found_at);
        }
        int64_t spoke = unix_milliseconds();
        if (present) {
            memcpy(roster_entries() + found_at, &spoke, sizeof spoke);
        } else {
            size_t cursor = (size_t)words[ROSTER_USED];
            if (header + cursor + stride > gift_roster.bytes) {
                shared_store((uint64_t *)gift_roster.address + ROSTER_SEQUENCE, even + 2);
                free(entries);
                roster_grow(header + cursor + stride);
                continue;
            }
            char *slot = roster_entries() + cursor;
            memcpy(slot, &spoke, sizeof spoke);
            uint32_t stored = (uint32_t)name_length;
            memcpy(slot + sizeof spoke, &stored, sizeof stored);
            if (name_length != EMPTY_COUNT) memcpy(slot + sizeof spoke + sizeof stored, name, name_length);
            found = words[ROSTER_COUNT];
            words[ROSTER_COUNT] = found + 1;
            words[ROSTER_USED] = cursor + stride;
        }
        shared_store((uint64_t *)gift_roster.address + ROSTER_SEQUENCE, even + 2);
        free(entries);
        return integer_value((long long)found);
    }
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
    if (close(descriptor) != SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    return integer_value((long long)written);
}

static Value offer_write_file(Value *arguments, size_t argument_count) {
    return offer_host_file_write_text(arguments, argument_count);
}

static Value offer_write_file_text(Value *arguments, size_t argument_count) {
    return offer_host_file_write_text(arguments, argument_count);
}

static Value offer_write_file_bytes(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "write_file_bytes expects a path");
    unsigned char *bytes = NULL;
    size_t length = EMPTY_COUNT;
    if (!list_to_bytes(arguments[RIGHT_OPERAND], &bytes, &length)) fail("write_file_bytes expects a byte list");
    char *path = copy_bytes(arguments[LEFT_OPERAND].text, arguments[LEFT_OPERAND].text_length);
    int descriptor = open(path, O_WRONLY | O_CREAT | O_TRUNC, TEXT_FILE_MODE);
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
    if (close(descriptor) != SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    return integer_value((long long)written);
}

static Value offer_read_file_bytes(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "read_file_bytes expects a path");
    char *path = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    int descriptor = open(path, O_RDONLY);
    free(path);
    if (descriptor < SYSTEM_CALL_OK) return nothing_value();
    size_t capacity = host_page_bytes();
    unsigned char *bytes = allocate_zeroed(capacity, sizeof *bytes);
    size_t filled = EMPTY_COUNT;
    for (;;) {
        if (filled == capacity) {
            if (capacity > SIZE_MAX / GROWTH_FACTOR) fail("read_file_bytes exceeds addressable memory");
            size_t grown = capacity * GROWTH_FACTOR;
            unsigned char *next = allocate_zeroed(grown, sizeof *next);
            memcpy(next, bytes, filled);
            free(bytes);
            bytes = next;
            capacity = grown;
        }
        ssize_t step = read(descriptor, bytes + filled, capacity - filled);
        if (step < SYSTEM_CALL_OK) {
            if (errno == EINTR) continue;
            close(descriptor);
            free(bytes);
            return nothing_value();
        }
        if (step == SYSTEM_CALL_OK) break;
        filled = checked_add(filled, (size_t)step);
    }
    close(descriptor);
    Value list = bytes_to_list(bytes, filled);
    free(bytes);
    return list;
}

static Value offer_host_file_read_text(Value *arguments, size_t argument_count);

static Value offer_read_file(Value *arguments, size_t argument_count) {
    return offer_host_file_read_text(arguments, argument_count);
}

typedef struct {
    unsigned char *bytes;
    size_t length;
    size_t capacity;
} ByteBuffer;

static void buffer_push(ByteBuffer *buffer, const void *chunk, size_t length) {
    if (length == EMPTY_COUNT) return;
    size_t needed = checked_add(buffer->length, length);
    if (needed > buffer->capacity) {
        size_t grown = grown_capacity(buffer->capacity, needed);
        unsigned char *next = allocate_zeroed(grown, sizeof *next);
        if (buffer->length != EMPTY_COUNT) memcpy(next, buffer->bytes, buffer->length);
        free(buffer->bytes);
        buffer->bytes = next;
        buffer->capacity = grown;
    }
    memcpy(buffer->bytes + buffer->length, chunk, length);
    buffer->length = needed;
}

static void buffer_push_u8(ByteBuffer *buffer, unsigned char value) {
    buffer_push(buffer, &value, sizeof value);
}

static void buffer_push_u32(ByteBuffer *buffer, uint32_t value) {
    unsigned char bytes[4];
    bytes[0] = (unsigned char)((value >> 24) & 0xffu);
    bytes[1] = (unsigned char)((value >> 16) & 0xffu);
    bytes[2] = (unsigned char)((value >> 8) & 0xffu);
    bytes[3] = (unsigned char)(value & 0xffu);
    buffer_push(buffer, bytes, sizeof bytes);
}

static void buffer_push_i64(ByteBuffer *buffer, int64_t value) {
    unsigned char bytes[8];
    for (size_t index = FIRST_INDEX; index < 8; index = checked_add(index, ONE_ELEMENT)) {
        bytes[index] = (unsigned char)(((uint64_t)value >> (8 * index)) & 0xffu);
    }
    buffer_push(buffer, bytes, sizeof bytes);
}

static Cell *find_cell_by_category(fk_node_id category) {
    for (size_t index = FIRST_INDEX; index < cell_count; index = checked_add(index, ONE_ELEMENT)) {
        if (cells[index] != NULL && cells[index]->category == category) return cells[index];
    }
    return NULL;
}

static int serialize_value(ByteBuffer *buffer, Value value) {
    if (value.kind == KIND_NOTHING) {
        buffer_push_u8(buffer, 0);
        return 1;
    }
    if (value.kind == KIND_INTEGER) {
        buffer_push_u8(buffer, 1);
        buffer_push_i64(buffer, (int64_t)value.integer);
        return 1;
    }
    if (value.kind == KIND_REAL) {
        buffer_push_u8(buffer, 2);
        uint64_t bits = 0;
        memcpy(&bits, &value.real, sizeof bits);
        buffer_push_i64(buffer, (int64_t)bits);
        return 1;
    }
    if (value.kind == KIND_TEXT) {
        buffer_push_u8(buffer, 3);
        if (value.text_length > UINT32_MAX) return 0;
        buffer_push_u32(buffer, (uint32_t)value.text_length);
        buffer_push(buffer, value.text, value.text_length);
        return 1;
    }
    if (value.kind == KIND_LIST) {
        buffer_push_u8(buffer, 4);
        if (value.item_count > UINT32_MAX) return 0;
        buffer_push_u32(buffer, (uint32_t)value.item_count);
        List *cursor = value.list;
        for (size_t index = FIRST_INDEX; index < value.item_count; index = checked_add(index, ONE_ELEMENT)) {
            if (cursor == NULL || !serialize_value(buffer, cursor->head)) return 0;
            cursor = cursor->tail;
        }
        return 1;
    }
    if (value.kind == KIND_NODE) {
        buffer_push_u8(buffer, 5);
        buffer_push_i64(buffer, (int64_t)node_word(value, "recipe_to_bytes expects nodes"));
        return 1;
    }
    return 0;
}

static int read_u32(const unsigned char *bytes, size_t length, size_t *cursor, uint32_t *out) {
    if (*cursor > length || length - *cursor < 4) return 0;
    uint32_t value = ((uint32_t)bytes[*cursor] << 24)
        | ((uint32_t)bytes[checked_add(*cursor, ONE_ELEMENT)] << 16)
        | ((uint32_t)bytes[checked_add(*cursor, ARITY_TWO)] << 8)
        | (uint32_t)bytes[checked_add(*cursor, ARITY_THREE)];
    *cursor = checked_add(*cursor, 4);
    *out = value;
    return 1;
}

static int read_i64(const unsigned char *bytes, size_t length, size_t *cursor, int64_t *out) {
    if (*cursor > length || length - *cursor < 8) return 0;
    uint64_t value = 0;
    for (size_t index = FIRST_INDEX; index < 8; index = checked_add(index, ONE_ELEMENT)) {
        value |= ((uint64_t)bytes[checked_add(*cursor, index)]) << (8 * index);
    }
    *cursor = checked_add(*cursor, 8);
    *out = (int64_t)value;
    return 1;
}

static int deserialize_value(const unsigned char *bytes, size_t length, size_t *cursor, Value *out) {
    if (*cursor >= length) return 0;
    unsigned char tag = bytes[*cursor];
    *cursor = checked_add(*cursor, ONE_ELEMENT);
    if (tag == 0) {
        *out = nothing_value();
        return 1;
    }
    if (tag == 1) {
        int64_t number = 0;
        if (!read_i64(bytes, length, cursor, &number)) return 0;
        *out = integer_value((long long)number);
        return 1;
    }
    if (tag == 2) {
        int64_t payload = 0;
        if (!read_i64(bytes, length, cursor, &payload)) return 0;
        uint64_t bits = (uint64_t)payload;
        double real = REAL_ZERO;
        memcpy(&real, &bits, sizeof real);
        *out = real_value(real);
        return 1;
    }
    if (tag == 3) {
        uint32_t text_length = 0;
        if (!read_u32(bytes, length, cursor, &text_length)) return 0;
        if (*cursor > length || length - *cursor < text_length) return 0;
        *out = text_value((const char *)(bytes + *cursor), text_length);
        *cursor = checked_add(*cursor, text_length);
        return 1;
    }
    if (tag == 4) {
        uint32_t count = 0;
        if (!read_u32(bytes, length, cursor, &count)) return 0;
        Value *items = allocate_zeroed(count, sizeof *items);
        for (uint32_t index = 0; index < count; index++) {
            if (!deserialize_value(bytes, length, cursor, items + index)) return 0;
        }
        *out = list_from(items, count);
        return 1;
    }
    if (tag == 5) {
        int64_t identity = 0;
        if (!read_i64(bytes, length, cursor, &identity)) return 0;
        Cell *cell = find_cell_by_category((fk_node_id)identity);
        *out = cell == NULL ? node_value((long long)identity) : node_from_cell(cell);
        return 1;
    }
    return 0;
}

static Value offer_recipe_to_bytes(Value *arguments, size_t argument_count) {
    (void)argument_count;
    ByteBuffer buffer = {0};
    const unsigned char header[4] = {'F', 'K', 'B', '2'};
    buffer_push(&buffer, header, sizeof header);
    if (!serialize_value(&buffer, arguments[SOLE_OPERAND])) {
        free(buffer.bytes);
        return nothing_value();
    }
    Value bytes = bytes_to_list(buffer.bytes, buffer.length);
    free(buffer.bytes);
    return bytes;
}

static Value offer_bytes_to_recipe(Value *arguments, size_t argument_count) {
    (void)argument_count;
    unsigned char *bytes = NULL;
    size_t length = EMPTY_COUNT;
    if (!list_to_bytes(arguments[SOLE_OPERAND], &bytes, &length)) return nothing_value();
    if (length < 4 || memcmp(bytes, "FKB2", 4) != STRING_COMPARE_EQUAL) {
        free(bytes);
        return nothing_value();
    }
    size_t cursor = 4;
    Value value = nothing_value();
    int ok = deserialize_value(bytes, length, &cursor, &value);
    free(bytes);
    if (!ok || cursor != length) return nothing_value();
    return value;
}

static Value offer_write_form_binary(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "write_form_binary expects a path");
    Value recipe_arguments[ARITY_ONE];
    recipe_arguments[FIRST_INDEX] = arguments[RIGHT_OPERAND];
    Value bytes = offer_recipe_to_bytes(recipe_arguments, ARITY_ONE);
    if (bytes.kind == KIND_NOTHING) return integer_value(-WORD_TRUE);
    Value write_arguments[ARITY_TWO];
    write_arguments[LEFT_OPERAND] = arguments[LEFT_OPERAND];
    write_arguments[RIGHT_OPERAND] = bytes;
    return offer_write_file_bytes(write_arguments, ARITY_TWO);
}

static Value offer_read_form_binary(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value read_arguments[ARITY_ONE];
    read_arguments[FIRST_INDEX] = arguments[SOLE_OPERAND];
    Value bytes = offer_read_file_bytes(read_arguments, ARITY_ONE);
    if (bytes.kind == KIND_NOTHING) return nothing_value();
    Value decode_arguments[ARITY_ONE];
    decode_arguments[FIRST_INDEX] = bytes;
    return offer_bytes_to_recipe(decode_arguments, ARITY_ONE);
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

static Value offer_host_file_read_slice(Value *arguments, size_t argument_count) {
    return offer_read_file_slice(arguments, argument_count);
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

static Value offer_file_read(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_INTEGER, "file_read expects a descriptor");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "file_read expects a length");
    long long descriptor = arguments[LEFT_OPERAND].integer;
    long long requested = arguments[RIGHT_OPERAND].integer;
    if (descriptor < WORD_FALSE || requested <= WORD_FALSE) return text_value("", EMPTY_COUNT);
    size_t length = (size_t)requested;
    char *buffer = allocate_zeroed(checked_add(length, ONE_ELEMENT), sizeof *buffer);
    ssize_t got = SYSTEM_CALL_OK;
    while (1) {
        got = read((int)descriptor, buffer, length);
        if (got < SYSTEM_CALL_OK && errno == EINTR) continue;
        break;
    }
    if (got < SYSTEM_CALL_OK) {
        free(buffer);
        return nothing_value();
    }
    Value text = text_value(buffer, (size_t)got);
    free(buffer);
    return text;
}

static Value offer_file_size(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "file_size expects a path");
    char *path = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    struct stat info;
    int found = stat(path, &info);
    free(path);
    if (found != SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    return integer_value((long long)info.st_size);
}

static Value offer_host_file_size(Value *arguments, size_t argument_count) {
    return offer_file_size(arguments, argument_count);
}

static Value offer_host_file_mtime(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "host_file_mtime expects text");
    struct stat info;
    if (stat(arguments[SOLE_OPERAND].text, &info) != SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    return integer_value((long long)info.st_mtime);
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
    int started = mkdir(path, DIRECTORY_MODE);
    int created = 0;
    if (started == SYSTEM_CALL_OK) created = 1;
    else if (errno == EEXIST) {
        struct stat info;
        if (stat(path, &info) == 0 && S_ISDIR(info.st_mode)) created = 0;
    }
    free(path);
    return integer_value((long long)created);
}

static int remove_tree(const char *path) {
    DIR *directory = opendir(path);
    if (directory == NULL) return -WORD_TRUE;
    struct dirent *entry = NULL;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        size_t length = strlen(path);
        size_t name_length = strlen(entry->d_name);
        char *child = allocate_zeroed(checked_add(checked_add(length, name_length), ARITY_TWO), sizeof *child);
        memcpy(child, path, length);
        child[length] = PATH_SEPARATOR;
        memcpy(child + length + ONE_ELEMENT, entry->d_name, name_length);
        struct stat info;
        int found = stat(child, &info);
        int removed = -WORD_TRUE;
        if (found == SYSTEM_CALL_OK && S_ISDIR(info.st_mode)) removed = remove_tree(child);
        else removed = unlink(child) == SYSTEM_CALL_OK ? WORD_FALSE : -WORD_TRUE;
        free(child);
        if (removed != WORD_FALSE) {
            closedir(directory);
            return -WORD_TRUE;
        }
    }
    closedir(directory);
    return rmdir(path) == SYSTEM_CALL_OK ? WORD_FALSE : -WORD_TRUE;
}

static Value offer_host_dir_rmdir(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "host_dir_rmdir expects a path");
    char *path = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    int removed = remove_tree(path);
    free(path);
    return integer_value(removed);
}

static Value offer_host_path_exists(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "host_path_exists expects a path");
    return boolean_word(path_exists(arguments[SOLE_OPERAND].text));
}

static Value offer_host_path_is_dir(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "host_path_is_dir expects a path");
    return boolean_word(path_is_dir(arguments[SOLE_OPERAND].text));
}

static Value offer_host_path_remove(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "host_path_remove expects a path");
    return integer_value(unlink(arguments[SOLE_OPERAND].text) == SYSTEM_CALL_OK ? WORD_FALSE : -WORD_TRUE);
}

static Value offer_host_path_rename(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "host_path_rename expects a source path");
    expect_kind(arguments[RIGHT_OPERAND], KIND_TEXT, "host_path_rename expects a target path");
    return integer_value(rename(arguments[LEFT_OPERAND].text, arguments[RIGHT_OPERAND].text) == SYSTEM_CALL_OK ? WORD_FALSE : -WORD_TRUE);
}

static Value offer_host_dir_list(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "host_dir_list expects a path");
    DIR *directory = opendir(arguments[SOLE_OPERAND].text);
    if (directory == NULL) return nothing_value();
    Value *rows = NULL;
    size_t count = EMPTY_COUNT;
    size_t capacity = EMPTY_COUNT;
    struct dirent *entry = NULL;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        size_t needed = checked_add(count, ONE_ELEMENT);
        rows = grow_list(rows, &capacity, sizeof *rows, needed);
        rows[count] = text_value(entry->d_name, strlen(entry->d_name));
        count = needed;
    }
    closedir(directory);
    return list_from(rows, count);
}

static Value offer_fs_exists(Value *arguments, size_t argument_count) {
    return offer_host_path_exists(arguments, argument_count);
}

static Value offer_fs_is_dir(Value *arguments, size_t argument_count) {
    return offer_host_path_is_dir(arguments, argument_count);
}

static Value offer_fs_mkdir(Value *arguments, size_t argument_count) {
    return offer_host_dir_mkdir(arguments, argument_count);
}

static Value offer_fs_rmdir(Value *arguments, size_t argument_count) {
    return offer_host_dir_rmdir(arguments, argument_count);
}

static Value offer_fs_remove(Value *arguments, size_t argument_count) {
    return offer_host_path_remove(arguments, argument_count);
}

static Value offer_fs_rename(Value *arguments, size_t argument_count) {
    return offer_host_path_rename(arguments, argument_count);
}

static Value offer_fs_list(Value *arguments, size_t argument_count) {
    return offer_host_dir_list(arguments, argument_count);
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

static Value offer_host_spawn_at(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_LIST, "host_spawn_at expects a command list");
    expect_kind(arguments[RIGHT_OPERAND], KIND_LIST, "host_spawn_at expects [stdin, stdout, stderr]");
    Value command = arguments[LEFT_OPERAND];
    Value redirects = arguments[RIGHT_OPERAND];
    if (command.item_count == EMPTY_COUNT || redirects.item_count < ARITY_THREE) return integer_value(-WORD_TRUE);

    char **argv = allocate_zeroed(checked_add(command.item_count, ONE_ELEMENT), sizeof *argv);
    List *cursor = command.list;
    for (size_t index = FIRST_INDEX; index < command.item_count; index = checked_add(index, ONE_ELEMENT)) {
        expect_kind(cursor->head, KIND_TEXT, "host_spawn_at expects text arguments");
        argv[index] = cursor->head.text;
        cursor = cursor->tail;
    }

    Value stdin_path = list_at(redirects, FIRST_INDEX);
    Value stdout_path = list_at(redirects, ONE_ELEMENT);
    Value stderr_path = list_at(redirects, ARITY_TWO);
    expect_kind(stdin_path, KIND_TEXT, "host_spawn_at stdin path must be text");
    expect_kind(stdout_path, KIND_TEXT, "host_spawn_at stdout path must be text");
    expect_kind(stderr_path, KIND_TEXT, "host_spawn_at stderr path must be text");
    const char *in_path = stdin_path.text_length == EMPTY_COUNT ? NULL_DEVICE : stdin_path.text;
    const char *out_path = stdout_path.text_length == EMPTY_COUNT ? NULL_DEVICE : stdout_path.text;
    const char *err_path = stderr_path.text_length == EMPTY_COUNT ? NULL_DEVICE : stderr_path.text;

    posix_spawn_file_actions_t actions;
    if (posix_spawn_file_actions_init(&actions) != SYSTEM_CALL_OK) {
        free(argv);
        return integer_value(-WORD_TRUE);
    }
    if (posix_spawn_file_actions_addopen(&actions, STDIN_FILENO, in_path, O_RDONLY, NULL_DEVICE_MODE) != SYSTEM_CALL_OK
        || posix_spawn_file_actions_addopen(&actions, STDOUT_FILENO, out_path, O_WRONLY | O_CREAT | O_TRUNC, TEXT_FILE_MODE) != SYSTEM_CALL_OK
        || posix_spawn_file_actions_addopen(&actions, STDERR_FILENO, err_path, O_WRONLY | O_CREAT | O_TRUNC, TEXT_FILE_MODE) != SYSTEM_CALL_OK) {
        posix_spawn_file_actions_destroy(&actions);
        free(argv);
        return integer_value(-WORD_TRUE);
    }

    pid_t pid = 0;
    int started = posix_spawnp(&pid, argv[FIRST_INDEX], &actions, NULL, argv, environ);
    posix_spawn_file_actions_destroy(&actions);
    free(argv);
    return integer_value(started == SYSTEM_CALL_OK ? (long long)pid : -WORD_TRUE);
}

static char *read_descriptor(int descriptor, size_t *length);

static Value offer_host_capture(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_LIST, "host_capture expects an argv list");
    Value command = arguments[SOLE_OPERAND];
    if (command.item_count == EMPTY_COUNT) return nothing_value();
    char **argv = allocate_zeroed(checked_add(command.item_count, ONE_ELEMENT), sizeof *argv);
    List *cursor = command.list;
    for (size_t index = FIRST_INDEX; index < command.item_count; index = checked_add(index, ONE_ELEMENT)) {
        expect_kind(cursor->head, KIND_TEXT, "host_capture expects text arguments");
        argv[index] = cursor->head.text;
        cursor = cursor->tail;
    }
    int outbound[ARITY_TWO];
    if (pipe(outbound) != SYSTEM_CALL_OK) {
        free(argv);
        return nothing_value();
    }
    pid_t pid = fork();
    if (pid < SYSTEM_CALL_OK) {
        free(argv);
        close(outbound[FIRST_INDEX]);
        close(outbound[ONE_ELEMENT]);
        return nothing_value();
    }
    if (pid == SYSTEM_CALL_OK) {
        dup2(outbound[ONE_ELEMENT], STDOUT_FILENO);
        close(outbound[FIRST_INDEX]);
        close(outbound[ONE_ELEMENT]);
        execvp(argv[FIRST_INDEX], argv);
        _exit(127);
    }
    free(argv);
    close(outbound[ONE_ELEMENT]);
    size_t length = EMPTY_COUNT;
    char *output = read_descriptor(outbound[FIRST_INDEX], &length);
    close(outbound[FIRST_INDEX]);
    int status = 0;
    waitpid(pid, &status, 0);
    if (output == NULL) return nothing_value();
    Value value = text_value(output, length);
    free(output);
    return value;
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

static size_t line_count_in_text(const char *bytes, size_t length) {
    if (length == EMPTY_COUNT) return EMPTY_COUNT;
    size_t lines = ONE_ELEMENT;
    for (size_t index = FIRST_INDEX; index < length; index = checked_add(index, ONE_ELEMENT)) {
        if (bytes[index] == NEWLINE) lines = checked_add(lines, ONE_ELEMENT);
    }
    return lines;
}

static void append_inventory_row(Value **rows, size_t *count, size_t *capacity, const char *relative, size_t relative_length, long long lines) {
    Value pair[ARITY_TWO];
    pair[FIRST_INDEX] = text_value(relative, relative_length);
    pair[ONE_ELEMENT] = integer_value(lines);
    size_t needed = checked_add(*count, ONE_ELEMENT);
    *rows = grow_list(*rows, capacity, sizeof **rows, needed);
    (*rows)[*count] = list_from(pair, ARITY_TWO);
    *count = needed;
}

static void walk_inventory(const char *root, const char *relative, const char *suffix, Value **rows, size_t *count, size_t *capacity) {
    size_t root_length = strlen(root);
    size_t relative_length = strlen(relative);
    char *path = allocate_zeroed(checked_add(checked_add(root_length, relative_length), ARITY_TWO), sizeof *path);
    memcpy(path, root, root_length);
    if (relative_length != EMPTY_COUNT) {
        path[root_length] = PATH_SEPARATOR;
        memcpy(path + root_length + ONE_ELEMENT, relative, relative_length);
    }
    DIR *directory = opendir(path);
    if (directory == NULL) {
        free(path);
        return;
    }
    struct dirent *entry = NULL;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        size_t name_length = strlen(entry->d_name);
        size_t child_rel_length = relative_length == EMPTY_COUNT ? name_length : checked_add(checked_add(relative_length, ONE_ELEMENT), name_length);
        char *child_rel = allocate_zeroed(checked_add(child_rel_length, ONE_ELEMENT), sizeof *child_rel);
        if (relative_length != EMPTY_COUNT) {
            memcpy(child_rel, relative, relative_length);
            child_rel[relative_length] = PATH_SEPARATOR;
            memcpy(child_rel + relative_length + ONE_ELEMENT, entry->d_name, name_length);
        } else {
            memcpy(child_rel, entry->d_name, name_length);
        }
        char *child_path = allocate_zeroed(checked_add(checked_add(root_length, child_rel_length), ARITY_TWO), sizeof *child_path);
        memcpy(child_path, root, root_length);
        child_path[root_length] = PATH_SEPARATOR;
        memcpy(child_path + root_length + ONE_ELEMENT, child_rel, child_rel_length);
        struct stat info;
        if (stat(child_path, &info) == SYSTEM_CALL_OK) {
            if (S_ISDIR(info.st_mode)) {
                walk_inventory(root, child_rel, suffix, rows, count, capacity);
            } else if (suffix == NULL || suffix[0] == TEXT_TERMINATOR
                       || (name_length >= strlen(suffix) && strcmp(entry->d_name + name_length - strlen(suffix), suffix) == 0)) {
                int descriptor = open(child_path, O_RDONLY);
                if (descriptor >= SYSTEM_CALL_OK) {
                    size_t length = EMPTY_COUNT;
                    char *contents = read_descriptor(descriptor, &length);
                    close(descriptor);
                    if (contents != NULL) {
                        append_inventory_row(rows, count, capacity, child_rel, child_rel_length, (long long)line_count_in_text(contents, length));
                        free(contents);
                    }
                }
            }
        }
        free(child_path);
        free(child_rel);
    }
    closedir(directory);
    free(path);
}

static Value offer_source_inventory(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[FIRST_INDEX], KIND_TEXT, "source_inventory expects a root path");
    expect_kind(arguments[ONE_ELEMENT], KIND_TEXT, "source_inventory expects a suffix");
    Value *rows = NULL;
    size_t count = EMPTY_COUNT;
    size_t capacity = EMPTY_COUNT;
    walk_inventory(arguments[FIRST_INDEX].text, "", arguments[ONE_ELEMENT].text, &rows, &count, &capacity);
    return list_from(rows, count);
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
    (void)argument_count;
    print_value(arguments[SOLE_OPERAND]);
    fputc(NEWLINE, stdout);
    return arguments[SOLE_OPERAND];
}

static Value offer_str_eq(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "str_eq expects text");
    expect_kind(arguments[RIGHT_OPERAND], KIND_TEXT, "str_eq expects text");
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
    char *text = malloc(checked_add(length, TEXT_TERMINATOR_SIZE));
    if (text == NULL) fail("out of memory");
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
    return text_value(name, strlen(name));
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

static Value offer_form_error(Value *arguments, size_t argument_count) {
    (void)argument_count;
    const char *message = "form_error";
    size_t length = strlen(message);
    if (arguments[SOLE_OPERAND].kind == KIND_TEXT) {
        message = arguments[SOLE_OPERAND].text;
        length = arguments[SOLE_OPERAND].text_length;
    }
    if (unwind_to_recovery()) return nothing_value();
    fprintf(stderr, "fkwu: form_error: %.*s\n", (int)length, message);
    exit(EXIT_FAILURE);
}

static Value offer_read_line(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
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

static Value offer_input_byte(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "input_byte expects an index");
    long long index = arguments[SOLE_OPERAND].integer;
    if (index < WORD_FALSE) return integer_value(WORD_FALSE);
    if ((unsigned long long)index >= source_length) return integer_value(WORD_FALSE);
    return integer_value((unsigned char)source_bytes[(size_t)index]);
}

static Value offer_value_eq(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return boolean_word(values_equal(arguments[LEFT_OPERAND], arguments[RIGHT_OPERAND]));
}

static Value offer_file_mtime(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "file_mtime expects text");
    struct stat info;
    if (stat(arguments[SOLE_OPERAND].text, &info) != SYSTEM_CALL_OK) return nothing_value();
    return integer_value((long long)info.st_mtime);
}

static Value offer_cell_path(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    return text_value(source_path, strlen(source_path));
}

static Value offer_intern_bool(Value *arguments, size_t argument_count) {
    (void)argument_count;
    fk_node_id identity = fk_nid_make(1, 1, 3, integer_is_nonzero(arguments[SOLE_OPERAND]) ? WORD_TRUE : WORD_FALSE);
    return node_value((long long)identity);
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
        List *cell = allocate_zeroed(ONE_ELEMENT, sizeof *cell);
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
    if (arguments[RECORD_WORD].kind != KIND_NODE || arguments[RECORD_WORD].cell == NULL) {
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
    Cell *cell = arguments[RECORD_WORD].cell;
    if (cell->source == NULL) {
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
    return list_from(records, frame_record_count);
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

static void shared_remap(size_t bytes) {
    size_t mapped_bytes = cover_pages(bytes);
    if (shared_header != NULL && munmap(shared_header, shared_mapped_bytes) != SYSTEM_CALL_OK) {
        fail("a shared mapping could not be released");
    }
    if (ftruncate(shared_descriptor, (off_t)mapped_bytes) != SYSTEM_CALL_OK) fail("a shared object could not grow");
    void *mapped = mmap(NULL, mapped_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, shared_descriptor, 0);
    if (mapped == MAP_FAILED) fail("a shared object could not be mapped");
    shared_header = mapped;
    shared_mapped_bytes = mapped_bytes;
    shared_bytes = mapped_bytes;
}

static int shared_ready(void) {
    if (shared_header != NULL) return 1;
    if (shared_unavailable) return 0;
    int descriptor = open(BUS_FILE_PATH, O_CREAT | O_RDWR, SHARED_OBJECT_MODE);
    if (descriptor < SYSTEM_CALL_OK) {
        shared_unavailable = 1;
        return 0;
    }
    struct stat info;
    if (fstat(descriptor, &info) != SYSTEM_CALL_OK) {
        close(descriptor);
        shared_unavailable = 1;
        return 0;
    }
    shared_descriptor = descriptor;
    size_t header_bytes = BUS_HEADER_WORDS * sizeof(uint64_t);
    size_t size = (size_t)info.st_size;
    if (size < header_bytes) size = header_bytes;
    shared_remap(size);
    if (shared_header[BUS_HEADER_MAGIC] == 0) {
        shared_header[BUS_HEADER_MAGIC] = BUS_MAGIC;
        shared_header[BUS_HEADER_LAYOUT] = BUS_LAYOUT_VERSION;
        shared_header[BUS_HEADER_COUNT] = 0;
        shared_header[BUS_HEADER_END] = header_bytes;
    } else if (shared_header[BUS_HEADER_MAGIC] != BUS_MAGIC || shared_header[BUS_HEADER_LAYOUT] != BUS_LAYOUT_VERSION) {
        munmap(shared_header, shared_mapped_bytes);
        close(shared_descriptor);
        shared_header = NULL;
        shared_descriptor = INVALID_DESCRIPTOR;
        shared_unavailable = 1;
        return 0;
    }
    shared_maps = ONE_ELEMENT;
    return 1;
}

static size_t shared_record_span(const unsigned char *record, size_t available) {
    size_t fields = BUS_RECORD_FIELDS * sizeof(uint64_t);
    if (available < fields) fail("a shared record is truncated");
    uint64_t stage_length = 0;
    uint64_t key_length = 0;
    memcpy(&stage_length, record + (BUS_RECORD_FIELDS - 2) * sizeof(uint64_t), sizeof stage_length);
    memcpy(&key_length, record + (BUS_RECORD_FIELDS - 1) * sizeof(uint64_t), sizeof key_length);
    if (stage_length > SIZE_MAX || key_length > SIZE_MAX) fail("a shared record exceeds addressable memory");
    return align_to_word(checked_add(fields, checked_add((size_t)stage_length, (size_t)key_length)));
}

static Value offer_bus_open(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!shared_ready()) return nothing_value();
    expect_kind(arguments[BUS_STAGE], KIND_TEXT, "bus_open expects text");
    expect_kind(arguments[BUS_KEY], KIND_TEXT, "bus_open expects text");
    size_t fields = BUS_RECORD_FIELDS * sizeof(uint64_t);
    size_t span = align_to_word(checked_add(fields, checked_add(arguments[BUS_STAGE].text_length, arguments[BUS_KEY].text_length)));
    size_t header_bytes = BUS_HEADER_WORDS * sizeof(uint64_t);
    size_t start = (size_t)shared_header[BUS_HEADER_END];
    if (start < header_bytes) start = header_bytes;
    size_t end = checked_add(start, span);
    if (end > shared_mapped_bytes) shared_remap(grown_capacity(shared_mapped_bytes, end));
    unsigned char *record = (unsigned char *)shared_header + start;
    uint64_t fields_out[BUS_RECORD_FIELDS];
    fields_out[0] = BUS_PHASE_OPEN;
    fields_out[1] = (uint64_t)getpid();
    fields_out[2] = monotonic_ns();
    fields_out[3] = 0;
    fields_out[4] = arguments[BUS_STAGE].text_length;
    fields_out[5] = arguments[BUS_KEY].text_length;
    memcpy(record, fields_out, fields);
    if (arguments[BUS_STAGE].text_length != EMPTY_COUNT) memcpy(record + fields, arguments[BUS_STAGE].text, arguments[BUS_STAGE].text_length);
    if (arguments[BUS_KEY].text_length != EMPTY_COUNT) {
        memcpy(record + fields + arguments[BUS_STAGE].text_length, arguments[BUS_KEY].text, arguments[BUS_KEY].text_length);
    }
    uint64_t count = shared_header[BUS_HEADER_COUNT];
    __atomic_store_n(&shared_header[BUS_HEADER_END], (uint64_t)end, __ATOMIC_RELEASE);
    __atomic_store_n(&shared_header[BUS_HEADER_COUNT], count + 1, __ATOMIC_RELEASE);
    return count_word((size_t)count);
}

static Value offer_bus_close(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!shared_ready()) return nothing_value();
    expect_kind(arguments[BUS_INDEX], KIND_INTEGER, "bus_close expects an index");
    expect_kind(arguments[BUS_OUTCOME], KIND_TEXT, "bus_close expects text");
    if (arguments[BUS_INDEX].integer < WORD_FALSE) fail("bus_close index is outside the log");
    size_t fields = BUS_RECORD_FIELDS * sizeof(uint64_t);
    size_t span = align_to_word(checked_add(fields, arguments[BUS_OUTCOME].text_length));
    size_t header_bytes = BUS_HEADER_WORDS * sizeof(uint64_t);
    size_t start = (size_t)shared_header[BUS_HEADER_END];
    if (start < header_bytes) start = header_bytes;
    size_t end = checked_add(start, span);
    if (end > shared_mapped_bytes) shared_remap(grown_capacity(shared_mapped_bytes, end));
    unsigned char *record = (unsigned char *)shared_header + start;
    uint64_t fields_out[BUS_RECORD_FIELDS];
    fields_out[0] = BUS_PHASE_CLOSE;
    fields_out[1] = (uint64_t)getpid();
    fields_out[2] = monotonic_ns();
    fields_out[3] = (uint64_t)arguments[BUS_INDEX].integer;
    fields_out[4] = arguments[BUS_OUTCOME].text_length;
    fields_out[5] = 0;
    memcpy(record, fields_out, fields);
    if (arguments[BUS_OUTCOME].text_length != EMPTY_COUNT) memcpy(record + fields, arguments[BUS_OUTCOME].text, arguments[BUS_OUTCOME].text_length);
    uint64_t count = shared_header[BUS_HEADER_COUNT];
    __atomic_store_n(&shared_header[BUS_HEADER_END], (uint64_t)end, __ATOMIC_RELEASE);
    __atomic_store_n(&shared_header[BUS_HEADER_COUNT], count + 1, __ATOMIC_RELEASE);
    return count_word((size_t)count);
}

static Value offer_bus_read(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (!shared_ready()) return nothing_value();
    expect_kind(arguments[BUS_CURSOR], KIND_INTEGER, "bus_read expects an index");
    if (arguments[BUS_CURSOR].integer < WORD_FALSE) return list_from(NULL, EMPTY_COUNT);
    size_t cursor = (size_t)arguments[BUS_CURSOR].integer;
    size_t header_bytes = BUS_HEADER_WORDS * sizeof(uint64_t);
    size_t end = (size_t)__atomic_load_n(&shared_header[BUS_HEADER_END], __ATOMIC_ACQUIRE);
    size_t count = (size_t)__atomic_load_n(&shared_header[BUS_HEADER_COUNT], __ATOMIC_ACQUIRE);
    Value *rows = allocate_zeroed(count > cursor ? count - cursor : EMPTY_COUNT, sizeof *rows);
    size_t row_count = EMPTY_COUNT;
    size_t offset = header_bytes;
    for (size_t index = FIRST_INDEX; index < count; index = checked_add(index, ONE_ELEMENT)) {
        if (offset >= end || offset > shared_mapped_bytes) break;
        size_t available = shared_mapped_bytes - offset;
        size_t span = shared_record_span((unsigned char *)shared_header + offset, available);
        if (index >= cursor) {
            uint64_t phase = 0;
            uint64_t time_ns = 0;
            memcpy(&phase, (unsigned char *)shared_header + offset, sizeof phase);
            memcpy(&time_ns, (unsigned char *)shared_header + offset + 2 * sizeof(uint64_t), sizeof time_ns);
            Value *fields = allocate_zeroed(ARITY_THREE, sizeof *fields);
            fields[0] = count_word(index);
            fields[1] = count_word((size_t)phase);
            fields[2] = integer_value((long long)time_ns);
            rows[row_count] = list_from(fields, ARITY_THREE);
            row_count = checked_add(row_count, ONE_ELEMENT);
        }
        offset = checked_add(offset, span);
    }
    return list_from(rows, row_count);
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
            census.hottest_tag = (size_t)operations[index].id;
        }
    }
    return census;
}

static size_t text_cell_count(void) {
    size_t strings = EMPTY_COUNT;
    for (size_t index = FIRST_INDEX; index < cell_count; index = checked_add(index, ONE_ELEMENT)) {
        if (cells[index]->kind == CELL_TEXT) strings = checked_add(strings, ONE_ELEMENT);
    }
    return strings;
}

static void gift_held(size_t *frames, size_t *bytes) {
    *frames = EMPTY_COUNT;
    *bytes = EMPTY_COUNT;
    for (size_t index = FIRST_INDEX; index < gift_mapping_count; index = checked_add(index, ONE_ELEMENT)) {
        if (!gift_mappings[index].held) continue;
        *frames = checked_add(*frames, ONE_ELEMENT);
        *bytes = checked_add(*bytes, gift_mappings[index].bytes);
    }
}

static size_t node_private_bytes(void) {
    size_t bytes = cell_capacity * sizeof(Cell *);
    for (size_t index = FIRST_INDEX; index < cell_count; index = checked_add(index, ONE_ELEMENT)) {
        Cell *cell = cells[index];
        bytes = checked_add(bytes, sizeof *cell);
        if (cell->text != NULL) bytes = checked_add(bytes, checked_add(cell->text_length, TEXT_TERMINATOR_SIZE));
        if (cell->children != NULL) bytes = checked_add(bytes, cell->child_count * sizeof(Value));
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
    int wrote = snprintf(name, room, "/fg-k%lld", pid);
    return wrote > WORD_FALSE && (size_t)wrote < room;
}

static void release_live_page(void) {
    char name[LIVE_NAME_ROOM];
    if (!live_page_name(name, sizeof name, (long long)getpid())) return;
    shm_unlink(name);
}

static void publish_live_page(long long pid, const int64_t *words) {
    char name[LIVE_NAME_ROOM];
    if (!live_page_name(name, sizeof name, pid)) return;
    static int release_armed = WORD_FALSE;
    if (!release_armed) {
        atexit(release_live_page);
        release_armed = WORD_TRUE;
    }
    int descriptor = shm_open(name, O_RDWR | O_CREAT, GIFT_OPEN_MODE);
    if (descriptor < SYSTEM_CALL_OK) return;
    struct stat standing;
    if (fstat(descriptor, &standing) != SYSTEM_CALL_OK) {
        close(descriptor);
        return;
    }
    if (standing.st_size != (off_t)(LIVE_PAGE_WORDS * sizeof(int64_t))) {
        if (standing.st_size != EMPTY_COUNT) {
            close(descriptor);
            if (shm_unlink(name) != SYSTEM_CALL_OK) return;
            descriptor = shm_open(name, O_RDWR | O_CREAT | O_EXCL, GIFT_OPEN_MODE);
            if (descriptor < SYSTEM_CALL_OK) return;
        }
        if (ftruncate(descriptor, (off_t)(LIVE_PAGE_WORDS * sizeof(int64_t))) != SYSTEM_CALL_OK) {
            close(descriptor);
            return;
        }
    }
    void *mapped = mmap(NULL, LIVE_PAGE_WORDS * sizeof(int64_t), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, EMPTY_COUNT);
    close(descriptor);
    if (mapped == MAP_FAILED) return;
    memcpy(mapped, words, LIVE_PAGE_WORDS * sizeof(int64_t));
    munmap(mapped, LIVE_PAGE_WORDS * sizeof(int64_t));
}

static int read_live_page(long long pid, int64_t *words) {
    char name[LIVE_NAME_ROOM];
    if (!live_page_name(name, sizeof name, pid)) return WORD_FALSE;
    int descriptor = shm_open(name, O_RDONLY, GIFT_OPEN_MODE);
    if (descriptor < SYSTEM_CALL_OK) return WORD_FALSE;
    struct stat standing;
    if (fstat(descriptor, &standing) != SYSTEM_CALL_OK || standing.st_size < (off_t)(LIVE_PAGE_WORDS * sizeof(int64_t))) {
        close(descriptor);
        return WORD_FALSE;
    }
    void *mapped = mmap(NULL, LIVE_PAGE_WORDS * sizeof(int64_t), PROT_READ, MAP_SHARED, descriptor, EMPTY_COUNT);
    close(descriptor);
    if (mapped == MAP_FAILED) return WORD_FALSE;
    memcpy(words, mapped, LIVE_PAGE_WORDS * sizeof(int64_t));
    munmap(mapped, LIVE_PAGE_WORDS * sizeof(int64_t));
    return WORD_TRUE;
}

static void fill_live_words(int64_t *words) {
    memset(words, EMPTY_COUNT, LIVE_PAGE_WORDS * sizeof *words);
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
    words[LIVE_NODES] = (int64_t)cell_count;
    words[LIVE_STRINGS] = (int64_t)text_cell_count();
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
    words[LIVE_MINTS] = (int64_t)cell_count;
}

static Value live_list(const int64_t *words) {
    Value *items = allocate_zeroed(LIVE_PAGE_WORDS, sizeof *items);
    for (size_t index = FIRST_INDEX; index < LIVE_PAGE_WORDS; index = checked_add(index, ONE_ELEMENT)) {
        items[index] = integer_value((long long)words[index]);
    }
    return list_from(items, LIVE_PAGE_WORDS);
}

static Value offer_kernel_stat(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "kernel_stat expects an integer key");
    long long key = arguments[SOLE_OPERAND].integer;
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
    if (key == STAT_CELLS || key == STAT_NODES_GAS || key == STAT_NODES_MINTED) return count_word(cell_count);
    if (key == STAT_STRINGS) return count_word(text_cell_count());
    if (key == STAT_CONS || key == STAT_FLOATS || key == STAT_JIT_BOXES || key == STAT_JIT_UNBOXES || key == STAT_JIT_NATIVE || key == STAT_JIT_CRYSTALLIZED || key == STAT_JIT_UNROLL || key == STAT_JIT_LOOP_ITERATIONS || key == STAT_FLOAT_CAPACITY || key == STAT_FIELD_FLOATS || key == STAT_NODES_WATER || key == STAT_NODES_ICE || key == STAT_NODE_LOGICAL_BYTES) return integer_value(WORD_FALSE);
    if (key == STAT_STACK) return count_word(frame_count);
    if (key == STAT_RECIPES) return count_word(function_count);
    if (key == STAT_OWNER_CONSTRUCTION_CLOCK) return count_word(owner_construction_clock);
    if (key == STAT_NODE_PRIVATE_BYTES) return count_word(node_private_bytes());
    return nothing_value();
}

static Value offer_kernel_live_pids(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    Value *pids = allocate_zeroed(ONE_ELEMENT, sizeof *pids);
    pids[FIRST_INDEX] = integer_value((long long)getpid());
    return list_from(pids, ONE_ELEMENT);
}

static Value offer_kernel_live(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "kernel_live expects a pid");
    long long pid = arguments[SOLE_OPERAND].integer;
    int64_t words[LIVE_PAGE_WORDS];
    if (pid == (long long)getpid()) {
        fill_live_words(words);
        publish_live_page(pid, words);
        return live_list(words);
    }
    if (!read_live_page(pid, words) || words[LIVE_PID] != pid || words[LIVE_ALIVE] != WORD_TRUE) return list_from(NULL, EMPTY_COUNT);
    return live_list(words);
}

static Value offer_kernel_page_hot(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_INTEGER, "kernel_page_hot expects a pid");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "kernel_page_hot expects a count");
    return list_from(NULL, EMPTY_COUNT);
}

static Value offer_kernel_rows_absent(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "a hot-row count is an integer");
    return list_from(NULL, EMPTY_COUNT);
}

static Value offer_metal_status(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    return text_value(METAL_ABSENT_STATUS, strlen(METAL_ABSENT_STATUS));
}

static Value offer_metal_live(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    return list_from(NULL, EMPTY_COUNT);
}

static Value offer_socket_listen(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "socket_listen expects a port");
    int descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
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
    int accepted = accept(descriptor, NULL, NULL);
    if (accepted < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    return integer_value((long long)socket_put(accepted));
}

static Value offer_socket_connect(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "socket_connect expects a host");
    expect_kind(arguments[RIGHT_OPERAND], KIND_INTEGER, "socket_connect expects a port");
    int descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    struct sockaddr_in address;
    memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_port = htons((uint16_t)arguments[RIGHT_OPERAND].integer);
    if (inet_pton(AF_INET, arguments[LEFT_OPERAND].text, &address.sin_addr) != 1
        || connect(descriptor, (struct sockaddr *)&address, sizeof address) != SYSTEM_CALL_OK) {
        close(descriptor);
        return integer_value(-WORD_TRUE);
    }
    return integer_value((long long)socket_put(descriptor));
}

static Value offer_socket_send(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_INTEGER, "socket_send expects a handle");
    int descriptor = socket_fetch(arguments[LEFT_OPERAND].integer);
    if (descriptor < SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
    if (arguments[RIGHT_OPERAND].kind == KIND_TEXT) {
        ssize_t sent = send(descriptor, arguments[RIGHT_OPERAND].text, arguments[RIGHT_OPERAND].text_length, 0);
        return integer_value(sent < SYSTEM_CALL_OK ? -WORD_TRUE : (long long)sent);
    }
    unsigned char *bytes = NULL;
    size_t length = EMPTY_COUNT;
    if (!list_to_bytes(arguments[RIGHT_OPERAND], &bytes, &length)) return integer_value(-WORD_TRUE);
    ssize_t sent = send(descriptor, bytes, length, 0);
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
    ssize_t got = recv(descriptor, bytes, budget, 0);
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

static Value offer_list(Value *arguments, size_t argument_count) {
    return list_from(arguments, argument_count);
}

static void register_builtin_operations(void) {
    register_operation("if", ARITY_THREE, FIXED_ARITY, LEAVE_ARGUMENTS, OP_IF, NULL);
    register_operation("do", ARITY_NONE, VARIADIC, LEAVE_ARGUMENTS, OP_DO, NULL);
    register_operation("and", ARITY_NONE, VARIADIC, LEAVE_ARGUMENTS, OP_AND, NULL);
    register_operation("list", ARITY_NONE, VARIADIC, EVALUATE_ARGUMENTS, OP_LIST, offer_list);
    register_operation("add", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_ADD, offer_add);
    register_operation("sub", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_SUB, offer_sub);
    register_operation("eq", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_EQ, offer_eq);
    register_operation("lt", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_LT, offer_lt);
    register_operation("ge", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_GE, offer_ge);
    register_operation("str_eq", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_STR_EQ, offer_str_eq);
    register_operation("str_concat", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_STR_CONCAT, offer_str_concat);
    register_operation("value_kind", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_VALUE_KIND, offer_value_kind);
    register_operation("nothing?", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_NOTHING, offer_nothing);
    register_operation(NAME_NOTHING, ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_NOTHING, offer_nothing_value);
    register_operation("read_line", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_READ_LINE, offer_read_line);
    register_operation("file_mtime", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FILE_MTIME, offer_file_mtime);
    register_operation("cell_path", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_CELL_PATH, offer_cell_path);
    register_operation("intern_trivial_bool", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_INTERN_BOOL, offer_intern_bool);
    register_operation("kernel_stat", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_stat);
    register_operation("kernel_live_pids", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_live_pids);
    register_operation("kernel_live", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_live);
    register_operation("kernel_page_hot", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_page_hot);
    register_operation("kernel_hot_rows", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_rows_absent);
    register_operation("kernel_box_rows", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_rows_absent);
    register_operation("fb_record", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FB_RECORD, offer_fb_record);
    register_operation("framebuffer-events", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FB_EVENTS, offer_fb_events);
    register_operation("framebuffer-clear", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FB_CLEAR, offer_fb_clear);
    register_operation("bus_open", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_BUS_OPEN, offer_bus_open);
    register_operation("bus_close", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_BUS_CLOSE, offer_bus_close);
    register_operation("bus_read", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_BUS_READ, offer_bus_read);
    register_operation("gt", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_GT, offer_gt);
    register_operation("le", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_LE, offer_le);
    register_operation("nth", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_NTH, offer_nth);
    operation_answers("nth");
    register_operation("slice", ARITY_NONE, VARIADIC, EVALUATE_ARGUMENTS, OP_SLICE, offer_slice);
    register_operation("+", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PLUS, offer_plus);
    register_operation("let", ARITY_TWO, FIXED_ARITY, LEAVE_ARGUMENTS, OP_LET, NULL);
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
    register_operation("print", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_print);
    register_operation("record_new", ARITY_NONE, VARIADIC, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_new);
    register_operation("record_has", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_has);
    register_operation("record_get", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_get);
    register_operation("record_set", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_set);
    register_operation("record?", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_type);
    register_operation("record_blueprint", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_blueprint);
    register_operation("record_keys", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_record_keys);
    register_operation("method_define", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_method_define);
    register_operation("method_has", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_method_has);
    register_operation("method_invoke", ARITY_NONE, VARIADIC, EVALUATE_ARGUMENTS, OP_PRINT, offer_method_invoke);
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
    register_operation("str_to_float", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_str_to_float);
    register_operation("float_to_int", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_float_to_int);
    register_operation("float_value", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_float_value);
    register_operation("make_float32", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_make_float32);
    register_operation("make_float64", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_make_float64);
    register_operation("intern_trivial_float", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_intern_trivial_float);
    register_operation("float_leaf", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_float_leaf);
    register_operation("math_pi", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_math_pi);
    register_operation("math_sqrt", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_math_sqrt);
    register_operation("math_exp", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_math_exp);
    register_operation("math_log", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_math_log);
    register_operation("math_pow", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_math_pow);
    register_operation("round_ndigits", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_round_ndigits);
    register_operation("str_byte_at", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_str_byte_at);
    register_operation("value_str", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_value_str);
    register_operation("value_eq", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_value_eq);
    register_operation("_get", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_get);
    register_operation("bp", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_bp);
    register_operation("range", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_range);
    register_operation("host_sleep_ms", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_sleep_ms);
    register_operation("host_monotonic_ms", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_monotonic_ms);
    register_operation("now_unix_ms", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_now_unix_ms);
    register_operation("host_getenv", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_getenv);
    register_operation("host_alive", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_alive);
    register_operation("host_kill", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_kill);
    register_operation("host_wait", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_wait);
    register_operation("host_dir_mkdir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_dir_mkdir);
    register_operation("fs_mkdir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_fs_mkdir);
    register_operation("host_dir_rmdir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_dir_rmdir);
    register_operation("fs_rmdir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_fs_rmdir);
    register_operation("host_path_exists", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_path_exists);
    register_operation("fs_exists", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_fs_exists);
    register_operation("host_path_is_dir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_path_is_dir);
    register_operation("fs_is_dir", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_fs_is_dir);
    register_operation("host_path_remove", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_path_remove);
    register_operation("fs_remove", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_fs_remove);
    register_operation("host_path_rename", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_path_rename);
    register_operation("fs_rename", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_fs_rename);
    register_operation("host_dir_list", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_dir_list);
    register_operation("fs_list", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_fs_list);
    register_operation("host_pid", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_pid);
    register_operation("temp_dir", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_temp_dir);
    register_operation("host_temp_dir", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_temp_dir);
    register_operation("host_cwd", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_cwd);
    register_operation("host_file_write_text", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_write_text);
    register_operation("write_file", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_write_file);
    register_operation("write_file_text", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_write_file_text);
    register_operation("write_file_bytes", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_write_file_bytes);
    register_operation("file_append_bytes", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_append_bytes);
    register_operation("host_file_append_bytes", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_append_bytes);
    register_operation("file_size", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_size);
    register_operation("host_file_size", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_size);
    register_operation("host_file_mtime", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_mtime);
    register_operation("host_file_read_text", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_read_text);
    register_operation("read_file", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_read_file);
    register_operation("read_file_bytes", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_read_file_bytes);
    register_operation("read_file_slice", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_read_file_slice);
    register_operation("host_file_read_slice", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_file_read_slice);
    register_operation("file_read", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_file_read);
    register_operation("recipe_to_bytes", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_recipe_to_bytes);
    register_operation("bytes_to_recipe", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_bytes_to_recipe);
    register_operation("write_form_binary", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_write_form_binary);
    register_operation("read_form_binary", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_read_form_binary);
    register_operation("source_inventory", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_source_inventory);
    register_operation("host_capture", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_capture);
    register_operation("input_byte", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_input_byte);
    register_operation("terminal_cols", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_terminal_cols);
    register_operation("shm_offer", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shm_offer);
    register_operation("shm_receive", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shm_receive);
    register_operation("shm_read", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shm_read);
    register_operation("shm_write", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shm_write);
    register_operation("shm_seq", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shm_seq);
    register_operation("shm_release", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_shm_release);
    register_operation("node_category", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_category);
    register_operation("node_eq", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_eq);
    register_operation("node_value", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_payload);
    register_operation("node_gift_read", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_gift_read);
    register_operation("node_gift_write", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_gift_write);
    register_operation("make_nodeid", ARITY_FOUR, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_make_nodeid);
    register_operation("node_pkg", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_pkg);
    register_operation("node_level", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_level);
    register_operation("node_type", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_type);
    register_operation("node_inst", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_inst);
    register_operation("intern_trivial_string", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_intern_trivial_string);
    register_operation("intern_trivial_int", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_intern_trivial_int);
    register_operation("intern_node", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_intern_node);
    register_operation("intern_node_at", ARITY_FIVE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_intern_node_at);
    register_operation("node_children", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_children);
    register_operation("node_source", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_source);
    register_operation("host-exec", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_exec);
    register_operation("form_error", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_form_error);
    register_operation("socket_listen", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_listen);
    register_operation("socket_port", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_port);
    register_operation("socket_accept", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_accept);
    register_operation("socket_connect", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_connect);
    register_operation("socket_send", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_send);
    register_operation("socket_recv", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_recv);
    register_operation("socket_close", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_close);
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
    register_operation("host_spawn_quiet", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_spawn_quiet);
    register_operation("host_spawn_at", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_host_spawn_at);
    register_operation(COALESCE_WORD, ARITY_TWO, FIXED_ARITY, LEAVE_ARGUMENTS, OP_COALESCE, NULL);
    register_operation(TRY_WORD, ARITY_TWO, FIXED_ARITY, LEAVE_ARGUMENTS, OP_TRY, NULL);
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
    if (left.kind == KIND_FUNCTION || right.kind == KIND_FUNCTION) fail("function identity is not compared");
    if (left.kind != right.kind) return 0;
    if (left.kind == KIND_NOTHING) return 1;
    if (left.kind == KIND_INTEGER) return left.integer == right.integer;
    if (left.kind == KIND_NODE) return left.cell == right.cell && left.integer == right.integer;
    if (left.kind == KIND_REAL) return left.real == right.real;
    if (left.kind == KIND_TEXT) return bytes_equal(left.text, left.text_length, right.text, right.text_length);
    return 0;
}

static int values_equal(Value left, Value right) {
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
            pair.left_cursor = pair.left_cursor->tail;
            pair.right_cursor = pair.right_cursor->tail;
            pair.index = checked_add(pair.index, ONE_ELEMENT);
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
        if (!atom_equal(pair.left, pair.right)) {
            same = 0;
            break;
        }
        pair_count -= ONE_ELEMENT;
    }
    free(pairs);
    return same;
}

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
    frame_count = needed;
    return &frames[frame_count - ONE_ELEMENT];
}

static void pop_frame(void) {
    Frame *frame = &frames[frame_count - ONE_ELEMENT];
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
    if (bytes_equal_cstr(bytes, length, NAME_NOTHING) || bytes_equal_cstr(bytes, length, FAIL_NAME) || bytes_equal_cstr(bytes, length, STOP_NAME)) {
        *out = nothing_value();
        return 1;
    }
    for (size_t index = global_count; index > FIRST_INDEX; ) {
        index -= ONE_ELEMENT;
        if (bytes_equal(globals[index].bytes, globals[index].length, bytes, length)) {
            *out = globals[index].value;
            return 1;
        }
    }
    return 0;
}

static Value value_from_atom(Form *form) {
    if (form->kind == FORM_INTEGER) return integer_value(form->integer);
    if (form->kind == FORM_REAL) return real_value(form->real);
    if (form->kind == FORM_TEXT) return text_value(form->text, form->text_length);
    if (form->kind == FORM_SYMBOL) {
        Value bound;
        if (find_value_binding(form->text, form->text_length, &bound)) return bound;
        Binding *function = find_function_text(form->text, form->text_length);
        if (function != NULL) return function->value;
        fail_at(form->offset, message_with_name(UNBOUND_PREFIX, form->text, form->text_length));
    }
    fail_at(form->offset, "a form has an unknown shape");
    return nothing_value();
}

static void offer_form(Form *form);

static void begin_function_call(Form *form, Value callee) {
    size_t argument_count = call_argument_count(form);
    if (argument_count != callee.parameter_count) fail_at(form->offset, "a function received the wrong number of arguments");
    Frame *frame = append_frame();
    frame->kind = FRAME_EAGER;
    frame->form = form;
    frame->argument_count = argument_count;
    frame->arguments = allocate_zeroed(argument_count, sizeof *frame->arguments);
    frame->callee = callee;
}

static void begin_call(Form *form) {
    if (form->part_count == EMPTY_COUNT) fail_at(form->offset, "a call needs a head");
    Form *head = form->parts[CALL_HEAD];
    if (head->kind != FORM_SYMBOL) {
        Frame *frame = append_frame();
        frame->kind = FRAME_APPLY;
        frame->form = form;
        return;
    }
    if (form_is_symbol(head, NAME_DEFN)) {
        if (inside_function()) bind_runtime_definition(form);
        else if (!form->is_bound) bind_definition(form);
        deliver(nothing_value());
        return;
    }
    size_t argument_count = call_argument_count(form);
    Value bound;
    if (find_value_binding(head->text, head->text_length, &bound) && bound.kind == KIND_FUNCTION) {
        begin_function_call(form, bound);
        return;
    }
    const Operation *operation = find_operation_text(head->text, head->text_length);
    Binding *function = find_function_text(head->text, head->text_length);
    if (operation != NULL && (operation->evaluation == LEAVE_ARGUMENTS || operation->answers)) function = NULL;
    if (function != NULL) {
        begin_function_call(form, function->value);
        return;
    }
    if (operation != NULL) {
        if (operation->id == OP_LET) {
            if (argument_count < ARITY_TWO) fail_at(form->offset, "let expects a name and a value");
        } else if (operation->arity_policy == FIXED_ARITY && argument_count != operation->arity) {
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
        else if (operation->id == OP_TRY) frame->kind = FRAME_TRY;
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

static void run_function_body(size_t index) {
    Frame frame = frames[index];
    size_t base = local_count;
    for (size_t parameter = FIRST_INDEX; parameter < frame.callee.parameter_count; parameter = checked_add(parameter, ONE_ELEMENT)) {
        append_local(frame.callee.parameters[parameter].bytes, frame.callee.parameters[parameter].length, frame.arguments[parameter]);
    }
    frame.local_base = base;
    frame.kind = FRAME_FUNCTION;
    frame.has_incoming = 0;
    frames[index] = frame;
    offer_form(frame.callee.body);
}

static void step_eager(size_t index) {
    Frame frame = frames[index];
    if (frame.has_incoming) {
        frame.arguments[frame.next_part] = frame.incoming;
        frame.has_incoming = 0;
        frame.next_part = checked_add(frame.next_part, ONE_ELEMENT);
        frames[index] = frame;
        return;
    }
    if (frame.next_part >= frame.argument_count) {
        if (frame.operation == NULL) {
            run_function_body(index);
            return;
        }
        operations[(size_t)(frame.operation - operations)].calls = checked_add(frame.operation->calls, ONE_ELEMENT);
        Value result = frame.operation->offer(frame.arguments, frame.argument_count);
        pop_frame();
        deliver(result);
        return;
    }
    Form *child = call_argument(frame.form, frame.next_part);
    frames[index] = frame;
    offer_form(child);
}

static void bind_global(const char *bytes, size_t length, Value value) {
    size_t needed = checked_add(global_count, ONE_ELEMENT);
    globals = grow_list(globals, &global_capacity, sizeof *globals, needed);
    globals[global_count].bytes = bytes;
    globals[global_count].length = length;
    globals[global_count].value = value;
    global_count = needed;
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

static void step_lambda(size_t index) {
    Frame frame = frames[index];
    if (call_argument_count(frame.form) != ARITY_TWO) fail_at(frame.form->offset, "a lambda names parameters and a body");
    Form *parameters = call_argument(frame.form, LEFT_OPERAND);
    Form *body = call_argument(frame.form, RIGHT_OPERAND);
    if (parameters->kind != FORM_GROUP) fail_at(parameters->offset, "a lambda parameter list must be a group");
    Value function = nothing_value();
    function.kind = KIND_FUNCTION;
    function.body = body;
    function.parameter_count = parameters->part_count;
    function.parameters = allocate_zeroed(parameters->part_count, sizeof *function.parameters);
    for (size_t parameter = FIRST_INDEX; parameter < parameters->part_count; parameter = checked_add(parameter, ONE_ELEMENT)) {
        Form *name = parameters->parts[parameter];
        if (name->kind != FORM_SYMBOL) fail_at(name->offset, "a lambda parameter must be a symbol");
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
    if (base > local_count) fail_at(frame.form->offset, "a lambda looked above its locals");
    size_t visible = local_count - base;
    size_t total = checked_add(outer_count, visible);
    if (total != EMPTY_COUNT) {
        Binding *captures = allocate_zeroed(total, sizeof *captures);
        if (outer_count != EMPTY_COUNT) memcpy(captures, outer, outer_count * sizeof *captures);
        if (visible != EMPTY_COUNT) memcpy(captures + outer_count, locals + base, visible * sizeof *captures);
        function.captures = captures;
        function.capture_count = total;
    }
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
        frame.has_accumulator = 1;
        frames[index] = frame;
    }
    if (frame.has_incoming) {
        Value value = frame.incoming;
        frame.has_incoming = 0;
        if (value.kind != KIND_NOTHING) {
            local_count = frame.local_base;
            pop_frame();
            deliver(value);
            return;
        }
        frame.stage = checked_add(frame.stage, ONE_ELEMENT);
        frames[index] = frame;
    }
    if (frame.stage >= call_argument_count(frame.form)) {
        local_count = frame.local_base;
        pop_frame();
        deliver(nothing_value());
        return;
    }
    Form *child = call_argument(frame.form, frame.stage);
    frames[index] = frame;
    offer_form(child);
}

static void step_try(size_t index) {
    Frame frame = frames[index];
    if (frame.stage == IF_STAGE_TEST) {
        frame.local_base = local_count;
        frame.stage = TRY_STAGE_BODY;
        frames[index] = frame;
        offer_form(call_argument(frame.form, LEFT_OPERAND));
        return;
    }
    if (frame.has_incoming) {
        Value value = frame.incoming;
        local_count = frame.local_base;
        pop_frame();
        deliver(value);
        return;
    }
    if (frame.stage == TRY_STAGE_HANDLER) {
        frames[index] = frame;
        offer_form(call_argument(frame.form, RIGHT_OPERAND));
        return;
    }
    fail("a try did not offer a result");
}

static int unwind_to_recovery(void) {
    for (size_t cursor = frame_count; cursor > FIRST_INDEX; ) {
        cursor -= ONE_ELEMENT;
        if (frames[cursor].kind == FRAME_CHOICE) {
            frames[cursor].has_incoming = 0;
            frames[cursor].stage = checked_add(frames[cursor].stage, ONE_ELEMENT);
            local_count = frames[cursor].local_base;
            frame_count = checked_add(cursor, ONE_ELEMENT);
            return 1;
        }
        if (frames[cursor].kind != FRAME_TRY || frames[cursor].stage == TRY_STAGE_HANDLER) continue;
        frames[cursor].has_incoming = 0;
        frames[cursor].stage = TRY_STAGE_HANDLER;
        local_count = frames[cursor].local_base;
        frame_count = checked_add(cursor, ONE_ELEMENT);
        return 1;
    }
    return 0;
}

static void step_fail(size_t index) {
    (void)index;
    if (unwind_to_recovery()) return;
    fail("a failure was not caught");
}

static void step_apply(size_t index) {
    Frame frame = frames[index];
    if (frame.has_incoming) {
        if (frame.stage == IF_STAGE_TEST) {
            if (frame.incoming.kind != KIND_FUNCTION) fail_at(frame.form->parts[CALL_HEAD]->offset, "a call head is not a function");
            size_t argument_count = call_argument_count(frame.form);
            if (argument_count != frame.incoming.parameter_count) fail_at(frame.form->offset, "a function received the wrong number of arguments");
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
    size_t argument_count = call_argument_count(frame.form);
    if (argument_count < ARITY_TWO) fail_at(frame.form->offset, "let expects a name and a value");
    if (!frame.has_incoming) {
        if (frame.stage == IF_STAGE_BRANCH) {
            if (frame.next_part >= argument_count) {
                Value result = frame.has_accumulator ? frame.accumulator : nothing_value();
                pop_frame();
                deliver(result);
                return;
            }
            Form *child = call_argument(frame.form, frame.next_part);
            frames[index] = frame;
            offer_form(child);
            return;
        }
        frames[index] = frame;
        offer_form(call_argument(frame.form, RIGHT_OPERAND));
        return;
    }
    if (frame.stage == IF_STAGE_BRANCH) {
        frame.accumulator = frame.incoming;
        frame.has_accumulator = 1;
        frame.has_incoming = 0;
        frame.next_part = checked_add(frame.next_part, ONE_ELEMENT);
        frames[index] = frame;
        return;
    }
    Form *name = call_argument(frame.form, LEFT_OPERAND);
    if (name->kind != FORM_SYMBOL) fail_at(name->offset, "a let name must be a symbol");
    if (inside_function()) append_local(name->text, name->text_length, frame.incoming);
    else bind_global(name->text, name->text_length, frame.incoming);
    Value value = frame.incoming;
    if (argument_count == ARITY_TWO) {
        pop_frame();
        deliver(value);
        return;
    }
    frame.stage = IF_STAGE_BRANCH;
    frame.next_part = ARITY_TWO;
    frame.accumulator = value;
    frame.has_accumulator = 1;
    frame.has_incoming = 0;
    frames[index] = frame;
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
    Frame *frame = append_frame();
    frame->kind = FRAME_SEQUENCE;
    frame->form = program;
    while (frame_count != EMPTY_COUNT) {
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
}

static void drive_active_frames(void) {
    while (frame_count != EMPTY_COUNT) {
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
}

static Value invoke_function_value(Value function, Value *arguments, size_t argument_count) {
    if (function.kind != KIND_FUNCTION) fail("method_invoke expects a function value");
    if (argument_count != function.parameter_count) fail("a function received the wrong number of arguments");
    Frame *saved_frames = frames;
    size_t saved_frame_count = frame_count;
    size_t saved_frame_capacity = frame_capacity;
    Binding *saved_locals = locals;
    size_t saved_local_count = local_count;
    size_t saved_local_capacity = local_capacity;
    Value saved_finished = finished;
    int saved_result_ready = result_ready;

    frames = NULL;
    frame_count = EMPTY_COUNT;
    frame_capacity = EMPTY_COUNT;
    locals = NULL;
    local_count = EMPTY_COUNT;
    local_capacity = EMPTY_COUNT;
    finished = nothing_value();
    result_ready = 0;

    Frame *frame = append_frame();
    frame->kind = FRAME_FUNCTION;
    frame->callee = function;
    frame->local_base = EMPTY_COUNT;
    for (size_t capture = FIRST_INDEX; capture < function.capture_count; capture = checked_add(capture, ONE_ELEMENT)) {
        append_local(function.captures[capture].bytes, function.captures[capture].length, function.captures[capture].value);
    }
    for (size_t index = FIRST_INDEX; index < argument_count; index = checked_add(index, ONE_ELEMENT)) {
        append_local(function.parameters[index].bytes, function.parameters[index].length, arguments[index]);
    }
    offer_form(function.body);
    drive_active_frames();
    if (!result_ready) fail("the function produced no result");
    Value result = finished;

    free(frames);
    free(locals);
    frames = saved_frames;
    frame_count = saved_frame_count;
    frame_capacity = saved_frame_capacity;
    locals = saved_locals;
    local_count = saved_local_count;
    local_capacity = saved_local_capacity;
    finished = saved_finished;
    result_ready = saved_result_ready;
    return result;
}

typedef struct {
    Value value;
    size_t index;
    int opened;
} PrintItem;

static void print_atom(Value value) {
    if (value.kind == KIND_INTEGER) {
        printf(INTEGER_FORMAT, value.integer);
        return;
    }
    if (value.kind == KIND_REAL) {
        printf(REAL_FORMAT, DBL_DECIMAL_DIG, value.real);
        return;
    }
    if (value.kind == KIND_TEXT) {
        fwrite(value.text, sizeof *value.text, value.text_length, stdout);
        return;
    }
    fputs(kind_name(value.kind), stdout);
}

static void print_value(Value root) {
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
            print_atom(item.value);
            item_count -= ONE_ELEMENT;
            continue;
        }
        if (!item.opened) {
            fputs(LIST_OPEN, stdout);
            item.opened = 1;
            item.index = FIRST_INDEX;
            items[item_count - ONE_ELEMENT] = item;
            if (item.value.item_count == EMPTY_COUNT) {
                fputs(LIST_CLOSE, stdout);
                item_count -= ONE_ELEMENT;
            }
            continue;
        }
        if (item.index == item.value.item_count) {
            fputs(LIST_CLOSE, stdout);
            item_count -= ONE_ELEMENT;
            continue;
        }
        if (item.index > FIRST_INDEX) fputs(LIST_SEPARATOR, stdout);
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
}

typedef struct {
    const char *bytes;
    size_t length;
    size_t offset;
} SourceCursor;

static char **loaded_units;
static size_t loaded_count;
static size_t loaded_capacity;

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

static char *resolve_unit(const char *from_file, const char *relative) {
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
    fail(message_with_name("a source could not be opened: ", relative, strlen(relative)));
    return NULL;
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

static int contains_text(const char *bytes, size_t length, const char *needle) {
    size_t needle_length = strlen(needle);
    if (needle_length > length) return 0;
    size_t last = length - needle_length;
    for (size_t index = FIRST_INDEX; index <= last; index = checked_add(index, ONE_ELEMENT)) {
        if (memcmp(bytes + index, needle, needle_length) == STRING_COMPARE_EQUAL) return 1;
    }
    return 0;
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
                if (!isfinite(number->real)) fail_at(mark, "a real is outside the native range");
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
                return parse_bml_postfix();
            }
            (void)name_length;
        }
        source_offset = saved;
        expect_character(CALL_OPEN, "a group was not opened");
        Form *inner = parse_bml_expression();
        expect_character(CALL_CLOSE, "a group was not closed");
        return inner;
    }
    if (is_symbol_char(character) && !is_digit_char(character)) return read_bml_symbol();
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
            expression = form_call_head(method, arguments, argument_count);
            continue;
        }
        if (character != INDEX_OPEN) break;
        size_t opening = source_offset;
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        Form *start = parse_bml_expression();
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == PAIR_MARK) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == INDEX_CLOSE) {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
                Form *arguments[] = {expression, start};
                expression = form_call("slice", arguments, ARITY_TWO, opening);
                continue;
            }
            Form *end = parse_bml_expression();
            expect_character(INDEX_CLOSE, "an index was not closed");
            Form *arguments[] = {expression, start, end};
            expression = form_call("slice", arguments, ARITY_THREE, opening);
            continue;
        }
        expect_character(INDEX_CLOSE, "an index was not closed");
        Form *arguments[] = {expression, start};
        expression = form_call("nth", arguments, ARITY_TWO, opening);
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
        expression = form_call(operation, arguments, ARITY_TWO, expression->offset);
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
            Form *equal = form_call("eq", equal_arguments, ARITY_TWO, expression->offset);
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
    Form *subject = parse_bml_expression();
    expect_character(SECTION_OPEN, "a match is missing its arms");
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
    if (take_keyword("match")) return parse_bml_match(opening);
    if (take_keyword("choice")) {
        expect_character(SECTION_OPEN, "a choice was not opened");
        Form **options = NULL;
        size_t option_count = EMPTY_COUNT;
        size_t option_capacity = EMPTY_COUNT;
        parse_bml_statements(&options, &option_count, &option_capacity);
        return form_call("choice", options, option_count, opening);
    }
    if (take_keyword(TRY_WORD)) {
        Form *body = parse_bml_expression();
        if (!take_keyword("catch") && !take_keyword("else")) fail_at(source_offset, "a try is missing catch");
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
        group_add(group, read_bml_symbol());
        skip_bml();
        if (source_offset < source_length && source_bytes[source_offset] == PAIR_MARK) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
            skip_bml();
            read_bml_symbol();
            skip_type_arguments();
            skip_bml();
        }
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
        if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
        }
    }
    return definition_form(name, parameters, body);
}

static Form *parse_pattern_binding(size_t opening) {
    Form *pattern = parse_bml_list();
    expect_character(NAME_MARK, "a binding is missing its value");
    Form *value = parse_bml_expression();
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
    }
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
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == PAIR_MARK) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
        skip_bml();
        read_bml_symbol();
        skip_type_arguments();
    }
    expect_character(NAME_MARK, "a binding is missing its value");
    Form *value = parse_bml_expression();
    skip_bml();
    if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
    }
    Form *arguments[] = {name, value};
    return form_call("let", arguments, ARITY_TWO, name->offset);
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
            if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
            }
            Form **rest = NULL;
            size_t rest_count = EMPTY_COUNT;
            size_t rest_capacity = EMPTY_COUNT;
            parse_bml_statements(&rest, &rest_count, &rest_capacity);
            Form *body = form_call("do", rest, rest_count, deferred->offset);
            Form *propagate = form_call(FAIL_NAME, NULL, EMPTY_COUNT, deferred->offset);
            Form *catch_statements[] = {deferred, propagate};
            Form *handler = form_call("do", catch_statements, ARITY_TWO, deferred->offset);
            Form *try_arguments[] = {body, handler};
            Form *attempt = form_call(TRY_WORD, try_arguments, ARITY_TWO, deferred->offset);
            Form *holder = match_subject_name(deferred->offset);
            Form *binding_arguments[] = {holder, attempt};
            Form *binding = form_call("let", binding_arguments, ARITY_TWO, deferred->offset);
            Form *sequence[] = {binding, deferred, holder};
            append_form(forms, count, capacity, form_call("do", sequence, ARITY_THREE, deferred->offset));
            return;
        }
        if (take_keyword("let") || take_keyword("ref") || take_keyword("thought")) {
            append_form(forms, count, capacity, parse_bml_binding());
            continue;
        }
        if (take_keyword(DEF_WORD)) {
            append_form(forms, count, capacity, parse_bml_definition());
            continue;
        }
        if (take_keyword("field")) {
            Form *name = read_bml_symbol();
            skip_type_arguments();
            expect_character(NAME_MARK, "a field is missing its value");
            Form *body = parse_bml_expression();
            expect_character(STATEMENT_END, "a field was not finished");
            append_form(forms, count, capacity, definition_form(name, new_form(FORM_GROUP, name->offset), body));
            continue;
        }
        if (take_keyword("member")) {
            skip_member();
            continue;
        }
        if (take_keyword("class") || take_keyword("template")) {
            Form *name = read_bml_symbol();
            bind_global(name->text, name->text_length, text_value(name->text, name->text_length));
            skip_type_arguments();
            expect_character(SECTION_OPEN, "a class was not opened");
            parse_bml_statements(forms, count, capacity);
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
            }
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
                while (cursor < index && isspace((unsigned char)bytes[cursor])) cursor = checked_add(cursor, ONE_ELEMENT);
                if (cursor >= index) break;
                size_t start = cursor;
                while (cursor < index && !isspace((unsigned char)bytes[cursor])) cursor = checked_add(cursor, ONE_ELEMENT);
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

static Form **parse_bml_source(size_t *form_count) {
    Form **forms = NULL;
    size_t count = EMPTY_COUNT;
    size_t capacity = EMPTY_COUNT;
    while (1) {
        skip_bml();
        if (source_offset >= source_length) break;
        if (take_keyword(IMPORT_WORD)) {
            char *relative = read_import_path();
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
            }
            load_unit(source_path, relative);
            free(relative);
            continue;
        }
        if (!take_keyword(SECTION_WORD)) {
            append_form(&forms, &count, &capacity, parse_bml_expression());
            skip_bml();
            if (source_offset < source_length && source_bytes[source_offset] == STATEMENT_END) {
                source_offset = checked_add(source_offset, ONE_ELEMENT);
            }
            continue;
        }
        expect_character(INDEX_OPEN, "a section name was not opened");
        size_t name_at = source_offset;
        while (source_offset < source_length && source_bytes[source_offset] != INDEX_CLOSE) {
            source_offset = checked_add(source_offset, ONE_ELEMENT);
        }
        size_t name_length = source_offset - name_at;
        expect_character(INDEX_CLOSE, "a section name was not closed");
        int executable = name_length == strlen(FORM_BML_SECTION) && memcmp(source_bytes + name_at, FORM_BML_SECTION, name_length) == STRING_COMPARE_EQUAL;
        expect_character(SECTION_OPEN, "a section was not opened");
        if (!executable) {
            size_t depth = ONE_ELEMENT;
            while (depth != EMPTY_COUNT) {
                skip_bml();
                if (source_offset >= source_length) fail_at(source_offset, "a section was not closed");
                char character = source_bytes[source_offset];
                if (character == TEXT_DELIMITER) {
                    parse_text();
                    continue;
                }
                source_offset = checked_add(source_offset, ONE_ELEMENT);
                if (character == SECTION_OPEN) depth = checked_add(depth, ONE_ELEMENT);
                if (character == SECTION_CLOSE) depth -= ONE_ELEMENT;
            }
            continue;
        }
        parse_bml_statements(&forms, &count, &capacity);
    }
    *form_count = count;
    return forms;
}

static Form **parse_current_source(size_t *form_count) {
    if (contains_text(source_bytes, source_length, SECTION_TAG)) return parse_bml_source(form_count);
    return parse_source(form_count);
}

static void load_unit(const char *from_file, const char *relative) {
    char *path = resolve_unit(from_file, relative);
    if (unit_is_loaded(path)) {
        free(path);
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
    if (form_count != EMPTY_COUNT) drive_forms(forms, form_count);
    source_path = saved_path;
    restore_source(saved);
}

static void drive_forms(Form **forms, size_t form_count) {
    Form *program = new_form(FORM_GROUP, FIRST_INDEX);
    program->parts = forms;
    program->part_count = form_count;
    drive(program);
}

int main(int argc, char **argv) {
    process_born_ms = unix_milliseconds();
    if (argc < PROGRAM_ARGUMENT_COUNT) {
        fputs(USAGE_TEXT, stderr);
        return EXIT_FAILURE;
    }
    register_builtin_operations();
    source_path = argv[SOURCE_PATH_ARGUMENT];
    source_bytes = read_source(source_path, &source_length);
    remember_unit(copy_bytes(source_path, strlen(source_path)));
    load_preludes(source_bytes, source_length, source_path);
    source_offset = FIRST_INDEX;
    size_t form_count = EMPTY_COUNT;
    Form **forms = parse_current_source(&form_count);
    bind_definitions(forms, form_count);
    drive_forms(forms, form_count);
    print_value(finished);
    fputs(LINE_ENDING, stdout);
    if (fflush(stdout) != SYSTEM_CALL_OK || ferror(stdout)) fail("the result could not be written");
    return EXIT_SUCCESS;
}
