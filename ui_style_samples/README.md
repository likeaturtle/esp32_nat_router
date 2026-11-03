# Web UI 风格样例

针对 ESP32 NAT Router 管理界面的四套视觉方向。

> **2026-11-03 已落地**：选定的 **01 Instrument** 已应用到 `/`、`/config`、`/mappings`、`/firewall`、`/vpn`、`/setup`、`/scan` 全部页面，登录/密码卡片、弹窗和提示也已统一。构建通过，尚未烧录真机。实现说明见 [components/http_server/THEME.md](../components/http_server/THEME.md)，真机前预览见 [shots/implemented](shots/implemented)。

同一套内容：状态面板、六项导航、Wi-Fi 表单、客户端列表、危险操作、页脚。
四种设计语言：仪表 / 纸本 / 原生 / 终端。

## 先看这个

- **[compare.html](compare.html)** — 四列对比页，点任意截图可打开对应完整样例。
- 单页样例：[01-instrument.html](01-instrument.html) · [02-editorial.html](02-editorial.html) · [03-native.html](03-native.html) · [04-console.html](04-console.html)
- 完整长图在 `shots/`，对比图是 `shots/compare-sheet.png`。

截图用 headless Edge 在 **492px CSS 视口**、2x 下渲染，对应手机宽度；页面本身已带 560 / 520 / 480px 断点。

## 为什么现在的页面“AI 味”重

当前实现里有这些很典型的生成式 UI 特征：

- 蓝紫渐变背景 + 青色发光标题（`text-shadow: 0 0 20px`）
- 毛玻璃卡片 `backdrop-filter: blur(10px)`、16px 大圆角
- 每个按钮都是渐变色 + 彩色投影 + hover 上浮
- emoji 当图标（🚀 📡 ⚙️ 🔀 🛡️ 🔒）
- 所有信息都塞进圆角卡片，层级靠颜色而不是排版
- 危险操作区域用高饱和黄底 + 白字

四套样例统一去掉了：渐变、发光、毛玻璃、装饰性阴影、emoji、大面积圆角、彩色投影。
强调色每套只留一个；状态用“文字 + 小色点”，不单靠颜色表达；IP / MAC / 数字用等宽字体和 `tabular-nums`。

## 四个方向

| | 关键词 | 手机易用性 | 实现成本 | 主要风险 |
|---|---|---|---|---|
| **01 Instrument** 工程仪表 | 暖灰、发丝线、深绿、等宽数字 | 高 | **低**：现有 table / form 结构基本不动 | 可能“太素”，需要靠间距和字重细节撑住 |
| **02 Editorial** 纸本手册 | 衬线、点线引导、编号索引、锈红 | 中高 | 中：状态行要加 leader 元素，字号体系要重排 | 页面变长；Android 中文字体回退会损失一些气质 |
| **03 Native** 系统原生 | 分组列表、系统蓝、44px 行高、开关 | **最高** | 中：部分表格改成分组列表，checkbox 改 switch | 像 iOS，品牌感弱；桌面端要再加宽布局 |
| **04 Console** 维护终端 | 近黑、琥珀、等宽、方角、1px 框线 | 中 | **低**：结构不动，换 CSS + 调对比度 | 户外/强光下深色反光；长文案用等宽略累 |

### 01 Instrument

像一台调好的设备：暖灰纸底、1px 分隔线、深绿只表示“通”。标题用 10px 大写字母 + 字距，数字走等宽。
克制但不简陋，信息密度高。最容易从现有页面迁移，因为表格、表单、按钮的 DOM 基本不用改。

### 02 Editorial

像一本排过版的手册：暖白纸色、衬线正文、锈红强调、点线引导、编号索引。
所有页面会变长一些，但阅读节奏最好，也最不像“模板生成”。如果希望这个项目有一点自己的气质，选它。

### 03 Native

iOS 设置页的语言：分组列表、系统蓝、行内开关、内缩分隔线、内联 SVG 图标。
对“只会用手机、第一次配路由器”的人最友好。代价是部分表格要改成分组列表，桌面宽屏要单独处理。

### 04 Console

设备直接说话：近黑面板、单一琥珀色、等宽字体、方角、`01 / connection` 式分区。
适合刷机、抓包、看日志的人。深色但去掉了霓虹和发光，是四套里唯一的暗色方案。

## 实现约束（已按固件页面的限制设计）

- 页面是 `components/http_server/pages/page_*.h` 里的分块 C 字符串，样式只能内联在 `<style>`。
- 不引外部 CSS / 字体 / 图标库；中文和英文都用系统字体栈。
- 图标全部是内联 SVG 线条，每个约 0.2–0.5 KB，可抽成 `<symbol>` 复用。
- 无新增 JS；登录、modal、表单提交逻辑保持现状。
- 预计每页 CSS 4–8 KB（未压缩），对 flash 和 chunked 发送都不是压力；实现时会按页拆 CSS。

## 建议

- **默认推荐 01 Instrument**：信息密度、工程感、迁移成本三者最平衡，桌面和手机都稳。
- 手机端优先、希望零学习成本：**03 Native**。
- 想要品牌记忆点、接受页面变长：**02 Editorial**。
- 主要用户是刷机 / 抓包 / 看日志的人：**04 Console**。

也可以混搭，例如：01 的信息布局 + 02 的标题与数字排版；03 的分组列表 + 01 的配色与等宽数字。

## 已落地（01 Instrument）

- 样式源文件 [components/http_server/theme.css](../components/http_server/theme.css)，构建时由 `theme_css.h` 提供 `/style.css`。
- 页面共用 `theme.h` 里的 topbar / logout / footer 宏；原有字段、name、id、表单 action 和 JS 逻辑保持不变。
- 所有页面移除了内联渐变、霓虹色、emoji 和旧样式，改为统一的浅色 instrument 风格。
- 手机端表格自动收起次要列；桌面端表单恢复两列标签布局。
- 静态预览（真实模板 + 示例数据）：[index](shots/implemented/index.png) · [config](shots/implemented/config-top.png) · [mappings](shots/implemented/mappings.png) · [firewall](shots/implemented/firewall.png) · [vpn](shots/implemented/vpn.png) · [setup](shots/implemented/setup.png) · [scan](shots/implemented/scan.png)。
- `idf.py build` 已通过（ESP32 目标），未烧录、未做真机浏览器回归。

## 下一步

1. 烧录到设备，在手机（Safari / Chrome）和桌面浏览器上做一次真机回归。
2. 检查带长 SSID、长设备名、空 ACL、NAT 关闭等边界数据下的换行。
3. 如需调整，优先改 `theme.css`，然后运行 `python3 components/http_server/tools/gen_theme_css.py` 再构建。
