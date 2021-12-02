use anyhow::Result;

use aoc2021::day1::*;

fn main() -> Result<()> {
    let input = read_input()?;
    let mut count: u32 = 0;
    for window in input.windows(2) {
        if window[0] < window[1] {
            count += 1;
        }
    }
    println!("{}", count);
    Ok(())
}
