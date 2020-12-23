cmake_minimum_required(VERSION 3.17)
project(mg_m2_booster)

set(CMAKE_CXX_STANDARD 20)

execute_process(
        COMMAND bash "-c" "php-config --configure-options | cut -d' ' -f 8 | cut -d'=' -f 2"
        OUTPUT_VARIABLE PHP_INI_DIR)
message("PHP ini directory: ${PHP_INI_DIR}")

execute_process(
        COMMAND bash "-c" "php-config --extension-dir"
        OUTPUT_VARIABLE PHP_EXTENSION_DIR)
message("PHP extension directory: ${PHP_EXTENSION_DIR}")
execute_process (
        COMMAND php-config --include-dir
        OUTPUT_VARIABLE PHP_SOURCE
)
message("Using source directory: ${PHP_SOURCE}")


add_library(mg_m2_booster SHARED main.cpp)

link_directories(/usr/local/lib)

find_library(PHP_CPP_LIB NAMES libphpcpp.so)
target_link_libraries(mg_m2_booster ${PHP_CPP_LIB})


if (APPLE)
    link_directories(/usr/local/lib)
    include_directories(/usr/local/include)
    #    target_include_directories(php_runpy PUBLIC /usr/local/include)
    set_property(TARGET mg_m2_booster PROPERTY PREFIX "")
    set_property(TARGET mg_m2_booster PROPERTY OUTPUT_NAME "morozov_m2_booster.so")
    set_property(TARGET mg_m2_booster PROPERTY SUFFIX "")
    #    set_property(TARGET OpenImageIO PROPERTY SOVERSION "32.1.2.0")
endif()
