# TEST - Geant4 BGO 模拟与 ROOT 输出

本项目使用 Geant4 构建一个简化的模拟：
- 探测器：一个 BGO 晶体（2.5 cm × 2.5 cm × 60 cm），位于空气世界体中心。
- 粒子源：1 GeV γ（gamma），沿 -Z 方向入射。
- 输出：每步、每事件、以及按 Z 深度分层（60 bins，-30 cm 到 30 cm）的能量沉积到 ROOT 文件。

## 依赖
- Geant4（含 OGL 可视化后端）
- ROOT（用于 TFile/TTree）
- CMake 与 C++17 编译器

## 构建
- 典型 CMake 流程：
  - mkdir build && cd build
  - cmake .. && make
- 可执行程序默认输出到 build/，运行后生成 output.root（见运行说明）。

## 运行
- 交互式：
  - 程序会查找 ../macros/init.mac；若存在则执行宏初始化，否则启用默认 OGL 可视化。
  - 可视化中不同粒子轨迹按粒子类型着色。
- 批处理：
  - ./yourExecutable /path/to/your.mac
- 运行结束后输出 ROOT 文件路径提示：output.root。

## 探测器与可视化
- 世界材料：G4_AIR。
- BGO 材料：Bi4Ge3O12，密度 7.12 g/cm3。
- BGO 可视化透明度：
  - 通过 G4VisAttributes/G4Colour 设置（部分后端支持 alpha），如 OGL。
  - 若透明度未生效，可切换/确认 OGL 后端或调整 alpha 值。

## 粒子源
- 每事件发射 1 个 γ，能量 1 GeV，位置 (0,0,50 cm)，方向 (0,0,-1)。

## 单位约定
- 每步能量沉积 edep 以 MeV 写入 ROOT。
- 事件总能量、深度分布数组同为 MeV。
- 位置以 mm，动量以 MeV，时间以 ns，步长以 mm 写入。

## ROOT 输出结构
- output.root 包含：
  - stepTree（每步）：
    - runID, eventID, trackID, stepNo, pdg
    - x,y,z[mm], px,py,pz[MeV], ekin[MeV], time[ns], stepLen[mm], edep[MeV]
    - volume, material, process（固定长度 C 字符串）
  - eventTree（每事件）：
    - totalEnergyDeposit[MeV]
    - weightedPosX/Y/Z（能量加权位置，单位与几何一致）
  - depthEventTree（每事件分层能量沉积）：
    - binEdep[MeV]，长度 kNBins=60，对应 Z∈[-30,30]cm，bin 宽度 1 cm
  - depthMeanTree（每 run 均值与近似 dEdz）：
    - meanEdep[i] = <E_dep(z_i)>[MeV]
    - dEdz[i] = meanEdep[i] / Δz，单位约为 MeV/cm

## 能量累加逻辑（重要变更）
- 仅在“非空气”材料中进行统计累加：
  - 当当前步材料名不为 "G4_AIR" 时，才累加：
    - 事件总能量（AddEnergyDeposit）
    - 能量加权位置（AddPosition/AddWeightSum）
    - 深度分布薄层能量（AddBinDeposit）
- 每步依然完整写入 stepTree，便于后处理分析。

## 常见问题
- 透明度不生效：确认使用 OGL 可视化驱动；不同后端对 alpha 支持不同。
- init.mac 未找到：程序会回退到默认可视化设置并提示路径；可通过命令行传入宏。
- 多线程运行：本项目 RootIO 在 RunAction 中以 UPDATE 打开文件；并发写入需额外同步管理（当前示例默认单线程或串行写入）。

## 分析提示
- 深度分布：使用 depthEventTree 的 binEdep 逐事件查看能量沉积随 Z 的分布；使用 depthMeanTree 的 meanEdep/dEdz 做平均曲线。
- 事件加权位置：当 fWeightSum>0 时，weightedPos 为能量加权几何中心。

## 说明
- 本项目是作为普通天文学期末作业的一个小小尝试，用于展示 Geant4 与 ROOT 的基本用法，并展示 BGO 晶体对 γ 的能量沉积。
- 鉴于笔者能力有限，暂时做不出更多的分析，只能将该项目停留在可视化层面
- build中包含了模拟的信息，一并上传