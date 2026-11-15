/* Web UI 中英文切换（方案 A：前端词典）
 *
 * 英文原文就是词典 key：页面模板和 handler 里的英文继续是唯一事实源，
 * 新增/改动的文案在词典里找不到时原样显示英文，不会编译失败、也不会白屏。
 * 所以上游改页面时不需要同步改这里，只要偶尔跑一下
 *   python3 tools/i18n_extract.py
 * 看看哪些新文案还没翻译。
 *
 * 词典在 i18n/zh.json，经 tools/gen_i18n.py 注入到占位符后生成
 * i18n_js.h（和 theme.css → theme_css.h 同一套路，勿手改生成文件）。
 *
 * 没有放进 DOM 的 JS 文案（OTA 进度、导入导出提示等）用 t('...') 包一层；
 * 由服务端拼出来的整句（"2 associated"、"Allowed 12"）用下面的 RULES 处理。
 */
(function () {
  'use strict';

  var DICT = __I18N_DICT__;

  /* 带数字/状态值的整句。顺序即优先级，锚定整串避免误伤用户数据。 */
  var RULES = [
    [/^(\d+) associated$/, '$1 个客户端'],
    [/^Allowed (\d+)$/, '允许 $1'],
    [/^Denied (\d+)$/, '拒绝 $1'],
    [/^No match (\d+)$/, '未命中 $1'],
    [/^Pool (.+)$/, '地址池 $1'],
    [/^since (.+)$/, '自 $1'],
    [/^Build (.+)$/, '构建 $1'],
    [/^\((\d+) captured, (\d+) dropped\)$/, '（已捕获 $1，已丢弃 $2）'],
    [/^Auto-refreshes every few seconds\. Found (\d+) networks?\.$/, '每几秒自动刷新。找到 $1 个网络。'],
    [/^(\d+)d (\d+):(\d+):(\d+)$/, '$1 天 $2:$3:$4'],
    [/^Wrong chip type \(firmware: (0x[0-9A-Fa-f]+), this device: (0x[0-9A-Fa-f]+)\)$/, '芯片型号不匹配（固件：$1，本机：$2）'],
    [/^Firmware validation failed: (.*)$/, '固件校验失败：$1'],
    [/^Uploading\.\.\. (\d+)%$/, '上传中… $1%']
  ];

  var ATTRS = ['placeholder', 'title', 'aria-label', 'alt'];
  var SKIP_TAGS = { SCRIPT: 1, STYLE: 1, NOSCRIPT: 1, TEXTAREA: 1, CODE: 1, PRE: 1 };
  var KEY = 'webui.lang';

  /* Data tables mix static verbs with user data (SSIDs, device names, MACs).
   * Cell text is translated only when it is one of these known labels, so a
   * device literally called "Home" is never rewritten. */
  var CELL_LABELS = {
    'Open': 1,
    'No networks found': 1,
    'No clients connected': 1,
    'No DHCP reservations configured': 1,
    'No port mappings configured': 1,
    'No rules (all packets allowed)': 1
  };

  function normalize(text) {
    return text.replace(/\s+/g, ' ').trim();
  }

  function lookup(text) {
    if (!text) return null;
    var value = normalize(text);
    if (!value) return null;
    if (Object.prototype.hasOwnProperty.call(DICT, value)) return DICT[value];
    for (var i = 0; i < RULES.length; i++) {
      if (RULES[i][0].test(value)) return value.replace(RULES[i][0], RULES[i][1]);
    }
    return null;
  }

  /* 给内联脚本用：t('Uploading...')、t('Loaded {n} items', {n: n}) */
  function t(text, vars) {
    var out = lookup(text);
    if (out === null) out = text;
    if (vars) {
      out = out.replace(/\{(\w+)\}/g, function (match, name) {
        return vars[name] != null ? vars[name] : match;
      });
    }
    return out;
  }

  function readStored() {
    try {
      var value = localStorage.getItem(KEY);
      return value === 'zh' || value === 'en' ? value : null;
    } catch (e) {
      return null;
    }
  }

  function readCookie() {
    var match = document.cookie.match(/(?:^|;\s*)lang=(en|zh)(?:;|$)/);
    return match ? match[1] : null;
  }

  function save(lang) {
    try { localStorage.setItem(KEY, lang); } catch (e) { /* private mode */ }
    try {
      document.cookie = 'lang=' + lang + '; path=/; max-age=31536000; SameSite=Lax';
    } catch (e) { /* cookie-less webview */ }
  }

  function detect() {
    var match = location.search.match(/[?&]lang=([^&]+)/);
    if (match) {
      var fromQuery = decodeURIComponent(match[1]).toLowerCase();
      var lang = fromQuery.indexOf('zh') === 0 ? 'zh' : 'en';
      save(lang);
      return lang;
    }
    var stored = readStored();
    if (stored) return stored;
    var cookie = readCookie();
    if (cookie) return cookie;
    var languages = navigator.languages || [];
    var preferred = (languages[0] || navigator.language || navigator.userLanguage || 'en').toLowerCase();
    return preferred.indexOf('zh') === 0 ? 'zh' : 'en';
  }

  var current = detect();

  function skipElement(element) {
    if (!element || !element.nodeName) return true;
    if (SKIP_TAGS[element.nodeName]) return true;
    if (element.classList &&
        (element.classList.contains('brand') || element.classList.contains('lang-toggle'))) {
      return true;
    }
    if (element.closest && element.closest('[data-i18n-skip]')) return true;
    return false;
  }

  function skipTextNode(node) {
    var parent = node.parentNode;
    if (!parent || skipElement(parent)) return true;
    if (parent.closest && parent.closest('.mono')) return true;
    if (parent.nodeName === 'TD' && parent.classList && parent.classList.contains('name')) return true;
    if (parent.nodeName === 'TD' && parent.closest && parent.closest('.data-table')) {
      return !CELL_LABELS[normalize(node.nodeValue)];
    }
    return false;
  }

  function translateNode(node) {
    var raw = node.nodeValue;
    if (!raw) return;
    var leading = raw.match(/^\s*/)[0];
    var trailing = raw.match(/\s*$/)[0];
    var core = raw.slice(leading.length, raw.length - trailing.length);
    var translated = lookup(core);
    if (translated !== null && translated !== core) {
      node.nodeValue = leading + translated + trailing;
    }
  }

  function translateAttributes(element) {
    for (var i = 0; i < ATTRS.length; i++) {
      var name = ATTRS[i];
      var value = element.getAttribute(name);
      if (!value) continue;
      var translated = lookup(value);
      if (translated !== null) element.setAttribute(name, translated);
    }
    if (element.nodeName === 'INPUT' && /^(submit|button|reset)$/i.test(element.type || '')) {
      var label = lookup(element.value);
      if (label !== null) element.value = label;
    }
  }

  function translatePage() {
    if (!document.body) return;
    var walker = document.createTreeWalker(document.body, NodeFilter.SHOW_TEXT, null, false);
    var nodes = [];
    var node;
    while ((node = walker.nextNode())) {
      if (!skipTextNode(node)) nodes.push(node);
    }
    for (var i = 0; i < nodes.length; i++) translateNode(nodes[i]);

    var elements = document.body.querySelectorAll('*');
    for (var j = 0; j < elements.length; j++) {
      if (!skipElement(elements[j])) translateAttributes(elements[j]);
    }

    var title = lookup(document.title);
    if (title !== null) document.title = title;
    document.documentElement.lang = 'zh-CN';
  }

  /* 顶栏注入 中/EN 切换按钮，各页面模板不需要改动。 */
  function addToggle() {
    var actions = document.querySelector('.topbar-actions');
    if (!actions || actions.querySelector('.lang-toggle')) return;
    var button = document.createElement('button');
    button.type = 'button';
    button.className = 'lang-toggle';
    button.textContent = current === 'zh' ? 'EN' : '中文';
    button.setAttribute('aria-label', current === 'zh' ? 'Switch to English' : '切换到中文');
    button.title = button.getAttribute('aria-label');
    button.addEventListener('click', function () {
      var next = current === 'zh' ? 'en' : 'zh';
      save(next);
      /* Drop all query params (lang included) so the saved choice wins and
       * stale one-shot params (error=..., saved flags) do not leak across
       * the reload. */
      location.replace(location.pathname + location.hash);
    });
    actions.insertBefore(button, actions.firstChild);
  }

  function start() {
    try {
      if (current === 'zh') translatePage();
    } finally {
      document.documentElement.classList.remove('i18n-pending');
      try { addToggle(); } catch (e) { /* never block the page */ }
    }
  }

  /* 先隐藏 body，等翻译完再显示，避免中文用户看到一瞬英文。
   * 这段在 <head> 里同步执行，CSS 规则在 theme.css。JS 被禁用时没有这个
   * class，页面照常显示英文。 */
  if (current === 'zh') document.documentElement.classList.add('i18n-pending');
  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', start);
  } else {
    start();
  }

  window.t = t;
  window.uiLang = current;
  window.uiI18n = { lang: current, t: t, lookup: lookup, setLang: save };
})();
