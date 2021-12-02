use anyhow::Result;

use aoc2021::day1::*;

fn main() -> Result<()> {
    let input = read_input()?;
    let windows = input
        .windows(3)
        .map(|window| window.iter().sum::<i32>())
        .collect::<Vec<_>>();

    let count = windows
        .windows(2)
        .filter(|window| window[0] < window[1])
        .count();
    println!("{}", count);
    Ok(())
}
