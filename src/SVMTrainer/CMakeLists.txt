add_library(SVMTrainer "")
include_directories("../../include/SVMTrainer"
                    "../../include/HOG")
target_sources(SVMTrainer PRIVATE
               "SVMTrainer.cpp"
               "../../include/SVMTrainer/SVMTrainer.h")
