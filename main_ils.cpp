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
            sprintf(densStr, "p0c0.%d_", i);
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
    bool tuning = false;

    // Parse required command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            path = argv[++i];
        }
    }

    // Validate parameters
    if (path == nullptr) {
        fprintf(stderr, "Usage: %s -i <path> [-t <time_limit_seconds>] [-p <perturbation>] [--tuning]\n", argv[0]);
        fprintf(stderr, "\nParameters:\n");
        fprintf(stderr, "  -i <path>                    : Path to the graph instance/s file/directory (required)\n");
        fprintf(stderr, "  -t <time_limit_seconds>      : Maximum execution time in seconds (default: %.2f)\n", time_limit);
        fprintf(stderr, "  -p <perturbation_factor>     : Fraction of nodes to remove in perturbation step (default: %d)\n", perturbation_factor);
        fprintf(stderr, "  --tuning                     : Enable tuning mode (default: false)\n");
        return 1;
    }

    // Parse optional command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            time_limit = atof(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            perturbation_factor = atof(argv[++i]);
        } else if (strcmp(argv[i], "--tuning") == 0) {
            tuning = true;
        }
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
        // Run single file with verbose parameter (unless tuning)

        NeighList *nl = loadGraph(path);
        if (nl == nullptr) {
            fprintf(stderr, "Error: Could not load graph from file: %s\n", path);
            return 1;
        }

        if (tuning) {
            int result = iteratedLocalSearch(nl, time_limit, perturbation_factor, nullptr, false);
            printf("%d", -result);
        } else
            iteratedLocalSearch(nl, time_limit, perturbation_factor, nullptr, true);

        delete nl;

        return 0;
    }


    // print csv header
    printf("Density,Tests,Avg_MISP_Size,Avg_Time(s),Avg_Iterations\n");


    // variables
    int lastDensityDecimal = 0;
    int tests = 0;
    double avgResult = 0.0;
    double avgTime = 0.0;
    double avgIterations = 0.0;


    for (int i = 0; i < fileCount; i++) {

        // get full path
        char *file = fileNames[i];
        char *fullPath = new char[strlen(path) + strlen(file) + 2];
        sprintf(fullPath, "%s/%s", path, file);

        // check density, if new density reset counters and print newline
        int currentDensityDecimal;
        sscanf(file, "%*[^.].%d_", &currentDensityDecimal);
        if (currentDensityDecimal != lastDensityDecimal) {
            lastDensityDecimal = currentDensityDecimal;
            if (i != 0) {
                avgResult = 0.0;
                avgTime = 0.0;
                avgIterations = 0.0;
                tests = 0;
                printf("\n");
            }
        }

        // Load graph
        NeighList *nl = loadGraph(fullPath);
        if (nl == nullptr) {
            fprintf(stderr, "Error: Could not load graph from file: %s\n", fullPath);
            return 1;
        }

        int iterations;

        // Run ILS and measure time
        auto start = std::chrono::high_resolution_clock::now();
        int misp_size = iteratedLocalSearch(nl, time_limit, perturbation_factor, &iterations);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        double execution_time = elapsed.count();

        // accumulate results pondered by number of tests
        avgResult = (avgResult * tests + misp_size) / (tests + 1);
        avgTime = (avgTime * tests + execution_time) / (tests + 1);
        avgIterations = (avgIterations * tests + iterations) / (tests + 1);
        tests++;

        // print current average results
        printf("\r0.%d,%d,%.2f,%.4f,%.0f   ", currentDensityDecimal, tests, avgResult, avgTime, avgIterations);
        fflush(stdout);

        // Cleanup
        delete nl;
    }

    printf("\n");

    return 0;
}
