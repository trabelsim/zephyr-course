#ifndef MY_DRIVER_API_H
#define MY_DRIVER_API_H

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

int my_driver_inc_blink_count(const struct device* dev);
int my_driver_get_blink_count(const struct device* dev);

#ifdef __cplusplus
}
#endif // extern "C"

#endif // MY_DRIVER_API_H