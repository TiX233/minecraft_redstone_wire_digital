#include "myTasks.h"
#include "rtx.h"
#include "cmdProcess.h"
#include "scan_LED.h"
#include "segment_LED.h"
#include "redstone_line.h"

void task_func_heart_beat(void *param);
void task_func_cmd(void *param);
void task_func_scanLED(void *param);
void task_func_rs_io(void *param);

void rs_clk_cb(void *param);


// 心拍周期任务对象
struct mt_Task_stu task_heart_beat = {
    .topic = {
        .flag = 0,
        .subscriber = NULL,

        .next = NULL,
    },

    .timer = {
        .tick_counts = 1000,
        .tick_reload = 1000,

        .topic = &(task_heart_beat.topic),
        .next = NULL,
    },

    .subscriber = {
        .callback_func = task_func_heart_beat,

        .next = NULL,
    },
};


// 命令处理周期任务对象
struct mt_Task_stu task_cmd = {
    .topic = {
        .flag = 0,
        .subscriber = NULL,

        .next = NULL,
    },

    .timer = {
        .tick_counts = 200,
        .tick_reload = 200,

        .topic = &(task_cmd.topic),
        .next = NULL,
    },

    .subscriber = {
        .callback_func = task_func_cmd,

        .next = NULL,
    },
};


// 扫描 LED 显示周期任务对象
struct mt_Task_stu task_scanLED = {
    .topic = {
        .flag = 0,
        .subscriber = NULL,

        .next = NULL,
    },

    .timer = {
        .tick_counts = 1,
        .tick_reload = 1, // 大于 4 相机拍会有频闪，，大于 8 会被肉眼看出闪烁

        .topic = &(task_scanLED.topic),
        .next = NULL,
    },

    .subscriber = {
        .callback_func = task_func_scanLED,

        .next = NULL,
    },
};


// 红石 io 管理器周期任务对象
struct mt_Task_stu task_rs_io = {
    .topic = {
        .flag = 0,
        .subscriber = NULL,

        .next = NULL,
    },

    .timer = {
        .tick_counts = 1,
        .tick_reload = 1,

        .topic = &(task_rs_io.topic),
        .next = NULL,
    },

    .subscriber = {
        .callback_func = task_func_rs_io,

        .next = NULL,
    },
};


// 红石时钟信号上升沿事件
struct rtx_Topic_stu topic_clk = {
    .flag = 0,
    .subscriber = NULL,

    .next = NULL,
};

// 红石时钟事件订阅者结构体
struct rtx_Topic_subscriber_stu subscriber_clk = {
    .callback_func = rs_clk_cb,

    .next = NULL,
};


// 添加周期任务 API
int mt_Task_add(struct mt_Task_stu *task){
    rtx_Topic_add(&(task->topic));
    rtx_Topic_subscribe(&(task->topic), &(task->subscriber));
    rtx_Timer_add(&(task->timer));

    return 0;
}

// -----------------初始化自定义任务----------------
int myTask_init(void){
    // 创建心拍周期任务
    mt_Task_add(&task_heart_beat);

    // 创建命令处理周期任务
    mt_Task_add(&task_cmd);

    // 创建扫描 LED 显示周期任务
    mt_Task_add(&task_scanLED);
    
    // 创建红石通信管理器周期任务
    mt_Task_add(&task_rs_io);

    // 绑定红石时钟信号事件
    rtx_Topic_add(&topic_clk);
    rtx_Topic_subscribe(&topic_clk, &subscriber_clk);

    return 0;
}


// 心拍任务
uint32_t heart_beat_count = 0;
void task_func_heart_beat(void *param){

    heart_beat_count ++;
    // LOG_FMT("Heartbeat: %d\n", heart_beat_count);

    // static uint8_t i = 5;
    // SEG_LED_show_num(&mySegLED, i);
    // i = (i+1)%SEG_LED_NUM_MAX;
}


uint8_t cmd_buffer[CMD_BUF_SIZE];
// 处理命令任务
void task_func_cmd(void *param){

    // 读取命令
    if(SEGGER_RTT_HasData(0)){
        int len = SEGGER_RTT_Read(0, cmd_buffer, CMD_BUF_SIZE - 1);
        if (len > 0) {
            cmd_buffer[len] = '\0'; // 添加字符串终止符

            // LOG_FMT(PRINT_DEBUG"cmd len: %d\n", len);
            cmd_process((char *)cmd_buffer); // 处理命令
        }
    }
}


// 扫描 LED 显示任务
void task_func_scanLED(void *param){
    scanLED_scan(&myScanLED);
}


// 红石 io 管理器周期任务
void task_func_rs_io(void *param){
    rs_io_maintainer(&(my_rs_line.io));
}


// 红石时钟处理
void rs_clk_cb(void *param){
    // 设置通信开始
    rs_io_communication_start(&(my_rs_line.io));
}
