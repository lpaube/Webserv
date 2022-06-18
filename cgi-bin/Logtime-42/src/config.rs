use std::fs;
use std::io::Read;

use serde::Deserialize;

const CONFIG_FILE: &str = "config.json";

#[derive(Deserialize)]
pub struct Config {
    pub client_id: String,
    pub secret: String,
    pub from: Option<String>,
    pub to: Option<String>,
    pub login: Option<String>,
}

pub fn get_config(path: Option<&str>) -> Result<Config, String> {
    let path = match path {
        Some(p) => p,
        None => CONFIG_FILE,
    };

    let config_file = match fs::File::open(path) {
        Ok(mut file) => {
            let mut content = String::new();
            file.read_to_string(&mut content).unwrap();
            content
        }
        Err(_) => {
            return Err(format!("Cannot open file '{}'", path));
        }
    };

    serde_json::from_str(&config_file).map_err(|e| format!("{}: {}", path, e))
}
