/**
 * @file test_framework.c
 * @brief Implementation of the unit testing framework
 */

#include "test_framework.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of tests that can be registered
#define MAX_TESTS 100

// Structure to hold test information
typedef struct {
  const char *name;
  TestFunction func;
} TestCase;

// Global test registry
static TestCase test_registry[MAX_TESTS];
static int test_count = 0;
static int tests_run = 0;
static int tests_passed = 0;

void test_framework_init(void) {
  test_count = 0;
  tests_run = 0;
  tests_passed = 0;
  memset(test_registry, 0, sizeof(test_registry));
}

void test_register(const char *test_name, TestFunction test_func) {
  if (test_count >= MAX_TESTS) {
    fprintf(stderr, "Error: Maximum number of tests (%d) exceeded\n",
            MAX_TESTS);
    return;
  }

  test_registry[test_count].name = test_name;
  test_registry[test_count].func = test_func;
  test_count++;
}

int test_run_all(void) {
  printf("========================================\n");
  printf("Running Tests\n");
  printf("========================================\n\n");

  for (int i = 0; i < test_count; i++) {
    printf("Running: %s\n", test_registry[i].name);
    tests_run++;

    bool result = test_registry[i].func();
    if (result) {
      tests_passed++;
      printf("  ✓ PASSED\n");
    } else {
      printf("  ✗ FAILED\n");
    }
  }

  // Print summary
  printf("\n========================================\n");
  printf("Test Summary:\n");
  printf("  Total:  %d\n", tests_run);
  printf("  Passed: %d\n", tests_passed);
  printf("  Failed: %d\n", tests_run - tests_passed);
  printf("========================================\n");

  // Return 0 if all tests passed, 1 otherwise
  return (tests_run == tests_passed) ? 0 : 1;
}

void test_print_section(const char *section_name) {
  printf("\n--- %s ---\n", section_name);
}

void test_framework_cleanup(void) {
  test_count = 0;
  tests_run = 0;
  tests_passed = 0;
}
