/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
	char *expr;
  /* TODO: Add more members if necessary */

} WP;

WP* new_wp();
void free_wp(WP *wp);

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
	WP* ret = free_;
	free_ = free_->next;
	ret->next = NULL;
	
	if (head == NULL) {
		head = ret;
	} else {
		WP *curr = head;
		while(curr->next != NULL) {
			curr = curr->next;
		}
		ret->next = curr->next;
		curr->next = ret;
	}

	return ret;
}

void free_wp(WP *wp) {
	WP *curr = head;
	if (head != NULL) {
		while(curr->next != NULL) {
			if (curr->next == wp) {
				curr->next = curr->next->next;
				break;
			}
			curr = curr->next;
		}
	}
	
	curr = free_;
	if (free_==NULL || wp->NO < free_->NO) {
		wp->next = free_;
		free_ = wp;
		return;
	}
	while(curr->next != NULL) {
		if (curr->NO < wp->NO && curr->next->NO > wp->NO) {
			wp -> next = curr -> next;
			curr -> next = wp;
			return;
		}
		curr = curr->next;
	}
	curr -> next = wp;
	wp -> next = NULL;
	return;
}

/* */

void add_wp(char *expr) {
	WP *newwp = new_wp();
	printf("WP No.%d\n", newwp->NO);
}

void rm_wp(int no) {
	if (head != NULL) {
		WP *curr = head;
		while (curr != NULL) {
			if (curr->NO == no) {
				break;
			}
			curr = curr->next;
		}
		//TODO: handle WP not find 
		//
		free_wp(curr);
	}
	//TODO: WP not find?
}

void display_wp() {
	WP *curr = head;
	if (head != NULL) {
		while (curr!=NULL) {
			printf("watchpoint No.%d \n", curr->NO);
			curr = curr->next;
		}
	}

	printf("----------\nTEMP test for free wp pool:\n");
	curr = free_;
	if (free_ != NULL) {
		while (curr!=NULL) {
			printf("watchpoint No.%d \n", curr->NO);
			curr = curr->next;
		}
	}
}
