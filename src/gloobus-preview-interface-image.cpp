#include "gloobus-preview-interface-image.h"

iImage::iImage()
	:m_pixbuf(0)
	,m_pixbufFull(0)
    ,m_animation(0)
	,m_image(0)
	,m_image_container(0)
{
	g_debug("Creating iImage");
}

// ========================= DESTRUCTOR ================================== //
iImage::~iImage()
{
	g_debug("Destroying iImage");
}
//=========================  END ========================================= //
void iImage::end()
{
	g_debug("Ending iImage");
	if(m_pixbuf)
		g_object_unref(m_pixbuf);
	if(m_pixbufFull)
		g_object_unref(m_pixbufFull);
}
// ========================= CONSTRUCTOR ================================= //
bool iImage::load()
{
	g_debug("Loading iImage");
	GdkPixbuf* pixbuf = get_pixbuf();
	if(pixbuf == NULL) {
		m_error = g_error_new(g_quark_from_string ("iImage"), 101,
			_("Error loading image"));
		return false;
	}
	int w = ui->get_max_width();
	int h = ui->get_max_height(true);
	m_pixbuf 	 = ui->pixbuf_scale(pixbuf, w, h);
	m_pixbufFull = ui->pixbuf_scale(pixbuf, ui->get_screen_width(), ui->get_screen_height());
	g_object_unref(pixbuf);

	return true;
}

int  iImage::get_width()
{
	return gdk_pixbuf_get_width(m_pixbuf);
}

int  iImage::get_height()
{
	return gdk_pixbuf_get_height(m_pixbuf);
}

// ================== DRAW =========================================== //
void iImage::draw( GtkContainer * container )
{
    m_image = gtk_image_new_from_pixbuf (m_pixbuf);

    //Check if it is an animation
    if (m_animation != NULL && !gdk_pixbuf_animation_is_static_image(m_animation))
            gtk_image_set_from_animation (GTK_IMAGE(m_image), m_animation);
            
	gtk_container_add (container, m_image);
	gtk_widget_show(m_image);
	
	draw_toolbar();
}

void iImage::draw_fullscreen ( GtkContainer * container )
{
	gtk_image_set_from_pixbuf	(GTK_IMAGE(m_image), m_pixbufFull);
	if (m_animation != NULL && !gdk_pixbuf_animation_is_static_image(m_animation))
    	gtk_image_set_from_animation (GTK_IMAGE(m_image), m_animation);
}

void iImage::draw_unfullscreen ( GtkContainer * container )
{
	gtk_image_set_from_pixbuf	(GTK_IMAGE(m_image), m_pixbuf);
	if (m_animation != NULL && !gdk_pixbuf_animation_is_static_image(m_animation))
     	gtk_image_set_from_animation (GTK_IMAGE(m_image), m_animation);
}
