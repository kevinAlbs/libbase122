/* b122 is an application for encoding and decoding base122 */
#include "base122.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* pstrerror is a portable strerror */
static const char *pstrerror(int err) {
#ifdef _WIN32
  static char buf[64];
  errno_t ret = strerror_s(buf, sizeof(buf), err);
  ASSERT(ret == 0, "strerror_s got error: %d", (int)ret);
  return buf;
#else
  return strerror(err);
#endif
}

int main(int argc, char **argv) {
  size_t in_len = 0, out_len = 0;
  size_t in_capacity = 0;
  unsigned char *in_data = 0, *out_data = 0;

  int decode = 0;
  if (argc > 1) {
    if (strcmp(argv[1], "-d") == 0) {
      decode = 1;
    } else {
      printf(
          "Usage: b122 [-d]\n  -d Decode input\n  b122 reads from stdin and outputs to stdout\n");
      return 1;
    }
  }

  /* Read all input */
  while (1) {
    size_t read;
    int err;

    if (in_len == in_capacity) {
      if (in_capacity == 0) {
        in_capacity = 512;
      } else {
        in_capacity *= 2;
      }
      in_data = realloc(in_data, in_capacity);
    }
    read = fread(in_data + in_len, sizeof(unsigned char), in_capacity - in_len, stdin);
    in_len += read;

    if (feof(stdin)) {
      break;
    }

    if ((err = ferror(stdin))) {
      fprintf(stderr, "error reading from stdin: %d %s\n", err, pstrerror(err));
      return 1;
    }
  }

  /* Compute output length. */
  {
    base122_error_t b122err;
    int got;

    if (decode) {
      got = base122_decode(in_data, in_len, 0 /* out */, 0 /* out_len */, &out_len, &b122err);
    } else {
      got = base122_encode(in_data, in_len, 0 /* out */, 0 /* out_len */, &out_len, &b122err);
    }
    if (got == -1) {
      fprintf(stderr, "error in base122_%s: %s\n", decode ? "decode" : "encode", b122err.msg);
    }
  }

  {
    base122_error_t b122err;
    int got;
    size_t out_written;

    out_data = malloc(out_len);
    if (decode) {
      got = base122_decode(in_data, in_len, out_data /* out */, out_len /* out_len */, &out_written,
                           &b122err);
    } else {
      got = base122_encode(in_data, in_len, out_data /* out */, out_len /* out_len */, &out_written,
                           &b122err);
    }
    if (got == -1) {
      fprintf(stderr, "error in base122_%s: %s\n", decode ? "decode" : "encode", b122err.msg);
    }
    fprintf(stderr, "%sd %d bytes\n", decode ? "decode" : "encode", (int)out_written);
  }

  fwrite(out_data, sizeof(unsigned char), out_len, stdout);

  free(out_data);
  free(in_data);
  return 0;
}