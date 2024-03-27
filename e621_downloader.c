#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

// Callback function to write received data into a string
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data) {
    size_t realsize = size * nmemb;
    memcpy(data + strlen(data), ptr, realsize);
    return realsize;
}

// Function to perform HTTP GET request and retrieve JSON response
char *http_get_request(const char *url) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize curl\n");
        return NULL;
    }

    char *response = malloc(1);
    if (!response) {
        fprintf(stderr, "Memory allocation failed\n");
        curl_easy_cleanup(curl);
        return NULL;
    }

    response[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Failed to perform curl request: %s\n", curl_easy_strerror(res));
        free(response);
        curl_easy_cleanup(curl);
        return NULL;
    }

    curl_easy_cleanup(curl);

    return response;
}

// Function to download a file
int download_file(const char *url, const char *filename) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize curl\n");
        return 0;
    }

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Failed to open file for writing\n");
        curl_easy_cleanup(curl);
        return 0;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Failed to download file: %s\n", curl_easy_strerror(res));
        fclose(fp);
        curl_easy_cleanup(curl);
        return 0;
    }

    fclose(fp);
    curl_easy_cleanup(curl);

    return 1;
}

// Function to parse JSON response and download images
void process_response(const char *json_response) {
    json_t *root;
    json_error_t error;

    root = json_loads(json_response, 0, &error);
    if (!root) {
        fprintf(stderr, "JSON parsing error on line %d: %s\n", error.line, error.text);
        return;
    }

    // Iterate through posts and download images
    size_t i;
    json_t *posts = json_object_get(root, "posts");
    if (json_is_array(posts)) {
        for (i = 0; i < json_array_size(posts); i++) {
            json_t *post = json_array_get(posts, i);
            json_t *file = json_object_get(post, "file");
            const char *url = json_string_value(json_object_get(file, "url"));

            // Download the image
            char filename[256];
            snprintf(filename, sizeof(filename), "%lu.jpg", (unsigned long)time(NULL)); // Naming with timestamp
            if (download_file(url, filename)) {
                printf("Downloaded: %s\n", filename);
            } else {
                fprintf(stderr, "Failed to download: %s\n", url);
            }
        }
    }

    json_decref(root);
}

int main() {
    const char *search_query = "protogen femboy -female rating:safe";
    const char *api_endpoint = "https://e621.net/posts.json";
    char url[512];

    // Construct the URL
    snprintf(url, sizeof(url), "%s?tags=%s", api_endpoint, search_query);

    // Perform HTTP GET request
    char *response = http_get_request(url);
    if (!response) {
        fprintf(stderr, "Failed to fetch data from %s\n", url);
        return 1;
    }

    // Process JSON response and download images
    process_response(response);

    // Clean up
    free(response);

    return 0;
}
