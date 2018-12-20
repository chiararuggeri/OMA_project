#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "mylib.h"
#include <list>
#include <vector>
#include <bits/stdc++.h>
#include <set>

int greedy1(int N_QUERIES, int N_INDEXES, int N_CONFIGURATIONS, int MEMORY, bool **ci_matrix, int *i_cost, int *i_mem, int **cq_gain, bool** cq_on, bool mode){

    int c_on[N_CONFIGURATIONS], i_on[N_INDEXES], q_on[N_QUERIES];
    int c_on_tmp[N_CONFIGURATIONS], i_on_tmp[N_INDEXES], cq_on_tmp[N_CONFIGURATIONS][N_QUERIES], q_on_tmp[N_QUERIES];
    int i_m[N_INDEXES], i_cost_[N_INDEXES];
    int cq_gain_[N_CONFIGURATIONS][N_QUERIES];

    for (int c = 0; c < N_CONFIGURATIONS; c++) {

        c_on[c] = 0; c_on_tmp[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            cq_on[c][q] = false;
            cq_gain_[c][q] = cq_gain[c][q];
            cq_on_tmp[c][q] = 0;
            q_on[q] = 0; q_on_tmp[q] = 0;
        }
    }

    for(int i=0; i<N_INDEXES; i++){
        i_m[i] = i_mem[i];
        i_cost_[i] = i_cost[i];
        i_on[i] = 0;  i_on_tmp[i] = 0;
    }


    int tot_mem = 0, tot_cost = 0, tot_gain = 0;
    int c_cost[N_CONFIGURATIONS];
    int max;
    int max_profit;
    int best_q = 0;
    int best_c = 0;
    int n_conf = 0;
    int cq_profit[N_CONFIGURATIONS][N_QUERIES];

    if(!mode)for (int c = 0; c < N_CONFIGURATIONS; c++){
        //Configuration Cost
        c_cost[c] = 0;
        for (int i = 0; i < N_INDEXES; i++)
            if (!c_on[c] && i_cost_[i] && ci_matrix[c][i])
                c_cost[c] += i_cost_[i] * ci_matrix[c][i];

        for(int q = 0; q<N_QUERIES; q++) {
            //Configurations Profit
            if (!c_on[c] && !q_on[q])
                cq_profit[c][q] = (cq_gain_[c][q] - c_cost[c]);
        }
    }

    while ((tot_mem < MEMORY) &&  n_conf <= N_QUERIES && n_conf <= N_CONFIGURATIONS) {

        max = -2147483647;
        for (int c = 0; c < N_CONFIGURATIONS; c++){

            if(mode) {
                //Configuration Cost
                c_cost[c] = 0;
                for (int i = 0; i < N_INDEXES; i++)
                    if (!c_on[c] && i_cost_[i] && ci_matrix[c][i])
                        c_cost[c] += i_cost_[i] * ci_matrix[c][i];
            }

            for(int q = 0; q<N_QUERIES; q++) {

                if(mode) {
                    //Configurations Profit
                    if (!c_on[c] && !q_on[q])
                        cq_profit[c][q] = (cq_gain_[c][q] - c_cost[c]);
                }

                if (!c_on[c])
                    if (cq_profit[c][q] > max) {
                        max = cq_profit[c][q];
                        best_c = c;
                    }
            }
        }

        c_on_tmp[best_c] = 1;

        tot_mem = 0;
        for (int i = 0; i < N_INDEXES; i++) {

            for (int c = 0; c < N_CONFIGURATIONS; c++)
                if (!i_on_tmp[i])
                    i_on_tmp[i] += ci_matrix[c][i] * c_on_tmp[c];

            tot_mem += i_on_tmp[i] * i_mem[i];
        }


        if (tot_mem < MEMORY) {
            n_conf = 0;
            for (int c = 0; c < N_CONFIGURATIONS; c++){
                c_on[c] = c_on_tmp[c];
                n_conf += c_on[c];
            }

            for (int i = 0; i < N_INDEXES; i++){
                i_on[i] = i_on_tmp[i];
                if (i_on[i])i_cost_[i] = 0;
            }
        }
    }

    for (int q = 0; q < N_QUERIES; q++)if (!q_on[q]) {
        max_profit = 0;
        for (int c = 0; c < N_CONFIGURATIONS; c++)if (c_on[c])
            if (cq_gain_[c][q] > max_profit) {
                max_profit = cq_gain_[c][q];
                best_c = c;
                best_q = q;
            }

       if (c_on[best_c] && !q_on[best_q]) {
           q_on[best_q] = 1;
           cq_on[best_c][best_q] = true;
        }
    }

    tot_gain = 0;
    for (int c = 0; c < N_CONFIGURATIONS; c++){
        c_on[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            if (q_on[q] && !c_on[c])
                c_on[c] += cq_on[c][q];

            tot_gain += cq_on[c][q] * cq_gain_[c][q];
        }
    }

    tot_mem = 0;  tot_cost = 0;
    for (int i = 0; i < N_INDEXES; i++) {
        i_on[i] = 0;

        for (int c = 0; c < N_CONFIGURATIONS; c++)if (!i_on[i])
                i_on[i] += ci_matrix[c][i] * c_on[c];

        tot_mem += i_mem[i] * i_on[i];

        tot_cost += i_on[i] * i_cost[i];
    }

    return tot_gain - tot_cost;
}

int greedy2(int N_QUERIES, int N_INDEXES, int N_CONFIGURATIONS, int MEMORY, bool **ci_matrix, int *i_cost, int *i_mem, int **cq_gain, bool** cq_on, bool mode){


    int c_on[N_CONFIGURATIONS], i_on[N_INDEXES], q_on[N_QUERIES];
    int c_on_tmp[N_CONFIGURATIONS], i_on_tmp[N_INDEXES], cq_on_tmp[N_CONFIGURATIONS][N_QUERIES], q_on_tmp[N_QUERIES];
    int i_m[N_INDEXES], i_cost_[N_INDEXES];
    int cq_gain_[N_CONFIGURATIONS][N_QUERIES];

    for (int c = 0; c < N_CONFIGURATIONS; c++) {

        c_on[c] = 0; c_on_tmp[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            cq_on[c][q] = false;
            cq_gain_[c][q] = cq_gain[c][q];
            cq_on_tmp[c][q] = 0;
            q_on[q] = 0; q_on_tmp[q] = 0;
        }
    }

    for(int i=0; i<N_INDEXES; i++){
        i_m[i] = i_mem[i];
        i_cost_[i] = i_cost[i];
        i_on[i] = 0;  i_on_tmp[i] = 0;
    }

    int tot_mem = 0, tot_cost = 0, tot_gain = 0;
    int c_cost[N_CONFIGURATIONS];
    int min_cost;
    int max_profit;
    int best_q = 0;
    int best_c = 0;
    int n_conf = 0;



    if(!mode)for (int c = 0; c < N_CONFIGURATIONS; c++){
        c_cost[c] = 0;
        //Configuration Cost
        for (int i = 0; i < N_INDEXES; i++)
            if (!c_on[c] && i_cost_[i] && ci_matrix[c][i])
                c_cost[c] += i_cost_[i] * ci_matrix[c][i];
    }

    while ((tot_mem < MEMORY) &&  n_conf <= N_QUERIES && n_conf <= N_CONFIGURATIONS) {

        min_cost = 2147483647;
        for (int c = 0; c < N_CONFIGURATIONS; c++){

            if(mode) {
                c_cost[c] = 0;
                //Configuration Cost
                for (int i = 0; i < N_INDEXES; i++)
                    if (!c_on[c] && i_cost_[i] && ci_matrix[c][i])
                        c_cost[c] += i_cost_[i] * ci_matrix[c][i];
            }

            if(!c_on[c])
                if (c_cost[c] < min_cost){
                    min_cost = c_cost[c];
                    best_c = c;
                }
        }

        c_on_tmp[best_c] = 1;

        tot_mem = 0;
        for (int i = 0; i < N_INDEXES; i++) {

            for (int c = 0; c < N_CONFIGURATIONS; c++)
                if (!i_on_tmp[i])
                    i_on_tmp[i] += ci_matrix[c][i] * c_on_tmp[c];

            tot_mem += i_on_tmp[i] * i_mem[i];
        }


        if (tot_mem < MEMORY) {
            n_conf = 0;
            for (int c = 0; c < N_CONFIGURATIONS; c++){
                c_on[c] = c_on_tmp[c];
                n_conf += c_on[c];
            }

            for (int i = 0; i < N_INDEXES; i++){
                i_on[i] = i_on_tmp[i];
                if (i_on[i])i_cost_[i] = 0;
            }
        }
    }

    for (int q = 0; q < N_QUERIES; q++)if (!q_on[q]) {
            max_profit = 0;
            for (int c = 0; c < N_CONFIGURATIONS; c++)if (c_on[c])
                    if (cq_gain_[c][q] > max_profit) {
                        max_profit = cq_gain_[c][q];
                        best_c = c;
                        best_q = q;
                    }

            if (c_on[best_c] && !q_on[best_q]) {
                q_on[best_q] = 1;
                cq_on[best_c][best_q] = true;
            }
        }

    tot_gain = 0;
    for (int c = 0; c < N_CONFIGURATIONS; c++){
        c_on[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            if (q_on[q] && !c_on[c])
                c_on[c] += cq_on[c][q];

            tot_gain += cq_on[c][q] * cq_gain_[c][q];
        }
    }

    tot_mem = 0;  tot_cost = 0;
    for (int i = 0; i < N_INDEXES; i++) {
        i_on[i] = 0;

        for (int c = 0; c < N_CONFIGURATIONS; c++)if (!i_on[i])
                i_on[i] += ci_matrix[c][i] * c_on[c];

        tot_mem += i_mem[i] * i_on[i];

        tot_cost += i_on[i] * i_cost[i];
    }

    return tot_gain - tot_cost;
}

int greedy3(int N_QUERIES, int N_INDEXES, int N_CONFIGURATIONS, int MEMORY, bool **ci_matrix, int *i_cost, int *i_mem, int **cq_gain, bool** cq_on, bool mode){


    int c_on[N_CONFIGURATIONS], i_on[N_INDEXES], q_on[N_QUERIES];
    int c_on_tmp[N_CONFIGURATIONS], i_on_tmp[N_INDEXES], cq_on_tmp[N_CONFIGURATIONS][N_QUERIES], q_on_tmp[N_QUERIES];
    int i_m[N_INDEXES], i_cost_[N_INDEXES];
    int cq_gain_[N_CONFIGURATIONS][N_QUERIES];


    for (int c = 0; c < N_CONFIGURATIONS; c++) {

        c_on[c] = 0; c_on_tmp[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            cq_on[c][q] = false;
            cq_gain_[c][q] = cq_gain[c][q];
            cq_on_tmp[c][q] = 0;
            q_on[q] = 0; q_on_tmp[q] = 0;
        }
    }

    for(int i=0; i<N_INDEXES; i++){
        i_m[i] = i_mem[i];
        i_cost_[i] = i_cost[i];
        i_on[i] = 0;  i_on_tmp[i] = 0;
    }

    int tot_mem = 0, tot_cost = 0, tot_gain = 0;
    int max_profit;
    int best_q = 0;
    int best_c = 0;
    int n_conf = 0;

    while ((tot_mem < MEMORY) &&  n_conf <= N_QUERIES && n_conf <= N_CONFIGURATIONS) {

        max_profit = - 2147483648;
        //More Profitable Configuration
        for (int c = 0; c < N_CONFIGURATIONS; c++)if(!c_on[c])
                for (int q = 0; q < N_QUERIES; q++)if(!q_on[q])
                        if (cq_gain_[c][q] > max_profit){
                            max_profit = cq_gain_[c][q];
                            best_c = c;
                        }

        c_on_tmp[best_c] = 1;


        tot_mem = 0;
        for (int i = 0; i < N_INDEXES; i++) {

            for (int c = 0; c < N_CONFIGURATIONS; c++)
                if (!i_on_tmp[i])
                    i_on_tmp[i] += ci_matrix[c][i] * c_on_tmp[c];

            tot_mem += i_on_tmp[i] * i_mem[i];
        }


        if (tot_mem < MEMORY) {
            n_conf = 0;
            for (int c = 0; c < N_CONFIGURATIONS; c++){
                c_on[c] = c_on_tmp[c];
                n_conf += c_on[c];
            }

            for (int i = 0; i < N_INDEXES; i++)
                i_on[i] = i_on_tmp[i];
        }
    }

    for (int q = 0; q < N_QUERIES; q++)if (!q_on[q]) {
            max_profit = 0;
            for (int c = 0; c < N_CONFIGURATIONS; c++)if (c_on[c])
                    if (cq_gain_[c][q] > max_profit) {
                        max_profit = cq_gain_[c][q];
                        best_c = c;
                        best_q = q;
                    }

            if (c_on[best_c] && !q_on[best_q]) {
                q_on[best_q] = 1;
                cq_on[best_c][best_q] = true;
            }
        }

    tot_gain = 0;
    for (int c = 0; c < N_CONFIGURATIONS; c++){
        c_on[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            if (q_on[q] && !c_on[c])
                c_on[c] += cq_on[c][q];

            tot_gain += cq_on[c][q] * cq_gain_[c][q];
        }
    }

    tot_mem = 0;  tot_cost = 0;
    for (int i = 0; i < N_INDEXES; i++) {
        i_on[i] = 0;

        for (int c = 0; c < N_CONFIGURATIONS; c++)if (!i_on[i])
                i_on[i] += ci_matrix[c][i] * c_on[c];

        tot_mem += i_mem[i] * i_on[i];

        tot_cost += i_on[i] * i_cost[i];
    }

    return tot_gain - tot_cost;
}

int greedy4(int N_QUERIES, int N_INDEXES, int N_CONFIGURATIONS, int MEMORY, bool **ci_matrix, int *i_cost, int *i_mem, int **cq_gain, bool** cq_on, bool mode){

    int c_on[N_CONFIGURATIONS], i_on[N_INDEXES], q_on[N_QUERIES];
    int c_on_tmp[N_CONFIGURATIONS], i_on_tmp[N_INDEXES], cq_on_tmp[N_CONFIGURATIONS][N_QUERIES], q_on_tmp[N_QUERIES];
    int i_m[N_INDEXES], i_cost_[N_INDEXES];
    int cq_gain_[N_CONFIGURATIONS][N_QUERIES];

    for (int c = 0; c < N_CONFIGURATIONS; c++) {

        c_on[c] = 0; c_on_tmp[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            cq_on[c][q] = false;
            cq_gain_[c][q] = cq_gain[c][q];
            cq_on_tmp[c][q] = 0;
            q_on[q] = 0; q_on_tmp[q] = 0;
        }
    }

    for(int i=0; i<N_INDEXES; i++){
        i_m[i] = i_mem[i];
        i_cost_[i] = i_cost[i];
        i_on[i] = 0;  i_on_tmp[i] = 0;
    }

    int tot_mem = 0, tot_cost = 0, tot_gain = 0;
    int c_mem[N_CONFIGURATIONS];
    int min_mem;
    int max_profit;
    int best_q = 0;
    int best_c = 0;
    int n_conf = 0;

    if(!mode)for (int c = 0; c < N_CONFIGURATIONS; c++){
        //Configuration Memory
        c_mem[c] = 0;
        for (int i = 0; i < N_INDEXES; i++)
            if (!c_on[c] && i_m[i] && ci_matrix[c][i])
                c_mem[c] += i_m[i] * ci_matrix[c][i];
    }

    while ((tot_mem < MEMORY) &&  n_conf <= N_QUERIES && n_conf <= N_CONFIGURATIONS) {

        min_mem = 2147483647;
        for (int c = 0; c < N_CONFIGURATIONS; c++) {

            if(mode) {
                //Configuration Memory
                c_mem[c] = 0;
                for (int i = 0; i < N_INDEXES; i++)
                    if (!c_on[c] && i_m[i] && ci_matrix[c][i])
                        c_mem[c] += i_m[i] * ci_matrix[c][i];
            }

            if(!c_on[c])if(c_mem[c] < min_mem){
                min_mem = c_mem[c];
                best_c = c;
            }
        }

        c_on_tmp[best_c] = 1;

        tot_mem = 0;
        for (int i = 0; i < N_INDEXES; i++) {

            for (int c = 0; c < N_CONFIGURATIONS; c++)
                if (!i_on_tmp[i])
                    i_on_tmp[i] += ci_matrix[c][i] * c_on_tmp[c];

            tot_mem += i_on_tmp[i] * i_mem[i];
        }

        if (tot_mem < MEMORY) {

            n_conf = 0;
            for (int c = 0; c < N_CONFIGURATIONS; c++){
                c_on[c] = c_on_tmp[c];
                n_conf += c_on[c];
            }

            for (int i = 0; i < N_INDEXES; i++){
                i_on[i] = i_on_tmp[i];
                if (i_on[i])i_m[i] = 0;
            }
        }
    }

    for (int q = 0; q < N_QUERIES; q++)if (!q_on[q]) {
        max_profit = 0;
        for (int c = 0; c < N_CONFIGURATIONS; c++)if (c_on[c])
            if (cq_gain_[c][q] > max_profit) {
                max_profit = cq_gain_[c][q];
                best_c = c;
                best_q = q;
            }

       if (c_on[best_c] && !q_on[best_q]) {
           q_on[best_q] = 1;
           cq_on[best_c][best_q] = true;
        }
    }

    tot_gain = 0;
    for (int c = 0; c < N_CONFIGURATIONS; c++){
        c_on[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            if (q_on[q] && !c_on[c])
                c_on[c] += cq_on[c][q];

            tot_gain += cq_on[c][q] * cq_gain_[c][q];
        }
    }

    tot_mem = 0;  tot_cost = 0;
    for (int i = 0; i < N_INDEXES; i++) {
        i_on[i] = 0;

        for (int c = 0; c < N_CONFIGURATIONS; c++)if (!i_on[i])
                i_on[i] += ci_matrix[c][i] * c_on[c];

        tot_mem += i_mem[i] * i_on[i];

        tot_cost += i_on[i] * i_cost[i];
    }
    
    return tot_gain - tot_cost;
}

int greedy5(int N_QUERIES, int N_INDEXES, int N_CONFIGURATIONS, int MEMORY, bool **ci_matrix, int *i_cost, int *i_mem, int **cq_gain, bool** cq_on, bool mode){

    int c_on[N_CONFIGURATIONS], i_on[N_INDEXES], q_on[N_QUERIES];
    int c_on_tmp[N_CONFIGURATIONS], i_on_tmp[N_INDEXES], cq_on_tmp[N_CONFIGURATIONS][N_QUERIES], q_on_tmp[N_QUERIES];
    int i_m[N_INDEXES], i_cost_[N_INDEXES];
    int cq_gain_[N_CONFIGURATIONS][N_QUERIES];


    for (int c = 0; c < N_CONFIGURATIONS; c++) {

        c_on[c] = 0; c_on_tmp[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            cq_on[c][q] = false;
            cq_gain_[c][q] = cq_gain[c][q];
            cq_on_tmp[c][q] = 0;
            q_on[q] = 0; q_on_tmp[q] = 0;
        }
    }

    for(int i=0; i<N_INDEXES; i++){
        i_m[i] = i_mem[i];
        i_cost_[i] = i_cost[i];
        i_on[i] = 0;  i_on_tmp[i] = 0;
    }

    int tot_mem = 0, tot_cost = 0, tot_gain = 0;
    int c_mem[N_CONFIGURATIONS], c_cost[N_CONFIGURATIONS];
    float min;
    int max_profit;
    int best_q = 0;
    int best_c = 0;
    int n_conf = 0;
    float c_memcost[N_CONFIGURATIONS];


    if(!mode)for (int c = 0; c < N_CONFIGURATIONS; c++) {
            c_mem[c] = 0;
            c_cost[c] = 0;
            //Configuration Memory
            for (int i = 0; i < N_INDEXES; i++) {
                if (!c_on[c] && i_m[i] && ci_matrix[c][i])
                    c_mem[c] += i_m[i] * ci_matrix[c][i];
                if (!c_on[c] && i_cost_[i] && ci_matrix[c][i])
                    c_cost[c] += i_cost_[i] * ci_matrix[c][i];
            }
            //Configurations Cost * Memory
            c_memcost[c] = c_cost[c] * c_mem[c];
    }

    while ((tot_mem < MEMORY) &&  n_conf <= N_QUERIES && n_conf <= N_CONFIGURATIONS) {

        min = 2147483647;
        for (int c = 0; c < N_CONFIGURATIONS; c++) {
            if(mode) {
                c_mem[c] = 0;
                c_cost[c] = 0;
                //Configuration Memory
                for (int i = 0; i < N_INDEXES; i++) {
                    if (!c_on[c] && i_m[i] && ci_matrix[c][i])
                        c_mem[c] += i_m[i] * ci_matrix[c][i];

                    if (!c_on[c] && i_cost_[i] && ci_matrix[c][i])
                        c_cost[c] += i_cost_[i] * ci_matrix[c][i];
                }
                //Configurations Cost * Memory
                c_memcost[c] = c_cost[c] * c_mem[c];
            }
            if(!c_on[c])
                if (c_memcost[c] < min){
                    min = c_memcost[c];
                    best_c = c;
                }
        }

        c_on_tmp[best_c] = 1;

        tot_mem = 0;
        for (int i = 0; i < N_INDEXES; i++) {

            for (int c = 0; c < N_CONFIGURATIONS; c++)
                if (!i_on_tmp[i])
                    i_on_tmp[i] += ci_matrix[c][i] * c_on_tmp[c];

            tot_mem += i_on_tmp[i] * i_mem[i];
        }

        if (tot_mem < MEMORY) {

            n_conf = 0;
            for (int c = 0; c < N_CONFIGURATIONS; c++){
                c_on[c] = c_on_tmp[c];
                n_conf += c_on[c];
            }

            for (int i = 0; i < N_INDEXES; i++){
                i_on[i] = i_on_tmp[i];
                if (i_on[i]){
                    i_m[i] = 0;
                    i_cost_[i] = 0;
                }
            }
        }
    }

    for (int q = 0; q < N_QUERIES; q++)if (!q_on[q]) {
        max_profit = 0;
        for (int c = 0; c < N_CONFIGURATIONS; c++)if (c_on[c])
            if (cq_gain_[c][q] > max_profit) {
                max_profit = cq_gain_[c][q];
                best_c = c;
                best_q = q;
            }

       if (c_on[best_c] && !q_on[best_q]) {
           q_on[best_q] = 1;
           cq_on[best_c][best_q] = true;
        }
    }

    tot_gain = 0;
    for (int c = 0; c < N_CONFIGURATIONS; c++){
        c_on[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            if (q_on[q] && !c_on[c])
                c_on[c] += cq_on[c][q];

            tot_gain += cq_on[c][q] * cq_gain_[c][q];
        }
    }

    tot_mem = 0;  tot_cost = 0;
    for (int i = 0; i < N_INDEXES; i++) {
        i_on[i] = 0;

        for (int c = 0; c < N_CONFIGURATIONS; c++)if (!i_on[i])
                i_on[i] += ci_matrix[c][i] * c_on[c];

        tot_mem += i_mem[i] * i_on[i];

        tot_cost += i_on[i] * i_cost[i];
    }

    return tot_gain - tot_cost;
}

int greedy6(int N_QUERIES, int N_INDEXES, int N_CONFIGURATIONS, int MEMORY, bool **ci_matrix, int *i_cost, int *i_mem, int **cq_gain, bool** cq_on, bool mode){

    int c_on[N_CONFIGURATIONS], i_on[N_INDEXES], q_on[N_QUERIES];
    int c_on_tmp[N_CONFIGURATIONS], i_on_tmp[N_INDEXES], cq_on_tmp[N_CONFIGURATIONS][N_QUERIES], q_on_tmp[N_QUERIES];
    int i_m[N_INDEXES], i_cost_[N_INDEXES];
    int cq_gain_[N_CONFIGURATIONS][N_QUERIES];

    for (int c = 0; c < N_CONFIGURATIONS; c++) {

        c_on[c] = 0; c_on_tmp[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            cq_on[c][q] = false;
            cq_gain_[c][q] = cq_gain[c][q];
            cq_on_tmp[c][q] = 0;
            q_on[q] = 0; q_on_tmp[q] = 0;
        }
    }

    for(int i=0; i<N_INDEXES; i++){
        i_m[i] = i_mem[i];
        i_cost_[i] = i_cost[i];
        i_on[i] = 0;  i_on_tmp[i] = 0;
    }

    int tot_mem = 0, tot_cost = 0, tot_gain = 0;
    int c_mem[N_CONFIGURATIONS];
    float max;
    int max_profit;
    int best_q = 0;
    int best_c = 0;
    int n_conf = 0;
    float cq_memgain[N_CONFIGURATIONS][N_QUERIES];

    if(!mode)for (int c = 0; c < N_CONFIGURATIONS; c++){
            //Configuration Memory
            c_mem[c] = 0;
            for (int i = 0; i < N_INDEXES; i++)
                if(!c_on[c] && i_m[i] && ci_matrix[c][i])
                    c_mem[c] += i_m[i] * ci_matrix[c][i];

            //Configurations Gain / Memory
            for(int q=0; q<N_QUERIES; q++)
                if(!c_on[c])
                    cq_memgain[c][q] = (float) cq_gain_[c][q] / (float) c_mem[c];
    }

    while ((tot_mem < MEMORY) &&  n_conf <= N_QUERIES && n_conf <= N_CONFIGURATIONS) {

        max = -1;
        for (int c = 0; c < N_CONFIGURATIONS; c++) {

            if(mode) {
                //Configuration Memory
                c_mem[c] = 0;
                for (int i = 0; i < N_INDEXES; i++)
                    if (!c_on[c] && i_m[i] && ci_matrix[c][i])
                        c_mem[c] += i_m[i] * ci_matrix[c][i];
            }

            //Configurations Gain / Memory
            for(int q=0; q<N_QUERIES; q++) {
                if(mode) {
                    if (!c_on[c])
                        cq_memgain[c][q] = (float) cq_gain_[c][q] / (float) c_mem[c];
                }

                if(!q_on[q])if(!c_on[c])
                    if (cq_memgain[c][q] > max){
                        max = cq_memgain[c][q];
                        best_c = c;
                    }
            }
        }

        c_on_tmp[best_c] = 1;

        tot_mem = 0;
        for (int i = 0; i < N_INDEXES; i++) {

            for (int c = 0; c < N_CONFIGURATIONS; c++)
                if (!i_on_tmp[i])
                    i_on_tmp[i] += ci_matrix[c][i] * c_on_tmp[c];

            tot_mem += i_on_tmp[i] * i_mem[i];
        }

        if (tot_mem < MEMORY) {

            n_conf = 0;
            for (int c = 0; c < N_CONFIGURATIONS; c++){
                c_on[c] = c_on_tmp[c];
                n_conf += c_on[c];
            }

            for (int i = 0; i < N_INDEXES; i++){
                i_on[i] = i_on_tmp[i];
                if (i_on[i]){
                    i_m[i] = 0;
                }
            }
        }
    }

    for (int q = 0; q < N_QUERIES; q++)if (!q_on[q]) {
        max_profit = 0;
        for (int c = 0; c < N_CONFIGURATIONS; c++)if (c_on[c])
            if (cq_gain_[c][q] > max_profit) {
                max_profit = cq_gain_[c][q];
                best_c = c;
                best_q = q;
            }

       if (c_on[best_c] && !q_on[best_q]) {
           q_on[best_q] = 1;
           cq_on[best_c][best_q] = true;
        }
    }

    tot_gain = 0;
    for (int c = 0; c < N_CONFIGURATIONS; c++){
        c_on[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            if (q_on[q] && !c_on[c])
                c_on[c] += cq_on[c][q];

            tot_gain += cq_on[c][q] * cq_gain_[c][q];
        }
    }

    tot_mem = 0;  tot_cost = 0;
    for (int i = 0; i < N_INDEXES; i++) {
        i_on[i] = 0;

        for (int c = 0; c < N_CONFIGURATIONS; c++)if (!i_on[i])
                i_on[i] += ci_matrix[c][i] * c_on[c];

        tot_mem += i_mem[i] * i_on[i];

        tot_cost += i_on[i] * i_cost[i];
    }
    
    return tot_gain - tot_cost;
}

int greedy7(int N_QUERIES, int N_INDEXES, int N_CONFIGURATIONS, int MEMORY, bool **ci_matrix, int *i_cost, int *i_mem, int **cq_gain, bool** cq_on, bool mode){

    int c_on[N_CONFIGURATIONS], i_on[N_INDEXES], q_on[N_QUERIES];
    int c_on_tmp[N_CONFIGURATIONS], i_on_tmp[N_INDEXES], cq_on_tmp[N_CONFIGURATIONS][N_QUERIES], q_on_tmp[N_QUERIES];
    int i_m[N_INDEXES], i_cost_[N_INDEXES];
    int cq_gain_[N_CONFIGURATIONS][N_QUERIES];

    for (int c = 0; c < N_CONFIGURATIONS; c++) {

        c_on[c] = 0; c_on_tmp[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            cq_on[c][q] = false;
            cq_gain_[c][q] = cq_gain[c][q];
            cq_on_tmp[c][q] = 0;
            q_on[q] = 0; q_on_tmp[q] = 0;
        }
    }

    for(int i=0; i<N_INDEXES; i++){
        i_m[i] = i_mem[i];
        i_cost_[i] = i_cost[i];
        i_on[i] = 0;  i_on_tmp[i] = 0;
    }

    int tot_mem = 0, tot_cost = 0, tot_gain = 0;
    int c_mem[N_CONFIGURATIONS], c_cost[N_CONFIGURATIONS];
    float max;
    int max_profit;
    int best_q = 0;
    int best_c = 0;
    int n_conf = 0;
    float cq_memprofit[N_CONFIGURATIONS][N_QUERIES];

    if(!mode)for (int c = 0; c < N_CONFIGURATIONS; c++){
            //Configuration Memory and Cost
            c_mem[c] = 0; c_cost[c] = 0;
            for (int i = 0; i < N_INDEXES; i++) {
                if(!c_on[c] && i_m[i] && ci_matrix[c][i])
                    c_mem[c] += i_m[i] * ci_matrix[c][i];

                if(!c_on[c] && i_cost_[i] && ci_matrix[c][i])
                    c_cost[c] += i_cost_[i] * ci_matrix[c][i];
            }
            for(int q=0; q<N_QUERIES; q++)if(!c_on[c]) {
                    if ((float) cq_gain_[c][q] - (float) c_cost[c] >= 0)
                        cq_memprofit[c][q] = (float) (cq_gain[c][q] - c_cost[c]) / (float) c_mem[c];
                    else
                        cq_memprofit[c][q] = -(float) (c_cost[c] - cq_gain_[c][q]) / (float) c_mem[c];
                }
    }

    while ((tot_mem < MEMORY) &&  n_conf <= N_QUERIES && n_conf <= N_CONFIGURATIONS) {

        max = -2147483647;
        for (int c = 0; c < N_CONFIGURATIONS; c++) {

            if(mode) {
                //Configuration Memory and Cost
                c_mem[c] = 0;
                c_cost[c] = 0;
                for (int i = 0; i < N_INDEXES; i++) {
                    if (!c_on[c] && i_m[i] && ci_matrix[c][i])
                        c_mem[c] += i_m[i] * ci_matrix[c][i];

                    if (!c_on[c] && i_cost_[i] && ci_matrix[c][i])
                        c_cost[c] += i_cost_[i] * ci_matrix[c][i];
                }
            }
            //Configurations Profit / Memory
            for(int q=0; q<N_QUERIES; q++)if(!c_on[c]) {
                if(mode) {
                    if ((float) cq_gain_[c][q] - (float) c_cost[c] >= 0)
                        cq_memprofit[c][q] = (float) (cq_gain[c][q] - c_cost[c]) / (float) c_mem[c];
                    else
                        cq_memprofit[c][q] = -(float) (c_cost[c] - cq_gain_[c][q]) / (float) c_mem[c];
                }

                if(!c_on[c])if(!q_on[q])
                    if (cq_memprofit[c][q] > max){
                        max = cq_memprofit[c][q];
                        best_c = c;
                    }
            }
        }

        c_on_tmp[best_c] = 1;

        tot_mem = 0;
        for (int i = 0; i < N_INDEXES; i++) {

            for (int c = 0; c < N_CONFIGURATIONS; c++)
                if (!i_on_tmp[i])
                    i_on_tmp[i] += ci_matrix[c][i] * c_on_tmp[c];

            tot_mem += i_on_tmp[i] * i_mem[i];
        }

        if (tot_mem < MEMORY) {

            n_conf = 0;
            for (int c = 0; c < N_CONFIGURATIONS; c++){
                c_on[c] = c_on_tmp[c];
                n_conf += c_on[c];
            }

            for (int i = 0; i < N_INDEXES; i++){
                i_on[i] = i_on_tmp[i];
                if (i_on[i]){
                    i_m[i] = 0;
                    i_cost_[i] = 0;
                }
            }
        }
    }

    for (int q = 0; q < N_QUERIES; q++)if (!q_on[q]) {
        max_profit = 0;
        for (int c = 0; c < N_CONFIGURATIONS; c++)if (c_on[c])
            if (cq_gain_[c][q] > max_profit) {
                max_profit = cq_gain_[c][q];
                best_c = c;
                best_q = q;
            }

       if (c_on[best_c] && !q_on[best_q]) {
           q_on[best_q] = 1;
           cq_on[best_c][best_q] = true;
        }
    }

    tot_gain = 0;
    for (int c = 0; c < N_CONFIGURATIONS; c++){
        c_on[c] = 0;

        for (int q = 0; q < N_QUERIES; q++) {
            if (q_on[q] && !c_on[c])
                c_on[c] += cq_on[c][q];

            tot_gain += cq_on[c][q] * cq_gain_[c][q];
        }
    }

    tot_mem = 0;  tot_cost = 0;
    for (int i = 0; i < N_INDEXES; i++) {
        i_on[i] = 0;

        for (int c = 0; c < N_CONFIGURATIONS; c++)if (!i_on[i])
                i_on[i] += ci_matrix[c][i] * c_on[c];

        tot_mem += i_mem[i] * i_on[i];

        tot_cost += i_on[i] * i_cost[i];
    }

    return tot_gain - tot_cost;
}

int random(int N_QUERIES, int N_INDEXES, int N_CONFIGURATIONS, int MEMORY, bool **ci_matrix, int *i_cost, int *i_mem, int **cq_gain, bool** cq_on, bool mode){

    for (int c = 0; c < N_CONFIGURATIONS; c++) for (int q = 0; q < N_QUERIES; q++) cq_on[c][q] = false;

    int c_on[N_CONFIGURATIONS], i_on[N_INDEXES], q_on[N_QUERIES], c_on_tmp[N_CONFIGURATIONS], i_on_tmp[N_INDEXES];
    int tot_mem = 0, tot_cost = 0, tot_gain = 0;
    int max_profit = 0, n = 0;
    int best_q = 0;
    int best_c = 0;

    struct timeval te;
    gettimeofday(&te, nullptr); // get current time
    srand((unsigned int)(te.tv_sec * 100) + (te.tv_usec / 100));

    while((tot_gain <= tot_cost) && n < 10) {

        int new_c = 0, n_conf = 0;
        int not_new;

        int old_c[N_CONFIGURATIONS];

        for (int c = 0; c < N_CONFIGURATIONS; c++) {
            c_on[c] = 0;
            c_on_tmp[c] = 0;
            old_c[c] = N_CONFIGURATIONS;

            for (int q = 0; q < N_QUERIES; q++) {
                cq_on[c][q] = false;
                q_on[q] = 0;
            }
        }

        for (int i = 0; i < N_INDEXES; i++) {
            i_on[i] = 0;
            i_on_tmp[i] = 0;
        }

        while ((tot_mem < MEMORY) && n_conf <= N_QUERIES && n_conf <= N_CONFIGURATIONS) {

            not_new = 1;
            while (not_new) {
                new_c = rand() % N_CONFIGURATIONS;
                not_new = 0;

                for (int c = 0; c < n_conf; c++)
                    if (old_c[c] == new_c)
                        not_new = 1;

                if (!not_new) {
                    old_c[n_conf] = new_c;
                    not_new = 0;
                }
            }

            c_on_tmp[new_c] = 1;

            tot_mem = 0;
            for (int i = 0; i < N_INDEXES; i++) {
                for (int c = 0; c < N_CONFIGURATIONS; c++)
                    if (!i_on_tmp[i])
                        i_on_tmp[i] += ci_matrix[c][i] * c_on_tmp[c];

                tot_mem += i_on_tmp[i] * i_mem[i];
            }

            if (tot_mem < MEMORY) {
                n_conf = 0;
                for (int c = 0; c < N_CONFIGURATIONS; c++) {
                    c_on[c] = c_on_tmp[c];
                    n_conf += c_on[c];
                }

                for (int i = 0; i < N_INDEXES; i++)
                    i_on[i] = i_on_tmp[i];
            }
        }

        for (int q = 0; q < N_QUERIES; q++)if (!q_on[q]) {
                max_profit = 0;
                for (int c = 0; c < N_CONFIGURATIONS; c++)if (c_on[c])
                        if ( cq_gain[c][q] > max_profit) {
                            max_profit = cq_gain[c][q];
                            best_c = c;
                            best_q = q;
                        }

                if (c_on[best_c] && !q_on[best_q]) {
                    q_on[best_q] = 1;
                    cq_on[best_c][best_q] = true;
                }
        }

        tot_gain = 0;
        for (int c = 0; c < N_CONFIGURATIONS; c++) {
            c_on[c] = 0;
            for (int q = 0; q < N_QUERIES; q++) {
                if (q_on[q] && !c_on[c])
                    c_on[c] += cq_on[c][q];

                tot_gain += cq_on[c][q] * cq_gain[c][q];
            }
        }

        tot_mem = 0; tot_cost = 0;
        for (int i = 0; i < N_INDEXES; i++){
            i_on[i] = 0;
            for (int c = 0; c < N_CONFIGURATIONS; c++)if (!i_on[i])
                    i_on[i] +=  ci_matrix[c][i] * c_on[c];

            tot_mem += i_mem[i] * i_on[i];
            tot_cost += i_on[i] * i_cost[i];
        }

        n++;
    }

    return tot_gain - tot_cost;
}