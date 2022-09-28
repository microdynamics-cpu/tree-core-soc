/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-20     bigmagic     first version
 */

#include <rtthread.h>


#define THREAD_PRIORITY   3
#define THREAD_STACK_SIZE 512
#define THREAD_TIMESLICE  5

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
// static char out_buf[16];
// extern char *itoa(unsigned int val, char *str, int base);

static void low_prior_entry(void *parameter)
{
    rt_uint32_t count = 0, id;
    while (1)
    {
        id = (rt_uint32_t)parameter;
        // itoa(count++, out_buf, 10);
        if(count >= 7) break;
        rt_kprintf("thread%u count: %u\n", id, count++);
        rt_thread_mdelay(40);
    }
    rt_kprintf("thread%u exit\n", id);
}

// ALIGN(RT_ALIGN_SIZE)
// static char thread3_stack[1024];
// static struct rt_thread thread3;
static void high_prior_entry(void *param)
{
    rt_uint32_t count = 0;
    for (count = 0; count < 4; ++count)
    {
        rt_kprintf("thread3 count: %d\n", count);
    }
    rt_kprintf("thread3 exit\n");
}

int thread_sample(void)
{
    tid1 = rt_thread_create("thread1",
                            low_prior_entry, (void*)1,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
    }


    tid2 = rt_thread_create("thread2",
                            low_prior_entry, (void*)2,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    if (tid2 != RT_NULL)
    {
        rt_thread_startup(tid2);
    }
        

    // rt_thread_init(&thread3,
    //                "thread3",
    //                high_prior_entry,
    //                RT_NULL,
    //                &thread3_stack[0],
    //                sizeof(thread3_stack),
    //                THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    // rt_thread_startup(&thread3);

    return 0;
}


int main(void)
{
    rt_kprintf("Hello RISC-V!\n");
    thread_sample();
    return 0;
}
