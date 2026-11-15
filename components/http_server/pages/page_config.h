/* Configuration page templates */
#include "router_config.h"
#include "wifi_config.h"

/* Configuration Page - Chunked for streaming. Styles live in /style.css. */
#define CONFIG_CHUNK_HEAD \
    THEME_DOC_OPEN("Configuration") \
    THEME_TOPBAR_OPEN

/* After the logout slot: page intro + the reboot-after-save script. */
#define CONFIG_CHUNK_SCRIPT \
    THEME_TOPBAR_CLOSE \
    "<div class='page-head'>" \
    "<p class='eyebrow'>Network</p>" \
    "<h1>Configuration</h1>" \
    "<p class='page-sub'>Wi-Fi, addressing, remote access and firmware.</p>" \
    "</div>" \
    "<script>" \
    "var qs = window.location.search.substr(1);" \
    "var formSubmitted = qs.indexOf('ap_ssid=') !== -1 || (qs.indexOf('ssid=') !== -1 && qs.indexOf('password=') !== -1) || qs.indexOf('staticip=') !== -1 || qs.indexOf('reset=') !== -1;" \
    "if (qs.indexOf('disable_interface=') !== -1) {" \
    "document.body.innerHTML = '<main id=\"container\"><div class=\"page-head\"><p class=\"eyebrow\">Web UI</p><h1>Interface disabled</h1><p class=\"page-sub\">Re-enable it from the console with <code>web_ui enable</code>.</p></div></main>';" \
    "} else if (formSubmitted) {" \
    "document.body.innerHTML = '<main id=\"container\"><div class=\"page-head\"><p class=\"eyebrow\">Saved</p><h1>Rebooting</h1><p class=\"page-sub\">Settings saved. The device returns in about 10 seconds.</p></div></main>';" \
    "setTimeout(\"location.href = '/'\", 10000);" \
    "}" \
    "</script>"

/* AP Settings section - uses %s for: ap_ssid, ap_ip, ap_hostname, ap_dns, ap_mac, ap_en_checked, open_checked, hidden_checked */
/* ETH_UPLINK builds add an extra %d for ap_channel */
#if CONFIG_ETH_UPLINK
#define CONFIG_CHUNK_AP_CHANNEL_ROW \
    "<tr><td>Channel</td><td><input type='number' name='ap_channel' min='0' max='13' value='%d'/> <span class='hint'>(0 = auto)</span></td></tr>"
#else
#define CONFIG_CHUNK_AP_CHANNEL_ROW ""
#endif

#define CONFIG_CHUNK_AP \
    "<div class='card'>" \
    "<div class='card-head'><h2>Access point</h2><span class='meta'>AP</span></div>" \
    "<form action='' method='GET'>" \
    "<table class='form-table'>" \
    "<tr><td>SSID</td><td><input type='text' name='ap_ssid' value='%s' placeholder='Network name'/></td></tr>" \
    "<tr><td>Password</td><td><input type='password' id='ap_pw' name='ap_password' placeholder='unchanged' oninput=\"document.getElementById('ap_op').checked=false;\"/></td></tr>" \
    "<tr><td>AP IP address</td><td><input type='text' name='ap_ip_addr' value='%s' placeholder='192.168.4.1'/></td></tr>" \
    "<tr><td>Hostname</td><td><input type='text' name='ap_hostname' value='%s' placeholder='esp32-nat-router' maxlength='32' pattern='[A-Za-z0-9-]*'/></td></tr>" \
    "<tr><td>DNS server</td><td><input type='text' name='ap_dns' value='%s' placeholder='empty = use upstream DNS'/></td></tr>" \
    "<tr><td>MAC address</td><td><input type='text' name='ap_mac' value='%s' placeholder='AA:BB:CC:DD:EE:FF'/></td></tr>" \
    CONFIG_CHUNK_AP_CHANNEL_ROW \
    "<tr><td>Security</td><td><select name='ap_auth'><option value='0' %s>WPA2 / WPA3</option><option value='1' %s>WPA2 only</option><option value='2' %s>WPA3 only</option></select></td></tr>" \
    "<tr><td>NAT</td><td><label><input type='checkbox' name='ap_nat' value='1' %s> <span class='hint'>Enabled (uncheck for routed mode)</span></label></td></tr>" \
    "<tr><td>Enabled</td><td><label><input type='checkbox' name='ap_enabled' value='1' %s> <span class='hint'>AP interface active</span></label></td></tr>" \
    "<tr><td>Options</td><td><label><input type='checkbox' id='ap_op' name='ap_open' value='1' %s onchange=\"if(this.checked)document.getElementById('ap_pw').value='';\"> <span class='hint'>Open (no password)</span></label><label><input type='checkbox' name='ap_hidden' value='1' %s> <span class='hint'>Hidden SSID</span></label></td></tr>" \
    "<tr><td colspan='2'><input type='submit' value='Save &amp; reboot' class='ok-button'/></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>"

#if !CONFIG_ETH_UPLINK
/* STA band preference row (5 GHz capable targets only) */
/* Uses %s x3 for: auto_selected, 2.4_selected, 5_selected */
#if WIFI_HAS_5GHZ
#define CONFIG_CHUNK_STA_BAND_ROW \
    "<tr><td>Band</td><td><select name='sta_band'>" \
    "<option value='0' %s>Auto (strongest signal)</option>" \
    "<option value='1' %s>2.4 GHz only</option>" \
    "<option value='2' %s>5 GHz only</option>" \
    "</select></td></tr>"
#else
#define CONFIG_CHUNK_STA_BAND_ROW ""
#endif

/* STA Settings section - uses %s for: ssid, sta_open checked, ent_username, ent_identity,
   eap_method selected x4, ttls_phase2 selected x4, cert_bundle checked, no_time_chk checked,
   [if WIFI_HAS_5GHZ: band_auto_sel, band_2g_sel, band_5g_sel,]
   sta_mac */
#define CONFIG_CHUNK_STA \
    "<div class='card'>" \
    "<div class='card-head'><h2>Station settings</h2><span class='meta'>uplink</span></div>" \
    "<form action='' method='GET'>" \
    "<table class='form-table'>" \
    "<tr><td>SSID</td><td><input type='text' name='ssid' value='%s' placeholder='Uplink network'/></td></tr>" \
    "<tr><td>Password</td><td><input type='password' id='sta_pw' name='password' placeholder='unchanged' oninput=\"document.getElementById('sta_op').checked=false;\"/></td></tr>" \
    "<tr><td colspan='2'><label><input type='checkbox' id='sta_op' name='sta_open' value='1' %s onchange=\"if(this.checked)document.getElementById('sta_pw').value='';\"> <span class='hint'>Open network (no password)</span></label></td></tr>" \
    CONFIG_CHUNK_STA_BAND_ROW \
    "<tr><td colspan='2' class='group-label'>WPA2 Enterprise (optional)</td></tr>" \
    "<tr><td>Username</td><td><input type='text' name='ent_username' value='%s' placeholder='Enterprise username'/></td></tr>" \
    "<tr><td>Identity</td><td><input type='text' name='ent_identity' value='%s' placeholder='Optional (defaults to username)'/></td></tr>" \
    "<tr><td>EAP method</td><td><select name='eap_method'>" \
    "<option value='0' %s>Auto</option>" \
    "<option value='1' %s>PEAP</option>" \
    "<option value='2' %s>TTLS</option>" \
    "<option value='3' %s>TLS</option>" \
    "</select></td></tr>" \
    "<tr><td>TTLS phase 2</td><td><select name='ttls_phase2'>" \
    "<option value='0' %s>MSCHAPv2</option>" \
    "<option value='1' %s>MSCHAP</option>" \
    "<option value='2' %s>PAP</option>" \
    "<option value='3' %s>CHAP</option>" \
    "</select></td></tr>" \
    "<tr><td>Options</td><td>" \
    "<label><input type='checkbox' name='cert_bundle' value='1' %s> <span class='hint'>Use CA cert bundle</span></label>" \
    "<label><input type='checkbox' name='no_time_chk' value='1' %s> <span class='hint'>Skip cert time check</span></label>" \
    "</td></tr>" \
    "<tr><td>MAC address</td><td><input type='text' name='sta_mac' value='%s' placeholder='AA:BB:CC:DD:EE:FF'/></td></tr>" \
    "<tr><td colspan='2'><input type='submit' value='Save &amp; reboot' class='ok-button'/></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>"
#endif

/* Static IP section - uses %s for: static_ip, subnet_mask, gateway */
#define CONFIG_CHUNK_STATIC \
    "<div class='card'>" \
    "<div class='card-head'><h2>Static IP</h2><span class='meta'>IPv4</span></div>" \
    "<form action='' method='GET'>" \
    "<table class='form-table'>" \
    "<tr><td>Static IP</td><td><input type='text' name='staticip' value='%s' placeholder='192.168.1.100'/></td></tr>" \
    "<tr><td>Subnet mask</td><td><input type='text' name='subnetmask' value='%s' placeholder='255.255.255.0'/></td></tr>" \
    "<tr><td>Gateway</td><td><input type='text' name='gateway' value='%s' placeholder='192.168.1.1'/></td></tr>" \
    "<tr><td colspan='2'><input type='submit' value='Save &amp; reboot' class='ok-button'/>" \
    "<p class='hint-block'>Leave empty for DHCP. Saving reboots the router.</p></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>"

/* Remote Console section - uses: rc_en_chk, rc_dis_chk, rc_color, rc_status, rc_kick, rc_port, rc_ap_chk, rc_sta_chk, rc_vpn_chk, rc_timeout */
#define CONFIG_CHUNK_RC \
    "<div class='card'>" \
    "<div class='card-head'><h2>Remote console</h2><span class='meta'>telnet</span></div>" \
    "<form action='' method='GET'>" \
    "<input type='hidden' name='rc_save' value='1'/>" \
    "<table class='form-table'>" \
    "<tr><td>Service</td><td>" \
    "<label><input type='radio' name='rc_enabled' value='1' %s> <span class='hint'>Enabled</span></label>" \
    "<label><input type='radio' name='rc_enabled' value='0' %s> <span class='hint'>Disabled</span></label>" \
    "</td></tr>" \
    "<tr><td>Status</td><td><strong class='%s'>%s</strong>%s</td></tr>" \
    "<tr><td>Port</td><td><input type='number' name='rc_port' value='%d' min='1' max='65535'/></td></tr>" \
    "<tr><td>Bind interfaces</td><td>" \
    "<label><input type='checkbox' name='rc_bind_ap' value='1' %s> <span class='hint'>AP</span></label>" \
    "<label><input type='checkbox' name='rc_bind_sta' value='1' %s> <span class='hint'>STA</span></label>" \
    "<label><input type='checkbox' name='rc_bind_vpn' value='1' %s> <span class='hint'>VPN</span></label>" \
    "</td></tr>" \
    "<tr><td>Idle timeout</td><td><input type='number' name='rc_timeout' value='%lu' min='0' max='86400'/> <span class='hint'>seconds (0 = no timeout)</span></td></tr>" \
    "<tr><td colspan='2'><input type='submit' value='Save' class='ok-button'/></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>"

/* PCAP section - uses: pcap_off/acl/promisc_sel, pcap_color, pcap_status, captured, dropped, snaplen, sta_ip */
#define CONFIG_CHUNK_PCAP \
    "<div class='card'>" \
    "<div class='card-head'><h2>Packet capture</h2><span class='meta'>PCAP</span></div>" \
    "<form action='' method='GET'>" \
    "<input type='hidden' name='pcap_save' value='1'/>" \
    "<table class='form-table'>" \
    "<tr><td>Mode</td><td><select name='pcap_mode'>" \
    "<option value='off' %s>Off</option>" \
    "<option value='acl' %s>ACL monitor</option>" \
    "<option value='promisc' %s>Promiscuous</option>" \
    "</select></td></tr>" \
    "<tr><td>Client</td><td><strong class='%s'>%s</strong></td></tr>" \
    "<tr><td>Stats</td><td><span class='mono'>%lu</span> captured <span class='muted-text'>/</span> <span class='mono'>%lu</span> dropped</td></tr>" \
    "<tr><td>Snaplen</td><td><input type='text' name='pcap_snaplen' value='%d' placeholder='64-1600'/> <span class='hint'>bytes</span></td></tr>" \
    "<tr><td colspan='2'><input type='submit' value='Save' class='ok-button'/>" \
    "<p class='hint-block'>Connect using <code>nc %s 19000 | wireshark -k -i -</code></p></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>"

/* Antenna switch section - uses: ant_gpio, ant_en, sel_onboard, sel_external,
   status_color, status_text */
#define CONFIG_CHUNK_ANT \
    "<div class='card'>" \
    "<div class='card-head'><h2>Antenna switch</h2><span class='meta'>RF</span></div>" \
    "<form action='' method='GET'>" \
    "<input type='hidden' name='ant_save' value='1'/>" \
    "<table class='form-table'>" \
    "<tr><td>Control GPIO</td><td><input type='number' name='ant_gpio' value='%d' min='-1' max='63'/> <span class='hint'>-1 = disabled</span></td></tr>" \
    "<tr><td>Enable GPIO</td><td><input type='number' name='ant_en' value='%d' min='-1' max='63'/> <span class='hint'>optional, held low (-1 = none)</span></td></tr>" \
    "<tr><td>Antenna</td><td><select name='ant_sel'>" \
    "<option value='0' %s>On-board</option>" \
    "<option value='1' %s>External</option>" \
    "</select></td></tr>" \
    "<tr><td>Status</td><td><strong class='%s'>%s</strong></td></tr>" \
    "<tr><td colspan='2'><input type='submit' value='Save' class='ok-button'/>" \
    "<p class='hint-block'>" ANTENNA_BOARD_HINT " Applied immediately, no reboot needed.</p></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>"

/* Device management and footer */
#define CONFIG_CHUNK_TAIL \
    "<div class='card'>" \
    "<div class='card-head'><h2>Device management</h2><span class='meta'>device</span></div>" \
    "<div class='card-body' id='mainContent'>" \
    "<h3>Firmware update (OTA)</h3>"

/* OTA info (running partition, version) is streamed dynamically here */
#define CONFIG_CHUNK_TAIL2 \
    "<table class='form-table'>" \
    "<tr><td>Upload</td><td>" \
    "<label class='file-btn'>" \
    "<input type='file' id='otaFile' accept='.bin' style='display:none;'/>" \
    "<span id='otaFileName'>Choose .bin file...</span>" \
    "</label>" \
    "<button type='button' onclick='uploadOTA()' class='ok-button'>Upload firmware</button>" \
    "<div id='otaProgress'>" \
    "<div id='otaBar' class='progress'><div id='otaBarFill' class='progress-fill'></div></div>" \
    "</div>" \
    "<div id='otaStatus' class='hint'></div>" \
    "</td></tr>" \
    "</table>" \
    "<h3>Config backup / restore</h3>" \
    "<table class='form-table'>" \
    "<tr><td>Export</td><td>" \
    "<input type='password' id='expPass' placeholder='Passphrase (leave empty for plain)'/>" \
    "<button type='button' onclick='downloadConfig()' class='ok-button'>Write config</button>" \
    "<div id='exportStatus' class='hint'></div>" \
    "</td></tr>" \
    "<tr><td>Import</td><td>" \
    "<input type='password' id='impPass' placeholder='Passphrase (if encrypted)'/>" \
    "<label class='file-btn'>" \
    "<input type='file' id='cfgFile' accept='.json' style='display:none;'/>" \
    "<span id='cfgFileName'>Choose file...</span>" \
    "</label>" \
    "<button type='button' onclick='uploadConfig()' class='ok-button'>Read config</button>" \
    "<div id='importStatus' class='hint'></div>" \
    "</td></tr>" \
    "</table>" \
    "<h3>Reboot</h3>" \
    "<form action='' method='GET'>" \
    "<table class='form-table'>" \
    "<tr><td>Device</td><td><input type='submit' name='reset' value='Reboot now' class='red-button'/></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>" \
    "</div>" \
    "<div id='rebootScreen' class='card reboot-screen'>" \
    "<h2 id='rebootTitle'>Success</h2>" \
    "<p id='rebootMsg'>The device is rebooting...</p>" \
    "<p class='countdown' id='countdown'></p>" \
    "<p class='hint'>Redirecting to the home page...</p>" \
    "</div>" \
    "<script>" \
    "document.getElementById('otaFile').addEventListener('change',function(){document.getElementById('otaFileName').textContent=this.files[0]?this.files[0].name:t('Choose .bin file...');});" \
    "function uploadOTA(){" \
    "var f=document.getElementById('otaFile').files[0];" \
    "if(!f){document.getElementById('otaStatus').textContent=t('Select a firmware file first.');return;}" \
    "if(!f.name.endsWith('.bin')){document.getElementById('otaStatus').innerHTML='<span class=\"bad-text\">'+t('Please select a .bin file')+'</span>';return;}" \
    "document.getElementById('otaStatus').textContent=t('Uploading...');" \
    "document.getElementById('otaBar').style.display='block';" \
    "var xhr=new XMLHttpRequest();" \
    "xhr.open('POST','/api/ota-upload',true);" \
    "xhr.upload.onprogress=function(e){if(e.lengthComputable){var pct=Math.round(e.loaded/e.total*100);document.getElementById('otaBarFill').style.width=pct+'%';document.getElementById('otaStatus').textContent=t('Uploading...')+' '+pct+'%';}};" \
    "xhr.onload=function(){" \
    "try{var d=JSON.parse(xhr.responseText);" \
    "if(d.ok){" \
    "document.getElementById('mainContent').style.display='none';" \
    "document.getElementById('rebootScreen').style.display='block';" \
    "document.getElementById('rebootTitle').textContent=t('Firmware updated');" \
    "document.getElementById('rebootMsg').textContent=t('The device is rebooting with new firmware.');" \
    "var c=10;var el=document.getElementById('countdown');" \
    "var timer=setInterval(function(){c--;el.textContent=c;if(c<=0){clearInterval(timer);window.location.href='/';}},1000);" \
    "}else{" \
    "document.getElementById('otaBarFill').style.width='0';document.getElementById('otaBar').style.display='none';" \
    "document.getElementById('otaStatus').innerHTML='<span class=\"bad-text\">'+t(d.msg)+'</span>';" \
    "}}catch(e){document.getElementById('otaStatus').innerHTML='<span class=\"bad-text\">'+t('Upload failed')+'</span>';}" \
    "};" \
    "xhr.onerror=function(){document.getElementById('otaStatus').innerHTML='<span class=\"bad-text\">'+t('Connection error')+'</span>';};" \
    "xhr.send(f);" \
    "}" \
    "document.getElementById('cfgFile').addEventListener('change',function(){document.getElementById('cfgFileName').textContent=this.files[0]?this.files[0].name:t('Choose file...');});" \
    "function downloadConfig(){" \
    "var pass=document.getElementById('expPass').value;" \
    "document.getElementById('exportStatus').textContent=t('Downloading...');" \
    "fetch('/api/config-export',{method:'POST',body:JSON.stringify({pass:pass}),headers:{'Content-Type':'application/json'}})" \
    ".then(function(res){" \
    "if(!res.ok){return res.text().then(function(t){throw new Error(t);});}" \
    "var cd=res.headers.get('Content-Disposition')||'';" \
    "var fn='esp32_nat_config.json';" \
    "var m=cd.match(/filename=\"([^\"]+)\"/);" \
    "if(m)fn=m[1];" \
    "return res.blob().then(function(b){return{blob:b,fn:fn};});" \
    "})" \
    ".then(function(obj){" \
    "var url=URL.createObjectURL(obj.blob);" \
    "var a=document.createElement('a');" \
    "a.href=url;a.download=obj.fn;" \
    "document.body.appendChild(a);a.click();" \
    "setTimeout(function(){URL.revokeObjectURL(url);a.remove();},100);" \
    "document.getElementById('exportStatus').textContent=pass?t('Downloaded (encrypted).'):t('Downloaded (plain).');" \
    "})" \
    ".catch(function(){document.getElementById('exportStatus').innerHTML='<span class=\"bad-text\">'+t('Export failed')+'</span>';});" \
    "}" \
    "function uploadConfig(){" \
    "var f=document.getElementById('cfgFile').files[0];" \
    "var pass=document.getElementById('impPass').value;" \
    "if(!f){document.getElementById('importStatus').textContent=t('Select a file first.');return;}" \
    "var r=new FileReader();" \
    "r.onload=function(){" \
    "document.getElementById('importStatus').textContent=t('Uploading...');" \
    "var hdrs={'Content-Type':'application/json'};" \
    "if(pass)hdrs['X-Config-Pass']=pass;" \
    "fetch('/api/config-import',{method:'POST',body:r.result,headers:hdrs})" \
    ".then(function(res){return res.json();})" \
    ".then(function(d){" \
    "if(d.ok){" \
    "document.getElementById('mainContent').style.display='none';" \
    "document.getElementById('rebootScreen').style.display='block';" \
    "document.getElementById('rebootTitle').textContent=t('Config imported');" \
    "document.getElementById('rebootMsg').textContent=t('The device is rebooting to apply the new configuration.');" \
    "var c=5;var el=document.getElementById('countdown');" \
    "var timer=setInterval(function(){c--;el.textContent=c;if(c<=0){clearInterval(timer);window.location.href='/';}},1000);" \
    "}else{" \
    "document.getElementById('importStatus').innerHTML='<span class=\"bad-text\">'+t(d.msg)+'</span>';" \
    "}" \
    "});" \
    "};" \
    "r.readAsText(f);" \
    "}" \
    "</script>"

/* Danger Zone section - uses %s for: web_bind_ap_chk, web_bind_sta_chk, web_bind_vpn_chk.
 * ETH_UPLINK variant labels the uplink checkbox "ETH" instead of "STA". */
#define CONFIG_CHUNK_DANGER_COMMON_HEAD \
    "<div class='card card-danger'>" \
    "<div class='card-head'><h2>Danger zone</h2><span class='meta'>lockout risk</span></div>" \
    "<div class='danger-body'>" \
    "<h3>Web UI access</h3>" \
    "<p>Restrict which network interface can reach the web UI. Unchecking the interface you are currently using will lock you out (re-enable via console: <code>web_ui bind all</code>).</p>" \
    "<form action='' method='GET'>" \
    "<input type='hidden' name='web_bind_save' value='1'/>" \
    "<table class='form-table'>" \
    "<tr><td>Interfaces</td><td>" \
    "<label><input type='checkbox' name='web_bind_ap' value='1' %s> <span class='hint'>AP</span></label>"

#define CONFIG_CHUNK_DANGER_COMMON_TAIL \
    "<label><input type='checkbox' name='web_bind_vpn' value='1' %s> <span class='hint'>VPN</span></label>" \
    "</td></tr>" \
    "<tr><td colspan='2'><input type='submit' value='Save access' class='red-button' onclick='return confirm(t(\"Restricting access may lock you out. Continue?\"));'/></td></tr>" \
    "</table>" \
    "</form>" \
    "<h3>Disable web interface</h3>" \
    "<p>Disables the web interface completely. Re-enable via console: <code>web_ui enable</code>.</p>" \
    "<form action='' method='GET'>" \
    "<table class='form-table'>" \
    "<tr><td>Disable</td>" \
    "<td><input type='submit' name='disable_interface' value='Disable web UI' class='red-button' onclick='return confirm(t(\"Are you sure? The web interface will be disabled and can only be re-enabled via the console.\"));'/></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>" \
    "</div>" \
    THEME_HOME_ROW \
    THEME_FOOTER \
    THEME_PAGE_END

#if CONFIG_ETH_UPLINK
/* ETH_UPLINK: uplink checkbox labeled "ETH" (maps to RC_BIND_STA / web_bind_sta) */
#define CONFIG_CHUNK_DANGER \
    CONFIG_CHUNK_DANGER_COMMON_HEAD \
    "<label><input type='checkbox' name='web_bind_sta' value='1' %s> <span class='hint'>ETH</span></label>" \
    CONFIG_CHUNK_DANGER_COMMON_TAIL
#else
/* STA variant */
#define CONFIG_CHUNK_DANGER \
    CONFIG_CHUNK_DANGER_COMMON_HEAD \
    "<label><input type='checkbox' name='web_bind_sta' value='1' %s> <span class='hint'>STA</span></label>" \
    CONFIG_CHUNK_DANGER_COMMON_TAIL
#endif
