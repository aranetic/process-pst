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
find_package(Boost 1.42.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# Use iconv if we have it.  This is required on non-Win32 platforms,
# because we don't necessarily know the encoding of wchar_t.
find_library(ICONV_LIBRARY NAMES iconv)

# Link our executables.
add_executable(spike spike.cpp)
add_executable(process-pst main.cpp)
if(ICONV_LIBRARY)
  target_link_libraries(spike ${ICONV_LIBRARY})
  target_link_libraries(process-pst ${ICONV_LIBRARY})
endif()


#--------------------------------------------------------------------------
#  Command-executable tests

add_test(NAME UsageExitValueTest COMMAND process-pst)
set_property(TEST UsageExitValueTest PROPERTY WILL_FAIL true)

add_test(NAME UsageTest COMMAND process-pst)
set_property(TEST UsageTest PROPERTY PASS_REGULAR_EXPRESSION "^Usage:")


