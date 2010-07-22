/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */


/*char *dcmd(const char *cmd, const char *key, char *val) {
	struct query *dq;

	dq = malloc(sizeof(struct query) + arch_strlen(key) + arch_strlen(val) + 2);

	dq->sep = '\0';
	arch_strlcpy(dq->cmd, cmd, 12);
	arch_strcpy(&dq->dat[0], key);
	arch_strcpy(&dq->dat[1024], val, 1024);

	psend(FD_DICT, (char*)&dq, (char*)&dq, , 0, PORT_DICT);

	if (dq.cmd[0] == '!') {
		arch_strcpy(val, dq.cmd);
	}
	else {
		arch_strcpy(val, dq.val);
	}

	return val;
}*/
