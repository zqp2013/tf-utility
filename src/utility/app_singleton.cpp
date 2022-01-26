/*
 * app_singleton.cpp
 *
 *  Created on: May 8, 2020
 *      Author: root
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utility/app_singleton.h"

#define kPidFileName "app.pid"

TF_EXT_CLASS bool enter_app_singleton() {
	int fd = open(kPidFileName, O_RDWR | O_TRUNC);
	if (fd == -1) {
		//对应的锁文件当前不存在，试图创建该锁文件
		fd = creat(kPidFileName, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		if (fd > 0) {
			fchmod(fd, S_IRUSR|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
			//printf("chmod return code is %d\n", retCode);
		} else {
			fd = open(kPidFileName, O_RDWR | O_TRUNC);
		}
	}
	if (fd < 0) {
		printf("Open file failed, error : %s", strerror(errno));
		exit(1);
	}

	// 将该文件锁定,锁定后的文件将不能够再次锁定
	struct flock fl;
	fl.l_type = F_WRLCK; // 写文件锁定
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	int ret = fcntl(fd, F_SETLK, &fl);
	if (ret < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			printf("%s already locked, error: %s\n", kPidFileName, strerror(errno));
			close(fd);
			exit(1);
		}
	}

	// 锁定文件后，将该进程的pid写入文件
	char buf[16] = {};
	sprintf(buf, "%d", getpid());
	ftruncate(fd, 0);
	ret = write(fd, buf, strlen(buf));
	if (ret < 0) {
		printf("Write file failed, file: %s, error: %s\n", kPidFileName, strerror(errno));
		close(fd);
		exit(1);
	}

	// 函数返回时不需要调用close(fd),不然文件锁将失效
	// 程序退出后kernel会自动close
	return true;
}
