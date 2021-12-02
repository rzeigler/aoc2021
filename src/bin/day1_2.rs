use anyhow::Result;

use aoc2021::day1::*;

fn main() -> Result<()> {
    let input = read_input()?;
    let windows = input.windows(3).collect::<Vec<_>>();

    let mut count: u32 = 0;

    for pair in windows.windows(2) {
        let sum_0 = pair[0].iter().sum::<i32>();
        let sum_1 = pair[1].iter().sum::<i32>();
        if sum_0 < sum_1 {
            count += 1;
        }
    }
    println!("{}", count);
    Ok(())
}
