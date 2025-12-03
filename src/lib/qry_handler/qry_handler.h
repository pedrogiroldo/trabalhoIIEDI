/**
 * @file qry_handler.h
 * @brief Query file command handler
 *
 * This module parses and executes commands from .qry files,
 * performing operations on the city such as anteparo transformations
 * and bomb operations (destruction, painting, cloning).
 */

#ifndef QRY_HANDLER_H
#define QRY_HANDLER_H

#include "../city/city.h"
#include "../file_reader/file_reader.h"

/**
 * @brief Processes a .qry file and executes commands on the city
 * @param city City instance to operate on
 * @param file_data File data containing .qry file lines
 * @param output_path Path to the output directory
 */
void qry_handler_process_file(City city, FileData file_data,
                              const char *output_path);

#endif // QRY_HANDLER_H