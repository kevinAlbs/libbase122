#ifndef LIBBASE_122_UTIL_H
#define LIBBASE_122_UTIL_H

#include <assert.h>
#include <stddef.h> // for size_t

typedef struct {
  const unsigned char *in;
  size_t len;
  size_t curIndex; // TODO: if bitreader_get7 is removed, remove curIndex.
  size_t curBit;
} bitreader_t;

/* get7 read seven bits of input data into out. Returns 0 if there is no input left and nothing was
 * read. */
static inline int bitreader_get7(bitreader_t *reader, unsigned char *out) {
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

/* bitreader_read reads nbits bits from reader.
 * nbits must be [1,8].
 * Returns the number of bits read. */
static inline size_t bitreader_read(bitreader_t *reader, size_t nbits, unsigned char *out) {
  assert(nbits > 0);
  assert(nbits <= 8);

  /* Do not read more bits than stored. */
  size_t bitLen = reader->len * 8;
  size_t maxNbits = bitLen - reader->curBit;
  if (nbits > maxNbits) {
    nbits = maxNbits;
  }

  /* Example of reading 6 bits:
   *
   * Read two bytes. Use 0 as second byte if only one byte is available.
   *
   * abcdefgh ijklmnop
   *    ^
   *    firstByteCurBit
   *
   * Shift right until only nbits requested remain.
   *
   * 0000000a bcdefghi
   *
   * Mask the second byte with nbits of 1's.
   *
   * 00000000 00defghi
   *
   * Output the second byte.
   */

  size_t firstByteIndex = reader->curBit / 8;
  size_t firstByteCurBit = reader->curBit % 8;
  unsigned short twoBytes = reader->in[firstByteIndex] << 8;
  if (firstByteIndex + 1 < reader->len) {
    size_t secondByteIndex = firstByteIndex + 1;
    twoBytes |= reader->in[secondByteIndex];
  }
  twoBytes >>= (8 - firstByteCurBit) + (8 - nbits);
  unsigned char mask = ~(0xFF << nbits);
  *out = (unsigned char)(twoBytes & mask);

  reader->curBit += nbits;

  return nbits;
}

#endif /* LIBBASE_122_UTIL_H */