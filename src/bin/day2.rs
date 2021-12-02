use std::env;
use std::fs::File;
use std::io::{BufRead, BufReader};
use std::ops::AddAssign;

enum Command {
    Up(i32),
    Down(i32),
    Forward(i32),
}

#[derive(Debug)]
struct Pos {
    horizontal: i32,
    depth: i32,
}

impl Pos {
    fn zero() -> Pos {
        Pos {
            horizontal: 0,
            depth: 0,
        }
    }
}

#[derive(Debug)]
struct PosAndAim {
    pos: Pos,
    aim: i32,
}

impl PosAndAim {
    fn zero() -> PosAndAim {
        PosAndAim {
            pos: Pos::zero(),
            aim: 0,
        }
    }
}

impl AddAssign<&Command> for Pos {
    fn add_assign(&mut self, rhs: &Command) {
        match rhs {
            &Command::Down(depth) => self.depth += depth,
            &Command::Up(depth) => self.depth -= depth,
            &Command::Forward(dist) => self.horizontal += dist,
        }
    }
}

impl AddAssign<&Command> for PosAndAim {
    fn add_assign(&mut self, rhs: &Command) {
        match rhs {
            &Command::Down(modify) => self.aim += modify,
            &Command::Up(modify) => self.aim -= modify,
            &Command::Forward(dist) => {
                self.pos.horizontal += dist;
                self.pos.depth += self.aim * dist;
            }
        }
    }
}

fn read_input() -> Vec<Command> {
    let input_path = env::args()
        .skip(1)
        .next()
        .expect("input argument is required");

    let mut output = Vec::<Command>::new();
    let file = File::open(input_path).unwrap();
    for line_result in BufReader::new(file).lines() {
        let line = line_result.unwrap();
        if !line.is_empty() {
            let parts = line.split_ascii_whitespace().collect::<Vec<_>>();
            if parts.len() != 2 {
                panic!("invalid file input");
            }

            let dist = parts[1].parse::<i32>().unwrap();
            if parts[0] == "forward" {
                output.push(Command::Forward(dist));
            } else if parts[0] == "up" {
                output.push(Command::Up(dist));
            } else if parts[0] == "down" {
                output.push(Command::Down(dist));
            } else {
                panic!("unrecognized input: {}", parts[1]);
            }
        }
    }
    output
}

pub fn main() {
    let motions = read_input();
    {
        let mut pos = Pos::zero();
        for motion in motions.iter() {
            pos += motion;
        }
        println!("{}", pos.depth * pos.horizontal)
    }
    {
        let mut pos = PosAndAim::zero();
        for motion in motions.iter() {
            pos += motion;
        }
        println!("{}", pos.pos.depth * pos.pos.horizontal)
    }
}
