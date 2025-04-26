#include "codin/codin.h"
#include "codin/test.h"

#include "base64.c"
#include "strings.c"

i32 main() {
  Test_Context ctx;
  test_context_init(&ctx, context.allocator);

  base64_add_tests(&ctx);
  strings_add_tests(&ctx);

  test_context_execute(&ctx);
  test_context_destroy(ctx);
}
