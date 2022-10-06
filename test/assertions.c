#include "assertions.h"

#include <string.h> // memcmp

bool bytes_equal(const byte *d1, size_t d1_len, const byte *d2, size_t d2_len) {
  if (d1_len != d2_len) {
    return false;
  }
  if ((d1 == NULL || d2 == NULL) && d1 != d2) {
    return false;
  }
  return 0 == memcmp(d1, d2, d1_len);
}

byte *hexstring_to_bytes(const char *hexstring, size_t *bytes_len) {
  byte *out = malloc((sizeof(byte)) * (strlen(hexstring) / 2 + 1));
  byte *outptr = out;
  int numgot = 0;
  byte curr = 0;
  for (const char *ptr = hexstring; *ptr != '\0'; ptr++) {
    byte val;
    if (*ptr >= '0' && *ptr <= '9') {
      val = (byte)(*ptr - '0');
    } else if (*ptr >= 'A' && *ptr <= 'F') {
      val = (byte)(*ptr - 'A' + 10);
    } else if (*ptr >= 'a' && *ptr <= 'f') {
      val = (byte)(*ptr - 'a' + 10);
    } else if (isspace(*ptr)) {
      continue;
    } else {
      TEST_ERROR("expected hex character, got: %c in %s", *ptr, hexstring);
    }

    curr = (byte)(curr * 16);
    curr = (byte)(curr + val);
    numgot += 1;
    if (numgot == 2) {
      *outptr = curr;
      outptr += 1;
      numgot = 0;
    }
  }
  if (numgot == 1) {
    TEST_ERROR("expected even number of hex characters, got extra");
  }
  if (bytes_len) {
    *bytes_len = (size_t)(outptr - out);
  }
  return out;
}

char *bytes_to_hexstring(const byte *bytes, size_t bytes_len) {
  char *hexstring = malloc(sizeof(char *) * (2 * bytes_len + 1));
  for (size_t i = 0; i < bytes_len; i++) {
    snprintf(hexstring + (2 * i), 3, "%02X", bytes[i]);
  }
  return hexstring;
}

byte *bitstring_to_bytes(const char *bitstring, size_t *bytes_len) {
  byte *out = malloc((sizeof(byte)) * (strlen(bitstring) / 8 + 1));
  byte *outptr = out;
  int numgot = 0;
  byte curr = 0;
  for (const char *ptr = bitstring; *ptr != '\0'; ptr++) {
    byte val;
    if (*ptr >= '0' && *ptr <= '1') {
      val = (byte)(*ptr - '0');
    } else if (isspace(*ptr)) {
      continue;
    } else {
      TEST_ERROR("expected 0 or 1 character, got: %c in %s", *ptr, bitstring);
    }

    curr = (byte)(curr * 2);
    curr = (byte)(curr + val);
    numgot += 1;
    if (numgot == 8) {
      *outptr = curr;
      outptr += 1;
      numgot = 0;
    }
  }
  if (bytes_len) {
    *bytes_len = (size_t)(outptr - out);
  }
  return out;
}
char *bytes_to_bitstring(const byte *bytes, size_t bytes_len) {
  char *bitstring = malloc((sizeof(byte)) * (9 * bytes_len + 1));
  char *ptr = bitstring;
  for (size_t i = 0; i < bytes_len; i++) {
    for (byte j = 0; j < 8; j++) {
      byte val = (byte)((1 << (7 - j)) & bytes[i]);
      if (val) {
        *ptr = '1';
      } else {
        *ptr = '0';
      }
      ptr += 1;
    }
    *(ptr++) = ' ';
  }
  if (ptr > bitstring) {
    ptr--;
  }
  *(ptr++) = '\0';
  return bitstring;
}
