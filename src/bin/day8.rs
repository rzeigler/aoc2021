use std::io;
use std::io::BufRead;

struct Line {
    start: Vec<String>,
    end: Vec<String>,
}

fn read_input() -> Vec<Line> {
    let stdin = io::stdin();
    stdin
        .lock()
        .lines()
        .map(|line| read_line(line.unwrap().as_str()))
        .collect()
}

fn read_line(line: &str) -> Line {
    let parts = line.split("|").collect::<Vec<_>>();
    if parts.len() != 2 {
        panic!("invalid input line");
    }
    Line {
        start: read_segments(parts[0]),
        end: read_segments(parts[1]),
    }
}

fn read_segments(part: &str) -> Vec<String> {
    part.split_ascii_whitespace()
        .map(|s| s.to_string())
        .collect()
}

const SEARCH_LEN: [usize; 4] = [
    2, // 1
    4, // 4
    3, // 7
    7, // 8
];

/*
    acedgfb: 8
    cdfbe: 5
    gcdfa: 2
    fbcad: 3
    dab: 7
    cefabd: 9
    cdfgeb: 6
    eafb: 4
    cagedb: 0
    ab: 1
*/
const SIGNAL_ON: [&str; 9] = [
    "cagedb",  // 0
    "ab",      // 1
    "gcdfa",   // 2
    "fbcad",   // 3
    "eafb",    // 4
    "cdfbe",   // 5
    "cdfgeb",  // 6
    "dab",     // 7
    "acedgfb", // 8
];

fn decide_signal(signal: &str) -> usize {
    for i in 0..9 {
        if is_permutation(signal, SIGNAL_ON[i]) {
            return i;
        }
    }
    panic!("could not match");
}

fn is_permutation(signal: &str, candidate: &str) -> bool {
    if signal.len() != candidate.len() {
        return false;
    }
    for c in signal.chars() {
        if !candidate.contains(c) {
            return false;
        }
    }
    return true;
}

fn main() {
    let input = read_input();
    {
        let amount = input
            .iter()
            .map(|line| &line.end)
            .flat_map(|end| end.iter().map(|s| s.as_str()))
            .filter(|segment| SEARCH_LEN.contains(&segment.len()))
            .count();
        println!("{}", amount);
    }
    {
        // I don't actually understand what the question is asking... \o/
        let decoded = input
            .iter()
            .map(|line| &line.end)
            .flat_map(|end| end.iter().map(|s| s.as_str()))
            .map(|seg| decide_signal(seg))
            .sum::<usize>();
        println!("{}", decoded);
    }
}
