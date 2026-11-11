# lib/concurrency
> Your library to manage parallel tasks in ASIL-B contexts

## What is it about?
Our software platform is build for ECUs with higher computational
power. This is most of the time achieved by enable parallel computation.
Within an ASIL-B context, it's not allowed to create threads at any point
in time. This is part of the resource allocation and needs to happen
at the start up of the system. Thus, a more sophisticated management
of threading resources is necessary.

Further, also in ASIL-QM systems, we have to ensure a clean shutdown
of the system. This clean shutdown also means that we need to power off
in time. If an implementor of a thread is not considering to listen
on shutdown requests, threads might block the overall shutdown and
cause a termination of an application. Such scenarios are hard to debug,
thus, this library shall offer a common way to request a stop from threads.

The overall idea is to provide a clean interface to enqueue tasks that can
be executed in parallel. Based on this then different worker strategies could
be implemented. The first of them being a `ThreadPool`, which allocates
a specified number of thread on initialization and joins them on deconstruction.  

## Maintainer

| Domain           | Member        |
| ---------------- |:-------------:|
| Architecture     | t.b.d         |
| System Functions | t.b.d         |
| P&S              | t.b.d         |
| Int-Layer        | t.b.d         |
