/*
 * lcd.h
 *
 *  Created on: 31/07/2016
 *      Author: xavier
 */

#ifndef LCD_H_
#define LCD_H_

void lcd_init();
void lcd_write(u8 *msg, uint8_t line, uint8_t msglen);
int lcd_is_writing ();
void *lcd_callback(request_t *req, response_t *res);

#endif /* LCD_H_ */
