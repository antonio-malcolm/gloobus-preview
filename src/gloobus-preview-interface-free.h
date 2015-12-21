#ifndef _IFREE_
#define _IFREE_

#include "gloobus-preview-interface-main.h"

class iFree : public interface
{
	public:
						iFree				( void );
						~iFree				( void );
		virtual	int		get_width			( void )=0;
		virtual	int		get_height			( void )=0;
		virtual	bool	load				( void );
		virtual	void	end					( void );
		virtual	void	draw				( GtkContainer * container )=0;
    	virtual	bool	key_press_event		( int ){ return false;}
};

#endif
