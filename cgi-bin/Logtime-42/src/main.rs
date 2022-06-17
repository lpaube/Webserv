use ansi_term::Color;
use args::{ARG_CONFIG, ARG_CUR_DAY, ARG_CUR_MONTH, ARG_CUR_WEEK, ARG_FROM, ARG_LOGIN, ARG_TO};
use clap::ArgMatches;
use config::Config;
use curl::easy::Easy;

mod args;
mod config;
mod date;
mod request;

const LINE_LEN: usize = 29;

fn print_blue_line(len: usize) {
    println!("{}", Color::Blue.bold().paint("─".repeat(len)));
}

fn print_header(from: &str, to: &str) {
    print_blue_line(LINE_LEN);
    println!(
        "From {} to {}",
        Color::Yellow.paint(from),
        Color::Yellow.paint(to)
    );
    print_blue_line(LINE_LEN);
}

fn print_user_logtime(easy: &mut Easy, config: &Config, login: &str, from: &str, to: &str) {
    if let Ok(token) = request::authenticate(easy, config) {
        print_header(from, to);

        match request::get_user_logtime(easy, &token, login, from, to) {
            Ok(time) => {
                let time = format!("{:01.0}h{:02.0}", time.trunc(), time.fract() * 60.0);
                println!("{} ➜  🕑 {}", login, Color::Green.bold().paint(&time),);
            }
            Err(e) => {
                // If curl error is set to 0 (curl success code), bad login
                if e.code() == 0 {
                    eprintln!("{} ➜  ❌ {}", login, Color::Red.bold().paint("bad login"),);
                }
            }
        }

        print_blue_line(LINE_LEN);
    }
}

fn get_login(arg_login: &Option<&str>, config_login: &Option<String>) -> Result<String, String> {
    match *arg_login {
        Some(login) => Ok(login.to_string()),
        None => match config_login {
            Some(login) => Ok(login.clone()),
            None => Err(
                "No login found in config file or options, for more information try --help"
                    .to_string(),
            ),
        },
    }
}

fn get_date_span(matches: &ArgMatches, config: &Config) -> Result<(String, String), String> {
    let month_flag = matches.is_present(ARG_CUR_MONTH);
    let day_flag = matches.is_present(ARG_CUR_DAY);
    let week_flag = matches.is_present(ARG_CUR_WEEK);
    let from = matches.value_of(ARG_FROM);
    let to = matches.value_of(ARG_TO);

    if month_flag {
        return Ok(date::current_month_span());
    }

    if day_flag {
        return Ok(date::current_day_span());
    }

    if week_flag {
        return Ok(date::current_week_span());
    }

    if let (Some(from), Some(to)) = (from, to) {
        if !date::valid_format(from) {
            return Err("Invalid date in input: 'from' date format must be YYYY-MM-DD".to_string());
        }
        if !date::valid_format(to) {
            return Err("Invalid date in input: 'to' date format must be YYYY-MM-DD".to_string());
        }

        return Ok((from.to_string(), to.to_string()));
    }

    match (&config.from, &config.to) {
        (Some(from), Some(to)) => {
            if !date::valid_format(from) {
                return Err(
                    "Invalid date in config file: 'from' date format must be YYYY-MM-DD"
                        .to_string(),
                );
            }
            if !date::valid_format(to) {
                return Err(
                    "Invalid date in config file: 'to' date format must be YYYY-MM-DD".to_string(),
                );
            }

            Ok((from.to_string(), to.to_string()))
        }
        // Default if no date span found
        _ => Ok(date::current_month_span()),
    }
}

fn main() {
    let matches = args::matches();

    let arg_login = matches.value_of(ARG_LOGIN);
    let config_file = matches.value_of(ARG_CONFIG);

    let config = config::get_config(config_file).unwrap_or_else(|e| {
        eprintln!("{}", e);
        // Config file is needed for 42 API key pair
        std::process::exit(1);
    });

    let login = get_login(&arg_login, &config.login).unwrap_or_else(|e| {
        eprintln!("{}", e);
        // Login is needed for the program to work
        std::process::exit(1);
    });

    let (from, to) = get_date_span(&matches, &config).unwrap_or_else(|e| {
        eprintln!("{}", e);
        // Date span is needed for the program to work
        std::process::exit(1);
    });

    let mut easy = Easy::new();
    print_user_logtime(&mut easy, &config, &login, &from, &to);
}
