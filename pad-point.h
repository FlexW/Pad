/*
 * 7.4.18 Felix Weilbach
 */

#ifndef __PAD_POINT_H__
#define __PAD_POINT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define PAD_TYPE_POINT pad_point_get_type()

G_DECLARE_DERIVABLE_TYPE(PadPoint, pad_point, PAD, POINT, GObject);

typedef struct _PadPointClass {
  GObjectClass parent_class;

  gpointer padding[12];

} PadPointClass;

PadPoint *pad_point_new(gdouble x, gdouble y);

void pad_point_set_max(PadPoint *self, PadPoint *pt);

void pad_point_set_min(PadPoint *self, PadPoint *pt);

gboolean pad_point_is_x_greater(PadPoint *self, PadPoint *pt);

gboolean pad_point_is_x_greater_equal(PadPoint *self, PadPoint *pt);

gboolean pad_point_is_x_lower(PadPoint *self, PadPoint *pt);

gboolean pad_point_is_x_lower_equal(PadPoint *self, PadPoint *pt);

gboolean pad_point_is_x_equal(PadPoint *self, PadPoint *pt);

gboolean pad_point_is_y_greater(PadPoint *self, PadPoint *pt);

gboolean pad_point_is_y_greater_equal(PadPoint *self, PadPoint *pt);

gboolean pad_point_is_y_lower(PadPoint *self, PadPoint *pt);

gboolean pad_point_is_y_lower_equal(PadPoint *self, PadPoint *pt);

gboolean pad_point_is_y_equal(PadPoint *self, PadPoint *pt);

G_END_DECLS

#endif /* __PAD_POINT_H__ */
