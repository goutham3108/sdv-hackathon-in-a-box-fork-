# lib/concurrency/future

Reimplementation of `std::future` with support for `score::cpp::stop_token` and errors instead of exceptions.

## General Description

To decouple asynchronous operations while maintaining communication between them, several concepts were applied over
time. One such concept is based on promises and futures. A producer owns a promise. From this promise once can create
futures that share a common state with the promise. These futures can be given to consumers. A consumer can wait/check
for availability of the data from the producer through the future. Promises and futures are one-time communication
mechanisms. Meaning, once the state of a promise is set, it can not be modified.

The benefit of futures and promises compared to other synchronization mechanisms like mutexes is, that both producer
and consumer own their part of the promise-future pair and thus must not consider lifetime of the other thread. Further,
any synchronization is handled internally, which improves the resilience of the mechanism against unintended concurrency
issues.

Two APIs are provided to set the state of an `InterruptiblePromise`. `SetValue(...)` is the API for normal operations
where the producer wants to provide data to the consumer(s). The type of data is defined by a class template parameter
of the promise. If there was an error while producing the data, the producer has the option to notify the consumer of
this event. This is done through `SetError(...)`. Since both APIs set the shared state, only one can be called per
promise. If the promise is destructed without calling one of the APIs, this will automatically set the
error `kPromiseBroken`.

To create an `InterruptibleFuture` that shares state with a promise, call `GetInterruptibleFuture()` on the promise.

A consumer can get the shared state via a call to `Get(const score::cpp::stop_token&)` on his future. This API is blocking and
will return a `score::Result` containing either the data or the error set by the producer. Waiting for the promise can be
aborted by setting the stop token. This will return an error `kStopRequested`. Further API to wait for shared state are
`Wait`, `WaitFor` and `WaitUntil`. They do not return the shared state but instead a success flag in form of an
`score::cpp::expected_blank<Error>`. A subsequent call to `Get(...)` in case of a positive return is guaranteed to not block.

## Sharing the future

A producer can also share data with multiple consumers. For this follow the instructions to retrieve an interruptible
future and call `Share()`. This will invalidate your future (call to `Valid()` returns `false` and it no longer shares
state with the promise). In turn, you receive an `InterruptibleSharedFuture` that is copyable.

## Aborting prematurely

In some cases producing the data is expensive and there is the interest to abort the production if there is no consumer
interested on the data anymore. This can be done by attaching a callback at promise side, that is called when the last
associated future was destructed and the state was not yet set. To attach such a callback use `OnAbort(...)`.

## Continuations

Sometimes a consumer does not want to actively wait on the fulfillment of a promise. Thus, APIs like `Wait`, `WaitFor`,
`WaitUntil` and `Get` are not an option. For such cases it is possible to attach a callback to a future that is executed
when the shared state is set. To set such a callback use `Then(...)`. The callback takes as parameter a `score::Result`
that either contains the data or the error within the shared state.

Please be aware, that the callback is executed by the producer. Hence, you must be careful of lifetime assumptions. (
Capturing `this` is in most cases a **very** bad idea)

Further, when attaching a continuation callback, this disables the functionality of `OnAbort(...)`.

## Pitfalls

Promises and futures have certain pitfalls. This section gives some advice how to avoid said pitfalls or at least be
aware of the risks.

### Cyclic dependencies

**TL;DR:** Do not capture the promise or any object storing it in a continuation that is associated to that promise. If
you cannot avoid doing so, make sure to always explicitly set the shared state.

It is quite easy to create memory leaks when not being careful with this library. The problem is that the lifetime
of the shared state is bound to the lifetime of the promise while continuations are attached via a future but stored
within the shared state.

If the lifetime of the promise is bound to an associated continuation, you risk a memory leak. That is, because this
creates a cyclic dependency between shared state and promise via the continuation.

This library tries to minimize the risk as long as the shared state is explicitly set via `SetValue()` or `SetError()`.
When one of these methods is called, all continuations are triggered and then removed from the shared state, which
breaks the cyclic dependency.

But, it is impossible to cover the case where the promise is destructed without explicitly setting the state. Normally,
the destructor of the promise would set the shared state implicitly to an error. But because of the cyclic dependency,
the destructor of the promise is never actually called. As a result, both the promise and the shared state are leaked.

## Class diagram

Diagrams can be found [here](design/Readme.md).
