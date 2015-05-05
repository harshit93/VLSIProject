/********************************************/
/*Program: Surge Current Minimization 					*
* Authors: Pritha Ganguly					*
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
#include <math.h>
#define ALU 4
#define MULTIPLIER 4
using namespace std;

//Structure for the Node to store the individual elements of the graphs.
typedef unsigned long long int LL;
typedef struct node {
	char node_name[10];
	node *next[10];
	node *pred[10];
	int node_number;
	int control_step_asap;
	int control_step_alap;
	int mobility;
}node;

//Main Function starts. All the computation of the algorithm resides here.
int main()
{
	ifstream file;	//File Handler
	file.open("Benchmarks/arf.dot",ios::in);	//Accessing the input file. It is in the .dot format. *Needs to be changed according to the input path of the file*
	node *arr[200];	
	for(LL i=0;i<200;i++)
		arr[i] = new node();
	for(LL i=0;i<200;i++)
	{
		for(LL j=0;j<10;j++)
		{
			arr[i]->next[j] = new node();
			arr[i]->pred[j] = new node();
		}
	}
	/*for(LL i=0;i<100;i++)
	{
		for(LL j=0;j<10;j++)
		{
			arr[i]->next[j] = 0;
			arr[i]->pred[j] = 0;
		}
	}*/
	
	
	//int MAP[8][3];
	char num1[10];
	char num2[10];
	int global_count=0;	//Global counter for the number of nodes present in the input graph.
	if(file.is_open())
	{
		string line;
		getline(file, line);
		getline(file, line);
		//file<<line
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
					//cout<<atoi(num1)<<" "<<atoi(num2)<<endl;
					node *vertex1,*vertex2;	//Declaring two node pointers to map one node to another.
					LL ii,kk;
					for(LL j = 0; j < global_count; j++)
					{
						if(arr[j]->node_number == atoi(num1))
							vertex1 = arr[j];
						if(arr[j]->node_number == atoi(num2))
							vertex2 = arr[j];
					}
					for(ii=0;ii<10 && vertex1->next[ii]!=NULL;ii++);//
					for(kk=0;kk<10 && vertex2->pred[kk]!=NULL;kk++);
					//if(vertex1->next[index2]==NULL)
						vertex1->next[ii] = vertex2;	//Defining the next relationship from one node to the other.
					/*else
					{
						//index1=index1+1;
						vertex1->next[++index2] = vertex2;
					}
					if(vertex2->pred[index1]==NULL)
						vertex2->pred[index1] = vertex1;	//Defining the previous relationship in the reverse order.
					else*/
						vertex2->pred[kk] = vertex1;
					//cout<<index1<<" "<<index2<<endl;
					for(int j=0; j<10; j++)
						num1[j]=num2[j]='\0';
				}
			}
		}
	}
	//cout<<index1<<" "<<index2<<endl;
	cout<<global_count<<endl;
	for(LL i=0;i<global_count;i++)
	{
		cout<<arr[i]->node_number<<endl;
		for(LL j=0;j<10;j++)
		{
			if(arr[i]->pred[j]!=NULL)
				cout<<" Pred: "<<j<<" "<<arr[i]->pred[j]->node_number<<endl;
		}
		for(LL j=0;j<10;j++)
		{
			if(arr[i]->next[j]!=NULL)
				cout<<" Succ: "<<j<<" "<<arr[i]->next[j]->node_number;
		}
		cout<<endl;
	}
	/*for(int i=0;i<global_count;i++)
		cout<<arr[i]->pred<<" "<<arr[i]->node_number<<" "<<arr[i]->next<<endl;*/
	file.close();	//Closing the openend file.
	int max=1;

	//ASAP Scheduling
	for(LL i = 0; i < global_count; i++)	//Scheduling the nodes according to ASAP notation.
	{
		int flag1 = 0;
		for(LL j = 0; j < 2; j++)
		{	
			if(arr[i]->pred[j] == NULL)
			{
				if(flag1 == 0)
				{
					arr[i]->control_step_asap = 1;	//All the nodes with no predecessor are assigned in the First control step.
					arr[i]->mobility = arr[i]->control_step_asap;	//Assigning the control step value to mobility
					flag1 = 1;
				}
			}			
			else
			{
				if(arr[i]->control_step_asap < (arr[i]->pred[j]->control_step_asap + 1))//ASAP Scheduling for the nodes which are not present in Control Step 1
				{
					arr[i]->control_step_asap = arr[i]->pred[j]->control_step_asap + 1;
					arr[i]->mobility = arr[i]->control_step_asap;
					if(max< (arr[i]->control_step_asap))
						max = arr[i]->control_step_asap;
					flag1 = 1;
				}	
			}
		}
	}
	//cout<<max<<endl;
	//ALAP Scheduling
	int max1=max;
	for(LL i = 0; i < global_count; i++)	//Scheduling the nodes in the last Control Step
	{
		for(LL j=0;j<3;j++)
		{
			if(arr[i]->next[j] == NULL)
				arr[i]->control_step_alap = max;
		}
	}
	for(LL i = 0; i < max; i++)
	{
		for(LL j = 0; j < global_count; j++)
		{
			for(LL k=0;k<3;k++)
			{
				if(arr[j]->next[k] != NULL)
					if(arr[j]->next[k]->control_step_alap == max1)
						arr[j]->control_step_alap = max1-1;
			}
		}
		max1--;
	}
	cout<<max<<" "<<max1<<endl;
	for(LL i = 0; i < global_count; i++)
	{	//Calculating the Mobility Value by Subtracting the previously computed ASAP Control Step Value from current ALAP Control Step value.
		arr[i]->mobility = arr[i]->control_step_alap - arr[i]->control_step_asap;
		cout<<arr[i]->node_number<<" "<<arr[i]->control_step_asap<<" "<<arr[i]->control_step_alap<<" "<<arr[i]->mobility<<endl;
	}
	//for(int i=0;i<100;++i)
	//	delete arr[i];
        //Generating uniqueness constraint equations......(1)
	ofstream file1;
	file1.open("equations_hal.opb",ios::out);
	bool c6,c5,c4,c3,c2,c1,c0;
	file1<<"min:";
	for(int range = 6; range >= 0 ; range--)
	{
		file1<<" +"<<pow(2,range)<<" c"<<range;
	}
	file1<<";"<<endl;
	//file1<<"min: +128 c7 +64 c6 +32 c5 +16 c4 +8 c3 +4 c2 +2 c1 +1 c0;"<<endl;
	for(LL i=0; i < global_count; i++)
	{
		for(LL j=arr[i]->control_step_asap; j <= arr[i]->control_step_alap; j++)
		{
			file1<<"+1 x"<<arr[i]->node_number<<j<<" ";
		}
		file1<<"= 1;"<<endl;
	}
	
	//Generating dependency constraint equations........(2)
	for(LL i=0; i < global_count; i++)
	{
		for(LL j=0;j<3;++j)
		{
			if((arr[i]->mobility) != 0 && (arr[i]->next[j]) != NULL)
			{
				if((arr[i]->next[j]->mobility) != 0)			
				{	
					for(LL jj=arr[i]->control_step_asap; jj <= arr[i]->control_step_alap; jj++)
						file1<<"+"<<jj<<" x"<<arr[i]->node_number<<jj<<" ";
					for(LL k=arr[i]->next[j]->control_step_asap; k <= arr[i]->next[j]->control_step_alap; k++)
						file1<<"-"<<k<<" x"<<arr[i]->next[j]->node_number<<k<<" ";
					file1<<"< 0;"<<endl;
				}
			}
		}
	}
	/*for(LL i=0; i < global_count; i++)
	{
		if((arr[i]->mobility) != 0 && (arr[i]->pred) != NULL)
		{
			if((arr[i]->pred->mobility) != 0)			
			{	
				for(LL j=arr[i]->pred->control_step_asap; j <= arr[i]->pred->control_step_alap; j++)
					file1<<"+"<<j<<" x"<<arr[i]->pred->node_number<<j<<" ";
				for(LL k=arr[i]->control_step_asap; k <= arr[i]->control_step_alap; k++)
					file1<<"-"<<k<<" x"<<arr[i]->node_number<<k<<" ";
				file1<<"< 0;"<<endl;
			}
		}
	}
	for(LL i=0; i < global_count; i++)
	{
		if((arr[i]->pred) != NULL)
		{	
			for(LL j=arr[i]->pred->control_step_asap; j <= arr[i]->pred->control_step_alap; j++)
				file1<<"+"<<j<<" x"<<arr[i]->pred->node_number<<j<<" ";
			for(LL k=arr[i]->control_step_asap; k <= arr[i]->control_step_alap; k++)
		 		file1<<"-"<<k<<" x"<<arr[i]->node_number<<k<<" ";
			file1<<"< 0;"<<endl;
		}
	}*/
	//Generating resource constraint equations(New addition to the existing constraints).....(3)
	/*int flag=0;
	for(LL i=1; i<= max; i++)
	{
		flag=0;
		for(LL j=0; j<global_count; j++)
		{
			if((arr[j]->control_step_asap <= i) && (arr[j]->control_step_alap >= i) && ((arr[j]->node_name[0] == 'a')||(arr[j]->node_name[0] == 's')||(arr[j]->node_name[0] == 'l')))
			{
				file1<<"+1 x"<<arr[j]->node_number<<i<<" ";
				flag = 1;
			}
		}
		if(flag == 1)
			file1<<"<= "<<ALU<<";"<<endl;
	}	
	for(LL i=1; i<= max; i++)
	{	
		flag=0;
		for(LL j=0; j<global_count; j++)
		{
			if((arr[j]->control_step_asap <= i) && (arr[j]->control_step_alap >= i) && (arr[j]->node_name[0] == 'm'))
			{
				file1<<"+1 x"<<arr[j]->node_number<<i<<" ";
				flag=1;
			}
		}
		if(flag == 1)
			file1<<"<= "<<MULTIPLIER<<";"<<endl;
	}*/
	/*for(LL i=1; i<= max; i++)
	{
		flag=0;
		for(LL j=0; j<global_count; j++)
		{
			if((arr[j]->control_step_asap <= i) && (arr[j]->control_step_alap >= i) && (arr[j]->node_name[0] == 's'))
			{
				file1<<"+1 x"<<arr[j]->node_number<<i<<" ";
				flag=1;
			}
		}
		if(flag == 1)
			file1<<"<= "<<SUBTRACTOR<<";"<<endl;
	}
	for(LL i=1; i<= max; i++)
	{
		flag = 0;
		for(LL j=0; j<global_count; j++)
		{
			if((arr[j]->control_step_asap <= i) && (arr[j]->control_step_alap >= i) && (arr[j]->node_name[0] == 'l'))
			{
				file1<<"+1 x"<<arr[j]->node_number<<i<<" ";
				flag = 1;
			}
		}
		if(flag == 1)
			file1<<"<= "<<COMPARATOR<<";"<<endl;
	}*/	
		
	//Each operation must be assigned to only one functional unit.......(4)
	for(LL j=0;j<global_count;j++)
	{
		if(arr[j]->node_name[0] == 'm')
		{
			for(int i=1;i<=MULTIPLIER;i++)
			{
				file1<<"+1 y"<<arr[j]->node_number<<"1"<<i<<" ";
			}
			file1<<"= 1;"<<endl;
		}
		else //if(arr[j]->node_name[0] == 'a')
		{
			for(int i=1;i<=ALU;i++)
			{
				file1<<"+1 y"<<arr[j]->node_number<<"2"<<i<<" ";
			}
			file1<<"= 1;"<<endl;
		}
		/*else if(arr[j]->node_name[0] == 's')
		{
			for(int i=1;i<=SUBTRACTOR;i++)
			{
				file1<<"+1 y"<<arr[j]->node_number<<"3"<<i<<" ";
			}
			file1<<"= 1;"<<endl;
		}
		else
		{
			for(int i=1;i<=COMPARATOR;i++)
			{
				file1<<"+1 y"<<arr[j]->node_number<<"4"<<i<<" ";
			}
			file1<<"= 1;"<<endl;
		}*/
	}
	//No two operations can be assigned to one functional unit......(5)
	LL **mult_matrix  = new LL*[max];
	LL **add_matrix  = new LL*[max];
	//LL **sub_matrix  = new LL*[max];
	//LL **comp_matrix  = new LL*[max];
	for(LL i=0;i<max;++i)
	{
		mult_matrix[i]=new LL[global_count];
		add_matrix[i]=new LL[global_count];
		//sub_matrix[i]=new LL[global_count];
		//comp_matrix[i]=new LL[global_count];
	}
	for(LL i=0; i<max; ++i)
	{
		for(LL j=0; j<global_count; j++)
		{
			mult_matrix[i][j]=0;
			add_matrix[i][j]=0;
			//sub_matrix[i][j]=0;
			//comp_matrix[i][j]=0;
		}
		cout<<endl;
	}
	for(LL i=0; i<max; i++)
	{
		//mult_matrix[i][0]=i+1;
		//add_matrix[i][0]=i+1;
		//sub_matrix[i][0]=i+1;
		//comp_matrix[i][0]=i+1;
		for(LL j=0; j<global_count; j++)
		{
			if(arr[j]->node_name[0] == 'm')
			{
				if((arr[j]->control_step_asap <= (i+1)) && ((i+1) <= arr[j]->control_step_alap))
					mult_matrix[i][j]=arr[j]->node_number;
			}
			else //if(arr[j]->node_name[0] == 'a')
			{
				if((arr[j]->control_step_asap <= (i+1)) && ((i+1) <= arr[j]->control_step_alap))
					add_matrix[i][j]=arr[j]->node_number;
			}
			/*else if(arr[j]->node_name[0] == 's')
			{
				if((arr[j]->control_step_asap <= (i+1)) && ((i+1) <= arr[j]->control_step_alap))
					sub_matrix[i][j]=arr[j]->node_number;
			}
			else
			{
				if((arr[j]->control_step_asap <= (i+1)) && ((i+1) <= arr[j]->control_step_alap))
					comp_matrix[i][j]=arr[j]->node_number;
			}*/
		}
	}
	for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			for(LL k=1;k<global_count; k++)
			{
				if((mult_matrix[i][j]!= 0) && (mult_matrix[i][k]!= 0) && (mult_matrix[i][j] < mult_matrix[i][k]))
				{
					for(LL l=1; l<=MULTIPLIER; l++)
					{
						file1<<"+1 y"<<mult_matrix[i][j]<<"1"<<l<<" +1 y"<<mult_matrix[i][k]<<"1"<<l<<" +1 x"<<mult_matrix[i][j]<<(i+1)<<" +1 x"<<mult_matrix[i][k]<<(i+1)<<" <= 3;"<<endl;
					}
				}
				
			}
		}
	}
	for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			for(LL k=1;k<global_count; k++)
			{
				if((add_matrix[i][j]!= 0) && (add_matrix[i][k]!= 0) && (add_matrix[i][j] < add_matrix[i][k]))
				{
					for(LL l=1; l<=ALU; l++)
					{
						file1<<"+1 y"<<add_matrix[i][j]<<"2"<<l<<" +1 y"<<add_matrix[i][k]<<"2"<<l<<" +1 x"<<add_matrix[i][j]<<(i+1)<<" +1 x"<<add_matrix[i][k]<<(i+1)<<" <= 3;"<<endl;
					}
				}
				
			}
		}
	}
	/*for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			for(LL k=1;k<global_count; k++)
			{
				if((sub_matrix[i][j]!= 0) && (sub_matrix[i][k]!= 0) && (sub_matrix[i][j] < sub_matrix[i][k]))
				{
					for(LL l=1; l<=SUBTRACTOR; l++)
					{
						file1<<"+1 y"<<sub_matrix[i][j]<<"3"<<l<<" +1 y"<<sub_matrix[i][k]<<"3"<<l<<" +1 x"<<sub_matrix[i][j]<<(i+1)<<" +1 x"<<sub_matrix[i][k]<<(i+1)<<" <= 3;"<<endl;
					}
				}
				
			}
		}
	}
	for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			for(LL k=1;k<global_count; k++)
			{
				if((comp_matrix[i][j]!= 0) && (comp_matrix[i][k]!= 0) && (comp_matrix[i][j] < comp_matrix[i][k]))
				{
					for(LL l=1; l<=COMPARATOR; l++)
					{
						file1<<"+1 y"<<comp_matrix[i][j]<<"4"<<l<<" +1 y"<<comp_matrix[i][k]<<"4"<<l<<" +1 x"<<comp_matrix[i][j]<<(i+1)<<" +1 x"<<comp_matrix[i][k]<<(i+1)<<" <= 3;"<<endl;
					}
				}
				
			}
		}
	}*/

	//If ith operation is scheduled in jth control step in kth functional unit, then kth F.U would be active in jth control step....(6)
	for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			if((mult_matrix[i][j]!= 0))
			{
				for(LL l=1; l<=MULTIPLIER; l++)
				{
					file1<<"+1 x"<<mult_matrix[i][j]<<(i+1)<<" +1 y"<<mult_matrix[i][j]<<"1"<<l<<" -1 z"<<"1"<<l<<(i+1)<<" <= 1;"<<endl;
				}
			}
				
		}
	}
	for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			if((add_matrix[i][j]!= 0))
			{
				for(LL l=1; l<=ALU; l++)
				{
					file1<<"+1 x"<<add_matrix[i][j]<<(i+1)<<" +1 y"<<add_matrix[i][j]<<"2"<<l<<" -1 z"<<"2"<<l<<(i+1)<<" <= 1;"<<endl;
				}
			}
				
		}
	}
	/*for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			if((sub_matrix[i][j]!= 0))
			{
				for(LL l=1; l<=SUBTRACTOR; l++)
				{
					file1<<"+1 x"<<sub_matrix[i][j]<<(i+1)<<" +1 y"<<sub_matrix[i][j]<<"3"<<l<<" -1 z"<<"3"<<l<<(i+1)<<" <= 1;"<<endl;
				}
			}
				
		}
	}
	for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			if((comp_matrix[i][j]!= 0))
			{
				for(LL l=1; l<=COMPARATOR; l++)
				{
					file1<<"+1 x"<<comp_matrix[i][j]<<(i+1)<<" +1 y"<<comp_matrix[i][j]<<"4"<<l<<" -1 z"<<"4"<<l<<(i+1)<<" <= 1;"<<endl;
				}
			}
				
		}
	}*/
	cout<<"Multiplier "<<endl;
	for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			cout<<mult_matrix[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<"Adder "<<endl;
	for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			cout<<add_matrix[i][j]<<" ";
		}
		cout<<endl;
	}
	/*cout<<"Subtractor "<<endl;
	for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			cout<<sub_matrix[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<"Comparator "<<endl;
	for(LL i=0; i<max; i++)
	{
		for(LL j=0; j<global_count; j++)
		{
			cout<<comp_matrix[i][j]<<" ";
		}
		cout<<endl;
	}*/
	for(LL i = 0; i < max; ++i)
	{
    		delete [] mult_matrix[i];
    		delete [] add_matrix[i];
    		//delete [] sub_matrix[i];
    		//delete [] comp_matrix[i];
	}
	delete [] mult_matrix;
	delete [] add_matrix;
	//delete [] sub_matrix;
	//delete [] comp_matrix;

	//initially F.Us are idle
	for(int i=1; i<=MULTIPLIER; i++)
	{
		file1<<"+1 z1"<<i<<"0 = 0;"<<endl;
	}
	for(int i=1; i<=ALU; i++)
	{
		file1<<"+1 z2"<<i<<"0 = 0;"<<endl;
	}
	/*for(int i=1; i<=SUBTRACTOR; i++)
	{
		file1<<"+1 z3"<<i<<"0 = 0;"<<endl;
	}
	for(int i=1; i<=COMPARATOR; i++)
	{
		file1<<"+1 z4"<<i<<"0 = 0;"<<endl;
	}*/

	//s(k,j) indicates F.U k is turned on at jth control step......(7)
	for(int j=1; j<=max; j++)
	{
		for(int k=1; k<=MULTIPLIER; k++)
		{
			file1<<"+1 z1"<<k<<j<<" -1 z1"<<k<<(j-1)<<" -1 s1"<<k<<j<<" <= 0;"<<endl;
		}
	}
	for(int j=1; j<=max; j++)
	{
		for(int k=1; k<=ALU; k++)
		{
			file1<<"+1 z2"<<k<<j<<" -1 z2"<<k<<(j-1)<<" -1 s2"<<k<<j<<" <= 0;"<<endl;
		}
	}
	/*for(int j=1; j<=max; j++)
	{
		for(int k=1; k<=SUBTRACTOR; k++)
		{
			file1<<"+1 z3"<<k<<j<<" -1 z3"<<k<<(j-1)<<" -1 s3"<<k<<j<<" <= 0;"<<endl;
		}
	}
	for(int j=1; j<=max; j++)
	{
		for(int k=1; k<=COMPARATOR; k++)
		{
			file1<<"+1 z4"<<k<<j<<" -1 z4"<<k<<(j-1)<<" -1 s4"<<k<<j<<" <= 0;"<<endl;
		}
	}*/

	//The summation of the numbers of active control steps of all functional units is equal to the number of operations......(8)
	for(int j=1; j<=max; j++)
	{
		for(int k=1; k<=MULTIPLIER; k++)
		{
			file1<<"+1 z1"<<k<<j<<" ";
		}
		for(int k=1; k<=ALU; k++)
		{
			file1<<"+1 z2"<<k<<j<<" ";
		}
		/*for(int k=1; k<=SUBTRACTOR; k++)
		{
			file1<<"+1 z3"<<k<<j<<" ";
		}
		for(int k=1; k<=COMPARATOR; k++)
		{
			file1<<"+1 z4"<<k<<j<<" ";
		}*/
	}
	file1<<" = "<<global_count<<";"<<endl;

	for(int j=1; j<=max; j++)
	{
		for(int k=1; k<=MULTIPLIER; k++)
		{
			file1<<"+56 s1"<<k<<j<<" ";
		}
		for(int k=1; k<=ALU;k++)
		{
			file1<<"+49 s2"<<k<<j<<" ";
		}
		/*for(int k=1; k<=SUBTRACTOR;k++)
		{
			file1<<"+49 s3"<<k<<j<<" ";
		}
		for(int k=1; k<=COMPARATOR;k++)
		{
			file1<<"+46 s4"<<k<<j<<" ";
		}*/
		/*for(int range = 11; range >= 0 ; range--)
		{
			file1<<" -"<<pow(2,range)<<" c"<<range;
		}
		file1<<" <= 0;"<<endl;*/
		file1<<" -64 c6 -32 c5 -16 c4 -8 c3 -4 c2 -2 c1 -1 c0 <= 0;"<<endl;
	}
	file1.close();
	for(LL i=0;i<200;++i)
	{
		/*for(LL j=0;j<20;++j)
		{
			delete[] arr[i]->next[j];
			delete[] arr[i]->pred[j];
		}*/
		delete arr[i];
	}
	
	//file.close(); //Closing the openend file.
	return 0;
}
