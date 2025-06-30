# PKU Campus Navigation

这是一个基于 Qt 开发的北京大学校园导航项目，支持路径规划和途径点管理，能够在校园地图上展示起点、终点及途径点的自定义图标，方便用户进行校园内部导航。

---

## 功能简介

- 加载北京大学校园地图，支持缩放和拖动功能
- 交互界面友好，支持点击节点查看信息
- 支持多点路径规划，包含起点、终点及多个途径点
- 路径搜索采用 A* 算法，保证路径最优
- 支持随机生成吃饭或学习的地点

---

## 项目结构

- `ImageView`：负责地图及节点图标的绘制
- `Graph`：校园地图数据结构，包含节点及边信息
- `MainWindow`：主界面，集成地图视图及路径输入控件
- `ClickNode`: 可点击查看详细信息的节点类
- `InfoDialog`: 可点击节点的介绍弹窗
- `IntroDialog`: 主程序介绍弹窗
- `NavigationInputDialog`: 导航界面弹窗
- `SubstringFilterProxyModel`: 实现地点搜索模糊匹配
- 资源文件夹：包含自定义图标及地图图片

---

## 依赖环境

- Qt 5.12 及以上版本
- C++11 标准

---

## 快速开始

1. 克隆仓库：

   ```bash
   git clone https://github.com/shenshan222/PKU-campus-navigation.git
   cd PKU-campus-navigation
   
2.使用 Qt Creator 打开项目文件 .pro，配置合适的 Qt 版本和编译套件。

3.编译并运行项目。

---

## 演示视频

[点击观看演示视频](https://disk.pku.edu.cn/link/AA31C181FCE64949D3A996137C84F7ACF4
文件名：111-演示.mp4
有效期限：2025-07-30 23:45
提取码：WKDT)

---

## 致谢

感谢项目组所有成员的共同努力！

---

## 联系方式

如有问题或建议，请通过 GitHub Issues 联系我们。

