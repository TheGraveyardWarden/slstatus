#ifndef _EXEC_H_
#define _EXEC_H_

#define EXEC_OUTPUT_LEN 2048

struct exec_ret {
	int exit;
	int outpipe;
};

/*	Runs cmd until completion.
 *	Return value:
 *		0 on success
 *		-1 on err
 *		< -1 child was terminated by a signal. return value is signal number * -1
 *		> 0 return value is cmd exit status
 */
struct exec_ret exec(const char *cmd, ...);
int exec_ret_read_output(struct exec_ret*, char*, size_t);

static inline void exec_ret_close(struct exec_ret* er) {
	close(er->outpipe);
}

#endif
