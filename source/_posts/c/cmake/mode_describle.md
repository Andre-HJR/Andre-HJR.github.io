---
title: CMake 自定义模块声明文件
date: 2026-02-22 21:59:34
tags:
    - [C, CMAKE]
categories:
    - C
    - CMAKE
---


## CMake 自定义模块声明文件



<!-- more -->


### 基于 OpenBlas 和 Armadillo 的 CMake 模块说明

> OpenBlas 和 Armadillo 的链接涉及到动态库的链接和测试运行时的加载，是相对比较复杂的主题，因此尝试拿来写一下


### Armadillo 的 Find 文件

{% codeblock "Armadillo的CMAKE查找文件说明" lang:txt %}
set(Armadillo_ROOT
	""
	CACHE PATH "ROOT OF ARMADILLO INSTALLATION")

find_path(Armadillo_INCLUDE_DIR
	NAMES armadillo
	HINTS "${Armadillo_ROOT}/include"
	)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	Armadillo
	REQUIRED_VARS Armadillo_INCLUDE_DIR)

if(Armadillo_FOUND)
	set(Armadillo_INCLUDE_DIRS ${Armadillo_INCLUDE_DIR})
	set(Armadillo_LIBRARES)
endif()
{% endcodeblock %}


这里需要说明的是， armadillo 是一个 header-only 的库，外部需要使用的时候，只需要将header-file导出给相关的项目即可，即上述的

{% codeblock lang:txt %}
if(Armadillo_FOUND)
	set(Armadillo_INCLUDE_DIRS ${Armadillo_INCLUDE_DIR})
	set(Armadillo_LIBRARES)
endif()
{% endcodeblock %}

上述的的代码中，对外部的使用只导出了真正的 Armadillo_INCLUDE_DIRS， 其他相关变量只是一个接口上的统一。当需要使用相关模块的时候，通过

{% codeblock "添加相关的cmake模块搜索路径" lang:txt %}
list(APPEND CMAKE_MODULE_PATH "${CAKE_SOURCE_DIR}/cmake/CMakeModules")
{% endcodeblock %}

当添加完相关的路径的时候，对于需要的用模块A，只需要将头文件的路径添加进去即可

{% codeblock "链接头文件" lang:txt %}
# After you had set(Armadillo_ROOT) in correct path
# And Call find_package(Armadillo REQUIRED)

add_executable(A ...)
target_include_directories(A PRIVATE ... ${Armadillo_INCLUDE_DIRS})
{% endcodeblock %}

上述即可完成头文件的链接。

