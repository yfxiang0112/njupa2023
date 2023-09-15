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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

static bool check_parentheses(int p, int q);

enum {
   TK_EQ, TK_MINU, TK_PLUS, TK_DIV, TK_MUL, TK_RB, TK_LB, TK_NUM, TK_NOTYPE = 256

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
	{"[0-9]+", TK_NUM},					// number digit
	{"\\(", TK_LB},
	{"\\)", TK_RB},					// left & right braces
	{"\\*", TK_MUL},					// multiple
	{"/", TK_DIV},						// divide
  {"\\+", TK_PLUS},          // plus
	{"\\-", TK_MINU},					// minus
  {"==", TK_EQ},        // equal
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

static Token tokens[32] __attribute__((used)) = {};
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

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

				/*TODO: handle too long expr.*/
				if (nr_token >= 32){
					assert(0);
					//TODO: handle too long expr.
				}

        switch (rules[i].token_type) {
					case TK_NOTYPE:
						/* bspace, quit. */
						break;

					case TK_NUM:
						/* number, eval the val. */
						printf("%d\n",atoi(substr_start));
						//TODO: store the value?
						
          default: 
						/* record the current token type & str. */
						tokens[nr_token].type = rules[i].token_type;
						if (substr_len<=32){
							strncpy(tokens[nr_token].str, substr_start, substr_len);
						} else {
							assert(0);
							//TODO: handle overflow error.
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

	printf("test check parentheses: %d \n", check_parentheses(0, nr_token-1));

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

  return 0;
}

static bool check_parentheses(int p, int q){
	if (p<0 || q>=32 || p>=q){ return false; }
	if (tokens[p].type!=TK_LB || tokens[q].type!=TK_RB){ return false; }

	/* check nu left brace == right brace? */
	/*
	int cnt_l=0, cnt_r=0;
	for (int i=p; i<=q; i++) {
		if (tokens[i].type==TK_LB){ cnt_l++; }
		if (tokens[i].type==TK_RB){ cnt_r++; }
	}
	if (cnt_l != cnt_r){ return false; }
	*/

	int nr_pair=0;
	for (int pos = p; pos<=q; pos++) {
		if (tokens[pos].type == TK_LB){ nr_pair++; }
		if (tokens[pos].type == TK_RB){ nr_pair--; }
		if (nr_pair <= 0) {
			return pos==q;
		}
	}
	
	return false;

	/*/
	do {
		p++;
	} while (tokens[p].type!=TK_LB && p<q-1);

	do {
		q--;
	} while (tokens[q].type!=TK_RB && q>p+1);

	if (p==q-1) {
		return true;
	} else if (q-p < 1) {
		return false;
	} else {
		return check_parentheses(p,q);
	} */
}
