#include "geometry.h"
#include "../shapes/line/line.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Internal Point structure
 */
struct Point {
  double x;
  double y;
};

Point geometry_point_create(double x, double y) {
  struct Point *point = malloc(sizeof(struct Point));
  if (!point) {
    return NULL;
  }

  point->x = x;
  point->y = y;

  return (Point)point;
}

void geometry_point_destroy(Point point) {
  if (!point) {
    return;
  }
  free((struct Point *)point);
}

double geometry_point_get_x(Point point) {
  if (!point) {
    return 0.0;
  }
  return ((struct Point *)point)->x;
}

double geometry_point_get_y(Point point) {
  if (!point) {
    return 0.0;
  }
  return ((struct Point *)point)->y;
}

double geometry_calculate_angle(double x, double y, double px, double py) {
  double dx = x - px;
  double dy = y - py;
  return atan2(dy, dx);
}

double geometry_distance(double x1, double y1, double x2, double y2) {
  double dx = x2 - x1;
  double dy = y2 - y1;
  return sqrt(dx * dx + dy * dy);
}

bool geometry_segment_intersects_ray(Line segment, double x, double y,
                                     double angle, double *distance) {
  if (!segment) {
    return false;
  }

  // Get segment endpoints
  double x1 = line_get_x1(segment);
  double y1 = line_get_y1(segment);
  double x2 = line_get_x2(segment);
  double y2 = line_get_y2(segment);

  // Ray direction vector
  double ray_dx = cos(angle);
  double ray_dy = sin(angle);

  // Segment direction vector
  double seg_dx = x2 - x1;
  double seg_dy = y2 - y1;

  // Calculate denominator for parametric intersection
  double denominator = ray_dx * seg_dy - ray_dy * seg_dx;

  // Check if ray and segment are parallel
  if (fabs(denominator) < 1e-10) {
    return false;
  }

  // Calculate parameters for intersection
  double t = ((x1 - x) * seg_dy - (y1 - y) * seg_dx) / denominator;
  double u = ((x1 - x) * ray_dy - (y1 - y) * ray_dx) / denominator;

  // Check if intersection is valid:
  // t >= 0: intersection is in the positive ray direction
  // 0 <= u <= 1: intersection is within the segment
  if (t >= 0.0 && u >= 0.0 && u <= 1.0) {
    if (distance != NULL) {
      *distance = t;
    }
    return true;
  }

  return false;
}

bool geometry_point_in_polygon(double x, double y, Point *vertices,
                               int vertex_count) {
  if (!vertices || vertex_count < 3) {
    return false;
  }

  // Ray casting algorithm: cast a ray from the point to infinity
  // and count how many times it crosses polygon edges
  int crossings = 0;

  for (int i = 0; i < vertex_count; i++) {
    int j = (i + 1) % vertex_count;

    double xi = geometry_point_get_x(vertices[i]);
    double yi = geometry_point_get_y(vertices[i]);
    double xj = geometry_point_get_x(vertices[j]);
    double yj = geometry_point_get_y(vertices[j]);

    // Check if edge crosses the horizontal ray from point (x, y) to the right
    if (((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi)) {
      crossings++;
    }
  }

  // Point is inside if number of crossings is odd
  return (crossings % 2) == 1;
}

int geometry_compare_angles(double angle1, double angle2) {
  // Normalize angles to [-PI, PI]
  while (angle1 > M_PI)
    angle1 -= 2.0 * M_PI;
  while (angle1 < -M_PI)
    angle1 += 2.0 * M_PI;
  while (angle2 > M_PI)
    angle2 -= 2.0 * M_PI;
  while (angle2 < -M_PI)
    angle2 += 2.0 * M_PI;

  double diff = angle1 - angle2;

  if (fabs(diff) < 1e-10) {
    return 0;
  }

  return (diff < 0) ? -1 : 1;
}

double geometry_cross_product(double x1, double y1, double x2, double y2,
                              double x3, double y3) {
  // Vectors: v1 = (x2-x1, y2-y1), v2 = (x3-x1, y3-y1)
  // Cross product: v1.x * v2.y - v1.y * v2.x
  double v1x = x2 - x1;
  double v1y = y2 - y1;
  double v2x = x3 - x1;
  double v2y = y3 - y1;

  return v1x * v2y - v1y * v2x;
}

bool geometry_segment_intersects_segment(double x1, double y1, double x2,
                                         double y2, double x3, double y3,
                                         double x4, double y4) {
  // Using cross product to check orientation
  double cp1 = geometry_cross_product(x1, y1, x2, y2, x3, y3);
  double cp2 = geometry_cross_product(x1, y1, x2, y2, x4, y4);
  double cp3 = geometry_cross_product(x3, y3, x4, y4, x1, y1);
  double cp4 = geometry_cross_product(x3, y3, x4, y4, x2, y2);

  if (((cp1 > 0 && cp2 < 0) || (cp1 < 0 && cp2 > 0)) &&
      ((cp3 > 0 && cp4 < 0) || (cp3 < 0 && cp4 > 0))) {
    return true;
  }

  // Note: This simplified check handles strict intersection.
  // Collinear or endpoint cases might need more checks if strictness is
  // required.
  return false;
}

double geometry_distance_point_segment(double px, double py, double x1,
                                       double y1, double x2, double y2) {
  double l2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
  if (l2 == 0.0)
    return geometry_distance(px, py, x1, y1);

  double t = ((px - x1) * (x2 - x1) + (py - y1) * (y2 - y1)) / l2;

  if (t < 0.0)
    return geometry_distance(px, py, x1, y1);
  if (t > 1.0)
    return geometry_distance(px, py, x2, y2);

  double proj_x = x1 + t * (x2 - x1);
  double proj_y = y1 + t * (y2 - y1);

  return geometry_distance(px, py, proj_x, proj_y);
}
