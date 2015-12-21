#ifndef _DEFINES_
#define _DEFINES_

/*Translations*/
#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)

// ======================= INTERFACE ==================== //
/*
INTERFACE
 
      .............
     ·/-----------\·
    ·|___HEADER____|·
   ··|             |··
   ··|    BODY     |··
   ··|             |··
   ··|_____________|··
    ·|   BOTTOM    |·
     ·\-----------/·
      ·············

         · = SHADOW

*/
#define HEADER_HEIGHT	24	//This is the header height
#define HEADER_RADIOUS 6
#define WINDOW_RADIOUS	6
#define BOTTOM_HEIGHT 	45	//This is the bottom height, were we can put our little information
#define SHADOW_R	6	// Shadow radius
#define SHADOW_A	0.4	// Shadow alpha
#define SHADOW_WIDTH	15	// Shadow Width
#define PI		3.14	// Typical PI Constant
#define TOOLBAR_HEIGHT 64

#define DEFAULT_WIDTH 	410
#define DEFAULT_HEIGHT	630

#define ICON_SIZE	128
#define TOOLBAR_ICON_SIZE 32

#define BG_R 0.08		//from 0 none to 1 full
#define BG_G 0.08		//from 0 none to 1 full
#define BG_B 0.08		//from 0 none to 1 full
#define BG_A 0.91		//from 0 transparent to 1 opaque

#define STROKE_R 0.71		//from 0 none to 1 full
#define STROKE_G 0.71		//from 0 none to 1 full
#define STROKE_B 0.71		//from 0 none to 1 full
#define STROKE_A 0.85		//from 0 transparent to 1 opaque

#define FONT_COLOR "#CCCCCC"	//Hex color

#define CLI_COLOR_RESET     "\x1b[0m"
#define CLI_COLOR_BOLD      "\x1b[01m"
#define CLI_COLOR_TEAL      "\x1b[36;06m"
#define CLI_COLOR_TURQUOISE "\x1b[36;01m"
#define CLI_COLOR_FUCSIA    "\x1b[35;01m"
#define CLI_COLOR_PURPLE    "\x1b[35;06m"
#define CLI_COLOR_BLUE      "\x1b[34;01m"
#define CLI_COLOR_DARKBLUE  "\x1b[34;06m"
#define CLI_COLOR_GREEN     "\x1b[32;01m"
#define CLI_COLOR_DARKGREEN "\x1b[32;06m"
#define CLI_COLOR_YELLOW    "\x1b[33;01m"
#define CLI_COLOR_BROWN     "\x1b[33;06m"
#define CLI_COLOR_RED       "\x1b[31;01m"
#define CLI_COLOR_DARKRED   "\x1b[31;06m"

#define MANAGER_VERSION 2

#endif
