cmake_minimum_required(VERSION 2.8)
project(process-pst)

set(pstsdk_VERSION_MAJOR 0)
set(pstsdk_VERSION_MINOR 0)

# Turn on testing.
enable_testing()

# Allow access to pstsdk headers.
include_directories("${PROJECT_SOURCE_DIR}/pstsdk")

# Set up our compiler flags.
if(MSVC)
  set(CMAKE_CXX_FLAGS "/DPSTSDK_VALIDATION_LEVEL_FULL /EHsc /nologo /W4 /WX")
elseif(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "-Wall -Werror -g -DPSTSDK_VALIDATION_LEVEL_FULL -std=c++0x")
endif()

# Make sure we have Boost.
include(FindBoost)
find_package(Boost 1.42.0 COMPONENTS system filesystem REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# Use iconv if we have it.  This is required on non-Win32 platforms,
# because we don't necessarily know the encoding of wchar_t.
find_library(ICONV_LIBRARY NAMES iconv)

# Our C++ source files, except for main.cpp.
add_library(ProcessPstLib utilities.cpp document.cpp)

# Link our executables.
add_executable(spike spike.cpp)
add_executable(process-pst main.cpp)
target_link_libraries(process-pst ProcessPstLib ${Boost_LIBRARIES})

if(ICONV_LIBRARY)
  target_link_libraries(spike ${ICONV_LIBRARY})
  target_link_libraries(process-pst ${ICONV_LIBRARY})
endif()


#--------------------------------------------------------------------------
#  C++ module tests

# Create list of C++ files containing unit tests.  CppTests.cpp will be
# the generated driver.
create_test_sourcelist(CppTestsFiles CppTests.cpp
                       utilities_spec.cpp document_spec.cpp)

# Build a single test executable for all our C++ libraries.
add_executable(CppTests ${CppTestsFiles})
target_link_libraries(CppTests ProcessPstLib)
if(ICONV_LIBRARY)
  target_link_libraries(CppTests ${ICONV_LIBRARY})
endif()

# Create individual tests for each C++ module.  This is based on example in
# "Mastering CMake", 5th ed.
set(TestsToRun ${CppTestsFiles})
remove(TestsToRun CppTests.cpp)
foreach (test ${TestsToRun})
  get_filename_component(TestName ${test} NAME_WE)
  add_test(NAME ${TestName} COMMAND CppTests ${TestName})
endforeach()


#--------------------------------------------------------------------------
#  Command-line executable tests (driven by CMake)

add_test(NAME UsageExitValueTest COMMAND process-pst)
set_property(TEST UsageExitValueTest PROPERTY WILL_FAIL true)

add_test(NAME UsageTest COMMAND process-pst)
set_property(TEST UsageTest PROPERTY PASS_REGULAR_EXPRESSION "^Usage:")

add_test(NAME NoSuchPstExitValueTest COMMAND process-pst nosuch.pst out)
set_property(TEST NoSuchPstExitValueTest PROPERTY WILL_FAIL true)

add_test(NAME NoSuchPstTest COMMAND process-pst nosuch.pst out)
set_property(TEST NoSuchPstTest PROPERTY PASS_REGULAR_EXPRESSION "Could not open PST: ")


#--------------------------------------------------------------------------
#  Command-line executable tests (driven by Ruby)

file(GLOB RubySpecFiles spec/*_spec.rb)
foreach (spec ${RubySpecFiles})
  get_filename_component(TestName ${spec} NAME_WE)
  add_test(NAME ${TestName} COMMAND bundle exec spec ${spec})
  set_property(TEST ${TestName} PROPERTY ENVIRONMENT "SOURCE_ROOT=${PROJECT_SOURCE_DIR}" "BUILD_ROOT=${PROJECT_BINARY_DIR}")
endforeach()
