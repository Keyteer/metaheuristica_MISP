#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <cstring>
#include <algorithm>
#include "HeuGreedy-MISP/greedy.h"
#include "HeuGreedy-MISP/NeighList.h"
#include "LocalSearch.h"

// Perturbation: randomly remove k nodes from solution and then reconstructs
void perturbation(int *solution, int &solution_size, int perturbation_factor, NeighList *nl) {

    // Determine number of nodes to remove
    int k = perturbation_factor % solution_size;
    int RemovedNodes[k];

    // Randomly select k nodes to remove
    for (int i = 0; i < k; i++) {
        int idx = rand() % solution_size;
        RemovedNodes[i] = solution[idx];
        // Remove node at idx by replacing with last element
        solution[idx] = solution[--solution_size];
    }

    // Reconstruct solution after perturbation
    // Prioritize adding nodes that weren't in the initial solution
    for (int node = 0; node < nl->n; node++) {
        if (std::find(solution, solution + solution_size, node) != solution + solution_size
            && std::find(RemovedNodes, RemovedNodes + k, node) != RemovedNodes + k
            && canAddNode(node, solution, solution_size, nl)) {

            solution[solution_size++] = node;
        }
    }
    // Fill remaining capacity from removed nodes
    for (int i = 0; i < k; i++) {
        int node = RemovedNodes[i];
        if (canAddNode(node, solution, solution_size, nl)) {
            solution[solution_size++] = node;
        }
    }
}

// Iterated Local Search for MISP
// nl [in]:                  neighborhood list
// time_limit [in]:          time limit in seconds
// perturbation_factor [in]: fraction of nodes to remove in perturbation
// returns:                  size of best MISP found
int iteratedLocalSearch(NeighList *nl, double time_limit, int perturbation_factor) {
    int n = nl->n;
    int *current_solution = new int[n];
    int *best_solution = new int[n];

    int current_size = 0;
    int best_size = 0;

    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // Generate initial solution using greedy
    current_size = greedy(n, nl, current_solution);
    memcpy(best_solution, current_solution, current_size * sizeof(int));
    best_size = current_size;

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
            memcpy(best_solution, current_solution, current_size * sizeof(int));
            best_size = current_size;
            improvements++;
        }

        // Perturbation
        perturbation(current_solution, current_size, perturbation_factor, nl);

        // Reconstruct after perturbation using local search
        localSearch(current_solution, current_size, nl, n);

        // Acceptance criterion: accept if better or equal
        if (current_size < best_size - (perturbation_factor % current_size)) {
            // If solution is too bad, restart from best
            memcpy(current_solution, best_solution, best_size * sizeof(int));
            current_size = best_size;
            perturbation(current_solution, current_size, perturbation_factor, nl);
        }

        iterations++;
    }

    delete[] best_solution;
    delete[] current_solution;

    return best_size;
}