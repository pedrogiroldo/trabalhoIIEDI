# Makefile atualizado para automatizar OBJETOS e dependências
PROJ_NAME = ted
LIBS = -lm
# Tenta find primeiro, se falhar usa wildcard
SRC_FILES := $(shell find src -name "*.c" ! -name "*.spec.c" 2>/dev/null)
ifeq ($(SRC_FILES),)
    SRC_FILES := $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c)
    SRC_FILES := $(filter-out %.spec.c,$(SRC_FILES))
endif
OBJETOS := $(SRC_FILES:.c=.o)       # Substitui .c por .o

# Test files
TEST_FILES := $(shell find src -name "*.spec.c" 2>/dev/null)
TEST_BINS := $(TEST_FILES:.spec.c=_test)
TEST_FRAMEWORK_SRC = src/lib/test_framework/test_framework.c

# Compilador e Flags
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration
LDFLAGS = -O0

# Regra principal
$(PROJ_NAME): $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(LDFLAGS) $(OBJETOS) $(LIBS)

# Regra para compilar arquivos .c em .o com dependências
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# ============================================================================
# Test Targets
# ============================================================================

# Build and run all tests
test: test-build test-run

# Build all test executables
test-build: $(TEST_BINS)

# Pattern rule to build test executables
# For each .spec.c file, compile it with the corresponding module and test framework
%_test: %.spec.c %.c $(TEST_FRAMEWORK_SRC)
	@echo "Building test: $@"
	$(CC) $(CFLAGS) -I$(dir $(TEST_FRAMEWORK_SRC)) -o $@ $^ $(LIBS)

# Run all tests
test-run: $(TEST_BINS)
	@echo "========================================="
	@echo "Running All Tests"
	@echo "========================================="
	@for test in $(TEST_BINS); do \
		echo ""; \
		echo ">>> Running $$test"; \
		./$$test || exit 1; \
	done
	@echo ""
	@echo "========================================="
	@echo "All Tests Passed!"
	@echo "========================================="

# Clean test executables
test-clean:
	@echo "Cleaning test executables..."
	rm -f $(TEST_BINS)

# Target para limpeza
clean: test-clean
	rm -f $(OBJETOS) $(PROJ_NAME)

# Target para debug (mostra variáveis)
debug:
	@echo "SRC_FILES: $(SRC_FILES)"
	@echo "OBJETOS: $(OBJETOS)"
	@echo "TEST_FILES: $(TEST_FILES)"
	@echo "TEST_BINS: $(TEST_BINS)"

run:
	./$(PROJ_NAME) -f src/test/test.geo -o src/test/results -q src/test/test.qry

.PHONY: test test-build test-run test-clean clean debug run