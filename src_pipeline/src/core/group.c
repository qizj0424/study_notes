/*
 * Ingenic IMP group.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Zoro <ykli@ingenic.cn>
 */

#include <constraints.h>
#include <imp/imp_log.h>
#include <imp/imp_system.h>
#include <system/module.h>
#include <system/group.h>
#include <system/vbm.h>
#include <system/device.h>
#include <system/system.h>
#ifdef DMALLOC
#include <imp/dmalloc.h>
#endif
#define TAG "Group"

static Module *g_modules[NR_MAX_DEVICES][NR_MAX_GROUPS_IN_DEV];

Module *get_module(IMPDeviceID dev_id, int grp_id)
{
	return g_modules[dev_id][grp_id];
}

int get_module_location(Module *module, IMPDeviceID *dev_id, int *grp_id)
{
	int i, j;

	for (i = 0; i < NR_MAX_DEVICES; i++) {
		for (j = 0; j < NR_MAX_GROUPS_IN_DEV; j++)
			if (g_modules[i][j] == module) {
				if (dev_id)
					*dev_id = i;

				if (grp_id)
					*grp_id = j;

				return 0;
			}
	}

	return -1;
}

void clear_all_modules(void)
{
	int i, j;
	for (i = 0; i < NR_MAX_DEVICES; i++) {
		for (j = 0; j < NR_MAX_GROUPS_IN_DEV; j++)
			g_modules[i][j] = NULL;
	}
}

static int is_last_node(Group *group)
{
  Module *module = group->module;

  if (module->num_of_observer == 0)
    return 1;

  return 0;
}

static int group_update(void *m, void *data)
{
	Group *group = (Group *)m;
	IMPFrameInfo *frame = (IMPFrameInfo *)data;
	int ret = 0;

	ret = group->OnGroupDataUpdate(group, frame);

	/* If FrameSource update error, it doesn't need release frame because of it hasn't been lock yet. */
	if (((ret < 0) || is_last_node(group)) && ((group->device->dev_id != DEV_ID_FS) && (group->device->dev_id != DEV_ID_EMU_FS))) {
		if (VBMReleaseFrame(frame) < 0) {
			printf("%s(%d)VBMReleaseFrame failed, group->module->name = %s, frame->pool_idx=%d, frame->index=%d\n", __func__, __LINE__, group->module->name, frame->pool_idx, frame->index);
		}
	}

	return ret;
}

Group *create_group(IMPDeviceID dev_id, int grp_id,
					const char *mod_name,
					int (*update_cb)(Group *, IMPFrameInfo *))
{
	Group *group;

	AllocModuleHelper(group, mod_name, Group);
	SetUpdateCallback(group->module, group_update);

	group->OnGroupDataUpdate = update_cb;
	group->group_index = grp_id;

	g_modules[dev_id][grp_id] = group->module;

	return group;
}

int destroy_group(Group *group, IMPDeviceID dev_id)
{
	g_modules[dev_id][group->group_index] = NULL;
	FreeModule(group->module);

	return 0;
}

int debug_func()
{
        printf("-- [%s,%d] --\n",__func__,__LINE__);
        return 0;

}
