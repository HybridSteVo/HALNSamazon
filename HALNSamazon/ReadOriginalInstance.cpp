#include "HALNSamazon.hpp"
CostMatrix CostMat;
CostMatrix DistMat;
double MaxDist;

int ReadOriginalInstance(string instance_name) {

	ifstream inFile;
	string str;

	char cwd[256];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Current working dir: %s\n", cwd);
	}
	else {
		perror("getcwd() error");
		return 1;
	}
	str = string(cwd) + "/Instances/" + instance_name;

	cout << "file name" << str << endl;
	inFile.open(str);
	if (!inFile) {
		cout << str << endl;
		cout << "Unable to open file";
		exit(1); // terminate with error
	}

	string uselessStr;

	//Customer
	inFile >> uselessStr;
	inFile >> uselessStr;
	inFile >> IN_dimension;
	
	cout << IN_dimension << endl;


	//Start Time
	inFile >> uselessStr;
	inFile >> uselessStr;
	inFile >> uselessStr;
	double start_service_of_route;
	inFile >> start_service_of_route;
	cout << start_service_of_route << endl;

	//Read Distance to every other Node
	CostMat.ij = vector<vector<double>>(IN_dimension + 1, vector<double>(IN_dimension + 1));
	DistMat.ij = vector<vector<double>>(IN_dimension + 1, vector<double>(IN_dimension + 1));

	for (int i = 1; i <= IN_dimension; i++)
	{
		for (int j = 1; j <= IN_dimension; j++)
		{
			inFile >> CostMat.ij[i][j];
		}
	}

	//Service Time
	inFile >> uselessStr;
	inFile >> uselessStr;
	inFile >> uselessStr;
	Ci = vector <Customer>(IN_dimension + 1);
	
	Ci[1].service_time = 0.0;

	for (int i = 2; i <= IN_dimension; i++)
	{
		inFile >> Ci[i].service_time;
	}
	
	//Time_Window_Section
	inFile >> uselessStr;
	inFile >> uselessStr;
	inFile >> uselessStr;

	Ci[0].etw=(0.0);
	Ci[0].ltw=(0.0);
	Ci[1].etw=(0.0);
	Ci[1].ltw=((9.0*3600.0));

	for (int i = 2; i <= IN_dimension; i++)
	{
		double e;
		double l;
		inFile >> e;
		inFile >> l;
		
		Ci[i].etw=(e- start_service_of_route);
		Ci[i].ltw=(l-Ci[i].service_time- start_service_of_route);
		cout <<"e: "<< Ci[i].etw << "," ;
		cout << "l: " << Ci[i].ltw << endl;

		Ci[i].Tightness = l - e;
	}

	inFile.close();

	//Check triangle inequality
	double Dij = 0.0;
	int changes = 0;
	bool repeat = true;

	while (repeat)
	{
		changes = 0;
		for (int i = 1; i <= IN_dimension; i++)
		{
			for (int j = 1; j <= IN_dimension; j++)
			{
				Dij = CostMat.ij[i][j];
				for (int k = 1; k <= IN_dimension; k++)
				{
					if (Dij > CostMat.ij[i][k] + CostMat.ij[k][j])
					{
						//cout << i <<" - "<< j << " - " << k<< endl;
						CostMat.ij[i][j] = CostMat.ij[i][k] + CostMat.ij[k][j];
						changes += 1;
					}

				}
			}
		}
		if (changes==0)
		{
			repeat = false;
		}
		cout << endl << changes << endl;
	}
	
	//Set DistanceMatrix (for times)
	for (int i = 1; i <= IN_dimension; i++)
	{
		for (int j = 1; j <= IN_dimension; j++)
		{

			DistMat.ij[i][j] = CostMat.ij[i][j];
		}
	
	}
	
	//Standardize CostMat	
	vector<double>time_list;
	double cum_time = 0.0;
	for (int i = 1; i <= IN_dimension; i++)
	{
		for (int j = 1; j <= IN_dimension; j++)
		{
			time_list.push_back(CostMat.ij[i][j]);
			cum_time += CostMat.ij[i][j];
		}
	}

	double avg_time = cum_time / time_list.size();
	

	double var = 0;
	for (int n = 0; n < time_list.size(); n++)
	{
		var += (time_list[n] - avg_time) * (time_list[n] - avg_time);
	}

	var /= time_list.size();

	double std_time = sqrt(var);

	double min_new_time = INFINITY;


	for (int i = 1; i <= IN_dimension; i++)
	{
		for (int j = 1; j <= IN_dimension; j++)
		{

			double old_time = DistMat.ij[i][j];
			double new_time = (old_time - avg_time) / std_time;
			if (new_time < min_new_time)
			{
				min_new_time = new_time;
				CostMat.ij[i][j] = new_time;
			}
		}
	}
	for (int i = 1; i <= IN_dimension; i++)
	{
		for (int j = 1; j <= IN_dimension; j++)
		{
			double new_time = CostMat.ij[i][j];
			double shifted_time = new_time - min_new_time;
			CostMat.ij[i][j] = shifted_time;
		}
	}
	
	//Reduce weights on edges from and to the depot
	for (int j = 1; j <= IN_dimension; j++)
	{

		CostMat.ij[1][j] = 0.10 * CostMat.ij[1][j];
		CostMat.ij[j][1] = 0.10 * CostMat.ij[j][1];

	}

	//Calculate nearest neighbor
	for (int i = 1; i <= IN_dimension; i++)
	{
		double shortestdistance = INFINITY;
		double distance = 0;
		double time_distance = 0;
		for (int j = 1; j <= IN_dimension; j++)
		{
			if (i != j) {
				if (Ci[i].etw + CostMat.ij[i][j] <= Ci[j].ltw);
				{time_distance = abs(Ci[i].etw - Ci[j].etw); }

				distance = CostMat.ij[i][j] + time_distance;
				if (distance < shortestdistance)
				{
					shortestdistance = distance;
					Ci[i].NN = j;
				}
			}
		}
	}

	//Get Maximum Distance
	double MaxDist = -INFINITY;
	for (int i = 2; i <= IN_dimension; i++)
	{
		for (int j = 2; j <= IN_dimension; j++) {

			if (CostMat.ij[i][j] > MaxDist && i != j)
			{
				MaxDist = CostMat.ij[i][j];
			}

		}
	}

	return 0;
}


