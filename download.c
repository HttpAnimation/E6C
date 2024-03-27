#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __APPLE__
#include <unistd.h>
#define DOWNLOAD_COMMAND "curl -O"
#else
#define DOWNLOAD_COMMAND "wget -P"
#endif

#define MAX_URL_LENGTH 1024

void create_folder(char *folder_name) {
    char command[256];
    snprintf(command, sizeof(command), "mkdir %s", folder_name);
    system(command);
}

void download_url(char *url, char *folder_name) {
    char command[MAX_URL_LENGTH + 64];
    snprintf(command, sizeof(command), "%s \"%s\"", DOWNLOAD_COMMAND, url);
    system(command);
}

void download_urls_from_file(char *filename, int home_dir) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }

    // Extract filename without extension to use as folder name
    char *filename_without_extension = strrchr(filename, '/');
    if (filename_without_extension == NULL)
        filename_without_extension = filename;
    else
        filename_without_extension++;

    char *dot = strrchr(filename_without_extension, '.');
    if (dot != NULL)
        *dot = '\0';

    if (home_dir) {
#ifdef __APPLE__
        char *home = getenv("HOME");
        if (home != NULL)
            snprintf(filename_without_extension, strlen(home) + 1, "%s", home);
        else {
            fprintf(stderr, "Error: Unable to get home directory.\n");
            exit(1);
        }
#else
        snprintf(filename_without_extension, 3, "~");
#endif
    }

    create_folder(filename_without_extension);

    char url[MAX_URL_LENGTH];
    while (fgets(url, sizeof(url), file) != NULL) {
        // Remove newline character if present
        size_t len = strlen(url);
        if (url[len - 1] == '\n')
            url[len - 1] = '\0';

        // Download URL
        download_url(url, filename_without_extension);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <file_containing_urls> [-h]\n", argv[0]);
        return 1;
    }

    int home_dir = 0;
    if (argc == 3 && strcmp(argv[2], "-h") == 0) {
        home_dir = 1;
#ifdef __APPLE__
        printf("Downloading to the home directory.\n");
#else
        printf("Home directory option is not supported on this platform.\n");
        return 1;
#endif
    }

#ifdef __APPLE__
    printf("Using curl for downloading on macOS.\n");
#else
    printf("Using wget for downloading on non-macOS systems.\n");
#endif

    printf("Compile command: gcc -o downloader %s -std=c11\n", argv[0]);
    download_urls_from_file(argv[1], home_dir);

    return 0;
}
