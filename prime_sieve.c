#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <endian.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

#define DIE(msg) do { perror(msg); exit(1); } while (0)

typedef struct {
    size_t length;
    size_t capacity;
    uint32_t* buf;
} primelist;

primelist primelist_new(void) {
    primelist ret;
    ret.length = 0;
    ret.capacity = 1;
    ret.buf = malloc(sizeof(*ret.buf));

    return ret;
}

void primelist_free(primelist* pl) { free(pl->buf); }

void primelist_expand(primelist* pl) {
    pl->capacity *= 2;
    const size_t len = sizeof(*pl->buf) * pl->capacity;
    pl->buf = realloc(pl->buf, len);
}

void primelist_append(primelist* pl, uint32_t prime) {
    if (pl->capacity == pl->length) primelist_expand(pl);
    pl->buf[pl->length++] = prime;
}

bool is_prime(const primelist* pl, uint32_t candidate) {
    const uint32_t max = (uint32_t) sqrt((double)candidate);
    for (size_t i = 0; i < pl->length; ++i) {
        const uint32_t cur = pl->buf[i];
        if (cur > max) break;
        if (candidate % cur == 0) return false;
    }
    return true;
}

void print_progress(uint32_t i, uint32_t found) {
    const uint32_t mask = 0xffff;
    const uint32_t masked = i & mask;
    if (masked != mask) return;
    const double factor = 100.0 / UINT32_MAX;
    const double percent = factor * i;
    fprintf(stderr, "%08x (%02.5lf%%) %10u\n", i, percent, found);
}

int main(int argc, char* argv[]) {
    const uint32_t max_track = sqrt(UINT32_MAX);

    primelist pl = primelist_new();
    uint32_t found = 0;
    for (uint32_t i = 3; i < UINT32_MAX; i += 2) {
        print_progress(i, found);
        if (!is_prime(&pl, i)) continue;
        found += 1;
        const uint32_t be = htobe32(i);
        if (fwrite(&be, sizeof(be), 1, stdout) != 1) DIE("write failed");
        if (i > max_track) continue;
        primelist_append(&pl, i);
    }

    primelist_free(&pl);
    return 0;
}
