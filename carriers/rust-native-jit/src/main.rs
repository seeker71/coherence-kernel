use form_rust_native_jit_carrier::{
    add1_hot_function, checked_div_hot_function, execute_add1, execute_checked_array_get,
    execute_checked_div, execute_checked_field_load, execute_hot_function, execute_null_array_get,
    execute_null_field_load, execute_payload, host_add1_payload, RouteInputs,
};

fn main() {
    let mut args = std::env::args().skip(1);
    let Some(cmd) = args.next() else {
        eprintln!("usage: form-rust-native-jit-carrier add1 <tagged-int> | div <num> <den> | jit-add1 <tagged-int> | jit-div <num> <den> | jit-array <index>|null|bounds | jit-field <slot>|null | route <pass|guard|runtime|invalidated|parity|stale> <tagged-int>");
        std::process::exit(2);
    };

    let outcome = match cmd.as_str() {
        "add1" => {
            let arg = parse_arg(args.next());
            execute_add1(arg)
        }
        "div" => {
            let numerator = parse_arg(args.next());
            let denominator = parse_arg(args.next());
            execute_checked_div(numerator, denominator)
        }
        "jit-add1" => {
            let arg = parse_arg(args.next());
            execute_hot_function(&add1_hot_function(), &[arg])
        }
        "jit-div" => {
            let numerator = parse_arg(args.next());
            let denominator = parse_arg(args.next());
            execute_hot_function(&checked_div_hot_function(), &[numerator, denominator])
        }
        "jit-array" => {
            let mode = args.next().unwrap_or_else(|| "1".to_string());
            let values = [11_i64, 22, 33];
            match mode.as_str() {
                "null" => execute_null_array_get(0, values.len() as i64),
                "bounds" => execute_checked_array_get(&values, values.len() as i64),
                _ => {
                    let index = parse_arg(Some(mode));
                    execute_checked_array_get(&values, index)
                }
            }
        }
        "jit-field" => {
            let mode = args.next().unwrap_or_else(|| "1".to_string());
            let fields = [101_i64, 202, 303];
            match mode.as_str() {
                "null" => execute_null_field_load(1),
                _ => {
                    let slot = parse_slot(Some(mode));
                    execute_checked_field_load(&fields, slot)
                }
            }
        }
        "route" => {
            let mode = args.next().unwrap_or_else(|| "pass".to_string());
            let arg = parse_arg(args.next());
            let route = match mode.as_str() {
                "pass" => RouteInputs::PASS,
                "guard" => RouteInputs {
                    guard_ok: false,
                    ..RouteInputs::PASS
                },
                "runtime" => RouteInputs {
                    runtime_ok: false,
                    ..RouteInputs::PASS
                },
                "invalidated" => RouteInputs {
                    invalidated: true,
                    ..RouteInputs::PASS
                },
                "parity" => RouteInputs {
                    parity_ok: false,
                    ..RouteInputs::PASS
                },
                "stale" => RouteInputs {
                    stale: true,
                    ..RouteInputs::PASS
                },
                _ => {
                    eprintln!("unknown route mode: {mode}");
                    std::process::exit(2);
                }
            };
            execute_payload(&host_add1_payload(), &[arg], route)
        }
        _ => {
            eprintln!("unknown command: {cmd}");
            std::process::exit(2);
        }
    };

    println!("{outcome}");
}

fn parse_arg(raw: Option<String>) -> i64 {
    raw.unwrap_or_else(|| "82".to_string())
        .parse::<i64>()
        .unwrap_or_else(|_| {
            eprintln!("argument must be a tagged integer");
            std::process::exit(2);
        })
}

fn parse_slot(raw: Option<String>) -> usize {
    let value = parse_arg(raw);
    if value < 0 {
        eprintln!("field slot must be non-negative");
        std::process::exit(2);
    }
    value as usize
}
