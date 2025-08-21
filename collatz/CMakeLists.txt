INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_compile_options(-Wno-shift-op-parentheses)
add_executable(collatz src/collatz.cpp)

# Override flags hanya untuk target ini, ga tau kenapa kalo dipaksa O3 dan unroll loop bakal ngestuck. kemungkinan karena operasi bitshiftnya
target_compile_options(collatz PRIVATE
  $<$<CXX_COMPILER_ID:GNU>: -O0 -fno-unroll-loops -fno-openmp>
  $<$<CXX_COMPILER_ID:Clang>: -O0  -fno-unroll-loops -fno-openmp>
)
