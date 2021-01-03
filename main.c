#include <gtk/gtk.h>

static void destroy(GtkWidget* widget, gpointer data);
static void activate(GtkApplication* app, gpointer data);

int main(int argc, char* argv[]) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("me.asayah.fixer", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

static void destroy(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget* window;
    GtkWidget* root;
    GtkWidget* widget_header;
    GtkWidget* widget_source;
    GtkWidget* widget_destination;
    GtkWidget* widget_set_source;
    GtkWidget* widget_set_destination;
    GtkWidget* widget_spacer;
    GtkWidget* widget_output_hint;
    GtkWidget* widget_output;
    GtkWidget* widget_start;
    
    window = gtk_application_window_new(app);
    root = gtk_grid_new();
    widget_header = gtk_label_new("");
    widget_source = gtk_label_new("No Source Set");
    widget_destination = gtk_label_new("No Destination Set");
    widget_set_source = gtk_button_new_with_label("Set Source");
    widget_set_destination = gtk_button_new_with_label("Set Destination");
    widget_spacer = gtk_label_new("");
    widget_output_hint = gtk_label_new("Command Output");
    widget_output = gtk_label_new("Process Ready");
    widget_start = gtk_button_new_with_label("Start");

    gtk_grid_attach(GTK_GRID(root), widget_header, 1, 1, 1, 1);
    gtk_grid_attach_next_to(GTK_GRID(root), widget_source, widget_header, GTK_POS_BOTTOM, 2, 1);
    gtk_grid_attach_next_to(GTK_GRID(root), widget_destination, widget_source, GTK_POS_BOTTOM, 2, 1);
    gtk_grid_attach_next_to(GTK_GRID(root), widget_set_source, widget_source, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to(GTK_GRID(root), widget_set_destination, widget_destination, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to(GTK_GRID(root), widget_spacer, widget_destination, GTK_POS_BOTTOM, 3, 1);
    gtk_grid_attach_next_to(GTK_GRID(root), widget_output_hint, widget_spacer, GTK_POS_BOTTOM, 3, 1);
    gtk_grid_attach_next_to(GTK_GRID(root), widget_output, widget_output_hint, GTK_POS_BOTTOM, 3, 1);
    gtk_grid_attach_next_to(GTK_GRID(root), widget_start, widget_output, GTK_POS_BOTTOM, 3, 1);

    gtk_window_set_title(GTK_WINDOW(window), "Fixer");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 300);

    gtk_container_add(GTK_CONTAINER(window), root);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);

    gtk_widget_show_all(window);
}