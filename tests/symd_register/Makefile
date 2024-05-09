test_symd: test_symd_register.cpp
	g++ test_symd_register.cpp -std=c++17 -march=native -O3 -o test_symd_register

clang: test_symd_register.cpp
	clang++ test_symd_register.cpp -std=c++17 -mavx -O3 -o test_symd_register

vc: test_symd_register.cpp
	cl.exe /EHsc /O2 /std:c++17 .\test_symd_register.cpp
