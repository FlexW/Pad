/*
 * 31.3.18 Felix Weilbach
 */

/**
 * SECTION:pad-coordinate-system
 * @Short_description: Represents a coordinate system
 * @Title: PadCoordinateSystem
 *
 * The #PadCoordinateSystem is used in the #PadCanvas and #PadCanvasItem to
 * represent the window, world and item coordinate system.
 */

#include "pad-coordinate-system.h"

#include <cairo.h>

struct _PadCoordinateSystem {
  GObject parent_instance;
  cairo_matrix_t matrix;
  gdouble bound_width, bound_height;
  gdouble scale;
};

G_DEFINE_TYPE(PadCoordinateSystem, pad_coordinate_system, G_TYPE_OBJECT)

enum { PROP_BOUND_WIDTH = 1, PROP_BOUND_HEIGHT, PROP_SCALE, N_PROPERTIES };

static GParamSpec *obj_properties[N_PROPERTIES] = {
    NULL,
};

static void pad_coordinate_system_set_bounds(PadCoordinateSystem *self,
                                             gdouble width, gdouble height) {
  self->bound_width = width;
  self->bound_height = height;
}

static void pad_coordinate_system_get_bounds(PadCoordinateSystem *self,
                                             gdouble *width, gdouble *height) {
  *width = self->bound_width;
  *height = self->bound_height;
}

static void pad_coordinate_system_set_scale(PadCoordinateSystem *self,
                                            gdouble scale) {
  self->scale *= scale;
  cairo_matrix_scale(&self->matrix, scale, scale);
}

static void pad_coordinate_system_get_scale(PadCoordinateSystem *self,
                                            gdouble *scale) {
  *scale = self->scale;
}

static void pad_coordinate_system_dispose(GObject *gobject) {
  G_OBJECT_CLASS(pad_coordinate_system_parent_class)->dispose(gobject);
}

static void pad_coordinate_system_finalize(GObject *gobject) {
  G_OBJECT_CLASS(pad_coordinate_system_parent_class)->finalize(gobject);
}

static void pad_coordinate_system_set_property(GObject *object, guint prop_id,
                                               const GValue *value,
                                               GParamSpec *pspec) {
  PadCoordinateSystem *cs = PAD_COORDINATE_SYSTEM(object);

  switch (prop_id) {
  case PROP_BOUND_WIDTH: {
    gdouble width = 0;
    gdouble height = 0;

    pad_coordinate_system_get_bounds(cs, &width, &height);
    width = g_value_get_double(value);
    pad_coordinate_system_set_bounds(cs, width, height);
  } break;

  case PROP_BOUND_HEIGHT: {
    gdouble width = 0;
    gdouble height = 0;

    pad_coordinate_system_get_bounds(cs, &width, &height);
    height = g_value_get_double(value);
    pad_coordinate_system_set_bounds(cs, width, height);
  } break;

  case PROP_SCALE: {
    gdouble scale = g_value_get_double(value);

    pad_coordinate_system_set_scale(cs, scale);
  } break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_coordinate_system_get_property(GObject *object, guint prop_id,
                                               GValue *value,
                                               GParamSpec *pspec) {
  PadCoordinateSystem *cs = PAD_COORDINATE_SYSTEM(object);

  switch (prop_id) {
  case PROP_BOUND_WIDTH: {
    gdouble width = 0;
    gdouble height = 0;

    pad_coordinate_system_get_bounds(cs, &width, &height);
    g_value_set_double(value, width);
  } break;

  case PROP_BOUND_HEIGHT: {
    gdouble width = 0;
    gdouble height = 0;

    pad_coordinate_system_get_bounds(cs, &width, &height);
    g_value_set_double(value, height);
  } break;

  case PROP_SCALE: {
    gdouble scale = 0;

    pad_coordinate_system_get_scale(cs, &scale);
    g_value_set_double(value, scale);
  } break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_coordinate_system_class_init(PadCoordinateSystemClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  gobject_class->set_property = pad_coordinate_system_set_property;
  gobject_class->get_property = pad_coordinate_system_get_property;
  gobject_class->dispose = pad_coordinate_system_dispose;
  gobject_class->finalize = pad_coordinate_system_finalize;

  obj_properties[PROP_BOUND_WIDTH] =
      g_param_spec_double("bound-width", "Bound width", "Bound width", 0,
                          G_MAXDOUBLE, 0, G_PARAM_READWRITE);

  obj_properties[PROP_BOUND_HEIGHT] =
      g_param_spec_double("bound-height", "Bound height", "Bound height", 0,
                          G_MAXDOUBLE, 0, G_PARAM_READWRITE);

  obj_properties[PROP_SCALE] = g_param_spec_double(
      "scale", "Scale", "Scale", 0.00000001, G_MAXDOUBLE, 1, G_PARAM_READWRITE);

  g_object_class_install_properties(gobject_class, N_PROPERTIES,
                                    obj_properties);
}

static void pad_coordinate_system_init(PadCoordinateSystem *self) {
  cairo_matrix_init_scale(&self->matrix, 1, 1);
}

/**
 * pad_coordinate_system_new:
 *
 * Creates a new #PadCoordinateSystem.
 *
 * Returns: The newly created #PadCoordinateSystem.
 */
PadCoordinateSystem *pad_coordinate_system_new(void) {
  PadCoordinateSystem *cs = g_object_new(PAD_TYPE_COORDINATE_SYSTEM, NULL);

  return cs;
}

/**
 * pad_coordinate_system_translate_to_point:
 * @self: The #PadCoordinateSystem you want points translated
 * @x: The x coordinate
 * @y: The y coordinate
 *
 * Translates the coordinates system matrix to the given x and y coordinate.
 */
void pad_coordinate_system_translate_to_point(PadCoordinateSystem *self,
                                              gdouble x, gdouble y) {
  g_return_if_fail(PAD_IS_COORDINATE_SYSTEM(self));

  gdouble curr_x = 0, curr_y = 0;

  pad_coordinate_system_point_in_space(self, &curr_x, &curr_y);
  x -= curr_x;
  y -= curr_y;
  cairo_matrix_translate(&self->matrix, x, y);
}
/**
 * pad_coordinate_system_point_in_space:
 * @self: The #PadCoordinateSystem you want points converted
 * @x: The x coordinate
 * @y: The y coordinate
 *
 * Converts the points x and y to the space of the given #PadCoordinateSystem.
 * That means applying the #PadCoordinateSystem matrix on x and y.
 */
void pad_coordinate_system_point_in_space(PadCoordinateSystem *self, gdouble *x,
                                          gdouble *y) {
  g_return_if_fail(PAD_IS_COORDINATE_SYSTEM(self));

  cairo_matrix_transform_point(&self->matrix, x, y);
}

/**
 * pad_coordinate_system_distance_in_space:
 * @self: The #PadCoordinateSystem you want points converted
 * @x: The x coordinate
 * @y: The y coordinate
 *
 * Converts the points x and y to the space of the given #PadCoordinateSystem.
 * That means applying the #PadCoordinateSystem matrix on x and y,
 * without applying the translate component of the matrix
 */
void pad_coordinate_system_distance_in_space(PadCoordinateSystem *self,
                                             gdouble *x, gdouble *y) {
  g_return_if_fail(PAD_IS_COORDINATE_SYSTEM(self));

  cairo_matrix_transform_distance(&self->matrix, x, y);
}
