#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "HeuGreedy-MISP/greedy.h"
#include "utils.h"
#include "LocalSearch.h"

// Perturbation: randomly remove k nodes from solution
void perturbation(MISP_Solution *sol, int perturbation_factor) {
    // Determine number of nodes to remove
    int k = std::min(sol->size * perturbation_factor / 100, sol->size);
    if (k <= 0) return;

    int *removedNodes = new int[k];

    // Randomly select k nodes to remove
    for (int i = 0; i < k; i++) {
        int idx = rand() % sol->size;
        removedNodes[i] = sol->solution[idx];

        // Remove node from solution
        sol->removeNode(removedNodes[i]);
    }

    // Reconstruct solution after perturbation
    // Prioritize adding nodes that weren't in the initial solution
    for (int node = 0; node < sol->graph->n; node++) {
        if (std::find(removedNodes, removedNodes + k, node) == removedNodes + k
            && sol->MISP_IndependentDegree[node] == 0) {
            sol->addNode(node);
        }
    }
    // Fill remaining capacity from removed nodes
    for (int i = 0; i < k; i++) {
        int node = removedNodes[i];
        if (sol->MISP_IndependentDegree[node] == 0) {
            sol->addNode(node);
        }
    }
    delete[] removedNodes;
}

// Copy solution from one MISP_Solution to another
void copySolution(MISP_Solution *source, MISP_Solution *dest) {
    dest->size = source->size;
    memcpy(dest->solution, source->solution, source->size * sizeof(int));
    memcpy(dest->MISP_IndependentDegree, source->MISP_IndependentDegree, source->graph->n * sizeof(bool));
}

// Iterated Local Search for MISP
// nl [in]:                  neighborhood list
// time_limit [in]:          time limit in seconds
// perturbation_factor [in]: number of nodes to remove in perturbation
// returns:                  size of best MISP found
int iteratedLocalSearch(NeighList *nl, double time_limit, int perturbation_factor, int *iterations = nullptr, bool verbose = false) {
    // Create MISP solution structures
    MISP_Solution *current_solution = new MISP_Solution(nl);
    MISP_Solution *best_solution = new MISP_Solution(nl);

    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // Generate initial solution using greedy
    int *temp_array = new int[nl->n];
    int initial_size = greedy(nl->n, nl, temp_array);

    // Copy greedy solution into current_solution
    for (int i = 0; i < initial_size; i++) {
        current_solution->addNode(temp_array[i]);
    }
    delete[] temp_array;

    // Copy to best solution
    copySolution(current_solution, best_solution);

    int iter_count = 0;
    int improvements = 0;
    int LocalSearchImprovements = 0;

    if (verbose) std::cout << "Initial solution size: " << best_solution->size << std::endl;

    // Main ILS loop
    while (true) {
        // Check time limit
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = current_time - start_time;
        if (elapsed.count() >= time_limit) {

            if (verbose) std::cout << "Time limit reached. Ending ILS." << std::endl;

            break;
        }

        // Perturbation
        perturbation(current_solution, perturbation_factor);

        // Local search on current solution
        if (localSearch(current_solution)) LocalSearchImprovements++;


        // Update best solution if improved
        if (current_solution->size > best_solution->size) {
            copySolution(current_solution, best_solution);
            improvements++;

            if (verbose) std::cout << "New best solution size: " << best_solution->size
                << " at iteration " << iter_count
                << " time: " << elapsed.count() << "s" << std::endl;
        }
        iter_count++;
    }

    // Store iteration count if pointer provided
    if (iterations != nullptr) {
        *iterations = iter_count;
    }

    int best_size = best_solution->size;

    delete current_solution;
    delete best_solution;

    return best_size;
}