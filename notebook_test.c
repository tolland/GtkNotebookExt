#include <geany/geany.h>
#include <geany/plugindata.h>


#include <geany/document.h>
#include <geany/editor.h>
#include <geany/encodings.h>
#include <geany/filetypes.h>
#include <geany/highlighting.h>
#include <geany/keybindings.h>
#include <geany/msgwindow.h>
#include <geany/prefs.h>
#include <geany/project.h>
#include <geany/search.h>
#include <geany/stash.h>
#include <geany/support.h>
//#include <geany/templates.h>
#include <geany/toolbar.h>
#include <geany/ui_utils.h>
#include <geany/utils.h>
#include <gtk/gtk.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkpaned.h>
GeanyPlugin     *geany_plugin;
GeanyData       *geany_data;
GeanyFunctions  *geany_functions;


PLUGIN_VERSION_CHECK(211)

PLUGIN_SET_INFO("MultilineTabView", "Just make central notebook multiline",
                "1.0", "Maxim Kot work.maydjin@gmail.com");



static GtkWidget *multiline_tab_notebook = NULL;


void plugin_init(GeanyData *data)
{
    GtkWidget *notebook;
    char bufferf[32];
    char bufferl[32];
    GtkWidget* frame;
    GtkWidget* label;
    GtkWidget* notebook_parent;



    notebook =  GTK_WIDGET(geany->main_widgets->notebook);
    notebook_parent = gtk_widget_get_parent(GTK_WIDGET(notebook));
    gtk_container_remove(GTK_CONTAINER(notebook_parent), GTK_WIDGET(notebook));

    /* create a new extended notebook which contains a regular GtkNotebook widget */
    multiline_tab_notebook = GTK_WIDGET(gtk_multiline_tab_notebook_new (GTK_NOTEBOOK (notebook)));
    gtk_paned_add2(GTK_PANED(notebook_parent),GTK_WIDGET(multiline_tab_notebook));
    gtk_paned_pack2(GTK_PANED(notebook_parent),GTK_WIDGET(multiline_tab_notebook),FALSE, FALSE);
    gtk_widget_show_all(notebook_parent);
}

void plugin_cleanup(void)
{
    GtkWidget *notebook;
    char bufferf[32];
    char bufferl[32];
    GtkWidget* frame;
    GtkWidget* label;
    GtkWidget* notebook_parent;


    notebook =  GTK_WIDGET(geany->main_widgets->notebook);
    notebook_parent = gtk_widget_get_parent(GTK_WIDGET(multiline_tab_notebook));

    g_object_ref(notebook);
         gtk_container_remove(GTK_CONTAINER(multiline_tab_notebook), GTK_WIDGET(notebook));
         gtk_container_remove(GTK_CONTAINER(notebook_parent), GTK_WIDGET(multiline_tab_notebook));
         gtk_paned_add2(GTK_PANED(notebook_parent),GTK_WIDGET(notebook));
         gtk_paned_pack2(GTK_PANED(notebook_parent),GTK_WIDGET(notebook),FALSE, FALSE);
    g_object_unref(notebook);

    gtk_widget_show_all(notebook_parent);
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
    gtk_widget_destroy(multiline_tab_notebook);
}

