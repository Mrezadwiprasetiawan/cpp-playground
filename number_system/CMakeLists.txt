#Include direktori header
include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)

# File sumber
set(SRC_SOURCES "src/")

# Buat executable
add_executable(prime ${SRC_SOURCES}prime.cxx)
add_executable(fibonacci ${SRC_SOURCES}fibonacci.cxx)

# add test
add_test(NAME "Test find prime on 2 <= p <= 10000 and print all" COMMAND prime -l 100000)
add_test(NAME "Test find 10000 prime and print all" COMMAND prime -s 100000)
add_test(NAME "Test, is Prime 104729? print the number and the state" COMMAND prime -n 104729)
add_test(NAME "Test suffix prime class and print as Test, is prime" COMMAND prime -n 10k)
add_test(NAME "Test find and print 100 fibonacci " COMMAND fibonacci -l 100)
add_test(NAME "Test find and print 100th fibonnaci" COMMAND fibonacci -i 100)

# Untuk Testing
add_executable(big_int_test ${SRC_SOURCES}/big_int.cxx)
#add_test(NAME "Test big_int" COMMAND big_int_test)
