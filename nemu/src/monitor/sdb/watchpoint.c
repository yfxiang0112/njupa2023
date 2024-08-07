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
	char expr[MAX_TOKENS_LEN];
	word_t val;
  /* TODO: Add more members if necessary */

} WP;

WP* new_wp();
void free_wp(WP *wp);

bool scan_wp();
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

/* ---------------------------------------- */
/* operations to pool LLs */

WP* new_wp(bool *success) {

	/* remove the first node in Free */
	WP* ret = free_;
	if (free_ != NULL) { 
		free_ = free_->next;
		ret->next = NULL;
	} else {
		*success = false;
		return NULL;
	}
	
	/* insert new wp to using wp, by nuber sequence */
	WP *curr = head;
	if (head == NULL || ret->NO < head->NO) {
		ret->next = head;
		head = ret;
		return ret;
	}
	while(curr->next != NULL) {
		if (curr->NO < ret->NO && curr->next->NO > ret->NO) {
			ret->next = curr->next;
			curr->next = ret;
			return ret;
		}
		curr = curr->next;
	}
	
	curr -> next = ret;
	ret -> next = NULL;
	return ret;
}

void free_wp(WP *wp) {

	/* find the wp and remove it from using wps */
	WP *curr = head;
	if (head == wp) {
		head = head->next;
	} else if (head != NULL) {
		while(curr->next != NULL) {
			if (curr->next == wp) {
				curr->next = curr->next->next;
				break;
			}
			curr = curr->next;
		}
	}
	
	/* insert it to free wp LL by number sequence */
	curr = free_;
	if (free_==NULL || wp->NO < free_->NO) {
		wp->next = free_;
		free_ = wp;
		return;
	}
	while(curr->next != NULL) {
		if (curr->NO < wp->NO && curr->next->NO > wp->NO) {
			wp->next = curr->next;
			curr-> next = wp;
			return;
		}
		curr = curr->next;
	}
	curr -> next = wp;
	wp -> next = NULL;
	return;
}

/* ---------------------------------------- */
/* add, remove and show info about wp-s. */

void add_wp(char *args) {
	bool success = true;

	/* check & eval the value of expr. */
	word_t val = expr(args, &success);
	if (!success) {
		return;
	}

	/* create a new wp*/
	WP *newwp = new_wp(&success);
	if (success) {
		newwp->val = val;
		strcpy(newwp->expr, args);
		printf("new watchpoint #%d for %s = %d\n", newwp->NO, newwp->expr, newwp->val);
		return;
	}
	printf("the maximum watchpoint number is %d.\n", NR_WP);
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
		if (curr != NULL) {
			free_wp(curr);
			return;
		}
	}
	printf("invalid watchpoint number\n");
}

void display_wp() {
	WP *curr = head;
	if (head != NULL) {
		while (curr!=NULL) {
			printf("watchpoint No.%d for %s\n", curr->NO, curr->expr);
			curr = curr->next;
		}
	}

	/* 
	printf("----------\nTEMP test for free wp pool:\n");
	curr = free_;
	if (free_ != NULL) {
		while (curr!=NULL) {
			printf("watchpoint No.%d \n", curr->NO);
			curr = curr->next;
		}
	}
	*/
}

/* scan the wp-s and check the change of val 
 * return TRUE if any val changed */
bool scan_wp() {
	bool changed = false;
	bool success = true;

	WP* curr = head;
	while (curr != NULL) {
		word_t n_val = expr(curr->expr, &success);
		if (n_val != curr->val) {
			changed = true;
			printf("watchpoint #%d: %s changed %d to %d\n", curr->NO, curr->expr, curr->val, n_val);
			curr->val = n_val;
		}

		curr = curr->next;
	}

	return changed;
}
