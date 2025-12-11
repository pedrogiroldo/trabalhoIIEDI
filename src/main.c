#include "lib/args_handler/args_handler.h"
#include "lib/city/city.h"
#include "lib/commons/sorting/sorting.h"
#include "lib/file_reader/file_reader.h"
#include "lib/geo_handler/geo_handler.h"
#include "lib/qry_handler/qry_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc >
      14) { // program -e path -f .geo -o output -q .qry -to timeout -i input
    printf("Error: Too many arguments\n");
    exit(1);
  }

  // Get arguments
  const char *prefix_path = get_option_value(argc, argv, "e");
  const char *geo_input_path = get_option_value(argc, argv, "f");
  const char *output_path = get_option_value(argc, argv, "o");
  const char *qry_input_path = get_option_value(argc, argv, "q");
  const char *ordenation_type = get_option_value(argc, argv, "to");
  char *min_insertionsort_size = get_option_value(argc, argv, "i");

  // Apply default value for -in if not provided
  if (min_insertionsort_size == NULL) {
    min_insertionsort_size = (char *)malloc(3);
    sprintf(min_insertionsort_size, "10");
  }

  // Parse sorting parameters
  SortType sort_type = SORT_QSORT; // Default to qsort
  if (ordenation_type != NULL && ordenation_type[0] == 'm') {
    sort_type = SORT_MERGESORT;
  }
  int sort_threshold = atoi(min_insertionsort_size);

  // Apply prefix_path if it exists (only to -f and -q)
  char *full_geo_path = NULL;
  char *full_qry_path = NULL;

  if (prefix_path != NULL) {
    size_t prefix_len = strlen(prefix_path);
    int needs_slash = (prefix_len > 0 && prefix_path[prefix_len - 1] != '/');

    if (geo_input_path != NULL) {
      size_t geo_len = strlen(geo_input_path);
      full_geo_path = (char *)malloc(prefix_len + geo_len + 2);
      if (needs_slash) {
        sprintf(full_geo_path, "%s/%s", prefix_path, geo_input_path);
      } else {
        sprintf(full_geo_path, "%s%s", prefix_path, geo_input_path);
      }
      geo_input_path = full_geo_path;
    }

    if (qry_input_path != NULL) {
      size_t qry_len = strlen(qry_input_path);
      full_qry_path = (char *)malloc(prefix_len + qry_len + 2);
      if (needs_slash) {
        sprintf(full_qry_path, "%s/%s", prefix_path, qry_input_path);
      } else {
        sprintf(full_qry_path, "%s%s", prefix_path, qry_input_path);
      }
      qry_input_path = full_qry_path;
    }
  }

  // Verify required arguments
  if (geo_input_path == NULL || output_path == NULL) {
    printf("Error: -f and -o are required\n");
    exit(1);
  }

  // Read .geo file
  FileData geo_file_data = file_data_create(geo_input_path);
  if (geo_file_data == NULL) {
    printf("Error: Failed to read .geo file: %s\n", geo_input_path);
    exit(1);
  }

  // Create city from .geo file
  City city =
      geo_handler_create_city_from_file(geo_file_data, output_path, NULL);
  if (city == NULL) {
    printf("Error: Failed to create city from .geo file\n");
    file_data_destroy(geo_file_data);
    exit(1);
  }

  // Process .qry file if provided
  if (qry_input_path != NULL) {
    FileData qry_file_data = file_data_create(qry_input_path);
    if (qry_file_data == NULL) {
      printf("Error: Failed to read .qry file: %s\n", qry_input_path);
      city_destroy(city);
      file_data_destroy(geo_file_data);
      exit(1);
    }

    // Process query commands
    qry_handler_process_file(city, geo_file_data, qry_file_data, output_path,
                             sort_type, sort_threshold);

    file_data_destroy(qry_file_data);
  }

  // Clean up
  city_destroy(city);
  file_data_destroy(geo_file_data);

  // Clean up allocated memory
  if (full_geo_path != NULL) {
    free(full_geo_path);
  }
  if (full_qry_path != NULL) {
    free(full_qry_path);
  }
  if (min_insertionsort_size != NULL &&
      get_option_value(argc, argv, "i") == NULL) {
    free(min_insertionsort_size);
  }

  return 0;
}