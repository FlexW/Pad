/*
 * 7.4.18 Felix Weilbach
 */

/**
 * SECTION:pad-point
 * @Short_description: A point object.
 * @Title: PadPoint
 *
 * The #PadPoint represents a generic point. It saves coordinates in %gdouble
 * values.
 */

#include "pad-point.h"

typedef struct _PadPointPrivate {

  gdouble x, y;

} PadPointPrivate;

enum { PROP_X = 1, PROP_Y, N_PROPERTIES };

G_DEFINE_TYPE_WITH_PRIVATE(PadPoint, pad_point, G_TYPE_OBJECT);

static void pad_point_set_property(GObject *object, guint prop_id,
                                   const GValue *value, GParamSpec *pspec) {
  PadPointPrivate *priv;

  g_return_if_fail(PAD_IS_POINT(object));

  priv = pad_point_get_instance_private(PAD_POINT(object));

  switch (prop_id) {

  case PROP_X:
    priv->x = g_value_get_double(value);
    break;

  case PROP_Y:
    priv->y = g_value_get_double(value);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_point_get_property(GObject *object, guint prop_id,
                                   GValue *value, GParamSpec *pspec) {
  PadPointPrivate *priv;

  g_return_if_fail(PAD_IS_POINT(object));

  priv = pad_point_get_instance_private(PAD_POINT(object));

  switch (prop_id) {

  case PROP_X:
    g_value_set_double(value, priv->x);
    break;

  case PROP_Y:
    g_value_set_double(value, priv->y);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_point_class_init(PadPointClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  gobject_class->set_property = pad_point_set_property;
  gobject_class->get_property = pad_point_get_property;

  g_object_class_install_property(
      gobject_class, PROP_X,
      g_param_spec_double("x", "X", "X coordinate", -G_MAXDOUBLE, G_MAXDOUBLE,
                          0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(
      gobject_class, PROP_Y,
      g_param_spec_double("y", "Y", "Y coordinate", -G_MAXDOUBLE, G_MAXDOUBLE,
                          0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void pad_point_init(PadPoint *self) {}

/**
 * pad_point_new:
 * @x X coordinate
 * @y Y coordinate
 * Creates a new #PadPoint
 *
 * Returns: The newly created #PadPoint.
 */
PadPoint *pad_point_new(gdouble x, gdouble y) {
  PadPoint *pt = g_object_new(PAD_TYPE_POINT, "x", x, "y", y, NULL);

  return pt;
}

/**
 * pad_point_set_max:
 * @self The point you want to set.
 * @pt The point to compare
 *
 * Sets self x and y coordinate to the maximum of pt and self.
 */
void pad_point_set_max(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_if_fail(PAD_IS_POINT(self));

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  priv->x = MAX(priv->x, pt_priv->x);
  priv->y = MAX(priv->y, pt_priv->y);
}

/**
 * pad_point_set_min:
 * @self The point you want to set.
 * @pt The point to compare
 *
 * Sets self x and y coordinate to the minimum of pt and self.
 */
void pad_point_set_min(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_if_fail(PAD_IS_POINT(self));

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  priv->x = MIN(priv->x, pt_priv->x);
  priv->y = MIN(priv->y, pt_priv->y);
}

gboolean pad_point_is_x_greater(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_val_if_fail(PAD_IS_POINT(self), FALSE);
  g_return_val_if_fail(PAD_IS_POINT(pt), FALSE);

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  return priv->x > pt_priv->x ? TRUE : FALSE;
}

gboolean pad_point_is_x_greater_equal(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_val_if_fail(PAD_IS_POINT(self), FALSE);
  g_return_val_if_fail(PAD_IS_POINT(pt), FALSE);

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  return priv->x >= pt_priv->x ? TRUE : FALSE;
}

gboolean pad_point_is_x_lower(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_val_if_fail(PAD_IS_POINT(self), FALSE);
  g_return_val_if_fail(PAD_IS_POINT(pt), FALSE);

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  return priv->x < pt_priv->x ? TRUE : FALSE;
}

gboolean pad_point_is_x_lower_equal(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_val_if_fail(PAD_IS_POINT(self), FALSE);
  g_return_val_if_fail(PAD_IS_POINT(pt), FALSE);

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  return priv->x <= pt_priv->x ? TRUE : FALSE;
}

gboolean pad_point_is_x_equal(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_val_if_fail(PAD_IS_POINT(self), FALSE);
  g_return_val_if_fail(PAD_IS_POINT(pt), FALSE);

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  return priv->x == pt_priv->x ? TRUE : FALSE;
}

gboolean pad_point_is_y_greater(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_val_if_fail(PAD_IS_POINT(self), FALSE);
  g_return_val_if_fail(PAD_IS_POINT(pt), FALSE);

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  return priv->y > pt_priv->y ? TRUE : FALSE;
}

gboolean pad_point_is_y_greater_equal(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_val_if_fail(PAD_IS_POINT(self), FALSE);
  g_return_val_if_fail(PAD_IS_POINT(pt), FALSE);

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  return priv->y >= pt_priv->y ? TRUE : FALSE;
}

gboolean pad_point_is_y_lower(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_val_if_fail(PAD_IS_POINT(self), FALSE);
  g_return_val_if_fail(PAD_IS_POINT(pt), FALSE);

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  return priv->y < pt_priv->y ? TRUE : FALSE;
}

gboolean pad_point_is_y_lower_equal(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_val_if_fail(PAD_IS_POINT(self), FALSE);
  g_return_val_if_fail(PAD_IS_POINT(pt), FALSE);

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  return priv->y <= pt_priv->y ? TRUE : FALSE;
}

gboolean pad_point_is_y_equal(PadPoint *self, PadPoint *pt) {
  PadPointPrivate *priv, *pt_priv;

  g_return_val_if_fail(PAD_IS_POINT(self), FALSE);
  g_return_val_if_fail(PAD_IS_POINT(pt), FALSE);

  priv = pad_point_get_instance_private(self);
  pt_priv = pad_point_get_instance_private(pt);

  return priv->y == pt_priv->y ? TRUE : FALSE;
}
