/*
 * 31.3.18 Felix Weilbach
 */

#ifndef __PAD_CANVAS_H__
#define __PAD_CANVAS_H__

#include <gtk.h>

#include "pad-canvas-item-group.h"
#include "pad-canvas-item-polyline.h"
#include "pad-canvas-item.h"
#include "pad-coordinate-system.h"

G_BEGIN_DECLS

#define PAD_TYPE_CANVAS pad_canvas_get_type()
G_DECLARE_DERIVABLE_TYPE(PadCanvas, pad_canvas, PAD, CANVAS, GtkContainer);

typedef struct _PadCanvasClass {
  GtkContainerClass parent_class;

  gpointer padding[12];

} PadCanvasClass;

// GType           goo_canvas_get_type	    (void) G_GNUC_CONST;

GtkWidget *pad_canvas_new(void);

void pad_canvas_set_world_bounds(PadCanvas *self, gdouble width,
                                 gdouble height);

void pad_canvas_set_world_scale(PadCanvas *self, gdouble scale);

void pad_canvas_window_to_world(PadCanvas *self, gdouble *x, gdouble *y);

void pad_canvas_world_to_window(PadCanvas *self, gdouble *x, gdouble *y);

PadCoordinateSystem *pad_canvas_get_world_cs(PadCanvas *self);

PadCanvasItem *pad_canvas_get_root_item(PadCanvas *self);

GdkWindow *pad_canvas_get_window(PadCanvas *self);

void pad_canvas_scroll_to(PadCanvas *self, gdouble vadjustment,
                          gdouble hadjustment);

G_END_DECLS

#endif /* __PAD_CANVAS_H__ */
