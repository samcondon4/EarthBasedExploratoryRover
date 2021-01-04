/*
 * uart.h
 *
 * Code adapted from the digilent uart userspace example and the lewan soul lx-16a arduino example
 */

#include <termios.h>
#include <unistd.h>
#include <stdbool.h>

#ifndef SRC_UART_H_
#define SRC_UART_H_

#define DEBUG


//Define movement id's//

#define MASTCAM 0
#define BODY 1

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define STOP 4
//////////////////////////


struct UartDevice {
	const char* filename;
	int rate;
	int fd;
	struct termios *tty;
};


class MotorComChannel {
	public:
		//attributes

		//methods
		MotorComChannel(const char* filename_, int rate_, bool canonical_); //constructor takes place of uart_start
		//ID of desired servo to move and destination angle from -120 to 120 degrees
		void moveTo(int id, float destination);
		//ID of desired servo to move and velocity from -100 to 100 percent
		void spinAt(int id, float velocity);

		//Trigger a move from pre-built buffers//
		int eber_move(int mode, int move);

		int uart_writen(const char *buf, size_t buf_len);
		int uart_reads(char *buf, size_t buf_len);
		
		~MotorComChannel(); //destructor takes place of uart_stop

	private:
		struct UartDevice uart_dev;
		
		/* command packets to start motor movement *////////////////////
		//left_spin: tells motor id 1 to move at speed 75
		//right_spin: motor id 1, move at speed -75
		//up_spin: motor id 2, move at speed 75
		//down_spin: motor id 2, move at speed -75

		const char left_spin_buf[10] = {0x55, 0x55, 0x01, 0x07, 0x1d, 0x01, 0x00, 0xee, 0x02, 0xe9};
		const char right_spin_buf[10] = {0x55, 0x55, 0x01, 0x07, 0x1d, 0x01, 0x00, 0x12, 0xfd, 0xca};
		const char up_spin_buf[10] = {0x55, 0x55, 0x02, 0x07, 0x1d, 0x01, 0x00, 0xee, 0x02, 0xe8};
		const char down_spin_buf[10] = {0x55, 0x55, 0x02, 0x07, 0x1d, 0x01, 0x00, 0x12, 0xfd, 0xc9};
	
		const char one_stop_buf[10] = {0x55, 0x55, 0x01, 0x07, 0x1d, 0x01, 0x00, 0x00, 0x00, 0xd9};
		const char two_stop_buf[10] = {0x55, 0x55, 0x02, 0x07, 0x1d, 0x01, 0x00, 0x00, 0x00, 0xd8};
		
		///////////////////////////////////////////////////////////////


};



#endif /* SRC_UART_H_ */
