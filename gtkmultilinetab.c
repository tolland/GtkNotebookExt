#include <gtk/gtk.h>

#include "gtkmultilinetab.h"

#define HORIZONTAL_GAP  0
#define VERTICAL_GAP    0

#define MULTILINE_TAB_DEFAULT_SIZE  100

static void     gtk_multiline_tab_class_init        (GtkMultilineTabClass *);
static void     gtk_multiline_tab_init              (GtkMultilineTab *);
static void     gtk_multiline_tab_destroy           (GtkObject *);
static GObject *g_object_clone(GObject *src);
/*** GtkWidget class methods ***/
static void     gtk_multiline_tab_realize           (GtkWidget *);
static void     gtk_multiline_tab_size_request      (GtkWidget *, GtkRequisition *);
static void     gtk_multiline_tab_size_allocate     (GtkWidget *, GtkAllocation *);
static gboolean gtk_multiline_tab_expose            (GtkWidget *, GdkEventExpose *);
static gboolean gtk_multiline_tab_button_press      (GtkWidget *, GdkEventButton *);
static gboolean gtk_multiline_tab_button_release    (GtkWidget *, GdkEventButton *);
static gboolean gtk_multiline_tab_motion_notify     (GtkWidget *, GdkEventMotion *);

static void     gtk_multiline_tab_page_add          (GtkNotebook *, GtkWidget *, guint, gpointer);
static void     gtk_multiline_tab_page_remove       (GtkNotebook *, GtkWidget *, guint, gpointer);
static gboolean gtk_multiline_tab_page_change       (GtkNotebook *, gint, gpointer);

static void     gtk_multiline_tab_realloc_size      (GtkWidget *, GdkRectangle *, gpointer);

/*** signal handlers emitting signals for original tabs ***/
static  gboolean    tab_press       (GtkWidget *, GdkEventButton *, gpointer);  /* "button-press-event" signal handler for the entire tab label widget*/
static  gboolean    tab_release     (GtkWidget *, GdkEventButton *, gpointer);  /* "button-release-event" signal handler for the entire tab label widget*/
static  gboolean    close_press     (GtkWidget *, GdkEventButton *, gpointer);  /* "button-press-event" signal handler for the label's close button widget*/
static  void        close_clicked   (GtkButton *, gpointer);                    /* "clicked" signal handler for the label's close button widget*/

/*** helper for actually creating (cloning) the tab label widget ***/
static  GtkWidget*  gtk_multiline_tab_create_label (GtkWidget *);

static GtkVBoxClass *parent_class = NULL;
static void gtk_multiline_get_tab_size(GtkMultilineTab* tab, gint pos, GtkAllocation* rectangle);
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

        multiline_tab_type = g_type_register_static (GTK_TYPE_BOX, "GtkMultilineTab", &multiline_tab_info, 0);
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

    parent_class = gtk_type_class (gtk_box_get_type ());

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

    gtk_widget_set_has_window (GTK_WIDGET (multiline_tab), TRUE);
    gtk_container_set_border_width (GTK_CONTAINER (multiline_tab), 0);
}

GtkWidget*
gtk_multiline_tab_new (GtkNotebook *notebook)
{
    GtkMultilineTab *multiline_tab;
    GtkWidget *child;
    GtkWidget *label;
    GtkWidget *tab;
    guint npages;
    guint i;

    g_return_if_fail (notebook != NULL);
    g_return_if_fail (GTK_IS_NOTEBOOK (notebook));
    
    multiline_tab = g_object_new (gtk_multiline_tab_get_type (), NULL);

    /* create children based on the labels of the notebook */
    npages = gtk_notebook_get_n_pages (notebook);
    for (i = 0; i < npages; i++)
    {
        child = gtk_notebook_get_nth_page (notebook, i);
        label = gtk_notebook_get_tab_label (notebook, child);

        tab = gtk_multiline_tab_create_label (label);
        gtk_container_add (GTK_CONTAINER (multiline_tab), tab);
    }

    g_signal_connect (GTK_OBJECT (notebook), "page-added",
        GTK_SIGNAL_FUNC (gtk_multiline_tab_page_add),
        (gpointer) multiline_tab);
    g_signal_connect (GTK_OBJECT (notebook), "page-removed",
        GTK_SIGNAL_FUNC (gtk_multiline_tab_page_remove),
        (gpointer) multiline_tab);
    g_signal_connect (GTK_OBJECT (notebook), "change-current-page",
        GTK_SIGNAL_FUNC (gtk_multiline_tab_page_change),
        (gpointer) multiline_tab);

    /* save the essential notebook widget (pointer) */
    multiline_tab->notebook = notebook;

    return GTK_WIDGET (multiline_tab);
}

static void
gtk_multiline_tab_destroy (GtkObject *object)
{
    GtkMultilineTab *multiline_tab;

    g_return_if_fail (object != NULL);
    g_return_if_fail (GTK_IS_MULTILINE_TAB (object));

    multiline_tab = GTK_MULTILINE_TAB (object);

    if (GTK_OBJECT_CLASS (parent_class)->destroy) 
        (* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static guint handler_id;

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

    if (!g_signal_handler_is_connected (widget->parent, handler_id))
        handler_id = g_signal_connect_after (GTK_OBJECT (widget->parent), "size-allocate",
            GTK_SIGNAL_FUNC (gtk_multiline_tab_realloc_size), (gpointer) widget);
}

static  GtkWidget*  
gtk_multiline_tab_create_label (GtkWidget *label)
{

    GtkWidget* lblCloned = (GtkWidget*)g_object_clone((GObject*)label);
    GList *children;
    GtkWidget *ebox;
    GtkWidget *hbox;
    GtkWidget *align;
    GtkWidget *btn;
    GtkWidget *image;
    GtkWidget *tab_label;
   // GtkWidget *close_button;


    if (GTK_IS_EVENT_BOX (label))
    {
        children = gtk_container_get_children (GTK_CONTAINER (label));
        hbox = children->data;
        
        if (GTK_IS_HBOX (hbox))
        {
            children = gtk_container_get_children (GTK_CONTAINER (hbox));
            tab_label = children->data;
            align = children->next->data;
            
            if (GTK_IS_LABEL (tab_label))
                tab_label = gtk_label_new (gtk_label_get_text (GTK_LABEL (tab_label)));
            
            if (GTK_IS_ALIGNMENT (align))
            {
                children = gtk_container_get_children (GTK_CONTAINER (align));
                btn = children->data;
            }
        }
    }

    ebox = gtk_event_box_new ();
    gtk_widget_set_has_window (ebox, FALSE);

    hbox = gtk_hbox_new (FALSE, 2);
    gtk_box_pack_start (GTK_BOX (hbox), lblCloned, FALSE, FALSE, 0);
    gtk_container_add (GTK_CONTAINER (ebox), hbox);

    /*close_button = gtk_button_new ();
    gtk_button_set_relief (GTK_BUTTON (close_button), GTK_RELIEF_NONE);
    gtk_button_set_focus_on_click (GTK_BUTTON (close_button), FALSE);
    gtk_widget_set_name (close_button, "geany-close-tab-button");*/

    /*image = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
    gtk_container_add (GTK_CONTAINER (close_button), image);*/

    //align = gtk_alignment_new (1.0, 0.5, 0.0, 0.0);
    //gtk_container_add(GTK_CONTAINER(hbox), label);
//    gtk_container_add (GTK_CONTAINER (align), close_button);
  //  gtk_box_pack_start (GTK_BOX (hbox), align, TRUE, TRUE, 0);

    /* establish the signal handlers chain */
    g_signal_connect (GTK_OBJECT (ebox), "button-press-event",
        GTK_SIGNAL_FUNC (tab_press), (gpointer) label);
    g_signal_connect (GTK_OBJECT (ebox), "button-release-event",
        GTK_SIGNAL_FUNC (tab_release), (gpointer) label);
    /*g_signal_connect (GTK_OBJECT (close_button), "button-press-event",
        GTK_SIGNAL_FUNC (close_press), (gpointer) btn);*/
    /*g_signal_connect (GTK_OBJECT (close_button), "clicked",
        GTK_SIGNAL_FUNC (close_clicked), (gpointer) btn);*/

    /* make the complex label widget visible */
    gtk_widget_show_all (ebox);
            
    return ebox;
}

static void
gtk_multiline_tab_page_add (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer data)
{
    GtkWidget *label;
    GtkWidget *tab;

    label = gtk_notebook_get_tab_label (notebook, child);

    /* create the child based on the label of the notebook */
    tab = gtk_multiline_tab_create_label (label);

    gtk_container_add (GTK_CONTAINER (data), tab);
    gtk_widget_queue_resize (GTK_WIDGET (data));
}

static void
gtk_multiline_tab_page_remove (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer data)
{
    GList *children;
    GtkWidget *tab;

    /* check if GtkMultilineTab container still exists */
    if (GTK_IS_CONTAINER (data))
    {
        children = gtk_container_get_children (GTK_CONTAINER (data));
        tab = g_list_nth_data (children, page_num);
        
        gtk_container_remove (GTK_CONTAINER (data), tab);
        gtk_widget_queue_resize (GTK_WIDGET (data));
    }
}

static gboolean 
gtk_multiline_tab_page_change (GtkNotebook *notebook, gint arg1, gpointer data)
{
    gtk_widget_queue_draw (GTK_WIDGET (data));
}

static  gboolean
tab_press (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GtkMultilineTab *multiline_tab;
    GList *children;
    guint i;
    gboolean stop;

    multiline_tab = GTK_MULTILINE_TAB (widget->parent);

    for (i = 0, children = gtk_container_get_children (GTK_CONTAINER (multiline_tab)); children; children = children->next, i++)
        if (widget == GTK_WIDGET (children->data))
            gtk_notebook_set_current_page (multiline_tab->notebook, i);

    g_signal_emit_by_name (G_OBJECT (data), "button-press-event", event, &stop);

    return FALSE;
}

static  gboolean
tab_release (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gboolean stop;

    g_signal_emit_by_name (G_OBJECT (data), "button-release-event", event, &stop);

    return FALSE;
}

static  gboolean
close_press (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gboolean stop;

    g_signal_emit_by_name (G_OBJECT (data), "button-press-event", event, &stop);

    return FALSE;
}

static void
close_clicked (GtkButton *button, gpointer data)
{
    g_signal_emit_by_name (G_OBJECT (data), "clicked");
}

static void
gtk_multiline_tab_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
    GtkMultilineTab *multiline_tab;
    GList *children;
    guint max_height;
    guint tab_height;
    gboolean first_line;
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
    first_line = TRUE;

    for (children = gtk_container_get_children (GTK_CONTAINER (multiline_tab)); children; children = children->next)
    {
        label = GTK_WIDGET (children->data);

        label_requisition.width  = 0;
        label_requisition.height = 0;

        if (gtk_widget_get_mapped (label))
            gtk_widget_size_request (label, &label_requisition);

        max_height = MAX (max_height, label_requisition.height);

        if (x + label_requisition.width + HORIZONTAL_GAP >= allocation.width && !first_line)
        {
            x = HORIZONTAL_GAP;
            y += max_height + VERTICAL_GAP;
            max_height = 0;
        }
        
        first_line = FALSE;

        x += label_requisition.width + HORIZONTAL_GAP;
    }

    /* no VERTICAL_GAP at the bottom */
    tab_height = y + (max_height ? max_height : label_requisition.height);

    requisition->width  = -1; /* "natural" size */
    requisition->height = tab_height;
}

static void
gtk_multiline_tab_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
    GtkMultilineTab *multiline_tab;
    GList *children;
    gint max_height;
    gboolean first_line;
    GtkWidget *label;
    GtkAllocation child_allocation;
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

        x = HORIZONTAL_GAP;
        y = VERTICAL_GAP;

        max_height = 0;
        first_line = TRUE;

        /* arrange the labels (child widgets) */
        for (children = gtk_container_get_children (GTK_CONTAINER (multiline_tab)); children; children = children->next)
        {
            label = GTK_WIDGET (children->data);
            gtk_widget_size_request (label, &requisition);

            max_height = MAX (max_height, requisition.height);

            if (x + requisition.width + HORIZONTAL_GAP >= allocation->width && !first_line)
            {
                x = HORIZONTAL_GAP;
                y += max_height + VERTICAL_GAP;
                max_height = 0;
            }

            first_line = FALSE;

            child_allocation.x = x;
            child_allocation.y = y;
            child_allocation.width  = requisition.width;
            child_allocation.height = requisition.height;

            gtk_widget_size_allocate (label, &child_allocation);

            x += requisition.width + HORIZONTAL_GAP;
        }
    }
}

static void
gtk_multiline_tab_realloc_size (GtkWidget *widget, GdkRectangle *allocation, gpointer data)
{
    GtkRequisition requisition;
    
    gtk_multiline_tab_size_request (GTK_WIDGET (data), &requisition);
    gtk_widget_set_size_request (GTK_WIDGET (data), requisition.width, requisition.height);

    /* GTK+ sometimes "forgets" about invalidating the tabs area */
    gtk_widget_queue_draw (GTK_WIDGET (data));
}

static gboolean
gtk_multiline_tab_expose (GtkWidget *widget, GdkEventExpose *event)
{
    GtkMultilineTab *multiline_tab;
    GList *children;
    GtkWidget *label;
    guint current_page;
    GtkStateType state_type;
    GtkAllocation allocation;
    gint i;

    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_MULTILINE_TAB (widget), FALSE);

    children = gtk_container_get_children (GTK_CONTAINER (widget));

    if (!gtk_widget_is_drawable (widget) || !children)
        return;

    multiline_tab = GTK_MULTILINE_TAB (widget);
    current_page = gtk_notebook_get_current_page (multiline_tab->notebook);

    for (i = 0; children; children = children->next, i++)
    {
        label = GTK_WIDGET (children->data);
        gtk_widget_get_allocation (label, &allocation);

        state_type = (i == current_page ? GTK_STATE_NORMAL : GTK_STATE_ACTIVE);
        gint verticalActiveGap = (i == current_page ? 0 : 3);

        gtk_paint_extension (GTK_WIDGET (multiline_tab->notebook)->style, 
            widget->window, state_type, GTK_SHADOW_OUT,
            &event->area, (GtkWidget*)multiline_tab->notebook, "tab",
            allocation.x - HORIZONTAL_GAP / 2, allocation.y - VERTICAL_GAP,
            allocation.width + HORIZONTAL_GAP, allocation.height + VERTICAL_GAP + verticalActiveGap+ 50,
                             gtk_notebook_get_tab_pos(multiline_tab->notebook));

    }
    printf("Childs count: %d\n\r", i);

    /* call GtkBox method implementation */
    (* GTK_WIDGET_CLASS (parent_class)->expose_event) (widget, event);

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

GObject *
g_object_clone(GObject *src)
{
    GObject *dst;
    GParameter *params;
    GParamSpec **specs;
    guint n, n_specs, n_params;

    specs = g_object_class_list_properties(G_OBJECT_GET_CLASS(src), &n_specs);
    params = g_new0(GParameter, n_specs);
    n_params = 0;

    for (n = 0; n < n_specs; ++n)
        if (strcmp(specs[n]->name, "parent") &&
            (specs[n]->flags & G_PARAM_READWRITE) == G_PARAM_READWRITE) {
            params[n_params].name = g_intern_string(specs[n]->name);
            g_value_init(&params[n_params].value, specs[n]->value_type);
            g_object_get_property(src, specs[n]->name, &params[n_params].value);
            ++ n_params;
        }

    dst = g_object_newv(G_TYPE_FROM_INSTANCE(src), n_params, params);
    g_free(specs);
    g_free(params);

    return dst;
}

void gtk_multiline_get_tab_size(GtkMultilineTab* tab, gint pos, GtkAllocation* rectangle)
{
    GtkMultilineTab *multiline_tab;
    GtkWidget* page;
    GtkWidget* label;

    multiline_tab = GTK_MULTILINE_TAB (tab);
    page= gtk_notebook_get_nth_page(multiline_tab->notebook, pos);
    label= gtk_notebook_get_tab_label(multiline_tab->notebook, page);
    gtk_widget_get_allocation(label, rectangle);
    rectangle->x = label->allocation.x;
    rectangle->y = label->allocation.y;
    rectangle->width = label->allocation.width;
    rectangle->height = page->allocation.y;
}
