#include "test.h"
#include "fmt.h"

extern void test_context_init(Test_Context *tc, Allocator allocator) {
  vector_init(&tc->tests, 0, 8, allocator);
}

extern void test_context_destroy(Test_Context tc) { vector_delete(tc.tests); }

extern void test_context_execute(Test_Context *tc) {
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
