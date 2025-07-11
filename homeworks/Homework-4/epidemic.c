#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum TYPE {S, I, R};

//TODO: Implement idx
//idx returns an integer to be used for hashing
//this integer should be unique for every x, y pair in your grid
int idx(int x, int y, int k)
{
    return (x + k) * (2 * k + 1) + (y + k);
}

typedef struct Host
{
    int id;
    int x, y;
    int t;
    enum TYPE type;
} THost;

typedef struct node_tag {
    THost host;                
    struct node_tag * next;
} node;

//create a node whose value is a specific host
//return a pointer to the created node
node * create_node(THost host) 
{
    node *new_node = (node *)malloc(sizeof(node));
    new_node->host = host;
    new_node->next = NULL;
    return new_node;
}

//add_first() should add to the beginning of a linked list
//note the type: 'node **head'
//note that it does not return a value 
void add_first(node **head, node *newnode)
{
    newnode->next = *head;
    *head = newnode;
}

//remove the first node from the list
//note the type: 'node **head'
//return a pointer to the removed content
node * remove_first(node **head) 
{
    if (*head == NULL) return NULL;
    node *removed_node = *head;
    *head = (*head)->next;
    return removed_node;
}

//remove all the nodes in the list
//and free all the allocated memory
void remove_all(node **head)
{
    node *current = *head;
    node *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    *head = NULL;
}

//location_match checks whether a linked list contains
//one or more hosts in the same location as 'host'
//return 1 if there is a match, 0 if not
int location_match(node *head, THost host)
{
    while (head != NULL) {
        if (head->host.x == host.x && head->host.y == host.y) {
            return 1;
        }
        head = head->next;
    }
    return 0;
}

//hash function included for your convenience :)
unsigned hash(unsigned a)
{
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}
//summary prints out the proportions of different host types.
//It returns 1 if the number of infected hosts is not 0. 
int summary(THost hosts[], int m)
{   
    int S_n, I_n, R_n;
    
    S_n = I_n = R_n = 0;
    for(int i = 0; i < m; i++)
    {   
        S_n += (hosts[i].type == S);
        I_n += (hosts[i].type == I);
        R_n += (hosts[i].type == R);
    }
    if(I_n == 0)
    {
        printf("    S        I        R\n");
        printf("%lf %lf %lf\n", (double)S_n/(S_n + I_n + R_n), 
        (double)I_n/(S_n + I_n + R_n), (double)R_n/(S_n + I_n + R_n));
    }
    return I_n > 0;
}

// one_round 
int one_round(THost *hosts, int m, node *p_arr[], int n_arr, int k, int T)
{
    //S -> I and I -> R
    for(int i = 0; i < m; i++)
    {
        if(hosts[i].type == S) //Note the use of enumerator S
        {
            int index = hash(idx(hosts[i].x, hosts[i].y, k)) % n_arr;
            if(location_match(p_arr[index], hosts[i]))
            {
                hosts[i].type = I;
                hosts[i].t = 0;
            }
        }
        else if(hosts[i].type == I)
        {
            hosts[i].t++;
            if(hosts[i].t >= T) {
                hosts[i].type = R;
            }
        }
    }

    //reset all linked lists
    for(int i = 0; i < n_arr; i++) {
        remove_all(&p_arr[i]);
    }

    for(int i = 0; i < m; i++)
    {
        int r = rand() % 4;
        //0: up, 1: right, 2: down, 3: left
        switch(r)
        {
            case 0: hosts[i].y++; break;
            case 1: hosts[i].x++; break;
            case 2: hosts[i].y--; break;
            case 3: hosts[i].x--; break;
        }

        //build linked list for I hosts
        if(hosts[i].type == I)
        {
            node *r = create_node(hosts[i]);
            int index = hash(idx(hosts[i].x, hosts[i].y, k)) % n_arr;
            add_first(&(p_arr[index]), r);
        }
    }

    return summary(hosts, m);
}

int main(int argc, char *argv[])
{
    if(argc != 5)
    {
        printf("Usage: %s k m T N\n", argv);
        return 0;
    }

    int k = atoi(argv);
    int m = atoi(argv);
    int T = atoi(argv);
    int N = atoi(argv);

    assert(k >= 0 && k <= 1000);
    assert(m >= 1 && m <= 100000);
    assert(T >= 1);
    assert(N > 0 && N <= 100000);
    srand(12345);

    //initialize hosts
    THost hosts[m];

    hosts.id = 0;
    hosts.x = 0;
    hosts.y = 0;
    hosts.t = 0;
    hosts.type = I;

    for(int i = 1; i < m; i ++)
    {
        hosts[i].id = i;
        hosts[i].x = rand() % (2*k + 1) - k;
        hosts[i].y = rand() % (2*k + 1) - k;
        hosts[i].t = 0;
        hosts[i].type = S;        
    }

    //initialize linked lists
    node *p_arr[N];

    for(int i = 0; i < N; i++)
    {
        p_arr[i] = NULL;
    }
    node *r = create_node(hosts);
    int index = hash(idx(hosts.x, hosts.y, k)) % N;
    add_first(&(p_arr[index]), r);

    //simulation
    while(one_round(hosts, m, p_arr, N, k, T));

    return 0;
}
