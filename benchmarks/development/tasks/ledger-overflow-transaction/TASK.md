Repair ledger credit/debit/transfer overflow preflight, preserving all current
reject-code precedence. After the existing existence/positive/open/funds checks,
reject44 if the prospective destination balance or incremented operation counter
would exceed I64 maximum. A rejected command must preserve the ENTIRE account
vector; a successful transfer updates both accounts once. Same-account/closed/
missing/insufficient cases keep codes33..43. Domain: valid accounts with balances,
credits and debits in0..I64MAX, valid name spans and unique names; positive amount
I64. No invalid metadata repair is required. Credit must guard balance and credits;
debit must guard debits; transfer must guard to.balance/to.credits/from.debits
BEFORE either vec.set. Keep parsing/process behavior and ownership/effect ceilings.
The initial transfer also loses an exclusive mutation marker. Aggregate total
outside I64 is outside this task; never call it on such a fixture. Context anchors:
`ledger_state.apply`, `ledger_state.process`.

Only the listed editable SLIM files may change. The project manifest is fixed.
Keep every original module and exported API accepted by the ordinary checker.
The entry file may be used for local callers, but its tests do not define acceptance.
Acceptance checks the complete submitted project and independent clients.
Public docs and all candidate source are available. The context condition also
offers the production context command for original qualified declarations;
baseline has the same source and ordinary check/build/run/interfaces commands.
Do not change syntax, runtime, compiler semantics, dependencies or effect rules.
