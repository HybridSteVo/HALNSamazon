#include "HALNSamazon.hpp"

//Instance Characteristics Declaration
string instance_name = "a";
string IN_name = "";
string IN_type = "";
int IN_dimension;


//Parameter
vector < Customer > Ci;
double TWPenaltyFactor = 0.1;

//ALNS Parameter
double s1 = 35;
double s2 = 2;
double s3 = 1;
double rate = 0.30;

//Genetic Algorithm Parameter
int PopulationSize = 12;
int gens = 10;
string type = "VRPTW";

//Random Number Generator
//with random seed
auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
mt19937 rng(seed);
//with fixed seed
//mt19937 rng;


//Compare Functions
bool compareByTotalCost(const Solution& Solution_a, const Solution& Solution_b)
{
	return Solution_a.TotalCost - Solution_a.Penalty + (Solution_a.Penalty / Solution_a.PenaltyFactor) * 1e9 <
		Solution_b.TotalCost - Solution_b.Penalty + (Solution_b.Penalty / Solution_b.PenaltyFactor) * 1e9;
}

bool compareByHamming(const Solution& Solution_a, const Solution& Solution_b)
{
	return Solution_a.Hamming > Solution_b.Hamming;
}

bool compareByRank(const Solution& Solution_a, const Solution& Solution_b)
{
	return Solution_a.Rank < Solution_b.Rank;
}



int main(int argc, char* argv[])
{
	//Default Inputs and Parameter
	double Temp_low = 1000;
	double cool_rate = 0.9997;
	double p_accept = 0.10;
	double p_binom = 0.15;
	double original_p_binom = 0.15;
	int iteration = 1;
	int replication = 1;
	bool WithCompareSolution = false;
	int BestSolutionFoundInGen = 0;
	instance_name = "RouteID_00a25e47-65c9-4efc-bfe9-8d9648a0df4e";

	Solution OverallBestSolution;
	Solution VRPSolution;
	Solution PopulationsBestSolution;
	vector<Solution> FinalSolutionHistory;

	if (argc > 1) {
		instance_name = argv[1];
		//WithActualSequence = atoi(argv[2]);
		iteration = atoi(argv[3]);
		//cool_rate = atof(argv[3]);
		//PopulationSize = atof(argv[4]);
		//gens = atoi(argv[5]);
		//p_accept = atof(argv[6]);
		//original_p_binom = atof(argv[7]);
		//Temp_low = atoi(argv[8]);
		//cfailed = atof(argv[9]);
	}

	double original_Temp_low = Temp_low;

	//Start Time
	clock_t startTime_complete = clock();

	//Generate VRP Solution
	WithCompareSolution = false;
	ReadOriginalInstance(instance_name);
		
	Solution CurrentSolution;
	Solution IncumbentSolution;
	Solution BestSolution;

	p_binom = original_p_binom;

	//Generates Start Solution by applying BestInsertion until all Customers are inserted
	InitializeSolution(CurrentSolution);

	IncumbentSolution = CurrentSolution;
	BestSolution = CurrentSolution;
	VRPSolution = CurrentSolution;

	//Determine Temp0 of SA by executing 400 iterations with high temperature and no cooling
	double Temp0 = SimulatedAnnealing(CurrentSolution, IncumbentSolution, BestSolution, VRPSolution, PopulationsBestSolution, INFINITY, 400, 400, 1, p_accept, p_binom, WithCompareSolution, 10000);

	//Simulated Annealing
	CurrentSolution = BestSolution;
	IncumbentSolution = BestSolution;

	SimulatedAnnealing(CurrentSolution, IncumbentSolution, BestSolution, VRPSolution, PopulationsBestSolution, Temp0, 5e5, Temp_low, cool_rate, p_accept, p_binom, WithCompareSolution, 10000);
	VRPSolution = BestSolution;

	//WriteSolution(VRPSolution, instance_name, "VRP", iteration, 0, 0, cool_rate, p_accept, VRPSolution.VehiclesUsed);
	Prepair(VRPSolution);
	CalcAllRoutes(VRPSolution);
	FinalSolutionHistory.push_back(VRPSolution);
	

	cout << "VRPSolution.DistanceCost: " << VRPSolution.DistanceCost << endl;
	double TWPenaltyLow = (VRPSolution.DistanceCost) * 0.1 / (9.0 * 3600.0);
	double TWPenaltyHigh = (VRPSolution.DistanceCost) * 2.00 / (9.0 * 3600.0);


	uniform_real_distribution<double> randpbinom(0.10, 0.35); //0.12, 0.24 for VRPMTW


	//Start Genetic Algorithm
	for (int gen = 0; gen < gens; gen++) {
		uniform_real_distribution<double> randPenalty(TWPenaltyLow, TWPenaltyHigh);

		if (gen == 0 || gen%5==0)
		{
			WithCompareSolution = false;
		}
		else
		{
			WithCompareSolution = true;
		}
		replication = PopulationSize * gen + 1;
		for (replication; replication <= gen * PopulationSize + PopulationSize; replication++) {
			TWPenaltyFactor = randPenalty(rng);


			if (replication < PopulationSize * gen + 0)
			{
				WithCompareSolution = false;
				//cout << endl << replication << ": without" << endl;

			}
			else
				if (gen != 0)
				{
					WithCompareSolution = true;
					//cout << endl << replication << ": with" << endl;
				}

			//Measure runtime
			clock_t startTime = clock();
			
			//Initialize Solution
			Solution CurrentSolution;
			Solution IncumbentSolution;
			Solution BestSolution;

			

			//p_binom = original_p_binom;
			p_binom = randpbinom(rng);

			//Generates Start Solution by applying BestInsertion until all Customers are inserted
			InitializeSolution(CurrentSolution);

			if (replication == 1)
			{
				CurrentSolution = VRPSolution;
			}

			IncumbentSolution = CurrentSolution;
			BestSolution = CurrentSolution;

			if (gen != 0 && gen%5!=0)
			{
				CurrentSolution = FinalSolutionHistory[replication - gen * PopulationSize - 1];
				IncumbentSolution = FinalSolutionHistory[replication - gen * PopulationSize - 1];
				BestSolution = FinalSolutionHistory[replication - gen * PopulationSize - 1];
				Temp_low = original_Temp_low;
			}

			//Determine Temp0 of SA by executing 400 iterations with high temperature and no cooling
			double Temp0 = SimulatedAnnealing(CurrentSolution, IncumbentSolution, BestSolution, VRPSolution, PopulationsBestSolution, INFINITY, 400, 400, 1, p_accept, p_binom, WithCompareSolution, gen);

			//Simulated Annealing
			CurrentSolution = BestSolution;
			IncumbentSolution = BestSolution;

			SimulatedAnnealing(CurrentSolution, IncumbentSolution, BestSolution, VRPSolution, PopulationsBestSolution, Temp0, 5e5, Temp_low, cool_rate, p_accept, p_binom, WithCompareSolution, gen);

			if (BestSolution.TotalCost < OverallBestSolution.TotalCost)
			{
				OverallBestSolution = BestSolution;
				BestSolutionFoundInGen = gen;
			}

			FinalSolutionHistory.push_back(BestSolution);

			//Print Results
			if (false) {
				cout << endl;
				cout << endl;
				cout << "Solution Report: ";
				cout << endl;
				cout << "Runtime: ";
				cout << double(clock() - startTime) / (double)CLOCKS_PER_SEC << " seconds." << endl;
				cout << endl;

				cout << "Best Solution Cost: " << BestSolution.TotalCost << endl;
				cout << "Best Solution Penalty: " << BestSolution.Penalty << endl;
				cout << "Best Solution Routing Cost: " << BestSolution.DistanceCost << endl;
				cout << "Penalty/Factor: " << BestSolution.Penalty / TWPenaltyFactor << endl;

				double Runtime = double(clock() - startTime) / (double)CLOCKS_PER_SEC;
			}



		}

		sort(FinalSolutionHistory.begin(), FinalSolutionHistory.end(), compareByTotalCost);
		PopulationsBestSolution = FinalSolutionHistory[0];
		if(PopulationsBestSolution.TotalCost< OverallBestSolution.TotalCost)
		{
			OverallBestSolution = PopulationsBestSolution;
		}

		FinalSolutionHistory[0].Rank = 0;
		FinalSolutionHistory[0].Hamming = INFINITY;



		for (int sol = 1; sol < FinalSolutionHistory.size(); sol++)
		{
			vector<int> BestSuccessor = FinalSolutionHistory[sol].Successor;
			int Difference = 0;
			FinalSolutionHistory[sol].Rank = sol;
			for (int j = 0; j < FinalSolutionHistory.size(); j++)
			{
				vector<int> CurrentSuccessor = FinalSolutionHistory[j].Successor;
				for (int i = 2; i <= IN_dimension; i++)
				{

					if (BestSuccessor[i] != CurrentSuccessor[i])
					{
						Difference++;
					}
				}
			}
			FinalSolutionHistory[sol].Hamming = Difference;
		}

		sort(FinalSolutionHistory.begin(), FinalSolutionHistory.end(), compareByHamming);

		for (int sol = 0; sol < FinalSolutionHistory.size(); sol++)
		{
			FinalSolutionHistory[sol].Rank = 2*FinalSolutionHistory[sol].Rank + sol;
		}



		sort(FinalSolutionHistory.begin(), FinalSolutionHistory.end(), compareByRank);

		
		while (FinalSolutionHistory.size() > 4 * PopulationSize)
		{
			FinalSolutionHistory.pop_back();
		}

	}


	//Print Results
	cout << endl;
	cout << endl;
	cout << endl;
	cout << "Solution Report: ";
	cout << endl;
	cout << "Overall Runtime: ";
	cout << double(clock() - startTime_complete) / (double)CLOCKS_PER_SEC << " seconds." << endl;
	cout << endl;

	cout << "Overall Best Solution Cost: " << OverallBestSolution.TotalCost << endl;
	cout << "Overall Best Solution Penalty: " << OverallBestSolution.Penalty << endl;
	cout << "Overall Best Solution Routing Cost: " << OverallBestSolution.DistanceCost << endl;
	cout << "Found in Gen: " << BestSolutionFoundInGen << endl;
	

	//Begin Report
	bool reporting = false;
	if (reporting) {
		PrintRoutes(OverallBestSolution);
		PrintStart_ServiceTime(OverallBestSolution);
		double TotalWaiting = 0.0;

		cout << OverallBestSolution.RouteSet.Demand << ", ";
		TotalWaiting += CostMat.ij[OverallBestSolution.RouteSet.LastCustomer][1] + OverallBestSolution.Forward_E[OverallBestSolution.RouteSet.LastCustomer] - OverallBestSolution.RouteSet.DistanceCost;


		cout << endl;
		for (int i = 2; i <= IN_dimension; i++)
		{
			cout << Ci[i].index << ": ";
			cout << Ci[i].etw << " - ";
			cout << OverallBestSolution.Forward_E[i] << " - ";
			cout << Ci[i].ltw << " - distance: ";
			cout << CostMat.ij[OverallBestSolution.Predecessor[i]][i] << endl;
		}

		cout << "TotalDuration: " << (double)TotalWaiting + OverallBestSolution.TotalCost << endl;
		//End REport
	}


	cout << "end";

	double OverallRuntime = double(clock() - startTime_complete) / (double)CLOCKS_PER_SEC;
	WriteSolution(OverallBestSolution, instance_name, "overall", iteration, OverallRuntime);
	cout << endl;

	return 0;
}