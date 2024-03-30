use nalgebra::{clamp, ComplexField};

use crate::teleBind::{self, *};

#[derive(Clone)]
pub struct Telescope {
	dev: i8,
}

impl Default for Telescope {
	fn default() -> Self {
		Self::new()
	}
}

impl Telescope {
	pub fn new() -> Telescope {
		Telescope { dev: 0 }
	}

	pub fn init(&mut self) -> bool {
		self.dev = unsafe { teleBind::init() };
		self.dev != 0
	}

	pub fn stop(&self) {
		unsafe { teleBind::stop() };
	}

	pub fn print_model(&self) {
		unsafe { teleBind::print_model() };
	}

	pub fn is_aligned(&self) -> bool {
		unsafe { teleBind::is_aligned() }
	}

	pub fn get_alt_az(&self) -> AltAzm {
		unsafe { teleBind::get_alt_az() }
	}

	pub fn goto_alt_az(&self, a: AltAzm) {
		unsafe { teleBind::goto_alt_az(a) }
	}

	pub fn goto_test(&self, a: AltAzm) {
		unsafe { teleBind::goto_alt_az_custom(a) }
		// let current = self.get_alt_az();
		// let dir:i8;
		// if a.alt > current.alt {
		// 	dir = TC_DIR_POSITIVE as i8;
		// } else if a.alt < current.alt {
		// 	dir = TC_DIR_NEGATIVE as i8;
		// } else {
		// 	unsafe {
		// 		teleBind::slew_variable(TC_AXIS_ALT as i8, TC_DIR_POSITIVE as i8, 0.0);
		// 	}
		// 	return;
		// }

		// let  mut rate: f32 = ((a.alt - current.alt)/5.0) as f32;
		// // dbg!(rate);
		// rate = rate.abs().clamp(0.0, 1.0)*SPEEDCONV as f32;
		// // dbg!(rate);
		// unsafe{teleBind::slew_variable(TC_AXIS_ALT as i8, dir, rate)}
	}
}
