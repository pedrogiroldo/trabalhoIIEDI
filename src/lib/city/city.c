#include "city.h"
#include "../shapes/circle/circle.h"
#include "../shapes/line/line.h"
#include "../shapes/rectangle/rectangle.h"
#include "../shapes/text/text.h"
#include "../visibility/geometry.h"
#include "../visibility/visibility.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  List shapes_list;
  Stack cleanup_stack;
  List svg_list;
  int next_id; // Next available unique ID for shapes
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
  city->next_id = 1; // Start IDs from 1

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

void city_get_bounding_box(City city, double *min_x, double *min_y,
                           double *max_x, double *max_y) {
  if (!city) {
    *min_x = 0;
    *min_y = 0;
    *max_x = 1000;
    *max_y = 1000;
    return;
  }
  CityImpl *impl = (CityImpl *)city;
  List svg_list = impl->svg_list;
  *min_x = DBL_MAX;
  *min_y = DBL_MAX;
  *max_x = -DBL_MAX;
  *max_y = -DBL_MAX;

  int size = list_size(svg_list);
  if (size == 0) {
    *min_x = 0;
    *min_y = 0;
    *max_x = 1000;
    *max_y = 1000;
    return;
  }

  for (int i = 0; i < size; i++) {
    Shape shape = list_get(svg_list, i);
    if (!shape)
      continue;

    ShapeType type = shape_get_type(shape);
    double x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    if (type == CIRCLE) {
      Circle c = (Circle)shape_get_shape(shape);
      double x = circle_get_x(c);
      double y = circle_get_y(c);
      double r = circle_get_radius(c);
      x1 = x - r;
      y1 = y - r;
      x2 = x + r;
      y2 = y + r;
    } else if (type == RECTANGLE) {
      Rectangle r = (Rectangle)shape_get_shape(shape);
      x1 = rectangle_get_x(r);
      y1 = rectangle_get_y(r);
      x2 = x1 + rectangle_get_width(r);
      y2 = y1 + rectangle_get_height(r);
    } else if (type == LINE) {
      Line l = (Line)shape_get_shape(shape);
      double lx1 = line_get_x1(l);
      double ly1 = line_get_y1(l);
      double lx2 = line_get_x2(l);
      double ly2 = line_get_y2(l);
      x1 = lx1 < lx2 ? lx1 : lx2;
      y1 = ly1 < ly2 ? ly1 : ly2;
      x2 = lx1 > lx2 ? lx1 : lx2;
      y2 = ly1 > ly2 ? ly1 : ly2;
    } else if (type == TEXT) {
      Text t = (Text)shape_get_shape(shape);
      double x = text_get_x(t);
      double y = text_get_y(t);
      const char *content = text_get_text(t);
      double width = 10.0 * strlen(content);
      char anchor = text_get_anchor(t);

      if (anchor == 'i' || anchor == 'I') {
        x1 = x;
        x2 = x + width;
      } else if (anchor == 'm' || anchor == 'M') {
        x1 = x - width / 2.0;
        x2 = x + width / 2.0;
      } else { // 'f' or 'F'
        x1 = x - width;
        x2 = x;
      }
      y1 = y; // Approximation
      y2 = y; // Approximation
    } else {
      continue;
    }

    if (x1 < *min_x)
      *min_x = x1;
    if (y1 < *min_y)
      *min_y = y1;
    if (x2 > *max_x)
      *max_x = x2;
    if (y2 > *max_y)
      *max_y = y2;
  }

  // Fallback if no valid shapes were found (all NULL or continue)
  if (*min_x == DBL_MAX || *max_x == -DBL_MAX) {
    *min_x = 0;
    *min_y = 0;
    *max_x = 1000;
    *max_y = 1000;
  }
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

  double min_x, min_y, max_x, max_y;
  city_get_bounding_box(city, &min_x, &min_y, &max_x, &max_y);

  double margin = 20.0;
  double vb_x = min_x - margin;
  double vb_y = min_y - margin;
  double vb_w = (max_x - min_x) + 2 * margin;
  double vb_h = (max_y - min_y) + 2 * margin;

  fprintf(file,
          "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%.2f %.2f %.2f "
          "%.2f\">\n",
          vb_x, vb_y, vb_w, vb_h);

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

void city_generate_svg_with_visibility(City city, const char *output_path,
                                       FileData geo_file_data,
                                       FileData qry_file_data,
                                       const char *command_suffix,
                                       void *visibility_polygon,
                                       double source_x, double source_y) {
  CityImpl *impl = (CityImpl *)city;

  // Extract geo file name (without extension)
  const char *geo_original_name = get_file_name(geo_file_data);
  size_t geo_name_len = strlen(geo_original_name);
  char *geo_name = malloc(geo_name_len + 1);
  if (geo_name == NULL) {
    printf("Error: Memory allocation failed for geo file name\n");
    return;
  }
  strcpy(geo_name, geo_original_name);
  strtok(geo_name, ".");

  // Extract qry file name (without extension)
  const char *qry_original_name = get_file_name(qry_file_data);
  size_t qry_name_len = strlen(qry_original_name);
  char *qry_name = malloc(qry_name_len + 1);
  if (qry_name == NULL) {
    printf("Error: Memory allocation failed for qry file name\n");
    free(geo_name);
    return;
  }
  strcpy(qry_name, qry_original_name);
  strtok(qry_name, ".");

  // Build filename: geoName-qryName-sfx.svg
  size_t path_len = strlen(output_path);
  size_t geo_processed_len = strlen(geo_name);
  size_t qry_processed_len = strlen(qry_name);
  size_t suffix_len = command_suffix ? strlen(command_suffix) : 0;
  // output_path + "/" + geoName + "-" + qryName + "-" + sfx + ".svg" + null
  size_t total_len = path_len + 1 + geo_processed_len + 1 + qry_processed_len +
                     1 + suffix_len + 4 + 1;

  char *output_path_with_file = malloc(total_len);
  if (output_path_with_file == NULL) {
    printf("Error: Memory allocation failed\n");
    free(geo_name);
    free(qry_name);
    return;
  }

  int result = snprintf(output_path_with_file, total_len, "%s/%s-%s-%s.svg",
                        output_path, geo_name, qry_name, command_suffix);
  if (result < 0 || (size_t)result >= total_len) {
    printf("Error: Path construction failed\n");
    free(output_path_with_file);
    free(geo_name);
    free(qry_name);
    return;
  }

  FILE *file = fopen(output_path_with_file, "w");
  if (file == NULL) {
    printf("Error: Failed to open file: %s\n", output_path_with_file);
    free(output_path_with_file);
    free(geo_name);
    free(qry_name);
    return;
  }

  fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

  double min_x, min_y, max_x, max_y;
  city_get_bounding_box(city, &min_x, &min_y, &max_x, &max_y);

  // Include source point in bounding box
  if (source_x < min_x)
    min_x = source_x;
  if (source_x > max_x)
    max_x = source_x;
  if (source_y < min_y)
    min_y = source_y;
  if (source_y > max_y)
    max_y = source_y;

  double margin = 20.0;
  double vb_x = min_x - margin;
  double vb_y = min_y - margin;
  double vb_w = (max_x - min_x) + 2 * margin;
  double vb_h = (max_y - min_y) + 2 * margin;

  fprintf(file,
          "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%.2f %.2f %.2f "
          "%.2f\">\n",
          vb_x, vb_y, vb_w, vb_h);

  // Draw visibility polygon if provided
  if (visibility_polygon != NULL) {
    VisibilityPolygon poly = (VisibilityPolygon)visibility_polygon;
    int vertex_count = visibility_polygon_get_vertex_count(poly);
    Point *vertices = visibility_polygon_get_vertices(poly);

    if (vertex_count > 0 && vertices != NULL) {
      fprintf(file, "  <polygon points=\"");
      for (int i = 0; i < vertex_count; i++) {
        double x = geometry_point_get_x(vertices[i]);
        double y = geometry_point_get_y(vertices[i]);
        fprintf(file, "%.2f,%.2f ", x, y);
      }
      fprintf(file, "\" fill=\"yellow\" fill-opacity=\"0.3\" stroke=\"orange\" "
                    "stroke-width=\"2\"/>\n");
    }

    // Draw source point marker
    fprintf(file,
            "  <circle cx='%.2f' cy='%.2f' r='5' fill='red' stroke='darkred' "
            "stroke-width='2'/>\n",
            source_x, source_y);
  }

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
            "  <circle cx='%.2f' cy='%.2f' r='%.2f' fill='%s' stroke='%s'/>\n",
            circle_get_x(circle), circle_get_y(circle),
            circle_get_radius(circle), circle_get_fill_color(circle),
            circle_get_border_color(circle));
      } else if (type == RECTANGLE) {
        Rectangle rectangle = (Rectangle)shape_get_shape(shape);
        fprintf(
            file,
            "  <rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' fill='%s' "
            "stroke='%s'/>\n",
            rectangle_get_x(rectangle), rectangle_get_y(rectangle),
            rectangle_get_width(rectangle), rectangle_get_height(rectangle),
            rectangle_get_fill_color(rectangle),
            rectangle_get_border_color(rectangle));
      } else if (type == LINE) {
        Line line = (Line)shape_get_shape(shape);
        fprintf(
            file,
            "  <line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
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
                "  <text x='%.2f' y='%.2f' fill='%s' stroke='%s' "
                "text-anchor='%s'>%s</text>\n",
                text_get_x(text), text_get_y(text), text_get_fill_color(text),
                text_get_border_color(text), text_anchor, text_get_text(text));
      }
    }
  }

  fprintf(file, "</svg>\n");
  fclose(file);
  free(output_path_with_file);
  free(geo_name);
  free(qry_name);
}

List city_get_barriers(City city) {
  if (!city) {
    return NULL;
  }

  CityImpl *impl = (CityImpl *)city;
  List barriers = list_create();
  if (!barriers) {
    return NULL;
  }

  int shape_count = list_size(impl->shapes_list);
  for (int i = 0; i < shape_count; i++) {
    Shape shape = list_get(impl->shapes_list, i);
    if (shape && shape_get_type(shape) == LINE) {
      Line line = (Line)shape_get_shape(shape);
      if (line_is_barrier(line)) {
        list_insert_back(barriers, shape);
      }
    }
  }

  return barriers;
}

bool city_remove_shape(City city, Shape shape) {
  if (!city || !shape) {
    return false;
  }

  CityImpl *impl = (CityImpl *)city;

  // Remove from shapes_list
  bool removed_from_list = list_remove(impl->shapes_list, shape);

  // Remove from svg_list
  list_remove(impl->svg_list, shape);

  // Note: We don't remove from cleanup_stack as it's used for final cleanup
  // The shape will still be freed when city is destroyed

  return removed_from_list;
}

int city_get_next_id(City city) {
  if (!city) {
    return -1;
  }

  CityImpl *impl = (CityImpl *)city;
  return impl->next_id++;
}

Shape city_get_shape_by_id(City city, int id) {
  if (!city) {
    return NULL;
  }

  CityImpl *impl = (CityImpl *)city;
  int shape_count = list_size(impl->shapes_list);

  for (int i = 0; i < shape_count; i++) {
    Shape shape = list_get(impl->shapes_list, i);
    if (!shape) {
      continue;
    }

    ShapeType type = shape_get_type(shape);
    int shape_id = -1;

    switch (type) {
    case CIRCLE:
      shape_id = circle_get_id((Circle)shape_get_shape(shape));
      break;
    case RECTANGLE:
      shape_id = rectangle_get_id((Rectangle)shape_get_shape(shape));
      break;
    case LINE:
      shape_id = line_get_id((Line)shape_get_shape(shape));
      break;
    case TEXT:
      shape_id = text_get_id((Text)shape_get_shape(shape));
      break;
    default:
      continue;
    }

    if (shape_id == id) {
      return shape;
    }
  }

  return NULL;
}

void city_update_max_id(City city, int id) {
  if (!city) {
    return;
  }

  CityImpl *impl = (CityImpl *)city;
  if (id >= impl->next_id) {
    impl->next_id = id + 1;
  }
}

void city_generate_qry_svg(City city, const char *output_path,
                           FileData geo_file_data, FileData qry_file_data,
                           List accumulated_polygons) {
  CityImpl *impl = (CityImpl *)city;

  // Extract geo file name (without extension)
  const char *geo_original_name = get_file_name(geo_file_data);
  size_t geo_name_len = strlen(geo_original_name);
  char *geo_name = malloc(geo_name_len + 1);
  if (geo_name == NULL) {
    printf("Error: Memory allocation failed for geo file name\n");
    return;
  }
  strcpy(geo_name, geo_original_name);
  strtok(geo_name, ".");

  // Extract qry file name (without extension)
  const char *qry_original_name = get_file_name(qry_file_data);
  size_t qry_name_len = strlen(qry_original_name);
  char *qry_name = malloc(qry_name_len + 1);
  if (qry_name == NULL) {
    printf("Error: Memory allocation failed for qry file name\n");
    free(geo_name);
    return;
  }
  strcpy(qry_name, qry_original_name);
  strtok(qry_name, ".");

  // Build combined filename: geoName-qryName.svg
  size_t path_len = strlen(output_path);
  size_t geo_processed_len = strlen(geo_name);
  size_t qry_processed_len = strlen(qry_name);
  // output_path + "/" + geoName + "-" + qryName + ".svg" + null
  size_t total_len =
      path_len + 1 + geo_processed_len + 1 + qry_processed_len + 4 + 1;

  char *output_path_with_file = malloc(total_len);
  if (output_path_with_file == NULL) {
    printf("Error: Memory allocation failed\n");
    free(geo_name);
    free(qry_name);
    return;
  }

  int result = snprintf(output_path_with_file, total_len, "%s/%s-%s.svg",
                        output_path, geo_name, qry_name);
  if (result < 0 || (size_t)result >= total_len) {
    printf("Error: Path construction failed\n");
    free(output_path_with_file);
    free(geo_name);
    free(qry_name);
    return;
  }

  FILE *file = fopen(output_path_with_file, "w");
  if (file == NULL) {
    printf("Error: Failed to open file: %s\n", output_path_with_file);
    free(output_path_with_file);
    free(geo_name);
    free(qry_name);
    return;
  }

  fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

  double min_x, min_y, max_x, max_y;
  city_get_bounding_box(city, &min_x, &min_y, &max_x, &max_y);

  double margin = 20.0;
  double vb_x = min_x - margin;
  double vb_y = min_y - margin;
  double vb_w = (max_x - min_x) + 2 * margin;
  double vb_h = (max_y - min_y) + 2 * margin;

  fprintf(file,
          "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%.2f %.2f %.2f "
          "%.2f\">\n",
          vb_x, vb_y, vb_w, vb_h);

  // Draw accumulated visibility polygons
  if (accumulated_polygons != NULL) {
    int poly_count = list_size(accumulated_polygons);
    for (int i = 0; i < poly_count; i++) {
      // Each item is a struct with: polygon, source_x, source_y
      // We need to cast it properly - the struct is defined in qry_handler
      // For now, we access it as three consecutive doubles after the polygon
      // ptr
      void *data_ptr = list_get(accumulated_polygons, i);
      if (data_ptr == NULL)
        continue;

      // The structure layout: VisibilityPolygon polygon, double source_x,
      // double source_y
      VisibilityPolygon poly = *((VisibilityPolygon *)data_ptr);
      double *coords = (double *)((char *)data_ptr + sizeof(VisibilityPolygon));
      double source_x = coords[0];
      double source_y = coords[1];

      int vertex_count = visibility_polygon_get_vertex_count(poly);
      Point *vertices = visibility_polygon_get_vertices(poly);

      if (vertex_count > 0 && vertices != NULL) {
        fprintf(file, "  <polygon points=\"");
        for (int j = 0; j < vertex_count; j++) {
          double x = geometry_point_get_x(vertices[j]);
          double y = geometry_point_get_y(vertices[j]);
          fprintf(file, "%.2f,%.2f ", x, y);
        }
        fprintf(file,
                "\" fill=\"yellow\" fill-opacity=\"0.3\" stroke=\"orange\" "
                "stroke-width=\"2\"/>\n");
      }

      // Draw source point marker
      fprintf(file,
              "  <circle cx='%.2f' cy='%.2f' r='5' fill='red' stroke='darkred' "
              "stroke-width='2'/>\n",
              source_x, source_y);
    }
  }

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
  free(geo_name);
  free(qry_name);
}
