#include "LOG.h"
#include "stdio.h"
#include "stdarg.h"
#include "usart.h"

/* UART_DEBUG */
void LOG(const char *format, ...)
{
	char print_buf[512] = { 0 };

	va_list args;
	va_start(args, format);
	int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
	va_end(args);

	if (r > 0) {
	    HAL_UART_Transmit(&huart3, print_buf, strlen(print_buf), 200);
	}
}
