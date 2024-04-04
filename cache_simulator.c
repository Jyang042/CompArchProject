#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_CACHE_SIZE 8
#define MAX_CACHE_SIZE 8192
#define MIN_BLOCK_SIZE 8
#define MAX_BLOCK_SIZE 64
#define MIN_ASSOCIATIVITY 1
#define MAX_ASSOCIATIVITY 16
#define MIN_PHYSICAL_MEMORY 1
#define MAX_PHYSICAL_MEMORY 4096

void printUsage() {
    printf("Usage: ./cache_simulator -s <cache size KB> -b <block size> -a <associativity> -r <replacement policy> -p <physical memory MB> -u <% phys mem used> -n <Instr / Time Slice> -f <trace file name(s)>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 17 || argc % 2 != 1) {
        printUsage();
        return 1;
    }

    int cache_size_kb = -1;
    int block_size = -1;
    int associativity = -1;
    char* replacement_policy = NULL;
    int physical_memory_mb = -1;
    int percent_mem_used = -1;
    int instr_time_slice = -1;
    char* trace_files[3];
    int num_trace_files = 0;

    printf("Cache Simulator - CS 3853 - Instructor Version: 2.10\n");
    printf("Trace File(s):\n");

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-s") == 0) {
            cache_size_kb = atoi(argv[i+1]);
            if (cache_size_kb < MIN_CACHE_SIZE || cache_size_kb > MAX_CACHE_SIZE) {
                printf("Invalid cache size. It must be between 8 KB and 8 MB.\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-b") == 0) {
            block_size = atoi(argv[i+1]);
            if (block_size < MIN_BLOCK_SIZE || block_size > MAX_BLOCK_SIZE) {
                printf("Invalid block size. It must be between 8 bytes and 64 bytes.\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            associativity = atoi(argv[i+1]);
            if (associativity < MIN_ASSOCIATIVITY || associativity > MAX_ASSOCIATIVITY) {
                printf("Invalid associativity. It must be between 1 and 16.\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-r") == 0) {
            replacement_policy = argv[i+1];
            if (strcmp(replacement_policy, "RR") != 0 && strcmp(replacement_policy, "RND") != 0) {
                printf("Invalid replacement policy. It must be RR or RND.\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-p") == 0) {
            physical_memory_mb = atoi(argv[i+1]);
            if (physical_memory_mb < MIN_PHYSICAL_MEMORY || physical_memory_mb > MAX_PHYSICAL_MEMORY) {
                printf("Invalid physical memory size. It must be between 1 MB and 4 GB.\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-u") == 0) {
            percent_mem_used = atoi(argv[i+1]);
            if (percent_mem_used < 0 || percent_mem_used > 100) {
                printf("Invalid percentage of physical memory used. It must be between 0 and 100.\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-n") == 0) {
            instr_time_slice = atoi(argv[i+1]);
            // No validation needed for this option
        } else if (strcmp(argv[i], "-f") == 0) {
            if (num_trace_files >= 3) {
                printf("Exceeded maximum number of trace files (3).\n");
                return 1;
            }
            trace_files[num_trace_files++] = argv[i+1];
        } else {
            printf("Invalid option: %s\n", argv[i]);
            printUsage();
            return 1;
        }
    }
  
    printf("Cache Simulator CS 3853 Spring 2024 - Group #06\n");
    printf("Trace Files:\n");
    for (int i = 0; i < num_trace_files; ++i) {
        printf("%s\n", trace_files[i]);
    }
    printf("***** Input Parameters *****\n");
    printf("Cache Size: %d KB\n", cache_size_kb);
    printf("Block Size: %d bytes\n", block_size);
    printf("Associativity: %d\n", associativity);
    printf("Replacement Policy: %s\n", (strcmp(replacement_policy, "RR") == 0) ? "Round Robin" : "Random");
    printf("Physical Memory: %d MB\n", physical_memory_mb);
    printf("Percent Memory Used by System: %d%%\n", percent_mem_used);
    printf("Instructions / Time Slice: %d\n", instr_time_slice);
    

    return 0;
}
