#include "redstone_line.h"
#include "scan_LED.h"
#include "segment_LED.h"
#include "myTasks.h"

void rs_line_init(struct rs_line_stu *rsl){
    for(uint8_t i = 0; i < RS_LINE_CH_NUM; i ++){
        rsl->io.pin_write(&(rsl->io.channels[i]), 1);
    }

    for(uint8_t i = 0; i < RS_LINE_CH_NUM; i ++){
        for(uint8_t j = 0; j < RS_CHANNEL_STEP_max; j ++){
            rsl->io.channels[i].pin_script[j] = 1;
        }
    }

    rsl->level = 0;
    rsl->bitmask_connection = 0;
    rsl->status = RS_LINE_SIG_WAITTIG;
}

uint8_t my_rs_io_pin_read(struct rs_channel_stu *ch){
    return HAL_GPIO_ReadPin(ch->pin.port, ch->pin.pin);
}

void my_rs_io_pin_write(struct rs_channel_stu *ch, uint8_t pin_state){
    HAL_GPIO_WritePin(ch->pin.port, ch->pin.pin, pin_state);
}

void my_rs_io_callback_communication_init(struct rs_io_stu *io){
    // 复位引脚脚本
    rs_line_init(&my_rs_line);

    // 确保 1ms 后再进入通信
    task_rs_io.topic.flag = 0;
}

void my_rs_io_callback_channel_change(struct rs_io_stu *io){
    
    if(my_rs_line.status == RS_LINE_SIG_WAITTIG){ // 此前没有任一引脚被拉低，此时检测到某些引脚被拉低
        if(io->tick > RS_TICK_6_r16_set && io->tick < RS_TICK_38_ack_set){ // 对于红石线，不检测 r18 和 r17 位
            
            for(uint8_t i = 0; i < RS_LINE_CH_NUM; i ++){
                if(io->bitmask_channel_change & (1 << i)){ // 为信号来源设置 ack 位
                    for(uint8_t j = (io->tick+1)/2; j < RS_CHANNEL_STEP_20_release; j ++){
                        io->channels[i].pin_script[j] = 0;
                    }
                    // 设置该方向的 led 亮起
                    my_rs_line.bitmask_connection |= (1 << i);
                }else { // 为其他方向传输红石信号
                    for(uint8_t j = (io->tick+1)/2; j < RS_CHANNEL_STEP_19_set_ack; j ++){
                        io->channels[i].pin_script[j] = 0;
                    }
                }
            }
            // 更新自身红石强度
            my_rs_line.level = 19 - (io->tick+1)/2;

            my_rs_line.status = RS_LINE_SIG_SENDING;
        }
    }else {
        if(io->tick == RS_TICK_39_ack_read){ // 在 ack 位有引脚被拉低，说明该方向有设备，点亮该方向 led
            for(uint8_t i = 0; i < RS_LINE_CH_NUM; i ++){
                if(io->bitmask_channel_change & (1 << i)){
                    // 设置该方向的 led 亮起
                    my_rs_line.bitmask_connection |= (1 << i);
                }
            }
        }
    }
}

void my_rs_io_callback_communication_over(struct rs_io_stu *io){
    // 更新强度显示
    SEG_LED_show_num(&mySegLED, my_rs_line.level);
    
    // 更新连接状态显示
    for(uint8_t i = 0; i < RS_LINE_CH_NUM; i ++){
        my_rs_line.led_pin_write(i, my_rs_line.bitmask_connection & (1 << i));
    }
}


void my_rs_line_led_pin_write(rs_line_chs_e led_num, uint8_t pin_state){
    scanLED_write_xy(&myScanLED, my_rs_line.leds[led_num].port, my_rs_line.leds[led_num].pin, pin_state);
}
