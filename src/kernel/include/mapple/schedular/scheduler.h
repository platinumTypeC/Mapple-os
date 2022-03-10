#pragma once

#include <stdint.h>
#include <stddef.h>

#define SCHEDULER_QUEUES_NUM	_SCHEDULER_QUEUES_NUM

typedef struct s_Thread {
	uint64_t	tid;
	struct Process*	process;
	struct s_Thread* 	    next;
} __attribute__((packed)) Thread_t;

typedef struct s_SchedulerContextFrameRegisters {
	uint64_t	rax, rbx, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11, r12, r13, r14, r15;
} SchedulerContextFrameRegisters_t;

typedef struct s_SchedulerContextFrame {
	SchedulerContextFrameRegisters_t general;
	uint64_t	rip, cs, rflags, rsp, ss;
	uint64_t	self;
} __attribute__((packed)) SchedulerContextFrame_t;

typedef struct s_SchedulerContext {
	SchedulerContextFrame_t	        context_frame;
	uint64_t						flags;
	uint64_t						error_code;
	Thread_t					    task;
} __attribute__((packed)) SchedulerContext_t;

typedef struct s_SchedulerNode {
	SchedulerContext_t	    context;
	size_t					queue_num;
	struct SchedulerNode*	previous;
	struct SchedulerNode*	next;
} __attribute__((packed)) SchedulerNode_t;


struct SchedulerTaskInitialState {
	SchedulerContextFrameRegisters_t    	general;
	void*									entry;	
	void*									stack;
	uint8_t									rpl;
};

enum SchedulerQueueNumber {
	SCHEDULER_QUEUE_PRIORITY,
	SCHEDULER_QUEUE_REGULAR,
	SCHEDULER_QUEUE_BATCH,
	_SCHEDULER_QUEUES_NUM
};

enum SchedulerQueueFlagBits {
	SCHEDULER_QUEUE_FLAG_LOCKED	= 0,
};


enum SchedulerContextFlagBits {
	SCHEDULER_CONTEXT_FLAG_LOCKED	= 0,
	SCHEDULER_CONTEXT_FLAG_FINISHED	= 1,
};

void InitializeShedular();
