/* Index page templates */
#include "router_config.h"

#if CONFIG_ETH_UPLINK
#define INDEX_TITLE "ESP32 NAT Router (LAN)"
#else
#define INDEX_TITLE "ESP32 NAT Router"
#endif

/* Index Page - Chunked for streaming.
 * The styles live in /style.css (theme_css.h). */
#define INDEX_CHUNK_HEAD \
    THEME_DOC_OPEN(INDEX_TITLE) \
    THEME_TOPBAR_OPEN

/* Logout link is streamed between HEAD and STATUS_OPEN. */
#define INDEX_CHUNK_STATUS_OPEN \
    THEME_TOPBAR_CLOSE \
    "<div class='page-head'>" \
    "<p class='eyebrow'>Dashboard</p>" \
    "<h1>System status</h1>" \
    "<p class='page-sub'>Uplink health, clients and quick links.</p>" \
    "</div>" \
    "<div class='card'>" \
    "<div class='card-head'><h2>Connection</h2></div>" \
    "<table class='status-table'>"

/* Status rows are streamed here. */

#define INDEX_CHUNK_STATUS_CLOSE \
    "</table></div>"

#if !CONFIG_ETH_UPLINK
#define INDEX_CHUNK_BUTTONS \
    "<nav class='nav-grid' aria-label='Sections'>" \
    "<a class='nav-tile' href='/setup'><span class='n'>01</span>Getting started<span class='arr'>&#8594;</span></a>" \
    "<a class='nav-tile' href='/scan'><span class='n'>02</span>Wi-Fi scan<span class='arr'>&#8594;</span></a>" \
    "<a class='nav-tile' href='/config'><span class='n'>03</span>Configuration<span class='arr'>&#8594;</span></a>" \
    "<a class='nav-tile' href='/mappings'><span class='n'>04</span>Mappings<span class='arr'>&#8594;</span></a>" \
    "<a class='nav-tile' href='/firewall'><span class='n'>05</span>Firewall<span class='arr'>&#8594;</span></a>" \
    "<a class='nav-tile' href='/vpn'><span class='n'>06</span>VPN<span class='arr'>&#8594;</span></a>" \
    "</nav>"
#else
#define INDEX_CHUNK_BUTTONS \
    "<nav class='nav-grid' aria-label='Sections'>" \
    "<a class='nav-tile' href='/config'><span class='n'>01</span>Configuration<span class='arr'>&#8594;</span></a>" \
    "<a class='nav-tile' href='/mappings'><span class='n'>02</span>Mappings<span class='arr'>&#8594;</span></a>" \
    "<a class='nav-tile' href='/firewall'><span class='n'>03</span>Firewall<span class='arr'>&#8594;</span></a>" \
    "<a class='nav-tile' href='/vpn'><span class='n'>04</span>VPN<span class='arr'>&#8594;</span></a>" \
    "</nav>"
#endif

/* Auth UI is streamed here. */

#define INDEX_CHUNK_TAIL \
    "<footer class='foot'>" \
    "<span class='mono'>v%s</span><span class='sep'>/</span>" \
    "<span>Build %s %s</span><span class='sep'>/</span>" \
    "<span class='mono'>ESP-IDF " IDF_VER "</span><span class='sep'>/</span>" \
    "<a href='https://github.com/martin-ger/esp32_nat_router' target='_blank' rel='noopener'>Source code</a>" \
    "</footer>" \
    "</main></body></html>"
