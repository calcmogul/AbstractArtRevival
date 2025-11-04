cmake_minimum_required(VERSION 3.28)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(AbstractArtRevival)

file(GLOB_RECURSE cpp_src src/*.cpp)
add_executable(AbstractArtRevival ${cpp_src})

if(NOT MSVC)
    target_compile_options(AbstractArtRevival PRIVATE -Wall -Wextra -pedantic)
else()
    # Suppress the following warnings:
    #   * C4244: lossy conversion
    target_compile_options(AbstractArtRevival PRIVATE /wd4244)
endif()
set_property(TARGET ${target} PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_compile_features(AbstractArtRevival PUBLIC cxx_std_23)
target_include_directories(AbstractArtRevival PRIVATE src)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)
target_link_libraries(AbstractArtRevival PUBLIC SFML::Graphics)

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build using shared libraries")
set(BUILD_TESTING OFF CACHE INTERNAL "Build Testing")
FetchContent_Declare(
    Sleipnir
    GIT_REPOSITORY https://github.com/SleipnirGroup/Sleipnir
    # main on 2025-11-01
    GIT_TAG 397ea39203f96d962b107204b523afc23b537e16
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(Sleipnir)
target_link_libraries(AbstractArtRevival PUBLIC Sleipnir::Sleipnir)

install(TARGETS AbstractArtRevival DESTINATION bin)
install(FILES data/arial.ttf DESTINATION bin/data)
