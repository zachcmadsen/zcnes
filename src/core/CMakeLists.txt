include(${PROJECT_SOURCE_DIR}/cmake/zcnes.cmake)

set(CORE_SRC nes.c)

add_library(core STATIC ${CORE_SRC})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
zcnes_options(core)

# An extra library increases the build time, but it keeps the build commands
# and CI workflow simpler.
# TODO: Put this target behind a condition to make release builds shorter?
add_library(core-proc-test STATIC ${CORE_SRC})
target_include_directories(core-proc-test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
zcnes_options(core-proc-test)
target_compile_definitions(core-proc-test PUBLIC ZCNES_PROCESSOR_TESTS)
