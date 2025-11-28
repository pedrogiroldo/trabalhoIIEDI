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

/**
 * Creates a new Circle shape
 * @param id Circle identifier
 * @param x X coordinate of center
 * @param y Y coordinate of center
 * @param radius Circle radius
 * @param border_color Border color string
 * @param fill_color Fill color string
 * @return Pointer to new Shape wrapper or NULL on error
 */
Shape shape_create_circle(int id, double x, double y, double radius,
                          const char *border_color, const char *fill_color);

/**
 * Creates a new Rectangle shape
 * @param id Rectangle identifier
 * @param x X coordinate of anchor point
 * @param y Y coordinate of anchor point
 * @param width Rectangle width
 * @param height Rectangle height
 * @param border_color Border color string
 * @param fill_color Fill color string
 * @return Pointer to new Shape wrapper or NULL on error
 */
Shape shape_create_rectangle(int id, double x, double y, double width,
                             double height, const char *border_color,
                             const char *fill_color);

/**
 * Creates a new Line shape
 * @param id Line identifier
 * @param x1 X coordinate of start point
 * @param y1 Y coordinate of start point
 * @param x2 X coordinate of end point
 * @param y2 Y coordinate of end point
 * @param color Line color string
 * @return Pointer to new Shape wrapper or NULL on error
 */
Shape shape_create_line(int id, double x1, double y1, double x2, double y2,
                        const char *color);

/**
 * Creates a new Text shape
 * @param id Text identifier
 * @param x X coordinate of text position
 * @param y Y coordinate of text position
 * @param border_color Border color string
 * @param fill_color Fill color string
 * @param anchor Text anchor character
 * @param text Text content string
 * @return Pointer to new Shape wrapper or NULL on error
 */
Shape shape_create_text(int id, double x, double y, const char *border_color,
                        const char *fill_color, char anchor, const char *text);

/**
 * Creates a new TextStyle shape
 * @param font_family Font family string
 * @param font_weight Font weight character
 * @param font_size Font size
 * @return Pointer to new Shape wrapper or NULL on error
 */
Shape shape_create_text_style(const char *font_family, char font_weight,
                              int font_size);

#endif // SHAPES_H
