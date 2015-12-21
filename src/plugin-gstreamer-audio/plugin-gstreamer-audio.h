#ifndef _GST_AUDIO_
#define _GST_AUDIO_

#ifndef PLUGIN_LOADER

/*#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/id3v2tag.h>*/
#include <config.h>
#include <string>

#include <gst/gst.h>

#include "../gloobus-preview-interface-media.h"

#define DURATION_IS_VALID(x) (x != 0 && x != (guint64) -1)

class iAudio : public iMedia
{
    private:
            GstElement  *   pipeline;
            string          title;
            string          artist;
            string          album;
            guint64         audio_duration;
            guint64         audio_current_time;
            GstState        state;
            GdkPixbuf   *   pix_cover;
            GdkPixbuf   *   pix_reflection;
            GtkWidget   *   cover;
            GtkWidget   *   reflection;
            GtkWidget   *   info_label; // for handle_tags?
            bool            cover_found; // for handle_tags?

            GtkWidget   *   menuItemSearchLyrics;

    public:
                        iAudio              ( void );
                        ~iAudio             ( void );
    virtual int         get_width           ( void );
    virtual int         get_height          ( void );
    virtual bool        load                ( void );
    virtual void        end                 ( void );
    virtual void        draw                ( GtkContainer * container );
    virtual bool        play_pause          ( void );
    virtual void        stop                ( void );
    virtual void        set_position        ( gint sec );
    virtual gint        get_position        ( void );
    virtual gint        get_duration        ( void );
    static  gboolean    bus_cb              ( GstBus *bus, GstMessage *message, gpointer data);
            void        handle_tags         ( GstTagList *tags );

            string      get_artist          ( void );
            string      get_title           ( void );
            string      get_album           ( void );
    static  void        search_lyrics_cb    ( GtkWidget*, gpointer );

    private:
        GdkPixbuf*  get_pixbuf          ( void );
            void    load_info           ( void );
        GdkPixbuf*  convert_to_pixbuf   ( char* ,int );
};

extern "C" interface * create() {
    return new iAudio;
}

extern "C" void destroy(interface * p) {
    delete (iAudio*)p;
}

extern "C" int getVersion() {
    return MANAGER_VERSION;
}

#endif

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("audio");

PluginManager::register_filetype("audio/3gpp", 1);
PluginManager::register_filetype("audio/ac3", 1);
PluginManager::register_filetype("audio/AMR", 1);
PluginManager::register_filetype("audio/AMR-WB", 1);
PluginManager::register_filetype("audio/basic", 1);
PluginManager::register_filetype("audio/flac", 1);
PluginManager::register_filetype("audio/midi", 1);
PluginManager::register_filetype("audio/mp2", 1);
PluginManager::register_filetype("audio/mp4", 1);
PluginManager::register_filetype("audio/mpeg", 1);
PluginManager::register_filetype("audio/ogg", 1);
PluginManager::register_filetype("audio/prs.sid", 1);
PluginManager::register_filetype("audio/vnd.rn-realaudio", 1);
PluginManager::register_filetype("audio/x-aiff", 1);
PluginManager::register_filetype("audio/x-ape", 1);
PluginManager::register_filetype("audio/x-flac", 1);
PluginManager::register_filetype("audio/x-gsm", 1);
PluginManager::register_filetype("audio/x-it", 1);
PluginManager::register_filetype("audio/x-m4a", 1);
PluginManager::register_filetype("audio/x-matroska", 1);
PluginManager::register_filetype("audio/x-mod", 1);
PluginManager::register_filetype("audio/x-mp3", 1);
PluginManager::register_filetype("audio/x-mpeg", 1);
PluginManager::register_filetype("audio/x-ms-asf", 1);
PluginManager::register_filetype("audio/x-ms-asx", 1);
PluginManager::register_filetype("audio/x-ms-wax", 1);
PluginManager::register_filetype("audio/x-ms-wma", 1);
PluginManager::register_filetype("audio/x-musepack", 1);
PluginManager::register_filetype("audio/x-opus+ogg", 1);
PluginManager::register_filetype("audio/x-pn-aiff", 1);
PluginManager::register_filetype("audio/x-pn-au", 1);
PluginManager::register_filetype("audio/x-pn-wav", 1);
PluginManager::register_filetype("audio/x-pn-windows-acm", 1);
PluginManager::register_filetype("audio/x-realaudio", 1);
PluginManager::register_filetype("audio/x-real-audio", 1);
PluginManager::register_filetype("audio/x-s3m", 1);
PluginManager::register_filetype("audio/x-sbc", 1);
PluginManager::register_filetype("audio/x-speex", 1);
PluginManager::register_filetype("audio/x-stm", 1);
PluginManager::register_filetype("audio/x-tta", 1);
PluginManager::register_filetype("audio/x-wav", 1);
PluginManager::register_filetype("audio/x-wavpack", 1);
PluginManager::register_filetype("audio/x-vorbis", 1);
PluginManager::register_filetype("audio/x-vorbis+ogg", 1);
PluginManager::register_filetype("audio/x-xm", 1);
PluginManager::register_filetype("application/x-flac", 1);

#endif

#endif

