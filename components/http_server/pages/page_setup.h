/* Getting Started page templates */
#include "router_config.h"

#if !CONFIG_ETH_UPLINK
/* Getting Started Page. Styles in /style.css. No logout slot on this page. */
#define SETUP_CHUNK_HEAD \
    THEME_DOC_OPEN("Getting Started") \
    THEME_TOPBAR_OPEN \
    THEME_TOPBAR_CLOSE \
    "<div class='page-head'>" \
    "<p class='eyebrow'>First run</p>" \
    "<h1>Getting started</h1>" \
    "<p class='page-sub'>Set the hotspot and uplink, then reboot.</p>" \
    "</div>" \
    "<script>" \
    "var qs = window.location.search.substr(1);" \
    "if (qs.indexOf('ap_ssid=') !== -1 || (qs.indexOf('ssid=') !== -1 && qs.indexOf('password=') !== -1)) {" \
    "document.body.innerHTML = '<main id=\"container\"><div class=\"page-head\"><p class=\"eyebrow\">Saved</p><h1>Rebooting</h1><p class=\"page-sub\">Settings saved. The device returns in about 10 seconds.</p></div></main>';" \
    "setTimeout(\"location.href = '/'\", 10000);" \
    "}" \
    "</script>"

/* Setup form - uses: safe_ap_ssid, safe_ssid, sta_open checked */
#define SETUP_CHUNK_FORM \
    "<div class='card'>" \
    "<div class='card-head'><h2>Quick setup</h2><span class='meta'>step 1</span></div>" \
    "<form action='/setup' method='GET'>" \
    "<h3>Access point</h3>" \
    "<table class='form-table'>" \
    "<tr><td>SSID</td><td><input type='text' name='ap_ssid' value='%s' placeholder='Hotspot name'/></td></tr>" \
    "<tr><td>Password</td><td><input type='password' name='ap_password' placeholder='unchanged'/></td></tr>" \
    "</table>" \
    "<h3>Uplink (internet)</h3>" \
    "<table class='form-table'>" \
    "<tr><td>SSID</td><td><input type='text' name='ssid' value='%s' placeholder='WiFi network'/></td></tr>" \
    "<tr><td>Password</td><td><input type='password' id='sta_pw' name='password' placeholder='unchanged' oninput=\"document.getElementById('sta_op').checked=false;\"/></td></tr>" \
    "<tr><td colspan='2'><label><input type='checkbox' id='sta_op' name='sta_open' value='1' %s onchange=\"if(this.checked)document.getElementById('sta_pw').value='';\"> <span class='hint'>Open network (no password)</span></label></td></tr>" \
    "<tr><td colspan='2'><input type='submit' value='Save &amp; reboot' class='ok-button'/></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>" \
    "<div class='home-row' style='justify-content:space-between;'>" \
    "<a class='btn-home' href='/scan'>Wi-Fi scan &#8594;</a>" \
    "<a class='btn-home' href='/'>&#8592; Home</a>" \
    "</div>" \
    THEME_FOOTER \
    THEME_PAGE_END
#endif /* !CONFIG_ETH_UPLINK */
