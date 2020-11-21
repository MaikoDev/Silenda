#pragma once

enum UserLevel
{
	user = 0,
	admin = 1,
	superuser = 2,
};

MSGPACK_ADD_ENUM(UserLevel);