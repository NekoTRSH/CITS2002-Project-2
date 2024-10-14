#include 	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>
#include	<limits.h>

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

// Write output... under construction
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

// Reads file properly
int *readFile(const char *filename, int *count) {

	FILE *inputFile = fopen(filename, "r");
	if (!inputFile) {
		fprintf(stderr, "error in opening file\n");
		return NULL;
	}

	// Count how many numbers there are
    int temp, total = 0;
    // Reads integer %d
    while (fscanf(inputFile, "%d", &temp) == 1) {
    	total++;
    }

    // Allocate memory for array
    int *numbers = (int *)malloc(total * sizeof(int));
    if (numbers == NULL) {
        printf("Memory allocation failed.\n");
        fclose(inputFile);
        return NULL;
    }

    // Put numbers into array
    rewind(inputFile);		// Reset file pointer to the start after counting
    *count = 0;				// Set count to 0
    // Reads next integer and puts it in numbers array via count
    while (fscanf(inputFile, "%d", &numbers[*count]) == 1) {
    	(*count)++;			// Increment to go to next element in array
    }

	fclose(inputFile);

	return numbers;
}

// Prints page table to output file
void printPageTable(FILE *out) {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        for (int j = 0; j < PAGE_PROCESS; j++) {
            fprintf(out, "%d", pageTable[i][j]);
            if (j < PAGE_PROCESS - 1) {
                fprintf(out, ", ");
            }
        }
        fprintf(out, "\n");
    }
}

// Prints RAM contents to output file
void printRAM(FILE *out) {
    for (int i = 0; i < RAM_CAPACITY; i++) {
        if (ram[i] != NULL) {
            fprintf(out, "%d,%d,%d", ram[i]->processID, ram[i]->pageNum, ram[i]->lastAccessed);
        } else {
            fprintf(out, "empty");
        }
        if (i < RAM_CAPACITY - 1) {
            fprintf(out, "; ");
        }
    }
    fprintf(out, "\n");
}

// Paging simulation function
void loadPage(int processID, int timeStep) {

	// Simplified the page to load
	int pageToLoad = timeStep % PAGE_PROCESS;

	// Selects the frame we are working with
	int frame = pageTable[processID][pageToLoad];

	// If the frame is already in the RAM
	if (frame != 99) {
		ram[frame]->lastAccessed = timeStep;		// The last accessed time is updated to the timeStep
		printf("Page %d process %d in RAM: frame %d, updating time.\n", pageToLoad, processID, frame);
		return;
	}

	printf("Page %d process %d needs to go RAM.\n", pageToLoad, processID);

	// Finds available frame in RAM, two consecutive slots
	int emptyFrame = -1;
    for (int i = 0; i < RAM_CAPACITY; i += 2) {  	// Pages occupy two locations
        if (ram[i] == NULL && ram[i + 1] == NULL) {	// If both are NULL(empty)
            emptyFrame = i / 2;  // Frame number
            break;
        }
    }

    // If no frames free, evict a page
    if (emptyFrame == -1) {

    	printf("No free frames, LRU for process %d.\n", processID);

    	// Find least recently used page of current process, local LRU
    	int LRUFrame = -1;							// Initialised to invalid -1
    	int LRUTime = INT_MAX;						// Use big arbitrary integer

    	for (int i = 0; i < RAM_CAPACITY; i += 2) {
    		// Check if ram is not NULL, processID is same, last accessed is smaller than LRUTime(initially always true)
    		if (ram[i] && ram[i]->processID == processID && ram[i]->lastAccessed < LRUTime) {
    			LRUTime = ram[i]->lastAccessed;		// LRUTime will be updated to last accessed
    			LRUFrame = i / 2;					// Iterates in steps of 2
    		}
    	}

    	// If no pages from the process in RAM, use global LRU
    	if (LRUFrame == -1) {
            printf("No pages from process %d in RAM, LRU.\n", processID);
            for (int i = 0; i < RAM_CAPACITY; i += 2) {
            	// Similar to before but don't check the processID
                if (ram[i] && ram[i]->lastAccessed < LRUTime) {
                    LRUTime = ram[i]->lastAccessed;
                    LRUFrame = i / 2;
                }
            }
        }

        printf("Evicting page %d process %d frame %d (last time %d).\n",
               ram[LRUFrame * 2]->pageNum, ram[LRUFrame * 2]->processID, LRUFrame, LRUTime);

        // Evicting the page to VM, update page table
        pageTable[ram[LRUFrame * 2]->processID][ram[LRUFrame * 2]->pageNum] = 99;

        // Empty frame loads the new page
        emptyFrame = LRUFrame;

    }

    // Load page into RAM
    // emptyFrame represents index of frame in RAM
    // RHS calculates index in VM where process is loaded from
    // processID * PAGE_PROCESS is base index, pageToLoad specifies which page of process
    ram[emptyFrame * 2] = virtualMemory[processID * PAGE_PROCESS + pageToLoad];
    ram[emptyFrame * 2 + 1] = virtualMemory[processID * PAGE_PROCESS + pageToLoad + 1];

    // Update lastAccessed of the frame in the RAM, important for LRU
    ram[emptyFrame * 2]->lastAccessed = timeStep;
    ram[emptyFrame * 2 + 1]->lastAccessed = timeStep;

    // Update page table after loading page into RAM
    pageTable[processID][pageToLoad] = emptyFrame;
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

	// Read file and obtain array with processID's
	int count;		// For the number of processID's
	int *processIDArray = readFile(argv[1], &count);

	// Testing
	printf("%d\n", count);
	for (int i = 0; i < count; i++) {
		printf("%d ", processIDArray[i]);
	}
	printf("\n");

	//
	for (int i = 0; i < count; i++) {
		int processID = processIDArray[i];
		loadPage(processID, i);
	}

    // Free the allocated memory
    free(processIDArray);


    FILE *out = fopen(argv[2], "w");
    if (!out) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    printPageTable(out);
    printRAM(out);

    fclose(out);

    for (int i = 0; i < VM_CAPACITY; i++) {
        free(virtualMemory[i]);
    }

	return 0;
}

