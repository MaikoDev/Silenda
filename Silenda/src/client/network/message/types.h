#pragma once

enum NetworkMessageType
{
	NETWORK_SERVER = 0, // ban, unban, kick, clear, mute, unmute,
	NETWORK_AUTH,       // cl_pkrequest, cl_pksend, sv_pksend, cl_pkconfirm, sv_pkconfirm, sv_deny
	NETWORK_CLIENT      // chatlog, chatmsg
};

/* RSA handshake procedure
1. Client connects to listening port of server.
2. Server assigns unique port to client to begin server-client communication.
3. Server sends a netmessage(cl_pkrequest) to request RSA public key from client. (unencrypted)
4. Client sends a netmessage(cl_pksend) with RSA public key attached. (unencrypted)
5. Server sends a netmessage(sv_pksend | sv_deny) with server RSA public key attached. (unencrypted)
6. Client sends a netmessage(cl_pkconfirm) send server's public key back. (encrypted)
7. Server sends a netmessage(sv_pkconfirm) send client's public key back. (encrypted)
*/