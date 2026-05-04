cmake_minimum_required(VERSION 3.10)

project(WebCrawler)

find_package(CURL REQUIRED)

find_path(GUMBO_INCLUDE_DIR NAMES gumbo.h)
find_library(GUMBO_LIBRARY NAMES gumbo)

add_executable(WebCrawler main.cpp)

target_include_directories(WebCrawler PRIVATE ${GUMBO_INCLUDE_DIR})
target_compile_definitions(WebCrawler PRIVATE CURL_STATICLIB)

target_link_libraries(WebCrawler PRIVATE 
    CURL::libcurl
    ${GUMBO_LIBRARY}
)