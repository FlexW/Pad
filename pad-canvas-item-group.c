/*
 * 31.3.18 Felix Weilbach
 */

/**
 * SECTION:pad-canvas-item-group
 * @Short_description: The canvas item group object.
 * @Title: PadCanvasItemGroup
 *
 * Canvas item that groups together multiple #PadCanvasItem.
 */

#include "pad-canvas-item-group.h"

typedef struct _PadCanvasItemGroupPrivate {

  GList *item_list;

} PadCanvasItemGroupPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(PadCanvasItemGroup, pad_canvas_item_group,
                           PAD_TYPE_CANVAS_ITEM)

static void pad_canvas_item_group_finalize(GObject *gobject) {
  G_OBJECT_CLASS(pad_canvas_item_group_parent_class)->finalize(gobject);
}

static void pad_canvas_item_group_dispose(GObject *gobject) {
  PadCanvasItemGroup *canvas = PAD_CANVAS_ITEM_GROUP(gobject);
  PadCanvasItemGroupPrivate *priv =
      pad_canvas_item_group_get_instance_private(canvas);

  if (priv->item_list) {
    for (GList *l = priv->item_list; l != NULL; l = l->next) {
      PadCanvasItem *item = PAD_CANVAS_ITEM(l->data);
      g_object_unref(item);
    }
    g_list_free(priv->item_list);
  }

  G_OBJECT_CLASS(pad_canvas_item_group_parent_class)->dispose(gobject);
}

static void pad_canvas_item_group_class_init(PadCanvasItemGroupClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  PadCanvasItemClass *canvas_item_class = PAD_CANVAS_ITEM_CLASS(klass);

  gobject_class->dispose = pad_canvas_item_group_dispose;
  gobject_class->finalize = pad_canvas_item_group_finalize;

  canvas_item_class->draw = pad_canvas_item_group_draw;
  canvas_item_class->add = pad_canvas_item_group_add;
}

static void pad_canvas_item_group_init(PadCanvasItemGroup *self) {}

/**
 * pad_canvas_item_group_new:
 *
 * Creates a new #PadCanvasItemGroup
 * @parent_item (allow-none) The items parent
 * @...: Optional pairs of property names and values and a terminating %NULL
 *
 * Returns: The newly created #PadCanvasItemGroup.
 */
PadCanvasItem *pad_canvas_item_group_new(PadCanvasItem *parent_item, ...) {
  va_list var_args;
  const char *first_property;
  PadCanvasItem *item = g_object_new(PAD_TYPE_CANVAS_ITEM_GROUP, "parent-item",
                                     parent_item, NULL);

  va_start(var_args, parent_item);
  first_property = va_arg(var_args, char *);
  if (first_property) {
    g_object_set_valist(G_OBJECT(item), first_property, var_args);
  }
  va_end(var_args);

  return item;
}

void pad_canvas_item_group_add(PadCanvasItem *self, PadCanvasItem *item) {
  g_return_if_fail(PAD_IS_CANVAS_ITEM_GROUP(self));

  PadCanvasItemGroupPrivate *priv =
      pad_canvas_item_group_get_instance_private(PAD_CANVAS_ITEM_GROUP(self));

  priv->item_list = g_list_append(priv->item_list, item);
}

void pad_canvas_item_group_draw(PadCanvasItem *self, cairo_t *cr,
                                PadCanvasDrawArea *draw_area) {
  g_return_if_fail(PAD_IS_CANVAS_ITEM_GROUP(self));

  PadCanvasItemGroupPrivate *priv =
      pad_canvas_item_group_get_instance_private(PAD_CANVAS_ITEM_GROUP(self));

  for (GList *l = priv->item_list; l != NULL; l = l->next) {
    PadCanvasItem *item = PAD_CANVAS_ITEM(l->data);

    pad_canvas_item_draw(item, cr, draw_area);
  }
}
