/********************************************/
/*Program: Graph Reordering in ALAP Form	*
*		   and Mobility 					*
* Authors: Harshit Agarwal					*
*		   Nitish Rai						*
*		   Pritha Ganguly					*
* Version: 1.0								*
* Description: This program takes a .dot 	*
* file as input and evaluates the ASAP		*
* form of the graph mentioned in the file.	*
* Structures used: Struct, Pointers, Loops,	*
* File Handlers, Library Functions.			*/
/********************************************/
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <string.h>
#include <assert.h>
using namespace std;

//Structure for the Node to store the individual elements of the graphs.
typedef unsigned long long int LL;
typedef struct node {
	char node_name[10];
	node *next = NULL;
	node *pred = NULL;
	int node_number;
	int control_step_asap;
	int control_step_alap;
	int mobility;

}node;

//Main Function starts. All the computation of the algorithm resides here.
int main()
{
	ifstream file;	//File Handler
	file.open("test.dot",ios::in);	//Accessing the input file. It is in the .dot format. *Needs to be changed according to the input path of the file*
	node *arr[100000];
	char num1[10];
	char num2[10];
	int global_count=0;	//Global counter for the number of nodes present in the input graph.
	if(file.is_open())
	{
		string line;
		getline(file, line);
		getline(file, line);
		while(!file.eof())
		{
			getline(file, line);
			for(LL i=0; i<line.length(); i++)
			{
				if(line[i] == '=' && line[i+1] == ' ')	//Condition for checking and accessing the lines containing the node information of the graphs.
				{
					node *vertex = new node();	//Dynamically defining a new node module. Vertex points to it
					vertex->node_name[0] = line[i+2];
					vertex->node_name[1] = line[i+3];
					vertex->node_name[2] = line[i+4];
					char num[10];
					int k=0;
					for(LL j = 0; j<line.length(); j++)
					{
						if(int(line[j])>=48 && int(line[j])<=57)
						{
							num[k]=line[j];
							k++;
							if(line[j+1]==' ')
								break;
						}
					}
					vertex->node_number = atoi(num);	//Storing the node number in the node structure.
					arr[global_count] = vertex;	//Adding the node pointer to an array, for access later while defining links.
					global_count++;
				}
				if(line[i] == '-' && line[i+1] == '>')	//Condition for accessing the lines where links are defined.
				{
					int k=0,l=0;
					for(LL j = 0; j<line.length(); j++)
					{
						if(int(line[j])>=48 && int(line[j])<=57)
						{
							num1[k]=line[j];
							k++;
							if(line[j+1] == ' ')
							{
								l=j+1;
								break;
							}
						}
					}
					int x=0;
					for(int y = l; y<line.length(); y++)
					{
						if(int(line[y])>=48 && int(line[y])<=57)
						{
							num2[x]=line[y];
							x++;
							if(line[y+1] == ' ')
								break;
						}
					}
					node *vertex1, *vertex2;	//Declaring two node pointers to map one node to another.
					for(LL j = 0; j < global_count; j++)
					{
						if(arr[j]->node_number == atoi(num1))
							vertex1 = arr[j];
						if(arr[j]->node_number == atoi(num2))
							vertex2 = arr[j];
					}
					vertex1->next = vertex2;	//Defining the next relationship from one node to the other.
					vertex2->pred = vertex1;	//Defining the previous relationship in the reverse order.
					for(int j=0; j<10; j++)
						num1[j]=num2[j]='\0';
				}
			}
		}
	}
	file.close();	//Colsing the openend file.
	int max=1;

	//ASAP Scheduling
	for(LL i = 0; i < global_count; i++)	//Scheduling the nodes according to ASAP notation.
	{
		if(arr[i]->pred == NULL)
		{
			arr[i]->control_step_asap = 1;	//All the nodes with no predecessor are assigned in the First control step.
			arr[i]->mobility = arr[i]->control_step_asap;	//Assigning the control step value to mobility
		}			
		else
		{
			if(arr[i]->control_step_asap < (arr[i]->pred->control_step_asap + 1))	//ASAP Scheduling for the nodes which are not present in Control Step 1
			{
				arr[i]->control_step_asap = arr[i]->pred->control_step_asap + 1;
				arr[i]->mobility = arr[i]->control_step_asap;
				if(max< (arr[i]->control_step_asap))
					max = arr[i]->control_step_asap;
			}	
		}
	}
	//ALAP Scheduling
	int max1=max;
	for(LL i = 0; i < global_count; i++)	//Scheduling the nodes in the last Control Step
	{
		if(arr[i]->next == NULL)
			arr[i]->control_step_alap = max;
	}
	for(LL i = 0; i < max; i++)
	{
		for(LL j = 0; j < global_count; j++)
		{
			if(arr[j]->next != NULL)
				if(arr[j]->next->control_step_alap == max1)
					arr[j]->control_step_alap = max1-1;
		}
		max1--;
	}
	/*for(LL i = 0; i < max; i++)	//Scheduling the nodes in the Control Step other than the last.
	{
		max1--;
		for(LL j = 0;  j < global_count; j++)
		{
			if(arr[j]->control_step == max1)
			{
				if((arr[j]->control_step + 1) != arr[j]->next->control_step)
					arr[j]->control_step = (arr[j]->next->control_step) - 1;
			}
		}*/	/*if(arr[i]->next != NULL)
			{
				if(arr[i]->next->control_step != (arr[i]->control_step + 1))
					arr[i]->control_step = (arr[i]->next->control_step) -1;
			}*/
	//}
	//	for(LL j = 0; j < global_count; j++)
	//	{
			
				//if(arr[j]->next->control_step == max1)
				//	arr[j]->control_step = (arr[j]->next->control_step) - 1;
	//		}
	//	}
	//	max1--;
	//}
	for(LL i = 0; i < global_count; i++)
	{	//Calculating the Mobility Value by Subtracting the previously computed ASAP Control Step Value from current ALAP Control Step value.
		arr[i]->mobility = arr[i]->control_step_alap - arr[i]->control_step_asap;
		cout<<arr[i]->node_number<<" "<<arr[i]->control_step_asap<<" "<<arr[i]->control_step_alap<<" "<<arr[i]->mobility<<endl;
	}


	//PRINT
	/*
	int count=0;
	for(LL i = 1; i < global_count; i++)	//Printing the Scheduled graph.
	{
		cout<<"For Control Step: "<<i<<endl;
		for(LL j = 0; j< global_count; j++)
		{
			if(arr[j]->control_step == i)
			{
				if(arr[j]->next != NULL)
				{
					cout<<arr[j]->node_number<<" "<<arr[j]->node_name<<" <"<<arr[j]->mobility<<"> "<<" -> "<<arr[j]->next->node_number<<" "<<arr[j]->next->node_name<<endl;
					count++;
				}
				else
				{
					cout<<arr[j]->node_number<<" "<<arr[j]->node_name<<" <"<<arr[j]->mobility<<"> "<<endl;
					count++;
				}
			}
		}
		cout<<endl;
		cout<<count<<endl;
		if(count == global_count)
			break;
	}*/
	//for(LL i = 0; i < global_count; i++)
	//	cout<<arr[i]->node_number<<" "<<arr[i]->mobility<<endl;
	return 0;
}