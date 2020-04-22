///
/// PME.h -- Ventriloquist
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// Doug McCoy, 2004.
///

#ifndef INCLUDE_PME_H
#define INCLUDE_PME_H

#include "CPoint.h"
#include "VBAP.h"
#include "ThreadUtilities.h"
#include "PracticalSocket.h"
#include "CGestalt.h"

//#ifdef DO_TIMING					// Here are the macros and globals for the timing code
#include <sys/time.h>
//#define GET_TIME(val) if (gettimeofday(val, 0) != 0) cout << "Output: Error reading current time" << endl;
//#define SUB_TIMES(t1, t2) (((t1->tv_sec - t2->tv_sec) * 1000000) + (t1->tv_usec - t2->tv_usec))
//#endif

using namespace csl;

// for use with sending over sockets

typedef struct {			
	int x;
	int y;
	int z;
	int dx;
	int dy;
	int dz;
	int glove_state;
} controller_str;

typedef enum {
	kStopped,
	kGrabbed,
	kOrbit,
	kDraw, 
	kBounce
} MovementType;

typedef enum {
	kInvalid = -1,
	kClosed  =  0,
	kPoint   =  1,
	kOpen	 = 15
} GloveState;

// for use with internal classes

class Controller {
protected:
	CPoint _position;
	CPoint _velocity;
	GloveState glove_state;	
	csl::SynchPthread sync;
	csl::ThreadPthread thread;
//	unsigned short local_port;
	unsigned short foreign_port;
	string foreign_net_address;
	UDPSocket sock;

public:
	Controller(string remote_net_addr, unsigned short remote_port): 
		glove_state(kOpen), foreign_port(remote_port), foreign_net_address(remote_net_addr) { };
	Controller(): glove_state(kOpen) { };
	~Controller(){ };
	void set_remote_addr_and_port(string addr, unsigned short port ){ foreign_port=port; foreign_net_address=addr;};
	void set_data(controller_str &str );
	void get_data(CPoint &p, CPoint &v, GloveState &glove_st );		// get pos and vel and put result into p and v
	void get_position(CPoint &p);
	void * remote_read_func(void *data);		// function to run in separate thread
	void start_reader_thread();
	void get_remote_data();
};

class Orbit {
protected:
	double a;		// semi-major axis
	double n;		// mean motion (radians per unit time)
	double e;		// eccentricity			(0 <= e < 1)
	double i;		// inclination angle	(0 <= i <= pi)
	double omega;	// longitude of ascending node	(0 <= omega < 2pi)
	double w;		// argument of perigee	(0 <= w < 2pi)
	double nu;		// true anomaly			(0 <= v < 2pi)
	CPoint e_vec;	// eccentricity vector
	CPoint n_vec;	// ascending node vector
	double mu;		// gravity coonstant valid for individual instance
	
public:
	Orbit(){ };
	~Orbit(){ };
	void calculate_eccentricity(CPoint R,  CPoint V);
	void calculate_orbital_params(CPoint R,  CPoint V);
	void calculate_absolute_position(CPoint &new_position);
	void calculate_new_position_in_orbit();
	void dump();
};

#define MAX_TRACE_LENGTH (100 * 15)		// 100 positions per second for max 15 seconds

class PMESource {
protected:
	SpatialSource *source;
	Orbit orbit;
	MovementType current_move_type;
	MovementType next_move_type;
	CPoint trace[MAX_TRACE_LENGTH];
	float bounce_distance;
	CPoint bounce_velocity;
	unsigned current_trace_index;
	unsigned trace_length;
	
public:
	void set_position(CPoint &P );
	CPoint get_position();
	void update_position();
	void set_orbit(CPoint &R, CPoint &V);
	MovementType get_current_move_type() { return current_move_type; };
	MovementType get_next_move_type() { return next_move_type; };
	void update_move_type(){ current_move_type = next_move_type; };
	void set_next_move_type (MovementType mov_type);
	void set_current_move_type (MovementType mov_type);
	void push_trace(CPoint &pos );
	void reset_trace(){ trace_length = current_trace_index = 0; };
	void set_bounce_velocity(CPoint bv){ bounce_velocity = bv; };
	void set_bounce_distance(float bd);
	
	PMESource(SpatialSource & s );
	PMESource();
	~PMESource(){ };
};

class PME {
protected:
//	csl::SynchPthread sync;
	csl::ThreadPthread management_thread;
	Controller controller;
	PMESource ** pme_source_list;
	PMESource *grabbed_source;
	unsigned short num_sources;
//	MovementType pme_move_type;
	void update_grabbed_position(CPoint &p);
	bool check_for_grabbed_source(CPoint &p );
	bool keep_processing_sources;
	
public:
	bool add_pme_source(PMESource &s );
	void remove_all_sources() {num_sources=0;};
	void manage_sources();
//	void set_movement_type(MovementType type ){ pme_move_type = type; };
	void set_remote_addr_and_port(string addr, unsigned short port ){ controller.set_remote_addr_and_port(addr, port); };
	static void * management_func(void *data);		// function to run in separate thread
	void start_management_thread();
	void stop_management_thread() ;
	
	PME(string remote_net_addr, unsigned short remote_port);
	PME();
	~PME();
};

#endif

