/*
 * 31.3.18 Felix Weilbach
 */

/**
 * SECTION:pad-canvas-item-polyline
 * @Short_description: The canvas item polyline object.
 * @Title: PadCanvasItemPolyline
 *
 * Canvas item that draws a polyline on #PadCanvas.
 */

#include "pad-canvas-item-polyline.h"
#include "pad-point.h"

typedef struct _PolylinePoint {

  PadPoint *pt;
  gdouble line_width;

} PolylinePoint;

typedef struct _PadCanvasItemPolylinePrivate {

  GList *points_list;

} PadCanvasItemPolylinePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(PadCanvasItemPolyline, pad_canvas_item_polyline,
                           PAD_TYPE_CANVAS_ITEM)

static void pad_canvas_item_polyline_finalize(GObject *gobject) {
  G_OBJECT_CLASS(pad_canvas_item_polyline_parent_class)->finalize(gobject);
}

static void pad_canvas_item_polyline_dispose(GObject *gobject) {
  PadCanvasItemPolylinePrivate *priv =
      pad_canvas_item_polyline_get_instance_private(
          PAD_CANVAS_ITEM_POLYLINE(gobject));

  if (priv->points_list) {
    for (GList *l = priv->points_list; l != NULL; l = l->next) {
      g_slice_free(PolylinePoint, l->data);
    }

    g_list_free(priv->points_list);
    priv->points_list = NULL;
  }

  G_OBJECT_CLASS(pad_canvas_item_polyline_parent_class)->dispose(gobject);
}

static void
pad_canvas_item_polyline_class_init(PadCanvasItemPolylineClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  PadCanvasItemClass *canvas_item_class = PAD_CANVAS_ITEM_CLASS(klass);

  gobject_class->dispose = pad_canvas_item_polyline_dispose;
  gobject_class->finalize = pad_canvas_item_polyline_finalize;

  canvas_item_class->draw = pad_canvas_item_polyline_draw;
  canvas_item_class->add = pad_canvas_item_polyline_add;
  canvas_item_class->update_bounding_box = pad_canvas_item_polyline_update_bounding_box;
}

static void pad_canvas_item_polyline_init(PadCanvasItemPolyline *self) {}

PadCanvasItem *pad_canvas_item_polyline_new(PadCanvasItem *parent_item, ...) {
  va_list var_args;
  const char *first_property;
  PadCanvasItem *item = g_object_new(PAD_TYPE_CANVAS_ITEM_POLYLINE,
                                     "parent-item", parent_item, NULL);

  va_start(var_args, parent_item);
  first_property = va_arg(var_args, char *);
  if (first_property) {
    g_object_set_valist(G_OBJECT(item), first_property, var_args);
  }
  va_end(var_args);

  return item;
}

/**
 * pad_canvas_item_polyline_add_point:
 * @self The #PadCanvasItemPolyline you want to use
 * @x X coordinate in world space
 * @y Y coordinate in world space
 * @line_width Line width
 *
 * Adds a new point to the polyline.
 */
void pad_canvas_item_polyline_add_point(PadCanvasItemPolyline *self, gdouble x,
                                        gdouble y, gdouble line_width) {
  g_return_if_fail(PAD_IS_CANVAS_ITEM_POLYLINE(self));

  PolylinePoint *point;
  PadCanvasItemPolylinePrivate *priv =
      pad_canvas_item_polyline_get_instance_private(self);

  point = g_slice_alloc(sizeof(PolylinePoint));
  point->pt = pad_point_new(x, y);
  point->line_width = line_width;

  priv->points_list = g_list_append(priv->points_list, point);

  pad_canvas_item_bounding_box_expand_to_point(PAD_CANVAS_ITEM(self), point->pt);
}

void pad_canvas_item_polyline_draw(PadCanvasItem *self, cairo_t *cr) {
  g_return_if_fail(PAD_IS_CANVAS_ITEM_POLYLINE(self));

  PadCanvasItemPolylinePrivate *priv =
      pad_canvas_item_polyline_get_instance_private(
          PAD_CANVAS_ITEM_POLYLINE(self));

  if (!priv->points_list) {
    return;
  }

  cairo_save(cr);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

  GList *l = priv->points_list;
  l = l->next;
  for (; l != NULL; l = l->next) {
    gdouble pt1_x, pt1_y, pt2_x, pt2_y;
    PolylinePoint *point1 = (PolylinePoint *)l->prev->data;
    PolylinePoint *point2 = (PolylinePoint *)l->data;

    g_object_get(point1->pt, "x", &pt1_x, "y", &pt1_y, NULL);
    g_object_get(point2->pt, "x", &pt2_x, "y", &pt2_y, NULL);

    cairo_set_line_width(cr, point1->line_width);
    cairo_line_to(cr, pt1_x, pt1_y);
    cairo_line_to(cr, pt2_x, pt2_y);
    cairo_stroke(cr);
  }
  cairo_restore(cr);
}

void pad_canvas_item_polyline_add(PadCanvasItem *self, PadCanvasItem *child) {
  self->child = child;
}

void pad_canvas_item_polyline_update_bounding_box(PadCanvasItem *self) {}
