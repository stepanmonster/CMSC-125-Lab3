CC     = gcc
CFLAGS = -g -Wall -O0 -std=c99 -Iinclude -pthread
DEBUG_FLAGS = -fsanitize=thread -pthread

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
DEBUG_OBJECTS = $(SOURCES:.c=.debug.o)
TARGET  = ban
DEBUG_TARGET = bankdb-debug
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

debug: $(DEBUG_TARGET)
	@echo "Debug build complete with ThreadSanitizer"
	@echo "Run with: ./$(DEBUG_TARGET) --accounts=tests/accounts.txt --trace=tests/trace_readers.txt --deadlock=prevention"

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.debug.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

# ─── Cleanup ─────────────────────────────────────────────────────────
.PHONY: all debug clean clean-objects clean-debug test test-debug

clean-objects:
	rm -f $(OBJECTS)

clean-debug:
	rm -f $(DEBUG_OBJECTS)

clean: clean-objects clean-debug
	rm -f $(TARGET) $(DEBUG_TARGET)

test: $(TARGET)
	@bash tests/test_suite.sh

test-debug: $(DEBUG_TARGET)
	@echo "Running tests with ThreadSanitizer..."
	@bash tests/test_suite.sh $(DEBUG_TARGET)

# ─── Individual test targets ─────────────────────────────────────────
test-readers: $(TARGET)
	@echo "=== Testing Concurrent Readers ==="
	./$(TARGET) --accounts=tests/accounts.txt --trace=tests/trace_readers.txt --deadlock=prevention --tick-ms=10

test-deadlock-prevention: $(TARGET)
	@echo "=== Testing Deadlock Prevention ==="
	./$(TARGET) --accounts=tests/accounts.txt --trace=tests/trace_deadlock.txt --deadlock=prevention --verbose

test-deadlock-detection: $(TARGET)
	@echo "=== Testing Deadlock Detection ==="
	./$(TARGET) --accounts=tests/accounts.txt --trace=tests/trace_deadlock.txt --deadlock=detection --verbose

test-buffer: $(TARGET)
	@echo "=== Testing Buffer Pool Saturation ==="
	./$(TARGET) --accounts=tests/accounts.txt --trace=tests/trace_buffer.txt --deadlock=prevention --tick-ms=10

test-abort: $(TARGET)
	@echo "=== Testing Insufficient Funds ==="
	./$(TARGET) --accounts=tests/accounts.txt --trace=tests/trace_abort.txt --deadlock=prevention

# ─── Performance comparison targets ───────────────────────────────────
benchmark-mutex: $(TARGET)
	@echo "=== Benchmark: Mutex (modify code to use mutex) ==="
	@# This would require code modification - documented in design.md
	@echo "Run with modified code using pthread_mutex_t instead of pthread_rwlock_t"

benchmark-rwlock: $(TARGET)
	@echo "=== Benchmark: Reader-Writer Locks ==="
	time ./$(TARGET) --accounts=tests/accounts.txt --trace=tests/trace_readers.txt --deadlock=prevention --tick-ms=10

# ─── Help target ─────────────────────────────────────────────────────
help:
	@echo "Available targets:"
	@echo "  make                 - Build regular version"
	@echo "  make debug           - Build with ThreadSanitizer (required for correctness)"
	@echo "  make clean           - Remove all build files"
	@echo "  make test            - Run test suite with regular build"
	@echo "  make test-debug      - Run test suite with ThreadSanitizer"
	@echo "  make test-readers    - Test concurrent readers"
	@echo "  make test-deadlock-prevention - Test deadlock prevention"
	@echo "  make test-deadlock-detection  - Test deadlock detection"
	@echo "  make test-buffer      - Test buffer pool saturation"
	@echo "  make test-abort       - Test insufficient funds"
	@echo "  make benchmark-rwlock - Benchmark reader-writer locks"