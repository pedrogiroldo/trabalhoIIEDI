#include "visibility.h"
#include "../commons/list/list.h"
#include "../shapes/line/line.h"
#include "../shapes/shapes.h"
#include "../test_framework/test_framework.h"
#include "geometry.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EPSILON 0.0001

// Helper function to compare doubles
static bool doubles_equal(double a, double b) { return fabs(a - b) < EPSILON; }

// ============================================================================
// Geometry Module Tests
// ============================================================================

bool test_geometry_point_create_and_destroy(void) {
  Point p = geometry_point_create(10.5, 20.3);
  ASSERT_NOT_NULL(p);
  ASSERT_TRUE(doubles_equal(10.5, geometry_point_get_x(p)));
  ASSERT_TRUE(doubles_equal(20.3, geometry_point_get_y(p)));
  geometry_point_destroy(p);
  return true;
}

bool test_geometry_calculate_angle(void) {
  // Angle from (0,0) to (1,0) should be 0
  double angle = geometry_calculate_angle(1.0, 0.0, 0.0, 0.0);
  ASSERT_TRUE(doubles_equal(0.0, angle));

  // Angle from (0,0) to (0,1) should be PI/2
  angle = geometry_calculate_angle(0.0, 1.0, 0.0, 0.0);
  ASSERT_TRUE(doubles_equal(M_PI / 2.0, angle));

  // Angle from (0,0) to (-1,0) should be PI or -PI
  angle = geometry_calculate_angle(-1.0, 0.0, 0.0, 0.0);
  ASSERT_TRUE(doubles_equal(M_PI, fabs(angle)));

  return true;
}

bool test_geometry_distance(void) {
  double dist = geometry_distance(0.0, 0.0, 3.0, 4.0);
  ASSERT_TRUE(doubles_equal(5.0, dist));

  dist = geometry_distance(1.0, 1.0, 1.0, 1.0);
  ASSERT_TRUE(doubles_equal(0.0, dist));

  dist = geometry_distance(0.0, 0.0, 1.0, 0.0);
  ASSERT_TRUE(doubles_equal(1.0, dist));

  return true;
}

bool test_geometry_segment_intersects_ray(void) {
  // Create a horizontal segment from (0,5) to (10,5)
  Shape line_shape = line_create(1, 0.0, 5.0, 10.0, 5.0, "black");
  ASSERT_NOT_NULL(line_shape);

  Line segment = (Line)shape_get_shape(line_shape);
  line_set_barrier(segment, true);

  // Ray from (5,0) pointing up (angle = PI/2) should intersect at distance 5
  double distance;
  bool intersects =
      geometry_segment_intersects_ray(segment, 5.0, 0.0, M_PI / 2.0, &distance);
  ASSERT_TRUE(intersects);
  ASSERT_TRUE(doubles_equal(5.0, distance));

  // Ray from (5,0) pointing down should not intersect
  intersects =
      geometry_segment_intersects_ray(segment, 5.0, 0.0, -M_PI / 2.0, NULL);
  ASSERT_FALSE(intersects);

  // Ray from (15,0) pointing up should not intersect (segment is at x=[0,10])
  intersects =
      geometry_segment_intersects_ray(segment, 15.0, 0.0, M_PI / 2.0, NULL);
  ASSERT_FALSE(intersects);

  shape_destroy(line_shape);
  return true;
}

bool test_geometry_point_in_polygon(void) {
  // Create a square polygon: (0,0), (10,0), (10,10), (0,10)
  Point vertices[4];
  vertices[0] = geometry_point_create(0.0, 0.0);
  vertices[1] = geometry_point_create(10.0, 0.0);
  vertices[2] = geometry_point_create(10.0, 10.0);
  vertices[3] = geometry_point_create(0.0, 10.0);

  // Point inside
  bool inside = geometry_point_in_polygon(5.0, 5.0, vertices, 4);
  ASSERT_TRUE(inside);

  // Point outside
  inside = geometry_point_in_polygon(15.0, 5.0, vertices, 4);
  ASSERT_FALSE(inside);

  for (int i = 0; i < 4; i++) {
    geometry_point_destroy(vertices[i]);
  }

  return true;
}

bool test_geometry_cross_product(void) {
  // Three points forming a right turn: (0,0), (1,0), (1,1)
  double cross = geometry_cross_product(0.0, 0.0, 1.0, 0.0, 1.0, 1.0);
  ASSERT_TRUE(cross > 0);

  // Three points forming a left turn: (0,0), (1,0), (1,-1)
  cross = geometry_cross_product(0.0, 0.0, 1.0, 0.0, 1.0, -1.0);
  ASSERT_TRUE(cross < 0);

  // Three collinear points: (0,0), (1,0), (2,0)
  cross = geometry_cross_product(0.0, 0.0, 1.0, 0.0, 2.0, 0.0);
  ASSERT_TRUE(doubles_equal(0.0, cross));

  return true;
}

// ============================================================================
// Visibility Module Tests
// ============================================================================

bool test_visibility_no_barriers(void) {
  List barriers = list_create();
  ASSERT_NOT_NULL(barriers);

  // Calculate visibility from (0,0) with no barriers
  VisibilityPolygon polygon = visibility_calculate(0.0, 0.0, barriers, 100.0);
  ASSERT_NOT_NULL(polygon);

  int vertex_count = visibility_polygon_get_vertex_count(polygon);
  ASSERT_TRUE(vertex_count > 0);

  // With no barriers, should create a circular region
  ASSERT_TRUE(vertex_count >= 32);

  visibility_polygon_destroy(polygon);
  list_destroy(barriers);
  return true;
}

bool test_visibility_single_barrier(void) {
  List barriers = list_create();
  ASSERT_NOT_NULL(barriers);

  // Create a vertical barrier from (5,0) to (5,10)
  Shape line_shape = line_create(1, 5.0, 0.0, 5.0, 10.0, "black");
  ASSERT_NOT_NULL(line_shape);

  Line segment = (Line)shape_get_shape(line_shape);
  line_set_barrier(segment, true);

  list_insert_back(barriers, line_shape);

  // Calculate visibility from (0,5) with one barrier
  VisibilityPolygon polygon = visibility_calculate(0.0, 5.0, barriers, 100.0);
  ASSERT_NOT_NULL(polygon);

  int vertex_count = visibility_polygon_get_vertex_count(polygon);
  ASSERT_TRUE(vertex_count > 0);

  visibility_polygon_destroy(polygon);
  shape_destroy(line_shape);
  list_destroy(barriers);
  return true;
}

bool test_visibility_multiple_barriers(void) {
  List barriers = list_create();
  ASSERT_NOT_NULL(barriers);

  // Create a box of barriers around (50,50)
  Shape line1 = line_create(1, 40.0, 40.0, 60.0, 40.0, "black"); // Bottom
  Shape line2 = line_create(2, 60.0, 40.0, 60.0, 60.0, "black"); // Right
  Shape line3 = line_create(3, 60.0, 60.0, 40.0, 60.0, "black"); // Top
  Shape line4 = line_create(4, 40.0, 60.0, 40.0, 40.0, "black"); // Left

  line_set_barrier((Line)shape_get_shape(line1), true);
  line_set_barrier((Line)shape_get_shape(line2), true);
  line_set_barrier((Line)shape_get_shape(line3), true);
  line_set_barrier((Line)shape_get_shape(line4), true);

  list_insert_back(barriers, line1);
  list_insert_back(barriers, line2);
  list_insert_back(barriers, line3);
  list_insert_back(barriers, line4);

  // Calculate visibility from (0,0) - outside the box
  VisibilityPolygon polygon = visibility_calculate(0.0, 0.0, barriers, 100.0);
  ASSERT_NOT_NULL(polygon);

  int vertex_count = visibility_polygon_get_vertex_count(polygon);
  ASSERT_TRUE(vertex_count > 0);

  // Point inside the box should not be visible
  bool visible = visibility_polygon_contains_point(polygon, 50.0, 50.0);
  ASSERT_FALSE(visible);

  visibility_polygon_destroy(polygon);
  shape_destroy(line1);
  shape_destroy(line2);
  shape_destroy(line3);
  shape_destroy(line4);
  list_destroy(barriers);
  return true;
}

bool test_visibility_polygon_memory_management(void) {
  List barriers = list_create();
  ASSERT_NOT_NULL(barriers);

  // Create and destroy multiple polygons to test memory management
  for (int i = 0; i < 10; i++) {
    VisibilityPolygon polygon = visibility_calculate(0.0, 0.0, barriers, 50.0);
    ASSERT_NOT_NULL(polygon);
    visibility_polygon_destroy(polygon);
  }

  list_destroy(barriers);
  return true;
}

bool test_visibility_null_inputs(void) {
  // Test with NULL barriers list
  VisibilityPolygon polygon = visibility_calculate(0.0, 0.0, NULL, 100.0);
  ASSERT_NULL(polygon);

  // Test polygon operations with NULL
  ASSERT_NULL(visibility_polygon_get_vertices(NULL));
  ASSERT_EQUAL(visibility_polygon_get_vertex_count(NULL), 0);
  ASSERT_FALSE(visibility_polygon_contains_point(NULL, 0.0, 0.0));

  // Destroying NULL should not crash
  visibility_polygon_destroy(NULL);

  return true;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main(void) {
  // Initialize test framework
  test_framework_init();

  // Geometry tests
  test_print_section("Testing Geometry Module");
  test_register("test_geometry_point_create_and_destroy",
                test_geometry_point_create_and_destroy);
  test_register("test_geometry_calculate_angle", test_geometry_calculate_angle);
  test_register("test_geometry_distance", test_geometry_distance);
  test_register("test_geometry_segment_intersects_ray",
                test_geometry_segment_intersects_ray);
  test_register("test_geometry_point_in_polygon",
                test_geometry_point_in_polygon);
  test_register("test_geometry_cross_product", test_geometry_cross_product);

  // Visibility tests
  test_print_section("Testing Visibility Module");
  test_register("test_visibility_no_barriers", test_visibility_no_barriers);
  test_register("test_visibility_single_barrier",
                test_visibility_single_barrier);
  test_register("test_visibility_multiple_barriers",
                test_visibility_multiple_barriers);
  test_register("test_visibility_polygon_memory_management",
                test_visibility_polygon_memory_management);
  test_register("test_visibility_null_inputs", test_visibility_null_inputs);

  // Run all tests
  int result = test_run_all();

  // Cleanup
  test_framework_cleanup();

  return result;
}
