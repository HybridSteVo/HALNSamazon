#include "HALNSamazon.hpp"

double GetPenalty(Solution& CurrentSolution, int c1, int candidate, int c3)
{

	double Forward_E_candidate;
	double TWPenalty_candidate = 0.0;


		TWPenalty_candidate = 0.0;

		if (c1 == 1)
		{
			Forward_E_candidate = max(DistMat.ij[c1][candidate] + Ci[candidate].service_time, Ci[candidate].etw + Ci[candidate].service_time);
			TWPenalty_candidate = max(DistMat.ij[c1][candidate] - Ci[candidate].ltw, 0.0);

		}
		else
		{
			Forward_E_candidate = max(CurrentSolution.Forward_E[c1] + Ci[candidate].service_time + DistMat.ij[c1][candidate], Ci[candidate].etw + Ci[candidate].service_time);
			TWPenalty_candidate = max(CurrentSolution.Forward_E[c1] + DistMat.ij[c1][candidate] - Ci[candidate].ltw, 0.0);
		}

		TWPenalty_candidate += max(Forward_E_candidate + DistMat.ij[candidate][c3] - CurrentSolution.Backward_L[c3], 0.0);

		if (TWPenalty_candidate > 0) {
			TWPenalty_candidate = TWPenalty_candidate * TWPenaltyFactor;
		}


	return (TWPenalty_candidate);


}

void Prepair(Solution& CurrentSolution)
{

	int FirstCustomer = CurrentSolution.RouteSet.FirstCustomer;
	int LastCustomer = CurrentSolution.RouteSet.LastCustomer;

	ForwardUpdate(CurrentSolution, FirstCustomer);
	BackwardUpdate(CurrentSolution, LastCustomer);

}


void Initialize(Solution& CurrentSolution)
{

	ForwardUpdate(CurrentSolution, CurrentSolution.RouteSet.FirstCustomer);
	BackwardUpdate(CurrentSolution, CurrentSolution.RouteSet.LastCustomer);
	
}

void ForwardUpdate(Solution& CurrentSolution, int ActualCustomer)
{
	CurrentSolution.RouteSet.TWPenalty = 0;
	int PreviousCustomer = 1;


	if (ActualCustomer != CurrentSolution.RouteSet.FirstCustomer)
	{
		PreviousCustomer = CurrentSolution.Predecessor[ActualCustomer];
	}
	else
	{
		CurrentSolution.Forward_E[ActualCustomer] = max(DistMat.ij[PreviousCustomer][ActualCustomer] + Ci[ActualCustomer].service_time, Ci[ActualCustomer].etw + Ci[ActualCustomer].service_time);
		CurrentSolution.RouteSet.TWPenalty += max(DistMat.ij[PreviousCustomer][ActualCustomer] - Ci[ActualCustomer].ltw, 0.0);
		
		PreviousCustomer = ActualCustomer;
		ActualCustomer = CurrentSolution.Successor[ActualCustomer];

	}


	while (ActualCustomer != 1)
	{

		CurrentSolution.Forward_E[ActualCustomer] = max(CurrentSolution.Forward_E[PreviousCustomer] + DistMat.ij[PreviousCustomer][ActualCustomer] + Ci[ActualCustomer].service_time, Ci[ActualCustomer].etw + Ci[ActualCustomer].service_time);
		
		if (CurrentSolution.Forward_E[PreviousCustomer] + DistMat.ij[PreviousCustomer][ActualCustomer] > Ci[ActualCustomer].ltw)
		{
			CurrentSolution.RouteSet.TWPenalty += max(CurrentSolution.Forward_E[PreviousCustomer] + DistMat.ij[PreviousCustomer][ActualCustomer] - Ci[ActualCustomer].ltw, 0.0);

			CurrentSolution.Forward_E[ActualCustomer] = Ci[ActualCustomer].ltw+ Ci[ActualCustomer].service_time;
		}
		
		PreviousCustomer = ActualCustomer;
		ActualCustomer = CurrentSolution.Successor[ActualCustomer];


	}

	ActualCustomer = CurrentSolution.RouteSet.LastCustomer;
	CurrentSolution.RouteSet.TWPenalty += max(CurrentSolution.Forward_E[ActualCustomer] + DistMat.ij[ActualCustomer][1] - Ci[1].ltw, 0.0);
	
}

void BackwardUpdate(Solution& CurrentSolution, int ActualCustomer)
{

	int FollowingCustomer = 1;
	if (ActualCustomer != CurrentSolution.RouteSet.LastCustomer)
	{
		FollowingCustomer = CurrentSolution.Successor[ActualCustomer];
	}
	else
	{
		CurrentSolution.Backward_L[ActualCustomer] = min(Ci[ActualCustomer].ltw, CurrentSolution.Backward_L[FollowingCustomer] - DistMat.ij[ActualCustomer][FollowingCustomer] - Ci[ActualCustomer].service_time);
		FollowingCustomer = ActualCustomer;
		ActualCustomer = CurrentSolution.Predecessor[ActualCustomer];
	}

	while (ActualCustomer != 1)
	{
		CurrentSolution.Backward_L[ActualCustomer] = min(Ci[ActualCustomer].ltw, CurrentSolution.Backward_L[FollowingCustomer] - DistMat.ij[ActualCustomer][FollowingCustomer] - Ci[ActualCustomer].service_time);
		FollowingCustomer = ActualCustomer;
		ActualCustomer = CurrentSolution.Predecessor[ActualCustomer];

	}

	ActualCustomer = CurrentSolution.RouteSet.FirstCustomer;

	CurrentSolution.RouteSet.TWPenalty += -1.0 * min(CurrentSolution.Backward_L[ActualCustomer], 0.0);

	CurrentSolution.RouteSet.TWPenalty *= TWPenaltyFactor;


}