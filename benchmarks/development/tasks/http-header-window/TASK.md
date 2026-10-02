Extend the actual HTTP/1.1 response parser with strict header field names
and a bounded complete header window. A name must be nonempty ASCII tchar:
letters/digits or !#$%&'*+-.^_`|~. Invalid/empty name reports code18 at its first
bad byte (empty: colon), before value/content-length/transfer checks. Existing
case-insensitive CL, duplicate CL16, forbidden TE15, missing colon12, missing CRLF11,
CL14 and body17 behavior remains. The header budget is8192 bytes beginning just
after the status CRLF, INCLUDING all line CRLFs and the final empty CRLF. Search
only this window. If a CRLF cannot finish within it while source has at least8192
header bytes, report13 at header_start+8192; short unterminated input remains11 at
current line start. Budget rejection precedes name/value errors outside the window.
Exactly8192 header bytes are admitted. Existing status/body boundaries and
non-header source handling remain unchanged. Context anchor: `http_parser.parse`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
