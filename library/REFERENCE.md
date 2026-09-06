# SLIM experimental library reference

Generated from canonical interface schema 3. Do not edit by hand.

## `std_ascii`

```slim-interface
(fn digit_value ((copy U8)) I64 (effects))
(fn hex_value ((copy U8)) I64 (effects))
(fn is_alpha ((copy U8)) Bool (effects))
(fn is_alphanumeric ((copy U8)) Bool (effects))
(fn is_digit ((copy U8)) Bool (effects))
(fn is_hex_digit ((copy U8)) Bool (effects))
(fn is_lower ((copy U8)) Bool (effects))
(fn is_space ((copy U8)) Bool (effects))
(fn is_upper ((copy U8)) Bool (effects))
```

## `std_bytes`

```slim-interface
(fn append ((copy Bytes) (exclusive (Vec U8))) Void (effects alloc partial))
(fn append_range ((copy Bytes) (copy I64) (copy I64) (exclusive (Vec U8))) Bool (effects alloc partial))
(fn count_byte ((copy Bytes) (copy U8) (copy I64) (copy I64)) I64 (effects partial))
(fn ends_with ((copy Bytes) (copy Bytes)) Bool (effects partial))
(fn equal ((copy Bytes) (copy Bytes)) Bool (effects partial))
(fn find_byte ((copy Bytes) (copy U8) (copy I64) (copy I64)) I64 (effects partial))
(fn range_equal ((copy Bytes) (copy I64) (copy Bytes) (copy I64) (copy I64)) Bool (effects partial))
(fn starts_with ((copy Bytes) (copy Bytes)) Bool (effects partial))
```

## `std_cursor`

```slim-interface
(variant CheckedCursor ((Invalid) (Valid std_cursor.Cursor)))
(record Cursor ((position I64) (end I64)))
(variant Read ((End) (Invalid) (Value U8 std_cursor.Cursor)))
(fn checked ((copy I64) (copy I64) (copy I64)) std_cursor.CheckedCursor (effects))
(fn done ((shared std_cursor.Cursor)) Bool (effects))
(fn read ((copy Bytes) (shared std_cursor.Cursor)) std_cursor.Read (effects partial))
(fn remaining ((shared std_cursor.Cursor)) I64 (effects partial))
(fn valid ((shared std_cursor.Cursor) (copy I64)) Bool (effects))
```

## `std_decimal`

```slim-interface
(variant Parsed ((Invalid I64) (Overflow I64) (Value I64 I64)))
(fn parse_i64 ((copy Bytes) (copy I64) (copy I64)) std_decimal.Parsed (effects partial))
(fn parse_u8 ((copy Bytes) (copy I64) (copy I64)) std_decimal.Parsed (effects partial))
```

## `std_i64`

```slim-interface
(variant Absolute ((Overflow) (Value I64)))
(fn absolute ((copy I64)) std_i64.Absolute (effects))
(fn clamp ((copy I64) (copy I64) (copy I64)) I64 (effects))
(fn maximum ((copy I64) (copy I64)) I64 (effects))
(fn minimum ((copy I64) (copy I64)) I64 (effects))
```

## `std_i64_vec`

```slim-interface
(fn contains ((shared (Vec I64)) (copy I64) (copy I64) (copy I64)) Bool (effects partial))
(fn filled ((copy I64) (copy I64) (copy I64) (exclusive (Vec I64))) Void (effects alloc partial))
(fn push_at ((exclusive (Vec I64)) (copy I64) (copy I64)) I64 (effects alloc partial))
(fn top ((shared (Vec I64)) (copy I64)) I64 (effects partial))
```

## `std_span`

```slim-interface
(variant CheckedSpan ((Invalid) (Valid std_span.Span)))
(record Span ((start I64) (end I64)))
(fn checked ((copy I64) (copy I64) (copy I64)) std_span.CheckedSpan (effects))
(fn contains ((shared std_span.Span) (copy I64)) Bool (effects))
(fn empty ((shared std_span.Span)) Bool (effects))
(fn length ((shared std_span.Span)) I64 (effects partial))
(fn valid ((shared std_span.Span) (copy I64)) Bool (effects))
```

## `std_test`

```slim-interface
(record Stats ((passed I64) (failed I64)))
(fn check ((copy Bytes) (copy Bool) (shared std_test.Stats)) std_test.Stats (effects io partial))
(fn equal_bytes ((copy Bytes) (copy Bytes) (copy Bytes) (shared std_test.Stats)) std_test.Stats (effects io partial))
(fn equal_i64 ((copy Bytes) (copy I64) (copy I64) (shared std_test.Stats)) std_test.Stats (effects io partial))
(fn finish ((shared std_test.Stats)) I64 (effects io))
(fn start () std_test.Stats (effects))
```

## `std_text`

```slim-interface
(fn append_byte ((exclusive (Vec U8)) (copy U8)) Void (effects alloc))
(fn append_bytes ((exclusive (Vec U8)) (copy Bytes)) Void (effects alloc partial))
(fn append_i64 ((exclusive (Vec U8)) (copy I64)) Void (effects alloc partial))
(fn append_span ((exclusive (Vec U8)) (copy Bytes) (copy I64) (copy I64)) Bool (effects alloc partial))
```

## `std_u8_vec`

```slim-interface
(fn count ((shared (Vec U8)) (copy I64) (copy U8) (copy I64) (copy I64)) I64 (effects partial))
(fn filled ((copy I64) (copy U8) (copy I64) (exclusive (Vec U8))) Void (effects alloc partial))
(fn push_at ((exclusive (Vec U8)) (copy I64) (copy U8)) I64 (effects alloc partial))
(fn sum ((shared (Vec U8)) (copy I64) (copy I64) (copy I64)) I64 (effects partial))
```
