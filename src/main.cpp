#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>

struct Memory_Structs{ 
    char* memory = nullptr; // The data we write
    size_t size = 0; // how much of the data we write
};

void search_for_links(GumboNode* node);
void search_for_script_links(GumboNode *node);

Memory_Structs memory_struct;

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp);

int main(){
    curl_global_init(CURL_GLOBAL_ALL); // initialise curl with all flags enabled
    
    CURL* curl = curl_easy_init(); // init curl easy handle
    curl_easy_setopt(curl, CURLOPT_URL, "https://zeppuzzle.lennlepez.workers.dev/"); // test site
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // The function with which we store the data
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &memory_struct); 
    CURLcode result = curl_easy_perform(curl);
    if(result == CURLE_OK && memory_struct.memory != nullptr){
        GumboOutput* output = gumbo_parse(memory_struct.memory);
        search_for_links(output->root);
        search_for_script_links(output->root);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
    }else{
        std::cerr << "CURL ERROR: " << curl_easy_strerror(result);
    }
    


    curl_global_cleanup();
    return 0;
}

void search_for_links(GumboNode *node){
    if(node->type != GUMBO_NODE_ELEMENT){
        return;
    }
    GumboAttribute* href;
    if(node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href"))){
        std::cout << href->value << '\n';
    }

    GumboVector* children = &node->v.element.children;
    for(unsigned int i = 0; i < children->length; i++){
        search_for_links(static_cast<GumboNode*>(children->data[i]));
    }
}

void search_for_script_links(GumboNode *node){
    if(node->type != GUMBO_NODE_ELEMENT){ // checks to see if the thing that is currently in the node is even a fucking node
        return; // NOTANODEGETTHEFUKCKOTUO
    }
    GumboAttribute* scripts; 

    if(node->v.element.tag == GUMBO_TAG_SCRIPT && (scripts = gumbo_get_attribute(&node->v.element.attributes, "src"))){ // checks for if the things at that node is a script
        std::cout << scripts->value << '\n'; // YAAAYAYAYAYAYAYAYA IS A FUCJ9IBG 9P SCRIPT!!!!!!
    }

    GumboVector* children = &node->v.element.children; // Walk farther into the HTML5 tree
    for(unsigned int i = 0; i < children->length; i++){
        search_for_script_links(static_cast<GumboNode*>(children->data[i]));
    }
}

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp)
{

    size_t realsize = size*nmemb; // Filesize = size of data * Number of memory bytes
    Memory_Structs *mem = (Memory_Structs*)userp; // Makes userp where we point to

    char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1); // Allocates memory for memory with hopefully enough size
    if(!ptr){
        // Not enough/ran out of memory
        std::cerr << "Not enough memory (Realloc returned NULL)";
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), buffer, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}
