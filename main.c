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

typedef struct WordNode
{
    char word[22];
    int wave_num;
    int kind;
    bool isAmbigiuos;
    struct WordNode *next;
} WordNode;

// BLOBAL VARIABLES;
HANDLE thread_id;
WordNode *head_words_list = NULL;
int words_list_count = 0;
int score = 0;
int wrong_indexes[22], wrong_counts;
int should_type_index = 0;
bool in_proccess;
bool gameOver = false;
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
enum Words_Kind
{
    WORD_NORMAL = 1,
    WORD_LONG,
    WORD_HARD,
};
struct
{
    struct UserInfo personal;
    struct GameLog games[3];
} userLogInfo;

struct
{
    int intensity;
    int game_ID;
} curr_game_info;

// FUNCTIONS PROTOTYPE
void my_callback_on_key_arrival(char c);
bool register_page();
bool login_page();
void generate_words();
void runGame();
void insertAtEnd(char *, int, int, bool);
void deleteFromBeg();
void emptyList();
void create_waves();
void printGame();
void finishGame(bool);
bool isExistInWrongs(int);
int main()
{
    setcolor(15);
    srand(time(NULL));
    char buffer[5];
    int selected_item;
    bool break_from_menu = false;
    // FIRST STEP OF THE GAME (LOGIN - REGISTER)
    while (true)
    {
        setcolor(6);
        printf("\n### Hi welcome to the game select one ### :\n");
        setcolor(15);
        printf("%d:Login\n%d:Register\n%d:Exit\n", LOGIN_CD, REGISTER_CD, EXIT_GAME);
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
        setcolor(6);
        printf("#########################################\nHI %s Here is your last 3 games , Enter its ID to continue playing it\nEnter 0 to start a new game\nEnter -1 to Exit:\n\n", userLogInfo.personal.name);
        setcolor(15);
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
        curr_game_info.game_ID = game_ID;
        curr_game_info.intensity = game_inetsity;
        runGame();
    }
    else
    {

        curr_game_info.game_ID = game_ID;
        if (game_ID == userLogInfo.games[0].ID)
            curr_game_info.intensity = userLogInfo.games[0].intensity;
        else if (game_ID == userLogInfo.games[1].ID)
            curr_game_info.intensity = userLogInfo.games[1].intensity;
        else if (game_ID == userLogInfo.games[2].ID)
            curr_game_info.intensity = userLogInfo.games[2].intensity;
        else
        {
            printf("invalid ID\n");
            exit(1);
        }
        runGame();
    }
    WaitForSingleObject(thread_id, INFINITE);
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

    printf("Enter Your Password:\n");
    char c;
    int cnt = 0;
    while ((c = getch()) != 13)
    {
        if (cnt > 100)
            break;
        if (c == 8)
        {
            if (cnt <= 0)
                continue;
            printf("\b \b");
            cnt--;
        }
        else
        {
            buffer[cnt++] = c;
            printf("*");
        }
    }
    printf("\n");
    buffer[cnt] = '\0';
    strcpy(user.password, buffer);

    FILE *fileUsers = fopen(USERINFO_FILENAME, "rb");
    FILE *fileLogs = fopen(GAMES_LOG_FILENAME, "rb");
    if (fileUsers == NULL)
    {
        setcolor(4);
        printf("\nUser info doesnt exist please try to register first\n");
        setcolor(15);
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
        setcolor(4);
        printf("\nuser not found !\n");
        setcolor(15);
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

    printf("Enter Your Password:\n");
    char c;
    int cnt = 0;
    while ((c = getch()) != 13)
    {
        if (cnt > 100)
            break;
        if (c == 8)
        {
            if (cnt <= 0)
                continue;
            printf("\b \b");
            cnt--;
        }
        else
        {
            buffer[cnt++] = c;
            printf("*");
        }
    }
    printf("\n");
    buffer[cnt] = '\0';
    strcpy(user.password, buffer);

    FILE *filePtrForRead = fopen(USERINFO_FILENAME, "rb");
    struct UserInfo temp_user;
    if (filePtrForRead)
    {
        while (!feof(filePtrForRead))
        {
            fread(&temp_user, sizeof(struct UserInfo), 1, filePtrForRead);
            if (strcmp(temp_user.username, user.username) == 0)
            {
                setcolor(4);
                printf("this username has already used ! try again :\n");
                setcolor(15);
                return false;
            }
        }
    }
    fclose(filePtrForRead);
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
    setcolor(2);
    printf("Registered Successfully . Enter any key to go back to menu and login ...\n");
    setcolor(15);
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
    // generate 1000 random words for normal words
    for (int i = 0; i < 1000; i++)
    {
        word_len = rand() % 7 + 3;
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
    // generate 1000 random words for long words
    for (int i = 0; i < 1000; i++)
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
    // generate 1000 random words for hard words
    char special_chars[] = "_@$^&!";
    char alphabet[32];
    for (int k = 0; k < 26; k++)
    {
        alphabet[k] = (char)('a' + k);
    }
    for (int k = 0, j = 26; k < 6; k++, j++)
        alphabet[j] = special_chars[k];
    for (int i = 0; i < 1000; i++)
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
void insertAtEnd(char *word, int kind, int wave_num, bool isAmbigious)
{
    if (words_list_count >= 24)
    {
        finishGame(false);
    }
    WordNode *newNode = malloc(sizeof(WordNode));
    newNode->next = NULL;
    newNode->kind = kind;
    newNode->isAmbigiuos = isAmbigious;
    newNode->wave_num = wave_num;
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
    words_list_count++;
}
// DELETE FROM BEGINING OF LINK LIST HANDLER
void deleteFromBeg()
{
    if (head_words_list == NULL)
        return;
    WordNode *temp = head_words_list;
    head_words_list = temp->next;
    free(temp);
    words_list_count--;
}
// EMPTY LINK LIST HANDLER
void emptyList()
{
    WordNode *temp = head_words_list;
    WordNode *temp2 = NULL;
    while (temp != NULL)
    {
        temp2 = temp;
        temp = temp->next;
        free(temp2);
    }
}

// PRINT GAME HANDLER
void printGame()
{
    if (in_proccess)
        Sleep(100);
    in_proccess = true;
    system("cls"); // clear console window

    // printing borders
    setcolor(2);
    gotoxy(1, 1);
    printf("###########################");
    for (int i = 2; i <= 25; i++)
    {
        gotoxy(1, i);
        printf("#");
    }
    for (int i = 2; i <= 25; i++)
    {
        gotoxy(27, i);
        printf("#");
    }
    gotoxy(1, 26);
    printf("###########################");
    gotoxy(6, 28);
    setcolor(9);
    printf("YOUR SCORE : %d", score);
    setcolor(2);
    for (int i = 27; i <= 29; i++)
    {
        gotoxy(1, i);
        printf("#");
    }
    for (int i = 27; i <= 29; i++)
    {
        gotoxy(27, i);
        printf("#");
    }
    gotoxy(1, 30);
    printf("###########################");
    setcolor(15);

    // printing words
    WordNode *temp = head_words_list;
    int counter = 0;
    while (temp != NULL)
    {
        gotoxy(4, 1 + words_list_count - counter);
        if (counter > 0 && temp->isAmbigiuos)
        {
            printf("****************");
        }
        else if (counter == 0)
        {
            int len = strlen(temp->word);
            int i;
            bool isWrong = false;
            for (i = 0; i < should_type_index; i++)
            {
                if (isExistInWrongs(i))
                    isWrong = true;
                else
                    isWrong = false;
                if (isWrong)
                {
                    setcolor(6);
                    printf("%c", temp->word[i]);
                }
                else
                {
                    setcolor(4);

                    printf("%c", temp->word[i]);
                }
            }
            setcolor(15);
            for (i = should_type_index; i < len; i++)
            {
                printf("%c", temp->word[i]);
            }
        }
        else
        {
            printf("%s", temp->word);
        }
        temp = temp->next;
        counter++;
    }
    gotoxy(0, 31);
    in_proccess = false;
}
// WAVES HANDLER
void create_waves()
{
    double wave_time, reduction;
    int wave_num = 0;
    switch (curr_game_info.intensity)
    {
    case INTENSITY_HARD:
        wave_time = HARD_INITIAL_WAVE;
        reduction = HARD_REDUCTION;
        break;
    case INTENSITY_NORMAL:
        wave_time = NORMAL_INITIAL_WAVE;
        reduction = NORMAL_REDUCTION;

        break;
    case INTERNSITY_EASY:
        wave_time = EASY_INITIAL_WAVE;
        reduction = EASY_REDUCTION;
        break;
    }
    FILE *normal_words_file = fopen(NORMAL_WORDS_FILENAME, "r");
    FILE *long_words_file = fopen(LONG_WORDS_FILENAME, "r");
    FILE *hard_words_file = fopen(HARD_WORDS_FILENAME, "r");
    if (!normal_words_file || !long_words_file || !hard_words_file)
    {
        printf("error while reading words from files\n");
        exit(1);
    }
    char buff[22];
    int kind;
    bool isAmbig;
    printGame();
    while (wave_time > WAVE_MIN)
    {
        wave_num++;
        double wait_time = wave_time / WAVE_SIZE;
        if (wave_num == 1)
        {
            for (int i = 0; i < WAVE_SIZE; i++)
            {
                fgets(buff, 22, normal_words_file);
                buff[strlen(buff) - 1] = '\0';
                kind = WORD_NORMAL;
                isAmbig = false;
                insertAtEnd(buff, kind, wave_num, isAmbig);
                printGame();
                Sleep(wait_time * 1000);
            }
        }
        else if (wave_num == 2)
        {
            int x1, x2;
            x1 = rand() % (WAVE_SIZE / 2) + WAVE_SIZE / 4; // long
            x2 = WAVE_SIZE - x1;                           // normal

            for (int i = 0; i < x1; i++)
            {
                fgets(buff, 22, long_words_file);
                buff[strlen(buff) - 1] = '\0';
                kind = WORD_LONG;

                insertAtEnd(buff, kind, wave_num, false);
                printGame();
                Sleep(wait_time * 1000);
            }
            for (int i = 0; i < x2; i++)
            {
                fgets(buff, 22, normal_words_file);
                buff[strlen(buff) - 1] = '\0';
                kind = WORD_NORMAL;
                insertAtEnd(buff, kind, wave_num, false);
                printGame();
                Sleep(wait_time * 1000);
            }
        }
        else
        {
            int x1, x2, x3;
            x1 = rand() % (WAVE_SIZE / 2) + 1; // hard
            x2 = rand() % (WAVE_SIZE / 2);     // long
            x3 = WAVE_SIZE - x1 + x2;          // normal
            int randomAmb;
            for (int i = 0; i < x1; i++)
            {
                fgets(buff, 22, hard_words_file);
                buff[strlen(buff) - 1] = '\0';
                kind = WORD_HARD;
                randomAmb = rand() % 5;
                isAmbig = randomAmb == 3 ? true : false;
                insertAtEnd(buff, kind, wave_num, isAmbig);
                printGame();
                Sleep(wait_time * 1000);
            }
            for (int i = 0; i < x2; i++)
            {
                fgets(buff, 22, long_words_file);
                buff[strlen(buff) - 1] = '\0';
                kind = WORD_LONG;
                randomAmb = rand() % 5;
                isAmbig = randomAmb == 3 ? true : false;
                insertAtEnd(buff, kind, wave_num, isAmbig);
                printGame();
                Sleep(wait_time * 1000);
            }
            for (int i = 0; i < x3; i++)
            {
                fgets(buff, 22, normal_words_file);
                buff[strlen(buff) - 1] = '\0';
                kind = WORD_NORMAL;
                randomAmb = rand() % 5;
                isAmbig = randomAmb == 3 ? true : false;
                insertAtEnd(buff, kind, wave_num, isAmbig);
                printGame();
                Sleep(wait_time * 1000);
            }
        }
        wave_time *= reduction;
    }
}
// EXIST IN WRONG INDEXES
bool isExistInWrongs(int x)
{
    for (int i = 0; i < wrong_counts; i++)
    {
        if (wrong_indexes[i] == x)
            return true;
    }
    return false;
}
// FINISH GAME HANDLER
void finishGame(bool win)
{
    time_t time_now = time(NULL);

    // handle new game logs
    if (curr_game_info.game_ID == 0)
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
        newLog.intensity = curr_game_info.intensity;
        newLog.score = score;
        newLog.user_ID = userLogInfo.personal.user_ID;
        fwrite(&newLog, sizeof(struct GameLog), 1, logFile);
        fclose(logFile);
    }
    else
    {
        // update old log
        FILE *logFile = fopen(GAMES_LOG_FILENAME, "rb");
        char tempfileName[] = "temp_log.bin";
        FILE *logFileTemp = fopen(tempfileName, "wb");
        if (!logFile || !logFileTemp)
        {
            printf("error while opening log files");
            exit(1);
        }
        struct GameLog currentLog;
        fseek(logFile, 0, SEEK_END);
        int items_count = ftell(logFile) / sizeof(struct GameLog);
        rewind(logFile);
        for (int i = 0; i < items_count; i++)
        {
            fread(&currentLog, sizeof(struct GameLog), 1, logFile);
            if (currentLog.ID == curr_game_info.game_ID)
            {
                currentLog.date_time = time_now;
                currentLog.score = score;
                fwrite(&currentLog, sizeof(struct GameLog), 1, logFileTemp);
            }
            else
            {
                fwrite(&currentLog, sizeof(struct GameLog), 1, logFileTemp);
            }
        }
        fclose(logFile);
        fclose(logFileTemp);
        remove(GAMES_LOG_FILENAME);
        rename(tempfileName, GAMES_LOG_FILENAME);
    }
    gotoxy(40, 10);
    setcolor(6);
    printf("_____________________________________________");
    for (int i = 11; i <= 16; i++)
    {
        gotoxy(39, i);
        printf("|");
    }
    for (int i = 11; i <= 16; i++)
    {
        gotoxy(85, i);
        printf("|");
    }
    gotoxy(40, 16);
    printf("_____________________________________________");
    if (win)
    {
        setcolor(10);
        gotoxy(45, 13);
        printf("Wow ! wondeful typing speed");
    }
    else
    {
        WordNode *temp = head_words_list;
        if (temp == NULL)
        {
            printf("eror");
            exit(1);
        }
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        setcolor(4);
        gotoxy(41, 13);
        printf("Oops! you lose in wave %d in %s intensity", temp->wave_num, curr_game_info.intensity == INTENSITY_HARD ? "Hard" : curr_game_info.intensity == INTERNSITY_EASY ? "Easy"
                                                                                                                                                                              : "Normal");
    }
    setcolor(15);
    gotoxy(0, 31);
    gameOver = true;
    Sleep(5000);
    emptyList(head_words_list);
    exit(1);
}

// RUN GAME HANDLER
void runGame()
{

    generate_words(); // generate 3 files of hundred words
    system("cls");
    gotoxy(0, 1);
    setcolor(9);
    printf("GAME GUIDE :\n1 - every word that you enter all of its letters correctly you will earn its score\n2 - when words reach end of the box the game is over\n3 - every letter that you enter wrong will give you a negetive point\n4 - its possibe to use backspace and correct a letter\n5 - use Esc button to exit the game\n");
    setcolor(15);
    printf("Enter any key to continue to the game ...\n");
    getch();
    thread_id = start_listening(my_callback_on_key_arrival);
    create_waves();
    Sleep(LAST_STOP * 1000);
    gameOver = true;
    if (words_list_count == 0)
    {
        finishGame(true);
    }
    else
    {
        finishGame(false);
    }
}

// HANDLE ENTERED CHARACTER
void my_callback_on_key_arrival(char c)
{
    if (gameOver || !head_words_list)
        return;
    char lock_letter = head_words_list->word[should_type_index];
    int len = strlen(head_words_list->word);
    // escape
    if (c == 27)
        exit(1);
    // backspace
    if (c == 8)
    {
        if (should_type_index <= 0)
            return;

        if (wrong_indexes[wrong_counts - 1] == should_type_index - 1)
        {
            wrong_counts--;
            should_type_index--;
        }
        else
        {
            should_type_index--;
        }
        printGame();
        return;
    }
    if (c == lock_letter)
    {
        should_type_index++;

        printGame();
    }
    else
    {
        score--;
        wrong_indexes[wrong_counts++] = should_type_index++;
        printGame();
        Beep(750, 100);
    }

    if (should_type_index == len)
    {
        if (wrong_counts == 0)
        {
            switch (head_words_list->kind)
            {
            case WORD_HARD:
                score += head_words_list->isAmbigiuos ? 4 : 3;
                break;
            case WORD_LONG:
                score += head_words_list->isAmbigiuos ? 3 : 2;
                break;
            case WORD_NORMAL:
                score += head_words_list->isAmbigiuos ? 2 : 1;
                break;
            }
        }
        should_type_index = 0;
        wrong_counts = 0;
        deleteFromBeg();
        printGame();
    }
}
