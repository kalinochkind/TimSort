all:
	g++ -O2 -Iinclude src/main.cpp -o timsort.exe
	g++ -O2 -Iinclude src/main.cpp -DFORCE_INPLACE_MERGE -o timsort-inplace.exe
clean:
	rm timsort.exe timsort-inplace.exe
