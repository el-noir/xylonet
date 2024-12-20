# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Define the project name and version
project(HybridBlockchain VERSION 1.0)

# Specify the C++ standard to use
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory for header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
set(SOURCES
    src/Transaction.cpp
    src/Wallet.cpp
    src/DAGGraph.cpp
    src/HybridConsensus.cpp
    src/Validator.cpp
    src/RSAEncryption.cpp
    main.cpp
)

# Add the executable for the main program
add_executable(${PROJECT_NAME} ${SOURCES})

# If you're using OpenSSL for RSA Encryption (in case you have the OpenSSL dependencies)
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)

# Optional: Create an installation target (if you plan to install the project)
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# install(DIRECTORY include/ DESTINATION include)

# Optional: If you want to use multi-threading (for example, if required by your code)
# find_package(Threads REQUIRED)
# target_link_libraries(${PROJECT_NAME} Threads::Threads)
