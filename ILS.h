#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "HeuGreedy-MISP/greedy.h"
#include "HeuGreedy-MISP/NeighList.h"
#include "LocalSearch.h"

// Iterated Local Search for MISP
// nl [in]:              neighborhood list
// time_limit [in]:      time limit in seconds
// perturbation_size [in]: number of nodes to remove in perturbation
// returns:              size of best MISP found
int iteratedLocalSearch(NeighList *nl, double time_limit, int perturbation_size = 3) {
    int n = nl->n;
    int *current_solution = new int[n];
    int *best_solution = new int[n];
    int *temp_solution = new int[n];
    
    int current_size = 0;
    int best_size = 0;
    
    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Generate initial solution using greedy
    current_size = greedy(n, nl, current_solution);
    copySolution(current_solution, current_size, best_solution, best_size);
    
    int iterations = 0;
    int improvements = 0;
    
    // Main ILS loop
    while (true) {
        // Check time limit
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = current_time - start_time;
        if (elapsed.count() >= time_limit) {
            break;
        }
        
        // Local search on current solution
        bool improved = localSearch(current_solution, current_size, nl, n);
        
        // Update best solution if improved
        if (current_size > best_size) {
            copySolution(current_solution, current_size, best_solution, best_size);
            improvements++;
        }
        
        // Perturbation
        copySolution(current_solution, current_size, temp_solution, current_size);
        perturbation(current_solution, current_size, perturbation_size);
        
        // Reconstruct after perturbation using local search
        localSearch(current_solution, current_size, nl, n);
        
        // Acceptance criterion: accept if better or equal
        if (current_size < best_size - perturbation_size) {
            // If solution is too bad, restart from best
            copySolution(best_solution, best_size, current_solution, current_size);
            perturbation(current_solution, current_size, perturbation_size);
        }
        
        iterations++;
    }
    
    // Copy best solution to current_solution for output
    copySolution(best_solution, best_size, current_solution, current_size);
    
    delete[] temp_solution;
    delete[] best_solution;
    
    return best_size;
}