#pragma once

// TODO: for linux, DONT FORGET ABOUT CHECKER_SUSPEND_PROCESS

#if !defined(CHECKER_ASCII) && !defined(CHECKER_WIDE)
    #if defined(_UNICODE) || defined(UNICODE)
        #define CHECKER_WIDE
    #else
        #define CHECKER_ASCII
    #endif
#else
    #if defined(CHECKER_ASCII) && defined(CHECKER_WIDE)
        #error You can't define both CHECKER_WIDE and CHECKER_ASCII
    #endif 
#endif

#pragma region Includes
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <Windows.h>
#ifdef CHECKER_SUSPEND_PROCESS
#include <Tlhelp32.h>
#endif
#define _CKRWIN
#endif
#include <sstream>
#include <string>
#ifndef CHECKER_SILENT
#pragma comment(lib, "Winmm")
#endif
#pragma endregion

#ifndef _MSC_VER
#define __FUNCTION__ __func__
#endif

#pragma region Util
#ifdef CHECKER_WIDE
#define _CKRCHAR wchar_t
#define _CKRF1(x) L ## x
#define _CKRF2(x) _CKRF1(x)
#define _CKRF3(x) _CKRF1(#x)
#define _CKRF4(x) _CKRF3(x)
#define _CKRAU(a, u) u
#else
#define _CKRCHAR char
#define _CKRF1(x) x
#define _CKRF2(x) _CKRF1(x)
#define _CKRF3(x) #x
#define _CKRF4(x) _CKRF3(x)
#define _CKRAU(a, u) a
#endif
#pragma endregion

struct CheckFailureInfo
{
#ifndef NDEBUG
    const _CKRCHAR* lineNumber;
    const _CKRCHAR* filePath;
    const _CKRCHAR* functionName;
    const _CKRCHAR* codeFragment;
#endif
    const _CKRCHAR* moduleName;
    const _CKRCHAR* generatedInfo;
    const _CKRCHAR* userInfo;
};
void CheckFailed(CheckFailureInfo info);

#pragma region CHECK
void _CKR_Throw_(CheckFailureInfo info)
{
    CheckFailed(info);
}
#ifdef NDEBUG
#define CHECK_EX(statement, info) if (!(statement)) _CKR_Throw_({0, 0, _CKRF1(info)})
#define CHECK(statement) if (!(statement)) _CKR_Throw_({0, 0, 0})
#else
#define CHECK_EX(statement, info) if (!(statement)) _CKR_Throw_({_CKRF4(__LINE__), _CKRF2(__FILE__), _CKRF2(__FUNCTION__), _CKRF1(#statement), 0, 0, _CKRF1(info)})
#define CHECK(statement) if (!(statement)) _CKR_Throw_({_CKRF4(__LINE__), _CKRF2(__FILE__), _CKRF2(__FUNCTION__), _CKRF1(#statement), 0, 0, 0})
#endif
#pragma endregion
#pragma region WIN_CHECK
#ifdef _CKRWIN
void _CKR_Throw_Windows(CheckFailureInfo info)
{
    static auto trim = [] (_CKRAU(std::string, std::wstring)& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [] (unsigned char ch) { return !std::isspace(ch); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [] (unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    };

    _CKRCHAR* msgBuf = 0;
    DWORD code = GetLastError();
    size_t size = _CKRAU(FormatMessageA, FormatMessageW)(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        0, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (_CKRCHAR*)&msgBuf, 0, 0
        );
    _CKRAU(std::string, std::wstring) ms(msgBuf);
    trim(ms);
    _CKRAU(std::stringstream, std::wstringstream) ss;
    ss << code << _CKRF1(": ") << ms;
    _CKRAU(std::string, std::wstring) s = ss.str();
    info.generatedInfo = s.c_str();

    CheckFailed(info);
    LocalFree(msgBuf);
}

#ifdef NDEBUG
#define WIN_CHECK_EX(statement, info) if (!(statement)) _CKR_Throw_Windows({_CKRF1("Windows.h"), 0, _CKRF1(info)})
#define WIN_CHECK(statement) if (!(statement)) _CKR_Throw_Windows({_CKRF1("Windows.h"), 0, 0})
#else
#define WIN_CHECK_EX(statement, info) if (!(statement)) _CKR_Throw_Windows({_CKRF4(__LINE__), _CKRF2(__FILE__), _CKRF2(__FUNCTION__), _CKRF1(#statement), _CKRF1("Windows.h"), 0, _CKRF1(info)})
#define WIN_CHECK(statement) if (!(statement)) _CKR_Throw_Windows({_CKRF4(__LINE__), _CKRF2(__FILE__), _CKRF2(__FUNCTION__), _CKRF1(#statement), _CKRF1("Windows.h"), 0, 0})
#endif
#endif
#pragma endregion


#ifndef CHECKER_MANUAL

_CKRAU(std::string, std::wstring) GenerateMessage(CheckFailureInfo info, bool colored)
{
    _CKRAU(std::stringstream, std::wstringstream) ss;
    const _CKRCHAR* yellow; const _CKRCHAR* red; const _CKRCHAR* green; const _CKRCHAR* dflt; const _CKRCHAR* impt;
    if (colored) { yellow = _CKRF1("\x1b[33m"); green = _CKRF1("\x1b[32m"); red = _CKRF1("\x1b[31m"); dflt = _CKRF1("\x1b[0m"); impt = _CKRF1("\x1b[31;1m"); }
    else { yellow = _CKRF1(""); green = _CKRF1(""); red = _CKRF1(""); dflt = _CKRF1(""); impt = _CKRF1(""); }
    ss << impt << _CKRF1("Fatal error happened!") << dflt << _CKRF1('\n');
#ifndef NDEBUG
    ss << yellow << _CKRF1("File") << dflt << ": " << info.filePath << _CKRF1('\n');
    ss << yellow << _CKRF1("Line") << dflt << ": " << info.lineNumber << _CKRF1('\n');
    ss << yellow << _CKRF1("Function") << dflt << ": " << info.functionName << _CKRF1('\n');
    ss << yellow << _CKRF1("Code Fragment") << dflt << ": \'" << red << info.codeFragment << dflt << '\'' << _CKRF1('\n');
#endif
    if (info.userInfo != 0) ss << yellow << _CKRF1("Specific Info") << dflt << ": \'" << green << info.userInfo << dflt << '\'' << _CKRF1('\n');
    if (info.moduleName != 0) { ss << yellow << _CKRF1("Module") << dflt << ": " << info.moduleName << _CKRF1('\n'); }
    if (info.generatedInfo != 0) { ss << yellow << _CKRF1("Generated Info") << dflt << ": \'" << green << info.generatedInfo << dflt << '\'' << _CKRF1('\n'); }
    ss << dflt;
    return ss.str();
}

#ifdef _CKRWIN

//TODO: get rid of warning
void CheckFailed(CheckFailureInfo info)
{
#ifdef CHECKER_SUSPEND_PROCESS
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
                    if (thread != NULL)
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
#endif

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

#ifndef CHECKER_SILENT
    PlaySoundW(L"C:\\Windows\\Media\\Windows Error.wav", NULL, SND_FILENAME | SND_ASYNC);
#endif

    _CKRAU(std::string, std::wstring) s;
    bool colored = false;
    if (useConsole)
    {
        DWORD dummy = 0;
        if (GetConsoleMode(hOutput, &dummy) != NULL)
        {
            dummy |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (SetConsoleMode(hOutput, dummy) != NULL)
                colored = true;
        }

        s = GenerateMessage(info, colored);

        SetActiveWindow(GetConsoleWindow());

        if (_CKRAU(WriteConsoleA, WriteConsoleW)(hOutput, s.c_str(), (DWORD)s.length(), &dummy, 0) == 0)
            goto mbox;

        char ch;
        if (_CKRAU(ReadConsoleA, ReadConsoleW)(GetStdHandle(STD_INPUT_HANDLE), &ch, 1, &dummy, 0) == 0)
            goto mbox;

        goto exit;
    }
mbox:
    if (colored)
        s = GenerateMessage(info, false);
    _CKRAU(MessageBoxA, MessageBoxW)(0, s.c_str(), _CKRF1("Error"), 0);
exit:
    ExitProcess(1);
}

#else

void CheckFailed(CheckFailureInfo info)
{
    exit(1);
}
#endif



#endif
