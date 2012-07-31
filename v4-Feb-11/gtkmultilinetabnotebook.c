#include <gtk/gtk.h>

#include "gtkmultilinetabnotebook.h"
#include "gtkmultilinetab.c"


static void     gtk_multiline_tab_notebook_class_init   (GtkMultilineTabNotebookClass *);
static void     gtk_multiline_tab_notebook_init         (GtkMultilineTabNotebook *);


GType
gtk_multiline_tab_notebook_get_type (void)
{
    static GType multiline_tab_notebook_type = 0;

    if (!multiline_tab_notebook_type) 
    {
        static const GTypeInfo multiline_tab_notebook_info =
        {
            sizeof (GtkMultilineTabNotebookClass),
            NULL,
            NULL,
            (GClassInitFunc) gtk_multiline_tab_notebook_class_init,
            NULL,
            NULL,
            sizeof (GtkMultilineTabNotebook),
            0,
            (GInstanceInitFunc) gtk_multiline_tab_notebook_init,
        };

        multiline_tab_notebook_type = g_type_register_static (GTK_TYPE_VBOX, "GtkMultilineTabNotebook", &multiline_tab_notebook_info, 0);
    }

  return multiline_tab_notebook_type;
}

static void 
gtk_multiline_tab_notebook_class_init (GtkMultilineTabNotebookClass *the_class)
{
}

GtkWidget*
gtk_multiline_tab_notebook_new (GtkNotebook *notebook)
{
    GtkMultilineTabNotebook *multiline_tab_notebook;
    GtkWidget *multiline_tab;

    if (!notebook)
        notebook = GTK_NOTEBOOK (gtk_notebook_new ());
    else
        g_return_if_fail (GTK_IS_NOTEBOOK (notebook));

    multiline_tab_notebook = GTK_MULTILINE_TAB_NOTEBOOK (g_object_new (gtk_multiline_tab_notebook_get_type (), NULL));

    multiline_tab = gtk_multiline_tab_new (notebook);
    multiline_tab_notebook->multiline_tab = GTK_MULTILINE_TAB (multiline_tab);

    gtk_notebook_set_tab_pos (notebook, GTK_POS_TOP);
    gtk_notebook_set_show_tabs (notebook, FALSE);
    gtk_notebook_set_scrollable (notebook, TRUE);

    gtk_box_pack_start (GTK_BOX (multiline_tab_notebook), GTK_WIDGET (multiline_tab), FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (multiline_tab_notebook), GTK_WIDGET (notebook), FALSE, FALSE, 0);

    return GTK_WIDGET (multiline_tab_notebook);
}

static void 
gtk_multiline_tab_notebook_init (GtkMultilineTabNotebook *multiline_tab_notebook)
{
    multiline_tab_notebook->multiline_tab = NULL;
}

GtkNotebook*
gtk_multiline_tab_notebook_get_notebook (GtkMultilineTabNotebook *multiline_tab_notebook)
{
    g_return_if_fail (GTK_IS_MULTILINE_TAB_NOTEBOOK (multiline_tab_notebook));
    
    return multiline_tab_notebook->multiline_tab->notebook;
}

