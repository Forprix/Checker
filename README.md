# Usage:
```cpp
#include "checker.h"

int main() {
    CHECK_EX(false, "Oops! Program 'failed'");

    return 0;
}
```

# Definitions:
### CHECKER_ASCII
tells Checker.h to use ASCII. Strings will be made of 'char'
### CHECKER_WIDE
tells Checker.h to use Unicode. Strings will be made of 'wchar_t'
### CHECKER_MANUAL
tells Checker.h that you want to use your own failure handler ('CheckFailed')
### CHECKER_SUSPEND_PROCESS
tells Checker.h to suspend everything when check fails
### CHECKER_SILENT
tells Checker.h to stop playing annoying sound when check fails

# Customized Usage:
```cpp
#include <Windows.h>

#define CHECKER_SILENT //no sound
#define CHECKER_SUSPEND_PROCESS
#include "checker.h"

int main() {
    CreateThread(0, 0, [](LPVOID lpThreadParameter) {
        MessageBoxW(0, L"Hang on", L"I am suspended", 0);
        return (DWORD)0;
    }, 0, 0, NULL);
    Sleep(100);

    CHECK_EX(false, "Program 'failed', so everything is suspended (Except this thread)");

    return 0;
}
```
