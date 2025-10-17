#pragma once

#include <cstring>
#include "HeuGreedy-MISP/NeighList.h"


struct MISP_Solution {
    NeighList *graph;
    int size;
    int *solution;
    int *MISP_IndependentDegree;

    MISP_Solution(NeighList *nl) {
        graph = nl;
        size = 0;
        solution = new int[nl->n];
        MISP_IndependentDegree = new int[nl->n];
        memset(MISP_IndependentDegree, 0, nl->n * sizeof(int));
    }
    MISP_Solution(NeighList *nl, int *nodes, int sz) {
        graph = nl;
        size = 0;
        solution = new int[nl->n];
        MISP_IndependentDegree = new int[nl->n];
        memset(MISP_IndependentDegree, 0, nl->n * sizeof(int));

        // Add nodes to solution
        for (int i = 0; i < sz; i++) {
            addNode(nodes[i]);
        }
    }
    ~MISP_Solution() {
        delete[] MISP_IndependentDegree;
        delete[] solution;
    }

    void addNode(int node) {
        if (MISP_IndependentDegree[node] > 0) {
            std::cerr << "Error: Trying to add a non-independent node" << node << "to MISP_Solution";
            return;
        }

        solution[size++] = node;
        MISP_IndependentDegree[node] = -1;
        for (Neighbor *N = graph->neighborhoods[node]; N != nullptr; N = N->next) {
            MISP_IndependentDegree[N->node] += 1;
        }
    }
    void removeNode(int node) {
        // Find and remove node from solution
        int *found = std::find(solution, solution + size, node);
        if (found < solution + size) {
            // Node found, remove it
            *found = solution[--size];
        } else {
            std::cerr << "Error: Trying to remove a non-existing node" << node << "from MISP_Solution";
            return;
        }

        MISP_IndependentDegree[node] = 0;
        // Re-evaluate independence of neighbors
        for (Neighbor *N = graph->neighborhoods[node]; N != nullptr; N = N->next) {
            MISP_IndependentDegree[N->node] -= 1;
        }
    }
};