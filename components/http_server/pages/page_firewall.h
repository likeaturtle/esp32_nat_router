/* Firewall page templates */
/* Firewall (ACL) Page - chunked for streaming. Styles in /style.css. */
#define FIREWALL_CHUNK_HEAD \
    THEME_DOC_OPEN("Firewall")

/* Error modal is streamed before this chunk. */
#define FIREWALL_CHUNK_MID1 THEME_TOPBAR_OPEN

/* After logout: page intro. */
#define FIREWALL_CHUNK_MID2 \
    THEME_TOPBAR_CLOSE \
    "<div class='page-head'>" \
    "<p class='eyebrow'>Security</p>" \
    "<h1>Firewall</h1>" \
    "<p class='page-sub'>Rules are matched top-down. First match wins; no match means allow.</p>" \
    "</div>"

/* ACL sections are streamed here. */

/* Add form and footer. */
#define FIREWALL_CHUNK_TAIL \
    "<div class='card'>" \
    "<div class='card-head'><h2>Add ACL rule</h2><span class='meta'>top of list</span></div>" \
    "<form action='/firewall' method='GET'>" \
    "<table class='form-table'>" \
    "<tr><td>Direction</td><td><select name='acl_list'>" \
    "<option value='0'>Internet to ESP (to_esp)</option>" \
    "<option value='1'>ESP to Internet (from_esp)</option>" \
    "<option value='2'>Clients to ESP (to_ap)</option>" \
    "<option value='3'>ESP to clients (from_ap)</option>" \
    "</select></td></tr>" \
    "<tr><td>Protocol</td><td><select name='proto'>" \
    "<option value='0'>IP (any)</option>" \
    "<option value='6'>TCP</option>" \
    "<option value='17'>UDP</option>" \
    "<option value='1'>ICMP</option>" \
    "</select></td></tr>" \
    "<tr><td>Source IP</td><td><input type='text' name='src_ip' placeholder='any, IP/CIDR, or device name'/></td></tr>" \
    "<tr><td>Source port</td><td><input type='text' name='src_port' placeholder='* or port (TCP/UDP)'/></td></tr>" \
    "<tr><td>Dest IP</td><td><input type='text' name='dst_ip' placeholder='any, IP/CIDR, or device name'/></td></tr>" \
    "<tr><td>Dest port</td><td><input type='text' name='dst_port' placeholder='* or port (TCP/UDP)'/></td></tr>" \
    "<tr><td>Action</td><td><select name='action'>" \
    "<option value='1'>Allow</option>" \
    "<option value='0'>Deny</option>" \
    "<option value='3'>Allow + monitor</option>" \
    "<option value='2'>Deny + monitor</option>" \
    "</select></td></tr>" \
    "<tr><td colspan='2'><input type='submit' name='acl_action' value='Add rule' class='ok-button'/></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>" \
    THEME_HOME_ROW \
    THEME_FOOTER \
    THEME_PAGE_END
