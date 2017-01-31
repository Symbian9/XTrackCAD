/** \file misc.c
 * Some basic window functions
 */

/*  XTrkCad - Model Railroad CAD
 *  Copyright (C) 2005 Dave Bullis
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//#define GTK_DISABLE_SINGLE_INCLUDES
//#define GDK_DISABLE_DEPRECATED
//#define GTK_DISABLE_DEPRECATED
//#define GSEAL_ENABLE

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include "gtkint.h"
#include "i18n.h"

wWin_p gtkMainW;

long debugWindow = 0;

char wConfigName[ 256 ];
		
const char * wNames[] = {
		"MAIN",
		"POPUP",
		"BUTT",
		"CANCEL",
		"POPUP",
		"TEXT",
		"INTEGER",
		"FLOAT",
		"LIST",
		"DROPLIST",
		"COMBOLIST",
		"RADIO",
		"TOGGLE",
		"DRAW",
		"MENU"
		"MULTITEXT",
		"MESSAGE",
		"LINES",
		"MENUITEM",
		"BOX"
		};


static wBool_t reverseIcon =
#if defined(linux)
				FALSE;
#else
				TRUE;
#endif



/*
 *****************************************************************************
 *
 * Internal Utility functions
 *
 *****************************************************************************
 */

GdkPixmap* wlibMakeIcon(
		GtkWidget * widget,
		wIcon_p ip,
		GdkBitmap ** mask )
{
	GdkPixmap * pixmap;
	char ** pixmapData;

	char line0[40];
	char line2[40];
	int row,col,wb;
	long rgb;
	const char * bits;
	GdkColor *transparent;
	
	transparent = &gtk_widget_get_style( gtkMainW->gtkwin )->bg[gtk_widget_get_state( gtkMainW->gtkwin )];
	
	if ( ip->gtkIconType == gtkIcon_pixmap ) {
		pixmap = gdk_pixmap_create_from_xpm_d( gtk_widget_get_window(gtkMainW->gtkwin), mask, transparent, (char**)ip->bits );
	}
	else {
		wb = (ip->w+7)/8;
		pixmapData = (char**)malloc( (3+ip->h) * sizeof *pixmapData );
		pixmapData[0] = line0;
		rgb = wDrawGetRGB(ip->color);
		sprintf( line0, " %d %d 2 1", ip->w, ip->h );
		sprintf( line2, "# c #%2.2lx%2.2lx%2.2lx", (rgb>>16)&0xFF, (rgb>>8)&0xFF, rgb&0xFF );
		pixmapData[1] = ". c None s None";
		pixmapData[2] = line2;
		bits = ip->bits;
		for (row = 0; row<ip->h; row++ ) {
			pixmapData[row+3] = (char*)malloc( (ip->w+1) * sizeof **pixmapData );
			for (col = 0; col<ip->w; col++ ) {
				if ( bits[ row*wb+(col>>3) ] & (1<<(col&07)) ) {
					pixmapData[row+3][col] = '#';
				} else {
					pixmapData[row+3][col] = '.';
				}
			}
			pixmapData[row+3][ip->w] = 0;
		}
		pixmap = gdk_pixmap_create_from_xpm_d( gtk_widget_get_window(gtkMainW->gtkwin), mask, transparent, pixmapData );
		for (row = 0; row<ip->h; row++ ) {
			free( pixmapData[row+3] );
		}
	}
	return pixmap;
}

/**
 * Add a label to an existing widget
 * 
 * \param b IN widget
 * \param labelStr IN label to add 
 * \returns size of label 
 */

int wlibAddLabel( wControl_p b, const char * labelStr )
{
	GtkRequisition requisition;
	if (labelStr == NULL)
		return 0;
	b->label = gtk_label_new(wlibConvertInput(labelStr));
	gtk_widget_size_request( b->label, &requisition );
	gtk_container_add( GTK_CONTAINER(b->parent->widget), b->label );
	gtk_fixed_move( GTK_FIXED(b->parent->widget), b->label, b->realX-requisition.width-8, b->realY+LABEL_OFFSET );

	gtk_widget_show( b->label );
	return requisition.width+8;
}

/**
 * Allocate and initialize the data structure for a new widget
 * 
 * \param parent IN parent window
 * \param type IN type of new widget
 * \param origX IN x position
 * \param origY IN y position
 * \param labelStr IN text label
 * \param size IN size
 * \param data IN user data to keep with widget
 * \returns 
 */

void * wlibAlloc(
		wWin_p parent,
		wType_e type,
		wPos_t origX,
		wPos_t origY,
		const char * labelStr,
		int size,
		void * data )
{
	wControl_p w = (wControl_p)malloc( size );
	char * cp;
	memset( w, 0, size );
	if (w == NULL)
		abort();
	w->type = type;
	w->parent = parent;
	w->origX = origX;
	w->origY = origY;
/*	gtkAccelChar = 0; */
	if (labelStr) {
		cp = (char*)malloc(strlen(labelStr)+1);
		w->labelStr = cp;
		for ( ; *labelStr; labelStr++ )
			if ( *labelStr != '&' )
				*cp++ = *labelStr;
			else {
/*				*cp++ = '_';
				gtkAccelChar = labelStr[1]; */
			}	
		*cp = 0;
	}
	w->doneProc = NULL;
	w->data = data;
	return w;
}

/**
 * Calculate the position for a widget
 * 
 * \param b IN widget
 */

void wlibComputePos(
		wControl_p b )
{
	wWin_p w = b->parent;

	if (b->origX >= 0) 
		b->realX = b->origX;
	else
		b->realX = w->lastX + (-b->origX) - 1;
	if (b->origY >= 0) 
		b->realY = b->origY + BORDERSIZE + ((w->option&F_MENUBAR)?MENUH:0);
	else
		b->realY = w->lastY + (-b->origY) - 1;
}

/**
 * Initialize the internal structure with the size of the widget
 * 
 * \param b IN widget 
 */

void wlibControlGetSize(
		wControl_p b )
{
	GtkRequisition requisition;
	gtk_widget_size_request( b->widget, &requisition );
	b->w = requisition.width;
	b->h = requisition.height;
}

/**
 * ???
 * \param b IN widget
 */

void wlibAddButton(
		wControl_p b )
{
	wWin_p win = b->parent;
	wBool_t resize = FALSE;
	if (win->first == NULL) {
		win->first = b;
	} else {
		win->last->next = b;
	}
	win->last = b;
	b->next = NULL;
	b->parent = win;
	win->lastX = b->realX + b->w;
	win->lastY = b->realY + b->h;
	if (win->option&F_AUTOSIZE) {
		if (win->lastX > win->realX) {
			win->realX = win->lastX;
			if (win->w != (win->realX + win->origX)) {
				resize = TRUE;
				win->w = (win->realX + win->origX);
			}
		}
		if (win->lastY > win->realY) {
			win->realY = win->lastY;
			if (win->h != (win->realY + win->origY)) {
			resize = TRUE;
				win->h = (win->realY + win->origY);
			}
		}
		if (win->shown) {
			if ( resize ) {
				gtk_widget_set_size_request( win->gtkwin, win->w, win->h );
				gtk_widget_set_size_request( win->widget, win->w, win->h );
			}
		}
	}
}

/**
 * Find the widget at a position
 * \param win IN searched widget's parent window
 * \param x IN x position inside parent
 * \param y IN y position inside parent
 * \returns the widget, NULL if none at the position
 */

wControl_p wlibGetControlFromPos(
		wWin_p win,
		wPos_t x,
		wPos_t y )
{
	wControl_p b;
	wPos_t xx, yy;
	for (b=win->first; b != NULL; b = b->next) {
		if ( b->widget && gtk_widget_get_visible(b->widget) ) {
			xx = b->realX;
			yy = b->realY;
			if ( xx <= x && x < xx+b->w &&
				 yy <= y && y < yy+b->h ) {
				return b;
			}
		}
	}
	return NULL;
}


/*
 *****************************************************************************
 *
 * Exported Utility Functions 
 *
 *****************************************************************************
 */

/**
 * Beep! 
 * \return
 */
void wBeep( void )
{
	gdk_display_beep(gdk_display_get_default());
}


/**
 * Flushs all commands to the Window.
 */

void wFlush(
		void )
{
	while ( gtk_events_pending() )
		gtk_main_iteration();

	gdk_display_sync(gdk_display_get_default());
}

/**
 * Not implemented
 * \returns 
 */
 
void wWinTop( wWin_p win )
{
}

/**
 * Not implemented
 * 
 * \param cursor IN 
 */

void wSetCursor( wCursor_t cursor )
{
}

/**
 * Not implemented
 * \returns 
 */

const char * wMemStats( void )
{
	return "No stats available";
}

/**
 * Get the size of the screen
 * 
 * \param w IN pointer to width
 * \param h IN pointer to height
 */

void wGetDisplaySize( wPos_t * w, wPos_t * h )
{

	*w = gdk_screen_width();
	*h = gdk_screen_height();
}

static dynArr_t conversionBuffer_da;

/**
 * Convert a string to UTF-8
 * 
 * \param inString IN string to convert
 * \returns pointer to converted string, valid until next call to conversion function
 */

char * wlibConvertInput( const char * inString )
{
	const char * cp;
	char * cq;
	int extCharCnt, inCharCnt;

	/* Already UTF-8 encoded? */
	if (g_utf8_validate(inString, -1, NULL))
		/* Yes, do not double-convert */
		return (char*)inString;

	for ( cp=inString, extCharCnt=0; *cp; cp++ ) {
		if ( ((*cp)&0x80) != '\0' )
			extCharCnt++;
	}
	inCharCnt = cp-inString;
	if ( extCharCnt == '\0' )
		return (char*)inString;
	DYNARR_SET( char, conversionBuffer_da, inCharCnt+extCharCnt+1 );
	for ( cp=inString, cq=(char*)conversionBuffer_da.ptr; *cp; cp++ ) {
		if ( ((*cp)&0x80) != 0 ) {
			*cq++ = 0xC0+(((*cp)&0xC0)>>6);
			*cq++ = 0x80+((*cp)&0x3F);
		} else {
			*cq++ = *cp;
		}
	}
	*cq = 0;
	return (char*)conversionBuffer_da.ptr;
}

/**
 * Convert a string from UTF-8 to system codepage
 * 
 * \param inString IN string to convert
 * \returns pointer to converted string, valid until next call to conversion function
 */

char * wlibConvertOutput( const char * inString )
{
	const char * cp;
	char * cq;
	int extCharCnt, inCharCnt;
	for ( cp=inString, extCharCnt=0; *cp; cp++ ) {
		if ( ((*cp)&0xC0) == 0x80 )
			extCharCnt++;
	}
	inCharCnt = cp-inString;
	if ( extCharCnt == '\0' )
		return (char*)inString;
	DYNARR_SET( char, conversionBuffer_da, inCharCnt+1 );
	for ( cp=inString, cq=(char*)conversionBuffer_da.ptr; *cp; cp++ ) {
		if ( ((*cp)&0x80) != 0 ) {
			*cq++ = 0xC0+(((*cp)&0xC0)>>6);
			*cq++ = 0x80+((*cp)&0x3F);
		} else {
			*cq++ = *cp;
		}
	}
	*cq = '\0';
	return (char*)conversionBuffer_da.ptr;
}

/*-----------------------------------------------------------------*/

typedef struct accelData_t {
		wAccelKey_e key;
		int modifier;
		wAccelKeyCallBack_p action;
		void * data;
		} accelData_t;
static dynArr_t accelData_da;
#define accelData(N) DYNARR_N( accelData_t, accelData_da, N )

static guint accelKeyMap[] = {
			0,			/* wAccelKey_None, */
			GDK_Delete,		/* wAccelKey_Del, */
			GDK_Insert,		/* wAccelKey_Ins, */
			GDK_Home,		/* wAccelKey_Home, */
			GDK_End,		/* wAccelKey_End, */
			GDK_Page_Up,	/* wAccelKey_Pgup, */
			GDK_Page_Down,	/* wAccelKey_Pgdn, */
			GDK_Up,		/* wAccelKey_Up, */
			GDK_Down,		/* wAccelKey_Down, */
			GDK_Right,		/* wAccelKey_Right, */
			GDK_Left,		/* wAccelKey_Left, */
			GDK_BackSpace,	/* wAccelKey_Back, */
			GDK_F1,		/* wAccelKey_F1, */
			GDK_F2,		/* wAccelKey_F2, */
			GDK_F3,		/* wAccelKey_F3, */
			GDK_F4,		/* wAccelKey_F4, */
			GDK_F5,		/* wAccelKey_F5, */
			GDK_F6,		/* wAccelKey_F6, */
			GDK_F7,		/* wAccelKey_F7, */
			GDK_F8,		/* wAccelKey_F8, */
			GDK_F9,		/* wAccelKey_F9, */
			GDK_F10,		/* wAccelKey_F10, */
			GDK_F11,		/* wAccelKey_F11, */
			GDK_F12		/* wAccelKey_F12, */
		};

/**
 * Create an accelerator key
 * 
 * \param key IN primary key stroke
 * \param modifier IN modifier (shift, ctrl, etc.)
 * \param action IN function to call
 * \param data IN data to pass to function 
 */

void wAttachAccelKey(
		wAccelKey_e key,
		int modifier,
		wAccelKeyCallBack_p action,
		void * data )
{
	accelData_t * ad;
	if ( key < 1 || key > wAccelKey_F12 ) {
		fprintf( stderr, "wAttachAccelKey(%d) out of range\n", (int)key );
		return;
	}
	DYNARR_APPEND( accelData_t, accelData_da, 10 );
	ad = &accelData(accelData_da.cnt-1);
	ad->key = key;
	ad->modifier = modifier;
	ad->action = action;
	ad->data = data;
}

/**
 * Check for accelerator definition a pressed key
 * 
 * \param event IN key press event
 * \returns pointer to accel key structure, NULL if not existing
 */

struct accelData_t * wlibFindAccelKey(
		GdkEventKey * event )
{
	accelData_t * ad;
	int modifier = 0;
	if ( ( event->state & GDK_SHIFT_MASK ) )
		modifier |= WKEY_SHIFT;
	if ( ( event->state & GDK_CONTROL_MASK ) )
		modifier |= WKEY_CTRL;
	if ( ( event->state & GDK_MOD1_MASK ) )
		modifier |= WKEY_ALT;
	for ( ad=&accelData(0); ad<&accelData(accelData_da.cnt); ad++ )
		if ( event->keyval == accelKeyMap[ad->key] &&
			 modifier == ad->modifier )
			return ad;
	return NULL;
}

/**
 * Perform action when an accelerator key was pressed
 * 
 * \param event IN key press event
 * \returns  TRUE if valid accelerator, FALSE if not
 */

wBool_t wlibHandleAccelKey(
		GdkEventKey *event )
{
	accelData_t * ad = wlibFindAccelKey( event );
	if ( ad ) {
		ad->action( ad->key, ad->data );
		return TRUE;
	}
	return FALSE;
}

/**
 * Add control to circular list of synonymous controls. Synonymous controls are kept in sync by 
 * calling wControlLinkedActive for one member of the list 
 * \todo This is similar to the concept of action in gtk/glib \
 * Maybe this would be easier to use 
 *
 * \param b1 IN  first control
 * \param b2 IN  second control
 * \return    none
 */
 
EXPORT void wControlLinkedSet( wControl_p b1, wControl_p b2 )
{
	
	b2->synonym = b1->synonym;
	if( b2->synonym == NULL )
		b2->synonym = b1;
		
	b1->synonym = b2;
} 	

/**
 * Activate/deactivate a group of synonymous controls.
 *
 * \param b IN  control
 * \param active IN  state
 * \return    none
 */

EXPORT void wControlLinkedActive( wControl_p b, int active )
{
	wControl_p savePtr = b;

	if( savePtr->type == B_MENUITEM )
		wMenuPushEnable( (wMenuPush_p)savePtr, active );
	else 	
		wControlActive( savePtr, active );		

	savePtr = savePtr->synonym;

	while( savePtr && savePtr != b ) {	

		if( savePtr->type == B_MENUITEM )
			wMenuPushEnable( (wMenuPush_p)savePtr, active );
		else 	
			wControlActive( savePtr, active );		

		savePtr = savePtr->synonym;
	}	
}
