use std::f64::consts::PI;
use std::thread::sleep;
use std::time::Duration;
use std::{env, io::stdin};
use tele::AltAz;
pub mod calc;
pub mod tele;

fn main() {
	let args: Vec<String> = env::args().collect();
	let lat: f64 = args.get(1).unwrap().parse().unwrap();
	let lon: f64 = args.get(2).unwrap().parse().unwrap();
	let alt: f64 = args.get(3).unwrap().parse().unwrap();

	// let lat2 = 40.74842524276716;
	// let lon2 = -73.98571551609963;
	// let alt2 = 380.0;

	let lat2: f64 = args.get(4).unwrap().parse().unwrap();
	let lon2: f64 = args.get(5).unwrap().parse().unwrap();
	let alt2: f64 = args.get(6).unwrap().parse().unwrap();

	let target = calc::calculate_angles(
		lat, lon, alt, lat2, lon2, alt2,
	);

	// dbg!(target);

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
		unsafe{dbg!(tele::get_alt_az())};
		println!("Not aligned");
		if !confirm(){
			panic!("Exiting since not aligned");
		}
	}

	// unsafe {
	// 	println!("Previous location:");
	// 	tele::get_loc();
	// 	tele::set_loc(lat, lon);
	// }
	
	// loop {
	// }
	unsafe {
		tele::goto_alt_az(target);
	}
}


fn confirm() -> bool{
	println!("Enter y to confirm");
	let mut x = String::new();
	stdin().read_line(&mut x).expect("Failed to read line");
	x = x.trim().to_string().to_lowercase();
	return x == "y";
}