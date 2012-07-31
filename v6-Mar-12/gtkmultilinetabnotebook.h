#ifndef __GTK_MULTILINE_TAB_NOTEBOOK_H__
#define __GTK_MULTILINE_TAB_NOTEBOOK_H__


#include <gtk/gtkvbox.h>

#include "gtkmultilinetab.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GTK_TYPE_MULTILINE_TAB_NOTEBOOK              (gtk_multiline_tab_notebook_get_type ())
#define GTK_MULTILINE_TAB_NOTEBOOK(obj)              (GTK_CHECK_CAST ((obj), GTK_TYPE_MULTILINE_TAB_NOTEBOOK, GtkMultilineTabNotebook))
#define GTK_MULTILINE_TAB_NOTEBOOK_CLASS(klass)      (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_MULTILINE_TAB_NOTEBOOK, GtkMultilineTabNotebookClass))
#define GTK_IS_MULTILINE_TAB_NOTEBOOK(obj)           (GTK_CHECK_TYPE ((obj), GTK_TYPE_MULTILINE_TAB_NOTEBOOK))
#define GTK_IS_MULTILINE_TAB_NOTEBOOK_CLASS(klass)   (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_MULTILINE_TAB_NOTEBOOK))
#define GTK_MULTILINE_TAB_NOTEBOOK_GET_CLASS(obj)    (GTK_CHECK_GET_CLASS ((obj), GTK_TYPE_MULTILINE_TAB_NOTEBOOK, GtkMultilineTabNotebookClass))

typedef struct _GtMultilineTabNotebook          GtkMultilineTabNotebook;
typedef struct _GtkMultilineTabNotebookClass    GtkMultilineTabNotebookClass;

struct _GtMultilineTabNotebook
{
    GtkVBox vbox;

    /* the child GtkMultilineTab widget */  
    GtkMultilineTab *multiline_tab;
};

struct _GtkMultilineTabNotebookClass
{
    GtkVBoxClass parent_class;
};


GtkType         gtk_multiline_tab_notebook_get_type     (void);
GtkWidget*      gtk_multiline_tab_notebook_new          (GtkNotebook *notebook);
GtkNotebook*    gtk_multiline_tab_notebook_get_notebook (GtkMultilineTabNotebook *multiline_tab_notebook);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_MULTILINE_TAB_NOTEBOOK_H__ */

