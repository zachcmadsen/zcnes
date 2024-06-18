include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG v2.4.11
)
message(STATUS "Fetching doctest...")
FetchContent_MakeAvailable(doctest)

add_subdirectory(cpu)