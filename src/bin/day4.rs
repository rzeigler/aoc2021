use std::io::{self, BufRead};

#[derive(Clone)]
struct Board {
    numbers: [[u32; 5]; 5],
    marks: [[bool; 5]; 5],
}

impl Board {
    fn new() -> Board {
        Board {
            numbers: [[0; 5]; 5],
            marks: [[false; 5]; 5],
        }
    }

    fn mark(&mut self, num: u32) {
        // Assumes no board has duplicate numbers
        for x in 0..5 {
            for y in 0..5 {
                if self.numbers[x][y] == num {
                    self.marks[x][y] = true;
                    return;
                }
            }
        }
    }

    fn is_winner(&self) -> bool {
        // Check rows
        for x in 0..5 {
            if (0..5).all(|y| self.marks[x][y]) {
                return true;
            }
        }
        // Check columns
        for y in 0..5 {
            if (0..5).all(|x| self.marks[x][y]) {
                return true;
            }
        }
        false
    }

    fn score(&self) -> u32 {
        let mut total: u32 = 0;
        for x in 0..5 {
            for y in 0..5 {
                if !self.marks[x][y] {
                    total += self.numbers[x][y]
                }
            }
        }
        total
    }
}

#[derive(Clone)]
struct PuzzleInput {
    draw_sequence: Vec<u32>,
    boards: Vec<Box<Board>>,
}

fn read_input() -> PuzzleInput {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();

    let draw_line = lines.next().expect("missing header line").unwrap();

    let draw_sequence = draw_line
        .split(",")
        .map(|cell| cell.parse::<u32>())
        .collect::<Result<Vec<_>, _>>()
        .unwrap();

    let mut boards = Vec::<Box<Board>>::new();

    // When there is another line that indicates the start of a board
    while let Some(result) = lines.next() {
        result.unwrap();
        let mut board = Box::new(Board::new());
        for i in 0..5 {
            let line = lines.next().expect("missing required board line").unwrap();
            let mut parts = line.split_ascii_whitespace();
            let row: [u32; 5] = [
                parts.next().expect("invalid line").parse().unwrap(),
                parts.next().expect("invalid line").parse().unwrap(),
                parts.next().expect("invalid line").parse().unwrap(),
                parts.next().expect("invalid line").parse().unwrap(),
                parts.next().expect("invalid line").parse().unwrap(),
            ];
            if parts.next().is_some() {
                panic!("too many line parts")
            }
            board.numbers[i] = row
        }
        boards.push(board)
    }

    PuzzleInput {
        draw_sequence,
        boards,
    }
}

fn main() {
    let input = read_input();

    {
        let mut boards1 = input.boards.clone();
        for draw in input.draw_sequence.clone() {
            for board in boards1.iter_mut() {
                board.mark(draw);
            }
            if let Some(winner) = boards1.iter().find(|board| board.is_winner()) {
                println!("{}", winner.score() * draw);
                break;
            }
        }
    }
    {}
}
