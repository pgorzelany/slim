#!/usr/bin/env python3
"""Bounded RFC-0152 allocator model; never a production allocator or SLIM checker."""

import argparse
import json
from dataclasses import dataclass


MAX_DEPTH = 6
MAX_STATES = 20_000
MAX_TRANSITIONS = 250_000
QUANTA = 8
REQUESTS = (0, 1, 2, 3, 4, 5, 8, 9)


@dataclass(frozen=True)
class State:
    free: tuple  # (order, offset), sorted; quantum size is abstract here
    live: tuple  # (offset, order, requested quanta), sorted


INITIAL = State(((3, 0),), ())


def allocate(state, size, fail=False):
    if size == 0:
        return ("empty", None), state
    if size < 0 or size > QUANTA or fail:
        return ("exhausted", None), state
    order = 0
    while (1 << order) < size:
        order += 1
    available = [block for block in state.free if block[0] >= order]
    if not available:
        return ("exhausted", None), state
    current, offset = min(available)
    free = set(state.free)
    free.remove((current, offset))
    while current > order:
        current -= 1
        free.add((current, offset + (1 << current)))
    live = (*state.live, (offset, order, size))
    return ("allocated", offset), State(tuple(sorted(free)), tuple(sorted(live)))


def release(state, offset):
    allocation = next((item for item in state.live if item[0] == offset), None)
    if allocation is None:
        return ("invalid-release", None), state
    _, order, _ = allocation
    free = set(state.free)
    while order < 3:
        sibling = offset ^ (1 << order)
        if (order, sibling) not in free:
            break
        free.remove((order, sibling))
        offset = min(offset, sibling)
        order += 1
    free.add((order, offset))
    live = tuple(item for item in state.live if item != allocation)
    return ("released", None), State(tuple(sorted(free)), live)


def oracle_free(live):
    # Independent occupied-cell representation; reconstruct all maximal empty
    # aligned intervals. This does not execute the split/coalescing algorithm.
    occupied = set()
    for offset, order, _ in live:
        cells = set(range(offset, offset + 2**order))
        assert not occupied & cells
        occupied |= cells
    free = []
    for width in (1, 2, 4, 8):
        for offset in range(0, QUANTA, width):
            cells = set(range(offset, offset + width))
            if cells & occupied:
                continue
            parent = offset // (width * 2) * width * 2
            parent_cells = set(range(parent, parent + width * 2))
            if width == QUANTA or parent_cells & occupied:
                free.append((width.bit_length() - 1, offset))
    return tuple(sorted(free))


def oracle(state, action):
    kind, value, fail = action
    live = list(state.live)
    if kind == "release":
        found = [entry for entry in live if entry[0] == value]
        if not found:
            return ("invalid-release", None), state
        live.remove(found[0])
        result = ("released", None)
    else:
        if value == 0:
            return ("empty", None), state
        if value < 0 or value > QUANTA or fail:
            return ("exhausted", None), state
        widths = [(2**order, offset) for order, offset in oracle_free(live)
                  if 2**order >= value]
        if not widths:
            return ("exhausted", None), state
        _, offset = min(widths)
        reserved = next(width for width in (1, 2, 4, 8) if width >= value)
        live.append((offset, reserved.bit_length() - 1, value))
        result = ("allocated", offset)
    live = tuple(sorted(live))
    return result, State(oracle_free(live), live)


def validate(state):
    assert state.free == oracle_free(state.live), state
    accounted = sum(2**order for order, _ in state.free)
    for offset, order, request in state.live:
        width = 2**order
        assert 0 <= offset <= QUANTA - width and offset % width == 0
        assert 0 < request <= width < 2 * request
        accounted += width
    assert accounted == QUANTA


def class_bytes(payload, header, capacity):
    # Candidate admission arithmetic, bounded before addition/doubling. The
    # integer oracle below is mathematical; this is not a C overflow proof.
    if payload < 0 or header < 0 or capacity < 64 or capacity > 2**30:
        return None
    if capacity & (capacity - 1):
        return None
    if payload == 0:
        return 0
    if header > capacity or payload > capacity - header:
        return None
    required = payload + header
    size = 64
    while size < required:
        size *= 2
    return size


class FreeIndex:
    """Hierarchical 64-bit occupancy words, with at most four levels."""

    def __init__(self, bits):
        if not 1 <= bits <= 2**24:
            raise ValueError("free-index extent outside fixed bound")
        self.bits = bits
        self.levels = []
        count = bits
        while True:
            count = (count + 63) // 64
            self.levels.append([0] * count)
            if count == 1:
                break
        assert len(self.levels) <= 4

    def update(self, index, present):
        if not 0 <= index < self.bits:
            raise ValueError("free-index offset outside extent")
        visits = 0
        for words in self.levels:
            word_index, bit = divmod(index, 64)
            prior = words[word_index]
            mask = 1 << bit
            current = prior | mask if present else prior & ~mask
            words[word_index] = current
            visits += 1
            if bool(prior) == bool(current):
                break
            present = bool(current)
            index = word_index
        assert visits <= 4
        return visits

    def minimum(self):
        if self.levels[-1][0] == 0:
            return None, 1
        index = 0
        visits = 0
        for words in reversed(self.levels):
            word = words[index]
            assert word != 0
            bit = (word & -word).bit_length() - 1
            index = index * 64 + bit
            visits += 1
        assert index < self.bits and visits <= 4
        return index, visits


def check_free_index():
    operations = 0
    maximum_visits = 0
    for bits in (1, 2, 63, 64, 65, 4095, 4096, 4097, 262144, 262145, 2**24):
        index = FreeIndex(bits)
        expected = set()
        positions = sorted({p for p in (0, 1, 62, 63, 64, 65, 4095, 4096,
                                        262143, 262144, bits // 2, bits - 1) if p < bits})
        actions = [(p, True) for p in reversed(positions)]
        actions += [(p, False) for p in positions[::2]]
        actions += [(p, True) for p in positions]
        actions += [(p, False) for p in reversed(positions)]
        for position, present in actions:
            visits = index.update(position, present)
            if present:
                expected.add(position)
            else:
                expected.discard(position)
            actual, reads = index.minimum()
            assert actual == (min(expected) if expected else None)
            maximum_visits = max(maximum_visits, visits, reads)
            operations += 1
        assert not expected and index.minimum()[0] is None
        for invalid in (-1, bits):
            try:
                index.update(invalid, True)
            except ValueError:
                pass
            else:
                raise AssertionError("invalid index accepted")
    words = sum(sum(len(level) for level in FreeIndex(2**order).levels) for order in range(25))
    return {"operations": operations, "maximum_word_visits": maximum_visits,
            "maximum_pool_index_bytes": words * 8 + 8}


def check_arithmetic():
    count = 0
    for capacity in (64, 128, 1024, 2**20, 2**30):
        for header in (0, 16, 64, capacity, capacity + 1):
            for payload in (-1, 0, 1, 63, 64, 65, capacity - header - 1,
                            capacity - header, capacity - header + 1,
                            2**63 - 1, 2**64 - 1):
                expected = None
                if payload == 0:
                    expected = 0
                elif payload > 0 and payload + header <= capacity:
                    expected = max(64, 2 ** ((payload + header - 1).bit_length()))
                assert class_bytes(payload, header, capacity) == expected
                count += 1
    for invalid in (-1, 0, 63, 65, 2**30 + 1, 2**63 - 1):
        assert class_bytes(1, 0, invalid) is None
        count += 1
    return count


def explore(depth, state_limit, transition_limit):
    seen = {INITIAL}
    frontier = [INITIAL]
    transitions = 0
    outcomes = {}
    for _ in range(depth):
        following = []
        for state in frontier:
            validate(state)
            actions = [("allocate", size, fail) for size in REQUESTS for fail in (False, True)]
            actions += [("release", item[0], False) for item in state.live]
            actions += [("release", -1, False)]
            for action in actions:
                if transitions >= transition_limit:
                    raise ValueError("transition budget exhausted; result unknown")
                transitions += 1
                kind, value, fail = action
                result = allocate(state, value, fail) if kind == "allocate" else release(state, value)
                assert result == oracle(state, action), (state, action, result)
                outcome, next_state = result
                validate(next_state)
                outcomes[outcome[0]] = outcomes.get(outcome[0], 0) + 1
                # Release every live allocation in reverse address order: no
                # leaks remain even after failed allocation/reuse histories.
                cleared = next_state
                for offset, _, _ in reversed(next_state.live):
                    _, cleared = release(cleared, offset)
                assert cleared == INITIAL
                if next_state not in seen:
                    if len(seen) >= state_limit:
                        raise ValueError("state budget exhausted; result unknown")
                    seen.add(next_state)
                    following.append(next_state)
        frontier = following
    return {"states": len(seen), "transitions": transitions, "outcomes": outcomes}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", required=True)
    parser.add_argument("--depth", type=int, default=4, choices=range(1, MAX_DEPTH + 1))
    parser.add_argument("--state-limit", type=int, default=MAX_STATES)
    parser.add_argument("--transition-limit", type=int, default=MAX_TRANSITIONS)
    args = parser.parse_args()
    if not __debug__:
        parser.error("model assertions must remain enabled")
    if not 1 <= args.state_limit <= MAX_STATES or not 1 <= args.transition_limit <= MAX_TRANSITIONS:
        parser.error("budgets must be positive and no greater than fixed model limits")
    try:
        result = explore(args.depth, args.state_limit, args.transition_limit)
        result["arithmetic_cases"] = check_arithmetic()
        result["free_index"] = check_free_index()
    except ValueError as error:
        parser.exit(1, f"m2 pool model: {error}\n")
    result.update(classification="bounded", scope="8-quantum single-domain allocator model",
                  depth=args.depth, state_limit=args.state_limit, transition_limit=args.transition_limit,
                  production_allocator="unknown; not implemented or verified by this model")
    print(json.dumps(result, sort_keys=True))


if __name__ == "__main__":
    main()
