## GitHub Copilot Chat

- Extension Version: 0.26.7 (prod)
- VS Code: vscode/1.99.3
- OS: Linux
- Remote Name: ssh-remote

## Network

User Settings:
```json
  "github.copilot.advanced.debug.useElectronFetcher": true,
  "github.copilot.advanced.debug.useNodeFetcher": false,
  "github.copilot.advanced.debug.useNodeFetchFetcher": true
```

Connecting to https://api.github.com:
- DNS ipv4 Lookup: 140.82.116.5 (39 ms)
- DNS ipv6 Lookup: Error (4 ms): getaddrinfo ENOTFOUND api.github.com
- Proxy URL: None (4 ms)
- Electron fetch: Unavailable
- Node.js https: HTTP 200 (184 ms)
- Node.js fetch (configured): HTTP 200 (361 ms)
- Helix fetch: HTTP 200 (1126 ms)

Connecting to https://api.individual.githubcopilot.com/_ping:
- DNS ipv4 Lookup: 140.82.112.22 (41 ms)
- DNS ipv6 Lookup: Error (4 ms): getaddrinfo ENOTFOUND api.individual.githubcopilot.com
- Proxy URL: None (1 ms)
- Electron fetch: Unavailable
- Node.js https: Error (303 ms): Error: certificate is not yet valid
    at TLSSocket.onConnectSecure (node:_tls_wrap:1677:34)
    at TLSSocket.emit (node:events:518:28)
    at TLSSocket._finishInit (node:_tls_wrap:1076:8)
    at TLSWrap.ssl.onhandshakedone (node:_tls_wrap:862:12)
- Node.js fetch (configured): Error (454 ms): TypeError: fetch failed
    at node:internal/deps/undici/undici:13502:13
    at processTicksAndRejections (node:internal/process/task_queues:95:5)
    at hM._fetch (/home/jose/.vscode-server/extensions/github.copilot-chat-0.26.7/dist/extension.js:784:25942)
    at /home/jose/.vscode-server/extensions/github.copilot-chat-0.26.7/dist/extension.js:815:134
    at vw.h (file:///home/jose/.vscode-server/cli/servers/Stable-17baf841131aa23349f217ca7c570c76ee87b957/server/out/vs/workbench/api/node/extensionHostProcess.js:111:41495)
  Error: certificate is not yet valid
      at TLSSocket.onConnectSecure (node:_tls_wrap:1677:34)
      at TLSSocket.emit (node:events:518:28)
      at TLSSocket._finishInit (node:_tls_wrap:1076:8)
      at TLSWrap.ssl.onhandshakedone (node:_tls_wrap:862:12)
- Helix fetch: Error (384 ms): FetchError: certificate is not yet valid
    at nut (/home/jose/.vscode-server/extensions/github.copilot-chat-0.26.7/dist/extension.js:304:29579)
    at processTicksAndRejections (node:internal/process/task_queues:95:5)
    at $Sr (/home/jose/.vscode-server/extensions/github.copilot-chat-0.26.7/dist/extension.js:304:31605)
    at mS.fetch (/home/jose/.vscode-server/extensions/github.copilot-chat-0.26.7/dist/extension.js:785:2495)
    at /home/jose/.vscode-server/extensions/github.copilot-chat-0.26.7/dist/extension.js:815:134
    at vw.h (file:///home/jose/.vscode-server/cli/servers/Stable-17baf841131aa23349f217ca7c570c76ee87b957/server/out/vs/workbench/api/node/extensionHostProcess.js:111:41495)

## Documentation

In corporate networks: [Troubleshooting firewall settings for GitHub Copilot](https://docs.github.com/en/copilot/troubleshooting-github-copilot/troubleshooting-firewall-settings-for-github-copilot).