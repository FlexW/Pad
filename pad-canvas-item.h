/*
 * 2.4.18 Felix Weilbach
 */

#ifndef __PAD_CANVAS_ITEM_H__
#define __PAD_CANVAS_ITEM_H__

#include <gtk.h>

//#include "pad-bounding-box.h"

G_BEGIN_DECLS

typedef struct _PadCanvas PadCanvas;
typedef struct _PadBoundingBox PadBoundingBox;
typedef struct _PadPoint PadPoint;

GType pad_canvas_item_bounds_get_type(void) G_GNUC_CONST;
#define PAD_TYPE_CANVAS_ITEM_BOUNDS (pad_canvas_item_bounds_get_type())

#define PAD_TYPE_CANVAS_ITEM (pad_canvas_item_get_type())
#define PAD_CANVAS_ITEM(item)                                                  \
  (G_TYPE_CHECK_INSTANCE_CAST((item), PAD_TYPE_CANVAS_ITEM, PadCanvasItem))
#define PAD_CANVAS_ITEM_CLASS(klass)                                           \
  (G_TYPE_CHECK_CLASS_CAST((klass), PAD_TYPE_CANVAS_ITEM, PadCanvasItemClass))
#define PAD_IS_CANVAS_ITEM(item)                                               \
  (G_TYPE_CHECK_INSTANCE_TYPE((item), PAD_TYPE_CANVAS_ITEM))
#define PAD_IS_CANVAS_ITEM_CLASS(klass)                                        \
  (G_TYPE_CHECK_CLASS_TYPE((klass), PAD_TYPE_CANVAS_ITEM))
#define PAD_CANVAS_ITEM_GET_CLASS(obj)                                         \
  (G_TYPE_INSTANCE_GET_CLASS((obj), PAD_TYPE_CANVAS_ITEM, PadCanvasItemClass))

typedef struct _PadCanvasItem PadCanvasItem;
typedef struct _PadCanvasItemPrivate PadCanvasItemPrivate;
typedef struct _PadCanvasItemClass PadCanvasItemClass;
typedef struct _PadCanvasItemClassPrivate PadCanvasItemClassPrivate;

struct _PadCanvasItem {
  /*< private >*/

  GObject parent_instance;

  /* The position of the item in the world coordinate system. */
  gdouble world_x, world_y;

  /* The items parent. */
  PadCanvasItem *parent_item;

  /* The items child. */
  PadCanvasItem *child;

  /* The items canvas. */
  PadCanvas *canvas;

  /* The items bounding box in world space. Update with:
   * pad_canvas_item_update_bounding_box() */
  PadBoundingBox *bounding_box;
};

typedef struct _PadCanvasItemClass {
  /*< private >*/
  GObjectClass parent_class;

  /* Virtual methods. */
  /*< public >*/
  void (*draw)(PadCanvasItem *self, cairo_t *cr);

  void (*add)(PadCanvasItem *self, PadCanvasItem *child);

  void (*update_bounding_box)(PadCanvasItem *self);

  /*< private >*/
  gpointer padding[30];

} PadCanvasItemClass;

GType pad_canvas_item_get_type(void) G_GNUC_CONST;

PadCanvasItem *pad_canvas_item_new(PadCanvasItem *parent_item, ...);

void pad_canvas_item_draw(PadCanvasItem *self, cairo_t *cr);

void pad_canvas_item_add(PadCanvasItem *self, PadCanvasItem *child);

void pad_canvas_item_update_bounding_box(PadCanvasItem *self);

gdouble pad_canvas_item_point_lower_bound(PadPoint *pt, gdouble line_width);

gdouble pad_canvas_item_point_upper_bound(PadPoint *pt, gdouble line_width);

gdouble pad_canvas_item_point_left_bound(PadPoint *pt, gdouble line_width);

gdouble pad_canvas_item_point_right_bound(PadPoint *pt, gdouble line_width);

void pad_canvas_item_polyline_update_bounding_box(PadCanvasItem *self);

void pad_canvas_item_bounding_box_expand_to_point(PadCanvasItem *self,
                                                  PadPoint *pt);

void pad_canvas_item_bounding_box_expand_to_box(PadCanvasItem *self,
                                                PadBoundingBox *bounding_box);

void pad_canvas_item_pdf_update_bounding_box(PadCanvasItem *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(PadCanvasItem, g_object_unref)

G_END_DECLS

#endif /* __PAD_CANVAS_ITEM_H__ */
