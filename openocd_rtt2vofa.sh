# 用来方便打开 rtt 并启动一个 OpenOCD 服务器的小脚本
# 因为 git bash 不能直接使用 openocd 命令，所以绕一下，调用 cmd 来执行

cmd //c "openocd -f rtt2vofa.cfg"