#include <time.h>
#include <signal.h>

#include "service.h"

void service_parseConfig(service_t *service);

void service_start(service_t *service);
void service_stop(service_t *service) {
	if(service == NULL) {
		return 100000;
	}
	kill(service->pid, SIGTERM);

	bool killed = false;

	for(int i = 0; i < service->timeoutStop; i++) {
		switch(kill(service->pid, 0)) {
			default:
			case 0:
				continue;
			case -1:
				killed = true;
				break;
		}
	}

	if(!killed) signal(service->pid, SIGKILL);
}

time_t service_shouldStart(service_t *service) {
	if(service == NULL) {
		return 100000;
	}
	struct timespec currentTime;
	clock_gettime(CLOCK_BOOTTIME, &currentTime);
	return (service->schedule.serviceStart + service->schedule.serviceDelay) - currentTime.tv_sec;
};

time_t service_getUptime(const service_t *service) {
	if(service == NULL) {
		return 0;
	}
	struct timespec currentTime;
	clock_gettime(CLOCK_BOOTTIME, &currentTime);
	return currentTime.tv_sec - service->status.serviceStart
}