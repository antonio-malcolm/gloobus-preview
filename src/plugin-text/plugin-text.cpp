#include "plugin-text.h"

GtkSourceBuffer * iPlain::get_textbuf () {
	GtkSourceLanguageManager 	*manager;
	GtkSourceLanguage 			*language;
	GtkSourceBuffer				*source;

	manager  = gtk_source_language_manager_get_default ();
	language = gtk_source_language_manager_guess_language(manager,m_filename.c_str(),NULL);
	source   = gtk_source_buffer_new(NULL);
	
	if(language) {
		gtk_source_buffer_set_language (source, language);
		g_debug("Language name: %s", gtk_source_language_get_name(language));
	} else {
		g_warning("No Source Language");
	}
	
	gsize count;
	gsize size = (gsize)get_file_size(m_filename)+1;
	char buffer[size];
	GFile * file = g_file_new_for_path(m_filename.c_str());
	GFileInputStream * stream = g_file_read(file, NULL, NULL);
	bool ok = g_input_stream_read_all(G_INPUT_STREAM(stream), buffer,
		size, &count, NULL, NULL);
	if( ok ) {
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(source), buffer, count);
	} else {
		return 0;
	}

	char* theme;
	GSettingsSchemaSource *schema_source = g_settings_schema_source_get_default ();
	GSettingsSchema *gedit_schema = g_settings_schema_source_lookup (schema_source, "org.gnome.gedit.preferences.editor", FALSE);
	if (gedit_schema) {
		GSettings *gedit_settings = g_settings_new ("org.gnome.gedit.preferences.editor");
		theme = g_settings_get_string (gedit_settings, "scheme");
		g_object_unref (gedit_settings);
		g_settings_schema_unref (gedit_schema);
	}

	if(theme) {
		GtkSourceStyleSchemeManager * sc_manager;
		sc_manager = gtk_source_style_scheme_manager_get_default();
		GtkSourceStyleScheme * scheme;
		scheme = gtk_source_style_scheme_manager_get_scheme(sc_manager,theme);
		if(scheme)
			gtk_source_buffer_set_style_scheme(source, scheme);
		g_free(theme);
	}
	
	return source;
}


