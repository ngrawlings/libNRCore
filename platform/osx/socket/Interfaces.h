#ifndef __NETLINK_H__
#define __NETLINK_H__

namespace nrcore {

	class Netlink {
	public:
		Netlink();
		virtual ~Netlink();

		void refreshInterfaces();

	private:
		int fd;

		void addInterfaces(struct nlmsghdr *h);
	};

};

#endif//__NETLINK_H__
