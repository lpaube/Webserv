use std::{thread::sleep, time::Duration};

use chrono::{DateTime, Utc};
use curl::easy::{Easy, List};
use http::StatusCode;
use serde::Deserialize;

use crate::config::Config;

const TOTAL_ITEMS_HEADER: &str = "x-total:";
const PAGE_SIZE: u32 = 100;
const URL: &str = "https://api.intra.42.fr/";
const BASE_API: &str = "https://api.intra.42.fr/v2/";

#[derive(Deserialize)]
struct Auth {
    access_token: String,
}

#[derive(Deserialize, Clone)]
pub struct User {
    pub id: u32,
    pub login: String,
}

#[derive(Deserialize, Debug)]
pub struct Location {
    pub begin_at: Option<String>,
    pub end_at: Option<String>,
}

fn check_response(easy: &mut Easy) -> Result<(), String> {
    match easy.response_code() {
        Ok(200) => Ok(()),
        Ok(code) => {
            let status = StatusCode::from_u16(code as u16).unwrap();
            Err(format!(
                "{} {}",
                status.as_u16(),
                status.canonical_reason().unwrap()
            ))
        }
        Err(e) => Err(format!("cURL error code: {}", e)),
    }
}

fn total_count(raw_headers: &[u8]) -> u32 {
    let string = String::from_utf8_lossy(raw_headers);
    let headers: Vec<&str> = string.split("\r\n").collect();
    let total_header = headers
        .iter()
        .find(|&&s| s.to_lowercase().starts_with(TOTAL_ITEMS_HEADER));
    match total_header {
        Some(&header) => {
            let parts: Vec<&str> = header.split_whitespace().collect();
            parts[1].parse().unwrap()
        }
        None => 0,
    }
}

fn add_authorization(easy: &mut Easy, token: &str) -> Result<(), curl::Error> {
    let mut headers = List::new();

    headers.append(format!("Authorization: Bearer {}", token).as_str())?;
    easy.http_headers(headers)?;

    Ok(())
}

fn url_encode(s: &str) -> String {
    url::form_urlencoded::Serializer::new(String::new())
        .append_key_only(s)
        .finish()
}

fn send_request(easy: &mut Easy, url: &str) -> Result<(Vec<u8>, Vec<u8>), curl::Error> {
    easy.url(url)?;

    let mut dst = Vec::new();
    let mut headers = Vec::new();
    {
        let mut transfer = easy.transfer();
        transfer.header_function(|header| {
            headers.extend_from_slice(header);
            true
        })?;
        transfer.write_function(|data| {
            dst.extend_from_slice(data);
            Ok(data.len())
        })?;
        transfer.perform()?;
    }

    match check_response(easy) {
        Ok(_) => Ok((dst, headers)),
        Err(e) => {
            eprintln!("{}", e);
            Err(curl::Error::new(1))
        }
    }
}

pub fn authenticate(easy: &mut Easy, config: &Config) -> Result<String, curl::Error> {
    let url = format!(
        "{url}oauth/token?grant_type=client_credentials&client_id={uid}&client_secret={secret}",
        url = URL,
        uid = url_encode(&config.client_id),
        secret = url_encode(&config.secret),
    );

    easy.reset();
    easy.post(true)?;

    let (response, _) = send_request(easy, &url)?;
    Ok(serde_json::from_slice::<Auth>(&response)
        .unwrap()
        .access_token)
}

pub fn get_user(easy: &mut Easy, token: &str, login: &str) -> Result<User, curl::Error> {
    let url = format!(
        "{url}users?filter[login]={login}",
        url = BASE_API,
        login = url_encode(login)
    );

    easy.reset();
    add_authorization(easy, token)?;

    let (response, _) = send_request(easy, &url)?;
    let mut users = serde_json::from_slice::<Vec<User>>(&response).unwrap();

    match users.is_empty() {
        true => Err(curl::Error::new(0)),
        false => Ok(users.swap_remove(0)),
    }
}

fn loc_url(user_id: u32, from: &str, to: &str, page: u32) -> String {
    format!(
        "{url}users/{id}/locations?page={page}&per_page={page_size}&range[begin_at]={start},{end}",
        url = BASE_API,
        id = user_id,
        page = page,
        page_size = PAGE_SIZE,
        start = url_encode(from),
        end = url_encode(to),
    )
}

fn get_locations(
    easy: &mut Easy,
    token: &str,
    user_id: u32,
    from: &str,
    to: &str,
) -> Result<Vec<Location>, curl::Error> {
    let mut locations = Vec::new();
    let first_page = 1;

    let url = loc_url(user_id, from, to, first_page);

    easy.reset();
    add_authorization(easy, token)?;

    let (response, headers) = send_request(easy, &url)?;
    locations.append(&mut serde_json::from_slice::<Vec<Location>>(&response).unwrap());

    let last_page = total_count(&headers) / PAGE_SIZE + 1;

    for page in first_page + 1..=last_page {
        // Sleep a bit to prevent Too Many Requests error
        sleep(Duration::from_secs_f64(1.0));

        let url = loc_url(user_id, from, to, page);

        easy.reset();
        add_authorization(easy, token)?;

        let (response, _) = send_request(easy, &url)?;
        locations.append(&mut serde_json::from_slice::<Vec<Location>>(&response).unwrap());
    }

    Ok(locations)
}

fn sum_time(locations: &[Location]) -> f64 {
    locations.iter().fold(0.0, |acc, loc: &Location| {
        let (start, end) = match (&loc.begin_at, &loc.end_at) {
            (Some(ref s), Some(ref e)) => {
                let start = DateTime::parse_from_rfc3339(s).unwrap();
                let end = DateTime::parse_from_rfc3339(e).unwrap();
                (start, end)
            }
            (Some(ref s), None) => {
                let start = DateTime::parse_from_rfc3339(s).unwrap();
                let end = Utc::now().with_timezone(start.offset());
                (start, end)
            }
            _ => return acc,
        };

        let time = end.signed_duration_since(start);
        let minutes = time.num_seconds() as f64 / 60.0;

        acc + minutes
    })
}

pub fn get_user_logtime(
    easy: &mut Easy,
    token: &str,
    login: &str,
    from: &str,
    to: &str,
) -> Result<f64, curl::Error> {
    let user = get_user(easy, token, login)?;
    let locations = get_locations(easy, token, user.id, from, to)?;

    Ok(sum_time(&locations) / 60.0)
}
