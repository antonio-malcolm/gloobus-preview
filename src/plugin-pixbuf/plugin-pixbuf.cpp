#include "plugin-pixbuf.h"

iPixbuf::iPixbuf() {
        g_debug("Creating iPixbuf");
        menuItemBackground = ui->add_menu_item ( _("Set as Background"), G_CALLBACK(set_as_background_cb), NULL );

}

iPixbuf::~iPixbuf() {
        g_debug("Destroying iPixbuf");
        gtk_widget_destroy(menuItemBackground);
}

static void
size_prepared_cb (GdkPixbufLoader *loader,
                  gint             width,
                  gint             height,
                  GdkRectangle    *desired_size)
{
        double desired_ratio, ratio, w, h;

        /*
         Background image should be resized and cropped. That means the 
         image is centered and scaled to make sure the shortest side fit 
         the home background exactly with keeping the aspect ratio of the 
         original image
         */

        desired_ratio = (1. * desired_size->width) / desired_size->height;
        ratio = (1. * width) / height;

        if (ratio > desired_ratio)
        {
                h = desired_size->height;
                w = ratio * desired_size->height;
        }
        else
        {
                h = desired_size->width / ratio;
                w = desired_size->width;
        }

        gdk_pixbuf_loader_set_size (loader, w, h);
}

GdkPixbuf * iPixbuf::get_pixbuf ()
{
        g_debug("PLUGIN: Loading photo (%s)",m_filename.c_str());
        //return gdk_pixbuf_new_from_file (m_filename.c_str(),NULL);
        GCancellable *cancellable = NULL;
        GError *error = NULL;
        GFileInputStream *stream = NULL;
        GdkPixbufLoader *loader = NULL;
        guchar buffer[8192];
        gssize read_bytes;
        GdkPixbuf *pixbuf = NULL;
        GdkRectangle *desired_size;


        /* Open file for read */
        stream = g_file_read (m_gfile, cancellable, &error);
        if (!stream)
                g_debug("error opening stream: %s", error->message); 

        /* Create pixbuf loader */
        desired_size = g_new0 (GdkRectangle, 1);
        desired_size->width = -1;
        desired_size->height = -1;

        loader = gdk_pixbuf_loader_new ();
        if (desired_size->width !=-1 || desired_size->height !=-1)
                g_signal_connect_data (loader, "size-prepared",
                                       G_CALLBACK (size_prepared_cb),
                                       desired_size,
                                       (GClosureNotify) g_free,
                                       G_CONNECT_AFTER);

        /* Parse input stream into the loader */
        do {
                read_bytes = g_input_stream_read (G_INPUT_STREAM (stream),
                                                  buffer,
                                                  sizeof (buffer),
                                                  cancellable,
                                                  &error);
                if (read_bytes < 0)
                {
                        gdk_pixbuf_loader_close (loader, NULL);
                        //TODO add error msg
                }

                if (!gdk_pixbuf_loader_write (loader,
                                              buffer,
                                              read_bytes,
                                              &error))
                {
                        gdk_pixbuf_loader_close (loader, NULL);
                        //TODO add error msg
                }
        } while (read_bytes > 0);

        /* Close pixbuf loader */
        if (!gdk_pixbuf_loader_close (loader, &error))
                ; //TODO add error msg
        /* Close input stream */
        if (!g_input_stream_close (G_INPUT_STREAM (stream),
                                   cancellable,
                                   &error))
                ; //TODO add error msg

        /* Set resulting pixbuf */
        m_animation = gdk_pixbuf_loader_get_animation (loader);
        pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
       
        return (pixbuf);
        
        /*return gdk_pixbuf_new_from_stream (G_INPUT_STREAM(stream), cancellable, &error);*/
}


//Set as Background cb

void iPixbuf::set_as_background_cb()
{
        const gchar *uri = g_file_get_uri (Gloobus::instance()->get_file());
        g_debug("Set as background current image %s", uri);

        GSettingsSchemaSource *schema_source = g_settings_schema_source_get_default ();
        GSettingsSchema *background_schema = g_settings_schema_source_lookup (schema_source, "org.gnome.desktop.background", FALSE);
        if (background_schema) {
                GSettings *background_settings = g_settings_new ("org.gnome.desktop.background");
                g_settings_set_string (background_settings, "picture-uri", uri);
                g_object_unref (background_settings);
                g_settings_schema_unref (background_schema);
	}
}
