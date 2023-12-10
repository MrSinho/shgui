cmake_minimum_required(VERSION 3.13)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)



include(phe-docs.cmake)
build_phe_docs()


#        
# PROJECT
#  
project(phe-docs C)