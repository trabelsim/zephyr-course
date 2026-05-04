if (CONFIG_MY_DRIVER)
    zephyr_library()
    zephyr_library_sources(my_driver.c)
endif()