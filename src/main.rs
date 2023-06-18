use crate::teleBind::AltAzm;
use std::{env, io::stdin};
pub mod calc;
pub mod tele;
pub mod teleBind;

fn main() {
	let args: Vec<String> = env::args().collect();

	let mut tel = tele::Telescope::new();
	let init: bool = tel.init();
	if !init {
		println!("Failed to initalize");
		if !confirm() {
			panic!("Failed to initalize");
		}
	}
	tel.print_model();

	let is_aligned = tel.is_aligned();
	if !is_aligned {
		dbg!(tel.get_alt_az());
		println!("Not aligned");
		if !confirm() {
			panic!("Exiting since not aligned");
		}
	}

	let x = args.get(1).unwrap();
	match x as &str {
		"loc" => {
			let lat1: f64 = args.get(2).unwrap().parse().unwrap();
			let lon1: f64 = args.get(3).unwrap().parse().unwrap();
			let alt1: f64 = args.get(4).unwrap().parse().unwrap();
			let lat2: f64 = args.get(5).unwrap().parse().unwrap();
			let lon2: f64 = args.get(6).unwrap().parse().unwrap();
			let alt2: f64 = args.get(7).unwrap().parse().unwrap();
			let offset = get_offset(8, args);
			let target =
				calc::calculate_angles(lat1, lon1, alt1, lat2, lon2, alt2);
			calc::print_visible(lat1, lon1, alt1, lat2, lon2, alt2);
			dbg!(apply_offset(target, offset));
			if slew_confirm() {
				tel.goto_alt_az(apply_offset(target, offset));
			}
		}
		"input" => {
			let lat1: f64 = args.get(2).unwrap().parse().unwrap();
			let lon1: f64 = args.get(3).unwrap().parse().unwrap();
			let alt1: f64 = args.get(4).unwrap().parse().unwrap();
			loop {
				let offset = get_offset(5, args.clone());
				println!("Enter a position (lat lon alt, separated by commas or spaces):");
				let mut x = String::new();
				stdin().read_line(&mut x).expect("Failed to read line");
				let floats: Vec<f64> = x
					.split(|c| c == ' ' || c == ',')
					.filter_map(|s| s.trim().parse().ok())
					.collect();

				if floats.len() == 3 {
					let lat2 = floats[0];
					let lon2 = floats[1];
					let alt2 = floats[2];
					dbg!(lat1, lon1, alt1);
					let target = calc::calculate_angles(
						lat1, lon1, alt1, lat2, lon2, alt2,
					);
					dbg!(target);
					calc::print_visible(lat1, lon1, alt1, lat2, lon2, alt2);
					if slew_confirm() {
						tel.goto_alt_az(apply_offset(target, offset));
					}
				}
			}
		}
		"altazm" => {
			loop {
				println!("Enter a position (alt azm, separated by commas or spaces):");
				let mut x = String::new();
				stdin().read_line(&mut x).expect("Failed to read line");
				let floats: Vec<f64> = x
					.split(|c| c == ' ' || c == ',')
					.filter_map(|s| s.trim().parse().ok())
					.collect();

				if floats.len() == 2 {
					let target = AltAzm {
						alt: floats[0],
						azm: floats[1],
					};
					dbg!(target);
					if slew_confirm() {
						tel.goto_alt_az(target);
					}
				}
			}
		}
		"pos" => {
			dbg!(tel.get_alt_az());
		}
		"calib" => {
			let lat1: f64 = args.get(2).unwrap().parse().unwrap();
			let lon1: f64 = args.get(3).unwrap().parse().unwrap();
			let alt1: f64 = args.get(4).unwrap().parse().unwrap();
			let lat2: f64 = args.get(5).unwrap().parse().unwrap();
			let lon2: f64 = args.get(6).unwrap().parse().unwrap();
			let alt2: f64 = args.get(7).unwrap().parse().unwrap();
			let true_pos =
				calc::calculate_angles(lat1, lon1, alt1, lat2, lon2, alt2);
			let current_pos = tel.get_alt_az();
			let target = AltAzm {
				alt: (current_pos.alt - true_pos.alt),
				azm: (current_pos.azm - true_pos.azm),
			};
			dbg!(target);
			if slew_confirm() {
				tel.goto_alt_az(target);
			}
		}
		"balloon" => {
			let lat1: f64 = args.get(2).unwrap().parse().unwrap();
			let lon1: f64 = args.get(3).unwrap().parse().unwrap();
			let alt1: f64 = args.get(4).unwrap().parse().unwrap();
			loop {
				let balloon = args.get(5).unwrap().to_string();
				let balloonpos = get_data(balloon);
				let lat2 = balloonpos.lat;
				let lon2 = balloonpos.lon;
				let alt2 = balloonpos.alt;
				let target =
					calc::calculate_angles(lat1, lon1, alt1, lat2, lon2, alt2);
				dbg!(target);
				if slew_confirm() {
					tel.goto_alt_az(target);
				}
				input();
			}
		}
		_ => {
			panic!("Unknown input");
		}
	}
}

fn get_data(balloon: String) -> calc::Position {
	let x = reqwest::blocking::get(format!(
		"http://api.v2.sondehub.org/sonde/{}",
		balloon
	))
	.unwrap()
	.text()
	.unwrap();
	let y = json::parse(&x).unwrap().pop();
	let lat = y["lat"].as_f64().unwrap();
	let lon = y["lon"].as_f64().unwrap();
	let alt = y["alt"].as_f64().unwrap();

	let pos = calc::Position {
		lat,
		lon,
		alt,
	};
	dbg!(&pos);
	pos
}

fn get_offset(i: usize, args: Vec<String>) -> f64 {
	match args.get(i) {
		Some(a) => a.parse().unwrap(),
		None => 0.0,
	}
}

fn apply_offset(target: AltAzm, offset: f64) -> AltAzm {
	AltAzm {
		alt: target.alt,
		azm: target.azm - offset,
	}
}

fn input() -> String {
	let mut x = String::new();
	stdin().read_line(&mut x).expect("Failed to read line");
	x.trim().to_string().to_lowercase()
}

fn confirm() -> bool {
	println!("Enter y to confirm");
	input() == "y"
}

fn slew_confirm() -> bool {
	println!("Slew to?");
	confirm()
}
