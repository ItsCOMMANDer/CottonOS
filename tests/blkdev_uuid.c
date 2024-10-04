#include <stdio.h>
#include <stdarg.h>

#include <time.h>

#include <math.h>

static int log_level = 3;

struct timespec getUpTime() {
    struct timespec uptime;
    clock_gettime(CLOCK_BOOTTIME, &uptime);
    return uptime;
}

int log_getLevel() {return log_level;}
void log_setLevel(int level) {log_level = level;}

void log_debug(const char* tag, const char* text, ...) {
    if(log_level < 4) return;
    va_list args;
    va_start(args, text);
    struct timespec uptime = getUpTime();
    printf("\e[38;2;180;171;171m[%5li.%06i] D %s: ", uptime.tv_sec, (int)(uptime.tv_nsec / powl(10, (((int)log10l(uptime.tv_nsec) + 1) - 6))), tag);
    vprintf(text, args);
    printf("\e[38;2;255;255;255m\n");
    va_end(args);
}

void log_info(const char* tag, const char* text, ...) {
    if(log_level < 3) return;
    va_list args;
    va_start(args, text);
    struct timespec uptime = getUpTime();
    printf("\e[38;2;0;255;0m[%5li.%06i] I %s: ", uptime.tv_sec, (int)(uptime.tv_nsec / powl(10, (((int)log10l(uptime.tv_nsec) + 1) - 6))), tag);
    vprintf(text, args);
    printf("\e[38;2;255;255;255m\n");
    va_end(args);
}

void log_warn(const char* tag, const char* text, ...) {
    if(log_level < 2) return;
    va_list args;
    va_start(args, text);
    struct timespec uptime = getUpTime();
    printf("\e[38;2;255;255;0m[%5li.%06i] W %s: ",  uptime.tv_sec, (int)(uptime.tv_nsec / powl(10, (((int)log10l(uptime.tv_nsec) + 1) - 6))), tag);
    vprintf(text, args);
    printf("\e[38;2;255;255;255m\n");
    va_end(args);
}

void log_error(const char* tag, const char* text, ...) {
    if(log_level < 1) return;
    va_list args;
    va_start(args, text);
    struct timespec uptime = getUpTime();
    printf("\e[38;2;255;0;0m[%5li.%06i] E %s: ", uptime.tv_sec, (int)(uptime.tv_nsec / powl(10, (((int)log10l(uptime.tv_nsec) + 1) - 6))), tag);
    vprintf(text, args);
    printf("\e[38;2;255;255;255m\n");
    va_end(args);

}


//! CHAT-GPT4o GENERATED, WILL REWRITE WHEN INCLUDING IN INIT


#include <blkid/blkid.h>
#include <stdio.h>
#include <errno.h>

int main() {
    {
        blkid_cache cache;
        blkid_dev dev;
        blkid_probe probe;
        blkid_dev_iterate iterate;
        const char* blkdev_name;
        const char* blkdev_uuid;

        int error;

        if((error = blkid_get_cache(&cache, NULL)) < 0) {
            log_error("BLKID", "BLKID failed to initilize blkid cache with code %i and errno %i", error, errno);
            log_error("BLKID", "errno string: \"%s\"", strerror(errno));
            //! ERROR?
        }

        log_info("BLKID", "initilized BLKID cache");

        error = 0;

        if((error = blkid_probe_all(cache)) < 0) {
            log_error("BLKID", "BLKID failed to probe all devices with code %i and errno %i", error, errno);
            log_error("BLKID", "errno string: \"%s\"", strerror(errno));
            //! ERROR?
        }

        log_info("BLKID", "probed all (devices?)");

        iterate = blkid_dev_iterate_begin(cache);
        while(blkid_dev_next(iterate, &dev) == 0) {
            blkdev_name = blkid_dev_devname(dev);
            probe = blkid_new_probe_from_filename(blkdev_name);
            if(probe == NULL) {
                log_error("BLKID", "Failed to craete probe for device \"%s\" with errno %i", blkdev_name, errno);
                log_error("BLKID", "errno string: \"%s\"", strerror(errno));
                continue;
                //! ERROR?
            }

            error = 0;
            if((error = blkid_do_probe(probe)) != 0) {
                log_error("BLKID", "Failed to do the probeing for device \"%s\" with errno %i", blkdev_name, errno);
                log_error("BLKID", "errno string: \"%s\"", strerror(errno));
                blkid_free_probe(probe);
                continue;
                //! ERROR?
            }

            if(blkid_probe_lookup_value(probe, "UUID", &blkdev_uuid, NULL) == 0) {
                //DO COMPARISON
                printf("DEV: %s uuid: %s\n", blkdev_name, blkdev_uuid);
            } else {
                log_info("BLKID", "Device \"%s\" does not have a uuid (on disk with mbr?)", blkdev_name);
            }

            blkid_free_probe(probe);
        }

        blkid_dev_iterate_end(iterate);
        blkid_put_cache(cache);
    }
}
