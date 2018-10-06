/**
 * 31.3.18 Felix Weilbach
 */

#ifndef __PAD_CANVAS_DEBUG_H__
#define __PAD_CANVAS_DEBUG_H__

#include <gtk/gtk.h>

void pad_canvas_debug_draw_world_bounds(GtkWidget *widget, cairo_t *cr);

void pad_canvas_debug_draw_world_center(GtkWidget *widget, cairo_t *cr);

void pad_canvas_debug_draw_grid(GtkWidget *widget, cairo_t *cr);

void pad_canvas_debug_draw_background(GtkWidget *widget, cairo_t *cr);

#endif /* __PAD_CANVAS_DEBUG_H__ */
