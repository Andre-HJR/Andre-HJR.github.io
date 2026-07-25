// Pagefind 搜索初始化（动态加载 UI 资源）
(function() {
    'use strict';

    // 1. 加载 CSS
    var link = document.createElement('link');
    link.rel = 'stylesheet';
    link.href = '/pagefind/pagefind-ui.css';
    document.head.appendChild(link);

    // 2. 加载 JS
    var script = document.createElement('script');
    script.src = '/pagefind/pagefind-ui.js';
    script.onload = function () {
        // 3. 创建搜索容器
        var sidebar = document.querySelector('.wy-side-scroll');
        if (!sidebar) return;

        var searchDiv = document.createElement('div');
        searchDiv.id = 'pagefind-search';
        sidebar.insertBefore(searchDiv, sidebar.firstChild);

        // 4. 初始化 Pagefind
        new PagefindUI({
            element: '#pagefind-search',
            showSubResults: true,
            showImages: false,
            resetStyles: false,
            highlightParam: 'highlight'
        });
    };
    document.head.appendChild(script);

    // 5. 快捷键 Ctrl+K
    document.addEventListener('keydown', function(e) {
        if ((e.ctrlKey || e.metaKey) && e.key === 'k') {
            e.preventDefault();
            var input = document.querySelector('.pagefind-ui__search-input');
            if (input) input.focus();
        }
    });
})();
