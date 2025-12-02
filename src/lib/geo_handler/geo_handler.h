/**
 * @file geo_handler.h
 * @brief Geographic file command handler
 *
 * This module parses and executes commands from .geo files,
 * creating geometric shapes and adding them to the city.
 */

#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H

#include "../city/city.h"
#include "../file_reader/file_reader.h"

/**
 * @brief Creates a city from a .geo file
 * @param file_data File data containing .geo file lines
 * @param output_path Path to the output directory
 * @param command_suffix Command suffix to add to the output file name
 * @return City instance with all shapes or NULL on error
 */
City geo_handler_create_city_from_file(FileData file_data,
                                       const char *output_path,
                                       const char *command_suffix);

#endif // GEO_HANDLER_H