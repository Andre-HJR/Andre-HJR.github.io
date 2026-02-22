---
title: Armadillo 的基础使用
date: 2026-02-22 21:57:22
tags:
    - [C, ARMA]
categories:
    - C
    - MATH
---

## ARMADILLO

{% blockquote %}
Armadillo is a high quality linear algebra library (matrix maths) for the C++ language, aiming towards a good balance between speed and ease of use
{% endblockquote %}


<!-- more -->


### 例子1： 使用Armadillo计算雅可比矩阵

{% blockquote %}
代码示例
{% endblockquote %}

{% codeblock "计算雅可比矩阵" lang:cpp mark:1 %}
arma::mat numerical_jacobian( // arma namespace from <armadillo>
    std::function<arma::vec(const arma::vec&)> f, // std::function class from <functional>
    const arma::vec& x,
    double h = 1e-6
)
{
    arma::vec f0 = f(x);
    size_t m = f0.n_rows;
    size_t n = x.n_rows;

    arma::mat J(m, n, arma::fill::zeros);
    
    for (size_t j = 0; j < n; ++j)
    {
        arma::vec x_plus = x;
        arma::vec x_minus = x;
        x_plus(j) += h;
        x_minus(j) -= h;
        arma::vec f_plus = f(x_plus);
        arma::vec f_minus = f(x_minus);
        J.col(j) = (f_plus - f_minus) / (2.0 * h);
    }
    return J;
}
{% endcodeblock %}


{% codeblock "分析雅可比矩阵" lang:cpp %}
arma::mat annalytical_jacobian(const  arma::vec& x)
{
    arma::mat J(2, 2);

    double x1 = x(0);
    double x2 = x(1);

    J(0, 0) = 2.0 * x1;
    J(0, 1) =  1.0;

    double c = std::cos(x1 * x2);

    J(1, 0) = x2 * c;
    J(1, 1) = x1 * c;

    return J;
}
{% endcodeblock %}


{% codeblock "测试雅可比矩阵" lang:cpp %}
TEST(A, JacobianTest) // THIS IS GOOGLE_TEST FRAMEWORK
{
    arma::vec x = {1.2, 0.7};
    arma::mat J_numeric = numerical_jacobian(_func, x);
    arma::mat J_exact = annalytical_jacobian(x);

    ASSERT_EQ(J_numeric.n_rows, J_exact.n_rows);
    ASSERT_EQ(J_numeric.n_cols, J_exact.n_cols);

    for (size_t i = 0; i < J_exact.n_rows; ++i)
    {
        for(size_t j = 0; j < J_exact.n_cols; ++j)
        {
            EXPECT_NEAR(J_numeric(i, j), J_exact(i, j), 1e-9);
        }
    }
}
{% endcodeblock %}