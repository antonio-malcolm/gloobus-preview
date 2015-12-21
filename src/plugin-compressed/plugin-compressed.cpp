#include "plugin-compressed.h"

#define ICOMPRESSED_ICON_SIZE 200

iCompressed::iCompressed()
	:m_finished(false)
	,m_io_channel(0)
	,cmd_pid(0)
	,m_list_started(false)
	,line_parser(NULL)
	,wait_id(0)
{
	g_debug("Creating iCompressed");
}

iCompressed::~iCompressed()
{
	g_debug("Destroying iCompressed");

	if(cmd_pid) {
		if(cmd_pid > 0) {
			g_source_remove(wait_id);
			kill(cmd_pid, 9);
			g_debug("Killed child process");
			g_spawn_close_pid(cmd_pid);
		}
	}
	if(m_io_channel)
		g_io_channel_unref(m_io_channel);
}


// ========================== GET PIXBUF ======================= //
GdkPixbuf * iCompressed::get_pixbuf ()
{
	return ui->get_file_icon(m_file_info, ICOMPRESSED_ICON_SIZE);
};

// ===================== GET FOLDER FILES ======================= //
bool iCompressed::get_archive_files () {
	gboolean ok;

	string type = get_safe_type(false);

	if (interface::is_program_available("bsdtar"))
	{
		cmd_args.push_back("bsdtar");
		cmd_args.push_back("-tf");
		line_parser = &iCompressed::parse_line;
	}
	else if ( type == GP_COMPRESSED_7Z )
	{
		cmd_args.push_back("7z");
		cmd_args.push_back("l");
		cmd_args.push_back("-slt");
		cmd_args.push_back("-bd");
		cmd_args.push_back("-y");
		cmd_args.push_back("--");
		line_parser = &iCompressed::parse_line_7z;
	}
	else if ( type == GP_COMPRESSED_RAR )
	{
		cmd_args.push_back("unrar");
		cmd_args.push_back("vb");
		cmd_args.push_back("-c-");
		cmd_args.push_back("-p-"); // do not query for password
		cmd_args.push_back("--");
		line_parser = &iCompressed::parse_line;
	}
	else if ( type == GP_COMPRESSED_TAR
			|| type == GP_COMPRESSED_TAR_BZIP
			|| type == GP_COMPRESSED_TAR_GZIP
			|| type == GP_COMPRESSED_TAR_LZMA
			|| type == GP_COMPRESSED_TAR_LZOP
			|| type == GP_COMPRESSED_TAR_XZ
			|| type == GP_COMPRESSED_TAR_Z )
	{
		cmd_args.push_back("tar");
		cmd_args.push_back("-tf");
		line_parser = &iCompressed::parse_line;
	}
	else if ( type == GP_COMPRESSED_AR
			|| type == GP_COMPRESSED_CPIO
			|| type == GP_COMPRESSED_ISO
			|| type == GP_COMPRESSED_ZIP )
	{
		cmd_args.push_back("zipinfo");
		cmd_args.push_back("-1");
		cmd_args.push_back("--");
		line_parser = &iCompressed::parse_line;
	}

	cmd_args.push_back(m_filename.c_str());

	ok = exec_cmd();
	if( !ok ) {
		set_error( g_error_new( g_quark_from_string ("iCompressed"), 601,
			_("Error listing files") ) );
		return false;
	}
	return true;
}

//========================= GET LIST STORE ================== //
GtkListStore*  	iCompressed::get_list_store		( void )
{
	return gtk_list_store_new  (1,G_TYPE_STRING);
}
//============================ GET N ITEMS ================== //
int iCompressed::get_n_items ( ) 
{
	//get_archive_files();
	//return m_files.size() - 1;
	return 0;
}
//============================ GET ITEM ================== //
string iCompressed::get_item (int i, int j)
{
	return m_files[i];
}
//============================ IS COLUMN PIXBUF ================== //
bool iCompressed::is_column_pixbuf(int i) 
{
	 	return false;
}
//============================ GET N COLUMNS ================== //
int iCompressed::get_n_columns ( void )
{
	return 1;
}
//========================= GET COLUMN TITLE ================== //
string	iCompressed::get_column_title 	( int i )
{
	if (i==0) return _("Filename");
		
	g_error("No more than 1 columns");
}

bool iCompressed::has_items( void ) {
	//g_debug("has_items");
	if(!m_io_channel)
		if( !get_archive_files() )
			return false;
	
	if( !m_finished ) {
		GIOStatus status;
		gchar *line = 0;
		gsize length;
		do {
			if(line) {
				g_free(line);
				line = 0;
			}
			status = g_io_channel_read_line(m_io_channel, &line, &length,
				NULL, NULL);
			if(status == G_IO_STATUS_EOF || status == G_IO_STATUS_ERROR) {
				m_finished = true;
			}
			if(m_finished) {
				g_free(line);
				return false;
			}
			if(line && line[length-1] == '\n')
				line[length-1] = NULL;
		} while ( ! (*this.*line_parser)(line) );
		if(line)
			g_free(line);
		return true;
	}
	return true;
}

bool iCompressed::exec_cmd( void ) {
	bool ok;
	int std_out;
	int asize = cmd_args.size();
	if( !asize )
		return false;
	char * argv[asize+1];
	for(int i=0 ; i < asize ; i++) {
		g_debug("%s", cmd_args[i].c_str());
		argv[i] = (char*)cmd_args[i].c_str();
	}
	argv[asize] = NULL;
	GError* error;

	ok = g_spawn_async_with_pipes(
		NULL, // *working_directory
		argv, // **argv
		NULL, // **envp
		(GSpawnFlags)(G_SPAWN_SEARCH_PATH
			| G_SPAWN_DO_NOT_REAP_CHILD), // flags
		NULL, // child_setup (callback)
		NULL, // user_data (for child_setup)
		&cmd_pid, // *child_pid
		NULL, // *standard_input
		&std_out, //*standard_output
		NULL, //*standard_error
		&error // **error
	);

	//g_debug("Child pid: %d", cmd_pid);
	wait_id = g_child_watch_add(cmd_pid, &wait_child, this);
	
	if( !ok ) {
		g_debug("Error spawning async: %s", error->message);
		return false;
	}

	m_io_channel = g_io_channel_unix_new(std_out);
	// g_io_channel_set_close_on_unref(m_io_channel, true); // do we need this?
	return true;
}

bool iCompressed::parse_line_7z( gchar * l) {
	g_debug("line: %s", l);
	string line(l);
	if(line.empty())
		return false;
	if (line.substr(0,5) == "Error") {
		m_finished = true;
	} else if ( !m_list_started ) {
		if ( line ==  "----------" )
			m_list_started = true;
		else if (line.substr(0,14) == "Multivolume = ")
			m_finished = true;
	} else if (line.substr(0,7) == "Path = ") {
		m_files.push_back(line.substr(7));
		return true;
	}
	return false;
}

bool iCompressed::parse_line( gchar * line) {
	m_files.push_back(line);
	return true;
}

void iCompressed::wait_child( GPid pid, gint status, gpointer data ) {
	iCompressed * self = (iCompressed*)data;
	g_debug("Child process returned");
	g_spawn_close_pid(self->cmd_pid);
	self->cmd_pid = -1;
}
