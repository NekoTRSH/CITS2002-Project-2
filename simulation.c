#include 	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>

#define	RAM_CAPACITY	16
#define VM_CAPACITY	32

struct {
	int processID;
	int pageNum;
	int lastAccessed;
} memory;


void writeOutput(const char *filename, const char *processes);

char *readFile(const char *filename) {

	FILE *inputFile = fopen(filename, "r");
	if (!inputFile) {
		fprintf(stderr, "error in opening file\n");
		return NULL;
	}

	size_t buffer = 128;
	char *processIDLine = (char *)malloc(buffer * sizeof(char));

	if (!processIDLine) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(inputFile);
        return NULL;
    }

	if (fgets(processIDLine, buffer, inputFile) == NULL) {
        fprintf(stderr, "Error reading from file or file is empty: %s\n", processIDLine);
        free(processIDLine);
        fclose(inputFile);
        return NULL;
    }

	fclose(inputFile);

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

	//printf("%s\n", line);

	writeOutput(argv[2], processIDLine);

	return 0;
}

void writeOutput (const char *filename, const char *processes) {

	FILE *outputFile = fopen(filename, "w");

	if (!outputFile) {
		fprintf(stderr, "Unable to open file\n");
		return;
	}

	fprintf(outputFile, "%s", processes);

	fclose(outputFile);
}

