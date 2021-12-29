#pragma once

#include <string>

namespace am {

class system_error {
public:
	static const std::wstring& error2str(unsigned long error);
};

}
