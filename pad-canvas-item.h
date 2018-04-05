/*
 * 2.4.18 Felix Weilbach
 */

#ifndef __PAD_CANVAS_ITEM_H__
#define __PAD_CANVAS_ITEM_H__

#include <gtk.h>
//#include "pad-canvas.h"

G_BEGIN_DECLS

#define PAD_TYPE_CANVAS_ITEM pad_canvas_item_get_type()
G_DECLARE_DERIVABLE_TYPE(PadCanvasItem, pad_canvas_item, PAD, CANVAS_ITEM,
                         GObject);

typedef struct _PadCanvasItemClass {
  GObjectClass parent_class;

  void (*draw) (PadCanvasItem *self, cairo_t *cr);

  gpointer padding[30];

} PadCanvasItemClass;

PadCanvasItem *pad_canvas_item_new(PadCanvasItem *parent_item);

void pad_canvas_item_draw(PadCanvasItem *self, cairo_t *cr);

G_END_DECLS

#endif /* __PAD_CANVAS_ITEM_H__ */
