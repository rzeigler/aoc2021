use std::env;
use std::fs;

use anyhow::{Context, Result};

pub fn read_input() -> Result<Vec<i32>> {
    let input_path = env::args()
        .skip(1)
        .next()
        .with_context(|| "input argument is required")?;

    let content = fs::read_to_string(input_path).with_context(|| "unable to read file content")?;
    let mut values = Vec::<i32>::new();
    for line in content.lines() {
        if !line.is_empty() {
            values.push(
                line.parse::<i32>()
                    .with_context(|| "invalid file content")?,
            );
        }
    }

    Ok(values)
}
