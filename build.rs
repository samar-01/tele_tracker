extern crate bindgen;
extern crate cc;

use std::env;
use std::path::PathBuf;

fn main() {
	// println!("cargo:rustc-link-lib=opencv_core");
	// println!("cargo:rerun-if-changed=build.rs");
	// println!("cargo:rerun-if-changed=path/to/Cargo.lock");

	println!("cargo:rerun-if-changed=src/teleC.cpp");
	cc::Build::new().file("src/teleC.cpp").file("src/nexstar.c").include("src/libnexstar/src").compile("teleC");
	let bindings = bindgen::Builder::default()
		.header("src/teleC.cpp")
		.parse_callbacks(Box::new(bindgen::CargoCallbacks))
		.generate()
		.expect("Unable to generate bindings");

	let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
	bindings
		.write_to_file(out_path.join("bindings.rs"))
		.expect("Couldn't write bindings!");
}
