/*
 * 5.4.18 Felix Weilbach
 */

/**
 * SECTION:pad-canvas-item-pdf
 * @Short_description: The canvas item pdf object.
 * @Title: PadCanvasItemPdf
 *
 * Canvas item that draws a pdf page on #PadCanvas.
 */

#include "pad-canvas-item-pdf.h"
#include "pad-canvas-item-group.h"

typedef struct _PadCanvasItemPdfPrivate {

  PopplerPage *pdf_page;

  gdouble image_data_width;
  gdouble image_data_height;

} PadCanvasItemPdfPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(PadCanvasItemPdf, pad_canvas_item_pdf,
                           PAD_TYPE_CANVAS_ITEM)

static void pad_canvas_item_pdf_finalize(GObject *gobject) {
  G_OBJECT_CLASS(pad_canvas_item_pdf_parent_class)->finalize(gobject);
}

static void pad_canvas_item_pdf_dispose(GObject *gobject) {
  PadCanvasItemPdfPrivate *priv =
      pad_canvas_item_pdf_get_instance_private(PAD_CANVAS_ITEM_PDF(gobject));

  G_OBJECT_CLASS(pad_canvas_item_pdf_parent_class)->dispose(gobject);
}

static void pad_canvas_item_pdf_class_init(PadCanvasItemPdfClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  PadCanvasItemClass *canvas_item_class = PAD_CANVAS_ITEM_CLASS(klass);

  gobject_class->dispose = pad_canvas_item_pdf_dispose;
  gobject_class->finalize = pad_canvas_item_pdf_finalize;

  canvas_item_class->draw = pad_canvas_item_pdf_draw;
  canvas_item_class->add = pad_canvas_item_add;
}

static void pad_canvas_item_pdf_init(PadCanvasItemPdf *self) {}

PadCanvasItem *pad_canvas_item_pdf_new(PadCanvasItem *parent_item,
                                       PopplerPage *pdfpage, ...) {
  PadCanvasItemPdfPrivate *priv;
  va_list var_args;
  const char *first_property;
  PadCanvasItem *item =
      g_object_new(PAD_TYPE_CANVAS_ITEM_PDF, "parent-item", parent_item, NULL);
  PadCanvasItemPdf *item_pdf = PAD_CANVAS_ITEM_PDF(item);

  priv = pad_canvas_item_pdf_get_instance_private(item_pdf);

  priv->pdf_page = pdfpage;
  poppler_page_get_size(pdfpage, &priv->image_data_width,
                        &priv->image_data_height);

  va_start(var_args, pdfpage);
  first_property = va_arg(var_args, char *);
  if (first_property) {
    g_object_set_valist(G_OBJECT(item), first_property, var_args);
  }
  va_end(var_args);

  return item;
}

void pad_canvas_item_pdf_draw(PadCanvasItem *self, cairo_t *cr,
                              PadCanvasDrawArea *draw_area) {
  PadCanvasItemPdfPrivate *priv;
  PopplerPage *pdf_page;

  priv = pad_canvas_item_pdf_get_instance_private(PAD_CANVAS_ITEM_PDF(self));
  pdf_page = priv->pdf_page;

  if (pdf_page == NULL) {
    return;
  }

  cairo_save(cr);

  cairo_translate(cr, self->world_x, self->world_y);

  cairo_new_path(cr);
  cairo_move_to(cr, 0, 0);
  cairo_line_to(cr, priv->image_data_width, 0);
  cairo_line_to(cr, priv->image_data_width, priv->image_data_height);
  cairo_line_to(cr, 0, priv->image_data_height);
  cairo_close_path(cr);
  cairo_clip(cr);

  cairo_set_source_rgb(cr, 1, 1, 1);

  cairo_paint(cr);

  poppler_page_render(pdf_page, cr);

  cairo_restore(cr);
}

void pad_canvas_item_pdf_add(PadCanvasItem *self, PadCanvasItem *child) {
  self->child = child;
}
