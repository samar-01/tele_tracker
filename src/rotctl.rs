pub use crate::tele::Telescope;
use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::rc::Rc;
use std::sync::{Arc, Mutex};
use std::thread;

use crate::teleBind::{goto_alt_az, goto_alt_az_custom, AltAzm};

// use self::tele::Telescope;

pub fn rotctl(tel: Telescope) {
	let host = "127.0.0.1"; // Change to the IP you want to listen on
	let port = 4533; // Change to your desired port
	let tel = Arc::new(Mutex::new(tel));
	let target = Arc::new(Mutex::new(AltAzm { alt: 0.0, azm: 0.0 }));
	let mut handles = vec![];
	{
		let tel = Arc::clone(&tel);
		let target = Arc::clone(&target);
		let h1 = thread::spawn(move || {
			run_tele(tel, target);
			// let x = target.lock().unwrap();
		});
		handles.push(h1);
	}
	{
		let tel = Arc::clone(&tel);
		let target = Arc::clone(&target);
		let h2 = thread::spawn(move || {
			let mut rotator_server = rotctlServer::new(host, port, tel, target);
			if let Err(e) = rotator_server.start() {
				eprintln!("Error: {}", e);
			}
		});
		handles.push(h2);
	}

	for handle in handles {
		handle.join().unwrap();
	}
}

fn run_tele(tel: Arc<Mutex<Telescope>>, target: Arc<Mutex<AltAzm>>) {
	loop {
		dbg!(&target);
		std::thread::sleep(std::time::Duration::from_secs(1));
	}
}

struct rotctlServer {
	host: String,
	port: u16,
	tel: Arc<Mutex<Telescope>>,
	target: Arc<Mutex<AltAzm>>,
}

impl rotctlServer {
	fn new(host: &str, port: u16, tel: Arc<Mutex<Telescope>>, target: Arc<Mutex<AltAzm>>) -> Self {
		rotctlServer {
			host: host.to_string(),
			port,
			tel,
			target, // target: AltAzm { alt: 0.0, azm: 0.0 },
		}
	}

	fn handle_client(&mut self, mut stream: TcpStream, address: std::net::SocketAddr) {
		println!("Connection from {} established.", address);

		let mut buffer = [0; 1024];
		while let Ok(n) = stream.read(&mut buffer) {
			if n == 0 {
				break;
			}
			if let Ok(data) = std::str::from_utf8(&buffer[..n]) {
				let data = data.trim();
				// println!("Received command: {}", data);

				let response = match data.chars().next() {
					Some('p') => {
						println!("");
						let current = self.tel.lock().unwrap().get_alt_az();
						format!("{}\n{}\n", current.azm, current.alt)
					}
					Some('S') => "RPRT 0\n".to_string(),
					Some('P') => {
						println!("New position");
						let dat = data.split(' ').collect::<Vec<&str>>();
						let azm: f64 = dat.get(1).unwrap().parse().unwrap();
						let alt: f64 = dat.get(2).unwrap().parse().unwrap();
						let target = AltAzm { alt, azm };
						// self.target.alt = alt;
						// self.target.azm = azm;
						// dbg!(self.target);
						// let x= self.target.alt;
						// Mutex::write(self.target, target);
						self.target.lock().unwrap().alt = alt;
						self.target.lock().unwrap().azm = azm;
						"RPRT 0\n".to_string()
					}
					_ => "".to_string(),
				};

				if let Err(_) = stream.write(response.as_str().as_bytes()) {
					break;
				}
			}
		}

		println!("Connection from {} closed.", address);
	}

	fn start(&mut self) -> std::io::Result<()> {
		let listener = TcpListener::bind(format!("{}:{}", self.host, self.port))?;
		println!("Rotator server listening on {}:{}...", self.host, self.port);

		for stream in listener.incoming() {
			match stream {
				Ok(stream) => {
					let addr = stream.peer_addr().unwrap();
					self.handle_client(stream, addr);
				}
				Err(e) => {
					eprintln!("Error: {}", e);
				}
			}
		}
		Ok(())
	}
}
