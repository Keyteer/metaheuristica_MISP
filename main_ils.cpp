#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <ctime>
#include <sys/stat.h>
#include "ILS.h"
#include "HeuGreedy-MISP/loader.h"

// Function to filter and sort files based on naming convention
// Sorted by density from 0.1 to 0.9
// fileNames [in/out]: array of file names
// fileCount [in]: number of files in fileNames
// returns: number of files that match the naming convention
int filterFiles(char **fileNames, int fileCount) {
    int matchCount = 0;
    for (int i = 1; i <= 9; i++) {
        for (int j = 0; j < fileCount; j++) {

            // selecting files with correct nomenclature and density [0.1, 0.2, ... , 0.9]
            char densStr[10];
            sprintf(densStr, "p0c0.%d_", i + 1);
            if (strstr(fileNames[j], densStr) != nullptr) {

                // Swap to front
                char *temp = fileNames[matchCount];
                fileNames[matchCount] = fileNames[j];
                fileNames[j] = temp;

                matchCount++;
            }
        }
    }
    return matchCount;
}

int main(int argc, char *argv[]) {
    // Default parameters
    char *path = nullptr;
    double time_limit = 10.0; // default time limit seconds
    int perturbation_factor = 20; // default perturbation factor

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            path = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            time_limit = atof(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            perturbation_factor = atof(argv[++i]);
        }
    }

    // Validate parameters
    if (path == nullptr) {
        fprintf(stderr, "Usage: %s -i <path> [-t <time_limit_seconds>] [-p <perturbation>]\n", argv[0]);
        fprintf(stderr, "\nParameters:\n");
        fprintf(stderr, "  -i <path>        : Path to the graph instance/s file/directory (required)\n");
        fprintf(stderr, "  -t <time_limit_seconds>   : Maximum execution time in seconds (default: %.2f)\n", time_limit);
        fprintf(stderr, "  -p <perturbation_factor>    : Fraction of nodes to remove in perturbation step (default: %d)\n", perturbation_factor);
        return 1;
    }

    if (time_limit <= 0) {
        fprintf(stderr, "Error: Time limit must be positive\n");
        return 1;
    }

    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        perror("Error accessing path");
        return 1;
    }

    char **fileNames;
    int fileCount = 0;


    if (S_ISDIR(path_stat.st_mode)) {
        // directory
        fileCount = getAllFiles(path, fileNames);
        fileCount = filterFiles(fileNames, fileCount);
        if (fileCount <= 0) {
            fprintf(stderr, "Error: No files found in directory: %s\n", path);
            return 1;
        }
    } else {
        // single file case
        // remove fileName from path and store it in fileNames[0]
        fileNames = (char **)malloc(1 * sizeof(char *));

        char *base = strstr(path, "/erdos");
        if (base == nullptr) {
            fprintf(stderr, "Error: File name does not follow naming convention\n");
            return 1;
        }

        base++[0] = '\0'; // null-terminate path and increment pointer to skip '/'
        fileNames[0] = (char *)malloc(strlen(base) + 1 * sizeof(char));
        strcpy(fileNames[0], base);

        fileCount = 1;
    }


    // print csv header
    printf("Density,Avg_MISP_Size\n");


    // variables
    int lastDensityDecimal = 0;
    int tests = 0;
    double avgResult = 0.0;


    for (int i = 0; i < fileCount; i++) {

        // get full path
        char *file = fileNames[i];
        char *fullPath = new char[strlen(path) + strlen(file) + 2];
        sprintf(fullPath, "%s/%s", path, file);

        // check density, if new density reset counters and print newline
        int currentDensityDecimal;
        sscanf(file, "%*[^.].%d_", &currentDensityDecimal);
        if (currentDensityDecimal != lastDensityDecimal && i != 0) {
            lastDensityDecimal = currentDensityDecimal;
            avgResult = 0.0;
            tests = 0;
            printf("\n");
        }

        // Load graph
        NeighList *nl = loadGraph(fullPath);
        if (nl == nullptr) {
            fprintf(stderr, "Error: Could not load graph from file: %s\n", fullPath);
            return 1;
        }

        // Run ILS
        int misp_size = iteratedLocalSearch(nl, time_limit, perturbation_factor);

        // accumulate results pondered by number of tests
        avgResult = (avgResult * tests + misp_size) / ++tests;

        // print current average results
        printf("\r0.%d,%f   ", currentDensityDecimal, avgResult);

        // Cleanup
        delete nl;
    }

    printf("\n");

    return 0;
}
