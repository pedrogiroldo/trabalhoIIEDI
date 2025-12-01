/**
 * @file line.spec.c
 * @brief Unit tests for line module
 *
 * Comprehensive unit tests for the line ADT functions defined in line.h
 */

#include "line.h"
#include "../../test_framework/test_framework.h"

#include <string.h>

// ============================================================================
// Tests for line_create()
// ============================================================================

/**
 * Test: line_create should return a valid Shape instance
 */
bool test_line_create_basic(void) {
  // Act: Create a line
  Shape shape = line_create(1, 10.5, 20.3, 30.5, 40.3, "red");

  // Assert: Shape should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: line_create should handle zero-length line
 */
bool test_line_create_zero_length(void) {
  // Act: Create a line with same start and end points
  Shape shape = line_create(2, 10.0, 20.0, 10.0, 20.0, "black");

  // Assert: Should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: line_create should handle negative coordinates
 */
bool test_line_create_negative_coords(void) {
  // Act: Create a line with negative coordinates
  Shape shape = line_create(3, -10.5, -20.3, -30.5, -40.3, "green");

  // Assert: Should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for line getter functions
// ============================================================================

/**
 * Test: line_get_id should return correct ID
 */
bool test_line_get_id(void) {
  // Arrange: Create a line
  Shape shape = line_create(42, 10.0, 20.0, 30.0, 40.0, "red");
  ASSERT_NOT_NULL(shape);
  Line line = (Line)shape_get_shape(shape);

  // Act: Get the ID
  int id = line_get_id(line);

  // Assert: Should return correct ID
  ASSERT_EQUAL(id, 42);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: line_get_x1 should return correct X1 coordinate
 */
bool test_line_get_x1(void) {
  // Arrange: Create a line
  Shape shape = line_create(1, 15.75, 20.0, 30.0, 40.0, "red");
  ASSERT_NOT_NULL(shape);
  Line line = (Line)shape_get_shape(shape);

  // Act: Get the X1 coordinate
  double x1 = line_get_x1(line);

  // Assert: Should return correct X1
  ASSERT_TRUE(x1 == 15.75);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: line_get_y1 should return correct Y1 coordinate
 */
bool test_line_get_y1(void) {
  // Arrange: Create a line
  Shape shape = line_create(1, 10.0, 25.5, 30.0, 40.0, "red");
  ASSERT_NOT_NULL(shape);
  Line line = (Line)shape_get_shape(shape);

  // Act: Get the Y1 coordinate
  double y1 = line_get_y1(line);

  // Assert: Should return correct Y1
  ASSERT_TRUE(y1 == 25.5);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: line_get_x2 should return correct X2 coordinate
 */
bool test_line_get_x2(void) {
  // Arrange: Create a line
  Shape shape = line_create(1, 10.0, 20.0, 35.75, 40.0, "red");
  ASSERT_NOT_NULL(shape);
  Line line = (Line)shape_get_shape(shape);

  // Act: Get the X2 coordinate
  double x2 = line_get_x2(line);

  // Assert: Should return correct X2
  ASSERT_TRUE(x2 == 35.75);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: line_get_y2 should return correct Y2 coordinate
 */
bool test_line_get_y2(void) {
  // Arrange: Create a line
  Shape shape = line_create(1, 10.0, 20.0, 30.0, 45.5, "red");
  ASSERT_NOT_NULL(shape);
  Line line = (Line)shape_get_shape(shape);

  // Act: Get the Y2 coordinate
  double y2 = line_get_y2(line);

  // Assert: Should return correct Y2
  ASSERT_TRUE(y2 == 45.5);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: line_get_color should return correct color
 */
bool test_line_get_color(void) {
  // Arrange: Create a line
  Shape shape = line_create(1, 10.0, 20.0, 30.0, 40.0, "blue");
  ASSERT_NOT_NULL(shape);
  Line line = (Line)shape_get_shape(shape);

  // Act: Get the color
  const char *color = line_get_color(line);

  // Assert: Should return correct color
  ASSERT_NOT_NULL(color);
  ASSERT_STR_EQUAL(color, "blue");

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for line_destroy()
// ============================================================================

/**
 * Test: line_destroy should handle NULL gracefully
 */
bool test_line_destroy_null(void) {
  // Act & Assert: Should not crash
  line_destroy(NULL);

  return true;
}

/**
 * Test: line_destroy should free all resources
 */
bool test_line_destroy_cleanup(void) {
  // Arrange: Create a line
  Shape shape = line_create(1, 10.0, 20.0, 30.0, 40.0, "red");
  ASSERT_NOT_NULL(shape);
  Line line = (Line)shape_get_shape(shape);

  // Act: Destroy the line (should not crash)
  line_destroy(line);

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for line_create
  test_print_section("Testing line_create()");
  test_register("test_line_create_basic", test_line_create_basic);
  test_register("test_line_create_zero_length", test_line_create_zero_length);
  test_register("test_line_create_negative_coords",
                test_line_create_negative_coords);

  // Register tests for line getters
  test_print_section("Testing line getter functions");
  test_register("test_line_get_id", test_line_get_id);
  test_register("test_line_get_x1", test_line_get_x1);
  test_register("test_line_get_y1", test_line_get_y1);
  test_register("test_line_get_x2", test_line_get_x2);
  test_register("test_line_get_y2", test_line_get_y2);
  test_register("test_line_get_color", test_line_get_color);

  // Register tests for line_destroy
  test_print_section("Testing line_destroy()");
  test_register("test_line_destroy_null", test_line_destroy_null);
  test_register("test_line_destroy_cleanup", test_line_destroy_cleanup);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
