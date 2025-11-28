#include "shapes.h"
#include "circle/circle.h"
#include "line/line.h"
#include "rectangle/rectangle.h"
#include "text/text.h"
#include "text_style/text_style.h"
#include <stdlib.h>

/**
 * Internal Shape structure that wraps all shape types
 */
struct ShapeWrapper {
  ShapeType type;
  void *shape;
};

/**
 * Creates a new Shape wrapper
 * @param type The type of shape
 * @param shape Pointer to the actual shape object
 * @return Pointer to new Shape wrapper or NULL on error
 */
Shape shape_create_wrapper(ShapeType type, void *shape) {
  if (!shape) {
    return NULL;
  }

  struct ShapeWrapper *wrapper = malloc(sizeof(struct ShapeWrapper));
  if (!wrapper) {
    return NULL;
  }

  wrapper->type = type;
  wrapper->shape = shape;

  return (Shape)wrapper;
}

ShapeType shape_get_type(Shape shape) {
  if (!shape) {
    return -1; // Invalid type
  }
  struct ShapeWrapper *wrapper = (struct ShapeWrapper *)shape;
  return wrapper->type;
}

void *shape_get_shape(Shape shape) {
  if (!shape) {
    return NULL;
  }
  struct ShapeWrapper *wrapper = (struct ShapeWrapper *)shape;
  return wrapper->shape;
}

void shape_destroy(Shape shape) {
  if (!shape) {
    return;
  }

  struct ShapeWrapper *wrapper = (struct ShapeWrapper *)shape;

  // Destroy the underlying shape based on its type
  switch (wrapper->type) {
  case CIRCLE:
    circle_destroy(wrapper->shape);
    break;
  case RECTANGLE:
    rectangle_destroy(wrapper->shape);
    break;
  case LINE:
    line_destroy(wrapper->shape);
    break;
  case TEXT:
    text_destroy(wrapper->shape);
    break;
  case TEXT_STYLE:
    text_style_destroy(wrapper->shape);
    break;
  }

  // Free the wrapper itself
  free(wrapper);
}

Shape shape_create_circle(int id, double x, double y, double radius,
                          const char *border_color, const char *fill_color) {
  return circle_create(id, x, y, radius, border_color, fill_color);
}

Shape shape_create_rectangle(int id, double x, double y, double width,
                             double height, const char *border_color,
                             const char *fill_color) {
  return rectangle_create(id, x, y, width, height, border_color, fill_color);
}

Shape shape_create_line(int id, double x1, double y1, double x2, double y2,
                        const char *color) {
  return line_create(id, x1, y1, x2, y2, color);
}

Shape shape_create_text(int id, double x, double y, const char *border_color,
                        const char *fill_color, char anchor, const char *text) {
  return text_create(id, x, y, border_color, fill_color, anchor, text);
}

Shape shape_create_text_style(const char *font_family, char font_weight,
                              int font_size) {
  return text_style_create(font_family, font_weight, font_size);
}
