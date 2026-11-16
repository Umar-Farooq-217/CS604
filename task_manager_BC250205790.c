
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int main() {
    char id[50], filename[70], report[500];
    int times[3], result[3];
    int p1[2], p2[2], fd;

    printf("Enter Student ID: ");
    scanf("%49s", id);

    for (int i = 0; i < 3; i++) {
        printf("Enter Task %d Execution Time: ", i + 1);
        scanf("%d", &times[i]);

        if (times[i] <= 0) {
            printf("Enter positive numbers only.\n");
            return 1;
        }
    }

    pipe(p1);
    pipe(p2);

    int pid = fork();

    if (pid == 0) {
        // Child Process

        close(p1[1]);
        close(p2[0]);

        read(p1[0], times, sizeof(times));

        result[0] = times[0] + times[1] + times[2];

        result[1] = 1;
        result[2] = times[0];

        for (int i = 1; i < 3; i++) {
            if (times[i] > result[2]) {
                result[2] = times[i];
                result[1] = i + 1;
            }
        }

        write(p2[1], result, sizeof(result));

        close(p1[0]);
        close(p2[1]);

    } 
    else if (pid > 0) {
        // Parent Process

        close(p1[0]);
        close(p2[1]);

        write(p1[1], times, sizeof(times));

        close(p1[1]);

        read(p2[0], result, sizeof(result));

        close(p2[0]);

        wait(NULL);

        sprintf(report,
            "\n--- Task Processing Report ---\n"
            "Student ID: %s\n"
            "Task 1: %d units\n"
            "Task 2: %d units\n"
            "Task 3: %d units\n"
            "Total Execution Time: %d units\n"
            "Longest Task: Task %d\n"
            "Maximum Execution Time: %d units\n",
            id, times[0], times[1], times[2],
            result[0], result[1], result[2]);

        printf("%s", report);

        sprintf(filename, "%s_report.txt", id);

        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        write(fd, report, strlen(report));

        close(fd);

        printf("Report saved to: %s\n", filename);
    }

    return 0;
}