# Safe C++

## Introduction

C++ is a general purpose programming language. In order to be used in safety critical systems, certain aspects need to be considered.
In the automotive field, the guidance on the aspects to be considered is provided by the standard ISO 26262 [1] (see Part 6 Section 5.4).

This page is a collection of topics that are relevant for C++ in a safety critical systems.

## Language features

Every new version of C++ brings new features with it. Some of them can have a positive impact due to making the code safer by reducing the chances to have undesired behavior.

In this section, you will learn about some of these features. It is not intended to be a complete list. You will learn about several of them from coding guidelines. Guidelines will prevent you from using certain features and constructs and they will offer you safer alternatives instead. Here, we will mention features that might not be listed in some or any of the guidelines or they might be not so obvious for the reader of the guidelines. It is also not a list of features that you must use. Instead, it is up to you to judge if certain features make sense or not in each particular use case.

The features are divided by C++ version where it became available. This structure can help you to judge which C++ version to use.

### C++11 and older

One of the challenges that comes with templates is how do you guarantee that each template instantiation is tested.
It might be tested for a specific type but not for another one, and if you check the code coverage at source code level you might not be able to differentiate that.
Even if you have tests for all the types that are used, how do you guarantee that when it is instantiated with a new type, the newly instantiated one will also be tested?

A way to control for which types a template is instantiated is to add a static assertion to the template definition checking that the type is one of the supported ones.
This will make sure that a user of the templated function/class cannot use it with untested types.

This is only possible, if you know all the types for which the template is valid.

In case you are not aware of all the types for which the template is valid, you can still restrict its instantiation using static asserts and type traits.
You will not be able to guarantee that it is tested with the used type, but you define your expectations to the used type and can build your unit tests upon them.

### C++17

#### [[nodiscard]]

`[[nodiscard]]` is an attribute to prevent that the return value of a function is ignored by the caller. This is specially useful for functions that return error codes because it forces the caller to check it or discard it explicitly.
`[[nodiscard]]` can also be applied to types; in that case, any function that returns that type, the return value cannot be silently ignored.

### C++20

#### Concepts

[Concepts](https://en.cppreference.com/w/cpp/language/constraints) provides a way to restrict templates that is better
that what could be achieved before C++20 with [enable_if](https://en.cppreference.com/w/cpp/types/enable_if) and
[static_assert](https://en.cppreference.com/w/cpp/language/static_assert).

#### consteval

[consteval](https://en.cppreference.com/w/cpp/language/consteval) guarantees that a function is evaluated at compile time.
The implications are that this function will not produce any code in the translation unit. This guarantees that dead code
in the function will not result in assembly dead code.

#### constinit

[constinit](https://en.cppreference.com/w/cpp/language/constinit) guarantees that a variable is static initialized. In
certain scenarios the use of constinit might be enough to prevent the [static initialization fiasco](https://en.cppreference.com/w/cpp/language/siof).

#### Modules

[Modules](https://en.cppreference.com/w/cpp/language/modules) help to reduce compilation times but also prevents the usage
of transitive dependencies. This helps to better define the boundaries of the components.

#### Spaceship operator

Spaceship operator is the name of the operator introduced in C++20 that allows to specify [default comparisons](https://en.cppreference.com/w/cpp/language/default_comparisons).
There are several scenarios where comparison operators are defined that are trivial but still there is the possibility
for the developer to do a mistake. Default comparison is similar to default copy/move assignment in the sense that the compiler
does it for you and reduces the chances of introducing a bug.

#### Designated initializers

[Designated initializers](https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers) helps to make
clear in aggregate initializers which parameter goes to which data member.

#### source_location

Macros have a bunch of associated problems and several coding guidelines forbid the use of them. However, before C++20,
they are needed to get the file name or the line location. In C++20 [source_location](https://en.cppreference.com/w/cpp/utility/source_location)
can be used instead.

#### jthread

A regular thread has the problem that if it gets destructor without an explicit join or an explicit detach `std::terminate()`
gets called. [jthread](https://en.cppreference.com/w/cpp/thread/jthread) is a thread that calls join on destruction.

#### stop_token

[stop_token](https://en.cppreference.com/w/cpp/thread/stop_token) provide a way to indicate that certain operation should be
interrupted. This is a critical piece in safety critical systems where a fast and correct shutdown is needed.

## Coding guidelines

One of the aspects to be considered when writing code for safety critical systems is coding guidelines.
Most programming languages are not specifically design for safety critical systems. Because of that, guidelines are created to fill the gap in order to have a language as safe as reasonably possible.

In this section, we will talk about some well known C++ guidelines and where to find them.

### MISRA C++:2023

These guidelines were released on November 2023, and they replace the AUTOSAR guidelines for C++14 and the old MISRA C++:2008 guidelines.

MISRA C++:2023 is designed to be used with C++17. If you use it in older C++ versions like C++14 or C++11, you would have to make sure that you do not need additional or different guidelines. For example, there have been guidelines from MISRA C++:2008 and/or AUTOSAR that have been dropped because the addressed issues are no longer a problem in C++17.

In case you are not already using MISRA C++:2023, these are the main reasons why you might want to start using them:

- These guidelines are for C++17 and C++17 brings some features that will help you to make your code safer (see above).
- MISRA C++:2008 and AUTOSAR guidelines have known issues. These known issues have been considered by the MISRA working group when creating MISRA C++:2023.
- If you want to follow AUTOSAR guidelines for C++14, you need to have two documents, the AUTOSAR guidelines and the MISRA C++:2008. This is because AUTOSAR guidelines refer to some guidelines of MISRA C++:2008. However, some of the MISRA C++:2008 guidelines have been rejected by AUTOSAR. Because of that, it is not easy to have an overview of the rules that apply. MISRA C++:2023 is a single document.
- Several rules from previous guidelines were reevaluated and considered no longer safety relevant.
- These guidelines where developed together with members from MISRA and AUTOSAR consortium.
- This is the first document of a series of updates. The following documents that will be release by MISRA for C++ will be assuming the usage of MISRA C++:2023.

## What will you find in the different folders

SafeCPP provides utilities to write safer C++ code for safety-critical systems. The C++ standard library is designed for general-purpose use and lacks some of the stricter semantics required in safety-critical applications. SafeCPP addresses these gaps.

Following components are provided by SafeCPP:

- **aborts_upon_exception**: Globally replaces C++ exception handling with `std::abort()` calls for safety compliance.
- **coverage_termination_handler**: Ensures coverage data is preserved in GoogleTest death tests.
- **safe_atomics**: Prevents overflow when adding numbers to atomic variables (thread-safe addition with `TryAtomicAdd`).
- **safe_math**: Provides overflow-safe arithmetic operations for integral and floating-point types.
- **scoped_function**: Offers controlled callable execution with explicit lifetime guarantees.
- **string_view**: Provides null-terminated string view types and safety utilities for secure string handling.

### Bazel targets for the standard library

In order to properly deal with exceptions, special care needs to be taken.
In the [stdlib folder](./stdlib/) you will find the targets required in order to use the standard library in QM and ASIL B applications.

### Code coverage termination handler

Code coverage computation for [death tests](https://github.com/google/googletest/blob/release-1.11.0/docs/advanced.md#death-tests) does not work out of the box.
In the [coverage_termination_handler folder](./coverage_termination_handler/) you will find what is needed to make it work.

### Tests for implementation defined types

The C++ specification has some parts that are implementation defined. For some of these parts, the implementation defined behavior can be checked programmatically. In the [test folder](./test/) you will find tests whick verify certain behavior we rely on.

## References

- [1] ISO 26262-6:2018 Road vehicles - Functional safety - Part 6: Product development at the software level
