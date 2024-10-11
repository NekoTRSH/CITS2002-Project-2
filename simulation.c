#include 	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>

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

char *readFile(const char *filename) {

	FILE *inputFile = fopen(filename, "r");
	if (!inputFile) {
		fprintf(stderr, "error in opening file\n");
		return NULL;
	}

	size_t bufferSize = 64;
	char *processIDLine = (char *)malloc(bufferSize * sizeof(char));

	if (!processIDLine) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(inputFile);
        return NULL;
    }

	// if (fgets(processIDLine, buffer, inputFile) == NULL) {
    //     fprintf(stderr, "Error reading from file or file is empty: %s\n", processIDLine);
    //     free(processIDLine);
    //     fclose(inputFile);
    //     return NULL;
    // }

    size_t currentLength = 0;
    int character;

    // Read the file character by character to dynamically resize buffer as needed
    while ((character = fgetc(inputFile)) != EOF) {
        // If the buffer is full, expand it
        if (currentLength + 1 >= bufferSize) {
            bufferSize *= 2; // Double the buffer size
            char *temp = realloc(processIDLine, bufferSize * sizeof(char));
            if (!temp) {
                fprintf(stderr, "Memory reallocation failed\n");
                free(processIDLine);
                fclose(inputFile);
                return NULL;
            }
            processIDLine = temp;
        }
        // Add the character to the string
        processIDLine[currentLength++] = character;
    }

    processIDLine[currentLength] = '\0';

	fclose(inputFile);

	// Handle empty file scenario
    if (currentLength == 0) {
        fprintf(stderr, "Error: File is empty\n");
        free(processIDLine);
        return NULL;
    }

    // Trim trailing whitespace
    while (currentLength > 0 && (processIDLine[currentLength - 1] == ' ' ||
                                 processIDLine[currentLength - 1] == '\t' ||
                                 processIDLine[currentLength - 1] == '\n' ||
                                 processIDLine[currentLength - 1] == '\r')) {
        processIDLine[--currentLength] = '\0';
    }

	return processIDLine;
}

int main(int argc, const char *argv[]) {
	
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <in.txt> <out.txt>\n", argv[0]);
		return 1;
	}

	char *processIDLine = readFile(argv[1]);
    if (!processIDLine) {
        return 1; 
    }

	writeOutput(argv[2], processIDLine);

	return 0;
}

