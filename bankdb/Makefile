CC     = gcc
CFLAGS = -g -Wall -O0 -std=c99 -Iinclude

# ─── Sources ─────────────────────────────────────────────────────────
SOURCES = src/main.c     \
          src/bank.c     \
          src/buffer_pool.c      \
          src/lock_mgr.c     \
          src/metrics.c       \
          src/timer.c  \
          src/transaction.c    \
          src/utils.c

OBJECTS = $(SOURCES:.c=.o)
TARGET  = bankdb
HEADERS = include/bank.h   \
          include/buffer_pool.h \
          include/lock_mgr.h     \
		  include/timer.h\
		  include/transaction.h\
          include/metrics.h

# ─── Build ───────────────────────────────────────────────────────────
all: $(TARGET)
	@echo "Build complete. Cleaning object files..."
	$(MAKE) clean-objects

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# ─── Cleanup ─────────────────────────────────────────────────────────
.PHONY: all clean clean-objects test

clean-objects:
	rm -f $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(TARGET)

test: $(TARGET)
	@bash tests/test_suite.sh