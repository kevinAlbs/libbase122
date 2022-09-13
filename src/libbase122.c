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
  unsigned char got;
  size_t out_index = 0;

  reader.in = in;
  reader.len = in_len;
  *out_written = 0;

  while ((nbits = bitreader_read(&reader, 7, &got)) > 0) {
    if (nbits < 7) {
      /* Align the first bit to start at position 6.
       * E.g. if nbits = 3: 0abc0000 */
      got <<= 7 - nbits;
    }

    if (is_illegal(got)) {
      unsigned char illegal_index = get_illegal_index(got);
      unsigned char next_bits;
      unsigned char b1 = 0xC2; /* 11000010 */
      unsigned char b2 = 0x80; /* 10000000 */
      unsigned char first_bit;

      /* This will be a two byte character. Try to get the next 7 bits. */
      size_t next_nbits = bitreader_read(&reader, 7, &next_bits);
      if (next_nbits == 0) {
        strncpy_safe(error->msg, "TODO: not implemented yet", sizeof(error->msg));
        return -1;
      } else {
        b1 |= (illegal_index << 2);
      }

      /* Push the first bit onto the first byte */
      first_bit = (next_bits >> 6) & 1;
      b1 |= first_bit;
      b2 |= next_bits & 0x3F /* 00111111 */;

      if (out_index == out_len) {
        strncpy_safe(error->msg, "output does not have sufficient size", sizeof(error->msg));
        return -1;
      }
      out[out_index] = b1;
      (*out_written)++;
      out_index++;

      if (out_index == out_len) {
        strncpy_safe(error->msg, "output does not have sufficient size", sizeof(error->msg));
        return -1;
      }
      out[out_index] = b2;
      (*out_written)++;
      out_index++;

    } else {
      if (out_index == out_len) {
        strncpy_safe(error->msg, "output does not have sufficient size", sizeof(error->msg));
        return -1;
      }
      out[out_index] = got;
      (*out_written)++;
      out_index++;
    }
  }

  return 0;
}

int base122_decode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_len,
                   size_t *out_written, base122_error_t *error) {
  return -1;
}
