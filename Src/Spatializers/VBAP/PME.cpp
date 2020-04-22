///
/// PME.h -- Ventriloquist
///	See the copyright notice and acknowledgment of authors in the file COPYRIGHT
/// Doug McCoy, 2004.
///

#include "PME.h"
#include <arpa/inet.h>

//extern float max_mu, min_mu, max_e, min_e, max_error, min_error;

double kTwoPi = 2.0 * M_PI;

void Controller::set_data (controller_str &str ){
	sync.lock();
	float x = (float)str.x / 32767.0;
	float y = (float)str.y / 32767.0;
	float z = (float)str.z / 32767.0;
	float dx = (float)str.dx / 32767.0;
	float dy = (float)str.dy / 32767.0;
	float dz = (float)str.dz / 32767.0;
	_position.set(x, y, z);
	_velocity.set(dx, dy, dz);
//	glove_state = str.glove_state;
	sync.unlock();
}

void Controller::get_data(CPoint &p, CPoint &v, GloveState &glove_st ){
	sync.lock();
	p = _position;
	v = _velocity;
	glove_st = glove_state;
	sync.unlock();
}

void Controller::get_position(CPoint &p ){
	sync.lock();
	p = _position;
	sync.unlock();
}

//void * Controller::remote_read_func(void *data ){
//	Controller * myself = (Controller*)data;
//	controller_str received_data;
//	string addr_recieved_from;
//	unsigned short port_recv_from;
//	string request_command = "send";
//
//	while (1){
//		// send request for controller data
//		myself->sock.sendTo(&request_command, request_command.size(), myself->foreign_net_address, myself->foreign_port);
//		
//		myself->sock.recvFrom(&received_data, sizeof(received_data), addr_recieved_from, port_recv_from);
//		myself->set_data(received_data);
//		csl::sleep_sec(0.01);
//	}
//}
//
//void Controller::start_reader_thread(){
//	thread.create_thread(remote_read_func, this);
//}

void Controller::get_remote_data() {
	controller_str received_data;
	string addr_recieved_from;
	unsigned short port_recv_from;
	char * request_command = "send";
	
	// send request for controller data
	sock.sendTo(request_command, 5, foreign_net_address, foreign_port);
	 
	sock.recvFrom(&received_data, sizeof(received_data), addr_recieved_from, port_recv_from);
//	cout <<  "recieved data" << endl;
	received_data.x = ntohl(received_data.x);
	received_data.y = ntohl(received_data.y);
	received_data.z = ntohl(received_data.z);
	received_data.dx = ntohl(received_data.dx);
	received_data.dy = ntohl(received_data.dy);
	received_data.dz = ntohl(received_data.dz);
	received_data.glove_state = ntohl(received_data.glove_state);
	
	if (1 == received_data.glove_state){
		
		received_data.glove_state;
	}
	
	set_data(received_data);
}

PMESource::PMESource(SpatialSource & s) 
		: source(&s), current_move_type(kStopped), 
		  bounce_distance(1.0), current_trace_index(0), trace_length(0) { };

PMESource::PMESource() 
		: source(NULL), current_move_type(kStopped), 
		  bounce_distance(1.0), current_trace_index(0), trace_length(0) { };

void PMESource::set_position(CPoint &p ){
	source->setPosition(p);
}

CPoint PMESource::get_position() {
	return source->getPosition();
}

void PMESource::update_position() {
	CPoint P, newP;
	switch (current_move_type){
		case kOrbit: {
			orbit.calculate_new_position_in_orbit();
			CPoint newpos;
			orbit.calculate_absolute_position(newpos );
			source->setPosition(newpos.x, newpos.y, newpos.z);		
			break;
		}	
		case kDraw:
			if (trace_length>0){
				source->setPosition(trace[current_trace_index].x, 
									 trace[current_trace_index].y, 
									 trace[current_trace_index].z);
				current_trace_index = (current_trace_index + 1) % trace_length;
			}
			break;
			
#define signof(x) (((x)<0)? -1.0 : 1.0)
		case kBounce:
			P = source->getPosition();
			if (fabs(P.x) > bounce_distance)
				P.x = signof(P.x) * bounce_distance;
			if (fabs(P.y) > bounce_distance)
				P.y = signof(P.y) * bounce_distance;
			if (fabs(P.z) > bounce_distance)
				P.z = signof(P.z) * bounce_distance;

			newP = P + bounce_velocity;
			if (fabs(newP.x) > bounce_distance)
				bounce_velocity.x = -bounce_velocity.x;
			if (fabs(newP.y) > bounce_distance)
				bounce_velocity.y = -bounce_velocity.y;
			if (fabs(newP.z) > bounce_distance)
				bounce_velocity.z = -bounce_velocity.z;
			newP = P + bounce_velocity;
			source->setPosition(newP.x, newP.y, newP.z);
			break;

		case kGrabbed:
			break;

		case kStopped:
			break;
	}
}

void PMESource::set_orbit(CPoint &R, CPoint &V){
	orbit.calculate_orbital_params(R, V );	
}

void PMESource::set_next_move_type (MovementType m){
	next_move_type = m;
}

void PMESource::set_current_move_type (MovementType m){
	current_move_type = m;
}

void  PMESource::set_bounce_distance(float bd){
	bounce_distance = bd;
}

void PMESource::push_trace(CPoint &pos ){
	if (trace_length >= MAX_TRACE_LENGTH){
		cerr << "PMESource:: push_trace() - Trace full!" << endl;
		return;
	}
	trace[trace_length] = pos;
	trace_length++;
}

unsigned exit_count = 0;

#define RMAX (1.0)	// meters
void Orbit::calculate_orbital_params(CPoint R,  CPoint V){
	calculate_eccentricity(R, V);
	double v = V.len();
	double r = R.len();
	a = -0.5 * mu / ((v*v*0.5) - (mu / r) );
	n = sqrt(mu/(a*a*a)); // radians per unit time
	CPoint h = R^V;		// cross product
	i = acos(h.z / h.len() );	// h.z = h cross (0,0,1)
	CPoint K(0, 0, 1);
	n_vec = K^h;
	omega = acos(n_vec.x / n_vec.len());
	if (n_vec.y < 0)
		omega = kTwoPi - omega;
	double temp = n_vec * e_vec / (n_vec.len() * e_vec.len() );
	w = acos(temp );
	if (e_vec.z < 0)
		w = kTwoPi - w;
	nu = acos(e_vec * R / (e_vec.len() * R.len() ) );
	if (R*V < 0)
		nu = kTwoPi - nu;	
}

void Orbit::calculate_eccentricity(CPoint R,  CPoint V){
	CPoint rtemp = R;
	CPoint vtemp = V;
	rtemp.normalize();
	vtemp.normalize();
	e = fabs(rtemp * vtemp);	// dot product should be between 0 and 1
	e = (e<0.99) ? e:0.99;
	
	mu = 0.4;		// first guess
	double vs = V.len();
	double r = R.len();
	double error = 100.0;	// some large number
	double delta = -1.0;
	double sign = -1.0;
	unsigned c = 0;
	while (c++ < 200000) {
		if (c>20000)
			c=c;
		rtemp = R;
		vtemp = V;
		rtemp *= (vs*vs - (mu/r));
		vtemp *= R*V;
		e_vec = rtemp - vtemp;
		e_vec *= (1.0/mu);

		double old_error = error;
		error = (e_vec.len() - e);
		delta = error - old_error;
		
		if (delta == 0.0){
			cout << "Delta == 0" << endl << endl;
			break;
		}

		if (fabs(error) < (0.0001 * e))
			break;
		if (delta >= 0.0)
			sign = -sign;
		mu *= 1.0 + (sign * error );
	}
	e = e_vec.len();		// correct initial e guess to actual value
	cout << "e:\t" << e << endl;
	cout << "error:\t" << error << endl;
//	
//	max_mu = max(max_mu, mu);
//	min_mu = min(min_mu, mu);
//	max_e = max(max_e, e);
//	min_e = min(min_e, e);
////	max_error = max(max_error, error/e);
//	min_error = min(min_error, error/e);
	
}

void Orbit::calculate_absolute_position(CPoint &newPosition){
	double r = (a * (1.0 - e*e)) / (1.0 + e * cos(nu));
	newPosition.x = r * (cos(omega)*cos(nu+w) - sin(omega)*sin(nu+w)*cos(i) );
	newPosition.y = r * (sin(omega)*cos(nu+w) - cos(omega)*sin(nu+w)*cos(i) );
	newPosition.z = r * (sin(nu+w)*sin(i) );
	
}

void Orbit::calculate_new_position_in_orbit(){
	double E = acos((e+cos(nu)) / (1.0+e*cos(nu)) );
	if (nu > M_PI)
		E = kTwoPi - E;
	double M = E - e*sin(E);
	M = M + n;
	while (M > (kTwoPi))
		M -= kTwoPi;
	double E_old;
	E = M;
	float error = 10.0;		// initial large error
	while (fabs(error) > 0.0001){
		E_old = E;
		E = M + e*sin(E);
		error = E - E_old;
	}
	nu = acos((cos(E) - e) / (1.0 - e*cos(E)) );
	if (E > M_PI)
		nu = kTwoPi - nu;

}


void Orbit::dump(){
	cout << "Orbit:"<< endl;
	cout << "semi-major axis:\t" << a << endl;
	cout << "eccentricity:\t" << e << endl;
	cout << "inclination angle:\t" << i << endl;
	cout << "longitude of ascending node:\t" << omega << endl;
	cout << "argument of perigee:\t" << w << endl;
	cout << "true anomaly:\t" << nu << endl;
	cout << "mean motion:\t" << sqrt(mu / (a*a*a) ) << endl;
	cout << "mu:\t\t\t" << mu << endl << endl;
}

PME::PME (string remote_net_addr, unsigned short remote_port)
		: controller(remote_net_addr, remote_port), grabbed_source(NULL), num_sources(0) {
//		pme_source_list = (PMESource**) malloc(MAX_NUM_VBAP_SOURCES * siizeof(PMESource*) );
	pme_source_list = new PMESource*[ MAX_NUM_VBAP_SOURCES ];
}

PME::PME ()
	:  grabbed_source(NULL), num_sources(0) {
//		pme_source_list = (PMESource**) malloc(MAX_NUM_VBAP_SOURCES * siizeof(PMESource*) );
	pme_source_list = new PMESource*[ MAX_NUM_VBAP_SOURCES ];
}

PME::~PME(){ delete pme_source_list; }

void PME::update_grabbed_position(CPoint &p) {
//	CVector p;
//	controller.getPosition(p );
	grabbed_source->set_position(p );
}

bool PME::add_pme_source(PMESource &s ){
	if (num_sources == MAX_NUM_VBAP_SOURCES){
		printf("Max number of VBAP sources exceeded. Source not added\n");
		return false;
	}
	pme_source_list[num_sources++] = &s;
	return true;
	
}

#define MIN_GRAB_DISTANCE_SQ (0.05)
bool PME::check_for_grabbed_source(CPoint &p ){
	for (unsigned i=0; i<num_sources; i++){
		CPoint pp = (pme_source_list[i]->get_position() );
		double distance_sq = p.distance2(pp );
		if (distance_sq < MIN_GRAB_DISTANCE_SQ){
			grabbed_source = pme_source_list[i];
			pme_source_list[i]->set_current_move_type(kGrabbed );
			return true;
		}
	}
	return false;		// did not grab source
}

void PME::manage_sources(){
	struct timeval start, end;
	CPoint P, V;
	GloveState gs;
	
//	pme_move_type = kDraw;
	keep_processing_sources = true;
	
	while (keep_processing_sources){
		GET_TIME(&start);
		controller.get_remote_data();
		controller.get_data(P, V, gs );
		if (gs == kClosed && grabbed_source==NULL){		// glove just grabbed or holding source

			if (check_for_grabbed_source(P ) ){	// finds near source and sets grabbed_source
				grabbed_source->set_current_move_type(kGrabbed);
				cout << "grabbed source"<<endl;
				grabbed_source->reset_trace();
			}
		}
		else if (grabbed_source != NULL ){
//			cout << "moving grabbed source" << endl;
			update_grabbed_position(P );
			MovementType mt = grabbed_source->get_next_move_type();
			if ((gs == kPoint) && (mt == kDraw) ){
				grabbed_source->push_trace(P );
				cout << "making trace" << endl;
			}
		}
		
		if (grabbed_source != NULL && gs == kOpen){		// source just released
			cout << "glove released" << endl;
			if (V.len() < 0.005)
				grabbed_source->set_current_move_type(kStopped);
			grabbed_source->update_move_type();
			switch (grabbed_source->get_current_move_type()){
				case kDraw:
					break;
				case kOrbit:
					grabbed_source->set_orbit(P, V );
					break;
				case kBounce:
					grabbed_source->set_bounce_velocity(V );
			}
			grabbed_source = NULL;
		}
			
		for (unsigned i=0; i<num_sources; i++){
			pme_source_list[i]->update_position();
		}
		
		GET_TIME(&end);
		
		struct timeval *s = &start, *e = &end;
		unsigned diff = SUB_TIMES(e, s );
//		cout<<"time:\t" << diff<<endl;
		if (diff < 10000)		
			csl::sleepUsec(10000-diff );		// each loop should now take 0.01 seconds
//		else
//			int temp=0;		
	}	
}

// thread functions

void * PME::management_func(void *data){
	PME *me = (PME*)data;
	me->manage_sources();
	return 0;
}

void PME::start_management_thread(){
	management_thread.createThread(management_func, (void*)this);
}

void PME::stop_management_thread(){
	keep_processing_sources = false;
}
