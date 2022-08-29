#include "../src/libbase122.h"

#include "assertions.h"
#include <stdio.h>

static void test_hexstring_to_bytes(void) {
  /* One byte. */
  {
    size_t got_len;
    byte *got = hexstring_to_bytes("AA", &got_len);
    byte expect[] = {0xAA};
    ASSERT_BYTES_EQUAL(got, got_len, expect, sizeof(expect), hexstring);
    free(got);
  }
  /* Two bytes. */
  {
    size_t got_len;
    byte *got = hexstring_to_bytes("AA BB", &got_len);
    byte expect[] = {0xAA, 0xBB};
    ASSERT_BYTES_EQUAL(got, got_len, expect, sizeof(expect), hexstring);
    free(got);
  }
}

static void test_bitstring_to_bytes(void) {
  /* One byte. */
  {
    size_t got_len;
    byte *got = bitstring_to_bytes("1000 0000", &got_len);
    byte expect[] = {0x80};
    ASSERT_BYTES_EQUAL(got, got_len, expect, sizeof(expect), bitstring);
    free(got);
  }
  /* Two bytes. */
  {
    size_t got_len;
    byte *got = bitstring_to_bytes("1000 0000 0000 1000", &got_len);
    byte expect[] = {0x80, 0x08};
    ASSERT_BYTES_EQUAL(got, got_len, expect, sizeof(expect), bitstring);
    free(got);
  }
}

int main() {
  test_hexstring_to_bytes();
  test_bitstring_to_bytes();

  /* Test encoding one byte */
  {
    size_t in_len;
    byte *in = bitstring_to_bytes("11111111", &in_len);
    byte got[2] = {0};
    size_t got_len;
    base122_error_t error;
    size_t expect_len;
    byte *expect = bitstring_to_bytes("01111111 01000000", &expect_len);
    int ret = base122_encode(in, in_len, got, sizeof(got), &got_len, &error);
    ASSERT(ret != -1, "base122_encode error: %s", error.msg);
    ASSERT_BYTES_EQUAL(in, in_len, got, got_len, bitstring);
    free(expect);
    free(in);
  }

  /* Test decoding two bytes */
  {
    size_t in_len;
    byte *in = bitstring_to_bytes("01111111 01000000", &in_len);
    byte got[1] = {0};
    size_t got_len;
    base122_error_t error;
    size_t expect_len;
    byte *expect = bitstring_to_bytes("11111111", &expect_len);
    int ret = base122_decode(in, in_len, got, sizeof(got), &got_len, &error);
    ASSERT(ret != -1, "base122_encode error: %s", error.msg);
    ASSERT_BYTES_EQUAL(in, in_len, got, got_len, bitstring);
    free(expect);
    free(in);
  }
}