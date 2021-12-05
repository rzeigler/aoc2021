use std::borrow::Borrow;
use std::cmp::Ordering;
use std::collections::BTreeMap;
use std::io::{self, BufRead};
use std::iter;

#[derive(PartialEq, Eq, Debug)]
struct Point {
    x: i32,
    y: i32,
}

impl Point {
    fn of(x: i32, y: i32) -> Point {
        Point { x, y }
    }

    fn of_tuple(xy: (i32, i32)) -> Point {
        Point { x: xy.0, y: xy.1 }
    }
}

impl PartialOrd for Point {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        match self.x.partial_cmp(&other.x) {
            Some(Ordering::Equal) => {}
            ord => return ord,
        }
        self.y.partial_cmp(&other.y)
    }
}

impl Ord for Point {
    fn cmp(&self, other: &Self) -> Ordering {
        match self.x.cmp(&other.x) {
            Ordering::Equal => {}
            ord => return ord,
        }
        self.y.cmp(&other.y)
    }
}

#[derive(Debug)]
struct Line {
    start: Point,
    end: Point,
}

impl Line {
    fn from(start: Point, end: Point) -> Line {
        Line { start, end }
    }

    fn is_straight(&self) -> bool {
        self.start.x == self.end.x || self.start.y == self.end.y
    }

    fn is_diagonal(&self) -> bool {
        let delta_x = (self.end.x - self.start.x).abs();
        let delta_y = (self.end.y - self.start.y).abs();
        delta_x == delta_y
    }

    fn to_points(&self) -> Box<dyn Iterator<Item = Point>> {
        if self.start.x == self.end.x {
            let const_x = self.start.x;
            let start_y = self.start.y.min(self.end.y);
            let end_y = self.start.y.max(self.end.y);
            Box::new((start_y..end_y + 1).map(move |y| Point::of(const_x, y)))
        } else if self.start.y == self.end.y {
            let const_y = self.start.y;
            let start_x = self.start.x.min(self.end.x);
            let end_x = self.start.x.max(self.end.x);
            Box::new((start_x..end_x + 1).map(move |x| Point::of(x, const_y)))
        } else if self.is_diagonal() {
            let start_x = self.start.x.min(self.end.x);
            let end_x = self.start.x.max(self.end.x);

            let start_y = self.start.y.min(self.end.y);
            let end_y = self.start.y.max(self.end.y);

            let x_iter: Box<dyn Iterator<Item = i32>> = if start_x == self.end.x {
                Box::new((start_x..end_x + 1).into_iter().rev())
            } else {
                Box::new((start_x..end_x + 1).into_iter())
            };

            let y_iter: Box<dyn Iterator<Item = i32>> = if start_y == self.end.y {
                Box::new((start_y..end_y + 1).into_iter().rev())
            } else {
                Box::new((start_y..end_y + 1).into_iter())
            };

            Box::new(x_iter.zip(y_iter).map(Point::of_tuple))

            // Box::new(
            //     (start_x..end_x + 1)
            //         .zip(start_y..end_y + 1)
            //         .map(Point::of_tuple),
            // )
        } else {
            panic!("cannot generate points from invalid line")
        }
    }
}

fn read_input() -> Vec<Line> {
    let stdin = io::stdin();
    let lines = stdin.lock().lines();

    lines
        .map(|line_res| read_line(&line_res.unwrap()))
        .collect()
}

fn read_line(s: &String) -> Line {
    let segments = s.split(" -> ").collect::<Vec<_>>();
    if segments.len() != 2 {
        panic!("invalid input line");
    }
    Line::from(read_point(segments[0]), read_point(segments[1]))
}

fn read_point(s: &str) -> Point {
    let segments = s.split(",").collect::<Vec<_>>();
    if segments.len() != 2 {
        panic!("invalid input point");
    }
    let x = segments[0].parse::<i32>().unwrap();
    let y = segments[1].parse::<i32>().unwrap();
    Point::of(x, y)
}

fn debug_print<L: Borrow<Line>>(lines: &Vec<L>, table: &BTreeMap<Point, u32>) {
    // Determine bounds
    let x_max = lines
        .iter()
        .flat_map(|line| iter::once(line.borrow().start.x).chain(iter::once(line.borrow().end.x)))
        .max()
        .unwrap();

    let y_max = lines
        .iter()
        .flat_map(|line| iter::once(line.borrow().start.y).chain(iter::once(line.borrow().end.y)))
        .max()
        .unwrap();

    for y in 0..y_max + 1 {
        for x in 0..x_max + 1 {
            let point = Point::of(x, y);
            if let Some(count) = table.get(&point) {
                print!("{} ", count);
            } else {
                print!(". ");
            }
        }
        println!()
    }
}

fn apply_lines<L>(lines: &Vec<L>) -> BTreeMap<Point, u32>
where
    L: Borrow<Line>,
{
    let mut groups = BTreeMap::<Point, u32>::new();
    for l in lines {
        let line = l.borrow();
        // println!("{:?}", line);
        for point in line.to_points() {
            // println!("{:?}", point);
            if let Some(v) = groups.get_mut(&point) {
                (*v) += 1
            } else {
                groups.insert(point, 1);
            }
        }
        // debug_print(lines, &groups)
    }
    groups
}

fn main() {
    let input = read_input();
    {
        let filtered = input
            .iter()
            .filter(|line| line.is_straight())
            .collect::<Vec<_>>();

        let groups = apply_lines(&filtered);
        let dangerous = groups.into_values().filter(|v| (*v) >= 2).count();
        println!("{}", dangerous)
    }
    {
        let filtered = input
            .iter()
            .filter(|line| line.is_straight() || line.is_diagonal())
            .collect::<Vec<_>>();

        let groups = apply_lines(&filtered);
        let dangerous = groups.into_values().filter(|v| (*v) >= 2).count();
        println!("{}", dangerous)
    }
}
