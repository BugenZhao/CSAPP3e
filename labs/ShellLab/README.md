## 要点

- 根据 `tracexx.txt` 的顺序逐步实现功能。

- 在 `eval` 中，调用 `fork()` 以及 `addjob()` 之前应屏蔽 `SIGCHLD` 信号：`sigprocmask(SIG_BLOCK, &mask_chld, &prev)`，避免在 `addjob()` 之前就调用了 `deletejob()`，`fork()` 后恢复之前的 mask ：`sigprocmask(SIG_SETMASK, &prev, NULL)`；同时，fork 出的子进程由于继承了父进程的内存映像，也需要恢复。

- 按照 Writeup 的要求，`waitfg` 中不调用 `waitpid()`, 直接通过循环调用 `sleep` 等待接收到 `SIGCHLD` 后 `handler` 回收子进程。

- `sigchld_handler` 中使用 `pid = waitpid(-1, &status, WNOHANG | WUNTRACED)` 回收， 其效果是 "*立即返回*，若有子进程停止或终止，返回其 PID，否则返回 0 "，因此可以通过 while 循环调用回收所有僵死进程。涉及到全局数据结构的读写时，临时屏蔽所有信号。

- `sigint_handler` 和 `sigtstp_handler` 中，应注意使用 `kill(-pid, SIGXXXX)` 将信号转发到整个任务组。

- 在真正的 shell 中运行 `tsh` 时，`tsh` 和启动的所有任务属于一个进程组，`sigint_handler` 转发信号到进程组时会导致自身也被 kill 掉，因此要在 `eval` 中子进程 `exec` 前调用 `setpgid(0, 0)` 设置新的进程组。

- `do_bgfg` 中，切换前台后台同时要向整个进程组发送 `SIGCONT` 信号。

- `execve` 不在 PATH 中寻找程序名， `execvp` 可以，但不接收 `envrion` 参数。Linux 中有非标准函数 `execvpe` 可以实现，考虑到 BSD/Darwin 的兼容性，不再追究此细节了。

- 关于异步信号安全可能还有诸多细节值得继续思考。其他要点请见代码注释。