// Task 16: Mini shell (fork + execvp + waitpid)
// Type commands like: ls -l, pwd, echo hello. Type "exit" to quit.
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    std::string line;
    while (true) {
        std::cout << "myshell> " << std::flush;
        if (!std::getline(std::cin, line)) { std::cout << "\n"; break; }   // Ctrl+D
        if (line == "exit") break;

        // tokenize on whitespace
        std::istringstream iss(line);
        std::vector<std::string> words;
        std::string w;
        while (iss >> w) words.push_back(w);
        if (words.empty()) continue;

        // built-in: cd must run in the shell itself, not in a child
        if (words[0] == "cd") {
            const char* dir = words.size() > 1 ? words[1].c_str() : getenv("HOME");
            if (!dir || chdir(dir) != 0) perror("cd");
            continue;
        }

        std::vector<char*> tokens;
        for (auto& s : words) tokens.push_back((char*)s.c_str());
        tokens.push_back(NULL);

        pid_t pid = fork();
        if (pid < 0) { perror("fork"); continue; }
        if (pid == 0) {
            execvp(tokens[0], tokens.data());
            perror("Command failed");
            exit(1);
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                printf("[exit status %d]\n", WEXITSTATUS(status));
            else if (WIFSIGNALED(status))
                printf("[killed by signal %d]\n", WTERMSIG(status));
        }
    }
    return 0;
}
