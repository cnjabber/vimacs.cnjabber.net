3月14日日报
===========

今天对执行单元和功能单元的源码进行了简单的阅读，熟悉了一点Scala/Chisel的语法。

execute.scala 中包含的 class
----------------------------

execute.scala中定义了以下类：

- ExeUnitResp
- FFlagsResp: 只在浮点运算相关的模块找到它的使用
- ExecutionUnitIO
- ExeUnit classes: 在execution_units.scala被使用，在不同的 ISSUE_WIDTH 配置下，ExecutionUnits会使用其中 ISSUE_WIDTH 数量的执行单元

  - ALUExeUnit
  - FDivSqrtExeUnit
  - MemExeUnit
  - ALUMemExeUnit

容易找到其中的一些类包含关系：

- ExeUnitResp: 在ExecutionUnitIO.resp 和 RobIo.wb_resps (rob.scala) 中被使用
- ExecutionUnitIO: ExecutionUnit.io为它的一个实例

functional_unit.scala
---------------------

该源文件定义了 FunctionalUnitIo, GetPredictionInfo, FuncUnitReq, FuncUnitResp, BypassData, BrResolutionInfo, BranchUnitResp 这些结构类，它们会和其他部件连接。

之后是各种功能单元的定义。和specification中说的一样，功能单元是按照抽象层次分类定义的。

- 最高层是 **abstract class FunctionalUnit**,其中有 **FunctionalUnitIo** 存放功能单元中需要包含的数据和信号。功能单元能分为流水和非流水两种，用 **abstract class PipelinedFunctionalUnit** 和 **abstract class UnPipelinedFunctionalUnit** 两种抽象类表示。
- 流水功能部件包含：

  - ALUUnit
  - MemAddrCalcUnit
  - FPUUnit
  - PipelinedMulUnit
- 非流水功能部件包含：

  - MuldivUnit
