#include "libbase122.h"
#include "util.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* strncpy_safe is like strncpy, except dest is always NULL terminated. */
static void strncpy_safe(char *dest, const char *src, size_t n) {
  strncpy(dest, src, n);
  dest[n - 1] = '\0';
}

static char illegals[] = {
    0 /* null */,          10 /* newline */,   13 /* carriage return */,
    34 /* double quote */, 38 /* ampersand */, 92 /* backslash */
};

static int is_illegal(unsigned char val) {
  size_t i;
  for (i = 0; i < sizeof(illegals) / sizeof(illegals[0]); i++) {
    if (illegals[i] == val) {
      return 1;
    }
  }
  return 0;
}

static unsigned char get_illegal_index(unsigned char val) {
  unsigned char i;
  for (i = 0; i < sizeof(illegals) / sizeof(illegals[0]); i++) {
    if (illegals[i] == val) {
      return i;
    }
  }
  assert(0 && "unreachable");
  return 255;
}

int base122_encode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_len,
                   size_t *out_written, base122_error_t *error) {
  bitreader_t reader = {0};
  size_t nbits;
  unsigned char bits;
  size_t out_index = 0;

  reader.in = in;
  reader.len = in_len;
  *out_written = 0;

#define OUTPUT_BYTE(b)                                                                             \
  if (out_index == out_len) {                                                                      \
    strncpy_safe(error->msg, "output does not have sufficient size", sizeof(error->msg));          \
    return -1;                                                                                     \
  }                                                                                                \
  out[out_index] = b;                                                                              \
  (*out_written)++;                                                                                \
  out_index++;

  while ((nbits = bitreader_read(&reader, 7, &bits)) > 0) {
    if (nbits < 7) {
      /* Align the first bit to start at position 6.
       * E.g. if nbits = 3: 0abc0000 */
      bits <<= 7 - nbits;
    }

    if (is_illegal(bits)) {
      unsigned char illegal_index = get_illegal_index(bits);
      unsigned char next_bits;
      unsigned char b1 = 0xC2; /* 11000010 */
      unsigned char b2 = 0x80; /* 10000000 */
      unsigned char first_bit;

      /* This will be a two byte character. Try to get the next 7 bits. */
      size_t next_nbits = bitreader_read(&reader, 7, &next_bits);
      if (next_nbits == 0) {
        b1 |= 0x7 << 2; /* 11100 */
        next_bits = bits;
      } else {
        b1 |= (illegal_index << 2);
      }

      /* Push the first bit onto the first byte */
      first_bit = (next_bits >> 6) & 1;
      b1 |= first_bit;
      b2 |= next_bits & 0x3F /* 00111111 */;

      OUTPUT_BYTE(b1)
      OUTPUT_BYTE(b2)

    } else {
      OUTPUT_BYTE(bits);
    }
  }

#undef OUTPUT_BYTE

  return 0;
}

int base122_decode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_len,
                   size_t *out_written, base122_error_t *error) {
  bitwriter_t writer = {0};
  size_t curByte;

  assert(in);
  assert(in_len > 0);
  assert(out);
  assert(out_len > 0);
  assert(out_written);

  writer.out = out;
  writer.len = out_len;
  writer.curBit = 0;

  for (curByte = 0; curByte < in_len; curByte++) {
    if (in[curByte] >> 7 == 0) {
      /* One byte sequence. */
      size_t nbits = 7;
      unsigned char curByteVal = in[curByte];

      if (curByte == in_len - 1) {
        /* Last input byte. */
        /* Do not write extra bytes. Write up to the nearest bit boundary. */
        nbits = 8 - (writer.curBit % 8);
        if ((nbits == 8) % 8 != 0) {
          strncpy_safe(error->msg, "Decoded data is not a byte multiple", sizeof(error->msg));
          return -1;
        }
        /* Shift bits to write. */
        curByteVal >>= 7 - nbits;
      }
      if (bitwriter_write(&writer, nbits, curByteVal) == -1) {
  return -1;
      }
    }
  }

  *out_written = writer.curBit / 8;
  return 0;
}
