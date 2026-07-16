#include <curl/curl.h>
#include <gumbo.h>
#include <iostream>

# ifndef CRAWL_FUNCTIONS_H
# define CRAWL_FUNCTIONS_H
# endif


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
