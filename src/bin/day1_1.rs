use anyhow::Result;

use aoc2021::day1::*;

fn main() -> Result<()> {
    let input = read_input()?;
    let count = input
        .windows(2)
        .filter(|window| window[0] < window[1])
        .count();
    println!("{}", count);
    Ok(())
}
