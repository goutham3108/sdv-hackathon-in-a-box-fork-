# Null-Termination Check for string-view like types

`std::string_view` does not give any guarantee w.r.t. its underlying character buffer to be null-terminated.
Same applies for `score::cpp::span` as well as `std::span`! As a result, providing a pointer to the underlying
buffer (e.g. via `string_view.data()`) to another API which expects the underlying character buffer to
be null-terminated (e.g. legacy C-APIs or `std::string`'s constructor), can easily result in undefined
behavior. Furthermore, programs could also easily produce unpredictable results in case the
null-termination occurs at a location which is beyond the view's range (see examples below).

Due to lack of a guaranteed null-terminated `string_view` type in C++'s stdlib, we provide a common
utility here which shall be used by any code requiring a null-termination guarantee in conjunction
with a `string_view`'s underlying buffer. This utility also works for `string_view`-like types
such as `score::cpp::span` and `std::span` as well as similar types from `ara::core`.

**NOTE:** In general, developers are requested to migrate their code (if possible) to make use of
`safecpp`'s types `zstring_view` or `zspan` since their underlying sequences are guaranteed to be
null-terminated.
Once C++'s stdlib or any other custom library (e.g. `amp`) offers null-terminated `span` and/or
`string_view` type(s), the respective types within the `safecpp` namespace could then get
replaced by aliases to these standard ones (if compatible).

<br>

## How to fix code which makes use of `string_view.data()` but does not use `string_view.size()` at the same time

### Problem: Unsafe usage of `string_view.data()` with C APIs

**❌ UNSAFE** - Do **NOT** do this:
```cpp
#include <string_view>
#include <cstring>
#include <cstdio>

namespace
{
// not an appropriate solution, see `UnsafeExamples()` below for explanation
constexpr std::string_view kMyStringConstant = "hello";
}

void PrintString(const std::string&);

void UnsafeExamples(std::string_view sv)
{
    // UNSAFE: C string functions assume null-termination
    auto len = strlen(sv.data());  // May read past buffer end!

    // UNSAFE: printf expects null-terminated string
    printf("Value: %s\n", sv.data());  // May read past buffer end!

    // UNSAFE: C string comparison
    if (strcmp(sv.data(), "expected") == 0)  // May compare past buffer end!
    {
        // ...
    }

    // UNSAFE: File operations expecting null-terminated paths
    const auto buffer[] = "filename.config";
    const std::string_view filename{buffer, 13U};  // 'filename.conf'
    FILE* file = fopen(filename.data(), "r");  // Will open wrong file, i.e. 'filename.config'!

    // safe by coincidence (and hence not an appropriate solution)
    // furthermore, clang-tidy's check `bugprone-suspicious-stringview-data-usage` will (correctly) emit a warning here
    printf("Constant: %s\n", kMyStringConstant.data());  // Only not an issue because its underlying literal "hello" is null-terminated
                                                         // However, `printf()` accesses the underlying buffer outside the view's range here!
                                                         // (since the underlying buffer's null terminator is not part of the view's range)

    // UNSAFE: `std::string`'s implicit constructor from `const char*` expects a null-terminated string
    printString(sv.data());
}
```

**✅ SAFE** - Use these patterns instead:

```cpp
#include "score/language/safecpp/string_view/zstring_view.h"

#include <string_view>
#include <string>
#include <cstring>
#include <cstdio>

namespace
{
using safecpp::literals::operator""_zsv;

constexpr auto kMyStringConstant = "hello"_zsv;
// or
constexpr safecpp::zstring_view kMyStringConstant = "hello"_zsv;
}

void PrintString(const std::string&);

void SafeExamples(std::string_view sv)
{
    // SAFE: Use string_view's own methods when possible
    const auto len = sv.size();  // Preferred over `strlen()`

    // SAFE: Convert to `std::string` first
    const std::string str{sv};  // But bear in mind that this involves memory allocation and results in the whole string getting copied!
    printf("Value: %s\n", str.c_str());

    // SAFE: Use string_view comparison
    if (sv == "expected")  // Preferred over `strcmp()`
    {
        // ...
    }

    // SAFE: `safecpp::zstring_view` guarantees its underlying buffer to be null-terminated
    printf("Constant: %s\n", kMyStringConstant.data());  // no more clang-tidy warning here

    // SAFE: explicitly convert to `std::string` beforhand (but involves memory allocation + copy)
    printString(std::string{sv});

    // SAFE: in case `sv` would be an `score::cpp::string_view`, write it as follows
    printString({sv.data(), sv.size()});
}

// SAFE: To avoid additional memory allocation as occurring above, better make use of `safecpp::zstring_view` instead.
void PreferredSolution(safecpp::zstring_view sv)
{
    // SAFE: `safecpp::zstring_view` guarantees null-termination of its underlying buffer
    FILE* file = fopen(sv.data(), "r");

    // SAFE: `safecpp::zstring_view` can get converted to `std::string` (but also involves memory allocation + copy)
    printString(std::string{sv});
}
```

<br>

### Problem: Constructor calls expecting null-terminated strings

**❌ UNSAFE:**
```cpp
#include <string_view>
#include <string>

void UnsafeExamples(std::string_view sv)
{
    // UNSAFE: `std::string(const char*)` constructor assumes null-termination
    std::string str{sv.data()};  // May include more characters than `sv` actually covered or read past buffer end!

    // UNSAFE: C-style string operations
    char buffer[100];
    strcpy(buffer, sv.data());  // May copy past buffer end!
}
```

**✅ SAFE:**
```cpp
#include <string_view>
#include <string>

void SafeExamples(std::string_view sv)
{
    // SAFE: Use `std::string`'s string_view constructor
    std::string str{sv};  // Correctly handles non-null-terminated data
    // SAFE: in case `sv` would be an `score::cpp::string_view`, write it as follows
    std::string str{sv.data(), sv.size()};  // Correctly handles non-null-terminated data

    // SAFE: use `std::string_view`'s copy() method with size limit
    char buffer[100];
    const auto num_copied = sv.copy(buffer, sizeof(buffer - 1U));
    buffer[num_copied] = '\0';  // Ensure null-termination
}
```

<br>

### Problem: Legacy C API integration

**❌ UNSAFE:**
```cpp
#include <string_view>

extern "C" {
    int legacy_function(const char* null_terminated_str);
    void log_message(const char* msg);
}

void UnsafeExamples(std::string_view message)
{
    // UNSAFE: Legacy C functions expect null-terminated strings
    int result = legacy_function(message.data());  // Undefined behavior!
    log_message(message.data());  // May log garbage or read past buffer end!
}
```

**✅ SAFE:**
```cpp
#include "score/language/safecpp/string_view/zstring_view.h"

#include <string_view>

extern "C" {
    int legacy_function(const char* null_terminated_str);
    void log_message(const char* msg);
}

void SafeExamples(safecpp::zstring_view message)
{
    // SAFE: `safecpp::zstring_view`'s underlying buffer is guaranteed to be null-terminated
    int result = legacy_function(message.data());
    log_message(message.data());
}
```

**✅ SAFE:**
```cpp
#include "score/language/safecpp/string_view/null_termination_check.h"

#include <span>

extern "C" {
    int legacy_function(const char* null_terminated_str);
    void log_message(const char* msg);
}

void SafeExamples(std::span<char> message)  // here, `message` is expected to contain a null-byte as its last element
{
    // SAFE: Ensure null-termination at `message`'s last element via `safecpp`'s utility function before passing to C APIs
    int result = legacy_function(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(message));
    log_message(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(message));
}
```

<br>

### Problem: Substring operations with C APIs

**❌ UNSAFE:**
```cpp
#include <string_view>
#include <cstdio>

void ProcessSubstringsUnsafe(std::string_view full_string)
{
    // UNSAFE: Taking substring and using `data()` in conjunction with C APIs
    auto sub = full_string.substr(5, 10);
    printf("Substring: %s\n", sub.data());  // Almost certainly not null-terminated!

    // UNSAFE: Even worse with multiple substrings
    auto part1 = full_string.substr(0, 5);
    auto part2 = full_string.substr(5, 5);
    printf("Parts: %s, %s\n", part1.data(), part2.data());  // Undefined behavior!
}
```

**✅ SAFE:**
```cpp
#include <string_view>
#include <string>
#include <cstdio>

void ProcessSubstringsSafe(std::string_view full_string)
{
    // SAFE: Convert substring to string when needed for C APIs
    const std::string sub_str{full_string.substr(5, 10)};
    printf("Substring: %s\n", sub_str.c_str());

    // SAFE: Handle multiple substrings properly
    const std::string part1{full_string.substr(0, 5)};
    const std::string part2{full_string.substr(5, 5)};
    printf("Parts: %s, %s\n", part1.c_str(), part2.c_str());
}
```

<br>

### Problem: Template code with string-like types

**❌ UNSAFE:**
```cpp
#include <span>
#include <string_view>
#include <cstdio>
#include <cstring>

template<typename StringLike>
void ProcessStringUnsafe(const StringLike& str)
{
    // UNSAFE: Assumes that `data()` returns null-terminated buffer
    auto len = strlen(str.data());  // Fails for non-null-terminated spans/string_views!

    // UNSAFE: Direct use with C APIs
    printf("Content: %s\n", str.data());
}

void UnsafeExamples()
{
    std::string_view sv = "hello world";
    char buffer[] = {'h', 'e', 'l', 'l', 'o'};  // Not null-terminated!
    std::span<char> span{buffer};

    ProcessStringUnsafe(sv);                        // May work by accident
    ProcessStringUnsafe(span);                      // Undefined behavior!
    ProcessStringUnsafe(std::string_view{buffer});  // Undefined behavior!
}
```

**✅ SAFE:**
```cpp
#include <span>
#include <string>
#include <type_traits>
#include <cstdio>

template<typename StringLike>
void ProcessStringSafe(const StringLike& str)
{
    if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<StringLike>>>, std::string>)
    {
        // SAFE: `std::string` already guarantees null-termination
        printf("Content: %s\n", safe_str.c_str());
    }
    else
    {
        // SAFE: Always ensure null-termination when needed
        const std::string buffer{str.data(), str.size()};
        printf("Content: %s\n", buffer.c_str());
    }
}

void SafeExamples()
{
    std::string_view sv = "hello world";
    char buffer[] = {'h', 'e', 'l', 'l', 'o'};  // Not null-terminated!
    std::span<char> span{buffer};

    ProcessStringSafe(sv);
    ProcessStringSafe(span);
    ProcessStringSafe(std::string{sv});
    ProcessStringSafe(std::string_view{buffer});
}
```

<br>

### Problem: Pointer arithmetic on underlying buffer of span/string_view

**❌ UNSAFE:**
```cpp
#include <span>
#include <cstring>
#include <cstdio>

void UnsafeSpanPointerArithmetic(std::span<char> span)
{
    // UNSAFE: Assuming span contains null-terminated string
    char* next_word = strchr(span.data(), ' ');  // `strchr()` assumes null-termination!
    if (next_word)
    {
        *next_word = '\0';  // May modify memory beyond span!
    }

    // UNSAFE: Pointer arithmetic without bounds
    const std::size_t [new_start, new_size] = <...>;  // some arbitrary calculation
    std::span<char> new_span{span.data() + new_start, new_size};  // Might point to memory outside the original span's range
    new_span.at(0) = 'x';  // Might write to unowned memory!

    // POTENTIALLY UNSAFE (and could also be written more concise): trimming a span to a new size
    std::size_t trimmed_size = <...>;  // some arbitrary calculation
    std::span<char> trimmed_span{span.data(), trimmed_size}  // Unsafe in case calculated value `trimmed_size` is larger than original `span`'s size
    trimmed_span.back() = 'x';  // Might write to unowned memory!
}
```

**✅ SAFE:**
```cpp
#include <span>
#include <algorithm>
#include <string>

void SafeSpanOperations(std::span<char> span)
{
    // SAFE: Use algorithms with proper bounds
    auto next_word = std::find(span.begin(), span.end(), ' ');
    if (next_word != span.end())
    {
        *next_word = '\0';
        printf("String: %s\n", span.data());
    }

    // SAFE: Use `last(n)` instead of manually creating a new span object via `span.data()`
    //       For `std::string_view`, consider `remove_prefix()`
    const std::size_t num_last_elements = <...>;  // some arbitrary calculation
    auto new_span = span.last(std::min(num_last_elements, span.size()));  // always valid and never out of bounds; however, `newspan` is still potentially not null-terminated!
    new_span.at(0) = 'x';  // Will throw now in case `new_span` would be empty

    // SAFE: Use `first(n)` for trimming a span to a new size
    //       For `std::string_view`, consider `remove_suffix()`
    std::size_t trimmed_size = <...>;  // some arbitrary calculation
    auto trimmed_span = span.first(std::min(trimmed_size, span.size()));  // safe even in case `trimmed_size` is too large or even zero; however, remember that `trimmed_span` is almost certainly not null-terminated!
    if (not(trimmed_span.empty()))
    {
        trimmed_span.back() = 'x';  // Always safe now
    }
}
```

<br>

### Key Principles for Safe Usage

1. **Never assume `span.data()` or `string_view.data()` are null-terminated!**
   **If you do**, you have **undefined behavior** in your code.
   But in case such null-termination guarantee *is* required, use `safecpp`'s `zspan` or `zstring_view` instead.
2. **Convert to `std::string` prior to interfacing with C APIs**
3. **For performance/memory critical code, use `safecpp`'s null-termination check utility beforehand**
4. **Use `string_view`'s own methods (`size()`, `copy()`, comparison operators) whenever possible**
5. **Be extra careful with subspans/substrings of views - they are almost never null-terminated**

<br>

### When `string_view.data()` is safe to use

The only safe direct uses of `string_view.data()` are:
- When combined with `string_view.size()` for APIs that accept both pointer and length
- For read-only operations that don't assume null-termination

```cpp
// SAFE: Using `data()` in conjunction with `size()`
void safe_direct_usage(std::string_view sv)
{
    // SAFE: `write()` takes pointer and length
    write(STDOUT_FILENO, sv.data(), sv.size());

    // SAFE: `memcmp` takes pointer and length
    if (memcmp(sv.data(), "prefix", std::min(sv.size(), 6)) == 0)
    {
        // ...
    }

    // SAFE: copying with known bounds
    char buffer[256];
    const auto num_copied = sv.copy(buffer, sizeof(buffer - 1U));
    buffer[num_copied] = '\0';  // Ensure null-termination
}
```
