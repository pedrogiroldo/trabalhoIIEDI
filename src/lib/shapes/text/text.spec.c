/**
 * @file text.spec.c
 * @brief Unit tests for text module
 *
 * Comprehensive unit tests for the text ADT functions defined in text.h
 */

#include "text.h"
#include "../../test_framework/test_framework.h"

#include <string.h>

// ============================================================================
// Tests for text_create()
// ============================================================================

/**
 * Test: text_create should return a valid Shape instance
 */
bool test_text_create_basic(void) {
  // Act: Create a text
  Shape shape = text_create(1, 10.5, 20.3, "red", "blue", 'n', "Hello World");

  // Assert: Shape should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_create should handle empty text
 */
bool test_text_create_empty_text(void) {
  // Act: Create a text with empty string
  Shape shape = text_create(2, 0.0, 0.0, "black", "white", 'c', "");

  // Assert: Should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_create should handle different anchors
 */
bool test_text_create_different_anchors(void) {
  // Act: Create texts with different anchors
  Shape shape1 = text_create(3, 10.0, 20.0, "red", "blue", 'n', "North");
  Shape shape2 = text_create(4, 10.0, 20.0, "red", "blue", 's', "South");
  Shape shape3 = text_create(5, 10.0, 20.0, "red", "blue", 'e', "East");
  Shape shape4 = text_create(6, 10.0, 20.0, "red", "blue", 'w', "West");

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
// Tests for text getter functions
// ============================================================================

/**
 * Test: text_get_id should return correct ID
 */
bool test_text_get_id(void) {
  // Arrange: Create a text
  Shape shape = text_create(42, 10.0, 20.0, "red", "blue", 'n', "Test");
  ASSERT_NOT_NULL(shape);
  Text text = (Text)shape_get_shape(shape);

  // Act: Get the ID
  int id = text_get_id(text);

  // Assert: Should return correct ID
  ASSERT_EQUAL(id, 42);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_get_x should return correct X coordinate
 */
bool test_text_get_x(void) {
  // Arrange: Create a text
  Shape shape = text_create(1, 15.75, 20.0, "red", "blue", 'n', "Test");
  ASSERT_NOT_NULL(shape);
  Text text = (Text)shape_get_shape(shape);

  // Act: Get the X coordinate
  double x = text_get_x(text);

  // Assert: Should return correct X
  ASSERT_TRUE(x == 15.75);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_get_y should return correct Y coordinate
 */
bool test_text_get_y(void) {
  // Arrange: Create a text
  Shape shape = text_create(1, 10.0, 25.5, "red", "blue", 'n', "Test");
  ASSERT_NOT_NULL(shape);
  Text text = (Text)shape_get_shape(shape);

  // Act: Get the Y coordinate
  double y = text_get_y(text);

  // Assert: Should return correct Y
  ASSERT_TRUE(y == 25.5);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_get_border_color should return correct color
 */
bool test_text_get_border_color(void) {
  // Arrange: Create a text
  Shape shape = text_create(1, 10.0, 20.0, "red", "blue", 'n', "Test");
  ASSERT_NOT_NULL(shape);
  Text text = (Text)shape_get_shape(shape);

  // Act: Get the border color
  const char *color = text_get_border_color(text);

  // Assert: Should return correct color
  ASSERT_NOT_NULL(color);
  ASSERT_STR_EQUAL(color, "red");

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_get_fill_color should return correct color
 */
bool test_text_get_fill_color(void) {
  // Arrange: Create a text
  Shape shape = text_create(1, 10.0, 20.0, "red", "blue", 'n', "Test");
  ASSERT_NOT_NULL(shape);
  Text text = (Text)shape_get_shape(shape);

  // Act: Get the fill color
  const char *color = text_get_fill_color(text);

  // Assert: Should return correct color
  ASSERT_NOT_NULL(color);
  ASSERT_STR_EQUAL(color, "blue");

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_get_anchor should return correct anchor
 */
bool test_text_get_anchor(void) {
  // Arrange: Create a text
  Shape shape = text_create(1, 10.0, 20.0, "red", "blue", 's', "Test");
  ASSERT_NOT_NULL(shape);
  Text text = (Text)shape_get_shape(shape);

  // Act: Get the anchor
  char anchor = text_get_anchor(text);

  // Assert: Should return correct anchor
  ASSERT_EQUAL(anchor, 's');

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: text_get_text should return correct text content
 */
bool test_text_get_text(void) {
  // Arrange: Create a text
  Shape shape = text_create(1, 10.0, 20.0, "red", "blue", 'n', "Hello World");
  ASSERT_NOT_NULL(shape);
  Text text = (Text)shape_get_shape(shape);

  // Act: Get the text content
  const char *content = text_get_text(text);

  // Assert: Should return correct text
  ASSERT_NOT_NULL(content);
  ASSERT_STR_EQUAL(content, "Hello World");

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for text_destroy()
// ============================================================================

/**
 * Test: text_destroy should handle NULL gracefully
 */
bool test_text_destroy_null(void) {
  // Act & Assert: Should not crash
  text_destroy(NULL);

  return true;
}

/**
 * Test: text_destroy should free all resources
 */
bool test_text_destroy_cleanup(void) {
  // Arrange: Create a text
  Shape shape = text_create(1, 10.0, 20.0, "red", "blue", 'n', "Test");
  ASSERT_NOT_NULL(shape);
  Text text = (Text)shape_get_shape(shape);

  // Act: Destroy the text (should not crash)
  text_destroy(text);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for text_create
  test_print_section("Testing text_create()");
  test_register("test_text_create_basic", test_text_create_basic);
  test_register("test_text_create_empty_text", test_text_create_empty_text);
  test_register("test_text_create_different_anchors",
                test_text_create_different_anchors);

  // Register tests for text getters
  test_print_section("Testing text getter functions");
  test_register("test_text_get_id", test_text_get_id);
  test_register("test_text_get_x", test_text_get_x);
  test_register("test_text_get_y", test_text_get_y);
  test_register("test_text_get_border_color", test_text_get_border_color);
  test_register("test_text_get_fill_color", test_text_get_fill_color);
  test_register("test_text_get_anchor", test_text_get_anchor);
  test_register("test_text_get_text", test_text_get_text);

  // Register tests for text_destroy
  test_print_section("Testing text_destroy()");
  test_register("test_text_destroy_null", test_text_destroy_null);
  test_register("test_text_destroy_cleanup", test_text_destroy_cleanup);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
