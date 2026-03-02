#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "vector.h"

#define MAX_INPUT 256

IntArray vec1; // global vector

// ------------------- Help -------------------
void print_help() {
    printf("Available commands:\n");
    printf("  help           - Show this help message\n");
    printf("  echo <text>    - Print text\n");
    printf("  add <a> <b>    - Add two numbers\n");
    printf("  exit           - Quit shell\n");
    printf("  vec1           - Boot into vec1\n");
}

void print_vec_help() {
    printf("vec1 commands:\n");
    printf("  help           - Show this help message\n");
    printf("  exit           - Quit vec1\n");
    printf("  push <value>   - Add value to vector\n");
    printf("  size           - Show current size of vector\n");
    printf("  capacity       - Show current capacity of vector\n");
    printf("  print          - Print all elements in vector\n");
}

// ------------------- vec1 Shell -------------------
void shell_vec1() {
    char input[MAX_INPUT];

    printf("Welcome to vec1! Type 'help' for commands.\n");
    while (1) {
        printf("vec1> ");
        if (!fgets(input, MAX_INPUT, stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            printf("Exiting vec1...\n");
            break;
        }
        else if (strcmp(input, "help") == 0) {
            print_vec_help();
        }
        else if (strncmp(input, "push ", 5) == 0) {
            int val;
            if (sscanf(input + 5, "%d", &val) == 1) {
                push(&vec1, val);
                printf("Pushed %d\n", val);
            }
            else {
                printf("Usage: push <value>\n");
            }
        }
        else if (strcmp(input, "size") == 0) {
            printf("Size: %d\n", size(&vec1));
        }
        else if (strcmp(input, "capacity") == 0) {
            printf("Capacity: %d\n", capacity(&vec1));
        }
        else if (strcmp(input, "print") == 0) {
            print_vector(&vec1);
        }
        else {
            printf("Unknown command. Type 'help'.\n");
        }
    }
}

// ------------------- Create Vector -------------------
void create_vector() {
    int initial_capacity;
    char line[64];

    printf("What's the initial capacity of the vector? ");
    if (!fgets(line, sizeof(line), stdin)) {
        printf("Failed to read input.\n");
        return;
    }

    if (sscanf(line, "%d", &initial_capacity) != 1 || initial_capacity <= 0) {
        printf("Initial capacity must be a positive integer.\n");
        return;
    }

    init(&vec1, initial_capacity);
    printf("Vector initialized with capacity %d\n", initial_capacity);

    // Enter vec1 shell
    shell_vec1();
}

// ------------------- Main Shell -------------------
int shell() {
    char input[MAX_INPUT];

    printf("V-Chell v1.0\n");

    while (1) {
        printf("> ");
        if (!fgets(input, MAX_INPUT, stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            printf("Exiting shell...\n");
            break;
        }
        else if (strcmp(input, "help") == 0) {
            print_help();
        }
        else if (strncmp(input, "echo ", 5) == 0) {
            printf("%s\n", input + 5);
        }
        else if (strncmp(input, "add ", 4) == 0) {
            int a, b;
            if (sscanf(input + 4, "%d %d", &a, &b) == 2) {
                printf("Result: %d\n", a + b);
            }
            else {
                printf("Usage: add <number1> <number2>\n");
            }
        }
        else if (strcmp(input, "vec1") == 0) {
            create_vector();
        }
        else {
            printf("Unknown command. Type 'help'.\n");
        }
    }

    return 0;
}