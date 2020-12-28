#pragma once

namespace Silenda
{
	struct ThreadProcess
	{
		std::function<boost::any()> process_func;
		boost::any assignment_ptr;
	};
}