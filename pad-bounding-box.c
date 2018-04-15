/*
 * 7.4.18 Felix Weilbach
 */

/**
 * SECTION:pad-bounding-box
 * @Short_description: A bounding box object.
 * @Title: PadBoundingBox
 *
 * The #PadBoundingBox represents a generic bounding box. I uses #PadPoint
 * intern.
 * The bounding box is represented with two points:
 * pt-1 ---------
 * |            |
 * |            |
 * |            |
 * |            |
 * ----------- pt-2
 */

#include "pad-bounding-box.h"
#include "pad-point.h"

typedef struct _PadBoundingBoxPrivate {

  PadPoint *pt_1, *pt_2;

} PadBoundingBoxPrivate;

enum { PROP_PT_1 = 1, PROP_PT_2, N_PROPERTIES };

G_DEFINE_TYPE_WITH_PRIVATE(PadBoundingBox, pad_bounding_box, G_TYPE_OBJECT);

static void pad_bounding_box_set_pt_1(PadBoundingBox *self, PadPoint *new_pt1) {
  PadBoundingBoxPrivate *priv = pad_bounding_box_get_instance_private(self);

  if (priv->pt_2) {
    if (pad_point_is_x_greater(new_pt1, priv->pt_2) &&
        pad_point_is_y_lower(new_pt1, priv->pt_2)) {
      g_warning("Not a valid point 1\n");
      return;
    }
  }
  if (priv->pt_1) {
    g_object_unref(priv->pt_1);
  }
  priv->pt_1 = new_pt1;
}

static void pad_bounding_box_set_pt_2(PadBoundingBox *self, PadPoint *new_pt2) {
  PadBoundingBoxPrivate *priv = pad_bounding_box_get_instance_private(self);

  if (priv->pt_1) {
    if (pad_point_is_x_lower(new_pt2, priv->pt_1) &&
        pad_point_is_y_greater(new_pt2, priv->pt_1)) {
      g_warning("Not a valid point 2\n");
      return;
    }
  }
  if (priv->pt_2) {
    g_object_unref(priv->pt_2);
  }
  priv->pt_2 = new_pt2;
}

static void pad_bounding_box_set_property(GObject *object, guint prop_id,
                                          const GValue *value,
                                          GParamSpec *pspec) {
  g_return_if_fail(PAD_IS_BOUNDING_BOX(object));

  switch (prop_id) {

  case PROP_PT_1:
    pad_bounding_box_set_pt_1(PAD_BOUNDING_BOX(object),
                              g_value_get_object(value));
    break;

  case PROP_PT_2:
    pad_bounding_box_set_pt_2(PAD_BOUNDING_BOX(object),
                              g_value_get_object(value));
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_bounding_box_get_property(GObject *object, guint prop_id,
                                          GValue *value, GParamSpec *pspec) {
  PadBoundingBoxPrivate *priv;

  g_return_if_fail(PAD_IS_BOUNDING_BOX(object));

  priv = pad_bounding_box_get_instance_private(PAD_BOUNDING_BOX(object));

  switch (prop_id) {

  case PROP_PT_1:
    g_value_set_object(value, priv->pt_1);
    break;

  case PROP_PT_2:
    g_value_set_object(value, priv->pt_2);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_bounding_box_dispose(GObject *gobject) {
  PadBoundingBoxPrivate *priv =
      pad_bounding_box_get_instance_private(PAD_BOUNDING_BOX(gobject));

  if (priv->pt_1) {
    g_clear_object(&priv->pt_1);
  }

  if (priv->pt_2) {
    g_clear_object(&priv->pt_2);
  }
}

static void pad_bounding_box_class_init(PadBoundingBoxClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  gobject_class->set_property = pad_bounding_box_set_property;
  gobject_class->get_property = pad_bounding_box_get_property;
  gobject_class->dispose = pad_bounding_box_dispose;

  g_object_class_install_property(
      gobject_class, PROP_PT_1,
      g_param_spec_object("pt-1", "Pt 1", "Point 1", PAD_TYPE_POINT,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(
      gobject_class, PROP_PT_2,
      g_param_spec_object("pt-2", "Pt 2", "Point 2", PAD_TYPE_POINT,
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void pad_bounding_box_init(PadBoundingBox *self) {}

/**
 * pad_bounding_box_new:
 * @pt1_x Point 1 x coordinate
 * @pt1_y Point 1 y coordinate
 * @pt4_x Point 4 x coordinate
 * @pt4_y Point 4 y coordinate
 *
 * Creates a new #PadBoundingBox
 *
 * Returns: The newly created #PadBoundingBox.
 */
PadBoundingBox *pad_bounding_box_new(gdouble pt1_x, gdouble pt1_y,
                                     gdouble pt2_x, gdouble pt2_y) {
  PadPoint *pt_1 = pad_point_new(pt1_x, pt1_y);
  PadPoint *pt_2 = pad_point_new(pt2_x, pt2_y);
  PadBoundingBox *bounding_box =
      g_object_new(PAD_TYPE_BOUNDING_BOX, "pt-1", pt_1, "pt-2", pt_2, NULL);

  return bounding_box;
}

/**
 * pad_bounding_box_is_intersect_box:
 * @self The #PadBoundingBox you want to check the intersect
 * @bounding_box The #PadBoundingBox you want to check intersect
 *
 * Checks if bounding_box intersect with the #PadBoundingBox of self.
 *
 * Returns: %TRUE on intersect else %FALSE.
 */
gboolean pad_bounding_box_is_intersect_box(PadBoundingBox *self,
                                           PadBoundingBox *bounding_box) {
  PadBoundingBoxPrivate *priv, *priv_bounding_box;
  PadPoint *a_pt1, *a_pt2, *b_pt1, *b_pt2;

  g_return_val_if_fail(PAD_IS_BOUNDING_BOX(self), FALSE);

  priv = pad_bounding_box_get_instance_private(self);
  priv_bounding_box = pad_bounding_box_get_instance_private(bounding_box);

  a_pt1 = priv->pt_1;
  a_pt2 = priv->pt_2;
  b_pt1 = priv->pt_1;
  b_pt2 = priv->pt_2;

  g_return_val_if_fail(a_pt1 && a_pt2 && b_pt1 && b_pt2, FALSE);

  if (pad_point_is_y_greater_equal(a_pt1, b_pt2) &&
      pad_point_is_y_lower_equal(a_pt2, b_pt1) &&
      pad_point_is_x_lower_equal(a_pt1, b_pt2) &&
      pad_point_is_x_greater_equal(a_pt2, b_pt1)) {
    return TRUE;
  }

  return FALSE;
}

/**
 * pad_bounding_box_is_intersect_point:
 * @self The #PadBoundingBox you want to check the intersect
 * @pt The Point you want to check intersect
 *
 * Checks if pt intersect with the #PadBoundingBox of self.
 *
 * Returns: %TRUE on intersect else %FALSE.
 */
gboolean pad_bounding_box_is_intersect_point(PadBoundingBox *self,
                                             PadPoint *pt) {
  PadBoundingBoxPrivate *priv;

  g_return_val_if_fail(PAD_IS_BOUNDING_BOX(self), FALSE);

  priv = pad_bounding_box_get_instance_private(self);

  g_return_val_if_fail(priv->pt_1 && priv->pt_2, FALSE);

  if (pad_point_is_y_greater_equal(pt, priv->pt_1) &&
      pad_point_is_y_lower_equal(pt, priv->pt_2) &&
      pad_point_is_x_greater_equal(pt, priv->pt_1) &&
      pad_point_is_x_lower_equal(pt, priv->pt_2)) {
    return TRUE;
  }

  return FALSE;
}

/**
 * pad_bounding_box_expand_to_point:
 * @self The #PadBoundingBox you want to expand
 * @pt Point that should intersect the increased #PadBoundingBox
 *
 * Expands the size of the #PadBoundingBox that pt will intersect  the
 * #PadBoundingBox of self.
 */
void pad_bounding_box_expand_to_point(PadBoundingBox *self, PadPoint *pt) {
  gdouble pt_x = 0, pt_y = 0;
  PadBoundingBoxPrivate *priv;

  g_return_if_fail(PAD_IS_BOUNDING_BOX(self));

  priv = pad_bounding_box_get_instance_private(self);

  g_return_if_fail(priv->pt_1 && priv->pt_2);

  g_object_get(pt, "x", &pt_x, "y", &pt_y, NULL);

  if (pad_point_is_x_lower(pt, priv->pt_1)) {
    g_object_set(priv->pt_1, "x", pt_x, NULL);
  }
  if (pad_point_is_y_greater(pt, priv->pt_1)) {
    g_object_set(priv->pt_1, "y", pt_y, NULL);
  }
  if (pad_point_is_x_greater(pt, priv->pt_2)) {
    g_object_set(priv->pt_2, "x", pt_x, NULL);
  }
  if (pad_point_is_y_lower(pt, priv->pt_2)) {
    g_object_set(priv->pt_2, "y", pt_y, NULL);
  }
}

void pad_bounding_box_expand_to_box(PadBoundingBox *self,
                                    PadBoundingBox *bounding_box) {
  PadBoundingBoxPrivate *priv;
  PadPoint *pt1, *pt2;

  g_return_if_fail(PAD_IS_BOUNDING_BOX(self));

  priv = pad_bounding_box_get_instance_private(self);

  g_object_get(bounding_box, "pt-1", &pt1, "pt-2", &pt2, NULL);

  pad_bounding_box_expand_to_point(self, pt1);
  pad_bounding_box_expand_to_point(self, pt2);
}
