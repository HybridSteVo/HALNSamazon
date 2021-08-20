#include "HALNSamazon.hpp"

void WriteSolution(Solution CurrentSolution, string instance_name,  string scenario_name, int replication, double runtime) {
	
	
	ofstream myfile;
	string myFileName = instance_name;
	myFileName += "-";
	myFileName += "sol";
	myFileName += "-";
	myFileName += to_string(replication);
	myFileName += ".txt";


	myfile.open(myFileName);
	myfile << "Instance: " << instance_name << endl;
	if (CurrentSolution.Penalty==0)
	{
		myfile << "Cost: " << CurrentSolution.TotalCost << endl;
	}
	else 
	{
		myfile << "Cost: " << CurrentSolution.DistanceCost << endl;
	}
	myfile << "Runtime: " << runtime << endl;
	myfile << "Successor" << endl;
	
	myfile << 0 << endl;
	

	int FirstCustomer = CurrentSolution.RouteSet.FirstCustomer;
	int LastCustomer = CurrentSolution.RouteSet.LastCustomer;
	int CustomerIterator = FirstCustomer;

	cout << CustomerIterator-1 << ",";
	myfile << (CustomerIterator - 1) << endl;

	while (CustomerIterator != LastCustomer)
	{
		cout << CurrentSolution.Successor[CustomerIterator] << ",";
		CustomerIterator = CurrentSolution.Successor[CustomerIterator];
		myfile << (CustomerIterator - 1) << endl;
			
	}
		
	cout << CurrentSolution.Successor[CustomerIterator]-1;
		


	myfile << endl;


	myfile.close();


}