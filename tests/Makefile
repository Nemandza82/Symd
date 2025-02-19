test_symd: all_tests.cpp
	g++ all_tests.cpp -std=c++17 -march=native -O3 -DNDEBUG -o all_tests

clang: all_tests.cpp
	clang++ all_tests.cpp -std=c++17 -mavx -mavx2 -O3 -o all_tests

vc: all_tests.cpp
	cl.exe /EHsc /O2 /std:c++17 .\all_tests.cpp