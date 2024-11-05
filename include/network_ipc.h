/*
 * (C) Copyright 2013-2023
 * Stefano Babic <stefano.babic@swupdate.org>
 *
 * SPDX-License-Identifier:     LGPL-2.1-or-later
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "swupdate_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Be careful to include further headers here. This file is the interface
 * to external programs interfacing with SWUpdate as client, and further
 * headers are not exported.
 */

#define IPC_MAGIC		0x14052001 // 统一的内部IPC消息魔数

typedef enum {
	REQ_INSTALL,									// 请求安装
	ACK,											// 确认
	NACK,											// 未确认
	GET_STATUS,										// 获取状态
	POST_UPDATE,									// 投递更新
	SWUPDATE_SUBPROCESS,							// 子进程
	SET_AES_KEY,									// 设置AES密钥
	SET_UPDATE_STATE,	/* set bootloader ustate */ // 设置更新状态
	GET_UPDATE_STATE,								// 获取更新状态
	REQ_INSTALL_EXT,								// 请求安装EXT
	SET_VERSIONS_RANGE,								// 设置版本区间
	NOTIFY_STREAM,									// 通知流
	GET_HW_REVISION,								// 获取硬件修订
	SET_SWUPDATE_VARS,								// 设置变量
	GET_SWUPDATE_VARS,								// 获取变量
} msgtype; // 消息类型

/*
 * Commands are used for IPC to subprocesses. The meaning is then interpreted
 * by the single subprocess
 */
enum { // IPC消息指令类型
	CMD_ACTIVATION,	/* this returns the answer if a SW can be activated */
	CMD_CONFIG, // 配置
	CMD_ENABLE,	/* Enable or disable suricatta mode */ // 启用suricatta模式
	CMD_GET_STATUS, // 获取状态
	CMD_SET_DOWNLOAD_URL // 设置下载URL
};

enum run_type {
	RUN_DEFAULT,
	RUN_DRYRUN,
	RUN_INSTALL
};

#define SWUPDATE_API_VERSION 	0x1
/*
 * Install structure to be filled before calling
 * ipc and async functions
 */
struct swupdate_request { // 自定义IPC的请求
	unsigned int apiversion;	// api版本
	sourcetype source;			// IPC
	enum run_type dry_run;		// 运行类型
	size_t len;					// 长度
	char info[512];				// 信息
	char software_set[256];		// 
	char running_mode[256];		// 运行模式
	bool disable_store_swu;		// 禁用存储
};

typedef union { // 联合类型
	char msg[128]; // 类型1: 128字长消息
	struct { 
		int current;
		int last_result;
		int error;
		char desc[2048];
	} status;	   // 类型2: 状态
	struct {
		int status;
		int error;
		int level;
		char msg[2048];
	} notify;	   // 类型3: 通知
	struct {
		struct swupdate_request req;
		unsigned int len;    /* Len of data valid in buf */
		char	buf[2048];   /*
				      * Buffer that each source can fill
				      * with additional information
				      */
	} instmsg;	   // 类型4: UDS插入消息
	struct {
		sourcetype source; /* Who triggered the update */
		int	cmd;	   /* Optional encoded command */
		int	timeout;     /* timeout in seconds if an aswer is expected */
		unsigned int len;    /* Len of data valid in buf */
		char	buf[2048];   /*
				      * Buffer that each source can fill
				      * with additional information
				      */
	} procmsg;     // 类型5: 进程消息
	struct {
		char key_ascii[65]; /* Key size in ASCII (256 bit, 32 bytes bin) + termination */
		char ivt_ascii[33]; /* Key size in ASCII (16 bytes bin) + termination */
	} aeskeymsg;   // 类型6: AES密钥消息
	struct {
		char minimum_version[256];
		char maximum_version[256];
		char current_version[256];
	} versions;    // 类型7: 版本信息
	struct {
		char boardname[256];
		char revision[256];
	} revisions;   // 类型8: 修订版信息
	struct {
		char varnamespace[256];
		char varname[256];
		char varvalue[256];
	} vars;        // 类型9: 变量
} msgdata;
	
typedef struct {
	int magic;	/* magic number */	// 自定义IPC消息的魔数
	int type;						// 自定义IPC消息的类型
	msgdata data;					// 自定义IPC消息的数据
} ipc_message; // 自定义的IPC消息

char *get_ctrl_socket(void);
int ipc_inst_start(void);
int ipc_inst_start_ext(void *priv, ssize_t size);
int ipc_send_data(int connfd, char *buf, int size);
void ipc_end(int connfd);
int ipc_get_status(ipc_message *msg);
int ipc_get_status_timeout(ipc_message *msg, unsigned int timeout_ms);
int ipc_notify_connect(void);
int ipc_notify_receive(int *connfd, ipc_message *msg);
int ipc_postupdate(ipc_message *msg);
int ipc_send_cmd(ipc_message *msg);

typedef int (*writedata)(char **buf, int *size);
typedef int (*getstatus)(ipc_message *msg);
typedef int (*terminated)(RECOVERY_STATUS status);
int ipc_wait_for_complete(getstatus callback);
void swupdate_prepare_req(struct swupdate_request *req);
int swupdate_image_write(char *buf, int size);
int swupdate_async_start(writedata wr_func, getstatus status_func,
				terminated end_func,
				void *priv, ssize_t size);
int swupdate_set_aes(char *key, char *ivt);
int swupdate_set_version_range(const char *minversion,
				const char *maxversion,
				const char *currentversion);
#ifdef __cplusplus
}   // extern "C"
#endif
