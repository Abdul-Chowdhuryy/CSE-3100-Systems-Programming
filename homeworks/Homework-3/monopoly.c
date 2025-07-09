#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//There will be m players in an array
typedef struct Player 
{
    int id;
    int loc;
    long balance;
} TPlayer;

//There will be n properties in an array
typedef struct Property
{
    int id;
    int owner_id;
    int rent;
} TProperty;

//TODO
//Implement the following function
//The player p1 needs to pay the player p2 'amount' of dollars
//If p1 has enough balance, the transaction will be successful
//Otherwise, p1 will pay all of their balance to p2, and the transaction
//is not successful 
//return 1 if the transaction is successful
//Otherwise, return 0
int transaction(TPlayer *payer, TPlayer *payee, int amount)
{
    if (payer->balance >= amount) {
        payer->balance -= amount;
        payee->balance += amount;
        return 1;
    } else {
        payee->balance += payer->balance;
        payer->balance = 0;
        return 0;
    }
}

//TODO
//Finish the following function
//If one player cannot pay rent to another player, this function should return 0. 
//The rest of the players will not get chances to play when this happens 
int one_round(int num_players, int num_properties, TPlayer players[], TProperty properties[])
{
    for (int i = 0; i < num_players; i++) {
        int steps = rand() % 6 + 1 + rand() % 6 + 1;
        players[i].loc = (players[i].loc + steps) % num_properties;

        if (players[i].loc == 0) {
            players[i].balance += num_properties;
        }

        int property_id = players[i].loc;
        if (properties[property_id].owner_id == -1) {
            properties[property_id].owner_id = players[i].id;
        } else if (properties[property_id].owner_id != players[i].id) {
            int owner_id = properties[property_id].owner_id;
            int rent = properties[property_id].rent;
            if (!transaction(&players[i], &players[owner_id], rent)) {
                return 0;
            }
        }
    }
    return 1;
}

//used for printing out results
void print_result(int num_players, TPlayer players[])
{
    printf("      id    balance\n");
    for (int i = 0; i < num_players; i++) {
        printf("%8d %8ld\n", players[i].id, players[i].balance);
    }
    long sum = 0;
    long max = 0;
    for (int i = 0; i < num_players; i++) {
        sum += players[i].balance;
        if (players[i].balance > max) max = players[i].balance;
    }
    printf("average: %f max: %ld, max/average = %lf\n", (double)sum/num_players, max, (double)max*num_players/sum); 
}

//max_rounds is needed because the game may never finish
void monopoly(int num_players, int num_properties, TPlayer players[], TProperty properties[], int max_rounds)
{
    srand(12345);
    int rounds = 1;
    while (one_round(num_players, num_properties, players, properties) && rounds < max_rounds) {
        rounds++;
    }

    print_result(num_players, players);
    printf("after %d rounds\n", rounds);
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("Usage: %s m n rounds\n", argv[0]);
        return -1;
    }
    int num_players = atoi(argv[1]);
    int num_properties = atoi(argv[2]);
    int max_rounds = atoi(argv[3]);
    assert(num_properties >= 13);
    assert(num_players >= 1);
    assert(max_rounds >= 1);

    TPlayer players[num_players];
    TProperty properties[num_properties];

    for (int i = 0; i < num_properties; i++) {
        properties[i].id = i;
        properties[i].owner_id = -1;
        properties[i].rent = i + 1;
    }

    for (int j = 0; j < num_players; j++) {
        players[j].id = j;
        players[j].loc = 0;
        players[j].balance = num_properties;
    }
    monopoly(num_players, num_properties, players, properties, max_rounds);
    return 0;    
}
