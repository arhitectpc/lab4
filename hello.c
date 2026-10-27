/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "How many times to print \"Hello, world!\" (default 1)");

struct hello_data {
	struct list_head node;
	ktime_t time;
};

static LIST_HEAD(hello_list);
static void cleanup_hello_list(void)
{
	struct hello_data *pos, *tmp;

	list_for_each_entry_safe(pos, tmp, &hello_list, node) {
		pr_info("Time: %lld\n", ktime_to_ns(pos->time));
		list_del(&pos->node);
		kfree(pos);
	}
}

static int __init hello_init(void)
{
	unsigned int i;
	struct hello_data *ptr;

	if (count > 10) {
		pr_err("Invalid count %u: must be 10 or less\n", count);
		return -EINVAL;
	}
	if (count == 0 || count >= 5)
		pr_warn("count is %u, continuing anyway\n", count);
	for (i = 0; i < count; i++) {
		ptr = kmalloc(sizeof(*ptr), GFP_KERNEL);
		if (!ptr) {
			cleanup_hello_list();
			return -ENOMEM;
		}
		ptr->time = ktime_get();
		list_add_tail(&ptr->node, &hello_list);
		pr_info("Hello, world!\n");
	}
	return 0;
}

static void __exit hello_exit(void)
{
	cleanup_hello_list();
}

module_init(hello_init);
module_exit(hello_exit);