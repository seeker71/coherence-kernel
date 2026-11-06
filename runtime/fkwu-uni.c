/* The Form runtime reads one source file and offers its forms.
 * A word is nothing, an integer, a real, text, a list, a function, or a node.
 * Definitions are bound before any form runs, so a later definition is visible.
 * A prelude line names sources whose definitions join this program.
 * A form.bml section lowers its definitions into the same forms.
 * Words live until the process exits. Collections grow with the data.
 */

#include <ctype.h>
#include <dirent.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
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
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#ifdef __APPLE__
#include <libproc.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach-o/dyld.h>
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
static const char *const METAL_ABSENT_STATUS = "metal=not-up\nmetal_owner=fkwu-form-cli\nmetal_carrier=dynamic\nmetal_loaded=false\nmetal_door=handle\n";
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
    const char *prior_failure;
    int prior_failure_reported;
    size_t stage_base;
};

static const char *source_bytes;
static const char *entry_source_bytes;
static size_t entry_source_length;
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
static const char *failure_message;
static int failure_reported;
static jmp_buf *evaluation_recovery;
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

static size_t stage_depth;
static void stage_unwind(size_t depth, long long phase, const char *why);
static Value stage_door(long long mode, Value value);
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
        fprintf(stderr, "  %.*s at byte %zu in %s\n", (int)head->text_length, head->text, form->offset, form->path != NULL ? form->path : "<native>");
        shown += ONE_ELEMENT;
    }
}

static void fail(const char *reason) {
    if (evaluation_recovery != NULL) {
        failure_message = strdup(reason);
        if (failure_message == NULL) { fputs("fkwu: out of memory retaining failure\n", stderr); exit(EXIT_FAILURE); }
        failure_reported = 1;
        fprintf(stderr, ERROR_BARE, reason);
        longjmp(*evaluation_recovery, 1);
    }
    fprintf(stderr, ERROR_BARE, reason);
    note_stack();
    exit(EXIT_FAILURE);
}

static void fail_at(size_t offset, const char *reason) {
    if (parser_choice != NULL) longjmp(*parser_choice, 1);
    if (evaluation_recovery != NULL) fail(reason);
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

static uint64_t composite_hash(Value category, Value *children, size_t count) {
    uint64_t hash = hash_mix(hash_mix(VALUE_HASH_OFFSET, (uint64_t)CELL_COMPOSITE), hash_value(category));
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

static Cell *find_composite(Value category, Value *children, size_t count, uint64_t hash) {
    for (CellBucket *slot = cell_chain(hash); slot != NULL; slot = slot->next) {
        Cell *cell = slot->cell;
        if (cell->hash != hash || cell->kind != CELL_COMPOSITE || !values_equal(cell->category_value, category) || cell->child_count != count) continue;
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
    Value *flat = NULL;
    if (children.item_count != EMPTY_COUNT) {
        flat = allocate_zeroed(children.item_count, sizeof *flat);
        List *cursor = children.list;
        for (size_t index = FIRST_INDEX; index < children.item_count; index = checked_add(index, ONE_ELEMENT)) {
            flat[index] = cursor->head;
            cursor = cursor->tail;
        }
    }
    uint64_t hash = composite_hash(category, flat, children.item_count);
    Cell *found = find_composite(category, flat, children.item_count, hash);
    if (found != NULL) {
        free(flat);
        return found;
    }
    Cell *cell = cell_hold();
    cell->kind = CELL_COMPOSITE;
    /* Category describes the composition; the value has its own storage
     * coordinate. Memoization and node_at must never alias all of a category. */
    cell->category = fk_nid_make(0, 2, 0, (long long)(cell_count - 1));
    cell->category_value = category;
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

static Value offer_node_at(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value index = arguments[0];
    if (index.kind != KIND_INTEGER || index.integer < 0 || (unsigned long long)index.integer >= cell_count) return nothing_value();
    return node_from_cell(cells[(size_t)index.integer]);
}

static Value offer_intern_trivial_string(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "intern_trivial_string expects text");
    Cell *found = find_text_cell(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
    if (found == NULL) {
        found = cell_hold();
        found->kind = CELL_TEXT;
        found->category = fk_nid_make(1, 1, 2, (long long)(cell_count - 1));
        found->text = copy_bytes(arguments[SOLE_OPERAND].text, arguments[SOLE_OPERAND].text_length);
        found->text_length = arguments[SOLE_OPERAND].text_length;
        found->hash = text_cell_hash(found->text, found->text_length);
        cell_index(found);
    }
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
        found->category = fk_nid_fits(1, 1, 1, found->integer)
            ? fk_nid_make(1, 1, 1, found->integer)
            : fk_nid_make(1, 1, 5, (long long)(cell_count - 1));
        found->hash = integer_cell_hash(found->integer);
        cell_index(found);
    }
    return node_from_cell(found);
}

static Value intern_real(double number, int width) {
    if (width == 32) {
        float narrow = (float)number;
        uint32_t bits;
        memcpy(&bits, &narrow, sizeof bits);
        return node_value((long long)fk_nid_make(1, 1, 6, bits));
    }
    uint64_t bits;
    memcpy(&bits, &number, sizeof bits);
    uint64_t hash = hash_mix(hash_mix(hash_mix(VALUE_HASH_OFFSET, CELL_REAL), (uint64_t)width), bits);
    for (CellBucket *slot = cell_chain(hash); slot != NULL; slot = slot->next) {
        Cell *cell = slot->cell;
        if (cell->kind == CELL_REAL && cell->hash == hash && memcmp(&cell->real, &number, sizeof number) == 0 &&
            fk_nid_get(cell->category, 2) == (width == 32 ? 6 : 7)) return node_from_cell(cell);
    }
    Cell *cell = cell_hold();
    cell->kind = CELL_REAL;
    cell->real = number;
    cell->category = fk_nid_make(1, 1, width == 32 ? 6 : 7, (long long)(cell_count - 1));
    cell->hash = hash;
    cell_index(cell);
    return node_from_cell(cell);
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
    return real_value(decimal_prefix(arguments[0]));
}

static Value offer_intern_trivial_float(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_TEXT, "intern_trivial_float expects source text");
    return intern_real(decimal_prefix(arguments[0]), 64);
}

static Value offer_make_float64(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind == KIND_REAL) return intern_real(arguments[0].real, 64);
    expect_kind(arguments[0], KIND_INTEGER, "make_float64 expects a number");
    return intern_real((double)arguments[0].integer, 64);
}

static Value offer_make_float32(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind == KIND_REAL) return intern_real(arguments[0].real, 32);
    expect_kind(arguments[0], KIND_INTEGER, "make_float32 expects a number");
    return intern_real((double)arguments[0].integer, 32);
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
    if (!isfinite(number) || number < (double)LLONG_MIN || number >= -(double)LLONG_MIN) fail("float_to_int is outside the native signed integer range");
    return integer_value((long long)number);
}

static Value offer_intern_node(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return node_from_cell(composite_cell(arguments[LEFT_OPERAND], arguments[RIGHT_OPERAND]));
}

static Value offer_node_children(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_NODE, "node_children expects a node");
    Cell *cell = arguments[SOLE_OPERAND].cell;
    if (cell == NULL || cell->child_count == EMPTY_COUNT) return list_from(NULL, EMPTY_COUNT);
    return list_from(cell->children, cell->child_count);
}

static Value offer_node_category(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    if (arguments[SOLE_OPERAND].kind != KIND_NODE) {
        fail(message_with_name("node_category expects a node, received ", kind_name(arguments[SOLE_OPERAND].kind), strlen(kind_name(arguments[SOLE_OPERAND].kind))));
    }
    if (arguments[SOLE_OPERAND].cell == NULL || arguments[SOLE_OPERAND].cell->kind != CELL_COMPOSITE) return arguments[SOLE_OPERAND];
    return arguments[SOLE_OPERAND].cell->category_value;
}

static Value offer_node_payload(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[SOLE_OPERAND].kind == KIND_NOTHING) return nothing_value();
    expect_kind(arguments[SOLE_OPERAND], KIND_NODE, "node_value expects a node");
    Cell *cell = arguments[SOLE_OPERAND].cell;
    if (cell == NULL) {
        fk_node_id coordinate = (fk_node_id)arguments[SOLE_OPERAND].integer;
        if (fk_nid_get(coordinate, 0) == 1 && fk_nid_get(coordinate, 1) == 1 &&
            (fk_nid_get(coordinate, 2) == 1 || fk_nid_get(coordinate, 2) == 3)) return integer_value(fk_nid_get(coordinate, 2) == 1 ? (int32_t)fk_nid_get(coordinate, 3) : fk_nid_get(coordinate, 3));
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
    form->path = source_path;
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

static Form **parse_source(size_t *form_count) {
    return parse_source_span(form_count, 0);
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
    function.body = form->parts[DEFN_BODY];
    function.parameter_count = parameters->part_count;
    function.parameters = allocate_zeroed(parameters->part_count, sizeof *function.parameters);
    for (size_t index = FIRST_INDEX; index < parameters->part_count; index = checked_add(index, ONE_ELEMENT)) {
        function.parameters[index].bytes = parameters->parts[index]->text;
        function.parameters[index].length = parameters->parts[index]->text_length;
    }
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
    name_index_set(&function_names, name->text, name->text_length, function_count);
    function_count = needed;
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

static Value offer_math_sqrt(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return real_value(sqrt(number_as_real(arguments[0])));
}

static Value offer_math_exp(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return real_value(exp(number_as_real(arguments[0])));
}

static Value offer_math_log(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return real_value(log(number_as_real(arguments[0])));
}

static Value offer_math_pow(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return real_value(pow(number_as_real(arguments[0]), number_as_real(arguments[1])));
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

static Value offer_value_str(Value *arguments, size_t argument_count);
static void print_value_to(Value root, FILE *output);

static Value offer_str(Value *arguments, size_t argument_count) {
    return offer_value_str(arguments, argument_count);
}

static Value offer_print_str(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_TEXT, "print_str expects text");
    if (arguments[SOLE_OPERAND].text_length != EMPTY_COUNT) {
        fwrite(arguments[SOLE_OPERAND].text, sizeof(char), arguments[SOLE_OPERAND].text_length, stdout);
    }
    fputc(NEWLINE, stdout);
    fflush(stdout);
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
    if (tail.list->dotted_tail != NULL) return *tail.list->dotted_tail;
    tail.list = tail.list->tail;
    tail.item_count -= ONE_ELEMENT;
    return tail;
}

static Value offer_cons(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[RIGHT_OPERAND].kind != KIND_LIST) {
        Value pair = list_from(arguments, 1);
        pair.list->dotted_tail = allocate_zeroed(1, sizeof(Value));
        *pair.list->dotted_tail = arguments[RIGHT_OPERAND];
        return pair;
    }
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

static Value offer_record_keys(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_RECORD, "record_keys expects a record");
    Record *record = arguments[0].record;
    return list_from(record->keys, record->count);
}

static Value offer_record_new(Value *arguments, size_t argument_count) {
    if (argument_count == EMPTY_COUNT) fail("record_new expects a blueprint or a size");
    size_t fields = argument_count - ONE_ELEMENT;
    if ((fields % ARITY_TWO) != EMPTY_COUNT) fail("record_new expects key and value pairs");
    Value created = record_value(allocate_zeroed(ONE_ELEMENT, sizeof(Record)));
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

static Value offer_record_type(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return boolean_word(arguments[0].kind == KIND_RECORD);
}

static Value offer_record_blueprint(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[0], KIND_RECORD, "record_blueprint expects a record");
    return arguments[0].record->blueprint;
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
    if (value.kind == KIND_NOTHING) return text_value("nothing", 7);
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
    return boolean_word(kill(pid, 0) == SYSTEM_CALL_OK || errno == EPERM);
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

static Value offer_file_mtime(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value path = arguments[0];
    if (path.kind != KIND_TEXT || memchr(path.text, 0, path.text_length) != NULL) return integer_value(-1);
    struct stat observed;
    return stat(path.text, &observed) == 0 ? integer_value((long long)observed.st_mtime) : integer_value(-1);
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
    if (found != SYSTEM_CALL_OK) return integer_value(-WORD_TRUE);
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
    int fd = open(arguments[0].text, O_RDONLY | O_NONBLOCK);
    if (fd < 0) return integer_value(0);
    close(fd);
    return integer_value(1);
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
    if (arguments[0].kind != KIND_TEXT || arguments[1].kind != KIND_INTEGER ||
        memchr(arguments[0].text, 0, arguments[0].text_length) != NULL ||
        arguments[1].integer < 0 || arguments[1].integer > 07777) return integer_value(-2);
    if (mkdir(arguments[0].text, (mode_t)arguments[1].integer) != 0) return integer_value(0);
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
} Publication;

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

static int publication_matches(int parent, const char *name, Value expected, int *released) {
    int fd = openat(parent, name, O_RDONLY | O_NONBLOCK);
    if (fd < 0) return -1;
    struct stat info;
    int stated = fstat(fd, &info), saved = errno;
    if (stated != 0 || !S_ISREG(info.st_mode)) {
        int closed = close(fd);
        if (closed != 0) *released = 0;
        if (stated != 0 && closed == 0) errno = saved;
        return stated != 0 || closed != 0 ? -1 : -2;
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
    if (close(fd) != 0) { *released = 0; return -1; }
    errno = saved;
    return match;
}

static Publication publish_path(const char *from, const char *to, Value expected) {
    Publication result = {0, 0, 1, 0, "publication-unavailable"};
    const char *from_name, *to_name;
    int lock = publication_parent(to, &to_name);
    if (lock < 0) { result.reason = "publication-directory-unavailable"; result.error = errno; return result; }
    int source = publication_parent(from, &from_name);
    if (source < 0) {
        result.reason = "publication-stage-directory-unavailable"; result.error = errno;
        result.released = close(lock) == 0; return result;
    }
    int locked;
    do { locked = flock(lock, LOCK_EX); } while (locked != 0 && errno == EINTR);
    if (locked != 0) {
        result.reason = "publication-lock-unavailable"; result.error = errno;
        int a = close(source), b = close(lock);
        result.released = a == 0 && b == 0; return result;
    }
    int comparison_released = 1, match;
    if (expected.kind == KIND_NOTHING) {
        struct stat info;
        int present = fstatat(lock, to_name, &info, AT_SYMLINK_NOFOLLOW);
        match = present == 0 ? 0 : errno == ENOENT ? 1 : -1;
    } else match = publication_matches(lock, to_name, expected, &comparison_released);
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
    int source_closed = close(source), source_error = errno;
    int closed = close(lock);
    result.released = comparison_released && source_closed == 0 && closed == 0;
    if (!result.released) {
        result.error = closed != 0 ? errno : source_closed != 0 ? source_error : comparison_error;
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
    Value fields[] = {integer_value(0), text_value("published", 9), integer_value(result.published),
        text_value("matched", 7), integer_value(result.matched), text_value("released", 8), integer_value(result.released),
        text_value("os_error", 8), integer_value(result.error), text_value("reason", 6), text_value(result.reason, strlen(result.reason))};
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

/* Form owns FORMBIN framing and node traversal. These two scalar modes only
 * transport representation bits; no C serializer or second wire authority. */
static Value offer_float_leaf(Value *arguments, size_t argument_count) {
    (void)argument_count;
    if (arguments[0].kind != KIND_INTEGER) return nothing_value();
    long long mode = arguments[0].integer;
    Value value = arguments[1];
    if (mode >= 29 && mode <= 32) return stage_door(mode, value);
    if (mode == 0) return offer_float_value(&value, 1);
    if (mode == 1) return offer_make_float32(&value, 1);
    if (mode == 2) return offer_make_float64(&value, 1);
    if (mode == 3) return real_value(3.14159265358979323846264338327950288);
    if (mode == 4) return offer_value_kind(&value, 1);
    if ((mode != 34 && mode != 35) || value.kind != KIND_LIST || value.item_count != 2) return nothing_value();
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
        return offer_intern_trivial_int(&scalar, 1);
    }
    double number;
    memcpy(&number, &bits, sizeof number);
    return intern_real(number, 64);
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
    return text_value(source_path, strlen(source_path));
}

static Value offer_self_source(Value *arguments, size_t argument_count) {
    (void)arguments; (void)argument_count;
    return text_value(entry_source_bytes != NULL ? entry_source_bytes : "", entry_source_length);
}

static Value offer_realpath(Value *arguments, size_t argument_count) {
    (void)argument_count;
    Value path = arguments[0];
    if (path.kind != KIND_TEXT || memchr(path.text, 0, path.text_length) != NULL) return nothing_value();
    char *resolved = realpath(path.text, NULL);
    if (resolved == NULL) return nothing_value();
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
    if (arguments[RECORD_WORD].kind != KIND_NODE) {
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
    if (key == 164) {
        const Operation *constructor = find_operation_text("record_new", 10);
        return constructor == NULL ? nothing_value() : count_word(constructor->calls);
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
    if (key == STAT_CELLS || key == STAT_NODES_GAS || key == STAT_NODES_MINTED) return count_word(cell_count);
    if (key == STAT_STRINGS) return count_word(text_cell_count());
    if (key == STAT_CONS || key == STAT_FLOATS || key == STAT_JIT_BOXES || key == STAT_JIT_UNBOXES || key == STAT_JIT_NATIVE || key == STAT_JIT_CRYSTALLIZED || key == STAT_JIT_UNROLL || key == STAT_JIT_LOOP_ITERATIONS || key == STAT_FLOAT_CAPACITY || key == STAT_FIELD_FLOATS || key == STAT_NODES_WATER || key == STAT_NODES_ICE || key == STAT_NODE_LOGICAL_BYTES) return integer_value(WORD_FALSE);
    if (key == STAT_STACK) return count_word(frame_count);
    if (key == STAT_RECIPES) return count_word(function_count);
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

static int kernel_page_state(Value pid) {
    if (pid.kind != KIND_INTEGER || pid.integer <= 0 || pid.integer > INT_MAX) return 0;
    char name[LIVE_NAME_ROOM];
    if (!live_page_name(name, sizeof name, pid.integer)) return 0;
    int fd = shm_open(name, O_RDONLY, GIFT_OPEN_MODE);
    if (fd < 0) return errno == ENOENT ? 0 : -1;
    long long seen = unix_milliseconds();
    int gone = offer_host_alive(&pid, 1).integer == 0;
    int64_t words[LIVE_PAGE_WORDS];
    struct stat extent;
    if (fstat(fd, &extent) != 0) { close(fd); return -1; }
    size_t got = extent.st_size < (off_t)sizeof words ? 0 : sizeof words;
    if (got != 0) {
        void *mapped = mmap(NULL, sizeof words, PROT_READ, MAP_SHARED, fd, 0);
        if (mapped == MAP_FAILED) { close(fd); return -1; }
        memcpy(words, mapped, sizeof words);
        if (munmap(mapped, sizeof words) != 0) { close(fd); return -1; }
    }
    int released = close(fd) == 0;
    if (!released) return -1;
    if (gone && (got < sizeof words ||
        (words[LIVE_PID] == pid.integer && (words[LIVE_ALIVE] == 0 || words[LIVE_START_MS] < seen))) &&
        offer_host_alive(&pid, 1).integer == 0) return 1;
    fd = shm_open(name, O_RDONLY, GIFT_OPEN_MODE);
    if (fd < 0) return errno == ENOENT ? 0 : -1;
    return close(fd) == 0 ? 2 : -1;
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
    int buried = same && offer_host_alive(&pid, 1).integer == 0 && shm_unlink(name) == 0;
    int released_latest = latest < 0 || close(latest) == 0;
    int released_first = close(first) == 0;
    if (buried) stage_lastbreath(pid.integer);
    return integer_value(buried && released_latest && released_first);
}

static Value offer_kernel_rows_absent(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "a hot-row count is an integer");
    return list_from(NULL, EMPTY_COUNT);
}

/* The carrier owns GPU objects; this bootstrap bridge only transports Values.
 * Form's metal-carrier.bml / metal-jit.bml own admission and execution policy. */
static struct {
    void *handle;
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
} metal;

static const char *runtime_executable;

static int metal_ready(void) {
    if (metal.handle != NULL) return 1;
    const char *selected = getenv("FKWU_METAL_CARRIER");
    char *path = NULL;
    if (selected == NULL || *selected == 0) {
        char *executable = NULL;
#ifdef __APPLE__
        uint32_t size = 0;
        _NSGetExecutablePath(NULL, &size);
        executable = allocate_zeroed(size, 1);
        if (_NSGetExecutablePath(executable, &size) != 0) { free(executable); return 0; }
#else
        executable = realpath(runtime_executable, NULL);
        if (executable == NULL) return 0;
#endif
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
    metal.handle = handle;
    return 1;
}

static Value metal_count(long long count) { return integer_value(count < 0 ? 0 : count); }

static Value offer_metal_pipeline(Value *a, size_t n) {
    (void)n;
    if (a[0].kind != KIND_TEXT || a[1].kind != KIND_TEXT || !metal_ready()) return integer_value(0);
    return metal_count(metal.pipeline(a[0].text, (long long)a[0].text_length, a[1].text, (long long)a[1].text_length));
}

static Value offer_metal_buf_alloc(Value *a, size_t n) {
    (void)n;
    return a[0].kind == KIND_INTEGER && metal_ready() ? metal_count(metal.buf_alloc(a[0].integer)) : integer_value(0);
}

static Value offer_metal_buf_from_file(Value *a, size_t n) {
    (void)n;
    if (a[0].kind != KIND_TEXT || a[1].kind != KIND_INTEGER || a[2].kind != KIND_INTEGER || !metal_ready()) return integer_value(0);
    return metal_count(metal.buf_from_file(a[0].text, (long long)a[0].text_length, a[1].integer, a[2].integer));
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
static Value offer_metal_buf_free(Value *a, size_t n) { (void)n; return a[0].kind == KIND_INTEGER && metal_ready() ? metal_count(metal.buf_free(a[0].integer)) : integer_value(0); }
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
static Value offer_socket_listen(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "socket_listen expects a port");
    if (arguments[0].integer < 0 || arguments[0].integer > 65535) return integer_value(-1);
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
    if (arguments[1].integer < 0 || arguments[1].integer > 65535 || memchr(arguments[0].text, 0, arguments[0].text_length) != NULL) return integer_value(-1);
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

static void register_builtin_operations(void) {
    register_operation("socket_listen", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_listen);
    register_operation("socket_port", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_port);
    register_operation("socket_accept", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_accept);
    register_operation("socket_connect", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_connect);
    register_operation("socket_send", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_send);
    register_operation("socket_recv", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_recv);
    register_operation("socket_close", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_socket_close);
    register_operation("if", ARITY_THREE, FIXED_ARITY, LEAVE_ARGUMENTS, OP_IF, NULL);
    register_operation("do", ARITY_NONE, VARIADIC, LEAVE_ARGUMENTS, OP_DO, NULL);
    register_operation("and", ARITY_NONE, VARIADIC, LEAVE_ARGUMENTS, OP_AND, NULL);
    register_operation("list", ARITY_NONE, VARIADIC, EVALUATE_ARGUMENTS, OP_LIST, offer_list);
    register_operation("add", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_ADD, offer_add);
    register_operation("sub", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_SUB, offer_sub);
    register_operation("eq", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_EQ, offer_eq);
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
    register_operation("read_line", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_READ_LINE, offer_read_line);
    register_operation("file_mtime", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FILE_MTIME, offer_file_mtime);
    register_operation("cell_path", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_CELL_PATH, offer_cell_path);
    register_operation("self_source", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_self_source);
    register_operation("host_realpath", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_realpath);
    register_operation("fs_truncate", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_fs_truncate);
    register_operation("intern_trivial_bool", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_INTERN_BOOL, offer_intern_bool);
    register_operation("kernel_stat", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_stat);
    register_operation("kernel_live_pids", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_live_pids);
    register_operation("kernel_live", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_live);
    register_operation("kernel_page_ended", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_kernel_page_ended);
    register_operation("kernel_page_bury", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_kernel_page_bury);
    register_operation("kernel_page_hot", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_page_hot);
    register_operation("kernel_hot_rows", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_rows_absent);
    register_operation("kernel_box_rows", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_rows_absent);
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
    register_operation("print", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_print);
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
    /* bp carries a symbolic category; fol-bp resolves the admitted coordinate registry. */
    register_operation("bp", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_intern_trivial_string);
    operation_answers("bp");
    register_operation("intern_trivial_int", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_intern_trivial_int);
    register_operation("intern_trivial_float", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_intern_trivial_float);
    register_operation("make_float64", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_make_float64);
    register_operation("float_value", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_float_value);
    register_operation("float_to_int", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_float_to_int);
    register_operation("str_to_float", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_str_to_float);
    register_operation("intern_node", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_intern_node);
    register_operation("intern_node_at", ARITY_FIVE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_intern_node_at);
    register_operation("node_children", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_children);
    register_operation("node_at", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_PRINT, offer_node_at);
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
    if (left.kind == KIND_FUNCTION || right.kind == KIND_FUNCTION) fail("function identity is not compared");
    if (left.kind != right.kind) return 0;
    if (left.kind == KIND_NOTHING) return 1;
    if (left.kind == KIND_INTEGER) return left.integer == right.integer;
    if (left.kind == KIND_NODE) return left.integer == right.integer;
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
    frames[frame_count].stage_base = stage_depth;
    frame_count = needed;
    return &frames[frame_count - ONE_ELEMENT];
}

static void pop_frame(void) {
    Frame *frame = &frames[frame_count - ONE_ELEMENT];
    if (frame->kind == FRAME_LET && frame->stage == 1) local_count = frame->local_base;
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
    size_t global = name_index_find(&global_names, bytes, length);
    if (global != SIZE_MAX) {
        *out = globals[global].value;
        return 1;
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
static void append_local(const char *bytes, size_t length, Value value);
static Value closure_value(Form *parameters, Form *body, size_t offset);

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
        if (form->is_bound) {
            deliver(nothing_value());
            return;
        }
        if (form->part_count != DEFN_PART_COUNT || form->parts[DEFN_NAME]->kind != FORM_SYMBOL) fail_at(form->offset, "a local definition needs a name, parameters, and a body");
        Form *name = form->parts[DEFN_NAME];
        Value function = closure_value(form->parts[DEFN_PARAMETERS], form->parts[DEFN_BODY], form->offset);
        size_t captured = function.capture_count;
        Binding *captures = allocate_zeroed(checked_add(captured, 1), sizeof *captures);
        if (captured != 0) memcpy(captures, function.captures, captured * sizeof *captures);
        free(function.captures);
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
    const Operation *operation = find_operation_text(head->text, head->text_length);
    Binding *function = find_function_text(head->text, head->text_length);
    if (operation != NULL && (operation->evaluation == LEAVE_ARGUMENTS || operation->answers)) function = NULL;
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

static void step_fail(size_t index);

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
    name_index_set(&global_names, bytes, length, global_count);
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

static Value closure_value(Form *parameters, Form *body, size_t offset) {
    if (parameters->kind != FORM_GROUP) fail_at(parameters->offset, "a lambda parameter list must be a group");
    Value function = nothing_value();
    function.kind = KIND_FUNCTION;
    function.body = body;
    function.parameter_count = parameters->part_count;
    function.parameters = allocate_zeroed(parameters->part_count, sizeof *function.parameters);
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
        Binding *captures = allocate_zeroed(total, sizeof *captures);
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
        fprintf(stderr, ERROR_BARE, failure_message);
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
    if (failure_reported) { note_stack(); exit(EXIT_FAILURE); }
    fail(failure_message != NULL ? failure_message : "a failure was not caught");
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

static void print_atom(Value value, FILE *output) {
    if (value.kind == KIND_NOTHING) { fputs("nothing", output); return; }
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
        fprintf(output, REAL_FORMAT, DBL_DECIMAL_DIG, value.real);
        return;
    }
    if (value.kind == KIND_TEXT) {
        fwrite(value.text, sizeof *value.text, value.text_length, output);
        return;
    }
    fputs(kind_name(value.kind), output);
}

static void print_value_to(Value root, FILE *output) {
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
    print_value_to(root, stdout);
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
        if (form->kind == FORM_SYMBOL && find_function_text(form->text, form->text_length) == NULL && find_operation_text(form->text, form->text_length) == NULL) {
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

static Form **parse_bml_source(size_t *form_count) {
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

static int source_has_section(void) {
    int quoted = 0, escaped = 0, line_comment = 0, block_comment = 0, line_start = 1;
    size_t word_length = strlen(SECTION_WORD);
    for (size_t i = 0; i < source_length; ++i) {
        char c = source_bytes[i];
        char next = i + 1 < source_length ? source_bytes[i + 1] : 0;
        if (line_comment) {
            if (c == '\n') { line_comment = 0; line_start = 1; }
            continue;
        }
        if (block_comment) {
            if (c == '*' && next == '/') { block_comment = 0; ++i; }
            if (c == '\n') line_start = 1;
            continue;
        }
        if (quoted) {
            if (escaped) escaped = 0;
            else if (c == '\\') escaped = 1;
            else if (c == TEXT_DELIMITER) quoted = 0;
            if (c == '\n') line_start = 1;
            else line_start = 0;
            continue;
        }
        if (c == COMMENT_INTRODUCER || (c == '/' && next == '/')) { line_comment = 1; continue; }
        if (c == '/' && next == '*') { block_comment = 1; ++i; continue; }
        if (c == TEXT_DELIMITER) { quoted = 1; line_start = 0; continue; }
        if (c == '\n') { line_start = 1; continue; }
        if (isspace((unsigned char)c)) continue;
        if (line_start && source_length - i >= word_length &&
            memcmp(source_bytes + i, SECTION_WORD, word_length) == 0) {
            size_t after = i + word_length;
            while (after < source_length && isspace((unsigned char)source_bytes[after])) ++after;
            if (after < source_length && source_bytes[after] == INDEX_OPEN) return 1;
        }
        line_start = 0;
    }
    return 0;
}

static Form **parse_current_source(size_t *form_count) {
    if (source_has_section()) return parse_bml_source(form_count);
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
    link_home_names(forms, form_count, path);
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
    setvbuf(stdin, NULL, _IONBF, 0);
    runtime_executable = argv[0];
    process_born_ms = unix_milliseconds();
    if (argc < PROGRAM_ARGUMENT_COUNT) {
        fputs(USAGE_TEXT, stderr);
        return EXIT_FAILURE;
    }
    register_builtin_operations();
    source_path = argv[SOURCE_PATH_ARGUMENT];
    source_bytes = read_source(source_path, &source_length);
    entry_source_bytes = source_bytes;
    entry_source_length = source_length;
    remember_unit(copy_bytes(source_path, strlen(source_path)));
    load_preludes(source_bytes, source_length, source_path);
    source_offset = FIRST_INDEX;
    size_t form_count = EMPTY_COUNT;
    Form **forms = parse_current_source(&form_count);
    bind_definitions(forms, form_count);
    link_home_names(forms, form_count, source_path);
    drive_forms(forms, form_count);
    print_value(finished);
    fputs(LINE_ENDING, stdout);
    if (fflush(stdout) != SYSTEM_CALL_OK || ferror(stdout)) fail("the result could not be written");
    return EXIT_SUCCESS;
}
