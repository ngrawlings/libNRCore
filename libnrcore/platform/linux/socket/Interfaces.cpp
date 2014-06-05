#include "Interfaces.h"


namespace nrcore {

	Interfaces::Interfaces() {
		fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
		struct sockaddr_nl local;
		memset(&local, 0, sizeof(local));
		local.nl_family = AF_NETLINK;
		local.nl_pid = getpid();
		local.nl_groups = 0;

		if (bind(fd, (struct sockaddr *) &local, sizeof(local)) < 0)
			throw -1;

		refreshInterfaces();
	}

	Interfaces::~Interfaces() {

	}

	void Interfaces::refreshInterfaces() {
		clear();

		typedef struct nl_req_s nl_req_t;

		struct nl_req_s {
				  struct nlmsghdr hdr;
				  struct rtgenmsg gen;
		};

		struct sockaddr_nl kernel;
		struct iovec io;

		memset(&kernel, 0, sizeof(kernel));

		{
			struct msghdr rtnl_msg;

			nl_req_t req;

			memset(&rtnl_msg, 0, sizeof(rtnl_msg));
			memset(&req, 0, sizeof(req));

			kernel.nl_family = AF_NETLINK; /* fill-in kernel address (destination) */

			req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
			req.hdr.nlmsg_type = RTM_GETLINK;
			req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
			req.hdr.nlmsg_seq = 1;
			req.hdr.nlmsg_pid = getpid();
			req.gen.rtgen_family = AF_PACKET; /*  no preferred AF, we will get *all* interfaces */

			io.iov_base = &req;
			io.iov_len = req.hdr.nlmsg_len;
			rtnl_msg.msg_iov = &io;
			rtnl_msg.msg_iovlen = 1;
			rtnl_msg.msg_name = &kernel;
			rtnl_msg.msg_namelen = sizeof(kernel);

			sendmsg(fd, (struct msghdr *) &rtnl_msg, 0);
		}

		{
			int end = 0;
			char reply[1024]; /* a large buffer */

			while (!end) {
				int len;
				struct nlmsghdr *msg_ptr;    /* pointer to current part */
				struct msghdr rtnl_reply;    /* generic msghdr structure */
				struct iovec io_reply;

				memset(&io_reply, 0, sizeof(io_reply));
				memset(&rtnl_reply, 0, sizeof(rtnl_reply));

				io.iov_base = reply;
				io.iov_len = 1024;
				rtnl_reply.msg_iov = &io;
				rtnl_reply.msg_iovlen = 1;
				rtnl_reply.msg_name = &kernel;
				rtnl_reply.msg_namelen = sizeof(kernel);

				len = recvmsg(fd, &rtnl_reply, 0);
				if (len) {
					for (msg_ptr = (struct nlmsghdr *) reply; NLMSG_OK(msg_ptr, len); msg_ptr = NLMSG_NEXT(msg_ptr, len)) {
						switch(msg_ptr->nlmsg_type) {
					    	case 3:     /* this is the NLMSG_DONE end msg */
					    		end++;
					            break;
					        case 16:    /* this is the RTM_NEWLINK msg */
					            addInterfaces(msg_ptr);
					            break;
					        default:
					            break;
					    }
					}
				}
			}
		}
	}

	void Interfaces::addInterfaces(struct nlmsghdr *h) {
		struct ifinfomsg *iface;
		struct rtattr *attribute;
		int len;

		iface = (ifinfomsg*)NLMSG_DATA(h);
		len = h->nlmsg_len - NLMSG_LENGTH(sizeof(*iface));

		for (attribute = IFLA_RTA(iface); RTA_OK(attribute, len); attribute = RTA_NEXT(attribute, len)) {
			switch(attribute->rta_type) {
				case IFLA_IFNAME:
					add(iface->ifi_index, (char *) RTA_DATA(attribute));
					//TODO: interface addrs must be added here
					break;
				default:
					break;
	      }
	    }
	}

};

