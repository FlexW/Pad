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

  cairo_pattern_t *image_data_pattern;
  gdouble image_data_width;
  gdouble image_data_height;

} PadCanvasItemPdfPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(PadCanvasItemPdf, pad_canvas_item_pdf,
                           PAD_TYPE_CANVAS_ITEM)

static cairo_surface_t *
pad_canvas_item_pdf_create_surface_from_poppler(PadCanvasItemPdf *self,
                                                PopplerPage *pdf_page) {
  PadCanvasItemPdfPrivate *priv =
      pad_canvas_item_pdf_get_instance_private(self);
  cairo_format_t format = CAIRO_FORMAT_ARGB32;
  return NULL;
}

static cairo_pattern_t *
pad_canvas_item_pdf_create_pattern_from_poppler(PadCanvasItemPdf *self,
                                                PopplerPage *pdf_page) {
  cairo_surface_t *surface;
  cairo_pattern_t *pattern;

  surface = pad_canvas_item_pdf_create_surface_from_poppler(self, pdf_page);
  pattern = cairo_pattern_create_for_surface(surface);

  cairo_surface_destroy(surface);

  return pattern;
}

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
}

static void pad_canvas_item_pdf_init(PadCanvasItemPdf *self) {}

PadCanvasItem *pad_canvas_item_pdf_new(PadCanvasItem *parent_item,
                                       PopplerPage *pdfpage, ...) {
  PadCanvasItemPdfPrivate *priv;
  va_list var_args;
  const char *first_property;
  PadCanvasItem *item = g_object_new(PAD_TYPE_CANVAS_ITEM_PDF, NULL);
  PadCanvasItemPdf *item_pdf = PAD_CANVAS_ITEM_PDF(item);

  priv = pad_canvas_item_pdf_get_instance_private(item_pdf);

  if (PAD_IS_CANVAS_ITEM_GROUP(parent_item)) {
    pad_canvas_item_group_add_item(PAD_CANVAS_ITEM_GROUP(parent_item), item);
  }

  priv->pdf_page = pdfpage;

  priv->image_data_pattern =
      0; // pad_canvas_item_pdf_create_pattern_from_poppler(item_pdf, pdfpage);
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

void pad_canvas_item_pdf_draw(PadCanvasItem *self, cairo_t *cr) {
  PadCanvasItemPdfPrivate *priv;
  PopplerPage *pdf_page;

  priv = pad_canvas_item_pdf_get_instance_private(PAD_CANVAS_ITEM_PDF(self));
  pdf_page = priv->pdf_page;

  if (pdf_page == NULL) {
    return;
  }

  cairo_save(cr);

  poppler_page_render(pdf_page, cr);

  cairo_restore(cr);
}
