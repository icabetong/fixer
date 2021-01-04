#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

static void destroy(GtkWidget* widget, gpointer data);
static void activate(GtkApplication* app, gpointer data);
static void choose_source(GtkWidget* widget, GtkLabel* widget_source);
static void choose_destination(GtkWidget* widget, GtkLabel* widget_destination);

static char* video_source;
static char* video_destination;

int find_index(char* str, char target) {
    int index = -1;

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == target)
            index = i;
    }
    return index + 2;
}

const char* get_filename(char* source) {
    int index = 0;
    int length = strlen(source);
    char* destination = malloc(sizeof(char) * 255); // 255 is the max char limit for filenames in ntfs and ext4

    const char target_index = find_index(source, '/');
    int i;

    while (index < length) {
        destination[index] = source[target_index + index - 1];
        index++;
    }
    destination[index] = '\0';

    return destination;
}

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

    gtk_label_set_xalign(GTK_LABEL(widget_source), 0);
    gtk_label_set_xalign(GTK_LABEL(widget_destination), 0);
    gtk_label_set_xalign(GTK_LABEL(widget_output_hint), 0);
    gtk_label_set_xalign(GTK_LABEL(widget_output), 0);
    gtk_label_set_yalign(GTK_LABEL(widget_set_source), 0);
    gtk_label_set_yalign(GTK_LABEL(widget_set_destination), 0);

    // Define Grid Attributes
    gtk_grid_set_row_spacing(GTK_GRID(root), 8);
    gtk_grid_set_column_spacing(GTK_GRID(root), 8);
    gtk_grid_set_row_homogeneous(GTK_GRID(root), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(root), TRUE);

    // Attach the widgets
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
    g_signal_connect(G_OBJECT(widget_set_source), "clicked", G_CALLBACK(choose_source), widget_source);
    g_signal_connect(G_OBJECT(widget_set_destination), "clicked", G_CALLBACK(choose_destination), widget_destination);

    gtk_widget_show_all(window);
}

static void choose_source(GtkWidget* widget, GtkLabel* widget_source) {
    GtkWindow* window = GTK_WINDOW(widget);

    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Choose Video Source", window, action, 
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        video_source = gtk_file_chooser_get_filename(chooser);

        gtk_label_set_text(GTK_LABEL(widget_source), video_source);
    }

    g_object_unref(dialog);
}

static void choose_destination(GtkWidget* widget, GtkLabel* widget_destination) {
    GtkWindow* window = GTK_WINDOW(widget);

    GtkWidget* dialog;  
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Choose Video Destination", window, action, 
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
    /*
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), default_folder_for_saving);
    */
    gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), get_filename(video_source));
    
    res = gtk_dialog_run(GTK_DIALOG(dialog));    

    if (res == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        video_destination = gtk_file_chooser_get_filename(chooser);

        gtk_label_set_text(GTK_LABEL(widget_destination), video_destination);
    }
    
    g_object_unref(dialog);
}