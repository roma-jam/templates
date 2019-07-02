/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RJ (jam_roma@yahoo.com)
*/

#include <string.h>
#include "fs.h"
#include "app_private.h"
#include "config.h"
#include "../../rexos/userspace/storage.h"
#include "../../rexos/userspace/stdio.h"
#include "../../rexos/userspace/vfs.h"

static inline bool fs_open_volume(APP* app)
{
    VFS_VOLUME_TYPE volume;

    volume.hal = HAL_FLASH;
    volume.process = KERNEL_HANDLE;
    volume.user = 0;
    volume.first_sector = 0;
    volume.sectors_count = FLASH_SECTORS_COUNT;
    volume.sector_mode = SECTOR_MODE_BER;

    return vfs_open_volume(&app->fs.vfs_record, &volume);
}

static inline bool fs_open_ber(APP* app)
{
    if (!vfs_open_ber(&app->fs.vfs_record, FS_BLOCK_SECTORS))
    {
        return false;
    }

    app->fs.ber_mounted = true;
    return true;
}

static void fs_close_ber(APP* app)
{
    if (app->fs.ber_mounted)
    {
        vfs_close_ber(&app->fs.vfs_record);
        app->fs.ber_mounted = false;
    }
}

static bool fs_mount(APP* app)
{
    if (!vfs_open_fs(&app->fs.vfs_record))
    {
        return false;
    }

    app->fs.mounted = true;
    return true;
}

static bool fs_ber_format_internal(APP* app)
{
    VFS_BER_FORMAT_TYPE format;
    format.block_sectors = FS_BLOCK_SECTORS;
    format.fs_blocks = FS_BLOCKS_COUNT;
    return vfs_format_ber(&app->fs.vfs_record, &format);
}

static bool fs_format_internal(APP* app)
{
    bool res;
    SYSTIME uptime;
    VFS_FAT_FORMAT_TYPE format;
    format.fat_count = 1;
    format.cluster_sectors = FS_BLOCK_SECTORS;
    format.root_entries = 64;
    format.serial = 0x43584839;
    strcpy(format.label, "TEST1");
    get_uptime(&uptime);
    res = vfs_format(&app->fs.vfs_record, &format);
    printf("Format time: %d ms\n", systime_elapsed_ms(&uptime));
    return res;
}

static inline void fs_info(APP* app)
{
    HANDLE fh;
    VFS_FIND_TYPE* find;
    char* label;
    label = vfs_read_volume_label(&app->fs.vfs_record);
    if (label)
        printf("FAT volume label: %s\n", label);
    else
        printf("FAT has no label\n");
    printf("FAT root list: \n");
    printf("----------------\n");
    vfs_cd(&app->fs.vfs_record, VFS_ROOT);
    fh = vfs_find_first(&app->fs.vfs_record);
    if (fh != INVALID_HANDLE)
    {
        while (vfs_find_next(&app->fs.vfs_record, fh))
        {
            find = io_data(app->fs.vfs_record.io);
            if (find->attr & VFS_ATTR_FOLDER)
                putc('d');
            if (find->attr & VFS_ATTR_HIDDEN)
                putc('h');
            if (find->attr & VFS_ATTR_READ_ONLY)
                putc('r');
            if (find->attr)
                putc(' ');
            printf("%s", find->name);
            if ((find->attr & VFS_ATTR_FOLDER) == 0)
                printf(" %d", find->size);
            printf("\n");
        }
        vfs_find_close(&app->fs.vfs_record, fh);
    }
    printf("----------------\n");
}

void fs_init(APP* app)
{
    app->fs.io = io_create(512 + sizeof(STORAGE_STACK));
    app->fs.vfs = vfs_create(VFS_PROCESS_SIZE, VFS_PROCESS_PRIORITY);
    app->fs.active = app->fs.ber_mounted = app->fs.mounted = false;
    vfs_record_create(app->fs.vfs, &app->fs.vfs_record);

    if (!storage_open(HAL_FLASH, KERNEL_HANDLE, 0))
    {
        printf("FS: storage open error %d\n", get_last_error());
        return;
    }

    if (!fs_open_volume(app))
    {
        printf("FS: open volume error %d\n", get_last_error());
        return;
    }

    if (!fs_open_ber(app))
    {
        printf("FS: open ber error %d\n", get_last_error());

        if(!fs_ber_format_internal(app))
        {
            printf("FS: ber format error %d\n", get_last_error());
            return;
        }

        if(!fs_open_ber(app))
        {
            printf("FS: second open ber error %d\n", get_last_error());
            return;
        }
    }

    if (!fs_mount(app))
    {
        printf("FS: mount error %d\n", get_last_error());

        if(!fs_format_internal(app))
        {
            printf("FS: format error %d\n", get_last_error());
            return;
        }

        if(!fs_mount(app))
        {
            printf("FS: second mount error %d\n", get_last_error());
            return;
        }
    }

    fs_info(app);
}
