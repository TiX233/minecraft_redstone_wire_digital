#include "paramProcess.h"
#include "myTasks.h"

// 心拍频率
uint16_t heart_beat_Hz = 1;
void param_read_heart_beat(struct param_stu *param){
    LOG_FMT(PRINT_LOG"Heart beat: %d Hz\n", heart_beat_Hz);
}
void param_write_heart_beat(struct param_stu *param, const char *new_val){
    uint16_t new_Hz;

    sscanf(new_val, "%d", &new_Hz);

    if(new_Hz > 1000 || new_Hz < 1){
        LOG_FMT(PRINT_WARNNING"New value(%d) not in range(1~1000)!\n", new_Hz);
        return ;
    }

    heart_beat_Hz = 1000/new_Hz;

    task_heart_beat.timer.tick_reload = heart_beat_Hz;
    task_heart_beat.timer.tick_counts = heart_beat_Hz;

    LOG_FMT(PRINT_LOG"Set heart beat to %d Hz\n", heart_beat_Hz);
}

// led 扫描停留周期
void param_read_scan_period(struct param_stu *param){
    LOG_FMT(PRINT_LOG"Scan period: %d\n", task_scanLED.timer.tick_reload);
}
void param_write_scan_period(struct param_stu *param, const char *new_val){
    uint16_t new_period;

    sscanf(new_val, "%d", &new_period);

    if(new_period < 1){
        LOG_FMT(PRINT_WARNNING"New value(%d) not in range(>=1)!\n", new_period);
        return ;
    }

    task_scanLED.timer.tick_reload = new_period;
    task_scanLED.timer.tick_counts = new_period;

    LOG_FMT(PRINT_LOG"Set heart beat to %d ms\n", new_period);

}

struct param_stu param_list[] = {
    {
        .param_name = "heart_beat_Hz",
        .param_read = param_read_heart_beat,
        .param_write = param_write_heart_beat,
    },

    {
        .param_name = "scan_period",
        .param_read = param_read_scan_period,
        .param_write = param_write_scan_period,
    },

    // 末尾项
    {
        .param_name = " ",
    },
};
