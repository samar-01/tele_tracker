#include <nexstar.h>
#include <stdio.h>

int dev;
void init(){
	dev = open_telescope("/dev/ttyUSB0");
	puts("Initialized");

	unsigned char major;
	unsigned char minor;
	int ver = tc_get_version(dev,(char*) &major,(char*) &minor);
	printf("Version %d.%d 0x%x\n", major, minor, ver);

	double lon,lat;
	int r= tc_get_location(dev, &lon, &lat);
	printf("lon = %f, lat = %f, res = %d\n", lon, lat, r);

	time_t tm;
	time(&tm);
	printf("tc_set_time() = %d\n",tc_set_time(dev, tm, -5, 0));
	time_t ttime;
	int tz;
	int dst;
	tc_get_time(dev, &ttime, &tz, &dst);
	printf("time = %stz = %d, dst = %d\n", ctime(&ttime), tz, dst);

}

void print_model(){
	int mountno= tc_get_model(dev); 
	char nex[100];
	get_model_name(mountno,nex,100);
	printf("Mount id=%d name=%s\n", mountno, nex);
}

void rotate(float xspeed, float yspeed){
	if (xspeed > 1){
		xspeed = 1;
	}
	if (xspeed < -1){
		xspeed = -1;
	}
	if (yspeed > 1){
		yspeed = 1;
	}
	if (yspeed < -1){
		yspeed = -1;
	}
	xspeed *= 8191.8;
	yspeed *= 8191.8;
	tc_slew_variable(dev, TC_AXIS_AZM, xspeed>0, xspeed);
	tc_slew_variable(dev, TC_AXIS_ALT, yspeed>0, yspeed);
}

void stop(){
	tc_slew_variable(dev, TC_AXIS_AZM, 0, 0);
	tc_slew_variable(dev, TC_AXIS_ALT, 0, 0);
}