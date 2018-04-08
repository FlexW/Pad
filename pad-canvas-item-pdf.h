/*
 * 5.4.18 Felix Weilbach
 */

#ifndef __PAD_CANVAS_ITEM_PDF_H__
#define __PAD_CANVAS_ITEM_PDF_H__

#include <poppler.h>

#include "pad-canvas-item.h"

G_BEGIN_DECLS

#define PAD_TYPE_CANVAS_ITEM_PDF pad_canvas_item_pdf_get_type()
G_DECLARE_DERIVABLE_TYPE(PadCanvasItemPdf, pad_canvas_item_pdf, PAD,
                         CANVAS_ITEM_PDF, PadCanvasItem);

typedef struct _PadCanvasItemPdfClass {
  PadCanvasItemClass parent_class;

  gpointer padding[30];

} PadCanvasItemPdfClass;

PadCanvasItem *pad_canvas_item_pdf_new(PadCanvasItem *parent_item,
                                       PopplerPage *pdfpage, ...);

void pad_canvas_item_pdf_draw(PadCanvasItem *self, cairo_t *cr);

void pad_canvas_item_pdf_add(PadCanvasItem *self, PadCanvasItem *child);

G_END_DECLS

#endif /* __PAD_CANVAS_ITEM_PDF__ */
