#include "asmp.hpp"

#include <string>
#include <iostream>
#include <Exception.hpp>
#include <io/file.hpp>

#if defined(_WIN32)
#include <windows.h>
#include <string>
#include <iostream>

bool preprocess(std::istream* input, std::ostream* output)
{
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;

    SECURITY_ATTRIBUTES saAttr; 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    if (!CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0))
        throw Exception::InternalError("StdIn CreatePipe failed");

    if (!SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
        throw Exception::InternalError("StdIn SetHandleInformation failed");

    if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0))
        throw Exception::InternalError("StdOut CreatePipe failed");

    if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
        throw Exception::InternalError("StdOut SetHandleInformation failed");

    PROCESS_INFORMATION piProcInfo; 
    STARTUPINFOA siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
    siStartInfo.cb = sizeof(STARTUPINFOA);
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.hStdInput = hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    std::string asmpPath = getExecutableDir() + "\\asmp.exe";
    std::string cmdLine = "\"" + asmpPath + "\" - -o -";

    BOOL success = CreateProcessA(
        NULL,
        cmdLine.data(),
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &siStartInfo,
        &piProcInfo);

    if (!success)
        throw Exception::InternalError("CreateProcess failed");

    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(hChildStd_OUT_Wr);

    std::string line;
    DWORD written;
    while (std::getline(*input, line))
    {
        line += "\n";
        if (!WriteFile(hChildStd_IN_Wr, line.data(), (DWORD)line.size(), &written, NULL))
            throw Exception::InternalError("WriteFile failed");
    }
    CloseHandle(hChildStd_IN_Wr);

    char buffer[4096];
    DWORD readBytes;
    while (ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer), &readBytes, NULL) && readBytes > 0)
    {
        output->write(buffer, readBytes);
    }
    CloseHandle(hChildStd_OUT_Rd);

    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    DWORD exitCode;
    if (!GetExitCodeProcess(piProcInfo.hProcess, &exitCode))
        throw Exception::InternalError("GetExitCodeProcess failed");

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    return exitCode == 0;
}


#elif defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#include <sys/wait.h>

bool preprocess(std::istream* input, std::ostream* output)
{
    int inPipe[2];
    int outPipe[2];

    if (pipe(inPipe) == -1 || pipe(outPipe) == -1)
        throw Exception::InternalError("pipe() failed");

    pid_t pid = fork();
    if (pid == -1)
        throw Exception::InternalError("fork() failed");

    if (pid == 0)
    {
        // Child process
        close(inPipe[1]);
        close(outPipe[0]);

        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);
        dup2(STDOUT_FILENO, STDERR_FILENO);

        close(inPipe[0]);
        close(outPipe[1]);

        std::string asmpPath = getExecutableDir() + "/asmp";
        const char* argv[] = { asmpPath.c_str(), "-", "-o", "-", nullptr };

        execvp(argv[0], const_cast<char* const*>(argv));
        perror("execvp");
        _exit(1);
    }
    else
    {
        // Parent process
        close(inPipe[0]);
        close(outPipe[1]);

        std::string line;
        while (std::getline(*input, line))
        {
            line += '\n';
            if (write(inPipe[1], line.data(), line.size()) == -1)
                throw Exception::InternalError("write() failed");
        }
        close(inPipe[1]);

        char buffer[4096];
        ssize_t count;
        while ((count = read(outPipe[0], buffer, sizeof(buffer))) > 0)
            output->write(buffer, count);

        close(outPipe[0]);

        int status;
        if (waitpid(pid, &status, 0) == -1)
            throw Exception::InternalError("waitpid() failed");

        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}


#else
    #error "Unsupported OS"

#endif