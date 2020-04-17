/*
    Copyright (C) 2001-2003 Paul Davis
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    
    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    $Id: time.h 948 2006-05-15 17:12:44Z pbd $
*/

#ifndef __jack_time_h__
#define __jack_time_h__

#include <jack/types.h>
#include <mach/mach_time.h>

extern double __jack_time_ratio;

static inline jack_time_t 
jack_get_microseconds(void) 
{  
        return  (jack_time_t) mach_absolute_time () * __jack_time_ratio;
}

#endif /* __jack_time_h__ */