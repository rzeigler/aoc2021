use std::io::{self, BufRead};

fn read_input() -> Vec<u64> {
    let stdin = io::stdin();
    let line = stdin
        .lock()
        .lines()
        .next()
        .expect("required an input line")
        .unwrap();
    line.split(",")
        .map(|age| age.parse::<u64>())
        .collect::<Result<Vec<_>, _>>()
        .unwrap()
}

fn prepare_input(input: &Vec<u64>) -> Box<[u64; 9]> {
    let mut prepared = Box::new([0u64; 9]);
    for i in input.iter().cloned() {
        prepared[i as usize] += 1;
    }
    prepared
}

fn iterate(state: &mut [u64; 9]) {
    let gave_birth = state[0];
    for i in 0..8 {
        state[i] = state[i + 1]
    }
    state[6] += gave_birth;
    state[8] = gave_birth;
}

// fn iterate(school: &mut Vec<u8>) {
//     let mut born_count = 0usize;
//     for fish in school.iter_mut() {
//         if *fish == 0 {
//             *fish = 6;
//             born_count += 1;
//         } else {
//             *fish -= 1;
//         }
//     }
//     school.reserve(born_count);
//     for _ in 0..born_count {
//         school.push(8);
//     }
// }

// const MAX_SLICE: usize = 100_000;

// fn iterate_2(school: &mut Vec<Vec<u8>>) {
//     let mut born_count = 0usize;
//     for slice in school.iter_mut() {
//         for fish in slice.iter_mut() {
//             if *fish == 0 {
//                 *fish = 6;
//                 born_count += 1;
//             } else {
//                 *fish -= 1;
//             }
//         }
//     }
//     if born_count != 0 {
//         while born_count > MAX_SLICE {
//             school.push(iter::repeat(8).take(MAX_SLICE).collect());
//             born_count -= MAX_SLICE
//         }
//         school.push(iter::repeat(8).take(born_count).collect());
//     }
// }

// fn count_2(school: &Vec<Vec<u8>>) -> usize {
//     school.iter().map(|slice| slice.len()).sum()
// }

fn main() {
    let input = read_input();
    {
        let mut state = prepare_input(&input);
        // println!("{:?}", state);
        for _ in 0..80 {
            // println!("{:?}", i);
            iterate(state.as_mut());
            // println!("{:?}", state);
            // println!("{:?}", state);
        }
        println!("{}", state.iter().sum::<u64>());
    }
    {
        let mut state = prepare_input(&input);
        // println!("{:?}", state);
        for _ in 0..256 {
            // println!("{:?}", i);
            iterate(state.as_mut());
            // println!("{:?}", state);
            // println!("{:?}", state);
        }
        println!("{}", state.iter().sum::<u64>());
    }
}
