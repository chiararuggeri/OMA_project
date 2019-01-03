//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <set>
#include <ctime> 
#include <map>
#include <list>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "mylib.h"

using namespace std;

typedef struct solution{
	vector<vector<bool>> matrix;
	int fitness;
} sol;

// global variables
int N_QUERIES = 0;
int N_INDEXES = 0;
int N_CONFIGURATIONS = 0;
int MEMORY = 0;

list<int> *configIndexes;
int * indexesCost;
int * indexesMem;
int ** configGain;
bool **ci_matrix;

// best current solution
int bestFitnessFoundSoFar = 0;
ofstream *outfile;

ifstream instanceFile;
string instanceFileName;

mutex safeWrite;

// function prototypes
void heuristic();
void performCrossover(solution best, solution random, set<solution>* children);
set<solution> generateChildren(set<solution> parents);
int calculateFitness(map<int, int>* confXquery);
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



bool operator < (const solution& sol1, const solution& sol2)
{
	return sol1.fitness > sol2.fitness;
}

int main(int argc, char** argv)
{
	srand((unsigned)time(0));
	int timelimit;

	// parsing command line parameters: 0 - executable name. 1 - instancefilename. 2 - "-t". 3 - timelimit
	if (argc < 4) {
		cout << "ERROR. Invalid specified args!" << endl << "Usage: $ODBDPsolver_OMAAL_group06.exe instancefilename -t timelimit" << endl;
		exit(0);
	}

	if (strcmp(argv[2], "-t")) {
		cout << "ERROR. Unknown parameter " << argv[2] << endl;
		cout << "Usage: $ODBDPsolver_OMAAL_group06.exe instancefilename -t timelimit" << endl;
		exit(0);
	}
	try {
		timelimit = stoi(argv[3], nullptr);
	}
	catch (...) {
		cout << "ERROR. wrong specified timelimit. Please specify an integer representing the number of seconds the program has to run." << endl;
		exit(0);
	}

	instanceFileName = argv[1];
	instanceFile.open(instanceFileName);
	if (!instanceFile.is_open()) {
		// ERROR - Instance file not found
		cout << "ERROR. Specified instance file cannot not be found." << endl;
		exit(0);
	}
	// remove ".odbdp" from instance file name
	std::size_t found = instanceFileName.find(".odbdp");
	if (found != std::string::npos)
		instanceFileName = instanceFileName.substr(0, found);

	mutex m;
	condition_variable cv;

	// create a separate thread to run our function
	std::thread t([&cv]()
	{
		heuristic();
		// this row will notify the main that thread has terminated. since our function is a while(true) loop, this row will never be executed
		cv.notify_one();
	});

	t.detach();


	unique_lock<mutex> l(m);
	// the next function will return only in 2 cases: when the given timeout elapses or when notify_one() function is called.
	// since notify_one function will never be reached we ensure that our thread will run only for the time passed as parameter
	cv.wait_for(l, timelimit * 1s);

	// if the other thread is updating the solution file, wait until it finishes before stop execution
	safeWrite.lock();
	// kill thread and exit
	t.~thread();
	safeWrite.unlock();
}

void heuristic(){


	std::clock_t start;
	start = std::clock();

	// create output file
	outfile = new ofstream(instanceFileName.append("_OMAAL_group06.sol"));

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
	//cout << "number of queries: " << N_QUERIES << endl << "number of indexes: " << N_INDEXES << endl << "number of configurations: " << N_CONFIGURATIONS << endl << "memory: " << MEMORY << endl;

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
			}
		}
		configIndexes[i] = indexForConf;
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
	//configGain = new vector<vector<int>>(N_CONFIGURATIONS, vector<int>(N_QUERIES));
	configGain = new int*[N_CONFIGURATIONS];
	for (int i = 0; i < N_CONFIGURATIONS; i++) {
		configGain[i] = new int[N_QUERIES];
		getline(instanceFile, line);
		for (int j = 0; j < N_QUERIES; j++) {
			string::size_type sz;
			configGain[i][j] = stoi(line, &sz);
			line = line.substr(sz + 1); // go to the next number in the row (skipping space)
		}
	}
	/* DEBUG print
		cout << "indexes cost:" << endl;
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
		}
	*/

	ci_matrix = new bool*[N_CONFIGURATIONS];
	for (int c = 0; c < N_CONFIGURATIONS; c++) {
		ci_matrix[c] = new bool[N_INDEXES];

		for (int i = 0; i < N_INDEXES; i++)
			ci_matrix[c][i] = false;

		for (auto it = configIndexes[c].begin(); it != configIndexes[c].end(); advance(it, 1)) {
			ci_matrix[c][*it] = true;
		}
	}


	uint8_t N_POPULATION = 10;

	clock_t begin = clock();

	// STEP 1 - initial population
	set <solution> parents = initial_population(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, N_POPULATION, MEMORY, configIndexes, indexesCost, indexesMem, configGain, true);

	//printf("Best Solution: %d", bestFitnessFoundSoFar);

	while (true) { // run until the end of the time

		// STEPS 2 TO 4 - grouping parents, generating children, improving children
		set<solution> children = generateChildren(parents);

		set<solution>::iterator it;

		// check if some of the generated children is equal to one of the parents (do not insert duplicate values)
		for (it = children.begin(); it != children.end(); ) {
			if (parents.find(*it) != parents.end()) {
				set<solution>::iterator next = ++it;
				children.erase(--it); // remove element if it already exists in parents set
				it = next;
				continue;
			}
			it++;
		}

		// STEP 5 - replacement
		// set k as parent size / 2 
		int k = parents.size() / 2;
		if (children.size() < k) {
			// if there are no enough child, fit the number of replacements
			k = children.size();
		}

		/* ERASE K WORST PARENTS*/
		for (int i = 0; i < k; i++) {
			/* POSITION ITERATOR */
			it = parents.begin();
			advance(it, parents.size() - 1);
			/* ERASE WORST PARENT */
			parents.erase(*it);
		}

		/* REPLACE WITH K BEST CHILDREN*/
		for (int i = 0; i < k; i++) {
			/* INSERT BEST CHILD */
			parents.insert(*children.begin());
			/* ERASE THAT CHILD */
			children.erase(*children.begin());
		}

		// substitute the last two elements of the population with two random
		it = parents.begin();
		advance(it, parents.size() - 2);
		solution randOne = *it;
		advance(it, 1);
		solution random2 = *it;
		bool ok1 = false, ok2 = false;
		while (!ok1 || !ok2){
			// iterate until we get a solution different from the ones already present in the solution set
			if (!ok1) {
				randOne.fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, indexesCost, indexesMem, configGain, randOne.matrix, true);
				if (parents.find(randOne) == parents.end()) {
					// this is a new solution - exit
					it = parents.begin();
					advance(it, parents.size() - 2);
					parents.erase(*it);
					parents.insert(randOne);
					ok1 = true;
				}
			}
			if (!ok2) {
				random2.fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, indexesCost, indexesMem, configGain, random2.matrix, true);
				if (parents.find(random2) == parents.end()) {
					// this is a new solution - exit
					it = parents.begin();
					advance(it, parents.size() - 1);
					parents.erase(*it);
					parents.insert(random2);
					ok2 = true;
				}
			}
		}
		clock_t end = clock();
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		//printf("Time spent: %f s\n", time_spent);

	}

}


/* FUNCTION TO GET INITIAL POPULATION */
set <solution> initial_population(int N_QUERIES, int N_INDEXES, int N_CONFIGURATIONS, int N_POPULATION, int MEMORY, list<int> *ci_matrix_lst, int *i_cost, int *i_mem, int **cq_gain, bool mode) {

	set<solution> init_pop;

	auto sol = new solution;
	sol->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		sol->matrix[i].resize(N_QUERIES);

	// Create 7 Greedy solutions
	sol->fitness = greedy1(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol->matrix, mode);
	init_pop.insert(*sol);
	//printf("G1: %d\n",sol->fitness);
	auto sol1 = new solution;
	sol1->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		sol1->matrix[i].resize(N_QUERIES);
	sol1->fitness = greedy2(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol1->matrix, mode);
	init_pop.insert(*sol1);
	//printf("G2: %d\n",sol->fitness);
	auto sol2 = new solution;
	sol2->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		sol2->matrix[i].resize(N_QUERIES);
	sol2->fitness = greedy3(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol2->matrix, mode);
	init_pop.insert(*sol2);
	//printf("G3: %d\n",sol->fitness);
	auto sol3 = new solution;
	sol3->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		sol3->matrix[i].resize(N_QUERIES);
	sol3->fitness = greedy4(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol3->matrix, mode);
	init_pop.insert(*sol3);
	//printf("G4: %d\n",sol->fitness);
	auto sol4 = new solution;
	sol4->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		sol4->matrix[i].resize(N_QUERIES);
	sol4->fitness = greedy5(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol4->matrix, mode);
	init_pop.insert(*sol4);
	//printf("G5: %d\n",sol->fitness);
	auto sol5 = new solution;
	sol5->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		sol5->matrix[i].resize(N_QUERIES);
	sol5->fitness = greedy6(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol5->matrix, mode);
	init_pop.insert(*sol5);
	//printf("G6: %d\n",sol->fitness);
	auto sol6 = new solution;
	sol6->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		sol6->matrix[i].resize(N_QUERIES);
	sol6->fitness = greedy7(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol6->matrix, mode);
	init_pop.insert(*sol6);
	//printf("G7: %d\n",sol->fitness);

	// Create N_POPULATION + 7 Random solutions (Random solutions will replace bad Greedy solutions)
	auto Rsol = new solution;
	Rsol->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		Rsol->matrix[i].resize(N_QUERIES);
	Rsol->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, Rsol->matrix, mode);
	init_pop.insert(*Rsol);

	auto Rsol1 = new solution;
	Rsol1->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		Rsol1->matrix[i].resize(N_QUERIES);
	Rsol1->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, Rsol1->matrix, mode);
	init_pop.insert(*Rsol1);

	auto Rsol2 = new solution;
	Rsol2->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		Rsol2->matrix[i].resize(N_QUERIES);
	Rsol2->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, Rsol2->matrix, mode);
	init_pop.insert(*Rsol2);

	auto Rsol3 = new solution;
	Rsol3->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		Rsol3->matrix[i].resize(N_QUERIES);
	Rsol3->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, Rsol3->matrix, mode);
	init_pop.insert(*Rsol3);

	auto Rsol4 = new solution;
	Rsol4->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		Rsol4->matrix[i].resize(N_QUERIES);
	Rsol4->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, Rsol4->matrix, mode);
	init_pop.insert(*Rsol4);

	auto Rsol5 = new solution;
	Rsol5->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		Rsol5->matrix[i].resize(N_QUERIES);
	Rsol5->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, Rsol5->matrix, mode);
	init_pop.insert(*Rsol5);

	auto Rsol6 = new solution;
	Rsol6->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		Rsol6->matrix[i].resize(N_QUERIES);
	Rsol6->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, Rsol6->matrix, mode);
	init_pop.insert(*Rsol6);

	auto Rsol7 = new solution;
	Rsol7->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		Rsol7->matrix[i].resize(N_QUERIES);
	Rsol7->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, Rsol7->matrix, mode);
	init_pop.insert(*Rsol7);

	auto Rsol8 = new solution;
	Rsol8->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		Rsol8->matrix[i].resize(N_QUERIES);
	Rsol8->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, Rsol8->matrix, mode);
	init_pop.insert(*Rsol8);

	auto Rsol9 = new solution;
	Rsol9->matrix.resize(N_CONFIGURATIONS);
	for (int i = 0; i < N_CONFIGURATIONS; i++)
		Rsol9->matrix[i].resize(N_QUERIES);
	Rsol9->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, Rsol9->matrix, mode);
	init_pop.insert(*Rsol9);

/*	for (uint32_t n = init_pop.size(); n < N_POPULATION + 7; n = init_pop.size()) {
		sol->fitness = random(N_QUERIES, N_INDEXES, N_CONFIGURATIONS, MEMORY, ci_matrix, i_cost, i_mem, cq_gain, sol->matrix, mode);
		init_pop.insert(*sol);
		//if(n < init_pop.size())printf("R%d: %d\n",n+1,sol->fitness);
	}*/

	// Erase 7 worst solutions
	set<solution>::iterator it;
	while(init_pop.size() > 10){
	//for (uint8_t n = 0; n < 7; n++) {
		it = init_pop.begin();
		advance(it, init_pop.size() - 1);
		init_pop.erase(*it);
	}

	// save best solution found so far
	bestFitnessFoundSoFar = init_pop.begin()->fitness;
	safeWrite.lock(); // make sure no one will stop solution update
	outfile->seekp(0); // start writing from the beginning of the file
	for (int j = 0; j < N_CONFIGURATIONS; j++) {
		for (int k = 0; k < N_QUERIES; k++) {
			if (k < N_QUERIES - 1)
				*outfile << init_pop.begin()->matrix[j][k] << " ";
			else
				*outfile << init_pop.begin()->matrix[j][k]; // do not add a space in the last column
		}
		if(j < N_CONFIGURATIONS - 1)
			*outfile << endl; // do not add a line feed for the last row
	}
	safeWrite.unlock();

	return init_pop;
}

set<solution> generateChildren(set<solution> parents) {
	set<solution> children;
	
	// while there are element in set
	while (parents.size() > 0) {
		// take the first element in set (the one with the biggest fitness) and remove it from the set
		solution best = *parents.begin();
		parents.erase(best);

		// now look for a random element
		solution random;
		// generate a random number between 0 and parent.size
		if (parents.size() > 0) {
			int i = (rand() % parents.size());
			set<solution>::iterator it = parents.begin();
			// move iterator of "i" position forward
			advance(it, i);
			random = *it;
		}
		else {
			// odd number of parents. return here without performing further computation on the last parent
			return children;
		}

		// generate children and add them to the children set
		performCrossover(best, random, &children);

		// remove random solution from set
		parents.erase(random);
	}

	// children has been generated
	return children;
}

void performCrossover(solution best, solution random, set<solution>* children) {
	// calculate crossing points as 1/3 and 2/3 of the lenght of the matrix
	int crossing1 = N_QUERIES / 3;
	int crossing2 = (N_QUERIES / 3) * 2;
	solution child1;
	solution child2;
	child1.matrix.resize(N_CONFIGURATIONS);
	child2.matrix.resize(N_CONFIGURATIONS);
	for (int c = 0; c < N_CONFIGURATIONS; c++) {
		child1.matrix[c].resize(N_QUERIES);
		child2.matrix[c].resize(N_QUERIES);
	}

	map<int, int> confXquery1; // store configuration used by qeury of child 1 (useful for fitness calculation)
	map<int, int> confXquery2; // store configuration used by query of child 2
	int j = 0, k = 0;
	for (int i = 0; i < N_QUERIES * N_CONFIGURATIONS; i++) {
		if (k < crossing1 || k > crossing2) {
			// here we are at the edges of the matrix, copy the values as expected
			child1.matrix[j][k] = best.matrix[j][k];
			child2.matrix[j][k] = random.matrix[j][k];
			if ((child1.matrix)[j][k] == 1) { // configuration j is used for query j
				if (configGain[j][k] > 0) // consider configuration active only if the gain is greater than 0. Otherwise let default configuration serve this query
					confXquery1.insert(make_pair(k, j)); // save query k and its configuration j
				else
					child1.matrix[j][k] = 0;
			}
			if ((child2.matrix)[j][k] == 1) {
				if (configGain[j][k] > 0) // consider configuration active only if the gain is greater than 0. Otherwise let default configuration serve this query
					confXquery2.insert(make_pair(k, j));
				else
					child2.matrix[j][k] = 0;
			}
		}
		else {
			// here we are between the 2 crossing points, swap values
			child1.matrix[j][k] = random.matrix[j][k];
			child2.matrix[j][k] = best.matrix[j][k];
			if ((child1.matrix)[j][k] == 1) { // configuration j is used for query j
				if (configGain[j][k] > 0) // consider configuration active only if the gain is greater than 0. Otherwise let default configuration serve this query
					confXquery1.insert(make_pair(k, j)); // save query k and its configuration j
				else
					child1.matrix[j][k] = 0;
			}
			if ((child2.matrix)[j][k] == 1) {
				if (configGain[j][k] > 0) // consider configuration active only if the gain is greater than 0. Otherwise let default configuration serve this query
					confXquery2.insert(make_pair(k, j));
				else
					child2.matrix[j][k] = 0;
			}
		}
		// change row if needed
		if (++k % N_QUERIES == 0) {
			k = 0;
			j++;
		}
	}
	// children has been created, now we need to check their feasibility and fitness value
	// calculateFitness() returns -1 if solution is infeasible
	child1.fitness = calculateFitness(&confXquery1);
	child2.fitness = calculateFitness(&confXquery2);

	// check if we found a solution better then our current best one
	if (child1.fitness > bestFitnessFoundSoFar || child2.fitness > bestFitnessFoundSoFar) {
		// one of the 2 solution (or both) are better than the best one,
		// write out the better between the 2 solution so that, if both
		// of them are better, just the best between them will be written.
		// (we avoid to update best solution twice in this case)
		if (child1.fitness > child2.fitness) {
			bestFitnessFoundSoFar = child1.fitness;
			safeWrite.lock(); // make sure no one will stop solution update
			outfile->seekp(0); // start writing from the beginning of the file
			for (int j = 0; j < N_CONFIGURATIONS; j++) {
				for (int k = 0; k < N_QUERIES; k++) {
					if (k < N_QUERIES - 1)
						*outfile << child1.matrix[j][k] << " ";
					else
						*outfile << child1.matrix[j][k]; // do not add a space in the last column
				}
				if(j < N_CONFIGURATIONS - 1)
					*outfile << endl; // do not add a line feed for the last row
			}
			safeWrite.unlock();
		}
		else {
			bestFitnessFoundSoFar = child2.fitness;
			safeWrite.lock(); // make sure no one will stop solution update
			outfile->seekp(0); // start writing from the beginning of the file
			for (int j = 0; j < N_CONFIGURATIONS; j++) {
				for (int k = 0; k < N_QUERIES; k++) {
					if (k < N_QUERIES - 1)
						*outfile << child2.matrix[j][k] << " ";
					else
						*outfile << child2.matrix[j][k]; // do not add a space in the last column
				}
				if (j < N_CONFIGURATIONS - 1)
					*outfile << endl; // do not add a line feed for the last row
			}
			safeWrite.unlock();
		}
		//printf("Best Solution: %d", bestFitnessFoundSoFar);
	}

	map<int, int> best1, best2;
	// STEP 4 - children improvement
	int bestFitness1 = child1.fitness;
	int bestFitness2 = child2.fitness;
	for (int c = 0; c < N_CONFIGURATIONS; c++) {
		map<int, int> onConfQuery1 = confXquery1; // create a copy of confXquery and modify this copy rather than the original one
		map<int, int> onConfQuery2 = confXquery2;
		map<int, int> offConfQuery1 = confXquery1; // create a copy of confXquery and modify this copy rather than the original one
		map<int, int> offConfQuery2 = confXquery2;


		bool changed1 = false, changed2 = false, changed1Off = false, changed2off = false;
		int bestForRow1 = 0, bestForRow2 = 0;
		// in onConfQuery1 try to make all queries served by configuration c if gain is greather than 0
		// in offConfQuery1 try to set to 0 all queries served by configuration c
		list<int> supportList;
		for (int q = 0; q < N_QUERIES; q++) {
			if (configGain[c][q] > 0) {
				supportList.push_front(q);

				// before activating this configuration for the specified query, check if query was served by another configuration,
				// if this is the case, remove that configuration for this query or the solution will become infeasible

				// child 1 - on map
				map<int, int>::iterator found = onConfQuery1.find(q);
				if (found != onConfQuery1.end()) {
					// remove old query-configuration pair
					onConfQuery1.erase(found);
				}
				// add new query-configuration pair
				onConfQuery1.insert(make_pair(q, c));

				changed1 = true;
				int currentFit = calculateFitness(&onConfQuery1);
				if (currentFit > bestForRow1) {
					bestForRow1 = currentFit;
					best1 = onConfQuery1;
				}

				// child 2 - on map
				found = onConfQuery2.find(q);
				if (found != onConfQuery2.end()) {
					// remove old query-configuration pair
					onConfQuery2.erase(found);
				}
				// add new query-configuration pair
				onConfQuery2.insert(make_pair(q, c));

				changed2 = true;

				currentFit = calculateFitness(&onConfQuery2);
				if (currentFit > bestForRow2) {
					bestForRow2 = currentFit;
					best2 = onConfQuery2;
				}
			}

			map<int, int>::iterator found = offConfQuery1.find(q);
			if (found != offConfQuery1.end()) {
				if (found->second == c) {
					// remove old query-configuration pair
					offConfQuery1.erase(found);
					changed1Off = true;
				}
			}

			found = offConfQuery2.find(q);
			if (found != offConfQuery2.end()) {
				if (found->second == c) {
					// remove old query-configuration pair
					offConfQuery2.erase(found);
					changed2off = true;
				}
			}
		}

		map<int, int> allConfQuery1 = offConfQuery1; // create a copy of confXquery and modify this copy rather than the original one
		map<int, int> allConfQuery2 = offConfQuery2;
		// now change all the other values one by one
		int k = supportList.size() - 1;
		map<int, int>::iterator found;
		while (k > 0) {
			for (int j = 0; j < k; j++) {
				list<int>::iterator currentq = supportList.begin();
				advance(currentq, j);
				// remove previous query
				if (j > 1) {
					// child 1
					found = allConfQuery1.find(*(--currentq));
					if (found != allConfQuery1.end()) {
						if (found->second == c)
							allConfQuery1.erase(found);
					}
					currentq++;

					// child 2
					found = allConfQuery2.find(*(--currentq));
					if (found != allConfQuery2.end()) {
						if (found->second == c)
							allConfQuery2.erase(found);
					}
					currentq++;
				}

				// set current query to 1 - child 1
				found = allConfQuery1.find(*currentq);
				if (found != allConfQuery1.end()) {
					allConfQuery1.erase(found);
				}
				allConfQuery1.insert(make_pair(*currentq, c));

				// set current query to 1 - child 2
				found = allConfQuery2.find(*currentq);
				if (found != allConfQuery2.end()) {
					allConfQuery2.erase(found);
				}
				allConfQuery2.insert(make_pair(*currentq, c));

				int currentFit = calculateFitness(&allConfQuery1);
				if (currentFit > bestForRow1) {
					bestForRow1 = currentFit;
					best1 = allConfQuery1;
				}

				currentFit = calculateFitness(&allConfQuery2);
				if (currentFit > bestForRow2) {
					bestForRow2 = currentFit;
					best2 = allConfQuery2;
				}
			}
			k--;
		}
		// in order to speed up procedure, compare fitness of on and off set and write only the best between them
		// in this way if both are better than the current one we update the current one just once
		if (changed1 || changed1Off) {
			// solution of child1 has changed. Evaluate new fitness
			int sol1 = -2147483647, sol2 = -2147483647;
			if (changed1)
				sol1 = calculateFitness(&onConfQuery1);
			if (changed1Off)
				sol2 = calculateFitness(&offConfQuery1);

			map<int, int>* mapPointer;
			if (sol1 > sol2) {
				bestFitness1 = sol1;
				mapPointer = &onConfQuery1;
			}
			else {
				bestFitness1 = sol2;
				mapPointer = &offConfQuery1;
			}

			if (bestForRow1 > bestFitness1) {
				bestFitness1 = bestForRow1;
				mapPointer = &best1;
			}

			if (bestFitness1 > child1.fitness) {
				child1.fitness = bestFitness1;
				outfile->seekp(0); // start writing from the beginning of the file (if needed)
				// we found a better solution, update child1
				if (bestFitness1 > bestFitnessFoundSoFar) {
					safeWrite.lock(); // make sure no one will stop solution update
				}
				// set all the rows to 0
				for (int c = 0; c < N_CONFIGURATIONS; c++) {
					fill(child1.matrix[c].begin(), child1.matrix[c].end(), 0);
					if (bestFitness1 > bestFitnessFoundSoFar) {
						// we also found the best solution so far, update solution file
						for (int q = 0; q < N_QUERIES; q++) {
							if (q < N_QUERIES - 1)
								*outfile << child1.matrix[c][q] << " ";
							else
								*outfile << child1.matrix[c][q]; // do not add a space in the last column
						}
						if (c < N_CONFIGURATIONS - 1)
							*outfile << endl; // do not add a line feed for the last row
					}
				}
				// now add ones
				for (map<int, int>::iterator copyIt = mapPointer->begin(); copyIt != mapPointer->end(); copyIt++) {
					child1.matrix[copyIt->second][copyIt->first] = 1;
					if (bestFitness1 > bestFitnessFoundSoFar) {
						// we also need to update 1 in the output file is this is the best solution we found
						int column = (copyIt->first * 2);
						int row = copyIt->second * ((N_QUERIES * 2) + 1);
						// move cursor to the right position
						outfile->seekp(row + column);
						*outfile << "1";
					}
				}
				// update best fitness
				if (bestFitness1 > bestFitnessFoundSoFar) {
					bestFitnessFoundSoFar = bestFitness1;
					//printf("Best Solution: %d", bestFitnessFoundSoFar);
					safeWrite.unlock();
				}
			}
		}
		if (changed2 || changed2off) {
			// solution of child2 has changed. Evaluate new fitness
			int sol1 = -2147483647, sol2 = -2147483647;
			if (changed2)
				sol1 = calculateFitness(&onConfQuery2);
			if (changed2off)
				sol2 = calculateFitness(&offConfQuery2);

			map<int, int>* mapPointer;
			if (sol1 > sol2) {
				bestFitness2 = sol1;
				mapPointer = &onConfQuery2;
			}
			else {
				bestFitness2 = sol2;
				mapPointer = &offConfQuery2;
			}

			if (bestForRow2 > bestFitness2) {
				bestFitness2 = bestForRow2;
				mapPointer = &best2;
			}

			if (bestFitness2 > child2.fitness) {
				child2.fitness = bestFitness2;
				outfile->seekp(0); // start writing from the beginning of the file (if needed)
				if (bestFitness2 > bestFitnessFoundSoFar) {
					safeWrite.lock(); // make sure no one will stop solution
				}
				// we found a better solution, update child1
				for (int c = 0; c < N_CONFIGURATIONS; c++) {
					// set all the rows to 0
					fill(child2.matrix[c].begin(), child2.matrix[c].end(), 0);
					if (bestFitness2 > bestFitnessFoundSoFar) {
						// we also found the best solution so far, update solution file
						for (int q = 0; q < N_QUERIES; q++) {
							if (q < N_QUERIES - 1)
								*outfile << child2.matrix[c][q] << " ";
							else
								*outfile << child2.matrix[c][q]; // do not add a space in the last column
						}
						if (c < N_CONFIGURATIONS - 1)
							*outfile << endl; // do not add a line feed for the last row
					}
				}
				// now add ones
				for (map<int, int>::iterator copyIt = mapPointer->begin(); copyIt != mapPointer->end(); copyIt++) {
					child2.matrix[copyIt->second][copyIt->first] = 1;
					if (bestFitness2 > bestFitnessFoundSoFar) {
						// we also need to update 1 in the output file is this is the best solution we found
						int column = (copyIt->first * 2);
						int row = copyIt->second * ((N_QUERIES * 2) + 1);
						// move cursor to the right position
						outfile->seekp(row + column);
						*outfile << "1";
					}
				}
				// update best fitness
				if (bestFitness2 > bestFitnessFoundSoFar) {
					bestFitnessFoundSoFar = bestFitness2;
					//printf("Best Solution: %d", bestFitnessFoundSoFar);
					safeWrite.unlock();
				}
			}
		}
	}

	// if we got feasible solutions save them in the solution set
	if (child1.fitness >= 0) {
		children->insert(child1);
	}
	if (child2.fitness >= 0) {
		children->insert(child2);
	}
}


int calculateFitness(map<int, int>* confXquery){
	bool* alreadyUsedIndex = (bool*)malloc(sizeof(bool)*N_INDEXES);
	memset(alreadyUsedIndex, 0, N_INDEXES);

	map<int, int>::iterator it;
	int usedMemory = 0;
	int totalCost = 0;
	int totalGain = 0;
	for(it = confXquery->begin(); it != confXquery->end(); it++){
		// configuration in it->second is active. calculate cost to build related indexes
		// get the list of indexes for this configuration
		list<int> indexes = configIndexes[it->second];
		for (list<int>::iterator listIt = indexes.begin(); listIt != indexes.end(); listIt++) {
			// get first element and remove it from the list
			int index = *listIt;
			// check if I already considered the cost of this index
			if (alreadyUsedIndex[index] == false) {
				alreadyUsedIndex[index] = true;
				// update total cost and used memory
				totalCost += indexesCost[index];
				usedMemory += indexesMem[index];
				// stop calculation here if memory constraint is unsatisfied
				if (usedMemory > MEMORY) {
					// infeasible
					return -1;
				}
			}
		}
		// now calculate the gain given by using this configuration
		totalGain += configGain[it->second][it->first];
	}

	free(alreadyUsedIndex);
	// calculate and return fitness value
	return totalGain - totalCost;
}
