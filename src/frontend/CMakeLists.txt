include(FetchContent)

# Turn these off to reduce the number of files to build.
set(SDL_STATIC OFF CACHE BOOL "Build a static version of SDL2")
set(SDL_TEST OFF CACHE BOOL "Build the SDL2_test library")

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL
    GIT_TAG         release-2.30.5
)
message(STATUS "Fetching SDL2...")
FetchContent_MakeAvailable(SDL2)

add_executable(zcnes)
target_sources(zcnes PRIVATE main.cpp)
target_link_libraries(zcnes PRIVATE SDL2::SDL2main SDL2::SDL2)

# Copy SDL2.dll to zcnes's output directory.
if (WIN32)
    add_custom_command(
        TARGET zcnes
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:zcnes>"
        VERBATIM
    )
endif()