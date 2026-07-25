================================
统计框架：缺失标签机制
================================

半监督学习的统计本质可以看作一个**缺失数据问题**——类标签 :math:`y` 对部分样本是缺失的。

缺失机制框架（Rubin, 1976）
=============================

Rubin (1976) 提出了分析缺失数据问题的经典框架，将缺失机制分为三类。

记标签指示变量 :math:`r_i`：

.. math::

   r_i =
   \begin{cases}
   1 & \text{若 } y_i \text{ 被观测到（有标签）} \\
   0 & \text{若 } y_i \text{ 缺失（无标签）}
   \end{cases}

完全随机缺失（MCAR）
---------------------

.. math::

   P(r_i = 1 \mid x_i, y_i) = P(r_i = 1)

标签的缺失**完全独立于特征和标签本身**。例如：随机选择一部分样本进行标注。

.. note::
   MCAR 是最强的假设，在 SSL 中通常不成立，但也最容易处理。

随机缺失（MAR）
----------------

.. math::

   P(r_i = 1 \mid x_i, y_i) = P(r_i = 1 \mid x_i)

标签的缺失仅依赖于**观测到的特征** :math:`x_i`，而与标签值 :math:`y_i` 无关。
例如：在某个特征值范围内，样本更可能被标注。

非随机缺失（NMAR / Non-ignorable）
------------------------------------

.. math::

   P(r_i = 1 \mid x_i, y_i) \neq P(r_i = 1 \mid x_i)

标签的缺失**依赖于标签本身**——即使控制了 :math:`x_i`，缺失概率仍与 :math:`y_i` 有关。

.. important::
   Ahfock & McLachlan 指出：当缺失机制是**不可忽略的**（non-ignorable）时，
   无标签数据可以提供额外的信息来补偿标签缺失带来的损失，甚至实现比完全有标签数据
   更低的期望错误率。

SSL 中无标签数据的信息
========================

似然函数分解
-------------

完全数据的似然可以分解为：

.. math::

   L(\theta \mid x, y) = L_1(\theta \mid x, y) \times L_2(\theta \mid x)

其中：

- :math:`L_1` 来自条件分布 :math:`P(y \mid x; \theta)` —— 分类器建模的目标
- :math:`L_2` 来自边际分布 :math:`P(x; \theta)` —— 特征分布的信息

半监督学习的关键问题：**边际分布 :math:`P(x)` 是否包含关于条件分布 :math:`P(y \mid x)` 的信息？**

.. list-table::
   :header-rows: 1

   * - 模型类型
     - :math:`P(x)` 是否包含 :math:`P(y \mid x)` 的信息
     - SSL 能否受益
   * - 生成式模型（如 GMM）
     - **是**——参数共享
     - 可以显著受益
   * - 判别式模型（如 LR、SVM）
     - **否**——参数独立
     - 通常不受益或难以量化
   * - 带流形假设的模型
     - **是**——通过假设连接
     - 可以受益

生成式分类器的 Fisher 信息
============================

对于生成式分类器，无标签样本的 Fisher 信息有清晰的理论刻画。

设 :math:`\theta` 为模型参数，完全有标签样本的 Fisher 信息为：

.. math::

   \mathcal{I}_{\text{sup}}(\theta) = \mathbb{E}_{x, y} \left[ -\frac{\partial^2 \log P(x, y; \theta)}{\partial \theta \partial \theta^\mathsf{T}} \right]

无标签样本的 Fisher 信息为：

.. math::

   \mathcal{I}_{\text{unsup}}(\theta) = \mathbb{E}_{x} \left[ -\frac{\partial^2 \log P(x; \theta)}{\partial \theta \partial \theta^\mathsf{T}} \right]

那么，半监督设置下的总 Fisher 信息为：

.. math::

   \mathcal{I}_{\text{semi}} = n \cdot \mathcal{I}_{\text{sup}} + m \cdot \mathcal{I}_{\text{unsup}}

其中 :math:`n` 为有标签样本数，:math:`m` 为无标签样本数。

.. tip::
   当 :math:`\mathcal{I}_{\text{unsup}}` 对 :math:`\theta` 中与分类相关的参数有贡献时，
   无标签数据可以提升估计效率。这正是生成式模型的情况。

渐近相对效率（ARE）
=====================

McLachlan (1975) 给出了线性判别分析（LDA）在部分分类样本下的渐近相对效率：

.. math::

   \text{ARE}(m, n) = \frac{\text{半分类样本的渐近方差}}{\text{完全分类样本的渐近方差}}

当 :math:`m \to \infty` 时，半分类样本的分类器可以达到与完全分类样本相同的效率——
也就是说，**无标签数据可以补偿标签的缺失**。
