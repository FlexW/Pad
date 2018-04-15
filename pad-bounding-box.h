/*
 * 7.4.18 Felix Weilbach
 */

#ifndef __PAD_BOUNDING_BOX_H__
#define __PAD_BOUNDING_BOX_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _PadPoint PadPoint;

#define PAD_TYPE_BOUNDING_BOX pad_bounding_box_get_type()

G_DECLARE_DERIVABLE_TYPE(PadBoundingBox, pad_bounding_box, PAD, BOUNDING_BOX,
                         GObject);

typedef struct _PadBoundingBoxClass {
  GObjectClass parent_class;

  gpointer padding[12];

} PadBoundingBoxClass;

PadBoundingBox *pad_bounding_box_new(gdouble pt1_x, gdouble pt1_y,
                                     gdouble pt2_x, gdouble pt2_y);

gboolean pad_bounding_box_is_intersect_box(PadBoundingBox *self,
                                           PadBoundingBox *bounding_box);

gboolean pad_bounding_box_is_intersect_point(PadBoundingBox *self,
                                             PadPoint *pt);

void pad_bounding_box_expand_to_point(PadBoundingBox *self, PadPoint *pt);

void pad_bounding_box_expand_to_box(PadBoundingBox *self,
                                    PadBoundingBox *bounding_box);

G_END_DECLS

#endif /* __PAD_BOUNDING_BOX_H__ */
