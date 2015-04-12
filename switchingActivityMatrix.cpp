/********************************************/
/*Program: Graph Reordering in ALAP Form	*
*		   and Mobility 					*
* Authors: Harshit Agarwal					*						*
* Version: 3.0								*
* Description: This program takes a .dot 	*
* file as input and evaluates the ASAP		*
* form of the graph mentioned in the file.  *
* Also takes care of the Switching Activity *
* Matrix formation. And the path scheduling *
* hence.									*
* The program now 							*
* Structures used: Struct, Pointers, Loops,	*
* File Handlers, Library Functions.			*/
/********************************************/
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>
using namespace std;

#define MULTIPLIER 2
#define ADDER 1
#define SUBTRACTOR 1
#define COMPARATOR 1

//Structure for the Node to store the individual elements of the graphs.
typedef double DD;
typedef unsigned long long int LL;
typedef struct node {
	char node_name[10];
	node *next[100] = {NULL};
	node *pred[100] = {NULL};
	int node_number;
	LL next_value;
	DD path_value;
	int control_step_asap;
	int control_step_alap;
	int mobility;
	DD weight;	//output of the node.
	LL clique;

}node;

//Random Generator Function
LL randomgen()
{
	LL num;
	num=rand() % 1000000 + 1;
	//cout<<num<<endl;
	return num;
}

//Converting Numbers to Binary
LL binaryConv(LL num)
{
	LL dec,rem,i=1,sum=0;
    do
    {
        rem=num%2;
        sum=sum + (i*rem);
        num=num/2;
        i=i*10;
    }while(num>0);
    return sum;
}

//Number of Transitions in an input vector.
LL flipCounter(LL num)
{
	LL flip=0;
	LL a,b;
	while(num!=0)
	{
		a = num%10;
		num = num/10;
		b = num%10;
		if(a != b)
			flip++;
	}
	return flip;
}


//Hamming Distance between two input vectors.
LL hammingDist(LL num1, LL num2)
{
	LL hamCount=0;
	LL a,b;
	while(num1 !=0 && num2 !=0)
	{
		a=num1%10;
		b=num2%10;
		if(a != b)
			hamCount++;
		num1 = num1/10;
		num2 = num2/10;
	}
	return hamCount;
}

//Total number of transitions possible in the generated input vector.
LL totalflip(LL num)
{
	LL totalCountOfFlip=0;
	while(num!=0){
		num=num/10;
		totalCountOfFlip++;}
	return totalCountOfFlip;
}

//Main Function starts. All the computation of the algorithm resides here.
int main()
{
	//Seed for the random number
	srand(time(NULL));

	//binaryConv(randomgen());
	ifstream file;	//File Handler
	file.open("Benchmarks/hal.dot",ios::in);	//Accessing the input file. It is in the .dot format. *Needs to be changed according to the input path of the file*
	node *arr[100000];	//node pointer file for all the nodes.
	char num1[10];
	char num2[10];
	LL global_count=0;	//Global counter for the number of nodes present in the input graph.
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
				if(line[i] == '=' && line[i+1] == ' ' && ((line[i+2]>=65 && line[i+2]<=90) || (line[i+2]>=97 && line[i+2]<=122)))	//Condition for checking and accessing the lines containing the node information of the graphs.
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
					LL xcount=0, ycount=0;
					for(LL l = 0; l<100; l++)
					{
						if(vertex1->next[l] != NULL)
							xcount++;
						if(vertex2->pred[l] != NULL)
							ycount++;
					}
					vertex1->next[xcount] = vertex2;	//Defining the next relationship from one node to the other.
					vertex2->pred[ycount] = vertex1;	//Defining the previous relationship in the reverse order.
					for(int j=0; j<10; j++)
						num1[j]=num2[j]='\0';
				}
			}
		}
	}
	file.close();	//Closing the openend file.
	int max=1;
	cout<<"Graph made"<<endl;

	//ASAP Scheduling
	for(LL i = 0; i < global_count; i++)	//Scheduling the nodes according to ASAP notation.
	{
		int flag = 0;
		for(LL j=0;j<100;j++)
			if(arr[i]->pred[j] != NULL)
				flag=1;
		if(flag == 0)
		{
			arr[i]->control_step_asap = 1;	//All the nodes with no predecessor are assigned in the First control step.
			arr[i]->mobility = arr[i]->control_step_asap;	//Assigning the control step value to mobility			
		}
		else
			arr[i]->control_step_asap = 99999;
	}

	for(LL i = 0; i < global_count; i++) //ASAP Scheduling for the nodes which are not present in Control Step 1
	{	int flag=1;
		for(LL j = 0; j < global_count; j++)
		{
			
			if(arr[j]->control_step_asap == max)
			{
				flag=0;
				for(LL k=0;k<100;k++)
					if(arr[j]->next[k] != NULL)
						arr[j]->next[k]->control_step_asap = max + 1;
			}
		}
		if(flag==0)
			max++;
	}
	max--;
	// for(LL i = 0; i < global_count; i++)
	// 	cout<<arr[i]->node_number<<" "<<arr[i]->node_name<<" "<<arr[i]->control_step_asap<<endl;
	cout<<"ASAP Done"<<endl;
	cout<<"max "<<max<<endl;
	
	//ALAP Scheduling
	int max1=max;
	for(LL i = 0; i < global_count; i++)	//Scheduling the nodes in the last Control Step
	{
		int flag=0;
		for(LL j=0; j<100; j++)
			if(arr[i]->next[j] != NULL)
				flag=1;
		if(flag==0)
			arr[i]->control_step_alap = max;
	}
	for(LL i = 0; i < max; i++)
	{
		for(LL j = 0; j < global_count; j++)
		{
			for(LL k=0;k<100;k++)
			{
				if(arr[j]->next[k] != NULL)
				{
					if(arr[j]->next[k]->control_step_alap == max1)
						arr[j]->control_step_alap = max1-1;
					break;
				}
			}
		}
		max1--;
	}

	// for(LL i = 0; i < global_count; i++)
	// 	cout<<arr[i]->node_number<<" "<<arr[i]->node_name<<" "<<arr[i]->control_step_alap<<endl;
	
	for(LL i = 0; i < global_count; i++)
	{	//Calculating the Mobility Value by Subtracting the previously computed ASAP Control Step Value from current ALAP Control Step value.
		arr[i]->mobility = arr[i]->control_step_alap - arr[i]->control_step_asap;
	}
	cout<<"ALAP Done"<<endl;

	//LIST BASED SCHEDULING
	/*int mul=MULTIPLIER;
	int add=ADDER;
	int sub=SUBTRACTOR;
	int comp=COMPARATOR;
	node *list[100000];
	for(LL i = 0; i < global_count; i++)
	{
		for(LL j = i; j < global_count; j++)
		{
			if(arr[i]->control_step_asap > arr[j]->control_step_asap)
			{
				node *temp;
				temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
		}
	}
	
	for(LL j = 1; j <= max; j++)
	{
		for(LL x = 1; x <= max; x++)
		{
			for(LL y = 0; y < global_count; y++)
			{
				if(arr[y]->control_step_asap == x)
				{
					for(LL k = y; k < global_count; k++)
					{
						if((arr[k]->control_step_asap == x) && (arr[k]->mobility < arr[y]->mobility))
						{
							node *temp;
							temp = arr[k];
							arr[k] = arr[y];
							arr[y] = temp;
						}
					}
				}
			}
		}
		for(LL i = 0; i < global_count; i++)
		{
			if(arr[i]->control_step_asap == j)
			{
				if(arr[i]->next!=NULL && arr[i]->next->control_step_asap == arr[i]->control_step_asap)
					arr[i]->next->control_step_asap++;
				if(arr[i]->node_name[0] == 'm' && arr[i]->node_name[1] == 'u')
				{
					if(mul != 0)
						mul--;
					else
					{
						arr[i]->control_step_asap++;
					}
				}
				if(arr[i]->node_name[0] == 'm' && arr[i]->node_name[1] == 'i')
				{
					if(sub != 0)
						sub--;
					else
					{
						arr[i]->control_step_asap++;
					}
				}
				if(arr[i]->node_name[0] == 'a')
				{
					if(add != 0)
						add--;
					else
					{
						arr[i]->control_step_asap++;
					}
				}
				if(arr[i]->node_name[0] == 'l')
				{
					if(comp != 0)
						comp--;
					else
					{
						arr[i]->control_step_asap++;
					}
				}
			}
		}
		mul=MULTIPLIER;
		add=ADDER;
		sub=SUBTRACTOR;
		comp=COMPARATOR;
	}
	for(LL i =0 ;i<global_count;i++)
	{}	//cout<<arr[i]->node_number<<" "<<arr[i]->control_step_asap<<" "<<arr[i]->mobility<<endl;
	//cout<<global_count;
	//for(int i =0 ; i<global_count; i++)
	//	cout<<arr[i]->node_number<<endl;//" "<<arr[i]->pred<<endl;
*/


	//SWITCHING ACTIVITY MATRIX FORMATION
	LL total=0;
	LL inputarray[global_count*2][2];
	int index=0;
	for(LL i = 0; i<global_count;i++)
	{
		int flag=0;
		for(LL j=0; j<100; j++)
			if(arr[i]->pred[j] != NULL)
				flag=1;
		if(flag == 0)
			total++;
			// inputarray[index][0] = arr[i]->node_number;
			// index++;
			// inputarray[index][0] = arr[i]->node_number;
			// index++;
	}	//Calculating the input nodes at level 0.

/*
	LL counter_input[global_count+1]= {0};
	for(LL i = 0; i<global_count; i++)
		if(arr[i]->next!=NULL)
			counter_input[arr[i]->next->node_number]++;
	for(LL i = 0; i<global_count+1; i++)
	{
		if(counter_input[i]==1) {
			inputarray[index][0] = i;
			index++;
		}
	}
	for(LL x = 1; x <= 10000; x++)
	{
		for(LL i = 0; i < index; i++)
			inputarray[i][1] = binaryConv(randomgen());
		for(LL i = 0; i < global_count; i++)
			arr[i]->next_value = binaryConv(randomgen());
		for(LL i = 0; i < global_count; i++)
		{
			LL hamming1,hamming2;
			int flag =0;
			if(arr[i]->pred == NULL)
			{
				for(LL j = 0; j < index; j++)
				{
					if(inputarray[j][0] == arr[i]->node_number && flag == 0){
						hamming1 = hammingDist(inputarray[j][1],arr[i]->next_value);
						flag=1; }
					if(inputarray[j][0] == arr[i]->node_number)
						hamming2 = hammingDist(inputarray[j][1],arr[i]->next_value);
				}
				if(hamming1 < hamming2)
					arr[i]->path_value = arr[i]->path_value + (flipCounter(arr[i]->next_value) + hamming1 ) / totalflip(arr[i]->next_value);
				else
					arr[i]->path_value = arr[i]->path_value + (flipCounter(arr[i]->next_value) + hamming2 ) / totalflip(arr[i]->next_value);
				if(arr[i]->next->pred != arr[i])
				{
					flag=0;
					for(LL j = 0; j < index; j++)
					{
						if(inputarray[j][0] == arr[i]->next->pred->node_number && flag == 0){
							hamming1 = hammingDist(inputarray[j][1],arr[i]->next_value);
							flag=1; }
						if(inputarray[j][0] == arr[i]->next->pred->node_number)
							hamming2 = hammingDist(inputarray[j][1],arr[i]->next_value);
					}
					DD temp1;
					if(hamming1 < hamming2)
						temp1 = arr[i]->path_value + (flipCounter(arr[i]->next_value) + hamming1 ) / totalflip(arr[i]->next_value);
					else
						temp1 = arr[i]->path_value + (flipCounter(arr[i]->next_value) + hamming2 ) / totalflip(arr[i]->next_value);
					if(temp1 < arr[i]->path_value)
						arr[i]->path_value = temp1;
				}
			}
			else
			{
				hamming1 = hamming2 = 0;
				for(LL j = 0; j < index; j++)
				{
					if(inputarray[j][0] == arr[i]->node_number)
					{
						hamming1 = hammingDist(inputarray[j][1], arr[i]->next_value);
					}
				}
				if(hamming1 == 0)
				{
					flag=0;
					for(LL j = 0; j < global_count; j++)
					{
						if(arr[j]->next == arr[i] && flag == 0) {
							hamming1 = hammingDist(arr[j]->next_value, arr[i]->next_value);
							flag = 1;
						}
						if(arr[j]->next == arr[i])
							hamming2 = hammingDist(arr[j]->next_value, arr[i]->next_value);
					}
				}
				else
				{
					for(LL j = 0; j < global_count; j++)
					{
						if(arr[j]->next == arr[i])
							hamming2 = hammingDist(arr[j]->next_value, arr[i]->next_value);
					}
				}
				if(hamming1 < hamming2)
					arr[i]->path_value = arr[i]->path_value + (flipCounter(arr[i]->next_value) + hamming1 ) / totalflip(arr[i]->next_value);
				else
					arr[i]->path_value = arr[i]->path_value + (flipCounter(arr[i]->next_value) + hamming2 ) / totalflip(arr[i]->next_value);
			}
		}
	}
	//for(LL i = 0; i < global_count; i++)
	//	 cout<<arr[i]->node_number<<" "<<(arr[i]->path_value) /10000 <<endl;

*/





	

	LL global_count_sam;	//Number of edges to be calculated at.
	global_count_sam = global_count + total + total;
	total = total * 2;
	DD sam[global_count_sam][global_count_sam];
	LL lowest = 999999;
	DD samlowest[global_count_sam][global_count_sam];
	for(LL x = 0; x < 1000; x++)
	{	
		LL sum = 0;
		DD sam1[global_count_sam][global_count_sam];
		LL inputs[global_count_sam];
		LL inputbin[global_count_sam];
		for(LL i=0; i < global_count_sam; i++)
		{
			inputs[i]=randomgen();
			inputbin[i]=binaryConv(inputs[i]);
			//cout<<inputs[i]<<endl;
		}
		for(LL i = 0 ; i<global_count_sam; i++)
		{
			for(LL j = 0; j<global_count_sam; j++)
			{
				if(i==j)
				{
					sam[i][j]=0;
					sam1[i][j]=0;
				}
				else
				{
					//cout<<totalflip(inputs[i])<<endl;
					sam[i][j] = int(sam[i][j] + ((flipCounter(inputbin[i]) + hammingDist(inputbin[i],inputbin[j])) / totalflip(inputbin[i])));
					sam1[i][j] = (flipCounter(inputbin[i]) + hammingDist(inputbin[i],inputbin[j])) / totalflip(inputbin[i]);
					sum = sum + (flipCounter(inputbin[i]) + hammingDist(inputbin[i],inputbin[j])) / totalflip(inputbin[i]);
				}
			}
		}
		if(sum < lowest)
		{
			lowest = sum;
			for(LL i = 0; i < global_count_sam; i++)
				for(LL j = 0; j < global_count_sam; j++)
					samlowest[i][j]=sam1[i][j];
		}
	}

	for(LL i =0; i<global_count_sam; i++)
	{
		for(LL j=0;j<global_count_sam;j++)
		{
			sam[i][j] = sam[i][j] /100 ;
			//cout<<sam[i][j]<<"   ";
		}
		//cout<<endl;
	}
	cout<<"SAM Made"<<endl;

	//for(LL i =0; i<total; i++)
	//{
	//	for(LL j=0;j<total;j++)
	//	{
			//cout<<samlowest[i][j]<<"   ";
	//	}
		//cout<<endl;
	//}

	/*for(LL j =0 ; j<10000; j++)
	{
		for(LL i = 0; i<global_count; i++)
		{
			if(arr[i]->pred == NULL){
				int input1=randomgen();
				int input2=randomgen();
				if(arr[i]->node_name == "add")
					arr[i]->next->weight = arr[i]->next->weight + (input1 + input2);
				if(arr[i]->node_name == "min")
					arr[i]->next->weight = arr[i]->next->weight + (abs(input1-input2));
				if(arr[i]->node_name == "mul")
					arr[i]->next->weight = arr[i]->next->weight + (input1 * input2);
				if(arr[i]->node_name == "div")
					arr[i]->next->weight == arr[i]->next->weight + input1/input2;
				if(arr[i]->node_name == "exp")
					arr[i]->next->weight == arr[i]->next->weight + pow(input1,input2);
				if(arr[i]->node_name == "les")
					if(input1 < input2)
						arr[i]->next->weight = arr[i]->next->weight + 1;
				else
					arr[i]->next->weight = arr[i]->next->weight + 0;
				if(arr[i]->node_name == "grt")
					if(input1 > input2)
						arr[i]->next->weight = arr[i]->next->weight + 1;
					else
						arr[i]->next->weight = arr[i]->next->weight + 0;
			}
			
		}
	}*/


	//CLIQUE PARTITION METHOD
	for(LL i = 0; i< global_count; i++)
		arr[i]->clique = i+1;

	for(LL i = 0; i<global_count; i++)
	{
		// int countcpm = 0;
		// for(LL l = 0; l<global_count; l++)
		// 	if(strcmp(arr[i]->node_name, arr[l]->node_name) == 0)
		// 		countcpm++;
		//cout<<"countcpm"<<countcpm<<endl;
		int k=0;
		//while(k<countcpm)
		//{
			DD lowestSwitchingValue=999999;
			LL lowestSwitchingClique=0;
			for(LL j = 0; j<global_count; j++)
			{

				if((strcmp(arr[i]->node_name, arr[j]->node_name) == 0) && (arr[i]->clique != arr[j]->clique))
				{
					//cout<<i<<" "<<j<<endl;
					if(sam[i][j] < lowestSwitchingValue) {
						lowestSwitchingValue = sam[i][j];
						lowestSwitchingClique = j;
					}
				}
			}
			//cout<<"countcpm"<<countcpm<<endl;
		//	k++;
		for(LL k=0; k<100; k++)
			if((arr[lowestSwitchingClique]->next[k] == arr[i]) || (arr[i]->next[k] == arr[lowestSwitchingClique]))
				arr[lowestSwitchingClique]->clique = arr[i]->clique;
		//}
	}
	//PRINTING the clique number of the functional units.




	for(LL i=0; i<global_count; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			if(strcmp(arr[i]->node_name, arr[j]->node_name) == 0)
			{
				int flag=0;
				for(LL k=0; k<global_count; k++)
				{
					if(arr[i]->clique == arr[k]->clique)
					{
						for(LL l=0; l<global_count; l++)
						{
							if(arr[j]->clique == arr[l]->clique)
							{
								if(arr[k]->control_step_asap == arr[l]->control_step_asap)
									flag=1;
							}
						}
					}
				}
				if(flag == 0)	//No Conflict
				{
					for(LL k=0; k<global_count; k++)
					{
						if(arr[k]->clique == arr[j]->clique)
							arr[k]->clique = arr[i]->clique;
					}
				}
				else	//Conflict
				{
					LL minmobility = 999;
					for(LL k=0; k<global_count; k++)
					{
						if(arr[k]->clique == arr[j]->clique)
						{
							if(arr[k]->mobility < minmobility)
								minmobility = arr[k]->mobility;
						}
					}
					int flag1=0;
					for(LL l=0; l<global_count; l++)
					{
						if(arr[l]->clique == arr[i]->clique)
						{
							for(LL m=0; m<global_count; m++)
							{
								if(arr[m]->clique == arr[j]->clique)
								{
									if(arr[l]->control_step_asap == (arr[m]->control_step_asap + minmobility))
										flag1=1;
								}
							}
						}
					}
					if(flag1 == 0)
					{
						for(LL l=0; l<global_count; l++)
						{
							if(arr[j]->clique == arr[l]->clique)
							{
								arr[l]->control_step_asap = arr[l]->control_step_asap + minmobility;
								arr[l]->clique = arr[i]->clique;
							}
						}
						break;
					}
				}
			}
		}
	}

	for(LL i=0; i<global_count; i++)
		cout<<arr[i]->node_number<<" "<<arr[i]->clique<<" "<<arr[i]->node_name<<endl;
	int count=0;
	int cliqueno=0;
	for(LL i=0;i<global_count;i++)
	{
		if(arr[i]->clique > cliqueno)
		{
			count++;
			cliqueno = arr[i]->clique;
		}
	}
	cout<<"Total Number of Cliques: "<<count<<endl;
	count=1;
	for(LL i=0; i<global_count; i++)
	{
		cout<<"Clique no. "<<count<<" : ";
		for(LL j=0;j<global_count;j++)
		{
			if(arr[j]->clique == i+1)
				cout<<arr[j]->node_number<<" , ";
		}
		cout<<endl;
		count++;
	}


/*##########################################################################*/

/*
	for(LL i=0; i<global_count;i++)
	{
		int count=0, maxcount=0, maxclique=0;
		for(LL j=0; j<global_count; j++)
		{
			if(strcmp(arr[i]->node_name, arr[j]->node_name) == 0 && arr[i]->clique != arr[j]->clique)
			{
				int flag=0, count=0;
				int minmobility =999;
				for(LL k=0; k<global_count; k++)
				{
					if(arr[k]->clique == arr[i]->clique)
					{
						flag=0;
						for(LL m=0; m<global_count; m++)
						{
							if(arr[m]->clique == arr[j]->clique)
							{
								if(arr[k]->control_step_asap == (arr[m]->control_step_asap + minmobility)){
									flag=1;
									break;}
							}
						}
						if(flag==1)
						{
							minmobility=999;
							for(LL m=0; m<global_count; m++)
							{
								if(arr[m]->clique == arr[j]->clique)
								{
									if(arr[m]->mobility <minmobility)
										minmobility = arr[m]->mobility;
								}
							}
						}
							break;
						count++;
					}
				}
			
				if(flag==0)
				{
					if(maxcount < count)
					{
						maxcount = count;
						maxclique = j;
					}
				}
			}
		}
		for(LL l=0;l<global_count;l++)
			if(arr[l]->clique == arr[maxclique]->clique)
				arr[l]->clique = arr[i]->clique;
	}
*/





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
		for(LL i=0; i<global_count; i++) {
			free(arr[i]);
		//	free(sam[i]);
			//free(sam1[i]);
			//free(inputs[i]);
			//free(inputbin[i]);
		//	free(samlowest[i]);
		}
		//free(global_count);
	return 0;
}