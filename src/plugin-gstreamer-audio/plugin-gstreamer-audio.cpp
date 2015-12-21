#include "plugin-gstreamer-audio.h"

#define IAUDIO_COVER_SIZE 128
#define IAUDIO_WIDTH 550
#define IAUDIO_HEIGHT 200
#define IAUDIO_PADDING 10

iAudio::iAudio( void )
	:state(GST_STATE_NULL)
	,cover_found(false)
{
	g_debug("Creating iAudio");

    menuItemSearchLyrics = ui->add_menu_item ( _("Search Lyrics for this Song"), G_CALLBACK(search_lyrics_cb), this );

}

iAudio::~iAudio( void )
{
	g_debug("Destroying iAudio");
	gst_object_unref (GST_OBJECT (pipeline));
    gtk_widget_destroy(menuItemSearchLyrics);
}

bool iAudio::load( void )
{
	//TODO: check for errors ?
	g_debug("Loading iAudio");
	gst_init (NULL, NULL);
	
	pipeline = gst_element_factory_make ("playbin", "player");
	GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch(bus, &bus_cb, this);
	gst_object_unref (bus);

	GdkPixbuf* temp = get_pixbuf();
	pix_cover = ui->pixbuf_scale(temp,IAUDIO_COVER_SIZE,IAUDIO_COVER_SIZE, true);
	pix_reflection = ui->pixbuf_reflect(pix_cover);
	g_object_unref(temp);

	//load_info(); // Use GStreamer instead of taglib
	
	return true;
}

/*void iAudio::load_info( void )
{
	TagLib::FileRef file(m_filename.c_str());
	
	TagLib::String title  = file.tag()->title(); 
	TagLib::String artist = file.tag()->artist();
	TagLib::String album  = file.tag()->album();
	
	this->title  = title.toCString(true);
	this->artist = artist.toCString(true);
	this->album  = album.toCString(true);
}*/

void iAudio::end( void )
{
	g_debug("Ending iAudio");
	stop_timeout();
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_NULL); //stop playback

	if (pix_cover)
		g_object_unref(pix_cover);

	if (pix_reflection)
		g_object_unref(pix_reflection);
}

int iAudio::get_width( void )
{
	return IAUDIO_WIDTH;
}

int iAudio::get_height( void )
{
	return IAUDIO_HEIGHT;
}

void iAudio::draw( GtkContainer *container )
{
	cover		= gtk_image_new_from_pixbuf (pix_cover);
	reflection	= gtk_image_new_from_pixbuf (pix_reflection);

	gtk_alignment_set(GTK_ALIGNMENT(container), 0.5,0.5,0,0);
	GtkWidget *align = gtk_alignment_new(0.5,0.5,1,1);
	gtk_widget_set_size_request(align, IAUDIO_WIDTH, IAUDIO_HEIGHT);
	gtk_alignment_set_padding(GTK_ALIGNMENT(align),
		IAUDIO_PADDING, IAUDIO_PADDING, IAUDIO_PADDING, IAUDIO_PADDING);

	GtkWidget *mpad = gtk_alignment_new(0.5,0.5,1,1);
	gtk_widget_set_size_request(mpad, 25, 20);

	string info = title + "\n\n" + artist + "\n\n" + album;
	info_label = ui->get_theme()->get_label(info.c_str());
	gtk_misc_set_alignment(GTK_MISC(info_label), 0.5 , 0.5 );
	gtk_label_set_justify(GTK_LABEL(info_label), GTK_JUSTIFY_CENTER);
	gtk_label_set_line_wrap(GTK_LABEL(info_label), true);

	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	GtkWidget *mbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	gtk_box_pack_start(GTK_BOX(mbox), cover, false, false, 0);
	gtk_box_pack_start(GTK_BOX(mbox), reflection, false, false, 0);

	gtk_box_pack_start(GTK_BOX(hbox), mpad, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), mbox, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), info_label, true, true, 0);

	gtk_container_add(GTK_CONTAINER(align), hbox);
	gtk_container_add(container, align);

	gtk_widget_show_all( GTK_WIDGET(container) );

	draw_toolbar();

	g_object_set (G_OBJECT (pipeline), "uri", g_file_get_uri ( m_gfile), NULL);
	set_playing( play_pause() );
	start_timeout();
}

bool iAudio::play_pause( void )
{
	// TODO: make state control with gst_element_get_state () for errors
	if( state == GST_STATE_PLAYING ) {
		gst_element_set_state (pipeline, GST_STATE_PAUSED);
		state = GST_STATE_PAUSED;
		return false;
	} else {
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
		state = GST_STATE_PLAYING;
		return true;
	}
}

void iAudio::stop( void )
{
	gst_element_set_state (pipeline, GST_STATE_READY);
	state = GST_STATE_READY;
}

void iAudio::set_position( gint sec )
{
	gst_element_seek_simple(pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, gint64(sec)*GST_SECOND);
}

gint iAudio::get_position( void )
{
	GstFormat format = GST_FORMAT_TIME;
    gint64 pos;
    gboolean result;
    result = gst_element_query_position (pipeline, format, &pos);
    if (!result || format != GST_FORMAT_TIME)
        return 0;
	return pos/GST_SECOND;
}

gint iAudio::get_duration( void )
{
	GstFormat format = GST_FORMAT_TIME;
	gint64 len;
	gboolean result;
	result = gst_element_query_duration (pipeline, format, &len);
	if( !result || format != GST_FORMAT_TIME ) 
		return 0;
	return len/GST_SECOND;
}

GdkPixbuf * iAudio::get_pixbuf( void )
{
	GdkPixbuf *temp = NULL;

/*	TagLib::MPEG::File file(m_filename.c_str());
	TagLib::ID3v2::Tag *tagv2 = file.ID3v2Tag();
	if(tagv2) {
		TagLib::ID3v2::FrameListMap tags  = tagv2->frameListMap();
		TagLib::ID3v2::FrameList pictures = tags["APIC"];
		
		if( !pictures.isEmpty() ) {
			
		    g_debug("Loading attached Cover");
		    TagLib::ID3v2::AttachedPictureFrame *picture = 
				static_cast<TagLib::ID3v2::AttachedPictureFrame*>(pictures.front());
		    
			TagLib::ByteVector pictureData  = picture->picture();
		   
		    int size   = pictureData.size();
		    char *data = pictureData.data();

			temp = convert_to_pixbuf(data,size);
			if(temp)
				return temp;
		}
	}*/
	
	// If there is no cover attached, load cover.jpg, then if there is not cover.jpg, try to load
	// cover.png, and there is not cover.png, then, load the default theme icon for audio/mpeg using the
	// function of the parent class "interface"

	string path;
	string dir = m_filename.substr(0,m_filename.rfind('/',m_filename.length())+1);	// Cut just filename

	const char *cover_filename[10] = {"folder", "cover", "album", "Folder", "Cover", "Album", "COVER", "ALBUM", "FOLDER", 0};
	const char *cover_ext[9] = {"png", "jpg", "jpeg", "gif", "PNG", "JPG", "JPEG", "GIF", 0};
	
	for (int i=0; cover_filename[i]; i++) {
		for (int j=0; cover_ext[j]; j++) {
			path = dir + string(cover_filename[i]) + "." + string(cover_ext[j]);
			if( g_file_test(path.c_str(), G_FILE_TEST_EXISTS) ) {
				g_debug("Tring to load %s", path.c_str());
				temp = gdk_pixbuf_new_from_file (path.c_str(), NULL);
				if (temp) {
					cover_found = true;
					return temp;
				}
			}
		}
	}
	
	g_warning("No cover found, using default icon");
	return ui->get_theme()->get_icon("nocover", IAUDIO_COVER_SIZE);
}

GdkPixbuf * iAudio::convert_to_pixbuf( char *data, int size )
{
	GdkPixbufLoader * loader = gdk_pixbuf_loader_new();
	gdk_pixbuf_loader_write(loader,(guchar*)data,size,NULL);
	gdk_pixbuf_loader_close(loader,NULL);
	GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
	g_object_ref(pixbuf);
	g_object_unref(loader);
	return pixbuf;
}

gboolean iAudio::bus_cb( GstBus *bus, GstMessage *message, gpointer data)
{
	iAudio *audio = (iAudio*)data;
	switch (GST_MESSAGE_TYPE (message)) {
		case GST_MESSAGE_EOS: {
			// TODO: when better configuration is ready, make here possible to go to the next file
			audio->next();
			audio->state = GST_STATE_NULL;
			gst_element_set_state (audio->pipeline, GST_STATE_NULL);
			audio->update_position(0);
			audio->set_playing(false);
			break;
		}
		case GST_MESSAGE_ERROR: {
			audio->stop_timeout();
			audio->state = GST_STATE_NULL;
			gst_element_set_state (audio->pipeline, GST_STATE_NULL);
			audio->update_position(0);
			audio->set_playing(false);
			
			GError *err = NULL;
			gchar *dbg_info = NULL;
    
			gst_message_parse_error (message, &err, &dbg_info);
			g_warning(_("iAudio error from %s : %s"),
				GST_OBJECT_NAME (message->src), err->message);
			g_debug("Debugging info: %s", (dbg_info) ? dbg_info : "none");
			g_error_free (err);
			g_free (dbg_info);
			break;
		}
		case GST_MESSAGE_TAG: {
			GstTagList *tags = NULL;
			gst_message_parse_tag (message, &tags);
			audio->handle_tags (tags);
			gst_tag_list_free (tags);
			break;
		}
	}
	return true;
}

void iAudio::handle_tags( GstTagList *tags )
{
	gchar *title, *artist, *album, *info;
	
	if( !gst_tag_list_get_string_index (tags, GST_TAG_TITLE, 0, &title) )
		title = g_strdup("");
	if( !gst_tag_list_get_string_index (tags, GST_TAG_ARTIST, 0, &artist) )
		artist = g_strdup("");
	if( !gst_tag_list_get_string_index (tags, GST_TAG_ALBUM, 0, &album) )
		album = g_strdup("");

	this->title = title;
	this->artist = artist;
	this->album = album;

	info = g_strdup_printf("%s\n\n%s\n\n%s", title, artist, album);
	gtk_label_set_text(GTK_LABEL(info_label), info);

	g_free(title);
	g_free(artist);
	g_free(album);
	g_free(info);

	GstSample *sample;
	GstBuffer *buff;
	GstMapInfo cover_info;

	if(gst_tag_list_get_sample_index (tags, GST_TAG_IMAGE, 0, &sample)) {
		buff = gst_sample_get_buffer (sample);
		if(gst_buffer_map (buff, &cover_info, GST_MAP_READ)) {
			g_debug("Cover?");
			GdkPixbuf *pixbuf = NULL;
			guchar * data = NULL;
			gsize size;
			GError *error = NULL;

			data = cover_info.data;
			size = cover_info.size;
			gst_buffer_unmap (buff, &cover_info);

			GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
			if(gdk_pixbuf_loader_write(loader, data, size, &error)) {
				if(gdk_pixbuf_loader_close(loader, NULL)) {
					pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
					g_object_ref(pixbuf);
				} else {
					g_warning("Error: %s", error->message);
					g_error_free(error);
				}
			} else {
				g_warning("Error: %s", error->message);
				g_error_free(error);
			}
			g_object_unref(loader);

			if(pixbuf) {
				g_debug("Cover");
				g_object_unref(pix_cover);
				g_object_unref(pix_reflection);
				pix_cover = ui->pixbuf_scale(pixbuf,IAUDIO_COVER_SIZE,IAUDIO_COVER_SIZE, true);
				pix_reflection = ui->pixbuf_reflect(pix_cover);

				gtk_image_set_from_pixbuf(GTK_IMAGE(cover), pix_cover);
				gtk_image_set_from_pixbuf(GTK_IMAGE(reflection), pix_reflection);
				g_object_unref(pixbuf);
			}
		}
		gst_sample_unref(sample);
	}
}

// ============================= GET FUNCTIONS ================================ //
string iAudio::get_artist() { return artist;}
string iAudio::get_title()  { return title;}
string iAudio::get_album()  { return album;}

// ============================ SEARCH LYRICS CB =============================== //
void iAudio::search_lyrics_cb(GtkWidget *widget, gpointer data)
{
    //http://mp3lyrics.com/Search/Advanced/?Track=don%27t+go+away&Artist=oasis
    iAudio * audio = (iAudio*)data;
    string uri = "http://mp3lyrics.com/Search/Advanced/?Track="+audio->get_title()+"&Artist=" + audio->get_artist();
    g_app_info_launch_default_for_uri(uri.c_str(), NULL, NULL);
    g_debug("Searching Lyrics: %s",uri.c_str());
}
