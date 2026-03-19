# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/MazeMotion_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/MazeMotion_autogen.dir/ParseCache.txt"
  "MazeMotion_autogen"
  )
endif()
