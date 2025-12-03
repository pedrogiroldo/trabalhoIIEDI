/**
 * @file visibility.h
 * @brief Visibility polygon calculation using angular sweep algorithm
 *
 * This module implements an angular sweep visibility algorithm to calculate
 * the region visible from a point source, considering barrier segments that
 * block visibility. The algorithm is used for bomb explosion effects in the
 * alien invasion simulation.
 */

#ifndef VISIBILITY_H
#define VISIBILITY_H

#include "../commons/list/list.h"
#include "geometry.h"
#include <stdbool.h>

/**
 * @brief Opaque pointer type for VisibilityPolygon instances
 */
typedef void *VisibilityPolygon;

/**
 * @brief Calculates the visibility polygon from a point source
 *
 * Uses an angular sweep algorithm to determine which regions are visible
 * from the source point (x, y), taking into account barrier segments that
 * block visibility.
 *
 * @param x Source point X coordinate
 * @param y Source point Y coordinate
 * @param barriers List of Line instances marked as barriers (is_barrier = true)
 * @param max_radius Maximum visibility radius (use large value for unbounded)
 * @return Pointer to VisibilityPolygon or NULL on error
 */
VisibilityPolygon visibility_calculate(double x, double y, List barriers,
                                       double max_radius);

/**
 * @brief Destroys a visibility polygon and frees all memory
 * @param polygon VisibilityPolygon instance to destroy
 */
void visibility_polygon_destroy(VisibilityPolygon polygon);

/**
 * @brief Gets the array of vertices forming the visibility polygon
 * @param polygon VisibilityPolygon instance
 * @return Array of Point instances (do not free individually)
 */
Point *visibility_polygon_get_vertices(VisibilityPolygon polygon);

/**
 * @brief Gets the number of vertices in the visibility polygon
 * @param polygon VisibilityPolygon instance
 * @return Number of vertices
 */
int visibility_polygon_get_vertex_count(VisibilityPolygon polygon);

/**
 * @brief Checks if a point is inside the visibility polygon
 * @param polygon VisibilityPolygon instance
 * @param x Point X coordinate
 * @param y Point Y coordinate
 * @return true if point is visible, false otherwise
 */
bool visibility_polygon_contains_point(VisibilityPolygon polygon, double x,
                                       double y);

#endif // VISIBILITY_H
