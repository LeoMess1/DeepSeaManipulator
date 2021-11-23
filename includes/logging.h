#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <ostream>
#include <sstream>
#include <time.h>
#include <iostream>
#include <iomanip>

#include "mutexclass.h"
#include "utilities.h"

// Make a bunch of macros for logging.  The way to log things is to stream
// things to LOG(<a particular severity level>).  E.g.,
//
//   LOG(INFO) << "Found " << num_cookies << " cookies";
//
// LOG LINE PREFIX FORMAT
//
// Log lines have this form:
//
//     Lmmdd hh:mm:ss.uuuuuu threadid file:line] msg...
//
// where the fields are defined as follows:
//
//   L                A single character, representing the log level
//                    (eg 'I' for INFO)
//   mm               The month (zero padded; ie May is '05')
//   dd               The day (zero padded)
//   hh:mm:ss.uuuuuu  Time in hours, minutes and fractional seconds
//   threadid         The space-padded thread ID as returned by GetTID()
//                    (this matches the PID on Linux)
//   file             The file name
//   line             The line number
//   msg              The user-supplied message
//
// Example:
//
//   I1103 11:57:31.739339 24395 google.cc:2341] Command line: ./some_prog
//   I1103 11:57:31.739403 24395 google.cc:2342] Process id 24395
//
// NOTE: although the microseconds are useful for comparing events on
// a single machine, clocks on different machines may not be well
// synchronized.  Hence, use caution when comparing the low bits of
// timestamps from different machines.
// A few definitions of macros that don't generate much code.  Since
// LOG(INFO) and its ilk are used all over our code, it's
// better to have compact code for these operations.

#define PRINT(severity, functionName) LOGMESSAGE->PrintStream(severity, __FILE__, __LINE__, functionName)

#define LOG(severity, functionName) LOGMESSAGE->Stream(severity, __FILE__, __LINE__, functionName)
#define LOG_FLUSH     LOGMESSAGE->Flush()

#define LOGMESSAGE LogMessage::Instance()

static const int LOG_INFO      = 0;
static const int LOG_WARNING   = 1;
static const int LOG_ERROR     = 2;
static const int LOG_FATAL     = 3;
static const int LOG_MES       = 4;
static const int NUM_SEVERITIES = 5;

class Logger 
{
public:
	static Logger* CreateFileObject();
	virtual ~Logger();
	virtual void Write(time_t timestamp,const char* message,int message_len) = 0;
	virtual void Flush() = 0;
    virtual bool CreateLogfile(const std::string& time_pid_string) = 0;
private:
    static const char* const path_;
    static const char* const extension_;
};

// Encapsulates all file-system related state
class LogFileObject : public Logger 
{
public:
	LogFileObject(const char* base_filename, const char* extension);
	~LogFileObject();

	virtual void Write(time_t timestamp, const char* message,int message_len);

	void SetBasename(const char* basename);
	void SetExtension(const char* ext);

	virtual void Flush();
	void FlushUnlocked();

private:
	MutexClass lock_;
	std::string base_filename_;
	std::string filename_extension_;     // option users can specify (eg to add port#)
	FILE* file_;
	unsigned int file_length_;
	unsigned int bytes_since_flush_;
	bool CreateLogfile(const std::string& time_pid_string);
        
        int listDir(const char *name, char **list);

    class MutexRapper
    {
    public:
        MutexRapper(MutexClass *lock){lock_ = lock; lock_->lock();}
        ~MutexRapper(){lock_->unlock();}
    private:
        MutexClass *lock_;
    };
};

class LogMessage 
{
public:
	static LogMessage *Instance();
	~LogMessage();
    
    std::ostringstream& Stream(int severity, const char* file, int line, const char *functionName);
    std::ostream& PrintStream(int severity, const char* file, int line, const char *functionName);
    void Flush();

	int NormalizeSeverity(const int severity);
	static const char* const LogSeverityNames[NUM_SEVERITIES];

private:
	LogMessage();
	LogMessage(const LogMessage&);
	void operator=(const LogMessage&);

    std::ostringstream stream_;
	Logger *logger_;
private:
	static LogMessage *instance_;
};

#endif // _LOGGING_H_
