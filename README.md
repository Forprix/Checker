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

# Example:
```cpp
#include "checker.h"

int main() {
    CHECK_EX(false, "Oops! Program 'failed'");

    return 0;
}
```