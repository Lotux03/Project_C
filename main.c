#include <stdio.h>
#include <stdlib.h>
#include "example_main.h"
#include "vector_lib.h"

typedef struct {
	int id;
	float value;
} MyStruct;

void main() {
	printf("Memory Tracker Test\n");

	Array int_arr, char_arr, double_arr, struct_arr;

	array_init(&int_arr, sizeof(int), 4);
	array_init(&char_arr, sizeof(char), 4);
	array_init(&double_arr, sizeof(double), 4);
	array_init(&struct_arr, sizeof(MyStruct), 4);

	// Now use each one
	int x = 42;
	array_push_back(&int_arr, &x);

	char c = 'A';
	array_push_back(&char_arr, &c);

	double d = 3.14;
	array_push_back(&double_arr, &d);

	MyStruct ms = { 1, 2.5f };
	array_push_back(&struct_arr, &ms);

	array_free(&int_arr);
	array_free(&char_arr);
	array_free(&double_arr);
	array_free(&struct_arr);
}