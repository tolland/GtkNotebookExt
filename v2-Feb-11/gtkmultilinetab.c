#include <gtk/gtk.h>
#include <gtk/gtkprivate.h>

#include "gtkmultilinetab.h"

#define HORIZONTAL_GAP  2
#define VERTICAL_GAP    2

#define MULTILINE_TAB_DEFAULT_SIZE  100

/* GtkWidget class methods */
static void     gtk_multiline_tab_class_init        (GtkMultilineTabClass *);
static void     gtk_multiline_tab_init              (GtkMultilineTab *);
static void     gtk_multiline_tab_destroy           (GtkObject *);
static void     gtk_multiline_tab_realize           (GtkWidget *);
static void     gtk_multiline_tab_size_request      (GtkWidget *, GtkRequisition *);
static void     gtk_multiline_tab_size_allocate     (GtkWidget *, GtkAllocation *);
static gboolean gtk_multiline_tab_expose            (GtkWidget *, GdkEventExpose *);
static gboolean gtk_multiline_tab_button_press      (GtkWidget *, GdkEventButton *);
static gboolean gtk_multiline_tab_button_release    (GtkWidget *, GdkEventButton *);
static gboolean gtk_multiline_tab_motion_notify     (GtkWidget *, GdkEventMotion *);

static void     gtk_multiline_tab_label_add         (GtkNotebook *, GtkWidget *, guint, gpointer);
static void     gtk_multiline_tab_label_remove      (GtkNotebook *, GtkWidget *, guint, gpointer);
static gboolean gtk_multiline_tab_label_change      (GtkNotebook *, gint, gpointer);

static GtkWidgetClass *parent_class = NULL;

GType
gtk_multiline_tab_get_type (void)
{
    static GType multiline_tab_type = 0;

    if (!multiline_tab_type) 
    {
        static const GTypeInfo multiline_tab_info =
        {
            sizeof (GtkMultilineTabClass),
            NULL,
            NULL,
            (GClassInitFunc) gtk_multiline_tab_class_init,
            NULL,
            NULL,
            sizeof (GtkMultilineTab),
            0,
            (GInstanceInitFunc) gtk_multiline_tab_init,
        };

        multiline_tab_type = g_type_register_static (GTK_TYPE_WIDGET, "GtkMultilineTab", &multiline_tab_info, 0);
    }

  return multiline_tab_type;
}

static void 
gtk_multiline_tab_class_init (GtkMultilineTabClass *the_class)
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;

    object_class = (GtkObjectClass*) the_class;
    widget_class = (GtkWidgetClass*) the_class;

    parent_class = gtk_type_class (gtk_widget_get_type ());

    object_class->destroy = gtk_multiline_tab_destroy;

    widget_class->realize = gtk_multiline_tab_realize;    
    widget_class->expose_event = gtk_multiline_tab_expose;
    widget_class->size_request = gtk_multiline_tab_size_request;
    widget_class->size_allocate = gtk_multiline_tab_size_allocate;
    widget_class->button_press_event = gtk_multiline_tab_button_press;
    widget_class->button_release_event = gtk_multiline_tab_button_release;
    widget_class->motion_notify_event = gtk_multiline_tab_motion_notify;
}

static void 
gtk_multiline_tab_init (GtkMultilineTab *multiline_tab)
{
    multiline_tab->notebook = NULL;
    multiline_tab->rectangles = (GList*) NULL;
    multiline_tab->tab_height = 0;

//  gtk_widget_set_has_window (GTK_WIDGET (multiline_tab), FALSE);
}

GtkWidget*
gtk_multiline_tab_new (GtkNotebook *notebook)
{
    GtkMultilineTab *multiline_tab;

    g_return_if_fail (notebook != NULL);
    g_return_if_fail (GTK_IS_NOTEBOOK (notebook));
    
    multiline_tab = g_object_new (gtk_multiline_tab_get_type (), NULL);

    /* the notebook widget */
    multiline_tab->notebook = notebook;

    g_signal_connect (GTK_OBJECT (notebook), "page-added",
        GTK_SIGNAL_FUNC (gtk_multiline_tab_label_add),
        (gpointer) multiline_tab);
    g_signal_connect (GTK_OBJECT (notebook), "page-removed",
        GTK_SIGNAL_FUNC (gtk_multiline_tab_label_remove),
        (gpointer) multiline_tab);
    g_signal_connect (GTK_OBJECT (notebook), "change-current-page",
        GTK_SIGNAL_FUNC (gtk_multiline_tab_label_change),
        (gpointer) multiline_tab);

    /* hide the original tabs */
    gtk_notebook_set_show_tabs (notebook, FALSE);
    
    return GTK_WIDGET (multiline_tab);
}

static void
gtk_multiline_tab_destroy (GtkObject *object)
{
    GtkMultilineTab *multiline_tab;
    GList* rectangles;

    g_return_if_fail (object != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (object));

    multiline_tab = GTK_MULTILINE_TAB (object);

    if (multiline_tab->rectangles) 
    {
//      for (rectangles = multiline_tab->rectangles; rectangles; rectangles = rectangles->next)
//          g_free ((GdkRectangle *) rectangles->data);

        g_list_free (multiline_tab->rectangles);
    }
    
    if (GTK_OBJECT_CLASS (parent_class)->destroy) 
        (* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void
gtk_multiline_tab_realize (GtkWidget *widget)
{
    GdkWindowAttr attributes;
    gint attributes_mask;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (widget));

    gtk_widget_set_realized (widget, TRUE);

    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.event_mask = gtk_widget_get_events (widget) | 
        GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
        GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK |
        GDK_POINTER_MOTION_HINT_MASK;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);

    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    widget->window = gdk_window_new (widget->parent->window, 
        &attributes, attributes_mask);

    gdk_window_set_user_data (widget->window, widget);

    widget->style = gtk_style_attach (widget->style, widget->window);
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);
}

static void
gtk_multiline_tab_label_add (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer data)
{
    if (GTK_IS_WIDGET (GTK_WIDGET (data)) && gtk_widget_get_mapped (GTK_WIDGET (data)))
        gtk_widget_queue_resize (GTK_WIDGET (data));
}

static void
gtk_multiline_tab_label_remove (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer data)
{
    if (GTK_IS_WIDGET (GTK_WIDGET (data)) && gtk_widget_get_mapped (GTK_WIDGET (data)))
        gtk_widget_queue_resize (GTK_WIDGET (data));
}

static gboolean 
gtk_multiline_tab_label_change (GtkNotebook *notebook, gint arg1, gpointer data)
{
    if (GTK_IS_WIDGET (GTK_WIDGET (data)) && gtk_widget_get_mapped (GTK_WIDGET (data)))
        gtk_widget_queue_resize (GTK_WIDGET (data));
}

static guint handler_id;

static void
gtk_multiline_tab_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
    GtkMultilineTab *multiline_tab;
    gint npages;
    gint i;
    GtkWidget *child;
    gint max_height;
    GtkWidget *label;
    GtkAllocation allocation;
    GtkRequisition label_requisition;
    gint x, y;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (widget));

    multiline_tab = GTK_MULTILINE_TAB (widget);

    gtk_widget_get_allocation (widget->parent, &allocation);

    x = HORIZONTAL_GAP;
    y = VERTICAL_GAP;

    max_height = 0;

    npages = gtk_notebook_get_n_pages (multiline_tab->notebook);
    for (i = 0; i < npages; i++)
    {
        child = gtk_notebook_get_nth_page (multiline_tab->notebook, i);
        label = gtk_notebook_get_tab_label (multiline_tab->notebook, child);

        gtk_widget_size_request (label, &label_requisition);

        max_height = MAX (max_height, label_requisition.height);

        if (x + label_requisition.width + HORIZONTAL_GAP >= allocation.width)
        {
            x = HORIZONTAL_GAP;
            y += max_height + VERTICAL_GAP;
            max_height = 0;
        }

        x += label_requisition.width + HORIZONTAL_GAP;
    }

    multiline_tab->tab_height = y + max_height + VERTICAL_GAP;

//  requisition->width   = allocation.width;
//  requisition->height  = multiline_tab->tab_height;

    if (g_signal_handler_is_connected (widget->parent, handler_id))
        g_signal_handler_disconnect (widget->parent, handler_id);

    requisition->width   = MULTILINE_TAB_DEFAULT_SIZE;
    requisition->height  = MULTILINE_TAB_DEFAULT_SIZE;
}

static void
gtk_multiline_tab_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
    GtkMultilineTab *multiline_tab;
    gint npages;
    gint i;
    GtkWidget *child;
    gint max_height;
    GtkWidget *label;
    GList *rectangles;
    GdkRectangle *rectangle;
    GtkRequisition requisition;
    gint x, y;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (widget));
    g_return_if_fail (allocation != NULL);

    multiline_tab = GTK_MULTILINE_TAB (widget);

    /* set GtkMultilineTab allocation */
    gtk_widget_set_allocation (widget, allocation);

    if (gtk_widget_get_realized (widget))
    {
        gdk_window_move_resize (widget->window,
            allocation->x, allocation->y,
            allocation->width, allocation->height);

        /* set label rectangles */

        if (multiline_tab->rectangles)
        {
            for (rectangles = multiline_tab->rectangles; rectangles; rectangles = rectangles->next)
                g_free ((GdkRectangle *) rectangles->data);

            g_list_free (multiline_tab->rectangles);
            multiline_tab->rectangles = NULL;
        }

        x = HORIZONTAL_GAP;
        y = VERTICAL_GAP;

        max_height = 0;

        npages = gtk_notebook_get_n_pages (multiline_tab->notebook);
        for (i = 0; i < npages; i++)
        {
            child = gtk_notebook_get_nth_page (multiline_tab->notebook, i);
            label = gtk_notebook_get_tab_label (multiline_tab->notebook, child);

            gtk_widget_size_request (label, &requisition);

            rectangle = g_new (GdkRectangle, 1);

            max_height = MAX (max_height, requisition.height);

            if (x + requisition.width + HORIZONTAL_GAP >= allocation->width)
            {
                x = HORIZONTAL_GAP;
                y += max_height + VERTICAL_GAP;
                max_height = 0;
            }

            rectangle->x = x;
            rectangle->y = y;
            rectangle->width  = requisition.width;
            rectangle->height = requisition.height;

            multiline_tab->rectangles = g_list_append (multiline_tab->rectangles, rectangle);

            x += requisition.width + HORIZONTAL_GAP;
        }

        multiline_tab->tab_height = y + max_height + VERTICAL_GAP;
    }

    if (!g_signal_handler_is_connected (widget->parent, handler_id))
        handler_id = g_signal_connect_swapped (GTK_OBJECT (widget->parent), "size-allocate",
            GTK_SIGNAL_FUNC (gtk_widget_queue_resize), (gpointer) widget);
}

static gboolean
gtk_multiline_tab_expose (GtkWidget *widget, GdkEventExpose *event)
{
    GtkMultilineTab *multiline_tab;
    GtkWidget *child;
    GtkStateType state_type;
    guint npages;
    guint current_page;
    gint i;
    GList *rectangles;
    GdkRectangle *rectangle;
    const gchar *text;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_MULTILINE_TAB (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    multiline_tab = GTK_MULTILINE_TAB (widget);

    if (gtk_widget_is_drawable (widget) && multiline_tab->rectangles) 
    {
        current_page = gtk_notebook_get_current_page (multiline_tab->notebook);

        npages = gtk_notebook_get_n_pages (multiline_tab->notebook);
        rectangles = multiline_tab->rectangles;
        
        for (i = 0; i < npages; i++)
        {
            child = gtk_notebook_get_nth_page (multiline_tab->notebook, i);
            text = gtk_notebook_get_tab_label_text (multiline_tab->notebook, child);

            state_type = (i == current_page ? GTK_STATE_NORMAL : GTK_STATE_ACTIVE);

            rectangle = (GdkRectangle *) rectangles->data;

            gtk_paint_extension (widget->style, widget->window,
                state_type, GTK_SHADOW_OUT,
                &event->area, widget, "tab",
                rectangle->x - 1, rectangle->y - 1,
                rectangle->width + 1, rectangle->height + 1,
                GTK_POS_BOTTOM);

            gtk_paint_string (widget->style, widget->window, state_type,
                &event->area, widget, "label",
                rectangle->x + HORIZONTAL_GAP, rectangle->y + rectangle->height - VERTICAL_GAP,
                text);
			    
            rectangles = rectangles->next;
        }
    }
    
    return FALSE;
}

static gboolean
gtk_multiline_tab_button_press (GtkWidget *widget, GdkEventButton *event)
{
    GtkMultilineTab *multiline_tab;
    guint npages;
    guint current_page;
    gint i;
    GList *rectangles;
    GdkRectangle *rectangle;

    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_MULTILINE_TAB (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    multiline_tab = GTK_MULTILINE_TAB (widget);

    npages = gtk_notebook_get_n_pages (multiline_tab->notebook);
    rectangles = multiline_tab->rectangles;

    for (i = 0; i < npages; i++)
    {
        rectangle = (GdkRectangle *) rectangles->data;

        if (event->x >= rectangle->x && event->x < (rectangle->x + rectangle->width) &&
            event->y >= rectangle->y && event->y < (rectangle->y + rectangle->height))
        {
            current_page = i;
            break;
        }

        rectangles = rectangles->next;
    }

    gtk_notebook_set_current_page (multiline_tab->notebook, i);
    gtk_widget_queue_draw (widget);

    return FALSE;
}

static gboolean
gtk_multiline_tab_button_release (GtkWidget *widget, GdkEventButton *event)
{
    return FALSE;
}

static gboolean
gtk_multiline_tab_motion_notify (GtkWidget *widget, GdkEventMotion *event)
{
    return FALSE;
}
