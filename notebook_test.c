
#include <gtk/gtk.h>
#include <gtk/gtknotebook.h>
#include "gtkmultilinetabnotebook.h"


static gboolean delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    return FALSE;
}


int main (int argc, char *argv[])
{

    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *multiline_tab_notebook;
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
    
    notebook = gtk_notebook_new ();

    /* create a new extended notebook which contains a regular GtkNotebook widget */
    multiline_tab_notebook = gtk_multiline_tab_notebook_new (GTK_NOTEBOOK (notebook));

    gtk_container_add (GTK_CONTAINER (window), multiline_tab_notebook);
    gtk_window_set_default_size (GTK_WINDOW (window), 600, 200);
    gtk_widget_show_all (window);

    /* append a bunch of pages to the notebook */

    for (i = 0; i < 15; i++)
    {
        sprintf(bufferf, "Append Frame %d", i + 1);
        sprintf(bufferl, "  Tab no%d  ", i + 1);
        
        printf("Child: %s\n\r", bufferl);
       // gtk_notebook_set_show_tabs(GTK_NOTEBOOK (notebook), TRUE);
        frame = gtk_frame_new (bufferf);
        gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
        gtk_widget_set_usize (frame, 100, 75);
        gtk_widget_show (frame);
        
        label = gtk_label_new (bufferl);
        gtk_widget_show (label);
        
        gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);
    }

    gtk_main ();
    
    return 0;
}

