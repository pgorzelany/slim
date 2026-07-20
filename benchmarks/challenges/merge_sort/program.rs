fn merge_sort(values: &mut [i64], scratch: &mut [i64], left: usize, right: usize) {
    if right - left <= 1 {
        return;
    }
    let middle = left + (right - left) / 2;
    merge_sort(values, scratch, left, middle);
    merge_sort(values, scratch, middle, right);
    let mut left_index = left;
    let mut right_index = middle;
    let mut output = left;
    while left_index < middle && right_index < right {
        if values[left_index] <= values[right_index] {
            scratch[output] = values[left_index];
            left_index += 1;
        } else {
            scratch[output] = values[right_index];
            right_index += 1;
        }
        output += 1;
    }
    while left_index < middle {
        scratch[output] = values[left_index];
        left_index += 1;
        output += 1;
    }
    while right_index < right {
        scratch[output] = values[right_index];
        right_index += 1;
        output += 1;
    }
    values[left..right].copy_from_slice(&scratch[left..right]);
}

fn main() {
    let count = 20_000usize;
    let mut values: Vec<i64> = (0..count).map(|index| (count - index) as i64).collect();
    let mut scratch = vec![0i64; count];
    merge_sort(&mut values, &mut scratch, 0, count);
    let total: i64 = values
        .into_iter()
        .enumerate()
        .map(|(index, value)| value * (index as i64 + 1))
        .sum();
    println!("{total}");
}

