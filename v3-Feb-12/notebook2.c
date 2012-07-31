#include <gtk/gtk.h>
#include "gtkmultilinetab.h"

/* callbacks */

void page_added (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer user_data)
{
    g_print ("page added\n");
}

static gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data)
{
    return FALSE;
}

static void destroy( GtkWidget *widget, gpointer   data )
{
    gtk_main_quit ();
}


int main( int   argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *multiline_tab;
    GtkWidget *table;
    GtkWidget *vbox;
    GtkWidget *button;
    char bufferf[32];
    char bufferl[32];
    GtkWidget *frame;
    GtkWidget *label;
    int i;
    
    gtk_init (&argc, &argv);
    
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (window, "delete-event", G_CALLBACK (delete_event), NULL);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    
    /* Create a new notebook, place the position of the tabs */
    notebook = gtk_notebook_new ();
    multiline_tab = gtk_multiline_tab_new (GTK_NOTEBOOK (notebook));
    g_signal_connect (notebook, "page-added", G_CALLBACK (page_added), NULL);
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
    gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
    
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), multiline_tab, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), notebook, FALSE, FALSE, 0);

    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_window_set_default_size (GTK_WINDOW (window), 600, 200);
    gtk_widget_show_all (window);

    /* append a bunch of pages to the notebook */
    button = gtk_button_new_with_label ("next page");

    for (i = 0; i < 5; i++) 
    {
        sprintf(bufferf, "Append Frame %d", i + 1);
        sprintf(bufferl, "Test #%d", i + 1);
        
        frame = gtk_frame_new (bufferf);
        gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
        gtk_widget_set_usize (frame, 100, 75);
        gtk_widget_show (frame);
        
        label = gtk_label_new (bufferl);
//        gtk_container_add (GTK_CONTAINER (frame), label);
        gtk_widget_show (label);
        
        gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);
    }

    int npages;
    const gchar *text;
    GtkWidget *child;
    
    npages = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook));    
    for (i = 0; i < npages; i++)
    {
        child = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), i);
        text = gtk_notebook_get_tab_label_text (GTK_NOTEBOOK (notebook), child);
        printf ("%s\n", text ? text : "NULL");
    }

    gtk_main ();
    
    return 0;
}

