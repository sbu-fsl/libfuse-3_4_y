/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  Implementation of the single-threaded FUSE session loop.

  This program can be distributed under the terms of the GNU LGPLv2.
  See the file COPYING.LIB
*/

#include "config.h"
#include "fuse_lowlevel.h"
#include "fuse_i.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

int fuse_session_loop(struct fuse_session *se)
{
	int res = 0, fd;
	ssize_t ret = 0;
	double total_time = 0;
	clock_t begin = 0, end;
	char str[20];
	struct fuse_buf fbuf = {
		.mem = NULL,
	};

	begin = clock();
	while (!fuse_session_exited(se)) {
		res = fuse_session_receive_buf_int(se, &fbuf, NULL);

		if (res == -EINTR)
			continue;
		if (res <= 0)
			break;

		fuse_session_process_buf_int(se, &fbuf, NULL);
	}
	end = clock();
	total_time = (double)(end - begin) / CLOCKS_PER_SEC;

	free(fbuf.mem);
	if(res > 0)
		/* No error, just the length of the most recently read
		   request */
		res = 0;
	if(se->error != 0)
		res = se->error;
	fuse_session_reset(se);

	fd = open("/root/new_clone/libfuse-3_4_y/lib/run_time.out", O_APPEND | O_WRONLY);
	snprintf(str, 20, "%lf", total_time);
	ret = pwrite(fd, str, strlen(str), 0);
	
	if (ret < 0)
		printf("write to result file failed\n");
	if (ret < strlen(str))
		printf("write to result file partially failed\n");
	close(fd);

	return res;
}
