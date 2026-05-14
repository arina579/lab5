cmake_minimum_required(VERSION 3.14)
project(BankingProject CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_library(banking STATIC 
    banking/Account.cpp 
    banking/Transaction.cpp
)

target_include_directories(banking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/banking)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests 
    tests/test_account.cpp 
    tests/test_transaction.cpp
)

target_link_libraries(unit_tests 
    banking 
    gtest_main 
    gmock_main
)

add_test(NAME RunBankingTests COMMAND unit_tests)
