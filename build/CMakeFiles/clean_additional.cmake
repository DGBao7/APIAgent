# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "APIAgent_autogen"
  [[CMakeFiles\APIAgent_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\APIAgent_autogen.dir\ParseCache.txt]]
  )
endif()
