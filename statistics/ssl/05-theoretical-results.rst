================================
理论结果与可改进性分析
================================

半监督学习的理论工作围绕一个核心问题：**何时以及为何无标签数据能帮助学习？**

Fisher 信息分析
=================

对于生成式分类器，无标签数据的贡献可以被精确量化。

考虑一个由 :math:`n` 个有标签和 :math:`m` 个无标签样本组成的部分分类样本。
似然函数为：

.. math::

   L(\theta) = \prod_{i=1}^n P(x_i, y_i; \theta) \times \prod_{i=n+1}^{n+m} P(x_i; \theta)

对应的 Fisher 信息矩阵：

.. math::

   \mathcal{I}_{\text{semi}} = n \mathcal{I}_{\text{sup}} + m \mathcal{I}_{\text{unsup}}

其中：

.. math::

   \mathcal{I}_{\text{sup}} = \mathbb{E}_{x,y} \left[ -\nabla^2 \log P(x, y \mid \theta) \right]

   \mathcal{I}_{\text{unsup}} = \mathbb{E}_{x} \left[ -\nabla^2 \log P(x \mid \theta) \right]

参数 :math:`\theta` 的渐近方差：

.. math::

   \text{Var}(\hat{\theta}_{\text{semi}}) \approx \frac{1}{n} \mathcal{I}_{\text{sup}}^{-1} \cdot \text{ARE}

其中 ARE 为渐近相对效率。

.. important::
   当 :math:`\mathcal{I}_{\text{unsup}}` 相对于 :math:`\mathcal{I}_{\text{sup}}` 是
   **正定** 的 (即无标签数据提供了关于 :math:`\theta` 的额外信息)，
   半监督估计的方差严格小于监督估计的方差。

可改进性条件
=============

Mey & Loog (2019) 系统地梳理了 SSL 可以（和不可以）改进性能的条件。

基本条件
---------

SSL 能够改进性能的必要条件：

.. math::

   P(y \mid x) \neq P(y) \quad \text{且} \quad
   P(x \mid y) \text{ 与 } P(y) \text{ 之间存在共享参数}

当 :math:`P(x)` 和 :math:`P(y \mid x)` 的参数不相交时，
无标签数据不提供任何关于分类边界的信息。

.. list-table::
   :header-rows: 1

   * - 模型类型
     - 参数共享
     - SSL 可改进性
   * - GMM (生成式)
     - 是——:math:`\mu_k, \Sigma_k` 影响 :math:`P(x)` 和 :math:`P(y \mid x)`
     - **可以** 显著改进
   * - 逻辑回归 (判别式)
     - 原则上不共享
     - 需要额外假设才能改进
   * - 神经网络
     - 隐式共享
     - 取决于架构和正则化

不可能性结果
-------------

在某些情况下，SSL **不可能** 带来任何改进：

.. code-block:: text

   1. 标签完全随机缺失 (MCAR) + 判别式模型
      → 无标签数据不包含任何分类信息

   2. 特征分布与标签条件分布完全独立
      → 边际分布 P(x) 与后验 P(y|x) 无关

   3. 模型已接近贝叶斯误差率
      → 无标签数据无法进一步降低误差

   4. 错误的假设（如数据不满足流形假设时使用基于图的方法）
      → 引入无标签数据反而降低性能

学习率分析
============

不同假设强度下的学习率：

.. list-table::
   :header-rows: 1

   * - 假设
     - 学习率
     - 说明
   * - 无假设
     - :math:`O(1)`
     - 不保证改进
   * - 弱 (Lipschitz 平滑)
     - :math:`O(n^{-1/2})`
     - 常数因子改进
   * - 聚类假设
     - :math:`O(n^{-1/2} \cdot \text{常数})`
     - 取决于聚类质量
   * - 流形假设
     - :math:`O(e^{-n})`
     - 指数级改进（在流形上）
   * - PAC 可学习
     - :math:`\tilde{O}(d/n)`
     - 有效 VC 维降低

.. note::
   监督学习的典型学习率是 :math:`O(n^{-1/2})` (VC 界) 或
   :math:`O(d/n)` (参数模型)。

   SSL 在弱假设下无法突破 :math:`O(n^{-1/2})` 的速率——
   无标签数据只能改善常数因子。

   但在强假设（如流形假设）下，SSL 可以达到指数级 :math:`O(e^{-n})`——
   因为有效样本量从标注样本数 :math:`n` 变成了流形上的总样本数 :math:`n + m`。

PAC 学习框架下的 SSL
======================

在 PAC 学习框架中，SSL 的样本复杂度分析：

.. math::

   m_{\text{sup}}(\epsilon, \delta) \geq \frac{1}{\epsilon} \left( d + \log\frac{1}{\delta} \right)

.. math::

   m_{\text{semi}}(\epsilon, \delta) \text{ 取决于 } P(x) \text{ 提供的关于 } P(y \mid x) \text{ 的信息量}

Balcan & Blum (2010) 证明了在特定的"兼容性"条件下，
无标签数据可以将样本复杂度从 :math:`O(1/\epsilon)` 降低到 :math:`O(\log(1/\epsilon))`。

泛化误差分解
===============

半监督学习的泛化误差可以分解为三个部分：

.. math::

   \underbrace{\mathcal{E}(f)}_{\text{泛化误差}} =
   \underbrace{\mathcal{E}_{\text{bias}}(f)}_{\text{偏差}} +
   \underbrace{\mathcal{E}_{\text{var}}(f)}_{\text{方差}} +
   \underbrace{\mathcal{E}_{\text{noise}}}_{\text{噪声}}

无标签数据的主要作用：

- **减少方差**：通过提供更多关于 :math:`P(x)` 的信息，提高参数估计精度
- **降低偏差**：通过揭示数据流形结构，引导更正确的函数空间
- **不能减少噪声**：贝叶斯误差是 SSL 无法逾越的下界

.. tip::
   理解 SSL 的统计本质：
   - 有标签数据同时提供"方向"（偏差）和"精度"（方差）
   - 无标签数据主要提供"精度"，仅在强假设下提供"方向"
