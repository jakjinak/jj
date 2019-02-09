# jj
Is a set of C++ libraries to provide unified usage on Windows and Linux.
## jjbase
Provides basic functionality. Does not require any dependencies beyond compiler.
## jjtest
Provides (yet another) unit test framework.
## jjgui
Provides API for GUI apps. This one depends on (wxWidgets)[http://wxwidgets.org/].
It intends to hide all specifics (of the system and wxWidgets) to the inside of the library.
See the TestAppMain.cpp on how it is being used.

**This is early development prototype so the API MAY CHANGE!**


# Compiler support
- (Windows) VS 2017
- (Linux (and cygwin)) gcc (4.9 or greater)

# How to use
- (VS) just open jj.sln
- (gcc) check `make info` for what's available, but basically `make all` or `make uiall` is the first to do
