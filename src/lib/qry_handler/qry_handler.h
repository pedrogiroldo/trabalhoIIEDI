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
#include "../commons/sorting/sorting.h"
#include "../file_reader/file_reader.h"

/**
 * @brief Processes a .qry file and executes commands on the city
 * @param city City instance to operate on
 * @param geo_file_data File data from .geo file (for SVG naming)
 * @param qry_file_data File data containing .qry file lines
 * @param output_path Path to the output directory
 * @param sort_type Sorting algorithm to use (SORT_QSORT or SORT_MERGESORT)
 * @param sort_threshold Threshold for InsertionSort (only for SORT_MERGESORT)
 */
void qry_handler_process_file(City city, FileData geo_file_data,
                              FileData qry_file_data, const char *output_path,
                              SortType sort_type, int sort_threshold);

#endif // QRY_HANDLER_H