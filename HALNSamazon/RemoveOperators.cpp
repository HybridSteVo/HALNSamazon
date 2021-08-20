#include "HALNSamazon.hpp"
uniform_real_distribution<double> randdouble(0, 1);
void UpdateRemovalCandidates_Tabu(Solution& CurrentSolution, Solution& BestSolution, int NumberOfTrials, double p_binom)
{

	vector<int> DifferentSuccessors;

	int Difference;
	bool noRemove = true;

	for (int i = 2; i <= IN_dimension; i++)
	{

		if (BestSolution.Successor[i] == CurrentSolution.Successor[i])
		{
			DifferentSuccessors.push_back(i);
			noRemove = false;
		}

	}

	if (noRemove == false)
	{
		CurrentSolution.Removal_Candidates = DifferentSuccessors;
	}
}

void UpdateRemovalCandidates(Solution& CurrentSolution, Solution& BestSolution, int NumberOfTrials, double p_binom)
{
	
	vector<int> DifferentSuccessors;

	int Difference;
	bool noRemove = true;

	for (int i = 2; i <= IN_dimension; i++)
	{

		if (BestSolution.Successor[i] != CurrentSolution.Successor[i] || randdouble(rng) < p_binom)
		{
			DifferentSuccessors.push_back(i);
			noRemove = false;
		}
		
	}

	if (noRemove == false)
	{
		CurrentSolution.Removal_Candidates = DifferentSuccessors;
	}
}


void RemoveCustomer(Solution& CurrentSolution, int candidate)
{
		
	if (CurrentSolution.RouteSet.NumberCustomersOnRoute == 1)
	{
		CurrentSolution.RouteSet.FirstCustomer = -1;
		CurrentSolution.RouteSet.LastCustomer = -1;
	}
	//Update Sucessor 
	else if (candidate == CurrentSolution.RouteSet.FirstCustomer)
	{
		int c3 = CurrentSolution.Successor[candidate];
		CurrentSolution.RouteSet.FirstCustomer = c3;
		CurrentSolution.Predecessor[c3] = 1;
	}
	else if (candidate == CurrentSolution.RouteSet.LastCustomer)
	{
		int c1 = CurrentSolution.Predecessor[candidate];
		CurrentSolution.RouteSet.LastCustomer = c1;
		CurrentSolution.Successor[c1] = 1;
	}
	else
	{
		int c1 = CurrentSolution.Predecessor[candidate];
		int c3 = CurrentSolution.Successor[candidate];
		CurrentSolution.Successor[c1] = c3;
		CurrentSolution.Predecessor[c3] = c1;
	}

	CurrentSolution.Predecessor[candidate] = -1;
	CurrentSolution.Successor[candidate] = -1;
	Ci[candidate].CountRemoved += 1.0;

}


void LastRemoval(Solution& CurrentSolution)
{	
	//Choose Customer to remove
	int candidate = CurrentSolution.Removal_Candidates.back();

	//Remove Customer from Route
	RemoveCustomer(CurrentSolution, candidate);
	
	//Update Candidate Lists
	CurrentSolution.Removal_Candidates.pop_back();
	CurrentSolution.Insertion_Candidates.push_back(candidate);

	//Routes have changed
	CurrentSolution.RouteSet.NumberCustomersOnRoute--;

	
}


void RandomRemoval(Solution& CurrentSolution, int NumberOfTrials)
{
	random_shuffle(CurrentSolution.Removal_Candidates.begin(), CurrentSolution.Removal_Candidates.end());


	//Remove NumberOfTrials customers from the solution with removal operators
	for (int i = 0; i < NumberOfTrials; i++)
	{
		LastRemoval(CurrentSolution);
	}
}



bool compareByHistoricCost_dec(const int& a, const int& b)
{
	return Ci[a].historic_cost / Ci[a].CountRemoved > Ci[b].historic_cost / Ci[b].CountRemoved;
}

void CostRemoval(Solution& CurrentSolution, int NumberOfTrials)
{
	sort(CurrentSolution.Removal_Candidates.begin(), CurrentSolution.Removal_Candidates.end(), compareByHistoricCost_dec);


	//Remove NumberOfTrials customers from the solution with removal operators
	for (int i = 0; i < NumberOfTrials; i++)
	{
		LastRemoval(CurrentSolution);
	}
}


void WorstRemoval(Solution& CurrentSolution, int NumberOfTrials)
{
	for (int i = 0; i < NumberOfTrials; i++)
	{
		int c1;
		int candidate;
		int c3;
		int worst_candidate;
		double worst_cost = -INFINITY;

		for (int i = 0; i < CurrentSolution.Removal_Candidates.size(); i++) {


			candidate = CurrentSolution.Removal_Candidates[i];
			c1 = CurrentSolution.Predecessor[candidate];
			c3 = CurrentSolution.Successor[candidate];
			double cost = 0;

			//Compare Costs: 
			if (true)
			{
				double TWPenalty_candidate = CurrentSolution.RouteSet.TWPenalty;

				if (c1 == 1)
				{
					TWPenalty_candidate -= max(CostMat.ij[c1][c3] - CurrentSolution.Backward_L[c3], 0.0) * TWPenaltyFactor;

				}
				else
				{
					TWPenalty_candidate -= max(CurrentSolution.Forward_E[c1] + CostMat.ij[c1][c3] - CurrentSolution.Backward_L[c3], 0.0) * TWPenaltyFactor;
				}

				TWPenalty_candidate = (TWPenalty_candidate / TWPenaltyFactor);

				cost = (CostMat.ij[c1][candidate] + CostMat.ij[candidate][c3] - CostMat.ij[c1][c3]) + TWPenalty_candidate;
			}
			else
				cost = (CostMat.ij[c1][candidate] + CostMat.ij[candidate][c3] - CostMat.ij[c1][c3]);




			if (cost - Ci[candidate].historic_min_cost >= worst_cost)
			{
				worst_candidate = candidate;
				worst_cost = cost - Ci[candidate].historic_min_cost;
			}
		}

		candidate = worst_candidate;
		//Which Route serves the customer?


		//Remove Customer from Route
		RemoveCustomer(CurrentSolution, candidate);

		//Update Candidate Lists
		quickDeleteValueWithOrder(candidate, CurrentSolution.Removal_Candidates);
		CurrentSolution.Insertion_Candidates.push_back(candidate);

		//Routes have changed
		CurrentSolution.RouteSet.NumberCustomersOnRoute--;

		if (CurrentSolution.RouteSet.NumberCustomersOnRoute > 0) {
			ForwardUpdate(CurrentSolution, CurrentSolution.RouteSet.FirstCustomer);
			BackwardUpdate(CurrentSolution, CurrentSolution.RouteSet.LastCustomer);
		}
	}

}


bool compareByRelatedness(const int& a, const int& b)
{
	return Ci[a].Relatedness > Ci[b].Relatedness;
}

void ShawRemoval(Solution& CurrentSolution, int NumberOfTrials)
{
	uniform_int_distribution<int> rand_Customer(0, CurrentSolution.Removal_Candidates.size() - 1);
	int SeedCustomer = rand_Customer(rng);
	double SeedEarliest = CurrentSolution.Forward_E[CurrentSolution.Removal_Candidates[SeedCustomer]];
	for (int i = 0; i < CurrentSolution.Removal_Candidates.size(); i++)
	{
		Ci[CurrentSolution.Removal_Candidates[i]].Relatedness = (CurrentSolution.Forward_E[CurrentSolution.Removal_Candidates[i]] - SeedEarliest) / Ci[1].ltw + CostMat.ij[CurrentSolution.Removal_Candidates[i]][CurrentSolution.Removal_Candidates[SeedCustomer]] / MaxDist;
	}
	sort(CurrentSolution.Removal_Candidates.begin(), CurrentSolution.Removal_Candidates.end(), compareByRelatedness);

	//Remove NumberOfTrials customers from the solution with removal operators
	for (int i = 0; i < NumberOfTrials; i++)
	{
		LastRemoval(CurrentSolution);
	}
}