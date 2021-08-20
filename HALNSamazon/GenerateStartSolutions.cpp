#include "HALNSamazon.hpp"

void InitializeSolution(Solution& CurrentSolution)
{

	CurrentSolution.PenaltyFactor = TWPenaltyFactor;

	for (int i = 0; i <= IN_dimension; i++)
	{

		CurrentSolution.Predecessor.push_back(-1);
		CurrentSolution.Successor.push_back(-1);
		CurrentSolution.Start_ServiceTime.push_back(-1);
	}


	CurrentSolution.Forward_E.push_back(-1);
	CurrentSolution.Backward_L.push_back(-1);

	for (int i = 1; i <= IN_dimension; i++)
	{
		CurrentSolution.Forward_E.push_back(Ci[i].etw);
		CurrentSolution.Backward_L.push_back(Ci[i].ltw);
	}

	for (int i = 2; i <= IN_dimension; i++)
	{
		CurrentSolution.Insertion_Candidates.push_back(i);
	}

	random_shuffle(CurrentSolution.Insertion_Candidates.begin(), CurrentSolution.Insertion_Candidates.end());
	int NumberOfTrials = CurrentSolution.Insertion_Candidates.size();
	uniform_int_distribution<int> randint(0, 1);

	if (randint(rng)==0)
	{
	for (int i = 0; i < NumberOfTrials; i++) {
		BestRouteInsertionWithFixedTW(CurrentSolution, true);
		}
	}
	else {
		for (int i = 0; i < NumberOfTrials; i++) {
			BestRouteInsertionWithFixedTW(CurrentSolution, false);
		}
	}


	Prepair(CurrentSolution);
	CalcSolutionCosts(CurrentSolution);
	



}

