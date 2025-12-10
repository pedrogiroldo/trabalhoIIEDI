#include "visibility.h"
#include "../commons/bst/bst.h"
#include "../commons/list/list.h"
#include "../shapes/line/line.h"
#include "../shapes/shapes.h"
#include "geometry.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct VisibilityPolygon {
  Point *vertices;
  int vertex_count;
  int capacity;
};

typedef struct {
  double x;
  double y;
} Point2D;

typedef struct Segment {
  Point2D p_initial;
  Point2D p_final;
  int id;
  BSTNode helper;
} Segment;

typedef enum { EVENT_START, EVENT_END } EventType;

typedef struct {
  Point2D point;
  EventType type;
  Segment *segment;
  double angle;
  double distance;
} Vertex;

typedef struct {
  Point2D source;
  double current_angle;
} SweepContext;

static bool add_vertex(struct VisibilityPolygon *polygon, double x, double y) {
  if (!polygon)
    return false;

  if (polygon->vertex_count >= polygon->capacity) {
    int new_capacity = polygon->capacity == 0 ? 16 : polygon->capacity * 2;
    Point *new_vertices =
        realloc(polygon->vertices, new_capacity * sizeof(Point));
    if (!new_vertices)
      return false;
    polygon->vertices = new_vertices;
    polygon->capacity = new_capacity;
  }

  if (polygon->vertex_count > 0) {
    Point last = polygon->vertices[polygon->vertex_count - 1];
    double lx = geometry_point_get_x(last);
    double ly = geometry_point_get_y(last);
    if (fabs(x - lx) < 1e-9 && fabs(y - ly) < 1e-9) {
      return true;
    }
  }

  Point point = geometry_point_create(x, y);
  if (!point)
    return false;
  polygon->vertices[polygon->vertex_count++] = point;
  printf("  ADD VERTEX %d: (%.2f, %.2f)\n", polygon->vertex_count - 1, x, y);
  return true;
}

static Point2D ray_segment_intersect(Point2D source, Point2D direction_point,
                                     Segment *s) {
  double dx = direction_point.x - source.x;
  double dy = direction_point.y - source.y;

  double x1 = s->p_initial.x, y1 = s->p_initial.y;
  double x2 = s->p_final.x, y2 = s->p_final.y;
  double sx = x2 - x1, sy = y2 - y1;

  double denom = dx * sy - dy * sx;
  if (fabs(denom) < 1e-10) {
    // Parallel - extend far in ray direction
    double len = sqrt(dx * dx + dy * dy);
    return (Point2D){source.x + dx / len * 10000, source.y + dy / len * 10000};
  }

  double t = ((x1 - source.x) * sy - (y1 - source.y) * sx) / denom;
  double u = ((x1 - source.x) * dy - (y1 - source.y) * dx) / denom;

  // Only valid if intersection is beyond direction_point (t >= 1) and on
  // segment (0 <= u <= 1)
  if (t >= 1.0 - 1e-9 && u >= -1e-9 && u <= 1.0 + 1e-9) {
    return (Point2D){source.x + t * dx, source.y + t * dy};
  }

  // No valid intersection in the forward direction - extend ray far
  double len = sqrt(dx * dx + dy * dy);
  return (Point2D){source.x + dx / len * 10000, source.y + dy / len * 10000};
}

static double calc_ray_segment_distance(Segment *s, Point2D source,
                                        double angle) {
  double dx = cos(angle);
  double dy = sin(angle);

  double x1 = s->p_initial.x, y1 = s->p_initial.y;
  double x2 = s->p_final.x, y2 = s->p_final.y;
  double sx = x2 - x1, sy = y2 - y1;

  double denom = dx * sy - dy * sx;
  if (fabs(denom) < 1e-10)
    return 1e18;

  double v1x = x1 - source.x, v1y = y1 - source.y;
  double t = (v1x * sy - v1y * sx) / denom;
  double u = (v1x * dy - v1y * dx) / denom;

  if (u < -1e-9 || u > 1.0 + 1e-9 || t < -1e-9)
    return 1e18;
  return t;
}

static int compare_segments(void *a, void *b, void *context) {
  Segment *s1 = (Segment *)a;
  Segment *s2 = (Segment *)b;
  SweepContext *ctx = (SweepContext *)context;

  if (s1 == s2)
    return 0;

  double d1 = calc_ray_segment_distance(s1, ctx->source, ctx->current_angle);
  double d2 = calc_ray_segment_distance(s2, ctx->source, ctx->current_angle);

  if (fabs(d1 - d2) > 1e-9) {
    return (d1 < d2) ? -1 : 1;
  }
  return (s1->id < s2->id) ? -1 : 1;
}

static int compare_vertices(const void *a, const void *b) {
  const Vertex *v1 = (const Vertex *)a;
  const Vertex *v2 = (const Vertex *)b;

  if (fabs(v1->angle - v2->angle) > 1e-9) {
    return (v1->angle < v2->angle) ? -1 : 1;
  }
  // When angles are equal, process START before END
  // This ensures new segment is in BST when looking for next biombo
  if (v1->type != v2->type) {
    return (v1->type == EVENT_START) ? -1 : 1;
  }
  if (fabs(v1->distance - v2->distance) > 1e-9) {
    return (v1->distance < v2->distance) ? -1 : 1;
  }
  return 0;
}

static bool is_in_front(Point2D v, double v_distance, Segment *biombo,
                        Point2D source, double angle) {
  if (!biombo)
    return true;
  double biombo_dist = calc_ray_segment_distance(biombo, source, angle);
  // If biombo doesn't intersect at current angle, v is in front
  if (biombo_dist >= 1e17)
    return true;
  return v_distance < biombo_dist - 1e-9;
}

// Helper struct for BST traversal to find closest segment
typedef struct {
  Segment *closest;
  double closest_dist;
  Point2D source;
  double angle;
} FindClosestContext;

static void find_closest_callback(void *data, void *ctx) {
  Segment *s = (Segment *)data;
  FindClosestContext *fcc = (FindClosestContext *)ctx;
  double dist = calc_ray_segment_distance(s, fcc->source, fcc->angle);
  if (dist > 0 && dist < fcc->closest_dist) {
    fcc->closest_dist = dist;
    fcc->closest = s;
  }
}

static Segment *find_closest_at_angle(BST tree, Point2D source, double angle) {
  FindClosestContext fcc = {NULL, 1e18, source, angle};
  bst_foreach(tree, find_closest_callback, &fcc);
  return fcc.closest;
}

VisibilityPolygon visibility_calculate(double x, double y, List barriers,
                                       double max_radius) {
  if (!barriers)
    return NULL;

  printf("\n=== VISIBILITY DEBUG ===\n");
  printf("Source: (%.2f, %.2f)\n", x, y);

  struct VisibilityPolygon *polygon = malloc(sizeof(struct VisibilityPolygon));
  if (!polygon)
    return NULL;
  polygon->vertices = NULL;
  polygon->vertex_count = 0;
  polygon->capacity = 0;

  Point2D source = {x, y};

  int segment_capacity = (list_size(barriers) + 4) * 2;
  int segment_count = 0;
  Segment **segments = malloc(sizeof(Segment *) * segment_capacity);

  // First pass: calculate bounding box from all barriers
  double min_x = x, max_x = x, min_y = y, max_y = y;
  int barrier_count = list_size(barriers);
  for (int i = 0; i < barrier_count; i++) {
    Shape shape = list_get(barriers, i);
    if (!shape)
      continue;
    Line l = (Line)shape_get_shape(shape);
    if (!l || !line_is_barrier(l))
      continue;

    double x1 = line_get_x1(l), y1 = line_get_y1(l);
    double x2 = line_get_x2(l), y2 = line_get_y2(l);
    if (x1 < min_x)
      min_x = x1;
    if (x2 < min_x)
      min_x = x2;
    if (x1 > max_x)
      max_x = x1;
    if (x2 > max_x)
      max_x = x2;
    if (y1 < min_y)
      min_y = y1;
    if (y2 < min_y)
      min_y = y2;
    if (y1 > max_y)
      max_y = y1;
    if (y2 > max_y)
      max_y = y2;
  }

  // Add margin around the content (fixed 50 units)
  double margin = 50;
  double box_min_x = min_x - margin;
  double box_max_x = max_x + margin;
  double box_min_y = min_y - margin;
  double box_max_y = max_y + margin;

  // Ensure bomb is inside the box
  if (x - margin < box_min_x)
    box_min_x = x - margin;
  if (x + margin > box_max_x)
    box_max_x = x + margin;
  if (y - margin < box_min_y)
    box_min_y = y - margin;
  if (y + margin > box_max_y)
    box_max_y = y + margin;

  double box[4][2] = {{box_min_x, box_min_y},
                      {box_max_x, box_min_y},
                      {box_max_x, box_max_y},
                      {box_min_x, box_max_y}};
  for (int i = 0; i < 4; i++) {
    Segment *s = malloc(sizeof(Segment));
    s->p_initial = (Point2D){box[i][0], box[i][1]};
    s->p_final = (Point2D){box[(i + 1) % 4][0], box[(i + 1) % 4][1]};
    s->id = -(i + 1);
    s->helper = NULL;
    segments[segment_count++] = s;
  }

  printf("\nBarriers:\n");
  for (int i = 0; i < barrier_count; i++) {
    Shape shape = list_get(barriers, i);
    if (!shape)
      continue;
    Line l = (Line)shape_get_shape(shape);
    if (!l || !line_is_barrier(l))
      continue;

    Segment *s = malloc(sizeof(Segment));
    s->p_initial = (Point2D){line_get_x1(l), line_get_y1(l)};
    s->p_final = (Point2D){line_get_x2(l), line_get_y2(l)};
    s->id = line_get_id(l);
    s->helper = NULL;
    segments[segment_count++] = s;
    printf("  id=%d: (%.1f,%.1f)->(%.1f,%.1f)\n", s->id, s->p_initial.x,
           s->p_initial.y, s->p_final.x, s->p_final.y);
  }

  // Angle 0 splitting
  int count_before_split = segment_count;
  for (int i = 0; i < count_before_split; i++) {
    Segment *s = segments[i];
    if ((s->p_initial.y > y && s->p_final.y < y) ||
        (s->p_initial.y < y && s->p_final.y > y)) {
      double t = (y - s->p_initial.y) / (s->p_final.y - s->p_initial.y);
      double ix = s->p_initial.x + t * (s->p_final.x - s->p_initial.x);
      if (ix > x) {
        Segment *s_new = malloc(sizeof(Segment));
        s_new->p_initial = (Point2D){ix, y};
        s_new->p_final = s->p_final;
        s_new->id = s->id;
        s_new->helper = NULL;
        s->p_final = (Point2D){ix, y};
        if (segment_count >= segment_capacity) {
          segment_capacity *= 2;
          segments = realloc(segments, sizeof(Segment *) * segment_capacity);
        }
        segments[segment_count++] = s_new;
      }
    }
  }

  Vertex *vertices = malloc(sizeof(Vertex) * segment_count * 2);
  int vertex_count = 0;

  printf("\nSegment angles:\n");
  for (int i = 0; i < segment_count; i++) {
    Segment *s = segments[i];
    double ang1 =
        geometry_calculate_angle(s->p_initial.x, s->p_initial.y, x, y);
    double ang2 = geometry_calculate_angle(s->p_final.x, s->p_final.y, x, y);
    if (ang1 < 0)
      ang1 += 2 * M_PI;
    if (ang2 < 0)
      ang2 += 2 * M_PI;

    if (ang1 > ang2) {
      Point2D tmp = s->p_initial;
      s->p_initial = s->p_final;
      s->p_final = tmp;
      double tmp_ang = ang1;
      ang1 = ang2;
      ang2 = tmp_ang;
    }

    double d1 = sqrt((s->p_initial.x - x) * (s->p_initial.x - x) +
                     (s->p_initial.y - y) * (s->p_initial.y - y));
    double d2 = sqrt((s->p_final.x - x) * (s->p_final.x - x) +
                     (s->p_final.y - y) * (s->p_final.y - y));

    if (s->id > 0) {
      printf("  id=%d: ang=[%.4f,%.4f] (%.1f,%.1f)->(%.1f,%.1f)\n", s->id, ang1,
             ang2, s->p_initial.x, s->p_initial.y, s->p_final.x, s->p_final.y);
    }

    vertices[vertex_count++] = (Vertex){s->p_initial, EVENT_START, s, ang1, d1};
    vertices[vertex_count++] = (Vertex){s->p_final, EVENT_END, s, ang2, d2};
  }

  qsort(vertices, vertex_count, sizeof(Vertex), compare_vertices);

  SweepContext ctx = {source, 0};
  BST active_segments = bst_create(compare_segments, &ctx);

  for (int i = 0; i < segment_count; i++) {
    Segment *s = segments[i];
    double dist = calc_ray_segment_distance(s, source, 1e-9);
    if (dist < 1e17 && dist > 0) {
      s->helper = bst_insert(active_segments, s);
    }
  }

  Segment *biombo = (Segment *)bst_find_min(active_segments);
  Point2D biombo_start = {0, 0};

  printf("\nInitial biombo: id=%d\n", biombo ? biombo->id : -999);

  if (biombo) {
    double dist = calc_ray_segment_distance(biombo, source, 0);
    biombo_start = (Point2D){source.x + dist, source.y};
    add_vertex(polygon, biombo_start.x, biombo_start.y);
  }

  printf("\n=== SWEEP ===\n");
  for (int i = 0; i < vertex_count; i++) {
    Vertex *v = &vertices[i];
    ctx.current_angle = v->angle;

    // Only show barrier events
    if (v->segment->id > 0) {
      double biombo_dist =
          biombo ? calc_ray_segment_distance(biombo, source, v->angle) : 1e18;
      printf("[%.4f] %s id=%d (%.1f,%.1f) dist=%.2f | biombo=%d "
             "biombo_dist=%.2f\n",
             v->angle, v->type == EVENT_START ? "START" : "END  ",
             v->segment->id, v->point.x, v->point.y, v->distance,
             biombo ? biombo->id : -999, biombo_dist);
    }

    if (v->type == EVENT_START) {
      Segment *s = v->segment;

      bool in_front =
          is_in_front(v->point, v->distance, biombo, source, v->angle);

      if (v->segment->id > 0) {
        printf("  is_in_front=%s\n", in_front ? "YES" : "NO");
      }

      if (in_front) {
        // Only add biombo intersection if biombo is valid at current angle
        // AND the intersection is closer than the current vertex
        if (biombo) {
          double biombo_dist =
              calc_ray_segment_distance(biombo, source, v->angle);
          if (biombo_dist < 1e17 && biombo_dist < v->distance - 1e-9) {
            Point2D y_inter = ray_segment_intersect(source, v->point, biombo);
            add_vertex(polygon, y_inter.x, y_inter.y);
          }
        }
        add_vertex(polygon, v->point.x, v->point.y);
        biombo = s;
        biombo_start = v->point;
        if (s->id > 0)
          printf("  -> NEW BIOMBO id=%d\n", s->id);
      }
      if (s->helper == NULL) {
        s->helper = bst_insert(active_segments, s);
      }

    } else {
      Segment *s = v->segment;

      if (s == biombo) {
        if (s->id > 0)
          printf("  Biombo ending\n");
        add_vertex(polygon, v->point.x, v->point.y);

        if (s->helper) {
          bst_remove_node(active_segments, s->helper);
          s->helper = NULL;
        }

        // Use find_closest_at_angle to find the actual closest segment at
        // current angle
        Segment *next =
            find_closest_at_angle(active_segments, source, v->angle);
        printf("  -> NEW BIOMBO id=%d\n", next ? next->id : -999);

        if (next) {
          double next_dist = calc_ray_segment_distance(next, source, v->angle);
          // For bounding box segments (id < 0), always add intersection to
          // close polygon For barrier-to-barrier, only add if next is closer
          if (next->id < 0 || next_dist < v->distance - 1e-9) {
            Point2D y_inter = ray_segment_intersect(source, v->point, next);
            add_vertex(polygon, y_inter.x, y_inter.y);
            biombo_start = y_inter;
          }
        }
        biombo = next;

      } else {
        if (s->helper) {
          bst_remove_node(active_segments, s->helper);
          s->helper = NULL;
        }
      }
    }
  }

  printf("\nTotal vertices: %d\n", polygon->vertex_count);
  printf("=== END DEBUG ===\n\n");

  bst_destroy(active_segments, NULL);
  free(vertices);
  for (int i = 0; i < segment_count; i++)
    free(segments[i]);
  free(segments);

  return (VisibilityPolygon)polygon;
}

void visibility_polygon_destroy(VisibilityPolygon polygon) {
  if (!polygon)
    return;
  struct VisibilityPolygon *poly = (struct VisibilityPolygon *)polygon;
  if (poly->vertices) {
    for (int i = 0; i < poly->vertex_count; i++)
      geometry_point_destroy(poly->vertices[i]);
    free(poly->vertices);
  }
  free(poly);
}

Point *visibility_polygon_get_vertices(VisibilityPolygon polygon) {
  if (!polygon)
    return NULL;
  return ((struct VisibilityPolygon *)polygon)->vertices;
}

int visibility_polygon_get_vertex_count(VisibilityPolygon polygon) {
  if (!polygon)
    return 0;
  return ((struct VisibilityPolygon *)polygon)->vertex_count;
}

bool visibility_polygon_contains_point(VisibilityPolygon polygon, double x,
                                       double y) {
  if (!polygon)
    return false;
  struct VisibilityPolygon *poly = (struct VisibilityPolygon *)polygon;
  return geometry_point_in_polygon(x, y, poly->vertices, poly->vertex_count);
}