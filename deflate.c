#include "deflate.h"

#include "hash.h"
#include "io.h"

extern bool my_inflate(Reader const *source, Writer const *dest) {
  struct {
    u8 CMF;
    u8 FLG;
  } header;

  isize n = or_return(read_any(source, &header), false);

  if (n < 2) {
    return false;
  }

  u8 compression_method = header.CMF & ((1 << 4) - 1);
  u8 compression_info   = header.CMF >> 4;

  // log_infof(LIT("Method: %04b"), (isize)compression_method);
  // log_infof(LIT("Info:   %04b"), (isize)compression_info  );

  isize check  = (isize)(header.CMF << 8) + (isize)header.FLG;
  bool  fdict  = !!(header.FLG & (1 << 5));
  u8    flevel = header.FLG >> 6;

  // log_infof(LIT("Check:  %d, %d"), check, check % 31);
  // log_infof(LIT("Level:  %d"), flevel);
  // log_infof(LIT("Dict:   %d"), fdict);

  Byte_Slice in_buffer;
  slice_init(&in_buffer,  1 << compression_info, context.temp_allocator);
  Byte_Slice out_buffer;
  slice_init(&out_buffer, 2 << compression_info, context.temp_allocator);

  isize current_byte = 0;
  isize current_bit  = 0;

#define read_bit() ({                                                          \
  bool ret = !!(in_buffer.data[current_bit >> 3] & (1 << (current_bit & 0x7)));  \
  current_bit += 1;                                                            \
  ret;                                                                         \
})

  in_buffer.len = or_return(read_bytes(source, in_buffer), false);

  loop {
    bool final = read_bit();
    u8   type  = read_bit() + (read_bit() << 1);
    if (final) {
      return true;
    }

    // log_infof(LIT("BType:  %02b"), (isize)type);

    isize len  = 0;
    isize nlen = 0;

    switch (type) {
    case 0b00:
      current_byte = ((current_bit - 1) & ~0x7) + 1;
      // log_infof(LIT("current_bit:  %d"), (isize)current_bit);
      // log_infof(LIT("current_byte: %d"), (isize)current_byte);
      len  = in_buffer.data[current_byte + 0] + (in_buffer.data[current_byte + 1] << 8);
      nlen = in_buffer.data[current_byte + 2] + (in_buffer.data[current_byte + 3] << 8);

      if (len != (0xFFFF ^ nlen)) {
        return false;
      }

      loop {
        or_return(write_bytes(dest, slice_start(in_buffer, in_buffer.len - current_byte)), false);
        if (in_buffer.len - current_byte < len) {
          in_buffer.len = or_return(read_bytes(source, in_buffer), false);
        }

        return false;
      }

      return false;
    case 0b01:
      // log_error(LIT("Unimplemented"));
      return false;
    case 0b10:
      // log_error(LIT("Unimplemented"));
      return false;
    case 0b11:
      return false;
    }

    return true;
  }

  return false;
#undef read_bit
}

extern bool my_deflate(Reader const *source, Writer const *dest) {
  struct {
    u8 CMF;
    u8 FLG;
  } header;

  header.CMF = 8 + (7 << 4);
  header.FLG = 31 - (header.CMF << 8) % 31;

  // u8 compression_method = header.CMF & ((1 << 4) - 1);
  // u8 compression_info   = header.CMF >> 4;

  isize check = (isize)(header.CMF << 8) + (isize)header.FLG;
  assert(!(check % 31));

  or_return(write_any(dest, &header), false);

  Byte_Slice in_buffer;
  slice_init(&in_buffer, (1 << 16) - 1, context.temp_allocator);

  u32 hash = 1;

  loop {
    isize n_read = or_break(read_bytes(source, in_buffer));

    or_return(write_byte(dest, n_read != in_buffer.len), false);

    or_return(write_byte(dest,  n_read       & 0xFF), false);
    or_return(write_byte(dest, (n_read >> 8) & 0xFF), false);

    isize complement = 0xFFFF ^ n_read;

    or_return(write_byte(dest,  complement       & 0xFF), false);
    or_return(write_byte(dest, (complement >> 8) & 0xFF), false);

    or_return(write_bytes(dest, slice_end(in_buffer, n_read)), false);
    hash = hash_bytes(hash, hash_adler_32, slice_end(in_buffer, n_read));

    if (n_read != in_buffer.len) {
      hash = endianness_swap(hash);
      or_return(write_any(dest, &hash), false);
      return true;
    }
  }

  return false;
}
