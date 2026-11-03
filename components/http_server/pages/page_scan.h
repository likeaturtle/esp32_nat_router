/* WiFi Scan page templates */
#include "router_config.h"

#if !CONFIG_ETH_UPLINK
/* WiFi Scan Page - one formatted string: %d refresh, %d count, %s extra
 * header cell, %s rows. Styles in /style.css. */
#define SCAN_PAGE \
    THEME_DOC_OPEN_REFRESH("WiFi Scan", "%d") \
    THEME_TOPBAR_OPEN \
    THEME_TOPBAR_CLOSE \
    "<div class='page-head'>" \
    "<p class='eyebrow'>Wireless</p>" \
    "<h1>Wi-Fi scan</h1>" \
    "<p class='page-sub'>Auto-refreshes every few seconds. Found %d networks.</p>" \
    "</div>" \
    "<div class='card'>" \
    "<div class='card-head'><h2>Nearby networks</h2><span class='meta'>2.4 / 5 GHz</span></div>" \
    "<div class='table-scroll'>" \
    "<table class='data-table'>" \
    "<thead><tr>" \
    "<th>SSID</th><th>Signal</th><th>Ch</th><th>Security</th>%s" \
    "</tr></thead>" \
    "<tbody>%s</tbody>" \
    "</table>" \
    "</div>" \
    "</div>" \
    "<div class='home-row' style='justify-content:space-between;'>" \
    "<a class='btn-home' href='/scan'>Scan again</a>" \
    "<a class='btn-home' href='/'>&#8592; Home</a></div>" \
    THEME_FOOTER \
    THEME_PAGE_END
#endif /* !CONFIG_ETH_UPLINK */
