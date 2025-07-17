# Minimum required CMake version for modern features
cmake_minimum_required(VERSION 3.10)

# Define your project. The first argument is the project name.
# The second argument specifies the languages used (here, C).
project(NetworkApp C)

# Add an executable target.
# This tells CMake to create an executable named 'MySocketClient'
# from the listed source files. CMake will figure out how to compile
# and link 'main.c' and 'socketutils.c' automatically.
add_executable(client
    main.c
    socketutils.c
)
add_executable(server
    server.c
    socketutils.c
)

# Tell the compiler where to find header files.
# The '.' indicates the current directory, which is where your
# 'socketutils.h' file resides. This ensures main.c can find it.
target_include_directories(client PRIVATE .)
target_include_directories(server PRIVATE .)

# (Optional but good practice) Enforce a specific C standard
set_property(TARGET client PROPERTY C_STANDARD 11)
set_property(TARGET client PROPERTY C_STANDARD_REQUIRED YES)