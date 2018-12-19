#include <iostream>
#include <string>
#include <stdlib.h>
#include <set>
#include <ctime>
#include <map>
#include <algorithm>

using namespace std;

typedef struct solution{
    bool** matrix;
    int fitness;
} sol;

bool operator < (const solution& sol1, const solution& sol2)
{
    return sol1.fitness > sol2.fitness;
}

set<solution> updatePopulation(set<solution> parents, set<solution> children, int k){

    if(k > parents.size()) k = parents.size(); //probably not necessary

    set<solution>::iterator it;

    /* ERASE K WORST PARENTS*/
    for (int i = 0; i < k; i++){
        /* POSITION ITERATOR */
        it = parents.begin();
        advance(it,parents.size()-1);
        /* ERASE WORST PARENT */
        parents.erase(*it);
    }

    /* REPLACE WITH K BEST CHILDREN*/
    for (int i = 0; i < k; i++){
        /* INSERT BEST CHILD */
        parents.insert(*children.begin());
        /* ERASE THAT CHILD */
        children.erase(*children.begin());
    }

    return parents;
}

int main() {

    set<solution> parents;
    set<solution> children;

    int N_CONFIGURATIONS = 4, N_QUERIES = 3;
    int k = 1; //number of parents to be replaced by children

    /* CREATE PARENTS */

    auto sol1 = new solution;
    sol1->matrix = new bool*[N_CONFIGURATIONS];
    for (int i = 0; i < N_CONFIGURATIONS; i++)
        sol1->matrix[i] = new bool[N_QUERIES];
    for (int j = 0; j < N_CONFIGURATIONS; j++)
        for (int m = 0; m < N_QUERIES; m++)
            sol1->matrix[j][m] = false;

    sol1->matrix[1][0] = true;
    sol1->matrix[0][1] = true;
    sol1->matrix[3][2] = true;
    sol1->fitness = 20;

    auto sol2 = new solution;
    sol2->matrix = new bool*[N_CONFIGURATIONS];
    for (int i = 0; i < N_CONFIGURATIONS; i++)
        sol2->matrix[i] = new bool[N_QUERIES];
    for (int j = 0; j < N_CONFIGURATIONS; j++)
        for (int m = 0; m < N_QUERIES; m++)
            sol2->matrix[j][m] = false;
    sol2->matrix[0][2] = true;
    sol2->matrix[2][1] = true;
    sol2->matrix[3][0] = true;
    sol2->fitness = 15;

    auto sol5 = new solution;
    sol5->matrix = new bool*[N_CONFIGURATIONS];
    for (int i = 0; i < N_CONFIGURATIONS; i++)
        sol5->matrix[i] = new bool[N_QUERIES];
    for (int j = 0; j < N_CONFIGURATIONS; j++)
        for (int m = 0; m < N_QUERIES; m++)
            sol5->matrix[j][m] = false;
    sol5->matrix[1][2] = true;
    sol5->matrix[2][1] = true;
    sol5->matrix[3][0] = true;
    sol5->fitness = 14;

    parents.insert(*sol1);
    parents.insert(*sol2);
    parents.insert(*sol5);

    /* CREATE CHILDREN */

    auto sol3 = new solution;
    sol3->matrix = new bool*[N_CONFIGURATIONS];
    for (int i = 0; i < N_CONFIGURATIONS; i++)
        sol3->matrix[i] = new bool[N_QUERIES];
    for (int j = 0; j < N_CONFIGURATIONS; j++)
        for (int m = 0; m < N_QUERIES; m++)
            sol3->matrix[j][m] = false;
    sol3->matrix[1][0] = true;
    sol3->matrix[1][2] = true;
    sol3->matrix[3][1] = true;
    sol3->fitness = 18;

    auto sol4 = new solution;
    sol4->matrix = new bool*[N_CONFIGURATIONS];
    for (int i = 0; i < N_CONFIGURATIONS; i++)
        sol4->matrix[i] = new bool[N_QUERIES];
    for (int j = 0; j < N_CONFIGURATIONS; j++)
        for (int m = 0; m < N_QUERIES; m++)
            sol4->matrix[j][m] = false;
    sol4->matrix[2][1] = true;
    sol4->matrix[3][2] = true;
    sol4->matrix[3][0] = true;
    sol4->fitness = 12;

    auto sol6 = new solution;
    sol6->matrix = new bool*[N_CONFIGURATIONS];
    for (int i = 0; i < N_CONFIGURATIONS; i++)
        sol6->matrix[i] = new bool[N_QUERIES];
    for (int j = 0; j < N_CONFIGURATIONS; j++)
        for (int m = 0; m < N_QUERIES; m++)
            sol6->matrix[j][m] = false;
    sol6->matrix[2][1] = true;
    sol6->matrix[3][2] = true;
    sol6->matrix[3][0] = true;
    sol6->fitness = 10;

    children.insert(*sol3);
    children.insert(*sol4);
    children.insert(*sol6);

    /* UPDATE POPULATION */

    parents = updatePopulation(parents, children, k);

    return 0;
}