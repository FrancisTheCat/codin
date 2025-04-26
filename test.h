#pragma once

#include "codin.h"
#include "fmt.h"

typedef bool (*Test_Procedure)();

typedef struct {
  Test_Procedure proc;
  String         name;
} Internal_Test;

typedef struct {
  Vector(Internal_Test) tests;
} Test_Context;

extern void test_context_init(Test_Context *tc, Allocator allocator);
extern void test_context_execute(Test_Context *tc);
extern void test_context_destroy(Test_Context tc);

#define test_add(ctx, test)                                                    \
  {                                                                            \
    Internal_Test t;                                                           \
    t.proc = test;                                                             \
    t.name = LIT(#test);                                                       \
    vector_append(&(ctx)->tests, t);                                           \
  }

#define test_expect(cond)                                                      \
  if (!(cond)) {                                                               \
    Source_Code_Location loc = CALLER_LOCATION;                                \
    fmt_wprintf(&std_err, LIT("Test failed at %L\n"), loc);                    \
    return false;                                                              \
  }

#define test_fail() test_expect(false)
