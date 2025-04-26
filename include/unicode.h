#pragma once

#include "codin.h"

#define UTF8_BOM LIT("\xEF\xBB\xBF")

extern rune utf8_first_rune(String str, isize *n);
extern rune utf8_rune_at(String str, isize index, isize *n);

// `buf` should point to an array of at least four `char`s
extern isize utf8_rune_encode(rune r, char *buf);
