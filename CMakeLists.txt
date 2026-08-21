cmake_minimum_required(VERSION 3.10)
project(WebCrawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ABSL_PROPAGATE_CXX_STD ON)

include(FetchContent)

FetchContent_Declare(
    absl
    URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20240116.0.zip
)
FetchContent_MakeAvailable(absl)

find_package(CURL REQUIRED)
find_package(unofficial-gumbo CONFIG REQUIRED)

set(ROBOTS_SKIP_DEPS ON CACHE BOOL "" FORCE)

add_subdirectory("C:/Users/NathanVanSteenberge/OneDrive - GlorieuxSecundair/C++/WebCrawler_C++/robotstxt")
add_executable(WebCrawler src/main.cpp)

target_include_directories(WebCrawler PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/robotstxt")

target_link_libraries(WebCrawler PRIVATE 
    CURL::libcurl
    unofficial::gumbo::gumbo
    robots
)