#include "header.h"


//Define signature for callback
typedef void (*topLoggingCallback)(struct Player);

typedef struct _loggingFactory {
    topLoggingCallback topGamesWonClbk;
    topLoggingCallback topPointClbk;
    topLoggingCallback topPPAClbk;
} loggingFactory;

loggingFactory logFactory;

// Callback functions
void topGamesWon(struct Player topPlayer) 
{
    printf("New Top Performer in Games Won: %s with %d wins\n", topPlayer.name, topPlayer.noWonGames);
}

void topPoints(struct Player topPlayer) 
{
    printf("New Top Performer in Points: %s with %d points\n", topPlayer.name, topPlayer.points);
}

void topPPA(struct Player topPlayer) 
{
    printf("New Top Performer in PPA: %s with %.2f PPA\n", topPlayer.name, topPlayer.PPA);
}

//Function that uses callbacks to get top performers
void findTopPerformers() 
{
    struct Player *currTopGamesWon = NULL, *currTopPoints = NULL, *currTopPPA = NULL;

    for (int i = 0; i < playerCount; i++) 
    {
        struct Player *currentPlayer = &playerList[i];
        //top games won
        if (currTopGamesWon == NULL || currentPlayer->noWonGames > currTopGamesWon->noWonGames) 
        {
            currTopGamesWon = currentPlayer;
            logFactory.topGamesWonClbk(*currTopGamesWon);
        }

        //top points
        if (currTopPoints == NULL || currentPlayer->points > currTopPoints->points) 
        {
            currTopPoints = currentPlayer;
            logFactory.topPointClbk(*currTopPoints);
        }

        //top PPA
        if (currTopPPA == NULL || currentPlayer->PPA > currTopPPA->PPA)
        {
            currTopPPA = currentPlayer;
            logFactory.topPPAClbk(*currTopPPA); //callback
        }
    }
}

void start_timer() 
{
    clock_gettime(CLOCK_MONOTONIC, &start);
}

void stop_timer(const char *functionName) 
{
    clock_gettime(CLOCK_MONOTONIC, &finish);
    double elapsed = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("\nExecution time for %s: %lf seconds\n", functionName, elapsed);
}

// Function to calculate the sum of all sets in a winning game of tennis
int calculateWinnerPoints(const char *scoreStr) 
{
    int player1Total = 0, player2Total = 0;
    char scoreCopy[BUFFER_SIZE];

    if (scoreStr == NULL) 
    {
        return 0; 
    }
    strncpy(scoreCopy, scoreStr, sizeof(scoreCopy));
    scoreCopy[BUFFER_SIZE - 1] = '\0'; 
    char *token = strtok(scoreCopy, " ");

    while (token != NULL) 
    {
        int player1Points, player2Points;
        sscanf(token, "%d-%d", &player1Points, &player2Points);

        player1Total += player1Points;
        player2Total += player2Points;   

        token = strtok(NULL, " ");
    }
    return (player1Total > player2Total) ? player1Total : player2Total;
}

//Function to find best scoring player in a single tennis match. Returns a player.
struct Player maxPointsInVictoryPlayer()
{
    int max = INT_MIN, playerIdx = 0;

    for(int i = 0; i < playerCapacity; i++)
    {
        if(max < playerList[i].maxPointsInVictory)
        {
            max = playerList[i].maxPointsInVictory;
            playerIdx = i;
        }
    }
    return playerList[playerIdx];
}

// Function to calculate PPA for each player
void calculatePPA() 
{
    for(int i = 0; i < playerCount; i++)
    {
        // Avoid division by zero
        if (playerList[i].noPlayedGames == 0) 
        {
            playerList[i].PPA = 0.0; 
        } 
        else 
        {
            playerList[i].PPA = (double)(playerList[i].points + playerList[i].noWonGames + playerList[i].maxPointsInVictory) / playerList[i].noPlayedGames;
        }
    }
}

// Function to find player index by player ID
int findPlayerIndexById(const int playerId) 
{
    for (int i = 0; i < playerCount; i++) 
    {
        if (playerList[i].playerId == playerId) 
        {
            return i;
        }
    }
    return -1;
}

// Function to increase player capacity
void resizeList(struct Player **list)
{
    playerCapacity = (playerCapacity == 0) ? 10 : playerCapacity * 2;
    struct Player *newList = realloc(*list, playerCapacity * sizeof(struct Player));

    if (newList == NULL)
    {
        fprintf(stderr, "Error: Allocation failed at capacity %d\n", playerCapacity);
        // Free previously allocated memory
        free(*list);
        exit(EXIT_FAILURE);
    }

    *list = newList;

    printf("Allocation successful. New capacity: %d\n", playerCapacity);
}

//Function to add a player to playerList 
void addPlayer(int playerId, char *name, bool wonGame, int pointsInGame)
{
    // Check if we need to resize the playerList
    if (playerCount == playerCapacity) 
        resizeList(&playerList);
    
    // Add new player
    if (name != NULL) 
    {
        strncpy(playerList[playerCount].name, name, MAX_NAME_LEN - 1);
        playerList[playerCount].name[MAX_NAME_LEN - 1] = '\0';
    } 
    else 
        return;

    playerList[playerCount].playerId = playerId;
    playerList[playerCount].noPlayedGames = 1;
    playerList[playerCount].noWonGames = wonGame ? 1 : 0;
    playerList[playerCount].maxPointsInVictory = wonGame ? pointsInGame : 0; 
    playerList[playerCount].points = -1; 
    playerList[playerCount].PPA = -1.0;
    playerCount++;
}

//Function to update a player to playerList 
void updatePlayer(int playerId, char *name, bool wonGame, int pointsInGame, int index)
{
    playerList[index].noPlayedGames++;
    if (wonGame) 
    {
        playerList[index].noWonGames++;
        if (playerList[index].maxPointsInVictory < pointsInGame)
            playerList[index].maxPointsInVictory = pointsInGame;  
    }
}

//Function to add or update stats of a player
void addOrUpdatePlayer(int playerId, char *name, bool wonGame, int pointsInGame) 
{
    int index = findPlayerIndexById(playerId);
    
    // Player not found
    if (index == -1) 
        addPlayer(playerId, name, wonGame, pointsInGame);
    // Player found, update info
    else
        updatePlayer(playerId, name, wonGame, pointsInGame, index);
    
}

// Function to calculate and print each players stats
void printPlayersData() 
{
    printf("Player Data:\n");
    printf("Name\t\tPlayer ID\tPlayed Games\tWon Games\tMax points in a victory\t\tPoints\tPPA\n");
    for (int i = 0; i < playerCount; i++) 
    {
        struct Player p = playerList[i];

        //sleep(1);
        printf("%s\t%d\t\t%d\t\t%d\t\t%d\t\t\t\t%d\t%.2f\n", p.name, p.playerId, p.noPlayedGames, p.noWonGames, p.maxPointsInVictory, p.points,p.PPA);
    }
}

//Function to process a line of a tennis doubles match
void processDoublesData(char *buffer)
{
    int currentColumn = 0;
    int winner1_id, winner2_id, loser1_id, loser2_id, pointsWinner;
    char *winner1_name = NULL, *winner2_name = NULL, *loser1_name = NULL, *loser2_name = NULL;
    buffer[strcspn(buffer, "\n")] = '\0';

    // Tokenize line 
    char *token = NULL;
    while ((token = strsep(&buffer, ",")) != NULL) 
    {

        //winners ids
        if (currentColumn == 7) 
            winner1_id = atoi(token);
        else if (currentColumn == 8) 
            winner2_id = atoi(token);
        //losers ids
        else if (currentColumn == 11) 
            loser1_id = atoi(token);
        else if (currentColumn == 12) 
            loser2_id = atoi(token);
        //score
        else if(currentColumn == 15)
            pointsWinner = calculateWinnerPoints(token);
        //winners names
        else if (currentColumn == 18) 
            winner1_name = token;
        else if (currentColumn == 23) 
            winner2_name = token;
        //losers names
        else if (currentColumn == 28) 
            loser1_name = token;
        else if (currentColumn == 33) 
            loser2_name = token;

        currentColumn++;
    }

    //Add (or update) winners to list 
    addOrUpdatePlayer(winner1_id, winner1_name, true, pointsWinner);
    addOrUpdatePlayer(winner2_id, winner2_name, true, pointsWinner);

    //Add (or update) losers to list 
    //Set pointsInGame to -1 to not overwrite default value 0
    addOrUpdatePlayer(loser1_id, loser1_name, false, -1);
    addOrUpdatePlayer(loser2_id, loser2_name, false, -1);
}

//Function to process a line of a tennis singles match
void processSinglesData(char *buffer)
{
    //delete \n
    buffer[strcspn(buffer, "\n")] = '\0';

    int winnerId, loserId;
    char *winnerName = NULL;
    char *loserName = NULL;
    char *score = NULL;
    char *token = NULL;

    int currentColumn = 0;

    // Tokenize line 
    while ((token = strsep(&buffer, ",")) != NULL) 
    {
        // empty token
        if (*token == '\0') 
        {
            currentColumn++;
            continue; // Skip empty tokens
        }
        //winner stats
        if (currentColumn == 7) 
            winnerId = atoi(token);
        else if (currentColumn == 10) 
            winnerName = token;
        //loser stats
        else if (currentColumn == 15) 
            loserId = atoi(token);
        else if (currentColumn == 18) 
            loserName = token;
        //score
        else if (currentColumn == 23) 
            score = token;

        currentColumn++;
    }
    
    //Add (or update) winner to list 
    int pointsWinner = calculateWinnerPoints(score);
    addOrUpdatePlayer(winnerId, winnerName, true, pointsWinner);
    
    //Add (or update) loser to list  
    //Set pointsInGame to -1 to not overwrite default value 0
    addOrUpdatePlayer(loserId, loserName, false, -1);
}

//Function to process a line of a tennis player ranking
void processRankingData(char *buffer)
{
    int currentColumn = 0;
    //delete \n
    buffer[strcspn(buffer, "\n")] = '\0';

    int playerId, points;
    // Tokenize line 
    char *token = NULL;
   // printf("\nPath: %s\n", path);
    //printf("\nProcessing line: %s\n", buffer);
    while ((token = strsep(&buffer, ",")) != NULL) 
    {
       // printf("Item%d: %s\n", currentColumn, token);

        if (currentColumn == 2) 
            playerId = atoi(token);
        else if (currentColumn == 3) 
            points = atoi(token);
        
        currentColumn++;
    }

    int index = findPlayerIndexById(playerId);
    if (index != -1)
    {
        playerList[index].points = points;
    }
}

//Function to decompose a file in lines
int analyzeFileData(const char *filePath) 
{
    FILE *file = fopen(filePath, "r");
    if (file == NULL) 
    {
        printf("Error in opening file: %s\n", filePath);
        return EXIT_FAILURE;
    }
    // Check if the file name contains "doubles"
    bool isDoubles = strstr(filePath, "doubles") != NULL;

    //Check if file name contains "rankings"
    bool isRanking = strstr(filePath, "rankings") != NULL;

    bool isFirstLine = true;

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, file) != NULL) 
    {
        // Skip header
        if(isFirstLine)
        {
            isFirstLine = false;
            continue;
        }
        // Process each line according to match type
        if (isDoubles)
            processDoublesData(buffer);
        else if(isRanking)
            processRankingData(buffer);
        else
           processSinglesData(buffer);
    }

    fclose(file);
    return EXIT_SUCCESS;
}

//Function to create the full path given a path and a file name 
char* createFullPath(const char *path, char *entryName)
{
    // Calculate the size required for the full path
    size_t pathLength = strlen(path);
    size_t entryNameLength = strlen(entryName);
    
    // Allocate memory for the full path
    char *fullPath = malloc((pathLength + entryNameLength + 2) * sizeof(char)); // +2 for "/" and null terminator
    
    if (fullPath == NULL) 
	{
        perror("Error in allocating path memory!");
        return NULL;
    }

    // Construct the full path
    sprintf(fullPath, "%s/%s", path, entryName);
    
    return fullPath;
}

//Function to traverse each file of a given directory and process its data
int traverseDirectory(const char *path) 
{
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL) 
	{
        printf("Error in opening directory!");
        return EXIT_FAILURE;
    }

    while ((entry = readdir(dir)) != NULL) 
	{
        // Skip current(.), parent directories(..) and .DS_Store(Mac OS)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".DS_Store") == 0 )
		{
            continue;
        }
        
		char *fullPath = createFullPath(path, entry->d_name); 
        
		analyzeFileData(fullPath);
        free(fullPath);
    }

    closedir(dir);
	return EXIT_SUCCESS;
}

// Wrapper Function that calls all the functions that produce data.
void produceData(const char *directoryPath)
{
    if (traverseDirectory(directoryPath) == EXIT_FAILURE) 
    {
        printf("Error: Failed to traverse directory %s\n", directoryPath);
        exit(EXIT_FAILURE);
    }
}

// Wrapper Function that calls all the functions that consume data.
void consumeData()
{
    calculatePPA();

    struct Player topPlayer = maxPointsInVictoryPlayer();
    printf("Top player in terms of max points in a single victory:\n");
    printf("Name: %s, Player ID: %d, Max Points in Victory: %d\n", 
           topPlayer.name, topPlayer.playerId, topPlayer.maxPointsInVictory);

    printPlayersData();

    printf("\nTop Performers Report:\n");
    printf("=====================\n");
    findTopPerformers();

}

int main(int argc, char *argv[]) 
{
    logFactory.topGamesWonClbk = topGamesWon;
    logFactory.topPointClbk = topPoints;
    logFactory.topPPAClbk = topPPA;

    start_timer();
    produceData("");//enter tennis_atp file path
    consumeData();
    stop_timer("main");
    free(playerList);

    return EXIT_SUCCESS;
}