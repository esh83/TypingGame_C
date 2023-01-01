#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
struct UserInfo
{
    char name[110];
    int age;
    char username[110];
    char password[110];
} userLogInfo;
enum
{
    LOGIN_CD = 1,
    REGISTER_CD,
    EXIT_GAME,
};
// FUNCTIONS PROTOTYPE
bool register_page();
bool login_page();
int main()
{
    char buffer[3];
    int selected_item;
    bool break_from_menu = false;
    // FIRST STEP OF THE GAME (LOGIN - REGISTER)
    while (true)
    {
        printf("\nHi :) welcome to the game . select one :\n%d:Login\n%d:Register\n%d:Exit\n", LOGIN_CD, REGISTER_CD, EXIT_GAME);
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
    // SECOND STEP OF THE GAME ()

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

    FILE *filePtr = fopen("userInfo.bin", "rb");
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
    FILE *filePtr = fopen("userInfo.bin", "wb");
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
