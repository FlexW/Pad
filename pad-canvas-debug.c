/*
 * 31.3.18 Felix Weilbach
 */

#include "pad-canvas-debug.h"
#include "pad-canvas.h"

void pad_canvas_debug_draw_world_bounds(GtkWidget *widget, cairo_t *cr) {
  PadCanvas *canvas = PAD_CANVAS(widget);
  gdouble x = 0, y = 0;
  PadCoordinateSystem *world_cs;
  gdouble world_bound_width, world_bound_height;

  g_object_get(widget, "world-coordinate-system", &world_cs, NULL);

  g_object_get(world_cs, "bound-width", &world_bound_width, "bound-height",
               &world_bound_height, NULL);
  /*
  g_print("pad_canvas_debug_draw_bounds: world_bound_width: %lf "
          "world_bound_height: %lf\n",
          world_bound_width, world_bound_height);
  */
  cairo_save(cr);

  cairo_new_path(cr);
  cairo_set_line_width(cr, 5);
  cairo_set_source_rgb(cr, 1, 0, 0);

  x = 0;
  y = 0;
  pad_canvas_world_to_window(canvas, &x, &y);
  cairo_line_to(cr, x, y);

  x = world_bound_width;
  y = 0;
  pad_canvas_world_to_window(canvas, &x, &y);
  cairo_line_to(cr, x, y);

  x = world_bound_width;
  y = world_bound_height;
  pad_canvas_world_to_window(canvas, &x, &y);
  cairo_line_to(cr, x, y);

  x = 0;
  y = world_bound_height;
  pad_canvas_world_to_window(canvas, &x, &y);
  cairo_line_to(cr, x, y);

  x = 0;
  y = 0;
  pad_canvas_world_to_window(canvas, &x, &y);
  cairo_line_to(cr, x, y);

  cairo_stroke(cr);

  cairo_restore(cr);

  g_object_unref(world_cs);
}

void pad_canvas_debug_draw_world_center(GtkWidget *widget, cairo_t *cr) {
  PadCanvas *canvas = PAD_CANVAS(widget);
  PadCoordinateSystem *world_cs;
  gdouble world_bound_width, world_bound_height, world_center_x, world_center_y;

  g_object_get(widget, "world-coordinate-system", &world_cs, NULL);

  g_object_get(world_cs, "bound-width", &world_bound_width, "bound-height",
               &world_bound_height, NULL);

  world_center_x = world_bound_width / 2;
  world_center_y = world_bound_height / 2;

  cairo_save(cr);

  cairo_set_source_rgb(cr, 0, 0, 0);

  pad_canvas_world_to_window(canvas, &world_center_x, &world_center_y);
  cairo_arc(cr, world_center_x, world_center_y, 5, 0, 360);

  cairo_fill(cr);

  cairo_restore(cr);

  g_object_unref(world_cs);
}

void pad_canvas_debug_draw_grid(GtkWidget *widget, cairo_t *cr) {
  PadCanvas *canvas = PAD_CANVAS(widget);
  PadCoordinateSystem *world_cs;
  gdouble world_bound_width, world_bound_height;
  gint nlen, mlen;

  g_object_get(widget, "world-coordinate-system", &world_cs, NULL);

  g_object_get(world_cs, "bound-width", &world_bound_width, "bound-height",
               &world_bound_height, NULL);

  nlen = (gint)world_bound_width; mlen = (gint)world_bound_height;

  pad_canvas_world_to_window(canvas, &world_bound_width, &world_bound_width);

  cairo_save(cr);

  cairo_set_line_width(cr, 1);
  cairo_set_source_rgb(cr, 0, 0, 0);

  for (int i = 5; i < nlen; i += 5) {
    gdouble x, y;

    x = i; y = 0;
    pad_canvas_world_to_window(canvas, &x, &y);
    cairo_line_to(cr, x, y);

    x = i; y = mlen;
    pad_canvas_world_to_window(canvas, &x, &y);
    cairo_line_to(cr, x, y);

    cairo_stroke(cr);
  }

  for (int i = 5; i < mlen; i += 5) {
    gdouble x, y;

    x = 0; y = i;
    pad_canvas_world_to_window(canvas, &x, &y);
    cairo_line_to(cr, x, y);

    x = nlen; y = i;
    pad_canvas_world_to_window(canvas, &x, &y);
    cairo_line_to(cr, x, y);

    cairo_stroke(cr);
  }

  cairo_stroke(cr);

  cairo_restore(cr);

  g_object_unref(world_cs);
}

void pad_canvas_debug_draw_background(GtkWidget *widget, cairo_t *cr) {
  cairo_save(cr);
  cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
  cairo_paint(cr);
  cairo_restore(cr);
}
