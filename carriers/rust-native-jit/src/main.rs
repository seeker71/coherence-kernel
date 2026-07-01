use form_rust_native_jit_carrier::{execute_add1, execute_payload, host_add1_payload, RouteInputs};

fn main() {
    let mut args = std::env::args().skip(1);
    let Some(cmd) = args.next() else {
        eprintln!("usage: form-rust-native-jit-carrier add1 <tagged-int> | route <pass|guard|runtime|invalidated|parity|stale> <tagged-int>");
        std::process::exit(2);
    };

    let outcome = match cmd.as_str() {
        "add1" => {
            let arg = parse_arg(args.next());
            execute_add1(arg)
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
