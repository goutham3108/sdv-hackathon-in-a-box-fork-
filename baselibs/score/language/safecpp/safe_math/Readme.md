# SafeMath

## Scope

Mathematical safety has two dimensions in computer science:

1. Lack of undefined behavior
2. Accuracy of operations

Both of these aspects are equally important to perform arithmetic operations on data in a safe manner. This is
especially important if the data operations are performed upon can not be fully sanitized. In such cases it is often
unknown whether an operation can be safely performed on said data or not without additional non-trivial checks on the
inputs.

Goal of this library is:

- to provide developers with easy-to-use tools to perform mathematical operations
- without the risk of undefined behavior
- while maintaining the highest possible accuracy of the result of an operation, given the involved data types.

## API

Interaction with this library happens exclusively via the `safe_math.h` header file.
It provides safe implementations for a number of arithmetic operations or related operations for signed and unsigned
integral types as well as floating point types.

### Absolute

In the standard library the functions `std::abs` and `std::fabs` are responsible to provide the absolute of a value.
This library provides the function `score::safe_math::Abs` as a safe replacement. Please note, that `score::safe_math::Abs`
will return a different type than the input type in some cases. This is intentional, to support the full numerical range
of this operation. You can combine the operation with a `score::safe_math::Cast` should you require a different return
type.

### Addition

When adding two numbers it is required to ensure that the result does not exceed the range of the result type. This
check is performed by `score::safe_math::Add` before performing the addition.

### Cast

When casting between arithmetic types, it is important to check whether the target type can store the value within its
numerical range. The check and cast can be performed by using `score::safe_math::Cast` which will return the casted value
(`static_cast`) if the is within bounds or an error if not.

For floating-point types an additional check is performed to ensure that the casting does not lead to loss of precision.
This includes casting from a floating-point type to a floating-point type with less width and to integral types.

### Comparison

Comparison of variables with the same type is trivial. If one must compare two variables with different types, several
corner cases can occur. SafeMath provides several APIs starting with `score::safe_math::Cmp...` that provide safe
comparison of any two supported arithmetic types.

Some comparison APIs require an additional parameter that specifies the tolerance of the comparison. This is because
these APIs check directly or indirectly for equality of floating-point types. Since floating-point types have limited
accuracy and thus are unable to perfectly represent every value, comparing them for perfect equality is prohibited by
MISRA C++:2008 Rule 6-2-2. Instead, such comparisons shall happen with some tolerance. Since the tolerance is highly
dependent on the magnitude of the values that are to be compared, this library does not choose a default tolerance.

Tolerance is provided in terms of ULPs, which loosely translates to amount of distinct floating-point values between
the two operands of the comparison.

### Division

Division is an arithmetic operation that has an undefined result when dividing real numbers by zero. The C++ standard
specifies any division by zero as [undefined behavior](https://timsong-cpp.github.io/cppwp/n4140/expr.mul#4). Most
floating point units adhering to IEEE754 will return infinity as a "close" estimation, since dividing by a very small
number will give you a result near to infinity.

SafeMath provides `score::safe_math::Divide` which will return an error in case the divisor equals zero, since this is the
original mathematical definition of the division. This includes the variant for floating-point parameters.

For integral divisions, `score::safe_math::Divide` will additionally return an error if the result of the division is
a floating-point value. This ensures that users of the library must explicitly care about accuracy of the result.

### Multiplication

When multiplying two numbers it is required to ensure that the result does not exceed the range of the result type. This
check is performed by `score::safe_math::Multiply` before performing the multiplication.

### Negation

Negation of arbitrary numbers is complex since in some cases the negated value is not representable in the type of the
original value. To simplify this for users, the function `score::safe_math::Negate` is provided.

### Subtraction

Subtraction of two numbers is also prone to result in a value that exceeds the numerical limits of the result type.
The function `score::safe_math::Subtract` provides safe subtraction for two operands with different types.

## Supported arithmetic types

The following arithmetic types are supported:

- `std::int8_t`, `std::int16_t`,`std::int32_t`,`std::int64_t` (2's complement representation)
- `std::uint8_t`, `std::uint16_t`,`std::uint32_t`,`std::uint64_t` (2's complement representation)
- `float`, `double` (if they adhere to IEEE754)

## Future plans

For the future this library shall be extended with:

- Wrapping signed integral types
- Strong types for all supported arithmetic types that provide safe and accurate arithmetic operators
- Chaining of operations to make error checking between operations optional

## Limitations

In some cases it is impossible to protect the developer from all risks. One such risk is catastrophic cancellation in
floating-point arithmetics. Catastrophic cancellation happens when multiple floating-point operations are chained
together and the mandatory rounding of the result of the first operation impacts the following operation in a way that
leads to a significantly different result as intended. Whether this effect occurs or not is highly dependent on (a) the
inputs of an algorithm and (b) how the different floating-point operations of the algorithm are combined. Especially the
latter aspect is outside the scope of this library, as it does not provide algorithms.

Arithmetic data types have a limited range of numbers they are able to depict. If a result shall be mathematically
accurate, it is important to never leave this range when performing calculations. Further, leaving the range of some
types also exhibits undefined behavior. Hence, this library checks upfront to every operation whether it can be safely
performed. This is trivial for operations on the same data type, but more complex when mixing different data types.
Sometimes it is not possible to find a common data type that fits all parameters into its range. In such cases the
library tries to find an equivalent set of operations that will lead to the same result while allowing for the largest
numerical range of input parameters. When this fails, the library will return an error specifying that the numerical
limits were exceeded.
