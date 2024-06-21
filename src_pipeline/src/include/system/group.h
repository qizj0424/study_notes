/*
 * IMP Group common function header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Zoro <ykli@ingenic.cn>
 */

#ifndef __GROUP_H__
#define __GROUP_H__

#include <constraints.h>
#include <imp/imp_system.h>

#include <system/module.h>

typedef struct Device Device;
typedef struct Group Group;

struct Group {
	Device *device;
	Module *module;
	int group_index;
	int nr_outputs;
	IMPFrameInfo *for_output_data[NR_MAX_OUTPUT_IN_GROUP];

	int (*OnGroupDataUpdate)(Group *modulex, IMPFrameInfo *data);
};

static inline void group_tick(Group *group)
{
	static int i;
	static void *tmp = &i;
	Module *module = group->module;

	module->Update(module, &tmp);
}

static inline int group_task_left(Group *group)
{
	return nr_module_empty_msg(group->module);
}

static inline int flush_group_tree(Group *group)
{
	return flush_module_tree_sync(group->module);
}


Module *get_module(IMPDeviceID dev_id, int grp_id);
int get_module_location(Module *module, IMPDeviceID *dev_id, int *grp_id);
void clear_all_modules(void);

Group *create_group(IMPDeviceID mod_id, int dev_id,
					const char *mod_name,
					int (*update_cb)(Group *, IMPFrameInfo *));

int destroy_group(Group *group, IMPDeviceID dev_id);

#endif /* __GROUP_H__ */
