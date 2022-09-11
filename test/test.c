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

#include "../src/util.h"

static void test_bitreader_read(void) {
  /* One byte. */
  {
    size_t in_len;
    byte *in = bitstring_to_bytes("11111111", &in_len);
    bitreader_t reader = {.in = in, .len = in_len};
    byte got;
    size_t ret = bitreader_read(&reader, 7, &got);
    ASSERT(ret == 7, "expected ret to be 7, got: %zu", ret);
    size_t expect_len;
    byte *expect = bitstring_to_bytes("01111111", &expect_len);
    ASSERT_BYTES_EQUAL(&got, 1, expect, expect_len, bitstring);
    free(expect);

    ret = bitreader_read(&reader, 7, &got);
    ASSERT(ret == 1, "expected ret to be 1, got: %zu", ret);
    expect = bitstring_to_bytes("00000001", &expect_len);
    ASSERT_BYTES_EQUAL(&got, 1, expect, expect_len, bitstring);
    free(expect);

    ret = bitreader_read(&reader, 7, &got);
    ASSERT(ret == 0, "expected ret to be 0, got: %zu", ret);
    expect = bitstring_to_bytes("00000000", &expect_len);
    ASSERT_BYTES_EQUAL(&got, 1, expect, expect_len, bitstring);
    free(expect);

    free(in);
  }

  /* Two byte. */
  {
    size_t in_len;
    byte *in = bitstring_to_bytes("10101010 11111111", &in_len);
    bitreader_t reader = {.in = in, .len = in_len};
    byte got;
    size_t ret = bitreader_read(&reader, 7, &got);
    ASSERT(ret == 7, "expected ret to be 7, got: %zu", ret);
    size_t expect_len;
    byte *expect = bitstring_to_bytes("01010101", &expect_len);
    ASSERT_BYTES_EQUAL(&got, 1, expect, expect_len, bitstring);
    free(expect);

    ret = bitreader_read(&reader, 7, &got);
    ASSERT(ret == 7, "expected ret to be 7, got: %zu", ret);
    expect = bitstring_to_bytes("00111111", &expect_len);
    ASSERT_BYTES_EQUAL(&got, 1, expect, expect_len, bitstring);
    free(expect);

    ret = bitreader_read(&reader, 7, &got);
    ASSERT(ret == 2, "expected ret to be 2, got: %zu", ret);
    expect = bitstring_to_bytes("00000011", &expect_len);
    ASSERT_BYTES_EQUAL(&got, 1, expect, expect_len, bitstring);
    free(expect);

    free(in);
  }
}

int main() {
  test_hexstring_to_bytes();
  test_bitstring_to_bytes();
  test_bitreader_read();

  typedef struct {
    const char *description;
    const char *data;
    const char *encoded;
  } roundtrip_test_t;

  roundtrip_test_t tests[] = {
      {.description = "one byte", .data = "11111111", .encoded = "01111111 01000000"},
      {.description = "several bytes",
       .data = "10101010 10101010 10101010 10101010",
       .encoded = "01010101 00101010 01010101 00101010 01010000"}};

  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    roundtrip_test_t *test = tests + i;

    printf("test '%s'\n", test->description);
    /* Test encoding 'data'. */
    {
      size_t in_len;
      byte *in = bitstring_to_bytes(test->data, &in_len);
      byte *got = malloc(sizeof(byte) * in_len);
      size_t got_len;
      base122_error_t error;
      size_t expect_len;
      byte *expect = bitstring_to_bytes(test->encoded, &expect_len);
      int ret = base122_encode(in, in_len, got, sizeof(got), &got_len, &error);
      ASSERT(ret != -1, "base122_encode error: %s", error.msg);
      ASSERT_BYTES_EQUAL(expect, expect_len, got, got_len, bitstring);
      free(expect);
      free(in);
    }

    /* Test decoding 'encoded'. */
    {
      size_t in_len;
      byte *in = bitstring_to_bytes(test->encoded, &in_len);
      byte got[1] = {0};
      size_t got_len;
      base122_error_t error;
      size_t expect_len;
      byte *expect = bitstring_to_bytes(test->data, &expect_len);
      int ret = base122_decode(in, in_len, got, sizeof(got), &got_len, &error);
      ASSERT(ret != -1, "base122_decode error: %s", error.msg);
      ASSERT_BYTES_EQUAL(expect, expect_len, got, got_len, bitstring);
      free(expect);
      free(in);
    }
  }
}