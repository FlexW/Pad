/*
 * 2.4.18 Felix Weilbach
 */

#ifndef __PAD_CANVAS_ITEM_POLYLINE_H__
#define __PAD_CANVAS_ITEM_POLYLINE_H__

#include "pad-canvas-item.h"

G_BEGIN_DECLS

#define PAD_TYPE_CANVAS_ITEM_POLYLINE pad_canvas_item_polyline_get_type()
G_DECLARE_DERIVABLE_TYPE(PadCanvasItemPolyline, pad_canvas_item_polyline, PAD,
                         CANVAS_ITEM_POLYLINE, PadCanvasItem);

typedef struct _PadCanvasItemPolylineClass {
  PadCanvasItemClass parent_class;

  gpointer padding[30];

} PadCanvasItemPolylineClass;

PadCanvasItem *pad_canvas_item_polyline_new(PadCanvasItem *parent_item);

void pad_canvas_item_polyline_add_point(PadCanvasItemPolyline *self, gdouble x,
                                        gdouble y, gdouble line_width);

void pad_canvas_item_polyline_draw(PadCanvasItem *self, cairo_t *cr);

G_END_DECLS

#endif /* __PAD_CANVAS_ITEM_POLYLINE__ */
