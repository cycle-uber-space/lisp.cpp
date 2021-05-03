
/* config */

#ifndef LISP_WANT_GLOBAL_API
#define LISP_WANT_GLOBAL_API 1
#endif

#ifndef LISP_WANT_SYSTEM_API
#define LISP_WANT_SYSTEM_API 1
#endif

#ifndef LISP_DEBUG
#define LISP_DEBUG 1
#endif

#ifndef LISP_DEBUG_USE_SIGNAL
#define LISP_DEBUG_USE_SIGNAL 1
#endif

#ifndef LISP_SYMBOL_NAME_OF_NIL
#define LISP_SYMBOL_NAME_OF_NIL 1
#endif

#ifndef LISP_MALLOC
#define LISP_MALLOC(size) malloc(size)
#endif

#ifndef LISP_REALLOC
#define LISP_REALLOC(ptr, size) realloc(ptr, size)
#endif

#ifndef LISP_FREE
#define LISP_FREE(ptr) free(ptr)
#endif

#ifndef LISP_READER_PARSE_QUOTE
#define LISP_READER_PARSE_QUOTE 1
#endif

#ifndef LISP_READER_PARSE_CHARACTER
#define LISP_READER_PARSE_CHARACTER 1
#endif

#ifndef LISP_PRINTER_PRINT_QUOTE
#define LISP_PRINTER_PRINT_QUOTE 1
#endif

/* includes */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if LISP_DEBUG_USE_SIGNAL
#include <signal.h>
#endif

/* defines */

#define LISP_RED     "\x1b[31m"
#define LISP_GREEN   "\x1b[32m"
#define LISP_YELLOW  "\x1b[33m"
#define LISP_BLUE    "\x1b[34m"
#define LISP_MAGENTA "\x1b[35m"
#define LISP_CYAN    "\x1b[36m"
#define LISP_RESET   "\x1b[0m"

/* base */

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

union V64
{
    U64 u;
    I64 i;
    F64 f;
};

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

static_assert(sizeof(V64) == 8, "");

/* test */

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

/* error */

#define LISP_FAIL(...)    error_fail(__VA_ARGS__);
#define LISP_WARN(...)    error_warn(__VA_ARGS__);

#define LISP_ASSERT_ALWAYS(x) assert(x)

#define LISP_ASSERT(x) LISP_ASSERT_ALWAYS(x)

#if LISP_DEBUG
#define LISP_ASSERT_DEBUG(x) LISP_ASSERT(x)
#else
#define LISP_ASSERT_DEBUG(x)
#endif

void error_fail(char const * fmt, ...);
void error_warn(char const * fmt, ...);

/* HashMap */

template <typename Key, typename Value>
class HashMap
{
public:
    bool has(Key const & key) const
    {
        return m_impl.find(key) != m_impl.end();
    }

    void put(Key const & key, Value const & value)
    {
        m_impl[key] = value;
    }

    Value get(Key const & key) const
    {
        return m_impl.at(key);
    }

private:
    std::unordered_map<Key, Value> m_impl;
};

/* HashSet */

template <typename Element>
class HashSet
{
public:
    bool contains(Element const & element) const
    {
        return m_impl.find(element) != m_impl.end();
    }

    void add(Element const & element)
    {
        m_impl.insert(element);
    }

private:
    std::unordered_set<Element> m_impl;
};

/* expr */

#define LISP_TYPE_BITS UINT64_C(8)
#define LISP_TYPE_MASK ((UINT64_C(1) << LISP_TYPE_BITS) - UINT64_C(1))

#define LISP_DATA_BITS (UINT64_C(64) - LISP_TYPE_BITS)
#define LISP_DATA_MASK ((UINT64_C(1) << LISP_DATA_BITS) - UINT64_C(1))

#define LISP_EXPR_MASK UINT64_C(0xffffffffffffffff)

typedef U64 Expr;

Expr make_expr(U64 type, U64 data);
U64 expr_type(Expr exp);
U64 expr_data(Expr exp);

enum
{
    TYPE_NIL = 0,
    TYPE_SYMBOL,
    TYPE_KEYWORD,
    TYPE_CONS,
    TYPE_GENSYM,
    TYPE_CHAR,
    TYPE_FIXNUM,
    TYPE_STRING,
    TYPE_STREAM,
    TYPE_BUILTIN_SPECIAL,
    TYPE_BUILTIN_FUNCTION,
    TYPE_BUILTIN_SYMBOL,
};

enum
{
    DATA_NIL = 0,
};

inline bool eq(Expr a, Expr b)
{
    return a == b;
}

typedef struct SystemState SystemState;

/* nil */

#define nil 0

inline static bool is_nil(Expr exp)
{
    return exp == nil;
}

/* symbol */

#define LISP_SYMBOL_T intern("t")

#define LISP_SYM_QUOTE intern("quote")
#define LISP_SYM_IF intern("if")
#define LISP_SYM_BACKQUOTE intern("backquote")
#define LISP_SYM_UNQUOTE intern("unquote")
#define LISP_SYM_UNQUOTE_SPLICING intern("unquote-splicing")

inline bool is_symbol(Expr exp)
{
    return expr_type(exp) == TYPE_SYMBOL;
}

/* keyword */

inline bool is_keyword(Expr exp)
{
    return expr_type(exp) == TYPE_KEYWORD;
}

/* cons */

inline bool is_cons(Expr exp)
{
    return expr_type(exp) == TYPE_CONS;
}

/* gensym */

#if LISP_WANT_GLOBAL_API
bool is_gensym(Expr exp);
Expr gensym();
#endif

/* char */

/* fixnum */

/* string */

inline bool is_string(Expr exp)
{
    return expr_type(exp) == TYPE_STRING;
}

/* stream */

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

inline bool is_stream(Expr exp)
{
    return expr_type(exp) == TYPE_STREAM;
}

/* builtin */

typedef std::function<Expr(Expr args, Expr env)> BuiltinFunc;

struct BuiltinInfo
{
    char const * name;
    BuiltinFunc func;
};

inline bool is_builtin_special(Expr exp)
{
    return expr_type(exp) == TYPE_BUILTIN_SPECIAL;
}

inline bool is_builtin_function(Expr exp)
{
    return expr_type(exp) == TYPE_BUILTIN_FUNCTION;
}

inline bool is_builtin_symbol(Expr exp)
{
    return expr_type(exp) == TYPE_BUILTIN_SYMBOL;
}

inline bool is_builtin(Expr exp)
{
    return is_builtin_special(exp) || is_builtin_function(exp) || is_builtin_symbol(exp);
}

/* system */

typedef struct SystemState
{
    StreamState stream;
} SystemState;

class SystemImpl;

class System
{
public:
    System();
    virtual ~System();

    /* symbol */

    char const * symbol_name(Expr exp);

    /* cons */

    Expr cons(Expr a, Expr b);
    Expr car(Expr exp);
    Expr cdr(Expr exp);
    void rplaca(Expr exp, Expr val);
    void rplacd(Expr exp, Expr val);

    /* stream */

    Expr stream_get_stdin();
    Expr stream_get_stdout();
    Expr stream_get_stderr();

    /* core */

    Expr list(Expr exp1);
    Expr list(Expr exp1, Expr exp2);
    Expr list(Expr exp1, Expr exp2, Expr exp3);
    Expr list(Expr exp1, Expr exp2, Expr exp3, Expr exp4, Expr exp5);

    Expr first(Expr seq);
    Expr second(Expr seq);

    bool equal(Expr a, Expr b);

    /* print */

    char const * repr(Expr exp);
    void print(Expr exp);
    void println(Expr exp);
    void display(Expr exp);
    void displayln(Expr exp);

    /* read */

    Expr intern(char const * name);
    Expr read_one_from_string(char const * src);

    /* env */

    Expr make_env(Expr outer);
    virtual Expr make_core_env();

    void env_def(Expr env, Expr var, Expr val);
    void env_defun(Expr env, char const * name, BuiltinFunc func);
    void env_defun_println(Expr env, char const * name);
    void env_defspecial(Expr env, char const * name, BuiltinFunc func);
    void env_defspecial_quote(Expr env);
    void env_defspecial_while(Expr env);
    void env_defsym(Expr env, char const * name, BuiltinFunc func);
    void env_del(Expr env, Expr var);

    Expr env_get(Expr env, Expr var);
    bool env_can_set(Expr env, Expr var);
    void env_set(Expr env, Expr var, Expr val);

    /* function */

    Expr make_function(Expr env, Expr name, Expr args, Expr body);

    /* eval */

    Expr eval(Expr exp, Expr env);
    void load_file(char const * path, Expr env);
    void repl(Expr env);

private:
    SystemImpl * m_impl = nullptr;
    static System * s_instance;
};
