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
