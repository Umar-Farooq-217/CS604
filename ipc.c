#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pipe1[2]; // Pipe 1: Parent writes, Child reads
    int pipe2[2]; // Pipe 2: Child writes, Parent reads
    pid_t pid;

    // Messages to be exchanged (Replace BC123456789 with your actual VUID)
    char parent_msg[] = "How are you? What's your VUID?";
    char child_reply[] = "I am fine. My VUID is BC123456789";
    char buffer[100];

    // Step 1: Create both unnamed pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    // Step 2: Create a child process using fork()
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid > 0) {
        // --- PARENT PROCESS ---
        
        // Close unused ends of both pipes
        close(pipe1[0]); // Parent won't read from pipe1
        close(pipe2[1]); // Parent won't write to pipe2

        // Send message to child through pipe1
        write(pipe1[1], parent_msg, strlen(parent_msg) + 1);
        close(pipe1[1]); // Close write end after sending

        // Wait for the child to process and send a reply
        wait(NULL);

        // Read the reply sent by the child from pipe2
        read(pipe2[0], buffer, sizeof(buffer));
        printf("Parent received: %s\n", buffer);
        close(pipe2[0]); // Close read end after reading

    } else {
        // --- CHILD PROCESS ---
        
        // Close unused ends of both pipes
        close(pipe1[1]); // Child won't write to pipe1
        close(pipe2[0]); // Child won't read from pipe2

        // Read the message sent by the parent from pipe1
        read(pipe1[0], buffer, sizeof(buffer));
        printf("Child received: %s\n", buffer);
        close(pipe1[0]); // Close read end after reading

        // Send reply back to parent through pipe2
        write(pipe2[1], child_reply, strlen(child_reply) + 1);
        close(pipe2[1]); // Close write end after sending
    }

    return 0;
}