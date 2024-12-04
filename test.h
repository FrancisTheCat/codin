#include "codin.h"

typedef b8 (*Test_Procedure)();

typedef struct {
  Test_Procedure proc;
  String name;
} Internal_Test;

typedef struct {
  Vector(Internal_Test) tests;
} Test_Context;

internal void test_context_init(Test_Context *tc, Allocator allocator) {
  vector_init(&tc->tests, 0, 8, allocator);
}

internal void test_context_destroy(Test_Context tc) { vector_delete(tc.tests); }

#define test_add(ctx, test)                                                    \
  {                                                                            \
    Internal_Test t;                                                           \
    t.proc = test;                                                             \
    t.name = LIT(#test);                                                       \
    vector_append(&(ctx)->tests, t);                                           \
  }

internal void test_context_execute(Test_Context *tc) {
  vector_iter(tc->tests, test, i, {
    b8 result = test->proc();
    fmt_printf(LIT("Test: '%S' "), test->name);
    if (result) {
      fmt_println(LIT("Succeeded\n"));
    } else {
      fmt_println(LIT("Failed\n"));
    }
  });
}

#define test_expect(cond)                                                      \
  if (!(cond)) {                                                               \
    Source_Code_Location loc = CALLER_LOCATION;                                \
    fmt_wprintf(&stderr, LIT("Test failed at %L\n"), loc);                     \
    return false;                                                              \
  }

#define test_fail() test_expect(false)
