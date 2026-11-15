#!/usr/bin/env python3
"""Extract visible Web UI strings and audit i18n/zh.json.

The firmware serves the UI from C string literals (pages/*.h, theme.h and the
inline HTML in http_server.c).  Translations are keyed by the exact English
text, so the English source stays the single source of truth: when upstream
changes a label the lookup simply misses and the page falls back to English.

Run after touching any page or handler:

    python3 tools/i18n_extract.py           # summary + missing / obsolete keys
    python3 tools/i18n_extract.py --list    # every extracted string

Exit code is 1 when zh.json is missing keys, so a release script can gate on
it.  JavaScript strings are reported separately: inline <script> text is not
DOM text, so those must be wrapped with t('...') by hand.
"""
import html
import json
import re
import sys
from pathlib import Path
from html.parser import HTMLParser

BASE = Path(__file__).resolve().parents[1]
REPO = BASE.parents[1]
SOURCES = [
    BASE / "pages" / "page_index.h",
    BASE / "pages" / "page_config.h",
    BASE / "pages" / "page_mappings.h",
    BASE / "pages" / "page_firewall.h",
    BASE / "pages" / "page_vpn.h",
    BASE / "pages" / "page_setup.h",
    BASE / "pages" / "page_scan.h",
    BASE / "theme.h",
    BASE / "http_server.c",
    REPO / "include" / "router_config.h",
]
DICT_FILE = BASE / "i18n" / "zh.json"

# Labels that stay identical in Chinese (protocol names, units, brand).
AS_IS = {
    "ESP32 NAT Router", "ESP32 NAT Router (LAN)", "Web UI", "ESP-IDF",
    "SSID", "AP", "STA", "ETH", "VPN", "NAT", "MAC", "DHCP", "PCAP", "IPv4",
    "IP", "TCP", "UDP", "ICMP", "WPA2", "WPA3", "WPA2 / WPA3", "PEAP", "TTLS",
    "TLS", "MSCHAPv2", "MSCHAP", "PAP", "CHAP", "GHz", "dBm", "N/A", "RF",
    "IPv4", "telnet", "steps",
    "IEEE 802.11", "PHY", "MTU", "DNS", "RSSI", "WireGuard VPN",
    "2.4 / 5 GHz", "AA:BB:CC:DD:EE:FF", "esp32-nat-router", "vx",
    "web_ui bind all", "web_ui enable",
}
# Fragments produced by concatenated C literals or macro boundaries.
FRAGMENTS = {"Applied immediately, no reboot needed.", "Build", "out", "in", "since"}
# Sentences composed at compile time (ANTENNA_BOARD_HINT + suffix); the full
# text never appears as one literal, so they can look stale to the audit.
COMPOSED = {
    "Waveshare ESP32-C5-Zero: control GPIO 26, no enable pin (0 = on-board, 1 = external IPEX-1). Applied immediately, no reboot needed.",
    "XIAO ESP32-C6: control GPIO 14 with enable GPIO 3 (0 = on-board ceramic, 1 = external). Applied immediately, no reboot needed.",
    "Check the board schematic for the GPIO that drives the RF switch. Applied immediately, no reboot needed.",
}

LITERAL = re.compile(r'"((?:\\.|[^"\\])*)"')

ESCAPES = {
    "n": "\n", "t": "\t", "r": "\r", '"': '"', "'": "'", "\\": "\\",
    "0": "\0", "a": "\a", "b": "\b", "f": "\f", "v": "\v",
}


def c_unescape(text: str) -> str:
    out = []
    i = 0
    while i < len(text):
        ch = text[i]
        if ch == "\\" and i + 1 < len(text):
            nxt = text[i + 1]
            if nxt == "x" and i + 3 < len(text):
                try:
                    out.append(chr(int(text[i + 2:i + 4], 16)))
                    i += 4
                    continue
                except ValueError:
                    pass
            out.append(ESCAPES.get(nxt, nxt))
            i += 2
        else:
            out.append(ch)
            i += 1
    return "".join(out)


def scan_literals(text: str):
    """Return (value, start, end) for every string literal, skipping comments.

    A plain regex desynchronises on quotes inside /* ... */ comments, which
    silently hides later literals, so walk the file with a small lexer.
    """
    out = []
    i, n = 0, len(text)
    while i < n:
        ch = text[i]
        if ch == "/" and i + 1 < n:
            if text[i + 1] == "/":
                end = text.find("\n", i)
                i = n if end < 0 else end + 1
                continue
            if text[i + 1] == "*":
                end = text.find("*/", i + 2)
                i = n if end < 0 else end + 2
                continue
        if ch == '"':
            j = i + 1
            buf = []
            while j < n:
                cur = text[j]
                if cur == "\\" and j + 1 < n:
                    buf.append(text[j:j + 2])
                    j += 2
                    continue
                if cur == '"':
                    break
                buf.append(cur)
                j += 1
            out.append((c_unescape("".join(buf)), i, j + 1))
            i = j + 1
            continue
        if ch == "'":
            j = i + 1
            while j < n:
                if text[j] == "\\":
                    j += 2
                    continue
                if text[j] == "'":
                    break
                j += 1
            i = j + 1
            continue
        i += 1
    return out


def string_runs(path: Path):
    """Yield runs of adjacent C string literals (C concatenation)."""
    text = path.read_text(encoding="utf-8").replace("\\\n", "")
    run = []
    for value, start, end in scan_literals(text):
        if run:
            gap = text[run[-1][2]:start]
            if gap.strip() == "":
                run.append((value, start, end))
                continue
            yield "".join(v for v, _, _ in run)
        run = [(value, start, end)]
    if run:
        yield "".join(v for v, _, _ in run)


class Collector(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.depth = 0
        self.text = []
        self.attrs = []
        self.js = []

    def handle_starttag(self, tag, attrs):
        if tag in ("script", "style", "noscript"):
            self.depth += 1
        self._attrs(tag, attrs)

    def handle_startendtag(self, tag, attrs):
        self._attrs(tag, attrs)

    def handle_endtag(self, tag):
        if tag in ("script", "style", "noscript") and self.depth:
            self.depth -= 1

    def handle_data(self, data):
        if self.depth:
            self.js.append(data)
        else:
            self.text.append(data)

    def _attrs(self, tag, attrs):
        values = dict(attrs)
        for name in ("placeholder", "title", "aria-label", "alt"):
            if values.get(name):
                self.attrs.append(values[name])
        if tag == "input" and (values.get("type") or "").lower() in ("submit", "button", "reset"):
            if values.get("value"):
                self.attrs.append(values["value"])


def norm(text: str) -> str:
    return re.sub(r"\s+", " ", text).strip()


def interesting(text: str) -> bool:
    text = norm(text)
    if len(text) < 2 or not re.search(r"[A-Za-z]{2}", text):
        return False
    if "%" in text or "://" in text:
        return False
    if any(ch in text for ch in "<>{};=*\\"):
        return False
    if text.startswith(("#", ".", "//", "/*", "http", "www.")):
        return False
    if re.match(r"^(document|window|function|var|let|const|return|if|else|new)\b", text):
        return False
    if re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", text) and text.isupper():
        return False
    return True


def extract():
    strings = set()
    dynamic = set()
    js_strings = set()
    titles = set()

    for path in SOURCES:
        raw = path.read_text(encoding="utf-8")
        for match in re.finditer(r'THEME_DOC_OPEN(?:_REFRESH)?\s*\(\s*"([^"]*)"', raw):
            titles.add(norm(match.group(1)))
        for match in re.finditer(r'#define\s+\w*_TITLE\s+"([^"]*)"', raw):
            titles.add(norm(match.group(1)))

        for run in string_runs(path):
            if "<" not in run:
                continue
            # Format specifiers would hide the label next to them; drop in a
            # placeholder so "<td>%d associated</td>" still yields "associated"
            # (the number itself is handled by a pattern rule in i18n.js).
            run = re.sub(
                r"%(?:[-+ #0]*\d*(?:\.\d+)?(?:hh|h|ll|l|z|j|t)?[diouxXcsfgep%])",
                lambda m: "%" if m.group(0) == "%%" else "x",
                run,
            )
            collector = Collector()
            try:
                collector.feed(run)
                collector.close()
            except Exception:
                continue
            for piece in collector.text + collector.attrs:
                if not interesting(piece):
                    continue
                piece = norm(piece)
                if re.search(r"(?<![\w])x(?![\w])", piece):
                    dynamic.add(piece)
                else:
                    strings.add(piece)
            # JS text never becomes DOM text on its own; only t('...') calls
            # need dictionary entries, so audit exactly those.
            for match in re.finditer(r"\bt\(\s*'((?:\\.|[^'\\])*)'", run):
                piece = c_unescape(match.group(1))
                if interesting(piece):
                    js_strings.add(norm(piece))

    strings |= titles
    strings -= FRAGMENTS
    strings -= AS_IS
    js_strings -= {s for s in js_strings if s in strings}
    return strings, dynamic, js_strings


def load_dict():
    if not DICT_FILE.exists():
        return {}
    data = json.loads(DICT_FILE.read_text(encoding="utf-8"))
    return data.get("strings", {})


def main():
    strings, dynamic, js_strings = extract()
    dictionary = load_dict()

    # A key may be reachable only through t(), an attribute handler, a JSON
    # message or text the DOM walker sees after formatting. Substring search
    # over every literal (entity-decoded) is the cheap, low-false-positive way
    # to spot keys that no longer exist anywhere.
    corpus = []
    for path in SOURCES:
        for run in string_runs(path):
            corpus.append(run)
    corpus = html.unescape("\n".join(corpus))

    missing = sorted(s for s in strings if s not in dictionary)
    unused = sorted(k for k in dictionary if k not in corpus and k not in COMPOSED)

    print(f"extracted {len(strings)} UI strings, dictionary has {len(dictionary)}")
    if missing:
        print(f"\nMISSING ({len(missing)}):")
        for item in missing:
            print(f"  {item}")
    if unused:
        print(f"\nUNUSED ({len(unused)}):")
        for item in unused:
            print(f"  {item}")
    if dynamic:
        print(f"\nDYNAMIC strings (numbers/values; cover with a pattern rule in i18n.js; {len(dynamic)}):")
        for item in sorted(dynamic):
            print(f"  {item}")
    if js_strings:
        print(f"\nJS strings passed through t('...') (must exist in zh.json; {len(js_strings)}):")
        for item in sorted(js_strings):
            print(f"  {item}")
    if "--list" in sys.argv:
        print("\nALL STRINGS:")
        for item in sorted(strings):
            print(f"  {item}")

    return 1 if missing else 0


if __name__ == "__main__":
    sys.exit(main())
