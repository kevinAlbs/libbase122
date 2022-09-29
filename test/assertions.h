#ifndef LIBBASE122_TEST_ASSERTIONS_H
#define LIBBASE122_TEST_ASSERTIONS_H

#include <ctype.h> // isspace
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // abort
#include <string.h> // strstr

typedef unsigned char byte;

#define TEST_ERROR(...)                                                                            \
  do {                                                                                             \
    fprintf(stderr, "error on %s:%d : ", __FILE__, __LINE__);                                      \
    fprintf(stderr, __VA_ARGS__);                                                                  \
    fprintf(stderr, "\n");                                                                         \
    fflush(stderr);                                                                                \
    abort();                                                                                       \
  } while (0)

#define ASSERT(stmt, ...)                                                                          \
  do {                                                                                             \
    if (!(stmt)) {                                                                                 \
      fprintf(stderr, "assertion failed on %s:%d : %s : ", __FILE__, __LINE__, #stmt);             \
      fprintf(stderr, __VA_ARGS__);                                                                \
      fprintf(stderr, "\n");                                                                       \
      fflush(stderr);                                                                              \
      abort();                                                                                     \
    }                                                                                              \
  } while (0)

#define ASSERT_STREQUAL(s1, s2)                                                                    \
  do {                                                                                             \
    const char *_s1 = (s1);                                                                        \
    const char *_s2 = (s2);                                                                        \
    if (((_s1 == NULL || _s2 == NULL) && _s1 != _s2) || 0 != strcmp(_s1, _s2)) {                   \
      fprintf(stderr, "assertion failed on %s:%d : '%s' != '%s' : ", __FILE__, __LINE__, _s1,      \
              _s2);                                                                                \
      fprintf(stderr, "\n");                                                                       \
      fflush(stderr);                                                                              \
      abort();                                                                                     \
    }                                                                                              \
  } while (0)

#define ASSERT_STRCONTAINS(s1, s2)                                                                 \
  do {                                                                                             \
    const char *_s1 = (s1);                                                                        \
    const char *_s2 = (s2);                                                                        \
    if (((_s1 == NULL || _s2 == NULL) && _s1 != _s2) || NULL == strstr(_s1, _s2)) {                \
      fprintf(stderr, "assertion failed on %s:%d : '%s' does not contain '%s' : ", __FILE__,       \
              __LINE__, _s1, _s2);                                                                 \
      fprintf(stderr, "\n");                                                                       \
      fflush(stderr);                                                                              \
      abort();                                                                                     \
    }                                                                                              \
  } while (0)

#define ASSERT_BYTES_EQUAL(b1, b1_len, b2, b2_len, fmt)                                            \
  do {                                                                                             \
    const byte *_b1 = (b1);                                                                        \
    const byte *_b2 = (b2);                                                                        \
    size_t _b1_len = (b1_len);                                                                     \
    size_t _b2_len = (b2_len);                                                                     \
    if (!bytes_equal(_b1, _b1_len, _b2, _b2_len)) {                                                \
      char *_b1_str = bytes_to_##fmt(_b1, _b1_len);                                                \
      char *_b2_str = bytes_to_##fmt(_b2, _b2_len);                                                \
      fprintf(stderr, "assertion failed on %s:%d : '%s' != '%s' : ", __FILE__, __LINE__, _b1_str,  \
              _b2_str);                                                                            \
      fprintf(stderr, "\n");                                                                       \
      fflush(stderr);                                                                              \
      free(_b2_str);                                                                               \
      free(_b1_str);                                                                               \
      abort();                                                                                     \
    }                                                                                              \
  } while (0)

bool bytes_equal(const byte *d1, size_t d1_len, const byte *d2, size_t d2_len);

byte *hexstring_to_bytes(const char *hexstring, size_t *bytes_len);

char *bytes_to_hexstring(const byte *bytes, size_t bytes_len);

byte *bitstring_to_bytes(const char *bitstring, size_t *bytes_len);

char *bytes_to_bitstring(const byte *bytes, size_t bytes_len);

#endif /* LIBBASE122_TEST_ASSERTIONS_H */
