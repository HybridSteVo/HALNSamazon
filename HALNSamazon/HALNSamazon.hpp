#pragma once

#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <math.h>
#include <set>
#include <cassert>
#include <time.h>
#include <random>
#include <thread>
#include <boost/fusion/include/hash.hpp>
#include <boost/random/mersenne_twister.hpp>


//Namespace
using namespace std;
using std::cerr;
using std::endl;
using std::ofstream;
using std::cin;
using std::cout;

//Random Number Generator
typedef shuffle_order_engine< linear_congruential_engine< uint32_t, 1366, 150889, 714025 >, 97 > kreutzer1986;
extern mt19937 rng;



//Structures
struct Customer
{
	int index;
	double x;
	double y;
	double etw;
	double ltw;
	double historic_cost = 0.0;
	double historic_min_cost = INFINITY;
	double Tightness = 0.0;
	double Relatedness = 0.0;
	double CountRemoved = 1.0;
	int NN;
	double service_time = 0.0;
	int CompareSuccessor;
	
};

struct Route
{
	double DistanceCost = 0.0;
	double TWPenalty = 0.0;
	int FirstCustomer = -1;
	int LastCustomer = -1;
	int NumberCustomersOnRoute = 0;
	double Demand = 0;
}
;

struct Solution
{
	vector<int> Predecessor;
	vector<int> Successor;

	vector<double> Forward_E;
	vector<double> Backward_L;

	vector <double> Start_ServiceTime;

	double DistanceCost = INFINITY;
	double Penalty = INFINITY;
	double TotalCost = INFINITY;
	double PenaltyFactor;
	double Hamming;
	int Rank;

	Route RouteSet;
	vector <int> Insertion_Candidates;
	vector <int> Removal_Candidates;

};

struct CostMatrix
{
	vector<vector<double>> ij;

};

struct Operator {
	int count = 1;
	vector <int> weight = { 1,1,1 };
	double prob = 0;

};

struct ALNSobj {
	Operator RandomRemoval;
	Operator CostRemoval;
	Operator WorstRemoval;
	Operator ShawRemoval;

	Operator BestInsertion;
	Operator RegretInsertion;
	Operator RandomInsertion;
	Operator GreedyInsertion;

	Operator TWCompare;
	Operator SuccCompare;
	Operator NoCompare;
	Operator NNCompare;
};

//Saves information of insert operators (usually the best position found so far)
struct insertion_obj {
	double total_cost = INFINITY;
	double distance_cost = INFINITY;
	double tw_penalty = INFINITY;
	int CustomerIndex = -1;
	int c1 = -1;
};

//Global available 
extern vector < Customer > Ci;
extern CostMatrix CostMat;
extern CostMatrix DistMat;

//User defined or hard coded parameters
extern int numberTWs;
extern double cfailed;
extern double TWPenaltyFactor;
extern int PopulationSize;

//Instance Characteristics
extern string instance_name;
extern string IN_name;
extern string IN_type;
extern int IN_dimension;
extern double MaxDist;


//*****************
//FUNCTIONS
//*****************

//Aux Functions (in AuxiliaryFunctions.cpp)
void Nminelements(vector<double>list1, int N, vector<int>& final_list);
void Nmaxelements(vector<double>list1, int N, vector<int>& final_list);
void PrintVector(vector<int> vec);
void PrintVector(vector<double> vec);
void PrintVector(vector<bool> vec);
void PrintRoutes(Solution CurrentSolution);
void PrintStart_ServiceTime(Solution CurrentSolution);
size_t HashFunction(Solution& CurrentSolution);
int FindElement(vector<int> vec, int SearchItem);
int FindElement(vector<double> vec, double SearchItem);
void quickDeleteID(int IdToErase, vector<int>& vec);
void quickDeleteValueWithOrder(int ValueToErase, vector<int>& vec);
void quickDeleteIDWithOrder(int IDToErase, vector<int>& vec);

//Read and Write Instances and Results
int ReadOriginalInstance(string instance_name);

//Cost Calculation Functions (in CostCalculations.cpp)
void CalcAllRoutes(Solution& CurrentSolution);
void CalcSolutionCosts(Solution& CurrentSolution);
void CalcRouteCosts(Solution& CurrentSolution);

//Start Solutions (in GenerateStartSolutions.cpp)
void InitializeSolution(Solution& CurrentSolution);

//Insertion Operators and Functions (in InsertOperators.cpp)
void InsertAtBest(Solution& CurrentSolution, int candidate, int c1);
void BestInsertion(Solution& CurrentSolution, int candidate, insertion_obj& obj);
void BestRouteInsertionWithFixedTW(Solution& CurrentSolution, bool NoiseSwitch);


//Removal Operators and Functions (in RemovalOperators.cpp)
void RemoveCustomer(Solution& CurrentSolution, int candidate);
void UpdateRemovalCandidates_Tabu(Solution& CurrentSolution, Solution& BestSolution, int NumberOfTrials, double p_binom);
void UpdateRemovalCandidates(Solution& CurrentSolution, Solution& BestSolution, int NumberOfTrials, double p_binom);
void LastRemoval(Solution& CurrentSolution);
void RandomRemoval(Solution& CurrentSolution, int NumberOfTrials);
void CostRemoval(Solution& CurrentSolution, int NumberOfTrials);
void WorstRemoval(Solution& CurrentSolution, int NumberOfTrials);
void ShawRemoval(Solution& CurrentSolution, int NumberOfTrials);


//TimeWindow Functions (in TimeWindowFunctions.cpp)
void Prepair(Solution& CurrentSolution);
void Initialize(Solution& CurrentSolution);
double GetPenalty(Solution& CurrentSolution, int c1, int candidate, int c3);
void ForwardUpdate(Solution& CurrentSolution, int ActualCustomer);
void BackwardUpdate(Solution& CurrentSolution, int ActualCustomer);


//Simulated Annealing (in SimulatedAnnealing.cpp)
bool compareByTightness(const int& a, const int& b);
double SimulatedAnnealing(Solution& CurrentSolution, Solution& IncumbentSolution, Solution& BestSolution, Solution& VRPSolution, Solution& PopulationsBestSolution, double Temp, int Iterations, double Temp_low, double cool_rate, double p_accept, double& p_binom, bool WithCompareSolution, int gen);


//ALNS (in ALNSFunctions.cpp)
extern double s1;
extern double s2;
extern double s3;
extern double rate;

void UpdateALNSCounts(ALNSobj& ALNS, int case_removal, int case_insertion, int case_compare);
void UpdateALNSWeights(ALNSobj& ALNS, int case_removal, int case_insertion, int case_compare, int successType);
void UpdateALNSProbs(ALNSobj& ALNS);
void ResetALNS(ALNSobj& ALNS);
int rouletteWheel(ALNSobj& ALNS, int OperatorNumber, string OperatorType);


//Write Solution (in WriteSolutionFile.cpp)
void WriteSolution(Solution CurrentSolution, string instance_name, string scenario_name, int replication, double runtime);