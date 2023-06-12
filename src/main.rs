use std::{env, io::stdin};

use crate::tele::AltAzm;
pub mod calc;
pub mod tele;

fn main() {
	let args: Vec<String> = env::args().collect();
	let x: bool = unsafe { tele::init() };
	if !x {
		panic!("Failed to initalize");
	}
	unsafe {
		tele::stop();
		tele::print_model();
	}

	let is_aligned: i8 = unsafe { tele::is_aligned() };
	if is_aligned != 1 {
		unsafe { dbg!(tele::get_alt_az()) };
		println!("Not aligned");
		if !confirm() {
			panic!("Exiting since not aligned");
		}
	}

	let x = args.get(1).unwrap();
	match x as &str {
		"loc" => {
			let lat: f64 = args.get(2).unwrap().parse().unwrap();
			let lon: f64 = args.get(3).unwrap().parse().unwrap();
			let alt: f64 = args.get(4).unwrap().parse().unwrap();
			let lat2: f64 = args.get(5).unwrap().parse().unwrap();
			let lon2: f64 = args.get(6).unwrap().parse().unwrap();
			let alt2: f64 = args.get(7).unwrap().parse().unwrap();
			let target =
				calc::calculate_angles(lat, lon, alt, lat2, lon2, alt2);
			dbg!(target);
			if slew_confirm() {
				unsafe {
					tele::goto_alt_az(target);
				}
			}
		}
		"input" => loop {
			let lat: f64 = args.get(2).unwrap().parse().unwrap();
			let lon: f64 = args.get(3).unwrap().parse().unwrap();
			let alt: f64 = args.get(4).unwrap().parse().unwrap();
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
				dbg!(lat, lon, alt);
				let target =
					calc::calculate_angles(lat, lon, alt, lat2, lon2, alt2);
				dbg!(target);
				if slew_confirm() {
					unsafe {
						tele::goto_alt_az(target);
					}
				}
			}
		},
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
						unsafe {
							tele::goto_alt_az(target);
						}
					}
				}
			}
		}
		_ => {
			panic!("Unknown input");
		}
	}
}

fn confirm() -> bool {
	println!("Enter y to confirm");
	let mut x = String::new();
	stdin().read_line(&mut x).expect("Failed to read line");
	x = x.trim().to_string().to_lowercase();
	return x == "y";
}

fn slew_confirm() -> bool {
	println!("Slew to?");
	confirm()
}
