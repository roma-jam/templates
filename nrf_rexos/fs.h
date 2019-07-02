/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RJ (jam_roma@yahoo.com)
*/

#ifndef FS_H_
#define FS_H_

#include "app.h"
#include "config.h"
#include "../../rexos/userspace/io.h"
#include "../../rexos/userspace/systime.h"
#include "../../rexos/userspace/vfs.h"

typedef struct {
    HANDLE vfs;
    VFS_RECORD_TYPE vfs_record;
    IO* io;
    bool active, mounted, ber_mounted;
} FS_TYPE;


void fs_init(APP* app);

#endif /* FS_H_ */
