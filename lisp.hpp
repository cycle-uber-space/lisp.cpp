
#ifndef _LISP_HPP_
#define _LISP_HPP_

#ifndef LISP_GLOBAL_API
#define LISP_GLOBAL_API 1
#endif

#ifndef LISP_DEBUG
#define LISP_DEBUG 1
#endif

#ifndef LISP_SYMBOL_NAME_OF_NIL
#define LISP_SYMBOL_NAME_OF_NIL 1
#endif

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include <functional>

#define LISP_RED     "\x1b[31m"
#define LISP_GREEN   "\x1b[32m"
#define LISP_YELLOW  "\x1b[33m"
#define LISP_BLUE    "\x1b[34m"
#define LISP_MAGENTA "\x1b[35m"
#define LISP_CYAN    "\x1b[36m"
#define LISP_RESET   "\x1b[0m"

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef float F32;
typedef double F64;

static_assert(sizeof(U8 ) == 1, "");
static_assert(sizeof(U16) == 2, "");
static_assert(sizeof(U32) == 4, "");
static_assert(sizeof(U64) == 8, "");

static_assert(sizeof(I8 ) == 1, "");
static_assert(sizeof(I16) == 2, "");
static_assert(sizeof(I32) == 4, "");
static_assert(sizeof(I64) == 8, "");

static_assert(sizeof(F32) == 4, "");
static_assert(sizeof(F64) == 8, "");

#ifndef LISP_MALLOC
#define LISP_MALLOC(size) malloc(size)
#endif

#ifndef LISP_REALLOC
#define LISP_REALLOC(ptr, size) realloc(ptr, size)
#endif

#ifndef LISP_FREE
#define LISP_FREE(ptr) free(ptr)
#endif

/* test.h */

typedef struct
{
    int num_tests;
    int num_failed;
    bool show_pass;
    bool exit_on_fail;
} TestState;

#define LISP_TEST_GROUP(test, text) test_group(test, text)
#define LISP_TEST_BEGIN(test)       test_begin(test)
#define LISP_TEST_FINISH(test)      test_finish(test)
#define LISP_TEST_ASSERT(test, exp) do { test_assert_try(test, exp, #exp); } while (0)

void test_begin(TestState * test);
void test_finish(TestState * test);
void test_group(TestState * test, char const * text);
void test_assert_try(TestState * test, bool exp, char const * msg);

/* error.h */

#define LISP_FAIL(...)    error_fail(__VA_ARGS__);
#define LISP_WARN(...)    error_warn(__VA_ARGS__);

#define LISP_ASSERT(x) assert(x)

#if LISP_DEBUG
#define LISP_ASSERT_DEBUG(x) LISP_ASSERT(x)
#else
#define LISP_ASSERT_DEBUG(x)
#endif

void error_fail(char const * fmt, ...);
void error_warn(char const * fmt, ...);

/* expr.h */

typedef U64 Expr;

Expr make_expr(U64 type, U64 data);
U64 expr_type(Expr exp);
U64 expr_data(Expr exp);

enum
{
    TYPE_NIL = 0,
    TYPE_SYMBOL,
    TYPE_CONS,
    TYPE_GENSYM,
    TYPE_STRING,
    TYPE_STREAM,
    TYPE_BUILTIN_SPECIAL,
    TYPE_BUILTIN_FUNCTION,
};

enum
{
    DATA_NIL = 0,
};

typedef struct SystemState SystemState;

/* nil.h */

#define nil 0

inline static bool is_nil(Expr exp)
{
    return exp == nil;
}

/* symbol.h */

#define LISP_SYMBOL_T intern("t")

#define LISP_SYM_QUOTE intern("quote")
#define LISP_SYM_IF intern("if")
#define LISP_SYM_BACKQUOTE intern("backquote")
#define LISP_SYM_UNQUOTE intern("unquote")
#define LISP_SYM_UNQUOTE_SPLICING intern("unquote-splicing")

#define LISP_MAX_SYMBOLS -1
#define LISP_DEF_SYMBOLS 16

typedef struct
{
    U64 num;
    U64 max;
    char ** names;
} SymbolState;

void symbol_init(SymbolState * symbol);
void symbol_quit(SymbolState * symbol);

inline static bool is_symbol(Expr exp)
{
    return expr_type(exp) == TYPE_SYMBOL;
}

Expr lisp_make_symbol(SymbolState * symbol, char const * name);
char const * lisp_symbol_name(SymbolState * symbol, Expr exp);

#if LISP_GLOBAL_API
Expr make_symbol(char const * name);
char const * symbol_name(Expr exp);
#endif

/* cons.h */

#define LISP_MAX_CONSES -1
#define LISP_DEF_CONSES  4

struct Pair
{
    Expr a, b;
};

typedef struct
{
    U64 num;
    U64 max;
    struct Pair * pairs;
} ConsState;

void cons_init(ConsState * cons);
void cons_quit(ConsState * cons);

bool is_cons(Expr exp);

Expr lisp_cons(ConsState * cons, Expr a, Expr b);
Expr lisp_car(ConsState * cons, Expr exp);
Expr lisp_cdr(ConsState * cons, Expr exp);
void lisp_rplaca(ConsState * cons, Expr exp, Expr val);
void lisp_rplacd(ConsState * cons, Expr exp, Expr val);

#if LISP_GLOBAL_API

Expr cons(Expr a, Expr b);
Expr car(Expr exp);
Expr cdr(Expr exp);
void rplaca(Expr exp, Expr val);
void rplacd(Expr exp, Expr val);

inline static Expr caar(Expr exp)
{
    return car(car(exp));
}

inline static Expr cadr(Expr exp)
{
    return car(cdr(exp));
}

inline static Expr cdar(Expr exp)
{
    return cdr(car(exp));
}

inline static Expr cddr(Expr exp)
{
    return cdr(cdr(exp));
}

inline static Expr caddr(Expr exp)
{
    return car(cdr(cdr(exp)));
}

inline static Expr cdddr(Expr exp)
{
    return cdr(cdr(cdr(exp)));
}

inline static Expr cadddr(Expr exp)
{
    return car(cdr(cdr(cdr(exp))));
}

inline static Expr cddddr(Expr exp)
{
    return cdr(cdr(cdr(cdr(exp))));
}

#endif

/* gensym.h */

typedef struct
{
    U64 counter;
} GensymState;

void gensym_init(GensymState * gensym);
void gensym_quit(GensymState * gensym);

bool is_gensym(Expr exp);

Expr lisp_gensym(GensymState * gensym);

/* string.h */

#define LISP_MAX_STRINGS        500000

typedef struct
{
    U64 count;
    char ** values;
} StringState;

void string_init(StringState * string);
void string_quit(StringState * string);

bool is_string(Expr exp);

Expr make_string(char const * str);
char const * string_value(Expr exp);
U64 string_length(Expr exp);

/* stream.h */

#define LISP_MAX_STREAMS 64

typedef struct
{
    FILE * file;
    bool close_on_quit;

    char * buffer;
    size_t size;
    size_t cursor;
} StreamInfo;

typedef struct
{
    U64 num;
    StreamInfo info[LISP_MAX_STREAMS];

    Expr stdin;
    Expr stdout;
    Expr stderr;
} StreamState;

void stream_init(StreamState * stream);
void stream_quit(StreamState * stream);

bool is_stream(Expr exp);

Expr lisp_make_file_input_stream(StreamState * stream, FILE * file, bool close_on_quit);
Expr lisp_make_file_output_stream(StreamState * stream, FILE * file, bool close_on_quit);
Expr lisp_make_string_input_stream(StreamState * stream, char const * str);
Expr lisp_make_buffer_output_stream(StreamState * stream, size_t size, char * buffer);

bool lisp_stream_at_end(StreamState * stream, Expr exp);

void lisp_stream_release(StreamState * stream, Expr exp);

Expr make_file_input_stream_from_path(char const * path);
Expr make_string_input_stream(char const * str);

char stream_peek_char(Expr exp);
void stream_skip_char(Expr exp);

void stream_put_char(Expr exp, char ch);
void stream_put_string(Expr exp, char const * str);
void stream_put_u64(Expr exp, U64 val);
void stream_put_x64(Expr exp, U64 val);

inline static char stream_get_char(Expr exp)
{
    /* TODO inline implementation */
    char const ret = stream_peek_char(exp);
    stream_skip_char(exp);
    return ret;
}

void stream_release(Expr exp);

/* builtin.h */

#define LISP_MAX_BUILTINS 64

typedef std::function<Expr(Expr args, Expr kwargs, Expr env)> BuiltinFun;

typedef struct
{
    char const * name;
    BuiltinFun fun;
} BuiltinInfo;

typedef struct
{
    U64 num;
    BuiltinInfo info[LISP_MAX_BUILTINS];
} BuiltinState;

void builtin_init(BuiltinState * builtin);
void builtin_quit(BuiltinState * builtin);

bool is_builtin_special(Expr exp);
bool is_builtin_function(Expr exp);

inline bool is_builtin(Expr exp)
{
    return is_builtin_special(exp) || is_builtin_function(exp);
}

Expr lisp_make_builtin_special(BuiltinState * builtin, char const * name, BuiltinFun fun);
Expr lisp_make_builtin_function(BuiltinState * builtin, char const * name, BuiltinFun fun);

char const * lisp_builtin_name(BuiltinState * builtin, Expr exp);
BuiltinFun lisp_builtin_fun(BuiltinState * builtin, Expr exp);

Expr make_builtin_special(char const * name, BuiltinFun fun);
Expr make_builtin_function(char const * name, BuiltinFun fun);

/* reader.h */

#ifndef LISP_READER_PARSE_QUOTE
#define LISP_READER_PARSE_QUOTE 1
#endif

bool lisp_maybe_parse_expr(SystemState * system, Expr in, Expr * exp);

Expr lisp_read_one_from_string(SystemState * system, char const * src);

#if LISP_GLOBAL_API
bool maybe_parse_expr(Expr in, Expr * exp);
Expr read_one_from_string(char const * src);
#endif

/* printer.h */

#ifndef LISP_PRINTER_RENDER_QUOTE
#define LISP_PRINTER_RENDER_QUOTE 1
#endif

void render_expr(Expr exp, Expr out);

/* util.h */

char * get_temp_buf(size_t size);

bool is_named_call(Expr exp, Expr name);

inline static bool eq(Expr a, Expr b)
{
    return a == b;
}

bool equal(Expr a, Expr b);

char const * repr(Expr exp);
void println(Expr exp);

Expr intern(char const * name);

Expr list_1(Expr exp1);
Expr list_2(Expr exp1, Expr exp2);
Expr list_3(Expr exp1, Expr exp2, Expr exp3);

Expr first(Expr seq);
Expr second(Expr seq);

Expr nreverse(Expr seq);
Expr append(Expr seq1, Expr seq2);

/* env.h */

Expr make_env(Expr outer);

void env_def(Expr env, Expr var, Expr val);
void env_del(Expr env, Expr var);

bool env_can_set(Expr env, Expr var);

Expr env_get(Expr env, Expr var);
void env_set(Expr env, Expr var, Expr val);

void env_destructuring_bind(Expr env, Expr vars, Expr vals);

/* eval.h */

Expr eval(Expr exp, Expr env);

Expr eval_list(Expr exps, Expr env);
Expr eval_body(Expr exps, Expr env);

/* system.h */

typedef struct SystemState
{
    SymbolState symbol;
    ConsState cons;
    StreamState stream;
    GensymState gensym;
    StringState string;
    BuiltinState builtin;
} SystemState;

void system_init(SystemState * system);
void system_quit(SystemState * system);

/* global.h */

#if LISP_GLOBAL_API

extern SystemState global;

void global_init();
void global_quit();

inline static bool stream_at_end(Expr exp)
{
    return lisp_stream_at_end(&global.stream, exp);
}

inline static char const * builtin_name(Expr exp)
{
    return lisp_builtin_name(&global.builtin, exp);
}

inline static BuiltinFun builtin_fun(Expr exp)
{
    return lisp_builtin_fun(&global.builtin, exp);
}

#endif

#endif /* _LISP_HPP_ */

#ifdef LISP_IMPLEMENTATION

#ifndef _LISP_CPP_
#define _LISP_CPP_

#define LISP_TEST_FILE stdout

void test_begin(TestState * test)
{
    test->num_tests    = 0;
    test->num_failed   = 0;
    test->show_pass    = true;
    test->exit_on_fail = false;
}

void test_finish(TestState * test)
{
    if (test->num_failed)
    {
        fprintf(LISP_TEST_FILE, LISP_RED "FAIL" LISP_RESET " " "%d/%d test(s)\n", test->num_failed, test->num_tests);
    }
    else
    {
        fprintf(LISP_TEST_FILE, LISP_GREEN "PASS" LISP_RESET " " "%d/%d test(s)\n", test->num_tests, test->num_tests);
    }
}

void test_group(TestState * test, char const * text)
{
    fprintf(LISP_TEST_FILE, "==== %s ====\n", text);
}

void test_assert_try(TestState * test, bool exp, char const * msg)
{
    ++test->num_tests;
    if (exp)
    {
        if (test->show_pass)
        {
            fprintf(LISP_TEST_FILE, LISP_GREEN "PASS" LISP_RESET " %s\n", msg);
        }
    }
    else
    {
        ++test->num_failed;
        fprintf(LISP_TEST_FILE, LISP_RED "FAIL" LISP_RESET " %s\n", msg);
        if (test->exit_on_fail)
        {
            exit(1);
        }
    }
}

void error_fail(char const * fmt, ...)
{
    FILE * const file = stderr;
    va_list ap;
    va_start(ap, fmt);
    fprintf(file, LISP_RED "[FAIL] " LISP_RESET);
    vfprintf(file, fmt, ap);
    va_end(ap);
    exit(1);
}

void error_warn(char const * fmt, ...)
{
    FILE * const file = stderr;
    va_list ap;
    va_start(ap, fmt);
    fprintf(file, LISP_YELLOW "[WARN] " LISP_RESET);
    vfprintf(file, fmt, ap);
    va_end(ap);
}

Expr make_expr(U64 type, U64 data)
{
    return (data << 8) | (type & 0xff);
}

U64 expr_type(Expr exp)
{
    return exp & 0xff;
}

U64 expr_data(Expr exp)
{
    return exp >> 8;
}

static void _symbol_maybe_realloc(SymbolState * symbol)
{
    if (symbol->num < symbol->max)
    {
        return;
    }

    if (LISP_MAX_SYMBOLS == -1 || symbol->max * 2 <= LISP_MAX_SYMBOLS)
    {
        if (symbol->max == 0)
        {
            symbol->max = LISP_DEF_SYMBOLS;
        }
        else
        {
            symbol->max *= 2;
        }

        symbol->names = (char **) LISP_REALLOC(symbol->names, sizeof(char *) * symbol->max);
        if (!symbol->names)
        {
            LISP_FAIL("symbol memory allocation failed\n");
        }
        return;
    }

    LISP_FAIL("intern ran over memory budget\n");
}

void symbol_init(SymbolState * symbol)
{
    LISP_ASSERT_DEBUG(symbol);

    memset(symbol, 0, sizeof(SymbolState));
    _symbol_maybe_realloc(symbol);
}

void symbol_quit(SymbolState * symbol)
{
    for (U64 i = 0; i < symbol->num; i++)
    {
        LISP_FREE(symbol->names[i]);
    }
    LISP_FREE(symbol->names);
    memset(symbol, 0, sizeof(SymbolState));
}

Expr lisp_make_symbol(SymbolState * symbol, char const * name)
{
    LISP_ASSERT(name);
    size_t const len = strlen(name);

    for (U64 index = 0; index < symbol->num; ++index)
    {
        char const * str = symbol->names[index];
        size_t const tmp = strlen(str); // TODO cache this?
        if (len == tmp && !strncmp(name, str, len))
        {
            return make_expr(TYPE_SYMBOL, index);
        }
    }

    _symbol_maybe_realloc(symbol);

    U64 const index = symbol->num;
    char * buffer = (char *) LISP_MALLOC(len + 1);
    memcpy(buffer, name, len);
    buffer[len] = 0;
    symbol->names[index] = buffer;
    ++symbol->num;

    return make_expr(TYPE_SYMBOL, index);
}

char const * lisp_symbol_name(SymbolState * symbol, Expr exp)
{
    LISP_ASSERT(is_symbol(exp));
    U64 const index = expr_data(exp);
    if (index >= symbol->num)
    {
        LISP_FAIL("illegal symbol index %" PRIu64 "\n", index);
    }
    return symbol->names[index];
}

#if LISP_GLOBAL_API
Expr make_symbol(char const * name)
{
    return lisp_make_symbol(&global.symbol, name);
}

char const * symbol_name(Expr exp)
{
#if LISP_SYMBOL_NAME_OF_NIL
    if (exp == nil)
    {
        return "nil";
    }
#endif
    return lisp_symbol_name(&global.symbol, exp);
}

#endif

static void _cons_realloc(ConsState * cons)
{
    cons->pairs = (struct Pair *) LISP_REALLOC(cons->pairs, sizeof(struct Pair) * cons->max);
    if (!cons->pairs)
    {
        LISP_FAIL("cons memory allocation failed\n");
    }
}

static void _cons_maybe_realloc(ConsState * cons)
{
    if (cons->num < cons->max)
    {
        return;
    }

    if (LISP_MAX_CONSES == -1 || cons->max * 2 <= (U64) LISP_MAX_CONSES)
    {
        if (cons->max == 0)
        {
            cons->max = LISP_DEF_CONSES;
        }
        else
        {
            cons->max *= 2;
        }

        _cons_realloc(cons);
        return;
    }

    LISP_FAIL("cons ran over memory budget\n");
}

static struct Pair * _cons_lookup(ConsState * cons, U64 index)
{
    LISP_ASSERT_DEBUG(index < cons->num);
    return &cons->pairs[index];
}

void cons_init(ConsState * cons)
{
    memset(cons, 0, sizeof(ConsState));
}

void cons_quit(ConsState * cons)
{
}

bool is_cons(Expr exp)
{
    return expr_type(exp) == TYPE_CONS;
}

Expr lisp_cons(ConsState * cons, Expr a, Expr b)
{
    _cons_maybe_realloc(cons);

    U64 const index = cons->num++;
    struct Pair * pair = _cons_lookup(cons, index);
    pair->a = a;
    pair->b = b;
    return make_expr(TYPE_CONS, index);
}

Expr lisp_car(ConsState * cons, Expr exp)
{
    LISP_ASSERT(is_cons(exp));

    U64 const index = expr_data(exp);
    struct Pair * pair = _cons_lookup(cons, index);
    return pair->a;
}

Expr lisp_cdr(ConsState * cons, Expr exp)
{
    LISP_ASSERT(is_cons(exp));

    U64 const index = expr_data(exp);
    struct Pair * pair = _cons_lookup(cons, index);
    return pair->b;
}

void lisp_rplaca(ConsState * cons, Expr exp, Expr val)
{
    LISP_ASSERT(is_cons(exp));

    U64 const index = expr_data(exp);
    struct Pair * pair = _cons_lookup(cons, index);
    pair->a = val;
}

void lisp_rplacd(ConsState * cons, Expr exp, Expr val)
{
    LISP_ASSERT(is_cons(exp));

    U64 const index = expr_data(exp);
    struct Pair * pair = _cons_lookup(cons, index);
    pair->b = val;
}

#if LISP_GLOBAL_API

Expr cons(Expr a, Expr b)
{
    return lisp_cons(&global.cons, a, b);
}

Expr car(Expr exp)
{
    return lisp_car(&global.cons, exp);
}

Expr cdr(Expr exp)
{
    return lisp_cdr(&global.cons, exp);
}

void rplaca(Expr exp, Expr val)
{
    lisp_rplaca(&global.cons, exp, val);
}

void rplacd(Expr exp, Expr val)
{
    lisp_rplacd(&global.cons, exp, val);
}

#endif

void gensym_init(GensymState * gensym)
{
    memset(gensym, 0, sizeof(GensymState));
}

void gensym_quit(GensymState * gensym)
{
}

bool is_gensym(Expr exp)
{
    return expr_type(exp) == TYPE_GENSYM;
}

Expr lisp_gensym(GensymState * gensym)
{
    return make_expr(TYPE_GENSYM, gensym->counter++);
}

static Expr string_alloc(StringState * string, size_t len)
{
    if (string->count < LISP_MAX_STRINGS)
    {
        U64 const index = string->count;
        string->values[index] = (char *) LISP_MALLOC(len + 1);
        ++string->count;
        return make_expr(TYPE_STRING, index);
    }

    LISP_FAIL("cannot make string of length %d\n", (int) len);
    return nil;
}

static char * string_buffer(StringState * string, Expr exp)
{
    LISP_ASSERT(is_string(exp));

    U64 const index = expr_data(exp);
    if (index >= string->count)
    {
        LISP_FAIL("illegal string index %" PRIu64 "\n", index);
    }

    return string->values[index];
}

void string_init(StringState * string)
{
    memset(string, 0, sizeof(StringState));
    string->values = (char **) LISP_MALLOC(sizeof(char *) * LISP_MAX_STRINGS);
}

void string_quit(StringState * string)
{
    LISP_FREE(string->values);
    memset(string, 0, sizeof(StringState));
}

bool is_string(Expr exp)
{
    return expr_type(exp) == TYPE_STRING;
}

Expr lisp_make_string(StringState * string, char const * str)
{
    size_t const len = strlen(str);

    /* TODO fixstrs sound attractive,
       but the string_value API breaks
       b/c it has to return a pointer */

    Expr ret = string_alloc(string, len);
    memcpy(string_buffer(string, ret), str, len + 1);
    return ret;
}

char const * lisp_string_value(StringState * string, Expr exp)
{
    return string_buffer(string, exp);
}

U64 lisp_string_length(StringState * string, Expr exp)
{
    // TODO cache this
    return (U64) strlen(lisp_string_value(string, exp));
}

Expr make_string(char const * str)
{
    return lisp_make_string(&global.string, str);
}

char const * string_value(Expr exp)
{
    return lisp_string_value(&global.string, exp);
}

U64 string_length(Expr exp)
{
    return lisp_string_length(&global.string, exp);
}

void stream_init(StreamState * stream)
{
    memset(stream, 0, sizeof(StreamState));

    stream->stdin = lisp_make_file_input_stream(stream, stdin, false);
    stream->stdout = lisp_make_file_output_stream(stream, stdout, false);
    stream->stderr = lisp_make_file_output_stream(stream, stderr, false);
}

void stream_quit(StreamState * stream)
{
    for (U64 i = 0; i < stream->num; i++)
    {
        StreamInfo * info = stream->info + i;
        if (info->close_on_quit)
        {
            fclose(info->file);
        }
    }
}

bool is_stream(Expr exp)
{
    return expr_type(exp) == TYPE_STREAM;
}

static Expr _make_file_stream(StreamState * stream, FILE * file, bool close_on_quit)
{
    LISP_ASSERT(stream->num < LISP_MAX_STREAMS);
    U64 const index = stream->num++;
    StreamInfo * info = stream->info + index;
    memset(info, 0, sizeof(StreamInfo));
    info->file = file;
    info->close_on_quit = close_on_quit;
    return make_expr(TYPE_STREAM, index);
}

static Expr _make_buffer_stream(StreamState * stream, size_t size, char * buffer)
{
    LISP_ASSERT(stream->num < LISP_MAX_STREAMS);
    U64 const index = stream->num++;
    StreamInfo * info = stream->info + index;
    memset(info, 0, sizeof(StreamInfo));
    info->size = size;
    info->buffer = buffer;
    info->cursor = 0;

    return make_expr(TYPE_STREAM, index);
}

void lisp_stream_show_info(StreamState * stream)
{
    for (U64 i = 0; i < stream->num; i++)
    {
        StreamInfo * info = stream->info + i;
        fprintf(stderr, "stream %d:\n", (int) i);
        fprintf(stderr, "- file: %p\n", info->file);
        fprintf(stderr, "- buffer: %p\n", info->buffer);
    }
}

Expr lisp_make_file_input_stream(StreamState * stream, FILE * file, bool close_on_quit)
{
    return _make_file_stream(stream, file, close_on_quit);
}

Expr lisp_make_file_output_stream(StreamState * stream, FILE * file, bool close_on_quit)
{
    return _make_file_stream(stream, file, close_on_quit);
}

Expr lisp_make_string_input_stream(StreamState * stream, char const * str)
{
    // TODO copy string into buffer?
    size_t const len = strlen(str);
    return _make_buffer_stream(stream, len + 1, (char *) str);
}

Expr lisp_make_buffer_output_stream(StreamState * stream, size_t size, char * buffer)
{
    return _make_buffer_stream(stream, size, buffer);
}

char lisp_stream_peek_char(StreamState * stream, Expr exp)
{
    U64 const index = expr_data(exp);
    LISP_ASSERT(index < stream->num);
    StreamInfo * info = stream->info + index;
    if (info->file)
    {
        int ch = fgetc(info->file);
        if (ch == EOF)
        {
            return 0;
        }
        ungetc(ch, info->file);
        return ch;
    }

    if (info->buffer)
    {
        if (info->cursor < info->size)
        {
            return info->buffer[info->cursor];
        }
        else
        {
            return 0;
        }
    }

    LISP_FAIL("cannot read from stream\n");
    return 0;
}

void lisp_stream_skip_char(StreamState * stream, Expr exp)
{
    U64 const index = expr_data(exp);
    LISP_ASSERT(index < stream->num);
    StreamInfo * info = stream->info + index;
    if (info->file)
    {
        fgetc(info->file);
        return;
    }

    if (info->buffer)
    {
        ++info->cursor;
        return;
    }

    LISP_FAIL("cannot read from stream\n");
}

bool lisp_stream_at_end(StreamState * stream, Expr exp)
{
    return lisp_stream_peek_char(stream, exp) == 0;
}

void lisp_stream_put_string(StreamState * stream, Expr exp, char const * str)
{
    LISP_ASSERT(is_stream(exp));
    U64 const index = expr_data(exp);
    LISP_ASSERT(index < stream->num);
    StreamInfo * info = stream->info + index;
    if (info->file)
    {
        fputs(str, info->file);
    }

    if (info->buffer)
    {
        size_t const len = strlen(str);
        LISP_ASSERT(info->cursor + len + 1 < info->size);
        memcpy(info->buffer + info->cursor, str, len + 1);
        info->cursor += len;
    }
}

void lisp_stream_put_char(StreamState * stream, Expr exp, char ch)
{
    // TODO make this more efficient
    char const tmp[2] = { ch, 0 };
    lisp_stream_put_string(stream, exp, tmp);
}

void lisp_stream_release(StreamState * stream, Expr exp)
{
    LISP_ASSERT(is_stream(exp));
    U64 const index = expr_data(exp);
    LISP_ASSERT(index < stream->num);
    StreamInfo * info = stream->info + index;
    if (info->close_on_quit)
    {
        fclose(info->file);
    }
    memcpy(info, stream->info + --stream->num, sizeof(StreamInfo));
}

#if LISP_GLOBAL_API

Expr make_file_input_stream_from_path(char const * path)
{
    FILE * file = fopen(path, "rb");
    LISP_ASSERT(file);
    return lisp_make_file_input_stream(&global.stream, file, true);
}

Expr make_string_input_stream(char const * str)
{
    return lisp_make_string_input_stream(&global.stream, str);
}

char stream_peek_char(Expr exp)
{
    return lisp_stream_peek_char(&global.stream, exp);
}

void stream_skip_char(Expr exp)
{
    lisp_stream_skip_char(&global.stream, exp);
}

void stream_put_char(Expr exp, char ch)
{
    lisp_stream_put_char(&global.stream, exp, ch);
}

void stream_put_string(Expr exp, char const * str)
{
    lisp_stream_put_string(&global.stream, exp, str);
}

void stream_put_u64(Expr exp, U64 val)
{
    char str[32];
    sprintf(str, "%" PRIu64, val);
    stream_put_string(exp, str);
}

void stream_put_x64(Expr exp, U64 val)
{
    char str[32];
    sprintf(str, "%016" PRIu64, val);
    stream_put_string(exp, str);
}

void stream_release(Expr exp)
{
    lisp_stream_release(&global.stream, exp);
}

#endif

void builtin_init(BuiltinState * builtin)
{
    memset(builtin, 0, sizeof(BuiltinState));
}

void builtin_quit(BuiltinState * builtin)
{
}

bool is_builtin_special(Expr exp)
{
    return expr_type(exp) == TYPE_BUILTIN_SPECIAL;
}

bool is_builtin_function(Expr exp)
{
    return expr_type(exp) == TYPE_BUILTIN_FUNCTION;
}

static Expr lisp_make_builtin(BuiltinState * builtin, char const * name, BuiltinFun fun, U64 type)
{
    LISP_ASSERT(builtin->num < LISP_MAX_BUILTINS);
    U64 const index = builtin->num++;
    BuiltinInfo * info = builtin->info + index;
    info->name = name; /* TODO take ownership of name? */
    info->fun = fun;
    return make_expr(type, index);
}

Expr lisp_make_builtin_special(BuiltinState * builtin, char const * name, BuiltinFun fun)
{
    return lisp_make_builtin(builtin, name, fun, TYPE_BUILTIN_SPECIAL);
}

Expr lisp_make_builtin_function(BuiltinState * builtin, char const * name, BuiltinFun fun)
{
    return lisp_make_builtin(builtin, name, fun, TYPE_BUILTIN_FUNCTION);
}

static BuiltinInfo * _builtin_expr_to_info(BuiltinState * builtin, Expr exp)
{
    LISP_ASSERT(is_builtin(exp));
    U64 const index = expr_data(exp);
    LISP_ASSERT(index < builtin->num);
    return builtin->info + index;
}

char const * lisp_builtin_name(BuiltinState * builtin, Expr exp)
{
    BuiltinInfo * info = _builtin_expr_to_info(builtin, exp);
    return info->name;
}

BuiltinFun lisp_builtin_fun(BuiltinState * builtin, Expr exp)
{
    BuiltinInfo * info = _builtin_expr_to_info(builtin, exp);
    return info->fun;
}

#if LISP_GLOBAL_API

Expr make_builtin_special(char const * name, BuiltinFun fun)
{
    return lisp_make_builtin_special(&global.builtin, name, fun);
}

Expr make_builtin_function(char const * name, BuiltinFun fun)
{
    return lisp_make_builtin_function(&global.builtin, name, fun);
}

#endif

static bool is_whitespace(char ch)
{
    return
        ch == ' ' ||
        ch == '\n' ||
        ch == '\t';
}

static bool is_symbol_start(char ch)
{
    return
        ch != 0 &&
        !is_whitespace(ch) &&
        ch != '"' &&
        ch != '(' && ch != ')' &&
        ch != ';' && ch != '\'';
}

static bool is_symbol_part(char ch)
{
    return is_symbol_start(ch);
}

static void skip_whitespace_or_comment(Expr in)
{
whitespace:
    while (is_whitespace(stream_peek_char(in)))
    {
        stream_skip_char(in);
    }

    if (stream_peek_char(in) != ';')
    {
        return;
    }

    stream_skip_char(in);

comment:
    if (stream_peek_char(in) == 0)
    {
        return;
    }

    if (stream_peek_char(in) == '\n')
    {
        stream_skip_char(in);
        goto whitespace;
    }

    stream_skip_char(in);
    goto comment;
}

static Expr parse_expr(SystemState * sys, Expr in);

static Expr parse_list(SystemState * sys, Expr in)
{
    Expr exp = nil;
    Expr head = nil;
    Expr tail = nil;

    if (stream_peek_char(in) != '(')
    {
        LISP_FAIL("expected '(', got '%c'\n", stream_peek_char(in));
        return nil;
    }

    stream_skip_char(in);

list_loop:
    skip_whitespace_or_comment(in);

    if (stream_peek_char(in) == 0)
    {
        LISP_FAIL("unexpected eof\n");
        return nil;
    }

    if (stream_peek_char(in) == ')')
    {
        goto list_done;
    }

    exp = parse_expr(sys, in);

    // TODO get rid of artifical symbol dependence for dotted lists
    if (exp == intern("."))
    {
        exp = parse_expr(sys, in);
        lisp_rplacd(&sys->cons, tail, exp);

        skip_whitespace_or_comment(in);

        goto list_done;
    }
    else
    {
        Expr next = lisp_cons(&sys->cons, exp, nil);
        if (head)
        {
            lisp_rplacd(&sys->cons, tail, next);
            tail = next;
        }
        else
        {
            head = tail = next;
        }
    }

    goto list_loop;

list_done:
    // TODO use expect_char(in, ')')
    if (stream_peek_char(in) != ')')
    {
        LISP_FAIL("missing ')'\n");
        return nil;
    }
    stream_skip_char(in);

    return head;
}

static char parse_hex_digit(Expr in, char val)
{
    char const ch = stream_get_char(in);

    if (ch >= '0' && ch <= '9')
    {
        val *= 16;
        val += ch - '0';
    }

    else if (ch >= 'a' && ch <= 'f')
    {
        val *= 16;
        val += 10 + ch - 'a';
    }

    else if (ch >= 'A' && ch <= 'F')
    {
        val *= 16;
        val += 10 + ch - 'A';
    }

    else
    {
        LISP_FAIL("malformed string");
    }

    return val;
}

static Expr parse_string(SystemState * sys, Expr in)
{
    enum
    {
        STATE_DEFAULT,
        STATE_ESCAPE,
    } state = STATE_DEFAULT;

    if (stream_peek_char(in) != '"')
    {
        LISP_FAIL("missing '\"'\n");
        return nil;
    }
    stream_skip_char(in);

    char lexeme[4096];
    Expr tok = lisp_make_buffer_output_stream(&sys->stream, 4096, lexeme);

string_loop:
    if (stream_peek_char(in) == 0)
    {
        LISP_FAIL("unexpected eof in string\n");
        return nil;
    }

    else if (state == STATE_DEFAULT)
    {
        if (stream_peek_char(in) == '"')
        {
            stream_skip_char(in);
            goto string_done;
        }
        else if (stream_peek_char(in) == '\\')
        {
            stream_skip_char(in);
            state = STATE_ESCAPE;
        }
        else
        {
            stream_put_char(tok, stream_get_char(in));
        }
    }

    else if (state == STATE_ESCAPE)
    {
        if (stream_peek_char(in) == 'n')
        {
            stream_skip_char(in);
            stream_put_char(tok, '\n');
        }

        else if (stream_peek_char(in) == 't')
        {
            stream_skip_char(in);
            stream_put_char(tok, '\t');
        }

        else if (stream_peek_char(in) == 'x')
        {
            stream_skip_char(in);

            char val = 0;
            val = parse_hex_digit(in, val);
            val = parse_hex_digit(in, val);

            /* TODO check for more digits? */

            stream_put_char(tok, val);
        }

        else
        {
            stream_put_char(tok, stream_get_char(in));
        }

        state = STATE_DEFAULT;
    }

    else
    {
        LISP_FAIL("internal error\n");
        return nil;
    }

    goto string_loop;

string_done:
    stream_put_char(tok, 0);
    Expr const ret = make_string(lexeme);
    stream_release(tok);
    return ret;
}

static Expr parse_expr(SystemState * sys, Expr in)
{
    skip_whitespace_or_comment(in);

    if (stream_peek_char(in) == '(')
    {
        return parse_list(sys, in);
    }
    else if (stream_peek_char(in) == '"')
    {
        return parse_string(sys, in);
    }
#if LISP_READER_PARSE_QUOTE
    else if (stream_peek_char(in) == '\'')
    {
        stream_skip_char(in);
        Expr const exp = list_2(intern("quote"), parse_expr(sys, in));
        return exp;
    }
    else if (stream_peek_char(in) == '`')
    {
        stream_skip_char(in);
        Expr const exp = list_2(intern("backquote"), parse_expr(sys, in));
        return exp;
    }
    else if (stream_peek_char(in) == ',')
    {
        stream_skip_char(in);
        if (stream_peek_char(in) == '@')
        {
            stream_skip_char(in);
            return list_2(intern("unquote-splicing"), parse_expr(sys, in));
        }
        return list_2(intern("unquote"), parse_expr(sys, in));
    }
#endif
    else if (is_symbol_start(stream_peek_char(in)))
    {
        char lexeme[4096];
        Expr tok = lisp_make_buffer_output_stream(&sys->stream, 4096, lexeme);
        stream_put_char(tok, stream_get_char(in));

    symbol_loop:
        if (is_symbol_part(stream_peek_char(in)))
        {
            stream_put_char(tok, stream_get_char(in));
            goto symbol_loop;
        }
        else
        {
            goto symbol_done;
        }

    symbol_done:
        stream_put_char(tok, 0);
        Expr const ret = intern(lexeme);
        stream_release(tok);
        return ret;
    }
    else
    {
        LISP_FAIL("cannot read expression, unexpected '%c'\n", stream_peek_char(in));
        return nil;
    }
}

bool lisp_maybe_parse_expr(SystemState * sys, Expr in, Expr * exp)
{
    skip_whitespace_or_comment(in);
    if (lisp_stream_at_end(&sys->stream, in))
    {
        return false;
    }
    *exp = parse_expr(sys, in);
    return true;
}

Expr lisp_read_one_from_string(SystemState * sys, char const * src)
{
    Expr const in = lisp_make_string_input_stream(&sys->stream, src);
    Expr ret = parse_expr(sys, in);
    lisp_stream_release(&sys->stream, in);
    //println(ret);
    return ret;
}

bool maybe_parse_expr(Expr in, Expr * exp)
{
    return lisp_maybe_parse_expr(&global, in, exp);
}

Expr read_one_from_string(char const * src)
{
    return lisp_read_one_from_string(&global, src);
}

static bool is_quote_call(Expr exp)
{
    return is_named_call(exp, intern("quote"));
}

void render_expr(Expr exp, Expr out);

void render_cons(Expr exp, Expr out)
{
#if LISP_PRINTER_RENDER_QUOTE
    if (is_quote_call(exp))
    {
        stream_put_char(out, '\'');
        render_expr(cadr(exp), out);
        return;
    }
#endif
    stream_put_char(out, '(');
    render_expr(car(exp), out);

    for (Expr tmp = cdr(exp); tmp; tmp = cdr(tmp))
    {
        if (tmp == exp)
        {
            stream_put_string(out, " ...");
            break;
        }
        else if (is_cons(tmp))
        {
            stream_put_char(out, ' ');
            render_expr(car(tmp), out);
        }
        else
        {
            stream_put_string(out, " . ");
            render_expr(tmp, out);
            break;
        }
    }

    stream_put_char(out, ')');
}

void render_builtin_special(Expr exp, Expr out)
{
    stream_put_string(out, "#:<special operator");
    char const * name = builtin_name(exp);
    if (name)
    {
        stream_put_string(out, " ");
        stream_put_string(out, name);
    }
    stream_put_string(out, ">");
}

void render_builtin_function(Expr exp, Expr out)
{
    stream_put_string(out, "#:<core function");
    char const * name = builtin_name(exp);
    if (name)
    {
        stream_put_string(out, " ");
        stream_put_string(out, name);
    }
    stream_put_string(out, ">");
}

void render_gensym(Expr exp, Expr out)
{
    LISP_ASSERT_DEBUG(is_gensym(exp));
    U64 const num = expr_data(exp);
    stream_put_string(out, "#:G");
    stream_put_u64(out, num);
}

void render_string(Expr exp, Expr out)
{
    stream_put_char(out, '"');
    char const * str = string_value(exp);
    size_t const len = string_length(exp);
    for (size_t i = 0; i < len; ++i)
    {
        // TODO \u****
        // TODO \U********
        char const ch = str[i];
        switch (ch)
        {
        case '"':
            stream_put_char(out, '\\');
            stream_put_char(out, '"');
            break;
        case '\n':
            stream_put_char(out, '\\');
            stream_put_char(out, 'n');
            break;
        case '\t':
            stream_put_char(out, '\\');
            stream_put_char(out, 't');
            break;
        default:
            if (ch == 0x1b) // TODO use a function to test what to escape
            {
                stream_put_char(out, '\\');
                stream_put_char(out, 'x');
                stream_put_x64(out, ch);
            }
            else
            {
                stream_put_char(out, ch);
            }
            break;
        }
    }
    stream_put_char(out, '"');
}

void render_expr(Expr exp, Expr out)
{
    switch (expr_type(exp))
    {
    case TYPE_NIL:
        LISP_ASSERT_DEBUG(expr_data(exp) == 0);
        stream_put_string(out, "nil");
        break;
    case TYPE_SYMBOL:
        stream_put_string(out, symbol_name(exp));
        break;
    case TYPE_CONS:
        render_cons(exp, out);
        break;
    case TYPE_GENSYM:
        render_gensym(exp, out);
        break;
    case TYPE_STRING:
        render_string(exp, out);
        break;
    case TYPE_BUILTIN_SPECIAL:
        render_builtin_special(exp, out);
        break;
    case TYPE_BUILTIN_FUNCTION:
        render_builtin_function(exp, out);
        break;
    default:
        LISP_FAIL("cannot print expression %016" PRIx64 "\n", exp);
        break;
    }
}

#define TEMP_BUF_SIZE  4096
#define TEMP_BUF_COUNT 4

bool is_named_call(Expr exp, Expr name)
{
    return is_cons(exp) && eq(car(exp), name);
}

char * get_temp_buf(size_t size)
{
    LISP_ASSERT(size <= TEMP_BUF_SIZE);

    static char buf[TEMP_BUF_COUNT][TEMP_BUF_SIZE];
    static int idx = 0;
    char * ret = buf[idx];
    idx = (idx + 1) % TEMP_BUF_COUNT;
    return ret;
}

bool equal(Expr a, Expr b)
{
    if (is_cons(a) && is_cons(b))
    {
        return equal(car(a), car(b)) && equal(cdr(a), cdr(b));
    }
    return eq(a, b);
}

char const * repr(Expr exp)
{
    // TODO multiple calls => need temp buffer per call
    size_t const size = 4096;
    char * buffer = get_temp_buf(size);
    Expr out = lisp_make_buffer_output_stream(&global.stream, size, buffer);
    render_expr(exp, out);
    stream_release(out);
    return buffer;
}

void println(Expr exp)
{
    Expr out = global.stream.stdout;
    render_expr(exp, out);
    stream_put_string(out, "\n");
}

Expr intern(char const * name)
{
    if (!strcmp("nil", name))
    {
        return nil;
    }
    else
    {
        return make_symbol(name);
    }
}

Expr list_1(Expr exp1)
{
    return cons(exp1, nil);
}

Expr list_2(Expr exp1, Expr exp2)
{
    return cons(exp1, cons(exp2, nil));
}

Expr list_3(Expr exp1, Expr exp2, Expr exp3)
{
    return cons(exp1, cons(exp2, cons(exp3, nil)));
}

Expr first(Expr seq)
{
    return car(seq);
}

Expr second(Expr seq)
{
    return car(cdr(seq));
}

Expr nreverse(Expr list)
{
    if (!list)
    {
        return list;
    }

    Expr prev = nil;
    Expr expr = list;
    while (is_cons(expr))
    {
        Expr next = cdr(expr);
        rplacd(expr, prev);
        prev = expr;
        expr = next;
    }
    if (expr)
    {
        Expr iter;
        for (iter = prev; cdr(iter); iter = cdr(iter))
        {
            Expr next = car(iter);
            rplaca(iter, expr);
            expr = next;
        }
        Expr next = car(iter);
        rplaca(iter, expr);
        rplacd(iter, next);
    }
    return prev;
}

Expr append(Expr a, Expr b)
{
    return a ? cons(car(a), append(cdr(a), b)) : b;
}

static Expr env_vars(Expr env);
static void env_set_vars(Expr env, Expr vars);
static Expr env_vals(Expr env);
static void env_set_vals(Expr env, Expr vals);
static Expr env_outer(Expr env);

static Expr _env_find_local(Expr env, Expr var)
{
    Expr vars = env_vars(env);
    Expr vals = env_vals(env);
    while (vars)
    {
        if (car(vars) == var)
        {
            return vals;
        }
        vars = cdr(vars);
        vals = cdr(vals);
    }
    return nil;
}

static Expr _env_find_global(Expr env, Expr var)
{
    while (env)
    {
        Expr const vals = _env_find_local(env, var);
        if (vals)
        {
            return vals;
        }
        else
        {
            env = env_outer(env);
        }
    }
    return nil;
}

Expr make_env(Expr outer)
{
    // ((<vars> . <vals>) . <outer>)
    // TODO add dummy conses as sentinels for vars and vals
    return cons(cons(nil, nil), outer);
}

static Expr env_vars(Expr env)
{
    return caar(env);
}

static void env_set_vars(Expr env, Expr vars)
{
    rplaca(car(env), vars);
}

static Expr env_vals(Expr env)
{
    return cdar(env);
}

static void env_set_vals(Expr env, Expr vals)
{
    rplacd(car(env), vals);
}

static Expr env_outer(Expr env)
{
    return cdr(env);
}

void env_def(Expr env, Expr var, Expr val)
{
    Expr const vals = _env_find_local(env, var);
    if (vals)
    {
        rplaca(vals, val);
    }
    else
    {
        env_set_vars(env, cons(var, env_vars(env)));
        env_set_vals(env, cons(val, env_vals(env)));
    }
}

void env_del(Expr env, Expr var)
{
    Expr prev_vars = nil;
    Expr prev_vals = nil;

    Expr vars = env_vars(env);
    Expr vals = env_vals(env);
    while (vars)
    {
        if (car(vars) == var)
        {
            if (prev_vars)
            {
                LISP_ASSERT(prev_vals);
                rplacd(prev_vars, cdr(vars));
                rplacd(prev_vals, cdr(vals));
            }
            else
            {
                env_set_vars(env, cdr(vars));
                env_set_vals(env, cdr(vals));
            }
            return;
        }

        prev_vars = vars;
        prev_vals = vals;
        vars = cdr(vars);
        vals = cdr(vals);
    }

    LISP_FAIL("unbound variable %s\n", repr(var));
}

bool env_can_set(Expr env, Expr var)
{
    Expr const tmp = _env_find_global(env, var);
    return tmp != nil;
}

Expr env_get(Expr env, Expr var)
{
    Expr const vals = _env_find_global(env, var);
    if (vals)
    {
        return car(vals);
    }
    else
    {
        LISP_FAIL("unbound variable %s\n", repr(var));
        return nil;
    }
}

void env_set(Expr env, Expr var, Expr val)
{
    Expr const vals = _env_find_global(env, var);
    if (vals)
    {
        rplaca(vals, val);
    }
    else
    {
        LISP_FAIL("unbound variable %s\n", repr(var));
    }
}

void env_destructuring_bind(Expr env, Expr vars, Expr vals)
{
    if (vars == nil)
    {
        if (vals != nil)
        {
            LISP_FAIL("no more parameters to bind\n");
        }
    }
    else if (is_cons(vars))
    {
        while (vars)
        {
            if (is_cons(vars))
            {
                LISP_ASSERT(is_cons(vals));
                env_destructuring_bind(env, car(vars), car(vals));
                vars = cdr(vars);
                vals = cdr(vals);
            }
            else
            {
                env_destructuring_bind(env, vars, vals);
                break;
            }
        }
    }
    else
    {
        env_def(env, vars, vals);
    }
}

static void env_defun(Expr env, char const * name, BuiltinFun fun)
{
    env_def(env, intern(name), make_builtin_function(name, fun));
}

static void env_defspecial(Expr env, char const * name, BuiltinFun fun)
{
    env_def(env, intern(name), make_builtin_special(name, fun));
}

Expr s_quote(Expr args, Expr kwargs, Expr env)
{
    return car(args);
}

Expr s_def(Expr args, Expr kwargs, Expr env)
{
    // TODO look for env in kwargs
    env_def(env, car(args), eval(cadr(args), env));
    return nil;
}

Expr s_if(Expr args, Expr kwargs, Expr env)
{
    if (eval(car(args), env) != nil)
    {
        return eval(cadr(args), env);
    }
    else if (cddr(args))
    {
        return eval(caddr(args), env);
    }
    else
    {
        return nil;
    }
}

Expr s_while(Expr args, Expr kwargs, Expr env)
{
    Expr const test = car(args);
    Expr const body = cdr(args);

    // TODO do we want to return a value?
    while (eval(test, env))
    {
        eval_body(body, env);
    }
    return nil;
}

Expr s_lambda(Expr args, Expr kwargs, Expr env)
{
    Expr const fun_args = car(args);
    Expr const fun_body = cdr(args);
    return cons(intern("lit"), cons(intern("clo"), cons(env, cons(fun_args, fun_body))));
}

bool is_unquote(Expr exp)
{
    return is_named_call(exp, LISP_SYM_UNQUOTE);
}

bool is_unquote_splicing(Expr exp)
{
    return is_named_call(exp, LISP_SYM_UNQUOTE_SPLICING);
}

static Expr backquote(Expr exp, Expr env);

static Expr backquote_list(Expr seq, Expr env)
{
    if (seq)
    {
        Expr item = car(seq);
        Expr rest = cdr(seq);
        if (is_unquote_splicing(item))
        {
            return append(eval(cadr(item), env), backquote_list(rest, env));
        }
        else
        {
            return cons(backquote(item, env), backquote_list(rest, env));
        }
    }
    else
    {
        return nil;
    }
}

static Expr backquote(Expr exp, Expr env)
{
    if (is_cons(exp))
    {
        if (is_unquote(exp))
        {
            return eval(cadr(exp), env);
        }
        else
        {
            return backquote_list(exp, env);
        }
    }
    else
    {
        return exp;
    }
}

Expr s_backquote(Expr args, Expr kwargs, Expr env)
{
    return backquote(car(args), env);
}

Expr s_syntax(Expr args, Expr kwargs, Expr env)
{
    Expr const fun_args = car(args);
    Expr const fun_body = cdr(args);
    return cons(intern("lit"), cons(intern("mac"), cons(env, cons(fun_args, fun_body))));
}

Expr f_eq(Expr args, Expr kwargs, Expr env)
{
    if (is_nil(args))
    {
        LISP_FAIL("not enough arguments in call %s\n", repr(cons(intern("eq"), args)));
    }
    Expr prv = car(args);
    Expr tmp = cdr(args);
    if (is_nil(tmp))
    {
        LISP_FAIL("not enough arguments in call %s\n", repr(cons(intern("eq"), args)));
    }
    for (; tmp; tmp = cdr(tmp))
    {
        Expr const exp = car(tmp);
        if (prv != exp)
        {
            return nil;
        }
        prv = exp;
    }
    return intern("t");
}

Expr f_equal(Expr args, Expr kwargs, Expr env)
{
    if (is_nil(args))
    {
        LISP_FAIL("not enough arguments in call %s\n", repr(cons(intern("equal"), args)));
    }
    Expr prv = car(args);
    Expr tmp = cdr(args);
    if (is_nil(tmp))
    {
        LISP_FAIL("not enough arguments in call %s\n", repr(cons(intern("equal"), args)));
    }

    for (; tmp; tmp = cdr(tmp))
    {
        Expr const exp = car(tmp);
        if (!equal(prv, exp))
        {
            return nil;
        }
        prv = exp;
    }

    return LISP_SYMBOL_T;
}

Expr f_cons(Expr args, Expr kwargs, Expr env)
{
    LISP_ASSERT(args != nil);
    LISP_ASSERT(cdr(args) != nil);
    LISP_ASSERT(cddr(args) == nil);

    Expr const exp1 = car(args);
    Expr const exp2 = cadr(args);
    return cons(exp1, exp2);
}

Expr f_car(Expr args, Expr kwargs, Expr env)
{
    LISP_ASSERT(args != nil);
    LISP_ASSERT(cdr(args) == nil);

    Expr const exp1 = car(args);
    return car(exp1);
}

Expr f_cdr(Expr args, Expr kwargs, Expr env)
{
    LISP_ASSERT(args != nil);
    LISP_ASSERT(cdr(args) == nil);

    Expr const exp1 = car(args);
    return cdr(exp1);
}

Expr f_println(Expr args, Expr kwargs, Expr env)
{
    Expr out = global.stream.stdout;
    for (Expr tmp = args; tmp; tmp = cdr(tmp))
    {
        if (tmp != args)
        {
            stream_put_char(out, ' ');
        }
        Expr exp = car(tmp);
        render_expr(exp, out);
    }
    stream_put_char(out, '\n');
    return nil;
}

Expr f_gensym(Expr args, Expr kwargs, Expr env)
{
    return lisp_gensym(&global.gensym);
}

bool is_op(Expr exp, Expr name)
{
    return is_cons(exp) && car(exp) == name;
}

bool is_quote(Expr exp)
{
    return is_op(exp, LISP_SYM_QUOTE);
}

bool is_if(Expr exp)
{
    return is_op(exp, LISP_SYM_IF);
}

Expr eval(Expr exp, Expr env);

Expr eval_list(Expr exps, Expr env)
{
    Expr ret = nil;
    for (Expr tmp = exps; tmp; tmp = cdr(tmp))
    {
        Expr const exp = car(tmp);
        ret = cons(eval(exp, env), ret);
    }
    return nreverse(ret);
}

Expr eval_body(Expr exps, Expr env)
{
    Expr ret = nil;
    for (Expr tmp = exps; tmp; tmp = cdr(tmp))
    {
        Expr const exp = car(tmp);
        ret = eval(exp, env);
    }
    return ret;
}

/* TODO move these */

/* (lit clo <env> <args> . <body>) */

bool is_closure(Expr exp, Expr kind)
{
    return is_cons(exp) &&
        eq(intern("lit"), car(exp)) &&
        is_cons(cdr(exp)) &&
        eq(kind, cadr(exp));
}

bool is_function(Expr exp)
{
    return is_closure(exp, intern("clo"));
}

bool is_macro(Expr exp)
{
    return is_closure(exp, intern("mac"));
}

Expr closure_env(Expr exp)
{
    return caddr(exp);
}

Expr closure_args(Expr exp)
{
    return cadddr(exp);
}

Expr closure_body(Expr exp)
{
    return cddddr(exp);
}

static void bind_args(Expr env, Expr vars, Expr vals)
{
    env_destructuring_bind(env, vars, vals);
}

static Expr wrap_env(Expr lenv)
{
    return make_env(lenv);
}

static Expr make_call_env_from(Expr lenv, Expr vars, Expr vals)
{
    Expr cenv = wrap_env(lenv);
    bind_args(cenv, vars, vals);
    return cenv;
}

Expr apply(Expr name, Expr args, Expr env)
{
    if (is_builtin_function(name))
    {
        // TODO parse keyword args
        Expr kwargs = nil;
        Expr vals = eval_list(args, env);
        return builtin_fun(name)(vals, kwargs, env);
    }
    else if (is_builtin_special(name))
    {
        // TODO parse keyword args
        Expr kwargs = nil;
        Expr vals = args;
        return builtin_fun(name)(vals, kwargs, env);
    }
    else if (is_function(name))
    {
        // TODO parse keyword args
        Expr kwargs = nil;
        Expr vals = eval_list(args, env);
        Expr body = closure_body(name);
        return eval_body(body, make_call_env_from(closure_env(name), closure_args(name), vals));
    }
    else if (is_macro(name))
    {
        Expr body = closure_body(name);
        Expr exp = eval_body(body, make_call_env_from(closure_env(name), closure_args(name), args));
        return eval(exp, env);
    }
    else
    {
        // TODO check for unlimited recursion
        return apply(eval(name, env), args, env);
    }
}

Expr eval(Expr exp, Expr env)
{
    if (exp == nil)
    {
        return nil;
    }

    switch (expr_type(exp))
    {
    case TYPE_STRING:
        return exp;
    case TYPE_SYMBOL:
        if (exp == intern("*env*"))
        {
            return env;
        }
        return env_get(env, exp);
    case TYPE_CONS:
        return apply(car(exp), cdr(exp), env);
    default:
        LISP_FAIL("cannot evaluate %s\n", repr(exp));
        return nil;
    }
}

void system_init(SystemState * system)
{
    symbol_init(&system->symbol);
    cons_init(&system->cons);
    gensym_init(&system->gensym);
    string_init(&system->string);
    stream_init(&system->stream);
    builtin_init(&system->builtin);
}

void system_quit(SystemState * system)
{
    builtin_quit(&system->builtin);
    string_quit(&system->string);
    gensym_quit(&system->gensym);
    stream_quit(&system->stream);
    cons_quit(&system->cons);
    symbol_quit(&system->symbol);
}

SystemState global;

void global_init()
{
    system_init(&global);
}

void global_quit()
{
    system_quit(&global);
}

class System
{
public:
    System()
    {
    }

    virtual ~System()
    {
    }

    virtual Expr make_core_env()
    {
        Expr env = make_env(nil);

        env_def(env, intern("t"), intern("t"));

        env_defspecial(env, "quote", s_quote);
        env_defspecial(env, "if", s_if);
        env_defspecial(env, "while", s_while);
        env_defspecial(env, "def", s_def);
        env_defspecial(env, "lambda", s_lambda);
        env_defspecial(env, "syntax", s_syntax);
        env_defspecial(env, "backquote", s_backquote);

        env_defun(env, "eq", f_eq);
        env_defun(env, "equal", f_equal);
        env_defun(env, "cons", f_cons);
        env_defun(env, "car", f_car);
        env_defun(env, "cdr", f_cdr);
        env_defun(env, "println", f_println);

        env_defun(env, "gensym", f_gensym);
        env_defun(env, "load-file", [this](Expr args, Expr kwargs, Expr env) -> Expr
        {
            load_file(string_value(first(args)), env);
            return nil;
        });

        return env;
    }

    void load_file(char const * path, Expr env)
    {
        Expr const in = make_file_input_stream_from_path(path);
        Expr exp = nil;
        while (maybe_parse_expr(in, &exp))
        {
            eval(exp, env);
        }
        stream_release(in);
    }
};

#endif /* _LISP_CPP_ */

#endif /* LISP_IMPLEMENTATION */
