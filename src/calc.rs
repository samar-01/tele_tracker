use crate::teleBind::AltAzm;
use nalgebra::Vector3;
use std::f64::consts::PI;
static EARTH_RADIUS: f64 = 6371000.0;

pub fn calculate_angles(
	lat1: f64,
	lon1: f64,
	alt1: f64,
	lat2: f64,
	lon2: f64,
	alt2: f64,
) -> AltAzm {
	let alt = calculate_alt(lat1, lon1, alt1, lat2, lon2, alt2);
	let azm = calculate_azm(lat1, lon1, lat2, lon2);
	AltAzm { alt, azm }
}

pub fn print_visible(
	lat1: f64,
	lon1: f64,
	alt1: f64,
	lat2: f64,
	lon2: f64,
	alt2: f64,
) {
	let alt = calculate_alt(lat1, lon1, alt1, lat2, lon2, alt2);
	if visible(alt1, alt) {
		println!("Visbile");
	} else {
		println!("Invisible");
	}
}

fn calculate_azm(lat1: f64, lon1: f64, lat2: f64, lon2: f64) -> f64 {
	let lat1_rad = lat1.to_radians();
	let lon1_rad = lon1.to_radians();
	let lat2_rad = lat2.to_radians();
	let lon2_rad = lon2.to_radians();

	let delta_lon = lon2_rad - lon1_rad;

	let bearing = (delta_lon.sin() * lat2_rad.cos()).atan2(
		lat1_rad.cos() * lat2_rad.sin()
			- lat1_rad.sin() * lat2_rad.cos() * delta_lon.cos(),
	);

	let initial_bearing = bearing.to_degrees();

	(initial_bearing + 360.0) % 360.0
}

fn calculate_normal_vector(lat: f64, lon: f64) -> Vector3<f64> {
	let lat_rad = lat.to_radians();
	let lon_rad = lon.to_radians();

	let x = lat_rad.cos() * lon_rad.cos();
	let y = lat_rad.cos() * lon_rad.sin();
	let z = lat_rad.sin();

	Vector3::new(x, y, z)
}

fn calculate_alt(
	lat1: f64,
	lon1: f64,
	alt1: f64,
	lat2: f64,
	lon2: f64,
	alt2: f64,
) -> f64 {
	let pos1 =
		spherical_to_cartesian(EARTH_RADIUS + alt1, lon1 + 180.0, lat1 - 90.0);
	let pos2 =
		spherical_to_cartesian(EARTH_RADIUS + alt2, lon2 + 180.0, lat2 - 90.0);

	let dif = pos2 - pos1;
	let distance = dif.norm();
	let dir = dif / distance;
	let normal = calculate_normal_vector(lat1, lon1);

	90.0 - (normal.dot(&dir) / (normal.norm() * dir.norm())).acos() * 180.0 / PI
}

fn spherical_to_cartesian(radius: f64, theta: f64, phi: f64) -> Vector3<f64> {
	let phi_rad = phi.to_radians();
	let theta_rad = theta.to_radians();

	let x = radius * phi_rad.sin() * theta_rad.cos();
	let y = radius * phi_rad.sin() * theta_rad.sin();
	let z = radius * phi_rad.cos();

	Vector3::new(x, y, z)
}

fn visible(alt1: f64, alt: f64) -> bool {
	horizon_angle(alt1) < alt
}

fn horizon_angle(alt1: f64) -> f64 {
	-(EARTH_RADIUS / (EARTH_RADIUS + alt1)).acos() * 180.0 / PI
}

#[cfg(test)]
mod tests {
	use super::*;
	use approx::relative_eq;
	#[test]
	fn straight() {
		assert_eq!(0.0, calculate_azm(0.0, 0.0, 0.0, 0.0));
		assert_eq!(0.0, calculate_azm(0.0, 0.0, 1.0, 0.0));
		assert_eq!(180.0, calculate_azm(0.0, 0.0, -1.0, 0.0));
		assert_eq!(90.0, calculate_azm(0.0, 0.0, 0.0, 1.0));
		assert_eq!(270.0, calculate_azm(0.0, 0.0, 0.0, -1.0));
		assert_eq!(270.0, calculate_azm(0.0, 0.0, 0.0, 355.0));

		assert_eq!(90.0, calculate_azm(0.0, 0.0, 0.0, 179.0));
	}
	#[test]
	fn diag() {
		let dir = calculate_azm(0.0, 0.0, 1.0, 1.0);
		assert!((dir - 45.0).abs() < 0.01);
	}

	#[test]
	fn normal() {
		assert!(relative_eq!(
			Vector3::new(1.0, 0.0, 0.0),
			calculate_normal_vector(0.0, 0.0)
		));
		assert!(relative_eq!(
			Vector3::new(-1.0, 0.0, 0.0),
			calculate_normal_vector(0.0, 180.0)
		));
		assert!(relative_eq!(
			Vector3::new(0.0, 1.0, 0.0),
			calculate_normal_vector(0.0, 90.0)
		));
		assert!(relative_eq!(
			Vector3::new(0.0, -1.0, 0.0),
			calculate_normal_vector(0.0, 270.0)
		));
		assert!(relative_eq!(
			Vector3::new(0.0, 0.0, 1.0),
			calculate_normal_vector(90.0, 0.0)
		));
		assert!(relative_eq!(
			Vector3::new(0.0, 0.0, -1.0),
			calculate_normal_vector(-90.0, 0.0)
		));
	}

	#[test]
	fn s2c() {
		assert!(relative_eq!(
			Vector3::new(0.0, 0.0, 0.0),
			spherical_to_cartesian(0.0, 0.0, 0.0)
		));
		assert!(relative_eq!(
			Vector3::new(0.0, 0.0, 1.0),
			spherical_to_cartesian(1.0, 0.0, 0.0)
		));
		assert!(relative_eq!(
			Vector3::new(0.0, 0.0, 123.0),
			spherical_to_cartesian(123.0, 0.0, 0.0)
		));
		assert!(relative_eq!(
			// assert_eq!(
			Vector3::new(1.0, 0.0, 0.0),
			spherical_to_cartesian(1.0, 0.0, 90.0)
		));
		assert!(relative_eq!(
			Vector3::new(0.0, 1.0, 0.0),
			spherical_to_cartesian(1.0, 90.0, 90.0)
		));
		assert!(relative_eq!(
			Vector3::new(2.0_f64.sqrt() / 2.0, 0.0, 2.0_f64.sqrt() / 2.0),
			spherical_to_cartesian(1.0, 0.0, 45.0)
		));
		assert!(relative_eq!(
			Vector3::new(2.0_f64.sqrt() / 2.0, 2.0_f64.sqrt() / 2.0, 0.0),
			spherical_to_cartesian(1.0, 45.0, 90.0)
		));
	}
}
