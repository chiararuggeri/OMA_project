#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "mylib.h"

#include <list>
#include <vector>
#include <bits/stdc++.h>
#include <set>

#include <fstream>
#include <string>
#include <ctime>
#include <map>
#include <list>

typedef struct solution{
    bool** matrix;
    int fitness;
} sol;

list<int> *configIndexes;
int * indexesCost;
int * indexesMem;
int ** configGain;

// global variables
int N_QUERIES = 0;
int N_INDEXES = 0;
int N_CONFIGURATIONS = 0;
int MEMORY = 0;

bool operator < (const solution& sol1, const solution& sol2)
{
    return sol1.fitness > sol2.fitness;
}

// best current solution
int bestFitnessFoundSoFar = 0;

/* FUNCTION PROTOTYPE */
set<solution> initial_population(int N_QUERIES,         // Number of Queries
                                 int N_INDEXES,         // Number of Indexes
                                 int N_CONFIGURATIONS,  // Number os Configurations
                                 int N_POPULATION,      // Number of Population
                                 int MEMORY,            // Max Memory
                                 list<int> *ci_matrix,  // Configurations x Indexes Matrix
                                 int *i_cost,           // Indexes cost
                                 int *i_mem,            // Indexes memory
                                 int **cq_gain,         // Configurations x Queries Gain Matrix
                                 bool mode);            // Mode = true -> Good solutions, more time expensive | Use mode = true as default
                                                        // Mode = false -> Bad solutions, less time expensive | mode = false only if really needed

int main()
{
    ifstream instanceFile;
    instanceFile.open("instances/instance01.odbdp");
    if (!instanceFile.is_open()) {
        // ERROR - Instance file not found
        cout << "Specified instance file cannot not be found.";
        exit(0);
    }
    string line;
    string number;

    // read first line
    getline(instanceFile, line);
    // get queries number from the first line
    number = line.substr(11);
    // reference to stoi function: http://www.cplusplus.com/reference/string/stoi/
    N_QUERIES = stoi(number, nullptr);

    // read second line and get indexes number from it
    getline(instanceFile, line);
    number = line.substr(11);
    N_INDEXES = stoi(number, nullptr);

    // read third line and get configurations number from it
    getline(instanceFile, line);
    number = line.substr(18);
    N_CONFIGURATIONS = stoi(number, nullptr);

    // read fourth line and get memory constraint from it
    getline(instanceFile, line);
    number = line.substr(8);
    MEMORY = stoi(number, nullptr);

    // print retrieved data
    cout << "number of queries: " << N_QUERIES << endl << "number of indexes: " << N_INDEXES << endl << "number of configurations: " << N_CONFIGURATIONS << endl << "memory: " << MEMORY << endl;

    // remove CONFIGURATIONS_INDEXES_MATRIX: row
    getline(instanceFile, line);

    // read CONFIGURATIONS_INDEXES_MATRIX and for each configuration save a list of its indexes
    configIndexes = new list<int>[N_CONFIGURATIONS];
    for (int i = 0; i < N_CONFIGURATIONS; i++) { // for each row
        list<int> indexForConf;
        getline(instanceFile, line);
        for (unsigned int j = 0; j < line.size(); j += 2) { // for each element in the row (+2 to skip spaces)
            if (line[j] == '1') {
                indexForConf.push_front(j / 2);
                //printf("%d ",indexForConf.front());
            }
        }
        configIndexes[i] = indexForConf;
        //printf("\n");
    }

    // remove INDEXES_FIXED_COST: row
    getline(instanceFile, line);

    // read INDEXES_FIXED_COST and save cost values in an array
    indexesCost = (int*)malloc(sizeof(int) * N_INDEXES);
    for (int i = 0; i < N_INDEXES; i++) {
        getline(instanceFile, line);
        indexesCost[i] = stoi(line, nullptr);
    }

    // remove INDEXES_MEMORY_OCCUPATION: row
    getline(instanceFile, line);

    // read INDEXES_MEMORY_OCCUPATION and save memory values in an array
    indexesMem = (int*)malloc(sizeof(int) * N_INDEXES);
    for (int i = 0; i < N_INDEXES; i++) {
        getline(instanceFile, line);
        indexesMem[i] = stoi(line, nullptr);
    }

    // remove CONFIGURATIONS_QUERIES_GAIN: row
    getline(instanceFile, line);

    // read CONFIGURATIONS_QUERIES_GAIN matrix and save it in an internal variable
    configGain = new int*[N_CONFIGURATIONS];
    for (int i = 0; i < N_CONFIGURATIONS; i++)
        configGain[i] = new int[N_QUERIES];
    for (int i = 0; i < N_CONFIGURATIONS; i++) {
        getline(instanceFile, line);
        for (int j = 0; j < N_QUERIES; j++) {
            string::size_type sz;
            configGain[i][j] = stoi(line, &sz);
            line = line.substr(sz + 1); // go to the next number in the row (skipping space)
        }
    }

    // DEBUG print
		/*cout << "indexes cost:" << endl;
		for (int i = 0; i < N_INDEXES; i++) {
			cout << indexesCost[i] << endl;
		}
		cout << "indexes memory:" << endl;
		for (int i = 0; i < N_INDEXES; i++) {
			cout << indexesMem[i] << endl;
		}
		cout << "CONFIGURATIONS_QUERIES_GAIN matrix:" << endl;
		for (int i = 0; i < N_CONFIGURATIONS; i++) {
			for (int j = 0; j < N_QUERIES; j++) {
				cout << configGain[i][j] << " ";
			}
			cout << endl;
		}*/

    uint8_t N_POPULATION = 10;

    clock_t begin = clock();

    set <solution> init_pop = initial_population(N_QUERIES,N_INDEXES,N_CONFIGURATIONS,N_POPULATION,MEMORY,configIndexes,indexesCost,indexesMem,configGain, true);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f s\n", time_spent);

    printf("Best Solution: %d", bestFitnessFoundSoFar);
}

/* FUNCTION TO GET INITIAL POPULATION */
set <solution> initial_population(int N_QUERIES, int N_INDEXES, int N_CONFIGURATIONS, int N_POPULATION, int MEMORY, list<int> *ci_matrix_lst, int *i_cost, int *i_mem, int **cq_gain, bool mode) {

    bool **ci_matrix = new bool*[N_CONFIGURATIONS];
    for (int c = 0; c < N_CONFIGURATIONS; c++) {
        ci_matrix[c] = new bool[N_INDEXES];

        for (int i = 0; i < N_INDEXES; i++)
            ci_matrix[c][i] = false;

        for (auto it = ci_matrix_lst[c].begin(); it != ci_matrix_lst[c].end(); advance(it,1)) {
            ci_matrix[c][*it] = true;
        }
    }

    set<solution> init_pop;

    auto sol = new solution;
    sol->matrix = new bool*[N_CONFIGURATIONS];
    for (int i = 0; i < N_CONFIGURATIONS; i++)
        sol->matrix[i] = new bool[N_QUERIES];

    // Create 7 Greedy solutions
    sol->fitness = greedy1(N_QUERIES,N_INDEXES,N_CONFIGURATIONS,MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol->matrix, mode);
    init_pop.insert(*sol);
    //printf("G1: %d\n",sol->fitness);
    sol->fitness = greedy2(N_QUERIES,N_INDEXES,N_CONFIGURATIONS,MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol->matrix, mode);
    init_pop.insert(*sol);
    //printf("G2: %d\n",sol->fitness);
    sol->fitness = greedy3(N_QUERIES,N_INDEXES,N_CONFIGURATIONS,MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol->matrix, mode);
    init_pop.insert(*sol);
    //printf("G3: %d\n",sol->fitness);
    sol->fitness = greedy4(N_QUERIES,N_INDEXES,N_CONFIGURATIONS,MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol->matrix, mode);
    init_pop.insert(*sol);
    //printf("G4: %d\n",sol->fitness);
    sol->fitness = greedy5(N_QUERIES,N_INDEXES,N_CONFIGURATIONS,MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol->matrix, mode);
    init_pop.insert(*sol);
    //printf("G5: %d\n",sol->fitness);
    sol->fitness = greedy6(N_QUERIES,N_INDEXES,N_CONFIGURATIONS,MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol->matrix, mode);
    init_pop.insert(*sol);
    //printf("G6: %d\n",sol->fitness);
    sol->fitness = greedy7(N_QUERIES,N_INDEXES,N_CONFIGURATIONS,MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol->matrix, mode);
    init_pop.insert(*sol);
    //printf("G7: %d\n",sol->fitness);

    // Create N_POPULATION + 7 Random solutions (Random solutions will replace bad Greedy solutions)
    for(uint32_t n = init_pop.size(); n < N_POPULATION + 7; n = init_pop.size()){
        sol->fitness = random(N_QUERIES,N_INDEXES,N_CONFIGURATIONS,MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol->matrix, mode);
        init_pop.insert(*sol);
        //if(n < init_pop.size())printf("R%d: %d\n",n+1,sol->fitness);
    }

    // Erase 7 worst solutions
    set<solution>::iterator it;
    for(uint8_t n = 0; n < 7; n++){
        it = init_pop.begin();
        advance(it,init_pop.size()-1);
        init_pop.erase(*it);
    }

    bestFitnessFoundSoFar = init_pop.begin()->fitness;

    return init_pop;
}