/*
 * This file contains a few tunables for libptytty. Normally there should
 * be little reason to change this file. It is provided to suit rare or
 * special cases only.
 */

/*
 * Default mode to restore when releasing the PTS device. It is relaxed to be
 * compatible with most systems, change it to a more secure value if your
 * system supports it (0640 for example).
 */
#ifndef RESTORE_TTY_MODE
# define RESTORE_TTY_MODE 0666
#endif

/*
 * Define if you want to use a separate process for pty/tty handling
 * when running setuid/setgid. You need this when making it setuid/setgid.
 */
#ifndef PTYTTY_HELPER
# define PTYTTY_HELPER 1
#endif
