# 使用Ubuntu 22.04作为基础镜像
FROM ubuntu:22.04

# 安装构建工具和依赖
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    nodejs \
    npm \
    libssl-dev \
    wget

# 下载并安装nlohmann/json
RUN mkdir -p /usr/include/nlohmann && \
    wget -qO- https://github.com/nlohmann/json/releases/download/v3.10.5/json.hpp -O /usr/include/nlohmann/json.hpp

# 下载并安装httplib
RUN mkdir -p /usr/include/httplib && \
    wget -qO- https://github.com/yhirose/cpp-httplib/raw/master/httplib.h -O /usr/include/httplib/httplib.h

# 设置工作目录
WORKDIR /app

# 复制前端应用的package.json和package-lock.json
COPY ./akcloud/package*.json ./akcloud/

# 安装前端应用依赖
RUN cd akcloud && npm install

# 复制前端应用文件
COPY ./akcloud ./akcloud

# 构建前端应用
RUN cd akcloud && npm run build

# 复制后端应用的CMakeLists.txt和源文件
COPY ./akcloud-backend/CMakeLists.txt ./akcloud-backend/
COPY ./akcloud-backend/server.cpp ./akcloud-backend/
COPY ./akcloud-backend/methods ./akcloud-backend/methods

# 创建构建目录并构建后端项目
RUN mkdir -p akcloud-backend/build && cd akcloud-backend/build && cmake .. && make

# 暴露前后端应用运行端口（根据你的应用配置调整）
EXPOSE 3000 8080

# 启动脚本
CMD ["sh", "-c", "cd /app/akcloud && npm start & cd /app/akcloud-backend/build && ./server"]
