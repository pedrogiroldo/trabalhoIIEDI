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
#include "../visibility/geometry.h"
#include "../visibility/visibility.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to hold visibility polygon data for accumulation
typedef struct {
  VisibilityPolygon polygon;
  double source_x;
  double source_y;
} VisPolygonData;

// Private helper functions
static void execute_anteparo_command(City city, FILE *txt_output);
static void execute_destruction_bomb(City city, const char *output_path,
                                     FileData geo_file_data,
                                     FileData qry_file_data, const char *suffix,
                                     FILE *txt_output, SortType sort_type,
                                     int sort_threshold,
                                     List accumulated_polygons);
static void execute_painting_bomb(City city, const char *output_path,
                                  FileData geo_file_data,
                                  FileData qry_file_data, const char *suffix,
                                  FILE *txt_output, SortType sort_type,
                                  int sort_threshold,
                                  List accumulated_polygons);
static void execute_cloning_bomb(City city, const char *output_path,
                                 FileData geo_file_data, FileData qry_file_data,
                                 const char *suffix, FILE *txt_output,
                                 SortType sort_type, int sort_threshold,
                                 List accumulated_polygons);
static bool shape_in_visibility_region(Shape shape, VisibilityPolygon polygon);
static const char *get_shape_type_name(ShapeType type);

// Visibility check helpers
static bool is_segment_visible(double x1, double y1, double x2, double y2,
                               VisibilityPolygon polygon);
static bool is_rectangle_visible(Rectangle rect, VisibilityPolygon polygon);
static bool is_circle_visible(Circle circle, VisibilityPolygon polygon);
static bool is_point_in_rect(double px, double py, Rectangle rect);

void qry_handler_process_file(City city, FileData geo_file_data,
                              FileData qry_file_data, const char *output_path,
                              SortType sort_type, int sort_threshold) {
  if (!city || !geo_file_data || !qry_file_data || !output_path) {
    return;
  }

  // Create text output file
  const char *original_file_name = get_file_name(qry_file_data);
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

  fprintf(txt_output, "Query Command Results\n");
  fprintf(txt_output, "=====================\n\n");

  // List to accumulate visibility polygons for bombs with suffix "-"
  List accumulated_polygons = list_create();

  // Process each command line
  Queue file_lines = get_file_lines_queue(qry_file_data);
  while (!queue_is_empty(file_lines)) {
    char *line = (char *)queue_dequeue(file_lines);
    char *command = strtok(line, " ");

    if (strcmp(command, "a") == 0) {
      execute_anteparo_command(city, txt_output);
    } else if (strcmp(command, "d") == 0) {
      execute_destruction_bomb(city, output_path, geo_file_data, qry_file_data,
                               NULL, txt_output, sort_type, sort_threshold,
                               accumulated_polygons);
    } else if (strcmp(command, "p") == 0) {
      execute_painting_bomb(city, output_path, geo_file_data, qry_file_data,
                            NULL, txt_output, sort_type, sort_threshold,
                            accumulated_polygons);
    } else if (strcmp(command, "cln") == 0) {
      execute_cloning_bomb(city, output_path, geo_file_data, qry_file_data,
                           NULL, txt_output, sort_type, sort_threshold,
                           accumulated_polygons);
    } else {
      fprintf(txt_output, "Unknown command: %s\n\n", command);
    }
  }

  fclose(txt_output);
  free(txt_path);

  // Generate final SVG with all modifications using geoName-qryName.svg pattern
  city_generate_qry_svg(city, output_path, geo_file_data, qry_file_data,
                        accumulated_polygons);

  // Clean up accumulated polygons
  int polygon_count = list_size(accumulated_polygons);
  for (int i = 0; i < polygon_count; i++) {
    VisPolygonData *data = (VisPolygonData *)list_get(accumulated_polygons, i);
    if (data) {
      visibility_polygon_destroy(data->polygon);
      free(data);
    }
  }
  list_destroy(accumulated_polygons);
}

static void execute_anteparo_command(City city, FILE *txt_output) {
  char *start_id_str = strtok(NULL, " ");
  char *end_id_str = strtok(NULL, " ");
  char *orientation = strtok(NULL, " ");

  if (!start_id_str || !end_id_str) {
    fprintf(txt_output, "Error: Command 'a' requires start and end IDs\n\n");
    return;
  }

  int start_id = atoi(start_id_str);
  int end_id = atoi(end_id_str);
  char orient = orientation ? orientation[0] : 'h';

  fprintf(txt_output, "Command: a %d %d %c\n", start_id, end_id, orient);
  fprintf(txt_output, "Transformed to barriers:\n");

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
                "  Circle id=%d -> Vertical segment id=%d "
                "(%.2f,%.2f)-(%.2f,%.2f)\n",
                id, new_id, cx, cy - r, cx, cy + r);
      } else {
        segment = line_create(new_id, cx - r, cy, cx + r, cy, color);
        fprintf(txt_output,
                "  Circle id=%d -> Horizontal segment id=%d "
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

      fprintf(txt_output, "  Rectangle id=%d -> Segments:", id);

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
      fprintf(txt_output, "  Line id=%d -> Marked as barrier\n", id);
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
              "  Text id=%d -> Segment id=%d (%.2f,%.2f)-(%.2f,%.2f)\n", id,
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
                                     FileData geo_file_data,
                                     FileData qry_file_data, const char *suffix,
                                     FILE *txt_output, SortType sort_type,
                                     int sort_threshold,
                                     List accumulated_polygons) {
  char *x_str = strtok(NULL, " ");
  char *y_str = strtok(NULL, " ");
  char *sfx = strtok(NULL, " ");

  if (!x_str || !y_str) {
    fprintf(txt_output, "Error: Command 'd' requires x and y coordinates\n\n");
    return;
  }

  double x = atof(x_str);
  double y = atof(y_str);

  fprintf(txt_output, "Command: d %.2f %.2f %s\n", x, y, sfx ? sfx : "-");
  fprintf(txt_output, "Destroyed shapes:\n");

  double min_x, min_y, max_x, max_y;
  city_get_bounding_box(city, &min_x, &min_y, &max_x, &max_y);
  double margin = 20.0;
  min_x -= margin;
  min_y -= margin;
  max_x += margin;
  max_y += margin;

  List barriers = city_get_barriers(city);
  VisibilityPolygon polygon =
      visibility_calculate(x, y, barriers, 1000.0, sort_type, sort_threshold,
                           min_x, min_y, max_x, max_y);

  if (!polygon) {
    fprintf(txt_output, "  Error calculating visibility region\n\n");
    list_destroy(barriers);
    return;
  }

  // Generate SVG with visibility polygon if suffix is provided and not "-"
  if (sfx != NULL && strcmp(sfx, "-") != 0) {
    city_generate_svg_with_visibility(city, output_path, geo_file_data,
                                      qry_file_data, sfx, polygon, x, y);
  } else if (sfx == NULL || strcmp(sfx, "-") == 0) {
    // Accumulate polygon for final SVG when suffix is "-"
    VisPolygonData *data = malloc(sizeof(VisPolygonData));
    if (data) {
      data->polygon = polygon;
      data->source_x = x;
      data->source_y = y;
      list_insert_back(accumulated_polygons, data);
      polygon = NULL; // Don't destroy, will be cleaned up later
    }
  }

  List shapes_to_destroy = list_create();
  List shapes_list = city_get_shapes_list(city);
  int shape_count = list_size(shapes_list);

  for (int i = 0; i < shape_count; i++) {
    Shape shape = list_get(shapes_list, i);
    VisibilityPolygon check_poly = polygon;
    if (check_poly == NULL && list_size(accumulated_polygons) > 0) {
      VisPolygonData *last =
          (VisPolygonData *)list_get_last(accumulated_polygons);
      check_poly = last->polygon;
    }
    if (shape && shape_in_visibility_region(shape, check_poly)) {
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
    fprintf(txt_output, "  No shapes destroyed\n");
  }

  list_destroy(shapes_to_destroy);
  list_destroy(barriers);
  if (polygon) {
    visibility_polygon_destroy(polygon);
  }

  fprintf(txt_output, "\n");
}

static void execute_painting_bomb(City city, const char *output_path,
                                  FileData geo_file_data,
                                  FileData qry_file_data, const char *suffix,
                                  FILE *txt_output, SortType sort_type,
                                  int sort_threshold,
                                  List accumulated_polygons) {
  char *x_str = strtok(NULL, " ");
  char *y_str = strtok(NULL, " ");
  char *color = strtok(NULL, " ");
  char *sfx = strtok(NULL, " ");

  if (!x_str || !y_str || !color) {
    fprintf(txt_output,
            "Error: Command 'p' requires x, y coordinates and color\n\n");
    return;
  }

  double x = atof(x_str);
  double y = atof(y_str);

  fprintf(txt_output, "Command: p %.2f %.2f %s %s\n", x, y, color,
          sfx ? sfx : "-");
  fprintf(txt_output, "Painted shapes:\n");

  double min_x, min_y, max_x, max_y;
  city_get_bounding_box(city, &min_x, &min_y, &max_x, &max_y);
  double margin = 20.0;
  min_x -= margin;
  min_y -= margin;
  max_x += margin;
  max_y += margin;

  List barriers = city_get_barriers(city);
  VisibilityPolygon polygon =
      visibility_calculate(x, y, barriers, 1000.0, sort_type, sort_threshold,
                           min_x, min_y, max_x, max_y);

  if (!polygon) {
    fprintf(txt_output, "  Error calculating visibility region\n\n");
    list_destroy(barriers);
    return;
  }

  // Generate SVG with visibility polygon if suffix is provided and not "-"
  if (sfx != NULL && strcmp(sfx, "-") != 0) {
    city_generate_svg_with_visibility(city, output_path, geo_file_data,
                                      qry_file_data, sfx, polygon, x, y);
  } else if (sfx == NULL || strcmp(sfx, "-") == 0) {
    // Accumulate polygon for final SVG when suffix is "-"
    VisPolygonData *data = malloc(sizeof(VisPolygonData));
    if (data) {
      data->polygon = polygon;
      data->source_x = x;
      data->source_y = y;
      list_insert_back(accumulated_polygons, data);
      polygon = NULL; // Don't destroy, will be cleaned up later
    }
  }

  List shapes_list = city_get_shapes_list(city);
  int shape_count = list_size(shapes_list);
  int painted_count = 0;

  VisibilityPolygon check_poly = polygon;
  if (check_poly == NULL && list_size(accumulated_polygons) > 0) {
    VisPolygonData *last =
        (VisPolygonData *)list_get_last(accumulated_polygons);
    check_poly = last->polygon;
  }

  for (int i = 0; i < shape_count; i++) {
    Shape shape = list_get(shapes_list, i);
    if (!shape || !shape_in_visibility_region(shape, check_poly)) {
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
      fprintf(txt_output, "  Text id=%d\n", id);
      painted_count++;
      break;
    }
    default:
      continue;
    }
  }

  if (painted_count == 0) {
    fprintf(txt_output, "  No shapes painted\n");
  }

  list_destroy(barriers);
  if (polygon) {
    visibility_polygon_destroy(polygon);
  }

  fprintf(txt_output, "\n");
}

static void execute_cloning_bomb(City city, const char *output_path,
                                 FileData geo_file_data, FileData qry_file_data,
                                 const char *suffix, FILE *txt_output,
                                 SortType sort_type, int sort_threshold,
                                 List accumulated_polygons) {
  char *x_str = strtok(NULL, " ");
  char *y_str = strtok(NULL, " ");
  char *dx_str = strtok(NULL, " ");
  char *dy_str = strtok(NULL, " ");
  char *sfx = strtok(NULL, " ");

  if (!x_str || !y_str || !dx_str || !dy_str) {
    fprintf(txt_output,
            "Error: Command 'cln' requires x, y, dx, dy coordinates\n\n");
    return;
  }

  double x = atof(x_str);
  double y = atof(y_str);
  double dx = atof(dx_str);
  double dy = atof(dy_str);

  fprintf(txt_output, "Command: cln %.2f %.2f %.2f %.2f %s\n", x, y, dx, dy,
          sfx ? sfx : "-");
  fprintf(txt_output, "Cloned shapes:\n");

  double min_x, min_y, max_x, max_y;
  city_get_bounding_box(city, &min_x, &min_y, &max_x, &max_y);
  double margin = 20.0;
  min_x -= margin;
  min_y -= margin;
  max_x += margin;
  max_y += margin;

  List barriers = city_get_barriers(city);
  VisibilityPolygon polygon =
      visibility_calculate(x, y, barriers, 1000.0, sort_type, sort_threshold,
                           min_x, min_y, max_x, max_y);

  if (!polygon) {
    fprintf(txt_output, "  Error calculating visibility region\n\n");
    list_destroy(barriers);
    return;
  }

  // Generate SVG with visibility polygon if suffix is provided and not "-"
  if (sfx != NULL && strcmp(sfx, "-") != 0) {
    city_generate_svg_with_visibility(city, output_path, geo_file_data,
                                      qry_file_data, sfx, polygon, x, y);
  } else if (sfx == NULL || strcmp(sfx, "-") == 0) {
    // Accumulate polygon for final SVG when suffix is "-"
    VisPolygonData *data = malloc(sizeof(VisPolygonData));
    if (data) {
      data->polygon = polygon;
      data->source_x = x;
      data->source_y = y;
      list_insert_back(accumulated_polygons, data);
      polygon = NULL; // Don't destroy, will be cleaned up later
    }
  }

  List shapes_to_clone = list_create();
  List shapes_list = city_get_shapes_list(city);
  int shape_count = list_size(shapes_list);

  VisibilityPolygon check_poly = polygon;
  if (check_poly == NULL && list_size(accumulated_polygons) > 0) {
    VisPolygonData *last =
        (VisPolygonData *)list_get_last(accumulated_polygons);
    check_poly = last->polygon;
  }

  for (int i = 0; i < shape_count; i++) {
    Shape shape = list_get(shapes_list, i);
    if (shape && shape_in_visibility_region(shape, check_poly)) {
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
    fprintf(txt_output, "  No shapes cloned\n");
  }

  list_destroy(shapes_to_clone);
  list_destroy(barriers);
  if (polygon) {
    visibility_polygon_destroy(polygon);
  }

  fprintf(txt_output, "\n");
}

static bool shape_in_visibility_region(Shape shape, VisibilityPolygon polygon) {
  if (!shape || !polygon) {
    return false;
  }

  ShapeType type = shape_get_type(shape);

  switch (type) {
  case CIRCLE:
    return is_circle_visible((Circle)shape_get_shape(shape), polygon);
  case RECTANGLE:
    return is_rectangle_visible((Rectangle)shape_get_shape(shape), polygon);
  case LINE: {
    Line line = (Line)shape_get_shape(shape);
    return is_segment_visible(line_get_x1(line), line_get_y1(line),
                              line_get_x2(line), line_get_y2(line), polygon);
  }
  case TEXT: {
    Text text = (Text)shape_get_shape(shape);
    // Approximate text as a point at its anchor for now, or small segment
    // Guide says text is treated as segment for barriers, but for visibility?
    // Assuming point check at anchor is sufficient for now, or use segment
    // logic if needed. Let's use point check for simplicity unless specified
    // otherwise.
    return visibility_polygon_contains_point(polygon, text_get_x(text),
                                             text_get_y(text));
  }
  default:
    return false;
  }
}

// Helper to check if a point lies on a segment (within tolerance)
static bool point_on_segment(double px, double py, double x1, double y1,
                             double x2, double y2) {
  // Check collinearity using cross product
  double cross = (py - y1) * (x2 - x1) - (px - x1) * (y2 - y1);
  if (fabs(cross) > 1e-6) {
    return false; // Not collinear
  }

  // Check if point is within segment bounds
  double min_x = fmin(x1, x2) - 1e-6;
  double max_x = fmax(x1, x2) + 1e-6;
  double min_y = fmin(y1, y2) - 1e-6;
  double max_y = fmax(y1, y2) + 1e-6;

  return px >= min_x && px <= max_x && py >= min_y && py <= max_y;
}

// Check if a point is on the boundary of the polygon
static bool point_on_polygon_boundary(double px, double py,
                                      VisibilityPolygon polygon) {
  Point *vertices = visibility_polygon_get_vertices(polygon);
  int count = visibility_polygon_get_vertex_count(polygon);

  for (int i = 0; i < count; i++) {
    int j = (i + 1) % count;
    double vx1 = geometry_point_get_x(vertices[i]);
    double vy1 = geometry_point_get_y(vertices[i]);
    double vx2 = geometry_point_get_x(vertices[j]);
    double vy2 = geometry_point_get_y(vertices[j]);

    if (point_on_segment(px, py, vx1, vy1, vx2, vy2)) {
      return true;
    }
  }
  return false;
}

static bool is_segment_visible(double x1, double y1, double x2, double y2,
                               VisibilityPolygon polygon) {
  Point *vertices = visibility_polygon_get_vertices(polygon);
  int count = visibility_polygon_get_vertex_count(polygon);

  // 1. Check if endpoints are inside
  if (visibility_polygon_contains_point(polygon, x1, y1) ||
      visibility_polygon_contains_point(polygon, x2, y2)) {
    return true;
  }

  // 2. Check if endpoints are on the polygon boundary
  if (point_on_polygon_boundary(x1, y1, polygon) ||
      point_on_polygon_boundary(x2, y2, polygon)) {
    return true;
  }

  // 3. Check intersection with polygon edges
  for (int i = 0; i < count; i++) {
    int j = (i + 1) % count;
    double px1 = geometry_point_get_x(vertices[i]);
    double py1 = geometry_point_get_y(vertices[i]);
    double px2 = geometry_point_get_x(vertices[j]);
    double py2 = geometry_point_get_y(vertices[j]);

    if (geometry_segment_intersects_segment(x1, y1, x2, y2, px1, py1, px2,
                                            py2)) {
      return true;
    }
  }

  return false;
}

static bool is_point_in_rect(double px, double py, Rectangle rect) {
  double rx = rectangle_get_x(rect);
  double ry = rectangle_get_y(rect);
  double rw = rectangle_get_width(rect);
  double rh = rectangle_get_height(rect);
  return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
}

static bool is_rectangle_visible(Rectangle rect, VisibilityPolygon polygon) {
  double rx = rectangle_get_x(rect);
  double ry = rectangle_get_y(rect);
  double rw = rectangle_get_width(rect);
  double rh = rectangle_get_height(rect);

  // Vertices of rectangle
  double vx[4] = {rx, rx + rw, rx + rw, rx};
  double vy[4] = {ry, ry, ry + rh, ry + rh};

  // 1. Check if any rect vertex is inside polygon
  for (int i = 0; i < 4; i++) {
    if (visibility_polygon_contains_point(polygon, vx[i], vy[i])) {
      return true;
    }
  }

  Point *poly_verts = visibility_polygon_get_vertices(polygon);
  int count = visibility_polygon_get_vertex_count(polygon);

  // 2. Check if any polygon vertex is inside rect
  for (int i = 0; i < count; i++) {
    if (is_point_in_rect(geometry_point_get_x(poly_verts[i]),
                         geometry_point_get_y(poly_verts[i]), rect)) {
      return true;
    }
  }

  // 3. Check intersection of edges
  for (int i = 0; i < 4; i++) {
    double r_x1 = vx[i];
    double r_y1 = vy[i];
    double r_x2 = vx[(i + 1) % 4];
    double r_y2 = vy[(i + 1) % 4];

    for (int j = 0; j < count; j++) {
      int k = (j + 1) % count;
      double p_x1 = geometry_point_get_x(poly_verts[j]);
      double p_y1 = geometry_point_get_y(poly_verts[j]);
      double p_x2 = geometry_point_get_x(poly_verts[k]);
      double p_y2 = geometry_point_get_y(poly_verts[k]);

      if (geometry_segment_intersects_segment(r_x1, r_y1, r_x2, r_y2, p_x1,
                                              p_y1, p_x2, p_y2)) {
        return true;
      }
    }
  }

  return false;
}

static bool is_circle_visible(Circle circle, VisibilityPolygon polygon) {
  double cx = circle_get_x(circle);
  double cy = circle_get_y(circle);
  double r = circle_get_radius(circle);

  // 1. Check if center is inside
  if (visibility_polygon_contains_point(polygon, cx, cy)) {
    return true;
  }

  Point *poly_verts = visibility_polygon_get_vertices(polygon);
  int count = visibility_polygon_get_vertex_count(polygon);

  // 2. Check if any polygon vertex is inside circle
  for (int i = 0; i < count; i++) {
    double px = geometry_point_get_x(poly_verts[i]);
    double py = geometry_point_get_y(poly_verts[i]);
    if (geometry_distance(cx, cy, px, py) <= r) {
      return true;
    }
  }

  // 3. Check if any polygon edge intersects circle (dist to center <= r)
  for (int i = 0; i < count; i++) {
    int j = (i + 1) % count;
    double px1 = geometry_point_get_x(poly_verts[i]);
    double py1 = geometry_point_get_y(poly_verts[i]);
    double px2 = geometry_point_get_x(poly_verts[j]);
    double py2 = geometry_point_get_y(poly_verts[j]);

    if (geometry_distance_point_segment(cx, cy, px1, py1, px2, py2) <= r) {
      return true;
    }
  }

  return false;
}

static const char *get_shape_type_name(ShapeType type) {
  switch (type) {
  case CIRCLE:
    return "Circle";
  case RECTANGLE:
    return "Rectangle";
  case LINE:
    return "Line";
  case TEXT:
    return "Text";
  default:
    return "Unknown";
  }
}
