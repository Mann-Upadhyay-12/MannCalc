
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_TEST_INPUTS 1000
#define MAX_INPUT_LENGTH 1024

// Function prototypes
void generate_random_input(char *buffer, size_t max_length);
void log_test_result(const char *input, const char *output);

int main() {
    // Seed the random number generator
    srand((unsigned int)time(NULL));

    char input[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH + 50]; // Adjusted to accommodate input + command
    char output[2048]; // Output buffer to capture program output

    FILE *fp;

    for (int i = 0; i < MAX_TEST_INPUTS; i++) {
        // Generate random input
        generate_random_input(input, MAX_INPUT_LENGTH);

        // Prepare command to test the main program
        snprintf(command, sizeof(command), "echo '%s' | ./bin/calc.out", input);

        // Open a process to run the command and capture output
        fp = popen(command, "r");
        if (fp == NULL) {
            fprintf(stderr, "Error running test command\n");
            continue;
        }

        // Capture output
        size_t offset = 0;
        while (fgets(output + offset, sizeof(output) - offset, fp) != NULL) {
            offset += strlen(output + offset);
        }

        // Close the process
        pclose(fp);

        // Log the result
        log_test_result(input, output);
    }

    return 0;
}

void generate_random_input(char *buffer, size_t max_length) {
    size_t length = rand() % (max_length - 1) + 1; // Random length between 1 and max_length
    for (size_t i = 0; i < length; i++) {
        int choice = rand() % 4;
        if (choice == 0) {
            buffer[i] = '0' + rand() % 10; // Random digit
        } else if (choice == 1) {
            buffer[i] = "+-*/=()"[rand() % 8]; // Random operator or parenthesis
        } else if (choice == 2) {
            buffer[i] = 'a' + rand() % 26; // Random lowercase letter
        } else {
            buffer[i] = 'A' + rand() % 26; // Random uppercase letter
        }
    }
    buffer[length] = '\0';
}

void log_test_result(const char *input, const char *output) {
    static FILE *log_file = NULL;
    if (!log_file) {
        log_file = fopen("fuzz_test_results.log", "w");
        if (!log_file) {
            fprintf(stderr, "Error opening log file\n");
            return;
        }
    }

    fprintf(log_file, "Input: %s\n", input);
    fprintf(log_file, "Output:\n%s\n", output);
    fprintf(log_file, "--------------------------------------------\n");

    fflush(log_file);
}

