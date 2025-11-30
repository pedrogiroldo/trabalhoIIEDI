/**
 * @file test_framework.h
 * @brief Simple unit testing framework for C99
 *
 * This module provides a lightweight testing framework with test
 * registration, execution, and reporting capabilities.
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdbool.h>

/**
 * @brief Test function signature
 * @return true if test passes, false otherwise
 */
typedef bool (*TestFunction)(void);

/**
 * @brief Initializes the test framework
 *
 * Must be called before registering or running tests.
 */
void test_framework_init(void);

/**
 * @brief Registers a test function
 *
 * @param test_name Name of the test (for reporting)
 * @param test_func Function pointer to the test
 */
void test_register(const char *test_name, TestFunction test_func);

/**
 * @brief Runs all registered tests
 *
 * Executes all tests that were registered via test_register()
 * and prints a summary of results.
 *
 * @return 0 if all tests passed, 1 if any test failed
 */
int test_run_all(void);

/**
 * @brief Prints a section header for organizing test output
 *
 * @param section_name Name of the test section
 */
void test_print_section(const char *section_name);

/**
 * @brief Cleans up test framework resources
 *
 * Should be called after all tests have been run.
 */
void test_framework_cleanup(void);

// Helper macros for assertions
#define ASSERT_TRUE(condition)                                                 \
  do {                                                                         \
    if (!(condition)) {                                                        \
      return false;                                                            \
    }                                                                          \
  } while (0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_NULL(ptr) ASSERT_TRUE((ptr) == NULL)

#define ASSERT_NOT_NULL(ptr) ASSERT_TRUE((ptr) != NULL)

#define ASSERT_EQUAL(a, b) ASSERT_TRUE((a) == (b))

#define ASSERT_NOT_EQUAL(a, b) ASSERT_TRUE((a) != (b))

#define ASSERT_STR_EQUAL(str1, str2)                                           \
  ASSERT_TRUE((str1) != NULL && (str2) != NULL && strcmp((str1), (str2)) == 0)

#endif // TEST_FRAMEWORK_H
