
#ifndef _LISP_HPP_
#define _LISP_HPP_

/* config */

#ifndef LISP_GLOBAL_API
#define LISP_GLOBAL_API 1
#endif

#ifndef LISP_DEBUG
#define LISP_DEBUG 1
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
#include <vector>
#include <string>

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

#define LISP_MAX_SYMBOLS -1
#define LISP_DEF_SYMBOLS 16

typedef struct
{
    U64 num;
    U64 max;
    char ** names;
} SymbolState;

/* cons */

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

inline bool is_cons(Expr exp)
{
    return expr_type(exp) == TYPE_CONS;
}

/* string */

#define LISP_MAX_STRINGS 500000

typedef struct
{
    U64 count;
    char ** values;
} StringState;

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

/* builtin */

#define LISP_MAX_BUILTINS 64

typedef std::function<Expr(Expr args, Expr env)> BuiltinFun;

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

/* system */

typedef struct SystemState
{
    SymbolState symbol;
    ConsState cons;
    StreamState stream;
    StringState string;
    BuiltinState builtin;
} SystemState;

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

bool is_character(Expr exp)
{
    return expr_type(exp) == TYPE_CHAR;
}

Expr make_character(U32 code)
{
    return make_expr(TYPE_CHAR, code);
}

U32 character_code(Expr exp)
{
    LISP_ASSERT(is_character(exp));
    return (U32) expr_data(exp);
}

bool is_printable_ascii(U32 ch)
{
    // NOTE we exclude space
    return ch >= 33 && ch <= 126;
}

#define TEMP_BUF_SIZE  4096
#define TEMP_BUF_COUNT 4

char * get_temp_buf(size_t size)
{
    LISP_ASSERT(size <= TEMP_BUF_SIZE);

    static char buf[TEMP_BUF_COUNT][TEMP_BUF_SIZE];
    static int idx = 0;
    char * ret = buf[idx];
    idx = (idx + 1) % TEMP_BUF_COUNT;
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

SystemState global;

class TypeImpl
{
public:
    U64 make(char const * name)
    {
        U64 const type = count();
        m_type_names.push_back(name);
        return type;
    }

    U64 count() const
    {
        return m_type_names.size();
    }

    char const * name(U64 type) const
    {
        LISP_ASSERT(type < count());
        return m_type_names[type].c_str();
    }

private:
    std::vector<std::string> m_type_names;
};

class GensymImpl
{
public:
    GensymImpl(U64 type = TYPE_GENSYM) : m_type(type), m_counter(0)
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

class System
{
public:
    System()
    {
        LISP_ASSERT(s_instance == nullptr);
        s_instance = this;
        system_init(&global);

        // TODO move to type_init?
        LISP_ASSERT_ALWAYS(TYPE_NIL              == make_type("nil"));
        LISP_ASSERT_ALWAYS(TYPE_SYMBOL           == make_type("symbol"));
        LISP_ASSERT_ALWAYS(TYPE_CONS             == make_type("cons"));
        LISP_ASSERT_ALWAYS(TYPE_GENSYM           == make_type("gensym"));
        LISP_ASSERT_ALWAYS(TYPE_CHAR             == make_type("char"));
        LISP_ASSERT_ALWAYS(TYPE_FIXNUM           == make_type("fixnum"));
        LISP_ASSERT_ALWAYS(TYPE_STRING           == make_type("string"));
        LISP_ASSERT_ALWAYS(TYPE_STREAM           == make_type("stream"));
        LISP_ASSERT_ALWAYS(TYPE_BUILTIN_SPECIAL  == make_type("builtin-special"));
        LISP_ASSERT_ALWAYS(TYPE_BUILTIN_FUNCTION == make_type("builtin-function"));
        LISP_ASSERT_ALWAYS(TYPE_BUILTIN_SYMBOL   == make_type("builtin-symbol"));
    }

    virtual ~System()
    {
        system_quit(&global);
        s_instance = nullptr;
    }

    void system_init(SystemState * system)
    {
        symbol_init(&system->symbol);
        cons_init(&system->cons);
        string_init(&system->string);
        stream_init(&system->stream);
        builtin_init(&system->builtin);
    }

    void system_quit(SystemState * system)
    {
        builtin_quit(&system->builtin);
        string_quit(&system->string);
        stream_quit(&system->stream);
        cons_quit(&system->cons);
        symbol_quit(&system->symbol);
    }

    /* core */

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
        Expr in = global.stream.stdin;
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

        env_defsym(env, "*env*", [this](Expr args, Expr env) -> Expr
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
            return all_eq(args) ? LISP_SYMBOL_T : nil;
        });

        env_defun(env, "equal", [this](Expr args, Expr) -> Expr
        {
            return all_equal(args) ? LISP_SYMBOL_T : nil;
        });

        env_defun(env, "cons", [this](Expr args, Expr env) -> Expr
        {
            return cons(car(args), cadr(args));
        });

        env_defun(env, "car", [this](Expr args, Expr env) -> Expr
        {
            return car(car(args));
        });

        env_defun(env, "cdr", [this](Expr args, Expr env) -> Expr
        {
            return cdr(car(args));
        });

        env_defun(env, "rplaca", [this](Expr args, Expr env) -> Expr
        {
            rplaca(car(args), cadr(args));
            return nil;
        });

        env_defun(env, "rplacd", [this](Expr args, Expr env) -> Expr
        {
            rplacd(car(args), cadr(args));
            return nil;
        });

        env_defun_println(env, "println");

        env_defun(env, "intern", [this](Expr args, Expr env) -> Expr
        {
            return intern(string_value(car(args)));
        });

        env_defun(env, "gensym", [this](Expr args, Expr env) -> Expr
        {
            return gensym();
        });

        env_defun(env, "load-file", [this](Expr args, Expr env) -> Expr
        {
            load_file(string_value(first(args)), env);
            return nil;
        });

        env_defun(env, "ord", [this](Expr args, Expr env) -> Expr
        {
            return make_number(utf8_decode_one(string_value_utf8(car(args))));
        });

        env_defun(env, "chr", [this](Expr args, Expr env) -> Expr
        {
            return make_string_from_utf32_char((U32) fixnum_value(car(args)));
        });

        return env;
    }

    void env_defspecial_quote(Expr env)
    {
        env_defspecial(env, "quote", [this](Expr args, Expr env) -> Expr
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
        env_defun(env, name, [this](Expr args, Expr env) -> Expr
        {
            Expr out = global.stream.stdout;
            for (Expr tmp = args; tmp; tmp = cdr(tmp))
            {
                if (tmp != args)
                {
                    stream_put_char(out, ' ');
                }
                Expr exp = car(tmp);
                print_expr(exp, out);
            }
            stream_put_char(out, '\n');
            return nil;
        });
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

    /* symbol */

    bool is_symbol(Expr exp)
    {
        return expr_type(exp) == TYPE_SYMBOL;
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

    /* cons */

    void cons_init(ConsState * cons)
    {
        memset(cons, 0, sizeof(ConsState));
    }

    void cons_quit(ConsState * cons)
    {
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

    Expr caar(Expr exp)
    {
        return car(car(exp));
    }

    Expr cadr(Expr exp)
    {
        return car(cdr(exp));
    }

    Expr cdar(Expr exp)
    {
        return cdr(car(exp));
    }

    Expr cddr(Expr exp)
    {
        return cdr(cdr(exp));
    }

    Expr caddr(Expr exp)
    {
        return car(cdr(cdr(exp)));
    }

    Expr cdddr(Expr exp)
    {
        return cdr(cdr(cdr(exp)));
    }

    Expr cadddr(Expr exp)
    {
        return car(cdr(cdr(cdr(exp))));
    }

    Expr cddddr(Expr exp)
    {
        return cdr(cdr(cdr(cdr(exp))));
    }

    /* gensym */

    bool is_gensym(Expr exp)
    {
        return m_gensym.isinstance(exp);
    }

    Expr gensym()
    {
        return m_gensym.make();
    }

    /* fixnum */

#define LISP_FIXNUM_SIGN_MASK (UINT64_C(1) << ((U64) LISP_DATA_BITS - UINT64_C(1)))
#define LISP_FIXNUM_BITS_MASK (LISP_EXPR_MASK >> (UINT64_C(64) + UINT64_C(1) - (U64) LISP_DATA_BITS))
#define LISP_FIXNUM_MINVAL    (-(INT64_C(1) << ((I64) LISP_DATA_BITS - INT64_C(1))))
#define LISP_FIXNUM_MAXVAL    ((INT64_C(1) << ((I64) LISP_DATA_BITS - INT64_C(1))) - INT64_C(1))

    bool is_fixnum(Expr exp)
    {
        return expr_type(exp) == TYPE_FIXNUM;
    }

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

    bool fixnum_equal(Expr a, Expr b)
    {
        return a == b;
    }

    /* number */

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
        return fixnum_equal(a, b);
    }

    /* string */

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

        Expr ret = _string_alloc(string, len);
        memcpy(_string_buffer(string, ret), str, len + 1);
        return ret;
    }

    char const * lisp_string_value(StringState * string, Expr exp)
    {
        return _string_buffer(string, exp);
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

    Expr make_string_from_utf8(U8 const * str)
    {
        // TODO this is not portable
        return make_string((char const *) str);
    }

    Expr make_string_from_utf32_char(U32 ch)
    {
        U8 bytes[5];
        U8 * out_bytes = bytes;

        if (ch < 0x80)
        {
            *out_bytes++ = (U8) ch;
        }
        else if (ch < 0x800)
        {
            *out_bytes++ = (U8) (0xc0 | ((ch >> 6) & 0x1f));
            *out_bytes++ = (U8) (0x80 | (ch & 0x3f));
        }
        else if (ch >= 0xd800 && ch < 0xe000)
        {
            LISP_FAIL("illegal code point %" PRIu64 " for utf-8 encoder\n", ch);
        }
        else if (ch < 0x10000)
        {
            *out_bytes++ = (U8) (0xe0 | ((ch >> 12) & 0xf));
            *out_bytes++ = (U8) (0x80 | ((ch >> 6) & 0x3f));
            *out_bytes++ = (U8) (0x80 | (ch & 0x3f));
        }
        else if (ch < 0x200000)
        {
            *out_bytes++ = (U8) (0xf0 | ((ch >> 18) & 0x7));
            *out_bytes++ = (U8) (0x80 | ((ch >> 12) & 0x3f));
            *out_bytes++ = (U8) (0x80 | ((ch >> 6) & 0x3f));
            *out_bytes++ = (U8) (0x80 | (ch & 0x3f));
        }
        else
        {
            LISP_FAIL("illegal code point %" PRIu64 " for utf-8 encoder\n", ch);
        }

        *out_bytes++ = 0;
        return make_string_from_utf8(bytes);
    }

    char const * string_value(Expr exp)
    {
        return lisp_string_value(&global.string, exp);
    }

    U8 const * string_value_utf8(Expr exp)
    {
        // TODO actually change internal representation
        return (U8 const *) string_value(exp);
    }

    U64 string_length(Expr exp)
    {
        return lisp_string_length(&global.string, exp);
    }

    bool string_equal(Expr exp1, Expr exp2)
    {
        // TODO use cached length test first
        return !strcmp(string_value(exp1), string_value(exp2));
    }

    Expr _string_alloc(StringState * string, size_t len)
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

    char * _string_buffer(StringState * string, Expr exp)
    {
        LISP_ASSERT(is_string(exp));

        U64 const index = expr_data(exp);
        if (index >= string->count)
        {
            LISP_FAIL("illegal string index %" PRIu64 "\n", index);
        }

        return string->values[index];
    }

    /* util */

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
            Expr const exp = car(tmp);
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
        Expr prv = car(exps);
        Expr tmp = cdr(exps);
        if (is_nil(tmp))
        {
            LISP_FAIL("not enough arguments in call to eq: %s\n", repr(exps));
        }

        for (; tmp; tmp = cdr(tmp))
        {
            Expr const exp = car(tmp);
            if (!eq(prv, exp))
            {
                return false;
            }
            prv = exp;
        }

        return true;
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

    char const * repr(Expr exp)
    {
        // TODO multiple calls => need temp buffer per call
        size_t const size = 4096;
        char * buffer = get_temp_buf(size);
        Expr out = lisp_make_buffer_output_stream(&global.stream, size, buffer);
        System::s_instance->print_expr(exp, out);
        stream_release(out);
        return buffer;
    }

    void println(Expr exp)
    {
        Expr out = global.stream.stdout;
        System::s_instance->print_expr(exp, out);
        stream_put_string(out, "\n");
    }

    void displayln(Expr exp)
    {
        Expr out = global.stream.stdout;
        System::s_instance->display_expr(exp, out);
        stream_put_string(out, "\n");
    }

    /* reader */

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

        char lexeme[4096];
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
            tok = lisp_make_buffer_output_stream(&global.stream, 4096, lexeme);

            while (!is_whitespace_or_eof(stream_peek_char(in)))
            {
                stream_put_char(tok, stream_read_char(in));
            }
            stream_put_char(tok, 0);
            stream_release(tok);

            // handle printable characters
            if (lexeme[2] == 0)
            {
                return make_character(lexeme[1]);
            }
            else if (!strcmp("\\bel", lexeme))
            {
                return make_character('\a');
            }
            else if (!strcmp("\\space", lexeme))
            {
                return make_character(' ');
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
            tok = lisp_make_buffer_output_stream(&global.stream, 4096, lexeme);

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
            tok = lisp_make_buffer_output_stream(&global.stream, 4096, lexeme);
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
        Expr tok = lisp_make_buffer_output_stream(&global.stream, 4096, lexeme);

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

    bool is_whitespace(char ch)
    {
        return
            ch == ' ' ||
            ch == '\n' ||
            ch == '\t';
    }

    bool is_whitespace_or_eof(char ch)
    {
        return ch == 0 || is_whitespace(ch);
    }

    bool is_symbol_start(char ch)
    {
        return
            ch != 0 &&
            !is_whitespace(ch) &&
            ch != '"' &&
            ch != '(' && ch != ')' &&
            ch != ';' && ch != '\'';
    }

    bool is_symbol_part(char ch)
    {
        return is_symbol_start(ch);
    }

    bool is_number_part(char ch)
    {
        return ch >= '0' && ch <= '9';
    }

    bool is_number_start(char ch)
    {
        return is_number_part(ch) || ch == '-' || ch == '+';
    }

    bool is_number_stop(char ch)
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

    /* printer */

    void display_expr(Expr exp, Expr out)
    {
        switch (expr_type(exp))
        {
        case TYPE_STRING:
            stream_put_string(out, string_value(exp));
            break;
        case TYPE_CHAR:
            stream_put_char(out, character_code(exp));
            break;
        default:
            print_expr(exp, out);
            break;
        }
    }

    void print_expr(Expr exp, Expr out)
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
            print_cons(exp, out);
            break;
        case TYPE_GENSYM:
            print_gensym(exp, out);
            break;
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

    void print_cons(Expr exp, Expr out)
    {
#if LISP_PRINTER_PRINT_QUOTE
        if (is_quote_call(exp))
        {
            stream_put_char(out, '\'');
            print_expr(cadr(exp), out);
            return;
        }
#endif
        stream_put_char(out, '(');
        print_expr(car(exp), out);

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
                print_expr(car(tmp), out);
            }
            else
            {
                stream_put_string(out, " . ");
                print_expr(tmp, out);
                break;
            }
        }

        stream_put_char(out, ')');
    }

    void print_builtin(Expr exp, Expr out, char const * flavor)
    {
        stream_put_string(out, "#:<");
        stream_put_string(out, flavor);
        char const * name = builtin_name(exp);
        if (name)
        {
            stream_put_string(out, " ");
            stream_put_string(out, name);
        }
        stream_put_string(out, ">");
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

    void print_gensym(Expr exp, Expr out)
    {
        LISP_ASSERT_DEBUG(is_gensym(exp));
        U64 const num = expr_data(exp);
        stream_put_string(out, "#:G");
        stream_put_u64(out, num);
    }

    void print_char(Expr exp, Expr out)
    {
        LISP_ASSERT_DEBUG(is_character(exp));
        U32 const code = character_code(exp);
        // TODO
        if (is_printable_ascii(code))
        {
            stream_put_char(out, '\\');
            stream_put_char(out, (char) code);
        }
        else if (code == '\a')
        {
            stream_put_string(out, "\\bel");
        }
        else if (code == ' ')
        {
            stream_put_string(out, "\\space");
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
                    stream_put_char(out, ch);
                }
                break;
            }
        }
        stream_put_char(out, '"');
    }

    /* stream */

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

    void stream_put_i64(Expr exp, U64 val)
    {
        char str[32];
        sprintf(str, "%" PRIi64, val);
        stream_put_string(exp, str);
    }

    void stream_put_x64(Expr exp, U64 val)
    {
        char str[32];
        sprintf(str, "%016" PRIx64, val);
        stream_put_string(exp, str);
    }

    void stream_release(Expr exp)
    {
        lisp_stream_release(&global.stream, exp);
    }

    char stream_read_char(Expr exp)
    {
        /* TODO inline implementation */
        char const ret = stream_peek_char(exp);
        stream_skip_char(exp);
        return ret;
    }

    bool stream_at_end(Expr exp)
    {
        return lisp_stream_at_end(&global.stream, exp);
    }

    /* builtin */

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

    bool is_builtin_symbol(Expr exp)
    {
        return expr_type(exp) == TYPE_BUILTIN_SYMBOL;
    }

    inline bool is_builtin(Expr exp)
    {
        return is_builtin_special(exp) || is_builtin_function(exp) || is_builtin_symbol(exp);
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

    Expr lisp_make_builtin_symbol(BuiltinState * builtin, char const * name, BuiltinFun fun)
    {
        return lisp_make_builtin(builtin, name, fun, TYPE_BUILTIN_SYMBOL);
    }

    BuiltinInfo * _builtin_expr_to_info(BuiltinState * builtin, Expr exp)
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

    Expr make_builtin_special(char const * name, BuiltinFun fun)
    {
        return lisp_make_builtin_special(&global.builtin, name, fun);
    }

    Expr make_builtin_function(char const * name, BuiltinFun fun)
    {
        return lisp_make_builtin_function(&global.builtin, name, fun);
    }

    Expr make_builtin_symbol(char const * name, BuiltinFun fun)
    {
        return lisp_make_builtin_symbol(&global.builtin, name, fun);
    }

    char const * builtin_name(Expr exp)
    {
        return lisp_builtin_name(&global.builtin, exp);
    }

    BuiltinFun builtin_fun(Expr exp)
    {
        return lisp_builtin_fun(&global.builtin, exp);
    }

    /* closure */

    /* (lit clo <env> <args> . <body>) */
    /* (lit mac <env> <args> . <body>) */

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

    Expr make_function(Expr env, Expr name, Expr args, Expr body)
    {
        return cons(intern("lit"), cons(intern("clo"), cons(env, cons(args, body))));
    }

    bool is_macro(Expr exp)
    {
        return is_closure(exp, intern("mac"));
    }

    Expr make_macro(Expr env, Expr name, Expr args, Expr body)
    {
        return cons(intern("lit"), cons(intern("mac"), cons(env, cons(args, body))));
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

    /* env */

    Expr make_env(Expr outer)
    {
        // ((<vars> . <vals>) . <outer>)
        // TODO add dummy conses as sentinels for vars and vals
        return cons(cons(nil, nil), outer);
    }

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

    void env_defun(Expr env, char const * name, BuiltinFun fun)
    {
        env_def(env, intern(name), make_builtin_function(name, fun));
    }

    void env_defspecial(Expr env, char const * name, BuiltinFun fun)
    {
        env_def(env, intern(name), make_builtin_special(name, fun));
    }

    void env_defsym(Expr env, char const * name, BuiltinFun fun)
    {
        env_def(env, intern(name), make_builtin_symbol(name, fun));
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
            return exp;
        case TYPE_SYMBOL:
        case TYPE_GENSYM:
            return env_get(env, exp);
        case TYPE_CONS:
            return apply(car(exp), cdr(exp), env);
        case TYPE_BUILTIN_SYMBOL:
            return builtin_fun(exp)(nil, env);
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
            return builtin_fun(name)(eval_list(args, env), env);
        }
        else if (is_builtin_special(name))
        {
            return builtin_fun(name)(args, env);
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
    GensymImpl m_gensym;

    static System * s_instance;
};

System * System::s_instance = nullptr;

#endif /* _LISP_CPP_ */

#endif /* LISP_IMPLEMENTATION */
