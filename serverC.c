#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<winsock2.h>

#define MAX_USER_COUNT 30
#define REQUEST_SIZE 2000
#define RESPONSE_HEADER_SIZE 2000
#define RESPONSE_SIZE 3000
#define HTTP_METHOD_SIZE 20
#define FILE_PATH_SIZE 300
#define FILE_EXTENSION_SIZE 20
#define CONTENT_TYPE_SIZE 30
#define READ_BUFFER_SIZE 10000
#define USERNAME_SIZE 51
#define PASSWORD_SIZE 51
#define FROM_USER_SIZE 51
#define TO_USER_SIZE 51
#define MESSAGE_SIZE 300
#define POST_MESSAGE_SIZE 1000
#define POST_METHOD_SIZE 50

//------------------------------------------------------------------

struct node
{
    char from[FROM_USER_SIZE];
    char to[TO_USER_SIZE];
    char msg[MESSAGE_SIZE];
    struct node *next;
};

struct node* getNewNode(char *from , char *to , char *msg)
{
    struct node* newNode = (struct node*)(malloc(sizeof(struct node)));
    strcpy((*newNode).from , from);
    strcpy((*newNode).to , to);
    strcpy((*newNode).msg , msg);
    (*newNode).next = NULL;

    return newNode;
}

//-------------------------------------------------------------------

int validateLoginUtil(char *userName , char *password)
{
    char tempUserName[USERNAME_SIZE];
    char tempPassword[PASSWORD_SIZE];

    FILE *fptr = fopen("UserData.dat" , "r");

    while(!feof(fptr))
    {
        fgets(tempUserName , USERNAME_SIZE , fptr);
        if(feof(fptr))
        {
            break;
        }
        fgets(tempPassword , PASSWORD_SIZE , fptr);

        tempUserName[strlen(tempUserName) - 1] = '\0';
        tempPassword[strlen(tempPassword) - 1] = '\0';

        if(strcmp(tempUserName , userName) == 0 && strcmp(tempPassword , password) == 0)
        {
            return 1;
        }
    }
    fclose(fptr);
    return 0;
}


void addNewUser(char *userName , char * password)
{
    FILE *fptr = fopen("UserData.dat" , "a");

    char tempUserName[USERNAME_SIZE];
    char tempPassword[PASSWORD_SIZE];

    int len1 = 0 , len2 = 0;    

    strcpy(tempUserName , userName);
    strcpy(tempPassword , password);

    len1 = strlen(tempUserName);
    len2 = strlen(tempPassword);

    tempUserName[len1] = '\n';
    tempUserName[len1 + 1] = '\0';
    tempPassword[len2] = '\n';
    tempPassword[len2 + 1] = '\0';

    fputs(tempUserName , fptr);
    fputs(tempPassword , fptr);

    fclose(fptr);
}

//----------------------------------------------------------

void loadUsers(char **UserNames , struct node **Messages , int *loginFlag , int *userCount)
{
    char tempUserName[USERNAME_SIZE];
    char tempPassword[PASSWORD_SIZE];

    FILE *fptr = fopen("UserData.dat" , "r");

    for(int i = 0 ; i < MAX_USER_COUNT ; i++)
    {
        UserNames[i] = (char*)(malloc(sizeof(char) * USERNAME_SIZE));
        Messages[i] = NULL;
        loginFlag[i] = 0;
    }


    while(!feof(fptr))
    {
        fgets(tempUserName , USERNAME_SIZE , fptr);
        if(feof(fptr))
        {
            break;
        }
        fgets(tempPassword , PASSWORD_SIZE , fptr);

        tempUserName[strlen(tempUserName) - 1] = '\0';
        strcpy(UserNames[*userCount] , tempUserName);
        *userCount += 1;
    }

    for(int i = 0 ; i < *userCount ; i++)
    {
        for(int j = 0 ; j < *userCount ; j++)
        {
            // printf("%d %d\n" , i , j);
            if(i == j)
            {
                continue;
            }
            struct node* newNode = getNewNode(UserNames[j] , UserNames[i] , "");
            if(Messages[i] == NULL)
            {
                Messages[i] = newNode;
            }
            else
            {
                struct node* itr = Messages[i];
                while((*itr).next != NULL)
                {
                    itr = (*itr).next;
                }
                (*itr).next = newNode;
            }
        }
    }

    fclose(fptr);
}

void printLoadedData(char **UserNames , struct node **Messages , int *loginFlag , int *userCount)
{
    for(int i = 0 ; i < *userCount ; i++)
    {
        printf("%s : ", UserNames[i]);
        struct node *itr = Messages[i];
        while(itr != NULL)
        {
            printf("%s,%s,%s|" , (*itr).from , (*itr).to , (*itr).msg);
            itr = (*itr).next;
        }
        printf("\n");
    }
    printf("\n%d\n" , *userCount);
}

int searchUserName(char **UserNames , int userCount , char *userName)
{
    for(int i = 0 ; i < userCount ; i++)
    {
        if(strcmp(UserNames[i] , userName) == 0)
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------

// Handling POST Requests

void validateLogin(char *postMessage , int clientSocket)
{
    //Offset
    while(*postMessage != ',')
    {
        postMessage += 1;
    }
    postMessage += 1;

    char userName[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    int idx = 0;
    while(*postMessage != ',')
    {
        userName[idx] = *postMessage;
        idx += 1;
        postMessage += 1;
    }
    postMessage += 1;
    userName[idx] = '\0';

    idx = 0;
    while(*postMessage != '\0')
    {
        password[idx] = *postMessage;
        idx += 1;
        postMessage += 1;
    }
    password[idx] = '\0';

    int res = validateLoginUtil(userName , password);
    printf("%s %s\n" , userName , password);
    if(res == 1)
    {
        send(clientSocket , "True" , 4 , 0);
    }
    else
    {
        send(clientSocket , "False" , 5 , 0);
    }
}

void validateSignUp(char *postMessage , char **UserNames , struct node **Messages , int *userCount , int *loginFlag , int clientSocket)
{
    //Offset
    while(*postMessage != ',')
    {
        postMessage += 1;
    }
    postMessage += 1;

    char userName[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    
    int idx = 0;
    while(*postMessage != ',')
    {
        userName[idx] = *postMessage;
        idx += 1;
        postMessage += 1;
    }
    userName[idx] = '\0';
    postMessage += 1;

    idx = 0;
    while(*postMessage != '\0')
    {
        password[idx] = *postMessage;
        idx += 1;
        postMessage += 1;
    }
    password[idx] = '\0';

    idx = searchUserName(UserNames , *userCount , userName);

    if(idx != -1)
    {
        send(clientSocket , "False" , 5 , 0);
    }
    else
    {
        strcpy(UserNames[*userCount] , userName);
        *userCount += 1;

        for(int i = 0 ; i < *userCount - 1 ; i++)
        {
            struct node* newNode = getNewNode(userName , UserNames[i] , "");

            if(Messages[i] == NULL)
            {
                Messages[i] = newNode;
            }
            else
            {
                struct node *itr = Messages[i];
                while((*itr).next != NULL)
                {
                    itr = (*itr).next;
                }
                (*itr).next = newNode;
            }
        }

        for(int i = 0 ; i < *userCount - 1 ; i++)
        {
            struct node* newNode = getNewNode(UserNames[i] , userName , "");

            if(Messages[*userCount - 1] == NULL)
            {
                Messages[*userCount - 1] = newNode;
            }
            else
            {
                struct node *itr = Messages[*userCount - 1];
                while((*itr).next != NULL)
                {
                    itr = (*itr).next;
                }
                (*itr).next = newNode;
            }
        }

        addNewUser(userName , password);
        send(clientSocket , "True" , 4 , 0);
    }
}

void sendMessage(char *postMessage , char **UserNames , struct node **Messages , int userCount , int *loginFlag)
{
    char fromUser[USERNAME_SIZE];
    char toUser[USERNAME_SIZE];
    char message[MESSAGE_SIZE];

    //Offset
    while(*postMessage != ',')
    {
        postMessage += 1;
    }
    postMessage += 1;

    int idx = 0;
    while(*postMessage != ',')
    {
        fromUser[idx] = *postMessage;
        idx += 1;
        postMessage += 1;
    }
    fromUser[idx] = '\0';
    postMessage += 1;

    // printf("%s" , fromUser);

    idx = 0;
    while(*postMessage != ',')
    {
        toUser[idx] = *postMessage;
        idx += 1;
        postMessage += 1;
    }
    toUser[idx] = '\0';
    postMessage += 1;

    idx = 0;
    while(*postMessage != '\0')
    {
        message[idx] = *postMessage;
        idx += 1;
        postMessage += 1;
    }
    message[idx] = '\0';

    // printf("%s %s %s\n" , fromUser , toUser , message);

    idx = searchUserName(UserNames , userCount , toUser);

    if(idx == -1)
    {
        printf("No such User Exists!!!!\n");
        return;
    }

    struct node *newNode = getNewNode(fromUser , toUser , message);
    if(Messages[idx] == NULL)
    {
        Messages[idx] = newNode;
    }
    else
    {
        struct node *itr = Messages[idx];
        while((*itr).next != NULL)
        {
            itr = (*itr).next;
        }
        (*itr).next = newNode;
    }
    int u = userCount;

    // FILE *fptr = fopen("MessagesLog.dat" , "a");

    //     // fputs(postMessage , fptr);
    // fputs(postMessage , fptr);
    // fclose(fptr);
    // printLoadedData(UserNames, Messages , loginFlag , &u);
}

void pushMessages(char *postMessage , char **UserNames , struct node **Messages , int userCount , int *loginFlag , int clientSocket)
{
    //Offset
    while(*postMessage != ',')
    {
        postMessage += 1;
    }
    postMessage += 1;

    char userName[USERNAME_SIZE];
    int idx = 0;
    while(*postMessage != '\0')
    {
        userName[idx] = *postMessage;
        idx += 1;
        postMessage += 1;
    }
    userName[idx] = '\0';
    printf("Here : %s\n" , userName);

    idx = searchUserName(UserNames , userCount , userName);
    if(idx == -1)
    {
        printf("\t========== Error : Invalid User asking for Messages ============\n");
        return;
    }
    struct node *itr , *temp;
    itr = Messages[idx];
    while(itr != NULL)
    {
        temp = itr;

        send(clientSocket , (*itr).from , strlen((*itr).from) , 0);
        send(clientSocket , "," , 1 , 0);
        send(clientSocket , (*itr).to , strlen((*itr).to) , 0);
        send(clientSocket , "," , 1 , 0);
        send(clientSocket , (*itr).msg , strlen((*itr).msg) , 0);
        send(clientSocket , "|" , 1 , 0);

        itr = (*itr).next;

        free(temp);
    }
    Messages[idx] = NULL;
}

void logoutHandler(char *postMessage , char **UserNames , struct node **Messages , int userCount)
{
    //Offset
    while(*postMessage != ',')
    {
        postMessage += 1;
    }
    postMessage += 1;

    char userName[USERNAME_SIZE];
    int idx = 0;
    while(*postMessage != '\0')
    {
        userName[idx] = *postMessage;
        idx += 1;
        postMessage += 1;
    }
    userName[idx] = '\0';

    printf("logout : %s\n" , userName);

    idx = searchUserName(UserNames , userCount , userName);

    for(int i = 0 ; i < userCount ; i++)
    {
        if(i == idx)
        {
            continue;
        }
        struct node *newNode = getNewNode(UserNames[i] , UserNames[idx] , "");
        if(Messages[idx] == NULL)
        {
            Messages[idx] = newNode;
        }
        else
        {
            struct node *itr = Messages[idx];
            while((*itr).next != NULL)
            {
                itr = (*itr).next;
            }
            (*itr).next = newNode;
        }
    }

    // printLoadedData(UserNames , Messages , NULL , &userCount);
}

char* getPOSTMessage(char *Request)
{
    //Offset
    while(*Request != '~' || *(Request + 1) != ':')
    {
        Request += 1;
    }
    Request += 2;

    char *postMessage = (char*)(malloc(sizeof(char) * POST_MESSAGE_SIZE));
    int idx = 0;

    while(*Request != '\0')
    {
        postMessage[idx] = *Request;
        idx += 1;
        Request += 1;
    }
    postMessage[idx] = '\0';

    return postMessage;
}

char* getPOSTMethod(char *postMessage)
{
    char *postMethod = (char*)(malloc(sizeof(char) * POST_METHOD_SIZE));
    int idx = 0;

    while(*postMessage != ',' && *postMessage != '\0')
    {
        postMethod[idx] = *postMessage;
        idx += 1;
        postMessage += 1;
    }
    postMethod[idx] = '\0';

    return postMethod;
}

void handlePOSTRequest(char *Request , int clientSocket , char **UserNames , struct node **Messages , int *userCount , int *loginFlag)
{
    char *postMessage = getPOSTMessage(Request);
    char *postMethod = getPOSTMethod(postMessage);
    
    if(strcmp(postMethod , "ValidateLogin") == 0)
    {
        validateLogin(postMessage , clientSocket);
    }
    else if(strcmp(postMethod , "SendMessage") == 0)
    {
        printf("Send Message : %s\n" , postMessage);
        sendMessage(postMessage , UserNames , Messages , *userCount , loginFlag);
    }
    else if(strcmp(postMethod , "ReceiveMessages") == 0)
    {
        pushMessages(postMessage , UserNames , Messages , *userCount , loginFlag , clientSocket);
        printLoadedData(UserNames , Messages , loginFlag , userCount);
    }
    else if(strcmp(postMethod , "ValidateSignUp") == 0)
    {
        // printf("%s" , postMessage);
        // validateSignUp(postMessage , UserNames , Messages , userCount , loginFlag , clientSocket);
        validateSignUp(postMessage , UserNames , Messages , userCount , loginFlag , clientSocket);
    }
    //else if(strcmp(postMethod , "logoutUser") == 0)
    //{
    //    logoutHandler(postMessage , UserNames , Messages , *userCount);
    //}

    free(postMessage);
    free(postMethod);
    return;
}


//---------------------------------------------------------------------


char* getHTTPMethod(char *Request)
{
    char *httpMethod = (char*)(malloc(sizeof(char) * HTTP_METHOD_SIZE));
    int idx = 0;

    while(*Request != ' ')
    {
        httpMethod[idx] = *Request;
        idx += 1;
        Request += 1;
    }
    httpMethod[idx] = '\0';

    return httpMethod;
}

char* getFilePath(char* Request)
{
    //Offset
    while(*Request != ' ')
    {
        Request += 1;
    }
    Request += 2;


    char *filePath = (char*)(malloc(sizeof(char) * FILE_PATH_SIZE));
    int idx = 0 , dotFlag = 0;

    if(*(Request - 1) == '/' && *Request == ' ')
    {
        // strcpy(filePath , "Login/index.html");
        return NULL;
    }



    while(*Request != ' ' || dotFlag != 1)
    {
        if(*Request == '.')
        {
            dotFlag = 1;
        }
        filePath[idx] = *Request;
        idx += 1;
        Request += 1;
    }
    filePath[idx] = '\0';
    
    if(strcmp(filePath , "UserData.dat") == 0)
    {
        strcpy(filePath , "UnauthorizedAccess");
        return filePath;
    }

    return filePath;
}

char* getFileExtension(char* filePath)
{
    //Offset
    while(*filePath != '.')
    {
        filePath += 1;
    }

    char *fileExtension = (char*)(malloc(sizeof(char) * FILE_EXTENSION_SIZE));
    int idx = 0;
    while(*filePath != '\0')
    {
        fileExtension[idx] = *filePath;
        idx += 1;
        filePath += 1;
    }
    fileExtension[idx] = '\0';

    return fileExtension;
}

char* getContentType(char* fileExtension)
{
    char *contentType = (char*)(malloc(sizeof(char) * CONTENT_TYPE_SIZE));
    memset(contentType , 0 , CONTENT_TYPE_SIZE);

    if(strcmp(fileExtension , ".html") == 0)
    {
        strcpy(contentType , "text/html");
    }
    else if(strcmp(fileExtension , ".css") == 0)
    {
        strcpy(contentType , "text/css");
    }
    else if(strcmp(fileExtension , ".js") == 0)
    {
        strcpy(contentType , "text/javascipt");
    }
    else if(strcmp(fileExtension , ".json") == 0)
    {
        strcpy(contentType , "application/json");
    }
    else if(strcmp(fileExtension , ".png") == 0)
    {
        strcpy(contentType , "image/png");
    }
    else if(strcmp(fileExtension , ".jpg") == 0)
    {
        strcpy(contentType , "image/jpeg");
    }
    else if(strcmp(fileExtension , ".jpeg") == 0)
    {
        strcpy(contentType , "image/jpeg");
    }
    else if(strcmp(fileExtension , ".svg") == 0)
    {
        strcpy(contentType , "image/svg+xml");
    }
    else
    {
        strcpy(contentType , "text/html");
    }

    return contentType;
}

void sendFile(char *filePath , int clientSocket)
{
    FILE *fptr = fopen(filePath , "rb");

    if(fptr == NULL)
    {
        printf("\t ========== Error : No File Found ==========\n");
        return;
    }

    int readBytes = 0 , totalReadBytes = 0;
    char ReadBuffer[READ_BUFFER_SIZE];

    while(!feof(fptr))
    {
        readBytes = fread(ReadBuffer , 1 , READ_BUFFER_SIZE - 1 , fptr);
        totalReadBytes += readBytes;

        ReadBuffer[readBytes] = '\0';

        send(clientSocket , ReadBuffer , readBytes , 0);
    }

    printf("Total Read Bytes : %d\n" , totalReadBytes);
    fclose(fptr);
    return;
}


//---------------------------------------------------------------



//------------------------------------------------------------------

int main(int argc , char **argv , char **envp)
{
    char **UserNames = (char**)(malloc(sizeof(char*) * MAX_USER_COUNT));
    struct node** Messages = (struct node**)(malloc(sizeof(struct node*) * MAX_USER_COUNT));
    int loginFlag[MAX_USER_COUNT];
    int userCount = 0;

    loadUsers(UserNames , Messages , loginFlag , &userCount);

    printLoadedData(UserNames , Messages , loginFlag , &userCount);

    char Request[REQUEST_SIZE];
    char ResponseHeader[RESPONSE_HEADER_SIZE];
    // char Response[RESPONSE_SIZE];

    memset(Request , 0 , REQUEST_SIZE);
    memset(ResponseHeader , 0 , RESPONSE_HEADER_SIZE);
    // memset(Response , 0 , RESPONSE_SIZE);

    char *httpMethod = NULL , *filePath = NULL , *fileExtension = NULL , *contentType = NULL;

    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2 , 2) , &wsa) != 0)
    {
        printf("Error Initializing Windows Socket DLL\n");
        return 1;
    }

    // Server Address
    int serverSocket = -1;
    struct sockaddr_in serverAddress;
    memset(&serverAddress , 0 , sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Client Address
    int clientSocket = -1;
    struct sockaddr_in clientAddress;
    memset(&clientAddress , 0 , sizeof(clientAddress));
    int clientAddressLength = sizeof(clientAddress);

    //Creating Server Socket
    serverSocket = socket(AF_INET , SOCK_STREAM , 0);

    //Binding Server Socket
    bind(serverSocket , (struct sockaddr*)(&serverAddress) , sizeof(serverAddress));

    //Listening Server Socket
    listen(serverSocket , 20000);

    //Accepting Requests
    while(1)
    {
        clientSocket = accept(serverSocket , (struct sockaddr*)(&clientAddress) , &clientAddressLength);

        recv(clientSocket , Request , REQUEST_SIZE , 0);

        httpMethod = getHTTPMethod(Request);

        printf("%s\n" , Request);

        if(strcmp(httpMethod , "GET") == 0)
        {
            filePath = getFilePath(Request);

            if(filePath == NULL)
            {
                sprintf(ResponseHeader , "HTTP/1.1 200 OK:\r\n" "Content-Type: text/html; charset=UTF-8\r\n\r\n<!DOCTYPE html><html><head>Starter</head><body><script>let url = `${window.location}`;let ipAddress = ``;let count = 0;for(let itr = 0 ; itr < url.length ; itr++){if(`${url[itr]}`.localeCompare(`/`) == 0){count++;}if(count == 3){break;}ipAddress += url[itr];}window.location = `${ipAddress}/StarterPage/index.html`;</script></body></html>");
                send(clientSocket , ResponseHeader , strlen(ResponseHeader) , 0);
            }
            else if(strcmp(filePath , "UnauthorizedAccess") == 0)
            {
                sprintf(ResponseHeader , "HTTP/1.1 200 OK:\r\n" "Content-Type: text/html; charset=UTF-8\r\n\r\nUnauthorized Access");
                send(clientSocket , ResponseHeader , strlen(ResponseHeader) , 0);
            }
            else
            {   
                fileExtension = getFileExtension(filePath);
                contentType = getContentType(fileExtension);

                sprintf(ResponseHeader , "HTTP/1.1 200 OK:\r\n" "Content-Type: %s; charset=UTF-8\r\n\r\n" , contentType);

                send(clientSocket , ResponseHeader , strlen(ResponseHeader) , 0);

                sendFile(filePath , clientSocket);

                // send(clientSocket , "\r\n\r\n" , 4 , 0);
            }

            free(filePath);
            free(fileExtension);
            free(contentType);
        }
        else if(strcmp(httpMethod , "POST") == 0)
        {
            sprintf(ResponseHeader , "HTTP/1.1 200 OK:\r\n" "Content-Type: text/html; charset=UTF-8\r\n\r\n");
            send(clientSocket , ResponseHeader , strlen(ResponseHeader) , 0);

            handlePOSTRequest(Request , clientSocket , UserNames , Messages , &userCount , loginFlag);
        }

        send(clientSocket , "\r\n\r\n" , 4 , 0);

        free(httpMethod);
        memset(Request , 0 , REQUEST_SIZE);
        memset(ResponseHeader , 0 , RESPONSE_HEADER_SIZE);
        closesocket(clientSocket);
        clientSocket = -1;
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}