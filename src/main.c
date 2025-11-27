#include "lib/args_handler/args_handler.h"
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
  const char *subvector_something = get_option_value(argc, argv, "i");

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

  // continue here

  // Clean up allocated memory
  if (full_geo_path != NULL) {
    free(full_geo_path);
  }
  if (full_qry_path != NULL) {
    free(full_qry_path);
  }

  return 0;
}