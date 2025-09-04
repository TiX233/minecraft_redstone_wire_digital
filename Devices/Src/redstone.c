#include "redstone.h"


int rs_io_communication_start(struct rs_io_stu *io){
    if(io->status != RS_IO_STATE_idle){
        return -1;
    }

    io->status = RS_IO_STATE_busy;
    io->tick = RS_TICK_0_start;

    io->callback_communication_init(io);

    return 0;
}

void rs_io_maintainer(struct rs_io_stu *io){
    uint8_t flag_channel_change = 0;
    uint8_t flag_communication_over = 0;
    io->bitmask_channel_change = 0;

    for(uint8_t i = 0; i < io->channel_num; i ++){
        if(io->channels[i].status != RS_CHANNEL_STATE_broken){
            switch(io->tick){
                case RS_TICK_0_start:
                    io->pin_write(&(io->channels[i]), 1);
                    io->channels[i].status = RS_CHANNEL_STATE_start;
                    break;

                case RS_TICK_1_check_wire:
                    io->channels[i].status = RS_CHANNEL_STATE_check;
                    if(!(io->pin_read(&(io->channels[i])))){ // 信号线低电平，认为线路出现问题，屏蔽
                        io->channels[i].status = RS_CHANNEL_STATE_broken;
                    }
                    io->channels[i].status = RS_CHANNEL_STATE_trans;
                    break;
                    
                case RS_TICK_2_r18_set:
                case RS_TICK_4_r17_set:
                case RS_TICK_6_r16_set:
                case RS_TICK_8_r15_set:
                case RS_TICK_10_r14_set:
                case RS_TICK_12_r13_set:
                case RS_TICK_14_r12_set:
                case RS_TICK_16_r11_set:
                case RS_TICK_18_r10_set:
                case RS_TICK_20_r9_set:
                case RS_TICK_22_r8_set:
                case RS_TICK_24_r7_set:
                case RS_TICK_26_r6_set:
                case RS_TICK_28_r5_set:
                case RS_TICK_30_r4_set:
                case RS_TICK_32_r3_set:
                case RS_TICK_34_r2_set:
                case RS_TICK_36_r1_set:
                case RS_TICK_38_ack_set:

                    io->pin_write(&(io->channels[i]), io->channels[i].pin_script[io->tick/2]);

                    break;

                case RS_TICK_3_r18_read:
                case RS_TICK_5_r17_read:
                case RS_TICK_7_r16_read:
                case RS_TICK_9_r15_read:
                case RS_TICK_11_r14_read:
                case RS_TICK_13_r13_read:
                case RS_TICK_15_r12_read:
                case RS_TICK_17_r11_read:
                case RS_TICK_19_r10_read:
                case RS_TICK_21_r9_read:
                case RS_TICK_23_r8_read:
                case RS_TICK_25_r7_read:
                case RS_TICK_27_r6_read:
                case RS_TICK_29_r5_read:
                case RS_TICK_31_r4_read:
                case RS_TICK_33_r3_read:
                case RS_TICK_35_r2_read:
                case RS_TICK_37_r1_read:
                case RS_TICK_39_ack_read:

                    if(io->pin_read(&(io->channels[i])) != io->channels[i].pin_script[io->tick/2]){ // 信号线电平与脚本不一致
                        if(io->channels[i].pin_script[io->tick/2]){ // 如果脚本为高电平，说明有其他设备拉低
                            flag_channel_change = 1;
                            io->bitmask_channel_change |= (1 << i);
                        }else { // 脚本电平为低电平，但是信号线却没有被强下拉拉低，认为线路故障
                            io->pin_write(&(io->channels[i]), 1);

                            io->channels[i].status = RS_CHANNEL_STATE_broken;
                        }
                    }
                    break;
                    
                case RS_TICK_40_wire_release: // 通信完成，释放线路，并发送通知

                    io->pin_write(&(io->channels[i]), 1);
                    flag_communication_over = 1;

                    break;

                
                default:

                    if(io->tick > RS_TICK_TIME_OUT){ // 超时未检测到时钟

                    }

                    break;
            }
        }
    }

    if(flag_channel_change){ // 有一个或几个信号线发生变化，调用回调处理
        io->callback_channel_change(io);
    }

    if(flag_communication_over){
        io->status = RS_IO_STATE_idle;
        io->callback_communication_over(io);
    }
    
    io->tick ++;
}
