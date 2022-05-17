#pragma once
#ifndef _CHECKER_
#define _CHECKER_

#include <cstdlib>

template <class R, class E>
struct CheckFailInfo
{
    const char* file;
    const char* function;
    int line;
    const char* code;
    const char* info;
    R* returned;
    E* extra;
};

#pragma region Util

#if defined(_WIN32) || defined(__CYGWIN__) || defined(_WIN64)
#define _CKR_WIN
#elif defined(__linux__)
#define _CKR_LNX
#error Linux is not supported yet
#elif defined(__APPLE__) && defined(__MACH__)
#define _CKR_MAC
#error MacOS is not supported yet
#else
#error Unknown & unsupported OS
#endif

#ifdef _MSC_VER
#define _CKR_FUN __FUNCTION__
#else
#define _CKR_FUN __func__
#endif

#define _CKR_STR(s) #s

template <class R, class E>
constexpr inline static CheckFailInfo<R, E> _CKR_CFI(const char* file, const char* function, int line, const char* code, const char* info, R returned, E extra)
{
    return CheckFailInfo<R, E>{file, function, line, code, info, &returned, &extra};
}
template <class R>
constexpr inline static CheckFailInfo<R, int> _CKR_CFI(const char* file, const char* function, int line, const char* code, const char* info, R returned)
{
    return CheckFailInfo<R, int>{file, function, line, code, info, &returned, 0};
}

#pragma endregion

template <class R, class E>
inline static void CheckFail(CheckFailInfo<R, E> info);
 
#pragma region CHECK
//CHECK_EX(value, condition, [extra])
#define CHECK_EX(val, con, ...) { auto r = (val); if (!(r con)) CheckFail(_CKR_CFI(__FILE__, _CKR_FUN, __LINE__, _CKR_STR(CHECK_EX(## val ##\,\  ## con ##\,\  ## ...##)), 0, r, __VA_ARGS__)); }
//CHECK(value, [extra])
#define CHECK(val, ...) { if (!(val)) CheckFail(_CKR_CFI(__FILE__, _CKR_FUN, __LINE__, _CKR_STR(CHECK(## val ##\,\  ## ...##)), 0, 0, __VA_ARGS__)); }
//FAIL([extra])
#define FAIL(...) { CheckFail(_CKR_CFI(__FILE__, _CKR_FUN, __LINE__, _CKR_STR(FAIL(##...##)), 0, 0, __VA_ARGS__)); }
#pragma endregion

#pragma region WIN_CHECK
#ifdef _WINDOWS_
template <class R, class E>
void _CKR_WINSYSMSG_FAIL(CheckFailInfo<R, E> info, DWORD error)
{
    char* msg = 0;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_MAX_WIDTH_MASK, 0, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&msg, 0, 0);

    char* l = msg;
    char* r = (char*)((size_t)msg + size - 1);
    while (r != l) { --r; if (!*r != ' ') { *(r + 1) = 0; break; } }
    while (r != l && (*l == ' ')) ++l;
    size_t trimmedMsgSize = r - l + 1;

    char* errorStr = new char[10 + 1];
    _ultoa_s(error, errorStr, 11, 10);
    size_t errorStrSize = strlen(errorStr);

    char* fullMsg = new char[errorStrSize + 2 + trimmedMsgSize + 1];
    memcpy(fullMsg, errorStr, errorStrSize);
    memcpy(fullMsg + errorStrSize, ": ", 2);
    memcpy(fullMsg + errorStrSize + 2, l, trimmedMsgSize + 1);

    info.info = fullMsg;
    CheckFail(info);

    LocalFree(msg);
    delete[] errorStr;
    delete[] fullMsg;
}
//WIN_CHECK_EX(value, condition, [extra])
#define WIN_CHECK_EX(val, con, ...) { auto r = (val); if (!(r con)) _CKR_WINSYSMSG_FAIL(_CKR_CFI(__FILE__, _CKR_FUN, __LINE__, _CKR_STR(CHECK_EX(## val ##\,\  ## con ##\,\  ## ...##)), 0, r, __VA_ARGS__), GetLastError()); }
//WIN_CHECK(value, [extra])
#define WIN_CHECK(val, ...) { if (!(val)) _CKR_WINSYSMSG_FAIL(_CKR_CFI(__FILE__, _CKR_FUN, __LINE__, _CKR_STR(CHECK(## val ##\,\  ## ...##)), 0, 0, __VA_ARGS__), GetLastError()); }
//WIN_FAIL([extra])
#define WIN_FAIL(...) { _CKR_WINSYSMSG_FAIL(_CKR_CFI(__FILE__, _CKR_FUN, __LINE__, _CKR_STR(FAIL(##...##)), 0, 0, __VA_ARGS__), GetLastError()); }
#ifdef _WINSOCK2API_
//WSA_CHECK_EX(value, condition, [extra])
#define WSA_CHECK_EX(val, con, ...) { auto r = (val); if (!(r con)) _CKR_WINSYSMSG_FAIL(_CKR_CFI(__FILE__, _CKR_FUN, __LINE__, _CKR_STR(CHECK_EX(## val ##\,\  ## con ##\,\  ## ...##)), 0, r, __VA_ARGS__), WSAGetLastError()); }
//WSA_CHECK(value, [extra])
#define WSA_CHECK(val, ...) { if (!(val)) _CKR_WINSYSMSG_FAIL(_CKR_CFI(__FILE__, _CKR_FUN, __LINE__, _CKR_STR(CHECK(## val ##\,\  ## ...##)), 0, 0, __VA_ARGS__), WSAGetLastError()); }
//WSA_FAIL([extra])
#define WSA_FAIL(...) { _CKR_WINSYSMSG_FAIL(_CKR_CFI(__FILE__, _CKR_FUN, __LINE__, _CKR_STR(FAIL(##...##)), 0, 0, __VA_ARGS__), WSAGetLastError()); }
#endif
#endif
#pragma endregion

#define HANDLE_CHECK_FAIL(info) template<class R, class E> void CheckFail(CheckFailInfo<R, E> info)

#endif
