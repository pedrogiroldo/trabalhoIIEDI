/**
 * @file text_style.spec.c
 * @brief Unit tests for text_style module
 *
 * Comprehensive unit tests for the text_style ADT functions defined in
 * text_style.h
 */

#include "text_style.h"
#include "../../test_framework/test_framework.h"

#include <string.h>

// ============================================================================
// Tests for text_style_create()
// ============================================================================

/**
 * Test: text_style_create should return a valid Shape instance
 */
bool test_text_style_create_basic(void) {
  // Act: Create a text style
  Shape shape = text_style_create("Arial", 'n', 12);

  // Assert: Shape should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_style_create should handle different font families
 */
bool test_text_style_create_different_fonts(void) {
  // Act: Create text styles with different fonts
  Shape shape1 = text_style_create("Arial", 'n', 12);
  Shape shape2 = text_style_create("Times New Roman", 'b', 14);
  Shape shape3 = text_style_create("Courier", 'i', 10);

  // Assert: All should be created successfully
  ASSERT_NOT_NULL(shape1);
  ASSERT_NOT_NULL(shape2);
  ASSERT_NOT_NULL(shape3);

  // Cleanup
  shape_destroy(shape1);
  shape_destroy(shape2);
  shape_destroy(shape3);

  return true;
}

/**
 * Test: text_style_create should handle different font weights
 */
bool test_text_style_create_different_weights(void) {
  // Act: Create text styles with different weights
  Shape shape1 = text_style_create("Arial", 'n', 12); // normal
  Shape shape2 = text_style_create("Arial", 'b', 12); // bold
  Shape shape3 = text_style_create("Arial", 'i', 12); // italic

  // Assert: All should be created successfully
  ASSERT_NOT_NULL(shape1);
  ASSERT_NOT_NULL(shape2);
  ASSERT_NOT_NULL(shape3);

  // Cleanup
  shape_destroy(shape1);
  shape_destroy(shape2);
  shape_destroy(shape3);

  return true;
}

/**
 * Test: text_style_create should handle different font sizes
 */
bool test_text_style_create_different_sizes(void) {
  // Act: Create text styles with different sizes
  Shape shape1 = text_style_create("Arial", 'n', 8);
  Shape shape2 = text_style_create("Arial", 'n', 12);
  Shape shape3 = text_style_create("Arial", 'n', 24);
  Shape shape4 = text_style_create("Arial", 'n', 48);

  // Assert: All should be created successfully
  ASSERT_NOT_NULL(shape1);
  ASSERT_NOT_NULL(shape2);
  ASSERT_NOT_NULL(shape3);
  ASSERT_NOT_NULL(shape4);

  // Cleanup
  shape_destroy(shape1);
  shape_destroy(shape2);
  shape_destroy(shape3);
  shape_destroy(shape4);

  return true;
}

// ============================================================================
// Tests for text_style getter functions
// ============================================================================

/**
 * Test: text_style_get_font_family should return correct font family
 */
bool test_text_style_get_font_family(void) {
  // Arrange: Create a text style
  Shape shape = text_style_create("Times New Roman", 'b', 14);
  ASSERT_NOT_NULL(shape);
  TextStyle text_style = (TextStyle)shape_get_shape(shape);

  // Act: Get the font family
  const char *font_family = text_style_get_font_family(text_style);

  // Assert: Should return correct font family
  ASSERT_NOT_NULL(font_family);
  ASSERT_STR_EQUAL(font_family, "Times New Roman");

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_style_get_font_weight should return correct font weight
 */
bool test_text_style_get_font_weight(void) {
  // Arrange: Create a text style
  Shape shape = text_style_create("Arial", 'b', 12);
  ASSERT_NOT_NULL(shape);
  TextStyle text_style = (TextStyle)shape_get_shape(shape);

  // Act: Get the font weight
  char font_weight = text_style_get_font_weight(text_style);

  // Assert: Should return correct font weight
  ASSERT_EQUAL(font_weight, 'b');

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_style_get_font_size should return correct font size
 */
bool test_text_style_get_font_size(void) {
  // Arrange: Create a text style
  Shape shape = text_style_create("Arial", 'n', 18);
  ASSERT_NOT_NULL(shape);
  TextStyle text_style = (TextStyle)shape_get_shape(shape);

  // Act: Get the font size
  int font_size = text_style_get_font_size(text_style);

  // Assert: Should return correct font size
  ASSERT_EQUAL(font_size, 18);

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for text_style_destroy()
// ============================================================================

/**
 * Test: text_style_destroy should handle NULL gracefully
 */
bool test_text_style_destroy_null(void) {
  // Act & Assert: Should not crash
  text_style_destroy(NULL);

  return true;
}

/**
 * Test: text_style_destroy should free all resources
 */
bool test_text_style_destroy_cleanup(void) {
  // Arrange: Create a text style
  Shape shape = text_style_create("Arial", 'n', 12);
  ASSERT_NOT_NULL(shape);
  TextStyle text_style = (TextStyle)shape_get_shape(shape);

  // Act: Destroy the text style (should not crash)
  text_style_destroy(text_style);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for text_style_create
  test_print_section("Testing text_style_create()");
  test_register("test_text_style_create_basic", test_text_style_create_basic);
  test_register("test_text_style_create_different_fonts",
                test_text_style_create_different_fonts);
  test_register("test_text_style_create_different_weights",
                test_text_style_create_different_weights);
  test_register("test_text_style_create_different_sizes",
                test_text_style_create_different_sizes);

  // Register tests for text_style getters
  test_print_section("Testing text_style getter functions");
  test_register("test_text_style_get_font_family",
                test_text_style_get_font_family);
  test_register("test_text_style_get_font_weight",
                test_text_style_get_font_weight);
  test_register("test_text_style_get_font_size", test_text_style_get_font_size);

  // Register tests for text_style_destroy
  test_print_section("Testing text_style_destroy()");
  test_register("test_text_style_destroy_null", test_text_style_destroy_null);
  test_register("test_text_style_destroy_cleanup",
                test_text_style_destroy_cleanup);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
