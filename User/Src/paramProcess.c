#include "paramProcess.h"
#include "myTasks.h"

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

struct param_stu param_list[] = {
    {
        .param_name = "heart_beat_Hz",
        .param_read = param_read_heart_beat,
        .param_write = param_write_heart_beat,
    },

    // 末尾项
    {
        .param_name = " ",
    },
};
