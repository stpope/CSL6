///
/// Accessor.h -- "Reflective" parameter name/setter/type acessor object.
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// 
/// These are used as "plugs" for dynamically controllable instruments (e.g., with MIDI voicers or OSC).
///

#ifndef INCLUDE_Accessor_H
#define INCLUDE_Accessor_H

/// Type flags

#define CSL_INT_TYPE		1
#define CSL_FLOAT_TYPE		2
#define CSL_STRING_TYPE		3

namespace csl  {

///
/// The Accessor class has public data members
///
class Accessor {
public:
						/// Convenient constructor
	Accessor(string na, unsigned sel, unsigned typ) 
		: mName(na), mSelector(sel), mType(typ) { };
	
						// public members
	string mName;			///< name
	unsigned mSelector;		///< operand selector (see macros in the instruments, e.g., set_attack_f)
	unsigned mType;			///< argument type (see macros above, e.g., CSL_FLOAT_TYPE)

};

///
/// Typedef for AccessorVector object
///

typedef vector<Accessor *> AccessorVector;

}

#endif
