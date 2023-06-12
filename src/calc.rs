use crate::tele::AltAz;
const SEMI_MAJOR_AXIS: f64 = 6378137.0; // Equatorial radius of WGS84 ellipsoid in meters
const SEMI_MINOR_AXIS: f64 = 6356752.314245; // Polar radius of WGS84 ellipsoid in meters

pub fn calculate_angles(
	latitude1: f64,
	longitude1: f64,
	altitude1: f64,
	latitude2: f64,
	longitude2: f64,
	altitude2: f64,
) -> AltAz {
	// Convert degrees to radians
	let latitude1_rad = latitude1.to_radians();
	let longitude1_rad = longitude1.to_radians();
	let latitude2_rad = latitude2.to_radians();
	let longitude2_rad = longitude2.to_radians();

	// Calculate differences in longitudes and latitudes
	let delta_longitude = longitude2_rad - longitude1_rad;
	let delta_latitude = latitude2_rad - latitude1_rad;

	// Calculate the average latitude
	let avg_latitude = (latitude1_rad + latitude2_rad) / 2.0;

	// Calculate the prime vertical radius of curvature
	let prime_vertical_radius = SEMI_MAJOR_AXIS
		/ (1.0
			- ((SEMI_MINOR_AXIS / SEMI_MAJOR_AXIS)
				* avg_latitude.sin().powi(2))
			.sqrt());

	// Calculate the north component and east component of the azimuth vector
	let north_component =
		delta_latitude.sin() * prime_vertical_radius * avg_latitude.cos();
	let east_component = delta_longitude.sin()
		* prime_vertical_radius
		* avg_latitude.sin().cos();

	// Calculate the azimuth angle
	let theta = east_component.atan2(north_component);

	// Calculate altitude angle
	let altitude = (altitude2 - altitude1).atan2(calculate_distance(
		latitude1_rad,
		longitude1_rad,
		latitude2_rad,
		longitude2_rad,
	));

	AltAz {
		azm: theta.to_degrees(),
		alt: altitude.to_degrees(),
	}
}

fn calculate_distance(
	latitude1: f64,
	longitude1: f64,
	latitude2: f64,
	longitude2: f64,
) -> f64 {
	let delta_latitude = latitude2 - latitude1;
	let delta_longitude = longitude2 - longitude1;

	let a = (delta_latitude / 2.0).sin().powi(2)
		+ latitude1.sin()
			* latitude2.sin()
			* (delta_longitude / 2.0).sin().powi(2);

	let c = 2.0 * a.sqrt().atan2((1.0 - a).sqrt());

	const R: f64 = SEMI_MAJOR_AXIS; // Approximating distance using equatorial radius

	R * c
}
