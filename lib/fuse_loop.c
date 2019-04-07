/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  Implementation of the single-threaded FUSE session loop.

  This program can be distributed under the terms of the GNU LGPLv2.
  See the file COPYING.LIB
*/

#include "config.h"
#include "fuse_lowlevel.h"
#include "fuse_kernel.h"
#include "fuse_i.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int fuse_session_loop(struct fuse_session *se)
{
	int res = 0;
	struct fuse_buf fbuf = {
		.mem = NULL,
	};

	while (!fuse_session_exited(se)) {
		res = fuse_session_receive_buf_int(se, &fbuf, NULL);

		if (res == -EINTR)
			continue;
		if (res <= 0)
			break;
               
		int count = 0;
                struct fuse_in_header *in_temp = NULL;
                in_temp = fbuf.mem;
                
                int i = 0;
                void *param2 = fbuf.mem;
                struct fuse_in_header *param3 = (struct fuse_in_header *)param2;

                while(count < res) {
                        fuse_session_process_buf_int(se, &fbuf, NULL);
                        param2 = param2 + param3->len;
                        count = count + param3->len;
                        param3 = (struct fuse_in_header *)param2;
                        fbuf.mem = param2;                   
                        i = i + 1;
                }               

                printf("Total requests: %d\n", i);
                printf("Count: %d\n", count);

                fbuf.mem = in_temp;

	}

	free(fbuf.mem);
	if(res > 0)
		/* No error, just the length of the most recently read
		   request */
		res = 0;
	if(se->error != 0)
		res = se->error;
	fuse_session_reset(se);
	return res;
}
