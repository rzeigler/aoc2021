use std::io;
use std::io::BufRead;

fn read_input() -> Vec<u32> {
    let stdin = io::stdin();
    let lines = stdin.lock().lines();
    lines
        .map(|r| r.unwrap())
        .filter(|line| !line.is_empty())
        .map(|line| u32::from_str_radix(line.as_str(), 2).unwrap())
        .collect()
}

fn high_bit(n: u32) -> u32 {
    let mut bit = 31;
    while 1 << bit & n != 1 << bit {
        bit -= 1;
    }
    bit
}

fn has_bit(n: u32, bit: u32) -> bool {
    n & 1 << bit == 1 << bit
}

fn gamma_rate(numbers: &Vec<u32>, high_bit: u32) -> u32 {
    let mut out = 0u32;
    for bit in 0..high_bit + 1 {
        let count = numbers.iter().cloned().filter(|n| has_bit(*n, bit)).count();
        if count > numbers.len() / 2 {
            out |= 1 << bit
        }
    }
    out
}

fn epsilon_rate(numbers: &Vec<u32>, high_bit: u32) -> u32 {
    let mut out = 0u32;
    for bit in 0..high_bit + 1 {
        let count = numbers.iter().cloned().filter(|n| has_bit(*n, bit)).count();
        if count < numbers.len() / 2 {
            out |= 1 << bit
        }
    }
    out
}

fn oxygen_rating(mut numbers: Vec<u32>, high_bit: u32) -> u32 {
    let mut bits = (0..high_bit + 1).into_iter().rev();
    while let Some(bit) = bits.next() {
        let count = numbers.iter().cloned().filter(|n| has_bit(*n, bit)).count();
        let off_count = numbers.len() - count;
        if count >= off_count {
            numbers.retain(|n| has_bit(*n, bit))
        } else {
            numbers.retain(|n| !has_bit(*n, bit))
        }
        if numbers.len() == 1 {
            return numbers[0];
        }
    }
    panic!("unable to find an oxygen rating");
}

fn co2_rating(mut numbers: Vec<u32>, high_bit: u32) -> u32 {
    let mut bits = (0..high_bit + 1).into_iter().rev();
    while let Some(bit) = bits.next() {
        let count = numbers.iter().cloned().filter(|n| has_bit(*n, bit)).count();
        let off_count = numbers.len() - count;
        if count < off_count {
            numbers.retain(|n| has_bit(*n, bit));
        } else {
            numbers.retain(|n| !has_bit(*n, bit));
        }
        if numbers.len() == 1 {
            return numbers[0];
        }
    }
    panic!("unable to find co2 rating");
}

fn main() {
    let input = read_input();
    {
        let high_bit = input
            .iter()
            .map(|n| high_bit(*n))
            .max()
            .expect("empty input");
        let gamma = gamma_rate(&input, high_bit);
        let epsilon = epsilon_rate(&input, high_bit);
        println!("{}", gamma * epsilon);
    }
    {
        let high_bit = input
            .iter()
            .map(|n| high_bit(*n))
            .max()
            .expect("empty input");
        let oxygen = oxygen_rating(input.clone(), high_bit);
        let co2 = co2_rating(input.clone(), high_bit);
        println!("{}", oxygen * co2);
    }
    // {
    //     let gamma = compute_gamma(&input);
    //     let epsilon = compute_epsilon(&input);
    //     println!("{}", gamma * epsilon);
    // }
    // {
    //     let oxygen = find_oxygen(&input);
    //     let co2 = find_co2(&input);
    //     println!("{}", oxygen * co2);
    // }
}
