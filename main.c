#include <gtk/gtk.h>
#include <syscall.h>
#include <stdio.h>
#include <string.h>

typedef struct MainWidgets {
    GtkWidget *window;
    GtkWidget *root;
    GtkWidget *widget_header;
    GtkWidget *widget_source;
    GtkWidget *widget_destination;
    GtkWidget *widget_set_source;
    GtkWidget *widget_set_destination;
    GtkWidget *widget_set_overwrite;
    GtkWidget *widget_spacer;
    GtkWidget *widget_output_hint;
    GtkWidget *widget_output;
    GtkWidget *widget_start;
} CommandArgs ;

static void onDestroy(GtkWidget* widget, gpointer data);
static void onActivate(GtkApplication* app, gpointer data);
static void onChooseSource(GtkWidget* widget, GtkLabel* widget_source);
static void onChooseDestination(GtkWidget* widget, GtkLabel* widget_destination);
static void onStartCommand(GtkWidget* widget, gpointer *data);

void wrap(char* source, char* destination) {
    char* target = (char*) calloc(255, sizeof(char));
    strcpy(target, "'");

    strcat(target, source);
    strcat(target, "'");
    
    strcpy(destination, target);
}

char* video_source = "";
char* video_destination = "";

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
    g_signal_connect(app, "activate", G_CALLBACK(onActivate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

static void onDestroy(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}

static void onActivate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *root;
    GtkWidget *widget_header;
    GtkWidget *widget_source;
    GtkWidget *widget_destination;
    GtkWidget *widget_set_source;
    GtkWidget *widget_set_destination;
    GtkWidget *widget_spacer;
    GtkWidget *widget_output_hint;
    GtkWidget *widget_output;
    GtkWidget *widget_start;
    
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
    gtk_container_set_border_width(GTK_CONTAINER(window), 16);

    CommandArgs *args = g_new0(CommandArgs, 1);
    args->window = window;
    args->root = root;
    args->widget_header = widget_header;
    args->widget_source = widget_source;
    args->widget_destination = widget_destination;
    args->widget_set_source = widget_set_source;
    args->widget_set_destination = widget_set_destination;
    args->widget_spacer = widget_spacer;
    args->widget_output_hint = widget_output_hint;
    args->widget_output = widget_output;

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(onDestroy), NULL);
    g_signal_connect(G_OBJECT(widget_set_source), "clicked", G_CALLBACK(onChooseSource), widget_source);
    g_signal_connect(G_OBJECT(widget_set_destination), "clicked", G_CALLBACK(onChooseDestination), widget_destination);
    g_signal_connect(G_OBJECT(widget_start), "clicked", G_CALLBACK(onStartCommand), args);

    gtk_widget_show_all(window);
}

static void onChooseSource(GtkWidget* widget, GtkLabel* widget_source) {
    GtkWindow* window = GTK_WINDOW(widget);

    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    GtkFileFilter* filter = gtk_file_filter_new();

    gtk_file_filter_set_name(GTK_FILE_FILTER(filter), "MPEG4 Video");
    gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "video/mp4");

    dialog = gtk_file_chooser_dialog_new("Choose Video Source", window, action, 
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        video_source = gtk_file_chooser_get_filename(chooser);

        gtk_label_set_text(GTK_LABEL(widget_source), video_source);
    }

    g_object_unref(dialog);
}

static void onChooseDestination(GtkWidget* widget, GtkLabel* widget_destination) {
    GtkWindow* window = GTK_WINDOW(widget);

    GtkWidget* dialog;  
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    GtkFileFilter* filter = gtk_file_filter_new();

    gtk_file_filter_set_name(GTK_FILE_FILTER(filter), "MPEG4 Video");
    gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "video/mp4");

    dialog = gtk_file_chooser_dialog_new("Choose Video Destination", window, action, 
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (video_source) {
        gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), get_filename(video_source));
    }
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        video_destination = gtk_file_chooser_get_filename(chooser);

        gtk_label_set_text(GTK_LABEL(widget_destination), video_destination);
    }
    
    g_object_unref(dialog);
}

static void onStartCommand(GtkWidget* widget, gpointer *data) {
    CommandArgs *args = (CommandArgs*) data;

    GtkWindow *window = GTK_WINDOW(args->window);
    GtkWidget *widget_header = GTK_WIDGET(args->widget_header);
    GtkWidget *widget_source = GTK_WIDGET(args->widget_source);
    GtkWidget *widget_destination = GTK_WIDGET(args->widget_destination);
    GtkWidget *widget_set_source = GTK_WIDGET(args->widget_set_source);
    GtkWidget *widget_set_destination = GTK_WIDGET(args->widget_set_destination);
    GtkWidget *widget_output_hint = GTK_WIDGET(args->widget_output_hint);
    GtkWidget *widget_output = GTK_WIDGET(args->widget_output);

    GtkWidget *dialog;
    char ch;
    FILE *_source, *_target;

    if (strlen(video_source) == 0 || video_source[0] == '\0') {
        dialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "The video source is empty");
    
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_CLOSE) {
            gtk_widget_destroy(dialog);
        }
        return;
    }

    if (strlen(video_destination) == 0 || video_destination[0] == '\0') {
        dialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "The video destination is empty"); 

        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_CLOSE) {
            gtk_widget_destroy(dialog);
        }
        return;
    }

    gtk_widget_set_sensitive(GTK_WIDGET(widget_header), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_source), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_destination), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_set_source), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_set_destination), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_output_hint), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_output), FALSE);
    
    char* first = "ffmpeg -y -i ";
    char* second = " -c:v libx264 -c:a aac -strict experimental -tune fastdecode -pix_fmt yuv420p -b:a 192k -ar 48000 ";
    char* command = calloc(1024, 1);

    char* source = calloc(255, 1);
    char* destination = calloc(255, 1);

    wrap(video_source, source);
    wrap(video_destination, destination);

    strcpy(command, first);
    strcat(command, source);
    strcat(command, second);
    strcat(command, destination);

    int status = system(command);
    if (status == 0) {
        dialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "The video is successfully fixed");

        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_CLOSE) {
            gtk_widget_destroy(dialog);
        }
    }

    gtk_widget_set_sensitive(GTK_WIDGET(widget_header), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_source), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_destination), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_set_source), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_set_destination), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_output_hint), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget_output), TRUE);
}