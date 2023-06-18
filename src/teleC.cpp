#include <nexstar.h>
#include <stdio.h>
#include <cstdint>
// #include <stdfloat>
// using namespace std;

int8_t dev;
int8_t init() {
	dev = open_telescope("/dev/ttyUSB0");
	enforce_protocol_version(dev, VER_AUTO);
	if (dev == -1) {
		return false;
	}
	printf("Initialized at port %d\n", dev);

	unsigned char major;
	unsigned char minor;
	int ver = tc_get_version(dev,(char*) &major,(char*) &minor);
	printf("Version %d.%d 0x%x\n", major, minor, ver);

	time_t tm;
	time(&tm);
	// printf("%d\n", tm);
	// printf("tc_set_time() = %d\n",tc_set_time(dev, tm, -5, 0));
	// tc_set_time(dev, tm, -5, 1);
	tc_set_time(dev, tm, 0, 0);
	time_t ttime;
	int tz;
	int dst;
	tc_get_time(dev, &ttime, &tz, &dst);
	printf("time = %stz = %d, dst = %d\n", ctime(&ttime), tz, dst);

	return dev;
}

void print_model() {
	int mountno = tc_get_model(dev);
	char nex[100];
	get_model_name(mountno, nex, 100);
	printf("Mount id=%d name=%s\n", mountno, nex);
}

void rotate(float xspeed, float yspeed) {
	if (xspeed > 1) {
		xspeed = 1;
	}
	if (xspeed < -1) {
		xspeed = -1;
	}
	if (yspeed > 1) {
		yspeed = 1;
	}
	if (yspeed < -1) {
		yspeed = -1;
	}
	xspeed *= 8191.8;
	yspeed *= 8191.8;
	tc_slew_variable(dev, TC_AXIS_AZM, xspeed > 0, xspeed);
	tc_slew_variable(dev, TC_AXIS_ALT, yspeed > 0, yspeed);
}

void stop() {
	tc_slew_variable(dev, TC_AXIS_AZM, 0, 0);
	tc_slew_variable(dev, TC_AXIS_ALT, 0, 0);
}

bool is_aligned() {
	return tc_check_align(dev);
}

int8_t set_loc(double lat, double lon){
	return tc_set_location(dev, lon, lat);
}

void get_loc(){
	double lon, lat;
	tc_get_location(dev, &lon, &lat);
	printf("%f, %f\n", lon, lat);
}

typedef struct AltAzm {
	double alt;
	double azm;
} AltAzm;

AltAzm get_alt_az(){
	AltAzm a;
	tc_get_azalt_p(dev, &a.azm, &a.alt);
	return a;
}

void goto_alt_az(AltAzm a){
	if (a.azm < 0) {
		a.azm += 360;
	}
	tc_goto_azalt_p(dev, a.azm, a.alt);
}