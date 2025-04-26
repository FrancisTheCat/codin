#include "codin/strings.h"
#include "codin/test.h"

bool cstring_len_test() {
  test_expect(cstring_len(nil)  == 0);
  test_expect(cstring_len("")   == 0);
  test_expect(cstring_len("\0") == 0);
  test_expect(cstring_len("0")  == 1);

  test_expect(cstring_len("1234567890") == 10);
  test_expect(cstring_len("12345678901234567890") == 20);
  test_expect(cstring_len("1234567890123456789012345678901234567890") == 40);
  test_expect(cstring_len("0123456789_öüä!") == 21);

  return true;
}

bool string_equal_test() {
  #define STR_SELF_COMPARE(str)                                                \
    test_expect(string_equal(LIT(str), LIT(str)));                             \
    test_expect(cstring_equal(str, str));

  #define STR_EXPECT_DIFFERENT(a, b)                                            \
    test_expect(!string_equal(LIT(a), LIT(b)));                                 \
    test_expect(!cstring_equal(a, b));

  STR_SELF_COMPARE("asdf");
  STR_SELF_COMPARE("1234567890");
  STR_SELF_COMPARE("0123456789_öüä!");
  STR_SELF_COMPARE("0123456789_öüä!\0!");
  STR_SELF_COMPARE("pbrMetallicRoughness");
  STR_SELF_COMPARE("12345678901234567890123456789012345678901234567890");
  STR_SELF_COMPARE("12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

  STR_EXPECT_DIFFERENT("asdf", "asd\0");
  STR_EXPECT_DIFFERENT("0123456789_öüä", "0123456789_öüä");
  STR_EXPECT_DIFFERENT(
    "0123456789_öüä0123456789_öüä0123456789_öüä!",
    "0123456789_öüä0123456789_öüä0123456789_öüä"
  );
  STR_EXPECT_DIFFERENT(
     "0123456789_öüä0123456789_öüä0123456789_öüä!",
     "0123456789_öüä0123456789_öüä0123456789_öüä1"
  );

  return true;
}

bool string_iter_test() {
  String str       = LIT("0123456789_öüä!");
  rune   runes[17] = {
      '0', '1', '2',  '3',  '4',  '5',  '6',  '7',
      '8', '9', '_', L'ö', L'ü', L'ä', L'', '!'
  };

  isize n = 0;
  string_iter(str, r, i, {
    test_expect(runes[n] == r);
    n += 1;
  });

  return true;
}

bool string_index_byte_test() {
  String str = LIT("0123456789!!______________________________________");

  test_expect(string_index_byte(str, '0') == 0);
  test_expect(string_index_byte(str, '1') == 1);
  test_expect(string_index_byte(str, '2') == 2);
  test_expect(string_index_byte(str, '3') == 3);
  test_expect(string_index_byte(str, '4') == 4);
  test_expect(string_index_byte(str, '5') == 5);
  test_expect(string_index_byte(str, '6') == 6);
  test_expect(string_index_byte(str, '7') == 7);
  test_expect(string_index_byte(str, '8') == 8);
  test_expect(string_index_byte(str, '9') == 9);

  test_expect(string_index_byte(str, '!') == 10);
  test_expect(string_index_byte(str, '_') == 12);

  test_expect(string_index_byte(str, 'X') == -1);

  return true;
}

bool string_index_rune_test() {
  String str = LIT("0123456789_öüä!");

  test_expect(string_index_rune(str, '0') == 0);
  test_expect(string_index_rune(str, '1') == 1);
  test_expect(string_index_rune(str, '2') == 2);
  test_expect(string_index_rune(str, '3') == 3);
  test_expect(string_index_rune(str, '4') == 4);
  test_expect(string_index_rune(str, '5') == 5);
  test_expect(string_index_rune(str, '6') == 6);
  test_expect(string_index_rune(str, '7') == 7);
  test_expect(string_index_rune(str, '8') == 8);
  test_expect(string_index_rune(str, '9') == 9);

  // these characters are two bytes in utf8
  test_expect(string_index_rune(str, L'ö') == 11);
  test_expect(string_index_rune(str, L'ü') == 13);
  test_expect(string_index_rune(str, L'ä') == 15);

  // and this one is three bytes
  test_expect(string_index_rune(str, L'') == 17);

  test_expect(string_index_rune(str, '!') == 20);

  test_expect(string_index_byte(str, 'X') == -1);

  return true;
}

bool string_to_lower_test() {
  #define CASE_TEST(lower, upper)                                                   \
    test_expect(                                                                    \
      string_equal(LIT(lower), string_to_lower(LIT(upper), context.temp_allocator)) \
    );                                                                              \
    test_expect(                                                                    \
      string_equal(LIT(upper), string_to_upper(LIT(lower), context.temp_allocator)) \
    );

  CASE_TEST("asdf", "ASDF");
  CASE_TEST("asdf1234", "ASDF1234");
  CASE_TEST(
    "asdf1234asßßßöööüüüdf1234asdf1234asdf1234asdf1234asdf1234asdf1234asdf1234asdf1234",
    "ASDF1234ASßßßöööüüüDF1234ASDF1234ASDF1234ASDF1234ASDF1234ASDF1234ASDF1234ASDF1234"
  );

  return true;
}

bool utf8_encode_test() {
  char  buf[4];
  isize n;

  for_range(i, 0, 128) {
    n = utf8_rune_encode(i, buf);
    test_expect(n == 1);
    test_expect(buf[0] == i);
  }

  n = utf8_rune_encode(L'ö', buf);
  test_expect(n == 2);
  test_expect(utf8_first_rune((String) { .data = (char *)buf, .len = n, }, nil) == L'ö');

  n = utf8_rune_encode(L'', buf);
  test_expect(n == 3);
  test_expect(utf8_first_rune((String) { .data = (char *)buf, .len = n, }, nil) == L'');

  n = utf8_rune_encode(L'𝔸', buf);
  test_expect(n == 4);
  test_expect(utf8_first_rune((String) { .data = (char *)buf, .len = n, }, nil) == L'𝔸');

  return true;
}

void strings_add_tests(Test_Context *ctx) {
  test_add(ctx, utf8_encode_test);

  test_add(ctx, cstring_len_test);
  test_add(ctx, string_equal_test);
  test_add(ctx, string_iter_test);
  test_add(ctx, string_index_byte_test);
  test_add(ctx, string_index_rune_test);
  test_add(ctx, string_to_lower_test);
}
