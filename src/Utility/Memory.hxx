/*
 * Memory.hxx
 * 
 * For *nix systems, return the memory used by the system. 
 * 
 * https://stackoverflow.com/questions/63166
 */
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

namespace Memory
{
    /**
     * Extract the memory usage, assuming the values end in " Kb"
     */
    int parseLine(char* line){
        int i = strlen(line);
        const char* p = line;
        while (*p <'0' || *p > '9') p++;
        line[i-3] = '\0';
        i = atoi(p);
        return i;
    }

    /**
     * Get the current physical memory used for this process, in KB
     */
    int getPhysicalMemory() {
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];

        while (fgets(line, 128, file) != NULL){
            if (strncmp(line, "VmRSS:", 6) == 0){
                result = parseLine(line);
                break;
            }
        }
        fclose(file);
        return result;    
    }

    /**
     * Get the current virtual memory used for this process, in KB
     */
    int getVirtualMemory(){
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];

        while (fgets(line, 128, file) != NULL){
            if (strncmp(line, "VmSize:", 7) == 0){
                result = parseLine(line);
                break;
            }
        }
        fclose(file);
        return result;
    }
}
