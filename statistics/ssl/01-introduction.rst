======================================
半监督学习导论
======================================

问题背景
=========

在机器学习和统计分类问题中，获取大量 **有标签样本** 往往代价高昂——需要专家标注、实验验证
或耗时的人工审核。与此相对，**无标签样本** 的获取成本极低甚至为零。

.. list-table::
   :header-rows: 1

   * - 场景
     - 有标签数据
     - 无标签数据
     - 标注成本
   * - 医疗影像诊断
     - 数十到数百张
     - 数百万张
     - 需要专家放射科医生
   * - 文本分类
     - 几千篇
     - 数十亿网页
     - 需要人工阅读标注
   * - 语音识别
     - 几百小时
     - 数百万小时
     - 需要人工转写
   * - 遥感图像
     - 少量标注区域
     - 全球卫星图像
     - 实地勘测成本高

半监督学习（Semi-Supervised Learning, SSL）正是为了解决这一问题而设计：
**利用少量有标签样本 + 大量无标签样本，构建比仅用有标签样本更好的分类器**。

问题形式化
===========

设有：

- 有标签样本集：:math:`\mathcal{L} = \{(x_1, y_1), (x_2, y_2), \ldots, (x_n, y_n)\}`
- 无标签样本集：:math:`\mathcal{U} = \{x_{n+1}, x_{n+2}, \ldots, x_{n+m}\}`

其中 :math:`n \ll m`（通常 :math:`m \gg n`），:math:`x_i \in \mathbb{R}^d` 为特征向量，
:math:`y_i \in \{1, \ldots, K\}` 为类别标签。

目标是利用 :math:`\mathcal{L} \cup \mathcal{U}` 学习一个分类器
:math:`f: \mathbb{R}^d \to \{1, \ldots, K\}`，使其在测试集上的泛化误差
尽可能小。

一个反直觉的发现
=================

Ahfock 与 McLachlan (2021) 在综述中指出了半监督学习中一个核心且看似矛盾的结果：

.. admonition:: 核心发现
   :class: important

   由**部分分类样本**（部分有标签、部分无标签）构建的分类器，
   其期望错误率**可能低于**由完全有标签样本构建的分类器。

这一结果通过在缺失标签的框架中引入**不可忽略的缺失机制**（non-ignorable missingness
mechanism）来解释。当类别标签的缺失机制满足某些条件时，无标签数据中的信息
可以弥补因不知道缺失标签而损失的信息，从而可能获得更低的错误率。

SSL 的历史脉络
===============

.. list-table::
   :header-rows: 1

   * - 时期
     - 里程碑
     - 贡献者
   * - 1960s
     - 首次提出自训练（self-training）的思想
     - C.A.B. Smith, Hills
   * - 1975
     - 首次理论分析：自训练期望误差率随无标签样本收敛
     - McLachlan
   * - 1977
     - EM 算法提出，为 SSL 提供理论基础
     - Dempster, Laird, Rubin
   * - 1990s
     - 半监督 SVM、协同训练（co-training）等经典方法出现
     - Vapnik, Blum, Mitchell
   * - 2000s
     - 基于图的 SSL 方法、流形正则化
     - Belkin, Niyogi, Sindhwani
   * - 2010s-2020s
     - 深度半监督学习：一致性正则化、伪标签、对比学习
     - 多项工作

.. seealso::

   本文主要参考 Ahfock & McLachlan (2021) *"Semi-Supervised Learning of
   Classifiers from a Statistical Perspective: A Brief Review"* 以及
   Mey & Loog (2019) *"Improvability through Semi-Supervised Learning:
   A Survey of Theoretical Results"*。
