/**************************************************************
	Celestron NexStar compatible telescope control library

	(C)2013-2016 by Rumen G.Bogdanovski
***************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**************************************************************
	ASTRONOMICAL ROUTINE LIBRARY

	ASCII to double and double to ASCII
	conversion routines prototypes.

	(C)2000 by Rumen G.Bogdanovski
***************************************************************/
#if !defined(__DEG2STR_H)
#define __DEG2STR_H

#define Err_NOERR  0
#define Err_INVSTR 1
#define Err_OUTOFR 2

//uncoment one of those to get needed hour and angle formats  
//#define HUNDRETHS				// XX:XX:XX.XX
#define TENTHS					// XX:XX:XX.X
//#define SECONDS				// XX:XX:XX

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
	#endif

	int a2dd(double* dd, const char* a);
	char* dd2a(double a, int plus);

	int a2dh(double* dh, const char* a);
	char* dh2a(double h);

	void dd2dms(double ang, unsigned char* deg, unsigned char* min, unsigned char* sec, char* sign);

	#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif  /* __DEG2STR_H */

/**************************************************************
	Celestron NexStar compatible telescope control library

	(C)2013-2016 by Rumen G.Bogdanovski
***************************************************************/
#if !defined(__NEXSTAR_H)
#define __NEXSTAR_H

#define NX_TC_TRACK_OFF      0
#define NX_TC_TRACK_ALT_AZ   1
#define NX_TC_TRACK_EQ_NORTH 2
#define NX_TC_TRACK_EQ_SOUTH 3

#define SW_TC_TRACK_OFF      0
#define SW_TC_TRACK_ALT_AZ   1
#define SW_TC_TRACK_EQ       2
#define SW_TC_TRACK_EQ_PEC   3

#define TC_TRACK_OFF         0
#define TC_TRACK_ALT_AZ      1
#define TC_TRACK_EQ_NORTH    2
#define TC_TRACK_EQ_SOUTH    3
#define TC_TRACK_EQ          4
#define TC_TRACK_EQ_PEC      5

#define TC_DIR_POSITIVE 1
#define TC_DIR_NEGATIVE 0

#define TC_AXIS_RA_AZM 1
#define TC_AXIS_DE_ALT 0

#define TC_AXIS_RA TC_AXIS_RA_AZM
#define TC_AXIS_DE TC_AXIS_DE_ALT

#define TC_AXIS_AZM TC_AXIS_RA_AZM
#define TC_AXIS_ALT TC_AXIS_DE_ALT

#define _TC_DIR_POSITIVE 6
#define _TC_DIR_NEGATIVE 7

#define _TC_AXIS_RA_AZM 16
#define _TC_AXIS_DE_ALT 17

/* return codes */
#define RC_OK            0	/* success */
#define RC_FAILED      (-1)	/* general error */
#define RC_PARAMS      (-2)	/* invalid parameters */
#define RC_DEVICE      (-3)	/* no responce from the device */
#define RC_DATA        (-4)	/* invalid data */
#define RC_UNSUPPORTED (-5) /* Unsupported command */

#define DEG2RAD (3.1415926535897932384626433832795/180.0)
#define RAD2DEG (180.0/3.1415926535897932384626433832795)

/* Supported NexStar protocol versions */
#define VER_1_2  0x10200
#define VER_1_6  0x10600
#define VER_2_2  0x20200
#define VER_2_3  0x20300
#define VER_3_1  0x30100
#define VER_4_10 0x40A00
#define VER_3_37_8 0x32508
#define VER_4_37_8 0x42508
/* All protocol versions */
#define VER_AUX  0xFFFFFF
#define VER_AUTO 0x0

#define VNDR_CELESTRON  0x1
#define VNDR_SKYWATCHER 0x2
#define VNDR_ALL_SUPPORTED (VNDR_CELESTRON | VNDR_SKYWATCHER)

/* There is no way to tell SkyWatcher from Celestron. Unfortunately both share the
   same IDs and some Celestron mounts have RTC wile SW does not. That is why the user
   should decide. Set nexstar_use_rtc to 1 to enable using the RTC on the mounts that have RTC. */
extern int nexstar_use_rtc;

extern int nexstar_proto_version;
/* version check macros */
#define RELEASE_MASK  0xFF0000
#define REVISION_MASK 0x00FF00
#define PATCH_MASK    0x0000FF

#define GET_RELEASE(ver)  ((ver & RELEASE_MASK)>>16)
#define GET_REVISION(ver) ((ver & REVISION_MASK)>>8)
#define GET_PATCH(ver)    (ver & PATCH_MASK)

#define REQUIRE_VER(req_ver)      { if(req_ver > nexstar_proto_version) return RC_UNSUPPORTED; }
#define REQUIRE_RELEASE(req_ver)  { if ((req_ver) > GET_RELEASE(nexstar_proto_version)) return RC_UNSUPPORTED; }
#define REQUIRE_REVISION(req_ver) { if ((req_ver) > GET_REVISION(nexstar_proto_version)) return RC_UNSUPPORTED; }
#define REQUIRE_PATCH(req_ver)    { if ((req_ver) > GET_PATCH(nexstar_proto_version)) return RC_UNSUPPORTED; }

extern int nexstar_mount_vendor;
/* vendor check macros */
#define REQUIRE_VENDOR(req_vendor) { if(!(nexstar_mount_vendor & req_vendor)) return RC_UNSUPPORTED; }
#define VENDOR(vendor) (nexstar_mount_vendor & vendor)
#define VENDOR_IS(vendor) (nexstar_mount_vendor == vendor)


#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
	#endif

	/* Telescope communication */
	int open_telescope(char* dev_file);
	int close_telescope(int dev_fd);
	int enforce_protocol_version(int devfd, int ver);
	#define write_telescope(dev_fd, buf, size) (write(dev_fd, buf, size))

	int _read_telescope(int devfd, char* reply, int len, char vl);
	#define read_telescope(devfd, reply, len) (_read_telescope(devfd, reply, len, 0))
	#define read_telescope_vl(devfd, reply, len) (_read_telescope(devfd, reply, len, 1))

	int guess_mount_vendor(int dev);
	int enforce_vendor_protocol(int vendor);

	/* Telescope commands */
	int tc_check_align(int dev);
	int tc_get_orientation(int dev);
	int tc_goto_in_progress(int dev);
	int tc_goto_cancel(int dev);
	int tc_echo(int dev, char ch);
	int tc_get_model(int dev);
	int tc_get_version(int dev, char* major, char* minor);
	int tc_get_tracking_mode(int dev);
	int tc_set_tracking_mode(int dev, char mode);

	int _tc_get_rade(int dev, double* ra, double* de, char precise);
	#define tc_get_rade(dev, ra, de) (_tc_get_rade(dev, ra, de, 0))
	#define tc_get_rade_p(dev, ra, de) (_tc_get_rade(dev, ra, de, 1))

	int _tc_get_azalt(int dev, double* az, double* alt, char precise);
	#define tc_get_azalt(dev, az, alt) (_tc_get_azalt(dev, az, alt, 0))
	#define tc_get_azalt_p(dev, az, alt) (_tc_get_azalt(dev, az, alt, 1))

	int _tc_goto_rade(int dev, double ra, double de, char precise);
	#define tc_goto_rade(dev, ra, de) (_tc_goto_rade(dev, ra, de, 0))
	#define tc_goto_rade_p(dev, ra, de) (_tc_goto_rade(dev, ra, de, 1))

	int _tc_goto_azalt(int dev, double az, double alt, char precise);
	#define tc_goto_azalt(dev, az, alt) (_tc_goto_azalt(dev, az, alt, 0))
	#define tc_goto_azalt_p(dev, az, alt) (_tc_goto_azalt(dev, az, alt, 1))

	int _tc_sync_rade(int dev, double ra, double de, char precise);
	#define tc_sync_rade(dev, ra, de) (_tc_sync_rade(dev, ra, de, 0))
	#define tc_sync_rade_p(dev, ra, de) (_tc_sync_rade(dev, ra, de, 1))

	int tc_get_location(int dev, double* lon, double* lat);
	int tc_set_location(int dev, double lon, double lat);

	time_t tc_get_time(int dev, time_t* ttime, int* tz, int* dst);
	int tc_set_time(char dev, time_t ttime, int tz, int dst);

	int tc_slew_fixed(int dev, char axis, char direction, char rate);
	int tc_slew_variable(int dev, char axis, char direction, float rate);

	char* get_model_name(int id, char* name, int len);

	/* Reverse engineered commands */
	/*
	int tc_get_guide_rate();
	int tc_set_guide_rate_fixed();
	int tc_set_guide_rate();
	*/
	int tc_get_autoguide_rate(int dev, char axis);
	int tc_set_autoguide_rate(int dev, char axis, char rate);
	int tc_get_backlash(int dev, char axis, char direction);
	int tc_set_backlash(int dev, char axis, char direction, char backlash);
	int tc_pass_through_cmd(int dev, char msg_len, char dest_id, char cmd_id,
		char data1, char data2, char data3, char res_len, char* response);
	/* End of reverse engineered commands */

	/* nextar turns <=> degrees conversion */
	int pnex2dd(char* nex, double* d1, double* d2);
	int nex2dd(char* nex, double* d1, double* d2);
	int dd2nex(double d1, double d2, char* nex);
	int dd2pnex(double d1, double d2, char* nex);

	#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif /* __NEXSTAR_H */
/**************************************************************
	Celestron NexStar compatible telescope control library

	(C)2013-2016 by Rumen G.Bogdanovski
***************************************************************/
#if !defined(__NEX_OPEN_H)
#define __NEX_OPEN_H

#define DEFAULT_PORT 9999

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
	#endif

	int parse_devname(char* device, char* host, int* port);
	int open_telescope_rs(char* dev_file);
	int open_telescope_tcp(char* host, int port);

	#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif /* __NEX_OPEN_H */


/* do not check protocol version by default */
int nexstar_proto_version = VER_AUX;

int nexstar_mount_vendor = VNDR_ALL_SUPPORTED;

/* do not use RTC by default */
int nexstar_use_rtc = 0;

/*****************************************************
 Telescope communication
 *****************************************************/
int open_telescope(char* dev_file) {
	int port, dev_fd;
	char host[255];

	if (parse_devname(dev_file, host, &port)) {
		/* this is network address */
		dev_fd = open_telescope_tcp(host, port);
	} else {
		/* should be tty port */
		dev_fd = open_telescope_rs(dev_file);
	}
	if (dev_fd < 0) return dev_fd;

	nexstar_mount_vendor = guess_mount_vendor(dev_fd);
	if (nexstar_mount_vendor < 0) {
		close_telescope(dev_fd);
		return RC_FAILED;
	}

	return dev_fd;
}

int close_telescope(int devfd) {
	return close(devfd);
}

int enforce_protocol_version(int devfd, int ver) {
	int version;
	if (ver != VER_AUTO) {
		nexstar_proto_version = ver;
		return RC_OK;
	}

	version = tc_get_version(devfd, NULL, NULL);
	if (version < 0) return version;
	nexstar_proto_version = version;
	return RC_OK;
}

int _read_telescope(int devfd, char* reply, int len, char fl) {
	char c;
	int res;
	int count = 0;

	while ((count < len) && ((res = read(devfd, &c, 1)) != -1)) {
		if (res == 1) {
			reply[count] = c;
			count++;
			if ((fl) && (c == '#')) return count;
			//printf("R: %d, C:%d\n", (unsigned char)reply[count-1], count);
		} else {
			return RC_FAILED;
		}
	}
	if (c == '#') {
		return count;
	} else {
		/* if the last byte is not '#', this means that the device did
		   not respond and the next byte should be '#' (hopefully) */
		res = read(devfd, &c, 1);
		if ((res == 1) && (c == '#')) {
			//printf("%s(): RC_DEVICE\n",__FUNCTION__);
			return RC_DEVICE;
		}
	}
	return RC_FAILED;
}

int guess_mount_vendor(int dev) {
	char reply[7];
	int res;

	REQUIRE_VER(VER_1_2);

	if (write_telescope(dev, "V", 1) < 1) return RC_FAILED;

	res = read_telescope_vl(dev, reply, sizeof reply);
	if (res < 0) return RC_FAILED;

	if (res == 3) { /* Celestron */
		return VNDR_CELESTRON;
	} else if (res == 7) { /* SkyWatcher */
		return VNDR_SKYWATCHER;
	}
	return RC_FAILED;
}

int enforce_vendor_protocol(int vendor) {

	if (!(vendor & VNDR_ALL_SUPPORTED)) return RC_FAILED;
	nexstar_mount_vendor = vendor;
	return vendor;
}

/*****************************************************
 Telescope commands
 *****************************************************/

int _tc_get_rade(int dev, double* ra, double* de, char precise) {
	char reply[18];

	if (precise) {
		REQUIRE_VER(VER_1_6);
		if (write_telescope(dev, "e", 1) < 1) return RC_FAILED;
	} else {
		REQUIRE_VER(VER_1_2);
		if (write_telescope(dev, "E", 1) < 1) return RC_FAILED;
	}

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	if (precise) pnex2dd(reply, ra, de);
	else nex2dd(reply, ra, de);

	return RC_OK;
}

int _tc_get_azalt(int dev, double* az, double* alt, char precise) {
	char reply[18];

	if (precise) {
		REQUIRE_VER(VER_2_2);
		if (write_telescope(dev, "z", 1) < 1) return RC_FAILED;
	} else {
		REQUIRE_VER(VER_1_2);
		if (write_telescope(dev, "Z", 1) < 1) return RC_FAILED;
	}

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	if (precise) pnex2dd(reply, az, alt);
	else nex2dd(reply, az, alt);

	return RC_OK;
}

int _tc_goto_rade(int dev, double ra, double de, char precise) {
	char nex[30];
	char reply;

	if ((ra < -0.1) || (ra > 360.1)) return RC_PARAMS;
	if ((de < -90.1) || (de > 90.1)) return RC_PARAMS;

	if (precise) {
		REQUIRE_VER(VER_1_6);
		nex[0] = 'r';
		dd2pnex(ra, de, nex + 1);
		if (write_telescope(dev, nex, 18) < 1) return RC_FAILED;
	} else {
		REQUIRE_VER(VER_1_2);
		nex[0] = 'R';
		dd2nex(ra, de, nex + 1);
		if (write_telescope(dev, nex, 10) < 1) return RC_FAILED;
	}

	if (read_telescope(dev, &reply, sizeof reply) < 0) return RC_FAILED;

	return RC_OK;
}

int _tc_goto_azalt(int dev, double az, double alt, char precise) {
	char nex[30];
	char reply;

	if ((az < -0.1) || (az > 360.1)) return RC_PARAMS;
	if ((alt < -90.1) || (alt > 90.1)) return RC_PARAMS;

	if (precise) {
		REQUIRE_VER(VER_2_2);
		nex[0] = 'b';
		dd2pnex(az, alt, nex + 1);
		if (write_telescope(dev, nex, 18) < 1) return RC_FAILED;
	} else {
		REQUIRE_VER(VER_1_2);
		nex[0] = 'B';
		dd2nex(az, alt, nex + 1);
		if (write_telescope(dev, nex, 10) < 1) return RC_FAILED;
	}

	if (read_telescope(dev, &reply, sizeof reply) < 0) return RC_FAILED;

	return RC_OK;
}

int _tc_sync_rade(int dev, double ra, double de, char precise) {
	char nex[20];
	char reply;

	if (VENDOR(VNDR_SKYWATCHER)) {
		REQUIRE_RELEASE(3);
		REQUIRE_REVISION(37);
	} else {
		REQUIRE_VER(VER_4_10);
	}

	if ((ra < 0) || (ra > 360)) return RC_PARAMS;
	if ((de < -90) || (de > 90)) return RC_PARAMS;

	if (precise) {
		nex[0] = 's';
		dd2pnex(ra, de, nex + 1);
		if (write_telescope(dev, nex, 18) < 1) return RC_FAILED;
	} else {
		nex[0] = 'S';
		dd2nex(ra, de, nex + 1);
		if (write_telescope(dev, nex, 10) < 1) return RC_FAILED;
	}

	if (read_telescope(dev, &reply, sizeof reply) < 0) return RC_FAILED;

	return RC_OK;
}

int tc_check_align(int dev) {
	char reply[2];

	REQUIRE_VER(VER_1_2);

	if (write_telescope(dev, "J", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	return reply[0];
}

int tc_get_orientation(int dev) {
	char reply[2];

	REQUIRE_VENDOR(VNDR_SKYWATCHER);
	REQUIRE_RELEASE(3);
	REQUIRE_REVISION(37);

	if (write_telescope(dev, "p", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	return reply[0];
}

int tc_goto_in_progress(int dev) {
	char reply[2];

	REQUIRE_VER(VER_1_2);

	if (write_telescope(dev, "L", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	if (reply[0] == '1') return 1;
	if (reply[0] == '0') return 0;

	return RC_FAILED;
}

int tc_goto_cancel(int dev) {
	char reply;

	REQUIRE_VER(VER_1_2);

	if (write_telescope(dev, "M", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, &reply, sizeof reply) < 0) return RC_FAILED;

	if (reply == '#') return RC_OK;

	return RC_FAILED;
}

int tc_echo(int dev, char ch) {
	char buf[2];

	REQUIRE_VER(VER_1_2);

	buf[0] = 'K';
	buf[1] = ch;
	if (write_telescope(dev, buf, sizeof buf) < 1) return RC_FAILED;

	if (read_telescope(dev, buf, sizeof buf) < 0) return RC_FAILED;

	return buf[0];
}

int tc_get_model(int dev) {
	char reply[2];

	REQUIRE_VER(VER_2_2);

	if (write_telescope(dev, "m", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	return reply[0];
}

int tc_get_version(int dev, char* major, char* minor) {
	char reply[7];
	int res;

	REQUIRE_VER(VER_1_2);

	if (write_telescope(dev, "V", 1) < 1) return RC_FAILED;

	res = read_telescope_vl(dev, reply, (sizeof reply));
	if (res < 0) return RC_FAILED;

	if (res == 3) { /* Celestron */
		if (major) *major = reply[0];
		if (minor) *minor = reply[1];
		return ((reply[0] << 16) + (reply[1] << 8));
	} else if (res == 7) { /* SkyWatcher */
		long maj, min, subv;
		reply[6] = '\0';
		subv = strtol(reply + 4, NULL, 16);
		reply[4] = '\0';
		min = strtol(reply + 2, NULL, 16);
		reply[2] = '\0';
		maj = strtol(reply, NULL, 16);
		if (major) *major = maj;
		if (minor) *minor = min;
		return ((maj << 16) + (min << 8) + subv);
	}
	return RC_FAILED;
}

int tc_get_tracking_mode(int dev) {
	char reply[2];

	REQUIRE_VER(VER_2_3);

	if (write_telescope(dev, "t", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;
	if (VENDOR_IS(VNDR_SKYWATCHER)) {
		switch (reply[0]) {
			case SW_TC_TRACK_OFF:      return TC_TRACK_OFF;
			case SW_TC_TRACK_ALT_AZ:   return TC_TRACK_ALT_AZ;
			case SW_TC_TRACK_EQ:       return TC_TRACK_EQ;
			case SW_TC_TRACK_EQ_PEC:   return TC_TRACK_EQ_PEC;
		}
	} else {
		switch (reply[0]) {
			case NX_TC_TRACK_OFF:      return TC_TRACK_OFF;
			case NX_TC_TRACK_ALT_AZ:   return TC_TRACK_ALT_AZ;
			case NX_TC_TRACK_EQ_NORTH: return TC_TRACK_EQ_NORTH;
			case NX_TC_TRACK_EQ_SOUTH: return TC_TRACK_EQ_SOUTH;
		}
	}

	return RC_FAILED;
}

int tc_set_tracking_mode(int dev, char mode) {
	char cmd[2];
	char res;
	char _mode = 0;
	double lat;

	REQUIRE_VER(VER_1_6);

	if (VENDOR_IS(VNDR_SKYWATCHER)) {
		switch (mode) {
			case TC_TRACK_OFF:
				_mode = SW_TC_TRACK_OFF;
				break;

			case TC_TRACK_ALT_AZ:
				_mode = SW_TC_TRACK_ALT_AZ;
				break;

			case TC_TRACK_EQ_NORTH:
			case TC_TRACK_EQ_SOUTH:
			case TC_TRACK_EQ:
				_mode = SW_TC_TRACK_EQ;
				break;

			case TC_TRACK_EQ_PEC:
				_mode = SW_TC_TRACK_EQ_PEC;
				break;

			default:
				return RC_PARAMS;
		}
	} else {
		switch (mode) {
			case TC_TRACK_OFF:
				_mode = NX_TC_TRACK_OFF;
				break;

			case TC_TRACK_ALT_AZ:
				_mode = NX_TC_TRACK_ALT_AZ;
				break;

			case TC_TRACK_EQ_NORTH:
				_mode = NX_TC_TRACK_EQ_NORTH;
				break;

			case TC_TRACK_EQ_SOUTH:
				_mode = NX_TC_TRACK_EQ_SOUTH;
				break;

			case TC_TRACK_EQ:
				res = tc_get_location(dev, NULL, &lat);
				if (res < 0) return res;
				if (lat < 0) /* Lat < 0 is South */
					_mode = NX_TC_TRACK_EQ_SOUTH;
				else
					_mode = NX_TC_TRACK_EQ_NORTH;
				break;

			case TC_TRACK_EQ_PEC:
				return RC_UNSUPPORTED;

			default:
				return RC_PARAMS;
		}
	}
	cmd[0] = 'T';
	cmd[1] = _mode;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	if (read_telescope(dev, &res, sizeof res) < 0) return RC_FAILED;

	return RC_OK;
}

int tc_slew_fixed(int dev, char axis, char direction, char rate) {
	char axis_id, cmd_id, res;

	if (VENDOR(VNDR_SKYWATCHER)) {
		REQUIRE_RELEASE(3);
		REQUIRE_REVISION(1);
	} else {
		REQUIRE_VER(VER_1_6);
	}

	if (axis > 0) axis_id = _TC_AXIS_RA_AZM;
	else axis_id = _TC_AXIS_DE_ALT;

	if (direction > 0) cmd_id = _TC_DIR_POSITIVE + 30;
	else cmd_id = _TC_DIR_NEGATIVE + 30;

	if ((rate < 0) || (rate > 9)) return RC_PARAMS;

	return tc_pass_through_cmd(dev, 2, axis_id, cmd_id, rate, 0, 0, 0, &res);
}

int tc_slew_variable(int dev, char axis, char direction, float rate) {
	char axis_id, cmd_id, res;

	if (VENDOR(VNDR_SKYWATCHER)) {
		REQUIRE_RELEASE(3);
		REQUIRE_REVISION(1);
	} else {
		REQUIRE_VER(VER_1_6);
	}

	if (axis > 0) axis_id = _TC_AXIS_RA_AZM;
	else axis_id = _TC_AXIS_DE_ALT;

	if (direction > 0) cmd_id = _TC_DIR_POSITIVE;
	else cmd_id = _TC_DIR_NEGATIVE;

	int16_t irate = (int)(4 * rate);
	unsigned char rateH = (char)(irate / 256);
	unsigned char rateL = (char)(irate % 256);
	//printf("rateH = %d, rateL = %d , irate = %d\n", rateH, rateL, irate);

	return tc_pass_through_cmd(dev, 3, axis_id, cmd_id, rateH, rateL, 0, 0, &res);
}

int tc_get_location(int dev, double* lon, double* lat) {
	unsigned char reply[9];

	REQUIRE_VER(VER_2_3);

	if (write_telescope(dev, "w", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, (char*)reply, sizeof reply) < 0) return RC_FAILED;

	if (lat) {
		*lat = (double)reply[0] + reply[1] / 60.0 + reply[2] / 3600.0;
		if (reply[3]) {
			*lat *= -1;
		}
	}

	if (lon) {
		*lon = (double)reply[4] + reply[5] / 60.0 + reply[6] / 3600.0;
		if (reply[7]) {
			*lon *= -1;
		}
	}
	return RC_OK;
}

int tc_set_location(int dev, double lon, double lat) {
	unsigned char cmd[9];
	char res;
	unsigned char deg, min, sec, sign;

	REQUIRE_VER(VER_2_3);

	cmd[0] = 'W';
	dd2dms(lat, &deg, &min, &sec, (char*)&sign);
	if (deg > 90) {
		return RC_PARAMS;
	}
	cmd[1] = deg;
	cmd[2] = min;
	cmd[3] = sec;
	cmd[4] = sign;

	dd2dms(lon, &deg, &min, &sec, (char*)&sign);
	if (deg > 180) {
		return RC_PARAMS;
	}
	cmd[5] = deg;
	cmd[6] = min;
	cmd[7] = sec;
	cmd[8] = sign;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	if (read_telescope(dev, &res, sizeof res) < 0) return RC_FAILED;

	return RC_OK;
}

time_t tc_get_time(int dev, time_t* ttime, int* tz, int* dst) {
	char reply[9];
	struct tm tms;

	REQUIRE_VER(VER_2_3);

	if (write_telescope(dev, "h", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	tms.tm_hour = reply[0];
	tms.tm_min = reply[1];
	tms.tm_sec = reply[2];
	tms.tm_mon = reply[3] - 1;
	tms.tm_mday = reply[4];
	tms.tm_year = 100 + reply[5];
	/* tms.tm_isdst = reply[7]; */
	/* set tm_isdst to -1 and leave mktime to set it,
	   but we use the one returned by the telescope
	   this way we avoid 1 hour error if dst is
	   not set correctly on the telescope.
	*/
	tms.tm_isdst = -1;
	*tz = (int)reply[6];
	if (*tz > 12) *tz -= 256;
	*dst = reply[7];
	tms.tm_gmtoff = (tms.tm_isdst + *tz) * 3600;
	*ttime = mktime(&tms);
	// printf("%s() %d %d:%d:%d %d %d\n", __FUNCTION__, *ttime, tms.tm_hour, tms.tm_min, tms.tm_sec, tms.tm_gmtoff, tms.tm_isdst);
	return *ttime;
}

int tc_set_time(char dev, time_t ttime, int tz, int dst) {
	unsigned char cmd[9];
	struct tm tms;
	char res;
	int model;
	int timezone;
	time_t ltime;

	REQUIRE_VER(VER_2_3);

	timezone = tz;
	if (tz < 0) tz += 256;

	if (dst) dst = 1;
	else dst = 0;

	/* calculate localtime stamp so that gmtime_r will return localtime */
	ltime = ttime + ((timezone + dst) * 3600);
	gmtime_r(&ltime, &tms);

	//printf("%s() %ld %d:%d:%d %d %d\n", __FUNCTION__, ttime, tms.tm_hour, tms.tm_min, tms.tm_sec, tms.tm_gmtoff, tms.tm_isdst);
	cmd[0] = 'H';
	cmd[1] = (unsigned char)tms.tm_hour;
	cmd[2] = (unsigned char)tms.tm_min;
	cmd[3] = (unsigned char)tms.tm_sec;
	cmd[4] = (unsigned char)(tms.tm_mon + 1);
	cmd[5] = (unsigned char)tms.tm_mday;
	// year is actual_year-1900
	// here is required actual_year-2000
	// so we need year-100
	cmd[6] = (unsigned char)(tms.tm_year - 100);
	cmd[7] = (unsigned char)tz;
	cmd[8] = (unsigned char)dst;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	if (read_telescope(dev, &res, sizeof res) < 0) return RC_FAILED;

	if (!nexstar_use_rtc) return RC_OK;

	model = tc_get_model(dev);
	if (model <= 0) return model;

	/* If the mount has RTC set date/time to RTC too */
	/* I only know CGE(5) and AdvancedVX(20) to have RTC */
	if ((model == 5) || (model == 20)) {
		gmtime_r(&ttime, &tms);

		/* set year */
		if (tc_pass_through_cmd(dev, 3, 178, 132,
			(unsigned char)((tms.tm_year + 1900) / 256),
			(unsigned char)((tms.tm_year + 1900) % 256),
			0, 0, &res)) {
			return RC_FAILED;
		}
		/* set month and day */
		if (tc_pass_through_cmd(dev, 3, 178, 131,
			(unsigned char)(tms.tm_mon + 1),
			(unsigned char)tms.tm_mday,
			0, 0, &res)) {
			return RC_FAILED;
		}
		/* set time */
		if (tc_pass_through_cmd(dev, 4, 178, 179,
			(unsigned char)tms.tm_hour,
			(unsigned char)tms.tm_min,
			(unsigned char)tms.tm_sec,
			0, &res)) {
			return RC_FAILED;
		}
	}
	return RC_OK;
}

char* get_model_name(int id, char* name, int len) {
	if (VENDOR(VNDR_CELESTRON)) {
		switch (id) {
			case 1:
				strncpy(name, "NexStar GPS Series", len);
				return name;
			case 3:
				strncpy(name, "NexStar i-Series", len);
				return name;
			case 4:
				strncpy(name, "NexStar i-Series SE", len);
				return name;
			case 5:
				strncpy(name, "CGE", len);
				return name;
			case 6:
				strncpy(name, "Advanced GT", len);
				return name;
			case 7:
				strncpy(name, "SLT", len);
				return name;
			case 9:
				strncpy(name, "CPC", len);
				return name;
			case 10:
				strncpy(name, "GT", len);
				return name;
			case 11:
				strncpy(name, "NexStar 4/5 SE", len);
				return name;
			case 12:
				strncpy(name, "NexStar 6/8 SE", len);
				return name;
			case 14:
				strncpy(name, "CGEM", len);
				return name;
			case 20:
				strncpy(name, "Advanced VX", len);
				return name;
			case 22:
				strncpy(name, "Nexstar Evolution", len);
				return name;
			default:
				name[0] = '\0';
				return NULL;
		}
	} else if (VENDOR(VNDR_SKYWATCHER)) {
		switch (id) {
			case 0:
				strncpy(name, "EQ6 Series", len);
				return name;
			case 1:
				strncpy(name, "HEQ5 Series", len);
				return name;
			case 2:
				strncpy(name, "EQ5 Series", len);
				return name;
			case 3:
				strncpy(name, "EQ3 Series", len);
				return name;
			case 4:
				strncpy(name, "EQ8 Series", len);
				return name;
			case 5:
				strncpy(name, "AZ-EQ6 Series", len);
				return name;
			case 6:
				strncpy(name, "AZ-EQ5 Series", len);
				return name;
			case 160:
				strncpy(name, "AllView Series", len);
				return name;
			default:
				if ((id >= 128) && (id <= 143)) {
					strncpy(name, "AZ Series", len);
					return name;
				}
				if ((id >= 144) && (id <= 159)) {
					strncpy(name, "DOB series", len);
					return name;
				}
				name[0] = '\0';
				return NULL;
		}
	}
	return NULL;
}

/******************************************************************
 The following commands are not officially documented by Celestron.
 They are reverse engineered, more information can be found here:
 http://www.paquettefamily.ca/nexstar/NexStar_AUX_Commands_10.pdf
 ******************************************************************/
 /*
 int tc_get_guide_rate() {
	 return RC_PARAMS;
 }
 int tc_set_guide_rate_fixed() {
	 return RC_PARAMS;
 }
 int tc_set_guide_rate() {
	 return RC_PARAMS;
 }
 */

int tc_get_autoguide_rate(int dev, char axis) {
	char axis_id;
	char res[2];

	REQUIRE_VER(VER_AUX);

	if (axis > 0) axis_id = _TC_AXIS_RA_AZM;
	else axis_id = _TC_AXIS_DE_ALT;

	/* Get autoguide rate cmd_id = 0x47 */
	if (tc_pass_through_cmd(dev, 1, axis_id, 0x47, 0, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	unsigned char rate = (unsigned char)(100 * (unsigned char)res[0] / 256);
	return rate;
}

int tc_set_autoguide_rate(int dev, char axis, char rate) {
	char axis_id;
	char res;
	unsigned char rrate;

	REQUIRE_VER(VER_AUX);

	if (axis > 0) axis_id = _TC_AXIS_RA_AZM;
	else axis_id = _TC_AXIS_DE_ALT;

	/* rate should be [0%-99%] */
	if ((rate < 0) || (rate > 99)) return RC_PARAMS;

	/* This is wired, but is done to match as good as
	   possible the values given by the HC */
	if (rate == 0) rrate = 0;
	else if (rate == 99) rrate = 255;
	else rrate = (unsigned char)(256 * rate / 100) + 1;

	/* Set autoguide rate cmd_id = 0x46 */
	return tc_pass_through_cmd(dev, 2, axis_id, 0x46, rrate, 0, 0, 0, &res);
}

int tc_get_backlash(int dev, char axis, char direction) {
	char axis_id, cmd_id;
	char res[2];

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if (axis > 0) axis_id = _TC_AXIS_RA_AZM;
	else axis_id = _TC_AXIS_DE_ALT;

	if (direction > 0) cmd_id = 0x40;  /* Get positive backlash */
	else cmd_id = 0x41; /* Get negative backlash */

	if (tc_pass_through_cmd(dev, 1, axis_id, cmd_id, 0, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	return (unsigned char)res[0];
}

int tc_set_backlash(int dev, char axis, char direction, char backlash) {
	char res, axis_id, cmd_id;

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if (axis > 0) axis_id = _TC_AXIS_RA_AZM;
	else axis_id = _TC_AXIS_DE_ALT;

	if (direction > 0) cmd_id = 0x10;  /* Set positive backlash */
	else cmd_id = 0x11; /* Set negative backlash */

	/* backlash should be [0-99] */
	if ((backlash < 0) || (backlash > 99)) return RC_PARAMS;

	return tc_pass_through_cmd(dev, 2, axis_id, cmd_id, backlash, 0, 0, 0, &res);
}

int tc_pass_through_cmd(int dev, char msg_len, char dest_id, char cmd_id,
	char data1, char data2, char data3, char res_len, char* response) {
	char cmd[8];

	cmd[0] = 'P';
	cmd[1] = msg_len;
	cmd[2] = dest_id;
	cmd[3] = cmd_id;
	cmd[4] = data1;
	cmd[5] = data2;
	cmd[6] = data3;
	cmd[7] = res_len;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	/* must read res_len + 1 bytes to accomodate "#" at the end */
	if (read_telescope(dev, response, res_len + 1) < 0) return RC_FAILED;

	return RC_OK;
}


/******************************************
 conversion:	nexstar <-> decimal degrees
 ******************************************/
int pnex2dd(char* nex, double* d1, double* d2) {
	unsigned int d1_x;
	unsigned int d2_x;
	double d1_factor;
	double d2_factor;

	sscanf(nex, "%x,%x", &d1_x, &d2_x);
	d1_factor = d1_x / (double)0xffffffff;
	d2_factor = d2_x / (double)0xffffffff;
	*d1 = 360 * d1_factor;
	*d2 = 360 * d2_factor;
	if (*d2 < -90.0001) *d2 += 360;
	if (*d2 > 90.0001) *d2 -= 360;

	return RC_OK;
}

int nex2dd(char* nex, double* d1, double* d2) {
	unsigned int d1_x;
	unsigned int d2_x;
	double d1_factor;
	double d2_factor;

	sscanf(nex, "%x,%x", &d1_x, &d2_x);
	d1_factor = d1_x / 65536.0;
	d2_factor = d2_x / 65536.0;
	*d1 = 360 * d1_factor;
	*d2 = 360 * d2_factor;
	if (*d2 < -90.0001) *d2 += 360;
	if (*d2 > 90.0001) *d2 -= 360;

	return RC_OK;
}

int dd2nex(double d1, double d2, char* nex) {
	d1 = d1 - 360 * floor(d1 / 360);
	d2 = d2 - 360 * floor(d2 / 360);

	if (d2 < 0) {
		d2 = d2 + 360;
	}
	//printf("%f *** %f\n" ,d1,d2);

	double d2_factor = d2 / 360.0;
	double d1_factor = d1 / 360.0;
	unsigned short int nex1 = (unsigned short int)(d1_factor * 65536);
	unsigned short int nex2 = (unsigned short int)(d2_factor * 65536);

	sprintf(nex, "%04X,%04X", nex1, nex2);
	return RC_OK;
}

int dd2pnex(double d1, double d2, char* nex) {
	d1 = d1 - 360 * floor(d1 / 360);
	d2 = d2 - 360 * floor(d2 / 360);

	if (d2 < 0) {
		d2 = d2 + 360;
	}
	//printf("%f *** %f\n" ,d1,d2);

	double d2_factor = d2 / 360.0;
	double d1_factor = d1 / 360.0;
	unsigned int nex1 = (unsigned)(d1_factor * (0xffffffff));
	unsigned int nex2 = (unsigned)(d2_factor * (0xffffffff));

	sprintf(nex, "%08X,%08X", nex1, nex2);
	return RC_OK;
}
/**************************************************************
	Celestron NexStar compatible telescope control library

	(C)2013-2016 by Rumen G.Bogdanovski
***************************************************************/
#if !defined(__NEXSTAR_PEC_H)
#define __NEXSTAR_PEC_H

#define PEC_START 1
#define PEC_STOP 0

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
	#endif

	int pec_index_found(int dev);
	int pec_seek_index(int dev);

	int pec_record(int dev, char action);
	int pec_record_complete(int dev);

	int pec_playback(int dev, char action);
	int pec_get_playback_index(int dev);

	int pec_get_data_len(int dev);

	int _pec_set_data(int dev, unsigned char index, char data);
	int _pec_get_data(int dev, unsigned char index);

	int pec_set_data(int dev, float* data, int len);
	int pec_get_data(int dev, float* data, const int max_len);

	#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif /* __NEXSTAR_PEC_H */

// START OF COPY
/**************************************************************
	ASTRONOMICAL ROUTINE LIBRARY

	ASCII to double and double to ASCII
	conversion routines.

	(C)2000 by Rumen G.Bogdanovski
***************************************************************/



int a2dd(double* dd, const char* a) {
	int i;
	double 	deg, min, sec, sign = 1;
	char* buff, * b1, format[30], * pnt;

	buff = (char*)a;                     //clear the spaces
	while (isspace(buff[0])) buff++;
	i = strlen(buff) - 1;
	while (isspace(buff[i])) i--;
	buff[i + 1] = '\0';

	if (buff[0] == '-') { sign = -1; buff++; }
	if (buff[0] == '+') buff++;

	if ((buff = (char*)strtok_r(buff, ":", &pnt)) == NULL) return Err_INVSTR;
	deg = (double)strtoul(buff, &b1, 10);
	if ((buff[0] == '\0') || (b1[0] != '\0')) return Err_INVSTR;

	if ((buff = (char*)strtok_r(NULL, ":", &pnt)) == NULL) return Err_INVSTR;
	min = (double)strtoul(buff, &b1, 10);
	if ((buff[0] == '\0') || (b1[0] != '\0')) return Err_INVSTR;

	if ((buff = (char*)strtok_r(NULL, "\0", &pnt)) == NULL) return Err_INVSTR;
	sec = (double)strtod(buff, &b1);
	if ((buff[0] == '\0') || (b1[0] != '\0')) return Err_INVSTR;

	if ((min >= 60) || (min < 0) || (sec >= 60) || (sec < 0)) return Err_OUTOFR;

	*dd = sign * (deg + min / 60 + sec / 3600);

	return Err_NOERR;
}


char* dd2a(double a, int plus) {
	int sign = 1;
	int min, deg;
	double sec;
	static char str[30], * fc;

	if (a < 0) { a *= -1; sign = -1; }

	#ifdef NUNDRETHS	
	a *= 360000;
	a = rint(a);
	a /= 360000;
	#endif 
	#ifdef TENTHS	
	a *= 36000;
	a = rint(a);
	a /= 36000;
	#endif 
	#ifdef SECONDS	
	a *= 3600;
	a = rint(a);
	a /= 3600;
	#endif 

	deg = (int)a;
	sec = (a - (int)(a)) * 3600;
	min = sec / 60;
	sec = fabs(sec - min * 60);


	#ifdef HUNDRETHS
	if (sign < 0) sprintf(str, "-%d:%02d:%05.2f", deg, min, sec);
	else if (plus > 0)
		sprintf(str, "+%d:%02d:%05.2f", deg, min, sec);
	else sprintf(str, "%d:%02d:%05.2f", deg, min, sec);
	#endif
	#ifdef TENTHS	
	if (sign < 0) sprintf(str, "-%d:%02d:%04.1f", deg, min, sec);
	else if (plus > 0)
		sprintf(str, "+%d:%02d:%04.1f", deg, min, sec);
	else sprintf(str, "%d:%02d:%04.1f", deg, min, sec);
	#endif
	#ifdef SECONDS	
	if (sign < 0) sprintf(str, "-%d:%02d:%02d", deg, min, (int)sec);
	else if (plus > 0)
		sprintf(str, "+%d:%02d:%02d", deg, min, (int)sec);
	else sprintf(str, "%d:%02d:%02d", deg, min, (int)sec);
	#endif
	if ((fc = strchr(str, ',')))
		*fc = '.';
	return str;
}

int a2dh(double* dh, const char* a) {
	int i;
	double 	hour, min, sec;
	char* buff, * b1, * pnt;

	buff = (char*)a;                     //clear the spaces
	while (isspace(buff[0])) buff++;
	i = strlen(buff) - 1;
	while (isspace(buff[i])) i--;
	buff[i + 1] = '\0';

	if ((buff = (char*)strtok_r(buff, ":", &pnt)) == NULL) return Err_INVSTR;
	hour = (double)strtoul(buff, &b1, 10);
	if ((buff[0] == '\0') || (b1[0] != '\0')) return Err_INVSTR;

	if ((buff = (char*)strtok_r(NULL, ":", &pnt)) == NULL) return Err_INVSTR;
	min = (double)strtoul(buff, &b1, 10);
	if ((buff[0] == '\0') || (b1[0] != '\0')) return Err_INVSTR;

	if ((buff = (char*)strtok_r(NULL, "\0", &pnt)) == NULL) return Err_INVSTR;
	sec = (double)strtod(buff, &b1);
	if ((buff[0] == '\0') || (b1[0] != '\0')) return Err_INVSTR;

	if ((hour < 0) || (hour >= 24) || (min >= 60) || (min < 0) || (sec >= 60) || (sec < 0))
		return Err_OUTOFR;

	*dh = hour + min / 60 + sec / 3600;

	return Err_NOERR;
}

char* dh2a(double h) {
	int min, hour;
	double sec;
	static char str[30];

	if ((h < 0) || (h >= 24)) return NULL;

	#ifdef NUNDRETHS	
	h *= 360000;
	h = rint(h);
	h /= 360000;
	#endif 
	#ifdef TENTHS	
	h *= 36000;
	h = rint(h);
	h /= 36000;
	#endif 
	#ifdef SECONDS	
	h *= 3600;
	h = rint(h);
	h /= 3600;
	#endif	
	hour = (int)h;
	sec = (h - (int)(h)) * 3600;
	min = sec / 60;
	sec = sec - min * 60;

	#ifdef HUNDRETHS	
	sprintf(str, "%02d:%02d:%05.2f", hour, min, sec);
	#endif

	#ifdef TENTHS	
	sprintf(str, "%02d:%02d:%04.1f", hour, min, sec);
	#endif
	#ifdef SECONDS	
	sprintf(str, "%02d:%02d:%02d", hour, min, (int)sec);
	#endif

	return str;
}

void dd2dms(double ang, unsigned char* deg, unsigned char* min, unsigned char* sec, char* sign) {
	if (ang >= 0) {
		int a = (int)(ang * 3600 + 0.5);
		*deg = (unsigned char)(a / 3600);
		int f = (int)(a % 3600);
		*min = (unsigned char)(f / 60);
		*sec = (unsigned char)(f % 60);
		*sign = 0;
		return;
	} else {
		ang *= -1;
		int a = (int)(ang * 3600 + 0.5);
		*deg = (unsigned char)(a / 3600);
		int f = (int)(a % 3600);
		*min = (unsigned char)(f / 60);
		*sec = (unsigned char)(f % 60);
		*sign = 1;
		return;
	}
}

/**************************************************************
		Celestron NexStar compatible telescope control library

		(C)2013-2016 by Rumen G.Bogdanovski
***************************************************************/



int parse_devname(char* device, char* host, int* port) {
	char* strp;
	int n;

	n = sscanf(device, "tcp://%s", host);
	if (n < 1) return 0;
	strp = host;
	strsep(&strp, ":");
	if (strp == NULL) {
		*port = DEFAULT_PORT;
	} else {
		*port = atoi(strp);
	}
	return 1;
}


int open_telescope_rs(char* dev_file) {
	int dev_fd;
	struct termios options;

	if ((dev_fd = open(dev_file, O_RDWR | O_NOCTTY | O_SYNC)) == -1) {
		return -1;
	}

	memset(&options, 0, sizeof options);
	if (tcgetattr(dev_fd, &options) != 0) {
		close(dev_fd);
		return -1;
	}

	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);
	/* Finaly!!!!  Works on Linux & Solaris  */
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN);
	options.c_oflag &= ~(OPOST);
	options.c_iflag &= ~(INLCR | ICRNL | IXON | IXOFF | IXANY | IMAXBEL);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cc[VMIN] = 0;	// read doesn't block
	options.c_cc[VTIME] = 50;	// 5 seconds read timeout

	if (tcsetattr(dev_fd, TCSANOW, &options) != 0) {
		close(dev_fd);
		return -1;
	}

	return dev_fd;
}


int open_telescope_tcp(char* host, int port) {
	struct sockaddr_in srv_info;
	struct hostent* he;
	int sock;
	struct timeval timeout;

	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	if ((he = gethostbyname(host)) == NULL) {
		return -1;
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		return -1;
	}

	memset(&srv_info, 0, sizeof(srv_info));
	srv_info.sin_family = AF_INET;
	srv_info.sin_port = htons(port);
	srv_info.sin_addr = *((struct in_addr*)he->h_addr);
	if (connect(sock, (struct sockaddr*)&srv_info, sizeof(struct sockaddr)) < 0) {
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		close(sock);
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		close(sock);
		return -1;
	}
	return sock;
}
/**************************************************************
	Celestron NexStar compatible telescope control library

	(C)2013-2016 by Rumen G.Bogdanovski
***************************************************************/


int pec_index_found(int dev) {
	char res[2];

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if (tc_pass_through_cmd(dev, 1, _TC_AXIS_RA_AZM, 0x18, 0, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	if (res[0] == 0) return 0;
	else return 1;
}


int pec_seek_index(int dev) {
	char res;

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if (tc_pass_through_cmd(dev, 1, _TC_AXIS_RA_AZM, 0x19, 0, 0, 0, 0, &res) < 0) {
		return RC_FAILED;
	}

	return RC_OK;
}


int pec_record(int dev, char action) {
	char res, cmd_id;

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if (action == PEC_START) {
		cmd_id = 0x0c;  /* Start PEC record */
	} else if (action == PEC_STOP) {
		cmd_id = 0x16;  /* Stop PEC record */
	} else {
		return RC_PARAMS;
	}

	if (tc_pass_through_cmd(dev, 1, _TC_AXIS_RA_AZM, cmd_id, 0, 0, 0, 0, &res) < 0) {
		return RC_FAILED;
	}

	return RC_OK;
}


int pec_record_complete(int dev) {
	char res[2];

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if (tc_pass_through_cmd(dev, 1, _TC_AXIS_RA_AZM, 0x15, 0, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	if (res[0] == 0) return 0;
	else return 1;
}


int pec_playback(int dev, char action) {
	char res;

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if ((action != PEC_START) && (action != PEC_STOP)) {
		return RC_PARAMS;
	}

	if (tc_pass_through_cmd(dev, 2, _TC_AXIS_RA_AZM, 0x0d, action, 0, 0, 0, &res) < 0) {
		return RC_FAILED;
	}

	return RC_OK;
}


int pec_get_playback_index(int dev) {
	char res[2];

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if (tc_pass_through_cmd(dev, 1, _TC_AXIS_RA_AZM, 0x0e, 0, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	return (unsigned char)res[0];
}


int pec_get_data_len(int dev) {
	char res[2];

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if (tc_pass_through_cmd(dev, 2, _TC_AXIS_RA_AZM, 0x30, 0x3f, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	return (unsigned char)res[0];
}


int _pec_set_data(int dev, unsigned char index, char data) {
	char res;

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if (tc_pass_through_cmd(dev, 4, _TC_AXIS_RA_AZM, 0x31, 0x40 + index, data, 0, 0, &res) < 0) {
		return RC_FAILED;
	}

	return RC_OK;
}


/* on success the returned value should be threated as a signed char */
int _pec_get_data(int dev, unsigned char index) {
	char res[2];

	REQUIRE_VER(VER_AUX);
	REQUIRE_VENDOR(VNDR_CELESTRON);

	if (tc_pass_through_cmd(dev, 2, _TC_AXIS_RA_AZM, 0x30, 0x40 + index, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	return (unsigned char)res[0];
}


int pec_set_data(int dev, float* data, int len) {
	int pec_len, i, rdiff;
	float diff, current = 0;

	pec_len = pec_get_data_len(dev);
	if (pec_len < 0) {
		return RC_FAILED;
	}

	if (pec_len != len) {
		return RC_PARAMS;
	}

	for (i = 0; i < pec_len; i++) {
		diff = data[i] - current;
		/* I have no idea why the values are different for positive and negative numbers
		   I thought the coefficient should be 0.0772 arcsec/unit as the resolution of
		   24bit number for 360 degrees. I tried to mach as best as I could the values
		   returned by Celestron's PECTool and I came up with this numbers... */
		if (diff < 0) {
			rdiff = (int)roundl(data[i] / 0.0845) - (int)roundl(current / 0.0845);
		} else {
			rdiff = (int)roundl(data[i] / 0.0774) - (int)roundl(current / 0.0774);
		}

		if ((rdiff > 127) || (rdiff < -127)) {
			return RC_DATA;
		}

		if (_pec_set_data(dev, i, (char)rdiff) < 0) {
			return RC_FAILED;
		}
		current = data[i];
	}

	if (pec_record(dev, PEC_STOP) < 0) {
		return RC_FAILED;
	}
	return RC_OK;
}


int pec_get_data(int dev, float* data, const int max_len) {
	int pec_len, i, diff;
	float current = 0;
	char rdata;

	pec_len = pec_get_data_len(dev);
	if (pec_len < 0) {
		return RC_FAILED;
	}

	if (pec_len > max_len) {
		return RC_PARAMS;
	}

	for (i = 0; i < pec_len; i++) {
		diff = _pec_get_data(dev, i);
		if (diff < 0) {
			return RC_FAILED;
		}
		/* we need thedata as a signed char */
		rdata = (char)diff;

		/* I have no idea why the values are different for positive and negative numbers.
		   Pease se the note in pec_set_data()! */
		if (rdata > 0) {
			current += rdata * 0.0774;
		} else {
			current += rdata * 0.0845;
		}
		data[i] = current;
	}
	return pec_len;
}
