add_executable(CQ_Test "main.cpp")
target_link_libraries(CQ_Test PRIVATE TBB::tbb)
target_link_libraries(CQ_Test PRIVATE benchmark::benchmark benchmark::benchmark_main)