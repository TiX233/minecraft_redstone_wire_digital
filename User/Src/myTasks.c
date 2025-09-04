#include "myTasks.h"
#include "rtx.h"
#include "cmdProcess.h"
#include "scan_LED.h"
#include "segment_LED.h"

void task_func_heart_beat(void *param);
void task_func_cmd(void *param);
void task_func_scanLED(void *param);


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
        .tick_counts = 5,
        .tick_reload = 5,

        .topic = &(task_scanLED.topic),
        .next = NULL,
    },

    .subscriber = {
        .callback_func = task_func_scanLED,

        .next = NULL,
    },
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
    // 创建周期心拍任务
    mt_Task_add(&task_heart_beat);

    // 创建命令处理周期任务
    mt_Task_add(&task_cmd);

    // 创建扫描 LED 显示周期任务
    mt_Task_add(&task_scanLED);


    return 0;
}


// 心拍任务
uint32_t heart_beat_count = 0;
void task_func_heart_beat(void *param){

    heart_beat_count ++;
    // LOG_FMT("Heartbeat: %d\n", heart_beat_count);

    static uint8_t i = 5;
    SEG_LED_show_num(&mySegLED, i);
    i = (i+1)%SEG_LED_NUM_MAX;
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
