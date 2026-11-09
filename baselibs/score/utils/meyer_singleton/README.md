# Meyer Singleton

This package provides a static helper class which allows creating a singleton in a thread safe manner.

The potential issues associated with creating singletons in a multithreaded context is described in Ticket-247205.
The root cause appears to be a compiler bug in gcc (which is not present in clang): https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99613/
Once this compiler bug is fixed, we could consider deleting this utility.

A full description of the problem can be found in the class docstring of MeyerSingleton.
