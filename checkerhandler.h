#pragma once
#ifndef _CHECKER_DEFAULT_HANDLER_
#define _CHECKER_DEFAULT_HANDLER_

#include <Windows.h>
#include <Tlhelp32.h>
#include <sstream>
#include <type_traits>

#ifndef _CHECKER_
#error "Include checker.h before this header"
#endif

template<class R, class E>
static std::string _CKR_GENFAILMSG(CheckFailInfo<R, E> info, bool colored)
{
    std::stringstream ss;


    const char* aqua = "", * red = "", * dflt = "";
    if (colored) { aqua = "\x1b[36m"; red = "\x1b[31m"; dflt = "\x1b[0m"; }
    ss << red << "Fatal error happened!" << dflt << '\n';

    if (info.file != 0) ss << red << "File" << dflt << ": " << aqua << info.file << '\n';
    if (info.function != 0) ss << red << "Function" << dflt << ": " << aqua << info.function << '\n';
    if (info.line != 0) ss << red << "Line" << dflt << ": " << aqua << info.line << '\n';
    if (info.code != 0) ss << red << "Code" << dflt << ": " << aqua << info.code << '\n';
    if (info.returned != 0) ss << red << "Return" << dflt << ": " << aqua << *info.returned << '\n';
    if (info.info != 0) ss << red << "System Info" << dflt << ": " << aqua << info.info << '\n';
    if (info.extra != 0) ss << "There's extra\n";

    ss << dflt;

    return ss.str();
}

#ifdef _CKR_WIN
static void _CKR_SUSPRCS()
{
    {
        DWORD currentProcessId = GetCurrentProcessId();
        DWORD currentThreadId = GetCurrentThreadId();
        HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (h != INVALID_HANDLE_VALUE)
        {
            THREADENTRY32 te;
            te.dwSize = sizeof(te);
            if (Thread32First(h, &te))
            {
                do
                {
                    if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
                    {
                        if (te.th32OwnerProcessID != currentProcessId)
                            continue;
                        if (te.th32ThreadID == currentThreadId)
                            continue;
                        HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
                        if (thread != 0)
                        {
                            SuspendThread(thread);
                            CloseHandle(thread);
                        }
                    }
                    te.dwSize = sizeof(te);
                } while (Thread32Next(h, &te));
            }
            CloseHandle(h);
        }
    }
}
template<class R, class E>
void CheckFail(CheckFailInfo<R, E> info)
{
    _CKR_SUSPRCS();

    const char* c; for (c = info.file + strlen(info.file) - 1; (c > info.file) && (*c != '\\') && (*c != '/'); --c);
    info.file = c + 1;

    bool useConsole = false;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOutput == INVALID_HANDLE_VALUE || hOutput == 0)
    {
        if (AttachConsole(ATTACH_PARENT_PROCESS) != 0 || AllocConsole() != 0)
        {
            hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
            if ((hOutput != INVALID_HANDLE_VALUE) && (hOutput != 0))
                useConsole = true;
        }
    }
    else
        useConsole = true;
    if (useConsole)
    {
        UINT cp = GetACP();
        if ((SetConsoleCP(cp) == 0) || (SetConsoleOutputCP(cp) == 0))
            useConsole = false;
    }

    //PlaySoundA("C:\\Windows\\Media\\Windows Critical Stop.wav", NULL, SND_FILENAME | SND_ASYNC);

    std::string s;
    bool colored = false;
    if (useConsole)
    {
        SetConsoleTitleA("Fatal Error");

        DWORD dummy = 0;
        if (GetConsoleMode(hOutput, &dummy) != NULL)
        {
            dummy |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (SetConsoleMode(hOutput, dummy) != NULL)
                colored = true;
        }

        s = _CKR_GENFAILMSG(info, colored);
        
        auto cw = GetConsoleWindow();

        ShowWindow(cw, SW_SHOW);
        SetActiveWindow(cw);

        if (!IsWindowVisible(cw))
            goto mbox;

        if (WriteConsoleA(hOutput, s.c_str(), (DWORD)s.length(), &dummy, 0) == 0)
            goto mbox;

        char ch;
        if (ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), &ch, 1, &dummy, 0) == 0)
            goto mbox;

        goto exit;
    }
mbox:
    if (colored)
        s = _CKR_GENFAILMSG(info, false);
    MessageBoxA(0, s.c_str(), "Error", 0);
exit:
    ExitProcess(1);
}
#endif
#endif
