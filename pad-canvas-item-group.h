/*
 * 2.4.18 Felix Weilbach
 */

#ifndef __PAD_CANVAS_ITEM_GROUP_H__
#define __PAD_CANVAS_ITEM_GROUP_H__

#include "pad-canvas-item.h"

G_BEGIN_DECLS

#define PAD_TYPE_CANVAS_ITEM_GROUP pad_canvas_item_group_get_type()
G_DECLARE_DERIVABLE_TYPE(PadCanvasItemGroup, pad_canvas_item_group, PAD,
                         CANVAS_ITEM_GROUP, PadCanvasItem);

typedef struct _PadCanvasItemGroupClass {
  PadCanvasItemClass parent_class;

  gpointer padding[35];

} PadCanvasItemGroupClass;

PadCanvasItem *pad_canvas_item_group_new(PadCanvasItem *parent_item, ...);

void pad_canvas_item_group_draw(PadCanvasItem *self, cairo_t *cr,
                                PadCanvasDrawArea *draw_area);

void pad_canvas_item_group_add(PadCanvasItem *self,
                               PadCanvasItem *item);

G_END_DECLS

#endif /* __PAD_CANVAS_ITEM_GROUP__ */
