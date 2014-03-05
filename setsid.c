/*
 * Copyright (c) 2014 Tristan Le Guern <tleguern@bouledef.eu>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void usage(void);

/*
 * setsid shall exit like nohup, with one of the following values:
 * 126 - The utility was found but could not be invoked.
 * 127 - An error occurred in the nohup utility, or the utility could
 *       not be found.
 */
#define EXIT_NOEXEC     126
#define EXIT_NOTFOUND   127
#define EXIT_MISC       127

int
main(int argc, char *argv[]) {
	int ch;
	int status = 1;
	pid_t pid;

	while ((ch = getopt(argc, argv, "")) != -1) {
		switch (ch) {
		default:
			usage();
			/* NOTREACHED */
		}
	}
	if (argc < 2) {
		usage();
		/* NOTREACHED */
	}

	if (getpgrp() == getpid()) {
		pid = fork();

		switch(pid) {
		case 0:
			break;
		case -1:
			err(EXIT_MISC, "fork");
			/* NOTREACHED */
		default:
			if (wait(&status) != pid)
				err(EXIT_MISC, "wait");
			if (WIFEXITED(status))
				return WEXITSTATUS(status);
			err(status, "child did not exit normally");
		}
	}

	if (setsid() < 0) {
		err(EXIT_MISC, "setsid");
	}

	execvp(argv[1], argv + 1);
	if (errno == ENOENT) {
		err(EXIT_NOTFOUND, "%s", argv[1]);
	}
	err(EXIT_NOEXEC, "execvp");
}

static void
usage(void) {
	fprintf(stderr, "usage: %s program [argument ...]\n", getprogname());
	exit(EXIT_MISC);
}

