cmake_minimum_required(VERSION 3.14)
project(PyramidScript C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(PURAMIDSCRIPT_SOURCES_TESTS "Enable tests" OFF)

set(PURAMIDSCRIPT_SOURCES
    compiler.c
    preprocessor.c
    pyramidscript.c
    tokenizer.c
    vm.c
)

set(PURAMIDSCRIPT_API
    api/pyramidscript.h
)

set(PURAMIDSCRIPT_INCLUDES
    compiler.h
    preprocessor.h
    tokenizer.h
    vm.h
)

set(PURAMIDSCRIPT_INCLUDES_DIR
    api
    include
)

list(TRANSFORM PURAMIDSCRIPT_SOURCES       PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/sources/)
list(TRANSFORM PURAMIDSCRIPT_INCLUDES_DIR  PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
list(TRANSFORM PURAMIDSCRIPT_API           PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
list(TRANSFORM PURAMIDSCRIPT_INCLUDES      PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/include/)

source_group("Public API" FILES ${PURAMIDSCRIPT_API})
source_group("Include" FILES ${PURAMIDSCRIPT_INCLUDES})
source_group("Source" FILES ${PURAMIDSCRIPT_SOURCES})

add_library(${PROJECT_NAME} STATIC ${PURAMIDSCRIPT_SOURCES} ${PURAMIDSCRIPT_API} ${PURAMIDSCRIPT_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PURAMIDSCRIPT_INCLUDES_DIR})

if(PURAMIDSCRIPT_SOURCES_TESTS)
    add_executable(script_dev_tester ${CMAKE_CURRENT_SOURCE_DIR}/tests/script_dev_tester.c)
    add_definitions(-DFORTH_TEST_COMPONENTS)
    target_link_libraries(script_dev_tester PRIVATE PyramidScript)
endif()


if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    # -WX
  )
endif()