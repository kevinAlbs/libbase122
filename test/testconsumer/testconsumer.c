#include "base122.h"

#include <stdio.h>

int main (void) {
    size_t out_bytes_needed;
    base122_error_t error;
    int got = base122_encode((const unsigned char*)"foo", 3, NULL, 0, &out_bytes_needed, &error);
    if (got == -1) {
        fprintf (stderr, "unexpected error: %s\n", error.msg);
        return 1;
    }
    if (out_bytes_needed != 4) {
        fprintf (stderr, "expected %zu, but got %zu\n", (size_t) 4, out_bytes_needed);
        return 1;
    }
    printf ("Test passed.\n");
    return 0;
}