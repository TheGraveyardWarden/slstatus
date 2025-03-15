#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdarg.h>
#include "exec.h"

struct exec_ret exec(const char *cmd, ...) {
	pid_t pid;
	int wstatus, ret, pipefd[2];
	va_list ap;
	char *c = NULL;
	size_t size = 0;
	struct exec_ret exec_ret = { .exit = -1, .outpipe = -1 };

	va_start(ap, cmd);
	ret = vsnprintf(c, size, cmd, ap);
	va_end(ap);

	if (ret < 0) {
		return exec_ret;
	}

	size = (size_t)ret + 1;
	c = malloc(size);
	if (c == NULL) {
		return exec_ret;
	}

	va_start(ap, cmd);
	ret = vsnprintf(c, size, cmd, ap);
	va_end(ap);

	if (ret < 0) {
		free(c);
		return exec_ret;
	}

	if (pipe(pipefd) < 0) {
		free(c);
		return exec_ret;
	}
	exec_ret.outpipe = pipefd[0];

	pid = fork();
	if (pid == 0) {
		close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
			perror("dup2()");
		}
		close(pipefd[1]);

		execl("/bin/sh", "/bin/sh", "-c", c, NULL);
		perror("execl()");
		exit(100);
	}
	else {
		close(pipefd[1]);
		free(c);
		if (waitpid(pid, &wstatus, 0) < 0) {
			perror("waitpid()");
			exit(1);
		}
		if (WIFEXITED(wstatus)) {
			exec_ret.exit = WEXITSTATUS(wstatus);
			return exec_ret;
		}
		else if (WIFSIGNALED(wstatus)) {
			exec_ret.exit = WTERMSIG(wstatus) * -1;
			return exec_ret;
		}
	}

	// unreachable
	return exec_ret;
}

int exec_ret_read_output(struct exec_ret *er, char *data, size_t size) {	
	return read(er->outpipe, data, size);
}

#ifdef EXEC_DEBUG

#include <stdio.h>

int main() {
	struct exec_ret ret;
	char buffer[1024] = {0};


	ret = exec("echo HAHAHAHA");

	if (ret.exit < 0) {
		perror("exec()");
		exit(1);
	}

	int nread = exec_ret_read_output(&ret, buffer, 3);
	printf("nread: %d\n", nread);
	if (nread < 0) {
		perror("exec_ret_read_output()");
		exit(1);
	}

	printf("%s\n", buffer);

	nread = exec_ret_read_output(&ret, buffer, 1024);
	printf("nread: %d\n", nread);
	if (nread < 0) {
		perror("exec_ret_read_output()");
		exit(1);
	}

	printf("%s\n", buffer);
	exec_ret_close(&ret);
}

#endif

