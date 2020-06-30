if(Polymake_FOUND)
    return()
endif()

# polymake-config

if(Polymake_PREFIX)
   message(STATUS "Adding path ${Polymake_PREFIX} to search path")
   list(APPEND CMAKE_PREFIX_PATH ${Polymake_PREFIX})
   set(Polymake_Config_EXECUTABLE ${Polymake_PREFIX}/bin/polymake-config)
   message(STATUS "THIS BRANCH")
else()
   find_program(Polymake_Config_EXECUTABLE polymake-config DOC "Polymake config executable")
   message(STATUS "Found Polymake executable: " ${Polymake_Config_EXECUTABLE})
endif()

# version

if(Polymake_Config_EXECUTABLE)
   execute_process(
      COMMAND "${Polymake_Config_EXECUTABLE}" --version
      OUTPUT_VARIABLE Polymake_VERSION_STRING
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
endif()
MESSAGE(STATUS "Polymake_VERSION_STRING: ${Polymake_VERSION_STRING}")

# compiler

if(Polymake_Config_EXECUTABLE)
   execute_process(
      COMMAND ${Polymake_Config_EXECUTABLE} --cc
      OUTPUT_VARIABLE Polymake_CXX
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )

   set(Polymake_LDFLAGS "${Polymake_CXX}"
      CACHE PATH "Polymake compiler")
endif()

MESSAGE(STATUS "Polymake_CXX:        ${Polymake_CXX}")

# cflags

if(Polymake_Config_EXECUTABLE)
   execute_process(
      COMMAND ${Polymake_Config_EXECUTABLE} --cflags
      OUTPUT_VARIABLE Polymake_CFLAGS
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )

   set(Polymake_CFLAGS ${Polymake_CFLAGS} CACHE PATH "Polymake cflags")
endif()
MESSAGE(STATUS "Polymake_CFLAGS:   ${Polymake_CFLAGS}")

# includes

if(Polymake_Config_EXECUTABLE)
   execute_process(
      COMMAND ${Polymake_Config_EXECUTABLE} --includes
      OUTPUT_VARIABLE Polymake_INCLUDE_DIRS
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )

   string(REGEX REPLACE "-I" "" Polymake_INCLUDE_DIRS "${Polymake_INCLUDE_DIRS}")
   separate_arguments(Polymake_INCLUDE_DIRS)

   set(Polymake_INCLUDE_DIRS ${Polymake_INCLUDE_DIRS} CACHE STRING "Location of Polymake include files")
endif()
MESSAGE(STATUS "Polymake_INCLUDE_DIRS:   ${Polymake_INCLUDE_DIRS}")

# libs 

if(Polymake_Config_EXECUTABLE)
   execute_process(
      COMMAND ${Polymake_Config_EXECUTABLE} --libs
      OUTPUT_VARIABLE Polymake_LIBS
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )

   string(REGEX REPLACE "-l" "" Polymake_LIBS "${Polymake_LIBS}")
   separate_arguments(Polymake_LIBS)

   set(Polymake_LIBS "${Polymake_LIBS}"
      CACHE STRING "Polymake library")
endif()

MESSAGE(STATUS "Polymake_LIBS:        ${Polymake_LIBS}")

if(Polymake_Config_EXECUTABLE)
   execute_process(
      COMMAND ${Polymake_Config_EXECUTABLE} --ldflags
      OUTPUT_VARIABLE Polymake_LDFLAGS
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )

   set(Polymake_LDFLAGS "${Polymake_LDFLAGS}"
      CACHE STRING "Polymake ldflags")
endif()

MESSAGE(STATUS "Polymake_LDFLAGS:        ${Polymake_LDFLAGS}")

# summary

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Polymake
   REQUIRED_VARS   Polymake_Config_EXECUTABLE Polymake_CXX Polymake_CFLAGS Polymake_INCLUDE_DIRS Polymake_LDFLAGS Polymake_LIBS
   VERSION_VAR     Polymake_VERSION_STRING
   FAIL_MESSAGE    "Polymake not found"
)
