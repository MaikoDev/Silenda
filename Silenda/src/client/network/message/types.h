#pragma once

enum NetMessageType
{
	sv_deny = 0,
	sv_pkvalidate,
	cl_pkvalidate,
	sv_pksend,
	cl_pksend,
	sv_pkrequest,

	chatlog,
	chatmsg,

	sv_auth_passed,
	sv_auth_failed,

	sv_uuidrequest,
	sv_uuidsend,
	cl_uuidsend,
	count
};

MSGPACK_ADD_ENUM(NetMessageType);