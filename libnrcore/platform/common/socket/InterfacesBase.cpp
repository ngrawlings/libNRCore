#include "InterfacesBase.h"

namespace nrcore {

	InterfacesBase::Interface::Interface(String name, String addr) {
		this->_name = name;
		this->_addr = addr;
	}

	InterfacesBase::Interface::Interface(const Interface &interface) {
		_name = interface._name;
		_addr = interface._addr;
	}

	InterfacesBase::Interface::~Interface() {

	}

	String& InterfacesBase::Interface::name() {
		return _name;
	}

	String& InterfacesBase::Interface::name(String value) {
		_name = value;
		return _name;
	}

	String& InterfacesBase::Interface::addr() {
		return _addr;
	}

	String& InterfacesBase::Interface::addr(String value) {
		_addr = value;
		return _addr;
	}

};
