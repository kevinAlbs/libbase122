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

static void test_bitwriter_write(void) {
  /* One byte. */
  {
    size_t in_len;
    byte got[1] = {0};
    byte *in = bitstring_to_bytes("00001111", &in_len);
    bitwriter_t writer = {.out = got, .len = sizeof(got)};

    int ret = bitwriter_write(&writer, 1, *in);
    ASSERT(ret != -1, "expected no error");
    size_t expect_len;
    byte *expect = bitstring_to_bytes("10000000", &expect_len);
    ASSERT_BYTES_EQUAL(got, sizeof(got), expect, expect_len, bitstring);
    free(expect);

    ret = bitwriter_write(&writer, 1, *in);
    ASSERT(ret != -1, "expected no error");
    expect = bitstring_to_bytes("11000000", &expect_len);
    ASSERT_BYTES_EQUAL(got, sizeof(got), expect, expect_len, bitstring);
    free(expect);

    ret = bitwriter_write(&writer, 5, *in);
    ASSERT(ret != -1, "expected no error");
    expect = bitstring_to_bytes("11011110", &expect_len);
    ASSERT_BYTES_EQUAL(got, sizeof(got), expect, expect_len, bitstring);
    free(expect);

    ret = bitwriter_write(&writer, 5, *in);
    ASSERT(ret == -1, "expected exceeded capacity error, but got no error");

    free(in);
  }

  /* Two byte. */
  {
    byte got[2] = {0};
    bitwriter_t writer = {.out = got, .len = sizeof(got)};

    int ret = bitwriter_write(&writer, 1, 0xFF);
    ASSERT(ret != -1, "expected no error");
    size_t expect_len;
    byte *expect = bitstring_to_bytes("10000000 00000000", &expect_len);
    ASSERT_BYTES_EQUAL(got, sizeof(got), expect, expect_len, bitstring);
    free(expect);

    ret = bitwriter_write(&writer, 8, 0x0F);
    ASSERT(ret != -1, "expected no error");
    expect = bitstring_to_bytes("10000111 10000000", &expect_len);
    ASSERT_BYTES_EQUAL(got, sizeof(got), expect, expect_len, bitstring);
    free(expect);

    ret = bitwriter_write(&writer, 1, 0x0);
    ASSERT(ret != -1, "expected no error");
    expect = bitstring_to_bytes("10000111 10000000", &expect_len);
    ASSERT_BYTES_EQUAL(got, sizeof(got), expect, expect_len, bitstring);
    free(expect);

    ret = bitwriter_write(&writer, 1, 0xFF);
    ASSERT(ret != -1, "expected no error");
    expect = bitstring_to_bytes("10000111 10100000", &expect_len);
    ASSERT_BYTES_EQUAL(got, sizeof(got), expect, expect_len, bitstring);
    free(expect);

    ret = bitwriter_write(&writer, 5, 0xFF);
    ASSERT(ret != -1, "expected no error");
    expect = bitstring_to_bytes("10000111 10111111", &expect_len);
    ASSERT_BYTES_EQUAL(got, sizeof(got), expect, expect_len, bitstring);
    free(expect);

    ret = bitwriter_write(&writer, 1, 0xFF);
    ASSERT(ret == -1, "expected exceeded capacity error, but got no error");
  }
}

static void test_decode(void) {
  typedef struct {
    const char *encoded;
    const char *expectError;
    size_t decodedLen;
    const char *expect;
  } decode_test_t;

  decode_test_t tests[] = {
      {.encoded = "01111111 01111111 01111111",
       .expectError = "Last byte has extra data",
       .decodedLen = 3},
      {.encoded = "01111111 01000000 01111111 01000000",
       .expectError = "Output does not have sufficient size",
       .decodedLen = 1},
      {.encoded = "11011110 11111111",
       .expectError = "Second byte of two byte sequence malformed",
       .decodedLen = 2},
      {.encoded = "11111111",
       .expectError = "First byte of two byte sequence malformed",
       .decodedLen = 1},
      {.encoded = "11011110",
       .expectError = "Two byte sequence is missing second byte",
       .decodedLen = 1},
      {.encoded = "11011110 10111111 01111111",
       .expectError = "Got unexpected extra data after shortened two byte sequence",
       .decodedLen = 3},
      {.encoded = "11011010 10111111",
       .expectError = "Got unrecognized illegal index",
       .decodedLen = 2},
      {.encoded = "01111111",
       .expectError = "Decoded data is not a byte multiple",
       .decodedLen = 1},
      {.encoded = "01111111 01111111",
       .expectError = "Encoded data is malformed. Last byte has extra data.",
       .decodedLen = 1},
      {.encoded = "01111111 11011110 10000000", .expect = "11111110", .decodedLen = 1},
      {.encoded = "01111111 11011111 10100000",
       .expectError = "Encoded data is malformed. Last byte has extra data.",
       .decodedLen = 1},
      {.encoded = "11011110 10000000",
       .expectError = "Decoded data is not a byte multiple",
       .decodedLen = 1},
      {.encoded = "00000000 11011110 10000000", .expect = "00000000", .decodedLen = 1},
      {.encoded = "00000000 11000010 10000000", .expect = "00000000 00000000", .decodedLen = 2}};

  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    decode_test_t *test = tests + i;

    printf("decode test %zu: '%s'\n", i, test->expectError ? test->expectError : test->encoded);

    size_t encoded_len;
    byte *encoded = bitstring_to_bytes(test->encoded, &encoded_len);
    size_t got_len = test->decodedLen ? test->decodedLen : 1;
    byte *got = malloc(sizeof(byte) * got_len);
    size_t written;
    base122_error_t error;

    int ret = base122_decode(encoded, encoded_len, got, got_len, &written, &error);
    if (test->expectError) {
      ASSERT(ret == -1, "expected base122_decode to error, but succeeded");
      ASSERT_STRCONTAINS(error.msg, test->expectError);
    } else {
      ASSERT(ret == 0, "expected base122_decode to succeed, but got: %s", error.msg);
      size_t expect_len;
      byte *expect = bitstring_to_bytes(test->expect, &expect_len);
      ASSERT_BYTES_EQUAL(expect, expect_len, got, got_len, bitstring);
      free(expect);
    }

    free(got);
    free(encoded);
  }
}

int main() {
  test_hexstring_to_bytes();
  test_bitstring_to_bytes();
  test_bitreader_read();
  test_bitwriter_write();
  test_decode();

  typedef struct {
    const char *description;
    const char *data;
    const char *encoded;
  } roundtrip_test_t;

  roundtrip_test_t tests[] = {
      {.description = "one byte", .data = "11111111", .encoded = "01111111 01000000"},
      {.description = "several bytes",
       .data = "10101010 10101010 10101010 10101010",
       .encoded = "01010101 00101010 01010101 00101010 01010000"},
      {.description = "illegal one byte",
       .data = "00000000 11111111",
       .encoded = "11000010 10111111 01100000"},
      {.description = "illegal last two bits",
       .data = "1111111 1111111 00",
       .encoded = "01111111 01111111 11011110 10000000"}};

  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    roundtrip_test_t *test = tests + i;
    size_t data_len;
    byte *data = bitstring_to_bytes(test->data, &data_len);
    size_t encoded_len;
    byte *encoded = bitstring_to_bytes(test->encoded, &encoded_len);

    printf("round trip test '%s'\n", test->description);
    /* Test encoding 'data'. */
    {
      size_t got_len = encoded_len;
      byte *got = malloc(sizeof(byte) * got_len);
      size_t written;
      base122_error_t error;
      int ret = base122_encode(data, data_len, got, got_len, &written, &error);
      ASSERT(ret != -1, "base122_encode error: %s", error.msg);
      ASSERT_BYTES_EQUAL(encoded, encoded_len, got, got_len, bitstring);
      free(got);
    }

    /* Test decoding 'encoded'. */
    {
      base122_error_t error;
      size_t got_len = data_len;
      byte *got = malloc(sizeof(byte) * got_len);
      int ret = base122_decode(encoded, encoded_len, got, sizeof(got), &got_len, &error);
      ASSERT(ret != -1, "base122_decode error: %s", error.msg);
      ASSERT_BYTES_EQUAL(data, data_len, got, got_len, bitstring);
      free(got);
    }

    free(data);
    free(encoded);
  }
}