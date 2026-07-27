# Core 1I bounded TCP evidence

Date: 2026-07-23
Decision: RFC-0076
Host: Apple arm64, local loopback

The maintained `tcp_client.slim` application completed two numeric-address TCP
exchanges against a local ephemeral-port server. The server received exact
`PING` requests and returned `PONG`. The program verified the successful four
bytes, an invalid-address failure, a two-byte response-limit failure, and
unchanged output after both failures. No public network was used.

The unsupported-target build used `SLIM_DISABLE_NETWORK=1`; the same runtime
operation returned typed `false`, retained an empty output, and did not mark
allocation failure.

An unrelated generated Hello executable measured 52,792 bytes with the POSIX
network implementation and 52,008 bytes with that implementation disabled:
a 1.015 ratio and 784-byte difference on this host. The permanent same-host
budget is 1.03. Both binaries produced identical output. This is a binary-size
cost, not runtime work or generated program state.
