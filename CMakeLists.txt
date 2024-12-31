cmake_minimum_required(VERSION 3.10)
project(Xylonet)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(src /usr/include/cryptopp)

# Source files
add_executable(xylonet 
    main.cpp
    src/transaction.cpp
    src/user.cpp
    src/network.cpp
    src/wallet.cpp
    src/Xylonet.cpp
    src/DAG.cpp   # Ensure DAG.cpp is included here
)

# Link Crypto++ library
find_library(CRYPTOPP_LIB cryptopp HINTS /usr/lib /usr/local/lib)
target_link_libraries(xylonet ${CRYPTOPP_LIB})
