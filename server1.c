#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>

#include "customer.h"
#include "employee.h"
#include "manager.h"
#include "admin.h"

#define PORT 8080

void handle_client() {
    setvbuf(stdout, NULL, _IONBF, 0);
    int choice;

    while(1) {
        printf("\n==============================\n");
        printf(" Select Role to Login:\n");
        printf(" 1. Customer\n");
        printf(" 2. Employee\n");
        printf(" 3. Manager\n");
        printf(" 4. Admin\n");
        printf(" 5. Exit\n");
        printf("==============================\n");
        printf("Enter choice: ");
        fflush(stdout);

        if (scanf("%d", &choice) <= 0)
            break;

        switch(choice) {
            case 1: {
                int id = customer_login();
                if(id > 0) customer_menu(id);
                break;
            }
            case 2: {
                int id = employee_login();
                if(id > 0) employee_menu(id);
                break;
            }
            case 3: {
                int id = manager_login();
                if(id > 0) manager_menu(id);
                break;
            }
            case 4: {
                int id = admin_login();
                if(id > 0) admin_menu(id);
                break;
            }
            case 5:
                return; // logout & end connection
            default:
                printf(" Invalid Choice!\n");
        }
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;

    signal(SIGCHLD, SIG_IGN); // prevent zombie children

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 10);

    printf("\n Server Started on Port %d\n", PORT);

    while(1) {
        socklen_t addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);

        if(client_socket < 0) continue;

        pid_t pid = fork();

        if(pid == 0) { 
            // CHILD: handle this one client
            close(server_socket);

            
            dup2(client_socket, STDIN_FILENO);
            dup2(client_socket, STDOUT_FILENO);
            dup2(client_socket, STDERR_FILENO);

            close(client_socket);

            handle_client();  

            exit(0);
        }
        else {
            // PARENT: keep listening
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}
