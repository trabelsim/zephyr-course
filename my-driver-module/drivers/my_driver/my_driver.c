#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT my_led_sensor

LOG_MODULE_REGISTER(my_driver, LOG_LEVEL_INF);

struct my_driver_config 
{
    struct gpio_dt_spec led_gpio; // led reference - from the overlay.
};

struct my_driver_data
{
    bool led_state;
    int blink_count;
};

int my_driver_inc_blink_count(const struct device* dev)
{
    struct my_driver_data* data = dev->data;
    data->blink_count++;
    return 0;
}

int my_driver_get_blink_count(const struct device* dev)
{
    struct my_driver_data* data = dev->data;
    return data->blink_count;
}

static int my_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    const struct my_driver_config* config = dev->config;
    struct my_driver_data* data = dev->data;

    gpio_pin_set_dt(&config->led_gpio, 1);
    data->led_state = true;

    LOG_INF("[Fetch] Led turned ON [%s]", dev->name);
    
    return 0;
}

static int my_driver_channel_get (const struct device* dev, enum sensor_channel chan, struct sensor_value* val)
{
    const struct my_driver_config* config = dev->config;
    struct my_driver_data* data = dev->data;

    gpio_pin_set_dt(&config->led_gpio, 0);
    data->led_state = false;
    val->val1 = data->led_state;
    val->val2 = 0;

    LOG_INF("[Get] Led turned OFF [%s]", dev->name);

    return 0;
}

static DEVICE_API(sensor, my_driver_api) = {
    .sample_fetch = my_driver_sample_fetch,
    .channel_get = my_driver_channel_get
};

static int init_my_driver(const struct device* dev)
{
    const struct my_driver_config* config = dev->config;
    struct my_driver_data* data = dev->data;

    if (!gpio_is_ready_dt(&config->led_gpio)) {
        LOG_ERR("LED GPIO device not ready");
        return -ENODEV;
    }

    LOG_INF("Configuring LED GPIO: port %s, pin %d", config->led_gpio.port->name, config->led_gpio.pin);

    if (gpio_pin_configure_dt(&config->led_gpio, GPIO_OUTPUT_ACTIVE) < 0) {
        LOG_ERR("Failed to configure LED GPIO");
        return -EIO;
    }

    data->led_state = true;

    LOG_INF("Device initialized [%s]", dev->name);
    return 0;
}

#define MY_DRIVER_INIT(inst) static struct my_driver_data data_##inst = {}; \
                             static const struct my_driver_config config_##inst = { \
                                .led_gpio = GPIO_DT_SPEC_GET(DT_INST_PHANDLE(inst, led), gpios) \
                             }; \
        DEVICE_DT_INST_DEFINE(inst, init_my_driver, NULL, &data_##inst, &config_##inst, \
                              POST_KERNEL, 80, &my_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MY_DRIVER_INIT)