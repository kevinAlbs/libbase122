#include "base122.h"
#include <stdio.h>

int main(void) {
  unsigned char *data = (unsigned char *)"\x34\x19\x2d\x46\x63\x3c\x14";
  unsigned char decoded[16] = {0};
  base122_error_t error;
  int got;
  size_t written;

  got = base122_decode(data, 8, decoded, sizeof(decoded), &written, &error);

  if (got == -1) {
    fprintf(stderr, "Error decoding: %s\n", error.msg);
    return 1;
  }
  printf("Decoded the string: %s\n", decoded);
  return 0;
}
