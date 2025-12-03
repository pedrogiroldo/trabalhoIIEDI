/**
 * @file city.h
 * @brief City management module
 *
 * This module manages the city data structure, storing geometric shapes
 * and providing operations for shape manipulation, barrier transformation,
 * bomb operations, and SVG generation.
 */

#ifndef CITY_H
#define CITY_H

#include "../commons/list/list.h"
#include "../commons/stack/stack.h"
#include "../file_reader/file_reader.h"
#include "../shapes/shapes.h"

/**
 * @brief Opaque pointer type for city instances
 */
typedef void *City;

/**
 * @brief Creates a new empty city
 * @return City instance or NULL on error
 */
City city_create(void);

/**
 * @brief Destroys the city and frees all associated memory
 * @param city City instance to destroy
 */
void city_destroy(City city);

/**
 * @brief Adds a shape to the city
 * @param city City instance
 * @param shape Shape to add
 */
void city_add_shape(City city, Shape shape);

/**
 * @brief Gets the shapes list from the city
 * @param city City instance
 * @return List containing all shapes
 */
List city_get_shapes_list(City city);

/**
 * @brief Gets the cleanup stack from the city
 * @param city City instance
 * @return Stack containing shapes to free
 */
Stack city_get_cleanup_stack(City city);

/**
 * @brief Generates an SVG file with all shapes in the city
 * @param city City instance
 * @param output_path Directory path for output
 * @param file_data File data containing original file name
 * @param command_suffix Optional suffix for the output file (can be NULL)
 */
void city_generate_svg(City city, const char *output_path, FileData file_data,
                       const char *command_suffix);

/**
 * @brief Generates an SVG file with all shapes and a visibility polygon
 * @param city City instance
 * @param output_path Directory path for output
 * @param file_data File data containing original file name
 * @param command_suffix Optional suffix for the output file (can be NULL)
 * @param visibility_polygon Visibility polygon to render (from visibility
 * module)
 * @param source_x X coordinate of the visibility source point
 * @param source_y Y coordinate of the visibility source point
 */
void city_generate_svg_with_visibility(City city, const char *output_path,
                                       FileData file_data,
                                       const char *command_suffix,
                                       void *visibility_polygon,
                                       double source_x, double source_y);

/**
 * @brief Gets all barrier segments from the city
 * @param city City instance
 * @return List of Line shapes marked as barriers
 */
List city_get_barriers(City city);

/**
 * @brief Removes a shape from the city by reference
 * @param city City instance
 * @param shape Shape to remove
 * @return true if removed, false if not found
 */
bool city_remove_shape(City city, Shape shape);

/**
 * @brief Gets the next available unique ID for shapes
 * @param city City instance
 * @return Next unique ID
 */
int city_get_next_id(City city);

/**
 * @brief Gets a shape by ID
 * @param city City instance
 * @param id Shape ID to find
 * @return Shape with given ID or NULL if not found
 */
Shape city_get_shape_by_id(City city, int id);

/**
 * @brief Updates the maximum ID tracked by the city
 * @param city City instance
 * @param id ID to consider for maximum
 */
void city_update_max_id(City city, int id);

#endif // CITY_H
