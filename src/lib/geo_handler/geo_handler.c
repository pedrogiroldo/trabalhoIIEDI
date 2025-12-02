#include "geo_handler.h"
#include "../city/city.h"
#include "../commons/queue/queue.h"
#include "../file_reader/file_reader.h"
#include "../shapes/circle/circle.h"
#include "../shapes/line/line.h"
#include "../shapes/rectangle/rectangle.h"
#include "../shapes/shapes.h"
#include "../shapes/text/text.h"
#include "../shapes/text_style/text_style.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Private functions for command execution
static void execute_circle_command(City city);
static void execute_rectangle_command(City city);
static void execute_line_command(City city);
static void execute_text_command(City city);
static void execute_text_style_command(City city);

City geo_handler_create_city_from_file(FileData file_data,
                                       const char *output_path,
                                       const char *command_suffix) {
  City city = city_create();
  if (city == NULL) {
    printf("Error: Failed to create city\n");
    return NULL;
  }

  Queue file_lines = get_file_lines_queue(file_data);

  while (!queue_is_empty(file_lines)) {
    char *line = (char *)queue_dequeue(file_lines);
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

  city_generate_svg(city, output_path, file_data, command_suffix);

  return city;
}

/**
**************************
* Private functions
**************************
*/
static void execute_circle_command(City city) {
  char *identifier = strtok(NULL, " ");
  char *pos_x = strtok(NULL, " ");
  char *pos_y = strtok(NULL, " ");
  char *radius = strtok(NULL, " ");
  char *border_color = strtok(NULL, " ");
  char *fill_color = strtok(NULL, " ");

  Shape shape = circle_create(atoi(identifier), atof(pos_x), atof(pos_y),
                              atof(radius), border_color, fill_color);

  city_add_shape(city, shape);
}

static void execute_rectangle_command(City city) {
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

  city_add_shape(city, shape);
}

static void execute_line_command(City city) {
  char *identifier = strtok(NULL, " ");
  char *x1 = strtok(NULL, " ");
  char *y1 = strtok(NULL, " ");
  char *x2 = strtok(NULL, " ");
  char *y2 = strtok(NULL, " ");
  char *color = strtok(NULL, " ");

  Shape shape = line_create(atoi(identifier), atof(x1), atof(y1), atof(x2),
                            atof(y2), color);

  city_add_shape(city, shape);
}

static void execute_text_command(City city) {
  char *identifier = strtok(NULL, " ");
  char *pos_x = strtok(NULL, " ");
  char *pos_y = strtok(NULL, " ");
  char *border_color = strtok(NULL, " ");
  char *fill_color = strtok(NULL, " ");
  char *anchor = strtok(NULL, " ");
  char *text = strtok(NULL, "");

  Shape shape = text_create(atoi(identifier), atof(pos_x), atof(pos_y),
                            border_color, fill_color, *anchor, text);

  city_add_shape(city, shape);
}

static void execute_text_style_command(City city) {
  char *font_family = strtok(NULL, " ");
  char *font_weight = strtok(NULL, " ");
  char *font_size = strtok(NULL, " ");

  Shape shape = text_style_create(font_family, *font_weight, atoi(font_size));

  city_add_shape(city, shape);
}