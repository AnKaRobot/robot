# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/xavier/gittest/robot/xiong/normalization

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/xavier/gittest/robot/xiong/normalization-build

# Include any dependencies generated for this target.
include CMakeFiles/normalization.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/normalization.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/normalization.dir/flags.make

CMakeFiles/normalization.dir/normalization.cpp.o: CMakeFiles/normalization.dir/flags.make
CMakeFiles/normalization.dir/normalization.cpp.o: /home/xavier/gittest/robot/xiong/normalization/normalization.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/xavier/gittest/robot/xiong/normalization-build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/normalization.dir/normalization.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/normalization.dir/normalization.cpp.o -c /home/xavier/gittest/robot/xiong/normalization/normalization.cpp

CMakeFiles/normalization.dir/normalization.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/normalization.dir/normalization.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/xavier/gittest/robot/xiong/normalization/normalization.cpp > CMakeFiles/normalization.dir/normalization.cpp.i

CMakeFiles/normalization.dir/normalization.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/normalization.dir/normalization.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/xavier/gittest/robot/xiong/normalization/normalization.cpp -o CMakeFiles/normalization.dir/normalization.cpp.s

CMakeFiles/normalization.dir/normalization.cpp.o.requires:
.PHONY : CMakeFiles/normalization.dir/normalization.cpp.o.requires

CMakeFiles/normalization.dir/normalization.cpp.o.provides: CMakeFiles/normalization.dir/normalization.cpp.o.requires
	$(MAKE) -f CMakeFiles/normalization.dir/build.make CMakeFiles/normalization.dir/normalization.cpp.o.provides.build
.PHONY : CMakeFiles/normalization.dir/normalization.cpp.o.provides

CMakeFiles/normalization.dir/normalization.cpp.o.provides.build: CMakeFiles/normalization.dir/normalization.cpp.o

# Object files for target normalization
normalization_OBJECTS = \
"CMakeFiles/normalization.dir/normalization.cpp.o"

# External object files for target normalization
normalization_EXTERNAL_OBJECTS =

normalization: CMakeFiles/normalization.dir/normalization.cpp.o
normalization: CMakeFiles/normalization.dir/build.make
normalization: /usr/lib/libopencv_videostab.so.2.4.8
normalization: /usr/lib/libopencv_video.so.2.4.8
normalization: /usr/lib/libopencv_ts.a
normalization: /usr/lib/libopencv_superres.so.2.4.8
normalization: /usr/lib/libopencv_stitching.so.2.4.8
normalization: /usr/lib/libopencv_photo.so.2.4.8
normalization: /usr/lib/libopencv_ocl.so.2.4.8
normalization: /usr/lib/libopencv_objdetect.so.2.4.8
normalization: /usr/lib/libopencv_nonfree.so.2.4.8
normalization: /usr/lib/libopencv_ml.so.2.4.8
normalization: /usr/lib/libopencv_legacy.so.2.4.8
normalization: /usr/lib/libopencv_imgproc.so.2.4.8
normalization: /usr/lib/libopencv_highgui.so.2.4.8
normalization: /usr/lib/libopencv_gpu.so.2.4.8
normalization: /usr/lib/libopencv_flann.so.2.4.8
normalization: /usr/lib/libopencv_features2d.so.2.4.8
normalization: /usr/lib/libopencv_core.so.2.4.8
normalization: /usr/lib/libopencv_contrib.so.2.4.8
normalization: /usr/lib/libopencv_calib3d.so.2.4.8
normalization: /lib/libGLU.so
normalization: /lib/libGL.so
normalization: /lib/libSM.so
normalization: /lib/libICE.so
normalization: /lib/libX11.so
normalization: /lib/libXext.so
normalization: /usr/lib/libopencv_nonfree.so.2.4.8
normalization: /usr/lib/libopencv_ocl.so.2.4.8
normalization: /usr/lib/libopencv_gpu.so.2.4.8
normalization: /usr/lib/libopencv_photo.so.2.4.8
normalization: /usr/lib/libopencv_objdetect.so.2.4.8
normalization: /usr/lib/libopencv_legacy.so.2.4.8
normalization: /usr/lib/libopencv_video.so.2.4.8
normalization: /usr/lib/libopencv_ml.so.2.4.8
normalization: /usr/lib/libopencv_calib3d.so.2.4.8
normalization: /usr/lib/libopencv_features2d.so.2.4.8
normalization: /usr/lib/libopencv_highgui.so.2.4.8
normalization: /usr/lib/libopencv_imgproc.so.2.4.8
normalization: /usr/lib/libopencv_flann.so.2.4.8
normalization: /usr/lib/libopencv_core.so.2.4.8
normalization: CMakeFiles/normalization.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable normalization"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/normalization.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/normalization.dir/build: normalization
.PHONY : CMakeFiles/normalization.dir/build

CMakeFiles/normalization.dir/requires: CMakeFiles/normalization.dir/normalization.cpp.o.requires
.PHONY : CMakeFiles/normalization.dir/requires

CMakeFiles/normalization.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/normalization.dir/cmake_clean.cmake
.PHONY : CMakeFiles/normalization.dir/clean

CMakeFiles/normalization.dir/depend:
	cd /home/xavier/gittest/robot/xiong/normalization-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xavier/gittest/robot/xiong/normalization /home/xavier/gittest/robot/xiong/normalization /home/xavier/gittest/robot/xiong/normalization-build /home/xavier/gittest/robot/xiong/normalization-build /home/xavier/gittest/robot/xiong/normalization-build/CMakeFiles/normalization.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/normalization.dir/depend

