#include "visibility.h"
#include "../commons/list/list.h"
#include "../shapes/line/line.h"
#include "../shapes/shapes.h"
#include "geometry.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Internal VisibilityPolygon structure
 */
struct VisibilityPolygon {
  Point *vertices;
  int vertex_count;
  int capacity;
};

/**
 * Structure to hold an endpoint with its angle and associated segment
 */
struct Endpoint {
  double x;
  double y;
  double angle;
  Line segment;
  bool is_start; // true if this is the start of the segment
};

/**
 * Comparison function for sorting endpoints by angle
 */
static int compare_endpoints(const void *a, const void *b) {
  const struct Endpoint *ep1 = (const struct Endpoint *)a;
  const struct Endpoint *ep2 = (const struct Endpoint *)b;

  int angle_cmp = geometry_compare_angles(ep1->angle, ep2->angle);
  if (angle_cmp != 0) {
    return angle_cmp;
  }

  // If angles are equal, start points come before end points
  if (ep1->is_start && !ep2->is_start) {
    return -1;
  }
  if (!ep1->is_start && ep2->is_start) {
    return 1;
  }

  return 0;
}

/**
 * Finds the closest segment intersection along a ray at given angle
 */
static double find_closest_intersection(double x, double y, double angle,
                                        List barriers) {
  double min_distance = -1.0;
  int barrier_count = list_size(barriers);

  for (int i = 0; i < barrier_count; i++) {
    Shape shape_wrapper = list_get(barriers, i);
    if (!shape_wrapper) {
      continue;
    }

    // Extract the Line from the Shape wrapper
    Line segment = (Line)shape_get_shape(shape_wrapper);
    if (!segment || !line_is_barrier(segment)) {
      continue;
    }

    double distance;
    if (geometry_segment_intersects_ray(segment, x, y, angle, &distance)) {
      if (min_distance < 0.0 || distance < min_distance) {
        min_distance = distance;
      }
    }
  }

  return min_distance;
}

/**
 * Adds a vertex to the visibility polygon
 */
static bool add_vertex(struct VisibilityPolygon *polygon, double x, double y) {
  if (!polygon) {
    return false;
  }

  // Expand capacity if needed
  if (polygon->vertex_count >= polygon->capacity) {
    int new_capacity = polygon->capacity * 2;
    if (new_capacity < 8) {
      new_capacity = 8;
    }

    Point *new_vertices =
        realloc(polygon->vertices, new_capacity * sizeof(Point));
    if (!new_vertices) {
      return false;
    }

    polygon->vertices = new_vertices;
    polygon->capacity = new_capacity;
  }

  // Create and add the new point
  Point point = geometry_point_create(x, y);
  if (!point) {
    return false;
  }

  polygon->vertices[polygon->vertex_count++] = point;
  return true;
}

VisibilityPolygon visibility_calculate(double x, double y, List barriers,
                                       double max_radius) {
  if (!barriers) {
    return NULL;
  }

  // Create the visibility polygon structure
  struct VisibilityPolygon *polygon = malloc(sizeof(struct VisibilityPolygon));
  if (!polygon) {
    return NULL;
  }

  polygon->vertices = NULL;
  polygon->vertex_count = 0;
  polygon->capacity = 0;

  int barrier_count = list_size(barriers);

  // If no barriers, create a circular visibility region
  if (barrier_count == 0) {
    // Create a circle approximation with 32 points
    int num_points = 32;
    for (int i = 0; i < num_points; i++) {
      double angle = 2.0 * M_PI * i / num_points;
      double vx = x + max_radius * cos(angle);
      double vy = y + max_radius * sin(angle);

      if (!add_vertex(polygon, vx, vy)) {
        visibility_polygon_destroy(polygon);
        return NULL;
      }
    }

    return (VisibilityPolygon)polygon;
  }

  // Collect all endpoints from barriers
  struct Endpoint *endpoints =
      malloc(barrier_count * 2 * sizeof(struct Endpoint));
  if (!endpoints) {
    visibility_polygon_destroy(polygon);
    return NULL;
  }

  int endpoint_count = 0;

  for (int i = 0; i < barrier_count; i++) {
    Shape shape_wrapper = list_get(barriers, i);
    if (!shape_wrapper) {
      continue;
    }

    Line segment = (Line)shape_get_shape(shape_wrapper);
    if (!segment || !line_is_barrier(segment)) {
      continue;
    }

    double x1 = line_get_x1(segment);
    double y1 = line_get_y1(segment);
    double x2 = line_get_x2(segment);
    double y2 = line_get_y2(segment);

    // Add start endpoint
    endpoints[endpoint_count].x = x1;
    endpoints[endpoint_count].y = y1;
    endpoints[endpoint_count].angle = geometry_calculate_angle(x1, y1, x, y);
    endpoints[endpoint_count].segment = segment;
    endpoints[endpoint_count].is_start = true;
    endpoint_count++;

    // Add end endpoint
    endpoints[endpoint_count].x = x2;
    endpoints[endpoint_count].y = y2;
    endpoints[endpoint_count].angle = geometry_calculate_angle(x2, y2, x, y);
    endpoints[endpoint_count].segment = segment;
    endpoints[endpoint_count].is_start = false;
    endpoint_count++;
  }

  // Sort endpoints by angle
  qsort(endpoints, endpoint_count, sizeof(struct Endpoint), compare_endpoints);

  // Perform angular sweep
  for (int i = 0; i < endpoint_count; i++) {
    double angle = endpoints[i].angle;

    // Cast a ray at this angle and find the closest intersection
    double distance = find_closest_intersection(x, y, angle, barriers);

    if (distance < 0.0) {
      distance = max_radius;
    }

    // Add the visibility point
    double vx = x + distance * cos(angle);
    double vy = y + distance * sin(angle);

    if (!add_vertex(polygon, vx, vy)) {
      free(endpoints);
      visibility_polygon_destroy(polygon);
      return NULL;
    }

    // Also check slightly before and after this angle to catch edge cases
    double epsilon = 1e-5;

    // Slightly before
    double angle_before = angle - epsilon;
    distance = find_closest_intersection(x, y, angle_before, barriers);
    if (distance < 0.0) {
      distance = max_radius;
    }
    vx = x + distance * cos(angle_before);
    vy = y + distance * sin(angle_before);
    if (!add_vertex(polygon, vx, vy)) {
      free(endpoints);
      visibility_polygon_destroy(polygon);
      return NULL;
    }

    // Slightly after
    double angle_after = angle + epsilon;
    distance = find_closest_intersection(x, y, angle_after, barriers);
    if (distance < 0.0) {
      distance = max_radius;
    }
    vx = x + distance * cos(angle_after);
    vy = y + distance * sin(angle_after);
    if (!add_vertex(polygon, vx, vy)) {
      free(endpoints);
      visibility_polygon_destroy(polygon);
      return NULL;
    }
  }

  free(endpoints);
  return (VisibilityPolygon)polygon;
}

void visibility_polygon_destroy(VisibilityPolygon polygon) {
  if (!polygon) {
    return;
  }

  struct VisibilityPolygon *poly = (struct VisibilityPolygon *)polygon;

  // Free all vertices
  if (poly->vertices) {
    for (int i = 0; i < poly->vertex_count; i++) {
      geometry_point_destroy(poly->vertices[i]);
    }
    free(poly->vertices);
  }

  free(poly);
}

Point *visibility_polygon_get_vertices(VisibilityPolygon polygon) {
  if (!polygon) {
    return NULL;
  }

  struct VisibilityPolygon *poly = (struct VisibilityPolygon *)polygon;
  return poly->vertices;
}

int visibility_polygon_get_vertex_count(VisibilityPolygon polygon) {
  if (!polygon) {
    return 0;
  }

  struct VisibilityPolygon *poly = (struct VisibilityPolygon *)polygon;
  return poly->vertex_count;
}

bool visibility_polygon_contains_point(VisibilityPolygon polygon, double x,
                                       double y) {
  if (!polygon) {
    return false;
  }

  struct VisibilityPolygon *poly = (struct VisibilityPolygon *)polygon;
  return geometry_point_in_polygon(x, y, poly->vertices, poly->vertex_count);
}
