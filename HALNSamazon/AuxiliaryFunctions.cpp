#include "HALNSamazon.hpp"

void Nminelements(vector<double>list1, int N, vector<int>& final_list)
{
	for (int i = 0; i < N; i++) {
		final_list.push_back(min_element(list1.begin(), list1.end()) - list1.begin());
		list1[final_list[i]] = INFINITY;
	}
}

void Nmaxelements(vector<double>list1, int N, vector<int>& final_list)
{
	for (int i = 0; i < N; i++) {
		final_list.push_back(max_element(list1.begin(), list1.end()) - list1.begin());
		list1[final_list[i]] = -INFINITY;
	}
}


void quickDeleteID(int IdToErase, vector<int>& vec)
{
	vec[IdToErase] = vec.back();
	vec.pop_back();
}


void quickDeleteValueWithOrder(int ValueToErase, vector<int>& vec)
{
	auto it = std::find(vec.begin(), vec.end(), ValueToErase);
	if (it != vec.end())
		vec.erase(it);
}

void quickDeleteIDWithOrder(int IDToErase, vector<int>& vec)
{
	vec.erase(vec.begin() + IDToErase);
}


void PrintVector(vector<int> vec)
{
	cout << endl;
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << ", ";
	}
	cout << endl;
}

void PrintVector(vector<double> vec)
{
	cout << endl;
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << ", ";
	}
	cout << endl;
}

void PrintVector(vector<bool> vec)
{
	cout << endl;
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << ", ";
	}
	cout << endl;
}

void PrintRoutes(Solution CurrentSolution)
{

		int FirstCustomer = CurrentSolution.RouteSet.FirstCustomer;
		int LastCustomer = CurrentSolution.RouteSet.LastCustomer;
		int CustomerIterator = FirstCustomer;

		cout << CustomerIterator << ",";

		while (CustomerIterator != LastCustomer)
		{	
			cout << CurrentSolution.Successor[CustomerIterator] << ",";
			CustomerIterator = CurrentSolution.Successor[CustomerIterator];
		}
		cout << CurrentSolution.Successor[CustomerIterator];

		cout << endl;

}

void PrintStart_ServiceTime(Solution CurrentSolution)
{

		int FirstCustomer = CurrentSolution.RouteSet.FirstCustomer;
		int LastCustomer = CurrentSolution.RouteSet.LastCustomer;
		int CustomerIterator = FirstCustomer;

		while (CustomerIterator != 1)
		{
			cout << "Customer: " << CustomerIterator << ", ";
			cout << "earliest: " << Ci[CustomerIterator].etw << " - ";
			cout << CurrentSolution.Forward_E[CustomerIterator] << " - ";
			cout << Ci[CustomerIterator].ltw << ", ";
			cout << "Distance: " << CostMat.ij[CurrentSolution.Predecessor[CustomerIterator]][CustomerIterator] << ", ";
			cout << "L: " << CurrentSolution.Backward_L[CustomerIterator] << ", ";
			cout << endl;
			CustomerIterator = CurrentSolution.Successor[CustomerIterator];
			
		}
		

		cout << endl;
	
}


int FindElement(vector<int> vec, int SearchItem)
{
	int i = -1;
	bool found = false;
	for (auto& elem : vec)
	{
		i++;
		if (elem == SearchItem)
		{
			found = true;
			break;
		}
	}
	if (found == false)
	{
		i = -1;
	}
	return i;
}

int FindElement(vector<double> vec, double SearchItem)
{
	int i = -1;
	bool found = false;
	for (auto& elem : vec)
	{
		i++;
		if (elem == SearchItem)
		{
			found = true;
			break;
		}
	}
	if (found == false)
	{
		i = -1;
	}
	return i;
}

size_t HashFunction(Solution& CurrentSolution)
{
	boost::hash<vector<int>> hash_fn;
	size_t str_hash = hash_fn(CurrentSolution.Successor);
	return str_hash;
}

