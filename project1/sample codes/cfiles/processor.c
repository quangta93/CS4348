#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"

int main() {

	pid_t pid;

	switch(pid = fork()) {	// creates memory process as a child of cpu
	case -1:
		// error
		perror("The fork failed!");
		break;
	case 0:
		/* Memory */
		_exit(0);
		break;
	default:
		/* CPU */
		break;
	}
}