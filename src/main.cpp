#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>

struct Memory_Structs{ 
    char* memory; // The data we write
    size_t size; // how much of the data we write
};

Memory_Structs memory_struct;

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp);

int main(){
    curl_global_init(CURL_GLOBAL_ALL); // initialise curl with all flags enabled
    
    CURL* curl = curl_easy_init(); // init curl easy handle
    curl_easy_setopt(curl, CURLOPT_URL, "https://zeppuzzle.lennlepez.workers.dev/"); // test site
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // The function with which we store the data
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, memory_struct); 
    CURLcode result = curl_easy_perform(curl);


    curl_global_cleanup();
    return 0;
}

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp){

    size_t realsize = size*nmemb; // Filesize = size of data * Number of memory bytes
    Memory_Structs *mem = (Memory_Structs*)userp; // Makes userp where we point to

    char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1); // Allocates memory for memory with hopefully enough size
    if(!ptr){
        // Not enough/ran out of memory
        std::cout << "Not enough memory (Realloc returned NULL)";
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), buffer, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}
