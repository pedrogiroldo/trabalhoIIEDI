#include "city.h"
#include "../shapes/circle/circle.h"
#include "../shapes/line/line.h"
#include "../shapes/rectangle/rectangle.h"
#include "../shapes/text/text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  List shapes_list;
  Stack cleanup_stack;
  List svg_list;
} CityImpl;

City city_create(void) {
  CityImpl *city = malloc(sizeof(CityImpl));
  if (city == NULL) {
    printf("Error: Failed to allocate memory for City\n");
    return NULL;
  }

  city->shapes_list = list_create();
  city->cleanup_stack = stack_create();
  city->svg_list = list_create();

  return (City)city;
}

void city_destroy(City city) {
  CityImpl *impl = (CityImpl *)city;

  list_destroy(impl->shapes_list);
  list_destroy(impl->svg_list);

  while (!stack_is_empty(impl->cleanup_stack)) {
    Shape shape = stack_pop(impl->cleanup_stack);
    shape_destroy(shape);
  }
  stack_destroy(impl->cleanup_stack);

  free(city);
}

void city_add_shape(City city, Shape shape) {
  CityImpl *impl = (CityImpl *)city;

  list_insert_back(impl->shapes_list, shape);
  stack_push(impl->cleanup_stack, shape);
  list_insert_back(impl->svg_list, shape);
}

List city_get_shapes_list(City city) {
  CityImpl *impl = (CityImpl *)city;
  return impl->shapes_list;
}

Stack city_get_cleanup_stack(City city) {
  CityImpl *impl = (CityImpl *)city;
  return impl->cleanup_stack;
}

void city_generate_svg(City city, const char *output_path, FileData file_data,
                       const char *command_suffix) {
  CityImpl *impl = (CityImpl *)city;

  const char *original_file_name = get_file_name(file_data);
  size_t name_len = strlen(original_file_name);
  char *file_name = malloc(name_len + 1);
  if (file_name == NULL) {
    printf("Error: Memory allocation failed for file name\n");
    return;
  }
  strcpy(file_name, original_file_name);
  strtok(file_name, ".");

  if (command_suffix != NULL) {
    strcat(file_name, "-");
    strcat(file_name, command_suffix);
  }

  // Calculate required buffer size: output_path + "/" + file_name + ".svg" +
  // null terminator
  size_t path_len = strlen(output_path);
  size_t processed_name_len = strlen(file_name);
  size_t total_len = path_len + 1 + processed_name_len + 4 +
                     1; // +1 for "/", +4 for ".svg", +1 for null terminator

  // Use dynamic allocation for safety
  char *output_path_with_file = malloc(total_len);
  if (output_path_with_file == NULL) {
    printf("Error: Memory allocation failed\n");
    free(file_name);
    return;
  }

  // Use snprintf for safe string construction
  int result = snprintf(output_path_with_file, total_len, "%s/%s.svg",
                        output_path, file_name);
  if (result < 0 || (size_t)result >= total_len) {
    printf("Error: Path construction failed\n");
    free(output_path_with_file);
    free(file_name);
    return;
  }

  FILE *file = fopen(output_path_with_file, "w");
  if (file == NULL) {
    printf("Error: Failed to open file: %s\n", output_path_with_file);
    free(output_path_with_file);
    free(file_name);
    return;
  }

  fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(
      file,
      "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 1000 1000\">\n");

  // Iterate over list using index-based access
  int svg_list_size = list_size(impl->svg_list);
  for (int i = 0; i < svg_list_size; i++) {
    Shape shape = list_get(impl->svg_list, i);
    if (shape != NULL) {
      ShapeType type = shape_get_type(shape);
      if (type == CIRCLE) {
        Circle circle = (Circle)shape_get_shape(shape);
        fprintf(
            file,
            "<circle cx='%.2f' cy='%.2f' r='%.2f' fill='%s' stroke='%s'/>\n",
            circle_get_x(circle), circle_get_y(circle),
            circle_get_radius(circle), circle_get_fill_color(circle),
            circle_get_border_color(circle));
      } else if (type == RECTANGLE) {
        Rectangle rectangle = (Rectangle)shape_get_shape(shape);
        fprintf(file,
                "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' fill='%s' "
                "stroke='%s'/>\n",
                rectangle_get_x(rectangle), rectangle_get_y(rectangle),
                rectangle_get_width(rectangle), rectangle_get_height(rectangle),
                rectangle_get_fill_color(rectangle),
                rectangle_get_border_color(rectangle));
      } else if (type == LINE) {
        Line line = (Line)shape_get_shape(shape);
        fprintf(file,
                "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
                line_get_x1(line), line_get_y1(line), line_get_x2(line),
                line_get_y2(line), line_get_color(line));
      } else if (type == TEXT) {
        Text text = (Text)shape_get_shape(shape);
        char anchor = text_get_anchor(text);
        const char *text_anchor = "start"; // default

        // Map anchor character to SVG text-anchor value
        if (anchor == 'm' || anchor == 'M') {
          text_anchor = "middle";
        } else if (anchor == 'e' || anchor == 'E') {
          text_anchor = "end";
        } else if (anchor == 's' || anchor == 'S') {
          text_anchor = "start";
        }

        fprintf(file,
                "<text x='%.2f' y='%.2f' fill='%s' stroke='%s' "
                "text-anchor='%s'>%s</text>\n",
                text_get_x(text), text_get_y(text), text_get_fill_color(text),
                text_get_border_color(text), text_anchor, text_get_text(text));
      }
    }
  }

  fprintf(file, "</svg>\n");
  fclose(file);
  free(output_path_with_file);
  free(file_name);
}
