#include <ch.h>
#include <hal.h>
#include <test.h>

#include "board.h"
#include "usb_device.h"
#include "console.h"
#include "blink.h"

/* GPIO initialization values */
const PALConfig pal_default_config = {
	{ VAL_GPIOAODR, VAL_GPIOACRL, VAL_GPIOACRH },
	{ VAL_GPIOBODR, VAL_GPIOBCRL, VAL_GPIOBCRH },
	{ VAL_GPIOCODR, VAL_GPIOCCRL, VAL_GPIOCCRH },
	{ VAL_GPIODODR, VAL_GPIODCRL, VAL_GPIODCRH },
	{ VAL_GPIOEODR, VAL_GPIOECRL, VAL_GPIOECRH },
};

void __early_init(void)
{
	stm32_clock_init();
}

void boardInit(void)
{
	/* alternate function init */
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_DISABLE |
		AFIO_MAPR_USART1_REMAP |
		AFIO_MAPR_SPI3_REMAP |
		AFIO_MAPR_CAN_REMAP_REMAP2;
}

static WORKING_AREA(heartbeat_wa, 16);
static msg_t heartbeat(void *data)
{
	chRegSetThreadName("heartbeat");

	for (;;) {
		blink(BLINK_GREEN, true);
		chThdSleepMilliseconds(200);
		blink(BLINK_GREEN, true);
		chThdSleepMilliseconds(1500);
	}

	return 0;
}

static void hello(void)
{
	int i;

	for (i = 0; i < 8; i++) {
		led_red_toggle();
		led_green_toggle();
		chThdSleepMilliseconds(50);
	}
}

int main(void)
{
	/* system init */
	halInit();
	chSysInit();

	/* local init */
	blink_init();
	usb_init();
#if 0
	console_init((BaseSequentialStream *)&SD1);
#else
	console_init((BaseSequentialStream *)usb_get_serial_driver());
#endif

	hello();
	led_green_on();

	sdStart(&SD1, NULL);

	chThdCreateStatic(heartbeat_wa, sizeof(heartbeat_wa),
			NORMALPRIO,
			heartbeat, NULL);

	for (;;) {
		console_poll();
		chThdSleepMilliseconds(500);
	}
}
