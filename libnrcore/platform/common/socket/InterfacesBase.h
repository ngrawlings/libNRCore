#ifndef __INTERFACESBASE_H__
#define __INTERFACESBASE_H__

#include <libnrcore/memory/String.h>

namespace nrcore {

	class InterfacesBase {
	public:
		virtual ~InterfacesBase() {}

		virtual void refreshInterfaces() = 0;
		virtual StringList getInterfaceList() = 0;
		virtual String getInterfaceName(int index) = 0;
		virtual String getInterfaceAddress(int index) = 0;

	protected:
		class Interface {
		public:
			Interface(String name, String addr);
			Interface(const Interface &interface);
			~Interface();

			String& name();
			String& name(String value);
			String& addr();
			String& addr(String value);

		private:
			String _name;
			String _addr;
		};
	};

};

#endif//__INTERFACESBASE_H__
