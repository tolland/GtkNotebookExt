#ifndef __GTK_MULTILINE_TAB_H__
#define __GTK_MULTILINE_TAB_H__


#include <gtk/gtknotebook.h>
#include <gtk/gtkbbox.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GTK_TYPE_MULTILINE_TAB              (gtk_multiline_tab_get_type ())
#define GTK_MULTILINE_TAB(obj)              (GTK_CHECK_CAST ((obj), GTK_TYPE_MULTILINE_TAB, GtkMultilineTab))
#define GTK_MULTILINE_TAB_CLASS(klass)      (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_MULTILINE_TAB, GtkMultilineTabClass))
#define GTK_IS_MULTILINE_TAB(obj)           (GTK_CHECK_TYPE ((obj), GTK_TYPE_MULTILINE_TAB))
#define GTK_IS_MULTILINE_TAB_CLASS(klass)   (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_MULTILINE_TAB))
#define GTK_MULTILINE_TAB_GET_CLASS(obj)    (GTK_CHECK_GET_CLASS ((obj), GTK_TYPE_MULTILINE_TAB, GtkMultilineTabClass))

typedef struct _GtMultilineTab          GtkMultilineTab;
typedef struct _GtkMultilineTabClass    GtkMultilineTabClass;

struct _GtMultilineTab
{
    GtkBox widget;

    /* the associated GtkNotebook widget */  
    GtkNotebook *notebook;

    /* label rectangles */
    GList* rectangles;
    
    /* dimension of the tab area */
    gint tab_height;

    /*page remove signal id*/
    guint page_remove_id;
    /*Tab label widgets*/
    GHashTable* labels;


};

struct _GtkMultilineTabClass
{
    GtkBoxClass parent_class;//he he he
};


static GtkWidget*  gtk_multiline_tab_new       (GtkNotebook *notebook);
static GtkType     gtk_multiline_tab_get_type  (void);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_MULTILINE_TAB_H__ */

