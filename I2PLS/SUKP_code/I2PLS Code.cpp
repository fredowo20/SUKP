//============================================================================
// Name        : I2PLS.cpp
// Author      : Zequn Wei (zequn.wei@gmail.com)
// Organization: LERIA, Université d’Angers, 2 Boulevard Lavoisier, 49045 Angers, France.
// Revised	   : March 2019
// Copyright   : This code can be freely used only for academic purpose.
// Description : Iterated two-phase local search for the Set-Union Knapsack Problem.
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <sstream>

#define MAXINT 2147483647
#define MININT -2147483648

char *instanceName;
int numItem;
int numElement;
int knapsackSize;
int *profit;			//profit of each item
int *weight;			//weight of each element
int **relationMatrix;
int **elementAddress;  //the elements correspond to each item
int *lengthEleAdd;	   //number of elements correspond to each item
int *chosenItem;	   //the selected items
int *chosenElement;	   //the selected
int *repeatElement;	   //number of each elements of the selected items
int *recordSwap;	   //record the frequency of moved items (integer vector F)

int totalProfit;	   //record the total profit of current solution
int totalWeight;	   //record the total weight of current solution
int bestTotalProfit;
int bestTotalWeight;

double maxRunTime;	   //time limit
double bestTime;
int seed;

using namespace std;

/* Read instance */
void read_instance()
{
	ifstream FIC;
	FIC.open(instanceName);

	if(FIC.fail())
	{
		cout << "1-can not open the file " << instanceName << endl;
		exit(0);
	}
	if(FIC.eof())
	{
		cout << "2-can not open the file " << instanceName << endl;
	}

	numItem = 0;
	numElement = 0;
	knapsackSize = 0;

	char str_reading[100];
	char str_ilne[50];
	char *p;
	int str[50];
	int a = 0;

	//read numItem/numElement/knapsack size
	for(int i = 0; i < 3; i++)
		FIC.getline(str_ilne,sizeof(str_ilne));

	p = strtok(str_ilne,"=");
	while(p != NULL)
	{
		str[a] = atoi(p);
		p = strtok(NULL,"=");
		a++;
	}
	numItem = str[1];
	numElement = str[2];
	knapsackSize = str[3];
	cout << "Read instance :" << "numItem= " << numItem << ", numElement= " << numElement << ", knapsackSize= " << knapsackSize << endl;

	lengthEleAdd = new int[numItem];
	chosenItem = new int[numItem];
	chosenElement = new int[numElement];
	repeatElement = new int[numElement];
	recordSwap = new int[numItem];

	//read profit of each item
	for(int i = 0; i < 5; i++)
		FIC >> str_reading;
	profit = new int[numItem + 1];
	for(int i = 0; i < numItem; i++)
	{
		FIC >> str_reading;
		profit[i] = atoi(str_reading);
	}
	profit[numItem] = 0;

	//read weight of each element
	for(int i = 0; i < 5; i++)
		FIC >> str_reading;
	weight = new int[numElement];
	for(int i = 0; i < numElement; i++)
	{
		FIC >> str_reading;
		weight[i] = atoi(str_reading);
	}

	//read the relation matrix
	a = 0;
	for(int i = 0; i < 2; i++)
		FIC >> str_reading;
	relationMatrix = new int *[numItem];
	for(int x = 0; x < numItem; x++)
		relationMatrix[x] = new int [numElement];
	for(int x = 0; x < numItem; x++)
		for(int y = 0; y < numElement; y++)
			relationMatrix[x][y] = 0;
	for(int x = 0; x < numItem; x++)
		for(int y = 0; y < numElement; y++)
		{
			FIC >> a;
			relationMatrix[x][y] = a;
		}

	cout << "Finish loading data." << endl;
	FIC.close();
}

/*Quick sort*/
void qsort(double *s, int *address, int l, int r)
{
	if (l < r)
	{
		int i = l, j = r;
		double x = s[l];
		int y = address[l];
		while (i < j)
		{
			while(i < j && s[j] <= x)
				j--;
			if(i < j)
				{
					s[i] = s[j];
					address[i] = address[j];
					i++;
				}

			while(i < j && s[i] > x)
				i++;
			if(i < j)
				{
					s[j] = s[i];
					address[j] = address[i];
					j--;
				}
		}
		s[i] = x;
		address[i] = y;
		qsort(s, address, l, i - 1);
		qsort(s, address, i + 1, r);
	}
}

/*Generate an initial solution*/
void initial_solution()
{
	double descent_profit_ratio [numItem];
	double profit_ratio[numItem];
	int item_weight [numItem];
	int descent_profit_ratio_address [numItem];
	int index;
	int sum_weight;
	int temp_weight;
	int initial_profit = 0;
	int initial_total_weight = 0;
	int l = 0, r = numItem;

	for(int i = 0; i < numItem; i++)
	{
		lengthEleAdd[i] = 0;
		profit_ratio[i] = 0.0;
		item_weight[i] = 0;
		descent_profit_ratio_address[i] = i;
		chosenItem[i] = 0;
	}
	for(int i = 0; i < numElement; i++)
	{
		chosenElement[i] = 0;
		repeatElement[i] = 0;
	}

	elementAddress = new int *[numItem];
	for(int x = 0; x < numItem; x++)
		elementAddress[x] = new int [numElement];
	for(int x = 0; x < numItem; x++)
		for(int y = 0; y < numElement; y++)
			elementAddress[x][y] = 0;

	for(int x = 0; x < numItem; x++)
	{
		index = 0;
		for(int y = 0; y < numElement; y++)
			if(relationMatrix[x][y] == 1)
			{
				elementAddress[x][index] = y;
				index++;
			}
		lengthEleAdd[x] = index;
	}

	//count weight of each item
	for(int x = 0; x < numItem; x++)
	{
		sum_weight = 0;
		for(int y = 0; y < lengthEleAdd[x]; y++)
		{
			sum_weight += weight[elementAddress[x][y]];
		}
		item_weight[x] = sum_weight;
	}

	//count profit_ratio of each item
	for(int i = 0; i < numItem; i++)
		{
			profit_ratio[i] = (double) profit[i] / item_weight[i];
			descent_profit_ratio[i] = profit_ratio[i];
		}

	qsort(descent_profit_ratio,descent_profit_ratio_address,l,r);

	int temp2 = 0;
	temp_weight = index = 0;
	while (temp_weight <= knapsackSize)
	{
		temp_weight = 0;
		temp2 = descent_profit_ratio_address[index];
		chosenItem[temp2] = 1;

		for(int i = 0; i < lengthEleAdd[temp2]; i++)
		{
			chosenElement[elementAddress[temp2][i]] = 1;
			repeatElement[elementAddress[temp2][i]] += 1;
		}

		for(int j = 0; j< numElement; j++)
			if(chosenElement[j] == 1)
				temp_weight += weight[j];
		index++;
	}

	chosenItem[temp2] = 0;  			//remove the last item to satisfy the constraint
	for(int k = 0; k < numItem; k++)
		if(chosenItem[k] == 1)
			initial_profit += profit[k];
	totalProfit = initial_profit;

	for(int i = 0; i < lengthEleAdd[temp2]; i++)
		repeatElement[elementAddress[temp2][i]] -= 1;

	for(int i = 0; i < numElement; i++)
			if(repeatElement[i] > 0)
				initial_total_weight += weight[i];
	totalWeight = initial_total_weight;
}

void variable_neighborhood_descent_search(int *bestChosenItem, int *bestChosenElement, int *bestRepeatElement, double beginTime)
{
	int is_improve;
	int remove_element;
	int add_element;
	int temp_re_ele[numElement], changed_element[numElement];
	int old_weight, new_weight, new_total_weight;
	int remove_neighboor[500], add_neighboor[500];
	int num_best_neighboor;
	int delta_profit = 0;
	int best_delta_profit = 0;
	int best_new_total_weight[500];
	int index = 0;
	int remove_item, add_item;
	int length1, length2;  //number of the selected items and non-selected items
	int count;
	int selected_address[numItem + 1], non_selected_address[numItem + 1];  //index of the selected items and non-selected items
	double sampling_probability;

	is_improve = 1;
	while(is_improve != 0)
	{
		if((clock()-beginTime) / CLOCKS_PER_SEC > maxRunTime)
			return;

		for(int x = 0; x < numItem + 1; x++)
		{
			selected_address[x] = 0;
			non_selected_address[x] = 0;
		}

		count = length2 = 0;
		for(int i = 0; i < numItem; i++)
			if(chosenItem[i] == 0)
			{
				non_selected_address[count] = i;
				count ++;
			}
		non_selected_address[count] = numItem;	//equal to do not add
		count ++;
		length2 = count;

		count = length1 = 0;
		for(int j = 0; j < numItem; j++)
			if(chosenItem[j] == 1)
			{
				selected_address[count] = j;
				count ++;
			}
		selected_address[count] = numItem;		//equal to do not remove
		count ++;
		length1 = count;

		//Explore the neighborhood N1
		best_delta_profit = num_best_neighboor = 0;
		for(int a = 0; a < length1; a ++)
			for(int b = 0; b < length2; b ++)
			{
				if(non_selected_address[b] == numItem)
					continue;

				//filtering strategy
				delta_profit = profit[non_selected_address[b]] - profit[selected_address[a]];
				if(delta_profit <= 0)
					continue;

				old_weight = new_weight = 0;
				for(int k = 0; k< numElement; k++)
				{
					temp_re_ele[k] = repeatElement[k];
					changed_element[k] = 0;
				}

				if(selected_address[a] != numItem)
					for(int p = 0; p < lengthEleAdd[selected_address[a]]; p++)
					{
						remove_element = elementAddress[selected_address[a]][p];
						temp_re_ele[remove_element] -= 1;
						changed_element[remove_element] = 1;
					}

				for(int q = 0; q < lengthEleAdd[non_selected_address[b]]; q++)
				{
					add_element = elementAddress[non_selected_address[b]][q];
					temp_re_ele[add_element] += 1;
					if(changed_element[add_element] == 0)
						changed_element[add_element] = 1;
				}

				for(int m = 0; m < numElement; m++)
					if(changed_element[m] == 1)
					{
						if(repeatElement[m] > 0)
							old_weight += weight[m];
						if(temp_re_ele[m] > 0)
							new_weight += weight[m];
					}
				new_total_weight = totalWeight + new_weight - old_weight;

				if(new_total_weight <= knapsackSize)
				{
					if(delta_profit > best_delta_profit)
					{
						best_delta_profit = delta_profit;
						best_new_total_weight[0] = new_total_weight;
						remove_neighboor[0] = selected_address[a];
						add_neighboor[0] = non_selected_address[b];
						num_best_neighboor = 1;
					}
					else if(delta_profit == best_delta_profit && num_best_neighboor < 500)
					{
						best_new_total_weight[num_best_neighboor] = new_total_weight;
						remove_neighboor[num_best_neighboor] = selected_address[a];
						add_neighboor[num_best_neighboor] = non_selected_address[b];
						num_best_neighboor ++;
					}
				}
			}

		//select a move and make a move
		if(best_delta_profit > 0 && num_best_neighboor > 0)
		{
			index = rand()%num_best_neighboor;
			remove_item = remove_neighboor[index];
			add_item = add_neighboor[index];

			recordSwap[add_item] ++;
			if(remove_item != numItem)
				recordSwap[remove_item] ++;

			if(remove_item != numItem)				//remove one item and add one item
			{
				chosenItem[remove_item] = 0;
				chosenItem[add_item] = 1;

				for(int p = 0; p < lengthEleAdd[remove_item]; p++)
				{
					remove_element = elementAddress[remove_item][p];
					repeatElement[remove_element] -= 1;
				}

				for(int q = 0; q < lengthEleAdd[add_item]; q++)
				{
					add_element = elementAddress[add_item][q];
					repeatElement[add_element] += 1;
				}
			}
			else									//only add one item
			{
				chosenItem[add_item] = 1;
				for(int q = 0; q < lengthEleAdd[add_item]; q++)
				{
					add_element = elementAddress[add_item][q];
					repeatElement[add_element] += 1;
				}
			}

			for(int k = 0; k < numElement; k++)
			{
				chosenElement[k] = 0;
				if(repeatElement[k] > 0)
					chosenElement[k] = 1;
			}

			totalProfit = totalProfit + best_delta_profit;
			totalWeight = best_new_total_weight[index];

			//record best solution found so far
			if(totalProfit > bestTotalProfit)
			{
				bestTime = (clock()-beginTime) / CLOCKS_PER_SEC;
				bestTotalProfit = totalProfit;
				bestTotalWeight = totalWeight;

				for(int i = 0; i < numItem; i++)
					bestChosenItem[i] = chosenItem[i];
				for(int j = 0; j < numElement; j++)
				{
					bestChosenElement[j] = chosenElement[j];
					bestRepeatElement[j] = repeatElement[j];
				}
			}

		is_improve = 1;
		}
		else 	 //Explore the neighborhood N2
		{
			if((clock()-beginTime) / CLOCKS_PER_SEC > maxRunTime)
				return;

			sampling_probability = 0.05;
			best_delta_profit = 0;
			num_best_neighboor = 0;
			for(int a = 0; a < length1; a ++)
				for(int b = a + 1; b < length1; b ++)
					for(int c = 0; c < length2; c ++)
						for(int d = c + 1; d < length2; d ++)
						{
							if(rand() % 100 / (double) 100 > sampling_probability)
								continue;

							//filtering strategy
							delta_profit = profit[non_selected_address[c]] + profit[non_selected_address[d]] - profit[selected_address[a]] - profit[selected_address[b]];
							if(delta_profit <= 0)
								continue;

							if(selected_address[b] == non_selected_address[d])
								continue;

							old_weight = new_weight = 0;
							for(int k = 0; k < numElement; k++)
							{
								temp_re_ele[k] = repeatElement[k];
								changed_element[k] = 0;
							}

							for(int p = 0; p < lengthEleAdd[selected_address[a]]; p++)
							{
								remove_element = elementAddress[selected_address[a]][p];
								temp_re_ele[remove_element] -= 1;
								changed_element[remove_element] = 1;
							}

							if(selected_address[b] != numItem)
								for(int p = 0; p < lengthEleAdd[selected_address[b]]; p++)
								{
									remove_element = elementAddress[selected_address[b]][p];
									temp_re_ele[remove_element] -= 1;
									if(changed_element[remove_element] == 0)
										changed_element[remove_element] = 1;
								}

							for(int q = 0; q < lengthEleAdd[non_selected_address[c]]; q++)
							{
								add_element = elementAddress[non_selected_address[c]][q];
								temp_re_ele[add_element] += 1;
								if(changed_element[add_element] == 0)
									changed_element[add_element] = 1;
							}

							if(non_selected_address[d] != numItem)
							for(int q = 0; q < lengthEleAdd[non_selected_address[d]]; q++)
							{
								add_element = elementAddress[non_selected_address[d]][q];
								temp_re_ele[add_element] += 1;
								if(changed_element[add_element] == 0)
									changed_element[add_element] = 1;
							}

							for(int m = 0; m < numElement; m++)
								if(changed_element[m] == 1)
								{
									if(repeatElement[m] > 0)
										old_weight += weight[m];
									if(temp_re_ele[m] > 0)
										new_weight += weight[m];
								}
							new_total_weight = totalWeight + new_weight - old_weight;

							if(new_total_weight <= knapsackSize)
							{
								if(delta_profit > best_delta_profit)
								{
									best_delta_profit = delta_profit;
									best_new_total_weight[0] = new_total_weight;
									remove_neighboor[0] = selected_address[a];
									remove_neighboor[1] = selected_address[b];
									add_neighboor[0] = non_selected_address[c];
									add_neighboor[1] = non_selected_address[d];
									num_best_neighboor = 2;
								}
								else if(delta_profit == best_delta_profit && num_best_neighboor < 500)
								{
									best_new_total_weight[num_best_neighboor] = new_total_weight;
									remove_neighboor[num_best_neighboor] = selected_address[a];
									remove_neighboor[num_best_neighboor + 1] = selected_address[b];
									add_neighboor[num_best_neighboor] = non_selected_address[c];
									add_neighboor[num_best_neighboor + 1] = non_selected_address[d];
									num_best_neighboor = num_best_neighboor + 2;
								}
							}
						}

			//select a move and make a move
			if(best_delta_profit > 0 && num_best_neighboor > 0)
			{
				index = rand()%(num_best_neighboor / 2);
				for(int x = 0; x < 2; x++)
				{
					remove_item = remove_neighboor[index * 2 + x];
					add_item = add_neighboor[index * 2 + x];

					if(add_item != numItem)
						recordSwap[add_item] ++;
					if(remove_item != numItem)
						recordSwap[remove_item] ++;

					if(remove_item != numItem && add_item != numItem)				//remove one item and add one item
					{

						chosenItem[remove_item] = 0;
						chosenItem[add_item] = 1;
						for(int p = 0; p < lengthEleAdd[remove_item]; p++)
						{
							remove_element = elementAddress[remove_item][p];
							repeatElement[remove_element] -= 1;
						}

						for(int q = 0; q < lengthEleAdd[add_item]; q++)
						{
							add_element = elementAddress[add_item][q];
							repeatElement[add_element] += 1;
						}
					}
					else if(remove_item == numItem && add_item != numItem)			//only add one item
					{
						chosenItem[add_item] = 1;

						for(int q = 0; q < lengthEleAdd[add_item]; q++)
						{
							add_element = elementAddress[add_item][q];
							repeatElement[add_element] += 1;
						}
					}
					else if(remove_item != numItem && add_item == numItem)			//only remove one item
					{
						chosenItem[remove_item] = 0;

						for(int p = 0; p < lengthEleAdd[remove_item]; p++)
						{
							remove_element = elementAddress[remove_item][p];
							repeatElement[remove_element] -= 1;
						}
					}
				}


				for(int k = 0; k < numElement; k++)
				{
					chosenElement[k] = 0;
					if(repeatElement[k] > 0)
						chosenElement[k] = 1;
				}

				totalProfit = totalProfit + best_delta_profit;
				totalWeight = best_new_total_weight[index * 2];

				//record best solution found so far
				if(totalProfit > bestTotalProfit)
				{
					bestTime = (clock()-beginTime) / CLOCKS_PER_SEC;
					bestTotalProfit = totalProfit;
					bestTotalWeight = totalWeight;

					for(int i = 0; i < numItem; i++)
						bestChosenItem[i] = chosenItem[i];
					for(int j = 0; j < numElement; j++)
					{
						bestChosenElement[j] = chosenElement[j];
						bestRepeatElement[j] = repeatElement[j];
					}
				}
				is_improve = 1;
			}
			else
				is_improve = 0;
		}
	}
}

void tabu_search(int *bestChosenItem, int *bestChosenElement, int *bestRepeatElement, double beginTime)
{
	int count;
	int non_improve;
	int delta_profit = 0;
	int max_delta_profit;
	int selected_index [numItem + 1];
	int non_selected_index [numItem + 1];
	int temp_re_ele [numElement];
	int changed_element [numElement];
	int tabu_tenure [numItem + 1];

	int old_weight, new_weight;
	int remove_item, add_item;
	int remove_element, add_element;
	int new_total_weight = 0;

	int num_neighboor;
	int remove_neighboor[500],add_neighboor[500];
	int best_new_total_weight[500];
	int rand_index;
	int length1 = 0,length2 = 0;
	int c1 = 0, c2 = 0;
	int ts_depth;

	for(int j = 0; j< numItem + 1; j++)
		tabu_tenure[j] = 0;

	for(int i = 0; i< numItem + 1; i++)
	{
		selected_index[i] = 0;
		non_selected_index[i] = 0;
	}
	for(int i = 0; i< numItem ; i++)
	{
		if(chosenItem[i] == 1)
			selected_index[i] = 1;
		else if(chosenItem[i] == 0)
			non_selected_index[i] = 1;
	}
	selected_index[numItem] = -1;			//equal to do not remove
	non_selected_index[numItem] = -1;		//equal to do not add

	for(int i = 0; i< numItem + 1 ; i++)
	{
		if(selected_index[i] == 1 || selected_index[i] == -1)
			length1 ++;
		if(non_selected_index[i] == 1 || non_selected_index[i] == -1)
			length2 ++;
	}

	count = non_improve = 0;
	ts_depth = 100;
	while(non_improve < ts_depth)
	{
		if((clock()-beginTime) / CLOCKS_PER_SEC > maxRunTime)
			return;

		num_neighboor = 0;
		max_delta_profit = MININT;
		for(int i = 0; i< numItem + 1 ; i++)
			if(non_selected_index[i] == 1|| non_selected_index[i] == -1)
			{
				if(count < tabu_tenure [i])
					continue;

				for(int j = 0; j < numItem + 1; j++)
					if(selected_index[j] == 1 || selected_index[j] == -1)
					{
						if(count < tabu_tenure [j])
							continue;

						if(i != j)
						{
							old_weight = new_weight = 0;
							for(int k = 0; k< numElement; k++)
							{
								temp_re_ele[k] = repeatElement[k];
								changed_element[k] = 0;
							}

							if(selected_index[j] == 1)
								for(int p = 0; p < lengthEleAdd[j]; p++)
								{
									remove_element = elementAddress[j][p];
									temp_re_ele[remove_element] -= 1;
									changed_element[remove_element] = 1;
								}

							if(non_selected_index[i] == 1)
								for(int q = 0; q < lengthEleAdd[i]; q++)
								{
									add_element = elementAddress[i][q];
									temp_re_ele[add_element] += 1;
									if(changed_element[add_element] == 0)
										changed_element[add_element] = 1;
								}

							for(int m = 0; m < numElement; m++)
								if(changed_element[m] == 1)
								{
									if(repeatElement[m] > 0)
										old_weight += weight[m];
									if(temp_re_ele[m] > 0)
										new_weight += weight[m];
								}
							new_total_weight = totalWeight + new_weight - old_weight;

							if(new_total_weight <= knapsackSize)
							{
								if(selected_index[j] == 1 && non_selected_index[i] == 1)		//remove one item and add one item
									delta_profit = profit[i] - profit[j];
								else if(selected_index[j] == -1 && non_selected_index[i] == 1)	//only add one item
									delta_profit = profit[i];
								else if(selected_index[j] == 1 && non_selected_index[i] == -1)  //only remove one item
									delta_profit = - profit[j];

								if(delta_profit > max_delta_profit)
								{
									max_delta_profit = delta_profit;
									best_new_total_weight[0] = new_total_weight;
									remove_neighboor[0] = j;
									add_neighboor[0] = i;
									num_neighboor = 1;
								}
								else if(delta_profit == max_delta_profit)
								{
									best_new_total_weight[num_neighboor] = new_total_weight;
									remove_neighboor[num_neighboor] = j;
									add_neighboor[num_neighboor] = i;
									num_neighboor ++;
								}
							}
						}
					}
			}

		//accept a tabu move
		if(num_neighboor > 0)
		{
			rand_index = rand() % num_neighboor;
			remove_item = remove_neighboor[rand_index];
			add_item = add_neighboor[rand_index];

			// make a move
			if(remove_item != numItem && add_item != numItem)				//remove one item and add one item
			{
				selected_index[remove_item] = 0;
				non_selected_index[remove_item] = 1;
				selected_index[add_item] = 1;
				non_selected_index[add_item] = 0;
				chosenItem[remove_item] = 0;
				chosenItem[add_item] = 1;

				for(int p = 0; p < lengthEleAdd[remove_item]; p++)
				{
					remove_element = elementAddress[remove_item][p];
					repeatElement[remove_element] -= 1;
				}

				for(int q = 0; q < lengthEleAdd[add_item]; q++)
				{
					add_element = elementAddress[add_item][q];
					repeatElement[add_element] += 1;
				}
			}
			else if(remove_item == numItem && add_item != numItem)			//only add one item
			{
				selected_index[add_item] = 1;
				non_selected_index[add_item] = 0;
				chosenItem[add_item] = 1;
				length1 ++;
				length2 --;

				for(int q = 0; q < lengthEleAdd[add_item]; q++)
				{
					add_element = elementAddress[add_item][q];
					repeatElement[add_element] += 1;
				}
			}
			else if(remove_item != numItem && add_item == numItem)			//only remove one item
			{
				selected_index[remove_item] = 0;
				non_selected_index[remove_item] = 1;
				chosenItem[remove_item] = 0;
				length1 --;
				length2 ++;

				for(int p = 0; p < lengthEleAdd[remove_item]; p++)
				{
					remove_element = elementAddress[remove_item][p];
					repeatElement[remove_element] -= 1;
				}
			}

			for(int k = 0; k < numElement; k++)
			{
				chosenElement[k] = 0;
				if(repeatElement[k] > 0)
					chosenElement[k] = 1;
			}

			totalProfit = totalProfit + max_delta_profit;
			totalWeight = best_new_total_weight[rand_index];

			//update the tabu list
			c1 = length1 * 0.4;
			c2 = length2 * 0.2 * (100 / numItem);
			if(remove_item != numItem)
				tabu_tenure[remove_item] = c1+ count;
			if(add_item != numItem)
				tabu_tenure[add_item] = c2 + count;
		}

		//record best solution found so far
		if(totalProfit > bestTotalProfit)
		{
			bestTime = (clock()-beginTime) / CLOCKS_PER_SEC;
			bestTotalProfit = totalProfit;
			bestTotalWeight = totalWeight;

			for(int i = 0; i < numItem; i++)
				bestChosenItem[i] = chosenItem[i];
			for(int j = 0; j < numElement; j++)
			{
				bestChosenElement[j] = chosenElement[j];
				bestRepeatElement[j] = repeatElement[j];
			}
			non_improve = 0;
		}
		else
			non_improve ++;

		count ++;
	}
}

void local_optima_exploration(int *bestChosenItem, int *bestChosenElement, int *bestRepeatElement, double beginTime)
{
	int count;
	int exploration_depth;
	int current_profit;

	count = 0;
	exploration_depth = 2;
	current_profit = bestTotalProfit;

	while(count < exploration_depth)
	{
		variable_neighborhood_descent_search(bestChosenItem, bestChosenElement, bestRepeatElement, beginTime);

		tabu_search(bestChosenItem, bestChosenElement, bestRepeatElement, beginTime);

		if(bestTotalProfit > current_profit)
		{
			current_profit = bestTotalProfit;
			count = 0;
		}
		else
			count ++;
	}
}

void local_optima_escaping(int *bestChosenItem, int *bestChosenElement, int *bestRepeatElement)
{
	int peturbation_length;
	int num_perturb = 0;
	int remove_index;
	int remove_item;
	int remove_element;
	int index;
	int num_selected = 0;
	double peturbation_strength;

	int temp,temp1,temp2;
	int length1 = 0;
	int length2 = 0;
	int add_index;
	int add_item = 0;
	int add_element;
	int count;
	int begin_index;

	//transmit the best solution found so far (S_b) to the current solution
	totalProfit = bestTotalProfit;
	totalWeight = bestTotalWeight;
	for(int i = 0; i < numItem; i++)
		chosenItem[i] = bestChosenItem[i];
	for(int j = 0; j < numElement; j++)
	{
		chosenElement[j] = bestChosenElement[j];
		repeatElement[j] = bestRepeatElement[j];
	}

	for(int x = 0; x < numItem; x++)
		if(chosenItem[x] == 1)
			num_selected ++;
	peturbation_strength = 0.5;
	peturbation_length = num_selected * peturbation_strength;

	for(int i = 0; i < numItem; i++)
		if(chosenItem[i] != 1)
			{
				recordSwap[i] = MAXINT;
				length2 ++;
			}

	int record_index[numItem];
	for(int i = 0; i < numItem; i++)
		record_index[i] = i;

	//sort items in ascending order according to frequency
	for(int i = 0; i < numItem; i ++)
		for(int j = i; j < numItem - 1; j ++)
			if(recordSwap[i] > recordSwap[j + 1])
			{
				temp1 = recordSwap[i];
				recordSwap[i] = recordSwap[j + 1];
				recordSwap[j + 1] = temp1;

				temp2 = record_index[i];
				record_index[i] = record_index[j + 1];
				record_index[j + 1] = temp2;
			}

	for(int i = 0; i < numItem; i ++)
		if(recordSwap[i] == 0)
			length1 ++;

	if(length1 < peturbation_length)
		length1 = peturbation_length;

	//remove the items with the lowest frequency in turn
	while(num_perturb < peturbation_length)
	{
		remove_index = rand() % length1;
		remove_item = record_index[remove_index];

		temp = record_index[remove_index];
		record_index[remove_index] = record_index[length1 - 1];
		record_index[length1 - 1] = temp;
		length1 --;

		chosenItem[remove_item] = 0;
		for(int p = 0; p < lengthEleAdd[remove_item]; p++)
		{
			remove_element = elementAddress[remove_item][p];
			repeatElement[remove_element] -= 1;
		}

		totalProfit = totalProfit - profit[remove_item];
		num_perturb ++;
	}

	totalWeight = 0;
	for(int k = 0; k < numElement; k++)
	{
		chosenElement[k] = 0;
		if(repeatElement[k] > 0)
			{
				chosenElement[k] = 1;
				totalWeight += weight[k];
			}
	}

	//generate a new solution form the non-selected items randomly
	count = index = 0;
	begin_index = numItem - length2;
	while(totalWeight <= knapsackSize)
	{
		index = rand() % length2;
		add_index = begin_index + index ;
		add_item = record_index[add_index];

		temp = record_index[add_index];
		record_index[add_index] = record_index[numItem - 1 - count];
		record_index[numItem - 1 - count] = temp;
		length2 --;
		count ++;

		//add a item
		chosenItem[add_item] = 1;
		for(int q = 0; q < lengthEleAdd[add_item]; q++)
		{
			add_element = elementAddress[add_item][q];
			repeatElement[add_element] += 1;
		}
		totalWeight = 0;
		for(int k = 0; k < numElement; k++)
		{
			chosenElement[k] = 0;
			if(repeatElement[k] > 0)
				{
					chosenElement[k] = 1;
					totalWeight += weight[k];
				}
		}

		totalProfit = totalProfit + profit[add_item];
	}

	//remove the last item to satisfy the constraint
	remove_item = add_item;
	chosenItem[remove_item] = 0;

	for(int p = 0; p < lengthEleAdd[remove_item]; p++)
	{
		remove_element = elementAddress[remove_item][p];
		repeatElement[remove_element] -= 1;
	}

	totalWeight = 0;
	for(int k = 0; k < numElement; k++)
	{
		chosenElement[k] = 0;
		if(repeatElement[k] > 0)
			{
				chosenElement[k] = 1;
				totalWeight += weight[k];
			}
	}

	totalProfit = totalProfit - profit[remove_item];
}

void check_result(int *globalChosenItem, int *globalChosenElement, int globalTotalProfit,int globalTotalWeight, double golbal_time)
{
	int testp = 0, testw = 0;

	for(int i = 0; i < numItem; i++)
		if(globalChosenItem[i] == 1)
			testp += profit[i];
	for(int j = 0; j < numElement; j++)
		if(globalChosenElement[j] == 1)
			testw += weight[j];

	if(globalTotalProfit == testp && globalTotalWeight == testw)
		{
			cout << "Result is right!" << endl;
			cout << endl;
			cout << "globalTotalProfit= " << globalTotalProfit << ", globalTotalWeight= " << globalTotalWeight;
			cout << ", golbal_time = " << golbal_time << endl;

			cout << "globalChosenItem: " << endl;
			for(int i = 0; i < numItem; i++)
				cout << globalChosenItem[i] << " ";
			cout << endl;

			cout << "globalChosenElement: " << endl;
			for(int k = 0; k < numElement; k++)
				cout << globalChosenElement[k] << " ";
			cout << endl;
		}
	else
		{
			cout << "result is wrong!" << endl;
			cout << "globalTotalProfit= " << globalTotalProfit << ", globalTotalWeight= " << globalTotalWeight << endl;
			cout << "total profit of globalChosenItem is: " << testp << ", total weight of globalChosenElement is: " << testw << endl;
		}
}

void I2PLS()
{
	double run_time = 0.0;
	double golbal_time = 0.0;
	double beginTime = clock();

	int bestChosenItem[numItem];
	int bestChosenElement[numElement];
	int bestRepeatElement[numElement];
	int globalTotalProfit = 0;
	int globalTotalWeight = 0;
	int globalChosenItem[numItem];
	int globalChosenElement[numElement];

	bestTotalProfit = 0;

	initial_solution();

	while(run_time < maxRunTime)
	{
		for(int i = 0; i < numItem; i ++)
			recordSwap[i] = 0;

		//record best solution found so far
		bestTime = (clock()-beginTime) / CLOCKS_PER_SEC;
		bestTotalProfit = totalProfit;
		bestTotalWeight = totalWeight;
		for(int i = 0; i < numItem; i++)
			bestChosenItem[i] = chosenItem[i];
		for(int j = 0; j < numElement; j++)
			bestChosenElement[j] = chosenElement[j];

		local_optima_exploration(bestChosenItem, bestChosenElement, bestRepeatElement, beginTime);

		if(bestTotalProfit > globalTotalProfit)
		{
			golbal_time = bestTime;
			globalTotalProfit = bestTotalProfit;
			globalTotalWeight = bestTotalWeight;

			for(int i = 0; i < numItem; i++)
				globalChosenItem[i] = bestChosenItem[i];
			for(int j = 0; j < numElement; j++)
				globalChosenElement[j] = bestChosenElement[j];
		}

		local_optima_escaping(bestChosenItem, bestChosenElement, bestRepeatElement);


		run_time = (clock() - beginTime) / CLOCKS_PER_SEC;
	}

	check_result(globalChosenItem, globalChosenElement, globalTotalProfit, globalTotalWeight, golbal_time);
}

int main(int argc, char ** argv)
{
	if (argc == 3)
	{
		instanceName = argv[1];
		seed = atoi(argv[2]);
	}
	else
	{
		cout << "Enter some parameters to run the program: " << endl;
		cout << "<instance file name>、<seed>" << endl;
		exit(0);
	}

	read_instance();

	maxRunTime = 500;
	srand(seed);

	I2PLS();

	delete [] profit;
	delete [] weight;
	delete [] lengthEleAdd;
	delete [] chosenItem;
	delete [] chosenElement;
	delete [] repeatElement;
	delete [] recordSwap;

	for(int x = 0; x < numItem; x++)
		delete [] relationMatrix[x];
	for(int y = 0; y < numElement; y++)
		delete [] elementAddress[y];
	delete [] relationMatrix;
	delete [] elementAddress;

	return 0;
}