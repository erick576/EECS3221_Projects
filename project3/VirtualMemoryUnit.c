#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

/*
 * Specifications
 * 2^8 entries in the page table
 * Page size of 2^8 bytes
 * 16 entries in the TLB
 * Frame size of 2^8 bytes
 * 256 frames
 * Physical memory of 65,536 bytes (256 frames × 256-byte frame size)
 *
 * Your program will read a file containing several 32-bit integer numbers that represent logical addresses.
 * However, you need only be concerned with 16-bit addresses,
 * so you must mask (i.e., ignore) the high-order 16 bits of each logical address.
 * The low-order 16 bits are divided into (1) an 8-bit page number and (2) an 8-bit page offset.
 */

double total = 0;
double hit_count = 0;
double fault_count = 0;
int Page_Faults_Rate = 0;
int TLB_Hits_Rate = 0;
int page_number = 0;
int frame_number = 0;
int curr_frame_index = 0;
int offset = 0;
int curr_index = 0;
int size;

// Main Structures
int tlb[16][2];

// Check if page number can exists on TLB if not it is a miss
int access_TLB(int val)
{
    for (int i = 0; i < 16; i++)
    {
        if (val == tlb[i][0]) return tlb[i][1]; // Found Page Number
    }
    // Miss
    return -1;
}
// Add page mapping to TLB (FIFO Replacement Policy. Use Circular Index for this cause)
void add_to_tlb(int page_val, int frame_val)
{
    tlb[curr_index][0] = page_val;
    tlb[curr_index][1] = frame_val;

    curr_index++;
    if (curr_index >= 16){
        curr_index = 0;
    }
}

int main(int argc, char *argv[])
{
    FILE* in;
    FILE* out;
    FILE* backing_store;

    if (argc != 4){
        exit(0);
    }

    size = atoi(argv[1]);
    char physical_memory[size][256];
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 256; j++) {
            physical_memory[i][j] = 0;
        }
    }

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 2; j++) {
            tlb[i][j] = -1;
        }
    }

    int page_table[size][3];
    for (int i = 0; i < size; i++) {
        page_table[i][0] = -1; // Page
        page_table[i][1] = -1; // Frame
        page_table[i][2] = 0; // Counter
    }

    // Open Files to read and write from
    backing_store = fopen(argv[2], "rb");
    in = fopen(argv[3],"r");
    char* out_filename = malloc(sizeof ("output") + sizeof(argv[1]) + sizeof (".csv"));
    strcpy(out_filename, "output");
    strcat(out_filename, argv[1]);
    strcat(out_filename, ".csv");
    out = fopen(out_filename,"w");

    if (in == NULL || out == NULL || backing_store == NULL)
    {
        exit(0);
    }

    // Now read the memory addresses in the text file line by line
    char parse_line[10];
    int address;

    while (fgets(parse_line, sizeof(parse_line), in)) {

        address = atoi(parse_line);
        // Shift and mask bits to get specific range of bits for page number
        page_number = (address >> 8) & 0xFF;
        // Mask bits to get lowest 8
        offset = address & 0xFF;

        // Now check if it is a TLB hit or miss first
        if (access_TLB(page_number) != -1)
        {
            hit_count++; // Increment hit count
            frame_number = access_TLB(page_number);

            // Move accessed page to the top
            for (int i = 0; i < size; i++) {
                if (page_table[i][0] == page_number) {
                    page_table[i][2] = 0; // Reset Count for access
                }
            }

        } else { // TLB Miss
            int val = -1;
            int index = -1;
            for (int i = 0; i < size; i++) {
                if (page_table[i][0] == page_number) {
                    val = page_table[i][1];
                    index = i;
                }
            }

            // Access the page table now
            if (val != -1) // Found In the Page table
            {
                // Update TLB
                frame_number = val;
                add_to_tlb(page_number, frame_number);

                // Move accessed page to the top
                page_table[index][2] = 0; // Reset Count for access

            } else { // Page Fault Occurs
                // Increment Fault Count
                fault_count ++;

                if (curr_frame_index == -1)
                {
                    // Page Replacement
                    // Search for the lru
                    int max = INT_MIN;
                    for (int i = 0; i < size; i++) {
                        if (page_table[i][2] > max){
                            max = page_table[i][2];
                        }
                    }
                    for (int i = 0; i < size; i++) {
                        if (page_table[i][2] == max) {
                            frame_number = i;
                            break;
                        }
                    }

                    // Access page address in the backing store
                    int page_address = page_number * 256;
                    if (fseek(backing_store, page_address, SEEK_SET) != 0)
                    {
                        // Error
                        exit(0);
                    }

                    // read and store in page frame in physical memory
                    // signed char == signed byte for third column
                    if (fread(physical_memory[frame_number], 1, 256, backing_store) == 0)
                    {
                        // Error Nothing read
                        exit(0);
                    }

                    // Update Page Table
                    page_table[frame_number][0] = page_number;
                    page_table[frame_number][1] = frame_number;
                    page_table[frame_number][2] = 0;

                } else {
                    // Memory is not full yet
                    // Get the current index in physical mem
                    frame_number = curr_frame_index;

                    // Access page address in the backing store
                    int page_address = page_number * 256;
                    if (fseek(backing_store, page_address, SEEK_SET) != 0)
                    {
                        // Error
                        exit(0);
                    }

                    // read and store in page frame in physical memory
                    // signed char == signed byte for third column
                    if (fread(physical_memory[frame_number], 1, 256, backing_store) == 0)
                    {
                        // Error Nothing read
                        exit(0);
                    }

                    // Update Page Table
                    page_table[frame_number][0] = page_number;
                    page_table[frame_number][1] = frame_number;
                    page_table[frame_number][2] = 0;

                    // Increment Frame index
                    curr_frame_index ++;
                    if (curr_frame_index >= size)
                    {
                        curr_frame_index = -1;
                    }

                }
                index = -1;
                for (int i = 0; i < size; i++) {
                    if (page_table[i][0] == page_number) {
                        index = i;
                    }
                }
                // Update TLB
                add_to_tlb(page_number, page_table[index][1]);
            }
        }

        // Increment Counters
        for (int i = 0; i < size; i++) {
            if (page_table[i][1] != -1){
                page_table[i][2] ++;
            }
        }

        total ++;  // Add to total operations

        // Add row
        int val = physical_memory[frame_number][offset];
        int phys = frame_number * 256 + offset;
        fprintf(out, "%d,%d,%d\n", address, phys, val);
    }
    // Write satistics
    char percentage = '%';
    fprintf(out, "Page Faults Rate, %.2f%c,", (fault_count / total) * 100, percentage);
    fprintf(out, "\n");
    fprintf(out, "TLB Hits Rate, %.2f%c,", (hit_count / total) * 100, percentage);

    // Close files
    fclose(backing_store);
    fclose(in);
    fclose(out);

    return 0;
}