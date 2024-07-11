/*
 * Author: Christian Storm
 * Copyright (C) 2016, Siemens AG
 *
 * SPDX-License-Identifier:     GPL-2.0-only
 */

#pragma once

#include <network_ipc.h>
#include "util.h"

/* Suricatta Server Interface.
 *
 * Each suricatta server has to implement this interface.
 * Cf. `server_hawkbit.c` for an example implementation targeted towards the
 * [hawkBit](https://projects.eclipse.org/projects/iot.hawkbit) server.
 */
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

bool register_server(const char *name, server_t *server);
