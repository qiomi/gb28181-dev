#!/bin/bash

# GB28181-Dev 构建脚本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 默认参数
BUILD_TYPE="Release"
CLEAN=false
INSTALL=false
EXTRA_ARGS=""

# 使用帮助
show_help() {
  cat << EOF
Usage: $0 [OPTIONS]

构建 GB28181-Dev 库

选项:
  -t, --type TYPE       构建类型 (Debug|Release|RelWithDebInfo), 默认：Release
  -c, --clean           清理构建目录
  -i, --install         安装到系统目录
  -h, --help            显示此帮助信息

示例:
  $0                    # 使用默认配置构建
  $0 -t Debug           # Debug 模式构建
  $0 -c                 # 清理并重新构建
  $0 -i                 # 构建并安装

EOF
}

# 解析参数
while [[ $# -gt 0 ]]; do
  case $1 in
    -t|--type)
      BUILD_TYPE="$2"
      shift 2
      ;;
    -c|--clean)
      CLEAN=true
      shift
      ;;
    -i|--install)
      INSTALL=true
      shift
      ;;
    -h|--help)
      show_help
      exit 0
      ;;
    *)
      echo "未知选项：$1"
      show_help
      exit 1
      ;;
  esac
done

# 验证构建类型
if [[ ! "$BUILD_TYPE" =~ ^(Debug|Release|RelWithDebInfo)$ ]]; then
  echo -e "${RED}错误：无效的构建类型 '$BUILD_TYPE'${NC}"
  echo "有效的构建类型：Debug, Release, RelWithDebInfo"
  exit 1
fi

# 创建或清理构建目录
BUILD_DIR="build"
if [ "$CLEAN" = true ] && [ -d "$BUILD_DIR" ]; then
  echo -e "${YELLOW}清理构建目录...${NC}"
  rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 配置 CMake
echo -e "${GREEN}配置 CMake (构建类型：$BUILD_TYPE)...${NC}"
cmake .. \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DGB28181_BUILD_EXAMPLES=ON \
  -DGB28181_BUILD_TESTS=ON \
  $EXTRA_ARGS

# 编译
echo -e "${GREEN}开始编译...${NC}"
make -j$(nproc)

# 运行测试
if [ "$BUILD_TYPE" = "Debug" ] || [ "$BUILD_TYPE" = "RelWithDebInfo" ]; then
  echo -e "${GREEN}运行测试...${NC}"
  ctest --output-on-failure
fi

# 安装
if [ "$INSTALL" = true ]; then
  echo -e "${GREEN}安装到系统目录...${NC}"
  sudo make install
fi

echo -e "${GREEN}构建完成！${NC}"
echo ""
echo "编译数据库已生成：$BUILD_DIR/compile_commands.json"
echo "可以使用 ln -s build/compile_commands.json . 链接到项目根目录"
