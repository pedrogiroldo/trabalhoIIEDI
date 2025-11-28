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
