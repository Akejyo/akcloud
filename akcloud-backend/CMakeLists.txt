# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

# 包含methods目录和第三方库
include_directories(methods)
include_directories(/usr/include/nlohmann)
include_directories(/usr/include/httplib)

# 链接OpenSSL库
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
link_libraries(${OPENSSL_LIBRARIES})

# 添加源文件
add_executable(server 
    server.cpp
    methods/composer.cpp
    methods/decomposer.cpp
    methods/pack_file.cpp
    methods/file_backup_restore.cpp
    methods/encrypt.cpp
    methods/check_file.cpp
)

# 链接OpenSSL库
target_link_libraries(server OpenSSL::SSL OpenSSL::Crypto)
