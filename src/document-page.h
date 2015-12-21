#ifndef __DOCUMENT_PAGE_H
#define __DOCUMENT_PAGE_H

#include <gtk/gtk.h>
#include <cairo.h>

G_BEGIN_DECLS

#define TYPE_DOCUMENT_PAGE              (document_page_get_type ())
#define DOCUMENT_PAGE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_DOCUMENT_PAGE, DocumentPage))
#define DOCUMENT_PAGE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_DOCUMENT_PAGE, DocumentPageClass))
#define IS_DOCUMENT_PAGE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_DOCUMENT_PAGE))
#define IS_DOCUMENT_PAGE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_DOCUMENT_PAGE))
#define DOCUMENT_PAGE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_DOCUMENT_PAGE, DocumentPageClass))


typedef struct _DocumentPage            DocumentPage;
typedef struct _DocumentPageClass       DocumentPageClass;
typedef struct _DocumentPagePrivate     DocumentPagePrivate;


struct _DocumentPage {
  GtkWidget widget;

  DocumentPagePrivate *priv;
};

struct _DocumentPageClass {
  GtkWidgetClass parent_class;
};

GType document_page_get_type (void) G_GNUC_CONST;

GtkWidget * document_page_new(gint width, gint height);

gint document_page_get_original_width(DocumentPage *page);
gint document_page_get_original_height(DocumentPage *page);

gint document_page_get_width(DocumentPage *page);
gint document_page_get_height(DocumentPage *page);

void document_page_set_loading(DocumentPage *page, gboolean loading);
gboolean document_page_is_loading(DocumentPage *page);

void document_page_scale(DocumentPage *page, gdouble scale);

void document_page_set_surface(DocumentPage *page, cairo_surface_t *surface);
cairo_surface_t * document_page_get_surface(DocumentPage *page);

G_END_DECLS

#endif /* __DOCUMENT_PAGE_H */
