/*****************************************************************************************\
* Name        : KBTS.cpp																 *
* Author      : Zequn Wei (zequn.wei@gmail.com) Jin-Kao Hao (jin-kao.hao@univ-angers.fr) *
* Organization: LERIA, Université d’Angers, 2 Boulevard Lavoisier, 49045 Angers, France. *
* Revised	  : January 2020															 *
* Copyright   : This code can be freely used only for academic purpose.					 *
* Description : Kernel based Tabu Search for the Set-Union Knapsack Problem.			 *
* Publication : Expert Systems With Applications.										 *
\*****************************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <sstream>
using namespace std;

#define MININT -2147483648

char *instanceName;
int seed;
char *rep;
char nameFinalResult[256];
double maxRunTime;
double beginTime;
double bestTime;

int numItem = 0;
int numElement = 0;
int knapsackSize = 0;
int *profit;            //profit of each item
int *weight;            //weight of each element
int **relationMatrix;

int **elementAddress;  //the elements correspond to each item
int *lengthEleAdd;     //number of elements correspond to each item
int *chosenItem;       //the selected items
int *repeatElement;       //number of each elements of the selected items
int *frequencyItem;			  //record frequency of items in the solution

int totalProfit;       //record the total profit of current solution
int totalWeight;       //record the total weight of current solution
int bestTotalProfit;
int bestTotalWeight;
int *bestChosenItem;
int *bestRepeatElement;

int *kernel;
int *non_kernel;
int globalTotalProfit = 0;

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
    cout << "read instance :" << "numItem= " << numItem;
    cout << ", numElement= " << numElement;
    cout << ", knapsackSize= " << knapsackSize << endl;

    lengthEleAdd = new int[numItem];
    chosenItem = new int[numItem];
    bestChosenItem = new int[numItem];
    bestRepeatElement = new int[numElement];
    repeatElement = new int[numElement];
    frequencyItem = new int[numItem];
	kernel = new int[numItem];
	non_kernel = new int[numItem];

	for(int x = 0; x < numItem; x++)
	{
		kernel[x] = -1;
		non_kernel[x] = -1;
	}

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

void check_result(int *chosenItem, int p, int w)
{
    int testp = 0, testw = 0;
    int test_re_ele[numElement];

    for(int i = 0; i < numElement; i++)
        test_re_ele[i] = 0;
    for(int i = 0; i < numItem; i++)
        if(chosenItem[i] == 1)
            for(int j = 0; j < lengthEleAdd[i]; j++)
                test_re_ele[elementAddress[i][j]] += 1;

    for(int i = 0; i < numItem; i++)
        if(chosenItem[i] == 1)
            testp += profit[i];
    for(int j = 0; j < numElement; j++)
        if(test_re_ele[j] > 0)
            testw += weight[j];

    if(testp == p && testw == w)
        cout << "result is right! " << endl;
    else
    {
        cout << "result is wrong!!!" << endl;
        exit(-1);
    }
}

void qsort_int(int *s, int *address, int l, int r)
{
	if (l < r)
    {
       int  i = l, j = r;
       int x = s[l];
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

        qsort_int(s, address, l, i - 1);
        qsort_int(s, address, i + 1, r);
    }
}

void dynamic_initializaton()
{
	double pr[numItem];
    int index;
    int sum_weight;
    double max_pr;
    int max_item;
    int temp_weight;

	for(int i = 0; i < numItem; i++)
	{
		lengthEleAdd[i] = 0;
		pr[i] = 0.0;
		chosenItem[i] = 0;
	}
	for(int i = 0; i < numElement; i++)
		repeatElement[i] = 0;

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

    temp_weight = 0; totalProfit = 0; totalWeight = 0;
	while(temp_weight <= knapsackSize)
	{
		max_pr = 0.0;
		max_item = -1;
		temp_weight = 0;

		for(int x = 0; x < numItem; x++)
			if(chosenItem[x] != 1)
			{
				sum_weight = 0;
				for(int y = 0; y < lengthEleAdd[x]; y++)
					if(repeatElement[elementAddress[x][y]] == 0)
					{
						sum_weight += weight[elementAddress[x][y]];
					}

				pr[x] = (double) profit[x] / sum_weight;
			}

		for(int x = 0; x < numItem; x++)
			if(pr[x] > max_pr)
			{
				max_pr = pr[x];
				max_item = x;
			}

		pr[max_item] = 0;

		for(int j = 0; j < lengthEleAdd[max_item]; j++)
			repeatElement[elementAddress[max_item][j]] += 1;

        for(int k = 0; k< numElement; k++)
        {
            if(repeatElement[k] > 0)
                temp_weight += weight[k];
        }

		if(temp_weight <= knapsackSize)
		{
			chosenItem[max_item] = 1;
			totalProfit += profit[max_item];
			totalWeight = temp_weight;
		}
        else
        {
            for(int j = 0; j < lengthEleAdd[max_item]; j++)
                repeatElement[elementAddress[max_item][j]] -= 1;
            break;
        }
	}
}

void update_best_so_far(int *item, int *element, int p, int w)
{
	if(bestTotalProfit < p)
		bestTime = (clock()-beginTime) / CLOCKS_PER_SEC;

	bestTotalProfit = p;
    bestTotalWeight = w;

    for(int i = 0; i < numItem; i++)
        bestChosenItem[i] = item[i];
    for(int j = 0; j < numElement; j++)
    	bestRepeatElement[j] = element[j];
}

void cal_index(int *sel, int *non_sel,int &count1, int &count2)
{
	for(int y = 0; y < numItem; y++)
    {
        if(chosenItem[y] == 1)
        {
            sel[count1] = y;
            count1 ++;
        }
        else if(chosenItem[y] == 0)
        {
            non_sel[count2] = y;
            count2 ++;
        }
    }

    sel[count1] = numItem;
    non_sel[count2] = numItem;
    count1 ++;
    count2 ++;
}

void cal_index_from_nonker(int *sel, int *non_sel,int &count1, int &count2, int length_nonker)
{
	 for(int y = 0; y < length_nonker; y++)
	 {
		 if(chosenItem[non_kernel[y]] == 1)
        {
            sel[count1] = non_kernel[y];
            count1 ++;
        }
        else if(chosenItem[non_kernel[y]] == 0)
        {
            non_sel[count2] = non_kernel[y];
            count2 ++;
        }
	 }

    sel[count1] = numItem;
    non_sel[count2] = numItem;
    count1 ++;
    count2 ++;
}

void cal_new_total_weight(int &sel_i, int &non_sel_j, int &new_total_weight)
{
	int old_weight = 0, new_weight = 0;
    int changed_element[numElement];
    int temp_re_ele[numElement];
    int remove, add;
	for(int k = 0; k < numElement; k++)
	{
	  changed_element[k] = 0;
	  temp_re_ele[k] = repeatElement[k];
	}

	if(sel_i != numItem)
	  for(int p = 0; p < lengthEleAdd[sel_i]; p++)
	  {
		  remove = elementAddress[sel_i][p];
		  temp_re_ele[remove] -= 1;
		  changed_element[remove] = 1;
	  }

	if(non_sel_j != numItem)
	  for(int q = 0; q < lengthEleAdd[non_sel_j]; q++)
	  {
		  add = elementAddress[non_sel_j][q];
		  temp_re_ele[add] += 1;
		  if(changed_element[add] == 0)
			  changed_element[add] = 1;
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
}

void tabu_move(int &remove_i, int &add_j, int *sel, int *non_sel, int & count1, int &count2)
{
	int remove_element, add_element;
    int remove_item = sel[remove_i];
    int add_item = non_sel[add_j];

	// make a move
	if(remove_item != numItem && add_item != numItem)                //remove one item and add one item
	{
		sel[remove_i] = add_item;
		non_sel[add_j] = remove_item;
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
	else if(remove_item == numItem && add_item != numItem)            //only add one item
	{
		sel[count1] = add_item;
		non_sel[add_j] = non_sel[count2 - 1];
		non_sel[count2 - 1] = -1;
		count1 ++;
		count2 --;
		chosenItem[add_item] = 1;

		for(int q = 0; q < lengthEleAdd[add_item]; q++)
		{
			add_element = elementAddress[add_item][q];
			repeatElement[add_element] += 1;
		}
	}
	else if(remove_item != numItem && add_item == numItem)            //only remove one item
	{
		sel[remove_i] = sel[count1 - 1];
		sel[count1 - 1] = -1;
		non_sel[count2] = remove_item;
		count1 --;
		count2 ++;
		chosenItem[remove_item] = 0;

		for(int p = 0; p < lengthEleAdd[remove_item]; p++)
		{
			remove_element = elementAddress[remove_item][p];
			repeatElement[remove_element] -= 1;
		}
	}
}

void get_ker_and_nker()
{
	int l_fre = 0;
	int l_ker = 0;
	int l_nker = 0;

	int temp_w = 0;
	int temp_s[numItem];
	int temp_re_ele[numElement];
	double kernel_coefficient = 0.6;

	for(int x = 0; x < numItem; x++)
	{
		if(frequencyItem[x] != 0)
			l_fre += 1;
	}

	if(l_fre != 0)
	{
		int l_best = 0;
		int des_num[numItem];
		int temp_num[numItem];

		for(int x = 0; x < numItem; x++)
		{
			kernel[x] = -1;
			non_kernel[x] = -1;
			des_num[x] = x;
			temp_num[x] = frequencyItem[x];

			if(bestChosenItem[x] == 1)
				l_best += 1;
		}

		qsort_int(temp_num, des_num, 0, numItem -1);

		l_ker = kernel_coefficient * l_best;

		for(int j = 0; j < l_ker; j++)
		{
			kernel[j] = des_num[j];
		}

		for(int j = l_ker; j < numItem; j++)
		{
			non_kernel[l_nker] = des_num[j];
			l_nker ++;
		}
	}

	//check S_k is a feasible solution
	for(int i = 0; i < numItem; i++)
	{
		temp_s[i] = 0;
	}
	for(int j = 0; j < numElement; j++)
	{
		temp_re_ele[j] = 0;
	}

	for(int j = 0; j < l_ker; j++)
	{
		temp_s[kernel[j]] = 1;

		for(int k = 0; k < lengthEleAdd[kernel[j]]; k++)
			temp_re_ele[elementAddress[kernel[j]][k]] += 1;
	}

	 for(int j = 0; j < numElement; j++)
		if(temp_re_ele[j] > 0)
			temp_w += weight[j];

	while(temp_w > knapsackSize)
	{
		int remove_item;
		remove_item = kernel[l_ker - 1];

		 if(temp_s[remove_item] != 1)
			 cout << "wrong in get kernel" << endl;
		 else
			 temp_s[remove_item] = 0;

		 for(int j = 0; j < lengthEleAdd[remove_item]; j++)
			 temp_re_ele[elementAddress[remove_item][j]] -= 1;

		 temp_w = 0;
		 for(int j = 0; j < numElement; j++)
			if(temp_re_ele[j] > 0)
				temp_w += weight[j];

		non_kernel[l_nker] = kernel[l_ker - 1];
		kernel[l_ker - 1] = -1;
		l_nker ++;
		l_ker --;
	}
}

void tabu_search(int mode)
{
    int selected_index[numItem + 1], non_selected_index[numItem + 1];
    int count = 0, count1 = 0, count2 = 0;
    int non_improve = 0;

    int tabu_tenure[numItem + 1];
    int remove_item, add_item;
    int new_total_weight = 0;
    int delta_profit;
    int max_delta_profit;

    int temp_p;
    int best_new_total_weight[500];
    int remove_neighboor[500],add_neighboor[500];
    int num_neighboor;
    int rand_index;
    int remove_i,add_j;
    int t1 = 0,t2 = 0;

    int fix_item[numItem];
    int move_frequency[numItem + 1];

    for(int x = 0; x < numItem + 1; x++)
    {
        selected_index[x] = -1;
        non_selected_index[x] = -1;
        tabu_tenure[x] = 0;
        move_frequency[x]=0;
    }

    if(mode == 1)						// tabu for kernel search
    	for(int y = 0; y< numItem; y++)
    		fix_item[y] = chosenItem[y];

    if(mode == 0 || mode == 1)
    	cal_index(selected_index, non_selected_index, count1, count2);   //normal tabu
    else
    	cal_index_from_nonker(selected_index, non_selected_index, count1, count2, mode);//tabu for non_kernel search (limited region)

    while(non_improve != -1)
    {
		if((clock()-beginTime) / CLOCKS_PER_SEC > maxRunTime)
			return;

		num_neighboor = 0;
		temp_p = MININT; max_delta_profit = MININT;
        for(int i = 0; i < count1; i ++)						//selected_index[i] -- sel item
            for(int j = 0; j < count2; j ++)					//non_selected_index[j] -- UNsel item
            {
                delta_profit = profit[non_selected_index[j]] - profit[selected_index[i]];

                if(delta_profit < temp_p)
					continue;

                if(mode == 1 && selected_index[i] != numItem && fix_item[selected_index[i]] == 1)
                	continue;

                if(delta_profit + totalProfit <= bestTotalProfit)		//aspiration criteria
            	{
            		if(count < tabu_tenure [selected_index[i]])
						continue;
                	if(count < tabu_tenure [non_selected_index[j]])
                        continue;
            	}

                if(selected_index[i] == numItem && non_selected_index[j] == numItem)
                    continue;

                cal_new_total_weight(selected_index[i], non_selected_index[j], new_total_weight);

                if(new_total_weight <= knapsackSize)
                {
                	if(delta_profit >= temp_p)
                		temp_p = delta_profit;

                    if(delta_profit > max_delta_profit)
                    {
                        max_delta_profit = delta_profit;
                        best_new_total_weight[0] = new_total_weight;
                        remove_neighboor[0] = i;
                        add_neighboor[0] = j;
                        num_neighboor = 1;
                    }
                    else if(delta_profit == max_delta_profit)
                    {
                        best_new_total_weight[num_neighboor] = new_total_weight;
                        remove_neighboor[num_neighboor] = i;
                        add_neighboor[num_neighboor] = j;
                        num_neighboor ++;
                    }
                }
            }

        //accept a tabu move
        remove_i = add_j = 0;
        if(num_neighboor > 0)
        {
        	rand_index = rand() % num_neighboor;
            remove_i = remove_neighboor[rand_index];
            add_j = add_neighboor[rand_index];
            remove_item = selected_index[remove_i];
            add_item = non_selected_index[add_j];

            totalProfit = totalProfit + max_delta_profit;
            totalWeight = best_new_total_weight[rand_index];

        	tabu_move(remove_i, add_j, selected_index, non_selected_index, count1, count2);

            //update the tabu list
            move_frequency[remove_item] ++;
            move_frequency[add_item] ++;

            t1 = move_frequency[remove_item];
            t2 = move_frequency[add_item];

            if(remove_item != numItem)
                tabu_tenure[remove_item] = t1+ count;
            if(add_item != numItem)
                tabu_tenure[add_item] = t2 + count;
        }
        else
        	non_improve = -1;

        if(totalProfit > bestTotalProfit)
        {
			update_best_so_far(chosenItem,repeatElement,totalProfit,totalWeight);

			if(mode == 0)
				for(int i = 0; i < numItem; i++)
					if(bestChosenItem[i] == 1)
						frequencyItem[i] += 1;
        }
        count ++;
    }

    if(mode == 0)
    	get_ker_and_nker();
}

void kernel_search()
{
	int count1 = 0;

    for(int j = 0; j < numItem; j++)
	{
    	if(kernel[j] != -1)
    		count1 ++;
	}

	//generate a new solution by kernel
    for(int i = 0; i < numItem; i++)
		chosenItem[i] = 0;
	for(int j = 0; j < numElement; j++)
		repeatElement[j] = 0;
	totalProfit = 0;
	totalWeight = 0;

	for(int j = 0; j < count1; j++)
	{
		chosenItem[kernel[j]] = 1;
		totalProfit += profit[kernel[j]];

		for(int k = 0; k < lengthEleAdd[kernel[j]]; k++)
			repeatElement[elementAddress[kernel[j]][k]] += 1;
	}

	 for(int j = 0; j < numElement; j++)
		if(repeatElement[j] > 0)
			totalWeight += weight[j];

	tabu_search(1);
}

void non_kernel_search()
{
	int temp_weight;
	int add_index, add_item;
	int temp;
	int length_nonker;
	int count2 = 0;

	for(int i = 0; i < numItem; i++)
		if(non_kernel[i] != -1)
			count2 ++;
	length_nonker = count2;

	//generate a new solution by non_kernel
	 for(int i = 0; i < numItem; i++)
		 chosenItem[i] = 0;
	 for(int j = 0; j < numElement; j++)
		repeatElement[j] = 0;
	totalProfit = 0;
	totalWeight = 0;

	for (int i = 0; i < count2; i++)
	{
		temp_weight = 0;
		add_index = rand() % count2;
		add_item = non_kernel[add_index];

		temp = non_kernel[add_index];
		non_kernel[add_index] = non_kernel[count2 - 1];
		non_kernel[count2 - 1] = temp;
		count2 --;

		for(int j = 0; j < lengthEleAdd[add_item]; j++)
			repeatElement[elementAddress[add_item][j]] += 1;

       for(int k = 0; k< numElement; k++)
           if(repeatElement[k] > 0)
               temp_weight += weight[k];

       if(temp_weight <= knapsackSize)
       {
           chosenItem[add_item] = 1;
           totalProfit += profit[add_item];
           totalWeight = temp_weight;
       }
       else
       {
           for(int j = 0; j < lengthEleAdd[add_item]; j++)
               repeatElement[elementAddress[add_item][j]] -= 1;
           break;
       }
	}

	for(int i = 0; i < numItem; i++)
		bestChosenItem[i] =  chosenItem[i];
	for(int j = 0; j < numElement; j++)
		bestRepeatElement[j] = repeatElement[j];
	bestTotalProfit = totalProfit;
	bestTotalWeight = totalWeight;

	tabu_search(length_nonker);

	for(int i = 0; i < numItem; i++)
		chosenItem[i] = bestChosenItem[i];
	for(int j = 0; j < numElement; j++)
		repeatElement[j] = bestRepeatElement[j];
	totalProfit = bestTotalProfit;
	totalWeight = bestTotalWeight;
}

void direct_perturbation()
{
    int selected_index[numItem + 1], non_selected_index[numItem + 1];
    int count1 = 0, count2 = 0;
	int perturbation_strength;
    int num_swap;
    int remove_i = -1, add_j = -1;
    int new_total_weight = 0;
    int temp;
    int i,j;

    for(int x = 0; x < numItem + 1; x++)
    {
        selected_index[x] = -1;
        non_selected_index[x] = -1;
    }

    totalProfit = bestTotalProfit;
	totalWeight = bestTotalWeight;
	for(int i = 0; i < numItem; i++)
		chosenItem[i] = bestChosenItem[i];
    for(int j = 0; j < numElement; j++)
	{
    	repeatElement[j] = bestRepeatElement[j];
	}

	cal_index(selected_index, non_selected_index, count1, count2);

	num_swap = 0;
	perturbation_strength = 3;
	while(num_swap < perturbation_strength)
	{
		temp = 0;
		while(temp != 1)
		{
			if((clock()-beginTime) / CLOCKS_PER_SEC > maxRunTime)
			return;

			i = rand() % count1;
			j = rand() % count2;

			if(selected_index[i] == numItem && non_selected_index[j] == numItem)
				continue;

			cal_new_total_weight(selected_index[i], non_selected_index[j], new_total_weight);

			if(new_total_weight <= knapsackSize)
			{
				remove_i = i;
				add_j = j;
				temp = 1;
			}
			else
				temp = 0;
		}

		totalProfit = totalProfit + profit[non_selected_index[add_j]] - profit[selected_index[remove_i]];
		totalWeight = new_total_weight;

		tabu_move(remove_i, add_j, selected_index, non_selected_index, count1, count2);

		num_swap ++;
	}
}

void update_global_best(int *item, int p, int w, double t)
{
	double global_time = 0.0;
	int globalTotalWeight = 0;
	int globalChosenItem[numItem];

    global_time = t;
	globalTotalProfit = p;
	globalTotalWeight = w;

	for(int i = 0; i < numItem; i++)
		globalChosenItem[i] = item[i];

	cout << endl;
	check_result(globalChosenItem, globalTotalProfit, globalTotalWeight);

	cout << "globalTotalProfit = " << globalTotalProfit << ", globalTotalWeight = " << globalTotalWeight;
	cout << ", global_time = " << global_time << endl;

	cout <<"The global best solution is : " << endl;
	for(int j = 0; j < numItem; j++)
		cout << globalChosenItem[j] << " ";
	cout << endl;
}

void KBTS()
{
    beginTime = clock();
	double run_time = 0.0;
	int iter = 1;
	int count;
	int temp_profit;
	int initial_time;
	int ls_depth = 3;

	dynamic_initializaton();

	initial_time = (clock()-beginTime) / CLOCKS_PER_SEC;
	update_global_best(chosenItem, totalProfit, totalWeight, initial_time);

	bestTime = 0.0;
    while(run_time < maxRunTime)
    {
    	for(int i = 0; i < numItem; i++)
			frequencyItem[i] = 0;

		update_best_so_far(chosenItem,repeatElement,totalProfit,totalWeight);

    	count = 0;
    	temp_profit = bestTotalProfit;
    	while(count < ls_depth)
    	{
    		if((clock()-beginTime) / CLOCKS_PER_SEC > maxRunTime)
    			return;

    		tabu_search(0);

    		kernel_search();

    		direct_perturbation();

    		if(bestTotalProfit > temp_profit)
    		{
    			temp_profit = bestTotalProfit;
    			count = 0;
    		}
    		else
    			count ++;
    	}

    	if(bestTotalProfit > globalTotalProfit)
		{
    		update_global_best(bestChosenItem, bestTotalProfit, bestTotalWeight, bestTime);
		}

     	non_kernel_search();

    	iter ++;
    	run_time = (clock() - beginTime) / CLOCKS_PER_SEC;
    }
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

	srand(seed);
    read_instance();

    if(numItem <= 500)
    	maxRunTime = 500;
    else
        maxRunTime = 1000;

    KBTS();

    delete [] profit;
    delete [] weight;
    delete [] lengthEleAdd;
    delete [] chosenItem;
    delete [] repeatElement;
    delete [] frequencyItem;
    delete [] bestChosenItem;
    delete [] kernel;
    delete [] non_kernel;
    delete [] bestRepeatElement;

    for(int x = 0; x < numItem; x++)
    {
    	delete [] relationMatrix[x];
        delete [] elementAddress[x];
    }
    delete [] relationMatrix;
    delete [] elementAddress;

    return 0;
}
