/**
 * @file geo_handler.h
 * @brief Geographic file command handler
 *
 * This module executes the commands from .geo files and manages
 * the city with all geometric shapes.
 */

#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H
#include "../commons/stack/stack.h"
#include "../file_reader/file_reader.h"

/**
 * @brief Opaque pointer type for city instances
 */
typedef void *City;

/**
 * @brief Creates a city from a .geo file
 * @param file_data File data containing .geo file lines
 * @param output_path Path to the output file
 * @param command_suffix Command suffix to add to the output file name
 * @return City instance with all shapes or NULL on error
 */
City city_create_from_file(FileData file_data, const char *output_path,
                           const char *command_suffix);

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
 * @brief Destroys the city and frees all associated memory
 *
 * This function should be called after all operations that use the city
 * are completed to prevent memory leaks.
 *
 * @param city City instance to destroy
 */
void city_destroy(City city);

#endif // GEO_HANDLER_H