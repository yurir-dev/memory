cmake_minimum_required(VERSION 3.10)

include_directories(${CMAKE_SOURCE_DIR} ../ ../histProfiler)

# Files common to all tests
set (COMMON_SOURCES 	../memoryPool.h)

set(TEST_INTERFACE test_interface)
add_executable(${TEST_INTERFACE} test_interface.cpp ${COMMON_SOURCES})


#set(exes ${TEST_THREADS} ${TEST_MICROS} ${TEST_MILLIS} ${TEST_INTERFACE})
set(exes ${TEST_INTERFACE})

if (UNIX)
foreach (exe IN LISTS exes)
	target_link_libraries(${exe} pthread)
endforeach()
endif()

foreach (exe IN LISTS exes)
	add_test(${exe} ${exe})
endforeach()

