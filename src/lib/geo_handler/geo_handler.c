#include "geo_handler.h"
#include "../commons/queue/queue.h"
#include "../commons/stack/stack.h"
#include "../file_reader/file_reader.h"
#include "../shapes/circle/circle.h"
#include "../shapes/line/line.h"
#include "../shapes/rectangle/rectangle.h"
#include "../shapes/shapes.h"
#include "../shapes/text/text.h"
#include "../shapes/text_style/text_style.h"
#include <stdio.h>
#include <string.h>

typedef struct {
  Queue shapes_queue;
  Stack cleanup_stack;
  Queue svg_queue;
} CityImpl;

// private functions defined as static and implemented on the end of the file
static void execute_circle_command(CityImpl *city);
static void execute_rectangle_command(CityImpl *city);
static void execute_line_command(CityImpl *city);
static void execute_text_command(CityImpl *city);
static void execute_text_style_command(CityImpl *city);
static void create_svg_queue(CityImpl *city, const char *output_path,
                             FileData file_data, const char *command_suffix);

City city_create_from_file(FileData file_data, const char *output_path,
                           const char *command_suffix) {
  CityImpl *city = malloc(sizeof(CityImpl));
  if (city == NULL) {
    printf("Error: Failed to allocate memory for City\n");
    exit(1);
  }

  city->shapes_queue = queue_create();
  city->cleanup_stack = stack_create();
  city->svg_queue = queue_create();
  while (!queue_is_empty(get_file_lines_queue(file_data))) {
    char *line = (char *)queue_dequeue(get_file_lines_queue(file_data));
    char *command = strtok(line, " ");

    // Circle command: c i x y r corb corp
    if (strcmp(command, "c") == 0) {
      execute_circle_command(city);
    }

    // Rectangle command: r i x y w h corb corp
    else if (strcmp(command, "r") == 0) {
      execute_rectangle_command(city);

    }
    // Line command: l i x1 y1 x2 y2 cor
    else if (strcmp(command, "l") == 0) {
      execute_line_command(city);

    }

    // Text command: t i x y corb corp a txto
    else if (strcmp(command, "t") == 0) {
      execute_text_command(city);
    }

    // Text style command: ts fFamily fWeight fSize
    else if (strcmp(command, "ts") == 0) {
      execute_text_style_command(city);
    } else {
      printf("Unknown command: %s\n", command);
    }
  }
  create_svg_queue(city, output_path, file_data, command_suffix);
  return city;
}

void city_destroy(City city) {
  CityImpl *impl = (CityImpl *)city;
  queue_destroy(impl->shapes_queue);
  queue_destroy(impl->svg_queue);
  while (!stack_is_empty(impl->cleanup_stack)) {
    Shape shape = stack_pop(impl->cleanup_stack);
    shape_destroy(shape);
  }
  stack_destroy(impl->cleanup_stack);
  free(city);
}

Queue city_get_shapes_queue(City city) {
  CityImpl *impl = (CityImpl *)city;
  return impl->shapes_queue;
}

Stack city_get_cleanup_stack(City city) {
  CityImpl *impl = (CityImpl *)city;
  return impl->cleanup_stack;
}

/**
**************************
* Private functions
**************************
*/
static void execute_circle_command(CityImpl *city) {
  char *identifier = strtok(NULL, " ");
  char *pos_x = strtok(NULL, " ");
  char *pos_y = strtok(NULL, " ");
  char *radius = strtok(NULL, " ");
  char *border_color = strtok(NULL, " ");
  char *fill_color = strtok(NULL, " ");

  Shape shape = circle_create(atoi(identifier), atof(pos_x), atof(pos_y),
                              atof(radius), border_color, fill_color);

  queue_enqueue(city->shapes_queue, shape);
  stack_push(city->cleanup_stack, shape);
  queue_enqueue(city->svg_queue, shape);
}

static void execute_rectangle_command(CityImpl *city) {
  char *identifier = strtok(NULL, " ");
  char *pos_x = strtok(NULL, " ");
  char *pos_y = strtok(NULL, " ");
  char *width = strtok(NULL, " ");
  char *height = strtok(NULL, " ");
  char *border_color = strtok(NULL, " ");
  char *fill_color = strtok(NULL, " ");

  Shape shape =
      rectangle_create(atoi(identifier), atof(pos_x), atof(pos_y), atof(width),
                       atof(height), border_color, fill_color);

  queue_enqueue(city->shapes_queue, shape);
  stack_push(city->cleanup_stack, shape);
  queue_enqueue(city->svg_queue, shape);
}

static void execute_line_command(CityImpl *city) {
  char *identifier = strtok(NULL, " ");
  char *x1 = strtok(NULL, " ");
  char *y1 = strtok(NULL, " ");
  char *x2 = strtok(NULL, " ");
  char *y2 = strtok(NULL, " ");
  char *color = strtok(NULL, " ");

  Shape shape = line_create(atoi(identifier), atof(x1), atof(y1), atof(x2),
                            atof(y2), color);

  queue_enqueue(city->shapes_queue, shape);
  stack_push(city->cleanup_stack, shape);
  queue_enqueue(city->svg_queue, shape);
}

static void execute_text_command(CityImpl *city) {
  char *identifier = strtok(NULL, " ");
  char *pos_x = strtok(NULL, " ");
  char *pos_y = strtok(NULL, " ");
  char *border_color = strtok(NULL, " ");
  char *fill_color = strtok(NULL, " ");
  char *anchor = strtok(NULL, " ");
  char *text = strtok(NULL, "");

  Shape shape = text_create(atoi(identifier), atof(pos_x), atof(pos_y),
                            border_color, fill_color, *anchor, text);

  queue_enqueue(city->shapes_queue, shape);
  stack_push(city->cleanup_stack, shape);
  queue_enqueue(city->svg_queue, shape);
}

static void execute_text_style_command(CityImpl *city) {
  char *font_family = strtok(NULL, " ");
  char *font_weight = strtok(NULL, " ");
  char *font_size = strtok(NULL, " ");

  Shape shape = text_style_create(font_family, *font_weight, atoi(font_size));

  queue_enqueue(city->shapes_queue, shape);
  stack_push(city->cleanup_stack, shape);
  queue_enqueue(city->svg_queue, shape);
}

static void create_svg_queue(CityImpl *city, const char *output_path,
                             FileData file_data, const char *command_suffix) {
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
    return;
  }

  // Use snprintf for safe string construction
  int result = snprintf(output_path_with_file, total_len, "%s/%s.svg",
                        output_path, file_name);
  if (result < 0 || (size_t)result >= total_len) {
    printf("Error: Path construction failed\n");
    free(output_path_with_file);
    return;
  }

  FILE *file = fopen(output_path_with_file, "w");
  if (file == NULL) {
    printf("Error: Failed to open file: %s\n", output_path_with_file);
    free(output_path_with_file);
    return;
  }
  fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(
      file,
      "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 1000 1000\">\n");
  while (!queue_is_empty(city->svg_queue)) {
    Shape shape = queue_dequeue(city->svg_queue);
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