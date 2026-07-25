==========
参考文献
==========

主要参考文献
=============

- Ahfock, D. & McLachlan, G. J. (2021).
  *Semi-Supervised Learning of Classifiers from a Statistical Perspective: A Brief Review*.
  arXiv:2104.04046. https://arxiv.org/abs/2104.04046

- Mey, A. & Loog, M. (2019).
  *Improvability through Semi-Supervised Learning: A Survey of Theoretical Results*.
  arXiv:1908.09574. https://arxiv.org/abs/1908.09574

- Rubin, D. B. (1976).
  *Inference and missing data*.
  Biometrika, 63(3), 581-592.

- Dempster, A. P., Laird, N. M. & Rubin, D. B. (1977).
  *Maximum likelihood from incomplete data via the EM algorithm*.
  Journal of the Royal Statistical Society: Series B, 39(1), 1-22.

- McLachlan, G. J. (1975).
  *Iterative reclassification procedure for constructing an
  asymptotically optimal rule of allocation in mixed-up populations*.
  Journal of the American Statistical Association, 70(350), 414-419.

经典 SSL 相关工作
===================

- Blum, A. & Mitchell, T. (1998).
  *Combining labeled and unlabeled data with co-training*.
  COLT '98.

- Joachims, T. (1999).
  *Transductive inference for text classification using support vector machines*.
  ICML '99.

- Zhu, X., Ghahramani, Z. & Lafferty, J. (2003).
  *Semi-supervised learning using Gaussian fields and harmonic functions*.
  ICML '03.

- Belkin, M., Niyogi, P. & Sindhwani, V. (2006).
  *Manifold regularization: A geometric framework for learning from
  labeled and unlabeled examples*.
  Journal of Machine Learning Research, 7, 2399-2434.

- Grandvalet, Y. & Bengio, Y. (2005).
  *Semi-supervised learning by entropy minimization*.
  NeurIPS '04.

- Balcan, M.-F. & Blum, A. (2010).
  *A discriminative model for semi-supervised learning*.
  Journal of the ACM, 57(3), 1-46.

深度 SSL 相关工作
===================

- Laine, S. & Aila, T. (2016).
  *Temporal ensembling for semi-supervised learning*.
  ICLR '17.

- Tarvainen, A. & Valpola, H. (2017).
  *Mean teachers are better role models: Weight-averaged consistency
  targets improve semi-supervised deep learning results*.
  NeurIPS '17.

- Miyato, T. et al. (2018).
  *Virtual adversarial training: A regularization method for supervised
  and semi-supervised learning*.
  IEEE TPAMI, 41(8).

- Xie, Q. et al. (2019).
  *Unsupervised data augmentation for consistency training*.
  NeurIPS '20.

- Sohn, K. et al. (2020).
  *FixMatch: Simplifying semi-supervised learning with consistency and
  confidence*.
  NeurIPS '20.

- Chen, T. et al. (2020).
  *A simple framework for contrastive learning of visual representations*.
  ICML '20.

更多资源
=========

综述论文
---------

- Van Engelen, J. E. & Hoos, H. H. (2020).
  *A survey on semi-supervised learning*.
  Machine Learning, 109(2), 373-440.

- Yang, X. et al. (2021).
  *A survey on deep semi-supervised learning*.
  arXiv:2103.00550.

书籍
-----

- Chapelle, O., Schölkopf, B. & Zien, A. (2006).
  *Semi-Supervised Learning*. MIT Press.

- Zhu, X. & Goldberg, A. B. (2009).
  *Introduction to Semi-Supervised Learning*.
  Synthesis Lectures on Artificial Intelligence and Machine Learning.

开源库
-------

- `FixMatch <https://github.com/google-research/fixmatch>`__ — Google Research
- `TorchSSL <https://github.com/TorchSSL/TorchSSL>`__ — PyTorch SSL 工具包
- `USB <https://github.com/microsoft/Semi-supervised-learning>`__ — Microsoft 的统一 SSL 基准

.. note::
   本文档主要基于 Ahfock & McLachlan (2021) 和 Mey & Loog (2019)
   两篇综述论文的框架进行组织。深度 SSL 部分参考了 Yang et al. (2021)
   的综述以及各方法原文。
