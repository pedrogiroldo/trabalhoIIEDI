/**
 * @file shapes.h
 * @brief Shape type enumeration and unified Shape ADT
 *
 * This module defines the enumeration of all geometric shape types
 * supported in the system and provides a unified Shape type that
 * wraps all specific shape types.
 */

#ifndef SHAPES_H
#define SHAPES_H

/**
 * @brief Enumeration of geometric shape types
 */
enum ShapeType {
  CIRCLE,    /**< Circle shape */
  RECTANGLE, /**< Rectangle shape */
  LINE,      /**< Line shape */
  TEXT,      /**< Text shape */
  TEXT_STYLE /**< Text style shape */
};

/**
 * @brief Type alias for ShapeType enumeration
 */
typedef enum ShapeType ShapeType;

/**
 * @brief Opaque pointer to unified Shape type
 */
typedef void *Shape;

/**
 * Gets the type of a shape
 * @param shape Shape instance
 * @return The ShapeType of the shape
 */
ShapeType shape_get_type(Shape shape);

/**
 * Gets the underlying shape object
 * @param shape Shape instance
 * @return Pointer to the underlying shape (Circle, Rectangle, Line, Text, etc.)
 */
void *shape_get_shape(Shape shape);

/**
 * Destroys a shape instance and frees all memory
 * @param shape Shape instance to destroy
 */
void shape_destroy(Shape shape);

/**
 * Creates a Shape wrapper (internal use by shape modules)
 * @param type The type of shape
 * @param shape Pointer to the actual shape object
 * @return Pointer to new Shape wrapper or NULL on error
 */
Shape shape_create_wrapper(ShapeType type, void *shape);

#endif // SHAPES_H
