enable_testing()

add_executable(serialize_test)
target_sources(serialize_test PRIVATE serialize_test.cpp)
target_link_libraries(serialize_test PRIVATE gtest gtest_main ast)
