#include "gloobus-preview-interface-list.h"

#define ILIST_WIDTH 500
#define ILIST_HEIGHT 500
#define ILIST_PADDING 10

iList::iList()
	:pix_icon(0)
	,pix_reflection(0)
	,n_items(0)
	,m_store(0)
	,m_scwin(0)
	,m_tree_view(0)
	,loader_id(0)
	,self_ptr(0)
{
	g_debug("Creating iList");
	self_ptr = new gpointer;
	*self_ptr = (gpointer)this;
}
iList::~iList()
{
	g_debug("Destroying iList");
	if(pix_icon)
		g_object_unref( pix_icon );
	if(pix_reflection)
		g_object_unref( pix_reflection );
	if(m_store)
		g_object_unref( m_store );
}
//============================= END =========================================== //
void iList::end()
{
	g_debug("Ending iList");
	if(loader_id)
		g_source_remove(loader_id);
	if(*self_ptr)
		*self_ptr = 0;
	else
		delete self_ptr;

	//if(m_scwin)
	//	gtk_widget_destroy(m_scwin);
}
// ========================= CONSTRUCTOR ================================= //
bool iList::load()
{
	g_debug("Loading iList");
	GdkPixbuf* temp = get_pixbuf();
	pix_icon 	 = ui->pixbuf_scale(temp ,ICON_SIZE,ICON_SIZE);
	pix_reflection = ui->pixbuf_reflect(pix_icon);
	g_object_unref(temp);

	m_scwin = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(m_scwin),
		GTK_POLICY_AUTOMATIC ,GTK_POLICY_AUTOMATIC );
	gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW(m_scwin),
		GTK_SHADOW_NONE);
	//gtk_widget_set_size_request( m_scwin , 480 , 285);
	//FIXME: shouldn't use 500. Should get window width.

	//n_items = get_n_items();
	m_store = get_list_store();

	GtkTreeViewColumn *col;
	GtkCellRenderer   *renderer;

	m_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(m_store));
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(m_tree_view), true);

	for(int k=0; k< get_n_columns(); k++) {
		if(is_column_pixbuf(k)) {
			renderer = gtk_cell_renderer_pixbuf_new( );                                                
			col = gtk_tree_view_column_new_with_attributes (
				get_column_title(k).c_str(), renderer, "pixbuf", k, NULL);
		} else {
			renderer = gtk_cell_renderer_text_new( );
			col = gtk_tree_view_column_new_with_attributes(
				get_column_title(k).c_str(), renderer, "markup", k, NULL);
			gtk_tree_view_column_set_sort_column_id  (col,k);
		}
		gtk_tree_view_append_column( GTK_TREE_VIEW(m_tree_view), col);
	}

	gtk_container_add( GTK_CONTAINER(m_scwin), m_tree_view);

	loader_id = g_idle_add(&file_loader, self_ptr);

	return true;
};

// =========================== GET SIZE (W)================================= //
int  iList::get_width()
{
	return ILIST_WIDTH;
};

// =========================== GET SIZE (H)================================= //
int  iList::get_height()
{
	return ILIST_HEIGHT;
};

void iList::draw( GtkContainer * container )
{
	gtk_alignment_set(GTK_ALIGNMENT(container), 0.5,0.5,0,0);
	GtkWidget *align = gtk_alignment_new(0.5,0.5,1,1);
	gtk_widget_set_size_request(align, ILIST_WIDTH, ILIST_HEIGHT);
	gtk_alignment_set_padding(GTK_ALIGNMENT(align),
		ILIST_PADDING, ILIST_PADDING, ILIST_PADDING, ILIST_PADDING);
	
	icon 		= gtk_image_new_from_pixbuf (pix_icon);
	reflection 	= gtk_image_new_from_pixbuf (pix_reflection);
	GtkWidget *mpad = gtk_alignment_new(0.5,0.5,1,1);
	gtk_widget_set_size_request(mpad, 25, 20);

	string info = get_file_info_string();
	GtkWidget *info_label = ui->get_theme()->get_label(info.c_str());
	gtk_misc_set_alignment(GTK_MISC(info_label), 0.5 , 0.5 );

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	GtkWidget *mbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	gtk_box_pack_start(GTK_BOX(mbox), icon, false, false, 0);
	gtk_box_pack_start(GTK_BOX(mbox), reflection, false, false, 0);

	gtk_box_pack_start(GTK_BOX(hbox), mpad, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), mbox, false, false, 0);
	gtk_box_pack_start(GTK_BOX(hbox), info_label, true, true, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, false, false, 0);
	gtk_box_pack_start(GTK_BOX(vbox), m_scwin, true, true, 0);

	gtk_container_add(GTK_CONTAINER(align), vbox);
	gtk_container_add(container, align);
	
	//gtk_fixed_put( GTK_FIXED(container), icon, 50, 10);
	//gtk_fixed_put( GTK_FIXED(container), reflection, 50, 10 + ICON_SIZE);
	//gtk_fixed_put( GTK_FIXED(container), m_scwin , 10 , 205 );
	//gtk_widget_show_all( m_scwin );
	//draw_properties ( container, get_file_info_string(), 250 , 60 );
	gtk_widget_show_all( GTK_WIDGET(container) );
}

void iList::draw_fullscreen ( GtkContainer * container )
{/*
	int x = ( ui->get_screen_width()-get_width() ) / 2;
	int y = ( ui->get_screen_height()-get_height() ) / 2;
	gtk_fixed_move( GTK_FIXED(container), icon, x+50, y+10);
	gtk_fixed_move( GTK_FIXED(container), reflection, x+50, y+10 + ICON_SIZE);
	gtk_fixed_move( GTK_FIXED(container), m_scwin , x+10 , y+205 );
	gtk_fixed_move( GTK_FIXED(container), m_info , x+250 , y+60 );*/
}

void iList::draw_unfullscreen ( GtkContainer * container )
{/*
	gtk_fixed_move( GTK_FIXED(container), icon, 50, 10);
	gtk_fixed_move( GTK_FIXED(container), reflection, 50, 10 + ICON_SIZE);
	gtk_fixed_move( GTK_FIXED(container), m_scwin , 10 , 205 );
	gtk_fixed_move( GTK_FIXED(container), m_info , 250 , 60 );*/
}

gboolean iList::file_loader(gpointer ptr) {
	g_debug("File loader started");
	if(*((iList**)ptr) == 0) {
		delete (iList**)ptr;
		return false;
	}
	iList* list = *((iList**)(ptr));
	GtkTreeIter iter;

	while(list->has_items()) {
		//g_debug("Adding %d", list->n_items);
		int i = list->n_items;
		gtk_list_store_append(list->m_store, &iter);
		for ( int j = 0 ; j < list->get_n_columns() ; j++) {
			if( list->is_column_pixbuf(j) ) {
				gtk_list_store_set ( list->m_store, &iter, j,
					list->get_item_pixbuf(i,j),-1);
			} else {
				gtk_list_store_set ( list->m_store, &iter, j,
					list->get_item(i,j).c_str(),-1);
			}
		}
		list->n_items++;
		while (gtk_events_pending ())
			gtk_main_iteration ();
		if(*((iList**)ptr) == 0) {
			delete (iList**)ptr;
			return false;
		}
	}
	if(list->n_items == 0) {
		if(!list->m_error) {
			list->m_error = g_error_new(g_quark_from_string ("iList"), 901,
			_("Error listing files"));
		}
	}
	return false;
}
