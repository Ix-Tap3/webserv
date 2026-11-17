// Shared "raw response" console: runs a real fetch() and prints status, headers and body.

function escapeHtml(s) {
  return String(s).replace(/[&<>"]/g, function (c) {
    if (c === '&') return '&amp;';
    if (c === '<') return '&lt;';
    if (c === '>') return '&gt;';
    return '&quot;';
  });
}

function statusClass(code) {
  if (code < 300)
    return 'ok';
  if (code < 400)
    return 'warn';
  return 'err';
}

// out: the <pre> element to print into
// opts: { method, path, body, contentType }
async function runRequest(out, opts) {
  const method = opts.method || 'GET';
  const path = opts.path || '/';
  const init = { method: method, redirect: 'manual' };

  if (opts.body !== undefined && method === 'POST') {
    init.body = opts.body;
    if (opts.contentType)
      init.headers = { 'Content-Type': opts.contentType };
  }

  out.innerHTML = '<span class="dim">' + escapeHtml(method + ' ' + path) + ' ...</span>';
  const t0 = performance.now();
  try {
    const res = await fetch(path, init);
    const ms = (performance.now() - t0).toFixed(1);

    if (res.type === 'opaqueredirect') {
      out.innerHTML = '<span class="warn">3xx Redirect</span>  <span class="dim">' + ms + ' ms</span>\n\n'
        + '<span class="dim">The server answered with a redirection. Open the link in a tab to follow it.</span>';
      return;
    }

    const body = await res.text();
    let headers = '';
    res.headers.forEach(function (v, k) { headers += escapeHtml(k) + ': ' + escapeHtml(v) + '\n'; });

    let shown = body;
    if (body.length > 4000)
      shown = body.slice(0, 4000) + '\n... (' + body.length + ' bytes total)';

    out.innerHTML =
      '<span class="' + statusClass(res.status) + '">HTTP ' + res.status + ' ' + escapeHtml(res.statusText) + '</span>'
      + '  <span class="dim">' + ms + ' ms</span>\n'
      + '<span class="dim">' + headers + '</span>\n'
      + escapeHtml(shown);
  } catch (e) {
    out.innerHTML = '<span class="err">Request failed: ' + escapeHtml(e) + '</span>\n'
      + '<span class="dim">The server closed the connection or is unreachable.</span>';
  }
}
