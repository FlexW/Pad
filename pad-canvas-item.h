/*
 * 2.4.18 Felix Weilbach
 */

#ifndef __PAD_CANVAS_ITEM_H__
#define __PAD_CANVAS_ITEM_H__

#include <gtk.h>

//#include "pad-canvas.h"

typedef struct _PadCanvasDrawArea PadCanvasDrawArea;

G_BEGIN_DECLS

#define PAD_TYPE_CANVAS_ITEM pad_canvas_item_get_type()
G_DECLARE_DERIVABLE_TYPE(PadCanvasItem, pad_canvas_item, PAD, CANVAS_ITEM,
                         GObject);

/*typedef struct _PadCanvasItem {

  If this is set to FALSE, the drawing method of the item will not be called.
  gboolean need_update;

} PadCanvasItem;
*/
typedef struct _PadCanvasItemClass {
  GObjectClass parent_class;

  void (*draw)(PadCanvasItem *self, cairo_t *cr, PadCanvasDrawArea *draw_area);

  gpointer padding[30];

} PadCanvasItemClass;

PadCanvasItem *pad_canvas_item_new(PadCanvasItem *parent_item);

void pad_canvas_item_draw(PadCanvasItem *self, cairo_t *cr,
                          PadCanvasDrawArea *draw_area);

G_END_DECLS

#endif /* __PAD_CANVAS_ITEM_H__ */
