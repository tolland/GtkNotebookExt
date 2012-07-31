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
static void     gtk_multiline_tab_map               (GtkWidget *);
static void     gtk_multiline_tab_unmap             (GtkWidget *);
static void     gtk_multiline_tab_draw              (GtkWidget *, GdkRectangle *);
static gboolean gtk_multiline_tab_expose            (GtkWidget *, GdkEventExpose *);
static gboolean gtk_multiline_tab_button_press      (GtkWidget *, GdkEventButton *);
static gboolean gtk_multiline_tab_button_release    (GtkWidget *, GdkEventButton *);
static gboolean gtk_multiline_tab_motion_notify     (GtkWidget *, GdkEventMotion *);

/* GtkContainer class methods */
static void     gtk_multiline_tab_add               (GtkContainer *, GtkWidget *);
static void     gtk_multiline_tab_remove            (GtkContainer *, GtkWidget *);

static void     gtk_multiline_tab_label_add         (GtkNotebook *, GtkWidget *, guint, gpointer);
static void     gtk_multiline_tab_label_remove      (GtkNotebook *, GtkWidget *, guint, gpointer);
static gboolean gtk_multiline_tab_label_change      (GtkNotebook *, gint, gpointer);
static gboolean gtk_multiline_tab_label_clicked     (GtkWidget *, GdkEvent *, gpointer);

static GtkContainerClass *parent_class = NULL;

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

        multiline_tab_type = g_type_register_static (GTK_TYPE_CONTAINER, "GtkMultilineTab", &multiline_tab_info, 0);
    }

  return multiline_tab_type;
}

static void 
gtk_multiline_tab_class_init (GtkMultilineTabClass *the_class)
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    object_class = (GtkObjectClass*) the_class;
    widget_class = (GtkWidgetClass*) the_class;
    container_class = (GtkContainerClass*) the_class;

    parent_class = gtk_type_class (gtk_container_get_type ());

    object_class->destroy = gtk_multiline_tab_destroy;

/*  widget_class->realize = gtk_multiline_tab_realize;    */
    widget_class->expose_event = gtk_multiline_tab_expose;
    widget_class->size_request = gtk_multiline_tab_size_request;
    widget_class->size_allocate = gtk_multiline_tab_size_allocate;
    widget_class->map = gtk_multiline_tab_map;
    widget_class->unmap = gtk_multiline_tab_unmap;
/*  widget_class->draw = gtk_multiline_tab_draw;    */
    widget_class->button_press_event = gtk_multiline_tab_button_press;
    widget_class->button_release_event = gtk_multiline_tab_button_release;
    widget_class->motion_notify_event = gtk_multiline_tab_motion_notify;
    
    container_class->add = gtk_multiline_tab_add;
    container_class->remove = gtk_multiline_tab_remove;
}

static void 
gtk_multiline_tab_init (GtkMultilineTab *multiline_tab)
{
    multiline_tab->notebook = NULL;
    multiline_tab->children = (GList*) NULL;
    multiline_tab->tab_height = 0;

    gtk_widget_set_has_window (GTK_WIDGET (multiline_tab), FALSE);
}

static void
gtk_multiline_tab_add (GtkContainer *container, GtkWidget *widget)
{
    GtkMultilineTab *multiline_tab;
    GList *last;

    g_return_if_fail (container != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (container));
    g_return_if_fail (widget != NULL);
    g_return_if_fail (widget->parent == NULL);

    multiline_tab = GTK_MULTILINE_TAB (container);
    
    last = g_list_last (multiline_tab->children);
    if (last && last->data == multiline_tab->notebook) 
    {
        multiline_tab->children = g_list_insert_before (multiline_tab->children, last, widget);
    }
    else 
    {
        multiline_tab->children = g_list_append (multiline_tab->children, widget);
    }
    
    gtk_widget_set_parent (widget, GTK_WIDGET (container));
  
    if (gtk_widget_get_realized (GTK_WIDGET (container))) 
    {
        gtk_widget_realize (widget);
    }

    if (gtk_widget_get_visible (GTK_WIDGET (container)) && gtk_widget_get_visible (widget)) 
    {
        if (gtk_widget_get_mapped (GTK_WIDGET (container))) 
        {
            gtk_widget_map (widget);
        }

        gtk_widget_queue_resize (widget);
    }
}

static void
gtk_multiline_tab_remove (GtkContainer *container, GtkWidget *widget)
{
    GtkMultilineTab *multiline_tab;
    GList *children;
    GtkWidget *child;

    g_return_if_fail (container != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (container));
    g_return_if_fail (widget != NULL);

    multiline_tab = GTK_MULTILINE_TAB (container);

    for (children = multiline_tab->children; children; children = children->next) 
    {
        child = children->data;

        if (child == widget) 
        {
            gboolean was_visible;

	        was_visible = GTK_WIDGET_VISIBLE (widget);
	        gtk_widget_unparent (widget);

            multiline_tab->children = g_list_remove_link (multiline_tab->children, children);
            g_list_free (children);

            /* queue resize regardless of GTK_WIDGET_VISIBLE (container) */
            if (was_visible) 
            {
                gtk_widget_queue_resize (GTK_WIDGET (container));
            }

            break;
        }
    }
}

GtkWidget*
gtk_multiline_tab_new (GtkNotebook *notebook)
{
    GtkMultilineTab *multiline_tab;
    guint npages, page;
    GtkWidget *child;
    GtkWidget *label;
    const gchar *text;

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

    npages = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook));

    for (page = 0; page < npages; page++) 
    {
        child = gtk_notebook_get_nth_page (notebook, page);
        label = gtk_notebook_get_tab_label (notebook, child);
        if (label) 
        {
            /* reparent */
            g_object_ref (label);
            gtk_container_remove (GTK_CONTAINER (notebook), label);
            gtk_container_add (GTK_CONTAINER (multiline_tab), label);
            g_object_unref (label);
        }
        else 
        {
            text = gtk_notebook_get_tab_label_text (notebook, child);
            /* text = g_strdup_printf ("Page %d", page); */
            label = gtk_label_new (text);
            gtk_container_add (GTK_CONTAINER (multiline_tab), label);
        }
        
        g_signal_connect (GTK_OBJECT (label), "button-press-event",
            GTK_SIGNAL_FUNC (gtk_multiline_tab_label_clicked),
            (gpointer) multiline_tab);
    }
    
    /* hide the original tabs */
    gtk_notebook_set_show_tabs (notebook, FALSE);
    
    gtk_container_add (GTK_CONTAINER (multiline_tab), GTK_WIDGET (notebook));

    return GTK_WIDGET (multiline_tab);
}

static void
gtk_multiline_tab_destroy (GtkObject *object)
{
    GtkMultilineTab *multiline_tab;
    GList* children;

    g_return_if_fail (object != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (object));

    multiline_tab = GTK_MULTILINE_TAB (object);

    if (multiline_tab->children) 
    {
        for (children = multiline_tab->children; children; children = children->next) 
        {
            g_object_unref (children->data);
        }

        g_list_free (multiline_tab->children);
    }
    
    if (GTK_OBJECT_CLASS (parent_class)->destroy) 
    {
        (* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
    }
}

static void
gtk_multiline_tab_realize (GtkWidget *widget)
{
    GtkMultilineTab *multiline_tab;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GList *children;
    GtkWidget *child;

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

    multiline_tab = GTK_MULTILINE_TAB (widget);

    for (children = multiline_tab->children; children; children = children->next) 
    {
        child = children->data;

        if (!gtk_widget_get_realized (child))
        {
            gtk_widget_realize (child);
        }
    }
}

static void
gtk_multiline_tab_label_add (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer data)
{
    GtkMultilineTab *multiline_tab;
    GtkWidget *label;
    const gchar *text;
    
    multiline_tab = GTK_MULTILINE_TAB (data);
    label = gtk_notebook_get_tab_label (notebook, child);

    if (label) 
    {
        /* reparent */
        g_object_ref (label);
        gtk_container_remove (GTK_CONTAINER (notebook), label);
        gtk_container_add (GTK_CONTAINER (multiline_tab), label);
        g_object_unref (label);
    }
    else 
    {
        text = gtk_notebook_get_tab_label_text (notebook, child);
        /* text = g_strdup_printf ("Page %d", page); */
        label = gtk_label_new (text);
        gtk_container_add (GTK_CONTAINER (multiline_tab), label);
    }
        
    g_signal_connect (GTK_OBJECT (label), "button-press-event",
        GTK_SIGNAL_FUNC (gtk_multiline_tab_label_clicked),
        (gpointer) multiline_tab);
}

static void
gtk_multiline_tab_label_remove (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer data)
{
    GtkWidget *label;
    
    label = gtk_notebook_get_tab_label (notebook, child);
    gtk_container_remove (GTK_CONTAINER (data), label);
}

static gboolean 
gtk_multiline_tab_label_change (GtkNotebook *notebook, gint arg1, gpointer data)
{
    gtk_widget_queue_draw (GTK_WIDGET (data));
}

static gboolean 
gtk_multiline_tab_label_clicked (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    GtkMultilineTab *multiline_tab;
    gint page;

    multiline_tab = GTK_MULTILINE_TAB (data);
    page = g_list_index (multiline_tab->children, widget);
    gtk_notebook_set_current_page (multiline_tab->notebook, page);
    
    gtk_widget_queue_draw (GTK_WIDGET (data));
    
    return FALSE;
}

static void 
gtk_multiline_tab_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
    GtkMultilineTab *multiline_tab;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (widget));

    multiline_tab = GTK_MULTILINE_TAB (widget);
    requisition->width  = MULTILINE_TAB_DEFAULT_SIZE;
    requisition->height = MULTILINE_TAB_DEFAULT_SIZE;
}

static void
gtk_multiline_tab_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
    GtkMultilineTab *multiline_tab;
    gint max_height;
    GList *labels;
    GtkWidget *label;
    gint x, y;
    GtkRequisition requisition;
    GdkRectangle rectangle;
    GtkAllocation notebook_allocation;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (widget));
    g_return_if_fail (allocation != NULL);

    multiline_tab = GTK_MULTILINE_TAB (widget);

    /* set GtkMultilineTab allocation */
    gtk_widget_set_allocation (widget, allocation);

    if (gtk_widget_get_realized (widget)) 
    {
/*      gdk_window_move_resize (widget->window,
            allocation->x, allocation->y,
            allocation->width, allocation->height); */

        /* set child allocations */
        
        x = HORIZONTAL_GAP;
        y = VERTICAL_GAP;
        
        max_height = 0;

        for (labels = multiline_tab->children; labels->data != GTK_WIDGET (multiline_tab->notebook); labels = labels->next) 
        {
            label = GTK_WIDGET (labels->data);
            
            max_height = MAX (max_height, requisition.height);

            if (x + requisition.width + HORIZONTAL_GAP < allocation->width) 
            {
                rectangle.x = x;
                x += requisition.width + HORIZONTAL_GAP;
            }
            else 
            {
                rectangle.x = HORIZONTAL_GAP;
                x = HORIZONTAL_GAP;
                y += max_height + VERTICAL_GAP;
                max_height = 0;
            }

            rectangle.y = y;
            rectangle.width  = requisition.width;
            rectangle.height = requisition.height;

            /* move the label */
            gtk_widget_size_allocate (label, &rectangle);
        }

        multiline_tab->tab_height = y + max_height + VERTICAL_GAP;
                
        /* move the notebook */
        notebook_allocation.x = 0;
        notebook_allocation.y = multiline_tab->tab_height;
        notebook_allocation.width  = allocation->width;
        notebook_allocation.height = allocation->height - multiline_tab->tab_height;
        gtk_widget_size_allocate (GTK_WIDGET (multiline_tab->notebook), &notebook_allocation);
    }
}

static void
gtk_multiline_tab_map (GtkWidget *widget)
{
    GtkMultilineTab *multiline_tab;
    GList *children;
    GtkWidget *child;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (widget));

    gtk_widget_set_mapped (widget, TRUE);
    multiline_tab = GTK_MULTILINE_TAB (widget);

    for (children = multiline_tab->children; children; children = children->next) 
    {
        child = children->data;

        if (gtk_widget_get_visible (child) && !gtk_widget_get_mapped (child)) 
        {
            gtk_widget_map (child);
        }
    }
}

static void
gtk_multiline_tab_unmap (GtkWidget *widget)
{
    GtkMultilineTab *multiline_tab;
    GList *children;
    GtkWidget *child;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (widget));

    gtk_widget_set_mapped (widget, FALSE);
    multiline_tab = GTK_MULTILINE_TAB (widget);

    for (children = multiline_tab->children; children; children = children->next) 
    {
        child = children->data;

        if (gtk_widget_get_visible (child) && gtk_widget_get_mapped (child)) 
        {
            gtk_widget_unmap (child);
        }
    }
}

static void
gtk_multiline_tab_draw (GtkWidget *widget, GdkRectangle *area)
{
    GtkMultilineTab *multiline_tab;
    GList *children;
    GtkWidget *child;
    GdkRectangle child_area;
  
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (widget));
   
    if (gtk_widget_is_drawable (widget)) 
    {
      multiline_tab = GTK_MULTILINE_TAB (widget);
	
        for (children = multiline_tab->children; children; children = children->next) 
        {
            child = children->data;

            if (gtk_widget_is_drawable (child) && gtk_widget_intersect (child, area, &child_area)) 
            {
                gtk_widget_draw (child, &child_area);
            }
        }
    }
}

static gboolean
gtk_multiline_tab_expose (GtkWidget *widget, GdkEventExpose *event)
{
    GtkMultilineTab *multiline_tab;
    GdkEventExpose child_event;
    GList *children;
    GtkWidget *child;
    GtkStateType state_type;
    guint current_page;
    gint i;
    
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_MULTILINE_TAB (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    if (gtk_widget_is_drawable (widget)) 
    {
        multiline_tab = GTK_MULTILINE_TAB (widget);
        current_page = gtk_notebook_get_current_page (multiline_tab->notebook);

        child_event = *event;

        for (i = 0, children = multiline_tab->children; children; children = children->next) 
        {
            child = children->data;

            if (gtk_widget_is_drawable (child) && !gtk_widget_get_has_window (child) &&
                gtk_widget_intersect (child, &event->area, &child_event.area))
            {
                gtk_widget_event (child, (GdkEvent*) &child_event);
            }
        
            state_type = (i == current_page ? GTK_STATE_ACTIVE : GTK_STATE_NORMAL);
        
            gtk_paint_extension (widget->style, widget->window,
                state_type, GTK_SHADOW_OUT,
                &event->area, widget, "tab",
                child->allocation.x - 1, child->allocation.y - 1,
                child->allocation.width + 1, child->allocation.height + 1,
                GTK_POS_BOTTOM);
        }
    }
    
    return FALSE;
}

static gboolean
gtk_multiline_tab_button_press (GtkWidget *widget, GdkEventButton *event)
{
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
