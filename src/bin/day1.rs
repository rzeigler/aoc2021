use std::env;
use std::fs;

pub fn read_input() -> Vec<i32> {
    let input_path = env::args().skip(1).next().expect("input argument required");

    let content = fs::read_to_string(input_path).unwrap();
    let mut values = Vec::<i32>::new();
    for line in content.lines() {
        if !line.is_empty() {
            values.push(line.parse::<i32>().expect("invalid file content"));
        }
    }

    values
}

fn main() {
    let input = read_input();
    {
        let count = input
            .windows(2)
            .filter(|window| window[0] < window[1])
            .count();
        println!("{}", count);
    }
    {
        let windows = input
            .windows(3)
            .map(|window| window.iter().sum::<i32>())
            .collect::<Vec<_>>();

        let count = windows
            .windows(2)
            .filter(|window| window[0] < window[1])
            .count();
        println!("{}", count);
    }
}
