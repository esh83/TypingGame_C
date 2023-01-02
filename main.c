#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "def.h"

struct UserInfo
{
    char name[110];
    int age;
    char username[110];
    char password[110];
} userLogInfo;
struct GameLog
{
    int ID;
    int intensity;
    int score;
    time_t date_time;
};
enum Main_Menu_items
{
    LOGIN_CD = 1,
    REGISTER_CD,
    EXIT_GAME,
};
enum Game_Intensity
{
    INTENSITY_HARD = 3,
    INTENSITY_NORMAL = 2,
    INTERNSITY_EASY = 1,
};

// FUNCTIONS PROTOTYPE
bool register_page();
bool login_page();
void generate_words();
void runGame(int, int);
int main()
{
    srand(time(NULL));
    char buffer[5];
    int selected_item;
    bool break_from_menu = false;
    // FIRST STEP OF THE GAME (LOGIN - REGISTER)
    while (true)
    {
        printf("\n**** Hi welcome to the game select one **** :\n%d:Login\n%d:Register\n%d:Exit\n", LOGIN_CD, REGISTER_CD, EXIT_GAME);
        fgets(buffer, 3, stdin);
        fflush(stdin);
        if (strlen(buffer) > 2)
        {
            printf("invalid input ! please enter again :\n");
            continue;
        }
        if (sscanf(buffer, "%d", &selected_item) != 1)
        {
            printf("invalid input ! please enter again :\n");

            continue;
        }
        if (selected_item != 1 && selected_item != 2 && selected_item != 3)
        {
            printf("invalid input ! please enter again :\n");

            continue;
        }
        switch (selected_item)
        {
        case REGISTER_CD:
            register_page();
            break;
        case LOGIN_CD:
            if (login_page())
                break_from_menu = true;
            break;
        case EXIT_GAME:
            exit(1);
            break;
        default:
            exit(1);
        }
        if (break_from_menu)
            break;
    }
    // SECOND STEP OF THE GAME (START A NEW GAME OR COUNTINUE OLD ONE)
    FILE *gameLogFilePtr = fopen(GAMES_LOG_FILENAME, "rb");
    int game_ID;
    while (true)
    {
        printf("** Here is your last 3 games , enter its ID to continue playing it or enter 0 to start a new game , enter -1 to Exit **:\n\n");
        if (gameLogFilePtr == NULL)
        {
            printf("your game history is empty ! enter 0 to start a new game\n");
        }
        else
        {
            struct GameLog log;
            fseek(gameLogFilePtr, 0, SEEK_END);
            int itm_counts = ftell(gameLogFilePtr) / sizeof(struct GameLog);
            if (itm_counts == 0)
                exit(1);
            rewind(gameLogFilePtr);
            for (int i = 1; i <= 3; i++)
            {
                if (fseek(gameLogFilePtr, (itm_counts - i) * sizeof(struct GameLog), SEEK_SET) != 0)
                    break;
                if (feof(gameLogFilePtr))
                    break;
                fread(&log, sizeof(struct GameLog), 1, gameLogFilePtr);
                struct tm *log_time = localtime(&log.date_time);
                printf("ID : %d --> Intensity : %s -- score : %d -- last time played : %s\n", log.ID, log.intensity == INTENSITY_HARD ? "Hard" : log.intensity == INTENSITY_NORMAL ? "Normal"
                                                                                                                                                                                   : "Easy",
                       log.score, asctime(log_time));
            }
        }

        fgets(buffer, 10, stdin);
        fflush(stdin);
        if (sscanf(buffer, "%d", &selected_item) != 1)
        {
            printf("invalid input ! please enter again :\n");

            continue;
        }
        if (selected_item == -1)
            exit(1);
        else
            game_ID = selected_item;
        break;
    }
    fclose(gameLogFilePtr);

    // THIRD_1 STEP OF THE GAME (START A NEW GAME)
    int game_inetsity;
    if (game_ID == 0)
    {
        while (true)
        {
            printf("Choose game intensity:\n1:Easy\n2:Normal\n3:Hard\n");

            fgets(buffer, 3, stdin);
            fflush(stdin);
            if (strlen(buffer) > 2)
            {
                printf("invalid input ! please enter again :\n");
                continue;
            }
            if (sscanf(buffer, "%d", &selected_item) != 1)
            {
                printf("invalid input ! please enter again :\n");

                continue;
            }
            if (selected_item != 1 && selected_item != 2 && selected_item != 3)
            {
                printf("invalid input ! please enter again :\n");

                continue;
            }
            game_inetsity = selected_item;
            break;
        }
        runGame(game_inetsity, game_ID);
    }

    return 0;
}
// LOGIN HANDLER
bool login_page()
{
    struct UserInfo user;
    struct UserInfo user_saved;
    char buffer[110];
    while (true)
    {
        printf("Enter Your Username:\n");
        fgets(buffer, 105, stdin);
        fflush(stdin);
        if (strlen(buffer) > 100)
        {
            printf("your input should be less than 100 character :( enter again :\n");
            continue;
        }
        buffer[strlen(buffer) - 1] = '\0';
        strcpy(user.username, buffer);
        break;
    }
    while (true)
    {
        printf("Enter Your Password:\n");
        fgets(buffer, 105, stdin);
        fflush(stdin);
        if (strlen(buffer) > 100)
        {
            printf("your input should be less than 100 character :( enter again :\n");
            continue;
        }
        buffer[strlen(buffer) - 1] = '\0';
        strcpy(user.password, buffer);
        break;
    }

    FILE *filePtr = fopen(USERINFO_FILENAME, "rb");
    if (filePtr == NULL)
    {
        printf("user info doesnt exist please try to register first\n");
        return false;
    }
    if (fread(&user_saved, sizeof(struct UserInfo), 1, filePtr) != 1)
    {
        printf("error occured while reading file \n");
        exit(1);
    }
    if (strcmp(user_saved.username, user.username) != 0)
    {
        printf("Username is wrong try again\n");
        return false;
    }
    if (strcmp(user_saved.password, user.password) != 0)
    {
        printf("Password is wrong try again\n");
        return false;
    }
    fclose(filePtr);
    userLogInfo.age = user_saved.age;
    strcpy(userLogInfo.name, user_saved.name);
    strcpy(userLogInfo.username, user_saved.username);
    strcpy(userLogInfo.password, user_saved.password);
    return true;
}
// RESGISTER HANDLER
bool register_page()
{
    struct UserInfo user;
    char buffer[110];
    while (true)
    {
        printf("Enter Name:\n");
        fgets(buffer, 105, stdin);
        fflush(stdin);
        if (strlen(buffer) > 100)
        {
            printf("your input should be less than 100 character :( enter again :\n");
            continue;
        }
        buffer[strlen(buffer) - 1] = '\0';
        strcpy(user.name, buffer);
        break;
    }
    while (true)
    {
        int age;
        printf("Enter Your Age:\n");
        fgets(buffer, 5, stdin);
        fflush(stdin);
        if (strlen(buffer) > 5)
        {
            printf("your input should be less than 5 character :( enter again :\n");
            continue;
        }
        if (sscanf(buffer, "%d", &age) != 1)
        {
            printf("invalid input ! please enter a number :( enter again :\n");
            continue;
        }
        user.age = age;
        break;
    }
    while (true)
    {
        printf("Enter Your Username:\n");
        fgets(buffer, 105, stdin);
        fflush(stdin);
        if (strlen(buffer) > 100)
        {
            printf("your input should be less than 100 character :( enter again :\n");
            continue;
        }
        buffer[strlen(buffer) - 1] = '\0';
        strcpy(user.username, buffer);
        break;
    }
    while (true)
    {
        printf("Enter Your Password:\n");
        fgets(buffer, 105, stdin);
        fflush(stdin);
        if (strlen(buffer) > 100)
        {
            printf("your input should be less than 100 character :( enter again :\n");
            continue;
        }
        buffer[strlen(buffer) - 1] = '\0';
        strcpy(user.password, buffer);
        break;
    }
    FILE *filePtr = fopen(USERINFO_FILENAME, "wb");
    if (filePtr == NULL)
    {
        printf("error eccured during saving info");
        exit(1);
    }
    if (fwrite(&user, sizeof(struct UserInfo), 1, filePtr) != 1)
    {
        printf("error eccured during saving info");
        exit(1);
    }
    fclose(filePtr);
    printf("Registered Successfully . Enter -1 to go back to menu and login:\n");
    while (true)
    {
        int entered;
        fgets(buffer, 4, stdin);
        fflush(stdin);
        if (strlen(buffer) > 3)
        {
            printf("invalid input ! enter again :\n");
            continue;
        }
        if (sscanf(buffer, "%d", &entered) != 1)
        {
            printf("invalid input ! please enter a number :( enter again :\n");
            continue;
        }
        if (entered != -1)
        {
            printf("invalid input ! please enter -1 :( enter again :\n");
            continue;
        }

        return true;
    }
}

// GENERATE WORDS HANDLER

void generate_words()
{
    FILE *file_normal_words = fopen(NORMAL_WORDS_FILENAME, "w");
    FILE *file_long_words = fopen(LONG_WORDS_FILENAME, "w");
    FILE *file_hard_words = fopen(HARD_WORDS_FILENAME, "w");
    if (!file_hard_words || !file_long_words || !file_hard_words)
    {
        printf("an error occured while generating words !\n");
        exit(1);
    }
    int word_len;
    char rand_letter;
    char buff[22];
    // generate 100 random words for normal words
    for (int i = 0; i < 100; i++)
    {
        word_len = rand() % 8 + 3;
        for (int j = 0; j < word_len; j++)
        {
            rand_letter = (char)(rand() % 26 + 'a');
            buff[j] = rand_letter;
        }
        buff[word_len] = '\n';
        buff[word_len + 1] = '\0';
        fputs(buff, file_normal_words);
    }
    fclose(file_normal_words);
    // generate 100 random words for long words
    for (int i = 0; i < 100; i++)
    {
        word_len = rand() % 13 + 8;
        for (int j = 0; j < word_len; j++)
        {
            rand_letter = (char)(rand() % 26 + 'a');
            buff[j] = rand_letter;
        }
        buff[word_len] = '\n';
        buff[word_len + 1] = '\0';
        fputs(buff, file_long_words);
    }
    fclose(file_long_words);
    // generate 100 random words for hard words
    char special_chars[] = "_@$^&!";
    char alphabet[32];
    for (int k = 0; k < 26; k++)
    {
        alphabet[k] = (char)('a' + k);
    }
    for (int k = 0, j = 26; k < 6; k++, j++)
        alphabet[j] = special_chars[k];
    for (int i = 0; i < 100; i++)
    {
        word_len = rand() % 18 + 3;
        for (int j = 0; j < word_len; j++)
        {
            rand_letter = alphabet[rand() % 32];
            buff[j] = rand_letter;
        }
        buff[word_len] = '\n';
        buff[word_len + 1] = '\0';
        fputs(buff, file_hard_words);
    }
    fclose(file_hard_words);
}

// RUN GAME HANDLER
void runGame(int intensity, int game_ID)
{
    generate_words();
    time_t time_now = time(NULL);
    // handle new game
    if (game_ID == 0)
    {
        /*FILE *logFile = fopen("gamesLog.bin", "ab");
        if (logFile == NULL)
        {
            printf("error while opening log files");
            exit(1);
        }
        struct GameLog newLog;
        newLog.date_time = time_now;
        fseek(logFile, 0, SEEK_END);
        int items_count = ftell(logFile) / sizeof(struct GameLog);
        newLog.ID = 1 + items_count;
        newLog.intensity = intensity;
        newLog.score = 4;
        fwrite(&newLog, sizeof(struct GameLog), 1, logFile);
        fclose(logFile);*/
    }
}
