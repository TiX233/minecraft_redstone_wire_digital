#ifndef __REDSTONE_H__
#define __REDSTONE_H__

#include "main.h"

// 通信时序
typedef enum {
    RS_TICK_0_start = 0,            // 开始通信，拉高所有信号线电平
    RS_TICK_1_check_wire,           // 检查线路电平，如果某个方向不为高则屏蔽该方向
    RS_TICK_2_r18_set,              // 输出红石信号强度为 18
    RS_TICK_3_r18_read,             // 读取红石信号强度，此时如果是低电平，说明传输过来的红石信号强度为 18
    RS_TICK_4_r17_set,
    RS_TICK_5_r17_read,
    RS_TICK_6_r16_set,
    RS_TICK_7_r16_read,
    RS_TICK_8_r15_set,
    RS_TICK_9_r15_read,
    RS_TICK_10_r14_set,
    RS_TICK_11_r14_read,
    RS_TICK_12_r13_set,
    RS_TICK_13_r13_read,
    RS_TICK_14_r12_set,
    RS_TICK_15_r12_read,
    RS_TICK_16_r11_set,
    RS_TICK_17_r11_read,
    RS_TICK_18_r10_set,
    RS_TICK_19_r10_read,
    RS_TICK_20_r9_set,
    RS_TICK_21_r9_read,
    RS_TICK_22_r8_set,
    RS_TICK_23_r8_read,
    RS_TICK_24_r7_set,
    RS_TICK_25_r7_read,
    RS_TICK_26_r6_set,
    RS_TICK_27_r6_read,
    RS_TICK_28_r5_set,
    RS_TICK_29_r5_read,
    RS_TICK_30_r4_set,
    RS_TICK_31_r4_read,
    RS_TICK_32_r3_set,
    RS_TICK_33_r3_read,
    RS_TICK_34_r2_set,
    RS_TICK_35_r2_read,
    RS_TICK_36_r1_set,
    RS_TICK_37_r1_read,
    RS_TICK_38_ack_set,             // 设置 ack
    RS_TICK_39_ack_read,            // 读取 ack
    RS_TICK_40_wire_release,        // 通信结束，释放信号线
    RS_TICK_41,
    RS_TICK_42,
    RS_TICK_43,
    RS_TICK_44,
    RS_TICK_45,
    RS_TICK_46,
    RS_TICK_47,
    RS_TICK_48,
    RS_TICK_49,

    RS_TICK_TIME_OUT = 500,         // 超时未获取到时钟信号
} rs_tick_e;

typedef enum {
    RS_IO_STATE_idle = 0,
    RS_IO_STATE_busy,
} rs_io_state_e;

// 通道状态
typedef enum {
    RS_CHANNEL_STATE_start = 0,
    RS_CHANNEL_STATE_check,
    RS_CHANNEL_STATE_trans,
    RS_CHANNEL_STATE_release,

    RS_CHANNEL_STATE_timeout,
    RS_CHANNEL_STATE_broken,
} rs_channel_state_e;

// 通信步骤
typedef enum {
    RS_CHANNEL_STEP_0_start = 0,
    RS_CHANNEL_STEP_1_set_r18,
    RS_CHANNEL_STEP_2_set_r17,
    RS_CHANNEL_STEP_3_set_r16,
    RS_CHANNEL_STEP_4_set_r15,
    RS_CHANNEL_STEP_5_set_r14,
    RS_CHANNEL_STEP_6_set_r13,
    RS_CHANNEL_STEP_7_set_r12,
    RS_CHANNEL_STEP_8_set_r11,
    RS_CHANNEL_STEP_9_set_r10,
    RS_CHANNEL_STEP_10_set_r9,
    RS_CHANNEL_STEP_11_set_r8,
    RS_CHANNEL_STEP_12_set_r7,
    RS_CHANNEL_STEP_13_set_r6,
    RS_CHANNEL_STEP_14_set_r5,
    RS_CHANNEL_STEP_15_set_r4,
    RS_CHANNEL_STEP_16_set_r3,
    RS_CHANNEL_STEP_17_set_r2,
    RS_CHANNEL_STEP_18_set_r1,
    RS_CHANNEL_STEP_19_set_ack,
    RS_CHANNEL_STEP_20_release,
    
    RS_CHANNEL_STEP_max,
} rs_channel_step_e;

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} rs_pin_stu_t;

// 通道结构体
struct rs_channel_stu {
    rs_channel_state_e status;

    rs_pin_stu_t pin;

    uint8_t pin_script[RS_CHANNEL_STEP_max + 5]; // 通信过程中的预设电平变化脚本，io 管理函数会在对应的通信时刻按照脚本来改变该通道的引脚电平
};

// 通信结构体
struct rs_io_stu {
    rs_io_state_e status;
    rs_tick_e tick;

    struct rs_channel_stu *channels;
    const uint8_t channel_num; // 所管理的通道总数量
    uint8_t bitmask_channel_change; // 通道变化标记

    uint8_t (*pin_read)(struct rs_channel_stu *ch);
    void (*pin_write)(struct rs_channel_stu *ch, uint8_t pin_state);

    void (*callback_channel_change)(struct rs_io_stu *io); // 某一个或几个通道电平与预设脚本不一致，调用该回调
    void (*callback_communication_init)(struct rs_io_stu *io); // 由 rs_io_communication_start 调用，用于在通信开始前做一些自定义的初始化
    void (*callback_communication_over)(struct rs_io_stu *io); // 一次红石通信完成后的回调
};

int rs_io_communication_start(struct rs_io_stu *io); // 接收到时钟信号后调用，用于通知 io 管理器进行通信
void rs_io_maintainer(struct rs_io_stu *io); // io 管理器，每个 tick 调用一次


#endif // __REDSTONE_H__
