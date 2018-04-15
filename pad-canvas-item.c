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
#include "pad-bounding-box.h"

typedef struct _PadCanvasItemPrivate {

  gboolean init_bounding_box;

} PadCanvasItemPrivate;

enum {
  PROP_WORLD_X = 1,
  PROP_WORLD_Y,
  PROP_PARENT_ITEM,
  PROP_ITEM_BOUNDS,
  N_PROPERTIES
};

G_DEFINE_TYPE_WITH_PRIVATE(PadCanvasItem, pad_canvas_item, G_TYPE_OBJECT);

static void pad_canvas_item_finalize(GObject *gobject) {
  G_OBJECT_CLASS(pad_canvas_item_parent_class)->finalize(gobject);
}

static void pad_canvas_item_dispose(GObject *gobject) {
  PadCanvasItem *item = PAD_CANVAS_ITEM(gobject);

  if (item->child) {
    g_clear_object(&item->child);
  }

  if (item->bounding_box) {
    g_clear_object(&item->bounding_box);
  }

  G_OBJECT_CLASS(pad_canvas_item_parent_class)->dispose(gobject);
}

static void pad_canvas_item_set_property(GObject *object, guint prop_id,
                                         const GValue *value,
                                         GParamSpec *pspec) {
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

  case PROP_PARENT_ITEM:
    item->parent_item = g_value_get_object(value);
    pad_canvas_item_add(item->parent_item, item);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_canvas_item_get_property(GObject *object, guint prop_id,
                                         GValue *value, GParamSpec *pspec) {
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

  case PROP_PARENT_ITEM:
    g_value_set_object(value, item->parent_item);
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

  g_object_class_install_property(
      gobject_class, PROP_PARENT_ITEM,
      g_param_spec_object("parent-item", "Parent item", "The items parent",
                          PAD_TYPE_CANVAS_ITEM,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void pad_canvas_item_init(PadCanvasItem *self) {
  PadCanvasItemPrivate *priv = pad_canvas_item_get_instance_private(self);

  self->bounding_box = pad_bounding_box_new(0, 0, 0, 0);
  priv->init_bounding_box = TRUE;
}

/**
 * pad_canvas_item_new:
 *
 * Creates a new #PadCanvasItem
 * @parent_item (allow-none) The items parent
 * @...: Optional pairs of property names and values and a terminating %NULL
 *
 * Returns: The newly created #PadCanvasItem.
 */
PadCanvasItem *pad_canvas_item_new(PadCanvasItem *parent_item, ...) {
  PadCanvasItem *item =
      g_object_new(PAD_TYPE_CANVAS_ITEM, "parent-item", parent_item, NULL);
  va_list var_args;
  const char *first_property;

  va_start(var_args, parent_item);
  first_property = va_arg(var_args, char *);
  if (first_property) {
    g_object_set_valist(G_OBJECT(item), first_property, var_args);
  }
  va_end(var_args);

  return item;
}

/**
 * pad_canvas_item_draw:
 * @self The #PadCanvasItem you want to draw
 * @cr The cairo context on that will be drawn
 * @draw_area The area in world coordinates that is visble to the user
 *
 * This function must be implemented by all subclasses of #PadCanvasItem.
 * It gets called from the #PadCanvas draw method. The received cairo context is
 * already transformed from the world coordinate system.
 */
void pad_canvas_item_draw(PadCanvasItem *self, cairo_t *cr) {
  PadCanvasItemClass *klass;

  g_return_if_fail(PAD_IS_CANVAS_ITEM(self));

  klass = PAD_CANVAS_ITEM_GET_CLASS(self);
  g_return_if_fail(klass->draw != NULL);

  klass->draw(self, cr);
}

/**
 * pad_canvas_item_add:
 * @self (allow-none) The #PadCanvasItem you want to use
 * @child (allow-none) The #PadCanvasItem that should be added.
 *
 * This function must be implemented by all subclasses of #PadCanvasItem.
 * It adds the child as a child of self. As a minimum the child should be
 * assigned to self->child (The previous child will be unrefed automatically),
 * it then gets freed if the object gets destroyed.
 */
void pad_canvas_item_add(PadCanvasItem *self, PadCanvasItem *child) {
  PadCanvasItemClass *klass;

  if (self == NULL) {
    return;
  }

  g_return_if_fail(PAD_IS_CANVAS_ITEM(self));

  klass = PAD_CANVAS_ITEM_GET_CLASS(self);
  g_return_if_fail(klass->draw != NULL);

  if (self->child) {
    g_object_unref(self->child);
  }

  klass->add(self, child);
}

void pad_canvas_item_update_bounding_box(PadCanvasItem *self) {
  PadCanvasItemClass *klass;

  g_return_if_fail(PAD_IS_CANVAS_ITEM(self));

  klass = PAD_CANVAS_ITEM_GET_CLASS(self);
  g_return_if_fail(klass->draw != NULL);

  klass->update_bounding_box(self);
}

gdouble pad_canvas_item_point_left_bound(PadPoint *pt, gdouble line_width) {
  gdouble x;

  line_width = line_width / 2;

  g_object_get(pt, "x", &x, NULL);

  x -= line_width;

  return x;
}

gdouble pad_canvas_item_point_right_bound(PadPoint *pt, gdouble line_width) {
  gdouble x;

  line_width = line_width / 2;

  g_object_get(pt, "x", &x, NULL);

  x += line_width;

  return x;
}

gdouble pad_canvas_item_point_upper_bound(PadPoint *pt, gdouble line_width) {
  gdouble y;

  line_width = line_width / 2;

  g_object_get(pt, "y", &y, NULL);

  y += line_width;

  return y;
}

gdouble pad_canvas_item_point_lower_bound(PadPoint *pt, gdouble line_width) {
  gdouble y;

  line_width = line_width / 2;

  g_object_get(pt, "y", &y, NULL);

  y -= line_width;

  return y;
}

/**
 * pad_canvas_item_bounding_box_expand_to_point:
 * @self The #PadCanvasItem you want the #PadBoundingBox to expand
 * @pt The point you want to have in the #PadBoundingBox
 *
 * A wrapper around #pad_bounding_box_expand_to_point().
 * Handles correct initialising of the #PadBoundingBox of the #PadCanvasItem.
 * Use this always instead of direct access of
 * #pad_bounding_box_expand_to_point().
 */
void pad_canvas_item_bounding_box_expand_to_point(PadCanvasItem *self,
                                                  PadPoint *pt) {
  PadCanvasItemPrivate *priv;

  g_return_if_fail(PAD_IS_CANVAS_ITEM(self));
  priv = pad_canvas_item_get_instance_private(self);

  if (priv->init_bounding_box) {
    // get pt alter x y
    PadPoint *pt_1, *pt_2;
    gdouble new_x, new_y;

    g_object_get(self->bounding_box, "pt-1", &pt_1, "pt-2", &pt_2, NULL);
    g_object_get(pt, "x", &new_x, "y", &new_y, NULL);

    g_object_set(pt_1, "x", new_x, "y", new_y, NULL);
    g_object_set(pt_2, "x", new_x, "y", new_y, NULL);

    priv->init_bounding_box = FALSE;

    return;
  }

  pad_bounding_box_expand_to_point(self->bounding_box, pt);

}

/**
 * pad_canvas_item_bounding_box_expand_to_box:
 * @self The #PadCanvasItem you want the #PadBoundingBox to expand
 * @bounding_box The bounding box you want to have in the #PadBoundingBox
 *
 * A wrapper around #pad_bounding_box_expand_to_box().
 * Handles correct initialising of the #PadBoundingBox of the #PadCanvasItem.
 * Use this always instead of direct access of
 * #pad_bounding_box_expand_to_box().
 */
void pad_canvas_item_bounding_box_expand_to_box(PadCanvasItem *self,
                                                PadBoundingBox *bounding_box) {

  PadCanvasItemPrivate *priv;

  g_return_if_fail(PAD_IS_CANVAS_ITEM(self));

  priv = pad_canvas_item_get_instance_private(self);

  if (priv->init_bounding_box) {
    PadPoint *self_pt_1, *self_pt_2, *bounding_box_pt_1, *bounding_box_pt_2;
    gdouble new_x, new_y;

    g_object_get(self->bounding_box, "pt-1", &self_pt_1, "pt-2", &self_pt_2,
                 NULL);
    g_object_get(bounding_box, "pt-1", &bounding_box_pt_1, "pt-2",
                 &bounding_box_pt_2, NULL);

    g_object_get(bounding_box_pt_1, "x", &new_x, "y", &new_y, NULL);
    g_object_set(self_pt_1, "x", new_x, "y", new_y, NULL);

    g_object_get(bounding_box_pt_2, "x", &new_x, "y", &new_y, NULL);
    g_object_set(self_pt_2, "x", new_x, "y", new_y, NULL);

    priv->init_bounding_box = FALSE;

    return;
  }

  pad_bounding_box_expand_to_box(self->bounding_box, bounding_box);

}
