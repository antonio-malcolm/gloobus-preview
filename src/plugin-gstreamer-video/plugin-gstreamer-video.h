#ifndef _GST_VIDEO_
#define _GST_VIDEO_

#ifndef PLUGIN_LOADER

#include <gst/gst.h>
#include <gst/video/video.h>
#include <string.h>

#include "../gloobus-preview-interface-media.h"

#define DURATION_IS_VALID(x) (x != 0 && x != (guint64) -1)

class iVideo : public iMedia
{
    private:
        GstBus      *   bus;
        GstElement  *   playbin;
        GstElement  *   audio_capsfilter;
        GstVideoOverlay * video_overlay;

        gulong          bus_msg_id;
        gulong          bus_msg_id_sync;
        gulong          interface_update_id;
        guint           resize_id;
        guint           configure_id;
        GtkWidget   *   video_box;
        gint            video_width;  // real video width
        gint            video_height; // real video height
        gint            video_par_n;  // video pixel-aspect-ratio numerator
        gint            video_par_d;  // video pixel-aspect-ratio denominator
        gint            scaled_width; // scaled video width
        gint            scaled_height;// scaled video height
        gint            video_fps_n;  // video framerate numerator
        gint            video_fps_d;  // video framerate denominator
        GstState        video_state;

        GstTagList  *   tags;

        GMutex      *   lock;

        GThread     *   gui_thread;

        bool            is_fullscreen;

    public:
                        iVideo                      ( void );
                        ~iVideo                     ( void );
    virtual int         get_width                   ( void );
    virtual int         get_height                  ( void );
    virtual bool        load                        ( void );
    virtual void        end                         ( void );
    virtual void        draw                        ( GtkContainer * container );
    virtual void        draw_fullscreen             ( GtkContainer * container );
    virtual void        draw_unfullscreen           ( GtkContainer * container );
    virtual bool        play_pause                  ( void );
    virtual void        stop                        ( void );
    virtual void        set_position                ( gint sec );
    virtual gint        get_position                ( void );
    virtual gint        get_duration                ( void );
    static  gboolean    start                       ( gpointer data );


    private:
            void        get_media_size              ( gint *width, gint *height );

            // video stream related callbacks and methods
    static  void        bus_message_cb              ( GstBus *bus, GstMessage *msg, gpointer data );
    static void     bus_message_sync_cb         ( GstBus *bus, GstMessage *msg, gpointer data );
            void        update_interface            ( void );
    static  gboolean    update_interfaces_delayed   ( gpointer data );
            void        handle_application_message  ( GstMessage *msg );
            void        update_stream_info          ( void );
    static  void        caps_set_cb                 ( GObject *obj, GParamSpec *pspec, gpointer data );
    static  void        stream_changed_cb           ( GstElement * obj, gpointer data );

            // video_box widget related callbacks
    static  gboolean    configure_cb                ( GtkWidget *widget, GdkEventConfigure *event, gpointer data );
    static  gboolean    resize_cb                   ( gpointer data );

            // tags related callbacks
    static  void        audio_tags_changed_cb       ( GstElement *playbin, gint stream, gpointer data );
    static  void        text_tags_changed_cb        ( GstElement *playbin, gint stream, gpointer data );
    static  void        video_tags_changed_cb       ( GstElement *playbin, gint stream, gpointer data );
    static  gboolean    update_tags_cb              ( gpointer data );

};

extern "C" interface * create() {
    return new iVideo;
}

extern "C" void destroy(interface * p) {
    delete (iVideo*)p;
}

extern "C" int getVersion() {
    return MANAGER_VERSION;
}

#endif

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("video");

PluginManager::register_filetype("application/mxf", 1);
PluginManager::register_filetype("application/ogg", 1);
PluginManager::register_filetype("application/ram", 1);
PluginManager::register_filetype("application/sdp", 1);
PluginManager::register_filetype("application/vnd.apple.mpegurl", 1);
PluginManager::register_filetype("application/vnd.ms-wpl", 1);
PluginManager::register_filetype("application/vnd.rn-realmedia", 1);
PluginManager::register_filetype("application/x-extension-m4a", 1);
PluginManager::register_filetype("application/x-extension-mp4", 1);
PluginManager::register_filetype("application/x-flash-video", 1);
PluginManager::register_filetype("application/x-matroska", 1);
PluginManager::register_filetype("application/x-netshow-channel", 1);
PluginManager::register_filetype("application/x-ogg", 1);
PluginManager::register_filetype("application/x-quicktimeplayer", 1);
PluginManager::register_filetype("application/x-shorten", 1);
PluginManager::register_filetype("audio/x-pn-realaudio", 1);
PluginManager::register_filetype("image/vnd.rn-realpix", 1);
PluginManager::register_filetype("image/x-pict", 1);
PluginManager::register_filetype("misc/ultravox", 1);
PluginManager::register_filetype("text/x-google-video-pointer", 1);
PluginManager::register_filetype("video/3gp", 1);
PluginManager::register_filetype("video/3gpp", 1);
PluginManager::register_filetype("video/dv", 1);
PluginManager::register_filetype("video/divx", 1);
PluginManager::register_filetype("video/fli", 1);
PluginManager::register_filetype("video/flv", 1);
PluginManager::register_filetype("video/mp2t", 1);
PluginManager::register_filetype("video/mp4", 1);
PluginManager::register_filetype("video/mp4v-es", 1);
PluginManager::register_filetype("video/mpeg", 1);
PluginManager::register_filetype("video/msvideo", 1);
PluginManager::register_filetype("video/ogg", 1);
PluginManager::register_filetype("video/quicktime", 1);
PluginManager::register_filetype("video/vivo", 1);
PluginManager::register_filetype("video/vnd.divx", 1);
PluginManager::register_filetype("video/vnd.mpegurl", 1);
PluginManager::register_filetype("video/vnd.rn-realvideo", 1);
PluginManager::register_filetype("video/vnd.vivo", 1);
PluginManager::register_filetype("video/webm", 1);
PluginManager::register_filetype("video/x-anim", 1);
PluginManager::register_filetype("video/x-avi", 1);
PluginManager::register_filetype("video/x-flc", 1);
PluginManager::register_filetype("video/x-fli", 1);
PluginManager::register_filetype("video/x-flic", 1);
PluginManager::register_filetype("video/x-flv", 1);
PluginManager::register_filetype("video/x-m4v", 1);
PluginManager::register_filetype("video/x-matroska", 1);
PluginManager::register_filetype("video/x-mpeg", 1);
PluginManager::register_filetype("video/x-mpeg2", 1);
PluginManager::register_filetype("video/x-ms-asf", 1);
PluginManager::register_filetype("video/x-ms-asx", 1);
PluginManager::register_filetype("video/x-msvideo", 1);
PluginManager::register_filetype("video/x-ms-wm", 1);
PluginManager::register_filetype("video/x-ms-wmv", 1);
PluginManager::register_filetype("video/x-ms-wmx", 1);
PluginManager::register_filetype("video/x-ms-wvx", 1);
PluginManager::register_filetype("video/x-nsv", 1);
PluginManager::register_filetype("video/x-ogm+ogg", 1);
PluginManager::register_filetype("video/x-theora+ogg", 1);
PluginManager::register_filetype("video/x-totem-stream", 1);

#endif

#endif

