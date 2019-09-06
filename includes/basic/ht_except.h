#ifndef _HT_EXCEPT_H_
#define _HT_EXCEPT_H_ 1

#include "object.h"
#include <exception>

HT_NAMESPACE_BEGIN

inline bool IS_AMF_ERROR(int32 code)	{	return ((code>>8)==0x11); }
inline bool IS_IVK_ERROR(int32 code)	{	return ((code>>8)==0x12); }
inline bool IS_COMM_ERROR(int32 code)	{	return ((code>>8)==0x13); }

HT_NAMESPACE_END

// success
const uint32 HT_SUCCESS				= 0L;		// ok

// base error based 0x1000
const uint32 HT_UNKNOW_ERROR		= 0x1001;	// unknow error
const uint32 HT_DATETIME_FMT		= 0x1002;	// Datetime format error. used as yyyy-mm-dd HH:MM:SS.xxxx

// amf error based 0x1100
const uint32 HT_AMF_NULL_OBJ		= 0x1100;	// call an null object
const uint32 HT_AMF_NO_CHILDREN		= 0x1101;	// get the child of object, but the object has no children
const uint32 HT_AMF_EOF				= 0x1102;	// iterator move to EOF, can't get data
const uint32 HT_AMF_CONVERT_INT		= 0x1103;	// the object can't convert to int
const uint32 HT_AMF_CONVERT_DOUBLE	= 0x1104;	// the object can't convert to double
const uint32 HT_AMF_CONVERT_STRING	= 0x1105;	// the object can't convert to string
const uint32 HT_AMF_CONVERT_TIME	= 0x1106;	// the object can't convert to time
const uint32 HT_AMF_NOT_ARRAY		= 0x1107;	// the object is not an array
const uint32 HT_AMF_NOT_ASOBJECT	= 0x1108;	// the object is't an asobject, so can't index from name
const uint32 HT_AMF_OVERFLOW		= 0x1109;	// array overflow
const uint32 HT_AMF_ITEM_NOT_EXIST	= 0x1110;	// the item not exist in the asobject
const uint32 HT_AMF_COMPRES			= 0X1111;	// compress failed
const uint32 HT_AMF_UNCOMPRESS		= 0X1112;	// uncompress failed

// invoke error based 0x1200
const uint32 HT_IVK_PARAM_NUM		= 0x1201;	// number of param is not matched
const uint32 HT_IVK_FUNC_NOT_EXIST	= 0x1202;	// call not exist function 
const uint32 HT_IVK_FUNC_EXCEPT		= 0x1203;	// call the function occured an exception
const uint32 HT_IVK_REGISTER		= 0x1204;	// function register error
const uint32 HT_IVK_RET_TYPE_ERROR	= 0x1205;	// return value type not supported

// comm	error based	0x1300
const uint32 HT_COMM_NO_ANSWER		= 0x1300;	// communication error, server no answer
const uint32 HT_COMM_DATA_PARSE		= 0x1301;	// data parse failed when communication
const uint32 HT_COMM_NOT_COMMIT		= 0x1302;	// call GetResult before the request is commit success
const uint32 HT_COMM_PROTOCOL_ERROR	= 0x1303;	// protocol of the data error

#endif //_HT_EXCEPT_H_
