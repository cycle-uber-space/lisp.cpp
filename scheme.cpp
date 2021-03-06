
//#define LISP_IMPLEMENTATION
#include "lisp.hpp"

namespace lisp {

class SchemeSystem
{
public:
    int main(int argc, char ** argv)
    {
        Expr env = make_env(nil);
        lang_defspecial(env, "define", [this](Expr args, Expr env)
        {
            Expr const head = car(args);
            Expr const tail = cdr(args);
            if (is_symbol(head))
            {
                env_def(env, head, eval(car(tail), env));
                return nil;
            }
            else
            {
                Expr const name = car(head);
                Expr const args = cdr(head);
                Expr const body = tail;
                env_def(env, name, make_function(env, name, args, body));
                return nil;
            }
        });

        lang_defspecial(env, "lambda", [this](Expr args, Expr env) -> Expr
        {
            Expr const fun_args = car(args);
            Expr const fun_body = cdr(args);
            return make_function(env, nil, fun_args, fun_body);
        });

        lang_defun(env, "display", [this](Expr args, Expr) -> Expr
        {
            display(car(args));
            return nil;
        });

        lang_defun(env, "displayln", [this](Expr args, Expr) -> Expr
        {
            displayln(car(args));
            return nil;
        });

        for (int i = 1; i < argc; i++)
        {
            load_file(argv[i], env);
        }

        repl(env);

        return 0;
    }
};

}

int main(int argc, char ** argv)
{
    return lisp::SchemeSystem().main(argc, argv);
}

#define LISP_IMPLEMENTATION
#include "lisp.hpp"
