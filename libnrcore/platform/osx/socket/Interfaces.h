#ifndef __NETLINK_H__
#define __NETLINK_H__

namespace nrcore {

	class Interfaces {
	public:
		Interfaces();
		virtual ~Interfaces();

		void refreshInterfaces();

	private:
		int fd;

		void addInterfaces(struct nlmsghdr *h);
	};

};

#endif//__NETLINK_H__
