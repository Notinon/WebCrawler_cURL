#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>

struct Memory_Structs{
    char* memory;
    size_t size;
};

Memory_Structs memory_struct;

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp);

int main(){
    curl_global_init(CURL_GLOBAL_ALL);
    
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "https://zeppuzzle.lennlepez.workers.dev/");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, memory_struct);
    CURLcode result = curl_easy_perform(curl);


    curl_global_cleanup();
    return 0;
}

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp){

    size_t realsize = size*nmemb;
    Memory_Structs *mem = (Memory_Structs*)userp;

    char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
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
