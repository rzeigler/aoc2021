use std::env;
use std::fs;

fn read_input() -> Vec<u32> {
    let input_path = env::args().skip(1).next().expect("input argument required");

    let content = fs::read_to_string(input_path).unwrap();
    let mut values = Vec::<u32>::new();
    for line in content.lines() {
        if !line.is_empty() {
            let num = u32::from_str_radix(line, 2).unwrap();
            values.push(num)
        }
    }
    values
}

const MASKS: [u32; 12] = [
    1 << 11,
    1 << 10,
    1 << 9,
    1 << 8,
    1 << 7,
    1 << 6,
    1 << 5,
    1 << 4,
    1 << 3,
    1 << 2,
    1 << 1,
    1,
];
const SHIFTS: [i32; 12] = [11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0];

fn compute_gamma(input: &Vec<u32>) -> u32 {
    let mut accum: u32 = 0;
    for i in 0..MASKS.len() {
        let mut count = 0;
        for n in input {
            if MASKS[i] & n == MASKS[i] {
                count += 1;
            }
        }
        if count > input.len() / 2 {
            accum |= 1 << SHIFTS[i]
        }
    }
    accum
}

fn compute_epsilon(input: &Vec<u32>) -> u32 {
    let mut accum: u32 = 0;
    for i in 0..MASKS.len() {
        // First we need to decide if any number stretches this far
        let mut use_mask = false;
        for n in input {
            if MASKS[i] & n == MASKS[i] {
                use_mask = true;
                break;
            }
        }

        if use_mask {
            let mut count = 0;
            for n in input {
                if MASKS[i] & n == MASKS[i] {
                    count += 1;
                }
            }
            if count < input.len() / 2 {
                accum |= 1 << SHIFTS[i]
            }
        }
    }
    accum
}

fn bit_active(bit: u32, num: u32) -> bool {
    let mask = 1u32 << bit;
    mask & num == mask
}

fn find_oxygen(input: &Vec<u32>) -> u32 {
    let mut bit = 31u32;
    loop {
        // Find the most common bit
        let active = input.iter().filter(|num| bit_active(bit, **num)).count();
        {
            let mut candidate_iter: Box<dyn Iterator<Item = &u32>> = if active > input.len() / 2 {
                Box::new(input.iter().filter(|num: &&u32| bit_active(bit, **num)))
            } else {
                Box::new(input.iter().filter(|num: &&u32| !bit_active(bit, **num)))
            };
            if let Some(num) = candidate_iter.next() {
                if candidate_iter.next().is_none() {
                    return *num;
                }
            }
        }
        bit -= 1;
    }
}

fn find_co2(input: &Vec<u32>) -> u32 {
    let mut bit = 31u32;
    loop {
        // Find the most common bit
        let active = input.iter().filter(|num| bit_active(bit, **num)).count();
        {
            let mut candidate_iter: Box<dyn Iterator<Item = &u32>> = if active < input.len() / 2 {
                Box::new(input.iter().filter(|num: &&u32| bit_active(bit, **num)))
            } else {
                Box::new(input.iter().filter(|num: &&u32| !bit_active(bit, **num)))
            };
            if let Some(num) = candidate_iter.next() {
                if candidate_iter.next().is_none() {
                    return *num;
                }
            }
        }
        bit -= 1;
    }
}

fn main() {
    let input = read_input();
    {
        let gamma = compute_gamma(&input);
        let epsilon = compute_epsilon(&input);
        println!("{}", gamma * epsilon);
    }
    {
        let oxygen = find_oxygen(&input);
        let co2 = find_co2(&input);
        println!("{}", oxygen * co2);
    }
}
