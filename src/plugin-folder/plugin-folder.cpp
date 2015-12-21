#include "plugin-folder.h"

#define IFOLDER_ICON_SIZE 200

iFolder::iFolder()
	:n_files(-1)
	,n_current(0)
{
	g_debug("Creating iFolder");
}

iFolder::~iFolder()
{
	g_debug("Destroying iFolder");
    for(unsigned int i = 0; i< m_icon_info.size(); i++)
	{
		g_debug("Deleting icon for the file %i of %i", i ,
			(int)m_icon_info.size());
		g_object_unref(m_icon_info[i]);
		g_object_unref(m_icons[i]);

	}
}
// ========================== GET PIXBUF ======================= //
GdkPixbuf * iFolder::get_pixbuf ()
{
	GdkPixbuf *temp;// = gdk_pixbuf_new_from_file (path.c_str(),NULL);
    string path;
    bool cover_found = false;

	const char *cover_filename[10]  = {"folder", "cover", "album", "Folder", "Cover", "Album", "COVER", "ALBUM", "FOLDER", 0};
	const char *cover_ext[9]        = {"png", "jpg", "jpeg", "gif", "PNG", "JPG", "JPEG", "GIF", 0};
	
	for (int i=0; cover_filename[i]; i++) {
		for (int j=0; cover_ext[j]; j++) {
			path = m_filename + "/" + string(cover_filename[i]) + "." + string(cover_ext[j]);
    		if( g_file_test(path.c_str(), G_FILE_TEST_EXISTS) ) 
            {
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
    temp = ui->get_file_icon(m_file_info, IFOLDER_ICON_SIZE);
	return temp;
};

// ===================== GET FOLDER FILES ======================= //
int iFolder::get_folder_files (string dir, vector<string> &files)
{
	GFile * folder	= g_file_new_for_path( dir.c_str() );
	GFileEnumerator * directory_files;
	directory_files = g_file_enumerate_children(folder,
//		G_FILE_ATTRIBUTE_STANDARD_NAME "," G_FILE_ATTRIBUTE_STANDARD_ICON,
		"standard::is-hidden,standard::name,standard::display-name,standard::content-type,access::can-execute,standard::size,standard::icon",
		G_FILE_QUERY_INFO_NONE, NULL, NULL);
    //GtkIconTheme *icon_theme;
	//GtkIconInfo *icon_info;

    //icon_theme = gtk_icon_theme_get_default();

	g_debug( CLI_COLOR_GREEN "PLUGIN FOLDER FILES" CLI_COLOR_RESET );
	
	GFileInfo * info;
	info = g_file_enumerator_next_file (directory_files, NULL, NULL);
	while(info) {
		g_debug("   Files:%s - \t%"G_GUINT64_FORMAT,
			g_file_info_get_name(info), g_file_info_get_size(info));

		files.push_back		(g_file_info_get_name(info));

    
        string mime = g_file_info_get_attribute_string(info, "standard::content-type");
        if(mime == "inode/directory" ) 
        {
            m_size.push_back	(_("Directory"));
        }
        else
        {
		    m_size.push_back	(g_format_size(g_file_info_get_size(info)));
        }
		m_icons.push_back	(g_file_info_get_icon (info));
		
		info = g_file_enumerator_next_file   (directory_files, NULL, NULL);
	}
    
    //sort( files.begin(), files.end() );
	g_object_unref(directory_files);

	return files.size();
}

//========================= GET LIST STORE ================== //
GtkListStore*  	iFolder::get_list_store		( void )
{
	return gtk_list_store_new  (3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING);
}
//============================ GET N ITEMS ================== //
int iFolder::get_n_items ( ) 
{
	if(n_files < 0 )
		n_files = get_folder_files(m_filename, m_files);
	return n_files;
}
//============================ GET ITEM ================== //
string iFolder::get_item (int i, int j)
{
	if(j==0) return "sfasdfa";
	if(j==1) return m_files[i];
	if(j==2) return m_size[i];
}
//============================ IS COLUMN PIXBUF ================== //
bool iFolder::is_column_pixbuf(int i) 
{
	 if(i==0) 
	 	return true;
	 else
	 	return false;
}
//============================ GET ITEM PIXBUF ================== //
GdkPixbuf* iFolder::get_item_pixbuf		( int i, int j)
{

    GtkIconInfo *icon_info;
    GtkIconTheme *icon_theme;

    icon_theme = gtk_icon_theme_get_default();
    icon_info = gtk_icon_theme_lookup_by_gicon(
                    icon_theme,
                    m_icons[i],
                    16,    			  // icon size 
                   GtkIconLookupFlags(GTK_ICON_LOOKUP_USE_BUILTIN | 
                   					  GTK_ICON_LOOKUP_GENERIC_FALLBACK | 
                   					  GTK_ICON_LOOKUP_FORCE_SIZE));
    	
   return gtk_icon_info_load_icon(icon_info, NULL);

   
}
//============================ GET N COLUMNS ================== //
int iFolder::get_n_columns ( void )
{
	return 3;
}
//========================= GET COLUMN TITLE ================== //
string	iFolder::get_column_title 	( int i )
{
	if (i==0) return _("Icon");
	if (i==1) return _("Filename");
	if (i==2) return _("Size");
	
	g_error("No more than 2 columns");
}

bool iFolder::has_items( void ) {
	if(n_files < 0 )
		n_files = get_folder_files(m_filename, m_files);
	if(n_current < n_files) {
		n_current++;
		return true;
	}
	return false;
}
