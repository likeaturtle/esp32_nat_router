#!/usr/bin/env python3
"""Regenerate i18n_js.h from i18n/zh.json + i18n/i18n.js.

The firmware serves one script from flash at /i18n.js.  The dictionary lives
in zh.json (one English string -> one Chinese string), the logic lives in
i18n.js, and this script injects the dictionary and emits a C header:

    python3 tools/gen_i18n.py

Run it after editing either file.  i18n_js.h is generated; do not edit it.
"""
import json
from pathlib import Path

BASE = Path(__file__).resolve().parents[1]
DICT = BASE / "i18n" / "zh.json"
LOGIC = BASE / "i18n" / "i18n.js"
HEADER = BASE / "i18n_js.h"
TOKEN = "__I18N_DICT__"


def main():
    data = json.loads(DICT.read_text(encoding="utf-8"))
    strings = data.get("strings")
    if not isinstance(strings, dict) or not strings:
        raise SystemExit(f"{DICT}: expected a non-empty 'strings' object")
    for key, value in strings.items():
        if not key or not isinstance(value, str) or not value:
            raise SystemExit(f"{DICT}: bad entry {key!r}: {value!r}")

    logic = LOGIC.read_text(encoding="utf-8")
    if logic.count(TOKEN) != 1:
        raise SystemExit(f"{LOGIC}: expected exactly one {TOKEN} placeholder")

    js = logic.replace(
        TOKEN,
        json.dumps(strings, ensure_ascii=False, separators=(",", ":")),
    )

    escaped = js.replace("\\", "\\\\").replace('"', '\\"')
    lines = escaped.split("\n")
    out = [
        "#ifndef I18N_JS_H",
        "#define I18N_JS_H",
        "",
        "#include <stddef.h>",
        "",
        "/* Generated from i18n/i18n.js + i18n/zh.json by tools/gen_i18n.py.",
        " * Do not edit. */",
        "static const char i18n_js[] =",
    ]
    out.extend(f'    "{line}\\n"' for line in lines)
    out.extend([
        "    ;",
        "static const size_t i18n_js_len = sizeof(i18n_js) - 1;",
        "",
        "#endif /* I18N_JS_H */",
    ])
    HEADER.write_text("\n".join(out) + "\n", encoding="utf-8")
    print(f"wrote {HEADER} from {len(js)} bytes of JS, {len(strings)} entries")


if __name__ == "__main__":
    main()
