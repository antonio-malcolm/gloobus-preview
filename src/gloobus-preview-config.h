#ifndef _GLOOBUSCONFIG_
#define _GLOOBUSCONFIG_

enum gp_action {
	GP_ACTION_PREVIEW,
	GP_ACTION_FILETYPES,
	GP_ACTION_HELP,
	GP_ACTION_VERSION,
	GP_ACTION_CONFIG,
	GP_ACTION_UNKNOWN
};

enum gp_sort {
	GP_SORT_BY_NAME,
	GP_SORT_BY_DATE,
	GP_SORT_BY_TYPE,
	GP_SORT_BY_SIZE
};

using namespace std;

class GloobusConfig
{
	private:
		bool			m_taskbar;
		bool			m_ontop;
		bool			m_focus;
		bool			m_winbar_layout;

		gboolean		cli_debug;
		gp_action		cli_action;
		char		*	cli_target;
		gp_sort			cli_sort;
		
		char		*	theme_path;
		bool			theme_gtk;
		int				theme_shadow_radius;
		double			theme_shadow_alpha;
		//GdkColor	*	theme_shadow_color;

	public:
		GloobusConfig					(void);
		~GloobusConfig					(void);

		/* loads and parses configuration file
		 * */
		bool		load_config			(void);

		
		bool		get_gsettings_win_layout(void);

		/* Returns if show gloobus-preview in taskbar
		 * */
		bool		get_taskbar			(void);

		/* Returns if show gloobus-preview allways on top
		 * */
		bool		get_ontop			(void);

		/* Returns if close gloobus-preview on lose focus
		 * */
		bool		get_focus			(void);
		
		bool		get_winbar_layout	(void);
		
		/* parses command line arguments
		 * */
		void		parse_args			(int, char **);
		void		parse_args			(int*, char ***);

		/* returns TRUE if debug mode is requested
		 * */
		bool		get_cli_debug		(void);

		/* returns pointer to filename
		 * */
		char*		get_cli_target		(void);

		/* returns what gloobus-preview must do
		 * */
		gp_action	get_cli_action		(void);

		char*		get_theme			( void );

		bool		get_theme_gtk		( void );
		int			get_shadow_radius	( void );
		double		get_shadow_alpha	( void );
};

#endif
