#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "HeuGreedy-MISP/NeighList.h"

// Check if a node can be added to the independent set
bool canAddNode(int node, int *solution, int solution_size, NeighList *nl) {
    for (int i = 0; i < solution_size; i++) {
        int v = solution[i];
        // Check if node is adjacent to any node in current solution
        struct Neighbor *neighbor = nl->neighborhoods[v];
        while (neighbor != nullptr) {
            if (neighbor->node == node) {
                return false;
            }
            neighbor = neighbor->next;
        }
    }
    return true;
}

// Local Search: Try to improve solution by swapping nodes
// Returns true if improvement was made
bool localSearch(int *solution, int &solution_size, NeighList *nl, int n) {
    bool improved = false;
    bool *inSolution = new bool[n];

    // Mark nodes in solution
    memset(inSolution, false, n * sizeof(bool));
    for (int i = 0; i < solution_size; i++) {
        inSolution[solution[i]] = true;
    }

    // Try to add nodes not in solution
    for (int node = 0; node < n; node++) {
        if (!inSolution[node] && canAddNode(node, solution, solution_size, nl)) {
            solution[solution_size++] = node;
            inSolution[node] = true;
            improved = true;
        }
    }

    delete[] inSolution;
    return improved;
}

// Perturbation: randomly remove k nodes from solution
void perturbation(int *solution, int &solution_size, int perturbation_factor) {

    // Determine number of nodes to remove
    int k = perturbation_factor % solution_size;

    // Randomly select k nodes to remove
    for (int i = 0; i < k; i++) {
        int idx = rand() % solution_size;
        // Remove node at idx by swapping with last element
        solution[idx] = solution[solution_size - 1];
        solution_size--;
    }
}