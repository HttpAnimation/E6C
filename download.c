#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_URL_LENGTH 1024

void create_folder(char *folder_name) {
    char command[256];
    snprintf(command, sizeof(command), "mkdir %s", folder_name);
    system(command);
}

void wget_url(char *url, char *folder_name) {
    char command[MAX_URL_LENGTH + 64];
    snprintf(command, sizeof(command), "wget -P %s \"%s\"", folder_name, url);
    system(command);
}

void wget_urls_from_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }

    char folder_name[20];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(folder_name, sizeof(folder_name), "%Y-%m-%d_%H-%M-%S", tm);
    create_folder(folder_name);

    char url[MAX_URL_LENGTH];
    while (fgets(url, sizeof(url), file) != NULL) {
        // Remove newline character if present
        size_t len = strlen(url);
        if (url[len - 1] == '\n')
            url[len - 1] = '\0';

        // Download URL
        wget_url(url, folder_name);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_containing_urls>\n", argv[0]);
        return 1;
    }

    printf("Compile command: gcc -o downloader %s -std=c11\n", argv[0]);
    while (1) {
        wget_urls_from_file(argv[1]);
    }

    return 0;
}
