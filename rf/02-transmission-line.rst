======================================
传输线理论
======================================

传输线是射频工程的核心基础——它连接射频信号源与负载，以电磁波形式传输能量。

传输线的基本模型
=================

集总参数单元
---------------

当传输线长度与波长可比时，用分布参数模型描述。每单位长度传输线由四个参数表征：

.. code-block:: text

      R (Ω/m)    L (H/m)
   ──╌╌─┬─╌╌╌╌─┬─╌╌─┬─╌╌╌╌─┬─╌╌──
        │       │     │       │
        ═ G (S/m)     ═ C (F/m)
        │       │     │       │
   ──╌╌─┴─╌╌╌╌─┴─╌╌─┴─╌╌╌╌─┴─╌╌──
   Δz              Δz

其中：

- :math:`R` = 单位长度串联电阻（导体损耗）
- :math:`L` = 单位长度串联电感
- :math:`G` = 单位长度并联电导（介质损耗）
- :math:`C` = 单位长度并联电容

电报方程
-----------

.. math::

   \frac{\partial V(z,t)}{\partial z} = -R\,i(z,t) - L\,\frac{\partial i(z,t)}{\partial t}

   \frac{\partial i(z,t)}{\partial z} = -G\,v(z,t) - C\,\frac{\partial v(z,t)}{\partial t}

稳态正弦信号下的解：

.. math::

   V(z) = V_0^+ e^{-\gamma z} + V_0^- e^{\gamma z}

   I(z) = I_0^+ e^{-\gamma z} + I_0^- e^{\gamma z}

传播常数与特征阻抗
====================

.. math::

   \gamma = \alpha + j\beta = \sqrt{(R + j\omega L)(G + j\omega C)}

   Z_0 = \sqrt{\frac{R + j\omega L}{G + j\omega C}}

**无耗传输线** ( :math:`R = 0,\ G = 0` )：

.. math::

   \alpha = 0, \quad \beta = \omega \sqrt{LC}, \quad
   Z_0 = \sqrt{\frac{L}{C}}, \quad
   v_p = \frac{1}{\sqrt{LC}}, \quad
   \lambda = \frac{2\pi}{\beta}

常见传输线类型
=================

微带线（Microstrip）
---------------------

.. code-block:: text

   ┌──────────────────────┐  ← 导体带（宽度 W）
   │     εr (介质基板)     │  ← 厚度 h
   └──────────────────────┘
   ═══════════════════════   ← 底层地平面

.. math::

   Z_0 \approx \frac{87}{\sqrt{\varepsilon_r + 1.41}} \ln\left(\frac{5.98 h}{0.8W + t}\right) \quad \text{(W/h < 1)}

   \varepsilon_{\text{eff}} = \frac{\varepsilon_r + 1}{2} + \frac{\varepsilon_r - 1}{2} \left(1 + 12\frac{h}{W}\right)^{-1/2}

.. list-table::
   :header-rows: 1

   * - 参数
     - 典型值
     - 说明
   * - 基板介电常数 :math:`\varepsilon_r`
     - 4.5 (FR4), 3.5 (Rogers 4350B)
     - FR4 损耗大，高频推荐 Rogers
   * - 基板厚度 :math:`h`
     - 0.254 ~ 1.6 mm
     - 厚基板阻抗高
   * - 铜厚 :math:`t`
     - 0.035 mm (1 oz) / 0.07 mm (2 oz)
     - 厚铜损耗小
   * - 损耗角正切 :math:`\tan\delta`
     - 0.02 (FR4), 0.0037 (Rogers)
     - FR4 在 GHz 级损耗过大

.. tip::
   微带线的设计中，通常使用 **Polar Si8000** 或 **ADS LineCalc** 工具
   根据目标阻抗反推线宽。

带状线（Stripline）
---------------------

.. code-block:: text

   ═══════════════════════  ← 顶层地平面
   ──────────────────────  ← 导体带（宽度 W）
   ═══════════════════════  ← 底层地平面

带状线位于上下地平面之间，无辐射损耗，适合多层 PCB 的内层 RF 走线。

共面波导（CPW）
----------------

.. code-block:: text

   ────  ────────────────────  ────
   GND    RF 信号走线         GND
   ────  ────────────────────  ────
   ════════════════════════════════  ← 地平面（可选）

CPW 两侧有地平面，串扰小，适合密集布线。

传输线阻抗不连续
==================

.. code-block:: text

   阶梯阻抗变化：

        Z₁ = 50 Ω           Z₂ = 75 Ω
   ──────────┬────────────────────────
              │
              │ 电容效应 + 电感效应
              │

   过孔引入的寄生参数：

   等效电路：
        ──┬── L_via ──┬──
          │           │
          ═ C_pad     ═ C_anti-pad
          │           │
        ──┴───────────┴──

.. warning::

   阻抗不连续会引入附加反射。在 PCB 设计中应尽量减少：
   - 走线宽度突变（用渐变线过渡）
   - 不必要的过孔（每个过孔约 0.5 nH 电感）
   - 直角拐弯（用 45° 或圆弧代替）

传输线端接条件
=================

.. list-table::
   :header-rows: 1

   * - 端接
     - :math:`Z_L`
     - :math:`\Gamma`
     - VSWR
     - 说明
   * - 匹配负载
     - :math:`Z_0`
     - 0
     - 1:1
     - 无反射，最大功率传输
   * - 短路
     - 0
     - -1
     - :math:`\infty`
     - 全反射，反相
   * - 开路
     - :math:`\infty`
     - +1
     - :math:`\infty`
     - 全反射，同相
   * - 纯电抗
     - :math:`jX`
     - :math:`\frac{jX - Z_0}{jX + Z_0}`
     - :math:`> 1`
     - 全反射，相移

传输线的输入阻抗
==================

长度为 :math:`l` 的传输线，端接负载 :math:`Z_L` 时，输入阻抗为：

.. math::

   Z_{\text{in}} = Z_0 \frac{Z_L + jZ_0 \tan(\beta l)}{Z_0 + jZ_L \tan(\beta l)}

特殊长度的输入阻抗：

.. list-table::
   :header-rows: 1

   * - 长度
     - :math:`Z_{\text{in}}`
     - 应用
   * - :math:`l = \lambda/4`
     - :math:`Z_0^2 / Z_L`
     - 四分之一波长阻抗变换器
   * - :math:`l = \lambda/2`
     - :math:`Z_L`
     - 半波长线，阻抗重现已
   * - :math:`l = \lambda/8`
     - :math:`Z_0 \frac{Z_L + jZ_0}{Z_0 + jZ_L}`
     - 阻抗变换
