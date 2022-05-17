# Usage:
```cpp
#include "checker.h"
#include "checkerhandler.h"

int main() {
    CHECK(false);
}
```
Or customize:
```cpp
#include <iostream>
#include "checker.h"

int main() {
    CHECK(false, "Oops! Program 'failed'");
}

template<class R, class E>
void CheckFail(CheckFailInfo<R, E> info)
{
    std::cout << "Error on line: " << info.line << '\n';
    std::cout << "Description: " << *info.extra << '\n';
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
- add options for checkerhandler.h
