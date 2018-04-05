/*
 * 31.3.18 Felix Weilbach
 */

#ifndef __PAD_COORDINATE_SYSTEM_H__
#define __PAD_COORDINATE_SYSTEM_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define PAD_TYPE_COORDINATE_SYSTEM pad_coordinate_system_get_type()
G_DECLARE_FINAL_TYPE(PadCoordinateSystem, pad_coordinate_system, PAD,
                     COORDINATE_SYSTEM, GObject);

PadCoordinateSystem *pad_coordinate_system_new(void);

void pad_coordinate_system_translate_to_point(PadCoordinateSystem *self,
                                              gdouble x, gdouble y);

void pad_coordinate_system_point_in_space(PadCoordinateSystem *self, gdouble *x,
                                          gdouble *y);

void pad_coordinate_system_distance_in_space(PadCoordinateSystem *self,
                                             gdouble *x, gdouble *y);

G_END_DECLS

#endif /* __PAD_COORDINATE_SYSTEM_H__ */
