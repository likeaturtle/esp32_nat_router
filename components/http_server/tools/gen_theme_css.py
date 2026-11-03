#!/usr/bin/env python3
"""Regenerate theme_css.h from theme.css.

The web UI serves one stylesheet from flash. Editing theme.css alone does not
change the firmware, so run this after every CSS change:

    python3 tools/gen_theme_css.py
"""
from pathlib import Path

base = Path(__file__).resolve().parents[1]
css = (base / "theme.css").read_text(encoding="utf-8")
escaped = css.replace("\\", "\\\\").replace('"', '\\"')
lines = escaped.split("\n")

out = [
    "#ifndef THEME_CSS_H",
    "#define THEME_CSS_H",
    "",
    "#include <stddef.h>",
    "",
    "/* Generated from theme.css by tools/gen_theme_css.py. Do not edit. */",
    "static const char theme_css[] =",
]
out.extend(f'    "{line}\\n"' for line in lines)
out.extend([
    "    ;",
    "static const size_t theme_css_len = sizeof(theme_css) - 1;",
    "",
    "#endif /* THEME_CSS_H */",
])
(base / "theme_css.h").write_text("\n".join(out) + "\n", encoding="utf-8")
print(f"wrote {base / 'theme_css.h'} from {len(css)} bytes of CSS")
