#ifndef OMA_CPP_MYLIB_H
#define OMA_CPP_MYLIB_H

#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <list>
#include <vector>
#include <bits/stdc++.h>
#include <set>

// Returns FITNESS                 BASED ON CONFIGURATION PROFIT
int greedy1(int N_QUERIES,         // Number of Queries
            int N_INDEXES,         // Number of Indexes
            int N_CONFIGURATIONS,  // Number os Configurations
            int MEMORY,            // Max Memory
            bool ** ci_matrix,     // Configurations x Indexes Matrix
            int * i_cost,          // Indexes cost
            int * i_mem,           // Indexes memory
            int ** cq_gain,        // Configurations x Queries Gain Matrix
            bool** cq_on,          // Configurations x Queries Connection Matrix
            bool mode);            // Mode = true -> Good solutions, more time expensive
                                   // Mode = false -> Bad solutions, less time expensive

// Returns FITNESS                 BASED ON CONFIGURATION COST
int greedy2(int N_QUERIES,         // Number of Queries
            int N_INDEXES,         // Number of Indexes
            int N_CONFIGURATIONS,  // Number os Configurations
            int MEMORY,            // Max Memory
            bool ** ci_matrix,     // Configurations x Indexes Matrix
            int * i_cost,          // Indexes cost
            int * i_mem,           // Indexes memory
            int ** cq_gain,        // Configurations x Queries Gain Matrix
            bool** cq_on,          // Configurations x Queries Connection Matrix
            bool mode);            // Mode = true -> Good solutions, more time expensive
                                   // Mode = false -> Bad solutions, less time expensive

// Returns FITNESS                 BASED ON CONFIGURATION GAIN
int greedy3(int N_QUERIES,         // Number of Queries
            int N_INDEXES,         // Number of Indexes
            int N_CONFIGURATIONS,  // Number os Configurations
            int MEMORY,            // Max Memory
            bool ** ci_matrix,     // Configurations x Indexes Matrix
            int * i_cost,          // Indexes cost
            int * i_mem,           // Indexes memory
            int ** cq_gain,        // Configurations x Queries Gain Matrix
            bool** cq_on,          // Configurations x Queries Connection Matrix
            bool mode);            // Mode = true -> Good solutions, more time expensive
                                   // Mode = false -> Bad solutions, less time expensive

// Returns FITNESS                 BASED ON CONFIGURATION MEMORY
int greedy4(int N_QUERIES,         // Number of Queries
            int N_INDEXES,         // Number of Indexes
            int N_CONFIGURATIONS,  // Number os Configurations
            int MEMORY,            // Max Memory
            bool ** ci_matrix,     // Configurations x Indexes Matrix
            int * i_cost,          // Indexes cost
            int * i_mem,           // Indexes memory
            int ** cq_gain,        // Configurations x Queries Gain Matrix
            bool** cq_on,          // Configurations x Queries Connection Matrix
            bool mode);            // Mode = true -> Good solutions, more time expensive
                                   // Mode = false -> Bad solutions, less time expensive

// Returns FITNESS                 BASED ON CONFIGURATION COST * MEMORY
int greedy5(int N_QUERIES,         // Number of Queries
            int N_INDEXES,         // Number of Indexes
            int N_CONFIGURATIONS,  // Number os Configurations
            int MEMORY,            // Max Memory
            bool ** ci_matrix,     // Configurations x Indexes Matrix
            int * i_cost,          // Indexes cost
            int * i_mem,           // Indexes memory
            int ** cq_gain,        // Configurations x Queries Gain Matrix
            bool** cq_on,          // Configurations x Queries Connection Matrix
            bool mode);            // Mode = true -> Good solutions, more time expensive
                                   // Mode = false -> Bad solutions, less time expensive

// Returns FITNESS                 BASED ON CONFIGURATION GAIN / MEMORY
int greedy6(int N_QUERIES,         // Number of Queries
            int N_INDEXES,         // Number of Indexes
            int N_CONFIGURATIONS,  // Number os Configurations
            int MEMORY,            // Max Memory
            bool ** ci_matrix,     // Configurations x Indexes Matrix
            int * i_cost,          // Indexes cost
            int * i_mem,           // Indexes memory
            int ** cq_gain,        // Configurations x Queries Gain Matrix
            bool** cq_on,          // Configurations x Queries Connection Matrix
            bool mode);            // Mode = true -> Good solutions, more time expensive
                                   // Mode = false -> Bad solutions, less time expensive

// Returns FITNESS                 BASED ON CONFIGURATION PROFIT / MEMORY
int greedy7(int N_QUERIES,         // Number of Queries
            int N_INDEXES,         // Number of Indexes
            int N_CONFIGURATIONS,  // Number os Configurations
            int MEMORY,            // Max Memory
            bool ** ci_matrix,     // Configurations x Indexes Matrix
            int * i_cost,          // Indexes cost
            int * i_mem,           // Indexes memory
            int ** cq_gain,        // Configurations x Queries Gain Matrix
            bool** cq_on,          // Configurations x Queries Connection Matrix
            bool mode);            // Mode = true -> Good solutions, more time expensive
                                   // Mode = false -> Bad solutions, less time expensive

// Returns FITNESS                 RANDOM CONFIGURATION SELECTION
int random (int N_QUERIES,         // Number of Queries
            int N_INDEXES,         // Number of Indexes
            int N_CONFIGURATIONS,  // Number os Configurations
            int MEMORY,            // Max Memory
            bool ** ci_matrix,     // Configurations x Indexes Matrix
            int * i_cost,          // Indexes cost
            int * i_mem,           // Indexes memory
            int ** cq_gain,        // Configurations x Queries Gain Matrix
            bool** cq_on,          // Configurations x Queries Connection Matrix
            bool mode);

#endif //OMA_CPP_MYLIB_H