Creating replayable logs
========================

SMTK does not yet provide undo/redo functionality for modeling operations
because implementing this can be very difficult given the lack of support
in underlying modeling kernels that SMTK interfaces with.
However, SMTK does provide a rich mechanism for logging modeling operations
that can be used to replay a session to an intermediate state.

The :smtk:`OperatorLog` class is an abstract base class that observes
all of the operations invoked across all the :smtk:`sessions <Session>`
which belong to a given model :smtk:`Manager`.
It calls pure virtual methods (1) when an operator is executed and (2) when
results from the operation are returned.
These methods can be overridden by subclasses to flush a log to disk,
to update an in-memory log, or any other behavior to support replay.

One shortcoming of many logging systems is that they capture low-level
information but not the user's intent.
This can make it hard for users to modify the log for use in a macro
or when debugging.
However, capturing user intent is a task that requires knowledge of
the application logic that SMTK, as a framework supporting applications,
does not have.
To resolve this, the OperatorLog class accepts **hints** from the
application during logging.

Recall that in SMTK, :smtk:`Operators <Operator>` are defined by an
:smtk:`Attribute` instance called the operator's specification.
As an operation's specification is being input into the application,
the logger can be notified of how :smtk:`Item` values in the specification
were provided by the user (e.g., was an entity picked in a 3-D view, or
chosen by searching for a particular name, or picked from a tree view
by its relationship to a parent container?).
Each item of an attribute can have 0 or 1 hints.
A hint is an SMTK attribute held in a separate attribute :smtk:`System`
that is owned by the OperatorLog.
Applications should use the log to define and construct attributes
and relate them to items of an operator's specification (held in
the :smtk:`Session's <Session>` attribute system) by calling methods
on the OperatorLog.
Then instances of the OperatorLog class are free to make use of
this information when generating logs.

Note that the OperatorLog is not required to make use of hints;
because logs may be used for many purposes, not all subclasses
of OperatorLog will do so.
It may be that an application provides its own subclass of
OperatorLog to handle hints that are application-specific.

Besides the base OperatorLog class, SMTK also provides a Python subclass
named PythonOperatorLog that generates Python scripts which can be
used to replay a sequence of modeling operations.
