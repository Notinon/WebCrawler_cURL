cmake_minimum_required(VERSION 3.10)

project(WebCrawler)

find_package(CURL REQUIRED)
find_package(unofficial-gumbo CONFIG REQUIRED)

add_executable(WebCrawler src/main.cpp)

target_include_directories(WebCrawler PRIVATE ${GUMBO_INCLUDE_DIR})

target_link_libraries(WebCrawler PRIVATE 
    CURL::libcurl
    unofficial::gumbo::gumbo
)