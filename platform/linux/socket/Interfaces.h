#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <libnrcore/platform/common/socket/InterfacesBase.h>
#include <libnrcore/memory/LinkedList.h>
#include <libnrcore/memory/StringList.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

namespace nrcore {

	class Interfaces : public InterfacesBase {
	public:
		Interfaces();
		virtual ~Interfaces();

		void refreshInterfaces();
		StringList getInterfaceList();
		String getInterfaceName(int index);
		String getInterfaceAddress(int index);

	private:
		class Interface {
		public:
			Interface(String name, String addr);
			Interface(const Interface &interface);
			~Interface();

			String name;
			String addr;
		};

		int fd;
		LinkedList<Interface> interfaces;

		void addInterfaces(struct nlmsghdr *h);
	};

};

#endif//__INTERFACE_H__
