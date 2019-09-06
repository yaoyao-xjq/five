/* uuid32.h 
   2007-09-15 Last created by cheungmine.
     Partly rights reserved by cheungmine.
*/

#ifndef UUID32_H_INCLUDED
#define UUID32_H_INCLUDED

#include <stdlib.h>
#include <assert.h>
#include <string>
#include <memory.h>
#include <config.h>

#ifdef _WIN32
#include <Guiddef.h>
typedef   GUID uuid;
#else
#include <uuid/uuid.h>
typedef    uuid_t uuid;
#endif

/**
* Generates a new UUID. The UUID is a time-based time 1 UUID.
* A random per-process node identifier is used to avoid keeping global
* state and maintaining inter-process synchronization.
**/
void HT_CLASS_EXPORT uuid_create(uuid* uid);

std::string HT_CLASS_EXPORT uuid_create_string(void); 
/**
* Translate a POS_UUID to a uuid string
*  return UUID string
**/
std::string HT_CLASS_EXPORT uuid_to_string(const uuid* uuid);

#endif	/* UUID32_H_INCLUDED */
