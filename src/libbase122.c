#include "libbase122.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

/* strncpy_safe is like strncpy, except dest is always NULL terminated. */
static void strncpy_safe(char *dest, const char *src, size_t n) {
  strncpy(dest, src, n);
  dest[n - 1] = '\0';
}

static int is_illegal(unsigned char val) { return 0; }

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
      strncpy_safe(error->msg, "TODO: not implemented yet", sizeof(error->msg));
      return -1;
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
