#include "HALNSamazon.hpp"

//Set all routes to "changed" and call CalcSolutionCosts, which calculates the costs of the solution
void CalcAllRoutes(Solution& CurrentSolution) {
	CalcSolutionCosts(CurrentSolution);
}

//Calculates Costs and Penalty of the whole solution by updating all routes that have changed
void CalcSolutionCosts(Solution& CurrentSolution)
{
	
	CurrentSolution.DistanceCost = 0;
	CurrentSolution.Penalty = 0;
	CurrentSolution.TotalCost = 0;
	
	CalcRouteCosts(CurrentSolution);
	
	CurrentSolution.DistanceCost += CurrentSolution.RouteSet.DistanceCost;


	CurrentSolution.Penalty = CurrentSolution.Penalty + CurrentSolution.RouteSet.TWPenalty;	

	CurrentSolution.TotalCost = CurrentSolution.DistanceCost + CurrentSolution.Penalty;

}

void CalcRouteCosts(Solution& CurrentSolution)
{
	CurrentSolution.RouteSet.DistanceCost = 0;

	int c1 = 1;
	int c2 = CurrentSolution.RouteSet.FirstCustomer;


	for (int i = 0; i <= CurrentSolution.RouteSet.NumberCustomersOnRoute; i++) 
	{
		if (i == 0)
		{
			c1 = 1;
			c2 = CurrentSolution.RouteSet.FirstCustomer;
		}
		else if (i != CurrentSolution.RouteSet.NumberCustomersOnRoute)
		{
			c1 = c2;
			c2 = CurrentSolution.Successor[c1];
		}
		else
		{
			c1 = c2;
			c2 = 1;
		}


		CurrentSolution.RouteSet.DistanceCost += CostMat.ij[c1][c2];


	}




}

