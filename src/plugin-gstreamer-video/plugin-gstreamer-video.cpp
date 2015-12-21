#include "plugin-gstreamer-video.h"

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>  // for GDK_WINDOW_XID
#endif

#define IVIDEO_WIDTH 200 //make it small, the window will be resized when video is loaded
#define IVIDEO_HEIGHT 200

struct iVideoTagsPack {
    iVideo      *video;
    GstTagList  *tags;
};

iVideo::iVideo()
    :bus(0)
    ,playbin(0)
    ,audio_capsfilter(0)
    ,video_overlay(0)
    ,bus_msg_id(0)
    ,bus_msg_id_sync(0)
    ,interface_update_id(0)
    ,resize_id(0)
    ,configure_id(0)
    ,video_box(0)
    ,video_width(0)
    ,video_height(0)
    ,video_state(GST_STATE_NULL)
    ,tags(0)
    ,lock( g_mutex_new() )
    ,gui_thread( g_thread_self() )
    ,is_fullscreen(false)
{
    g_debug("Creating iVideo");
}

iVideo::~iVideo()
{
    g_debug("Destroy iVideo");
    if(playbin)
        gst_object_unref (GST_OBJECT (playbin));
    if(bus)
        gst_object_unref (GST_OBJECT (bus));
    if(tags)
        gst_tag_list_free (tags);
}

bool iVideo::load( void )
{
    g_debug("Loading iVideo");

    GstElement *audio_sink, *video_sink;

    gst_init (NULL, NULL);
    //gst_pb_utils_init ();

    playbin = gst_element_factory_make ("playbin", "playbin");
    if( !playbin ) {
        m_error = g_error_new(g_quark_from_string ("iVideo"), 1601,
            _("Error initializing video interface"));
        return false;
    }

    bus = gst_element_get_bus (playbin);
    gst_bus_add_signal_watch (bus);
    bus_msg_id = g_signal_connect( bus, "message", G_CALLBACK (bus_message_cb), this);

    audio_sink = gst_element_factory_make ("autoaudiosink", "audio-sink");
    if( !audio_sink ) {
        m_error = g_error_new(g_quark_from_string ("iVideo"), 1602,
            _("Failed to open audio output"));
        return false;
     }

    video_sink = gst_element_factory_make ("autovideosink", "video-sink");
    if( !video_sink ) {
        m_error = g_error_new(g_quark_from_string ("iVideo"), 1602,
            _("Failed to open video output"));
        return false;
    }

    GstStateChangeReturn ret;

    gst_element_set_bus(video_sink, bus);
    ret = gst_element_set_state (video_sink, GST_STATE_READY);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        gst_element_set_state (video_sink, GST_STATE_NULL);
        gst_object_unref (video_sink);
        /* Try again with ximagesink */
        video_sink = gst_element_factory_make ("ximagesink", "video-sink");
        gst_element_set_bus (video_sink, bus);
        ret = gst_element_set_state (video_sink, GST_STATE_READY);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            m_error = g_error_new(g_quark_from_string ("iVideo"), 1602,
                _("Failed to open video output"));
            return false;
        }
    }

    gst_element_set_state (audio_sink, GST_STATE_NULL);
    GstElement *bin;
    GstPad *pad;

    audio_capsfilter = gst_element_factory_make ("capsfilter", "audiofilter");
    bin = gst_bin_new ("audiosinkbin");
    gst_bin_add_many (GST_BIN (bin), audio_capsfilter, audio_sink, NULL);
    gst_element_link_pads (audio_capsfilter, "src", audio_sink, "sink");

    pad = gst_element_get_static_pad (audio_capsfilter, "sink");
    gst_element_add_pad (bin, gst_ghost_pad_new ("sink", pad));
    gst_object_unref (pad);

    audio_sink = bin;

    g_object_set (playbin, "video-sink", video_sink, NULL);
    g_object_set (playbin, "audio-sink", audio_sink, NULL);

    g_signal_connect(playbin, "video-changed", G_CALLBACK(stream_changed_cb), this);
    g_signal_connect(playbin, "audio-changed", G_CALLBACK(stream_changed_cb), this);
    g_signal_connect(playbin, "text-changed" , G_CALLBACK(stream_changed_cb), this);

    g_signal_connect (playbin, "audio-tags-changed",
        G_CALLBACK (audio_tags_changed_cb), this);
    g_signal_connect (playbin, "text-tags-changed",
        G_CALLBACK (text_tags_changed_cb), this);
    g_signal_connect (playbin, "video-tags-changed",
        G_CALLBACK (video_tags_changed_cb), this);

    GstElement *element = gst_bin_get_by_interface (GST_BIN (video_sink),
                                        GST_TYPE_VIDEO_OVERLAY);
    if (GST_IS_VIDEO_OVERLAY (element)) {
        g_debug("Found video overlay: %s", GST_OBJECT_NAME (element));
        video_overlay = GST_VIDEO_OVERLAY (element);
    } else {
        g_debug("No video overlay found");
        if (element)
            gst_object_unref (element);
        video_overlay = NULL;
    }

    //g_signal_connect (video_sink, "element-added",
    //  G_CALLBACK (got_new_video_sink_bin_element), this);

    gst_bus_set_sync_handler (bus, gst_bus_sync_signal_handler, this, NULL);
    bus_msg_id_sync = g_signal_connect( bus, "sync-message::element",
        G_CALLBACK (bus_message_sync_cb), this);

    return true;
}

void iVideo::end( void )
{
    g_debug("Ending iVideo");
    stop_timeout();
    if(configure_id)
        g_source_remove(configure_id);
    if(playbin)
        gst_element_set_state(GST_ELEMENT (playbin), GST_STATE_NULL); //stop video
}

int iVideo::get_width( void )
{
    return IVIDEO_WIDTH;
}

int iVideo::get_height( void )
{
    return IVIDEO_HEIGHT;
}

void iVideo::draw( GtkContainer *container )
{
    g_debug("Drawing iVideo");

    gtk_alignment_set(GTK_ALIGNMENT(container), 0.5,0.5,0,0);

    video_box = gtk_event_box_new();

    GdkScreen * screen = gdk_screen_get_default();
    GdkVisual *visual = gdk_screen_get_system_visual(screen);
    if (visual) {
        gtk_widget_set_visual(video_box, visual);
    }

    GdkRGBA color = {0.,0.,0.,1.};
    gtk_widget_override_background_color (video_box, GTK_STATE_FLAG_NORMAL, &color);

    gtk_widget_set_double_buffered (video_box, false);

    gtk_container_add(container, video_box);

    gtk_widget_show_all(GTK_WIDGET(container));

    draw_toolbar();

    // start playback when all widgets are created
    g_idle_add(&start,this);
}

void iVideo::draw_fullscreen( GtkContainer *container )
{
    g_debug("DRAWING FULLSCREEN");
    is_fullscreen = true;
    gint w, h, s_w, s_h;
    s_w = ui->get_screen_width();
    s_h = ui->get_screen_height();

    double vratio = double(video_height)/double(video_width);
    double sratio = double(s_h)/double(s_w);

    if(vratio == sratio) {
        g_debug("1");
        w = s_w;
        h = s_h;
    } else if(vratio < sratio) {
        g_debug("2");
        w = s_w;
        h = double(video_height)*(double(s_w)/double(video_width));
    } else {
        g_debug("3");
        w = double(video_width)*(double(s_h)/double(video_height));
        h = s_h;
    }
    g_debug("HM: %dx%d", w, h);

    gtk_widget_set_size_request(video_box, w, h);
}

void iVideo::draw_unfullscreen( GtkContainer *container )
{
    is_fullscreen = false;
    gtk_widget_set_size_request(video_box, scaled_width, scaled_height);
}

gboolean iVideo::start( gpointer data )
{

    g_debug("starting iVideo");
    iVideo *video = (iVideo*)data;
    g_object_set( G_OBJECT (video->playbin), "uri", g_file_get_uri( video->m_gfile ), NULL);
    video->set_playing( video->play_pause() );
    video->start_timeout();
    return false;
}

bool iVideo::play_pause( void )
{
    GstState state;
    gst_element_get_state(playbin, &state, NULL, 0);
    if( state > GST_STATE_PAUSED ) {
        gst_element_set_state (playbin, GST_STATE_PAUSED);
        video_state = GST_STATE_PAUSED;
        return true;
    } else {
        gst_element_set_state(playbin, GST_STATE_PLAYING);
        video_state = GST_STATE_PLAYING;
        return false;
    }
}

void iVideo::stop( void )
{
    gst_element_set_state (playbin, GST_STATE_READY);
    video_state = GST_STATE_READY;
}

void iVideo::set_position( gint sec )
{
    gst_element_seek_simple(playbin, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, gint64(sec)*GST_SECOND);
    update_position( sec );
}

gint iVideo::get_position( void )
{
    GstFormat format = GST_FORMAT_TIME;
    gint64 pos;
    gboolean result;
    result = gst_element_query_position (playbin, format, &pos);
    if (!result || format != GST_FORMAT_TIME)
        return 0;
    return pos/GST_SECOND;
}

gint iVideo::get_duration( void )
{
    GstFormat format = GST_FORMAT_TIME;
    gint64 len;
    gboolean result;
    result = gst_element_query_duration (playbin, format, &len);
    if( !result || format != GST_FORMAT_TIME )
        return 0;
    return len/GST_SECOND;
}

void
iVideo::bus_message_cb( GstBus *bus, GstMessage *message, gpointer data)
{
//g_debug("BUS_MESSAGE_CB START");
    iVideo *video = (iVideo*)data;

    switch (GST_MESSAGE_TYPE (message)) {
        case GST_MESSAGE_EOS: {
            // TODO: when better configuration is ready, make here possible to go to the next file
            //video->state = GST_STATE_READY;
            gst_element_set_state (video->playbin, GST_STATE_READY);
            video->update_position(0);
            video->set_playing(false);
            video->stop_timeout();
            break;
        }
        case GST_MESSAGE_ERROR: {
            gst_element_set_state (video->playbin, GST_STATE_NULL);
            video->update_position(0);
            video->set_playing(false);
            video->stop_timeout();

            GError *err = NULL;
            gchar *dbg_info = NULL;

            gst_message_parse_error (message, &err, &dbg_info);
            g_warning(_("iVideo error from %s : %s"),
                GST_OBJECT_NAME (message->src), err->message);
            g_debug("Debugging info: %s", (dbg_info) ? dbg_info : "none");
            g_error_free (err);
            g_free (dbg_info);
            break;
        }
        case GST_MESSAGE_WARNING: {
            g_warning("Warning message: %" GST_PTR_FORMAT, message);
            break;
        }
        case GST_MESSAGE_APPLICATION: {
            video->handle_application_message(message);
            break;
        }
        case GST_MESSAGE_STATE_CHANGED: {
            GstState old_state, new_state;
            gchar *src_name;

            gst_message_parse_state_changed (message, &old_state, &new_state, NULL);

            if (old_state == new_state)
                break;

            /* we only care about playbin (pipeline) state changes */
            if (GST_MESSAGE_SRC (message) != GST_OBJECT (video->playbin))
                break;

            if (new_state <= GST_STATE_PAUSED) {
                video->set_playing(false);
                video->stop_timeout();
            } else if (new_state > GST_STATE_PAUSED) {
                video->set_playing(true);
                video->start_timeout();
            }
            break;
        }
    }
//g_debug("BUS_MESSAGE_CB END");
}

void
iVideo::bus_message_sync_cb( GstBus *bus, GstMessage *msg, gpointer data )
{
//g_debug("BUS_MESSAGE_SYNC_CB START");
    iVideo *video = (iVideo*)data;

    g_assert (msg->type == GST_MESSAGE_ELEMENT);

    if (gst_is_video_overlay_prepare_window_handle_message (msg)) {
        gulong xid;

        g_debug("Handling sync prepare-xwindow-id message");

        g_mutex_lock (video->lock);
        video->update_interface();
        if (video->video_overlay == NULL) {
            GstObject *sender = GST_MESSAGE_SRC (msg);
            if (sender && GST_IS_VIDEO_OVERLAY (sender))
                video->video_overlay = GST_VIDEO_OVERLAY (gst_object_ref (sender));
        }
        g_mutex_unlock (video->lock);

        g_return_if_fail (video->video_overlay != NULL);

        xid = GDK_WINDOW_XID (gtk_widget_get_window(video->video_box));
        gst_video_overlay_set_window_handle (video->video_overlay, xid);
    }
//g_debug("BUS_MESSAGE_SYNC_CB END");
}

/* Must be called with lock held */
void iVideo::update_interface( void )
{
    GstVideoOverlay *old_video_overlay = video_overlay;
    GstElement *video_sink, *element, *play;

    if (g_thread_self() != gui_thread) {
        if (video_overlay)
            gst_object_unref (video_overlay);
            video_overlay = NULL;
    if (interface_update_id)
        g_source_remove (interface_update_id);
        interface_update_id = g_idle_add( &update_interfaces_delayed, this);
        return;
    }

    play = (GstElement*)gst_object_ref(playbin);

    g_mutex_unlock (lock);
    g_object_get (playbin, "video-sink", &video_sink, NULL);
    g_assert (video_sink != NULL);
    g_mutex_lock (lock);

    gst_object_unref(play);

    /* We try to get an element supporting XOverlay interface */
    if (GST_IS_BIN (video_sink)) {
        g_debug ("Retrieving video overlay from bin ...");
        element = gst_bin_get_by_interface (GST_BIN (video_sink), GST_TYPE_VIDEO_OVERLAY);
    } else {
        element = (GstElement*)gst_object_ref(video_sink);
    }

    if (GST_IS_VIDEO_OVERLAY (element)) {
        g_debug ("Found video overlay: %s", GST_OBJECT_NAME (element));
        video_overlay = GST_VIDEO_OVERLAY (element);
    } else {
        GST_DEBUG ("No video overlay found");
        if (element)
            gst_object_unref (element);
        video_overlay = NULL;
    }

    if (old_video_overlay)
        gst_object_unref (GST_OBJECT (old_video_overlay));

    gst_object_unref (video_sink);
}

gboolean iVideo::update_interfaces_delayed( gpointer data )
{
//g_debug("UPDATE_INTERFACE_DELAYED START");
    iVideo *video = (iVideo*)data;

    g_debug("Delayed updating interface implementations");

    g_mutex_lock (video->lock);
    video->update_interface();
    video->interface_update_id = 0;
    g_mutex_unlock (video->lock);

//g_debug("UPDATE_INTERFACE_DELAYED END");
    return false;
}

gboolean // TODO: check if we really don't need this and remove
iVideo::configure_cb( GtkWidget *widget,
    GdkEventConfigure *event, gpointer data )
{
//g_debug("CONFIGURE_CB START");
    iVideo *video = (iVideo*)data;
    if (video->video_overlay != NULL && GST_IS_VIDEO_OVERLAY (video->video_overlay)) {
        gst_video_overlay_expose (video->video_overlay);
    }
//g_debug("CONFIGURE_CB end");
    return false;
}

void iVideo::get_media_size( gint *width, gint *height )
{
    //guint movie_par_n, movie_par_d, disp_par_n, disp_par_d, num, den;
    guint disp_par_n, disp_par_d, num, den;

    // TODO: calc display pixel aspect ratio:
    disp_par_n = 1;
    disp_par_d = 1;

    // TODO: forced by user movie aspect ration (4:3 16:9 1:1 ...)

    if(video_width == 0 || video_height == 0) {
        g_debug("width and/or height 0, assuming 1/1 ratio");
        num = 1;
        den = 1;
    } else if ( !gst_video_calculate_display_ratio (&num, &den,
        video_width, video_height, video_par_n, video_par_d, disp_par_n, disp_par_d)) {
        g_debug("overflow calculating display aspect ratio!");
        num = 1;   /* FIXME: what values to use here? */
        den = 1;
    }

    // TODO: take care of screen size and max window size
    if(video_height % den == 0) {
        g_debug("keeping video height");
        scaled_width = (guint) gst_util_uint64_scale (video_height, num, den);
        scaled_height = video_height;
    } else if (video_width % num == 0) {
        g_debug("keeping video width");
        scaled_width = video_width;
        scaled_height = (guint) gst_util_uint64_scale (video_width, den, num);
    } else {
        g_debug("approximating while keeping video height");
        scaled_width = (guint) gst_util_uint64_scale (video_height, num, den);
        scaled_height = video_height;
    }
    g_debug("scaling to %dx%d", scaled_width, scaled_height);

    *width = scaled_width;
    *height = scaled_height;
}

void iVideo::handle_application_message( GstMessage *msg )
{
    const gchar *msg_name;
    const GstStructure *structure;
    //GdkWindow *window;
    //GtkAllocation allocation;

    structure = gst_message_get_structure (msg);
    msg_name = gst_structure_get_name (structure);
    g_return_if_fail (msg_name != NULL);

    g_debug("Handling application message: %" GST_PTR_FORMAT, structure);

    if( !g_strcmp0( msg_name, "stream-changed" ) ) {
        update_stream_info();
    } else if( !g_strcmp0( msg_name, "video-size" ) ) {
        g_debug("video-size application message");
    } else {
        g_message ("Unhandled application message %s", msg_name);
    }
}

void iVideo::update_stream_info( void )
{
    GstPad *videopad = NULL;
    gint n_audio, n_video;
    //bool media_has_video, media_has_audio; // TODO: make this class members ?

    g_object_get(G_OBJECT (playbin), "n-audio", &n_audio, "n-video", &n_video, NULL);

    //media_has_video = false;
    if (n_video > 0) {
        gint i;

        //media_has_video = true;
        for (i = 0; i < n_video && videopad == NULL; i++)
            g_signal_emit_by_name (playbin, "get-video-pad", i, &videopad);
    }

    //media_has_audio = false;

    if (videopad) {
        GstCaps *caps;

        if( (caps = gst_pad_get_current_caps( videopad ) ) ) {
            caps_set_cb( G_OBJECT( videopad ), NULL, this );
            gst_caps_unref (caps);
        }
        g_signal_connect (videopad, "notify::caps", G_CALLBACK (caps_set_cb), this);
        gst_object_unref (videopad);
    } // TODO: else ???
}

void iVideo::caps_set_cb( GObject *obj, GParamSpec *pspec, gpointer data )
{
//g_debug("CAPS_SET_CB START");
    iVideo *video = (iVideo*)data;

    GstPad *pad = GST_PAD (obj);
    GstStructure *s;
    GstCaps *caps;

    if( !(caps = gst_pad_get_current_caps(pad) ) )
        return;

    /* Get video decoder caps */
    s = gst_caps_get_structure (caps, 0);
    if (s) {
        const GValue *movie_par;

        /* We need at least width/height and framerate */
        if( !(gst_structure_get_fraction (s, "framerate",
                &video->video_fps_n, &video->video_fps_d) &&
            gst_structure_get_int (s, "width", &video->video_width) &&
            gst_structure_get_int (s, "height", &video->video_height))
            ) return;

        /* Get the movie PAR if available */
        movie_par = gst_structure_get_value (s, "pixel-aspect-ratio");
        if (movie_par) {
            video->video_par_n = gst_value_get_fraction_numerator (movie_par);
            video->video_par_d = gst_value_get_fraction_denominator (movie_par);
        } else {
            /* Square pixels */
            video->video_par_n = 1;
            video->video_par_d = 1;
        }

        // FIXME: do we really need to do next things? :
        // >>>
        GstMessage *msg;
        msg = gst_message_new_application (GST_OBJECT (video->playbin),
            gst_structure_new ("video-size", "width", G_TYPE_INT,
            video->video_width, "height", G_TYPE_INT,
            video->video_height, NULL));
        gst_element_post_message (video->playbin, msg);
        // <<<

        if( video->resize_id )
            g_source_remove(video->resize_id);
        video->resize_id = g_idle_add(&resize_cb, data);
    }

    gst_caps_unref (caps);
//g_debug("CAPS_SET_CB END");
}

void iVideo::stream_changed_cb (GstElement * obj, gpointer data)
{
//g_debug("STREAM_CHANGED_CB START");
    iVideo *video = (iVideo*)data;
    GstMessage *msg;

    /* we're being called from the streaming thread, so don't do anything here */
    msg = gst_message_new_application (GST_OBJECT (video->playbin),
        gst_structure_new ("stream-changed", NULL));
    gst_element_post_message (video->playbin, msg);
//g_debug("STREAM_CHANGED_CB END");
}

gboolean iVideo::resize_cb( gpointer data)
{
//g_debug("RESIZE_CB START");
    iVideo *video = (iVideo*)data;

    gint w, h, max_w, max_h;

    video->get_media_size(&w, &h);

    max_w = video->ui->get_max_width();
    max_h = video->ui->get_max_height(true);

    if(w > max_w) {
        h = double(h)/(double(w)/double(max_w));
        w = max_w;
    }
    if(h > max_h) {
        w = double(w)/(double(h)/double(max_h));
        h = max_h;
    }

    video->scaled_width = w;
    video->scaled_height = h;

    g_debug("iVideo resizing to %dx%d", w, h);
    if(video->is_fullscreen) {
        video->draw_fullscreen(NULL);
    } else {
        gtk_widget_set_size_request(video->video_box, w, h);
    }
    video->ui->resize(w,h);

    video->resize_id = 0;
//g_debug("RESIZE_CB END");
    return false;
}

void
iVideo::audio_tags_changed_cb( GstElement *playbin,
    gint stream, gpointer data )
{
//g_debug("AUDIO_TAGS_CHANGED_CB START");
    iVideo *video = (iVideo*)data;

    GstTagList *tags = NULL;
    gint current_stream_id = 0;

    g_object_get (G_OBJECT (video->playbin), "current-text", &current_stream_id, NULL);

    // Only get the updated tags if it's for our current stream id
    if (current_stream_id != stream)
        return;

    g_signal_emit_by_name (G_OBJECT (video->playbin), "get-audio-tags", stream, &tags);

    if (tags) {
        iVideoTagsPack *pack = g_slice_new0(iVideoTagsPack);
        pack->video = video;
        pack->tags = tags;
        g_idle_add(&update_tags_cb, (gpointer)pack);
    }
//g_debug("TEXT_TAGS_CHANGED_CB END");
}

void
iVideo::text_tags_changed_cb( GstElement *playbin,
    gint stream, gpointer data )
{
//g_debug("TEXT_TAGS_CHANGED_CB START");
    iVideo *video = (iVideo*)data;

    GstTagList *tags = NULL;
    gint current_stream_id = 0;

    g_object_get (G_OBJECT (video->playbin), "current-audio", &current_stream_id, NULL);

    // Only get the updated tags if it's for our current stream id
    if (current_stream_id != stream)
        return;

    g_signal_emit_by_name (G_OBJECT (video->playbin), "get-text-tags", stream, &tags);

    if (tags) {
        iVideoTagsPack *pack = g_slice_new0(iVideoTagsPack);
        pack->video = video;
        pack->tags = tags;
        g_idle_add(&update_tags_cb, (gpointer)pack);
    }
//g_debug("TEXT_TAGS_CHANGED_CB END");
}

void
iVideo::video_tags_changed_cb( GstElement *playbin,
    gint stream, gpointer data )
{
//g_debug("VIDEO_TAGS_CHANGED_CB START");
    iVideo *video = (iVideo*)data;

    GstTagList *tags = NULL;
    gint current_stream_id = 0;

    g_object_get (G_OBJECT (video->playbin), "current-video", &current_stream_id, NULL);

    // Only get the updated tags if it's for our current stream id
    if (current_stream_id != stream)
        return;

    g_signal_emit_by_name (G_OBJECT (video->playbin), "get-video-tags", stream, &tags);

    if (tags) {
        iVideoTagsPack *pack = g_slice_new0(iVideoTagsPack);
        pack->video = video;
        pack->tags = tags;
        g_idle_add(&update_tags_cb, (gpointer)pack);
    }
//g_debug("VIDEO_TAGS_CHANGED_CB END");
}

gboolean iVideo::update_tags_cb( gpointer data)
{
//g_debug("UPDATE_TAGS_CB START");
    iVideoTagsPack *pack = (iVideoTagsPack*)data;

    GstTagList *tags;

    //g_debug("Tags: %" GST_PTR_FORMAT, pack->tags);
    tags = gst_tag_list_merge (pack->video->tags, pack->tags, GST_TAG_MERGE_REPLACE);
    if( pack->video->tags )
        gst_tag_list_free ( pack->video->tags );
    pack->video->tags = tags;

    gst_tag_list_free (pack->tags);

    //check_for_cover_pixbuf (); // TODO: port this to iAudio

    g_slice_free(iVideoTagsPack, pack);
//g_debug("UPDATE_TAGS_CB END");
    return false;
}
