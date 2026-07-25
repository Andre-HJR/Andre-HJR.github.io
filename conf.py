# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os
import sys

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'HJR Docs'
copyright = '2026, HJR'
author = 'HJR'
release = '1.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    # Markdown 支持
    'myst_parser',
    # 自动从源码生成 API 文档
    'sphinx.ext.autodoc',
    'sphinx.ext.autosummary',
    # 代码块增强
    'sphinx_copybutton',
    # UI 组件（卡片、网格、标签页）
    'sphinx_design',
    'sphinx_togglebutton',
    # 图表支持
    'sphinxcontrib.mermaid',
    # Open Graph 社交分享
    'sphinxext.opengraph',
    # 自定义 404（Sphinx 9.x 暂不兼容 sphinx-notfound-page）
    # Markdown 表格
    'sphinx_markdown_tables',
    # 自动摘要
    'autodocsumm',
    # 数学公式
    'sphinx.ext.mathjax',
    # 查看源码链接
    'sphinx.ext.viewcode',
    # 交叉引用
    'sphinx.ext.intersphinx',
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

language = 'zh_CN'

# 抑制某些非关键的警告
suppress_warnings = ['ref.citation', 'ref.footnote']

# -- MyST 配置（Markdown 解析） ------------------------------------------------
myst_enable_extensions = [
    'colon_fence',
    'deflist',
    'dollarmath',
    'fieldlist',
    'html_admonition',
    'html_image',
    'linkify',
    'replacements',
    'smartquotes',
    'strikethrough',
    'substitution',
    'tasklist',
]
myst_heading_anchors = 3

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_logo = ''
html_favicon = ''
html_css_files = []
html_js_files = ['pagefind-init.js']

html_theme_options = {
    'navigation_depth': 4,
    'collapse_navigation': False,
    'sticky_navigation': True,
    'includehidden': True,
    'titles_only': False,
}

# -- MathJax 配置 -----------------------------------------------------------
# 使用 MathJax 3 稳定版（而非 Sphinx 9 默认的 v4 beta）
mathjax_path = 'https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js'

# -- Intersphinx 映射 ---------------------------------------------------------
intersphinx_mapping = {
    'python': ('https://docs.python.org/3', None),
}

# -- Autodoc 配置 -------------------------------------------------------------
autodoc_default_options = {
    'members': True,
    'show-inheritance': True,
}

# -- Copybutton 配置 ----------------------------------------------------------
copybutton_prompt_text = r'>>> |\.\.\. |\$ |\$\s*'
copybutton_prompt_is_regexp = True
