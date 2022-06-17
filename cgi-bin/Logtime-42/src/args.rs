use clap::{Arg, ArgMatches, Command};

pub const ARG_CONFIG: &str = "config";
pub const ARG_LOGIN: &str = "user";
pub const ARG_CUR_MONTH: &str = "month";
pub const ARG_CUR_WEEK: &str = "week";
pub const ARG_CUR_DAY: &str = "day";
pub const ARG_FROM: &str = "from";
pub const ARG_TO: &str = "to";

pub fn matches() -> ArgMatches {
    Command::new("42 GetTime")
        .author("Mikastiv <mleblanc@student.42quebec.com>")
        .about("View logtime of a 42 school student")
        .version("0.1.0")
        .arg(
            Arg::new(ARG_CONFIG)
                .short('c')
                .long("config")
                .value_name("FILE")
                .help("Explicit path of config file")
                .takes_value(true),
        )
        .arg(
            Arg::new(ARG_LOGIN)
                .short('l')
                .long("login")
                .value_name("LOGIN")
                .help("42 login of the user")
                .takes_value(true),
        )
        .arg(
            Arg::new(ARG_CUR_MONTH)
                .short('m')
                .long("month")
                .help("Logtime of the current month")
                .conflicts_with_all(&[ARG_CUR_DAY, ARG_CUR_WEEK]),
        )
        .arg(
            Arg::new(ARG_CUR_WEEK)
                .short('w')
                .long("week")
                .help("Logtime of the current week")
                .conflicts_with_all(&[ARG_CUR_DAY, ARG_CUR_MONTH]),
        )
        .arg(
            Arg::new(ARG_CUR_DAY)
                .short('d')
                .long("day")
                .help("Logtime of the current day")
                .conflicts_with_all(&[ARG_CUR_WEEK, ARG_CUR_MONTH]),
        )
        .arg(
            Arg::new(ARG_FROM)
                .short('f')
                .long("from")
                .help("Beginning of date span")
                .takes_value(true)
                .value_name("YYYY-MM-DD")
                .requires(ARG_TO)
                .conflicts_with_all(&[ARG_CUR_DAY, ARG_CUR_WEEK, ARG_CUR_MONTH]),
        )
        .arg(
            Arg::new(ARG_TO)
                .short('t')
                .long("to")
                .help("End of date span")
                .takes_value(true)
                .value_name("YYYY-MM-DD")
                .requires(ARG_FROM)
                .conflicts_with_all(&[ARG_CUR_DAY, ARG_CUR_WEEK, ARG_CUR_MONTH]),
        )
        .get_matches()
}
