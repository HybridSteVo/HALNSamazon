#include "HALNSamazon.hpp"

//Inserts the customer candidate after c1 and before c3
//old route: c1 - c3
//new route: c1 - candidate - c3
//cases: 
// - route is empty
// - c1 = depot
// - c3 = depot
void InsertAtBest(Solution& CurrentSolution, int candidate, int c1)
{
	//Insert in empty route
	if (CurrentSolution.RouteSet.NumberCustomersOnRoute == 0)
	{	
		//Predecessor and Successor is depot ( = 1)
		CurrentSolution.Predecessor[candidate] = 1;
		CurrentSolution.Successor[candidate] = 1;

		//First and Last Customer on route is candidate
		CurrentSolution.RouteSet.FirstCustomer = candidate;
		CurrentSolution.RouteSet.LastCustomer = candidate;
	}
	else {
		//If customer has to be inserted directly after depot
		if (c1 == 1)
		{	
			//The first customer on the old route is retrieved
			int c3 = CurrentSolution.RouteSet.FirstCustomer;
			
			//The predecessor of the old first customer is the candidate
			CurrentSolution.Predecessor[c3] = candidate;
			
			//The predecessor of candidate is the depot
			CurrentSolution.Predecessor[candidate] = 1;
			
			//The successor of candidate is c3 (the old first customer)
			CurrentSolution.Successor[candidate] = c3;

			//The new first customer on the route is candidate
			CurrentSolution.RouteSet.FirstCustomer = candidate;
		}
		//IF route serves more than one customer and customer is not inserted directly after depot
		else
		{
			//Predecessor of candidate is c1 ( = the customer after which candidate is inserted)
			CurrentSolution.Predecessor[candidate] = c1;

			//The old successor of c1 is now the successor of candidate
			int c3 = CurrentSolution.Successor[c1];
			CurrentSolution.Successor[candidate] = c3;

			//The sucessor of c1 is now candidate
			CurrentSolution.Successor[c1] = candidate;

			//If the old successor is the depot, the customer will be the last on the route
			if (c3 == 1)
			{
				CurrentSolution.RouteSet.LastCustomer = candidate;
			}
			//Otherwise the predecessor of c3 is now candidate
			else
			{
				CurrentSolution.Predecessor[c3] = candidate;
			}
		}

	}

}

void BestInsertion(Solution& CurrentSolution, int candidate, insertion_obj& obj)
{
	//Initialize Variables
	int c1 = -1;
	int c3 = -1;
	double distance_cost = 0.0;
	double total_cost = 0.0;
	//Penalties according to quantity above capacity and Time Window Penalty
	double tw_penalty = 0.0;


	//Cost Calculation
	for (int i = 0; i <= CurrentSolution.RouteSet.NumberCustomersOnRoute; i++) {

		//If route is empty, predecessor and successor is depot (c1, c3 = 1)
		if (CurrentSolution.RouteSet.NumberCustomersOnRoute == 0)
		{
			c1 = 1;
			c3 = 1;
		}
		//if route is not empty
		else
		{
			//if first position:
			//customer is inserted between depot and first customer: depot - candidate - first customer
			//c1 = depot 
			//c3 = first customer on route
			if (i == 0)
			{
				c1 = 1;
				c3 = CurrentSolution.RouteSet.FirstCustomer;
			}
			//every other position:
			//customer is inserted between customers: c1 - candidate - c3
			//c1 = successor iteration before
			//c3 = successor of c1
			//Example: 
			//iteration 0: depot (c1) - candidate - first customer (c3)
			//iteration 1: first customer (old c3, now c1) - candidate - successor of first customer (c3)
			else
			{
				c1 = c3;
				c3 = CurrentSolution.Successor[c1];
			}
		}

		//Compare Costs: 
		//c1 - candidate - c3 
		//versus.
		//c1 - c3

		distance_cost = (CostMat.ij[c1][candidate] + CostMat.ij[candidate][c3] - CostMat.ij[c1][c3]);


		tw_penalty = GetPenalty(CurrentSolution, c1, candidate, c3);

		if ((distance_cost + tw_penalty) < obj.total_cost)
		{
			obj.c1 = c1;
			obj.distance_cost = distance_cost;

			obj.tw_penalty = tw_penalty;
			obj.total_cost = distance_cost + tw_penalty;

		}
		
	}
	return;
}

//Iterates across all routes to find the best position to insert the customer
void BestRouteInsertionWithFixedTW(Solution& CurrentSolution, bool NoiseSwitch)
{
	//Choose customer to insert
	int candidate = CurrentSolution.Insertion_Candidates.back();

	//Initialize variables

	int c1;
	insertion_obj best_obj;
	

	//Try to find the Route in which the customer can be inserted with the lowest cost
	//iterate over all routes


	//Find the best customer c1 after which the customer candidate is to be inserted in that route		
	BestInsertion(CurrentSolution, candidate, best_obj);

	//Remove Customer from the currrent Insertion Candidates and add it to the Removal Candidates (as the request is now inserted into a route)
	CurrentSolution.Insertion_Candidates.pop_back();
	CurrentSolution.Removal_Candidates.push_back(candidate);

	//Insert Customer into the Route with the lowest cost --> all information is in the insertion object "obj"
	c1 = best_obj.c1;
	
	//Insert Customer 
	InsertAtBest(CurrentSolution, candidate, c1);

	//Update CurrentSolution
	CurrentSolution.RouteSet.DistanceCost += best_obj.distance_cost;
	CurrentSolution.RouteSet.TWPenalty += best_obj.tw_penalty;
	CurrentSolution.RouteSet.NumberCustomersOnRoute++;

	//TimeWindow Update
	ForwardUpdate(CurrentSolution, CurrentSolution.RouteSet.FirstCustomer);
	BackwardUpdate(CurrentSolution, CurrentSolution.RouteSet.LastCustomer);

	//Update information on insertion
	Ci[candidate].historic_cost += best_obj.total_cost;
	Ci[candidate].historic_min_cost = min(best_obj.total_cost, Ci[candidate].historic_min_cost);


}