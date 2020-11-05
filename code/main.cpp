#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_gyroscope.h"
#include "Components/l3gd20/l3gd20.c"
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/BoardConfiguration.hpp>
#include "MessageQueue.hpp"
#include <stdio.h>
#include <string.h>

using namespace touchgfx;

/* Kernel includes. */
#include "ee.h"

/* Private macro -------------------------------------------------------------*/
#define ABS(x)         (x < 0) ? (-x) : x

/* TASKs Declaration */
DeclareTask(Task1);
DeclareTask(Task2);
DeclareTask(Task3);


MessageQueue* msg = MessageQueue::getInstance();

/* ISR */
ISR2(ButtonsISR)
{
    msg -> setChangeMode();
    HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN);
}

/* TASKs */
extern UART_HandleTypeDef	UartHandle;

TASK(Task1)
{
    SetEvent(Task1, FrameBufferEvent);
    GetResource(HalResource);
    touchgfx::HAL::getInstance()->taskEntry();
    TerminateTask();
}

float gyroBuffer[3];
float xVal, yVal, zVal = 0;
float xMaxVal, yMaxVal = 0;

TASK(Task2)
{
    /*
     * Read Gyro Angular data (data is already converted in mdps)
     * In this example I use the default fullscale of 500dps (set at line 116 of stm32f429i_discovery_gyroscope.c)
     */
	BSP_GYRO_GetXYZ(gyroBuffer);

	GetResource(GyroscopeResource);

	xVal = gyroBuffer[0];
	yVal = gyroBuffer[1];
	zVal = gyroBuffer[2];

	/* Doing this I give more importance to fluctuations that affect multiple axes because they only "weigh" on one wheel */
	if((ABS(xVal) + ABS(yVal)) > (ABS(xMaxVal) + ABS(yMaxVal))) {
		xMaxVal = xVal;
		yMaxVal = yVal;
	}

    ReleaseResource(GyroscopeResource);

	gyroBuffer[0] = 0.001 * gyroBuffer[0];
	gyroBuffer[1] = 0.001 * gyroBuffer[1];
	gyroBuffer[2] = 0.001 * gyroBuffer[2];
	msg->setGyroData(gyroBuffer);

    TerminateTask();
}

static uint8_t task3Buffer[256];
float suspension[4] = {0, 0, 0, 0};
int red_flag = 0;

TASK(Task3)
{
	/*
	 * x < 0 if pitch down
	 * y < 0 if roll left
	 *
	 * (xMaxVal > 30000 && yMaxVal > 30000)
	 * (xMaxVal > 30000 && -30000 < yMaxVal < 30000)
	 * (xMaxVal > 30000 && yMaxVal < -30000)

     * (30000 > xMaxVal > -30000 && yMaxVal > 30000)
	 * (30000 > xMaxVal > -30000 && -30000 < yMaxVal < 30000)
	 * (30000 > xMaxVal > -30000 && yMaxVal < -30000)

	 * (xMaxVal < -30000 && yMaxVal > 30000)
	 * (xMaxVal < -30000 && -30000 < yMaxVal < 30000)
	 * (xMaxVal < -30000 && yMaxVal < -30000)
	 *
	 * Use xMaxVal and yMaxVal instead of xVal and yVal so it's possible to "capture"
	 * the most significant movement between two Task3 activation and not only the last one
	 * (xVal and yVal are updated at least 6 times between two Task3 activation!)
	 */

	red_flag = 0;

	GetResource(GyroscopeResource);

	if(int(ABS(xMaxVal)) + int(ABS(yMaxVal)) > 180000) {
		red_flag = 1000;
	}

	if(xMaxVal > 30000) {     													// pitch up
		if(yMaxVal > 30000) { 													// roll right
			// sospensione 4 compressa, sospensione 1 alzata
			suspension[0] = -1000;
			suspension[1] = 0;
			suspension[2] = 0;
			suspension[3] = 1000 + red_flag;
			sprintf((char *)task3Buffer, "TASK3: x > 30000, y > 30000, xMaxVal = [%d] - yMaxVal = [%d]\r\n", int(xMaxVal), int(yMaxVal));
		}
		else {
			if(yMaxVal < -30000) {												// roll left
				// sospensione 3 compressa, sospensione 2 alzata
				suspension[0] = 0;
				suspension[1] = -1000;
				suspension[2] = 1000 + red_flag;
				suspension[3] = 0;
				sprintf((char *)task3Buffer, "TASK3: x > 30000, y < -30000, xMaxVal = [%d] - yMaxVal = [%d]\r\n", int(xMaxVal), int(yMaxVal));
			}
			else {															// stable roll
				// sospensioni 3,4 compresse, sospensioni 1,2 alzate
				suspension[0] = -1000;
				suspension[1] = -1000;
				suspension[2] = 1000 + red_flag;
				suspension[3] = 1000 + red_flag;
				sprintf((char *)task3Buffer, "TASK3: x > 30000, -30000 < y < 30000, xMaxVal = [%d] - yMaxVal = [%d]\r\n", int(xMaxVal), int(yMaxVal));
			}
		}
	}
	else {
		if(xMaxVal < -30000) {													// pitch down
			if(yMaxVal > 30000) { 												// roll right
				// sospensione 2 compressa, sospensione 3 alzata
				suspension[0] = 0;
				suspension[1] = 1000 + red_flag;
				suspension[2] = -1000;
				suspension[3] = 0;
				sprintf((char *)task3Buffer, "TASK3: x < -30000, y > 30000, xMaxVal = [%d] - yMaxVal = [%d]\r\n", int(xMaxVal), int(yMaxVal));
			}
			else {
				if(yMaxVal < -30000) {											// roll left
					// sospensione 1 compressa, sospensione 4 alzata
					suspension[0] = 1000 + red_flag;
					suspension[1] = 0;
					suspension[2] = 0;
					suspension[3] = -1000;
					sprintf((char *)task3Buffer, "TASK3: x < -30000, y < -30000, xMaxVal = [%d] - yMaxVal = [%d]\r\n", int(xMaxVal), int(yMaxVal));
				}
				else {														// stable roll
					// sospensioni 1,2 compresse, sospensioni 3,4 alzate
					suspension[0] = 1000 + red_flag;
					suspension[1] = 1000 + red_flag;
					suspension[2] = -1000;
					suspension[3] = -1000;
					sprintf((char *)task3Buffer, "TASK3: x < -30000, -30000 < y < 30000, xMaxVal = [%d] - yMaxVal = [%d]\r\n", int(xMaxVal), int(yMaxVal));
				}
			}
		}
		else {																// stable pitch
			if(yMaxVal > 30000) { 												// roll right
				// sospensioni 2,4 compresse, sospensioni 1,3 alzate
				suspension[0] = -1000;
				suspension[1] = 1000 + red_flag;
				suspension[2] = -1000;
				suspension[3] = 1000 + red_flag;
				sprintf((char *)task3Buffer, "TASK3: -30000 < x < 30000, y > 30000, xMaxVal = [%d] - yMaxVal = [%d]\r\n", int(xMaxVal), int(yMaxVal));
			}
			else {
				if(yMaxVal < -30000) {											// roll left
					// sospensione 1,3 compressa, sospensione 2,4 alzata
					suspension[0] = 1000 + red_flag;
					suspension[1] = -1000;
					suspension[2] = 1000 + red_flag;
					suspension[3] = -1000;
					sprintf((char *)task3Buffer, "TASK3: -30000 < x < 30000, y < -30000, xMaxVal = [%d] - yMaxVal = [%d]\r\n", int(xMaxVal), int(yMaxVal));
				}
				else {														// stable roll
					suspension[0] = 0;
					suspension[1] = 0;
					suspension[2] = 0;
					suspension[3] = 0;
					sprintf((char *)task3Buffer, "TASK3: -30000 < x < 30000, -30000 < y < 30000, xMaxVal = [%d] - yMaxVal = [%d]\r\n", int(xMaxVal), int(yMaxVal));
				}
			}
		}
	}
	xMaxVal = 0;
	yMaxVal = 0;

	ReleaseResource(GyroscopeResource);

	msg->setSuspensionData(suspension);

	GetResource(SerialResource);
    HAL_UART_Transmit_IT(&UartHandle, task3Buffer, strlen((const char*)task3Buffer));
    ReleaseResource(SerialResource);

    TerminateTask();
}

/* Hooks */
void PreTaskHook(void)
{
}

void PostTaskHook(void)
{
}

void StartupHook(void)
{
    BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);

    BSP_GYRO_Init();
    BSP_GYRO_Reset();

    ResumeOSInterrupts();
}

void idle_hook ( void )
{
	;
}

void ErrorHook(StatusType Error)
{
    switch (Error) {
    default:
       break;
    };
}

extern void touchgfx::hw_init();
extern void touchgfx::touchgfx_init();

int main(void)
{
    SuspendOSInterrupts();

    hw_init();

    touchgfx_init();

    StartOS(OSDEFAULTAPPMODE);

    return 0;
}
