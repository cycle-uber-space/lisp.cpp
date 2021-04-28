
#define LISP_IMPLEMENTATION
#include "lisp.hpp"

namespace lisp {

class System
{
};

class MySystem : public System
{
public:
    int main(int argc, char ** argv)
    {
        if (argc < 2)
        {
            fail("missing command\n");
        }
        char const * cmd = argv[1];
        if (!strcmp("unit", cmd))
        {
            global_init();
            TestState _test;
            TestState * test = &_test;
            LISP_TEST_BEGIN(test);
            for (int i = 2; i < argc; i++)
            {
                if (!strcmp("--exit-on-fail", argv[i]))
                {
                    test->exit_on_fail = true;
                }
                else if (!strcmp("--no-exit-on-fail", argv[i]))
                {
                    test->exit_on_fail = false;
                }
            }

            unit_test(test);
            LISP_TEST_GROUP(test, "summary");
            LISP_TEST_FINISH(test);
            global_quit();
        }
        else if (!strcmp("load", cmd))
        {
            global_init();
            Expr env = make_core_env();
            for (int i = 2; i < argc; i++)
            {
                load_file(argv[i], env);
            }
            global_quit();
        }
        else if (!strcmp("repl", cmd))
        {
            global_init();
            Expr env = make_core_env();

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
            global_quit();
        }
        else
        {
            fail("unknown command: %s\n", cmd);
        }
        return 0;
    }

    void unit_test(TestState * test)
    {
        unit_test_expr(test);
        unit_test_nil(test);
        unit_test_symbol(test);
        unit_test_cons(test);
        unit_test_stream(test);
        unit_test_reader(test);
        unit_test_printer(test);
        unit_test_util(test);
        unit_test_env(test);
        unit_test_eval(test);
    }

    void unit_test_expr(TestState * test)
    {
        LISP_TEST_GROUP(test, "expr");
        LISP_TEST_ASSERT(test, expr_type(make_expr(23, 42)) == 23);
        LISP_TEST_ASSERT(test, expr_data(make_expr(23, 42)) == 42);
    }

    void unit_test_nil(TestState * test)
    {
        LISP_TEST_GROUP(test, "nil");
        LISP_TEST_ASSERT(test, expr_type(nil) == TYPE_NIL);
        LISP_TEST_ASSERT(test, expr_data(nil) == DATA_NIL);
        LISP_TEST_ASSERT(test, (bool) nil == false);
        LISP_TEST_ASSERT(test, is_nil(nil));
    }

    void unit_test_symbol(TestState * test)
    {
        LISP_TEST_GROUP(test, "symbol");
        LISP_TEST_ASSERT(test, !strcmp("foo", symbol_name(intern("foo"))));
        LISP_TEST_ASSERT(test, !strcmp("bar", symbol_name(intern("bar"))));
#if LISP_SYMBOL_NAME_OF_NIL
        LISP_TEST_ASSERT(test, !strcmp("nil", symbol_name(intern("nil"))));
#endif

        LISP_TEST_ASSERT(test, intern("foo") == intern("foo"));
        LISP_TEST_ASSERT(test, intern("foo") != intern("bar"));
    }

    void unit_test_cons(TestState * test)
    {
        LISP_TEST_GROUP(test, "cons");
        LISP_TEST_ASSERT(test, is_cons(cons(nil, nil)));
        LISP_TEST_ASSERT(test, car(cons(nil, nil)) == nil);
        LISP_TEST_ASSERT(test, cdr(cons(nil, nil)) == nil);
    }

    void unit_test_stream(TestState * test)
    {
        LISP_TEST_GROUP(test, "stream");
        LISP_TEST_ASSERT(test, is_stream(global.stream.stdin));
        LISP_TEST_ASSERT(test, is_stream(global.stream.stdout));
        LISP_TEST_ASSERT(test, is_stream(global.stream.stderr));
    }

    void unit_test_reader(TestState * test)
    {
        Expr foo = intern("foo");
        LISP_TEST_GROUP(test, "reader");
        LISP_TEST_ASSERT(test, read_one_from_string("nil") == nil);
        LISP_TEST_ASSERT(test, read_one_from_string("foo") == foo);
        LISP_TEST_ASSERT(test, read_one_from_string("()") == nil);
        LISP_TEST_ASSERT(test, equal(read_one_from_string("(foo bar baz)"), list_3(foo, intern("bar"), intern("baz"))));

        LISP_TEST_ASSERT(test, equal(read_one_from_string("'foo"), make_quote(foo)));
        LISP_TEST_ASSERT(test, equal(read_one_from_string("`foo"), make_backquote(foo)));
        LISP_TEST_ASSERT(test, equal(read_one_from_string(",foo"), list_2(intern("unquote"), foo)));
        LISP_TEST_ASSERT(test, equal(read_one_from_string(",@foo"), list_2(intern("unquote-splicing"), foo)));
    }

    void unit_test_printer(TestState * test)
    {
        LISP_TEST_GROUP(test, "printer");
        LISP_TEST_ASSERT(test, !strcmp("nil", repr(nil)));
    }

    void unit_test_util(TestState * test)
    {
        LISP_TEST_GROUP(test, "util");
        LISP_TEST_ASSERT(test, intern("nil") == nil);
        LISP_TEST_ASSERT(test, is_nil(intern("nil")));
        LISP_TEST_ASSERT(test, is_symbol(intern("nul")));

        {
            Expr const foo = intern("foo");
            Expr const bar = intern("bar");
            LISP_TEST_ASSERT(test, first(list_1(foo)) == foo);
            LISP_TEST_ASSERT(test, first(list_2(foo, bar)) == foo);
            LISP_TEST_ASSERT(test, second(list_2(foo, bar)) == bar);
        }
    }

    void unit_test_env(TestState * test)
    {
        LISP_TEST_GROUP(test, "env");
        {
            Expr env = make_env(nil);
            Expr const foo = intern("foo");
            Expr const bar = intern("bar");
            LISP_TEST_ASSERT(test, !env_can_set(env, foo));
            LISP_TEST_ASSERT(test, !env_can_set(env, bar));
            env_def(env, foo, bar);
            LISP_TEST_ASSERT(test,  env_can_set(env, foo));
            LISP_TEST_ASSERT(test, !env_can_set(env, bar));
            LISP_TEST_ASSERT(test, env_get(env, foo) == bar);

            env_def(env, bar, foo);
            LISP_TEST_ASSERT(test,  env_can_set(env, foo));
            LISP_TEST_ASSERT(test,  env_can_set(env, bar));
            LISP_TEST_ASSERT(test, env_get(env, foo) == bar);
            LISP_TEST_ASSERT(test, env_get(env, bar) == foo);

            env_del(env, foo);
            LISP_TEST_ASSERT(test, !env_can_set(env, foo));
            LISP_TEST_ASSERT(test,  env_can_set(env, bar));
            LISP_TEST_ASSERT(test, env_get(env, bar) == foo);
        }
        {
            Expr env1 = make_env(nil);
            Expr env2 = make_env(env1);
            Expr const foo = intern("foo");
            Expr const bar = intern("bar");

            LISP_TEST_ASSERT(test, !env_can_set(env1, foo));
            LISP_TEST_ASSERT(test, !env_can_set(env2, foo));

            LISP_TEST_ASSERT(test, !env_can_set(env1, bar));
            LISP_TEST_ASSERT(test, !env_can_set(env2, bar));

            env_def(env1, foo, foo);

            LISP_TEST_ASSERT(test,  env_can_set(env1, foo));
            LISP_TEST_ASSERT(test,  env_can_set(env2, foo));
            LISP_TEST_ASSERT(test, !env_can_set(env2, bar));
        }
    }

    char const * eval_src(char const * src, Expr env)
    {
        Expr const ret = eval(read_one_from_string(src), env);
        //println(ret);
        return repr(ret);
    }

    void unit_test_eval(TestState * test)
    {
        LISP_TEST_GROUP(test, "eval");
        LISP_TEST_ASSERT(test, eval(nil, nil) == nil);

        {
            Expr env = make_core_env();
            Expr t = intern("t");
            LISP_TEST_ASSERT(test, eval(t, env) == t);

            LISP_TEST_ASSERT(test, eval(intern("*env*"), env) == env);

            Expr foo = intern("foo");
            LISP_TEST_ASSERT(test, eval(make_quote(foo), env) == foo);
        }

        {
            Expr env = make_core_env();
            LISP_TEST_ASSERT(test, !strcmp("nil", eval_src("nil", env)));
            LISP_TEST_ASSERT(test, !strcmp("t", eval_src("t", env)));
            LISP_TEST_ASSERT(test, !strcmp("foo", eval_src("(quote foo)", env)));
#if LISP_READER_PARSE_QUOTE
            LISP_TEST_ASSERT(test, !strcmp("foo", eval_src("'foo", env)));
#endif
            LISP_TEST_ASSERT(test, !strcmp("foo", eval_src("(if t (quote foo) (quote bar))", env)));
            LISP_TEST_ASSERT(test, !strcmp("bar", eval_src("(if nil (quote foo) (quote bar))", env)));

            LISP_TEST_ASSERT(test, !strcmp("t", eval_src("(eq nil nil)", env)));
            LISP_TEST_ASSERT(test, !strcmp("nil", eval_src("(eq t nil)", env)));
            LISP_TEST_ASSERT(test, !strcmp("nil", eval_src("(eq t nil t)", env)));
            LISP_TEST_ASSERT(test, !strcmp("t", eval_src("(eq nil nil nil)", env)));
            LISP_TEST_ASSERT(test, !strcmp("t", eval_src("(eq t t t)", env)));

            LISP_TEST_ASSERT(test, !strcmp("t", eval_src("(equal nil nil)", env)));
            LISP_TEST_ASSERT(test, !strcmp("nil", eval_src("(equal t nil)", env)));
            LISP_TEST_ASSERT(test, !strcmp("nil", eval_src("(equal t nil t)", env)));
            LISP_TEST_ASSERT(test, !strcmp("t", eval_src("(equal nil nil nil)", env)));
            LISP_TEST_ASSERT(test, !strcmp("t", eval_src("(equal t t t)", env)));

            LISP_TEST_ASSERT(test, !strcmp("nil", eval_src("(println 'foo)", env)));

            LISP_TEST_ASSERT(test, !strcmp("(foo . bar)", eval_src("(cons 'foo 'bar)", env)));
            LISP_TEST_ASSERT(test, !strcmp("foo", eval_src("(car (cons 'foo 'bar))", env)));
            LISP_TEST_ASSERT(test, !strcmp("bar", eval_src("(cdr (cons 'foo 'bar))", env)));

            LISP_TEST_ASSERT(test, !strcmp("foo", eval_src("`foo", env)));
            LISP_TEST_ASSERT(test, !strcmp("foo", eval_src("`,'foo", env)));

            LISP_TEST_ASSERT(test, !strcmp("(foo bar)", eval_src("`(,@'(foo bar))", env)));
        }
    }

    void fail(char const * fmt, ...)
    {
        if (fmt)
        {
            va_list ap;
            va_start(ap, fmt);
            vfprintf(stderr, fmt, ap);
            va_end(ap);
        }
        fprintf(stderr,
                "usage: lisp <command> <options>\n"
                "commands:\n"
                "  unit ......... run unit tests\n"
                "  load {FILE} .. load source files\n"
            );
        exit(1);
    }

    Expr make_quote(Expr exp)
    {
        return list_2(intern("quote"), exp);
    }

    Expr make_backquote(Expr exp)
    {
        return list_2(intern("backquote"), exp);
    }
};

}

int main(int argc, char ** argv)
{
    return lisp::MySystem().main(argc, argv);
}
