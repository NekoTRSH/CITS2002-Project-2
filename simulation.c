#include 	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>

#define	RAM_CAPACITY	16
#define VM_CAPACITY		32
#define NUM_PROCESSES	4
#define PAGE_PROCESS	4
#define FRAMES			8

// Structure of the memory
typedef struct {
	int processID;
	int pageNum;
	int lastAccessed;
} Memory;

// Arrays that represent the RAM, VM and page table
Memory *ram[RAM_CAPACITY];
Memory *virtualMemory[VM_CAPACITY];
int pageTable[NUM_PROCESSES][PAGE_PROCESS];

// Function to initialise the virtual memory
void initialiseVM() {
	for (int i = 0; i < VM_CAPACITY; i++) {
		// Allocate memory for each
		virtualMemory[i] = (Memory *)malloc(sizeof(Memory));
		// Pages per process is 4, so i / PAGE_PROCESS will be the processID
		// For example if 'i' is between 0 to 3 the page process will be 0
		virtualMemory[i]->processID = i / PAGE_PROCESS;
		// pageNum is the specific page number within the process
		virtualMemory[i]->pageNum = (i % PAGE_PROCESS);
		// Initialised to 0 for each page
		virtualMemory[i]->lastAccessed = 0;
	}
}

void writeOutput(const char *filename, const char *processes) {

	FILE *outputFile = fopen(filename, "w");

	if (!outputFile) {
		fprintf(stderr, "Unable to open file\n");
		return;
	}

	// Testing out 
	fprintf(outputFile, "%s", processes);

	fclose(outputFile);
}

int *readFile(const char *filename, int *count) {

	FILE *inputFile = fopen(filename, "r");
	if (!inputFile) {
		fprintf(stderr, "error in opening file\n");
		return NULL;
	}

	// size_t buffer = 128;
	// char *processIDLine = (char *)malloc(buffer * sizeof(char));

	// if (!processIDLine) {
    //     fprintf(stderr, "Memory allocation failed\n");
    //     fclose(inputFile);
    //     return NULL;
    // }

	// if (fgets(processIDLine, buffer, inputFile) == NULL) {
    //     fprintf(stderr, "Error reading from file or file is empty: %s\n", processIDLine);
    //     free(processIDLine);
    //     fclose(inputFile);
    //     return NULL;
    // }

    int temp, total = 0;
    while (fscanf(inputFile, "%d", &temp) == 1) {
    	total++;
    }

    int *numbers = (int *)malloc(total * sizeof(int));
    if (numbers == NULL) {
        printf("Memory allocation failed.\n");
        fclose(inputFile);
        return NULL;
    }

    rewind(inputFile);
    *count = 0;
    while (fscanf(inputFile, "%d", &numbers[*count]) == 1) {
    	(*count)++;
    }

	fclose(inputFile);

	// int count = 0;
    // char *tempToken = strtok(processIDLine, " ");
    // while (tempToken) {
    //     count++;
    //     tempToken = strtok(NULL, " ");
    // }

    // int *array = (int *)malloc(count * sizeof(int));
    // if (!array) {
    // 	fprintf(stderr, "Memory allocation failed\n");
    //     free(processIDLine);
    //     return NULL;
    // }


	// // Reset the strtok process to split again
    // strtok(processIDLine, " "); // Reset strtok to the start of the string
    // char *token = strtok(processIDLine, " ");
    // int i = 0;
    // while (token) {
    //     array[i] = atoi(token);
    //     token = strtok(NULL, " ");
    //     i++;
    // }

    // free(processIDLine);

    // *size = count;

	return numbers;
}

int main(int argc, const char *argv[]) {
	
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <in.txt> <out.txt>\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Initialise VM
	initialiseVM();

	// Initialise RAM
	for (int i = 0; i < RAM_CAPACITY; i++) {
		ram[i] = NULL;
	}

	// Initialise page table to default, 99
	for (int i = 0; i < NUM_PROCESSES; i++) {
		for (int j = 0; j < PAGE_PROCESS; j++) {
			pageTable[i][j] = 99;
		}
	}

	int count;
	int *numbers = readFile(argv[1], &count);

	if (numbers != NULL) {
        // Print the numbers
        printf("Numbers in the file:\n");
        for (int i = 0; i < count; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");

        // Free the allocated memory
        free(numbers);
    }

    //printf("%s", argv[1]);

	//writeOutput(argv[2], processIDLine);

	return 0;
}

