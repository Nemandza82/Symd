test_symd: test_kernel_functions.cpp
	g++ test_kernel_functions.cpp -std=c++17 -march=native -O3 -o test_kernel_functions

clang: test_kernel_functions.cpp
	clang++ test_kernel_functions.cpp -std=c++17 -mavx -O3 -o test_kernel_functions

vc: test_kernel_functions.cpp
	cl.exe /EHsc /O2 /std:c++17 .\test_kernel_functions.cpp
