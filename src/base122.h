#ifndef LIBBASE122_H
#define LIBBASE122_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> /* for size_t */

typedef struct {
  char msg[254];
} base122_error_t;

/* Define export macro. */
#define BASE122_EXPORT

#if defined(_MSC_VER) && !defined(BASE122_STATIC) && !defined(BASE122_COMPILING)
/* Using shared library. Import symbols. */
#undef BASE122_EXPORT
#define BASE122_EXPORT __declspec(dllimport)
#endif

#if defined(_MSC_VER) && !defined(BASE122_STATIC) && defined(BASE122_COMPILING)
/* Building shared library. Export symbols. */
#undef BASE122_EXPORT
#define BASE122_EXPORT __declspec(dllexport)
#endif

/* Encodes data in base122.
 * Pass NULL for `out` to determine the exact size required for `out`.
 * Returns -1 on error. */
int BASE122_EXPORT base122_encode(const unsigned char *in, size_t in_len, unsigned char *out,
                                  size_t out_len, size_t *out_written, base122_error_t *error);

/* Decodes data in base122.
 * Pass NULL for `out` to determine the exact size required for `out`.
 * Returns -1 on error. */
int BASE122_EXPORT base122_decode(const unsigned char *in, size_t in_len, unsigned char *out,
                                  size_t out_len, size_t *out_written, base122_error_t *error);


#ifdef __cplusplus
}
#endif

#endif /* LIBBASE122_H */
