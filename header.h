#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#define BUFFER_SIZE 1024
#define MAX_NAME_LEN 1024

struct timespec start, finish;

struct Player 
{
    int playerId;
    char name[MAX_NAME_LEN];
    int noPlayedGames;
    int noWonGames;
    int maxPointsInVictory;
    int points;
    double PPA;
};

struct Player *playerList = NULL;  
int playerCount = 0;        
int playerCapacity = 0; 
