"""Fixed typed-input SLIM probe constructor; no expected poststate calculation."""

MIN = -9223372036854775808
MAX = 9223372036854775807

def require(value, message):
    if not value:
        raise ValueError(message)

def integer(value):
    require(type(value) is int and MIN <= value <= MAX, 'fixed I64 input')
    return str(value)

def account(fields):
    require(type(fields) is list and len(fields) == 6 and type(fields[3]) is bool, 'fixed account input')
    values = [integer(fields[n]) for n in (0, 1, 2, 4, 5)]
    return ('ledger_model.Account(name_start: ' + values[0] + ', name_end: ' + values[1] +
            ', balance: ' + values[2] + ', open: ' + ('true' if fields[3] else 'false') +
            ', credits: ' + values[3] + ', debits: ' + values[4] + ')')

def command(value):
    kind = value['kind']
    require(kind in ('Open', 'Credit', 'Debit', 'Transfer', 'Close'), 'fixed command input')
    first = value['first']
    require(type(first) is list and len(first) == 2, 'fixed first span')
    args = [integer(n) for n in first]
    if kind == 'Transfer':
        second = value['second']
        require(type(second) is list and len(second) == 2, 'fixed second span')
        args.extend(integer(n) for n in second)
    if kind != 'Close':
        args.append(integer(value['amount']))
    return 'ledger_model.Command::' + kind + '(' + ', '.join(args) + ')'

def construct(prefix, states):
    require(type(prefix) is bytes and prefix.startswith(b'module ledger_probe\n'), 'fixed probe prefix')
    require(len(states) == 171, 'fixed typed state count')
    parts = [prefix]
    for index, case in enumerate(states):
        label = case['label']
        require(type(label) is str and label and label.isascii() and label.isalnum(), 'fixed printable label')
        require(len(case['accounts']) == 3, 'fixed complete three-account input')
        lines = ['\nfn case_' + str(index).zfill(4) + '() -> Void effects[alloc, io, partial]:',
                 '  let accounts: Vec[ledger_model.Account] = vec.new()']
        for fields in case['accounts']:
            lines.append('  vec.push(@accounts, ' + account(fields) + ')')
        lines.extend(('  let command: ledger_model.Command = ' + command(case['command']),
                      '  let result: ledger_model.Applied = ledger_state.apply("alice bob carol missing", command, @accounts)',
                      '  print_case("' + label + '", result, accounts)', ''))
        parts.append('\n'.join(lines).encode('ascii'))
    lines = ['\nfn states() -> I64 effects[alloc, io, partial]:']
    lines.extend('  case_' + str(index).zfill(4) + '()' for index in range(171))
    lines.extend(('  0', '', 'fn main(args: Vec[Bytes]) -> I64 effects[alloc, io, partial]:',
                  '  if vec.len(args) == 2:',
                  '    if std_bytes.equal(vec.get(args, 1), "states"):',
                  '      states()', '    else:',
                  '      if std_bytes.equal(vec.get(args, 1), "partial-total"):',
                  '        partial_total()', '      else:', '        64', '  else:', '    64', ''))
    parts.append('\n'.join(lines).encode('ascii'))
    data = b''.join(parts)
    require(len(data) <= 1048576, 'fixed probe source cap')
    return data
