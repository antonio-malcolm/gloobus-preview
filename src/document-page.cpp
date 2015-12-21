#include "document-page.h"

#include "gloobus-preview-defines.h"

struct _DocumentPagePrivate
{
    gint                  original_width;
    gint                  original_height;

    gint                  required_width;
    gint                  required_height;

    gboolean              loading;

    cairo_surface_t      *surface;
};

static void document_page_init                  (DocumentPage      *page);

static void document_page_class_init            (DocumentPageClass *klass);

static gint document_page_draw                  (GtkWidget         *widget,
                                                 cairo_t           *cr);

static void document_page_get_preferred_width   (GtkWidget         *widget,
                                                 gint              *minimum,
                                                 gint              *natural);

static void document_page_get_preferred_height  (GtkWidget         *widget,
                                                 gint              *minimum,
                                                 gint              *natural);

static void document_page_destroy               (GtkWidget         *widget);

enum {
    PROP_NONE,
    PROP_PAGE_WIDTH,
    PROP_PAGE_HEIGHT
};

G_DEFINE_TYPE (DocumentPage, document_page, GTK_TYPE_WIDGET)

static void
document_page_class_init(DocumentPageClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    widget_class->draw = document_page_draw;
    widget_class->destroy = document_page_destroy;
    widget_class->get_preferred_width = document_page_get_preferred_width;
    widget_class->get_preferred_height = document_page_get_preferred_height;

    g_type_class_add_private (klass, sizeof (DocumentPagePrivate));
}

static void
document_page_init(DocumentPage *page)
{
    DocumentPagePrivate *priv;

    page->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (page,
                                                     TYPE_DOCUMENT_PAGE,
                                                     DocumentPagePrivate);

    gtk_widget_set_has_window (GTK_WIDGET (page), FALSE);

    priv->surface = NULL;
}

static void
document_page_destroy (GtkWidget *widget)
{
    DocumentPage *page = DOCUMENT_PAGE (widget);

    if (page->priv->surface != NULL) {
        cairo_surface_destroy(page->priv->surface);
        page->priv->surface = NULL;
    }

    GTK_WIDGET_CLASS (document_page_parent_class)->destroy (widget);
}

GtkWidget * document_page_new(gint width, gint height)
{
   DocumentPage *page = DOCUMENT_PAGE(g_object_new(TYPE_DOCUMENT_PAGE, NULL));

   DocumentPagePrivate *priv = page->priv;

   priv->original_width = width;
   priv->original_height = height;

   priv->required_width = width;
   priv->required_height = height;

   return GTK_WIDGET(page);
}

void document_page_set_surface(DocumentPage *page, cairo_surface_t *surface)
{
    DocumentPagePrivate *priv = page->priv;

    if (priv->surface != NULL) {
        cairo_surface_destroy(priv->surface);
        priv->surface = NULL;
    }

    if (surface != NULL) {
        priv->surface = cairo_surface_reference(surface);
    }

    priv->loading = false;

    gtk_widget_queue_draw (GTK_WIDGET(page));
}

cairo_surface_t * document_page_get_surface(DocumentPage *page)
{
    return page->priv->surface;
}

gint document_page_get_original_width(DocumentPage *page)
{
    return page->priv->original_width;
}

gint document_page_get_original_height(DocumentPage *page)
{
    return page->priv->original_height;
}

gint document_page_get_width(DocumentPage *page)
{
    return page->priv->required_width;
}

gint document_page_get_height(DocumentPage *page)
{
    return page->priv->required_height;
}

void document_page_set_loading(DocumentPage *page, gboolean loading)
{
    page->priv->loading = loading;
}

gboolean document_page_is_loading(DocumentPage *page)
{
    return page->priv->loading;
}

void document_page_scale(DocumentPage *page, gdouble scale)
{
    DocumentPagePrivate *priv = page->priv;
    priv->required_width = scale * priv->original_width + 0.5;
    priv->required_height = scale * priv->original_height + 0.5;
    gtk_widget_queue_draw (GTK_WIDGET(page));
}

static gint
document_page_draw (GtkWidget *widget,
                    cairo_t   *cr)
{
    g_return_val_if_fail (IS_DOCUMENT_PAGE (widget), FALSE);

    DocumentPage *page = DOCUMENT_PAGE (widget);
    DocumentPagePrivate *priv = page->priv;

    double x = 0.5 * (gtk_widget_get_allocated_width (widget) - priv->required_width);
    double y = 0.5 * (gtk_widget_get_allocated_height (widget) - priv->required_height);

    if (priv->surface != NULL) {
        int width = cairo_image_surface_get_width(priv->surface);
        if (priv->required_width != width) {
            double scale = (double)priv->required_width / (double)width;
            cairo_scale(cr, scale, scale);
        }
        cairo_set_source_surface(cr, priv->surface, x, y);
        cairo_paint(cr);
    }
    else {
        cairo_rectangle(cr, x, y, priv->required_width, priv->required_height);
        cairo_clip(cr);
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_paint(cr);

        cairo_select_font_face( cr,
                                "sans-serif",
                                CAIRO_FONT_SLANT_NORMAL,
                                CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size( cr, 28 );
        cairo_set_source_rgb( cr, 0.5, 0.5, 0.5 );

        cairo_text_extents_t extents;
        cairo_text_extents (cr, _("LOADING"), &extents);

        x = 0.5 * (priv->required_width - extents.width);
        y = 0.5 * (priv->required_height - extents.height);
        cairo_move_to (cr, x, y);
        cairo_show_text (cr, _("LOADING"));
    }

    return false;
}


static void
document_page_get_preferred_width (GtkWidget *widget,
                                   gint      *minimum,
                                   gint      *natural)
{
    *natural = *minimum = DOCUMENT_PAGE(widget)->priv->required_width;
}

static void
document_page_get_preferred_height (GtkWidget *widget,
                                    gint      *minimum,
                                    gint      *natural)
{
    *natural = *minimum = DOCUMENT_PAGE(widget)->priv->required_height;
}
