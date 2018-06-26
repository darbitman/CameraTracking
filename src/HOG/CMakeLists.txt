add_library(HOG "")
include_directories("../../include/HOG")
target_sources(HOG PRIVATE
               "HOG.cpp"
               "../../include/HOG/HOG.h")
