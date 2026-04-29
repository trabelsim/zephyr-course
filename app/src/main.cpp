#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(app_led)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    /* Is the LED driver initialized and ready?*/
    if (!gpio_is_ready_dt(&led)) return 0;

    if (!IS_ENABLED(CONFIG_LED_SUBSYSTEM))
    {
        LOG_INF("Led subsystem not activated. Please activate in menuconfig. Exiting.");
        return 0;
    }

    /* Configure the LED driver to output and sets its state to active*/
    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    bool led_state = true;
    int sleep_ms = CONFIG_APP_HEARTBEAT_PERIOD_MS;
    LOG_INF("LED Subsystem set time: %ld", sleep_ms);

    while (1) {
        if (gpio_pin_toggle_dt(&led) < 0) return 0;

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");

        k_msleep(sleep_ms);
    }

    return 0;
}
