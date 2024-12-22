#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define PIPE "/home/smykovvd/Pr/pipe"

using namespace std;

int main() {
    char first_mas[] = "Waiting for the client";
    char buffer[30];
    bool connection_established = false, warn_serv = true;

    
    
    while (true) {

        int pipe_read = open(PIPE, O_RDONLY);
        if (pipe_read == -1) {
            perror("Server is not available");
            return 1;
        }
       
        int bytesRead = read(pipe_read, buffer, sizeof(buffer) - 1);
        close(pipe_read);
        char request[] = "ping";
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0'; 

            if (strcmp(buffer, first_mas) == 0) {
                if (!connection_established) {
                    cout << "Connection is established." << endl;
                    connection_established = true;
                    warn_serv = false;

                    int pipe_write = open(PIPE, O_WRONLY);
                    write(pipe_write, request, strlen(request));
                    cout << "Sent first request: " << request << endl;
                    close(pipe_write);
                    sleep(1);
                    continue;
                }
            }
            else {
                if (warn_serv) {
                    cerr<<"Server is not available";
                    warn_serv = false;
                    return 1;
                }
                
            }

            if (strcmp(buffer, "pong") == 0) {
                cout << "Got answer: "<< buffer << endl;
            }
            else {
                cerr << "Unknown answer: " << buffer << endl;
                break;
            }

            if (connection_established) {
                int pipe_write = open(PIPE, O_WRONLY);
                write(pipe_write, request, strlen(request) + 1);
                cout << "Sent request: " << request << endl;
                close(pipe_write);
                sleep(1);
            }
        }
        else if (bytesRead == 0 && connection_established) {
            cout << "Server closed the connection." << endl;
            break;
        }
        else {
            perror("Error reading pipe");
            break;
        }
    }
    return 0;
}