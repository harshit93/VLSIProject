#include<bits/stdc++.h>

#define CHAOS_SIZE 101
#define POPULATION_SIZE 100

using namespace std;

//table used to convert decimal numbers to binary
string dec_to_binary[] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001"};

//fucntion to generate random values in a given range
double get_random(double low, double high)
{
  double random = (double)rand()/RAND_MAX;
  return low + (high - low)*random;
}

//member of the population
typedef struct
{
  double R;
  double chaos_function_values[CHAOS_SIZE];
  double average;
  int chaos[CHAOS_SIZE];
  int count[2][2];
  int single[2];
  double fitness_value;
  char c[21];
}member;

vector <member *> initial_population;
vector <member *> final_population;

//function to check if a chromosome passes fitness test or not if yes then return true else retrun false
bool iffit(member *temp)
{
  //generate chaotic map function values
  for(int j=1;j<CHAOS_SIZE;j++)
    temp->chaos_function_values[j] = (temp->R)*(temp->chaos_function_values[j-1])*(1-temp->chaos_function_values[j-1]);
  
  double sum = 0;
  for(int j=0;j<CHAOS_SIZE;j++)
    sum += temp->chaos_function_values[j];
  //fill the average value of member
  temp->average = sum/CHAOS_SIZE;
  
  //convert the numbers to a chaotic string based on the threshold value which is the average
  for(int j=0;j<CHAOS_SIZE;j++)
    temp->chaos[j] = (temp->chaos_function_values[j]>temp->average?1:0);
  
  temp->count[0][0] = 0;
  temp->count[1][0] = 0;
  temp->count[0][1] = 0;
  temp->count[1][1] = 0;
  temp->single[0] = 0;
  temp->single[1] = 0;
  
  //count the number of 1's and 0's in the chaos
  for(int j=0;j<CHAOS_SIZE;j++)
    temp->single[temp->chaos[j]]++;
  
  for(int j=0;j<CHAOS_SIZE-1;j++)
    temp->count[temp->chaos[j]][temp->chaos[j+1]]++;
  
  double n00 = temp->count[0][0];
  double n01 = temp->count[0][1];
  double n10 = temp->count[1][0];
  double n11 = temp->count[1][1];
  double n0 = temp->single[0];
  double n1 = temp->single[1];
  double n = CHAOS_SIZE;
  
  //fill fitness value
  temp->fitness_value = (((double)4/(n-1))*(n00*n00+n01*n01+n10*n10+n11*n11)) - (((double)2/n)*(n0*n0+n1*n1)) + 1;
  
  if(temp->fitness_value < 7.8147)
    return true;
  else
    return false;
}

//function accepts a double as input and returns first five integers after decimal point
int get_decimals(double x)
{
  return (x-floor(x))*100000;
}

//function to select the best chromosome of the current generation
member* choose_best()
{
  double best_fitness = INT_MAX;
  int index = 0;
  for(int i=0;i<POPULATION_SIZE;i++)
  {
    if(best_fitness > initial_population[i]->fitness_value)
    {
      best_fitness = initial_population[i]->fitness_value;
      index = i;
    }
  }
  return initial_population[index];
}

//function to convert the first five digits of x0 to bcd
void convert_to_bcd()
{
  for(int i=0;i<POPULATION_SIZE;i++)
  {
    int temp = get_decimals(initial_population[i]->chaos_function_values[0]);
    string c1 = "";
    for(int j=0;j<5;j++)
    {
      c1 = dec_to_binary[temp%10] + c1;
      temp /= 10;
    }
    for(int k=0;k<20;k++)
      initial_population[i]->c[k] = c1[k];
    initial_population[i]->c[21] = '\0';
//     cout<<initial_population[i]->c<<" "<<initial_population[i]->chaos_function_values[0]<<endl;
  }
}

void crossover()
{
  for(int i = 0;i<POPULATION_SIZE;i++)
  {
    char temp1 = initial_population[i]->c[3], temp2 = initial_population[i]->c[13];
    initial_population[i]->c[3] = initial_population[i]->c[5];
    initial_population[i]->c[13] = initial_population[i]->c[15];
    initial_population[i]->c[5] = temp1;
    initial_population[i]->c[15] = temp2;
  }
}

int get_random_int(int a, int b)
{
  int random = rand()%(b-a+1);
  return a+random;
}

void mutation()
{
  for(int i=0;i<POPULATION_SIZE;i++)
  {
    int mut_point1 = get_random_int(6,9), mut_point2 = get_random_int(16,19);
    initial_population[i]->c[mut_point1] = (initial_population[i]->c[mut_point1] == '1'?'0':'1');
    initial_population[i]->c[mut_point2] = (initial_population[i]->c[mut_point2] == '1'?'0':'1');
//     cout<<initial_population[i]->c<<endl;
  }
}

void convert_to_decimal()
{
  for(int i=0;i<POPULATION_SIZE;i++)
  {
    string c1 = initial_population[i]->c;
//     cout<<c1<<" ";
    int a, sum = 0, weight = 1, x = 0;
    for(int j = 0;j<20;j+=4)
    {
      sum = 0;
      weight = 1;
      for(int l=3;l>=0;l--)
      {
	sum += (int(c1[j+l]) - 48)*weight;
	weight *= 2;
      }
      x += (sum%10);
      x *= 10;
    }
    x /= 10;
    initial_population[i]->chaos_function_values[0] = (double)x/100000;
  }
}

double get_absolute(double x)
{
  return (x<0?-x:x);
}

int main()
{
  srand(time(NULL));
  int size = 0;
  
  //generate 100 members of initial population
  while(size != POPULATION_SIZE)
  {
    member *temp = (member *)malloc(sizeof(member));
    temp->R = get_random(3.5699456, 4.00);
    temp->chaos_function_values[0] = get_random(0, 1);
    if(iffit(temp))
    {
      initial_population.push_back(temp);
      size++;
    }
    else
    {
      free(temp);
    }
  }
  
  int count = 0;
  member *best = (member *)malloc(sizeof(member));
  best = choose_best();
  int generations = 1;
  while(count != 3)
  {
    generations++;
    convert_to_bcd();
    crossover();
    mutation();
    convert_to_decimal();
    
    for(int i = 0;i<POPULATION_SIZE;i++)
      iffit(initial_population[i]);
    
    member *temp_best = (member *)malloc(sizeof(member));
    temp_best = choose_best();
    
    //we have found a new best chromosome
    if(best->fitness_value < temp_best->fitness_value) 
      count++;
    else if(get_absolute(best->fitness_value - temp_best->fitness_value) <= 0.0001)
    {
      count++;
      best = (best->fitness_value > temp_best->fitness_value)?temp_best:best;
    }
    else
    {
      best = temp_best;
      count = 0;
    }
  }
  
  cout<<best->R<<" "<<best->chaos_function_values[0]<<" "<<best->fitness_value<<endl;
  cout<<generations<<endl;
  return 0;
}