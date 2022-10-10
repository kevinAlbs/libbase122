#include "../src/base122.h"
#include "assertions.h"

#include <stdint.h>

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  base122_error_t error;
  unsigned char *encoded;
  size_t encoded_len;

  // Get encoded_len.
  {
    int got = base122_encode(Data, Size, NULL /* out */, 0 /* out_len */, &encoded_len, &error);
    ASSERT(got != -1, "unexpected error in base122_encode: %s", error.msg);
  }

  encoded = malloc(encoded_len);

  {
    size_t written;
    int got = base122_encode(Data, Size, encoded, encoded_len, &written, &error);
    ASSERT(got != -1, "unexpected error in base122_encode: %s", error.msg);
    ASSERT(written == encoded_len, "expected %zu == %zu", written, encoded_len);
  }

  unsigned char *decoded;
  size_t decoded_len;
  {
    int got =
        base122_decode(encoded, encoded_len, NULL /* out */, 0 /* out_len */, &decoded_len, &error);
    ASSERT(got != -1, "unexpected error in base122_decode: %s", error.msg);
  }

  decoded = malloc(decoded_len);

  {
    size_t written;
    int got = base122_decode(encoded, encoded_len, decoded, decoded_len, &written, &error);
    ASSERT(got != -1, "unexpected error in base122_decode: %s", error.msg);
    ASSERT(written == decoded_len, "expected %zu == %zu", written, decoded_len);
  }

  ASSERT_BYTES_EQUAL(Data, Size, decoded, decoded_len, hexstring);

  free(decoded);
  free(encoded);
  return 0;
}
