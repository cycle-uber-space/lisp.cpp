
//#define LISP_IMPLEMENTATION
#include "lisp.hpp"

#define LISP_SYM_LIT intern("lit")
#define LISP_SYM_CLO intern("clo")

namespace lisp {

class BelSystem
{
public:
    BelSystem()
    {
        // TODO use a proper rng
        srand(time(NULL));
    }

    int main(int argc, char ** argv)
    {
        Expr env = make_env(nil);

        lang_defspecial_quote(env);
        lang_defspecial_while(env); // TODO should be a macro

        lang_defspecial(env, "fn", [this](Expr args, Expr env) -> Expr
        {
            return list(LISP_SYM_LIT, LISP_SYM_CLO, env, car(args), cdr(args));
        });

        lang_defun(env, "coin", [this](Expr args, Expr) -> Expr
        {
            LISP_ASSERT(args == nil);
            return (rand() & 1) ? LISP_SYMBOL_T : nil;
        });

        lang_defun_println(env, "prn");

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
    return lisp::BelSystem().main(argc, argv);
}

#define LISP_IMPLEMENTATION
#include "lisp.hpp"
