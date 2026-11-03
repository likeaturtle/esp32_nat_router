/* Shared markup helpers for the web UI theme. */
#ifndef THEME_H
#define THEME_H

/* Product mark: a small instrument-style signal scale. */
#define THEME_BRAND_SVG \
    "<svg width='15' height='15' viewBox='0 0 16 16' fill='none' stroke='currentColor' stroke-width='1.5' stroke-linecap='square' aria-hidden='true'>" \
    "<path d='M2.5 12.5V10M5.8 12.5V7.5M9.1 12.5V4.8M12.4 12.5V2.5'/></svg>"

/* <head> + <body>; the page supplies its own <title>. */
#define THEME_DOC_OPEN(title) \
    "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>" \
    "<meta name='viewport' content='width=device-width, initial-scale=1'>" \
    "<title>" title "</title>" \
    "<link rel='icon' href='/favicon.png'>" \
    "<link rel='stylesheet' href='/style.css'>" \
    "</head><body>"

/* Same as THEME_DOC_OPEN, plus an auto-refresh interval (seconds is a
 * string literal, typically "%d"). */
#define THEME_DOC_OPEN_REFRESH(title, seconds) \
    "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>" \
    "<meta name='viewport' content='width=device-width, initial-scale=1'>" \
    "<meta http-equiv='refresh' content='" seconds "'>" \
    "<title>" title "</title>" \
    "<link rel='icon' href='/favicon.png'>" \
    "<link rel='stylesheet' href='/style.css'>" \
    "</head><body>"

/* Brand bar, ends with the action slot open so the logout link can be
 * streamed conditionally. Close it with THEME_TOPBAR_CLOSE. */
#define THEME_TOPBAR_OPEN \
    "<header class='topbar'><div class='topbar-in'>" \
    "<a class='brand' href='/' aria-label='ESP32 NAT Router home'>" \
    "<span class='brand-mark'>" THEME_BRAND_SVG "</span>" \
    "<span class='brand-text'><span class='brand-name'>ESP32 NAT Router</span>" \
    "<span class='brand-sub'>Web UI</span></span>" \
    "</a><div class='topbar-actions'>"

#define THEME_TOPBAR_CLOSE \
    "</div></div></header><main id='container'>"

#define THEME_LOGOUT \
    "<a class='logout' href='/?logout=1'>" \
    "<svg width='11' height='11' viewBox='0 0 12 12' fill='none' stroke='currentColor' " \
    "stroke-width='1.3' stroke-linecap='round' stroke-linejoin='round' aria-hidden='true'>" \
    "<path d='M5 2H2.5A1.5 1.5 0 0 0 1 3.5v5A1.5 1.5 0 0 0 2.5 10H5M8 8.5 10.5 6 8 3.5M10.5 6H4.5'/></svg>" \
    "Log out</a>"

#define THEME_FOOTER \
    "<footer class='foot'><span>ESP32 NAT Router</span><span class='sep'>/</span>" \
    "<a href='https://github.com/martin-ger/esp32_nat_router' target='_blank' rel='noopener'>Source</a></footer>"

#define THEME_HOME_ROW \
    "<div class='home-row'><a class='btn-home' href='/'>&#8592; Home</a></div>"

#define THEME_PAGE_END \
    "</main></body></html>"

#endif /* THEME_H */
