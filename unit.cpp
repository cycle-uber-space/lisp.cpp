
#define LISP_IMPLEMENTATION
#include "lisp.hpp"

int main(int argc, char ** argv)
{
    TestState _test;
    TestState * test = &_test;
    LISP_TEST_BEGIN(test);
    LISP_TEST_GROUP(test, "summary");
    LISP_TEST_FINISH(test);
    return 0;
}
