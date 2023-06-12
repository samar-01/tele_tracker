pub mod tele;

fn main() {
	unsafe {
		tele::init();
		tele::print_model();
		tele::stop();
	}
}
