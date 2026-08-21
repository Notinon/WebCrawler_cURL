#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include <unordered_set>
#include <queue>
#include <string>
#include <thread> 
#include <chrono>
#include "robots.h"
#include <vector>

struct Memory_Structs{
    char* memory = nullptr; // The data we write
    size_t size = 0; // how much of the data we write
};

bool is_Crawlable(const std::string contents, const std::string user_agent, const std::string url_path);

void search_for_links(GumboNode* node, int parent_depth);
void search_for_script_links(GumboNode* node);
void search_for_images(GumboNode* node);

struct Crawl_task{ // Crawlers variables
    std::string url; // Crawled url
    int depth; 
};

const std::string user_agent =  "WebCrawler/1.0 (https://github.com/Notinon/WebCrawler_cURL); Nathanvansteenberge158@gmail.com";

void Crawl_process(CURL* curl, std::queue<Crawl_task> &urls);
std::queue<Crawl_task> urls;
std::unordered_set<std::string> visited_urls;
std::unordered_set<std::string> robotstxts;
int current_depth = 0;

Memory_Structs memory_struct;

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userp);
void reset_memory(Memory_Structs *mem);

const int MAX_DEPTH = 4;

int main(){
    curl_global_init(CURL_GLOBAL_ALL); // initialise curl with all flags enabled
    
    CURL* curl = curl_easy_init(); // init curl easy handle
    urls.push({"https://zeppuzzle.lennlepez.workers.dev/", current_depth}); // test site first site
    curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // The function with which we store the data
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &memory_struct); 
    Crawl_process(curl, urls);
    
    free(memory_struct.memory);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}

bool is_Crawlable(const std::string& contents, const std::string& user_agent, const std::string& url_path){
    googlebot::RobotsMatcher matcher;
    
    std::vector<std::string> user_agents;
    user_agents.push_back(user_agent);

    return matcher.AllowedByRobots(contents, &user_agents, url_path);
}

void search_for_links(GumboNode *node, int parent_depth)
{
    if(node->type != GUMBO_NODE_ELEMENT){
        return; 
    }
    GumboAttribute* href;
    if(node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href"))){
        int next_depth = parent_depth+1;
        if(next_depth <= MAX_DEPTH){
            urls.push({href->value, next_depth});
        }
    }

    GumboVector* children = &node->v.element.children;
    for(unsigned int i = 0; i < children->length; i++){
        search_for_links(static_cast<GumboNode*>(children->data[i]), parent_depth);
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

void search_for_images(GumboNode* node){
    if(node->type != GUMBO_NODE_ELEMENT){
        return;
    }
    GumboAttribute* rel = gumbo_get_attribute(&node->v.element.attributes, "rel");
    GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
    GumboAttribute* type = gumbo_get_attribute(&node->v.element.attributes, "type");

    if(node->v.element.tag == GUMBO_TAG_LINK && rel && href && type){
        std::cout << "https://" << href->value << '\n';
    }

    GumboVector* children = &node->v.element.children;
    for(unsigned int i = 0; i < children->length; i++){
        search_for_images(static_cast<GumboNode*>(children->data[i]));
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

void reset_memory(Memory_Structs *mem){
    if (mem->memory){
        free(mem->memory);
        mem->memory = nullptr;
    }
    mem->size = 0;
}

void Crawl_process(CURL* curl, std::queue<Crawl_task> &urls){
    using namespace std::chrono_literals;
    using namespace std::this_thread;
    using std::chrono::system_clock;

    while(!urls.empty()){
        Crawl_task current = urls.front();
        urls.pop();

        if(visited_urls.count(current.url)){
            continue;
        }
        visited_urls.insert(current.url);

        current.url = current.url+"/robots.txt";
        curl_easy_setopt(curl, CURLOPT_URL, current.url);
        CURLcode result_robottxt = curl_easy_perform(curl);
        if (result_robottxt == CURLE_OK && memory_struct.memory != nullptr){
            robotstxts.insert(memory_struct.memory);
        }
        // TODO: store base domain temporarily so you can get it back or try and replace /robots.txt and figure that mess out. 
        std::cout << "Next crawling: " << current.url << ' ' << "Depth: " << current.depth << '\n';
        std::this_thread::sleep_until(system_clock::now() + 2s);

        reset_memory(&memory_struct);
        curl_easy_setopt(curl, CURLOPT_URL, current.url.c_str());
        CURLcode result = curl_easy_perform(curl);

        if(result == CURLE_OK && memory_struct.memory != nullptr){
            GumboOutput *output = gumbo_parse(memory_struct.memory);

            search_for_links(output->root, current.depth);
            search_for_script_links(output->root);
            search_for_images(output->root);

            gumbo_destroy_output(&kGumboDefaultOptions, output);
        }else{
            std::cerr << "CURL ERROR: " << current.url << ": " << curl_easy_strerror(result) << '\n';
        }
        

    }
}
