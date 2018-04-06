#include <poppler.h>

#include "pad-canvas-item-pdf.h"
#include "pad-canvas.h"
//#include "pad-canvas-item-group.h"
//#include "pad-canvas-item-polyline.h"

static GtkWidget *canvas = NULL;
static GtkWidget *spinbtn_bound_x, *spinbtn_bound_y, *spinbtn_scale;
static GtkGesture *zoom_gesture = NULL;

static gboolean button_pressed = FALSE;
static PadCanvasItem *current_line;
static PadCanvasItem *root_item;

static gdouble get_pressure(GdkEventMotion *event) {
  GdkDevice *device;
  gdouble *axes;
  gdouble rawpressure;

  device = gdk_event_get_source_device((GdkEvent *)event);

  if (gdk_device_get_source(device) == GDK_SOURCE_PEN) {
    axes = event->axes;
    rawpressure = axes[2];
    return rawpressure;
  }

  return 1;
}

typedef guint zoom_direction;

enum { ZOOM_DIRECTION_IN, ZOOM_DIRECTION_OUT };

#define ZOOM_IN_STEP 1.05
#define ZOOM_OUT_STEP 1 / ZOOM_IN_STEP

static void smooth_zoom(PadCanvas *canvas, zoom_direction zdirection) {
  gdouble hadjustment, vadjustment, world_scale;

  g_object_get(canvas, "hadjustment", &hadjustment, "vadjustment", &vadjustment,
               NULL);

  switch (zdirection) {

  case ZOOM_DIRECTION_IN:
    g_object_set(canvas, "world-scale", ZOOM_IN_STEP, NULL);
    break;

  case ZOOM_DIRECTION_OUT:
    g_object_set(canvas, "world-scale", ZOOM_OUT_STEP, NULL);
    break;
  }

  g_object_get(canvas, "world-scale", &world_scale, NULL);

  // hadjustment *= world_scale;
  // vadjustment *= world_scale;

  pad_canvas_scroll_to(canvas, vadjustment, hadjustment);
  // g_object_set(canvas, "hadjustment", hadjustment, "vadjustment",
  // vadjustment, NULL);
}

static PopplerDocument *open_pdf_file(gchar *filename) {
  char *contents;
  gsize length;
  GError *error = NULL;
  PopplerDocument *mdoc;

  if (!g_file_get_contents(filename, &contents, &length, &error)) {
    return NULL;
  }

  if (length < 4 || strncmp(contents, "%PDF", 4)) {
    g_free(contents);
    contents = NULL;
    return NULL;
  }

  mdoc = poppler_document_new_from_data(contents, length, NULL, NULL);

  return mdoc;
}

static void set_pdf_page() {
  static PopplerDocument *doc = NULL;
  static PopplerPage *page = NULL;
  static PadCanvasItem *item_pdf;

  if (doc == NULL && root_item != NULL) {
    gdouble width, height;
    doc = open_pdf_file("/home/human/Uni/Analysis1/ana1.pdf");
    if (doc == NULL) {
      return;
    }

    int npages = poppler_document_get_n_pages(doc);
    for (int i = 0; i < npages; i++) {
      page = poppler_document_get_page(doc, i);
      poppler_page_get_size(page, &width, &height);
      item_pdf = pad_canvas_item_pdf_new(root_item, page, "world-y",
                                         (height + 10) * i, NULL);
    }
    pad_canvas_set_world_bounds(PAD_CANVAS(canvas), width,
                                (height + 10) * npages);
  }
}

static void on_canvas_scale_changed(GtkGestureZoom *controller, gdouble scale,
                                    GtkWidget *widget) {
  static gdouble prev_scale = 1;
  g_print("on_canvas_scale_changed\n");
  g_print("on_canvas_scale_changed: scale: %lf\n", scale);

  if (prev_scale < scale) {
    smooth_zoom(PAD_CANVAS(widget), ZOOM_DIRECTION_OUT);
    // pad_canvas_set_world_scale(PAD_CANVAS(widget), 1.05);
  } else if (prev_scale > scale) {
    smooth_zoom(PAD_CANVAS(widget), ZOOM_DIRECTION_IN);
    // pad_canvas_set_world_scale(PAD_CANVAS(widget), 0.95);
  }
  prev_scale = scale;
}
static gboolean on_canvas_motion_notify_event(GtkWidget *widget,
                                              GdkEventMotion *event) {
  if (!button_pressed) {
    return FALSE;
  }

  if (current_line) {
    g_print("on_canvas_motion_notify_event\n");
    gdouble x = event->x;
    gdouble y = event->y;
    gdouble line_width = get_pressure(event) * 3;

    g_print("on_canvas_motion_notify_event: window space x: %lf y: %lf\n", x,
            y);
    pad_canvas_window_to_world(PAD_CANVAS(canvas), &x, &y);
    g_print("on_canvas_motion_notify_event: world space x: %lf y: %lf\n", x, y);
    pad_canvas_item_polyline_add_point(PAD_CANVAS_ITEM_POLYLINE(current_line),
                                       x, y, line_width);

    gtk_widget_queue_draw(canvas);
  }

  return FALSE;
}

static gboolean on_canvas_button_press_event(GtkWidget *widget, GdkEvent *event,
                                             gpointer data) {
  g_print("on_canvas_button_press_event\n");
  gdouble x = event->motion.x;
  gdouble y = event->motion.y;
  gdouble line_width = get_pressure(&event->motion) * 3;

  current_line = pad_canvas_item_polyline_new(NULL);
  pad_canvas_item_group_add_item(PAD_CANVAS_ITEM_GROUP(root_item),
                                 current_line);
  pad_canvas_window_to_world(PAD_CANVAS(canvas), &x, &y);
  pad_canvas_item_polyline_add_point(PAD_CANVAS_ITEM_POLYLINE(current_line), x,
                                     y, line_width);
  gtk_widget_queue_draw(canvas);

  button_pressed = TRUE;

  return FALSE;
}

static gboolean on_canvas_button_release_event(GtkWidget *widget,
                                               GdkEvent *event, gpointer data) {
  g_print("on_canvas_button_release_event\n");
  if (current_line) {
    gdouble x = event->motion.x;
    gdouble y = event->motion.y;
    gdouble line_width = get_pressure(&event->motion) * 3;

    pad_canvas_window_to_world(PAD_CANVAS(canvas), &x, &y);
    pad_canvas_item_polyline_add_point(PAD_CANVAS_ITEM_POLYLINE(current_line),
                                       x, y, line_width);
    gtk_widget_queue_draw(canvas);
  }

  button_pressed = FALSE;

  return FALSE;
}
static void on_spin_button_value_changed(GtkSpinButton *spin_button,
                                         gpointer data) {
  g_print("on_spin_button_value_changed\n");
  static gdouble bound_x = 0, bound_y = 0, scale = 1;

  if (spin_button == GTK_SPIN_BUTTON(spinbtn_bound_x)) {
    bound_x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbtn_bound_x));
    bound_y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbtn_bound_y));
    pad_canvas_set_world_bounds(PAD_CANVAS(canvas), bound_x, bound_y);
  } else if (spin_button == GTK_SPIN_BUTTON(spinbtn_bound_y)) {
    bound_x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbtn_bound_x));
    bound_y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbtn_bound_y));
    pad_canvas_set_world_bounds(PAD_CANVAS(canvas), bound_x, bound_y);
  } else if (spin_button == GTK_SPIN_BUTTON(spinbtn_scale)) {
    scale = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbtn_scale));
    pad_canvas_set_world_scale(PAD_CANVAS(canvas), scale);
  }
}

static void build_control_bar(GtkBox *pbox) {
  GtkWidget *hbox;
  GtkWidget *tv_bound_x, *tv_bound_y, *tv_scale;
  GtkAdjustment *adj_bound_x, *adj_bound_y, *adj_scale;

  adj_bound_x = gtk_adjustment_new(1, -999999, 999999, 0.1, 2, 0);
  adj_bound_y = gtk_adjustment_new(0, -999999, 999999, 0.1, 2, 0);
  adj_scale = gtk_adjustment_new(1, -999999, 999999, 0.1, 2, 0);

  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
  gtk_box_pack_start(GTK_BOX(pbox), hbox, FALSE, FALSE, 1);

  tv_bound_x = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tv_bound_x), FALSE);
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_bound_x)),
                           "Bound x:", -1);
  gtk_box_pack_start(GTK_BOX(hbox), tv_bound_x, FALSE, FALSE, 1);
  gtk_widget_set_size_request(tv_bound_x, 20, 20);

  spinbtn_bound_x = gtk_spin_button_new(adj_bound_x, 3, 2);
  gtk_box_pack_start(GTK_BOX(hbox), spinbtn_bound_x, FALSE, FALSE, 1);
  gtk_widget_set_size_request(spinbtn_bound_x, 20, 20);
  g_signal_connect(spinbtn_bound_x, "value-changed",
                   G_CALLBACK(on_spin_button_value_changed), NULL);

  tv_bound_y = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tv_bound_y), FALSE);
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_bound_y)),
                           "Bound y:", -1);
  gtk_box_pack_start(GTK_BOX(hbox), tv_bound_y, FALSE, FALSE, 1);
  gtk_widget_set_size_request(tv_bound_y, 20, 20);

  spinbtn_bound_y = gtk_spin_button_new(adj_bound_y, 3, 2);
  gtk_box_pack_start(GTK_BOX(hbox), spinbtn_bound_y, FALSE, FALSE, 1);
  gtk_widget_set_size_request(spinbtn_bound_y, 20, 20);
  g_signal_connect(spinbtn_bound_y, "value-changed",
                   G_CALLBACK(on_spin_button_value_changed), NULL);

  tv_scale = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tv_scale), FALSE);
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_scale)),
                           "Scale:", -1);
  gtk_box_pack_start(GTK_BOX(hbox), tv_scale, FALSE, FALSE, 1);
  gtk_widget_set_size_request(tv_scale, 20, 20);

  spinbtn_scale = gtk_spin_button_new(adj_scale, 3, 2);
  gtk_box_pack_start(GTK_BOX(hbox), spinbtn_scale, FALSE, FALSE, 1);
  gtk_widget_set_size_request(spinbtn_scale, 20, 20);
  g_signal_connect(spinbtn_scale, "value-changed",
                   G_CALLBACK(on_spin_button_value_changed), NULL);
}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window, *mbox, *scrolled_win;
  GdkWindow *canvas_window;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "pad_test_driver_canvas");
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);

  mbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
  gtk_container_add(GTK_CONTAINER(window), mbox);

  build_control_bar(GTK_BOX(mbox));

  scrolled_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_win),
                                      GTK_SHADOW_IN);
  gtk_box_pack_start(GTK_BOX(mbox), scrolled_win, TRUE, TRUE, 1);

  canvas = pad_canvas_new();
  g_object_get(canvas, "root-item", &root_item, NULL);
  gtk_container_add(GTK_CONTAINER(scrolled_win), canvas);
  g_signal_connect(scrolled_win, "motion-notify-event",
                   G_CALLBACK(on_canvas_motion_notify_event), NULL);
  g_signal_connect(scrolled_win, "button-press-event",
                   G_CALLBACK(on_canvas_button_press_event), NULL);
  g_signal_connect(scrolled_win, "button-release-event",
                   G_CALLBACK(on_canvas_button_release_event), NULL);
  zoom_gesture = gtk_gesture_zoom_new(canvas);
  gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(zoom_gesture),
                                             GTK_PHASE_BUBBLE);
  g_signal_connect(zoom_gesture, "scale-changed",
                   G_CALLBACK(on_canvas_scale_changed), canvas);

  set_pdf_page();

  gtk_widget_show_all(window);
  g_object_get(canvas, "canvas-window", &canvas_window, NULL);
  gdk_window_set_event_compression(canvas_window, FALSE);
}

int main(int argc, char *argv[]) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("de.pad.testcanvas", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
