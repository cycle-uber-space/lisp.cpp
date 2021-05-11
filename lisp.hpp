
#ifndef _LISP_HPP_
#define _LISP_HPP_

#line 2 "src/config.decl"
/* config */

#ifndef LISP_WANT_GLOBAL_API
#define LISP_WANT_GLOBAL_API 1
#endif

#ifndef LISP_WANT_SYSTEM_API
#define LISP_WANT_SYSTEM_API 1
#endif

#ifndef LISP_WANT_GENSYM
#define LISP_WANT_GENSYM 1
#endif

#ifndef LISP_WANT_POINTER
#define LISP_WANT_POINTER 1
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

#line 2 "src/includes.decl"
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

#line 2 "src/defines.decl"
/* defines */

#define LISP_RED     "\x1b[31m"
#define LISP_GREEN   "\x1b[32m"
#define LISP_YELLOW  "\x1b[33m"
#define LISP_BLUE    "\x1b[34m"
#define LISP_MAGENTA "\x1b[35m"
#define LISP_CYAN    "\x1b[36m"
#define LISP_RESET   "\x1b[0m"

#line 2 "src/base.decl"
/* base */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

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

bool is_printable_ascii(U32 ch);

char * get_temp_buf(size_t size);

U64 i64_as_u64(I64 val);
I64 u64_as_i64(U64 val);

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/test.decl"
/* test */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

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

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/error.decl"
/* error */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

#define LISP_FAIL(...)    error_fail(__VA_ARGS__);
#define LISP_WARN(...)    error_warn(__VA_ARGS__);

#define LISP_ASSERT_ALWAYS(x) assert(x)

#define LISP_ASSERT(x) LISP_ASSERT_ALWAYS(x)

#if LISP_DEBUG
#define LISP_ASSERT_DEBUG(x) LISP_ASSERT(x)
#else
#define LISP_ASSERT_DEBUG(x)
#endif

class ErrorHandler
{
public:
    virtual ~ErrorHandler()
    {
    }

    void fail(char const * fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        vfail(fmt, ap);
        va_end(ap);
    }

    void warn(char const * fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        vwarn(fmt, ap);
        va_end(ap);
    }

    virtual void vfail(char const * fmt, va_list ap) = 0;
    virtual void vwarn(char const * fmt, va_list ap) = 0;
};

void error_fail(char const * fmt, ...);
void error_warn(char const * fmt, ...);

void error_push(ErrorHandler * handler);
void error_pop();

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/expr.decl"
/* expr */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

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
    // data embedded in expr
    TYPE_NIL = 0,
    TYPE_CHAR,
    TYPE_FIXNUM,
    // needs separate memory
    TYPE_SYMBOL,
    TYPE_KEYWORD,
    TYPE_CONS,
#if LISP_WANT_GENSYM
    TYPE_GENSYM,
#endif
#if LISP_WANT_POINTER
    TYPE_POINTER,
#endif
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

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/nil.decl"
/* nil */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

#define nil 0

inline static bool is_nil(Expr exp)
{
    return exp == nil;
}

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/symbol.decl"
/* symbol */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

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

#if LISP_WANT_GLOBAL_API

Expr make_symbol(char const * name);
char const * symbol_name(Expr exp);

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/keyword.decl"
/* keyword */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

inline bool is_keyword(Expr exp)
{
    return expr_type(exp) == TYPE_KEYWORD;
}

#if LISP_WANT_GLOBAL_API

Expr make_keyword(char const * name);

char const * keyword_name(Expr exp);

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/cons.decl"
/* cons */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

inline bool is_cons(Expr exp)
{
    return expr_type(exp) == TYPE_CONS;
}

#if LISP_WANT_GLOBAL_API

Expr cons(Expr exp1, Expr exp2);
Expr car(Expr exp);
Expr cdr(Expr exp);
void set_car(Expr exp, Expr val);
void set_cdr(Expr exp, Expr val);

inline void rplaca(Expr exp, Expr val) { set_car(exp, val); }
inline void rplacd(Expr exp, Expr val) { set_cdr(exp, val); }

inline Expr caar(Expr exp) { return car(car(exp)); }
inline Expr cadr(Expr exp) { return car(cdr(exp)); }
inline Expr cdar(Expr exp) { return cdr(car(exp)); }
inline Expr cddr(Expr exp) { return cdr(cdr(exp)); }

inline Expr caddr(Expr exp) { return car(cdr(cdr(exp))); }
inline Expr cdddr(Expr exp) { return cdr(cdr(cdr(exp))); }

inline Expr cadddr(Expr exp) { return car(cdr(cdr(cdr(exp)))); }
inline Expr cddddr(Expr exp) { return cdr(cdr(cdr(cdr(exp)))); }

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/gensym.decl"
/* gensym */

#if LISP_WANT_GENSYM

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

#if LISP_WANT_GLOBAL_API

bool is_gensym(Expr exp);
Expr gensym();

#endif

#ifdef LISP_NAMESPACE
}
#endif

#endif

#line 2 "src/pointer.decl"
/* pointer */

#if LISP_WANT_POINTER

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

#if LISP_WANT_GLOBAL_API

bool is_pointer(Expr exp);
Expr make_pointer(void * ptr);
void * pointer_value(Expr exp);

#endif

#ifdef LISP_NAMESPACE
}
#endif

#endif

#line 2 "src/fixnum.decl"
/* fixnum */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

inline bool is_fixnum(Expr exp)
{
    return expr_type(exp) == TYPE_FIXNUM;
}

Expr make_fixnum(I64 value);
I64 fixnum_value(Expr exp);

Expr fixnum_neg(Expr a);
Expr fixnum_add(Expr a, Expr b);
Expr fixnum_mul(Expr a, Expr b);
Expr fixnum_div(Expr a, Expr b);

bool fixnum_eq(Expr a, Expr b);
bool fixnum_lt(Expr a, Expr b);

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/char.decl"
/* char */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

inline bool is_char(Expr exp)
{
    return expr_type(exp) == TYPE_CHAR;
}

Expr make_char(U32 code);
U32 char_code(Expr exp);

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/string.decl"
/* string */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

inline bool is_string(Expr exp)
{
    return expr_type(exp) == TYPE_STRING;
}

#if LISP_WANT_GLOBAL_API

Expr make_string(char const * str);
Expr make_string_from_utf8(U8 const * str);
Expr make_string_from_utf32_char(U32 code);
char const * string_value(Expr exp);
U8 const * string_value_utf8(Expr exp);
U64 string_length(Expr exp);
bool string_equal(Expr exp1, Expr exp2);

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/stream.decl"
/* stream */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

struct StreamInfo
{
    FILE * file;
    bool close_on_quit;
    U32 peek;

    char * buffer;
    size_t size;
    size_t cursor;
};

inline bool is_stream(Expr exp)
{
    return expr_type(exp) == TYPE_STREAM;
}

#if LISP_WANT_GLOBAL_API

Expr make_file_input_stream_from_path(char const * path);

Expr make_string_input_stream(char const * str);
Expr make_buffer_output_stream(size_t size, char * str);

U32 stream_read_char(Expr exp);
U32 stream_peek_char(Expr exp);
void stream_skip_char(Expr exp);

void stream_put_cchar(Expr exp, char ch);
void stream_put_char(Expr exp, U32 ch);
void stream_put_cstring(Expr exp, char const * str);
void stream_put_u64(Expr exp, U64 val);
void stream_put_i64(Expr exp, U64 val);
void stream_put_x64(Expr exp, U64 val);
void stream_put_pointer(Expr exp, void const * ptr);

void stream_release(Expr exp);
bool stream_at_end(Expr exp);

Expr stream_get_stdin();
Expr stream_get_stdout();
Expr stream_get_stderr();

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/builtin.decl"
/* builtin */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

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

#if LISP_WANT_GLOBAL_API
Expr make_builtin_special(char const * name, BuiltinFunc func);
Expr make_builtin_function(char const * name, BuiltinFunc func);
Expr make_builtin_symbol(char const * name, BuiltinFunc func);

char const * builtin_name(Expr exp);
BuiltinFunc builtin_func(Expr exp);
#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/number.decl"
/* number */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

bool is_number(Expr exp);

Expr make_number(I64 value);

Expr number_neg(Expr a);
Expr number_add(Expr a, Expr b);
Expr number_mul(Expr a, Expr b);
Expr number_div(Expr a, Expr b);
bool number_equal(Expr a, Expr b);

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/core.decl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

Expr intern(char const * name);

bool is_named_call(Expr exp, Expr name);
bool is_quote_call(Expr exp);
bool is_unquote(Expr exp);
bool is_unquote_splicing(Expr exp);

Expr list(Expr exp1);
Expr list(Expr exp1, Expr exp2);
Expr list(Expr exp1, Expr exp2, Expr exp3);
Expr list(Expr exp1, Expr exp2, Expr exp3, Expr exp4, Expr exp5);

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/print.decl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

char const * repr(Expr exp);

void print(Expr exp);
void println(Expr exp);

void display(Expr exp);
void displayln(Expr exp);

Expr b_println(Expr args, Expr env);

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/read.decl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

bool maybe_parse_expr(Expr in, Expr * exp);
Expr read_one_from_string(char const * src);

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/closure.decl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

bool is_closure(Expr exp, Expr kind);
Expr closure_env(Expr exp);
Expr closure_args(Expr exp);
Expr closure_body(Expr exp);

bool is_function(Expr exp);
Expr make_function(Expr env, Expr name, Expr args, Expr body);

bool is_macro(Expr exp);
Expr make_macro(Expr env, Expr name, Expr args, Expr body);

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/env.decl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/system.decl"
/* system */

#if LISP_WANT_SYSTEM_API

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class SystemImpl;

class System
{
public:
    System();
    virtual ~System();

    /* core */

    Expr list(Expr exp1);
    Expr list(Expr exp1, Expr exp2);
    Expr list(Expr exp1, Expr exp2, Expr exp3);
    Expr list(Expr exp1, Expr exp2, Expr exp3, Expr exp4, Expr exp5);

    Expr first(Expr seq);
    Expr second(Expr seq);

    bool equal(Expr a, Expr b);

    /* read */

    Expr intern(char const * name);
    Expr read_one_from_string(char const * src);
    bool maybe_parse_expr(Expr in, Expr * exp);

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
    Expr eval_body(Expr exps, Expr env);
    void load_file(char const * path, Expr env);
    void repl(Expr env);

private:
    SystemImpl * m_impl = nullptr;
    static System * s_instance;
};

#ifdef LISP_NAMESPACE
}
#endif

#endif

#endif /* _LISP_HPP_ */

#ifdef LISP_IMPLEMENTATION

#ifndef _LISP_CPP_
#define _LISP_CPP_

#line 2 "src/HashMap.impl"
/* HashMap */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

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

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/HashSet.impl"
/* HashSet */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

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

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/test.impl"
/* test */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

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

void test_group(TestState *, char const * text)
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

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/error.impl"
/* error */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class PrintAndExit : public ErrorHandler
{
public:
    PrintAndExit(FILE * file, int code) : m_file(file), m_code(code)
    {
    }

    void vfail(char const * fmt, va_list ap)
    {
        fprintf(m_file, LISP_RED "[FAIL] " LISP_RESET);
        vfprintf(m_file, fmt, ap);
#if LISP_DEBUG_USE_SIGNAL
        raise(SIGINT);
#endif
        exit(m_code);
    }

    void vwarn(char const * fmt, va_list ap)
    {
        fprintf(m_file, LISP_YELLOW "[WARN] " LISP_RESET);
        vfprintf(m_file, fmt, ap);
    }

private:
    FILE * m_file;
    int m_code;
};

class ErrorSystem
{
public:
    ErrorSystem() : m_default(stderr, 1)
    {
    }

    void vfail(char const * fmt, va_list ap)
    {
        if (m_handlers.empty())
        {
            m_default.vfail(fmt, ap);
        }
        else
        {
            m_handlers.back()->vfail(fmt, ap);
        }
    }

    void vwarn(char const * fmt, va_list ap)
    {
        if (m_handlers.empty())
        {
            m_default.vwarn(fmt, ap);
        }
        else
        {
            m_handlers.back()->vwarn(fmt, ap);
        }
    }

    void push(ErrorHandler * handler)
    {
        m_handlers.push_back(handler);
    }

    void pop()
    {
        assert(!m_handlers.empty());
        m_handlers.pop_back();
    }

private:
    PrintAndExit m_default;
    std::vector<ErrorHandler *> m_handlers;
} g_error;

void error_fail(char const * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    g_error.vfail(fmt, ap);
    va_end(ap);
}

void error_warn(char const * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    g_error.vwarn(fmt, ap);
    va_end(ap);
}

void error_push(ErrorHandler * handler)
{
    g_error.push(handler);
}

void error_pop()
{
    g_error.pop();
}

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/expr.impl"
/* expr */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

Expr make_expr(U64 type, U64 data)
{
    return (data << LISP_TYPE_BITS) | (type & LISP_TYPE_MASK);
}

U64 expr_type(Expr exp)
{
    return exp & LISP_TYPE_MASK;
}

U64 expr_data(Expr exp)
{
    return exp >> LISP_TYPE_BITS;
}

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/base.impl"
/* base */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

bool is_printable_ascii(U32 ch)
{
    // NOTE we exclude space
    return ch >= 33 && ch <= 126;
}

#define LISP_TEMP_BUF_SIZE  4096
#define LISP_TEMP_BUF_COUNT 4

char * get_temp_buf(size_t size)
{
    LISP_ASSERT(size <= LISP_TEMP_BUF_SIZE);

    static char buf[LISP_TEMP_BUF_COUNT][LISP_TEMP_BUF_SIZE];
    static int idx = 0;
    char * ret = buf[idx];
    idx = (idx + 1) % LISP_TEMP_BUF_COUNT;
    return ret;
}

U64 i64_as_u64(I64 val)
{
    V64 v;
    v.i = val;
    return v.u;
}

I64 u64_as_i64(U64 val)
{
    V64 v;
    v.u = val;
    return v.i;
}

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/type.impl"
/* type */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class TypeImpl
{
public:
    U64 make(char const * name)
    {
        U64 const type = count();
        m_names.push_back(name);
        return type;
    }

    U64 count() const
    {
        return m_names.size();
    }

    char const * name(U64 type) const
    {
        LISP_ASSERT(type < count());
        return m_names[type].c_str();
    }

private:
    std::vector<std::string> m_names;
};

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/fixnum.impl"
/* fixnum */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

#define LISP_FIXNUM_SIGN_MASK (UINT64_C(1) << ((U64) LISP_DATA_BITS - UINT64_C(1)))
#define LISP_FIXNUM_BITS_MASK (LISP_EXPR_MASK >> (UINT64_C(64) + UINT64_C(1) - (U64) LISP_DATA_BITS))
#define LISP_FIXNUM_MINVAL    (-(INT64_C(1) << ((I64) LISP_DATA_BITS - INT64_C(1))))
#define LISP_FIXNUM_MAXVAL    ((INT64_C(1) << ((I64) LISP_DATA_BITS - INT64_C(1))) - INT64_C(1))

Expr make_fixnum(I64 value)
{
    LISP_ASSERT(value >= LISP_FIXNUM_MINVAL);
    LISP_ASSERT(value <= LISP_FIXNUM_MAXVAL);

    /* TODO probably no need to mask off the sign
       bits, as they get shifted out by make_expr */
    U64 const data = i64_as_u64(value) & LISP_DATA_MASK;
    return make_expr(TYPE_FIXNUM, data);
}

I64 fixnum_value(Expr exp)
{
    LISP_ASSERT(is_fixnum(exp));

    U64 data = expr_data(exp);

    if (data & LISP_FIXNUM_SIGN_MASK)
    {
        data |= LISP_EXPR_MASK << LISP_DATA_BITS;
    }

    return u64_as_i64(data);
}

Expr fixnum_neg(Expr a)
{
    return make_fixnum(-fixnum_value(a));
}

Expr fixnum_add(Expr a, Expr b)
{
    return make_fixnum(fixnum_value(a) + fixnum_value(b));
}

Expr fixnum_mul(Expr a, Expr b)
{
    return make_fixnum(fixnum_value(a) * fixnum_value(b));
}

Expr fixnum_div(Expr a, Expr b)
{
    return make_fixnum(fixnum_value(a) / fixnum_value(b));
}

bool fixnum_eq(Expr a, Expr b)
{
    return a == b;
}

bool fixnum_lt(Expr a, Expr b)
{
    return fixnum_value(a) < fixnum_value(b);
}

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/char.impl"
/* char */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

Expr make_char(U32 code)
{
    return make_expr(TYPE_CHAR, code);
}

U32 char_code(Expr exp)
{
    LISP_ASSERT(is_char(exp));
    return (U32) expr_data(exp);
}

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/symbol.impl"
/* symbol */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class SymbolImpl
{
public:
    SymbolImpl(U64 type) : m_type(type)
    {
    }

    inline bool isinstance(Expr exp) const
    {
        return expr_type(exp) == m_type;
    }

    Expr make(char const * name)
    {
        LISP_ASSERT_DEBUG(name);

        // TODO check hash set of names first
        for (U64 index = 0; index < count(); ++index)
        {
            if (m_names[index] == name)
            {
                return make_expr(m_type, index);
            }
        }

        U64 const index = count();
        m_names.push_back(name);
        return make_expr(m_type, index);
    }

    char const * name(Expr exp)
    {
    #if LISP_SYMBOL_NAME_OF_NIL
        if (exp == nil)
        {
            return "nil";
        }
    #endif

        LISP_ASSERT(isinstance(exp));
        U64 const index = expr_data(exp);
        if (index >= count())
        {
            LISP_FAIL("illegal symbol index %" PRIu64 "\n", index);
        }
        return m_names[index].c_str();
    }

protected:
    U64 count() const
    {
        return (U64) m_names.size();
    }

private:
    U64 m_type;
    std::vector<std::string> m_names;
};

#if LISP_WANT_GLOBAL_API

static SymbolImpl g_symbol(TYPE_SYMBOL);

Expr make_symbol(char const * name)
{
    return g_symbol.make(name);
}

char const * symbol_name(Expr exp)
{
    return g_symbol.name(exp);
}

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/keyword.impl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

#if LISP_WANT_GLOBAL_API

SymbolImpl g_keyword(TYPE_KEYWORD);

Expr make_keyword(char const * name)
{
    return g_keyword.make(name);
}

char const * keyword_name(Expr exp)
{
    return g_keyword.name(exp);
}

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/cons.impl"
/* cons */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

struct ExprPair
{
    Expr exp1, exp2;
};

class ConsImpl
{
public:
    ConsImpl(U64 type) : m_type(type)
    {
    }

    inline bool isinstance(Expr exp) const
    {
        return expr_type(exp) == m_type;
    }

    Expr make(Expr a, Expr b)
    {
        U64 const index = count();
        ExprPair pair;
        pair.exp1 = a;
        pair.exp2 = b;
        m_pairs.push_back(pair);
        return make_expr(m_type, index);
    }

    Expr car(Expr exp)
    {
        LISP_ASSERT(isinstance(exp));
        Expr const index = expr_data(exp);
        LISP_ASSERT(index < count());
        return m_pairs[index].exp1;
    }

    Expr cdr(Expr exp)
    {
        LISP_ASSERT(isinstance(exp));
        Expr const index = expr_data(exp);
        LISP_ASSERT(index < count());
        return m_pairs[index].exp2;
    }

    void set_car(Expr exp, Expr val)
    {
        LISP_ASSERT(isinstance(exp));
        Expr const index = expr_data(exp);
        LISP_ASSERT(index < count());
        m_pairs[index].exp1 = val;
    }

    void set_cdr(Expr exp, Expr val)
    {
        LISP_ASSERT(isinstance(exp));
        Expr const index = expr_data(exp);
        LISP_ASSERT(index < count());
        m_pairs[index].exp2 = val;
    }

protected:
    U64 count() const
    {
        return (U64) m_pairs.size();
    }

private:
    U64 m_type;
    std::vector<ExprPair> m_pairs;
};

template <typename T>
class ConsMixin
{
public:
    ConsMixin(ConsImpl & impl) : m_impl(impl)
    {
    }

    Expr cons(Expr exp1, Expr exp2)
    {
        return m_impl.make(exp1, exp2);
    }

    Expr car(Expr exp)
    {
        return m_impl.car(exp);
    }

    Expr cdr(Expr exp)
    {
        return m_impl.cdr(exp);
    }

    void rplaca(Expr exp, Expr val)
    {
        m_impl.set_car(exp, val);
    }

    void rplacd(Expr exp, Expr val)
    {
        m_impl.set_cdr(exp, val);
    }

    inline Expr caar(Expr exp)
    {
        return car(car(exp));
    }

    inline Expr cadr(Expr exp)
    {
        return car(cdr(exp));
    }

    inline Expr cdar(Expr exp)
    {
        return cdr(car(exp));
    }

    inline Expr cddr(Expr exp)
    {
        return cdr(cdr(exp));
    }

    inline Expr caddr(Expr exp)
    {
        return car(cdr(cdr(exp)));
    }

    inline Expr cdddr(Expr exp)
    {
        return cdr(cdr(cdr(exp)));
    }

    inline Expr cadddr(Expr exp)
    {
        return car(cdr(cdr(cdr(exp))));
    }

    inline Expr cddddr(Expr exp)
    {
        return cdr(cdr(cdr(cdr(exp))));
    }

private:
    ConsImpl & m_impl;
};

#if LISP_WANT_GLOBAL_API

static ConsImpl g_cons(TYPE_CONS);

Expr cons(Expr exp1, Expr exp2)
{
    return g_cons.make(exp1, exp2);
}

Expr car(Expr exp)
{
    return g_cons.car(exp);
}

Expr cdr(Expr exp)
{
    return g_cons.cdr(exp);
}

void set_car(Expr exp, Expr val)
{
    g_cons.set_car(exp, val);
}

void set_cdr(Expr exp, Expr val)
{
    g_cons.set_cdr(exp, val);
}

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/string.impl"
/* string */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class StringImpl
{
public:
    StringImpl(U64 type) : m_type(type)
    {
    }

    inline bool isinstance(Expr exp) const
    {
        return expr_type(exp) == m_type;
    }

    Expr make(char const * str)
    {
        U64 const index = count();
        m_strings.push_back(str);
        return make_expr(m_type, index);
    }

    char const * value(Expr exp)
    {
        return impl(exp).c_str();
    }

    U64 length(Expr exp)
    {
        return (U64) impl(exp).size();
    }

    bool equal(Expr exp1, Expr exp2)
    {
        return impl(exp1) == impl(exp2);
    }

protected:
    U64 count() const
    {
        return (U64) m_strings.size();
    }

    std::string const & impl(Expr exp)
    {
        LISP_ASSERT(isinstance(exp));
        U64 const index = expr_data(exp);
        LISP_ASSERT(index < count());
        return m_strings[index];
    }

private:
    U64 m_type;
    std::vector<std::string> m_strings;
};

#if LISP_WANT_GLOBAL_API

StringImpl g_string(TYPE_STRING);

Expr make_string(char const * str)
{
    return g_string.make(str);
}

Expr make_string_from_utf8(U8 const * str)
{
    // TODO assert this works?
    return make_string((char const *) str);
}

Expr make_string_from_utf32_char(U32 code)
{
    // TODO use string output stream
    U8 bytes[5];
    U8 * out_bytes = bytes;

    if (code < 0x80)
    {
        *out_bytes++ = (U8) code;
    }
    else if (code < 0x800)
    {
        *out_bytes++ = (U8) (0xc0 | ((code >> 6) & 0x1f));
        *out_bytes++ = (U8) (0x80 | (code & 0x3f));
    }
    else if ((code >= 0xd800 && code < 0xe000))
    {
        LISP_FAIL("illegal code point %" PRIu64 "\n", code);
    }
    else if (code < 0x10000)
    {
        *out_bytes++ = (U8) (0xe0 | ((code >> 12) & 0xf));
        *out_bytes++ = (U8) (0x80 | ((code >> 6) & 0x3f));
        *out_bytes++ = (U8) (0x80 | (code & 0x3f));
    }
    else if (code <= 0x10ffff)
    {
        *out_bytes++ = (U8) (0xf0 | ((code >> 18) & 0x7));
        *out_bytes++ = (U8) (0x80 | ((code >> 12) & 0x3f));
        *out_bytes++ = (U8) (0x80 | ((code >> 6) & 0x3f));
        *out_bytes++ = (U8) (0x80 | (code & 0x3f));
    }
    else
    {
        LISP_FAIL("illegal code point %" PRIu64 " for utf-8 encoder\n", code);
    }

    *out_bytes++ = 0;
    return make_string_from_utf8(bytes);
}

char const * string_value(Expr exp)
{
    return g_string.value(exp);
}

U8 const * string_value_utf8(Expr exp)
{
    // TODO actually change internal representation
    return (U8 const *) string_value(exp);
}

U64 string_length(Expr exp)
{
    return g_string.length(exp);
}

bool string_equal(Expr exp1, Expr exp2)
{
    return g_string.equal(exp1, exp2);
}

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/stream.impl"
/* stream */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class StreamImpl
{
public:
    StreamImpl()
    {
        m_stdin = make_file_input(stdin, false);
        m_stdout = make_file_output(stdout, false);
        m_stderr = make_file_output(stderr, false);
    }

    ~StreamImpl()
    {
        // TODO add this to stream api?
        for (auto & info : m_info)
        {
            if (info.close_on_quit)
            {
                if (info.file)
                {
                    fclose(info.file);
                    info.file = NULL;
                }
            }
        }
        m_info.clear();
    }

    Expr get_stdin()
    {
        return m_stdin;
    }

    Expr get_stdout()
    {
        return m_stdout;
    }

    Expr get_stderr()
    {
        return m_stderr;
    }

    Expr make_file_input(FILE * file, bool close_on_quit)
    {
        return make_file(file, close_on_quit);
    }

    Expr make_file_output(FILE * file, bool close_on_quit)
    {
        return make_file(file, close_on_quit);
    }

    Expr make_string_input(char const * str)
    {
        // TODO copy string into buffer?
        size_t const len = strlen(str);
        return make_buffer(len + 1, (char *) str);
    }

    Expr make_buffer_output(size_t size, char * buffer)
    {
        return make_buffer(size, buffer);
    }

    U8 read_byte(Expr exp)
    {
        StreamInfo & info = get_info(exp);
        if (info.file)
        {
            int ch = fgetc(info.file);
            if (ch == EOF)
            {
                return 0;
            }
            return (U8) ch;
        }

        if (info.buffer)
        {
            return info.buffer[info.cursor++];
        }

        //LISP_FAIL("cannot read from stream %s\n", repr(exp));
        LISP_FAIL("cannot read from stream\n");
        return 0;
    }

    U32 do_read_char(Expr exp)
    {
        U8 ch = read_byte(exp);
        if (ch & 0x80)
        {
            U32 val = 0;
            if ((ch >> 5) == 0x6)
            {
                val |= ch & 0x1f;

                ch = read_byte(exp);
                val <<= 6;
                val |= (ch & 0x3f);
            }
            else if ((ch >> 4) == 0xe)
            {
                val |= ch & 0xf;

                ch = read_byte(exp);
                val <<= 6;
                val |= (ch & 0x3f);

                ch = read_byte(exp);
                val <<= 6;
                val |= (ch & 0x3f);
            }
            else if ((ch >> 3) == 0x1e)
            {
                val |= ch & 0x7;

                ch = read_byte(exp);
                val <<= 6;
                val |= (ch & 0x3f);

                ch = read_byte(exp);
                val <<= 6;
                val |= (ch & 0x3f);

                ch = read_byte(exp);
                val <<= 6;
                val |= (ch & 0x3f);
            }
            if (val == 0 || (val >= 0xd000 && val <= 0xdfff) || val >= 0x10ffff)
            {
                LISP_FAIL("illegal UTF-8\n");
            }

            return val;
        }
        else
        {
            return ch;
        }
    }

    U32 read_char(Expr exp)
    {
        StreamInfo & info = get_info(exp);
        if (info.peek)
        {
            auto const ret = info.peek;
            info.peek = 0;
            return ret;
        }

        return do_read_char(exp);
    }

    U32 peek_char(Expr exp)
    {
        StreamInfo & info = get_info(exp);
        if (!info.peek)
        {
            info.peek = read_char(exp);
        }
        return info.peek;
    }

    void skip_char(Expr exp)
    {
        read_char(exp);
    }

    bool at_end(Expr exp)
    {
        return peek_char(exp) == 0;
    }

    void put_char(Expr exp, U32 code)
    {
        U8 bytes[5] = { 0 };
        U8 * out_bytes = bytes;
        if (code < 0x80)
        {
            *out_bytes++ = (U8) code;
        }
        else if (code < 0x800)
        {
            *out_bytes++ = (U8) (0xc0 | ((code >> 6) & 0x1f));
            *out_bytes++ = (U8) (0x80 | (code & 0x3f));
        }
        else if ((code >= 0xd800 && code < 0xe000))
        {
            LISP_FAIL("illegal code point %" PRIu64 "\n", code);
        }
        else if (code < 0x10000)
        {
            *out_bytes++ = (U8) (0xe0 | ((code >> 12) & 0xf));
            *out_bytes++ = (U8) (0x80 | ((code >> 6) & 0x3f));
            *out_bytes++ = (U8) (0x80 | (code & 0x3f));
        }
        else if (code <= 0x10ffff)
        {
            *out_bytes++ = (U8) (0xf0 | ((code >> 18) & 0x7));
            *out_bytes++ = (U8) (0x80 | ((code >> 12) & 0x3f));
            *out_bytes++ = (U8) (0x80 | ((code >> 6) & 0x3f));
            *out_bytes++ = (U8) (0x80 | (code & 0x3f));
        }
        else
        {
            LISP_FAIL("illegal code point %" PRIu64 " for utf-8 encoder\n", code);
        }

        put_bytes(exp, out_bytes - bytes, bytes);
    }

    void put_bytes(Expr exp, size_t size, U8 const * bytes)
    {
        StreamInfo & info = get_info(exp);
        if (info.file)
        {
            fwrite(bytes, size, 1, info.file);
            return;
        }

        if (info.buffer)
        {
            LISP_ASSERT(info.cursor + size < info.size);
            memcpy(info.buffer + info.cursor, bytes, size);
            info.cursor += size;
            return;
        }

        LISP_FAIL("cannot put bytes into stream\n");
    }

    void put_cstring(Expr exp, char const * str)
    {
        put_bytes(exp, strlen(str), (U8 const *) str);
    }

    void put_cchar(Expr exp, char ch)
    {
        //LISP_ASSERT(ch < 0x80);
        U8 val = (U8) ch;
        put_bytes(exp, 1, &val);
    }

    void release(Expr exp)
    {
        auto & info = get_info(exp);
        if (info.close_on_quit)
        {
            if (info.file)
            {
                fclose(info.file);
                info.file = NULL;
            }
        }

        // TODO add to free list
    }

protected:
    U64 count() const
    {
        return (U64) m_info.size();
    }

    StreamInfo & get_info(Expr exp)
    {
        LISP_ASSERT(is_stream(exp));
        U64 const index = expr_data(exp);
        LISP_ASSERT(index < count());
        return m_info[index];
    }

    Expr make_file(FILE * file, bool close_on_quit)
    {
        StreamInfo info;
        memset(&info, 0, sizeof(StreamInfo));
        info.file = file;
        info.close_on_quit = close_on_quit;
        return make_from_info(info);
    }

    Expr make_buffer(size_t size, char * buffer)
    {
        StreamInfo info;
        memset(&info, 0, sizeof(StreamInfo));
        info.size = size;
        info.buffer = buffer;
        info.cursor = 0;
        return make_from_info(info);
    }

    Expr make_from_info(StreamInfo const & info)
    {
        U64 const index = count();
        m_info.push_back(info);
        return make_expr(TYPE_STREAM, index);
    }

    StreamInfo & info(Expr exp)
    {
        LISP_ASSERT(is_stream(exp));
        Expr const index = expr_data(exp);
        LISP_ASSERT(index < count());
        return m_info[index];
    }

private:
    Expr m_stdin;
    Expr m_stdout;
    Expr m_stderr;
    std::vector<StreamInfo> m_info;
};

#if LISP_WANT_GLOBAL_API

StreamImpl g_stream;

Expr make_file_input_stream_from_path(char const * path)
{
    FILE * file = fopen(path, "rb");
    LISP_ASSERT(file);
    return g_stream.make_file_input(file, true);
}

Expr make_string_input_stream(char const * str)
{
    return g_stream.make_string_input(str);
}

Expr make_buffer_output_stream(size_t size, char * str)
{
    return g_stream.make_buffer_output(size, str);
}

U32 stream_read_char(Expr exp)
{
    return g_stream.read_char(exp);
}

U32 stream_peek_char(Expr exp)
{
    return g_stream.peek_char(exp);
}

void stream_skip_char(Expr exp)
{
    g_stream.skip_char(exp);
}

void stream_put_cchar(Expr exp, char ch)
{
    g_stream.put_cchar(exp, ch);
}

void stream_put_char(Expr exp, U32 ch)
{
    g_stream.put_char(exp, ch);
}

void stream_put_cstring(Expr exp, char const * str)
{
    g_stream.put_cstring(exp, str);
}

void stream_put_u64(Expr exp, U64 val)
{
    char str[32];
    sprintf(str, "%" PRIu64, val);
    stream_put_cstring(exp, str);
}

void stream_put_i64(Expr exp, U64 val)
{
    char str[32];
    sprintf(str, "%" PRIi64, val);
    stream_put_cstring(exp, str);
}

void stream_put_x64(Expr exp, U64 val)
{
    char str[32];
    sprintf(str, "%016" PRIx64, val);
    stream_put_cstring(exp, str);
}

void stream_put_pointer(Expr exp, void const * ptr)
{
    char str[32];
    sprintf(str, "%p", ptr);
    stream_put_cstring(exp, str);
}

void stream_release(Expr exp)
{
    g_stream.release(exp);
}

bool stream_at_end(Expr exp)
{
    return g_stream.at_end(exp);
}

Expr stream_get_stdin()
{
    return g_stream.get_stdin();
}

Expr stream_get_stdout()
{
    return g_stream.get_stdout();
}

Expr stream_get_stderr()
{
    return g_stream.get_stderr();
}

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/gensym.impl"
/* gensym */

#if LISP_WANT_GENSYM

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class GensymImpl
{
public:
    GensymImpl(U64 type) : m_type(type), m_counter(0)
    {
    }

    inline bool isinstance(Expr exp) const
    {
        return expr_type(exp) == m_type;
    }

    Expr make()
    {
        U64 const data = m_counter++;
        return make_expr(m_type, data);
    }

private:
    U64 m_type;
    U64 m_counter;
};

#if LISP_WANT_GLOBAL_API

GensymImpl g_gensym(TYPE_GENSYM);

bool is_gensym(Expr exp)
{
    return g_gensym.isinstance(exp);
}

Expr gensym()
{
    return g_gensym.make();
}

#endif

#ifdef LISP_NAMESPACE
}
#endif

#endif

#line 2 "src/pointer.impl"
/* pointer */

#if LISP_WANT_POINTER

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class PointerImpl
{
public:
    PointerImpl(U64 type) : m_type(type)
    {
    }

    inline bool isinstance(Expr exp) const
    {
        return expr_type(exp) == m_type;
    }

    Expr make(void * ptr)
    {
        U64 const index = count();
        m_values.push_back(ptr);
        return make_expr(m_type, index);
    }

    void * value(Expr exp)
    {
        LISP_ASSERT(isinstance(exp));
        auto const index = expr_data(exp);
        LISP_ASSERT(index < count());
        return m_values[index];
    }

protected:
    U64 count() const
    {
        return m_values.size();
    }

private:
    U64 m_type;
    std::vector<void *> m_values;
};

#if LISP_WANT_GLOBAL_API
PointerImpl g_pointer(TYPE_POINTER);

bool is_pointer(Expr exp)
{
    return g_pointer.isinstance(exp);
}

Expr make_pointer(void * ptr)
{
    return g_pointer.make(ptr);
}

void * pointer_value(Expr exp)
{
    return g_pointer.value(exp);
}

#endif

#ifdef LISP_NAMESPACE
}
#endif

#endif

#line 2 "src/builtin.impl"
/* builtin */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class BuiltinImpl
{
public:
    Expr make_special(char const * name, BuiltinFunc func)
    {
        return make(name, func, TYPE_BUILTIN_SPECIAL);
    }

    Expr make_function(char const * name, BuiltinFunc func)
    {
        return make(name, func, TYPE_BUILTIN_FUNCTION);
    }

    Expr make_symbol(char const * name, BuiltinFunc func)
    {
        return make(name, func, TYPE_BUILTIN_SYMBOL);
    }

    char const * name(Expr exp)
    {
        return info(exp).name;
    }

    BuiltinFunc func(Expr exp)
    {
        return info(exp).func;
    }

protected:
    U64 count() const
    {
        return (U64) m_info.size();
    }

    Expr make(char const * name, BuiltinFunc func, U64 type)
    {
        U64 const index = count();
        BuiltinInfo info;
        info.name = name; /* TODO take ownership of name? */
        info.func = func;
        m_info.push_back(info);
        return make_expr(type, index);
    }

    BuiltinInfo & info(Expr exp)
    {
        LISP_ASSERT(is_builtin(exp));
        U64 const index = expr_data(exp);
        LISP_ASSERT(index < count());
        return m_info[index];
    }

private:
    std::vector<BuiltinInfo> m_info;
};

#if LISP_WANT_GLOBAL_API

BuiltinImpl g_builtin;

Expr make_builtin_special(char const * name, BuiltinFunc func)
{
    return g_builtin.make_special(name, func);
}

Expr make_builtin_function(char const * name, BuiltinFunc func)
{
    return g_builtin.make_function(name, func);
}

Expr make_builtin_symbol(char const * name, BuiltinFunc func)
{
    return g_builtin.make_symbol(name, func);
}

char const * builtin_name(Expr exp)
{
    return g_builtin.name(exp);
}

BuiltinFunc builtin_func(Expr exp)
{
    return g_builtin.func(exp);
}

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/number.impl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

bool is_number(Expr exp)
{
    return is_fixnum(exp);
}

Expr make_number(I64 value)
{
    return make_fixnum(value);
}

Expr number_neg(Expr a)
{
    return fixnum_neg(a);
}

Expr number_add(Expr a, Expr b)
{
    return fixnum_add(a, b);
}

Expr number_mul(Expr a, Expr b)
{
    return fixnum_mul(a, b);
}

Expr number_div(Expr a, Expr b)
{
    return fixnum_div(a, b);
}

bool number_equal(Expr a, Expr b)
{
    return fixnum_eq(a, b);
}

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/core.impl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class CoreImpl
{
public:
    CoreImpl(SymbolImpl & symbol, SymbolImpl & keyword) : m_symbol(symbol), m_keyword(keyword)
    {
    }

    Expr intern(char const * name)
    {
        if (!strcmp("nil", name))
        {
            return nil;
        }
        else if (name[0] == ':')
        {
            return make_keyword(name + 1);
        }
        else
        {
            return make_symbol(name);
        }
    }

protected:
    Expr make_symbol(char const * name)
    {
        return m_symbol.make(name);
    }

    Expr make_keyword(char const * name)
    {
        return m_keyword.make(name);
    }

private:
    SymbolImpl & m_symbol;
    SymbolImpl & m_keyword;
};

#if LISP_WANT_GLOBAL_API

CoreImpl g_core(g_symbol, g_keyword);

Expr intern(char const * name)
{
    return g_core.intern(name);
}

bool is_named_call(Expr exp, Expr name)
{
    return is_cons(exp) && eq(car(exp), name);
}

bool is_quote_call(Expr exp)
{
    return is_named_call(exp, intern("quote"));
}

bool is_unquote(Expr exp)
{
    return is_named_call(exp, LISP_SYM_UNQUOTE);
}

bool is_unquote_splicing(Expr exp)
{
    return is_named_call(exp, LISP_SYM_UNQUOTE_SPLICING);
}

Expr list(Expr exp1)
{
    return cons(exp1, nil);
}

Expr list(Expr exp1, Expr exp2)
{
    return cons(exp1, cons(exp2, nil));
}

Expr list(Expr exp1, Expr exp2, Expr exp3)
{
    return cons(exp1, cons(exp2, cons(exp3, nil)));
}

Expr list(Expr exp1, Expr exp2, Expr exp3, Expr exp4, Expr exp5)
{
    return cons(exp1, cons(exp2, cons(exp3, cons(exp4, cons(exp5, nil)))));
}

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/print.impl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class PrintImpl
{
public:
    char const * repr(Expr exp)
    {
        // TODO multiple calls => need temp buffer per call
        size_t const size = 4096;
        char * buffer = get_temp_buf(size);
        Expr out = make_buffer_output_stream(size, buffer);
        HashSet<Expr> seen;
        print_expr(exp, out, seen);
        stream_release(out);
        return buffer;
    }

    void print(Expr exp)
    {
        Expr const out = stream_get_stdout();
        HashSet<Expr> seen;
        print_expr(exp, out, seen);
    }

    void println(Expr exp)
    {
        Expr const out = stream_get_stdout();
        HashSet<Expr> seen;
        print_expr(exp, out, seen);
        stream_put_char(out, '\n');
    }

    void display(Expr exp)
    {
        Expr const out = stream_get_stdout();
        HashSet<Expr> seen;
        display_expr(exp, out, seen);
    }

    void displayln(Expr exp)
    {
        Expr const out = stream_get_stdout();
        HashSet<Expr> seen;
        display_expr(exp, out, seen);
        stream_put_char(out, '\n');
    }

    void display_expr(Expr exp, Expr out, HashSet<Expr> & seen)
    {
        switch (expr_type(exp))
        {
        case TYPE_STRING:
            stream_put_cstring(out, string_value(exp));
            break;
        case TYPE_CHAR:
            stream_put_char(out, char_code(exp));
            break;
        default:
            print_expr(exp, out, seen);
            break;
        }
    }

    void print_expr(Expr exp, Expr out, HashSet<Expr> & seen)
    {
        switch (expr_type(exp))
        {
        case TYPE_NIL:
            LISP_ASSERT_DEBUG(expr_data(exp) == 0);
            stream_put_cstring(out, "nil");
            break;
        case TYPE_KEYWORD:
            stream_put_char(out, ':');
            stream_put_cstring(out, keyword_name(exp));
            break;
        case TYPE_SYMBOL:
            stream_put_cstring(out, symbol_name(exp));
            break;
        case TYPE_CONS:
            print_cons(exp, out, seen);
            break;
#if LISP_WANT_GENSYM
        case TYPE_GENSYM:
            stream_put_cstring(out, "#:G");
            stream_put_u64(out, expr_data(exp));
            break;
#endif
#if LISP_WANT_POINTER
        case TYPE_POINTER:
            stream_put_cstring(out, "#:<pointer ");
            stream_put_pointer(out, pointer_value(exp));
            stream_put_cstring(out, ">");
            break;
#endif
        case TYPE_CHAR:
            print_char(exp, out);
            break;
        case TYPE_FIXNUM:
            stream_put_i64(out, fixnum_value(exp));
            break;
        case TYPE_STRING:
            print_string(exp, out);
            break;
        case TYPE_BUILTIN_SPECIAL:
            print_builtin_special(exp, out);
            break;
        case TYPE_BUILTIN_FUNCTION:
            print_builtin_function(exp, out);
            break;
        case TYPE_BUILTIN_SYMBOL:
            print_builtin_symbol(exp, out);
            break;
        default:
            LISP_FAIL("cannot print expression %016" PRIx64 "\n", exp);
            break;
        }
    }

    void print_cons(Expr exp, Expr out, HashSet<Expr> & seen)
    {
#if LISP_PRINTER_PRINT_QUOTE
        if (is_quote_call(exp))
        {
            stream_put_char(out, '\'');
            print_expr(cadr(exp), out, seen);
            return;
        }
#endif

        if (seen.contains(exp))
        {
            stream_put_cstring(out, "...");
            return;
        }
        seen.add(exp);

        stream_put_char(out, '(');
        print_expr(car(exp), out, seen);

        for (Expr tmp = cdr(exp); tmp; tmp = cdr(tmp))
        {
            if (tmp == exp)
            {
                stream_put_cstring(out, " ...");
                break;
            }
            else if (is_cons(tmp))
            {
                stream_put_char(out, ' ');
                print_expr(car(tmp), out, seen);
            }
            else
            {
                stream_put_cstring(out, " . ");
                print_expr(tmp, out, seen);
                break;
            }
        }

        stream_put_char(out, ')');
    }

    void print_builtin(Expr exp, Expr out, char const * flavor)
    {
        stream_put_cstring(out, "#:<");
        stream_put_cstring(out, flavor);
        char const * name = builtin_name(exp);
        if (name)
        {
            stream_put_cstring(out, " ");
            stream_put_cstring(out, name);
        }
        stream_put_cstring(out, ">");
    }

    void print_builtin_special(Expr exp, Expr out)
    {
        print_builtin(exp, out, "special operator");
    }

    void print_builtin_function(Expr exp, Expr out)
    {
        print_builtin(exp, out, "core function");
    }

    void print_builtin_symbol(Expr exp, Expr out)
    {
        print_builtin(exp, out, "symbol macro");
    }

    void print_char(Expr exp, Expr out)
    {
        LISP_ASSERT_DEBUG(is_char(exp));
        U32 const code = char_code(exp);
        // TODO
        if (is_printable_ascii(code))
        {
            stream_put_char(out, '\\');
            stream_put_char(out, (char) code);
        }
        else if (code == '\a')
        {
            stream_put_cstring(out, "\\bel");
        }
        else if (code == ' ')
        {
            stream_put_cstring(out, "\\space");
        }
        else
        {
            LISP_FAIL("cannot render character %" PRIu32 "\n", code);
        }
    }

    void print_string(Expr exp, Expr out)
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
                    stream_put_cchar(out, ch);
                }
                break;
            }
        }
        stream_put_char(out, '"');
    }


    Expr b_println(Expr args, Expr /*env*/)
    {
        {
            Expr out = stream_get_stdout();
            for (Expr tmp = args; tmp; tmp = cdr(tmp))
            {
                if (tmp != args)
                {
                    stream_put_char(out, ' ');
                }
                Expr exp = car(tmp);
                HashSet<Expr> seen;
                print_expr(exp, out, seen);
            }
            stream_put_char(out, '\n');
            return nil;
        }
    }
};

PrintImpl g_print;

char const * repr(Expr exp)
{
    return g_print.repr(exp);
}

void print(Expr exp)
{
    g_print.print(exp);
}

void println(Expr exp)
{
    g_print.println(exp);
}

void display(Expr exp)
{
    g_print.display(exp);
}

void displayln(Expr exp)
{
    g_print.displayln(exp);
}

Expr b_println(Expr args, Expr env)
{
    return g_print.b_println(args, env);
}

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/read.impl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class ReadImpl
{
public:
    Expr read_one_from_string(char const * src)
    {
        Expr const in = make_string_input_stream(src);
        Expr ret = parse_expr(in);
        stream_release(in);
        //println(ret);
        return ret;
    }

    bool maybe_parse_expr(Expr in, Expr * exp)
    {
        skip_whitespace_or_comment(in);
        if (stream_at_end(in))
        {
            return false;
        }
        *exp = parse_expr(in);
        return true;
    }

    Expr parse_expr(Expr in)
    {
        skip_whitespace_or_comment(in);

        char lexeme[4096] = { 0 };
        Expr tok = nil;

        if (stream_peek_char(in) == '(')
        {
            return parse_list(in);
        }
        else if (stream_peek_char(in) == '"')
        {
            return parse_string(in);
        }
#if LISP_READER_PARSE_CHARACTER
        else if (stream_peek_char(in) == '\\')
        {
            tok = make_buffer_output_stream(4096, lexeme);

            while (!is_whitespace_or_eof(stream_peek_char(in)))
            {
                stream_put_char(tok, stream_read_char(in));
            }
            stream_put_char(tok, 0);
            stream_release(tok);

            // handle printable characters
            if (lexeme[2] == 0)
            {
                return make_char(lexeme[1]);
            }
            else if (!strcmp("\\bel", lexeme))
            {
                return make_char('\a');
            }
            else if (!strcmp("\\space", lexeme))
            {
                return make_char(' ');
            }
            else
            {
                LISP_FAIL("cannot parse character '%s'\n", lexeme);
                return nil;
            }
        }
#endif
#if LISP_READER_PARSE_QUOTE
        else if (stream_peek_char(in) == '\'')
        {
            stream_skip_char(in);
            Expr const exp = list(intern("quote"), parse_expr(in));
            return exp;
        }
        else if (stream_peek_char(in) == '`')
        {
            stream_skip_char(in);
            Expr const exp = list(intern("backquote"), parse_expr(in));
            return exp;
        }
        else if (stream_peek_char(in) == ',')
        {
            stream_skip_char(in);
            if (stream_peek_char(in) == '@')
            {
                stream_skip_char(in);
                return list(intern("unquote-splicing"), parse_expr(in));
            }
            return list(intern("unquote"), parse_expr(in));
        }
#endif

        else if (is_number_start(stream_peek_char(in)))
        {
            tok = make_buffer_output_stream(4096, lexeme);

            bool neg = 0;
            if (stream_peek_char(in) == '-')
            {
                neg = 1;
                stream_put_char(tok, stream_read_char(in));
            }
            else if (stream_peek_char(in) == '+')
            {
                stream_put_char(tok, stream_read_char(in));
            }

            if (is_number_part(stream_peek_char(in)))
            {
                Expr val = make_number(0);
                Expr ten = make_number(10);
                Expr one = make_number(1);
                Expr den = one;

                // TODO extract function: parse_int
                while (1)
                {
                    char const ch = stream_peek_char(in);
                    if (!is_number_part(ch))
                    {
                        break;
                    }

                    I64 const digit = ch - '0';

                    val = number_mul(val, ten);
                    val = number_add(val, make_number(digit));

                    stream_put_char(tok, stream_read_char(in));
                }

                // TODO
                if (stream_peek_char(in) == '.')
                {
                    stream_put_char(tok, stream_read_char(in));

                    while (1)
                    {
                        char const ch = stream_peek_char(in);
                        if (!is_number_part(ch))
                        {
                            break;
                        }

                        I64 const digit = ch - '0';

                        val = number_mul(val, ten);
                        val = number_add(val, make_number(digit));
                        den = number_mul(den, ten);

                        stream_put_char(tok, stream_read_char(in));
                    }
                }

                if (neg)
                {
                    val = number_neg(val);
                }

                if (!is_number_stop(stream_peek_char(in)))
                {
                    goto symbol_loop;
                }

                if (!number_equal(den, one))
                {
                    val = number_div(val, den);
                }

                return val;
            }

            goto symbol_loop;
        }

        else if (is_symbol_start(stream_peek_char(in)))
        {
            tok = make_buffer_output_stream(4096, lexeme);
            stream_put_char(tok, stream_read_char(in));

        symbol_loop:
            if (is_symbol_part(stream_peek_char(in)))
            {
                stream_put_char(tok, stream_read_char(in));
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

    Expr parse_list(Expr in)
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

        exp = parse_expr(in);

        // TODO get rid of artifical symbol dependence for dotted lists
        if (exp == intern("."))
        {
            exp = parse_expr(in);
            rplacd(tail, exp);

            skip_whitespace_or_comment(in);

            goto list_done;
        }
        else
        {
            Expr next = cons(exp, nil);
            if (head)
            {
                rplacd(tail, next);
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

    Expr parse_string(Expr in)
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
        Expr tok = make_buffer_output_stream(4096, lexeme);

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
                stream_put_char(tok, stream_read_char(in));
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
                stream_put_char(tok, stream_read_char(in));
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

    char parse_hex_digit(Expr in, char val)
    {
        char const ch = stream_read_char(in);

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

    bool is_whitespace(U32 ch)
    {
        return
            ch == ' ' ||
            ch == '\n' ||
            ch == '\t';
    }

    bool is_whitespace_or_eof(U32 ch)
    {
        return ch == 0 || is_whitespace(ch);
    }

    bool is_symbol_start(U32 ch)
    {
        return
            ch != 0 &&
            !is_whitespace(ch) &&
            ch != '"' &&
            ch != '(' && ch != ')' &&
            ch != ';' && ch != '\'';
    }

    bool is_symbol_part(U32 ch)
    {
        return is_symbol_start(ch);
    }

    bool is_number_part(U32 ch)
    {
        return ch >= '0' && ch <= '9';
    }

    bool is_number_start(U32 ch)
    {
        return is_number_part(ch) || ch == '-' || ch == '+';
    }

    bool is_number_stop(U32 ch)
    {
        return ch == 0 || ch == ')' || is_whitespace(ch);
    }

    void skip_whitespace_or_comment(Expr in)
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
};

ReadImpl g_read;

bool maybe_parse_expr(Expr in, Expr * exp)
{
    return g_read.maybe_parse_expr(in, exp);
}

Expr read_one_from_string(char const * src)
{
    return g_read.read_one_from_string(src);
}

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/closure.impl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

/* (lit clo <env> <args> . <body>) */
/* (lit mac <env> <args> . <body>) */

bool is_closure(Expr exp, Expr kind)
{
    return is_cons(exp) &&
        eq(intern("lit"), car(exp)) &&
        is_cons(cdr(exp)) &&
        eq(kind, cadr(exp));
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

bool is_function(Expr exp)
{
    return is_closure(exp, intern("clo"));
}

Expr make_function(Expr env, Expr /*name*/, Expr args, Expr body)
{
    return cons(intern("lit"), cons(intern("clo"), cons(env, cons(args, body))));
}

bool is_macro(Expr exp)
{
    return is_closure(exp, intern("mac"));
}

Expr make_macro(Expr env, Expr /*name*/, Expr args, Expr body)
{
    return cons(intern("lit"), cons(intern("mac"), cons(env, cons(args, body))));
}

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/env.impl"
#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

class EnvImpl
{
public:
    Expr make(Expr outer)
    {
        // ((<vars> . <vals>) . <outer>)
        // TODO add dummy conses as sentinels for vars and vals
        return cons(cons(nil, nil), outer);
    }
};

template <typename T>
class EnvMixin
{
public:
    EnvMixin(EnvImpl & impl) : m_impl(impl)
    {
    }

    Expr make_env(Expr outer)
    {
        return m_impl.make(outer);
    }

private:
    EnvImpl & m_impl;
};

#if LISP_WANT_GLOBAL_API

EnvImpl g_env;

#endif

#ifdef LISP_NAMESPACE
}
#endif

#line 2 "src/system.impl"
/* system */

#ifdef LISP_NAMESPACE
namespace LISP_NAMESPACE {
#endif

U32 utf8_decode_one(U8 const * buf)
{
    U8 ch = *buf++;
    if (ch < 0x80)
    {
        return ch;
    }

    // TODO check for leading 0b10 in continuation bytes

    U32 val = 0;
    if ((ch >> 5) == 0x6)
    {
        val |= ch & 0x1f;

        ch = *buf++;
        val <<= 6;
        val |= (ch & 0x3f);
    }
    else if ((ch >> 4) == 0xe)
    {
        val |= ch & 0xf;

        ch = *buf++;
        val <<= 6;
        val |= (ch & 0x3f);

        ch = *buf++;
        val <<= 6;
        val |= (ch & 0x3f);
    }
    else if ((ch >> 3) == 0x1e)
    {
        val |= ch & 0x7;

        ch = *buf++;
        val <<= 6;
        val |= (ch & 0x3f);

        ch = *buf++;
        val <<= 6;
        val |= (ch & 0x3f);

        ch = *buf++;
        val <<= 6;
        val |= (ch & 0x3f);
    }
    else
    {
        LISP_FAIL("illegal utf-8 string\n", val);
    }

    if (val >= 0xd800 && val < 0xe000)
    {
        LISP_FAIL("illegal surrogate pair %" PRIu32 " in utf-8 string\n", val);
    }

    return val;
}

class SystemImpl : public EnvMixin<SystemImpl>
{
public:
    SystemImpl() :
        EnvMixin(m_env),
        m_dummy(0)
    {
        // TODO move to type_init?
        LISP_ASSERT_ALWAYS(TYPE_NIL == make_type("nil"));
        LISP_ASSERT_ALWAYS(TYPE_CHAR == make_type("char"));
        LISP_ASSERT_ALWAYS(TYPE_FIXNUM == make_type("fixnum"));
        LISP_ASSERT_ALWAYS(TYPE_SYMBOL == make_type("symbol"));
        LISP_ASSERT_ALWAYS(TYPE_KEYWORD == make_type("keyword"));
        LISP_ASSERT_ALWAYS(TYPE_CONS == make_type("cons"));
#if LISP_WANT_GENSYM
        LISP_ASSERT_ALWAYS(TYPE_GENSYM == make_type("gensym"));
#endif
#if LISP_WANT_POINTER
        LISP_ASSERT_ALWAYS(TYPE_POINTER == make_type("pointer"));
#endif
        LISP_ASSERT_ALWAYS(TYPE_STRING == make_type("string"));
        LISP_ASSERT_ALWAYS(TYPE_STREAM == make_type("stream"));
        LISP_ASSERT_ALWAYS(TYPE_BUILTIN_SPECIAL == make_type("builtin-special"));
        LISP_ASSERT_ALWAYS(TYPE_BUILTIN_FUNCTION == make_type("builtin-function"));
        LISP_ASSERT_ALWAYS(TYPE_BUILTIN_SYMBOL == make_type("builtin-symbol"));
    }

    virtual ~SystemImpl()
    {
    }

    /* core */

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

    void repl(Expr env)
    {
        // TODO make a proper prompt input stream
        Expr in = stream_get_stdin();
    loop:
        {
            /* read */
            // TODO use global.stream.stdout
            fprintf(stdout, "> ");
            fflush(stdout);

            Expr exp = nil;
            if (!maybe_parse_expr(in, &exp))
            {
                goto done;
            }

            /* eval */
            Expr ret = eval(exp, env);

            /* print */
            println(ret);

            goto loop;
        }
    done:
        ;
    }

    virtual Expr make_core_env()
    {
        Expr env = make_env(nil);

        env_def(env, intern("t"), intern("t"));

        env_defsym(env, "*env*", [](Expr, Expr env) -> Expr
        {
            return env;
        });

        env_defspecial_quote(env);

        env_defspecial(env, "if", [this](Expr args, Expr env) -> Expr
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
        });

        env_defspecial_while(env);

        env_defspecial(env, "def", [this](Expr args, Expr env) -> Expr
        {
            env_def(env, car(args), eval(cadr(args), env));
            return nil;
        });

        env_defspecial(env, "lambda", [this](Expr args, Expr env) -> Expr
        {
            Expr const fun_args = car(args);
            Expr const fun_body = cdr(args);
            return make_function(env, nil, fun_args, fun_body);
        });

        env_defspecial(env, "syntax", [this](Expr args, Expr env) -> Expr
        {
            Expr const mac_args = car(args);
            Expr const mac_body = cdr(args);
            return make_macro(env, nil, mac_args, mac_body);
        });

        env_defspecial(env, "backquote", [this](Expr args, Expr env) -> Expr
        {
            return backquote(car(args), env);
        });

        env_defun(env, "eq", [this](Expr args, Expr) -> Expr
        {
            return all_eq(args) ? LISP_SYMBOL_T : nil; // TODO use make_truth()
        });

        env_defun(env, "equal", [this](Expr args, Expr) -> Expr
        {
            return all_equal(args) ? LISP_SYMBOL_T : nil; // TODO use make_truth()
        });

        env_defun(env, "cons", [this](Expr args, Expr) -> Expr
        {
            return cons(car(args), cadr(args));
        });

        env_defun(env, "car", [this](Expr args, Expr) -> Expr
        {
            return car(car(args));
        });

        env_defun(env, "cdr", [this](Expr args, Expr) -> Expr
        {
            return cdr(car(args));
        });

        env_defun(env, "rplaca", [this](Expr args, Expr) -> Expr
        {
            rplaca(car(args), cadr(args));
            return nil;
        });

        env_defun(env, "rplacd", [this](Expr args, Expr) -> Expr
        {
            rplacd(car(args), cadr(args));
            return nil;
        });

        env_defun_println(env, "println");

        env_defun(env, "intern", [this](Expr args, Expr) -> Expr
        {
            return intern(string_value(car(args)));
        });

#if LISP_WANT_GENSYM
        env_defun(env, "gensym", [this](Expr, Expr) -> Expr
        {
            return gensym();
        });
#endif

#if LISP_WANT_POINTER
        env_defun(env, "fopen", [this](Expr args, Expr) -> Expr
        {
            // TODO use builtin_arg1(name, args) for better error checking
            Expr const path = first(args);
            Expr const mode = second(args);
            return make_pointer(fopen(string_value(path), string_value(mode)));
        });

        env_defun(env, "fclose", [this](Expr args, Expr) -> Expr
        {
            // TODO use builtin_arg1(name, args) for better error checking
            Expr const file = first(args);
            fclose((FILE *) pointer_value(file));
            return nil;
        });

        env_defun(env, "write-u8", [this](Expr args, Expr) -> Expr
        {
            Expr const file = first(args);
            Expr const value = second(args);
            U8 const val = fixnum_value(value); // TODO use, say, expr_to_u8()
            fwrite(&val, 1, 1, (FILE *) pointer_value(file));
            return nil;
        });
#endif

        env_defun(env, "load-file", [this](Expr args, Expr env) -> Expr
        {
            load_file(string_value(first(args)), env);
            return nil;
        });

        env_defun(env, "ord", [this](Expr args, Expr) -> Expr
        {
            return make_number(utf8_decode_one(string_value_utf8(car(args))));
        });

        env_defun(env, "chr", [this](Expr args, Expr) -> Expr
        {
            return make_string_from_utf32_char((U32) fixnum_value(car(args)));
        });

        env_defun(env, "type", [this](Expr args, Expr) -> Expr
        {
            Expr const arg1 = car(args);
            return intern(type_name(expr_type(arg1)));
        });

        return env;
    }

    void env_defspecial_quote(Expr env)
    {
        env_defspecial(env, "quote", [this](Expr args, Expr) -> Expr
        {
            return car(args);
        });
    }

    void env_defspecial_while(Expr env)
    {
        env_defspecial(env, "while", [this](Expr args, Expr env) -> Expr
        {
            Expr const test = car(args);
            Expr const body = cdr(args);

            // TODO do we want to return a value?
            while (eval(test, env))
            {
                eval_body(body, env);
            }
            return nil;
        });
    }

    void env_defun_println(Expr env, char const * name)
    {
        env_defun(env, name, b_println);
    }

    /* type */

    U64 make_type(char const * name)
    {
        return m_type.make(name);
    }

    char const * type_name(U64 type)
    {
        return m_type.name(type);
    }

    /* util */

    // TODO use is_named_call()
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

    bool equal(Expr a, Expr b)
    {
        if (is_cons(a) && is_cons(b))
        {
            return equal(car(a), car(b)) && equal(cdr(a), cdr(b));
        }
        else if (is_string(a) && is_string(b))
        {
            return string_equal(a, b);
        }
        return eq(a, b);
    }

    bool all_equal(Expr exps)
    {
        if (is_nil(exps))
        {
            LISP_FAIL("not enough arguments in call to equal: %s\n", repr(exps));
        }
        Expr prv = car(exps);
        Expr tmp = cdr(exps);
        if (is_nil(tmp))
        {
            LISP_FAIL("not enough arguments in call to equal: %s\n", repr(exps));
        }

        for (; tmp; tmp = cdr(tmp))
        {
            auto const exp = car(tmp);
            if (!equal(prv, exp))
            {
                return false;
            }
            prv = exp;
        }

        return true;
    }

    bool all_eq(Expr exps)
    {
        if (is_nil(exps))
        {
            LISP_FAIL("not enough arguments in call to eq: %s\n", repr(exps));
        }
        auto prv = car(exps);
        auto tmp = cdr(exps);
        if (is_nil(tmp))
        {
            LISP_FAIL("not enough arguments in call to eq: %s\n", repr(exps));
        }

        for (; tmp; tmp = cdr(tmp))
        {
            auto const exp = car(tmp);
            if (!eq(prv, exp))
            {
                return false;
            }
            prv = exp;
        }

        return true;
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

    /* reader */

    Expr env_vars(Expr env)
    {
        return caar(env);
    }

    void env_set_vars(Expr env, Expr vars)
    {
        rplaca(car(env), vars);
    }

    Expr env_vals(Expr env)
    {
        return cdar(env);
    }

    void env_set_vals(Expr env, Expr vals)
    {
        rplacd(car(env), vals);
    }

    Expr env_outer(Expr env)
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

    void env_defun(Expr env, char const * name, BuiltinFunc func)
    {
        env_def(env, intern(name), make_builtin_function(name, func));
    }

    void env_defspecial(Expr env, char const * name, BuiltinFunc func)
    {
        env_def(env, intern(name), make_builtin_special(name, func));
    }

    void env_defsym(Expr env, char const * name, BuiltinFunc func)
    {
        env_def(env, intern(name), make_builtin_symbol(name, func));
    }

    Expr _env_find_local(Expr env, Expr var)
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

    Expr _env_find_global(Expr env, Expr var)
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

    /* eval */

    Expr eval(Expr exp, Expr env)
    {
        switch (expr_type(exp))
        {
        case TYPE_NIL:
        case TYPE_CHAR:
        case TYPE_FIXNUM:
        case TYPE_STRING:
        case TYPE_KEYWORD:
#if LISP_WANT_POINTER
        case TYPE_POINTER:
#endif
            return exp;
        case TYPE_SYMBOL:
#if LISP_WANT_GENSYM
        case TYPE_GENSYM:
#endif
            return env_get(env, exp);
        case TYPE_CONS:
            return apply(car(exp), cdr(exp), env);
        case TYPE_BUILTIN_SYMBOL:
            return builtin_func(exp)(nil, env);
        default:
            LISP_FAIL("cannot evaluate %s\n", repr(exp));
            return nil;
        }
    }

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

    Expr apply(Expr name, Expr args, Expr env)
    {
        if (is_builtin_function(name))
        {
            return builtin_func(name)(eval_list(args, env), env);
        }
        else if (is_builtin_special(name))
        {
            return builtin_func(name)(args, env);
        }
        else if (is_function(name))
        {
            return eval_body(closure_body(name),
                             make_call_env_from(closure_env(name),
                                                closure_args(name),
                                                eval_list(args, env)));
        }
        else if (is_macro(name))
        {
            return eval(eval_body(closure_body(name),
                                  make_call_env_from(closure_env(name),
                                                     closure_args(name),
                                                     args)),
                        env);
        }
        else
        {
            // TODO check for unlimited recursion
            return apply(eval(name, env), args, env);
        }
    }

    void bind_args(Expr env, Expr vars, Expr vals)
    {
        env_destructuring_bind(env, vars, vals);
    }

    Expr make_call_env_from(Expr lenv, Expr vars, Expr vals)
    {
        Expr cenv = make_env(lenv);
        bind_args(cenv, vars, vals);
        return cenv;
    }

    /* backquote */

    Expr backquote(Expr exp, Expr env)
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

    Expr backquote_list(Expr seq, Expr env)
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

    TypeImpl m_type;
    EnvImpl m_env;
    int m_dummy;
};

/* system */

#if LISP_WANT_SYSTEM_API

System * System::s_instance = nullptr;

System::System()
{
    LISP_ASSERT(s_instance == nullptr);
    s_instance = this;

    m_impl = new SystemImpl();
}

System::~System()
{
    delete m_impl;

    s_instance = nullptr;
}

/* core */

Expr System::list(Expr exp1)
{
    return ::list(exp1);
}

Expr System::list(Expr exp1, Expr exp2)
{
    return ::list(exp1, exp2);
}

Expr System::list(Expr exp1, Expr exp2, Expr exp3)
{
    return ::list(exp1, exp2, exp3);
}

Expr System::list(Expr exp1, Expr exp2, Expr exp3, Expr exp4, Expr exp5)
{
    return ::list(exp1, exp2, exp3, exp4, exp5);
}

Expr System::first(Expr seq)
{
    return m_impl->first(seq);
}

Expr System::second(Expr seq)
{
    return m_impl->second(seq);
}

bool System::equal(Expr a, Expr b)
{
    return m_impl->equal(a, b);
}

/* read */

Expr System::intern(char const * name)
{
    return ::intern(name);
}

Expr System::read_one_from_string(char const * src)
{
    return ::read_one_from_string(src);
}

bool System::maybe_parse_expr(Expr in, Expr * exp)
{
    return ::maybe_parse_expr(in, exp);
}

/* env */

Expr System::make_env(Expr outer)
{
    return m_impl->make_env(outer);
}

Expr System::make_core_env()
{
    return m_impl->make_core_env();
}

void System::env_def(Expr env, Expr var, Expr val)
{
    m_impl->env_def(env, var, val);
}

void System::env_defun(Expr env, char const * name, BuiltinFunc func)
{
    m_impl->env_defun(env, name, func);
}

void System::env_defun_println(Expr env, char const * name)
{
    m_impl->env_defun_println(env, name);
}

void System::env_defspecial(Expr env, char const * name, BuiltinFunc func)
{
    m_impl->env_defspecial(env, name, func);
}

void System::env_defspecial_quote(Expr env)
{
    m_impl->env_defspecial_quote(env);
}

void System::env_defspecial_while(Expr env)
{
    m_impl->env_defspecial_while(env);
}

void System::env_defsym(Expr env, char const * name, BuiltinFunc func)
{
    m_impl->env_defsym(env, name, func);
}

void System::env_del(Expr env, Expr var)
{
    return m_impl->env_del(env, var);
}

Expr System::env_get(Expr env, Expr var)
{
    return m_impl->env_get(env, var);
}

bool System::env_can_set(Expr env, Expr var)
{
    return m_impl->env_can_set(env, var);
}

void System::env_set(Expr env, Expr var, Expr val)
{
    m_impl->env_set(env, var, val);
}

/* function */

Expr System::make_function(Expr env, Expr name, Expr args, Expr body)
{
    return ::make_function(env, name, args, body);
}

/* eval */

Expr System::eval(Expr exp, Expr env)
{
    return m_impl->eval(exp, env);
}

Expr System::eval_body(Expr exps, Expr env)
{
    return m_impl->eval_body(exps, env);
}

void System::load_file(char const * path, Expr env)
{
    m_impl->load_file(path, env);
}

void System::repl(Expr env)
{
    return m_impl->repl(env);
}

#endif

#ifdef LISP_NAMESPACE
}
#endif

#endif /* _LISP_CPP_ */

#endif /* LISP_IMPLEMENTATION */
