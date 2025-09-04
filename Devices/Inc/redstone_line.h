#ifndef __RS_LINE_H__
#define __RS_LINE_H__

#include "redstone.h"

typedef enum {
    RS_LINE_CH1_up = 0,
    RS_LINE_CH2_down,
    RS_LINE_CH3_left,
    RS_LINE_CH4_right,

    RS_LINE_CH_NUM,
} rs_line_chs_e;

typedef enum {
    RS_LINE_SIG_WAITTIG = 0,    // 等待某些方向的红石信号传来
    RS_LINE_SIG_SENDING,        // 已经有某些方向传来了红石信号，正在为其他方向传递红石信号
} rs_line_sig_state_e;

typedef struct {
    uint8_t port;
    uint8_t pin;
} rs_line_led_t;

struct rs_line_stu {
    rs_line_sig_state_e status;
    uint8_t level; // 红石信号强度
    uint8_t bitmask_connection; // 各方向连接状态

    rs_line_led_t *leds;

    void (*led_pin_write)(rs_line_chs_e led_num, uint8_t pin_state);

    struct rs_io_stu io;
};


void rs_line_init(struct rs_line_stu *rsl);

uint8_t my_rs_io_pin_read(struct rs_channel_stu *ch);
void my_rs_io_pin_write(struct rs_channel_stu *ch, uint8_t pin_state);

void my_rs_io_callback_channel_change(struct rs_io_stu *io);
void my_rs_io_callback_communication_init(struct rs_io_stu *io);
void my_rs_io_callback_communication_over(struct rs_io_stu *io);

void my_rs_line_led_pin_write(rs_line_chs_e led_num, uint8_t pin_state);

#endif // __RS_LINE_H__
