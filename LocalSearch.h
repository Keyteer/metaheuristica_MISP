#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "utils.h"

// Local Search: Try to improve solution by adding independent nodes
// strafe: Do 1 for 1 swaps
// Returns true if improvement was made
bool localSearch(MISP_Solution *sol, bool strafe = false) {
    bool improved = false;
    int best_size = sol->size;

    // Try to make a 2 for 1 swap
    for (int i = 0; i < sol->size; i++) {
        int curr_node = sol->solution[i];
        int addableNeighbors_count = 0;
        int addableNeighbors[sol->graph->degrees[curr_node]];
        for (Neighbor *N = sol->graph->neighborhoods[curr_node]; N != nullptr; N = N->next) {
            if (sol->MISP_IndependentDegree[N->node] == 1) {
                addableNeighbors[addableNeighbors_count++] = N->node;
            }
        }
        if (addableNeighbors_count > 1 || (strafe && addableNeighbors_count == 1)) {
            // Found a candidate for swap
            sol->removeNode(curr_node);
            for (int j = 0; j < addableNeighbors_count; j++) {
                if (sol->MISP_IndependentDegree[addableNeighbors[j]] == 0) {
                    sol->addNode(addableNeighbors[j]);
                }
            }
        }
    }

    if (sol->size > best_size) {
        improved = true;
    }

    return improved;
}
