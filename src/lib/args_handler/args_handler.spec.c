/**
 * @file args_handler.spec.c
 * @brief Unit tests for args_handler module
 *
 * This file contains comprehensive unit tests for the command-line
 * argument parsing functions defined in args_handler.h
 */

#include "args_handler.h"
#include "../test_framework/test_framework.h"

#include <string.h>

// ============================================================================
// Tests for get_option_value()
// ============================================================================

/**
 * Test: get_option_value should return the value when option exists
 */
bool test_get_option_value_basic(void) {
  char *argv[] = {"program", "-f", "input.txt", "-o", "output.txt"};
  int argc = 5;

  char *result = get_option_value(argc, argv, "f");
  ASSERT_STR_EQUAL(result, "input.txt");

  result = get_option_value(argc, argv, "o");
  ASSERT_STR_EQUAL(result, "output.txt");

  return true;
}

/**
 * Test: get_option_value should return NULL when option doesn't exist
 */
bool test_get_option_value_not_found(void) {
  char *argv[] = {"program", "-f", "input.txt"};
  int argc = 3;

  char *result = get_option_value(argc, argv, "x");
  ASSERT_NULL(result);

  return true;
}

/**
 * Test: get_option_value should return NULL when option is last argument
 */
bool test_get_option_value_no_value(void) {
  char *argv[] = {"program", "-f", "input.txt", "-o"};
  int argc = 4;

  char *result = get_option_value(argc, argv, "o");
  ASSERT_NULL(result);

  return true;
}

/**
 * Test: get_option_value with empty arguments
 */
bool test_get_option_value_empty_args(void) {
  char *argv[] = {"program"};
  int argc = 1;

  char *result = get_option_value(argc, argv, "f");
  ASSERT_NULL(result);

  return true;
}

/**
 * Test: get_option_value with multiple options
 */
bool test_get_option_value_multiple_options(void) {
  char *argv[] = {"program", "-e",      "path/", "-f",       "file.geo",
                  "-o",      "out.svg", "-q",    "query.txt"};
  int argc = 9;

  char *result_e = get_option_value(argc, argv, "e");
  ASSERT_STR_EQUAL(result_e, "path/");

  char *result_f = get_option_value(argc, argv, "f");
  ASSERT_STR_EQUAL(result_f, "file.geo");

  char *result_o = get_option_value(argc, argv, "o");
  ASSERT_STR_EQUAL(result_o, "out.svg");

  char *result_q = get_option_value(argc, argv, "q");
  ASSERT_STR_EQUAL(result_q, "query.txt");

  return true;
}

/**
 * Test: get_option_value with option value that looks like another option
 */
bool test_get_option_value_with_dash_value(void) {
  char *argv[] = {"program", "-f", "-special-file.txt"};
  int argc = 3;

  char *result = get_option_value(argc, argv, "f");
  ASSERT_STR_EQUAL(result, "-special-file.txt");

  return true;
}

// ============================================================================
// Tests for get_command_suffix()
// ============================================================================

/**
 * Test: get_command_suffix should return the last non-option argument
 */
bool test_get_command_suffix_basic(void) {
  char *argv[] = {"program", "-f", "input.txt", "command"};
  int argc = 4;

  char *result = get_command_suffix(argc, argv);
  ASSERT_STR_EQUAL(result, "command");

  return true;
}

/**
 * Test: get_command_suffix with no suffix (only options)
 */
bool test_get_command_suffix_no_suffix(void) {
  char *argv[] = {"program", "-f", "input.txt", "-o", "output.txt"};
  int argc = 5;

  char *result = get_command_suffix(argc, argv);
  ASSERT_NULL(result);

  return true;
}

/**
 * Test: get_command_suffix with only program name
 */
bool test_get_command_suffix_only_program(void) {
  char *argv[] = {"program"};
  int argc = 1;

  char *result = get_command_suffix(argc, argv);
  ASSERT_NULL(result);

  return true;
}

/**
 * Test: get_command_suffix with suffix and no options
 */
bool test_get_command_suffix_no_options(void) {
  char *argv[] = {"program", "suffix"};
  int argc = 2;

  char *result = get_command_suffix(argc, argv);
  ASSERT_STR_EQUAL(result, "suffix");

  return true;
}

/**
 * Test: get_command_suffix with complex argument list
 */
bool test_get_command_suffix_complex(void) {
  char *argv[] = {"program",  "-e", "path/",   "-f",
                  "file.geo", "-o", "out.svg", "vis"};
  int argc = 8;

  char *result = get_command_suffix(argc, argv);
  ASSERT_STR_EQUAL(result, "vis");

  return true;
}

/**
 * Test: get_command_suffix with suffix before options
 */
bool test_get_command_suffix_before_options(void) {
  char *argv[] = {"program", "suffix", "-f", "input.txt"};
  int argc = 4;

  char *result = get_command_suffix(argc, argv);
  ASSERT_STR_EQUAL(result, "suffix");

  return true;
}

/**
 * Test: get_command_suffix with standalone option flag at the end
 */
bool test_get_command_suffix_option_at_end(void) {
  char *argv[] = {"program", "-f", "input.txt", "-v", "suffix"};
  int argc = 5;

  // -v is treated as an option with "suffix" as its value, so no suffix
  char *result = get_command_suffix(argc, argv);
  ASSERT_NULL(result);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for get_option_value
  test_print_section("Testing get_option_value()");
  test_register("test_get_option_value_basic", test_get_option_value_basic);
  test_register("test_get_option_value_not_found",
                test_get_option_value_not_found);
  test_register("test_get_option_value_no_value",
                test_get_option_value_no_value);
  test_register("test_get_option_value_empty_args",
                test_get_option_value_empty_args);
  test_register("test_get_option_value_multiple_options",
                test_get_option_value_multiple_options);
  test_register("test_get_option_value_with_dash_value",
                test_get_option_value_with_dash_value);

  // Register tests for get_command_suffix
  test_print_section("Testing get_command_suffix()");
  test_register("test_get_command_suffix_basic", test_get_command_suffix_basic);
  test_register("test_get_command_suffix_no_suffix",
                test_get_command_suffix_no_suffix);
  test_register("test_get_command_suffix_only_program",
                test_get_command_suffix_only_program);
  test_register("test_get_command_suffix_no_options",
                test_get_command_suffix_no_options);
  test_register("test_get_command_suffix_complex",
                test_get_command_suffix_complex);
  test_register("test_get_command_suffix_before_options",
                test_get_command_suffix_before_options);
  test_register("test_get_command_suffix_option_at_end",
                test_get_command_suffix_option_at_end);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
