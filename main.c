#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "def.h"
#include "colorize.h"
#include "helper_windows.h"

struct GameLog
{
    int ID;
    int user_ID;
    int intensity;
    int score;
    time_t date_time;
};
struct UserInfo
{
    int user_ID;
    char name[110];
    int age;
    char username[110];
    char password[110];
};

struct
{
    struct UserInfo personal;
    struct GameLog games[3];
} userLogInfo;

typedef struct WordNode
{
    char word[22];
    struct WordNode *next;
} WordNode;
WordNode *head_words_list = NULL;

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
void insertAtEnd(char *);
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

    int game_ID;
    while (true)
    {
        printf("************************\nHere is your last 3 games , Enter its ID to continue playing it\nEnter 0 to start a new game\nEnter -1 to Exit:\n\n");

        if (userLogInfo.games[0].ID == -1)
        {
            printf("Your game history is empty ! Enter 0 to start a new game\n");
        }
        else
        {
            struct tm *log_time = NULL;
            for (int i = 0; i < 3; i++)
            {
                if (userLogInfo.games[i].ID == -1)
                    continue;
                log_time = localtime(&userLogInfo.games[i].date_time);
                printf("ID : %d --> Intensity : %s | score : %d | last time played : %s", userLogInfo.games[i].ID, userLogInfo.games[i].intensity == INTENSITY_HARD ? "Hard" : userLogInfo.games[i].intensity == INTENSITY_NORMAL ? "Normal"
                                                                                                                                                                                                                                  : "Easy",
                       userLogInfo.games[i].score, asctime(log_time));
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

    FILE *fileUsers = fopen(USERINFO_FILENAME, "rb");
    FILE *fileLogs = fopen(GAMES_LOG_FILENAME, "rb");
    if (fileUsers == NULL)
    {
        printf("\n*User info doesnt exist please try to register first\n");
        return false;
    }
    fseek(fileUsers, 0, SEEK_END);
    int itm_counts = ftell(fileUsers) / sizeof(struct UserInfo);
    rewind(fileUsers);
    int same_pass = 0, same_username = 0;
    for (int i = 0; i < itm_counts; i++)
    {
        if (fread(&user_saved, sizeof(struct UserInfo), 1, fileUsers) != 1)
        {
            printf("error occured while reading file \n");
            exit(1);
        }
        if (strcmp(user_saved.username, user.username) == 0)
        {
            same_username = 1;
        }
        if (strcmp(user_saved.password, user.password) == 0)
        {
            same_pass = 1;
        }
        if (same_pass && same_username)
            break;
        same_pass = same_username = 0;
    }

    fclose(fileUsers);
    if (same_pass && same_username)
    {

        userLogInfo.personal = user_saved;
        userLogInfo.games[0].ID = -1;
        userLogInfo.games[1].ID = -1;
        userLogInfo.games[2].ID = -1;
        if (fileLogs)
        {
            fseek(fileLogs, 0, SEEK_END);
            int logs_num = ftell(fileLogs) / sizeof(struct GameLog);
            struct GameLog current_log;
            int counter = 0;
            fseek(fileLogs, -(sizeof(struct GameLog)), SEEK_END);
            for (int i = 0; i < logs_num; i++)
            {
                fread(&current_log, sizeof(struct GameLog), 1, fileLogs);
                if (current_log.user_ID == user_saved.user_ID)
                {
                    userLogInfo.games[counter++] = current_log;
                }
                if (counter >= 3)
                    break;

                fseek(fileLogs, -2 * (sizeof(struct GameLog)), SEEK_CUR);
            }
            fclose(fileLogs);
        }
    }
    else
    {
        printf("\n*user not found !\n");
        return false;
    }
    return true;
}
// REGISTER HANDLER
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

    FILE *filePtr = fopen(USERINFO_FILENAME, "ab");
    if (filePtr == NULL)
    {
        printf("error eccured during saving info");
        exit(1);
    }
    fseek(filePtr, 0, SEEK_END);
    int items_count = ftell(filePtr) / sizeof(struct UserInfo);
    user.user_ID = 1 + items_count;
    if (fwrite(&user, sizeof(struct UserInfo), 1, filePtr) != 1)
    {
        printf("error eccured during saving info");
        exit(1);
    }
    fclose(filePtr);
    printf("Registered Successfully . Enter any key to go back to menu and login ...\n");
    getch();
    return true;
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
// INSERT AT THE END OF LINK LIST HANDLER
void insertAtEnd(char *word)
{
    WordNode *newNode = malloc(sizeof(WordNode));
    newNode->next = NULL;
    strcpy(newNode->word, word);
    if (head_words_list == NULL)
    {
        head_words_list = newNode;
    }
    else
    {
        WordNode *temp = head_words_list;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
    }
}
// DELETE FROM BEGINING OF LINK LIST HANDLER

// EMPTY LINK LIST HANDLER

// RUN GAME HANDLER
void runGame(int intensity, int game_ID)
{

    system("cls");    // clear console window
    generate_words(); // generate 3 files of hundred words
    time_t time_now = time(NULL);
    FILE *normal_words_file = fopen(NORMAL_WORDS_FILENAME, "r");
    FILE *long_words_file = fopen(LONG_WORDS_FILENAME, "r");
    FILE *hard_words_file = fopen(HARD_WORDS_FILENAME, "r");
    if (!normal_words_file || !long_words_file || !hard_words_file)
    {
        printf("error while reading words from files\n");
        exit(1);
    }
    char buff[22];
    for (int i = 0; i < 10; i++)
    {
        fgets(buff, 22, normal_words_file);
        insertAtEnd(buff);
        printf("%s\n", buff);
        Sleep(1000);
    }
    // handle new game
    if (game_ID == 0)
    {
        FILE *logFile = fopen(GAMES_LOG_FILENAME, "ab");
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
        newLog.user_ID = userLogInfo.personal.user_ID;
        fwrite(&newLog, sizeof(struct GameLog), 1, logFile);
        fclose(logFile);
    }
}
