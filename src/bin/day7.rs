use std::io;
use std::io::BufRead;

fn read_input() -> Vec<i32> {
    let stdin = io::stdin();
    let line = stdin
        .lock()
        .lines()
        .next()
        .expect("expected a line of input")
        .unwrap();

    line.split(",")
        .map(|n| n.parse::<i32>())
        .collect::<Result<Vec<_>, _>>()
        .unwrap()
}

fn solve1(input: &Vec<i32>) -> i32 {
    let mut cost = i32::MAX;
    let low = input.iter().cloned().min().unwrap();
    let high = input.iter().cloned().max().unwrap();
    for reference in low..high + 1 {
        let mut local_cost = 0;
        for i in input.iter().cloned() {
            local_cost += (reference - i).abs();
        }
        cost = cost.min(local_cost);
    }
    cost
}

fn solve2(input: &Vec<i32>) -> i32 {
    let mut cost = i32::MAX;
    let low = input.iter().cloned().min().unwrap();
    let high = input.iter().cloned().max().unwrap();
    for reference in low..high + 1 {
        let candidate_cost = move_cost(&input, reference);
        cost = cost.min(candidate_cost);
    }
    cost
}

fn move_cost(input: &Vec<i32>, to: i32) -> i32 {
    let mut candidate_cost = 0;
    for i in input.iter().cloned() {
        let delta = (to - i).abs();
        let delta_cost = (0..delta + 1).sum::<i32>();
        candidate_cost += delta_cost;
    }
    candidate_cost
}

fn main() {
    let input = read_input();
    println!("{}", solve1(&input));
    println!("{}", solve2(&input));
}
