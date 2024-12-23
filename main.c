// Rock-Paper-Scissors Game - Base Code for Client-Server Connection
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


#define PORT 8080
#define BUFFER_SIZE 1024
#define MSG_PROMPT "choose rock paper or scissors"
#define MSG_RESULT_WON "win"
#define MSG_RESULT_TIE "tie"
#define MSG_RESULT_LOST "lose"
#define NULL '/0'
#define TIME_OUT 90


// Function to send a message from the server
void server_send(int client_socket, const char *message) {
    if (send(client_socket, message, strlen(message), 0) == -1) {
        perror("Server send failed");
    }
}

// Function to receive a message on the server
int server_receive(int client_socket, char *buffer) {
    // memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("Server receive failed");
    }
    return bytes_received;
}

// Function to send a message from the client
void client_send(int client_socket, const char *message) {
    if (send(client_socket, message, strlen(message), 0) == -1) {
        perror("Client send failed");
    }
}

// Function to receive a message on the client
int client_receive(int client_socket, char *buffer) {
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("Client receive failed");
    }
    return bytes_received;
}

// Function to initialize the server
int start_server() {
    int server_fd, client_socket; // fd = file descriptor
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr); // socklen_t is a length and type of and address datatype
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    server_addr.sin_family = AF_INET; // sin is socet internet
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);  // htons is host to network short, is the translation between internet itegers to "normal" integers (16 bit) 

    // Bind socket to the port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }


    // Start listening for connections
    if (listen(server_fd, 1) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running and waiting for connections on port %d...\n", PORT);

    // Accept client connection
    if ((client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) == -1) { // accept will halt the program untill a client will connect
        perror("Client connection failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected!\n");

    //Communicate with the client TODO: change this a more apropiate communication (separte function)
    
    return client_socket;

    // //TODO: change to its own function
    // close(client_socket);
    // close(server_fd);
}

// Function to initialize the client
int start_client(const char *server_ip) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("created socket \n");
    // Se up server address sttruct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // still setup its just a translation of the human readble ip to machine readble
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("translating ip done\n");
    // Connect to server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection to server failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server!\n");

    return client_fd;
}

int connect_server_clint(const int *is_server, int *comm_socket){
    // set the user mode to server or client
    if (*is_server == 1){
        
        *comm_socket = start_server();
    } else if (*is_server == 0)
    {
        // ask for the server ip 
        char server_ip[20];
        printf("enter the server ip: \n");

        if(scanf("%s", server_ip) != 1){
            perror("server ip fetch error! \n");
            return EXIT_FAILURE;
        }
        printf("attempting to connect to : %s",server_ip);
        *comm_socket = start_client(server_ip);
    }else{
        perror("what are you?");
        return 1;
    }
    
    return 0;
}

void server_game_loop(const int *client_socket){
    char temp_input[50];
    char msg[BUFFER_SIZE];
    char buff[BUFFER_SIZE];

    while (1){
        memset(temp_input,0,sizeof(char) * 50);
        memset(buff,0,BUFFER_SIZE);
        memset(msg,0,BUFFER_SIZE);

        printf(MSG_PROMPT);
        printf("\n");
        //for (char *p = temp_input; *p; ++p) *p = tolower(*p);
        strcpy(msg, MSG_PROMPT);
        server_send(*client_socket,msg);


        //memset(buff,0,BUFFER_SIZE);
        scanf("%s", temp_input);
    
        int no_response = 1;
        for (int i = 0; i < TIME_OUT; i++)
        {
            server_receive(*client_socket,buff);
            if (*buff != NULL)
            {
                printf("client answer recived! \n");
                no_response = 0;
                break;
            }
            sleep(1);
        }
        if (no_response)
        {
            printf("no response \n");
        }
        

        
        if( strcmp(temp_input, "rock") == 0){
            if (strcmp(buff, "rock") == 0){
                printf("you tie\n");
                strcpy(msg,MSG_RESULT_TIE);
            }
            else if (strcmp(buff, "paper") == 0){
                printf("you lose\n");
                strcpy(msg,MSG_RESULT_WON);
            }
            else if (strcmp(buff, "scissors") == 0){
                printf("you won\n");
                strcpy(msg,MSG_RESULT_LOST);
            }
            else {printf("bad client input\n");strcpy(msg,"input_error");}
        }
        else if (strcmp(temp_input, "paper") == 0){
            if (strcmp(buff, "rock") == 0){
                printf("you won\n");
                strcpy(msg,MSG_RESULT_LOST);
                }
            else if (strcmp(buff, "paper") == 0){
                printf("you tie\n");
                strcpy(msg,MSG_RESULT_TIE);
                }
            else if (strcmp(buff, "scissors") == 0){
                printf("you lose\n");
                strcpy(msg,MSG_RESULT_WON);
                }
            else {printf("bad client input\n");strcpy(msg,"input_error");}
        }
        else if (strcmp(temp_input, "scissors") == 0){
            if (strcmp(buff, "rock") == 0){
                printf("you lose\n");
                strcpy(msg,MSG_RESULT_WON);
                }
            else if (strcmp(buff, "paper") == 0){
                printf("you won\n");
                strcpy(msg,MSG_RESULT_LOST);
                }
            else if (strcmp(buff, "scissors") == 0){
                printf("you tie\n");
                strcpy(msg,MSG_RESULT_TIE);
                }
            else {printf("bad client input\n");strcpy(msg,"input_error");}
        }
        else{perror("invalid input\n");} 
          
        server_send(*client_socket,msg);
    }
    

}

// main loop for client is to listen and respose
void client_game_loop(const int *server_socket){
    //char temp_input[50];
    char msg[BUFFER_SIZE];
    char buff[BUFFER_SIZE];
    
    while(1){
        //memset(temp_input,0,sizeof(char) * 50);
        memset(buff,0,BUFFER_SIZE);
        memset(msg,0,BUFFER_SIZE);

        client_receive(*server_socket,buff);
        if (*buff == '/0'){
            continue;
        }else if (strcmp(buff, MSG_PROMPT) == 0){
            printf("please pick rock, paper or scissors \n");
            scanf("%s", msg);
            //for (char *p = msg; *p; ++p) *p = tolower(*p);
            client_send(*server_socket,msg);
        }else if(strcmp(buff, MSG_RESULT_WON) == 0){
            printf("you %s\n",buff);
        }else if(strcmp(buff, MSG_RESULT_LOST) == 0){
            printf("you %s\n",buff);
        }else if(strcmp(buff, MSG_RESULT_TIE) == 0){
            printf("you %s\n",buff);
        }else{
            printf("some unexpected network communnication nothing to worry about :) (i'm scared) \n");
            printf("%s \n",buff);
        }
    }
}

int main(void){
    // urrraaaa this is either the server or client socket 
    int comm_socket;

    // are you a server or a client?
    int is_server;
    printf("enter 1 to be the server \nenter 0 for client: \n");
    if(scanf("%d", &is_server) != 1){
        perror("is_server selection error! 0 \n");
        return EXIT_FAILURE;
    } else if (is_server < 0 || is_server > 1){
        perror("is_server selection error! 1 \n");
        return EXIT_FAILURE;
    }
    printf("You entered: %d\n", is_server);


    //  establish conection between server and client
    connect_server_clint(&is_server, &comm_socket);
    
    //  beging game loop 
    if (is_server)
    {
        server_game_loop(&comm_socket);
    } else if (is_server == 0)
    {
        client_game_loop(&comm_socket);
        /* code */
    }else {
        perror("now how did we got here? 2548742");
        return EXIT_FAILURE;
    }

    
    return EXIT_SUCCESS;
}
