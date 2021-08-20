#include "HALNSamazon.hpp"


bool compareByTightness(const int& a, const int& b)
{	
	return Ci[a].Tightness > Ci[b].Tightness;
}

bool compareByHistoricCost(const int& a, const int& b)
{
	return Ci[a].historic_cost/Ci[a].CountRemoved < Ci[b].historic_cost/ Ci[b].CountRemoved;
}


double SimulatedAnnealing(Solution& CurrentSolution, Solution& IncumbentSolution, Solution& BestSolution, Solution& VRPSolution, Solution& PopulationsBestSolution, double Temp, int Iterations, double Temp_low, double cool_rate, double p_accept, double& p_binom, bool WithCompareSolution, int gen) {
	
	
	
	for (auto& n : Ci)
	{
		n.historic_min_cost = INFINITY;
		n.historic_cost = 0.0;
		n.CountRemoved = 1.0;
	}

	Solution CompareSolution = BestSolution;
	

	if (WithCompareSolution)
	{
		CompareSolution = PopulationsBestSolution;
	}


	
	int NoImprovement = 0;
	

	//Random Distributions
	uniform_real_distribution<double> unif(0, 1);
	binomial_distribution<int> binom_p(IN_dimension - 1, p_binom);
	uniform_int_distribution<int> randint(0, 1);

	//Initialization
	vector<double> SampleSolution;

	

	vector<int> p_binom_successType;
	vector<int> p_binom_Trials;

	int NumberOfTrials;
	bool NoiseSwitch;

	ALNSobj ALNS;

	int case_removal;
	int case_insertion;
	int case_compare;
	int successType;

	set<size_t> SolutionHistory;

	//Updates Operator Probabilities
	UpdateALNSProbs(ALNS);
	
	
	

	vector<int> Removal_Candidates;
	for (int i = 2; i <= IN_dimension; i++)
	{
		Removal_Candidates.push_back(i);
	}

	//Execute iterations until maximum iterations or ...
	for (int n = 0; n < Iterations; n++) {
		
		//... break if Temperature falls below Temp_low
		if (NoImprovement>Temp_low || (gen==-1 && BestSolution.Penalty==0))
		{
			//cout << "\n number of it: " << n;
			//cout << "\n Temp: " << Temp;
			break;
		}

		NoImprovement++;
		

		//choose removal and insertion operator
		successType = -1;
		
		
		

		
		if (WithCompareSolution)
		{
			case_compare = rouletteWheel(ALNS, 2, "Compare"); 
			case_removal = rouletteWheel(ALNS, 4, "Removal");
			case_insertion = rouletteWheel(ALNS, 4, "Insertion");
		}
		else
		{
			case_compare = rouletteWheel(ALNS, 2, "Compare");
			case_removal = rouletteWheel(ALNS, 4, "Removal");
			case_insertion = rouletteWheel(ALNS, 4, "Insertion");
		}
		
		//Copy IncumbentSolution
		CurrentSolution = IncumbentSolution;
		
		//Shuffle Removal candidates randomly (operators will then remove always the last of the removal candidate vector)
		//Removal candidates are all customers inserted in a route
		
		//Number of Customers to be removed is binomial distributed
		binomial_distribution<int> binom_p(IN_dimension-2, p_binom);
		NumberOfTrials = max(2,binom_p(rng));
		
		CurrentSolution.Removal_Candidates = Removal_Candidates;
		
		if (WithCompareSolution && CompareSolution.TotalCost < BestSolution.TotalCost)
		{
			switch (case_compare) {
			case 0:
				UpdateRemovalCandidates(CurrentSolution, CompareSolution, NumberOfTrials, p_binom);
				break;
			case 1:
				UpdateRemovalCandidates(CurrentSolution, CompareSolution, NumberOfTrials, p_binom);
				break;
			}
		}	
		else
		{
			switch (case_compare){
				case 0:
					UpdateRemovalCandidates(CurrentSolution, BestSolution, NumberOfTrials, p_binom);
					break;
				case 1:
					UpdateRemovalCandidates(CurrentSolution, BestSolution, NumberOfTrials, p_binom);
					break;
				case 2:
					UpdateRemovalCandidates(CurrentSolution, CompareSolution, NumberOfTrials, p_binom);
					break;
				case 3:
					UpdateRemovalCandidates(CurrentSolution, CompareSolution, NumberOfTrials, p_binom);
					break;
			}
		}
		
		//Remove NumberOfTrials customers from the solution with removal operators
		NumberOfTrials = min((int)CurrentSolution.Removal_Candidates.size(), NumberOfTrials);

		switch (case_removal)
		{
		case 0:
			RandomRemoval(CurrentSolution, NumberOfTrials);
			break;
		case 1:
			CostRemoval(CurrentSolution, NumberOfTrials);	
			break;
		case 2:
			WorstRemoval(CurrentSolution, NumberOfTrials);
			break;
		case 3:
			ShawRemoval(CurrentSolution, NumberOfTrials);
			break;
		}
		

		//Calculate Solution Costs after customers have been removed, updates only changed Routes
		Prepair(CurrentSolution);

		//Shuffle insertion candidates (insertion candidates have been previously removed)		
		switch (case_insertion)
		{
		case 0:
			break;
		case 1:
			sort(CurrentSolution.Insertion_Candidates.begin(), CurrentSolution.Insertion_Candidates.end(), compareByHistoricCost);
			break;
		case 2:
			sort(CurrentSolution.Insertion_Candidates.begin(), CurrentSolution.Insertion_Candidates.end(), compareByTightness);
			break;
		case 3:
			sort(CurrentSolution.Insertion_Candidates.begin(), CurrentSolution.Insertion_Candidates.end(), compareByHistoricCost);
			break;
		}
			

		//Call Insertion Operator until all customers have been inserted
		NumberOfTrials = CurrentSolution.Insertion_Candidates.size();

		
		for (int i = 0; i < NumberOfTrials; i++){
			switch (case_insertion) {
			case 0: 
				BestRouteInsertionWithFixedTW(CurrentSolution, false);
				break;
			case 1:
				BestRouteInsertionWithFixedTW(CurrentSolution, false);
				break;
			case 2:
				BestRouteInsertionWithFixedTW(CurrentSolution, false);
				break;
			case 3:
				BestRouteInsertionWithFixedTW(CurrentSolution, false);
				break;
			}
			
		}
		

		Prepair(CurrentSolution);
		CalcSolutionCosts(CurrentSolution);

		//Calculate hash of solution
		size_t hashvalue = HashFunction(CurrentSolution);


		//Determine if current solution is better than incumbent solution
		if (CurrentSolution.TotalCost < IncumbentSolution.TotalCost)
		{
			
			//update incumbent solution
			IncumbentSolution = CurrentSolution;
			IncumbentSolution.PenaltyFactor = TWPenaltyFactor;
			//cout << "\n Current better than Incumbent Solution Cost in " << n << ": " << IncumbentSolution.Cost;
			
			//if it is a previously unknown solution -> successtype 1
			if (SolutionHistory.find(hashvalue) == SolutionHistory.end())
			{
				successType = 1;
				SolutionHistory.insert(hashvalue);
				p_binom_successType.push_back(s2);
				p_binom_Trials.push_back(NumberOfTrials);
				
			}


			//if the solution is better than best known solution -> successtype -> 0
			if (IncumbentSolution.TotalCost < BestSolution.TotalCost)
			{
				
				successType = 0;

				BestSolution = IncumbentSolution;
				

				Prepair(BestSolution);
				CalcAllRoutes(BestSolution);
				
				//cout << "\n New Best Solution Cost in " << n << ": " << IncumbentSolution.TotalCost << "after: " << BestSolution.TotalCost;
				

				p_binom_successType.push_back(s1);
				p_binom_Trials.push_back(NumberOfTrials);
				NoImprovement = 0;

				
			}
		}
		//If current solution is not better than incumbent
		else
		{
			//SampleSolution saves the first 1000 solution solutions which are worse than the incumbent solution
			//needed to determine Temp0 of Simulated Annealing
			if (n < 1000) {
				
				SampleSolution.push_back(CurrentSolution.TotalCost - BestSolution.TotalCost);
				
			}
			//Check if Solution is still accepted according to SA criterion, if accepted and previously unknown successtype -> 2
			double currentRandomNumber = unif(rng);

			double delta = exp(-(CurrentSolution.TotalCost - BestSolution.TotalCost)/(Temp));
	
			if (delta > currentRandomNumber && delta < 1) {
				
				IncumbentSolution = CurrentSolution;
				IncumbentSolution.PenaltyFactor = TWPenaltyFactor;
				//cout << "\n SA accepted Cost in " << n << ": " << IncumbentSolution.TotalCost;
				if (SolutionHistory.find(hashvalue) == SolutionHistory.end())
				{
					successType = 2;
					SolutionHistory.insert(hashvalue);
					p_binom_successType.push_back(s3);
					p_binom_Trials.push_back(NumberOfTrials);
				}
				
			}
		}

		//Reduce temperature
		Temp *= cool_rate;

		//Update ALNS weights if either successtype 0,1 or 2
		if (successType != -1)
		{
			UpdateALNSWeights(ALNS, case_removal, case_insertion, case_compare, successType);
			//UpdateALNSProbs(ALNS);
		}
		if (n == 1000)
		{
			p_binom_successType = vector<int>();
			p_binom_Trials = vector<int>();
		}
		//Update Operator Probabilities every 1000th iteration
		if (n%50==0 && n > 1000)
		{
			if (n % 1000 == 0) {
				for (auto& n : Ci)
				{
					
					n.historic_cost = 0.0;
					n.historic_min_cost = INFINITY;
					n.CountRemoved = 1.0;
				}
			}
			
			if (p_binom_Trials.size() > 100) {
				UpdateALNSProbs(ALNS);
				double p_binom_Trials_Sum = 0;
				double p_binom_Weights_Sum = 0;

				for (int i = 0; i < p_binom_Trials.size(); i++)
				{
					p_binom_Trials_Sum += p_binom_Trials[i] * p_binom_successType[i];
					p_binom_Weights_Sum += p_binom_successType[i];
				}

				p_binom = p_binom*(1.0-rate) + rate*((double)p_binom_Trials_Sum / (double)p_binom_Weights_Sum) / (double)(IN_dimension-1);
				
				
				p_binom_successType = vector<int>();
				p_binom_Trials = vector<int>();			

			}

		}
	}

	//Function returns Temp0, which is Average of SampleSum (average over cost increases) divided by log of acceptance probability
	double SampleSum = 0;

	for (auto& n : SampleSolution)
		SampleSum += n;

	Temp = -(SampleSum / (double)SampleSolution.size()) / log(p_accept);

	//Print ALNS Report
	if (false) {
		cout << endl << "ALNS Report: " << endl;


		cout << "\nTWCompare: " << ALNS.TWCompare.prob;
		cout << "\nSuccCompare: " << ALNS.SuccCompare.prob;
		cout << "\nNNCompare: " << ALNS.NNCompare.prob;
		cout << "\nNoCompare: " << ALNS.NoCompare.prob << endl;;

		cout << "\nRandomRemoval: " << ALNS.RandomRemoval.prob;
		cout << "\nCost: " << ALNS.CostRemoval.prob;
		cout << "\nWorst: " << ALNS.WorstRemoval.prob;
		cout << "\nShawRemoval: " << ALNS.ShawRemoval.prob << endl;



		cout << "\nTightnessInsertion: " << ALNS.BestInsertion.prob << endl;
		cout << "PenaltyInsertion: " << ALNS.RegretInsertion.prob << endl;
		cout << "Rand TightnessInsertion: " << ALNS.RandomInsertion.prob << endl;
		cout << "Rand PenaltyInsertion: " << ALNS.GreedyInsertion.prob << endl;

		cout << endl << "number of unique solutions: " << SolutionHistory.size() << endl;
	}

	//Update all routes, to be sure that delta evaluations worked
	Prepair(BestSolution);
	CalcAllRoutes(BestSolution);
	
	

	return Temp;
}