/* The Form runtime reads one source file and offers its forms.
 * A word is nothing, an integer, a real, text, a list, a function, or a node.
 * Definitions are bound before any form runs, so a later definition is visible.
 * Words live until the process exits. Collections grow with the data.
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

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
static const char *const LIST_OPEN = "[";
static const char *const LIST_SEPARATOR = ", ";
static const char *const LIST_CLOSE = "]";
static const char *const LINE_ENDING = "\n";
static const char *const SOURCE_OPEN_MODE = "rb";
static const char *const INTEGER_FORMAT = "%lld";
static const char *const REAL_FORMAT = "%.*g";
static const char *const ERROR_FORMAT = "fkwu: %s at byte %zu\n";
static const char *const ERROR_BARE = "fkwu: %s\n";
static const char *const USAGE_TEXT = "usage: fkwu-rewrite SOURCE\n";

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
    KIND_NODE
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
    OP_BUS_READ
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
    LEFT_OPERAND = 0,
    RIGHT_OPERAND = 1,
    SOLE_OPERAND = 0,
    ARITY_NONE = 0,
    ARITY_ONE = 1,
    ARITY_TWO = 2,
    ARITY_THREE = 3,
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
    STAT_SHARED_MAPS = 41,
    STAT_SHARED_BYTES = 42,
    STAT_ROOTS_STANDING = 54,
    STAT_ROOT_CAPACITY = 55,
    STAT_NODE_IDENTITY_BYTES = 63,
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
};

typedef enum {
    FRAME_SEQUENCE,
    FRAME_DO,
    FRAME_AND,
    FRAME_IF,
    FRAME_EAGER,
    FRAME_FUNCTION
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

static int shared_descriptor = INVALID_DESCRIPTOR;
static uint64_t *shared_header;
static size_t shared_mapped_bytes;
static int shared_unavailable;
static size_t shared_maps;
static size_t shared_bytes;

static void fail(const char *reason) {
    fprintf(stderr, ERROR_BARE, reason);
    exit(EXIT_FAILURE);
}

static void fail_at(size_t offset, const char *reason) {
    fprintf(stderr, ERROR_FORMAT, reason, offset);
    exit(EXIT_FAILURE);
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
        .parameter_count = EMPTY_COUNT
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

static Value node_value(long long identity) {
    Value value = nothing_value();
    value.kind = KIND_NODE;
    value.integer = identity;
    return value;
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
        if (source_bytes[source_offset] != COMMENT_INTRODUCER) return;
        source_offset = checked_add(source_offset, ONE_ELEMENT);
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

static Form *parse_text(void) {
    size_t opening = source_offset;
    source_offset = checked_add(source_offset, ONE_ELEMENT);
    size_t start = source_offset;
    while (source_offset < source_length && source_bytes[source_offset] != TEXT_DELIMITER) {
        source_offset = checked_add(source_offset, ONE_ELEMENT);
    }
    if (source_offset >= source_length) fail_at(opening, "a text value was not closed");
    size_t length = source_offset - start;
    Form *form = new_form(FORM_TEXT, opening);
    form->text = copy_bytes(source_bytes + start, length);
    form->text_length = length;
    source_offset = checked_add(source_offset, ONE_ELEMENT);
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
    operation_count = needed;
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
    if (find_operation_text(name->text, name->text_length) != NULL) {
        fail_at(name->offset, "a definition uses a reserved operation name");
    }
    if (find_function_text(name->text, name->text_length) != NULL) fail_at(name->offset, "a definition name is already bound");
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
    size_t needed = checked_add(function_count, ONE_ELEMENT);
    functions = grow_list(functions, &function_capacity, sizeof *functions, needed);
    functions[function_count].bytes = name->text;
    functions[function_count].length = name->text_length;
    functions[function_count].value = function;
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
    return boolean_word(values_equal(arguments[LEFT_OPERAND], arguments[RIGHT_OPERAND]));
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

static Value offer_str_eq(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "str_eq expects text");
    expect_kind(arguments[RIGHT_OPERAND], KIND_TEXT, "str_eq expects text");
    return boolean_word(bytes_equal(arguments[LEFT_OPERAND].text, arguments[LEFT_OPERAND].text_length,
                                    arguments[RIGHT_OPERAND].text, arguments[RIGHT_OPERAND].text_length));
}

static Value offer_str_concat(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[LEFT_OPERAND], KIND_TEXT, "str_concat expects text");
    expect_kind(arguments[RIGHT_OPERAND], KIND_TEXT, "str_concat expects text");
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

static Value offer_nothing(Value *arguments, size_t argument_count) {
    (void)argument_count;
    return boolean_word(arguments[SOLE_OPERAND].kind == KIND_NOTHING);
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
    return node_value(integer_is_nonzero(arguments[SOLE_OPERAND]) ? BOOLEAN_NODE_TRUE : BOOLEAN_NODE_FALSE);
}

static Value count_word(size_t count) {
    if (count > (size_t)LLONG_MAX) fail("a counter exceeds the native integer range");
    return integer_value((long long)count);
}

static Value list_from(Value *items, size_t item_count) {
    Value value = nothing_value();
    value.kind = KIND_LIST;
    value.items = items;
    value.item_count = item_count;
    return value;
}

static Value offer_fb_record(Value *arguments, size_t argument_count) {
    (void)argument_count;
    frame_entered = checked_add(frame_entered, ONE_ELEMENT);
    frame_last_index = frame_entered;
    if (arguments[RECORD_WORD].kind == KIND_FUNCTION) {
        frame_rejected = checked_add(frame_rejected, ONE_ELEMENT);
        return nothing_value();
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
    return count_word(frame_record_count - ONE_ELEMENT);
}

static Value offer_fb_events(Value *arguments, size_t argument_count) {
    (void)arguments;
    (void)argument_count;
    Value *records = allocate_zeroed(frame_record_count, sizeof *records);
    for (size_t index = FIRST_INDEX; index < frame_record_count; index = checked_add(index, ONE_ELEMENT)) {
        Value *fields = allocate_zeroed(ARITY_THREE, sizeof *fields);
        fields[RECORD_WORD] = frame_records[index].word;
        fields[RECORD_PATH] = text_value(frame_records[index].path, frame_records[index].path_length);
        fields[RECORD_POSITION] = integer_value(frame_records[index].position);
        records[index] = list_from(fields, ARITY_THREE);
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

static Value offer_kernel_stat(Value *arguments, size_t argument_count) {
    (void)argument_count;
    expect_kind(arguments[SOLE_OPERAND], KIND_INTEGER, "kernel_stat expects an integer key");
    long long key = arguments[SOLE_OPERAND].integer;
    if (key == STAT_ROOTS_SINCE_CLEAR) return count_word(frame_since_clear);
    if (key == STAT_REJECTED) return count_word(frame_rejected);
    if (key == STAT_ENTERED) return count_word(frame_entered);
    if (key == STAT_ACCEPTED) return count_word(frame_accepted);
    if (key == STAT_LAST_INDEX) return count_word(frame_last_index);
    if (key == STAT_SHARED_MAPS) return count_word(shared_maps);
    if (key == STAT_SHARED_BYTES) return count_word(shared_bytes);
    if (key == STAT_ROOTS_STANDING) return count_word(frame_record_count);
    if (key == STAT_ROOT_CAPACITY) return count_word(frame_record_capacity);
    if (key == STAT_NODE_IDENTITY_BYTES) return integer_value((long long)sizeof(NodeIdentity));
    return nothing_value();
}

static Value offer_list(Value *arguments, size_t argument_count) {
    Value value = nothing_value();
    value.kind = KIND_LIST;
    value.items = arguments;
    value.item_count = argument_count;
    return value;
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
    register_operation("read_line", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_READ_LINE, offer_read_line);
    register_operation("file_mtime", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FILE_MTIME, offer_file_mtime);
    register_operation("cell_path", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_CELL_PATH, offer_cell_path);
    register_operation("intern_trivial_bool", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_INTERN_BOOL, offer_intern_bool);
    register_operation("kernel_stat", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_KERNEL_STAT, offer_kernel_stat);
    register_operation("fb_record", ARITY_THREE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FB_RECORD, offer_fb_record);
    register_operation("framebuffer-events", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FB_EVENTS, offer_fb_events);
    register_operation("framebuffer-clear", ARITY_NONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_FB_CLEAR, offer_fb_clear);
    register_operation("bus_open", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_BUS_OPEN, offer_bus_open);
    register_operation("bus_close", ARITY_TWO, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_BUS_CLOSE, offer_bus_close);
    register_operation("bus_read", ARITY_ONE, FIXED_ARITY, EVALUATE_ARGUMENTS, OP_BUS_READ, offer_bus_read);
}

typedef struct {
    Value left;
    Value right;
    size_t index;
    int entered;
} EqualPair;

static int atom_equal(Value left, Value right) {
    if (left.kind == KIND_FUNCTION || right.kind == KIND_FUNCTION) fail("function identity is not compared");
    if (left.kind != right.kind) return 0;
    if (left.kind == KIND_NOTHING) return 1;
    if (left.kind == KIND_INTEGER || left.kind == KIND_NODE) return left.integer == right.integer;
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
    pairs[pair_count].index = FIRST_INDEX;
    pairs[pair_count].entered = 0;
    pair_count = needed;
    while (pair_count != EMPTY_COUNT) {
        EqualPair pair = pairs[pair_count - ONE_ELEMENT];
        if (pair.left.kind == KIND_LIST || pair.right.kind == KIND_LIST) {
            if (pair.left.kind != KIND_LIST || pair.right.kind != KIND_LIST) return 0;
            if (pair.left.item_count != pair.right.item_count) return 0;
            if (!pair.entered) {
                pair.entered = 1;
                pair.index = FIRST_INDEX;
            }
            if (pair.index == pair.left.item_count) {
                pair_count -= ONE_ELEMENT;
                continue;
            }
            Value child_left = pair.left.items[pair.index];
            Value child_right = pair.right.items[pair.index];
            pair.index = checked_add(pair.index, ONE_ELEMENT);
            pairs[pair_count - ONE_ELEMENT] = pair;
            needed = checked_add(pair_count, ONE_ELEMENT);
            pairs = grow_list(pairs, &pair_capacity, sizeof *pairs, needed);
            pairs[pair_count].left = child_left;
            pairs[pair_count].right = child_right;
            pairs[pair_count].index = FIRST_INDEX;
            pairs[pair_count].entered = 0;
            pair_count = needed;
            continue;
        }
        if (!atom_equal(pair.left, pair.right)) return 0;
        pair_count -= ONE_ELEMENT;
    }
    return 1;
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

static Value lookup_value_binding(const char *bytes, size_t length) {
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
        if (bytes_equal(locals[index].bytes, locals[index].length, bytes, length)) return locals[index].value;
    }
    if (bytes_equal_cstr(bytes, length, NAME_TRUE)) return integer_value(WORD_TRUE);
    if (bytes_equal_cstr(bytes, length, NAME_FALSE)) return integer_value(WORD_FALSE);
    if (bytes_equal_cstr(bytes, length, NAME_NOTHING)) return nothing_value();
    if (find_function_text(bytes, length) != NULL) fail("a function is offered by calling it");
    fail("an unbound name was offered");
    return nothing_value();
}

static Value value_from_atom(Form *form) {
    if (form->kind == FORM_INTEGER) return integer_value(form->integer);
    if (form->kind == FORM_REAL) return real_value(form->real);
    if (form->kind == FORM_TEXT) return text_value(form->text, form->text_length);
    if (form->kind == FORM_SYMBOL) return lookup_value_binding(form->text, form->text_length);
    fail_at(form->offset, "a form has an unknown shape");
    return nothing_value();
}

static void offer_form(Form *form);

static void begin_call(Form *form) {
    if (form->part_count == EMPTY_COUNT) fail_at(form->offset, "a call needs a head");
    Form *head = form->parts[CALL_HEAD];
    if (head->kind != FORM_SYMBOL) fail_at(head->offset, "a call head must be a name");
    if (form_is_symbol(head, NAME_DEFN)) {
        if (form->is_bound) {
            deliver(nothing_value());
            return;
        }
        fail_at(form->offset, "a definition inside a function is not offered");
    }
    size_t argument_count = call_argument_count(form);
    const Operation *operation = find_operation_text(head->text, head->text_length);
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
        else {
            frame->kind = FRAME_EAGER;
            frame->arguments = allocate_zeroed(argument_count, sizeof *frame->arguments);
        }
        return;
    }
    Binding *function = find_function_text(head->text, head->text_length);
    if (function == NULL) fail_at(head->offset, "an unbound name was offered");
    if (argument_count != function->value.parameter_count) fail_at(form->offset, "a function received the wrong number of arguments");
    Frame *frame = append_frame();
    frame->kind = FRAME_EAGER;
    frame->form = form;
    frame->argument_count = argument_count;
    frame->arguments = allocate_zeroed(argument_count, sizeof *frame->arguments);
    frame->callee = function->value;
    return;
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
        frame_count -= ONE_ELEMENT;
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
            frame_count -= ONE_ELEMENT;
            deliver(integer_value(WORD_FALSE));
            return;
        }
        frame.next_part = checked_add(frame.next_part, ONE_ELEMENT);
        frames[index] = frame;
        return;
    }
    if (frame.next_part >= call_argument_count(frame.form)) {
        frame_count -= ONE_ELEMENT;
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
        frame_count -= ONE_ELEMENT;
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
        Value result = frame.operation->offer(frame.arguments, frame.argument_count);
        frame_count -= ONE_ELEMENT;
        deliver(result);
        return;
    }
    Form *child = call_argument(frame.form, frame.next_part);
    frames[index] = frame;
    offer_form(child);
}

static void step_function(size_t index) {
    Frame frame = frames[index];
    if (!frame.has_incoming) fail("a function body was not offered");
    Value result = frame.incoming;
    local_count = frame.local_base;
    frame_count -= ONE_ELEMENT;
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
        else if (kind == FRAME_EAGER) step_eager(index);
        else if (kind == FRAME_FUNCTION) step_function(index);
        else fail("an evaluation frame has an unknown kind");
    }
    if (!result_ready) fail("the program produced no result");
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
        Value child = item.value.items[item.index];
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

int main(int argc, char **argv) {
    if (argc != PROGRAM_ARGUMENT_COUNT) {
        fputs(USAGE_TEXT, stderr);
        return EXIT_FAILURE;
    }
    register_builtin_operations();
    source_path = argv[SOURCE_PATH_ARGUMENT];
    source_bytes = read_source(source_path, &source_length);
    size_t form_count = EMPTY_COUNT;
    Form **forms = parse_source(&form_count);
    bind_definitions(forms, form_count);
    Form *program = new_form(FORM_GROUP, FIRST_INDEX);
    program->parts = forms;
    program->part_count = form_count;
    drive(program);
    print_value(finished);
    fputs(LINE_ENDING, stdout);
    if (fflush(stdout) != SYSTEM_CALL_OK || ferror(stdout)) fail("the result could not be written");
    return EXIT_SUCCESS;
}
