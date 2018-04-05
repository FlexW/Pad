/*
 * 2.4.18 Felix Weilbach
 */

#ifndef __PAD_CANVAS_ITEM_H__
#define __PAD_CANVAS_ITEM_H__

#include <gtk.h>

//#include "pad-canvas.h"

typedef struct _PadCanvasDrawArea PadCanvasDrawArea;

G_BEGIN_DECLS

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

  /* If this is set to FALSE, the drawing method of the item will not be called.
   */
  gboolean need_update;

  /* <private> */
  /* We might use this in future for things like a cache. */
  gpointer priv;
};

/*
#define PAD_TYPE_CANVAS_ITEM pad_canvas_item_get_type()
G_DECLARE_DERIVABLE_TYPE(PadCanvasItem, pad_canvas_item, PAD, CANVAS_ITEM,
                         GObject);
*/
typedef struct _PadCanvasItemClass {
  /*< private >*/
  GObjectClass parent_class;

  /* Virtual methods. */
  /*< public >*/
  void (*draw)(PadCanvasItem *self, cairo_t *cr, PadCanvasDrawArea *draw_area);

  gboolean (*is_in_draw_area)(PadCanvasItem *self,
                              PadCanvasDrawArea *draw_area);

  /*< private >*/
  gpointer padding[30];

} PadCanvasItemClass;

GType pad_canvas_item_get_type(void) G_GNUC_CONST;

PadCanvasItem *pad_canvas_item_new(PadCanvasItem *parent_item);

gboolean pad_canvas_item_is_in_draw_area(PadCanvasItem *self,
                                         PadCanvasDrawArea *draw_area);

void pad_canvas_item_draw(PadCanvasItem *self, cairo_t *cr,
                          PadCanvasDrawArea *draw_area);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(PadCanvasItem, g_object_unref)

G_END_DECLS

#endif /* __PAD_CANVAS_ITEM_H__ */
