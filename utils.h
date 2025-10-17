#pragma once

#include <cstring>
#include "HeuGreedy-MISP/NeighList.h"

struct MISP_Solution {
    NeighList *graph;
    int size;
    int *solution;
    bool *MISP_Independent;
    
    MISP_Solution(NeighList *nl) {
        graph = nl;
        size = 0;
        solution = new int[nl->n];
        MISP_Independent = new bool[nl->n];
        memset(MISP_Independent, true, nl->n * sizeof(bool));
    }
    MISP_Solution(NeighList *nl, int *nodes, int sz) {
        graph = nl;
        size = 0;
        solution = new int[nl->n];
        MISP_Independent = new bool[nl->n];
        memset(MISP_Independent, true, nl->n * sizeof(bool));
        
        // Add nodes to solution
        for (int i = 0; i < sz; i++) {
            addNode(nodes[i]);
        }
    }
    ~MISP_Solution() {
        delete[] MISP_Independent;
        delete[] solution;
    }

    void addNode(int node) {
        solution[size++] = node;
        MISP_Independent[node] = false;
        for (Neighbor *N = graph->neighborhoods[node]; N != nullptr; N = N->next) {
            MISP_Independent[N->node] = false;
        }
    }
    void removeNode(int node) {
        // Find and remove node from solution
        for (int i = 0; i < size; i++) {
            if (solution[i] == node) {
                solution[i] = solution[--size];
                break;
            }
        }
        MISP_Independent[node] = true;
        // Re-evaluate independence of neighbors
        for (Neighbor *N = graph->neighborhoods[node]; N != nullptr; N = N->next) {
            bool independent = true;
            for (int j = 0; j < size; j++) {
                if (solution[j] == N->node) {
                    independent = false;
                    break;
                }
            }
            MISP_Independent[N->node] = independent;
        }
    }
};