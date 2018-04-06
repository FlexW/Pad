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

enum { PROP_WORLD_X = 1, PROP_WORLD_Y, N_PROPERTIES };

G_DEFINE_TYPE_WITH_PRIVATE(PadCanvasItem, pad_canvas_item, G_TYPE_OBJECT)

static void pad_canvas_item_finalize(GObject *gobject) {
  G_OBJECT_CLASS(pad_canvas_item_parent_class)->finalize(gobject);
}

static void pad_canvas_item_dispose(GObject *gobject) {
  G_OBJECT_CLASS(pad_canvas_item_parent_class)->dispose(gobject);
}

static void pad_canvas_item_set_property(GObject *object, guint prop_id,
                                         const GValue *value,
                                         GParamSpec *pspec) {
  g_print("pad_canvas_item_set_property\n");
  g_return_if_fail(PAD_IS_CANVAS_ITEM(object));

  PadCanvasItem *item = PAD_CANVAS_ITEM(object);
  PadCanvasItemPrivate *priv = pad_canvas_item_get_instance_private(item);

  switch (prop_id) {

  case PROP_WORLD_X:
    item->world_x = g_value_get_double(value);
    break;

  case PROP_WORLD_Y:
    item->world_y = g_value_get_double(value);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_canvas_item_get_property(GObject *object, guint prop_id,
                                         GValue *value, GParamSpec *pspec) {
  g_print("pad_canvas_item_get_property\n");
  g_return_if_fail(PAD_IS_CANVAS_ITEM(object));

  PadCanvasItem *item = PAD_CANVAS_ITEM(object);
  PadCanvasItemPrivate *priv = pad_canvas_item_get_instance_private(item);

  switch (prop_id) {

  case PROP_WORLD_X:
    g_value_set_double(value, item->world_x);
    break;

  case PROP_WORLD_Y:
    g_value_set_double(value, item->world_y);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_canvas_item_class_init(PadCanvasItemClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  gobject_class->dispose = pad_canvas_item_dispose;
  gobject_class->finalize = pad_canvas_item_finalize;
  gobject_class->set_property = pad_canvas_item_set_property;
  gobject_class->get_property = pad_canvas_item_get_property;

  g_object_class_install_property(
      gobject_class, PROP_WORLD_X,
      g_param_spec_double("world-x", "World X", "X Position of item in world",
                          -G_MAXDOUBLE, G_MAXDOUBLE, 0, G_PARAM_READWRITE));

  g_object_class_install_property(
      gobject_class, PROP_WORLD_Y,
      g_param_spec_double("world-y", "World Y", "Y Position of item in world",
                         -G_MAXDOUBLE, G_MAXDOUBLE, 0, G_PARAM_READWRITE));
}

static void pad_canvas_item_init(PadCanvasItem *self) {
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

  klass->draw(self, cr, draw_area);
}
