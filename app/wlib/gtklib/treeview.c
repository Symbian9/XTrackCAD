/** \file treeview.c
 * Basic treeview functionality for dropbox and listbox
 */

/* XTrkCad - Model Railroad CAD
 * 
 * Copyright 2016 Martin Fischer <m_fischer@users.sf.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "gtkint.h"
#include "i18n.h"

#define ROW_HEIGHT (15)

/**
 * Get the count of columns in list
 * 
 * \param b IN widget
 * \returns count row
 */

int
wTreeViewGetCount( wList_p b )
{
	return b->count;
}	

/**
 * Clear the list
 * 
 * \param b IN widget
 */

void
wTreeViewClear( wList_p b )
{
	assert( b != NULL );
	
	wlibListStoreClear( b->listStore );
}	

/**
 * Get the user data for a list element
 * 
 * \param b IN widget
 * \param inx IN row
 * \returns the user data for the specified row
 */
 
void *
wTreeViewGetItemContext( wList_p b, int row )
{
	wListItem_p id_p;
	
	id_p = wlibListItemGet( b->listStore, row, NULL );
	if ( id_p )
		return id_p->itemData;
	else
		return NULL;
}

/**
 * Returns the current selected list entry.
 * If <val> if '-1' then no entry is selected.
 *
 * \param b IN widget
 * \returns row of selected entry or -1 if none is selected
 */

wIndex_t wListGetIndex(
		wList_p b )
{
	assert(b!=NULL);
	
	return b->last;
}

/**
 * Set the selected state of an entry in the list
 * 
 * \param b IN widget
 * \param index IN entry
 * 
 */
 
void
wlibTreeViewToggleSelected( wList_p b, int index )
{
	GtkTreeSelection *sel;
	GtkTreeIter iter;	
	int current = b->last;
	wListItem_p id_p;
	
	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(b->treeView));

	if (current != -1) {
		if( gtk_tree_selection_get_selected( sel, NULL, NULL ) )
			gtk_tree_selection_unselect_all( sel );
		
		id_p = wlibListItemGet( b->listStore, index, NULL );
		//id_p = getListItem( b, cur, NULL );
		if ( id_p )
			id_p->selected = FALSE;
	}
	if (index != -1) {
		gtk_tree_model_iter_nth_child (GTK_TREE_MODEL(b->listStore),
                                      &iter,
                                      NULL,
                                      index);			
        gtk_tree_selection_select_iter (sel,
										&iter);                           
											
		//id_p = getListItem( b, val, NULL );
		id_p = wlibListItemGet( b->listStore, index, NULL );
		if ( id_p )
			id_p->selected = TRUE;
	}
}		

/**
 * Create a new tree view for a list store. Titles are enabled optionally.
 * 
 * \param ls IN list store
 * \param showTitles IN add column titles
 * \param multiSelection IN enable selecting multiple rows
 * \returns 
 */
 
GtkWidget *
wlibNewTreeView(GtkListStore *ls, int showTitles, int multiSelection)	
{
	GtkWidget *treeView;
	GtkTreeSelection *sel;
	assert( ls != NULL );
	
	/* create and configure the tree view */
	treeView = gtk_tree_view_new_with_model( GTK_TREE_MODEL( ls ));
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(treeView), showTitles );
	
	/* set up selection handling */
	sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(treeView));
	gtk_tree_selection_set_mode (sel, 
								(multiSelection)?GTK_SELECTION_MULTIPLE:GTK_SELECTION_BROWSE );                       
	
	return( treeView );
}


static int changeListColumnWidth(
		GtkTreeViewColumn * column,
		void * width )
{
	//wList_p bl = (wList_p)data;

	//if (bl->recursion)
		//return 0;
	//if ( col >= 0 && col < bl->colCnt )
		//bl->colWidths[col] = width;
	return 0;
}

/**
 * Create and initialize a column in treeview. Initially all columns are 
 * invisible. Visibility is set when values are added to the specific 
 * column
 * 
 * \param tv IN treeview
 * \param renderer IN renderer to use
 * \param attribute IN attribute for column
 * \param value IN value to set 
 */

static void
wlibAddColumn( GtkWidget *tv, int visibility, GtkCellRenderer *renderer, char *attribute, int value )
{
	GtkTreeViewColumn *column;
	
    column = gtk_tree_view_column_new();
	gtk_tree_view_column_pack_start (column,
                                     renderer,
  								     TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, attribute, value );
    gtk_tree_view_column_set_visible(column, visibility );
	gtk_tree_view_column_set_resizable (column, TRUE );
                                    
    gtk_tree_view_append_column (GTK_TREE_VIEW(tv), column);

//	g_signal_connect( column, "notify::width", G_CALLBACK(changeListColumnWidth), tv );
}
	
/**
 * Add a number of columns to the text view. This includes the bitmap
 * columns and a given number of text columns.
 * 
 * \param tv IN tree view
 * \param count IN number of text columns
 * \returns number of columns
 */

int
wlibTreeViewAddColumns( GtkWidget *tv, int count )
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	int i;
	
	assert( tv != NULL );	
	renderer = gtk_cell_renderer_pixbuf_new();
	/* first visible column is used for bitmaps */
	wlibAddColumn( tv, FALSE, renderer, "pixbuf", LISTCOL_BITMAP );
	
 	renderer = gtk_cell_renderer_text_new();
	/* add renderers to all columns */
	for ( i = 0; i < count; i++ ) {
		wlibAddColumn( tv, TRUE, renderer, "text", i + LISTCOL_TEXT );
	}
	return i;
}

/**
 * Add the titles to all columns in a tree view. 
 * 
 * \param tv IN treeview
 * \param titles IN titles
 * \returns number of titles set
 */

int
wlibAddColumnTitles( GtkWidget *tv, const char **titles )
{
	int i = 0;
	GtkTreeViewColumn *column;
	
	assert( tv != NULL );
	if( titles ) {
		while( *titles ) {
			column = gtk_tree_view_get_column (GTK_TREE_VIEW(tv), i + 1 );
			if( column ) {
				gtk_tree_view_column_set_title( column, titles[ i ] );
				i++;
			} else {
				break;
			}	
		}	
	}
	return i;
}	

/**
 * Add text to the text columns of the tree view and update the context
 * information
 * 
 * \param tv IN treeview
 * \param cols IN number of cols to change
 * \param label IN tab separated string of valules
 * \param userData IN additional context information
 * \returns 
 */

int
wlibTreeViewAddData( GtkWidget *tv, int cols, char *label, GdkPixbuf *pixbuf, wListItem_p userData )
{
	int usedCols; 
	GtkListStore *listStore = GTK_LIST_STORE( gtk_tree_view_get_model( GTK_TREE_VIEW(tv)));
	GtkTreeViewColumn *column;
	
	usedCols = wlibListStoreAddData( listStore, pixbuf, cols, userData );
	if( pixbuf ) {
		// first column in list store has pixbuf
		column = gtk_tree_view_get_column ( GTK_TREE_VIEW(tv), 0);
		gtk_tree_view_column_set_visible (column,
                                   TRUE);
	}	
}	

/**
 * Add a row to the tree view. As necessary the adjustment is update in
 * order to make sure, that the list box is fully visible or has a 
 * scrollbar. 
 * 
 * \param b IN the list box
 * \param label IN the text labels
 * \param bm IN bitmap to show at start
 * \param id_p IN user data
 */
 
void
wlibTreeViewAddRow( wList_p b, char *label, wIcon_p bm, wListItem_p id_p )
{
	GtkAdjustment *adj;
	GdkPixbuf *pixbuf;
	
	if( bm )
		pixbuf = wlibMakePixbuf( bm );
		
	wlibTreeViewAddData( b->treeView, b->colCnt, (char *)label, pixbuf, id_p );
	 
	adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(b->widget));

	if( gtk_adjustment_get_upper( adj ) < gtk_adjustment_get_step_increment( adj ) * (b->count+1)) {
		gtk_adjustment_set_upper( adj, 
								  gtk_adjustment_get_upper( adj ) + 
								  gtk_adjustment_get_step_increment( adj ));
		gtk_adjustment_changed( adj );
	}
}


/**
 * Callback for handling a selection change
 * 
 * \param selection IN current selection
 * \param data IN the widget
 */

static void
tree_selection_changed_cb (GtkTreeSelection *selection, gpointer data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GList *selectedRows;
    GtkTreePath *treePath;
    char *text;
	wList_p bl = (wList_p)data;
	wListItem_p id_p = NULL;
	GValue value = { 0 };	// never forget to initialize to zero
	
	int row;

	if (bl->recursion)
		return ;

    if (selectedRows = gtk_tree_selection_get_selected_rows (selection, &model ))
    {
		treePath = (GtkTreePath *)g_list_nth_data (selectedRows, 0 );
		text = gtk_tree_path_to_string( treePath );
		row = atoi( text );
		g_free( text );

		gtk_tree_model_get_iter( model, &iter, treePath );
		gtk_tree_model_get_value (model, &iter, LISTCOL_DATA, &value);
		
		id_p = g_value_get_pointer( &value );
		
		g_list_foreach (selectedRows, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (selectedRows);		
		
		if ( id_p == NULL ) return;
		bl->editted = FALSE;
		if ( (bl->option&BL_MANY)==0 && bl->last == row )
			return;
		bl->last = row;
		id_p->selected = TRUE;
		if (bl->valueP)
			*bl->valueP = row;
		if (bl->action)
			bl->action( row, id_p->label, 1, bl->data, id_p->itemData );	
    }
	return;
}

/**
 * Create a multi column list box.
 * 
 * \param parent IN parent window
 * \param x IN X-position
 * \param y IN Y-position
 * \param helpStr IN Help string
 * \param labelStr IN Label
 * \param option IN Options
 * \param number IN Number of displayed entries
 * \param width IN Width of list
 * \param colCnt IN Number of columns
 * \param colWidths IN Width of columns
 * \param colRightJust IN justification of columns
 * \param colTitles IN array of titles for columns
 * \param valueP IN Selected index
 * \param action IN Callback
 * \param data IN Context
 * \returns created list box
 */

wList_p wListCreate(
		wWin_p	parent,	
		wPos_t	x,		
		wPos_t	y,		
		const char 	* helpStr,	
		const char	* labelStr,	
		long	option,		
		long	number,		
		wPos_t	width,		
		int	colCnt,		
		wPos_t	* colWidths,	
		wBool_t * colRightJust,	
		const char 	** colTitles,	
		long	*valueP,
		wListCallBack_p action,
		void 	*data )
{
	GtkTreeSelection *sel;
	wList_p bl;
	static wPos_t zeroPos = 0;
	int i;
	
	assert( width != 0 );

	bl = (wList_p)wlibAlloc( parent, B_LIST, x, y, labelStr, sizeof *bl, data );
	bl->option = option;
	bl->number = number;
	bl->count = 0;
	bl->last = -1;
	bl->valueP = valueP;
	bl->action = action;
	bl->listX = bl->realX;

	if ( colCnt <= 0 ) {
		colCnt = 1;
		colWidths = &zeroPos;
	}

	bl->colCnt = colCnt;
	bl->colWidths = (wPos_t*)malloc( colCnt * sizeof *(wPos_t*)0 );
	memcpy( bl->colWidths, colWidths, colCnt * sizeof *(wPos_t*)0 );

	/* create the data structure for data */
	bl->listStore = wlibNewListStore(colCnt);
	/* create the widget for the list store */
	bl->treeView = wlibNewTreeView( bl->listStore, 
									colTitles != NULL,
									option & BL_MANY );	
    
    	
	sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(bl->treeView));                  
	g_signal_connect (G_OBJECT (sel), "changed",
                  G_CALLBACK (tree_selection_changed_cb),
                  bl);
	
	
	wlibTreeViewAddColumns( bl->treeView, colCnt );

	wlibAddColumnTitles( bl->treeView, colTitles ); 

	wlibComputePos( (wControl_p)bl );
	
	bl->widget = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (bl->widget),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW(bl->widget), bl->treeView );

	gtk_widget_set_size_request( bl->widget, width, (number+1)*ROW_HEIGHT );
	//g_signal_connect( GTK_OBJECT(bl->list), "unselect_row", G_CALLBACK(unselectCList), bl );
///	g_signal_connect( GTK_OBJECT(bl->list), "resize_column", G_CALLBACK(changeListColumnWidth), bl );
	//gtk_container_set_focus_vadjustment (GTK_CONTAINER (bl->list),
				//gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (bl->widget)));

	gtk_widget_show_all( bl->widget );

	gtk_fixed_put( GTK_FIXED(parent->widget), bl->widget, bl->realX, bl->realY );
	wlibControlGetSize( (wControl_p)bl );
	if (labelStr)
		bl->labelW = wlibAddLabel( (wControl_p)bl, labelStr );
	wlibAddButton( (wControl_p)bl );
	wlibAddHelpString( bl->widget, helpStr );

	return bl;
}
