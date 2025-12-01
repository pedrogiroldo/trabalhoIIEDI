/**
 * @file circle.spec.c
 * @brief Unit tests for circle module
 *
 * Comprehensive unit tests for the circle ADT functions defined in circle.h
 */

#include "circle.h"
#include "../../test_framework/test_framework.h"

#include <string.h>

// ============================================================================
// Tests for circle_create()
// ============================================================================

/**
 * Test: circle_create should return a valid Shape instance
 */
bool test_circle_create_basic(void) {
  // Act: Create a circle
  Shape shape = circle_create(1, 10.5, 20.3, 5.0, "red", "blue");

  // Assert: Shape should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: circle_create should handle zero radius
 */
bool test_circle_create_zero_radius(void) {
  // Act: Create a circle with zero radius
  Shape shape = circle_create(2, 0.0, 0.0, 0.0, "black", "white");

  // Assert: Should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: circle_create should handle negative coordinates
 */
bool test_circle_create_negative_coords(void) {
  // Act: Create a circle with negative coordinates
  Shape shape = circle_create(3, -10.5, -20.3, 5.0, "green", "yellow");

  // Assert: Should be created successfully
  ASSERT_NOT_NULL(shape);

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for circle getter functions
// ============================================================================

/**
 * Test: circle_get_id should return correct ID
 */
bool test_circle_get_id(void) {
  // Arrange: Create a circle
  Shape shape = circle_create(42, 10.0, 20.0, 5.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Circle circle = (Circle)shape_get_shape(shape);

  // Act: Get the ID
  int id = circle_get_id(circle);

  // Assert: Should return correct ID
  ASSERT_EQUAL(id, 42);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: circle_get_x should return correct X coordinate
 */
bool test_circle_get_x(void) {
  // Arrange: Create a circle
  Shape shape = circle_create(1, 15.75, 20.0, 5.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Circle circle = (Circle)shape_get_shape(shape);

  // Act: Get the X coordinate
  double x = circle_get_x(circle);

  // Assert: Should return correct X
  ASSERT_TRUE(x == 15.75);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: circle_get_y should return correct Y coordinate
 */
bool test_circle_get_y(void) {
  // Arrange: Create a circle
  Shape shape = circle_create(1, 10.0, 25.5, 5.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Circle circle = (Circle)shape_get_shape(shape);

  // Act: Get the Y coordinate
  double y = circle_get_y(circle);

  // Assert: Should return correct Y
  ASSERT_TRUE(y == 25.5);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: circle_get_radius should return correct radius
 */
bool test_circle_get_radius(void) {
  // Arrange: Create a circle
  Shape shape = circle_create(1, 10.0, 20.0, 7.25, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Circle circle = (Circle)shape_get_shape(shape);

  // Act: Get the radius
  double radius = circle_get_radius(circle);

  // Assert: Should return correct radius
  ASSERT_TRUE(radius == 7.25);

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: circle_get_border_color should return correct color
 */
bool test_circle_get_border_color(void) {
  // Arrange: Create a circle
  Shape shape = circle_create(1, 10.0, 20.0, 5.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Circle circle = (Circle)shape_get_shape(shape);

  // Act: Get the border color
  const char *color = circle_get_border_color(circle);

  // Assert: Should return correct color
  ASSERT_NOT_NULL(color);
  ASSERT_STR_EQUAL(color, "red");

  // Cleanup
  shape_destroy(shape);

  return true;
}

/**
 * Test: circle_get_fill_color should return correct color
 */
bool test_circle_get_fill_color(void) {
  // Arrange: Create a circle
  Shape shape = circle_create(1, 10.0, 20.0, 5.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Circle circle = (Circle)shape_get_shape(shape);

  // Act: Get the fill color
  const char *color = circle_get_fill_color(circle);

  // Assert: Should return correct color
  ASSERT_NOT_NULL(color);
  ASSERT_STR_EQUAL(color, "blue");

  // Cleanup
  shape_destroy(shape);

  return true;
}

// ============================================================================
// Tests for circle_destroy()
// ============================================================================

/**
 * Test: circle_destroy should handle NULL gracefully
 */
bool test_circle_destroy_null(void) {
  // Act & Assert: Should not crash
  circle_destroy(NULL);

  return true;
}

/**
 * Test: circle_destroy should free all resources
 */
bool test_circle_destroy_cleanup(void) {
  // Arrange: Create a circle
  Shape shape = circle_create(1, 10.0, 20.0, 5.0, "red", "blue");
  ASSERT_NOT_NULL(shape);
  Circle circle = (Circle)shape_get_shape(shape);

  // Act: Destroy the circle (should not crash)
  circle_destroy(circle);

  // Note: We don't destroy the shape wrapper here as it's already destroyed

  return true;
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Register tests for circle_create
  test_print_section("Testing circle_create()");
  test_register("test_circle_create_basic", test_circle_create_basic);
  test_register("test_circle_create_zero_radius",
                test_circle_create_zero_radius);
  test_register("test_circle_create_negative_coords",
                test_circle_create_negative_coords);

  // Register tests for circle getters
  test_print_section("Testing circle getter functions");
  test_register("test_circle_get_id", test_circle_get_id);
  test_register("test_circle_get_x", test_circle_get_x);
  test_register("test_circle_get_y", test_circle_get_y);
  test_register("test_circle_get_radius", test_circle_get_radius);
  test_register("test_circle_get_border_color", test_circle_get_border_color);
  test_register("test_circle_get_fill_color", test_circle_get_fill_color);

  // Register tests for circle_destroy
  test_print_section("Testing circle_destroy()");
  test_register("test_circle_destroy_null", test_circle_destroy_null);
  test_register("test_circle_destroy_cleanup", test_circle_destroy_cleanup);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
