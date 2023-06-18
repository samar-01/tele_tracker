use crate::teleBind::{self, *};
use pid::Pid;
use std::sync::{Arc, Mutex};
use std::{thread, time::Duration};

pub struct Telescope {
	dev: i8,
	kp: f64,
	ki: f64,
	kd: f64,
	// pid: Pid<f64>,
}
impl Telescope {
	pub fn new_blank() -> Telescope {
		Telescope {
			dev: 0,
			kp: 0.0,
			ki: 0.0,
			kd: 0.0,
		}
	}
	pub fn new(kp: f64, ki: f64, kd: f64) -> Telescope {
		Telescope {
			dev: 0,
			kp: kp,
			ki: ki,
			kd: kd,
			// pid: Pid::new(0, 0),
		}
		// a.pid.p(kp, 100.0);
		// a.pid.i(ki, 100.0);
		// a.pid.d(kd, 100.0);
		// a
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

	pub fn pid_goto(&mut self, a: AltAzm) {
		
		// let mut alttar = a.alt;
		// let mut altpid = Pid::new(alttar, 1.0);
		// init_pid(&mut altpid, self.kp, self.ki, self.kd);

		// let foo = Arc::new(Mutex::new(altpid));
		
		// let inputpid = foo.clone();
		// let handle = thread::spawn(move || loop {
		// 	let x: f64 = super::input().parse().unwrap();
		// 	// inputpid.setpoint(x);
		// 	// inputpid = x.into();
		// 	inputpid.lock().unwrap().setpoint(x);
		// });
		
		// let targetthing = foo.clone();
		// loop {
		// 	// rotate(0.0, );
		// 	// alttar = 0.0;
		// 	let alt = self.get_alt_az().alt;
		// 	dbg!(alt);
		// 	// dbg!(&targetthing);
		// 	let yspeed = targetthing.lock().unwrap().next_control_output(alt).output;
		// 	dbg!(yspeed);
		// 	self.rotate(0.0, yspeed);
		// }
		// self.stop();
		// handle.join().unwrap();

		let mut alttar = a.azm;
		let mut altpid = Pid::new(alttar, 1.0);
		init_pid(&mut altpid, self.kp, self.ki, self.kd);

		let foo = Arc::new(Mutex::new(altpid));
		
		let inputpid = foo.clone();
		let handle = thread::spawn(move || loop {
			let x: f64 = super::input().parse().unwrap();
			// inputpid.setpoint(x);
			// inputpid = x.into();
			inputpid.lock().unwrap().setpoint(x);
		});
		
		let targetthing = foo.clone();
		loop {
			// rotate(0.0, );
			// alttar = 0.0;
			let mut alt = self.get_alt_az().azm;
			// dbg!(alt);
			
			// dbg!(&targetthing);
			let yspeed = targetthing.lock().unwrap().next_control_output(alt).output;
			dbg!(yspeed);
			self.rotate(yspeed, 0.0);
		}
		self.stop();
		handle.join().unwrap();
	}

	fn rotate(&self, xpeed: f64, yspeed: f64) {
		unsafe { teleBind::rotate(xpeed, yspeed) };
	}
}

fn init_pid(pid: &mut Pid<f64>, kp: f64, ki: f64, kd: f64) {
	pid.p(kp, 1.0);
	pid.i(ki, 0.0);
	pid.d(kd, 1.0);
}
