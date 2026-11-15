# Web UI theme

The admin UI uses one light, instrument-style theme: warm neutrals, hairline
rules, a single deep-green signal accent, monospaced figures, no gradients or
glow.

## Files

- `theme.css` — source of truth for the stylesheet (about 20 KB).
- `theme_css.h` — generated C string served at `/style.css`; do not edit.
- `theme.h` — shared markup helpers: document head, top bar, logout link,
  footer.
- `pages/page_*.h` — page chunks. They only contain markup; all styles live in
  `theme.css`.
- `tools/gen_theme_css.py` — regenerates `theme_css.h` from `theme.css`.
- `i18n/zh.json` — English → Chinese dictionary (source of truth).
- `i18n/i18n.js` — language detection, DOM walker, dynamic pattern rules, toggle.
- `i18n_js.h` — generated C string served at `/i18n.js`; do not edit.
- `tools/gen_i18n.py` — regenerates `i18n_js.h` from the two files above.
- `tools/i18n_extract.py` — lists page text missing from `zh.json` and stale keys.

## Editing

1. Change `theme.css`.
2. Run `python3 tools/gen_theme_css.py`.
3. Build and flash.

The stylesheet is served with `Cache-Control: no-cache`, so a browser refresh
picks up changes after an OTA without a cache-busting query string.

## Languages

The UI ships English markup plus a Chinese dictionary; English text is the
dictionary key, so upstream edits never need a matching change here — an
unknown label simply stays English. Nothing is added to NVS or config export.

1. Translate in `i18n/zh.json`, then run `python3 tools/gen_i18n.py`.
2. After touching `pages/*.h`, `theme.h` or the inline HTML in
   `http_server.c`, run `python3 tools/i18n_extract.py` and fill in anything
   under MISSING. It exits non-zero when a string is untranslated.
3. Numbers/status sentences (`2 associated`, `Allowed 12`) are covered by the
   anchored rules at the top of `i18n/i18n.js`; keep user data out of them.
4. JS-only text (OTA progress, import/export status) must be wrapped with
   `t('...')`; dictionary lookups never see `<script>` text.
5. The `中文` / `EN` toggle is injected into `.topbar-actions` by `i18n.js`,
   so page templates stay untouched. The choice lives in `localStorage` and a
   `lang` cookie; `?lang=zh` / `?lang=en` overrides it for one visit and is
   remembered.
6. Chinese users never see a flash of English: `i18n.js` adds
   `html.i18n-pending` from `<head>` and removes it after the DOM walk
   (`theme.css` hides `body` while the class is set). JS disabled means no
   class and an English page.

## Constraints

- No external CSS, fonts, or icon libraries: the UI must work on the isolated
  AP and in the captive portal.
- Icons are inline SVG.
- Pages are chunked C strings; keep `%s` / `%d` placeholders and their order
  intact when editing `page_*.h`.
- The page shell is responsive at 360–1200 px; wide tables scroll horizontally
  and selected columns collapse on phones.

