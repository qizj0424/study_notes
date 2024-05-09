#pragma once
#ifndef __MAIN_H_
#define __MAIN_H_

struct AAA {
	int a;
	int b;
	int c;
	int pri[0];
};

typedef int (*CallbackFunction)(struct AAA *test1);

int init(CallbackFunction cb);
#endif
