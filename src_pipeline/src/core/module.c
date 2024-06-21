/*
 * module.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Zoro <ykli@ingenic.cn>
 */

#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/prctl.h>

#include <imp/imp_log.h>

#include <system/group.h>
#include <system/device.h>
#include <system/module.h>
#include <system/system.h>
#include <system/vbm.h>
#ifdef DMALLOC
#include <imp/dmalloc.h>
#endif
#define TAG "Module"
#define SHOW_MODULE_TIME

static int add_observer(Module *modsub, Module *modob, void **data)
{
	int i;

	for (i = 0; i < NR_MAX_OBSERVERS; i++) {
		if (modsub->observerList[i].module == NULL) {
			modsub->observerList[i].module = modob;
			modsub->observerList[i].data = data;
			modsub->num_of_observer++;
			modob->subjectModule = modsub;
			return 0;
		}
	}

	printf("%s %s error: Can't add more observers\n", modsub->name, __func__);

	return -1;
}

static int remove_observer(Module *modsub, Module *modob)
{
	int i;

	for (i = 0; i < NR_MAX_OBSERVERS; i++) {
		if (modsub->observerList[i].module == modob) {
			modsub->observerList[i].module = NULL;
			modsub->observerList[i].data = NULL;
			modsub->num_of_observer--;
			modob->subjectModule = NULL;
			return 0;
		}
	}

	printf("%s %s error: Can't find observer: %s\n",
		   modsub->name, __func__, modob->name);

	return -1;
}

static int notify_observers(Module *modsub)
{
	int i;
	//Group * grpsub = module_pri(modsub);
	//int dev_id  = grpsub->device->dev_id;

	for (i = 0; i < modsub->num_of_observer; i++) {
		int ret;
		Module *modob = modsub->observerList[i].module;
		if ((modsub->observerList[i].data != NULL)
			&& ((*modsub->observerList[i].data) != NULL)) {
			ret = modob->Update(modob, modsub->observerList[i].data);
			if (ret < 0) {
				VBMUnLockFrame((IMPFrameInfo *)(*(modsub->observerList[i].data)));
				printf("%s update failed, frame->pool_idx=%d, frame->index = %d\n", modob->name, ((IMPFrameInfo *)(*(modsub->observerList[i].data)))->pool_idx, ((IMPFrameInfo *)(*(modsub->observerList[i].data)))->index);
				continue;
			}
			//printf("%s notify %s sucess\n", modsub->name, modob->name);
		}
	}

	return 0;
}

static int write_msg(Module *module, void *data)
{
	sem_wait(&module->sem_msg);

	pthread_mutex_lock(&module->mutex);
	module->msg_head->msg = data;

	module->msg_head = module->msg_head->next;
	module->num_msgs_in_queue++;

	pthread_mutex_unlock(&module->mutex);

	return 0;
}

static void *read_msg(Module *module)
{
	void *data;

	pthread_mutex_lock(&module->mutex);
	if (module->num_msgs_in_queue == 0) {
		printf("read msg error: Empty\n");
		pthread_mutex_unlock(&module->mutex);
		return NULL;
	}
	data = module->msg_tail->msg;

	module->msg_tail = module->msg_tail->next;
	module->num_msgs_in_queue--;
	pthread_mutex_unlock(&module->mutex);

	sem_post(&module->sem_msg);

	return data;
}

static int update(Module *modob, void **data)
{
	int ret;

	if (*data == NULL)
		return 0;

	if (modob->num_msgs_in_queue < 8) {
		ret = write_msg(modob, *data);
		if (ret < 0)
			return ret;

		sem_post(&modob->sem);
	}

	return 0;
}

static void* module_thread(void *m)
{
	Module *module = (Module *)m;
	int ret = -1;
	/* Rename thread name*/
	prctl(PR_SET_NAME, (unsigned long)module->name);

	while (1) {
		module->in_process = 0;
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		sem_wait(&module->sem);
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		module->in_process = 1;

		void *p = read_msg(module);

		if (module->OnDataUpdate) {
			ret = module->OnDataUpdate(module_pri(module), p);
			if (ret >= 0) {
				module->NotifyObservers(module);
			}
			/* for debug */
			module->update_cnt++;
		}
	}

	return 0;
}

Module *AllocModule(const char *name, int extras)
{
	Module *module;
	int i, ret;

	module = malloc(sizeof(struct Module) + extras);
	if (module == NULL) {
		printf("malloc module error\n");
		return NULL;
	}
	memset(module, 0, sizeof(struct Module) + extras);

	i = strlen(name);
	if (i > MAX_MODULE_NAME_LEN) {
		printf("The length of name %d is longer that %d\n",
			   i, MAX_MODULE_NAME_LEN);
		goto free;
	}
	strcpy(module->name, name);

	/* Observer initialize */
	module->num_of_observer = 0;
	for (i = 0; i < NR_MAX_OBSERVERS; i++) {
		module->observerList[i].module = NULL;
		module->observerList[i].data = NULL;
	}
	module->subjectModule = NULL;
	sem_init(&module->sem, 0, 0);
	sem_init(&module->sem_msg, 0, MAX_MODULE_MSG_DEPTH);

	ret = pthread_mutex_init(&module->mutex, NULL);
	if (ret != 0) {
		printf("pthread_mutex_init() error\n");
	}

	ret = pthread_create(&module->tid, NULL, module_thread, module);
	if (ret) {
		printf("module_thread module->name=%s create error\n", module->name);
		goto free;
	}

	module->AddObserver = add_observer;
	module->RemoveObserver = remove_observer;
	module->NotifyObservers = notify_observers;
	module->Update = update;

	/* Message queue initialize */
	for (i = 0; i < MAX_MODULE_MSG_DEPTH; i++) {
		struct msg_list *msg_tmp = &module->msg_queue[i];
		msg_tmp->msg = NULL;
		msg_tmp->next = &module->msg_queue[i + 1];
		if (i == (MAX_MODULE_MSG_DEPTH - 1))
			msg_tmp->next = &module->msg_queue[0];
	}
	module->msg_head = &module->msg_queue[0];
	module->msg_tail = &module->msg_queue[0];
	module->num_msgs_in_queue = 0;

	return module;
free:
	free(module);

	return NULL;
}

void FreeModule(Module *module)
{
	Group *group = module_pri(module);
	int ret = pthread_cancel(module->tid);
	if (ret != 0)
		printf("%s: pthread_cancel error: %s\n", __func__, strerror(errno));

	pthread_join(module->tid, NULL);
	pthread_mutex_destroy(&module->mutex);
	sem_destroy(&module->sem);
	sem_destroy(&module->sem_msg);
	if ((((int)group->device->dev_id != (int)DEV_ID_FS)
			&& ((int)group->device->dev_id != (int)DEV_ID_EMU_FS))) {
		int i = 0;
		for (i = 0; i < module->num_msgs_in_queue; i++) {
			IMPFrameInfo *frame = NULL;
			frame = module->msg_tail->msg;
			module->msg_tail = module->msg_tail->next;
			module->num_msgs_in_queue--;
			VBMUnLockFrame(frame);
		}
	}

	free(module);
}

void BindObserverToSubject(Module *module_src, Module *module_dst, void **data)
{
	if (module_src == NULL) {
		printf("module_src is NULL!\n");
		return;
	}

	if (module_dst == NULL) {
		printf("module_dst is NULL!\n");
		return;
	}

	module_src->AddObserver(module_src, module_dst, data);
}

void UnBindObserverFromSubject(Module *module_src, Module *module_dst)
{
	if (module_src == NULL) {
		printf("module_src is NULL!\n");
		return;
	}

	if (module_dst == NULL) {
		printf("module_dst is NULL!\n");
		return;
	}

	module_src->RemoveObserver(module_src, module_dst);
}

int flush_module_tree_sync(Module *module)
{
	int i, ret, timeout = 0x100;

	while ((nr_module_empty_msg(module) || module->in_process) && --timeout)
		usleep(20000);

	if (timeout < 0) {
		printf("flush module:%s timeout, nr_module_empty_msg=%d in_process=%d\n",
					module->name, nr_module_empty_msg(module), module->in_process);
		return -1;
	}

	for (i = 0; i < nr_module_observer(module); i++) {
		Module *modob = module_observer_by_num(module, i);
		ret = flush_module_tree_sync(modob);
		if (ret < 0) {
			printf("flush module:%s observer-%d error\n",
						module->name, i);
			return -1;
		}
	}

	return 0;
}
