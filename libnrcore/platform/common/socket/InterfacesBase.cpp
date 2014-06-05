#include "InterfacesBase.h"

#include <assert.h>

namespace nrcore {

	InterfacesBase::Interface::Interface(int index, String name) {
		this->_index = index;
		this->_name = name;
	}

	InterfacesBase::Interface::Interface(const Interface &interface) {
		_index = interface._index;
		_name = interface._name;
		_addrs = interface._addrs;
	}

	InterfacesBase::Interface::~Interface() {

	}

	int InterfacesBase::Interface::index() {
		return _index;
	}

	String& InterfacesBase::Interface::name() {
		return _name;
	}

	const Array<Address> InterfacesBase::Interface::getAddrs() {
		return _addrs;
	}

	void InterfacesBase::Interface::addAddr(Address value) {
		_addrs.push(value);
	}

	void InterfacesBase::add(int index, String name) {
		if (get(index).index() != -1) {
			assert(false); // Note: For debugging purposes, to be removed later
			return;
		}

		Interface _if(index, name);
		interfaces.push(_if);
	}

	InterfacesBase::Interface &InterfacesBase::get(int index) {
		int len = interfaces.length();
		for (int i=0; i<len; i++) {
			if (interfaces[i].index() == index)
				return interfaces[i];
		}
		return Interface("", -1);
	}

	void InterfacesBase::addAddr(int index, Address addr) {
		Interface _if = get(index);
		_if.addAddr(addr);
	}

	void InterfacesBase::remove(int index) {
		int len = interfaces.length();
		LinkedListState<Interface> addrs_state(&interfaces);
		for (int i=0; i<len; i++) {
			Interface _if = addrs_state.next();
			if (_if.index() == index) {
				addrs_state.remove();
				break;
			}
		}
	}

	void InterfacesBase::clear() {
		while(interfaces.length())
			interfaces.remove(0);
	}

	const Array<InterfacesBase::Interface> InterfacesBase::getInterfaceList() {
		return interfaces;
	}

	const String InterfacesBase::getInterfaceName(int index) {
		Interface _if = get(index);
		return _if.name();
	}

	const Array<Address> InterfacesBase::getInterfaceAddressList(int index) {
		Interface _if = get(index);
		return _if.getAddrs();
	}

};
