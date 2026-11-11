# Scoped Functions

```c++
score::safecpp::Scope<> scope{};
score::safecpp::CopyableScopedFunction<void()> copyable_scoped_function{scope, [](){
    /* only called while scope is not expired */
}};
score::safecpp::MoveOnlyScopedFunction<void()> move_only_scoped_function{scope, [](){
    /* only called while scope is not expired */
}};
```

```c++
score::cpp::stop_source stop_source{};
score::safecpp::CopyableScopedFunction<void()> copyable_scoped_function{stop_source.get_token(), [](){
    /* only called while stop_source.request_stop() was not called */
}};
score::safecpp::MoveOnlyScopedFunction<void()> move_only_scoped_function{stop_source.get_token(), [](){
    /* only called while stop_source.request_stop() was not called */
}};
```

A `ScopedFunction` allows users to consciously limit when an enclosed callable can be invoked.

There are two variants of a `ScopedFunction`:

- `MoveOnlyScopedFunction`
- `CopyableScopedFunction`

In case of doubt and if possible use the `MoveOnlyScopedFunction`.

This document refers to both variants simply as `ScopedFunction`, when their differences are irrelevant in the given
context.

## Limitation when the enclosed callable can be invoked

To limit when the enclosed callable can be invoked, a `ScopedFunction` must be associated either to a `Scope` or
a `stop_source` at construction.

A `ScopedFunction` wraps a callable and ensures that it is only called as long as:

- the associated `Scope` is not expired, or
- the associated `stop_source` did not receive a stop request.

### Scope

The `Scope` will start the expiration process in any of the following situations:

- when the `Scope` destructor is called
- when the `Expire` function in the `Scope` class is called

The expiration process will finish once the ongoing calls of all associated `ScopedFunctions` have finished.

If a `ScopedFunction` is called once the associated `Scope` is expired, the enclosed callable will not be called.

### stop_source

The `stop_source` will start the expiration process when its `request_stop()` function is called.

> [!CAUTION]
> In contrast to a `Scope`, the destruction of a `stop_source` does not implicitly start the expiration process!
> Destructing a `stop_source` without calling `request_stop()` before, will neither wait for ongoing invocations of the
> enclosed callable of associated `ScopedFunction`s to complete, nor will it prohibit future invocations.

The first call to the `request_stop()` function of a `stop_source` will block until the expiration process is finished.
The expiration process will finish once the ongoing calls of all associated `ScopedFunctions` have finished.

> [!CAUTION]
> This can lead to deadlocks if any of the associated `ScopedFunction`s waits for a resource held by the thread that is
> blocked on the call to `request_stop()`.

> [!CAUTION]
> Any following or concurrent calls to `request_stop()` will not wait until the expiration process is finished!
> Any call to `request_stop()` will return `false` if this is the case.
> Also see the notes in: https://en.cppreference.com/w/cpp/thread/stop_source/request_stop#Notes

If a `ScopedFunction` is called once the `request_stop()` function of the associated `stop_source` caused
the `stop_token` to expire, the enclosed callable will not be called.

## Construction and Assignment

The different variants of `ScopedFunction` originate from the requirements they place on the enclosed callables
and their therefore available copy- and move-constructors and assignment operators.

All variants of `ScopedFunction<FunctionType>` place the following requirements on their
enclosed callables (`Callable`):

- With `FunctionType = R(Args...)`, `std::is_invocable_r_v(R, Callable, Args...)` must be `true`
- With C++17 and `FunctionType = R(Args...) noexcept`, `std::is_nothrow_invocable_r_v(R, Callable, Args...)` must
  be `true`

The variants of `ScopedFunction` place the following additional requirements on their enclosed callables:

- `MoveOnlyScopedFunction` requires enclosed callables to
  be [MoveConstructible](https://en.cppreference.com/w/cpp/named_req/MoveConstructible)
- `CopyableScopedFunction` requires enclosed callables to
  be [CopyConstructible](https://en.cppreference.com/w/cpp/named_req/CopyConstructible)

ScopedFunction variants fulfill the
following [Basic C++ Named Requirements](https://en.cppreference.com/w/cpp/named_req):

|                        | DefaultConstructible | CopyConstructible | CopyAssignable | MoveConstructible | MoveAssignable    | Destructible |
|------------------------|----------------------|-------------------|----------------|-------------------|-------------------|--------------|
| MoveOnlyScopedFunction | Yes                  | No                | No             | Yes               | Yes               | Yes          |
| CopyableScopedFunction | Yes                  | Yes               | Yes            | Yes <sup>1)</sup> | Yes <sup>1)</sup> | Yes          |

1 ) if the enclosed callable is [MoveConstructible](https://en.cppreference.com/w/cpp/named_req/MoveConstructible)

It is possible to create a `MoveOnlyScopedFunction` from a `CopyableScopedFunction`, but not the other way around.

A `Scope` fulfills the
following [Basic C++ Named Requirements](https://en.cppreference.com/w/cpp/named_req):

|       | DefaultConstructible | CopyConstructible | CopyAssignable | MoveConstructible | MoveAssignable | Destructible |
|-------|----------------------|-------------------|----------------|-------------------|----------------|--------------|
| Scope | Yes                  | No                | No             | Yes               | Yes            | Yes          |

When an object of type `Scope` is the target of a move-assignment, all its associated `ScopedFunction`s expire.

## Destruction

The destructor of `Scope` will block for as long as any invocation of its associated `ScopedFunction`s is still ongoing.

> [!CAUTION]
> In contrast to a `Scope`, the destruction of the `stop_source` does not implicitly start the expiration process!
> Destructing a `stop_source` without calling `request_stop()` before, will neither wait for ongoing invocations of the
> enclosed callable of associated `ScopedFunction`s to complete, nor will it prohibit future invocations.

The enclosed callable of a `ScopedFunction` is destroyed by the same thread that destructs the `ScopedFunction`.

## Thread-Safety

The class `Scope` is thread-safe.

Calling `ScopedFunction` is thread-safe if the enclosed callable is thread-safe.
All other `ScopedFunction` operations are thread-safe.

## Performance

At construction of a `Scope` a shared mutex is created.
Such shared mutex gets locked for every invocation of the `ScopedFunction`.
Please be aware that this has a potential performance impact when invoking `ScopedFunction`s highly frequently.

It is allowed to use a `Scope` for multiple `ScopedFunction`s.

## Return Type

A `ScopedFunction` returns the return value of the enclosed callable wrapped into an `score::cpp::optional<>`.

To determine if the enclosed callable was called, you may check if the returned `score::cpp::optional<>` has a value,
using `score::cpp::optional<ReturnType>::has_value()`.

```c++
template <class ReturnType, class... Args>
score::cpp::optional<ReturnType> ScopedFunction<ReturnType(Args...)>::operator()(Args args...);

template <class ReturnType, class... Args>
score::cpp::optional<std::reference_wrapper<ReturnType>> ScopedFunction<ReturnType&(Args...)>::operator()(Args args...);

template <class ReturnType, class... Args>
score::cpp::optional<score::cpp::blank> ScopedFunction<void(Args...)>::operator()(Args args...);
```

## Placement of the Scope

For `ScopedFunction`s to work correctly, the developer must be careful with the placement of the `Scope`.
This sections provides some instructions on how to correctly place `Scope`s in various situations.

The `Scope` of a `ScopedFunction` must always appear after any captured variables of the enclosed callable.
This is because variables are destroyed in the reverse order of their appearance.
Hence, for the `ScopedFunction` to work appropriately, the Scope must appear last to be destroyed before the captured
variables.

```c++
{
  /* Variables that will be captured by the enclosed callable */
  bool var1{false};
  std::int32_t val2{42};

  /* No variable captured by the enclosed callable shall be declared after the Scope creation */
  Scope scope{};
  
  ScopedFunction<void()> scoped_function{scope, [&var1, &var2](){ /* ... */ }};
}
```

This also applies when you place a `Scope` of a `ScopedFunction` within a class as one of its attributes.
You must make sure that it is placed after all other attributes that are captured by the enclosed callable.

If the enclosed callable captures `this`, make sure that the attributes of the class follow this order:

1. All attributes of the class except for the `Scope` and its associated `ScopedFunction`s
2. The `Scope`
3. The `ScopedFunctions` associated to the `Scope` (if they must be stored in the class)

Further, if you have a custom destructor, you must explicitly expire the scope (`Expire()`) before doing anything else
in the destructor.
This is because class attributes are only destroyed after the custom destructor is finished. Hence,
the `ScopedFunction`s associated to the scope may still be invoked during the invocation of the custom destructor.

```c++
class CustomClass {
  public:
    /* The custom destructor is only necessary if "class-specific destruction" is required */
    ~CustomClass() {
        /* Explicitly expire the scope */
        scope_.Expire();
        
        /* Do class-specific destruction here */
        worker_.DoSomeLastMinuteStuff();
    }
    
    CopyableScopedFunction<void()> GetAvailabilityHelper() const noexcept { return helper_; }
    
  private:
    /* Other class attributes */
    Worker worker_{};
    
    /* Scope is the last attribute in the class (except for associated ScopedFunctions) */
    Scope scope_{};
    
    /* If you must store the associated ScopedFunction, do it after the Scope */
    CopyableScopedFunction<void()> helper_{scope_, []() noexcept {}};
};

std::int32_t main() {
    CopyableScopedFunction<void()> function{};
    bool is_available{false};
    is_available = function().has_value();
    // is_available == false
    
    {
        CustomClass custom_class{};
        function = custom_class.GetAvailabilityHelper()};
        
        is_available = function().has_value();
        // is_available == true
    }
    
    is_available = function().has_value();
    // is_available == false
    
    return 0;
}
```

## Limitations

### Dynamic Memory Allocation

The constructors of `Scope` and `ScopedFunction` both require dynamic memory allocation.
Both classes adhere to the C++ named
requirement [AllocatorAwareContainer](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer), except for
the following limitations:

- The class `ScopedFunction` does not provide the type trait `allocator_type`
- The class `Scope` is move-constructible but does not provide an allocator-aware move constructor
- The class `Scope` is only move-assignable with allocators that propagate on move-assignment
- Allocators used for `ScopedFunction` and `Scope` must support rebinding

These limitations are a direct result of the functionality. Hence, lifting them is impossible.

## Small callable optimization

The current implementation does not provide small callable optimization like `std::function`, `std::copyable_function`
or `std::move_only_function`. All callables, independent of their size, are dynamically allocated. A pull request that
provides this optimization is welcome.
