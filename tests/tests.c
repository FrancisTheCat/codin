#include "codin/codin.h"
#include "codin/test.h"

#include "base64.c"

i32 main() {
  Test_Context ctx;
  test_context_init(&ctx, context.allocator);

  test_add(&ctx, base64_decode_test);
  test_add(&ctx, base64_encode_test);

  test_context_execute(&ctx);
  test_context_destroy(ctx);
}
