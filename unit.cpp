
#define LISP_WANT_SYSTEM_API 0
#define LISP_WANT_GLOBAL_API 1
#include "lisp.hpp"

int main(int argc, char ** argv)
{
    TestState _test;
    TestState * test = &_test;
    LISP_TEST_BEGIN(test);
    LISP_TEST_GROUP(test, "base");
    LISP_TEST_ASSERT(test, sizeof(U8 ) == 1);
    LISP_TEST_ASSERT(test, sizeof(U16) == 2);
    LISP_TEST_ASSERT(test, sizeof(U32) == 4);
    LISP_TEST_ASSERT(test, sizeof(U64) == 8);
    LISP_TEST_ASSERT(test, sizeof(I8 ) == 1);
    LISP_TEST_ASSERT(test, sizeof(I16) == 2);
    LISP_TEST_ASSERT(test, sizeof(I32) == 4);
    LISP_TEST_ASSERT(test, sizeof(I64) == 8);
    LISP_TEST_ASSERT(test, sizeof(F32) == 4);
    LISP_TEST_ASSERT(test, sizeof(F64) == 8);
    LISP_TEST_ASSERT(test, sizeof(V64) == 8);
    LISP_TEST_GROUP(test, "fixnum");
    LISP_TEST_ASSERT(test, fixnum_value(make_fixnum(0)) == 0);
    LISP_TEST_ASSERT(test, fixnum_value(make_fixnum(1)) == 1);
    LISP_TEST_ASSERT(test, fixnum_value(make_fixnum(-1)) == -1);
    LISP_TEST_GROUP(test, "summary");
    LISP_TEST_FINISH(test);
    return 0;
}

#define LISP_IMPLEMENTATION
#include "lisp.hpp"
