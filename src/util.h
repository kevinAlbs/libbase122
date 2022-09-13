#ifndef LIBBASE_122_UTIL_H
#define LIBBASE_122_UTIL_H

#include <assert.h>
#include <stddef.h> /* for size_t */

typedef struct {
  const unsigned char *in;
  size_t len;
  size_t curIndex; /* TODO: if bitreader_get7 is removed, remove curIndex. */
  size_t curBit;
} bitreader_t;

/* bitreader_read reads nbits bits from reader.
 * nbits must be [1,8].
 * Returns the number of bits read. */
static size_t bitreader_read(bitreader_t *reader, size_t nbits, unsigned char *out) {

  /* Do not read more bits than stored. */
  size_t bitLen = reader->len * 8;
  size_t maxNbits = bitLen - reader->curBit;
  size_t firstByteIndex;
  size_t firstByteCurBit;
  unsigned short twoBytes;
  unsigned char mask;

  assert(nbits > 0);
  assert(nbits <= 8);

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

  firstByteIndex = reader->curBit / 8;
  firstByteCurBit = reader->curBit % 8;
  twoBytes = reader->in[firstByteIndex] << 8;
  if (firstByteIndex + 1 < reader->len) {
    size_t secondByteIndex = firstByteIndex + 1;
    twoBytes |= reader->in[secondByteIndex];
  }
  twoBytes >>= (8 - firstByteCurBit) + (8 - nbits);
  mask = (unsigned char)(~(255u << nbits));
  *out = (unsigned char)(twoBytes & mask);

  reader->curBit += nbits;

  return nbits;
}

#endif /* LIBBASE_122_UTIL_H */