/***
TODO:                                       STATUS

    Implement basic function:
        insert_ready_ops                    DONE
        schedule_op                         DONE
        delete                              DONE
        first                               DONE
    Implement List Scheduling               WIP
    Implement Schedule Structure            DONE, (FEATURES MAY BE ADDED)
    Implement Plist                         DONE, (FEATURES MAY BE ADDED)
    Graph Linking                           Done

** Requires reading schedule from Graph with ASAP , ALAP & Mobility of each vertex already computed.
    ---> Function added to compute mobility,asap,alap
BUGS
    1. Inadequate hw pushes other operation in that Control Step to below it - FIXED (It ignores the new schedule operation)

***/

#include<iostream>
#include<malloc.h>
#include<vector>
#include<string>
#include<stdlib.h>
#include<stack>

# define OP_PRINT 0
# define LIST_PRINT 1
#define SCHEDULE_OPS_PRINT 2

#define SUCCESS 1
#define FAILURE 0

#define MAX_LT 100

using namespace std;
typedef struct PList
{
    char op;
    PList *next; //Schedule Structure
    //PList *fwd; // Original Graph Sequence
    int mobility;
    int vertex_no;
    int cStep;
    PList(char init_op)
    {
        op=init_op;
        next=NULL;
        mobility=0;
        vertex_no=0;
        cStep=-1;
    }
    int toString()
    {
        cout<<" "<<vertex_no<<"<"<<mobility<<"> ";
        return 1;
    }
};
typedef struct PListHead
{
    PList *head;
};
typedef struct PListMain
{
    int M; /// No of Operation ----NOT---- no of vertex
    char op_type[MAX_LT];
    PListHead *direct;
    PListMain(string ops)
    {
        M=ops.length();
        direct=(PListHead *) malloc(M * sizeof(PListHead));
        for(int i=0;i<M;i++)
        {
             op_type[i]=ops[i];
        }
    }

 int toString()
    {
        cout<<"\n\n--------PLIST CONTENTS------------\n\n";
        for(int i=0;i<M;i++)
        {
            printf("%c ",op_type[i]);
            PList *p=direct[i].head;
            while(p!=NULL)
            {
                cout<<" "<<p->vertex_no<<"<"<<p->mobility<<"> ";
                p=p->next;

            }
            cout<<"\n";

        }
        cout<<"\n--------PLIST CONTENTS------------\n\n";
    }

};
typedef struct ScheduleHead
{
    PList *head;
};
typedef struct Schedule
{
    int n_cSteps;
    int n_ops;
    ScheduleHead *pOps;// 100 - max no of operations
    string op_arrange;
    Schedule(int n_cS,string op)
    {
        //op - String for hardware constraints like adder
        /***
        n_cS - no of control steps
        n_o - no of operators (5 for below)
        Usage : For 2 multiplication, 1 adder, 1 subtractor, 1 comparator
        op should be : **+-> (in any order)
        ***/
        n_cSteps=n_cS;
        n_ops=op.length();
        op_arrange=op;
        pOps = (ScheduleHead*) malloc(n_ops * sizeof(ScheduleHead));

        for (int i = 0; i < n_ops; ++i)
        {
            PList *p=new PList(op[i]);
            pOps[i].head = p;
        /*for(int i=0;i<n_ops;i++)
        {
        	pOps[i]=new PList(op[i]);
        	//pOps[i]->next=NULL;
        }*/
        }

    }
    int toString()
    {
        cout<<"\n\n-----------------SCHEDULE-----------------\n\n";
        cout<<"VERTEX NUMBER < MOBILITY > CONTROL STEP\n";
        cout<<"\ncSteps: "<<n_cSteps<<"\t nOps: "<<n_ops<<"\n";
            for(int j=0;j<n_ops;j++)
            {
                cout<<""<<op_arrange[j]<<"\t";
                PList *temp=pOps[j].head;
                while(temp!=NULL)
                {
                    if(temp->cStep!=-1)
                    {
                    temp->toString();
                    cout<<" "<<temp->cStep;
                    cout<<" -->  ";
                    }
                    temp=temp->next;

                }
                 cout<<"\n";
            }
        cout<<"\n-----------------SCHEDULE-----------------\n\n";
        return 1;
    }
};
typedef struct AdjListNode
{
    int dest;
    AdjListNode* next;
};

// A structure to represent an adjacency liat
typedef struct AdjList
{
    AdjListNode *head;  // pointer to head node of list
};

// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
typedef struct Graph
{
    int V;
    AdjList* su;
    AdjList* pre;
    int mob[MAX_LT];
    char operation[MAX_LT];
    int stat[MAX_LT];
};
// A utility function to create a new adjacency list node
AdjListNode* newAdjListNode(int dest)
{
    AdjListNode* newNode =(AdjListNode*) malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}
///  Global Variable Declaration HERE
///
///
///


// A utility function that creates a graph of V vertices
Graph* createGraph(int V)
{
    Graph* graph = (Graph*) malloc(sizeof(Graph));
    graph->V = V;

    // Create an array of adjacency lists.  Size of array will be V
    graph->su = (AdjList*) malloc(V * sizeof(AdjList));
    graph->pre = (AdjList*) malloc(V * sizeof(AdjList));

     // Initialize each adjacency list as empty by making head as NULL

    for (int i = 0; i < V; ++i){
        graph->operation[i]='E';
        graph->su[i].head = NULL;
        graph->pre[i].head = NULL;
        graph->mob[i]=-1;
        graph->stat[i]=1;
        }
    return graph;
}
void addEdge(Graph* graph, int src, int dest)
{
    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.  The node is added at the begining
    AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->su[src].head;
    graph->su[src].head = newNode;

    // Since graph is undirected, add an edge from dest to src also
    newNode = newAdjListNode(src);
    newNode->next = graph->pre[dest].head;
    graph->pre[dest].head = newNode;
}

/*** Allocates mobility to vertices after computation ***/
int alloc_op_mbty(Graph* graph,char *op,int *arr)
{
    //cout<<"\n";
    //cout<<op<<"\n";
    for(int v=0;v<graph->V;++v)
    {

        graph->mob[v]=arr[v];
        graph->operation[v]=op[v];
       // cout<<v<<" : "<<graph->mobility[v]<<"\n";
    }
    return 1;
}

int checkConflicts(PList *p,PList *k,int Cstep)
{
    while(p!=NULL)
    {
        if(p->cStep>0 && p->cStep==Cstep)
            return 1;
        p=p->next;
    }
return 0;
}
PList* delete_(PList *p,int del)
{
    if(p==NULL)
    {
        return p;
    }
    if(p->vertex_no==del)
    {
        //cout<<"\n\tDelete Testing\n";
        p=p->next;
        return p;
    }
    PList *temp=p;
    while(temp!=NULL)
    {
        if(temp->vertex_no==del)
        {
                /***   VERIFICATION NEEDED IF IT DELETES PROPERLY LAST NODE OF LINKED LIST  ***/
                PList *t=temp->next;
                temp->next=NULL;
                temp=t;
        }
        temp=temp->next;
    }
    return p;
}
PList* insert_PList(PList *p,PList *k,int x)
{
    int t=0;
    if(p->cStep==-1)
    {
        p=k;t=1;
    }
    else if(p->next==NULL)
    {
        if(k->cStep>p->cStep)
        {
            p->next=k;
        }
        else
        {
            k->next=p;
            p=k;
        }
        t=1;
    }
        PList *prev=p;
        PList *curr=p->next;
        /*Code to insert a node by comparing cStep*/
        while(curr!=NULL)
        {
            if(curr->cStep>x)// IMP
            {
                prev->next=k;
                k->next=curr;
                break;
            }
            prev=curr;
            curr=curr->next;

        }
    if(t==0)
    {
        prev->next=k;
    }
    k->cStep=x;
    return p;
}
int schedule_op(Schedule *S,PList *tk,int Cstep,int funit)
{
        int loc=-1;
        /*for(int i=0;i<S->op_arrange.length();i++)
        {
        //cout<<"\nINDEX: "<<loc<<"\n";
            if(checkConflicts(S->pOps[i],tk,Cstep))
            {
                loc=i;
                break;
            }
        }
        */
        loc=funit;
        //loc=0;
        //cout<<"\nINDEX: "<<loc<<"\n";
        if(loc>=0)
        {
            S->pOps[loc].head=insert_PList(S->pOps[loc].head,tk,Cstep);
            /*PList *temp=S->pOps[loc].head;
            tk->cStep=Cstep;
            //tk->toString();
            //S->pOps[loc]=insert_PList(S->pOps[loc],tk,Cstep);
            //cout<<tk->cStep;
            if(temp->cStep==-1)
            {
                S->pOps[loc].head=tk;
            }
            /*else
            {
                while(temp->next!=NULL)
                    temp=temp->next;
                temp->next=tk;
            }
            */
            return SUCCESS;
        }
    return FAILURE;
}
/*Graph* delete_Pred(Graph* graph,int V)
{
    for(int v=0;v<graph->V;v++)
    {

    }
}*/
Graph* delete_Succ(Graph *graph,int fromDel,int toDel)
{
    AdjListNode *curr=graph->pre[fromDel].head;
    AdjListNode *prev=curr;
    //printf("\nFrom %d delete %d\n",curr->dest,toDel);
    if(curr==NULL)
        {
        curr=NULL;
       // cout<<"1.\n";
        }
    else if(curr->dest==toDel)
        {
        /// Not Working
        /// FIXED
        graph->pre[fromDel].head=graph->pre[fromDel].head->next;
       // cout<<"2.\n";
       //printf("Delete from %d to %d\n",fromDel,toDel);
        }
    else
    {
    prev=curr;
    curr=curr->next;
    //cout<<"3.\t";
    while(curr!=NULL)
    {
       // cout<<curr->dest<<"\n";

        if(curr->dest==toDel)
        {
            prev->next=curr->next;
            curr->next=NULL;
            break;
        }

        prev=curr;
        curr=curr->next;
    }
    }
return graph;
}
int checkPred(Graph *graph,int v)
{
    //return true for empty predecessors
     AdjListNode* p = graph->pre[v].head;
     if(p==NULL)
        return SUCCESS;
    else
        return FAILURE;
}

int insert_ready_ops(Graph *graph,PListMain *plMain)
{
    /***
    Scans all nodes, check for predecessors ,
    If empty deletes V ,and appends to Plist based on operation type
    ***/
    vector<int> visited(graph->V,0);
    for(int v=0;v<graph->V;v++)
    {
        //checks for NULL ref
        // check if adjNodes are there or not
        if(checkPred(graph,v) && visited[v]!=1 && graph->stat[v]!=0)
        {
            //printf("Deleting %d.\n",v);
            PList *t=new PList(graph->operation[v]);
            t->vertex_no=v;
            t->mobility=graph->mob[v];
            // current node to be deleted
            //goto successor of v delete all ref to v
            AdjListNode *p=graph->su[v].head;
            while(p!=NULL)
            {
                //cout<<p->dest<<"\t"<<v<<"\n";
                delete_Succ(graph,p->dest,v);
                visited[p->dest]=1;
                p=p->next;
            }
            //del all ref of curr
             graph->su[v].head=NULL;
             graph->stat[v]=0;
             /***
                Above removal from list successfull .
                Below adding the removed nodes to PList
            ***/

            char type=graph->operation[v];
            //printf("\nCame  Here 1\n");
            for(int i=0;i<plMain->M;i++)
            {
            // printf("(%c,%c)\t",plMain->op_type[m]);
                if( plMain->op_type[i]==type)
                {
                    // printf("Came  Here 1 (%c,%d) \t",type,i);
                    // type match found int PList
                    /// Insert in corresp. PLMain
                    //PList *tr;

                    if(plMain->direct[i].head==NULL)
                    {
                        plMain->direct[i].head=t;
                        break;
                    }
                    PList *pCrawl=plMain->direct[i].head;
                    while(pCrawl->next)
                    {
                        pCrawl=pCrawl->next;
                    }
                    pCrawl->next=t;

                    //t->next=pCrawl;
                    //plMain->direct[i].head=t;

                    /// Printing
                    //plMain->direct[i].head->toString();
                   // cout<<"\t";
                    /// Printing
                //printf("Successfully Added %d to %d ( %c )\n",v,i,type);
                break;
                }
            }
        }

    }
    /*for(int i=0;i<plMain->M;i++)
    {
        if(plMain->direct[i].head!=NULL)
            reverse_(plMain->direct[i].head->next);
    }
    */

    return SUCCESS;
}
/*int reverse_(PList *head_ref)
{
    PList* prev   = NULL;
    PList* current = head_ref;
    PList* next;
    while (current != NULL)
    {
        next  = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    head_ref = prev;
    return SUCCESS;
}*/
// A utility function to print the adjacency list representation of graph
void printGraph(Graph* graph,int i=0)
{
    int v;
    cout<< "\n\n-------------Graph Printing----------------\n";
if(i==0)
    {
    for (v = 0; v < graph->V; ++v)
    {
        if(graph->stat[v]!=0)
        cout<<v<<": "<< graph->mob[v] <<" "<<graph->operation[v]<<"\n";
    }
    }
else if(i==1)
    {
    for (v = 0; v < graph->V; ++v)
    {
         if(graph->stat[v]!=0)
         {
        AdjListNode* pCrawl = graph->su[v].head;
        printf("\n Adjacency list of vertex %d: head ", v);
        while (pCrawl)
        {
            printf("-> %d", pCrawl->dest);
            pCrawl = pCrawl->next;
        }
        }

    }
    printf("\n");
    printf("..........");
    printf("\n");
    for (v = 0; v < graph->V; ++v)
    {
        if(graph->stat[v]!=0)
         {
        AdjListNode* pCrawl = graph->pre[v].head;
        printf("\n Predecesor list of vertex %d : head ", v);

        while (pCrawl)
        {
            printf("-> %d", pCrawl->dest);
            pCrawl = pCrawl->next;
        }
        }
        //printf("\n");
    }
    }
    cout<< "\n-------------Graph Printing---------------\n\n";
}
void createdot(struct Graph* graph,int V)
{
    FILE *fp;
    char ch;
    fp=fopen("graph.dot","w");
    fprintf(fp, "digraph "" {\n");
    for (int v = 0; v < graph->V; ++v)
    {
        struct AdjListNode* pCrawl = graph->su[v].head;
        while (pCrawl)
        {
            fprintf(fp, "%d -> %d\n",v+1,(pCrawl->dest)+1);
            pCrawl = pCrawl->next;
        }
    }
    fprintf(fp, "}");
    fclose(fp);
}
int PListEmptyCheck(PListMain *p)
{
    int m=p->M;
    for(int i=0;i<m;i++)
    {
        if(p->direct[i].head!=NULL)
            return SUCCESS;
    }
    return FAILURE;
}
int asap_alap(Graph *graph)
{
stack<int> f;
int t;
vector <int> as(MAX_LT,0);
vector <int> al(MAX_LT,0);
for (int v = 0; v < graph->V; ++v)
{
    struct AdjListNode* pCrawl = graph->pre[v].head;
    if(!pCrawl)
    {
        as[v]=1;
        f.push(v);
    }

}
while(!f.empty())
{
    struct AdjListNode* pCrawl = graph->su[f.top()].head;
    t=int(f.top());
    f.pop();
    while (pCrawl)
    {
        if(pCrawl->dest)
        {
            //printf("%d---%d----\n",int(pCrawl->dest),t);
            if(as[int(pCrawl->dest)]==0)
                as[int(pCrawl->dest)]=as[t]+1;
            else
            {
                if(as[int(pCrawl->dest)]<as[t]+1)
                as[int(pCrawl->dest)]=as[t]+1;
            }
            f.push(int(pCrawl->dest));
        }
        pCrawl = pCrawl->next;
    }
        if(f.empty())
            break;
}
//finding asap scheduling
//finding alap scheduling
for (int v = 0; v < graph->V; ++v)
{
    struct AdjListNode* pCrawl = graph->su[v].head;
    if(!pCrawl)
    {
        al[v]=MAX_LT;
        f.push(v);
    }
}
while(!f.empty())
{
    struct AdjListNode* pCrawl = graph->pre[f.top()].head;
    t=int(f.top());
    f.pop();
    while (pCrawl)
    {
        if(pCrawl->dest)
        {
           // printf("%d---%d----\n",int(pCrawl->dest),t);
            if(al[int(pCrawl->dest)]==0)
                al[int(pCrawl->dest)]=al[t]-1;
            else
            {
                if(al[int(pCrawl->dest)]>al[t]-1)
                    al[int(pCrawl->dest)]=al[t]-1;
            }
            f.push(int(pCrawl->dest));
        }
    pCrawl = pCrawl->next;
    }
    if(f.empty())
        break;
}
    int m=MAX_LT;
    for(int i=0;i<graph->V;i++)
    {
            if(al[i]!=0 && m>al[i])
            m=al[i];
     }
     cout<<"\n\n";
    for(int i=0;i<graph->V;i++)
    {
        if(al[i]!=0)
            al[i]=al[i]-m+1;
        al[i]=al[i]+1;
        //printf("----ALAP-----%d %d\n",i,al[i]);
    }
    for(int i=0;i<graph->V;i++)
    {
       // printf("Mobility of -----%d ---- %d\n",i,abs(al[i]-as[i]));
        graph->mob[i]=abs(al[i]-as[i]);
    }
}
/// MAIN CORE
int ListScheduling(Graph *graph,Schedule *sch,PListMain *plMain)
{
    /// m -- No of PList operators
    /// k -- ??

    insert_ready_ops(graph,plMain);

    int CStep=-1;
    while(PListEmptyCheck(plMain))//TRUE (Remains Inside), FALSE (BREAKS OUT)
    {
        CStep+=1;
        for(int k=0;k<(plMain->M);k++)
        {
            PList *temp=plMain->direct[k].head;
            if(temp)
            {
                for(int funit=0;funit<(sch->n_ops);funit++)
                {
                    temp=plMain->direct[k].head;
                    if(temp!=NULL && temp->op==sch->op_arrange[funit])
                    {
                       // printf("Scheduling '%c' %d<%d> at step %d(%d)\n",temp->op,temp->vertex_no,temp->mobility,CStep,funit);
                        PList *tr=new PList(temp->op);
                        tr->mobility=temp->mobility;
                        tr->vertex_no=temp->vertex_no;
                        tr->cStep=CStep;
                        schedule_op(sch,tr,CStep,funit);

                            temp = plMain->direct[k].head->next;
                            plMain->direct[k].head=temp;

                    }
                }

            }
        }

        insert_ready_ops(graph,plMain);
    }

    sch->n_cSteps=CStep+1;
    //sch->toString();
    return SUCCESS;
}
int main()
{
    cout<<"\n";
    string op_arr="";
    /*** Example from notes
        V=11
        E=11
    ***/
    int V=11;
    //int mob[]={0,0,1,2,0,1,0,0,2,2,2};
    char ops[]="******--++<";
    char hw[]="**+-<";
    Graph* graph = createGraph(V);
    addEdge(graph, 0, 4);
    addEdge(graph, 1, 4);
    addEdge(graph, 4, 6);
    addEdge(graph, 6, 7);
    addEdge(graph, 5, 7);
    addEdge(graph, 2, 5);
    addEdge(graph, 3, 8);
    addEdge(graph, 9, 10);

    PListMain *plM=new PListMain(hw);
    asap_alap(graph);
    printGraph(graph,LIST_PRINT);
    alloc_op_mbty(graph,ops,graph->mob);

    //printGraph(graph,OP_PRINT);
    //printGraph(graph,LIST_PRINT);

    Schedule *currS=new Schedule(1,hw);

    ListScheduling(graph,currS,plM);

    currS->toString();
   // currS->toString();

    cout<<"\n";
return 0;
}
