#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_URL_LENGTH 1024
#define MAX_COMMAND_LENGTH 2048

void downloadFile(char *url) {
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "wget -q \"%s\"", url);
    system(command);
}

int main() {
    char searchQuery[MAX_URL_LENGTH];
    printf("Enter search query: ");
    fgets(searchQuery, sizeof(searchQuery), stdin);
    searchQuery[strcspn(searchQuery, "\n")] = 0; // Remove newline character

    // Format the search query to be URL friendly
    char formattedSearchQuery[MAX_URL_LENGTH];
    sprintf(formattedSearchQuery, "\"%s\"", searchQuery);

    // Construct the API URL
    char apiUrl[MAX_URL_LENGTH];
    sprintf(apiUrl, "https://e621.net/posts.json?limit=320&tags=%s", formattedSearchQuery);

    // Fetch post information from API
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "curl -s \"%s\"", apiUrl);
    FILE *apiOutput = popen(command, "r");

    char postInfo[MAX_COMMAND_LENGTH];
    char postURL[MAX_URL_LENGTH];
    while (fgets(postInfo, sizeof(postInfo), apiOutput) != NULL) {
        // Parse the post URL from the API response
        char *start = strstr(postInfo, "\"file\":{\"url\":\"");
        if (start != NULL) {
            start += strlen("\"file\":{\"url\":\"");
            char *end = strstr(start, "\"");
            if (end != NULL) {
                *end = '\0'; // Terminate the URL string
                strcpy(postURL, start);
                downloadFile(postURL);
            }
        }
    }
    pclose(apiOutput);

    // Create a folder with the same name as the search query and move downloaded files into it
    char folderName[MAX_URL_LENGTH];
    sprintf(folderName, "%s", searchQuery);
    mkdir(folderName, 0777); // Create folder with read/write/execute permissions for owner, group, and others

    // Move downloaded files into the folder
    system("mv *.jpg *.png *.gif *.jpeg *.webp *.mp4 *.webm *.swf *.zip *.rar *.bmp *.txt *.pdf *.doc *.docx *.ppt *.pptx *.psd *.ai *.fla *.mp3 *.ogg *.wav *.midi *.swf *.svg *.xcf *.7z *.gz *.tgz *.tar *.bz2 *.xz *.lzh *.lha *.ace *.cbr *.cbz *.apk *.deb *.iso *.rpm *.torrent *.flac *.aac *.mkv *.m4a *.m4v *.mov *.bin *.crx *.exe *.jar *.bat *.sh *.ps *.e6x *.partial* *.part* %s/", folderName);

    printf("Download completed!\n");
    return 0;
}
