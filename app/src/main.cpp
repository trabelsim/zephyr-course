#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include "my_driver_api.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

const struct device* my_dev = DEVICE_DT_GET(DT_NODELABEL(my_driver0));

int main(void)
{
    if (!device_is_ready(my_dev)) {
        LOG_ERR("My driver device not ready");
        return 0;
    }

    if (!IS_ENABLED(CONFIG_LED_SUBSYSTEM))
    {
        LOG_INF("Led subsystem not activated. Please activate in menuconfig. Exiting.");
        return 0;
    }

    int sleep_ms = CONFIG_APP_HEARTBEAT_PERIOD_MS;
    LOG_INF("LED Subsystem set time: %ld", sleep_ms);

    while (1) 
    {
        sensor_sample_fetch(my_dev);
        my_driver_inc_blink_count(my_dev);
        k_msleep(sleep_ms);

        struct sensor_value val;
        sensor_channel_get(my_dev, SENSOR_CHAN_ALL, &val);

        int count = my_driver_get_blink_count(my_dev);
        LOG_INF("Blink count: %d", count);
        
        k_msleep(sleep_ms);
    }

    return 0;
}
