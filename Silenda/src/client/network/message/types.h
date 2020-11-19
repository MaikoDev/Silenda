#pragma once

#include "msgpack.hpp"

enum NetworkMessageType
{
	NETWORK_SERVER = 0, // ban, unban, kick, clear, mute, unmute,
	NETWORK_AUTH,       // cl_pkrequest, cl_pksend, sv_pksend, cl_pkconfirm, sv_pkconfirm, sv_deny
	NETWORK_CLIENT      // chatlog, chatmsg
};

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
	cl_auth_failed,
	sv_auth_passed,
	count
};

MSGPACK_ADD_ENUM(NetMessageType);

/* RSA handshake procedure
1. Client connects to listening port of server.
2. Server assigns unique port to client to begin server-client communication.
3. Server sends a netmessage(sv_pkrequest) to request RSA public key from client. (unencrypted)
4. Client sends a netmessage(cl_pksend) with RSA public key attached. (unencrypted)
5. Server sends a netmessage(sv_pksend | sv_deny) with server RSA public key attached. (unencrypted)
6. Server sends a netmessage(sv_pkvalidate) send client's public key back. (encrypted)
7. Client sends a netmessage(cl_pkvalidate) send server's public key back. (encrypted)
8. Server send a netmessage(sv_auth_passed | sv_deny) for final auth message.
*/