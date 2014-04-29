#ifndef __INTERFACESBASE_H__
#define __INTERFACESBASE_H__

#include <libnrcore/socket/Address.h>
#include <libnrcore/memory/String.h>
#include <libnrcore/memory/Array.h>

namespace nrcore {

	class InterfacesBase {
	protected:
		class Interface {
		public:
			Interface(int index, String name);
			Interface(const Interface &interface);
			~Interface();

			int index();

			String& name();

			const Array<Address> getAddrs();
			void addAddr(Address value);

		private:
			int _index;
			String _name;
			Array<Address> _addrs;
		};
	public:
		virtual ~InterfacesBase() {}

		virtual void refreshInterfaces() = 0;

		const Array<Interface> getInterfaceList();
		const String getInterfaceName(int index);
		const Array<Address> getInterfaceAddressList(int index);

	protected:
		Array<Interface> interfaces;

		void add(int index, String name);
		Interface &get(int index);
		void remove(int index);
		void clear();

		void addAddr(int index, Address addr);
	};

};

#endif//__INTERFACESBASE_H__
