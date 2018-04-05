/*
 * 31.3.18 Felix Weilbach
 */

/**
 * SECTION:pad-canvas-item
 * @Short_description: The canvas item object.
 * @Title: PadCanvasItem
 *
 * Canvas item that can be drawn on a #PadCanvas.
 */

#include "pad-canvas-item.h"

typedef struct _PadCanvasItemPrivate {

  gdouble dummy;

} PadCanvasItemPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(PadCanvasItem, pad_canvas_item, G_TYPE_OBJECT)

static void pad_canvas_item_finalize(GObject *gobject) {
  G_OBJECT_CLASS(pad_canvas_item_parent_class)->finalize(gobject);
}

static void pad_canvas_item_dispose(GObject *gobject) {
  G_OBJECT_CLASS(pad_canvas_item_parent_class)->dispose(gobject);
}

static void pad_canvas_item_class_init(PadCanvasItemClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  gobject_class->dispose = pad_canvas_item_dispose;
  gobject_class->finalize = pad_canvas_item_finalize;
}

static void pad_canvas_item_init(PadCanvasItem *self) {
  self->need_update = FALSE;
}

PadCanvasItem *pad_canvas_item_new(PadCanvasItem *parent_item) {
  PadCanvasItem *item = g_object_new(PAD_TYPE_CANVAS_ITEM, NULL);

  return item;
}

gboolean pad_canvas_item_is_in_draw_area(PadCanvasItem *self,
                                         PadCanvasDrawArea *draw_area) {
  PadCanvasItemClass *klass;

  g_return_val_if_fail(PAD_CANVAS_ITEM(self), FALSE);

  klass = PAD_CANVAS_ITEM_GET_CLASS(self);
  g_return_val_if_fail(klass->is_in_draw_area != NULL, FALSE);

  return klass->is_in_draw_area(self, draw_area);
}

void pad_canvas_item_draw(PadCanvasItem *self, cairo_t *cr,
                          PadCanvasDrawArea *draw_area) {
  PadCanvasItemClass *klass;

  g_return_if_fail(PAD_IS_CANVAS_ITEM(self));

  klass = PAD_CANVAS_ITEM_GET_CLASS(self);
  g_return_if_fail(klass->draw != NULL);

  if (!pad_canvas_item_is_in_draw_area(self, draw_area)) {
    return;
  }

  if (self->need_update) {
    klass->draw(self, cr, draw_area);
  }
}
