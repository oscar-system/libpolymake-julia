cmake_minimum_required(VERSION 3.01)

project(libpolymake_julia)

find_package(JlCxx REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

SET(POLYMAKE_MIN_VERSION "4.0")

find_package(Polymake REQUIRED ${POLYMAKE_MIN_VERSION})

include(CheckCXXCompilerFlag)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)

SET( CMAKE_CXX_COMPILER "${Polymake_CXX}")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -std=c++14 ${Polymake_CFLAGS}" )
SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${Polymake_LDFLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${Polymake_LDFLAGS}" )

# avoid gcc 9 internal compiler error,
# see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90998
if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9.0
      AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.3)
   SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-conversion" )
endif()

if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
   message( FATAL_ERROR "gcc version must be at least 7 to build the polymake CxxWrap interface." )
endif()

set(JLPOLYMAKE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(JLPOLYMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(JLPOLYMAKE_GENERATED ${CMAKE_CURRENT_BINARY_DIR}/include)

set(JLPOLYMAKE_TARGET polymake_julia)

set(JLPOLYMAKE_RUN_SCRIPT polymake_run_script)

include_directories(${JlCxx_DIR}/../../../include/)

file(GLOB JLPOLYMAKE_SRC "${JLPOLYMAKE_SOURCE_DIR}/*.cpp")
add_library(${JLPOLYMAKE_TARGET} SHARED ${JLPOLYMAKE_SRC})
target_link_libraries(${JLPOLYMAKE_TARGET} JlCxx::cxxwrap_julia "${Polymake_LIBS}")

file(GLOB JLPOLYMAKE_RUNNER "${JLPOLYMAKE_SOURCE_DIR}/runner/*.cpp")
add_executable(${JLPOLYMAKE_RUN_SCRIPT} ${JLPOLYMAKE_RUNNER})
target_link_libraries(${JLPOLYMAKE_RUN_SCRIPT} "${Polymake_LIBS}")

set(TYPE_TRANSLATOR ${CMAKE_CURRENT_BINARY_DIR}/jl/type_translator.jl)

set(GENERATED_HEADERS
    ${JLPOLYMAKE_GENERATED}/generated/get_type_names.h
    ${JLPOLYMAKE_GENERATED}/generated/map_inserts.h
    ${JLPOLYMAKE_GENERATED}/generated/option_set_take.h
    ${JLPOLYMAKE_GENERATED}/generated/polymake_call_function_feed_argument.h
    ${JLPOLYMAKE_GENERATED}/generated/type_declarations_extern.h
    ${JLPOLYMAKE_GENERATED}/generated/type_declarations.h
)

add_custom_command(
   OUTPUT ${GENERATED_HEADERS} ${TYPE_TRANSLATOR}
   COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/type_setup.pl ${CMAKE_CURRENT_BINARY_DIR}
   DEPENDS type_setup.pl
)

add_custom_target(generated_header ALL DEPENDS ${GENERATED_HEADERS})
add_dependencies(${JLPOLYMAKE_TARGET} generated_header)

target_include_directories(${JLPOLYMAKE_TARGET} PUBLIC
  JLCxx::cxxwrap_julia
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${JLPOLYMAKE_INCLUDE_DIR}>
  $<BUILD_INTERFACE:${JLPOLYMAKE_GENERATED}>
  "$<BUILD_INTERFACE:${Polymake_INCLUDE_DIRS}>"
)
target_include_directories(${JLPOLYMAKE_RUN_SCRIPT} PUBLIC
  $<BUILD_INTERFACE:${JLPOLYMAKE_INCLUDE_DIR}>
  $<BUILD_INTERFACE:${JLPOLYMAKE_GENERATED}>
  "$<BUILD_INTERFACE:${Polymake_INCLUDE_DIRS}>"
)

install(TARGETS
   ${JLPOLYMAKE_TARGET} ${JLPOLYMAKE_RUN_SCRIPT}
ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${TYPE_TRANSLATOR}
   DESTINATION ${CMAKE_INSTALL_DATADIR}/libpolymake_julia
)

