use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::thread;

pub fn rotctl() {
	let host = "127.0.0.1"; // Change to the IP you want to listen on
	let port = 4533; // Change to your desired port
	let rotator_server = rotctlServer::new(host, port);
	if let Err(e) = rotator_server.start() {
		eprintln!("Error: {}", e);
	}
}

struct rotctlServer {
	host: String,
	port: u16,
}

impl rotctlServer {
	fn new(host: &str, port: u16) -> Self {
		rotctlServer {
			host: host.to_string(),
			port,
		}
	}

	fn handle_client(&self, mut stream: TcpStream, address: std::net::SocketAddr) {
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
					Some('p') => "0.000000\n0.000000\n",
					Some('S') => "RPRT 0\n",
					Some('P') => {
						println!("New position");
						let dat = data.split(' ').collect::<Vec<&str>>();
						let azm: f32 = dat.get(1).unwrap().parse().unwrap();
						let alt: f32 = dat.get(2).unwrap().parse().unwrap();
						dbg!(azm);
						dbg!(alt);
						"RPRT 0\n"
					}
					_ => "",
				};

				if let Err(_) = stream.write(response.as_bytes()) {
					break;
				}
			}
		}

		println!("Connection from {} closed.", address);
	}

	fn start(&self) -> std::io::Result<()> {
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
