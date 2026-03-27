#!/bin/bash

# 代码格式化脚本

set -e

echo "格式化代码..."

# 查找所有 C/C++ 源文件和头文件
find src include -type f \( -name "*.h" -o -name "*.cpp" -o -name "*.c" \) | while read -r file; do
  echo "Formatting: $file"
  clang-format -i "$file"
done

echo "代码格式化完成！"
