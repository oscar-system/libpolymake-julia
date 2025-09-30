cmake_minimum_required(VERSION 3.10)

project(libpolymake_julia)

find_package(JlCxx REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

SET(POLYMAKE_MIN_VERSION "4.0")

find_package(Polymake REQUIRED ${POLYMAKE_MIN_VERSION})

include(CheckCXXCompilerFlag)
include(GNUInstallDirs)

# override c++ compat, the interface doesn't really need c++20 and
# the c++ library in some sys-roots doesn't support full c++20
set_target_properties(JlCxx::cxxwrap_julia PROPERTIES INTERFACE_COMPILE_FEATURES "")
set(CMAKE_CXX_STANDARD 17)

SET( CMAKE_CXX_COMPILER "${Polymake_CXX}")
# this is to allow filtering of the -fopenmp flag when building natively
# on macos (only the cross-compiler for macos in BB supports -fopenmp)
if(POLYMAKE_DISABLE_OPENMP)
    string(REPLACE "-fopenmp" "" Polymake_CFLAGS ${Polymake_CFLAGS})
    string(REPLACE "-fopenmp" "" Polymake_LDFLAGS ${Polymake_LDFLAGS})
endif()
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g ${Polymake_CFLAGS}" )
SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${Polymake_LDFLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${Polymake_LDFLAGS}" )

# this works around a design issue in polymake where the corresponding symbol is
# defined in a bundled extension and thus not available at link-time
# this can be removed once flint is a core-dependency of polymake
if(APPLE)
   SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-U,__ZN2pm18QuadraticExtensionINS_8RationalEE9normalizeEv" )
else()
   SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--undefined,__ZN2pm18QuadraticExtensionINS_8RationalEE9normalizeEv" )
endif()

# to avoid lib64 dirs as binarybuilder uses lib everywhere
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_INSTALL_LIBDIR "lib")

# avoid gcc 9 internal compiler error,
# see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90998
if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9.0
      AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.3)
   SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-conversion" )
endif()

if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
   message( FATAL_ERROR "gcc version must be at least 7 to build the polymake CxxWrap interface." )
endif()

set(TYPE_GENERATOR ${CMAKE_CURRENT_SOURCE_DIR}/src/polymake/type_setup.pl)
set(GENERATOR_STAGING_DIR ${CMAKE_CURRENT_BINARY_DIR}/tmp/)
set(GENERATOR_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(GENERATOR_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${TYPE_GENERATOR})

file(REMOVE_RECURSE ${GENERATOR_STAGING_DIR})
file(MAKE_DIRECTORY ${GENERATOR_STAGING_DIR})

execute_process(COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/src/polymake/type_setup.pl ${GENERATOR_STAGING_DIR} COMMAND_ERROR_IS_FATAL ANY)

file(MAKE_DIRECTORY ${GENERATOR_OUTPUT_DIR}/src)
file(MAKE_DIRECTORY ${GENERATOR_OUTPUT_DIR}/jl)
file(MAKE_DIRECTORY ${GENERATOR_OUTPUT_DIR}/include/jlpolymake/generated)

# Remove stale files from final generator output directory
file(GLOB_RECURSE CURR_GENERATED_FILES RELATIVE "${GENERATOR_OUTPUT_DIR}/"
   "${GENERATOR_OUTPUT_DIR}/src/*"
   "${GENERATOR_OUTPUT_DIR}/jl/*"
   "${GENERATOR_OUTPUT_DIR}/include/jlpolymake/generated/*")
foreach(FILE ${CURR_GENERATED_FILES})
    if(NOT EXISTS "${GENERATOR_STAGING_DIR}/${FILE}")
        file(REMOVE "${GENERATOR_OUTPUT_DIR}/${FILE}")
    endif()
endforeach()

## Copy modified files from staging to final generator output directory
file(GLOB_RECURSE GENERATED_FILES RELATIVE "${GENERATOR_STAGING_DIR}/"
   "${GENERATOR_STAGING_DIR}/src/*"
   "${GENERATOR_STAGING_DIR}/jl/*"
   "${GENERATOR_STAGING_DIR}/include/jlpolymake/generated/*")
foreach(FILE ${GENERATED_FILES})
   execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GENERATOR_STAGING_DIR}/${FILE}" "${GENERATOR_OUTPUT_DIR}/${FILE}" COMMAND_ERROR_IS_FATAL ANY)
endforeach()

set(JLPOLYMAKE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(JLPOLYMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(JLPOLYMAKE_GENERATED ${CMAKE_CURRENT_BINARY_DIR}/include)

file(GLOB GENERATED_CPP
   ${CMAKE_CURRENT_BINARY_DIR}/src/unbox_pv*.cpp
   ${CMAKE_CURRENT_BINARY_DIR}/src/wrap_types*.cpp
)

set(JLPOLYMAKE_TARGET polymake_julia)

include_directories(${JlCxx_DIR}/../../../include/)

set(GENERATED_HEADERS
    ${JLPOLYMAKE_GENERATED}/jlpolymake/generated/get_type_names.h
    ${JLPOLYMAKE_GENERATED}/jlpolymake/generated/map_inserts.h
    ${JLPOLYMAKE_GENERATED}/jlpolymake/generated/option_set_take.h
    ${JLPOLYMAKE_GENERATED}/jlpolymake/generated/call_function_feed_argument.h
    ${JLPOLYMAKE_GENERATED}/jlpolymake/generated/type_declarations_extern.h
    ${JLPOLYMAKE_GENERATED}/jlpolymake/generated/type_declarations.h
)
file(GLOB JLPOLYMAKE_HEADERS "${JLPOLYMAKE_INCLUDE_DIR}/jlpolymake/*.h")


file(GLOB JLPOLYMAKE_SRC "${JLPOLYMAKE_SOURCE_DIR}/*.cpp")
add_library(${JLPOLYMAKE_TARGET} SHARED ${JLPOLYMAKE_SRC} ${GENERATED_CPP})
target_link_libraries(${JLPOLYMAKE_TARGET} JlCxx::cxxwrap_julia "${Polymake_LIBS}" "${EXTRA_LIBRARIES}")

set(TYPE_TRANSLATOR ${CMAKE_CURRENT_BINARY_DIR}/jl/type_translator.jl)

set(GEN_DEPS_TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/polymake/generate_deps_tree.jl)

set_target_properties(${JLPOLYMAKE_TARGET} PROPERTIES
   PUBLIC_HEADER "${JLPOLYMAKE_HEADERS}")

add_custom_target(generated_files ALL DEPENDS ${GENERATED_HEADERS} ${GENERATED_CPP})
add_dependencies(${JLPOLYMAKE_TARGET} generated_files)

target_include_directories(${JLPOLYMAKE_TARGET} PUBLIC
  JLCxx::cxxwrap_julia
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${JLPOLYMAKE_INCLUDE_DIR}>
  $<BUILD_INTERFACE:${JLPOLYMAKE_GENERATED}>
  "$<BUILD_INTERFACE:${Polymake_INCLUDE_DIRS}>"
)

install(TARGETS
   ${JLPOLYMAKE_TARGET}
ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/jlpolymake
PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/jlpolymake)

install(FILES ${TYPE_TRANSLATOR} ${GEN_DEPS_TREE}
   DESTINATION ${CMAKE_INSTALL_DATADIR}/libpolymake_julia
)

install(FILES ${GENERATED_HEADERS}
             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/jlpolymake/generated/
             COMPONENT Headers)
