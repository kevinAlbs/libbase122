#ifndef LIBBASE_122_UTIL_H
#define LIBBASE_122_UTIL_H

#include <stddef.h> // for size_t

typedef struct {
  const unsigned char *in;
  size_t len;
  size_t curIndex;
  size_t curBit;
} bitreader_t;

/* get7 read seven bits of input data into out. Returns 0 if there is no input left and nothing was
 * read. */
static int bitreader_get7(bitreader_t *reader, unsigned char *out) {
  unsigned char firstByte, secondByte, firstPart, secondPart;

  if (reader->curIndex >= reader->len)
    return 0;

  /* Shift, mask, unshift to get first part. */
  firstByte = reader->in[reader->curIndex];
  firstPart = ((0xFE >> reader->curBit) & firstByte) << reader->curBit;
  /* Align it to a seven bit chunk. */
  firstPart >>= 1;
  /* Check if we need to go to the next byte for more bits. */
  reader->curBit += 7;
  if (reader->curBit < 8) {
    *out = firstPart;
    return 1; /* Do not need next byte. */
  }
  reader->curBit -= 8;
  reader->curIndex++;
  /* Now we want bits [0..curBit] of the next byte if it exists. */
  if (reader->curIndex >= reader->len) {
    *out = firstPart;
    return 1;
  }
  secondByte = reader->in[reader->curIndex];
  secondPart = ((0xFF00 >> reader->curBit) & secondByte) & 0xFF;
  /* Align it. */
  secondPart >>= 8 - reader->curBit;
  *out = firstPart | secondPart;
  return 1;
}

#endif /* LIBBASE_122_UTIL_H */