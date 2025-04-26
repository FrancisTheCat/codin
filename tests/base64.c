#include "codin/codin.h"

#include "codin/base64.h"
#include "codin/strings.h"
#include "codin/test.h"

bool base64_decode_test() {
  String input = LIT("TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu");
  Byte_Slice output;
  bool ok = base64_decode(input, &output, context.allocator);
  if (!ok) {
    return false;
  }

  return string_equal(LIT("Many hands make light work."), bytes_to_string(output));
}

bool base64_encode_test() {
  String input = LIT("Many hands make light work.");
  String output = base64_encode(string_to_bytes(input), context.allocator);
  return string_equal(LIT("TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"), output);
}

void base64_add_tests(Test_Context *ctx) {
  test_add(ctx, base64_decode_test);
  test_add(ctx, base64_encode_test);
}
