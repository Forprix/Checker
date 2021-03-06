# Description:
Single header library for assertion
# Usage:
```cpp
#include "checker.h"
#include "checkerhandler.h" //default

int main() {
    CHECK(false);
}
```
Or customize:
```cpp
#include <iostream>
#include "checker.h"

int main()
{
    CHECK(false, "Oops! Program 'failed'");
}

HANDLE_CHECK_FAIL(info)
{
    std::cout << "Error on line: " << info.line << '\n';
    std::cout << "Description: " << *info.extra << '\n';
    std::cin.get(); //make console hang
    exit(1);
}
```

- CHECK
- CHECK_EX
- FAIL
- WIN_CHECK (*Windows only*)
- WIN_CHECK_EX (*Windows only*)
- WIN_FAIL (*Windows only*)
- WSA_CHECK (*Windows only*)
- WSA_CHECK_EX (*Windows only*)
- WSA_FAIL (*Windows only*)


# To do:
- Minimize CheckFailInfo for Release
- add CHECKER_FORCE_DEBUG
- Add options for checkerhandler.h
- Add Linux & MacOS support for checkerhandler.h
