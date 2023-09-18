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

#include <isa.h>
#include <memory/paddr.h>
#include <cpu/cpu.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

#define MAX_TOKENS_LEN 65535
#define MAX_STR_SIZE 32

static bool check_parentheses(uint32_t p, uint32_t q);
static word_t eval_expr(uint32_t p, uint32_t q, bool *success);
unsigned int op_prio(int type);

enum {
   TK_NUM, TK_NEG, TK_DRF, TK_REG, TK_LB, TK_RB, TK_MUL, TK_DIV, TK_PLUS, TK_MINU, TK_EQ, TK_NEQ, TK_AND, TK_OR, TK_NOTYPE = 256
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
  {" +", TK_NOTYPE},          // spaces
	{"[0-9]+", TK_NUM},					// number digit
	{"\\(", TK_LB},
	{"\\)", TK_RB},			    		// left & right braces
	{"\\*", TK_MUL},				  	// multiple
	{"/", TK_DIV},					  	// divide
  {"\\+", TK_PLUS},           // plus
	{"\\-", TK_MINU},			  		// minus
  {"==", TK_EQ},              // equal
  {"!=", TK_NEQ},             // nonequal
	{"&&", TK_AND},	  		  	  // and
	{"\\|\\|", TK_OR},	      	 // or 
	{"\\$[a-z|A-Z]+", TK_REG}		// register pointer
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[MAX_TOKENS_LEN] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

				if (nr_token >= MAX_TOKENS_LEN){
					printf("Max expression length is %d. Please input a valid expression.", MAX_TOKENS_LEN);
					return false;
				}

        switch (rules[i].token_type) {
					case TK_NOTYPE:
						/* bspace, quit. */
						break;
						
					/* case of deference (single '*') or neg sign (single '-'). 
					 * Modify the value of tokens[i]. */
					case TK_MUL: case TK_MINU:
						if ((nr_token==0) || (tokens[nr_token-1].type != TK_RB && tokens[nr_token-1].type != TK_NUM)) {
							if (rules[i].token_type == TK_MUL) { tokens[nr_token].type = TK_DRF; }
							else if (rules[i].token_type == TK_MINU) { tokens[nr_token].type = TK_NEG; }
							nr_token++;
							break;
						}
          default: 
						/* record the current token type & str. */
						tokens[nr_token].type = rules[i].token_type;
						if (substr_len<=MAX_STR_SIZE){
							strncpy(tokens[nr_token].str, substr_start, substr_len);
							tokens[nr_token].str[substr_len] = '\0';

						} else {
							printf("Max token length is %d. Please input a valid expression.", MAX_STR_SIZE);
							return false;
						}
						nr_token ++;
        }
        break;
      }
    }

		/* Matching Fail */
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  return eval_expr(0, nr_token-1, success);
}


static word_t eval_expr(uint32_t p, uint32_t q, bool *success) {

	/* case 1. dereference */
	if (tokens[p].type == TK_DRF) {
		word_t addr = eval_expr(p+1, q, success);
		if(addr-CONFIG_MBASE > CONFIG_MSIZE) {
			printf("Invalid memory address: %x \n", addr);
			*success = false;
			return 0;
		}
		return paddr_read(addr, 4);
	}

	/* case 2. negtive number */
	else if (tokens[p].type == TK_NEG) {
		return 0-eval_expr(p+1, q, success);
	}

	/* case 3. single number */
	else if (p==q && tokens[p].type == TK_NUM) { 
		return atoi(tokens[p].str);
	}

	/* case 4. single register value */
	else if (p==q && tokens[p].type == TK_REG) {
		printf("%s\n", tokens[p].str+1);
		return 0;
			//TODO
	}

	/* case 5. closed by braces */
	else if (check_parentheses(p, q)) { 
		return eval_expr(p+1, q-1, success); 
	}

	/* case 6. other valid expr, find main operator. */
	else if (p<q) {
		int main_op=0;
		unsigned int m_prio = -1;
		word_t l_expr, r_expr;

		/* scan expr & find rightmost plus/minus and multi/divide operator. */
		int nr_brk = 0;
		for (int i=p; i<=q; i++) {
			if (tokens[i].type==TK_LB) { nr_brk++; }
			if (tokens[i].type==TK_RB) { nr_brk--; }

			if (nr_brk==0) {
				if (op_prio(tokens[i].type)>0 && op_prio(tokens[i].type)<=m_prio) {
					main_op = i;
					m_prio = op_prio(tokens[i].type);
				}
			}
		}

		/* invalid expr if no main op */
		if (main_op == 0) {
			*success = false;
			printf("Invalid expression, cannot find main op. Please input a valid expression.\n");
			return 0;
		}
		

		/*evaluate left and right expr, eval curr expr with main op. */
		l_expr = eval_expr(p, main_op-1, success);
		r_expr = eval_expr(main_op+1, q, success);
		switch(tokens[main_op].type){
			case TK_PLUS:
				return l_expr+r_expr;
			case TK_MINU:
				return l_expr-r_expr;
			case TK_MUL:
				return l_expr*r_expr;
			case TK_DIV:
				if (r_expr!=0){ return l_expr/r_expr; }
				else {
					*success = false;
					printf("Nan (Divide by 0)\n");
					return 0;
				}
			case TK_AND:
				return l_expr&&r_expr;
			case TK_OR:
				return l_expr||r_expr;
			case TK_EQ:
				return l_expr==r_expr;
			case TK_NEQ:
				return l_expr!=r_expr;
			default:
				*success = false;
				printf("Invalid operator. Please input a valid expression.\n");
				return 0;
		}
		return 0;
	}

	/* other cases, invalid expr. */
	else {
		*success = false;
		printf("Invalid expression. Please input a valid expression.\n");
		return 0;
	}
}

static bool check_parentheses(uint32_t p, uint32_t q){
	/* invalid input & unclosed braces. */
	if (p<0 || q>=nr_token || p>=q){ return false; }
	if (tokens[p].type!=TK_LB || tokens[q].type!=TK_RB){ return false; }

	/* check if braces are paired & p-q are paired. */
	int nr_pair=0;
	for (int pos = p; pos<=q; pos++) {
		if (tokens[pos].type == TK_LB){ nr_pair++; }
		if (tokens[pos].type == TK_RB){ nr_pair--; }
		if (nr_pair <= 0) {
			return pos==q;
		}
	}
	
	return false;
}

unsigned int op_prio(int type) {
	switch(type) {
		case TK_PLUS: case TK_MINU:
									return 1;
		case TK_MUL: case TK_DIV:
								 return 2;
		case TK_EQ: case TK_NEQ:
								 return 3;
		case TK_AND:
								 return 4;
		case TK_OR:
								 return 5;
		default:
								 return 0;
	}
}
