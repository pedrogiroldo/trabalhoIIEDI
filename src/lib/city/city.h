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

#include "../commons/queue/queue.h"
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
 * @brief Gets the shapes queue from the city
 * @param city City instance
 * @return Queue containing all shapes
 */
Queue city_get_shapes_queue(City city);

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

#endif // CITY_H
