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

typedef struct _PolylinePoint {

  gdouble x;
  gdouble y;
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
}

static void pad_canvas_item_polyline_init(PadCanvasItemPolyline *self) {}

PadCanvasItem *pad_canvas_item_polyline_new(PadCanvasItem *parent_item) {
  PadCanvasItem *canvas_item_polyline =
      g_object_new(PAD_TYPE_CANVAS_ITEM_POLYLINE, NULL);

  return canvas_item_polyline;
}

void pad_canvas_item_polyline_add_point(PadCanvasItemPolyline *self, gdouble x,
                                        gdouble y, gdouble line_width) {
  PolylinePoint *point;
  PadCanvasItemPolylinePrivate *priv =
      pad_canvas_item_polyline_get_instance_private(self);

  point = g_slice_alloc(sizeof(PolylinePoint));
  point->x = x;
  point->y = y;
  point->line_width = line_width;

  priv->points_list = g_list_append(priv->points_list, point);
}

void pad_canvas_item_polyline_draw(PadCanvasItem *self, cairo_t *cr,
                                   PadCanvasDrawArea *draw_area) {
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
    PolylinePoint *point1 = (PolylinePoint *)l->prev->data;
    PolylinePoint *point2 = (PolylinePoint *)l->data;

    cairo_set_line_width(cr, point1->line_width);
    cairo_line_to(cr, point1->x, point1->y);
    cairo_line_to(cr, point2->x, point2->y);
    cairo_stroke(cr);
  }
  cairo_restore(cr);
}
