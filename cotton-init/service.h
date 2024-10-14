#ifndef COTTON_INIT_SERIVE_H
#define COTTON_INIT_SERIVE_H

#include <time.h>

#include <stdbool.h>

enum status {
	STATUS_RUNNING,
	STATUS_STOPPED,
	STATUS_FAILED,
};

enum startupType {
	STARTUP_ON_BOOT,
	STARTUP_AUTOMATICLY,
	STARTUP_MANUAL,
};

enum onDeath {
	ONDEATH_NOTHING,
	ONDEATH_RESTART,
};

typedef struct cotton_init_service_s service_t;

typedef struct cotton_init_service_s {
	char* name;
	char* desciption;

	pid_t pid;

	char* executable;
	char* commandlineArguments;
	char* workingDirectory;
	char* envorinmentVariables;

	int numOfDependencies;
	service_t *dependencies;

	enum startupType startType;

	enum onDeath onFail;
	enum onDeath onSuccess;

	char* logFile;
	int logFileFD;

	char* stdinFile;
	int stdinFileFD;

	int timoutStop;

	bool scheduled;
	struct _ {
			time_t serviceStart;
			time_t serviceDelay;
	} schedule;

	struct _ {
		enum status status;
		time_t serviceStart; // used for uptime calculation
	} status;
}service_t;

void service_parseConfig(service_t *service);

void service_start(service_t *service);
void service_stop(service_t *service);

time_t service_shouldStart(service_t *service);

time_t service_getUptime(servuce_t service);

#endif