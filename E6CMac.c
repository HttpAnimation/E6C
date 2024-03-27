#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define API_URL "https://e621.net/posts.json"
#define USER_AGENT "YourUserAgent/1.0 (by YourUsername on e621)"
#define API_KEY "YourAPIKeyHere"

// Struct to store response data from libcurl
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write response data from libcurl
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        // Out of memory
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Function to perform GET request using libcurl
CURLcode performGetRequest(const char *url, struct MemoryStruct *chunk, const char *authHeader) {
    CURL *curl_handle;
    CURLcode res;

    curl_handle = curl_easy_init();
    if(curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);

        // Set user agent and authorization header
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, USER_AGENT);
        if (authHeader)
            headers = curl_slist_append(headers, authHeader);
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        res = curl_easy_perform(curl_handle);

        // Clean up
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);

        return res;
    }

    return CURLE_FAILED_INIT;
}

// Function to perform a search and download posts
void searchAndDownloadPosts(const char *query) {
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1); // Will be grown as needed by realloc
    chunk.size = 0;           // No data at this point

    char apiUrl[500];
    sprintf(apiUrl, "%s?tags=%s", API_URL, query);

    // Perform the GET request
    res = performGetRequest(apiUrl, &chunk, NULL);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    else {
        // Process the response
        // Here you would parse the JSON response and extract the URLs of the images
        // Then you can use libcurl again to download each image

        // For demonstration purposes, let's just print the response
        printf("Response:\n%s\n", chunk.memory);
    }

    free(chunk.memory);
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <search query>\n", argv[0]);
        return 1;
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Perform search and download posts
    searchAndDownloadPosts(argv[1]);

    // Cleanup libcurl
    curl_global_cleanup();

    return 0;
}
