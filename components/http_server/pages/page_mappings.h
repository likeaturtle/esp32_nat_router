/* Mappings page templates */
#include "router_config.h"

/* Mappings Page (DHCP Reservations + Port Forwarding). Styles in /style.css. */
#define MAPPINGS_CHUNK_HEAD \
    THEME_DOC_OPEN("Mappings") \
    "<script>" \
    "function fillDhcpForm(mac, ip, name) {" \
    "document.getElementById('dhcp_mac').value = mac;" \
    "document.getElementById('dhcp_ip').value = ip;" \
    "document.getElementById('dhcp_name').value = name;" \
    "document.getElementById('dhcp_mac').scrollIntoView({behavior: 'smooth', block: 'center'});" \
    "}" \
    "</script>"

/* Error modal is streamed before this chunk. */
#define MAPPINGS_CHUNK_MID1 THEME_TOPBAR_OPEN

/* After logout: page intro + connected clients card. */
#define MAPPINGS_CHUNK_MID2 \
    THEME_TOPBAR_CLOSE \
    "<div class='page-head'>" \
    "<p class='eyebrow'>Network</p>" \
    "<h1>Mappings</h1>" \
    "<p class='page-sub'>Known clients, fixed addresses and forwarded ports.</p>" \
    "</div>" \
    "<div class='card'>" \
    "<div class='card-head'><h2>Connected clients</h2></div>" \
    "<div class='table-scroll'>" \
    "<table class='data-table'>" \
    "<thead><tr>" \
    "<th>MAC address</th><th>IP address</th><th>Device name</th><th class='traffic-col'>Traffic (TX / RX)</th><th>Action</th>" \
    "</tr></thead>" \
    "<tbody>"

/* Same table without the Traffic column (used when per-client stats are disabled) */
#define MAPPINGS_CHUNK_MID2_NOSTATS \
    THEME_TOPBAR_CLOSE \
    "<div class='page-head'>" \
    "<p class='eyebrow'>Network</p>" \
    "<h1>Mappings</h1>" \
    "<p class='page-sub'>Known clients, fixed addresses and forwarded ports.</p>" \
    "</div>" \
    "<div class='card'>" \
    "<div class='card-head'><h2>Connected clients</h2></div>" \
    "<div class='table-scroll'>" \
    "<table class='data-table'>" \
    "<thead><tr>" \
    "<th>MAC address</th><th>IP address</th><th>Device name</th><th>Action</th>" \
    "</tr></thead>" \
    "<tbody>"

/* Close clients card, open DHCP reservations card (pool info + known-only
 * toggle are streamed between this and MID3B). */
#define MAPPINGS_CHUNK_MID3 \
    "</tbody></table></div></div>" \
    "<div class='card'>" \
    "<div class='card-head'><h2>DHCP reservations</h2><span class='meta'>fixed IP</span></div>" \
    "<div class='card-body'>"

/* "Allow only known clients" switch, streamed after the pool info:
   MAPPINGS_CHUNK_KNOWN_PRE + ("checked" or "") + MAPPINGS_CHUNK_KNOWN_POST */
#define MAPPINGS_CHUNK_KNOWN_PRE \
    "<form action='/mappings' method='GET' style='margin:12px 0 2px;'>" \
    "<input type='hidden' name='set_known' value='1'/>" \
    "<label style='display:inline-flex;align-items:center;gap:8px;cursor:pointer;'><input type='checkbox' name='known_only' value='1' "

#define MAPPINGS_CHUNK_KNOWN_POST " \
onchange=\"if(!this.checked||confirm(t('Only clients with a fixed-IP reservation below will be able to connect. Make sure your own device is listed.')))this.form.submit();else this.checked=false;\"> \
<span>Allow only known clients</span></label> \
<p class='hint-block'>MACs with a fixed-IP reservation below; applies to new connections.</p>\
</form>"

#define MAPPINGS_CHUNK_MID3B \
    "</div>" \
    "<div class='table-scroll'>" \
    "<table class='data-table'>" \
    "<thead><tr>" \
    "<th>MAC address</th><th>IP address</th><th>Name</th><th>Action</th>" \
    "</tr></thead>" \
    "<tbody>"

/* After dhcp tbody: close the reservations card and open the add form. */
#define MAPPINGS_CHUNK_MID4 \
    "</tbody></table></div></div>" \
    "<div class='card'>" \
    "<div class='card-head'><h2>Add reservation</h2><span class='meta'>DHCP</span></div>" \
    "<form action='/mappings' method='GET'>" \
    "<table class='form-table'>" \
    "<tr><td>MAC address</td><td><input type='text' name='dhcp_mac' id='dhcp_mac' placeholder='AA:BB:CC:DD:EE:FF'/></td></tr>" \
    "<tr><td>IP address</td><td><input type='text' name='dhcp_ip' id='dhcp_ip' placeholder='192.168.4.100'/></td></tr>" \
    "<tr><td>Name (optional)</td><td><input type='text' name='dhcp_name' id='dhcp_name' placeholder='My Device'/></td></tr>" \
    "<tr><td colspan='2'><input type='submit' name='dhcp_action' value='Add reservation' class='ok-button'/>" \
    "<input type='submit' name='dhcp_action' value='Block device' class='red-button' onclick=\"document.getElementById('dhcp_ip').value='0.0.0.0';\"/></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>"

#define MAPPINGS_CHUNK_PORTFWD_HEAD \
    "<div class='card'>" \
    "<div class='card-head'><h2>Port forwarding</h2><span class='meta'>NAT</span></div>" \
    "<div class='table-scroll'>" \
    "<table class='data-table'>" \
    "<thead><tr>" \
    "<th>Interface</th><th>Protocol</th><th>Ext. port</th><th>Internal IP</th><th>Int. port</th><th>Action</th>" \
    "</tr></thead>" \
    "<tbody>"

#if CONFIG_ETH_UPLINK
#define PORTMAP_IFACE_OPTIONS "<option value='STA'>ETH (Ethernet)</option><option value='VPN'>VPN</option>"
#else
#define PORTMAP_IFACE_OPTIONS "<option value='STA'>STA (WiFi)</option><option value='VPN'>VPN</option>"
#endif

/* After portmap tbody: close the port table and add the forward form. */
#define MAPPINGS_CHUNK_PORTFWD_TAIL \
    "</tbody></table></div></div>" \
    "<div class='card'>" \
    "<div class='card-head'><h2>Add port forward</h2><span class='meta'>NAT</span></div>" \
    "<form action='/mappings' method='GET'>" \
    "<table class='form-table'>" \
    "<tr><td>Interface</td><td><select name='iface'>" PORTMAP_IFACE_OPTIONS "</select></td></tr>" \
    "<tr><td>Protocol</td><td><select name='proto'><option value='TCP'>TCP</option><option value='UDP'>UDP</option></select></td></tr>" \
    "<tr><td>External port</td><td><input type='number' name='ext_port' min='1' max='65535' placeholder='8080'/></td></tr>" \
    "<tr><td>Internal IP</td><td><input type='text' name='int_ip' placeholder='IP or device name'/></td></tr>" \
    "<tr><td>Internal port</td><td><input type='number' name='int_port' min='1' max='65535' placeholder='80'/></td></tr>" \
    "<tr><td colspan='2'><input type='submit' name='port_action' value='Add forward' class='ok-button'/></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>"

#define MAPPINGS_CHUNK_PAGE_FOOTER \
    THEME_HOME_ROW \
    THEME_FOOTER \
    THEME_PAGE_END
