#include "codin.h"
#include "image.h"
#include "bit_array.h"

#define QR_BYTE_ENCODING 0b0100
#define QR_ECI_ENCODING  0b0111

#define QR_ERROR_CORRECTIONS(X) \
  X(QR_Error_Correction_L)      \
  X(QR_Error_Correction_M)      \
  X(QR_Error_Correction_Q)      \
  X(QR_Error_Correction_H)      \

X_ENUM(QR_Error_Correction, QR_ERROR_CORRECTIONS);

#undef QR_ERROR_CORRECTIONS

internal isize qr__code_capacities[enum_len(QR_Error_Correction)][41] = {
  [QR_Error_Correction_L] = {
    17,   32,   53,   78,   106,  134,  154,  192,  230,  271,
    321,  367,  425,  458,  520,  586,  644,  718,  792,  858,
    929,  1003, 1091, 1171, 1273, 1367, 1465, 1528, 1628, 1732,
    1840, 1952, 2068, 2188, 2303, 2431, 2563, 2699, 2809, 2953,
  },
  [QR_Error_Correction_M] = {
    14,   26,   42,   62,   84,   106,  122,  152,  180,  213,
    251,  287,  331,  362,  412,  450,  504,  560,  624,  666,
    711,  779,  857,  911,  997,  1059, 1125, 1190, 1264, 1370,
    1452, 1538, 1628, 1722, 1809, 1911, 1989, 2099, 2213, 2331,
  },
  [QR_Error_Correction_Q] = {
    11,   20,   32,   46,   60,   74,   86,   108,  130,  151,
    177,  203,  241,  258,  292,  322,  364,  394,  442,  482,
    509,  565,  611,  661,  715,  751,  805,  868,  908,  982,
    1030, 1112, 1168, 1228, 1283, 1351, 1423, 1499, 1579, 1663,
  },
  [QR_Error_Correction_H] = {
    7,   14,  24,  34,  44,  58,   64,   84,   98,   119,
    137, 155, 177, 194, 220, 250,  280,  310,  338,  382,
    403, 439, 461, 511, 535, 593,  625,  658,  698,  742,
    790, 842, 898, 958, 983, 1051, 1093, 1139, 1219, 1273,
  },
};

#define QR_ERROR_CORRECTION_INFO(correction_level, version) \
  qr__error_correction_table[version * enum_len(QR_Error_Correction) + correction_level]

typedef struct {
  i16 data_words;
  i16 error_words_per_block;
  i16 blocks_group1;
  i16 data_words_per_block_group1;
  i16 blocks_group2;
  i16 data_words_per_block_group2;
} QR_Error_Correction_Info;

internal QR_Error_Correction_Info qr__error_correction_table[] = {
  [1  * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {   19,  7,  1,  19,  0,   0 },
  [1  * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {   16, 10,  1,  16,  0,   0 },
  [1  * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {   13, 13,  1,  13,  0,   0 },
  [1  * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {    9, 17,  1,   9,  0,   0 },
  [2  * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {   34, 10,  1,  34,  0,   0 },
  [2  * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {   28, 16,  1,  28,  0,   0 },
  [2  * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {   22, 22,  1,  22,  0,   0 },
  [2  * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {   16, 28,  1,  16,  0,   0 },
  [3  * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {   55, 15,  1,  55,  0,   0 },
  [3  * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {   44, 26,  1,  44,  0,   0 },
  [3  * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {   34, 18,  2,  17,  0,   0 },
  [3  * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {   26, 22,  2,  13,  0,   0 },
  [4  * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {   80, 20,  1,  80,  0,   0 },
  [4  * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {   64, 18,  2,  32,  0,   0 },
  [4  * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {   48, 26,  2,  24,  0,   0 },
  [4  * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {   36, 16,  4,   9,  0,   0 },
  [5  * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  108, 26,  1, 108,  0,   0 },
  [5  * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {   86, 24,  2,  43,  0,   0 },
  [5  * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {   62, 18,  2,  15,  2,  16 },
  [5  * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {   46, 22,  2,  11,  2,  12 },
  [6  * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  136, 18,  2,  68,  0,   0 },
  [6  * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  108, 16,  4,  27,  0,   0 },
  [6  * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {   76, 24,  4,  19,  0,   0 },
  [6  * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {   60, 28,  4,  15,  0,   0 },
  [7  * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  156, 20,  2,  78,  0,   0 },
  [7  * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  124, 18,  4,  31,  0,   0 },
  [7  * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {   88, 18,  2,  14,  4,  15 },
  [7  * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {   66, 26,  4,  13,  1,  14 },
  [8  * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  194, 24,  2,  97,  0,   0 },
  [8  * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  154, 22,  2,  38,  2,  39 },
  [8  * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  110, 22,  4,  18,  2,  19 },
  [8  * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {   86, 26,  4,  14,  2,  15 },
  [9  * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  232, 30,  2, 116,  0,   0 },
  [9  * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  182, 22,  3,  36,  2,  37 },
  [9  * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  132, 20,  4,  16,  4,  17 },
  [9  * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  100, 24,  4,  12,  4,  13 },
  [10 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  274, 18,  2,  68,  2,  69 },
  [10 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  216, 26,  4,  43,  1,  44 },
  [10 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  154, 24,  6,  19,  2,  20 },
  [10 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  122, 28,  6,  15,  2,  16 },
  [11 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  324, 20,  4,  81,  0,   0 },
  [11 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  254, 30,  1,  50,  4,  51 },
  [11 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  180, 28,  4,  22,  4,  23 },
  [11 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  140, 24,  3,  12,  8,  13 },
  [12 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  370, 24,  2,  92,  2,  93 },
  [12 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  290, 22,  6,  36,  2,  37 },
  [12 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  206, 26,  4,  20,  6,  21 },
  [12 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  158, 28,  7,  14,  4,  15 },
  [13 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  428, 26,  4, 107,  0,   0 },
  [13 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  334, 22,  8,  37,  1,  38 },
  [13 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  244, 24,  8,  20,  4,  21 },
  [13 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  180, 22, 12,  11,  4,  12 },
  [14 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  461, 30,  3, 115,  1, 116 },
  [14 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  365, 24,  4,  40,  5,  41 },
  [14 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  261, 20, 11,  16,  5,  17 },
  [14 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  197, 24, 11,  12,  5,  13 },
  [15 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  523, 22,  5,  87,  1,  88 },
  [15 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  415, 24,  5,  41,  5,  42 },
  [15 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  295, 30,  5,  24,  7,  25 },
  [15 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  223, 24, 11,  12,  7,  13 },
  [16 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  589, 24,  5,  98,  1,  99 },
  [16 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  453, 28,  7,  45,  3,  46 },
  [16 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  325, 24, 15,  19,  2,  20 },
  [16 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  253, 30,  3,  15, 13,  16 },
  [17 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  647, 28,  1, 107,  5, 108 },
  [17 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  507, 28, 10,  46,  1,  47 },
  [17 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  367, 28,  1,  22, 15,  23 },
  [17 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  283, 28,  2,  14, 17,  15 },
  [18 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  721, 30,  5, 120,  1, 121 },
  [18 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  563, 26,  9,  43,  4,  44 },
  [18 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  397, 28, 17,  22,  1,  23 },
  [18 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  313, 28,  2,  14, 19,  15 },
  [19 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  795, 28,  3, 113,  4, 114 },
  [19 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  627, 26,  3,  44, 11,  45 },
  [19 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  445, 26, 17,  21,  4,  22 },
  [19 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  341, 26,  9,  13, 16,  14 },
  [20 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  861, 28,  3, 107,  5, 108 },
  [20 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  669, 26,  3,  41, 13,  42 },
  [20 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  485, 30, 15,  24,  5,  25 },
  [20 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  385, 28, 15,  15, 10,  16 },
  [21 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = {  932, 28,  4, 116,  4, 117 },
  [21 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  714, 26, 17,  42,  0,   0 },
  [21 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  512, 28, 17,  22,  6,  23 },
  [21 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  406, 30, 19,  16,  6,  17 },
  [22 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1006, 28,  2, 111,  7, 112 },
  [22 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  782, 28, 17,  46,  0,   0 },
  [22 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  568, 30,  7,  24, 16,  25 },
  [22 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  442, 24, 34,  13,  0,   0 },
  [23 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1094, 30,  4, 121,  5, 122 },
  [23 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  860, 28,  4,  47, 14,  48 },
  [23 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  614, 30, 11,  24, 14,  25 },
  [23 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  464, 30, 16,  15, 14,  16 },
  [24 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1174, 30,  6, 117,  4, 118 },
  [24 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = {  914, 28,  6,  45, 14,  46 },
  [24 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  664, 30, 11,  24, 16,  25 },
  [24 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  514, 30, 30,  16,  2,  17 },
  [25 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1276, 26,  8, 106,  4, 107 },
  [25 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1000, 28,  8,  47, 13,  48 },
  [25 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  718, 30,  7,  24, 22,  25 },
  [25 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  538, 30, 22,  15, 13,  16 },
  [26 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1370, 28, 10, 114,  2, 115 },
  [26 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1062, 28, 19,  46,  4,  47 },
  [26 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  754, 28, 28,  22,  6,  23 },
  [26 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  596, 30, 33,  16,  4,  17 },
  [27 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1468, 30,  8, 122,  4, 123 },
  [27 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1128, 28, 22,  45,  3,  46 },
  [27 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  808, 30,  8,  23, 26,  24 },
  [27 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  628, 30, 12,  15, 28,  16 },
  [28 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1531, 30,  3, 117, 10, 118 },
  [28 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1193, 28,  3,  45, 23,  46 },
  [28 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  871, 30,  4,  24, 31,  25 },
  [28 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  661, 30, 11,  15, 31,  16 },
  [29 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1631, 30,  7, 116,  7, 117 },
  [29 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1267, 28, 21,  45,  7,  46 },
  [29 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  911, 30,  1,  23, 37,  24 },
  [29 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  701, 30, 19,  15, 26,  16 },
  [30 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1735, 30,  5, 115, 10, 116 },
  [30 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1373, 28, 19,  47, 10,  48 },
  [30 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = {  985, 30, 15,  24, 25,  25 },
  [30 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  745, 30, 23,  15, 25,  16 },
  [31 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1843, 30, 13, 115,  3, 116 },
  [31 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1455, 28,  2,  46, 29,  47 },
  [31 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = { 1033, 30, 42,  24,  1,  25 },
  [31 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  793, 30, 23,  15, 28,  16 },
  [32 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 1955, 30, 17, 115,  0,   0 },
  [32 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1541, 28, 10,  46, 23,  47 },
  [32 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = { 1115, 30, 10,  24, 35,  25 },
  [32 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  845, 30, 19,  15, 35,  16 },
  [33 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 2071, 30, 17, 115,  1, 116 },
  [33 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1631, 28, 14,  46, 21,  47 },
  [33 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = { 1171, 30, 29,  24, 19,  25 },
  [33 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  901, 30, 11,  15, 46,  16 },
  [34 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 2191, 30, 13, 115,  6, 116 },
  [34 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1725, 28, 14,  46, 23,  47 },
  [34 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = { 1231, 30, 44,  24,  7,  25 },
  [34 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  961, 30, 59,  16,  1,  17 },
  [35 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 2306, 30, 12, 121,  7, 122 },
  [35 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1812, 28, 12,  47, 26,  48 },
  [35 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = { 1286, 30, 39,  24, 14,  25 },
  [35 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = {  986, 30, 22,  15, 41,  16 },
  [36 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 2434, 30,  6, 121, 14, 122 },
  [36 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1914, 28,  6,  47, 34,  48 },
  [36 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = { 1354, 30, 46,  24, 10,  25 },
  [36 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = { 1054, 30,  2,  15, 64,  16 },
  [37 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 2566, 30, 17, 122,  4, 123 },
  [37 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 1992, 28, 29,  46, 14,  47 },
  [37 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = { 1426, 30, 49,  24, 10,  25 },
  [37 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = { 1096, 30, 24,  15, 46,  16 },
  [38 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 2702, 30,  4, 122, 18, 123 },
  [38 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 2102, 28, 13,  46, 32,  47 },
  [38 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = { 1502, 30, 48,  24, 14,  25 },
  [38 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = { 1142, 30, 42,  15, 32,  16 },
  [39 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 2812, 30, 20, 117,  4, 118 },
  [39 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 2216, 28, 40,  47,  7,  48 },
  [39 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = { 1582, 30, 43,  24, 22,  25 },
  [39 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = { 1222, 30, 10,  15, 67,  16 },
  [40 * enum_len(QR_Error_Correction) + QR_Error_Correction_L] = { 2956, 30, 19, 118,  6, 119 },
  [40 * enum_len(QR_Error_Correction) + QR_Error_Correction_M] = { 2334, 28, 18,  47, 31,  48 },
  [40 * enum_len(QR_Error_Correction) + QR_Error_Correction_Q] = { 1666, 30, 34,  24, 34,  25 },
  [40 * enum_len(QR_Error_Correction) + QR_Error_Correction_H] = { 1276, 30, 20,  15, 61,  16 },
};

internal b8 qr__luts_generated = false;
internal u8 qr__galois_power_2_lut    [256] = {0};
internal u8 qr__galois_power_2_lut_rev[256] = {0};
internal u8 qr__bit_order_swap_lut    [256] = {0};

internal void qr__generate_lut() {
  spall_begin_fn();
  for_range(i, 0, 255) {
    isize value = 1;
    for_range(j, 0, i) {
      value = value << 1;
      if (value > 255) {
        value = value ^ 285;
      }
    }
    qr__galois_power_2_lut[i]         = value;
    qr__galois_power_2_lut_rev[value] = i;

    u8 swap = 0;
    for_range(j, 0, 8) {
      swap |= !!(i & (1 << j)) << (7 - j);
    }
    qr__bit_order_swap_lut[i] = swap;
  }
  qr__luts_generated = true;
  spall_end();
}

internal u8 qr__galois_mul(u8 a, u8 b) {
  if (a == 0 || b == 0) {
    return 0;
  }
  isize a_log2 = qr__galois_power_2_lut_rev[a];
  isize b_log2 = qr__galois_power_2_lut_rev[b];
  return qr__galois_power_2_lut[(a_log2 + b_log2) % 255];
}

// [n]: factor for nth power of x
typedef Slice(u8) Galois_Polynomial;

// will generate a generator polynomial with degree = p.len - 1
internal void qr__generate_generator_polynomial(Galois_Polynomial p) {
  spall_begin_fn();
  assert(qr__luts_generated);
  assert(p.len >= 3);

  mem_zero(p.data, p.len);

  p.data[0] = qr__galois_power_2_lut[1];
  p.data[1] = qr__galois_power_2_lut[25];
  p.data[2] = qr__galois_power_2_lut[0];

  for (isize i = 3; i < p.len; i += 1) {
    for (isize j = i; j >= 0; j -= 1) {
      isize a  = p.data[j];
      isize a_ = j > 0 ? p.data[j - 1] : 0;

      p.data[j] = a_ ^ qr__galois_mul(a, qr__galois_power_2_lut[i - 1]);
    }
  }
  spall_end();
}

internal void qr__modulo_galois_polynomial(
  Galois_Polynomial dividend,
  Galois_Polynomial divisor
) {
  spall_begin_fn();
  assert(qr__luts_generated);
  for (isize i = dividend.len - 1; i >= 0; i -= 1) {
    if (i + 1 < divisor.len) {
      break;
    }

    u8 m = IDX(dividend, i);
    for (isize j = 0; j < divisor.len; j += 1) {
      IDX(dividend, i + j - divisor.len + 1) ^= qr__galois_mul(m, IDX(divisor, j));
    }
  }
  spall_end();
}

internal void qr__generate_error_correction_codes(
  QR_Error_Correction_Info const *info,
  Byte_Slice                      data,
  Byte_Buffer                    *ecs
) {
  spall_begin_fn();
  assert(qr__luts_generated);
  assert(data.len == info->data_words);

  Galois_Polynomial g = alloca_slice(Galois_Polynomial, info->error_words_per_block + 1);
  qr__generate_generator_polynomial(g);

  Galois_Polynomial m = alloca_slice(Galois_Polynomial, info->data_words_per_block_group1 + info->error_words_per_block);

  Galois_Polynomial out;

  isize data_offset = 0;

  for_range(i, 0, info->blocks_group1) {
    mem_zero(m.data, info->error_words_per_block);
    slice_iter_v(slice_range(data, data_offset, data_offset + info->data_words_per_block_group1), d, i, {
      IDX(m, info->error_words_per_block + info->data_words_per_block_group1 - i - 1) = d;
    });
    // mem_copy(m.data + info->error_words_per_block, &IDX(data, data_offset), info->data_words_per_block_group1);
    // slice_reverse(slice_start(m, info->error_words_per_block));

    // slice_iter(m, b, i, {
    //   *b = qr__bit_order_swap_lut[*b];
    // });

    qr__modulo_galois_polynomial(m, g);

    out = slice_end(m, info->error_words_per_block);
    slice_reverse(out);

    vector_append_slice(ecs, slice_to_bytes(out));

    data_offset += info->data_words_per_block_group1;
  }
  
  m = alloca_slice(Galois_Polynomial, info->data_words_per_block_group2 + info->error_words_per_block);

  for_range(i, 0, info->blocks_group2) {
    mem_zero(m.data, info->error_words_per_block);
    // mem_copy(m.data + info->error_words_per_block, &IDX(data, data_offset), info->data_words_per_block_group2);
    // slice_reverse(slice_start(m, info->error_words_per_block));
    slice_iter_v(slice_range(data, data_offset, data_offset + info->data_words_per_block_group2), d, i, {
      IDX(m, info->error_words_per_block + info->data_words_per_block_group2 - i - 1) = d;
    });

    // slice_iter(m, b, i, {
    //   *b = qr__bit_order_swap_lut[*b];
    // });

    qr__modulo_galois_polynomial(m, g);

    out = slice_end(m, info->error_words_per_block);
    slice_reverse(out);

    vector_append_slice(ecs, slice_to_bytes(out));

    data_offset += info->data_words_per_block_group2;
  }
  spall_end();
}

internal isize qr__required_version(isize data_len, QR_Error_Correction correction_level) {
  Slice(isize) capacities = c_array_to_slice_t(type_of(capacities), qr__code_capacities[correction_level]);
  slice_iter_v(capacities, cap, version, {
    if (data_len <= cap) {
      return version + 1;
    }
  });
  return -1;
}

internal isize qr__character_count_bits(isize version) {
  if (in_range(version, 0, 10)) {
    return 8;
  }
  if (in_range(version, 10, 27)) {
    return 16;
  }
  if (in_range(version, 27, 41)) {
    return 16;
  }
  unreachable();
}

internal void qr__generate_data_bits(
  isize               version,
  QR_Error_Correction correction_level,
  Byte_Slice          data,
  Bit_Array          *ba
) {
  spall_begin_fn();
  assert(ba->len == 0);

  bit_array_append_n(ba, QR_BYTE_ENCODING, 4);
  bit_array_append_n(ba, data.len, qr__character_count_bits(version));

  slice_iter_v(data, value, i, {
    bit_array_append_u8(ba, value);
  });

  if (ba->len % 8) {
    bit_array_append_n(ba, 0, 8 - ba->len % 8);
  }
  spall_end();
}

internal isize qr__alignment_locations[41][7] = {
  [ 2] = { 6, 18,                        },
  [ 3] = { 6, 22,                        },
  [ 4] = { 6, 26,                        },
  [ 5] = { 6, 30,                        },
  [ 6] = { 6, 34,                        },
  [ 7] = { 6, 22, 38,                    },
  [ 8] = { 6, 24, 42,                    },
  [ 9] = { 6, 26, 46,                    },
  [10] = { 6, 28, 50,                    },
  [11] = { 6, 30, 54,                    },
  [12] = { 6, 32, 58,                    },
  [13] = { 6, 34, 62,                    },
  [14] = { 6, 26, 46, 66,                },
  [15] = { 6, 26, 48, 70,                },
  [16] = { 6, 26, 50, 74,                },
  [17] = { 6, 30, 54, 78,                },
  [18] = { 6, 30, 56, 82,                },
  [19] = { 6, 30, 58, 86,                },
  [20] = { 6, 34, 62, 90,                },
  [21] = { 6, 28, 50, 72,  94,           },
  [22] = { 6, 26, 50, 74,  98,           },
  [23] = { 6, 30, 54, 78, 102,           },
  [24] = { 6, 28, 54, 80, 106,           },
  [25] = { 6, 32, 58, 84, 110,           },
  [26] = { 6, 30, 58, 86, 114,           },
  [27] = { 6, 34, 62, 90, 118,           },
  [28] = { 6, 26, 50, 74,  98, 122,      },
  [29] = { 6, 30, 54, 78, 102, 126,      },
  [30] = { 6, 26, 52, 78, 104, 130,      },
  [31] = { 6, 30, 56, 82, 108, 134,      },
  [32] = { 6, 34, 60, 86, 112, 138,      },
  [33] = { 6, 30, 58, 86, 114, 142,      },
  [34] = { 6, 34, 62, 90, 118, 146,      },
  [35] = { 6, 30, 54, 78, 102, 126, 150, },
  [36] = { 6, 24, 50, 76, 102, 128, 154, },
  [37] = { 6, 28, 54, 80, 106, 132, 158, },
  [38] = { 6, 32, 58, 84, 110, 136, 162, },
  [39] = { 6, 26, 54, 82, 110, 138, 166, },
  [40] = { 6, 30, 58, 86, 114, 142, 170, },
};

internal u64 qr__format_strings[32] = {
  [QR_Error_Correction_L * 8 + 0] = 0b111011111000100,
  [QR_Error_Correction_L * 8 + 1] = 0b111001011110011,
  [QR_Error_Correction_L * 8 + 2] = 0b111110110101010,
  [QR_Error_Correction_L * 8 + 3] = 0b111100010011101,
  [QR_Error_Correction_L * 8 + 4] = 0b110011000101111,
  [QR_Error_Correction_L * 8 + 5] = 0b110001100011000,
  [QR_Error_Correction_L * 8 + 6] = 0b110110001000001,
  [QR_Error_Correction_L * 8 + 7] = 0b110100101110110,
  [QR_Error_Correction_M * 8 + 0] = 0b101010000010010,
  [QR_Error_Correction_M * 8 + 1] = 0b101000100100101,
  [QR_Error_Correction_M * 8 + 2] = 0b101111001111100,
  [QR_Error_Correction_M * 8 + 3] = 0b101101101001011,
  [QR_Error_Correction_M * 8 + 4] = 0b100010111111001,
  [QR_Error_Correction_M * 8 + 5] = 0b100000011001110,
  [QR_Error_Correction_M * 8 + 6] = 0b100111110010111,
  [QR_Error_Correction_M * 8 + 7] = 0b100101010100000,
  [QR_Error_Correction_Q * 8 + 0] = 0b011010101011111,
  [QR_Error_Correction_Q * 8 + 1] = 0b011000001101000,
  [QR_Error_Correction_Q * 8 + 2] = 0b011111100110001,
  [QR_Error_Correction_Q * 8 + 3] = 0b011101000000110,
  [QR_Error_Correction_Q * 8 + 4] = 0b010010010110100,
  [QR_Error_Correction_Q * 8 + 5] = 0b010000110000011,
  [QR_Error_Correction_Q * 8 + 6] = 0b010111011011010,
  [QR_Error_Correction_Q * 8 + 7] = 0b010101111101101,
  [QR_Error_Correction_H * 8 + 0] = 0b001011010001001,
  [QR_Error_Correction_H * 8 + 1] = 0b001001110111110,
  [QR_Error_Correction_H * 8 + 2] = 0b001110011100111,
  [QR_Error_Correction_H * 8 + 3] = 0b001100111010000,
  [QR_Error_Correction_H * 8 + 4] = 0b000011101100010,
  [QR_Error_Correction_H * 8 + 5] = 0b000001001010101,
  [QR_Error_Correction_H * 8 + 6] = 0b000110100001100,
  [QR_Error_Correction_H * 8 + 7] = 0b000100000111011,
};

internal u64 qr__version_info_strings[41] = {
 [7]  = 0b000111110010010100,
 [8]  = 0b001000010110111100,
 [9]  = 0b001001101010011001,
 [10] = 0b001010010011010011,
 [11] = 0b001011101111110110,
 [12] = 0b001100011101100010,
 [13] = 0b001101100001000111,
 [14] = 0b001110011000001101,
 [15] = 0b001111100100101000,
 [16] = 0b010000101101111000,
 [17] = 0b010001010001011101,
 [18] = 0b010010101000010111,
 [19] = 0b010011010100110010,
 [20] = 0b010100100110100110,
 [21] = 0b010101011010000011,
 [22] = 0b010110100011001001,
 [23] = 0b010111011111101100,
 [24] = 0b011000111011000100,
 [25] = 0b011001000111100001,
 [26] = 0b011010111110101011,
 [27] = 0b011011000010001110,
 [28] = 0b011100110000011010,
 [29] = 0b011101001100111111,
 [30] = 0b011110110101110101,
 [31] = 0b011111001001010000,
 [32] = 0b100000100111010101,
 [33] = 0b100001011011110000,
 [34] = 0b100010100010111010,
 [35] = 0b100011011110011111,
 [36] = 0b100100101100001011,
 [37] = 0b100101010000101110,
 [38] = 0b100110101001100100,
 [39] = 0b100111010101000001,
 [40] = 0b101000110001101001,
};

#define QR__FIXED_BIT 0x80

internal void qr__place_fixed_patterns(Image const *image, isize version, QR_Error_Correction level) {
  spall_begin_fn();
  // Finder Patterns
  #define QR__FINDER_PIXEL(x, y, v)                                                        \
    IDX(image->pixels, x +                     y                      * image->width) = v; \
    IDX(image->pixels, x + image->width - 7 +  y                      * image->width) = v; \
    IDX(image->pixels, x +                    (y + image->height - 7) * image->width) = v; \

  for_range(y, 0, 7) {
    for_range(x, 0, 7) {
      QR__FINDER_PIXEL(x, y, 0 | QR__FIXED_BIT);
    }
  }

  for_range(i, 1, 6) {
    QR__FINDER_PIXEL(i, 1, 1 | QR__FIXED_BIT);
    QR__FINDER_PIXEL(i, 5, 1 | QR__FIXED_BIT);
    QR__FINDER_PIXEL(1, i, 1 | QR__FIXED_BIT);
    QR__FINDER_PIXEL(5, i, 1 | QR__FIXED_BIT);
  }

  for_range(x, 0, 8) {
    IDX(image->pixels, x +                    7                   * image->width) = 1 | QR__FIXED_BIT;
    IDX(image->pixels, x + image->width - 8 + 7                   * image->width) = 1 | QR__FIXED_BIT;
    IDX(image->pixels, x                    + (image->height - 8) * image->width) = 1 | QR__FIXED_BIT;
  }

  for_range(y, 0, 7) {
    IDX(image->pixels, 7                +  y                      * image->width) = 1 | QR__FIXED_BIT;
    IDX(image->pixels, image->width - 8 +  y                      * image->width) = 1 | QR__FIXED_BIT;
    IDX(image->pixels, 7                + (y + image->height - 7) * image->width) = 1 | QR__FIXED_BIT;
  }

  #undef QR__FINDER_PIXEL

  // Alignment Patterns
  Slice(isize) alignment_locations = { .data = qr__alignment_locations[version], .len = 7 };

  slice_iter_v(alignment_locations, x, i, {
    if (!x) {
      break;
    }
    slice_iter_v(alignment_locations, y, j, {
      if (!y) {
        break;
      }

      b8 skip = false;
      for_range(_y, y - 2, y + 3) {
        for_range(_x, x - 2, x + 3) {
          if (IDX(image->pixels, _x + _y * image->width) & QR__FIXED_BIT) {
            skip = true;
          }
        }
      }

      if (!skip) {
        for_range(_y, y - 2, y + 3) {
          for_range(_x, x - 2, x + 3) {
            isize dx = x - _x;
            isize dy = y - _y;

            if (dx < 0) dx = -dx;
            if (dy < 0) dy = -dy;

            isize dist = max(dx, dy);
            IDX(image->pixels, _x + _y * image->width) = (dist & 1) | QR__FIXED_BIT;
          }
        }
      }
    });
  });

  // Timing Patterns
  for_range(i, 8, image->width - 8) {
    IDX(image->pixels, i + image->width * 6) = (i & 1) | QR__FIXED_BIT;
    IDX(image->pixels, i * image->width + 6) = (i & 1) | QR__FIXED_BIT;
  }
  spall_end();
}

internal void qr__place_format_and_version_info(
  Image              *image,
  isize               version,
  QR_Error_Correction level,
  isize               mask
) {
  spall_begin_fn();
  // Format Information
  IDX(image->pixels, (image->width - 8) * image->width + 8) = 0 | QR__FIXED_BIT; // doesn't really matter whether its fixed

  u64 format_string = qr__format_strings[level * 8 + mask];

  for_range(i, 0, 6) {
    b8 b = (format_string & (1l << (14 - i))) != 0;
    IDX(image->pixels, 8 * image->width + i) = !b | QR__FIXED_BIT;
    IDX(image->pixels, 8 + image->width * (image->width - i - 1)) = !b | QR__FIXED_BIT;
  }

  b8 bit_6 = (format_string & (1l << (14 - 6))) != 0;
  IDX(image->pixels, 8 * image->width + 7) = !bit_6 | QR__FIXED_BIT;
  IDX(image->pixels, 8 + image->width * (image->width - 6 - 1)) = !bit_6 | QR__FIXED_BIT;

  for_range(i, 7, 9) {
    b8 b = (format_string & (1l << (14 - i))) != 0;
    IDX(image->pixels, 8 + image->width * (15 - i)) = !b | QR__FIXED_BIT;
    IDX(image->pixels, 8 * image->width + i - 7 + image->width - 8) = !b | QR__FIXED_BIT;
  }

  for_range(i, 9, 15) {
    b8 b = (format_string & (1l << (14 - i))) != 0;
    IDX(image->pixels, 8 + image->width * (14 - i)) = !b | QR__FIXED_BIT;
    IDX(image->pixels, 8 * image->width + i - 9 + image->width - 6) = !b | QR__FIXED_BIT;
  }

  // Version information
  if (version < 7) {
    spall_end();
    return;
  }

  u64 version_info_string = qr__version_info_strings[version];
  assert(version_info_string);

  for_range(i, 0, 18) {
    b8 bit = (version_info_string >> i) & 1;
    IDX(image->pixels, i / 3 + image->width * (i % 3 + image->height - 11)) = !bit | QR__FIXED_BIT;
    IDX(image->pixels, i % 3 + image->width - 11 + image->width * (i / 3))  = !bit | QR__FIXED_BIT;
  }
  spall_end();
}

internal b8 qr__mask(isize mask, isize row, isize column) {
  switch (mask) {
  CASE 0:
    return !((column + row) & 1);
  CASE 1:
    return !(row & 1);
  CASE 2:
    return column % 3 == 0;
  CASE 3:
    return (column + row) % 3 == 0;
  CASE 4:
    return !((column / 2 + row / 3) & 1);
  CASE 5:
    return ((row * column) % 2) + ((row * column) % 3) == 0;
  CASE 6:
    return (((row * column) % 2) + ((row * column) % 3)) % 2 == 0;
  CASE 7:
    return (((row + column) % 2) + ((row * column) % 3)) % 2 == 0;
  }
  return 0;
}

internal void qr__place_data_bit(
  Image *image,
  isize *cursor_x,
  isize *cursor_y,
  b8    *down,
  b8    *left,
  b8     value,
  isize  mask
) {
  if (*cursor_x == 6) {
    *cursor_x -= 1;
    *left      = false;
  }

  isize row    = *cursor_y;
  isize column = *cursor_x - *left;

  IDX(image->pixels, column + row * image->width) = value ^ qr__mask(mask, row, column);

  do {
    if (!*left) {
      *left = true;
      continue;
    }
    *left = false;
    if (*down) {
      *cursor_y += 1;
      if (*cursor_y == image->height) {
        *cursor_y  = image->height - 1;
        *down      = !*down;
        *cursor_x -= 2;
      }
    } else {
      *cursor_y -= 1;
      if (*cursor_y < 0) {
        *cursor_y  = 0;
        *down      = !*down;
        *cursor_x -= 2;
      }
    }
  } while (IDX(image->pixels, *cursor_x - *left + *cursor_y * image->width) & QR__FIXED_BIT);
}

internal u8 qr__remainder_bits[] = {
  [ 1] = 0,
  [ 2] = 7,
  [ 3] = 7,
  [ 4] = 7,
  [ 5] = 7,
  [ 6] = 7,
  [ 7] = 0,
  [ 8] = 0,
  [ 9] = 0,
  [10] = 0,
  [11] = 0,
  [12] = 0,
  [13] = 0,
  [14] = 3,
  [15] = 3,
  [16] = 3,
  [17] = 3,
  [18] = 3,
  [19] = 3,
  [20] = 3,
  [21] = 4,
  [22] = 4,
  [23] = 4,
  [24] = 4,
  [25] = 4,
  [26] = 4,
  [27] = 4,
  [28] = 3,
  [29] = 3,
  [30] = 3,
  [31] = 3,
  [32] = 3,
  [33] = 3,
  [34] = 3,
  [35] = 0,
  [36] = 0,
  [37] = 0,
  [38] = 0,
  [39] = 0,
  [40] = 0,
};

internal void qr__place_data_bits(
  Image                          *image,
  QR_Error_Correction_Info const *info,
  isize                           version,
  Byte_Slice                      data,
  Byte_Slice                      ecs,
  isize                           mask
) {
  spall_begin_fn();
  isize cursor_x = image->width  - 1;
  isize cursor_y = image->height - 1;
  b8    left     = false;
  b8    down     = false;

  for_range(dw, 0, max(info->data_words_per_block_group1, info->data_words_per_block_group2)) {
    if (dw < info->data_words_per_block_group1) {
      for_range(block, 0, info->blocks_group1) {
        u8 d = IDX(data, dw + info->data_words_per_block_group1 * block);
        for_range(j, 0, 8) {
          b8 b = d & (0x80 >> j);
          qr__place_data_bit(image, &cursor_x, &cursor_y, &down, &left, !b, mask);
        }
      }
    }

    if (dw < info->data_words_per_block_group2) {
      for_range(block, 0, info->blocks_group2) {
        u8 d = IDX(data, dw + info->data_words_per_block_group2 * block + info->blocks_group1 * info->data_words_per_block_group1);
        for_range(j, 0, 8) {
          b8 b = d & (0x80 >> j);
          qr__place_data_bit(image, &cursor_x, &cursor_y, &down, &left, !b, mask);
        }
      }
    }
  }

  for_range(ew, 0, info->error_words_per_block) {
    for_range(block, 0, info->blocks_group1 + info->blocks_group2) {
      u8 d = IDX(ecs, ew + block * info->error_words_per_block);
      for_range(j, 0, 8) {
        b8 b = d & (0x80 >> j);
        qr__place_data_bit(image, &cursor_x, &cursor_y, &down, &left, !b, mask);
      }
    }
  }

  isize remainder_bits = qr__remainder_bits[version];
  for_range(i, 0, remainder_bits) {
    qr__place_data_bit(image, &cursor_x, &cursor_y, &down, &left, 1, mask);
  }

  spall_end();
}

internal isize qr__evaluate_penalty_consecutive(Image const *image) {
  spall_begin_fn();
  isize penalty = 0;

  spall_begin("horziontal");
  for_range(row, 0, image->height) {
    isize run = 0;
    u8    run_value = 1;
    for_range(col, 0, image->width) {
      isize value = IDX(image->pixels, col + row * image->width) & 1;
      if (value == run_value) {
        run += 1;
      } else {
        if (run >= 5) {
          penalty += 3 + run - 5;
        }
        run = 1;
        run_value = value;
      }
    }
  }
  spall_end();

  spall_begin("vertical");
  for_range(col, 0, image->width) {
    isize run = 0;
    u8    run_value = 1;
    for_range(row, 0, image->height) {
      isize value = IDX(image->pixels, col + row * image->width) & 1;
      if (value == run_value) {
        run += 1;
      } else {
        if (run >= 5) {
          penalty += 3 + run - 5;
        }
        run = 1;
        run_value = value;
      }
    }
  }
  spall_end();

  spall_end();
  return penalty;
}

internal isize qr__evaluate_penalty_squares(Image const *image) {
  spall_begin_fn();
  isize penalty = 0;

  for_range(row, 0, image->height - 1) {
    for_range(col, 0, image->width - 1) {
      isize value = IDX(image->pixels, col + row * image->width) & 1;

      if (value != (IDX(image->pixels, col + 1 +  row      * image->width) & 1)) {
        continue;
      }
      if (value != (IDX(image->pixels, col     + (row + 1) * image->width) & 1)) {
        continue;
      }
      if (value != (IDX(image->pixels, col + 1 + (row + 1) * image->width) & 1)) {
        continue;
      }

      penalty += 3;
    }
  }

  spall_end();
  return penalty;
}

internal isize qr__evaluate_penalty_weird(Image const *image) {
  spall_begin_fn();
  isize penalty = 0;

  u16 pattern_1 = 0b01000101111;
  u16 pattern_2 = 0b11110100010;

  for_range(row, 0, image->height) {
    u16 window = 0;
    for_range(col, 0, image->width) {
      window <<= 1;
      isize value = IDX(image->pixels, col + row * image->width);
      window |= value & 1;
      window &= 0b11111111111;
      if ((window == pattern_1 || window == pattern_2) && col >= 11) {
        penalty += 40;
      }
    }
  }

  for_range(col, 0, image->width) {
    u16 window = 0;
    for_range(row, 0, image->height) {
      window <<= 1;
      isize value = IDX(image->pixels, col + row * image->width);
      window |= value & 1;
      window &= 0b11111111111;
      if ((window == pattern_1 || window == pattern_2) && row >= 11) {
        penalty += 40;
      }
    }
  }

  spall_end();
  return penalty;
}

internal isize qr__evaluate_penalty_balance(Image const *image) {
  spall_begin_fn();
  isize n_dark = 0;
  slice_iter_v(image->pixels, p, i, {
    if (!(p & 1)) {
      n_dark += 1;
    }
  });

  isize portion_dark  = (isize)((f64)n_dark / (f64)image->pixels.len);
  isize prev_multiple = (portion_dark / 5) * 5;
  isize next_multiple = (portion_dark / 5) * 5 + 5;

  prev_multiple -= 50;
  next_multiple -= 50;

  if (prev_multiple < 0) { prev_multiple = -prev_multiple; }
  if (next_multiple < 0) { next_multiple = -next_multiple; }

  prev_multiple /= 5;
  next_multiple /= 5;

  spall_end();
  return min(prev_multiple, next_multiple) * 10;
}

internal isize qr__evaluate_penalty(Image const *image) {
  return
    qr__evaluate_penalty_consecutive(image) +
    qr__evaluate_penalty_squares    (image) +
    qr__evaluate_penalty_weird      (image) +
    qr__evaluate_penalty_balance    (image);
    
}

internal void qr__change_mask(Image const *image, isize old_mask, isize new_mask) {
  spall_begin_fn();
  if (old_mask == new_mask) {
  //   spall_end();
    return;
  }

  for_range(row, 0, image->height) {
    for_range(col, 0, image->width) {
      if ((IDX(image->pixels, row * image->width + col) & QR__FIXED_BIT) == 0) {
        IDX(image->pixels, row * image->width + col) ^= qr__mask(old_mask, row, col) ^ qr__mask(new_mask, row, col);
      }
    }
  }
  spall_end();
}

#define QR_MASK_AUTOMATIC -1

internal b8 qr_code_generate_image(
  Byte_Slice          data,
  Image              *image,
  QR_Error_Correction level,
  isize               mask,
  Allocator           allocator
) {
  if (!qr__luts_generated) {
    qr__generate_lut();
  }

  isize version = qr__required_version(data.len, level);
  if (version < 1) {
    return false;
  }
  isize size = (version - 1) * 4 + 21;

  image->components = 1;
  image->width      = size;
  image->height     = size;
  image->pixel_type = PT_u8;
  image->pixels     = slice_make(Byte_Slice, size * size, allocator);

  qr__place_fixed_patterns(image, version, level);

  Bit_Array ba = bit_array_make(0, data.len * 8 + 32, allocator);
  qr__generate_data_bits(version, level, data, &ba);

  QR_Error_Correction_Info const *eci = &QR_ERROR_CORRECTION_INFO(level, version);
  isize i = 0;
  while (ba.len / 8 < eci->data_words) {
    if (i & 1) {
      bit_array_append_u8(&ba, 0b10001000);
    } else {
      bit_array_append_u8(&ba, 0b00110111);
    }
    i += 1;
  }

  Byte_Slice raw_data = bit_array_to_bytes(&ba);

  Byte_Buffer ecs = byte_buffer_make(0, data.len, allocator);
  qr__generate_error_correction_codes(eci, raw_data, &ecs);

  if (mask == QR_MASK_AUTOMATIC) {
    qr__place_format_and_version_info(image, version, level, 0);
    qr__place_data_bits(image, eci, version, raw_data, buffer_to_bytes(ecs), 0);

    isize best_mask  = 0;
    isize best_score = qr__evaluate_penalty(image);

    for_range(m, 0, 7) {
      qr__place_format_and_version_info(image, version, level, m + 1);
      qr__change_mask(image, m, m + 1);
      isize score = qr__evaluate_penalty(image);
      if (score <= best_score) {
        best_mask  = m + 1;
        best_score = score;
      }
    }

    if (best_mask != 7) {
      qr__place_format_and_version_info(image, version, level, best_mask);
      qr__change_mask(image, 7, best_mask);
    }
  } else {
    assert(mask >= 0);
    assert(mask <  8);
    qr__place_format_and_version_info(image, version, level, mask);
    qr__place_data_bits(image, eci, version, raw_data, buffer_to_bytes(ecs), mask);
  }

  spall_begin("transform_pixels");
  slice_iter(image->pixels, p, i, {
    *p = -(*p & 1);
  });
  spall_end();
  
  bit_array_destroy(&ba);
  return true;
}
