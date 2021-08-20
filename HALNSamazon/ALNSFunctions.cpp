#include "HALNSamazon.hpp"

//Reset Counters and Weights of Operators
void ResetALNS(ALNSobj& ALNS)
{

	ALNS.RandomRemoval.count = 1;
	ALNS.CostRemoval.count = 1;
	ALNS.WorstRemoval.count = 1;
	ALNS.ShawRemoval.count = 1;


	ALNS.BestInsertion.count = 1;
	ALNS.RegretInsertion.count = 1;
	ALNS.RandomInsertion.count = 1;
	ALNS.GreedyInsertion.count = 1;

	ALNS.TWCompare.count = 1;
	ALNS.SuccCompare.count = 1;
	ALNS.NNCompare.count = 1;
	ALNS.NoCompare.count = 1;

	
	for (int successType = 0; successType < 3; successType++) {
		ALNS.RandomRemoval.weight[successType] = 1;
		ALNS.CostRemoval.weight[successType] = 1;
		ALNS.WorstRemoval.weight[successType] = 1;
		ALNS.ShawRemoval.weight[successType] = 1;
	

		ALNS.BestInsertion.weight[successType] = 1;
		ALNS.RegretInsertion.weight[successType] = 1;
		ALNS.RandomInsertion.weight[successType] = 1;
		ALNS.GreedyInsertion.weight[successType] = 1;

		ALNS.TWCompare.weight[successType] = 1;
		ALNS.SuccCompare.weight[successType] = 1;
		ALNS.NNCompare.weight[successType] = 1;
		ALNS.NoCompare.weight[successType] = 1;

	}

}

//Increase Count of Operator, if Operator was used. 
//Attention!! 
//operators have to get the same number here, i.e. case_removal = 0 -> RandomRemoval
void UpdateALNSCounts(ALNSobj &ALNS, int case_removal, int case_insertion, int case_compare)
{
	switch (case_removal)
	{
	case 0:
		ALNS.RandomRemoval.count++;
		break;
	case 1:
		ALNS.CostRemoval.count++;
		break;
	case 2:
		ALNS.WorstRemoval.count++;
		break;
	case 3:
		ALNS.ShawRemoval.count++;
		break;
	}

	switch (case_insertion)
	{
	case 0:
		ALNS.BestInsertion.count++;
		break;
	case 1:
		ALNS.RegretInsertion.count++;
		break;
	case 2:
		ALNS.RandomInsertion.count++;
		break;
	case 3:
		ALNS.GreedyInsertion.count++;
		break;
	}

	switch (case_compare)
	{
		case 0:
			ALNS.TWCompare.count++;
			break;
		case 1:
			ALNS.SuccCompare.count++;
			break;
		case 2:
			ALNS.NNCompare.count++;
			break;
		case 3:
			ALNS.NoCompare.count++;
			break;
	}
}

//Update Weights if successtype 0, 1 or 2
void UpdateALNSWeights(ALNSobj& ALNS, int case_removal, int case_insertion, int case_compare, int successType)
{
	switch (case_removal)
	{

	case 0:
		ALNS.RandomRemoval.weight[successType]++;
		break;
	case 1:
		ALNS.CostRemoval.weight[successType]++;
		break;
	case 2:
		ALNS.WorstRemoval.weight[successType]++;
		break;
	case 3:
		ALNS.ShawRemoval.weight[successType]++;
		break;
	}

	switch (case_insertion)
	{

	case 0:
		ALNS.BestInsertion.weight[successType]++;
		break;
	case 1:
		ALNS.RegretInsertion.weight[successType]++;
		break;
	case 2:
		ALNS.RandomInsertion.weight[successType]++;
		break;
	case 3:
		ALNS.GreedyInsertion.weight[successType]++;
		break;
	}

	switch (case_compare)
	{

	case 0:
		ALNS.TWCompare.weight[successType]++;
		break;
	case 1:
		ALNS.SuccCompare.weight[successType]++;
		break;
	case 2:
		ALNS.NNCompare.weight[successType]++;
		break;
	case 3:
		ALNS.NoCompare.weight[successType]++;
		break;
	}
}


//Update Probabilities of operators
void UpdateALNSProbs(ALNSobj& ALNS)
{	
	
	//RemovalWeight is calculated by sum(weight_successtype_i*s_i) divided by count (actually count + 1 to avoid dividing by 0)
	vector <double> RemovalWeights;

	RemovalWeights.push_back((double)(ALNS.RandomRemoval.weight[0] * s1 + ALNS.RandomRemoval.weight[1] * s2 + ALNS.RandomRemoval.weight[2] * s3) / ((double)ALNS.RandomRemoval.count));
	RemovalWeights.push_back((double)(ALNS.CostRemoval.weight[0] * s1 + ALNS.CostRemoval.weight[1] * s2 + ALNS.CostRemoval.weight[2] * s3) / ((double)ALNS.CostRemoval.count));
	RemovalWeights.push_back((double)(ALNS.WorstRemoval.weight[0] * s1 + ALNS.WorstRemoval.weight[1] * s2 + ALNS.WorstRemoval.weight[2] * s3) / ((double)ALNS.WorstRemoval.count));
	RemovalWeights.push_back((double)(ALNS.ShawRemoval.weight[0] * s1 + ALNS.ShawRemoval.weight[1] * s2 + ALNS.ShawRemoval.weight[2] * s3) / ((double)ALNS.ShawRemoval.count));

	double SumRemovalWeights=0;
	for (auto& n : RemovalWeights)
		SumRemovalWeights += n;

	//Update operator probability by old value*(1-rate) + rate * proportional weight 
	ALNS.RandomRemoval.prob = ALNS.RandomRemoval.prob * (1 - rate) + rate * RemovalWeights[0]/ SumRemovalWeights;
	ALNS.CostRemoval.prob = ALNS.CostRemoval.prob * (1 - rate) + rate * RemovalWeights[1] / SumRemovalWeights;
	ALNS.WorstRemoval.prob = ALNS.WorstRemoval.prob * (1 - rate) + rate * RemovalWeights[2] / SumRemovalWeights;
	ALNS.ShawRemoval.prob = ALNS.ShawRemoval.prob * (1 - rate) + rate * RemovalWeights[3] / SumRemovalWeights;
	//

	//sum all probs to normalize value
	double sumProbs = ALNS.RandomRemoval.prob + ALNS.CostRemoval.prob + ALNS.WorstRemoval.prob + ALNS.ShawRemoval.prob;

	ALNS.RandomRemoval.prob /= sumProbs;
	ALNS.CostRemoval.prob /= sumProbs;
	ALNS.WorstRemoval.prob /= sumProbs;
	ALNS.ShawRemoval.prob /= sumProbs;

	//Same procedure for insertion operator
	vector <double> InsertionWeights;

	InsertionWeights.push_back((double)(ALNS.BestInsertion.weight[0]*s1 + ALNS.BestInsertion.weight[1]*s2 + ALNS.BestInsertion.weight[2]*s3) / ((double)ALNS.BestInsertion.count));
	InsertionWeights.push_back((double)(ALNS.RegretInsertion.weight[0] * s1 + ALNS.RegretInsertion.weight[1] * s2 + ALNS.RegretInsertion.weight[2] * s3) / ((double)ALNS.RegretInsertion.count));
	InsertionWeights.push_back((double)(ALNS.RandomInsertion.weight[0] * s1 + ALNS.RandomInsertion.weight[1] * s2 + ALNS.RandomInsertion.weight[2] * s3) / ((double)ALNS.RandomInsertion.count));
	InsertionWeights.push_back((double)(ALNS.GreedyInsertion.weight[0] * s1 + ALNS.GreedyInsertion.weight[1] * s2 + ALNS.GreedyInsertion.weight[2] * s3) / ((double)ALNS.GreedyInsertion.count));

	double SumInsertionWeights=0;
	for (auto& n : InsertionWeights)
		SumInsertionWeights += n;

	ALNS.BestInsertion.prob = ALNS.BestInsertion.prob * (1 - rate) + rate * InsertionWeights[0] / SumInsertionWeights;
	ALNS.RegretInsertion.prob = ALNS.RegretInsertion.prob * (1 - rate) + rate * InsertionWeights[1] / SumInsertionWeights;
	ALNS.RandomInsertion.prob = ALNS.RandomInsertion.prob * (1 - rate) + rate * InsertionWeights[2] / SumInsertionWeights;
	ALNS.GreedyInsertion.prob = ALNS.GreedyInsertion.prob * (1 - rate) + rate * InsertionWeights[3] / SumInsertionWeights;

	sumProbs = ALNS.BestInsertion.prob + ALNS.RegretInsertion.prob + ALNS.RandomInsertion.prob + ALNS.GreedyInsertion.prob;

	ALNS.BestInsertion.prob /= sumProbs;
	ALNS.RegretInsertion.prob /= sumProbs;
	ALNS.RandomInsertion.prob /= sumProbs;
	ALNS.GreedyInsertion.prob /= sumProbs;

	//Same procedure for compare operator
	vector <double> CompareWeights;

	CompareWeights.push_back((double)(ALNS.TWCompare.weight[0] * s1 + ALNS.TWCompare.weight[1] * s2 + ALNS.TWCompare.weight[2] * s3) / ((double)ALNS.TWCompare.count));
	CompareWeights.push_back((double)(ALNS.SuccCompare.weight[0] * s1 + ALNS.SuccCompare.weight[1] * s2 + ALNS.SuccCompare.weight[2] * s3) / ((double)ALNS.SuccCompare.count));
	CompareWeights.push_back((double)(ALNS.NNCompare.weight[0] * s1 + ALNS.NNCompare.weight[1] * s2 + ALNS.NNCompare.weight[2] * s3) / ((double)ALNS.NNCompare.count));
	CompareWeights.push_back((double)(ALNS.NoCompare.weight[0] * s1 + ALNS.NoCompare.weight[1] * s2 + ALNS.NoCompare.weight[2] * s3) / ((double)ALNS.NoCompare.count));

	double SumCompareWeights = 0;
	for (auto& n : CompareWeights)
		SumCompareWeights += n;

	ALNS.TWCompare.prob = ALNS.TWCompare.prob * (1 - rate) + rate * CompareWeights[0] / SumCompareWeights;
	ALNS.SuccCompare.prob = ALNS.SuccCompare.prob * (1 - rate) + rate * CompareWeights[1] / SumCompareWeights;
	ALNS.NNCompare.prob = ALNS.NNCompare.prob * (1 - rate) + rate * CompareWeights[2] / SumCompareWeights;
	ALNS.NoCompare.prob = ALNS.NoCompare.prob * (1 - rate) + rate * CompareWeights[3] / SumCompareWeights;

	sumProbs = ALNS.TWCompare.prob + ALNS.SuccCompare.prob + ALNS.NNCompare.prob + ALNS.NoCompare.prob;

	ALNS.TWCompare.prob /= sumProbs;
	ALNS.SuccCompare.prob /= sumProbs;
	ALNS.NNCompare.prob /= sumProbs;
	ALNS.NoCompare.prob /= sumProbs;

	
	
}


//RouletteWheel Selection of operator
int rouletteWheel(ALNSobj& ALNS, int OperatorNumber, string OperatorType) {

	//save all operator probs in one vector ...

	vector <double> PROBS;

	if (OperatorType == "Removal")
	{
		PROBS.push_back(ALNS.RandomRemoval.prob);
		PROBS.push_back(ALNS.CostRemoval.prob);
		PROBS.push_back(ALNS.WorstRemoval.prob);
		PROBS.push_back(ALNS.ShawRemoval.prob);
	}
	if (OperatorType == "Insertion")
	{
		PROBS.push_back(ALNS.BestInsertion.prob);
		PROBS.push_back(ALNS.RegretInsertion.prob);
		PROBS.push_back(ALNS.RandomInsertion.prob);
		PROBS.push_back(ALNS.GreedyInsertion.prob);

	}
	if (OperatorType == "Compare")
	{
		PROBS.push_back(ALNS.TWCompare.prob);
		PROBS.push_back(ALNS.SuccCompare.prob);
		PROBS.push_back(ALNS.NNCompare.prob);
		PROBS.push_back(ALNS.NoCompare.prob);

	}

	//... and sum all operator probabilities
	double sum = 0;
	for (int i = 0; i < OperatorNumber; i++)
	{
		sum += PROBS[i];
	}

	//Generate uniform real random number between 0 and probability sum = turning the wheel
	uniform_real_distribution<double> unif(0, sum);

	double rndNumber = unif(rng);
	double offset = 0.0;
	int pick = 0;
	
	//return the chosen operator
	for (int i = 0; i < OperatorNumber; i++) {
		offset += PROBS[i];
		if (rndNumber < offset) {
			pick = i;
			break;
		}
	}
	return pick;
}







