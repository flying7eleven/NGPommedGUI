#include <sys/types.h> // pid_t
#include <sys/stat.h> // umask
#include <stdlib.h> // exit
#include <unistd.h>
#include <fcntl.h> // open
#include <sched.h> // fork, _exit
#include <signal.h> // signal
#include <syslog.h>
#include <boost/format.hpp>
#include "ngpommedgui.hxx"

void daemonSignalHandler( int sig ) {
	switch( sig ) {
		case SIGTERM:
			syslog( LOG_DEBUG | LOG_DAEMON, "Caught SIGTERM. Exiting the daemon gracefully." );
			_exit( EXIT_SUCCESS );
			break;
	}
}

bool isRunning() {
	// try to open the lock file (pidfile),
	int lockFileFileHandle = open( NGPOMMEDGUI_PID_FILE, O_RDWR | O_CREAT | O_TRUNC, 0640 );

	if( lockFileFileHandle < 0 ) {
		syslog( LOG_ERR | LOG_DAEMON, boost::str( boost::format( "Failed to open %1%. Please delete the file and try again." ) % NGPOMMEDGUI_PID_FILE ).c_str() );
		return true;
	}

	// if we cannot lock the file, it seems that we are already running
	if( lockf( lockFileFileHandle, F_TLOCK, 0 ) < 0 ) {
		return true;
	}

	// if we could lock the file, we can write our pid into the file and then tell the caller that we were not running before
	const std::string pidStr = boost::str( boost::format( "%1%" ) % getpid() );
	syslog( LOG_DEBUG | LOG_DAEMON, pidStr.c_str() );
	write( lockFileFileHandle, pidStr.c_str(), pidStr.length() );
	return false;
}

int main( int argc, char **argv ) {
	// fork the process
	pid_t forkSuccess = fork();

	// if the fork returned -1 as a process id of the child, the fork failed
	if( -1 == forkSuccess ) {
		exit( EXIT_FAILURE + 0 );

	}
	// if the new process id is larger than 0, we are still in the parent process and can exit here
	else if( forkSuccess > 0 ) {
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

	// catch some signals for the daemon
	signal( SIGTERM, daemonSignalHandler ); // software termination signal from kill
	signal( SIGCHLD, SIG_IGN ); // ignore child
	signal( SIGTSTP, SIG_IGN ); // TODO
	signal( SIGTTOU, SIG_IGN ); // ignore tty OUT signal
	signal( SIGTTIN, SIG_IGN ); // ignore tty IN signal

	// be sure that we just start the daemon if it is not running already
	if( isRunning() ) {
		syslog( LOG_ERR | LOG_DAEMON, boost::str( boost::format( "Terminating. It seems that the daemon is already running. If not, delete %1% and try it again." ) % NGPOMMEDGUI_PID_FILE ).c_str() );
		_exit( EXIT_FAILURE + 1 );
	}

	// from now on we are a daemon process
	syslog( LOG_DEBUG | LOG_DAEMON, "Next Generation pommed GUI daemon started" );

	while( true ) {
		sleep( 1 );
	}

	// everything went okay
	return EXIT_SUCCESS;
}
