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

fn decode_1(wiring: &Vec<String>) -> &str {
    wiring
        .iter()
        .map(|s| s.as_str())
        .find(|s| s.len() == 2)
        .expect("Unable to decode 1")
}

fn decode_4(wiring: &Vec<String>) -> &str {
    wiring
        .iter()
        .map(|s| s.as_str())
        .find(|s| s.len() == 4)
        .expect("unable to decode 4")
}

fn decode_7(wiring: &Vec<String>) -> &str {
    wiring
        .iter()
        .map(|s| s.as_str())
        .find(|s| s.len() == 3)
        .expect("unable to decode 7")
}

fn decode_8(wiring: &Vec<String>) -> &str {
    wiring
        .iter()
        .map(|s| s.as_str())
        .find(|s| s.len() == 7)
        .expect("unable to decode 8")
}

fn decode_0(wiring: &Vec<String>) -> &str {
    let eight = decode_8(wiring);
    // 0 should have 6 segments and overlap completely with 1
    wiring
        .iter()
        .map(|s| s.as_str())
        .filter(|s| s.len() == 6)
        .find(|s| full_overlap(*s, eight))
        .expect("unable to decode 0")
}

fn decode_3(wiring: &Vec<String>) -> &str {
    let seven = decode_7(wiring);
    wiring
        .iter()
        .map(|s| s.as_str())
        .filter(|s| s.len() == 5)
        .find(|s| full_overlap(*s, seven))
        .expect("unable to decode 3")
}

// 2 and 5

fn decode_top(wiring: &Vec<String>) -> char {
    let one = decode_1(wiring);
    let seven = decode_7(wiring);
    seven
        .chars()
        // THe top is the only char in 7 that doesn't also occur in one
        .find(|c| !one.chars().any(|o| o == *c))
        .expect("unable to decode top")
}

fn decode_bottom_right(wiring: &Vec<String>) -> char {
    let one = decode_1(wiring);
    let six = decode_6(wiring);
    one.chars()
        // The bottom right is the only char in 1 that is also in 6
        .find(|x| six.chars().any(|s| s == *x))
        .expect("unable to decode bottom right")
}

fn decode_top_right(wiring: &Vec<String>) -> char {
    let one = decode_1(wiring);
    let bottom_right = decode_bottom_right(wiring);
    one.chars()
        .find(|c| *c != bottom_right)
        .expect("unable to decode top right")
}

fn decode_2(wiring: &Vec<String>) -> &str {
    let top_right = decode_top_right(wiring);
    wiring
        .iter()
        .map(|s| s.as_str())
        .filter(|s| s.len() == 5)
        .find(|s| s.contains(top_right))
        .expect("unable to decode 2")
}

fn decode_5(wiring: &Vec<String>) -> &str {
    let bottom_right = decode_bottom_right(wiring);
    wiring
        .iter()
        .map(|s| s.as_str())
        .filter(|s| s.len() == 5)
        .find(|s| s.contains(bottom_right))
        .expect("unable to decode 5")
}

fn decode_9(wiring: &Vec<String>) -> &str {
    let seven = decode_7(wiring);
    wiring
        .iter()
        .map(|s| s.as_str())
        .filter(|s| s.len() == 6)
        .find(|s| full_overlap(*s, seven))
        .expect("unable to decode 9")
}

fn decode_6(wiring: &Vec<String>) -> &str {
    let nine = decode_0(wiring);
    wiring
        .iter()
        .map(|s| s.as_str())
        .filter(|s| s.len() == 6)
        .find(|s| (*s) != nine && (*s) != zero)
        .expect("unable to decode 6")
}

fn full_overlap(outer: &str, inner: &str) -> bool {
    inner.chars().all(|c| outer.chars().any(|o| o == c))
}

fn decode_line(line: &Line) -> Vec<usize> {
    let table = [
        decode_0(&line.start),
        decode_1(&line.start),
        decode_2(&line.start),
        decode_3(&line.start),
        decode_4(&line.start),
        decode_5(&line.start),
        decode_6(&line.start),
        decode_7(&line.start),
        decode_8(&line.start),
    ];
    line.end
        .iter()
        .map(|s| decode(s.as_str(), &table))
        .collect()
}

fn decode(output: &str, table: &[&str; 9]) -> usize {
    for (i, encoding) in table.iter().cloned().enumerate() {
        if is_permuted(encoding, output) {
            return i;
        }
    }
    panic!("unable to decode output {} from {:?}", output, table)
}

fn is_permuted(left: &str, right: &str) -> bool {
    left.chars().all(|l| right.chars().any(|r| l == r))
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
        let sum = input
            .iter()
            .flat_map(|line| decode_line(line).into_iter())
            .sum::<usize>();
        println!("{}", sum);
        // I don't actually understand what the question is asking... \o/

        // println!("{}", decoded);
    }
}
