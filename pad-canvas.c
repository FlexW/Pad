/*
 * 31.3.18 Felix Weilbach
 */

/**
 * SECTION:pad-canvas
 * @Short_description: The canvas object.
 * @Title: PadCanvas
 *
 * The #PadCanvas is used as the drawing area in the pad.
 */

#include "pad-canvas.h"
//#include "pad-bounding-box.h"
#include "pad-canvas-debug.h"
#include "pad-canvas-item-group.h"
#include "pad-canvas-item-polyline.h"

struct _PadCanvasPrivate {
  /* The world coordinate system. */
  PadCoordinateSystem *world_cs;

  /* Cached size of world bounds. Gets set in pad_canvas_set_scale() and
     pad_canvas_set_bounds(). */
  gdouble world_bound_width;
  gdouble world_bound_height;
  gdouble world_bound_width_scaled;
  gdouble world_bound_height_scaled;
  gdouble world_scale;

  /* GtkScrollablePolicy for each adjustment. */
  guint hscroll_policy : 1;
  guint vscroll_policy : 1;

  /* The adjustments used for scrolling. */
  GtkAdjustment *hadjustment;
  GtkAdjustment *vadjustment;

  /* The main canvas window,that gets scrolled around. */
  GdkWindow *canvas_window;

  /* Cached size of the widgets allocated size. */
  gint window_alloc_width;
  gint window_alloc_height;

  /* Offset of the canvas within the window. These are used when the canvas is
     smaller than the window size. */
  gint canvas_x_offset;
  gint canvas_y_offset;

  gdouble window_x;
  gdouble window_y;

  /* The canvas root item which holds all other items. */
  PadCanvasItem *root_item;

  /* The area that is visible to the user in world space. Update with:
   * pad_canvas_update_view_bounding_box(). */
  //PadBoundingBox view_bounding_box;
};

typedef struct _PadCanvasPrivate PadCanvasPrivate;

enum {
  PROP_HADJUSTMENT = 1,
  PROP_VADJUSTMENT,
  PROP_HSCROLL_POLICY,
  PROP_VSCROLL_POLICY,
  PROP_WORLD_SCALE,
  PROP_ROOT_ITEM,
  PROP_CANVAS_WINDOW,
  PROP_WORLD_COORDINATE_SYSTEM,
  N_PROPERTIES
};

G_DEFINE_TYPE_WITH_CODE(PadCanvas, pad_canvas, GTK_TYPE_CONTAINER,
                        G_ADD_PRIVATE(PadCanvas)
                            G_IMPLEMENT_INTERFACE(GTK_TYPE_SCROLLABLE, NULL))

/* Sets the offset of the canvas. This is needed if the canvas is smaller than
  the window. After a call to this function the offset can be read from
  priv->canvas_x_offset and priv->canvas_y_offset. */
static void pad_canvas_set_offset(PadCanvas *self) {
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);
  PadCoordinateSystem *world_cs = priv->world_cs;
  gdouble world_cs_bound_width, world_cs_bound_height;

  g_object_get(world_cs, "bound-width", &world_cs_bound_width, "bound-height",
               &world_cs_bound_height, NULL);
  pad_coordinate_system_distance_in_space(world_cs, &world_cs_bound_width,
                                          &world_cs_bound_height);

  priv->canvas_x_offset = (priv->window_alloc_width - world_cs_bound_width) / 2;
  priv->canvas_y_offset =
      (priv->window_alloc_height - world_cs_bound_height) / 2;

  if (priv->canvas_x_offset < 0) {
    priv->canvas_x_offset = 0;
  }
  if (priv->canvas_y_offset < 0) {
    priv->canvas_y_offset = 0;
  }
}

static void pad_canvas_configure_hadjustment(PadCanvas *self,
                                             gint window_width) {
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);
  GtkAdjustment *adj = priv->hadjustment;
  gboolean configure = FALSE;
  gdouble value, max_value, lower, upper, step_inc, page_inc, page_size;
  GtkAllocation allocation;

  value = gtk_adjustment_get_value(adj);
  lower = gtk_adjustment_get_lower(adj);
  upper = gtk_adjustment_get_upper(adj);
  step_inc = gtk_adjustment_get_step_increment(adj);
  page_inc = gtk_adjustment_get_page_increment(adj);
  page_size = gtk_adjustment_get_page_size(adj);

  if (upper != window_width) {
    upper = window_width;
    configure = TRUE;
  }

  gtk_widget_get_allocation(GTK_WIDGET(self), &allocation);
  if (page_size != allocation.width) {
    page_size = allocation.width;
    page_inc = page_size * 0.9;
    step_inc = page_size * 0.1;
    configure = TRUE;
  }

  max_value = MAX(0.0, upper - page_size);
  if (value > max_value) {
    value = max_value;
    configure = TRUE;
  }

  if (configure) {
    gtk_adjustment_configure(adj, value, lower, upper, step_inc, page_inc,
                             page_size);
  }
}

static void pad_canvas_configure_vadjustment(PadCanvas *self,
                                             gint window_height) {
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);
  GtkAdjustment *adj = priv->vadjustment;
  gboolean configure = FALSE;
  gdouble value, max_value, lower, upper, step_inc, page_inc, page_size;
  GtkAllocation allocation;

  value = gtk_adjustment_get_value(adj);
  lower = gtk_adjustment_get_lower(adj);
  upper = gtk_adjustment_get_upper(adj);
  step_inc = gtk_adjustment_get_step_increment(adj);
  page_inc = gtk_adjustment_get_page_increment(adj);
  page_size = gtk_adjustment_get_page_size(adj);

  if (upper != window_height) {
    upper = window_height;
    configure = TRUE;
  }

  gtk_widget_get_allocation(GTK_WIDGET(self), &allocation);
  if (page_size != allocation.height) {
    page_size = allocation.height;
    page_inc = page_size * 0.9;
    step_inc = page_size * 0.1;
    configure = TRUE;
  }

  max_value = MAX(0.0, upper - page_size);
  if (value > max_value) {
    value = max_value;
    configure = TRUE;
  }

  if (configure) {
    gtk_adjustment_configure(adj, value, lower, upper, step_inc, page_inc,
                             page_size);
  }
}

/* Sets the offset and then calls the draw queue. */
static void pad_canvas_update(PadCanvas *self) {
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);

  if (!priv->world_cs) {
    return;
  }

  if (gtk_widget_get_realized(GTK_WIDGET(self))) {
    pad_canvas_set_offset(self);
    gtk_widget_queue_draw(GTK_WIDGET(self));
  }
}

/* Gets the real window size since the bounds of the world coordinate system
   maybe bigger than the allocated height and width of the canvas. */
static void pad_canvas_get_window_size(PadCanvas *self, gdouble *window_width,
                                       gdouble *window_height) {
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);
  gdouble world_width, world_height;

  g_object_get(priv->world_cs, "bound-width", &world_width, "bound-height",
               &world_height, NULL);
  pad_coordinate_system_distance_in_space(priv->world_cs, &world_width,
                                          &world_height);
  *window_width = MAX((gint)world_width, priv->window_alloc_width);
  *window_height = MAX((gint)world_height, priv->window_alloc_height);
}

/* Sets the adjustments for scrolled window. This should be called after any
 * change to the scrolled window or if the bounds of the world coordinate system
 * change. */
static void pad_canvas_update_adjustments(PadCanvas *self) {
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);
  gdouble window_width, window_height;

  pad_canvas_get_window_size(self, &window_width, &window_height);

  if (priv->hadjustment) {
    pad_canvas_configure_hadjustment(self, window_width);
    priv->window_x = -gtk_adjustment_get_value(priv->hadjustment);
  }

  if (priv->vadjustment) {
    pad_canvas_configure_vadjustment(self, window_height);
    priv->window_y = -gtk_adjustment_get_value(priv->vadjustment);
  }
}

static void pad_canvas_adjustment_value_changed(GtkAdjustment *adjustment,
                                                PadCanvas *canvas) {
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(canvas);
  gdouble window_width, window_height;

  if (gtk_widget_get_realized(GTK_WIDGET(canvas))) {
    /* NOTE: GTK+ 3.0 always redraws the entire window when scrolling, so
       we don't need to do this. We could possibly use something like
       GtkPixelCache in future to speed up scrolling, if it is made public. */
    /* Move the window to the new position. */

    pad_canvas_get_window_size(canvas, &window_width, &window_height);

    pad_canvas_update_adjustments(canvas);

    gdk_window_move_resize(priv->canvas_window, priv->window_x, priv->window_y,
                           window_width, window_height);

    pad_canvas_update(canvas);
  }
}

static void pad_canvas_set_hadjustment(PadCanvas *self,
                                       GtkAdjustment *adjustment) {
  g_return_if_fail(PAD_IS_CANVAS(self));

  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);

  if (adjustment && priv->hadjustment == adjustment) {
    return;
  }

  if (priv->hadjustment) {
    g_signal_handlers_disconnect_by_func(
        priv->hadjustment, pad_canvas_adjustment_value_changed, self);
    g_object_unref(priv->hadjustment);
  }

  if (adjustment == NULL) {
    adjustment = gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  }

  g_signal_connect(adjustment, "value-changed",
                   G_CALLBACK(pad_canvas_adjustment_value_changed), self);
  priv->hadjustment = g_object_ref_sink(adjustment);
  pad_canvas_update(self);

  g_object_notify(G_OBJECT(self), "hadjustment");
}

static void pad_canvas_set_vadjustment(PadCanvas *self,
                                       GtkAdjustment *adjustment) {
  g_return_if_fail(PAD_IS_CANVAS(self));

  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);

  if (adjustment && priv->vadjustment == adjustment) {
    return;
  }

  if (priv->vadjustment) {
    g_signal_handlers_disconnect_by_func(
        priv->vadjustment, pad_canvas_adjustment_value_changed, self);
    g_object_unref(priv->vadjustment);
  }

  if (adjustment == NULL)
    adjustment = gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  g_signal_connect(adjustment, "value-changed",
                   G_CALLBACK(pad_canvas_adjustment_value_changed), self);
  priv->vadjustment = g_object_ref_sink(adjustment);
  pad_canvas_update(self);

  g_object_notify(G_OBJECT(self), "vadjustment");
}

static void pad_canvas_dispose(GObject *gobject) {
  PadCanvas *canvas = PAD_CANVAS(gobject);
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(canvas);

  if (priv->world_cs) {
    g_clear_object(&priv->world_cs);
  }

  if (priv->hadjustment) {
    g_clear_object(&priv->hadjustment);
  }

  if (priv->vadjustment) {
    g_clear_object(&priv->vadjustment);
  }

  if (priv->root_item) {
    g_clear_object(&priv->root_item);
  }

  G_OBJECT_CLASS(pad_canvas_parent_class)->dispose(gobject);
}

static void pad_canvas_finalize(GObject *gobject) {
  G_OBJECT_CLASS(pad_canvas_parent_class)->finalize(gobject);
}

static void pad_canvas_get_property(GObject *object, guint prop_id,
                                    GValue *value, GParamSpec *pspec) {
  g_return_if_fail(PAD_IS_CANVAS(object));

  PadCanvas *canvas = PAD_CANVAS(object);
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(canvas);

  switch (prop_id) {

  case PROP_HADJUSTMENT:
    g_value_set_object(value, priv->hadjustment);
    break;

  case PROP_VADJUSTMENT:
    g_value_set_object(value, priv->vadjustment);
    break;

  case PROP_HSCROLL_POLICY:
    g_value_set_enum(value, priv->hscroll_policy);
    break;

  case PROP_VSCROLL_POLICY:
    g_value_set_enum(value, priv->vscroll_policy);
    break;

  case PROP_WORLD_SCALE:
    g_value_set_double(value, priv->world_scale);
    break;

  case PROP_ROOT_ITEM:
    g_value_set_object(value, priv->root_item);
    break;

  case PROP_CANVAS_WINDOW:
    g_value_set_object(value, priv->canvas_window);
    break;

  case PROP_WORLD_COORDINATE_SYSTEM:
    g_value_set_object(value, priv->world_cs);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_canvas_set_property(GObject *object, guint prop_id,
                                    const GValue *value, GParamSpec *pspec) {
  g_return_if_fail(PAD_IS_CANVAS(object));

  PadCanvas *canvas = PAD_CANVAS(object);
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(canvas);

  switch (prop_id) {

  case PROP_HADJUSTMENT:
    pad_canvas_set_hadjustment(canvas, g_value_get_object(value));
    break;

  case PROP_VADJUSTMENT:
    pad_canvas_set_vadjustment(canvas, g_value_get_object(value));
    break;

  case PROP_HSCROLL_POLICY:
    priv->hscroll_policy = g_value_get_enum(value);
    gtk_widget_queue_resize(GTK_WIDGET(canvas));
    break;

  case PROP_VSCROLL_POLICY:
    priv->vscroll_policy = g_value_get_enum(value);
    gtk_widget_queue_resize(GTK_WIDGET(canvas));
    break;

  case PROP_WORLD_SCALE:
    pad_canvas_set_world_scale(PAD_CANVAS(object), g_value_get_double(value));
    break;

  case PROP_ROOT_ITEM:
    g_object_unref(priv->root_item);
    priv->root_item = g_value_get_object(value);
    break;

  case PROP_CANVAS_WINDOW:
    g_object_unref(priv->canvas_window);
    priv->canvas_window = g_value_get_object(value);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void pad_canvas_size_allocate(GtkWidget *widget,
                                     GtkAllocation *allocation) {
  g_print("pad_canvas_size_allocate\n");
  g_return_if_fail(PAD_IS_CANVAS(widget));

  PadCanvasPrivate *priv = pad_canvas_get_instance_private(PAD_CANVAS(widget));

  gtk_widget_set_allocation(widget, allocation);

  if (!gtk_widget_get_realized(widget)) {
    return;
  }

  priv->window_alloc_width = allocation->width;
  priv->window_alloc_height = allocation->height;

  gdk_window_move_resize(gtk_widget_get_window(widget), allocation->x,
                         allocation->y, allocation->width, allocation->height);

  pad_canvas_update(PAD_CANVAS(widget));
}

static void pad_canvas_add(GtkContainer *container, GtkWidget *widget) {}

static void pad_canvas_remove(GtkContainer *container, GtkWidget *widget) {}

static void pad_canvas_forall(GtkContainer *container,
                              gboolean include_internals, GtkCallback callback,
                              gpointer callback_data) {}

static gboolean pad_canvas_draw(GtkWidget *widget, cairo_t *cr) {
  g_print("pad_canvas_draw\n");
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(PAD_CANVAS(widget));
  gdouble clip_bounds_x1, clip_bounds_x2, clip_bounds_y1, clip_bounds_y2;

  if (!gtk_cairo_should_draw_window(cr, priv->canvas_window)) {
    return FALSE;
  }
  cairo_clip_extents(cr, &clip_bounds_x1, &clip_bounds_y1, &clip_bounds_x2,
                     &clip_bounds_y2);

  // draw_area.x = priv->canvas_x_offset;
  // draw_area.y = priv->canvas_y_offset;
  // draw_area.width = priv->world_bound_width;
  // draw_area.height = priv->world_bound_height;

  cairo_save(cr);

  // cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
  // cairo_paint(cr);

  cairo_translate(cr, priv->window_x, priv->window_y);
  cairo_translate(cr, priv->canvas_x_offset, priv->canvas_y_offset);

  cairo_new_path(cr);
  cairo_move_to(cr, 0, 0);
  cairo_line_to(cr, priv->world_bound_width_scaled, 0);
  cairo_line_to(cr, priv->world_bound_width_scaled,
                priv->world_bound_height_scaled);
  cairo_line_to(cr, 0, priv->world_bound_height_scaled);
  cairo_close_path(cr);
  cairo_clip(cr);

  // pad_canvas_debug_draw_background(widget, cr);

  cairo_scale(cr, priv->world_scale, priv->world_scale);
  pad_canvas_item_draw(priv->root_item, cr);

  // pad_canvas_debug_draw_world_center(widget, cr);
  // pad_canvas_debug_draw_grid(widget, cr);
  // pad_canvas_debug_draw_world_bounds(widget, cr);

  cairo_restore(cr);

  return TRUE;
}

static void pad_canvas_realize(GtkWidget *widget) {
  g_print("pad_canvas_realize\n");
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(PAD_CANVAS(widget));
  GtkAllocation allocation;
  GdkWindow *window;
  GdkWindowAttr attributes;
  gint attributes_mask;
  gdouble width_pixels, height_pixels;

  gtk_widget_set_realized(widget, TRUE);

  gtk_widget_get_allocation(widget, &allocation);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = allocation.x;
  attributes.y = allocation.y;
  attributes.width = allocation.width;
  attributes.height = allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual(widget);
  attributes.event_mask = GDK_VISIBILITY_NOTIFY_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;

  window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes,
                          attributes_mask);
  gtk_widget_set_window(widget, window);
  gdk_window_set_user_data(window, widget);

  g_object_get(priv->world_cs, "bound-width", &width_pixels, "bound-height",
               &height_pixels, NULL);
  pad_coordinate_system_distance_in_space(priv->world_cs, &width_pixels,
                                          &height_pixels);
  /* We want to round the sizes up to the next pixel. */
  height_pixels++;
  width_pixels++;

  attributes.x =
      priv->hadjustment ? -gtk_adjustment_get_value(priv->hadjustment) : 0,
  attributes.y =
      priv->vadjustment ? -gtk_adjustment_get_value(priv->vadjustment) : 0;
  /* The actual window size is always at least as big as the widget's window.*/
  attributes.width = MAX((gint)width_pixels, allocation.width);
  attributes.height = MAX((gint)height_pixels, allocation.height);
  attributes.event_mask =
      GDK_EXPOSURE_MASK | GDK_SCROLL_MASK | GDK_BUTTON_PRESS_MASK |
      GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK |
      GDK_KEY_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |
      GDK_FOCUS_CHANGE_MASK | gtk_widget_get_events(widget);

  priv->window_x = attributes.x;
  priv->window_y = attributes.y;

  priv->canvas_window = gdk_window_new(window, &attributes, attributes_mask);
  gdk_window_set_user_data(priv->canvas_window, widget);

  pad_canvas_size_allocate(widget, &allocation);
}

static void pad_canvas_unrealize(GtkWidget *widget) {
  PadCanvas *canvas;
  PadCanvasPrivate *priv;

  g_return_if_fail(PAD_IS_CANVAS(widget));

  canvas = PAD_CANVAS(widget);
  priv = pad_canvas_get_instance_private(canvas);

  gdk_window_set_user_data(priv->canvas_window, NULL);
  gdk_window_destroy(priv->canvas_window);
  priv->canvas_window = NULL;

  return;
  if (GTK_WIDGET_CLASS(pad_canvas_parent_class)->unrealize) {
    GTK_WIDGET_CLASS(pad_canvas_parent_class)->unrealize(widget);
  }
}

static void pad_canvas_map(GtkWidget *widget) {
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(PAD_CANVAS(widget));
  GTK_WIDGET_CLASS(pad_canvas_parent_class)->map(widget);

  gtk_widget_set_mapped(widget, TRUE);

  gdk_window_show(priv->canvas_window);
  gdk_window_show(gtk_widget_get_window(widget));
}

static void pad_canvas_unmap(GtkWidget *widget) {
  GTK_WIDGET_CLASS(pad_canvas_parent_class)->unmap(widget);
}

static void pad_canvas_class_init(PadCanvasClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS(klass);

  gobject_class->dispose = pad_canvas_dispose;
  gobject_class->finalize = pad_canvas_finalize;
  gobject_class->set_property = pad_canvas_set_property;
  gobject_class->get_property = pad_canvas_get_property;

  widget_class->size_allocate = pad_canvas_size_allocate;
  widget_class->realize = pad_canvas_realize;
  widget_class->unrealize = pad_canvas_unrealize;
  widget_class->draw = pad_canvas_draw;
  widget_class->map = pad_canvas_map;
  widget_class->unmap = pad_canvas_unmap;
  widget_class->realize = pad_canvas_realize;
  widget_class->unrealize = pad_canvas_unrealize;
  widget_class->draw = pad_canvas_draw;
  widget_class->map = pad_canvas_map;
  widget_class->unmap = pad_canvas_unmap;

  container_class->add = pad_canvas_add;
  container_class->remove = pad_canvas_remove;
  // container_class->forall = pad_canvas_forall;

  g_object_class_install_property(
      gobject_class, PROP_WORLD_SCALE,
      g_param_spec_double("world-scale", "World scale",
                          "Scale of world coordinate system", 0.00001,
                          G_MAXDOUBLE, 1.0, G_PARAM_READWRITE));

  g_object_class_install_property(
      gobject_class, PROP_ROOT_ITEM,
      g_param_spec_object("root-item", "Root item",
                          "The root item of the canvas", PAD_TYPE_CANVAS_ITEM,
                          G_PARAM_READWRITE));

  g_object_class_install_property(
      gobject_class, PROP_CANVAS_WINDOW,
      g_param_spec_object("canvas-window", "Canvas window",
                          "The window of the canvas that gets scrolled around",
                          GDK_TYPE_WINDOW, G_PARAM_READABLE));

  g_object_class_install_property(
      gobject_class, PROP_WORLD_COORDINATE_SYSTEM,
      g_param_spec_object("world-coordinate-system", "World coordinate system",
                          "The canvas world coordinate system",
                          PAD_TYPE_COORDINATE_SYSTEM, G_PARAM_READABLE));

  /* GtkScrollable Interface */
  g_object_class_override_property(gobject_class, PROP_HADJUSTMENT,
                                   "hadjustment");
  g_object_class_override_property(gobject_class, PROP_VADJUSTMENT,
                                   "vadjustment");
  g_object_class_override_property(gobject_class, PROP_HSCROLL_POLICY,
                                   "hscroll-policy");
  g_object_class_override_property(gobject_class, PROP_VSCROLL_POLICY,
                                   "vscroll-policy");
}

static void pad_canvas_init(PadCanvas *self) {
  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);

  priv->world_scale = 1;
  priv->world_cs = pad_coordinate_system_new();
  priv->root_item = PAD_CANVAS_ITEM(pad_canvas_item_group_new(NULL, NULL));
}

/**
 * pad_canvas_new:
 *
 * Creates a new #PadCanvas
 *
 * Returns: The newly created #PadCanvas.
 */
GtkWidget *pad_canvas_new(void) {
  GtkWidget *widget = g_object_new(PAD_TYPE_CANVAS, NULL);

  return widget;
}

/**
 * pad_canvas_set_world_bounds:
 * @self: The #PadCanvas you want to set world bounds
 * @width: The width of the bound in x direction
 * @height: The height of the bound in y direction
 *
 * The width and height of the #PadCanvas world bounds are the actually
 * drawing area. If for example you set width and height to 10 you got a area of
 * width * height = 100 to draw on.
 */
void pad_canvas_set_world_bounds(PadCanvas *self, gdouble width,
                                 gdouble height) {
  g_return_if_fail(PAD_IS_CANVAS(self));

  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);

  g_object_set(priv->world_cs, "bound-width", width, "bound-height", height,
               NULL);

  priv->world_bound_width = width;
  priv->world_bound_height = height;

  pad_coordinate_system_distance_in_space(priv->world_cs, &width, &height);
  priv->world_bound_width_scaled = width;
  priv->world_bound_height_scaled = height;

  pad_canvas_update_adjustments(self);

  pad_canvas_update(self);
}

/**
 * pad_canvas_set_world_scale:
 * @self The #PadCanvas you want to scale the world
 * @scale The factor on which the world should be scaled
 *
 * Scales the #PadCanvas world #PadCoordinateSystem.
 */
void pad_canvas_set_world_scale(PadCanvas *self, gdouble scale) {
  gdouble width, height;
  PadCanvasPrivate *priv;

  g_return_if_fail(PAD_IS_CANVAS(self));

  priv = pad_canvas_get_instance_private(self);

  g_object_set(priv->world_cs, "scale", scale, NULL);
  priv->world_scale *= scale;

  width = priv->world_bound_width;
  height = priv->world_bound_height;
  pad_coordinate_system_distance_in_space(priv->world_cs, &width, &height);

  priv->world_bound_width_scaled = width;
  priv->world_bound_height_scaled = height;

  pad_canvas_update(self);
}

/**
 * pad_canvas_window_to_world:
 * @self The #PadCanvas you want the coordinates converted
 * @x The x coordinate
 * @y The y coordinate
 *
 * Converts the coordinates x and y from #PadCanvas window space to
 * #PadCanvas world space.
 */
void pad_canvas_window_to_world(PadCanvas *self, gdouble *x, gdouble *y) {
  g_return_if_fail(PAD_IS_CANVAS(self));

  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);
  gdouble org_x = priv->canvas_x_offset, org_y = priv->canvas_y_offset;
  gdouble world_cs_scale = priv->world_scale;

  *x -= org_x;
  *y -= org_y;
  *x *= 1 / world_cs_scale;
  *y *= 1 / world_cs_scale;
}

/**
 * pad_canvas_world_to_window:
 * @self The #PadCanvas you want the coordinates converted
 * @x The x coordinate
 * @y The y coordinate
 *
 * Converts the coordinates x and y from #PadCanvas world space to
 * #PadCanvas window space.
 */
void pad_canvas_world_to_window(PadCanvas *self, gdouble *x, gdouble *y) {
  g_return_if_fail(PAD_IS_CANVAS(self));

  PadCanvasPrivate *priv = pad_canvas_get_instance_private(self);

  pad_coordinate_system_point_in_space(priv->world_cs, x, y);
  x += priv->canvas_x_offset;
  y += priv->canvas_y_offset;
}

/**
 * pad_canvas_scroll_to:
 * NOT IMPLEMENTED YET
 */
void pad_canvas_scroll_to(PadCanvas *self, gdouble vadjustment,
                          gdouble hadjustment) {

  return;
  PadCanvasPrivate *priv;

  g_return_if_fail(PAD_IS_CANVAS(self));

  priv = pad_canvas_get_instance_private(self);

  gtk_adjustment_set_value(priv->vadjustment, vadjustment);
  gtk_adjustment_set_value(priv->hadjustment, hadjustment);

  pad_canvas_adjustment_value_changed(NULL, self);
}
