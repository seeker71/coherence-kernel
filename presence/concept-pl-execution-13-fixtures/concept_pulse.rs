// concept: selective-retention
fn main() {
    let signals: [i64; 6] = [2, 7, 1, 8, 2, 8];
    let threshold: i64 = 5;
    let score_floor: i64 = 100;
    let retained_floor: i64 = 3;
    let mut score: i64 = 0; let mut retained: i64 = 0;
    for (index, value) in signals.iter().enumerate() {
        score += *value * (index as i64 + 1);
        if *value >= threshold { retained += 1; }
    }
    let verdict = if score >= score_floor && retained >= retained_floor { "coherent" } else { "fragmented" };
    println!("{}:{}:{}", verdict, score, retained);
}
