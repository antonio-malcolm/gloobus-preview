#include <string>
#include <glib.h>
#include <gio/gio.h>
#include "gloobus-preview-config.h"
#include "config.h"

GloobusConfig::GloobusConfig( void )
	:m_taskbar(true)
	,m_ontop(false)
	,m_focus(false)
	,m_winbar_layout(true)
	,cli_debug(false)
	,cli_action(GP_ACTION_PREVIEW)
	,cli_target(0)
	,theme_path(0)
	,theme_gtk(false)
	,theme_shadow_radius(7)
	,theme_shadow_alpha(0.7)
{
}

GloobusConfig::~GloobusConfig( void ){}

bool GloobusConfig::get_gsettings_win_layout()
{
	gchar* button_layout = NULL;
	GSettingsSchemaSource *schema_source = g_settings_schema_source_get_default ();
	GSettingsSchema *wm_schema = g_settings_schema_source_lookup (schema_source, "org.gnome.desktop.wm.preferences", FALSE);
	if (wm_schema) {
		GSettings *wm_settings = g_settings_new ("org.gnome.desktop.wm.preferences");
		button_layout = g_settings_get_string (wm_settings, "button-layout");
		g_object_unref (wm_settings);
		g_settings_schema_unref (wm_schema);
	}

	if (!button_layout) {
		return false;
	}
	if (int(string(button_layout).find("close")) - int(string(button_layout).find(":")) > 0)
		m_winbar_layout = false;
	else
		m_winbar_layout = true;
		
	return true;
}

//============== LOAD CONFIG ==============//
bool GloobusConfig::load_config()
{
	string home = g_get_home_dir();
	string configFile = home + "/.config/gloobus/gloobus-preview.cfg";

	GKeyFile * keyFile = g_key_file_new ( );
	//g_debug("Loading config file %s", configFile.c_str());

	if(g_key_file_load_from_file( keyFile,configFile.c_str(),G_KEY_FILE_NONE,NULL ) )
	{
		m_ontop 	= g_key_file_get_boolean( keyFile,"Main","ontop",	NULL );
		m_focus 	= g_key_file_get_boolean( keyFile,"Main","focus",	NULL );
		theme_gtk	= g_key_file_get_boolean( keyFile,"Theme","gtk",	NULL );
		
		if (!get_gsettings_win_layout())
			m_winbar_layout = (bool)g_key_file_get_integer 	( keyFile,"Main","winbar_layout",	NULL );
		g_key_file_free										( keyFile );

		if(m_ontop || m_focus)
			  m_taskbar = false;

		return true;
	}
	else
	{
		g_warning("Config file couldn't be readed");
		return false;
	}
}

// =============== PARSE COMMAND LINE ARGUMENTS =============== //
void GloobusConfig::parse_args(int argc, char **argv) {
	// TODO: use gtk cli parser here
	for (int i=1 ; i < argc ; i++) {
		if( !g_strcmp0(argv[i] , "--show-supported-filetypes") ) {
			cli_action = GP_ACTION_FILETYPES;
		} else if( !g_strcmp0(argv[i], "--config") || !g_strcmp0(argv[i], "-c") ) {
			cli_action = GP_ACTION_CONFIG;
		} else if( !g_strcmp0(argv[i], "--help") || !g_strcmp0(argv[i], "-h") ) {
			cli_action = GP_ACTION_HELP;
		} else if( !g_strcmp0(argv[i], "--version") || !g_strcmp0(argv[i], "-v") ) {
			cli_action = GP_ACTION_VERSION;
		} else if( !g_strcmp0(argv[i], "--debug") || !g_strcmp0(argv[i], "-d") ) {
			cli_debug = true;
		} else if( !g_strcmp0(argv[i], "--use-gtk") || !g_strcmp0(argv[i], "-d") ) {
			theme_gtk = true;
		} else if( !g_strcmp0(argv[i], "-s") ) {
			//TODO: read sorting here
		} else if( !g_strcmp0(argv[i], "-t") ) {
			i++;
			theme_path = g_strdup(argv[i]);
		} else {
			cli_target = argv[i];
		}
	}
}

void GloobusConfig::parse_args(int *argc, char ***argv)
{
	//parse_args(*argc, *argv);

	gboolean list_filetypes = false;
	gboolean config = false;
	gchar *sort = NULL;
	gchar *theme = NULL;
	gboolean version = false;
	GOptionEntry entries[] = 
	{
		{"list-filetypes"	, 'l' , 0 , G_OPTION_ARG_NONE		, &list_filetypes	, "List supported filetypes"		, NULL},
		{"config"			, 'c' , 0 , G_OPTION_ARG_NONE		, &config			, "Show configuration window"	, NULL},
		{"debug"			, 'd' , 0 , G_OPTION_ARG_NONE		, &cli_debug		, "Print debug messages"			, NULL},
		{"gtk"				, 'g' , 0 , G_OPTION_ARG_NONE		, &theme_gtk		, "Use GTK theme"					, NULL},
//		{"sort"			, 's' , 0 , G_OPTION_ARG_STRING		, &sort				, "Set file sorting type: name|size|type", "SORT"},
		{"theme"			, 't' , 0 , G_OPTION_ARG_FILENAME	, &theme			, "Set theme path"				, "PATH"},
		{"version"			, 'v' , 0 , G_OPTION_ARG_NONE		, &version	, "Show version information"		, NULL},
		{ NULL }
	};
	
	GError *error = NULL;
	GOptionContext *context;

	context = g_option_context_new ("- don't know what to put here :(");
	g_option_context_add_main_entries(context, entries, "gloobus-preview");
	if (!g_option_context_parse (context, argc, argv, &error)) {
		g_warning("option parsing failed: %s", error->message);
		g_error_free(error);
		cli_action = GP_ACTION_UNKNOWN;
 	}
	
	if(list_filetypes)
		cli_action = GP_ACTION_FILETYPES;
	else if(config)
		cli_action = GP_ACTION_CONFIG;
	else if(version)
		cli_action = GP_ACTION_VERSION;
	if(sort) {
		//TODO: set sorting here
		g_free(sort);
	}
	if(theme) {
		theme_path = theme;
	}
	if(*argc > 1) {
		//TODO: check if it is a file?
		cli_target = (*argv)[1];
		
	}
        if (!cli_target)
                cli_target = g_get_current_dir();
	g_option_context_free(context);
}
//=============== GET METHODS =============== //
bool GloobusConfig::get_taskbar( void )
{
	return m_taskbar;
}

bool GloobusConfig::get_ontop( void )
{
	return m_ontop;
}

bool GloobusConfig::get_focus( void )
{
	return m_focus;
}

bool GloobusConfig::get_winbar_layout( void )
{
	return m_winbar_layout;
}

bool GloobusConfig::get_cli_debug( void )
{
	return cli_debug;
}

char *	GloobusConfig::get_cli_target( void )
{
	return cli_target;
}

gp_action GloobusConfig::get_cli_action( void )
{
	return cli_action;
}

char* GloobusConfig::get_theme( void )
{
	return theme_path;
}

bool GloobusConfig::get_theme_gtk( void )
{
	return theme_gtk;
}

int GloobusConfig::get_shadow_radius( void)
{
	return theme_shadow_radius;
}

double GloobusConfig::get_shadow_alpha( void )
{
	return theme_shadow_alpha;
}
