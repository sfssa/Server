# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/atpdxy/GitHub/Server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/atpdxy/GitHub/Server/build

# Include any dependencies generated for this target.
include CMakeFiles/atpdxy.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/atpdxy.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/atpdxy.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/atpdxy.dir/flags.make

CMakeFiles/atpdxy.dir/atpdxy/log.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/log.cpp.o: ../atpdxy/log.cpp
CMakeFiles/atpdxy.dir/atpdxy/log.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/log.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/log.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/log.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/log.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/log.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/log.cpp

CMakeFiles/atpdxy.dir/atpdxy/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/log.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/log.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/log.cpp > CMakeFiles/atpdxy.dir/atpdxy/log.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/log.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/log.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/log.cpp -o CMakeFiles/atpdxy.dir/atpdxy/log.cpp.s

CMakeFiles/atpdxy.dir/atpdxy/util.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/util.cpp.o: ../atpdxy/util.cpp
CMakeFiles/atpdxy.dir/atpdxy/util.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/util.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/util.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/util.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/util.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/util.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/util.cpp

CMakeFiles/atpdxy.dir/atpdxy/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/util.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/util.cpp > CMakeFiles/atpdxy.dir/atpdxy/util.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/util.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/util.cpp -o CMakeFiles/atpdxy.dir/atpdxy/util.cpp.s

CMakeFiles/atpdxy.dir/atpdxy/config.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/config.cpp.o: ../atpdxy/config.cpp
CMakeFiles/atpdxy.dir/atpdxy/config.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/config.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/config.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/config.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/config.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/config.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/config.cpp

CMakeFiles/atpdxy.dir/atpdxy/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/config.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/config.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/config.cpp > CMakeFiles/atpdxy.dir/atpdxy/config.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/config.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/config.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/config.cpp -o CMakeFiles/atpdxy.dir/atpdxy/config.cpp.s

CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.o: ../atpdxy/thread.cpp
CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/thread.cpp

CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/thread.cpp > CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/thread.cpp -o CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.s

CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.o: ../atpdxy/mutex.cpp
CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/mutex.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/mutex.cpp

CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/mutex.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/mutex.cpp > CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/mutex.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/mutex.cpp -o CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.s

CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.o: ../atpdxy/fiber.cpp
CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/fiber.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/fiber.cpp

CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/fiber.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/fiber.cpp > CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/fiber.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/fiber.cpp -o CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.s

CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.o: ../atpdxy/scheduler.cpp
CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/scheduler.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/scheduler.cpp

CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/scheduler.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/scheduler.cpp > CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/scheduler.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/scheduler.cpp -o CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.s

CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.o: ../atpdxy/iomanager.cpp
CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/iomanager.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/iomanager.cpp

CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/iomanager.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/iomanager.cpp > CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/iomanager.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/iomanager.cpp -o CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.s

CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.o: ../atpdxy/timer.cpp
CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/timer.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/timer.cpp

CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/timer.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/timer.cpp > CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/timer.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/timer.cpp -o CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.s

CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.o: ../atpdxy/hook.cpp
CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/hook.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/hook.cpp

CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/hook.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/hook.cpp > CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/hook.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/hook.cpp -o CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.s

CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.o: CMakeFiles/atpdxy.dir/flags.make
CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.o: ../atpdxy/fd_manager.cpp
CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.o: CMakeFiles/atpdxy.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/fd_manager.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.o -MF CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.o.d -o CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.o -c /home/atpdxy/GitHub/Server/atpdxy/fd_manager.cpp

CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/fd_manager.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/atpdxy/GitHub/Server/atpdxy/fd_manager.cpp > CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.i

CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"atpdxy/fd_manager.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/atpdxy/GitHub/Server/atpdxy/fd_manager.cpp -o CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.s

# Object files for target atpdxy
atpdxy_OBJECTS = \
"CMakeFiles/atpdxy.dir/atpdxy/log.cpp.o" \
"CMakeFiles/atpdxy.dir/atpdxy/util.cpp.o" \
"CMakeFiles/atpdxy.dir/atpdxy/config.cpp.o" \
"CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.o" \
"CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.o" \
"CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.o" \
"CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.o" \
"CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.o" \
"CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.o" \
"CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.o" \
"CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.o"

# External object files for target atpdxy
atpdxy_EXTERNAL_OBJECTS =

../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/log.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/util.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/config.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/thread.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/mutex.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/fiber.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/scheduler.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/iomanager.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/timer.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/hook.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/atpdxy/fd_manager.cpp.o
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/build.make
../lib/libatpdxy.so: CMakeFiles/atpdxy.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/atpdxy/GitHub/Server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Linking CXX shared library ../lib/libatpdxy.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/atpdxy.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/atpdxy.dir/build: ../lib/libatpdxy.so
.PHONY : CMakeFiles/atpdxy.dir/build

CMakeFiles/atpdxy.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/atpdxy.dir/cmake_clean.cmake
.PHONY : CMakeFiles/atpdxy.dir/clean

CMakeFiles/atpdxy.dir/depend:
	cd /home/atpdxy/GitHub/Server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/atpdxy/GitHub/Server /home/atpdxy/GitHub/Server /home/atpdxy/GitHub/Server/build /home/atpdxy/GitHub/Server/build /home/atpdxy/GitHub/Server/build/CMakeFiles/atpdxy.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/atpdxy.dir/depend

