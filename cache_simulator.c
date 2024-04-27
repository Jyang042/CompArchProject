#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MIN_CACHE_SIZE 8
#define MAX_CACHE_SIZE 8192
#define MIN_BLOCK_SIZE 8
#define MAX_BLOCK_SIZE 64
#define MIN_ASSOCIATIVITY 1
#define MAX_ASSOCIATIVITY 16
#define MIN_PHYSICAL_MEMORY 1
#define MAX_PHYSICAL_MEMORY 4096
#define MAX_LINE_LENGTH 100
#define MAX_PRINT_LINES 20

typedef struct {
    int valid;
    unsigned int tag;
} CacheEntry;

void printUsage() {
    printf("Usage: ./cache_simulator -s <cache size KB> -b <block size> -a <associativity> -r <replacement policy> -p <physical memory MB> -u <percentage of phys mem used> -n <Instr / Time Slice> -f <trace file name(s)>\n");
}

void simulateCacheAccess(CacheEntry** cache, int cache_size, int block_size, unsigned int address, int* cache_hits, int* compulsory_misses, int* conflict_misses, double* cpi) {
    unsigned int tag = address >> (int)log2(block_size);
    int index = (address / block_size) % (cache_size / block_size);

    if (cache[index]->valid && cache[index]->tag == tag) {
        (*cache_hits)++;
        (*cpi) += 1.0;
    }
    else {
        if (!cache[index]->valid) {
            (*compulsory_misses)++;
        }
        else {
            (*conflict_misses)++;
        }

        cache[index]->valid = 1;
        cache[index]->tag = tag;

        int cache_access_cycles = 1;
        int cache_miss_cycles = 4;
        int instruction_execution_cycles = 2;
        (*cpi) += (cache_access_cycles + cache_miss_cycles + instruction_execution_cycles);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 17 || argc % 2 != 1) {
        printUsage();
        return 1;
    }

    FILE* file1;
    FILE* file2;
    FILE* file3;
    char* trace_files[3];
    int num_trace_files = 0;
    char* fileRead[100];

    int cache_size_kb = -1;
    int block_size = -1;
    int associativity = -1;
    int physical_memory_mb = -1;
    int percent_mem_used = -1;
    int instr_time_slice = -1;
    char* replacement_policy = NULL;


    printf("Cache Simulator - CS 3853 - Instructor Version: 2.10\n");
    printf("Trace File(s):\n");

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-s") == 0) {
            cache_size_kb = atoi(argv[i + 1]);
            if (cache_size_kb < MIN_CACHE_SIZE || cache_size_kb > MAX_CACHE_SIZE) {
                printf("Invalid cache size. It must be between 8 KB and 8 MB.\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-b") == 0) {
            block_size = atoi(argv[i + 1]);
            if (block_size < MIN_BLOCK_SIZE || block_size > MAX_BLOCK_SIZE) {
                printf("Invalid block size. It must be between 8 bytes and 64 bytes.\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-a") == 0) {
            associativity = atoi(argv[i + 1]);
            if (associativity < MIN_ASSOCIATIVITY || associativity > MAX_ASSOCIATIVITY) {
                printf("Invalid associativity. It must be between 1 and 16.\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-r") == 0) {
            replacement_policy = argv[i + 1];
            if (strcmp(replacement_policy, "rr") != 0 && strcmp(replacement_policy, "rnd") != 0) {
                printf("Invalid replacement policy. It must be rr or rnd.\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-p") == 0) {
            physical_memory_mb = atoi(argv[i + 1]);
            if (physical_memory_mb < MIN_PHYSICAL_MEMORY || physical_memory_mb > MAX_PHYSICAL_MEMORY) {
                printf("Invalid physical memory size. It must be between 1 MB and 4 GB.\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-u") == 0) {
            percent_mem_used = atoi(argv[i + 1]);
            if (percent_mem_used < 0 || percent_mem_used > 100) {
                printf("Invalid percentage of physical memory used. It must be between 0 and 100.\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-n") == 0) {
            instr_time_slice = atoi(argv[i + 1]);
            // No validation needed for this option
        }
        else if (strcmp(argv[i], "-f") == 0) {
            if (num_trace_files >= 3) {
                printf("Exceeded maximum number of trace files (3).\n");
                return 1;
            }
            trace_files[num_trace_files++] = argv[i + 1];
        }
        else {
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
    printf("\n***** Input Parameters *****\n\n");
    printf("Cache Size: %d KB\n", cache_size_kb);
    printf("Block Size: %d bytes\n", block_size);
    printf("Associativity: %d\n", associativity);
    printf("Replacement Policy: %s\n", (strcmp(replacement_policy, "rr") == 0) ? "Round Robin" : "Random");
    printf("Physical Memory: %d MB\n", physical_memory_mb);
    printf("Percent Memory Used by System: %d\n", percent_mem_used);
    printf("Instructions / Time Slice: %d\n", instr_time_slice);

    // Cache Math and printing
    int cache_size_b = cache_size_kb * 1024;
    int total_rows = (cache_size_b / (block_size * associativity));
    int total_block = cache_size_b / block_size;
    int index_size = log2(total_rows);
    int tag_size = 32 - (index_size + log2(block_size));
    int overhead = (total_block * (tag_size + 1)) / 8;
    double imp_mem_size = (overhead + cache_size_b) / 1024.00;
    double cost = imp_mem_size * 0.15;

    printf("\n***** Cache Calculated Values ****\n\n");
    printf("Total # Blocks: %d\n", total_block);
    printf("Tag Size: %d bits\n", tag_size);
    printf("Index Size: %d bits\n", index_size);
    printf("Total # Rows: %d\n", total_rows);
    printf("Overhead Size: %d bytes\n", overhead);
    printf("Implementation Memory Size: %.2f KB (%.0f bytes)\n", imp_mem_size, (imp_mem_size * 1024));
    printf("Cost: $%.2f @ $0.15 / KB\n", cost);

    int physical_pages = (physical_memory_mb / 4) * 1024;
    int system_pages = physical_pages * ((double)percent_mem_used / 100);
    int page_size = 19;
    int page_ram = system_pages * page_size;

    printf("\n***** Physical Memory Calculated Values *****\n\n");
    printf("Number of Physical Pages: %d\n", physical_pages);
    printf("Number of Pages for System: %d\n", system_pages);
    printf("Size of Page Table Entry: %d bits\n", page_size);
    printf("Total RAM for Page Table(s): %d bytes\n", page_ram);

    //Address printing
    /*char line[MAX_LINE_LENGTH];
    int lines_read = 0;
    file1 = fopen(trace_files[0], "r");
    while (fgets(line, sizeof(line), file1) != NULL && lines_read < MAX_PRINT_LINES) {
        char lengthS[3];
        char address[10];
        if (line[0] == 'E')
        {
            lines_read++;
            sscanf(line, "EIP (%2s): %8s", lengthS, address);
            unsigned int hex_address;
            sscanf(address, "%x", &hex_address);
            int length = atoi(lengthS);
            printf("0x%x: (%d)\n", hex_address, length);
        }
    }*/

    // Variables for cache simulation
    int total_cache_accesses = 0;
    int instruction_bytes = 0;
    int src_dst_bytes = 0;
    int cache_hits = 0;
    int cache_misses = 0;
    int compulsory_misses = 0;
    int conflict_misses = 0;
    double cpi = 0.0;
    int bytes_accessed = 0;
    int inst_counter = 0;

    // Allocate memory for cache
    int cache_size_bytes = cache_size_kb * 1024;
    int num_cache_entries = cache_size_bytes / block_size;
    CacheEntry** cache = (CacheEntry**)malloc(num_cache_entries * sizeof(CacheEntry*));
    for (int i = 0; i < num_cache_entries; i++) {
        cache[i] = (CacheEntry*)malloc(sizeof(CacheEntry));
        cache[i]->valid = 0;
        cache[i]->tag = 0;
    }

    // Process each trace file
    for (int i = 0; i < num_trace_files; ++i) {
        FILE* file;
        char line[MAX_LINE_LENGTH];
        file = fopen(trace_files[i], "r");
        if (file == NULL) {
            printf("Error opening trace file %s\n", trace_files[i]);
            continue; // Skip to the next trace file if unable to open
        }

        // Process each line of the trace file
        while (fgets(line, sizeof(line), file) != NULL) {
            // Parse relevant information from the line
            if (line[0] == 'E')
            {
                inst_counter += 1;
                char address[9]; // Memory address is 8 characters (hexadecimal) plus null terminator
                int length;
                sscanf(line, "EIP (%d): %8s", &length, address);

                // Convert address string to hexadecimal integer
                unsigned int hex_address;
                sscanf(address, "%x", &hex_address);

                // Calculate bytes accessed based on instruction length
                 instruction_bytes += length; // Each byte is represented by two characters plus a space
                 bytes_accessed += instruction_bytes;

                // Simulate cache access and calculate CPI
                simulateCacheAccess(cache, cache_size_bytes, block_size, hex_address, &cache_hits, &compulsory_misses, &conflict_misses, &cpi);
            }
            else if (line[0] == 'd') {
                char dstM[9];
                char srcM[9];
                char trash[13];
                int sLength = 4;
                int dLength = 4;

                sscanf(line, "dstM: %9s %13s    srcM: %9s", dstM, trash, srcM);

                unsigned int dHex_address;
                sscanf(dstM, "%x", &dHex_address);

                unsigned int sHex_address;
                sscanf(srcM, "%x", &sHex_address);

                if (dHex_address != 0) {
                    src_dst_bytes += dLength;
                    simulateCacheAccess(cache, cache_size_bytes, block_size, dHex_address, &cache_hits, &compulsory_misses, &conflict_misses, &cpi);
                }

                if (sHex_address != 0) {
                    src_dst_bytes += sLength;
                    simulateCacheAccess(cache, cache_size_bytes, block_size, dHex_address, &cache_hits, &compulsory_misses, &conflict_misses, &cpi);
                }

                bytes_accessed += src_dst_bytes;
            }
        }

        fclose(file);
    }


    // Calculate cache hit rate, miss rate, CPI, unused cache space, etc.
    int total_misses = compulsory_misses + conflict_misses;
    total_cache_accesses = total_misses + cache_hits;
    double hit_rate = (((double)cache_hits * 100) / total_cache_accesses);
    double miss_rate = 100 - hit_rate;
    // Calculate unused KB
    double unused_kb = ((total_block - (double)compulsory_misses) * ((double)block_size + overhead)) /  1024.0;
    // Calculate waste
    double waste = cost * unused_kb;
    // Calculate percentage of unused cache space
    double percentage_unused = (unused_kb / ((double)total_block * ((double)block_size + overhead) / 1024.0)) * 100.0;

    //Calculate cpi
    cpi = cpi / inst_counter;
    

    // Print simulation results
    printf("\n");
    printf("***** CACHE SIMULATION RESULTS *****\n");
    printf("Total Cache Accesses: %d\n", total_cache_accesses);
    printf("Instruction Bytes: %d\t SrcDst Bytes: %d\n", instruction_bytes, src_dst_bytes);
    printf("Cache Hits: %d\n", cache_hits);
    printf("Cache Misses: %d\n", compulsory_misses + conflict_misses);
    printf("--- Compulsory Misses: %d\n", compulsory_misses);
    printf("--- Conflict Misses: %d\n", conflict_misses);
    printf("\n***** CACHE HIT & MISS RATE *****\n");
    printf("Hit Rate: %.4f%%\n", hit_rate);
    printf("Miss Rate: %.4f%%\n", miss_rate);
    printf("CPI:\t%.2f Cycles/Instruction  (%d)\n", cpi, inst_counter);
    printf("Unused Cache Space: %f% \t Waste: $%.2f\n", percentage_unused, waste);
    printf("Unused Cache Blocks: %lf", unused_kb);

    // Free allocated memory
    for (int i = 0; i < num_cache_entries; i++) {
        free(cache[i]);
    }
    free(cache);

    return 0;
}