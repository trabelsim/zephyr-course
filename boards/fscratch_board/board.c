/*
 * Copyright (c) 2025 STMicroelectronics
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/init.h>
#include <stm32_ll_rcc.h>
#include <stddef.h>
#include <errno.h>
/* Boot init print*/
#include <zephyr/sys/printk.h>

typedef __PACKED_STRUCT
{
	uint8_t additional_data[8];	/*!< 64 bits of data to fill OTP slot Ex: MB184510 */
	uint8_t bd_address[6];		/*!< Bluetooth Device Address*/
	uint8_t hsetune;		/*!< Load capacitance to be applied on HSE pad */
	uint8_t index;			/*!< Structure index */
} OTP_Data_s;

#define DEFAULT_OTP_IDX     0

static HAL_StatusTypeDef OTP_Read(uint8_t index, OTP_Data_s **otp_ptr);


/*------------------------------- Board initialization print -------------------------------*/
static int board_print_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

/* From the tests done, board_late_init_hook (below) runs too early before UART is initialized
	
   From the Zephyr documentation: https://docs.zephyrproject.org/latest/doxygen/html/group__sys__init.html#gaf507cc0613add8113c41896bd631254f
   Zephyr offers an infrastructure to call initialization code before main.
   Such initialization calls can be registered using SYS_INIT() or SYS_INIT_NAMED() macros.

   SYS_INIT - Register an initialization function.
	The function will be called during system initialization according to the given level and priority.
*/
SYS_INIT(board_print_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

/*------------------------------------------------------------------------------------------*/

/* Board late init hook left for proper HSE clock initialization */
void board_late_init_hook(void)
{
	HAL_StatusTypeDef status;
	OTP_Data_s *otp_ptr = NULL;

	status = OTP_Read(DEFAULT_OTP_IDX, &otp_ptr);
	if (status != HAL_OK) {
		/* OTP no present in flash, apply default gain */
		LL_RCC_HSE_SetClockTrimming(0x0C);
	} else {
		LL_RCC_HSE_SetClockTrimming(otp_ptr->hsetune);
	}
}

static HAL_StatusTypeDef OTP_Read(uint8_t index, OTP_Data_s **otp_ptr)
{
	*otp_ptr = (OTP_Data_s *) (FLASH_OTP_BASE + FLASH_OTP_SIZE - 16);

	while (((*otp_ptr)->index != index) && ((*otp_ptr) != (OTP_Data_s *) FLASH_OTP_BASE)) {
		(*otp_ptr) -= 1;
	}

	if ((*otp_ptr)->index != index) {
		return HAL_ERROR;
	}

	return HAL_OK;
}
