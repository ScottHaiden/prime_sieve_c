CFLAGS := -O3 -std=c17 -march=native -Wall -Werror -Wpedantic
LDFLAGS := -fpic -pie -lm

BINS := prime_sieve

.PHONY: all
all: $(BINS)

.PHONY: clean
clean:
	rm -f $(BINS)
