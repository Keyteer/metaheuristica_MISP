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

        // Remove node at idx by replacing with last element
        sol->solution[idx] = sol->solution[sol->size - 1];
        sol->size--;
    }

    // Reset MISP_Independent array based on current solution
    memset(sol->MISP_Independent, true, sol->graph->n * sizeof(bool));
    for (int i = 0; i < sol->size; i++) {
        int node = sol->solution[i];
        sol->MISP_Independent[node] = false;
        // Mark neighbors as not independent
        for (Neighbor *N = sol->graph->neighborhoods[node]; N != nullptr; N = N->next) {
            sol->MISP_Independent[N->node] = false;
        }
    }


    // std::cout << sol->size << " ";


    // Reconstruct solution after perturbation
    // Prioritize adding nodes that weren't in the initial solution
    for (int node = 0; node < sol->graph->n; node++) {
        if (std::find(removedNodes, removedNodes + k, node) == removedNodes + k
            && sol->MISP_Independent[node]) {
            sol->addNode(node);
        }
    }
    // Fill remaining capacity from removed nodes
    for (int i = 0; i < k; i++) {
        int node = removedNodes[i];
        if (sol->MISP_Independent[node]) {
            sol->addNode(node);
        }
    }


    // std::cout << sol->size << " ";

    delete[] removedNodes;
}

// Copy solution from one MISP_Solution to another
void copySolution(MISP_Solution *source, MISP_Solution *dest) {
    dest->size = source->size;
    memcpy(dest->solution, source->solution, source->size * sizeof(int));
    memcpy(dest->MISP_Independent, source->MISP_Independent, source->graph->n * sizeof(bool));
}

// Iterated Local Search for MISP
// nl [in]:                  neighborhood list
// time_limit [in]:          time limit in seconds
// perturbation_factor [in]: number of nodes to remove in perturbation
// returns:                  size of best MISP found
int iteratedLocalSearch(NeighList *nl, double time_limit, int perturbation_factor) {
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

    int iterations = 0;
    int improvements = 0;
    int LocalSearcheImprovements = 0;

    // Main ILS loop
    while (true) {
        // Check time limit
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = current_time - start_time;
        if (elapsed.count() >= time_limit) {
            // printf("\nTotal Iterations: %d, Improvements: %d, Local Search Improvements: %d\n", iterations, improvements, LocalSearcheImprovements);
            break;
        }

        // Perturbation
        perturbation(current_solution, perturbation_factor);

        // Local search on current solution
        if(localSearch(current_solution)) {

        }

        // Update best solution if improved
        if (current_solution->size > best_solution->size) {
            copySolution(current_solution, best_solution);
            improvements++;
        }

        // std::cout << current_solution->size << " ";

        iterations++;
    }

    int best_size = best_solution->size;

    delete current_solution;
    delete best_solution;

    return best_size;
}