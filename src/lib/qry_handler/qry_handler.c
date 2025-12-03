#include "qry_handler.h"
#include "../city/city.h"
#include "../commons/list/list.h"
#include "../commons/queue/queue.h"
#include "../file_reader/file_reader.h"
#include "../shapes/circle/circle.h"
#include "../shapes/line/line.h"
#include "../shapes/rectangle/rectangle.h"
#include "../shapes/shapes.h"
#include "../shapes/text/text.h"
#include "../visibility/visibility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Private helper functions
static void execute_anteparo_command(City city, FILE *txt_output);
static void execute_destruction_bomb(City city, const char *output_path,
                                     FileData file_data, const char *suffix,
                                     FILE *txt_output);
static void execute_painting_bomb(City city, const char *output_path,
                                  FileData file_data, const char *suffix,
                                  FILE *txt_output);
static void execute_cloning_bomb(City city, const char *output_path,
                                 FileData file_data, const char *suffix,
                                 FILE *txt_output);
static bool shape_in_visibility_region(Shape shape, VisibilityPolygon polygon);
static void get_shape_center(Shape shape, double *x, double *y);
static const char *get_shape_type_name(ShapeType type);

void qry_handler_process_file(City city, FileData file_data,
                              const char *output_path) {
  if (!city || !file_data || !output_path) {
    return;
  }

  // Create text output file
  const char *original_file_name = get_file_name(file_data);
  size_t name_len = strlen(original_file_name);
  char *file_name = malloc(name_len + 1);
  if (!file_name) {
    printf("Error: Memory allocation failed for file name\n");
    return;
  }
  strcpy(file_name, original_file_name);
  strtok(file_name, ".");

  // Build output path for text file
  size_t path_len = strlen(output_path);
  size_t processed_name_len = strlen(file_name);
  size_t total_len = path_len + 1 + processed_name_len + 4 + 1;

  char *txt_path = malloc(total_len);
  if (!txt_path) {
    printf("Error: Memory allocation failed\n");
    free(file_name);
    return;
  }

  snprintf(txt_path, total_len, "%s/%s.txt", output_path, file_name);
  free(file_name);

  FILE *txt_output = fopen(txt_path, "w");
  if (!txt_output) {
    printf("Error: Failed to open text output file: %s\n", txt_path);
    free(txt_path);
    return;
  }

  fprintf(txt_output, "Resultados dos Comandos de Consulta\n");
  fprintf(txt_output, "====================================\n\n");

  // Process each command line
  Queue file_lines = get_file_lines_queue(file_data);
  while (!queue_is_empty(file_lines)) {
    char *line = (char *)queue_dequeue(file_lines);
    char *command = strtok(line, " ");

    if (strcmp(command, "a") == 0) {
      execute_anteparo_command(city, txt_output);
    } else if (strcmp(command, "d") == 0) {
      execute_destruction_bomb(city, output_path, file_data, NULL, txt_output);
    } else if (strcmp(command, "p") == 0) {
      execute_painting_bomb(city, output_path, file_data, NULL, txt_output);
    } else if (strcmp(command, "cln") == 0) {
      execute_cloning_bomb(city, output_path, file_data, NULL, txt_output);
    } else {
      fprintf(txt_output, "Comando desconhecido: %s\n\n", command);
    }
  }

  fclose(txt_output);
  free(txt_path);

  // Generate final SVG with all modifications
  city_generate_svg(city, output_path, file_data, NULL);
}

static void execute_anteparo_command(City city, FILE *txt_output) {
  char *start_id_str = strtok(NULL, " ");
  char *end_id_str = strtok(NULL, " ");
  char *orientation = strtok(NULL, " ");

  if (!start_id_str || !end_id_str) {
    fprintf(txt_output, "Erro: Comando 'a' requer IDs inicial e final\n\n");
    return;
  }

  int start_id = atoi(start_id_str);
  int end_id = atoi(end_id_str);
  char orient = orientation ? orientation[0] : 'h';

  fprintf(txt_output, "Comando: a %d %d %c\n", start_id, end_id, orient);
  fprintf(txt_output, "Transformados em anteparos:\n");

  List shapes_to_remove = list_create();
  List segments_to_add = list_create();

  // Find and transform shapes in the ID range
  for (int id = start_id; id <= end_id; id++) {
    Shape shape = city_get_shape_by_id(city, id);
    if (!shape) {
      continue;
    }

    ShapeType type = shape_get_type(shape);
    list_insert_back(shapes_to_remove, shape);

    switch (type) {
    case CIRCLE: {
      Circle circle = (Circle)shape_get_shape(shape);
      double cx = circle_get_x(circle);
      double cy = circle_get_y(circle);
      double r = circle_get_radius(circle);
      const char *color = circle_get_border_color(circle);

      int new_id = city_get_next_id(city);
      Shape segment;

      if (orient == 'v' || orient == 'V') {
        segment = line_create(new_id, cx, cy - r, cx, cy + r, color);
        fprintf(txt_output,
                "  Circulo id=%d -> Segmento vertical id=%d "
                "(%.2f,%.2f)-(%.2f,%.2f)\n",
                id, new_id, cx, cy - r, cx, cy + r);
      } else {
        segment = line_create(new_id, cx - r, cy, cx + r, cy, color);
        fprintf(txt_output,
                "  Circulo id=%d -> Segmento horizontal id=%d "
                "(%.2f,%.2f)-(%.2f,%.2f)\n",
                id, new_id, cx - r, cy, cx + r, cy);
      }

      Line line = (Line)shape_get_shape(segment);
      line_set_barrier(line, true);
      list_insert_back(segments_to_add, segment);
      break;
    }

    case RECTANGLE: {
      Rectangle rect = (Rectangle)shape_get_shape(shape);
      double x = rectangle_get_x(rect);
      double y = rectangle_get_y(rect);
      double w = rectangle_get_width(rect);
      double h = rectangle_get_height(rect);
      const char *color = rectangle_get_border_color(rect);

      fprintf(txt_output, "  Retangulo id=%d -> Segmentos:", id);

      int ids[4];
      for (int i = 0; i < 4; i++) {
        ids[i] = city_get_next_id(city);
      }

      Shape segments[4];
      segments[0] = line_create(ids[0], x, y, x + w, y, color);
      segments[1] = line_create(ids[1], x + w, y, x + w, y + h, color);
      segments[2] = line_create(ids[2], x + w, y + h, x, y + h, color);
      segments[3] = line_create(ids[3], x, y + h, x, y, color);

      for (int i = 0; i < 4; i++) {
        Line line = (Line)shape_get_shape(segments[i]);
        line_set_barrier(line, true);
        list_insert_back(segments_to_add, segments[i]);
        fprintf(txt_output, " id=%d", ids[i]);
      }
      fprintf(txt_output, "\n");
      break;
    }

    case LINE: {
      Line line = (Line)shape_get_shape(shape);
      line_set_barrier(line, true);
      fprintf(txt_output, "  Linha id=%d -> Marcada como anteparo\n", id);
      list_remove(shapes_to_remove, shape);
      break;
    }

    case TEXT: {
      Text text = (Text)shape_get_shape(shape);
      double x = text_get_x(text);
      double y = text_get_y(text);
      const char *text_str = text_get_text(text);
      char anchor = text_get_anchor(text);
      const char *color = text_get_border_color(text);

      int text_len = strlen(text_str);
      double x1, x2;

      if (anchor == 'i' || anchor == 'I') {
        x1 = x;
        x2 = x + 10.0 * text_len;
      } else if (anchor == 'f' || anchor == 'F') {
        x1 = x - 10.0 * text_len;
        x2 = x;
      } else {
        x1 = x - 10.0 * text_len / 2.0;
        x2 = x + 10.0 * text_len / 2.0;
      }

      int new_id = city_get_next_id(city);
      Shape segment = line_create(new_id, x1, y, x2, y, color);
      Line line = (Line)shape_get_shape(segment);
      line_set_barrier(line, true);
      list_insert_back(segments_to_add, segment);

      fprintf(txt_output,
              "  Texto id=%d -> Segmento id=%d (%.2f,%.2f)-(%.2f,%.2f)\n", id,
              new_id, x1, y, x2, y);
      break;
    }

    default:
      break;
    }
  }

  int remove_count = list_size(shapes_to_remove);
  for (int i = 0; i < remove_count; i++) {
    Shape shape = list_get(shapes_to_remove, i);
    city_remove_shape(city, shape);
  }

  int add_count = list_size(segments_to_add);
  for (int i = 0; i < add_count; i++) {
    Shape segment = list_get(segments_to_add, i);
    city_add_shape(city, segment);
  }

  list_destroy(shapes_to_remove);
  list_destroy(segments_to_add);

  fprintf(txt_output, "\n");
}

static void execute_destruction_bomb(City city, const char *output_path,
                                     FileData file_data, const char *suffix,
                                     FILE *txt_output) {
  char *x_str = strtok(NULL, " ");
  char *y_str = strtok(NULL, " ");
  char *sfx = strtok(NULL, " ");

  if (!x_str || !y_str) {
    fprintf(txt_output, "Erro: Comando 'd' requer coordenadas x e y\n\n");
    return;
  }

  double x = atof(x_str);
  double y = atof(y_str);

  fprintf(txt_output, "Comando: d %.2f %.2f %s\n", x, y, sfx ? sfx : "-");
  fprintf(txt_output, "Formas destruidas:\n");

  List barriers = city_get_barriers(city);
  VisibilityPolygon polygon = visibility_calculate(x, y, barriers, 1000.0);

  if (!polygon) {
    fprintf(txt_output, "  Erro ao calcular regiao de visibilidade\n\n");
    list_destroy(barriers);
    return;
  }

  // Generate SVG with visibility polygon if suffix is provided and not "-"
  if (sfx != NULL && strcmp(sfx, "-") != 0) {
    city_generate_svg_with_visibility(city, output_path, file_data, sfx,
                                      polygon, x, y);
  }

  List shapes_to_destroy = list_create();
  List shapes_list = city_get_shapes_list(city);
  int shape_count = list_size(shapes_list);

  for (int i = 0; i < shape_count; i++) {
    Shape shape = list_get(shapes_list, i);
    if (shape && shape_in_visibility_region(shape, polygon)) {
      list_insert_back(shapes_to_destroy, shape);
    }
  }

  int destroy_count = list_size(shapes_to_destroy);
  for (int i = 0; i < destroy_count; i++) {
    Shape shape = list_get(shapes_to_destroy, i);
    ShapeType type = shape_get_type(shape);
    int id = -1;

    switch (type) {
    case CIRCLE:
      id = circle_get_id((Circle)shape_get_shape(shape));
      break;
    case RECTANGLE:
      id = rectangle_get_id((Rectangle)shape_get_shape(shape));
      break;
    case LINE:
      id = line_get_id((Line)shape_get_shape(shape));
      break;
    case TEXT:
      id = text_get_id((Text)shape_get_shape(shape));
      break;
    default:
      continue;
    }

    fprintf(txt_output, "  %s id=%d\n", get_shape_type_name(type), id);
    city_remove_shape(city, shape);
  }

  if (destroy_count == 0) {
    fprintf(txt_output, "  Nenhuma forma destruida\n");
  }

  list_destroy(shapes_to_destroy);
  list_destroy(barriers);
  visibility_polygon_destroy(polygon);

  fprintf(txt_output, "\n");
}

static void execute_painting_bomb(City city, const char *output_path,
                                  FileData file_data, const char *suffix,
                                  FILE *txt_output) {
  char *x_str = strtok(NULL, " ");
  char *y_str = strtok(NULL, " ");
  char *color = strtok(NULL, " ");
  char *sfx = strtok(NULL, " ");

  if (!x_str || !y_str || !color) {
    fprintf(txt_output, "Erro: Comando 'p' requer coordenadas x, y e cor\n\n");
    return;
  }

  double x = atof(x_str);
  double y = atof(y_str);

  fprintf(txt_output, "Comando: p %.2f %.2f %s %s\n", x, y, color,
          sfx ? sfx : "-");
  fprintf(txt_output, "Formas pintadas:\n");

  List barriers = city_get_barriers(city);
  VisibilityPolygon polygon = visibility_calculate(x, y, barriers, 1000.0);

  if (!polygon) {
    fprintf(txt_output, "  Erro ao calcular regiao de visibilidade\n\n");
    list_destroy(barriers);
    return;
  }

  // Generate SVG with visibility polygon if suffix is provided and not "-"
  if (sfx != NULL && strcmp(sfx, "-") != 0) {
    city_generate_svg_with_visibility(city, output_path, file_data, sfx,
                                      polygon, x, y);
  }

  List shapes_list = city_get_shapes_list(city);
  int shape_count = list_size(shapes_list);
  int painted_count = 0;

  for (int i = 0; i < shape_count; i++) {
    Shape shape = list_get(shapes_list, i);
    if (!shape || !shape_in_visibility_region(shape, polygon)) {
      continue;
    }

    ShapeType type = shape_get_type(shape);
    int id = -1;

    switch (type) {
    case CIRCLE: {
      Circle circle = (Circle)shape_get_shape(shape);
      id = circle_get_id(circle);
      circle_set_colors(circle, color);
      fprintf(txt_output, "  Circulo id=%d\n", id);
      painted_count++;
      break;
    }
    case RECTANGLE: {
      Rectangle rect = (Rectangle)shape_get_shape(shape);
      id = rectangle_get_id(rect);
      rectangle_set_colors(rect, color);
      fprintf(txt_output, "  Retangulo id=%d\n", id);
      painted_count++;
      break;
    }
    case LINE: {
      Line line = (Line)shape_get_shape(shape);
      id = line_get_id(line);
      line_set_color(line, color);
      fprintf(txt_output, "  Linha id=%d\n", id);
      painted_count++;
      break;
    }
    case TEXT: {
      Text text = (Text)shape_get_shape(shape);
      id = text_get_id(text);
      text_set_colors(text, color);
      fprintf(txt_output, "  Texto id=%d\n", id);
      painted_count++;
      break;
    }
    default:
      continue;
    }
  }

  if (painted_count == 0) {
    fprintf(txt_output, "  Nenhuma forma pintada\n");
  }

  list_destroy(barriers);
  visibility_polygon_destroy(polygon);

  fprintf(txt_output, "\n");
}

static void execute_cloning_bomb(City city, const char *output_path,
                                 FileData file_data, const char *suffix,
                                 FILE *txt_output) {
  char *x_str = strtok(NULL, " ");
  char *y_str = strtok(NULL, " ");
  char *dx_str = strtok(NULL, " ");
  char *dy_str = strtok(NULL, " ");
  char *sfx = strtok(NULL, " ");

  if (!x_str || !y_str || !dx_str || !dy_str) {
    fprintf(txt_output,
            "Erro: Comando 'cln' requer coordenadas x, y, dx, dy\n\n");
    return;
  }

  double x = atof(x_str);
  double y = atof(y_str);
  double dx = atof(dx_str);
  double dy = atof(dy_str);

  fprintf(txt_output, "Comando: cln %.2f %.2f %.2f %.2f %s\n", x, y, dx, dy,
          sfx ? sfx : "-");
  fprintf(txt_output, "Formas clonadas:\n");

  List barriers = city_get_barriers(city);
  VisibilityPolygon polygon = visibility_calculate(x, y, barriers, 1000.0);

  if (!polygon) {
    fprintf(txt_output, "  Erro ao calcular regiao de visibilidade\n\n");
    list_destroy(barriers);
    return;
  }

  // Generate SVG with visibility polygon if suffix is provided and not "-"
  if (sfx != NULL && strcmp(sfx, "-") != 0) {
    city_generate_svg_with_visibility(city, output_path, file_data, sfx,
                                      polygon, x, y);
  }

  List shapes_to_clone = list_create();
  List shapes_list = city_get_shapes_list(city);
  int shape_count = list_size(shapes_list);

  for (int i = 0; i < shape_count; i++) {
    Shape shape = list_get(shapes_list, i);
    if (shape && shape_in_visibility_region(shape, polygon)) {
      list_insert_back(shapes_to_clone, shape);
    }
  }

  int clone_count = list_size(shapes_to_clone);
  for (int i = 0; i < clone_count; i++) {
    Shape original = list_get(shapes_to_clone, i);
    ShapeType type = shape_get_type(original);
    int original_id = -1;
    int clone_id = city_get_next_id(city);
    Shape clone = NULL;

    switch (type) {
    case CIRCLE: {
      Circle circle = (Circle)shape_get_shape(original);
      original_id = circle_get_id(circle);
      clone = circle_create(
          clone_id, circle_get_x(circle) + dx, circle_get_y(circle) + dy,
          circle_get_radius(circle), circle_get_border_color(circle),
          circle_get_fill_color(circle));
      break;
    }
    case RECTANGLE: {
      Rectangle rect = (Rectangle)shape_get_shape(original);
      original_id = rectangle_get_id(rect);
      clone = rectangle_create(
          clone_id, rectangle_get_x(rect) + dx, rectangle_get_y(rect) + dy,
          rectangle_get_width(rect), rectangle_get_height(rect),
          rectangle_get_border_color(rect), rectangle_get_fill_color(rect));
      break;
    }
    case LINE: {
      Line line = (Line)shape_get_shape(original);
      original_id = line_get_id(line);
      clone = line_create(clone_id, line_get_x1(line) + dx,
                          line_get_y1(line) + dy, line_get_x2(line) + dx,
                          line_get_y2(line) + dy, line_get_color(line));
      break;
    }
    case TEXT: {
      Text text = (Text)shape_get_shape(original);
      original_id = text_get_id(text);
      clone =
          text_create(clone_id, text_get_x(text) + dx, text_get_y(text) + dy,
                      text_get_border_color(text), text_get_fill_color(text),
                      text_get_anchor(text), text_get_text(text));
      break;
    }
    default:
      continue;
    }

    if (clone) {
      city_add_shape(city, clone);
      fprintf(txt_output, "  %s id=%d -> Clone id=%d\n",
              get_shape_type_name(type), original_id, clone_id);
    }
  }

  if (clone_count == 0) {
    fprintf(txt_output, "  Nenhuma forma clonada\n");
  }

  list_destroy(shapes_to_clone);
  list_destroy(barriers);
  visibility_polygon_destroy(polygon);

  fprintf(txt_output, "\n");
}

static bool shape_in_visibility_region(Shape shape, VisibilityPolygon polygon) {
  if (!shape || !polygon) {
    return false;
  }

  double cx, cy;
  get_shape_center(shape, &cx, &cy);

  return visibility_polygon_contains_point(polygon, cx, cy);
}

static void get_shape_center(Shape shape, double *x, double *y) {
  if (!shape || !x || !y) {
    return;
  }

  ShapeType type = shape_get_type(shape);

  switch (type) {
  case CIRCLE: {
    Circle circle = (Circle)shape_get_shape(shape);
    *x = circle_get_x(circle);
    *y = circle_get_y(circle);
    break;
  }
  case RECTANGLE: {
    Rectangle rect = (Rectangle)shape_get_shape(shape);
    *x = rectangle_get_x(rect) + rectangle_get_width(rect) / 2.0;
    *y = rectangle_get_y(rect) + rectangle_get_height(rect) / 2.0;
    break;
  }
  case LINE: {
    Line line = (Line)shape_get_shape(shape);
    *x = (line_get_x1(line) + line_get_x2(line)) / 2.0;
    *y = (line_get_y1(line) + line_get_y2(line)) / 2.0;
    break;
  }
  case TEXT: {
    Text text = (Text)shape_get_shape(shape);
    *x = text_get_x(text);
    *y = text_get_y(text);
    break;
  }
  default:
    *x = 0.0;
    *y = 0.0;
    break;
  }
}

static const char *get_shape_type_name(ShapeType type) {
  switch (type) {
  case CIRCLE:
    return "Circulo";
  case RECTANGLE:
    return "Retangulo";
  case LINE:
    return "Linha";
  case TEXT:
    return "Texto";
  default:
    return "Desconhecido";
  }
}
