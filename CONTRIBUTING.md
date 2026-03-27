# 贡献指南

首先，感谢您考虑为 GB28181-Dev 项目做出贡献！

## 如何贡献

### 报告 Bug

如果您发现了 Bug，请创建一个 Issue 并包含以下信息：

1. 您的操作系统和编译器版本
2. 详细的复现步骤
3. 预期的行为和实际的行为
4. 如果可能，提供最小复现代码

### 提出新功能

如果您有新功能的想法，请先创建一个 Issue 讨论：

1. 详细描述功能需求
2. 说明使用场景
3. 可能的实现方案

### 提交代码

1. Fork 本仓库
2. 创建您的特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交您的更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启一个 Pull Request

### 代码规范

- 遵循 Google C++ Style
- 使用 4 空格缩进
- 大括号换行
- 使用 `clang-format` 格式化代码

在提交前，请运行：

```bash
./scripts/format.sh
```

### 测试

确保您的修改通过了所有测试：

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
ctest --output-on-failure
```

## 开发环境设置

### 依赖

- CMake >= 3.14
- C++17 编译器 (g++ >= 7 或 clang++ >= 5)
- pthread

### 克隆项目

```bash
git clone https://github.com/qiomi/gb28181-dev.git
cd gb28181-dev
```

### 构建

```bash
./scripts/build.sh -t Debug
```

## Pull Request 流程

1. PR 应该有清晰的标题和描述
2. 链接到相关的 Issue（如果适用）
3. 通过 CI 检查
4. 至少需要一个维护者的审查批准

## 社区准则

- 保持友好和尊重
- 对事不对人
- 欢迎各种水平的贡献者
- 互相帮助，共同成长

## 许可证

通过贡献代码，您同意您的贡献遵循本项目的 MIT 许可证。

## 联系方式

- GitHub Issues: https://github.com/qiomi/gb28181-dev/issues
- Email: [你的邮箱]

再次感谢您的贡献！
