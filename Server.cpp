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

    if (mkfifo(PIPE, 0666) == -1 && errno != EEXIST) {
        perror("Error creating pipe");
        return 1;
    }
    int pipe_write = open(PIPE, O_WRONLY);
    
    if (pipe_write == -1) {
        perror("Error opening pipe for writing");
        return 1;
    }
    
    write(pipe_write, first_mas, strlen(first_mas));
    close(pipe_write);
    
    cout << "Waiting for the client" << endl;
    bool connection_established = false;

    while (true) {

        int pipe_read = open(PIPE, O_RDONLY);
        if (pipe_read == -1) {
            perror("Error opening pipe for reading");
            return 1;
        }
        char buffer[30];
        int bytesRead = read(pipe_read, buffer, sizeof(buffer)-1);
        //cout << bytesRead<<endl;
        close(pipe_read);

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';

            if (strcmp(buffer, first_mas) == 0) {
                continue;
            }
            else {
                if (!connection_established) {
                    cout << "Connection is established." << endl;
                    connection_established = true;
                }
            }

            if (strcmp(buffer, "ping") == 0) {
                cout << "Get request: " << buffer << endl;
                char response[] = "pong";
                pipe_write = open(PIPE, O_WRONLY);
                write(pipe_write, response, strlen(response));
                close(pipe_write);
                cout << "Send response: " << response << endl;
                sleep(1);
            }
            else {
                cerr << "Unknown request: " << buffer << endl;
                break;
            }
        }
        else if (bytesRead == 0 && connection_established) {
            cout << "Client closed connection." << endl;
            break;
        }
        else {
            perror("Error reading from pipe");
            break;
        }
    }
    unlink(PIPE);
    return 0;
}
