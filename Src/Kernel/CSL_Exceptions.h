///
///  CSL_Exceptions.h -- the CSL Exception classes (specifications + implementations)
///
/// Exception classes
///
/// Status - enumerated return flag type
/// CException - Base class of CSL exceptions (written upper-case).
/// MemoryError - Malloc failure subclass
/// Wrong kind of operand error
/// TimingError - Time-out
/// RunTimeError - Illegal operation at run time
/// LogicError - Impossible operation
/// DomainError - Numerical data of wrong type
/// OutOfRangeError - Data out of range
/// IOError - IO Error
///
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
///

#ifndef CSL_EXCEPTIONS_H
#define CSL_EXCEPTIONS_H

#include <exception>		// Standard C++ exception library
#include <string>	

namespace csl {

using namespace std;

///
/// CSL status flags (for return values)
///

typedef enum {
	kOk,				///< "OK" return status
	kFound,				///< "found" return status
	kNotFound,			///< "not found" return status
	kEmpty,				///< "empty" return status
	kErr				///< "error" return status
} Status;

#ifndef CSL_ENUMS
//	class exception { };	// forward def needed for SWIG
#endif

///
/// Base class of CSL exceptions (written upper-case).
///	Has a string message
///

class CException : public std::exception {
public:
	string mMessage;
	CException(const string & msg) throw() : mMessage(msg) { };
	~CException() throw() { };
	const char * what() { return mMessage.c_str(); };
};

///
/// Malloc failure subclass
///

class MemoryError : public CException {  
public:
	MemoryError(const string & msg) : CException(msg) { };
};

///
/// Wrong kind of operand error
///

class ValueError : public CException {  
public:
	ValueError(const string & msg) : CException(msg) { };
};

///
/// Time-out
///

class TimingError : public CException {  
public:
	TimingError(const string & msg) : CException(msg) { };
};

///
/// Illegal operation at run time
///

class RunTimeError : public CException {  
public:
	RunTimeError(const string & msg) : CException(msg) { };
};

///
/// Impossible operation
///

class LogicError : public CException {  
public:
	LogicError(const string & msg) : CException(msg) { };
};

///
/// Numerical data of wrong type
///

class DomainError : public ValueError { 
public:
	DomainError(const string & msg) : ValueError(msg) { };
};

///
/// Data out of range
///

class OutOfRangeError : public ValueError {  
public:
	OutOfRangeError(const string & msg) : ValueError(msg) { };
};

///
/// IO Error
///

class IOError : public ValueError {  
public:
	IOError(const string & msg) : ValueError(msg) { };
};

///
/// DB Error
///

class DBError : public LogicError {  
public:
	DBError(const string & msg) : LogicError(msg) { };
};

///
/// Processing Error
///

class ProcessingError : public ValueError {  
public:
	ProcessingError(const string & msg) : ValueError(msg) { };
};

}

#endif
