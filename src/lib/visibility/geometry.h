/**
 * @file geometry.h
 * @brief Geometry utility functions for visibility calculations
 *
 * This module provides geometric primitives and calculations needed for
 * the visibility algorithm, including point operations, angle calculations,
 * ray-segment intersections, and point-in-polygon tests.
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "../shapes/line/line.h"
#include <stdbool.h>

/**
 * @brief Opaque pointer type for Point instances
 */
typedef void *Point;

/**
 * @brief Creates a new point with the given coordinates
 * @param x X coordinate
 * @param y Y coordinate
 * @return Pointer to new Point or NULL on error
 */
Point geometry_point_create(double x, double y);

/**
 * @brief Destroys a point instance and frees memory
 * @param point Point instance to destroy
 */
void geometry_point_destroy(Point point);

/**
 * @brief Gets the X coordinate of a point
 * @param point Point instance
 * @return X coordinate
 */
double geometry_point_get_x(Point point);

/**
 * @brief Gets the Y coordinate of a point
 * @param point Point instance
 * @return Y coordinate
 */
double geometry_point_get_y(Point point);

/**
 * @brief Calculates the polar angle from source point (px, py) to target point
 * (x, y)
 * @param x Target point X coordinate
 * @param y Target point Y coordinate
 * @param px Source point X coordinate
 * @param py Source point Y coordinate
 * @return Angle in radians [-PI, PI]
 */
double geometry_calculate_angle(double x, double y, double px, double py);

/**
 * @brief Calculates the Euclidean distance between two points
 * @param x1 First point X coordinate
 * @param y1 First point Y coordinate
 * @param x2 Second point X coordinate
 * @param y2 Second point Y coordinate
 * @return Distance between the points
 */
double geometry_distance(double x1, double y1, double x2, double y2);

/**
 * @brief Checks if a ray from point (x, y) at given angle intersects a line
 * segment
 * @param segment Line segment to test (opaque Line type)
 * @param x Ray origin X coordinate
 * @param y Ray origin Y coordinate
 * @param angle Ray direction in radians
 * @param distance Output parameter for intersection distance (can be NULL)
 * @return true if ray intersects segment, false otherwise
 */
bool geometry_segment_intersects_ray(Line segment, double x, double y,
                                     double angle, double *distance);

/**
 * @brief Determines if a point is inside a polygon using ray casting
 * @param x Point X coordinate
 * @param y Point Y coordinate
 * @param vertices Array of Point instances forming the polygon
 * @param vertex_count Number of vertices in the polygon
 * @return true if point is inside polygon, false otherwise
 */
bool geometry_point_in_polygon(double x, double y, Point *vertices,
                               int vertex_count);

/**
 * @brief Compares two angles, handling wraparound at -PI/PI boundary
 * @param angle1 First angle in radians
 * @param angle2 Second angle in radians
 * @return Negative if angle1 < angle2, 0 if equal, positive if angle1 > angle2
 */
int geometry_compare_angles(double angle1, double angle2);

/**
 * @brief Calculates the cross product of vectors (p1->p2) and (p1->p3)
 * @param x1 Point p1 X coordinate
 * @param y1 Point p1 Y coordinate
 * @param x2 Point p2 X coordinate
 * @param y2 Point p2 Y coordinate
 * @param x3 Point p3 X coordinate
 * @param y3 Point p3 Y coordinate
 * @return Cross product value (positive = counterclockwise, negative =
 * clockwise, 0 = collinear)
 */
double geometry_cross_product(double x1, double y1, double x2, double y2,
                              double x3, double y3);

/**
 * @brief Checks if two line segments intersect
 * @param x1 First segment start X
 * @param y1 First segment start Y
 * @param x2 First segment end X
 * @param y2 First segment end Y
 * @param x3 Second segment start X
 * @param y3 Second segment start Y
 * @param x4 Second segment end X
 * @param y4 Second segment end Y
 * @return true if segments intersect, false otherwise
 */
bool geometry_segment_intersects_segment(double x1, double y1, double x2,
                                         double y2, double x3, double y3,
                                         double x4, double y4);

/**
 * @brief Calculates the shortest distance from a point to a line segment
 * @param px Point X coordinate
 * @param py Point Y coordinate
 * @param x1 Segment start X
 * @param y1 Segment start Y
 * @param x2 Segment end X
 * @param y2 Segment end Y
 * @return Shortest distance from point to segment
 */
double geometry_distance_point_segment(double px, double py, double x1,
                                       double y1, double x2, double y2);

#endif // GEOMETRY_H
