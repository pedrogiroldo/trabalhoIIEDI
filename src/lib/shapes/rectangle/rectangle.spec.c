/**
 * @file rectangle.spec.c
 * @brief Unit tests for rectangle module
 *
 * Comprehensive unit tests for the rectangle ADT functions defined in
 * rectangle.h
 */

#include "rectangle.h"
#include "../../test_framework/test_framework.h"

#include <string.h>

// ============================================================================
// Tests for rectangle_create()
// ============================================================================

/**
 * Test: rectangle_create should return a valid Shape instance
 */
bool test_rectangle_create_basic(void) {
  // Act: Create a rectangle
  Shape shape = rectangle_create(1, 10.5, 20.3, 15.0, 25.0, "red", "blue");

  // Assert: Shape should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: rectangle_create should handle zero dimensions
 */
bool test_rectangle_create_zero_dimensions(void) {
  // Act: Create a rectangle with zero dimensions
  Shape shape = rectangle_create(2, 0.0, 0.0, 0.0, 0.0, "black", "white");

  // Assert: Should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: rectangle_create should handle negative coordinates
 */
bool test_rectangle_create_negative_coords(void) {
  // Act: Create a rectangle with negative coordinates
  Shape shape =
      rectangle_create(3, -10.5, -20.3, 15.0, 25.0, "green", "yellow");

  // Assert: Should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for rectangle getter functions
// ============================================================================

/**
 * Test: rectangle_get_id should return correct ID
 */
bool test_rectangle_get_id(void) {
  // Arrange: Create a rectangle
  Shape shape = rectangle_create(42, 10.0, 20.0, 15.0, 25.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Rectangle rectangle = (Rectangle)shape_get_shape(shape);

  // Act: Get the ID
  int id = rectangle_get_id(rectangle);

  // Assert: Should return correct ID
  ASSERT_EQUAL(id, 42);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: rectangle_get_x should return correct X coordinate
 */
bool test_rectangle_get_x(void) {
  // Arrange: Create a rectangle
  Shape shape = rectangle_create(1, 15.75, 20.0, 15.0, 25.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Rectangle rectangle = (Rectangle)shape_get_shape(shape);

  // Act: Get the X coordinate
  double x = rectangle_get_x(rectangle);

  // Assert: Should return correct X
  ASSERT_TRUE(x == 15.75);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: rectangle_get_y should return correct Y coordinate
 */
bool test_rectangle_get_y(void) {
  // Arrange: Create a rectangle
  Shape shape = rectangle_create(1, 10.0, 25.5, 15.0, 25.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Rectangle rectangle = (Rectangle)shape_get_shape(shape);

  // Act: Get the Y coordinate
  double y = rectangle_get_y(rectangle);

  // Assert: Should return correct Y
  ASSERT_TRUE(y == 25.5);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: rectangle_get_width should return correct width
 */
bool test_rectangle_get_width(void) {
  // Arrange: Create a rectangle
  Shape shape = rectangle_create(1, 10.0, 20.0, 17.25, 25.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Rectangle rectangle = (Rectangle)shape_get_shape(shape);

  // Act: Get the width
  double width = rectangle_get_width(rectangle);

  // Assert: Should return correct width
  ASSERT_TRUE(width == 17.25);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: rectangle_get_height should return correct height
 */
bool test_rectangle_get_height(void) {
  // Arrange: Create a rectangle
  Shape shape = rectangle_create(1, 10.0, 20.0, 15.0, 27.5, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Rectangle rectangle = (Rectangle)shape_get_shape(shape);

  // Act: Get the height
  double height = rectangle_get_height(rectangle);

  // Assert: Should return correct height
  ASSERT_TRUE(height == 27.5);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: rectangle_get_border_color should return correct color
 */
bool test_rectangle_get_border_color(void) {
  // Arrange: Create a rectangle
  Shape shape = rectangle_create(1, 10.0, 20.0, 15.0, 25.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Rectangle rectangle = (Rectangle)shape_get_shape(shape);

  // Act: Get the border color
  const char *color = rectangle_get_border_color(rectangle);

  // Assert: Should return correct color
  ASSERT_NOT_NULL(color);
  ASSERT_STR_EQUAL(color, "red");

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: rectangle_get_fill_color should return correct color
 */
bool test_rectangle_get_fill_color(void) {
  // Arrange: Create a rectangle
  Shape shape = rectangle_create(1, 10.0, 20.0, 15.0, 25.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Rectangle rectangle = (Rectangle)shape_get_shape(shape);

  // Act: Get the fill color
  const char *color = rectangle_get_fill_color(rectangle);

  // Assert: Should return correct color
  ASSERT_NOT_NULL(color);
  ASSERT_STR_EQUAL(color, "blue");

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for rectangle_destroy()
// ============================================================================

/**
 * Test: rectangle_destroy should handle NULL gracefully
 */
bool test_rectangle_destroy_null(void) {
  // Act & Assert: Should not crash
  rectangle_destroy(NULL);

  return true;
}

/**
 * Test: rectangle_destroy should free all resources
 */
bool test_rectangle_destroy_cleanup(void) {
  // Arrange: Create a rectangle
  Shape shape = rectangle_create(1, 10.0, 20.0, 15.0, 25.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Rectangle rectangle = (Rectangle)shape_get_shape(shape);

  // Act: Destroy the rectangle (should not crash)
  rectangle_destroy(rectangle);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for rectangle_create
  test_print_section("Testing rectangle_create()");
  test_register("test_rectangle_create_basic", test_rectangle_create_basic);
  test_register("test_rectangle_create_zero_dimensions",
                test_rectangle_create_zero_dimensions);
  test_register("test_rectangle_create_negative_coords",
                test_rectangle_create_negative_coords);

  // Register tests for rectangle getters
  test_print_section("Testing rectangle getter functions");
  test_register("test_rectangle_get_id", test_rectangle_get_id);
  test_register("test_rectangle_get_x", test_rectangle_get_x);
  test_register("test_rectangle_get_y", test_rectangle_get_y);
  test_register("test_rectangle_get_width", test_rectangle_get_width);
  test_register("test_rectangle_get_height", test_rectangle_get_height);
  test_register("test_rectangle_get_border_color",
                test_rectangle_get_border_color);
  test_register("test_rectangle_get_fill_color", test_rectangle_get_fill_color);

  // Register tests for rectangle_destroy
  test_print_section("Testing rectangle_destroy()");
  test_register("test_rectangle_destroy_null", test_rectangle_destroy_null);
  test_register("test_rectangle_destroy_cleanup",
                test_rectangle_destroy_cleanup);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
