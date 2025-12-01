/**
 * @file shapes.spec.c
 * @brief Unit tests for shapes wrapper module
 *
 * Comprehensive unit tests for the unified Shape wrapper functions defined in
 * shapes.h
 */

#include "shapes.h"
#include "../test_framework/test_framework.h"

#include <string.h>

// ============================================================================
// Tests for shape_create_circle()
// ============================================================================

/**
 * Test: shape_create_circle should return a valid Shape
 */
bool test_shape_create_circle_basic(void) {
  // Act: Create a circle shape
  Shape shape = shape_create_circle(1, 10.0, 20.0, 5.0, "red", "blue");

  // Assert: Shape should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: shape_create_circle should have correct type
 */
bool test_shape_create_circle_type(void) {
  // Arrange: Create a circle shape
  Shape shape = shape_create_circle(1, 10.0, 20.0, 5.0, "red", "blue");
  ASSERT_NOT_NULL(shape);

  // Act: Get the shape type
  ShapeType type = shape_get_type(shape);

  // Assert: Should be CIRCLE type
  ASSERT_EQUAL(type, CIRCLE);

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for shape_create_rectangle()
// ============================================================================

/**
 * Test: shape_create_rectangle should return a valid Shape
 */
bool test_shape_create_rectangle_basic(void) {
  // Act: Create a rectangle shape
  Shape shape =
      shape_create_rectangle(1, 10.0, 20.0, 15.0, 25.0, "red", "blue");

  // Assert: Shape should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: shape_create_rectangle should have correct type
 */
bool test_shape_create_rectangle_type(void) {
  // Arrange: Create a rectangle shape
  Shape shape =
      shape_create_rectangle(1, 10.0, 20.0, 15.0, 25.0, "red", "blue");
  ASSERT_NOT_NULL(shape);

  // Act: Get the shape type
  ShapeType type = shape_get_type(shape);

  // Assert: Should be RECTANGLE type
  ASSERT_EQUAL(type, RECTANGLE);

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for shape_create_line()
// ============================================================================

/**
 * Test: shape_create_line should return a valid Shape
 */
bool test_shape_create_line_basic(void) {
  // Act: Create a line shape
  Shape shape = shape_create_line(1, 10.0, 20.0, 30.0, 40.0, "red");

  // Assert: Shape should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: shape_create_line should have correct type
 */
bool test_shape_create_line_type(void) {
  // Arrange: Create a line shape
  Shape shape = shape_create_line(1, 10.0, 20.0, 30.0, 40.0, "red");
  ASSERT_NOT_NULL(shape);

  // Act: Get the shape type
  ShapeType type = shape_get_type(shape);

  // Assert: Should be LINE type
  ASSERT_EQUAL(type, LINE);

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for shape_create_text()
// ============================================================================

/**
 * Test: shape_create_text should return a valid Shape
 */
bool test_shape_create_text_basic(void) {
  // Act: Create a text shape
  Shape shape =
      shape_create_text(1, 10.0, 20.0, "red", "blue", 'n', "Hello World");

  // Assert: Shape should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: shape_create_text should have correct type
 */
bool test_shape_create_text_type(void) {
  // Arrange: Create a text shape
  Shape shape =
      shape_create_text(1, 10.0, 20.0, "red", "blue", 'n', "Hello World");
  ASSERT_NOT_NULL(shape);

  // Act: Get the shape type
  ShapeType type = shape_get_type(shape);

  // Assert: Should be TEXT type
  ASSERT_EQUAL(type, TEXT);

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for shape_create_text_style()
// ============================================================================

/**
 * Test: shape_create_text_style should return a valid Shape
 */
bool test_shape_create_text_style_basic(void) {
  // Act: Create a text style shape
  Shape shape = shape_create_text_style("Arial", 'n', 12);

  // Assert: Shape should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: shape_create_text_style should have correct type
 */
bool test_shape_create_text_style_type(void) {
  // Arrange: Create a text style shape
  Shape shape = shape_create_text_style("Arial", 'n', 12);
  ASSERT_NOT_NULL(shape);

  // Act: Get the shape type
  ShapeType type = shape_get_type(shape);

  // Assert: Should be TEXT_STYLE type
  ASSERT_EQUAL(type, TEXT_STYLE);

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for shape_get_type()
// ============================================================================

/**
 * Test: shape_get_type should correctly identify all shape types
 */
bool test_shape_get_type_all_types(void) {
  // Arrange: Create one of each shape type
  Shape circle = shape_create_circle(1, 10.0, 20.0, 5.0, "red", "blue");
  Shape rectangle =
      shape_create_rectangle(2, 10.0, 20.0, 15.0, 25.0, "red", "blue");
  Shape line = shape_create_line(3, 10.0, 20.0, 30.0, 40.0, "red");
  Shape text = shape_create_text(4, 10.0, 20.0, "red", "blue", 'n', "Test");
  Shape text_style = shape_create_text_style("Arial", 'n', 12);

  // Assert: All should be created
  ASSERT_NOT_NULL(circle);
  ASSERT_NOT_NULL(rectangle);
  ASSERT_NOT_NULL(line);
  ASSERT_NOT_NULL(text);
  ASSERT_NOT_NULL(text_style);

  // Assert: Each should have correct type
  ASSERT_EQUAL(shape_get_type(circle), CIRCLE);
  ASSERT_EQUAL(shape_get_type(rectangle), RECTANGLE);
  ASSERT_EQUAL(shape_get_type(line), LINE);
  ASSERT_EQUAL(shape_get_type(text), TEXT);
  ASSERT_EQUAL(shape_get_type(text_style), TEXT_STYLE);

  // Cleanup
  shape_destroy(circle);
  shape_destroy(rectangle);
  shape_destroy(line);
  shape_destroy(text);
  shape_destroy(text_style);

  return true;
}

// ============================================================================
// Tests for shape_get_shape()
// ============================================================================

/**
 * Test: shape_get_shape should return underlying shape
 */
bool test_shape_get_shape_basic(void) {
  // Arrange: Create a circle shape
  Shape shape = shape_create_circle(1, 10.0, 20.0, 5.0, "red", "blue");
  ASSERT_NOT_NULL(shape);

  // Act: Get the underlying shape
  void *underlying = shape_get_shape(shape);

  // Assert: Should not be NULL
  ASSERT_NOT_NULL(underlying);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: shape_get_shape should return correct underlying shapes for all types
 */
bool test_shape_get_shape_all_types(void) {
  // Arrange: Create one of each shape type
  Shape circle = shape_create_circle(1, 10.0, 20.0, 5.0, "red", "blue");
  Shape rectangle =
      shape_create_rectangle(2, 10.0, 20.0, 15.0, 25.0, "red", "blue");
  Shape line = shape_create_line(3, 10.0, 20.0, 30.0, 40.0, "red");
  Shape text = shape_create_text(4, 10.0, 20.0, "red", "blue", 'n', "Test");
  Shape text_style = shape_create_text_style("Arial", 'n', 12);

  // Act: Get underlying shapes
  void *circle_shape = shape_get_shape(circle);
  void *rectangle_shape = shape_get_shape(rectangle);
  void *line_shape = shape_get_shape(line);
  void *text_shape = shape_get_shape(text);
  void *text_style_shape = shape_get_shape(text_style);

  // Assert: All should not be NULL
  ASSERT_NOT_NULL(circle_shape);
  ASSERT_NOT_NULL(rectangle_shape);
  ASSERT_NOT_NULL(line_shape);
  ASSERT_NOT_NULL(text_shape);
  ASSERT_NOT_NULL(text_style_shape);

  // Cleanup
  shape_destroy(circle);
  shape_destroy(rectangle);
  shape_destroy(line);
  shape_destroy(text);
  shape_destroy(text_style);

  return true;
}

// ============================================================================
// Tests for shape_destroy()
// ============================================================================

/**
 * Test: shape_destroy should handle NULL gracefully
 */
bool test_shape_destroy_null(void) {
  // Act & Assert: Should not crash
  shape_destroy(NULL);

  return true;
}

/**
 * Test: shape_destroy should free all resources for all shape types
 */
bool test_shape_destroy_all_types(void) {
  // Arrange: Create one of each shape type
  Shape circle = shape_create_circle(1, 10.0, 20.0, 5.0, "red", "blue");
  Shape rectangle =
      shape_create_rectangle(2, 10.0, 20.0, 15.0, 25.0, "red", "blue");
  Shape line = shape_create_line(3, 10.0, 20.0, 30.0, 40.0, "red");
  Shape text = shape_create_text(4, 10.0, 20.0, "red", "blue", 'n', "Test");
  Shape text_style = shape_create_text_style("Arial", 'n', 12);

  // Act: Destroy all shapes (should not crash)
  shape_destroy(circle);
  shape_destroy(rectangle);
  shape_destroy(line);
  shape_destroy(text);
  shape_destroy(text_style);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for shape_create_circle
  test_print_section("Testing shape_create_circle()");
  test_register("test_shape_create_circle_basic",
                test_shape_create_circle_basic);
  test_register("test_shape_create_circle_type", test_shape_create_circle_type);

  // Register tests for shape_create_rectangle
  test_print_section("Testing shape_create_rectangle()");
  test_register("test_shape_create_rectangle_basic",
                test_shape_create_rectangle_basic);
  test_register("test_shape_create_rectangle_type",
                test_shape_create_rectangle_type);

  // Register tests for shape_create_line
  test_print_section("Testing shape_create_line()");
  test_register("test_shape_create_line_basic", test_shape_create_line_basic);
  test_register("test_shape_create_line_type", test_shape_create_line_type);

  // Register tests for shape_create_text
  test_print_section("Testing shape_create_text()");
  test_register("test_shape_create_text_basic", test_shape_create_text_basic);
  test_register("test_shape_create_text_type", test_shape_create_text_type);

  // Register tests for shape_create_text_style
  test_print_section("Testing shape_create_text_style()");
  test_register("test_shape_create_text_style_basic",
                test_shape_create_text_style_basic);
  test_register("test_shape_create_text_style_type",
                test_shape_create_text_style_type);

  // Register tests for shape_get_type
  test_print_section("Testing shape_get_type()");
  test_register("test_shape_get_type_all_types", test_shape_get_type_all_types);

  // Register tests for shape_get_shape
  test_print_section("Testing shape_get_shape()");
  test_register("test_shape_get_shape_basic", test_shape_get_shape_basic);
  test_register("test_shape_get_shape_all_types",
                test_shape_get_shape_all_types);

  // Register tests for shape_destroy
  test_print_section("Testing shape_destroy()");
  test_register("test_shape_destroy_null", test_shape_destroy_null);
  test_register("test_shape_destroy_all_types", test_shape_destroy_all_types);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
