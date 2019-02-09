#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

//time
#define ONVIF_TIME_OUT 60000      //60s
//port
#define ONVIF_UDP_PORT 3702
#define ONVIF_TCP_PORT 5000
//ip
#define ONVIF_UDP_IP "239.255.255.250"
#define ONVIF_TCP_IP "117.117.117.117"
//frame size
#define ONVIF_FRAME_WIDTH 720
#define ONVIF_FRAME_HEIGHT 480
