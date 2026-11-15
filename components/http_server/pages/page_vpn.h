/* VPN page templates - chunked for streaming. Styles in /style.css. */
#define VPN_CHUNK_HEAD \
    THEME_DOC_OPEN("WireGuard VPN") \
    THEME_TOPBAR_OPEN

/* After the logout slot: page intro + reboot-after-save script. */
#define VPN_CHUNK_MID \
    THEME_TOPBAR_CLOSE \
    "<div class='page-head'>" \
    "<p class='eyebrow'>Tunnel</p>" \
    "<h1>WireGuard VPN</h1>" \
    "<p class='page-sub'>Peer, keys, routing and kill switch.</p>" \
    "</div>" \
    "<script>" \
    "var qs = window.location.search.substr(1);" \
    "if (qs.indexOf('vpn_enabled=') !== -1 || qs.indexOf('vpn_privkey=') !== -1) {" \
    "document.body.innerHTML = '<main id=\"container\"><div class=\"page-head\"><p class=\"eyebrow\">Saved</p><h1>Rebooting</h1><p class=\"page-sub\">Settings saved. The device returns in about 10 seconds.</p></div></main>';" \
    "setTimeout(\"location.href = '/'\", 10000);" \
    "}" \
    "</script>"

/* Import section - paste a standard WireGuard .conf. */
#define VPN_CHUNK_IMPORT \
    "<div class='card'>" \
    "<div class='card-head'><h2>Import config</h2><span class='meta'>.conf</span></div>" \
    "<div class='card-body'>" \
    "<p class='hint' style='margin-bottom:10px;'>Paste a WireGuard .conf. The fields are filled and the device reboots.</p>" \
    "<textarea id='wgconf' rows='9' placeholder='[Interface]&#10;PrivateKey = ...&#10;Address = 10.2.0.2/32&#10;DNS = 10.2.0.1&#10;&#10;[Peer]&#10;PublicKey = ...&#10;Endpoint = host:51820&#10;AllowedIPs = 0.0.0.0/0'></textarea>" \
    "<button type='button' class='ok-button' style='width:100%;margin-top:10px;' onclick='importWg()'>Import &amp; reboot</button>" \
    "<div id='wgmsg' class='hint' style='margin-top:6px;'></div>" \
    "</div>" \
    "<script>" \
    "function importWg(){" \
    "var conf=document.getElementById('wgconf').value;" \
    "var m=document.getElementById('wgmsg');" \
    "if(!conf.trim()){m.className='hint bad-text';m.textContent=t('Paste a config first.');return;}" \
    "m.className='hint';m.textContent=t('Importing...');" \
    "fetch('/api/vpn-import',{method:'POST',body:conf}).then(function(r){return r.json();}).then(function(d){" \
    "if(d.ok){m.className='hint ok-text';m.textContent=t(d.msg);setTimeout(function(){location.href='/';},10000);}" \
    "else{m.className='hint bad-text';m.textContent=t(d.msg||'Import failed');}" \
    "}).catch(function(){m.className='hint bad-text';m.textContent=t('Request failed');});" \
    "}" \
    "</script>" \
    "</div>"

/* VPN form - static wrapper (no format strings) */
#define VPN_CHUNK_FORM_OPEN \
    "<div class='card'>" \
    "<div class='card-head'><h2>Configuration</h2><span class='meta'>peer</span></div>" \
    "<form action='/vpn' method='GET'>" \
    "<table class='form-table'>"

#define VPN_CHUNK_FORM_CLOSE \
    "<tr><td colspan='2'><input type='submit' value='Save &amp; reboot' class='ok-button'/></td></tr>" \
    "</table>" \
    "</form>" \
    "</div>"

/* Page footer - emitted after the import section */
#define VPN_CHUNK_PAGE_END \
    THEME_HOME_ROW \
    THEME_FOOTER \
    THEME_PAGE_END
