# expected

This is an implementation of the standardized `std::expected`.
It follows the standard, taken from https://timsong-cpp.github.io/cppwp/n4950/expected,
as close as possible with C++17 features.

> [!CAUTION]
> Currently, there are the following divergences from the standard:
>
> 1. Limitations to explicitness of constructors due to the smaller feature set of C++17
> 2. Missing partial template specialization for `template <class E> expected<void, E>`
> 3. Addition of `[[nodiscard]]` on all observers and monadic/equality operators (intentional divergence)

All divergences are highlighted with comments in code.

## Additional features

For convenience, some additional functions have been provided in the header `extensions.h`.

The following features are provided:

- Conversion from/to score::cpp::expected
- Conversion to std/score::cpp::optional with explicit error handling

These extensions are explicitly not part of the `expected` type, to ease migration to a stdlib implementation once the
project switches to C++23.
The extensions are written in a way, that they will continue to work with any implementation following the standard.

## Divergence from score::cpp::expected

This implementation diverges from score::cpp::expected in some ways.
Below we give a short overview of the divergences and why this implementation refrains from compatibility.

### Construction

score::cpp::expected did not fully follow the standard with construction. Especially in-place construction was sometimes
falling short of what the standard requires. This implementation follows the standard instead.

### Construction on Assignment

If a type was only copy-constructible but not copy-assignable, score::cpp::expected is still copy-assignable. Under the hood,
it constructed the non-assignable type instead of assigning it.
The same trick was done for move-assignment.

The standard actually prohibits this:
- https://timsong-cpp.github.io/cppwp/n4950/expected#object.assign-4
- https://timsong-cpp.github.io/cppwp/n4950/expected#object.assign-5

This implementation follows the standard instead.

### Observers

score::cpp::expected does not provide all observers required by the standard.
The missing observers are:

- `value_or` (https://timsong-cpp.github.io/cppwp/n4950/expected#object.obs-18)
- `error_or` (https://timsong-cpp.github.io/cppwp/n4950/expected#object.obs-22)

This implementation follows the standard and provides these observers.

### Monadic operators

score::cpp::expected does not provide any monadic operators required by the standard.
https://timsong-cpp.github.io/cppwp/n4950/expected#object.monadic

This implementation follows the standard and provides all required monadic operators.

### score::cpp::expected_blank

AMP provided a way for functions to return an error when there is no result value to return
(e.g. the function would normally return `void`).

This was done through `score::cpp::expected_blank`.

The standard does not provide such a type and instead defines a partial template specialization:

```
template <class E>
expected<void, E>;
```

This implementation currently falls short in this regard. There is no partial template specialization as required by the
standard.
