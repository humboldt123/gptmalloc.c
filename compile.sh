gcc -c gptmalloc.c -o gptmalloc.o 
ar rcs libgptmalloc.a gptmalloc.o
gcc main.c -L. -lgptmalloc -o out -lcurl -I/usr/local/include -L/usr/local/lib
