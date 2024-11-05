/*
 * (C) Copyright 2015-2023
 * Stefano Babic <stefano.babic@swupdate.org>
 *
 * SPDX-License-Identifier:     LGPL-2.1-or-later
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/*
 * This is used to send back the result of an update.
 * It is strictly forbidden to change the order of entries.
 * New values should be put at the end without altering the order.
 */

typedef enum { // 用于送回更新结果。严格禁止更改entries的次序，新值应放到尾部。
	IDLE,			// 空闲
	START,			// 启动
	RUN,			// 运行
	SUCCESS,		// 成功
	FAILURE,		// 失败
	DOWNLOAD,		// 下载
	DONE,			// 完成
	SUBPROCESS,		// 子进程
	PROGRESS,		// 进度
} RECOVERY_STATUS;

typedef enum {
	SOURCE_UNKNOWN,				// 未知	
	SOURCE_WEBSERVER, 			// webserver
	SOURCE_SURICATTA, 			// suricatta
	SOURCE_DOWNLOADER, 			// 普通下载器
	SOURCE_LOCAL,				// 本地
	SOURCE_CHUNKS_DOWNLOADER 	// 块下载器
} sourcetype;	// 子进程类型

#ifdef __cplusplus
}   // extern "C"
#endif
