/*
 * uart.c
 *
 * @date 2019/08/09
 * @author Cosmin Tanislav
 * @author Cristian Fatu
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>
#include "MotorComChannel.h"


/*
 * Start the UART device.
 *
 * @param dev points to the UART device to be started, must have filename and rate populated
 * @param canonical whether to define some compatibility flags for a canonical interface
 *
 * @return - 0 if the starting procedure succeeded
 *         - negative if the starting procedure failed
 */
int uart_start(struct UartDevice* dev, bool canonical) {
	struct termios *tty;
	int fd;
	int rc;

	fd = open(dev->filename, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		printf("%s: failed to open UART device\r\n", __func__);
		return fd;
	}

	tty = (termios*)malloc(sizeof(*tty));
	if (!tty) {
		printf("%s: failed to allocate UART TTY instance\r\n", __func__);
		return -ENOMEM;
	}

	memset(tty, 0, sizeof(*tty));

	/*
	 * Set baud-rate.
	 */
	tty->c_cflag |= dev->rate;

    /* Ignore framing and parity errors in input. */
    tty->c_iflag |=  IGNPAR;

    /* Use 8-bit characters. This too may affect standard streams,
     * but any sane C library can deal with 8-bit characters. */
    tty->c_cflag |=  CS8;

    /* Enable receiver. */
    tty->c_cflag |=  CREAD;

    if (canonical) {
        /* Enable canonical mode.
         * This is the most important bit, as it enables line buffering etc. */
        tty->c_lflag |= ICANON;
    } else {
        /* To maintain best compatibility with normal behaviour of terminals,
         * we set TIME=0 and MAX=1 in noncanonical mode. This means that
         * read() will block until at least one byte is available. */
        tty->c_cc[VTIME] = 0;
        tty->c_cc[VMIN] = 1;
    }

	/*
	 * Flush port.
	 */
	tcflush(fd, TCIFLUSH);

	/*
	 * Apply attributes.
	 */
	rc = tcsetattr(fd, TCSANOW, tty);
	if (rc) {
		printf("%s: failed to set attributes\r\n", __func__);
		return rc;
	}

	dev->fd = fd;
	dev->tty = tty;

	return 0;
}


MotorComChannel::MotorComChannel(const char* filename_, int rate_, bool canonical_)
{

	uart_dev.filename = filename_;
	uart_dev.rate = rate_;
	uart_start(&uart_dev, canonical_);

}

MotorComChannel::~MotorComChannel()
{
	free(uart_dev.tty);
}

/* Move to specified angle */
void MotorComChannel::moveTo(int id, float destination)
{
	int x = 0;
}

/* Spin motors at specified speed */
void MotorComChannel::spinAt(int id, float velocity)
{
	int x = 0;
}

int MotorComChannel::eber_move(int mode, int move)
{
	switch(mode)
	{
		case MASTCAM:
			switch(move)
			{
				case LEFT:
					this->uart_writen(left_spin_buf,10);
				break;

				case RIGHT:
					this->uart_writen(right_spin_buf,10);
				break;

				case UP:
					this->uart_writen(up_spin_buf,10);
				break;
				
				case DOWN:
					this->uart_writen(down_spin_buf,10);
				break;

				case STOP:
//					this->uart_writen(stop_spin_buf, 10);
				break;

			}
		break;

		case BODY:
			printf("\n body movements not implemented yet \n");
		break;
	}
}

/* send data over uart interface */
int MotorComChannel::uart_writen(const char *buf, size_t buf_len){
	return write(uart_dev.fd, buf, buf_len);
}

/* read data from uart interface */
int MotorComChannel::uart_reads(char *buf, size_t buf_len)
{
	int rc;

	rc = read(uart_dev.fd, buf, buf_len - 1);
	if (rc < 0) {
		printf("%s: failed to read uart data\r\n", __func__);
		return rc;
	}

	return rc;
}




