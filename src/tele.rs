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

	pub fn goto_alt_az_custom(&self, a: AltAzm) {
		unsafe { teleBind::goto_alt_az_custom(a) }
	}
}
