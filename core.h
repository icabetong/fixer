void wrap(char* source, char* destination) {
    char* target = (char*) calloc(255, sizeof(char));
    strcpy(target, "'");

    strcat(target, source);
    strcat(target, "'");
    
    strcpy(destination, target);
}

int verify_packages(GtkWidget *window) {
    int package_installed = system("ffmpeg -h");
    GtkWidget *widget;

    widget = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_YES_NO, "ffmpeg package is not installed. Do you want to install it?");
    
    if (gtk_dialog_run(GTK_DIALOG(widget)) == GTK_RESPONSE_CLOSE) {
        gtk_widget_destroy(widget);
    }
    return;
}

int find_index(char* str, char target) {
    int index = -1;

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == target)
            index = i;
    }
    return index + 2;
}

int run(char* video_source, char* video_destination) {
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