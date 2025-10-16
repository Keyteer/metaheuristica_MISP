#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <ctime>
#include "ILS.h"
#include "HeuGreedy-MISP/loader.h"

int main(int argc, char *argv[]) {
    // Default parameters
    char *instance_file = nullptr;
    double time_limit = 60.0; // default 60 seconds
    int perturbation_size = 3; // default perturbation size
    int seed = time(nullptr);
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            instance_file = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            time_limit = atof(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            perturbation_size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            seed = atoi(argv[++i]);
        }
    }
    
    // Validate parameters
    if (instance_file == nullptr) {
        fprintf(stderr, "Usage: %s -i <instance_file> -t <time_limit_seconds> [-p <perturbation_size>] [-s <random_seed>]\n", argv[0]);
        fprintf(stderr, "\nParameters:\n");
        fprintf(stderr, "  -i <instance_file>        : Path to the graph instance file (required)\n");
        fprintf(stderr, "  -t <time_limit_seconds>   : Maximum execution time in seconds (required)\n");
        fprintf(stderr, "  -p <perturbation_size>    : Number of nodes to remove in perturbation (default: 3)\n");
        fprintf(stderr, "  -s <random_seed>          : Random seed for reproducibility (default: current time)\n");
        return 1;
    }
    
    if (time_limit <= 0) {
        fprintf(stderr, "Error: Time limit must be positive\n");
        return 1;
    }
    
    // Set random seed
    srand(seed);
    
    printf("=== Iterated Local Search for MISP ===\n");
    printf("Instance: %s\n", instance_file);
    printf("Time limit: %.2f seconds\n", time_limit);
    printf("Perturbation size: %d\n", perturbation_size);
    printf("Random seed: %d\n", seed);
    printf("======================================\n\n");
    
    // Load graph
    NeighList *nl = loadGraph(instance_file);
    if (nl == nullptr) {
        fprintf(stderr, "Error: Could not load graph from file: %s\n", instance_file);
        return 1;
    }
    
    printf("Graph loaded: %d nodes\n\n", nl->n);
    
    // Run ILS
    auto start = std::chrono::high_resolution_clock::now();
    int misp_size = iteratedLocalSearch(nl, time_limit, perturbation_size);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> elapsed = end - start;
    printf("\nTotal execution time: %.2f seconds\n", elapsed.count());
    
    // Cleanup
    delete nl;
    
    return 0;
}
