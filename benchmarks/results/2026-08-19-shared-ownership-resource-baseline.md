# Shared-ownership resource baseline reconciliation

Date: 2026-08-19

Command: `cargo run --release --bin slim-bench -- resources`

Decision: accepted RFC-0110

The RFC-0110 migration made every plain affine parameter a shared borrow. The
resource analyzer correctly classifies those bindings as `shared`, but the
durable application baseline still counted them as owned. The same migration
also shortened explicit ownership-mode syntax in fifteen challenge sources;
the baseline still contained their pre-migration byte sizes.

Every changed row is recorded below. `source_bytes`, `owned_bindings`, and
`max_live_owned` are the only changed columns. Recurrence profiles, call-work
facts, expression nodes, allocation sites, trap sites, effect classifications,
and totality classifications are byte-for-byte unchanged.

| Challenge | Source bytes | Owned bindings | Max live owned | Reason |
| --- | ---: | ---: | ---: | --- |
| `gcd_fib` | 443 → 443 | 1 → 0 | 1 → 0 | `main` argument became shared |
| `sieve` | 1293 → 1287 | 2 → 1 | 1 → 1 | shared affine parameters and shorter mode syntax |
| `bfs` | 735 → 731 | 2 → 1 | 1 → 1 | shared affine parameters and shorter mode syntax |
| `matrix` | 1380 → 1367 | 3 → 2 | 2 → 2 | shared affine parameters and shorter mode syntax |
| `merge_sort` | 2857 → 2824 | 3 → 2 | 2 → 2 | shared affine parameters and shorter mode syntax |
| `bytefreq` | 1240 → 1235 | 3 → 2 | 2 → 1 | shared affine parameter removed the peak overlap |
| `binary_search` | 1171 → 1164 | 2 → 1 | 1 → 1 | shared affine parameters and shorter mode syntax |
| `prefix_sum` | 706 → 702 | 2 → 1 | 1 → 1 | shared affine parameters and shorter mode syntax |
| `records` | 601 → 601 | 1 → 0 | 1 → 0 | `main` argument became shared |
| `variants` | 785 → 785 | 1 → 0 | 1 → 0 | `main` argument became shared |
| `state_machine` | 1172 → 1172 | 1 → 0 | 1 → 0 | `main` argument became shared |
| `signal_network` | 1806 → 1806 | 1 → 0 | 1 → 0 | `main` argument became shared |
| `arena_sum` | 827 → 817 | 3 → 2 | 2 → 2 | shared affine parameters and shorter mode syntax |
| `knapsack` | 1256 → 1249 | 2 → 1 | 1 → 1 | shared affine parameters and shorter mode syntax |
| `floyd_warshall` | 2052 → 2042 | 3 → 1 | 1 → 1 | two affine parameters became shared |
| `n_queens` | 1921 → 1918 | 2 → 1 | 1 → 1 | shared affine parameters and shorter mode syntax |
| `union_find` | 1699 → 1684 | 2 → 1 | 1 → 1 | shared affine parameters and shorter mode syntax |
| `game_of_life` | 2915 → 2890 | 3 → 2 | 2 → 2 | shared affine parameters and shorter mode syntax |
| `image_convolution` | 2496 → 2473 | 3 → 2 | 2 → 2 | shared affine parameters and shorter mode syntax |
| `edit_distance` | 3330 → 3303 | 3 → 2 | 3 → 2 | shared affine parameter removed the peak overlap |

This is a correction to the exact evidence ledger, not a relaxed performance
budget or a new precision claim.
