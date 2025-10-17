#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "utils.h"

// Local Search: Try to improve solution by adding independent nodes
// Returns true if improvement was made
bool localSearch(MISP_Solution *sol) {
    bool improved = false;
    int best_size = sol->size;

    // Try to make a 2 for 1 swap
    for (int node = 0; node < sol->graph->n; node++) {
        if (sol->MISP_Independent[node]) {
            // Find a node in the solution to remove
            for (int i = 0; i < sol->size; i++) {
                int removed = sol->solution[i];
                sol->removeNode(removed);
                sol->addNode(node);
                if (sol->size > best_size) {
                    best_size = sol->size;
                    improved = true;
                }
                sol->removeNode(node);
                sol->addNode(removed);
            }
        }
    }

    return improved;
}
