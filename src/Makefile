###
### Files
###

EXE = a

OBJS = main.o benchmark.o


###
### Rules
###

all: $(EXE) .depend

clean: 
	$(RM) *.o .depend a


###
### Compiler:
###

CXX = g++
# CXX = g++-4.2
# CXX = icpc


###
### Dependencies
###

$(EXE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

.depend:
	$(CXX) -MM $(OBJS:.o=.cpp) > $@

include .depend


###
### Compiler and linker switches
###

# Enable/disable debugging:

CXXFLAGS += -DNDEBUG


# Compile with full warnings, and symbol names

CXXFLAGS += -Wall -g


# General optimization flags.  Note that -O2 might be faster than -O3 on some
# systems; this requires testing.

CXXFLAGS += -O3 -fno-exceptions -fomit-frame-pointer -fno-rtti -fstrict-aliasing

# Compiler optimization flags for the Intel C++ compiler in Mac OS X:

# CXXFLAGS += -mdynamic-no-pic -no-prec-div -ipo -static -xP


# Profiler guided optimization with the Intel C++ compiler.  To use it, first
# create the directory ./profdata if it does not already exist, and delete its
# contents if it does exist.  Then compile with -prof_gen, and run the 
# resulting binary for a while (for instance, do ./glaurung bench 128 1, and
# wait 15 minutes for the benchmark to complete).  Then do a 'make clean', and 
# recompile with -prof_use.

# CXXFLAGS += -prof_gen -prof_dir ./profdata
# CXXFLAGS += -prof_use -prof_dir ./profdata


# Profiler guided optimization with GCC.  I've never been able to make this 
# work.

# CXXFLAGS += -fprofile-generate
# LDFLAGS += -fprofile-generate
# CXXFLAGS += -fprofile-use
# CXXFLAGS += -fprofile-use


# General linker flags

LDFLAGS += -lm -pthread


# Compiler switches for generating binaries for various CPUs in Mac OS X.
# Note that 'arch ppc' and 'arch ppc64' only works with g++, and not with
# the intel compiler.

# CXXFLAGS += -arch ppc
# CXXFLAGS += -arch ppc64
# CXXFLAGS += -arch i386
# CXXFLAGS += -arch x86_64
# LDFLAGS += -arch ppc
# LDFLAGS += -arch ppc64
# LDFLAGS += -arch i386
# LDFLAGS += -arch x86_64


# Backwards compatibility with Mac OS X 10.4 when compiling under 10.5 with 
# GCC 4.0.  I haven't found a way to make it work with GCC 4.2.

# CXXFLAGS += -isysroot /Developer/SDKs/MacOSX10.4u.sdk
# CXXFLAGS += -mmacosx-version-min=10.4
# LDFLAGS += -isysroot /Developer/SDKs/MacOSX10.4u.sdk
# LDFLAGS += -Wl,-syslibroot /Developer/SDKs/MacOSX10.4u.sdk
# LDFLAGS += -mmacosx-version-min=10.4


# Backwards compatibility with Mac OS X 10.4 when compiling with ICC.  Doesn't
# work yet. :-(

# CXXFLAGS += -DMAC_OS_X_VERSION_MIN_REQUIRED=1040
# CXXFLAGS += -DMAC_OS_X_VERSION_MAX_ALLOWED=1040
# CXXFLAGS += -D__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__=1040
# CXXFLAGS += -F/Developer/SDKs/MacOSX10.4u.sdk/
# LDFLAGS += -Wl,-syslibroot -Wl,/Developer/SDKs/MacOSX10.4u.sdk
