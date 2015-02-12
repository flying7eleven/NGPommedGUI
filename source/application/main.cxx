#include <sys/types.h> // pid_t
#include <sys/stat.h> // umask
#include <stdlib.h> // exit
#include <unistd.h>
#include <fcntl.h> // open
#include <sched.h> // fork, _exit
#include <syslog.h>
#include "ngpommedgui.hxx"

int main( int argc, char **argv ) {
	// fork the process
	pid_t processId = fork();

	// if the fork returned -1 as a process id of the child, the fork failed
	if( -1 == processId ) {
		exit( EXIT_FAILURE + 0 );

	}
	// if the new process id is larger than 0, we are still in the parent process and can exit here
	else if( processId > 0 ) {
		exit( EXIT_SUCCESS );
	}

	// obtain a new process group
	setsid();

	// inherited descriptors and standard I/O descriptors should be closed
	for( int i = getdtablesize(); i >= 0; --i ) {
		close( i );
	}
	int processNullDescriptor = open( "/dev/null", O_RDWR );
	dup( processNullDescriptor );
	dup( processNullDescriptor );

	// set a safe file creation mask
	umask( 027 );

	// from now on we are a daemon process
	syslog( LOG_DEBUG, "Next Generation pommed GUI daemon started" );

	// everything went okay
	return EXIT_SUCCESS;
}
