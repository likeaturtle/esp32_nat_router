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

## Editing

1. Change `theme.css`.
2. Run `python3 tools/gen_theme_css.py`.
3. Build and flash.

The stylesheet is served with `Cache-Control: no-cache`, so a browser refresh
picks up changes after an OTA without a cache-busting query string.

## Constraints

- No external CSS, fonts, or icon libraries: the UI must work on the isolated
  AP and in the captive portal.
- Icons are inline SVG.
- Pages are chunked C strings; keep `%s` / `%d` placeholders and their order
  intact when editing `page_*.h`.
- The page shell is responsive at 360–1200 px; wide tables scroll horizontally
  and selected columns collapse on phones.

## Preview

Static previews rendered from the real page templates (with sample data) are in
`ui_style_samples/shots/implemented-*.png`.
