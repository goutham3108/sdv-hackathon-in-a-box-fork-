# Coverage Termination Handler

An issue exists when trying to achieve 100% line coverage with DEATH_TESTS, since DEATH_TESTS ensure that a process is
terminated and thus the needed coverage information is not written onto the filesystem.
The termination handler is a link-only dependency, which will register a custom termination handler, that ensures that
all coverage data is written onto the filesystem before fully terminating.
