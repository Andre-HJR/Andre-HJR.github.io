================================
现代深度半监督学习方法
================================

深度学习时代的半监督学习以三种核心技术为基础：**一致性正则化**、**熵正则化**
和**数据增强**。这些方法通常组合使用，在各类基准上取得了显著效果。

一致性正则化
==============

核心思想
---------

一致性正则化源于平滑性假设：对无标签样本施加小的扰动，其预测应保持不变。

.. math::

   \mathcal{L}_{\text{consist}} = \mathbb{E}_{x \in \mathcal{U}} \,
   \left[ \| f_\theta(x) - f_\theta(\tilde{x}) \|^2 \right]

其中 :math:`\tilde{x}` 是对 :math:`x` 施加扰动后的版本。

经典方法演进
--------------

.. list-table::
   :header-rows: 1

   * - 方法
     - 年度
     - 核心思路
     - 扰动方式
   * - Π-Model
     - 2016
     - 两次前向传播的一致性
     - Dropout + 数据增强
   * - Temporal Ensembling
     - 2016
     - EMA 累积预测作为目标
     - 指数移动平均
   * - Mean Teacher
     - 2017
     - 学生-教师模型框架
     - 教师参数 = EMA(学生参数)
   * - UDA
     - 2019
     - 使用强数据增强
     - RandAugment + BackTranslation
   * - FixMatch
     - 2020
     - 弱增强产生伪标签，强增强执行一致性
     - 弱/强增强组合

FixMatch：一个统一框架
------------------------

.. code-block:: text

   输入 x (无标签)
         │
         ├── 弱增强 → 预测 → 伪标签（置信度高才保留）
         │                           │
         └── 强增强 → 预测 ──────────┘
                                    │
                           两者应一致（交叉熵损失）

.. code-block:: text

   总损失 = 有标签样本的监督损失
          + λ × 无标签样本的一致性损失

   监督损失:  交叉熵(有标签 x, y)
   一致性损失: 交叉熵(强增强 x, 弱增强 x 的伪标签)

.. math::

   \mathcal{L} = \underbrace{\frac{1}{n} \sum_{i=1}^n H(y_i, f_\theta(x_i))}_{\text{监督损失}}
   + \lambda \underbrace{\frac{1}{m} \sum_{i=n+1}^{n+m} \mathbb{1}_{(\max(p_i) > \tau)} H(\hat{y}_i, f_\theta(\tilde{x}_i))}_{\text{一致性损失}}

其中：

- :math:`\tau` 是置信度阈值
- :math:`\hat{y}_i` 是基于弱增强预测的伪标签
- :math:`\tilde{x}_i` 是强增强版本

.. tip::
   FixMatch 的核心设计是仅在模型对弱增强版本**高度自信**（:math:`\max(p) > \tau`）时
   才将伪标签用于一致性训练。这一机制有效防止了错误累积。

熵正则化
=========

熵正则化通过最小化预测的熵来鼓励模型做出"自信"的预测。

.. math::

   \mathcal{L}_{\text{entropy}} = -\frac{1}{m} \sum_{i=n+1}^{n+m} \sum_{k=1}^K
   f_\theta(x_i)_k \, \log f_\theta(x_i)_k

.. code-block::

   高熵 (不自信)              低熵 (自信)
   [0.25, 0.25, 0.25, 0.25]  [0.95, 0.02, 0.01, 0.02]
   H = -4 × 0.25 log 0.25    H = -(0.95 log 0.95 + 3×0.02 log 0.02)
     = 0.602                    = 0.132

.. note::
   熵正则化相当于在决策边界的**低密度分离假设**上施加软约束——
   要使预测熵小，决策边界必须远离数据点。

虚拟对抗训练（VAT）
----------------------

VAT 将对抗训练的思想引入 SSL：

.. math::

   \mathcal{L}_{\text{VAT}} = \mathbb{E}_{x \in \mathcal{U}} \,
   \max_{\|\delta\| \leq \epsilon} \text{KL}[f_\theta(x) \| f_\theta(x + \delta)]

VAT 寻找最"对抗性"的扰动方向并强制预测一致性，
比随机扰动更强效。

数据增强在 SSL 中的角色
==========================

数据增强在 SSL 中扮演双重角色：

.. list-table::
   :header-rows: 1

   * - 角色
     - 说明
     - 示例
   * - 扩充有标签数据
     - 增加训练样本的多样性
     - 翻转、裁剪、颜色抖动
   * - 一致性正则化的锚点
     - 定义"哪些变换不影响语义"
     - RandAugment, CTAugment

.. important::
   SSL 中数据增强编码了关于任务不变性的**先验知识**：
   增强后的图像应该保持类别不变。

   好的增强策略对 SSL 的成功至关重要。UDA 和 FixMatch
   的成功很大程度上归功于**强数据增强**（如 RandAugment）。

伪标签方法
============

伪标签（Pseudo-Label）是自训练在深度网络中的现代版本。

.. code-block:: text

   核心改进：
   1. 使用网络的 softmax 输出作为伪标签
   2. 仅对最大概率超过阈值 τ 的样本使用
   3. 将伪标签损失与熵正则化结合

   η(t) × 无标签损失 + 有标签损失

   η(t) 是随训练轮次增长的权重系数（先预热，再增加无标签损失权重）

对比学习与 SSL
================

对比学习（Contrastive Learning）的最新进展也被引入 SSL：

.. code-block:: text

   1. 无监督预训练阶段：
      使用 SimCLR / MoCo 等对比学习方法
      在大量无标签数据上学习特征表示

   2. 微调阶段：
      在少量有标签数据上微调分类器

   → 这种方法在极小标注比例下（每类 1-10 张）效果显著

深度 SSL 方法的共同框架
=========================

.. math::

   \mathcal{L}_{\text{total}} = \mathcal{L}_{\text{sup}} + \lambda(t) \cdot \mathcal{L}_{\text{unsup}}

其中 :math:`\lambda(t)` 是时间相关的权重函数：

.. code-block:: text

   常见退火策略：
   - 线性上升：λ(t) = t / T_max × λ_max
   - 高斯预热：λ(t) = exp[-5(1 - t/T_max)²] × λ_max
   - 阈值触发：当验证集性能超过某个阈值后启用

   λ(t) 防止早期网络预测完全随机时引入噪声。
