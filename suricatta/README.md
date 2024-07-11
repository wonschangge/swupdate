## 1.在 main() 执行之前，hawkbit/lua/general 先注册服务器：

```c
static server_t server = {
	.has_pending_action = &server_has_pending_action,
	.install_update = &server_install_update,
	.send_target_data = &server_send_target_data,
	.get_polling_interval = &server_get_polling_interval,
	.start = &server_start,
	.stop = &server_stop,
	.ipc = &server_ipc,
	.help = &server_print_help,
};

// main函数之前执行
__attribute__((constructor))
static void register_server_general(void)
{
	register_server("general", &server);
}
```

其中服务器实例满足：

```c
typedef struct {
	server_op_res_t (*has_pending_action)(int *action_id); // 正在进行的动作
	server_op_res_t (*install_update)(void); // 安装更新
	server_op_res_t (*send_target_data)(void); // 发送目标数据
	unsigned int (*get_polling_interval)(void); // 获取轮询时间间隔
	server_op_res_t (*start)(const char *fname, int argc, char *argv[]); // 启动
	server_op_res_t (*stop)(void); // 停止
	server_op_res_t (*ipc)(ipc_message *msg); // IPC消息
	void (*help)(void); // 帮助
} server_t; // Suricatta远程服务器必须实现的接口实现函数
```

## 2.swupdate.c 中启动 suricatta 进程

```c
start_subprocess(SOURCE_SURICATTA, "suricatta", uid, gid, // 开启子进程
				 cfgfname, argcount,
				 argvalues, start_suricatta); //
```

## 3.suricatta.c中start_suricatta函数

先设置信号集
再读取传给suricatta的参数
再读取配置文件中的suricatta模块的设置（enable, server）
处理 d(disable), e(enable), S(server type) 参数
初始化suricatta_enable_sema信号量，用于轮询休眠期的唤醒
启动IPC消息线程
进入轮询的死循环

## 4.server_general.c中

由suricatta server->start启动面向通用服务器实现的server_start
读配置文件 gservice模块、identify模块，处理传入的参数
    url/logurl/polldelay, channel通用
解析传入的参数，可覆盖之上的内容
    u: url
    l: logurl
    p: polling_interval
    r: retries
    w: retry_sleep
    n: max_download_speed
    2: cached_file，新的额外补充
    a: custom-http-header，新的额外补充
把 custom-http-header 作为