#include <stdio.h>
#include <windows.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 1024

void execute_command(char* command) {
    PROCESS_INFORMATION processInfo;
    STARTUPINFO startupInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInfo, sizeof(processInfo));

    // CreateProcess requires the command to be mutable (non-const)
    char full_command[MAX_COMMAND_LENGTH + 10];
    snprintf(full_command, sizeof(full_command), "cmd /c %s", command);

    if(!CreateProcess(
        NULL,
        full_command,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &startupInfo,
        &processInfo)
    ) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char prompt[] = "MyTerminal> ";

    while(1) {
        printf("%s", prompt);

        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            continue;
        }

        // Remove trailing newline
        command[strcspn(command, "\n")] = 0;

        // Exit command
        if (strcmp(command, "exit") == 0) {
            break;
        }

        // Change directory
        if (strncmp(command, "cd ", 3) == 0) {
            char* newDir = command + 3;

            while(*newDir == ' ' || *newDir == '\"') {
                newDir++;
            }
            char* end = newDir + strlen(newDir) - 1;
            while(end > newDir && *end == '\"') {
                *end = '\0'; 
                end--;
            }

            if (!SetCurrentDirectory(newDir)) {
                printf("Could not change directory to %s\n", newDir);
            }
            continue;
        }

        // Clear screen
        if (strcmp(command, "cls") == 0) {
            system("cls");
            continue;
        }

        // Run other commands
        execute_command(command);
    }

    return 0;
}
