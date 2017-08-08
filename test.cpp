#include "base.h"

#define AF_NETLINK 16
#define NETLINK_ROUTE 0
#define SOCK_RAW 3


struct nlmsghdr{
	unsigned int nlmsg_len;
	unsigned short nlmsg_type;
	unsigned short nlmsg_flags;
	unsigned int nlmsg_seq;
	unsigned int nlmsg_pid;
};

struct sockaddr_nl{
    int nl_family;
	unsigned short nl_pad;
	unsigned int nl_pid;
	unsigned int nl_groups;
};


class Messagehandle{
public:
	void Run();
};

class Socket{
public:
	~Socket(){
		Close();
	}
protected:
	Socket(int family,int type,int protocol = 0);
	bool Open();
	void Close();
	bool Bind();
	//bool Connect();

	int fd_;
	sockaddr_nl local_addr ;
	

private:
	int family_;
	int type_;
	int protocol_;
};

Socket::Socket(int family,int type,int protocol):fd_(-1),family_(family),type_(type),protocol_(protocol){  
}

bool Socket::Open(){
	if(fd_ != -1) return true;
	fd_ = ::socket(family_,type_,protocol_);
	if(fd_  == -1) return false;
	return true;
}

bool Socket::Bind(){
    int ret = ::bind(fd_,(struct sockaddr*)&local_addr,sizeof(local_addr));
	if(ret == -1)return false;
	return true;
}

void Socket::Close(){
	if(fd_ == -1)return;
	int tmp_fd = fd_;
	fd_ = -1;
	::close(tmp_fd);
}


unsigned int Id(){
    return 1;
}

unsigned int NlMsgSeq(){
    static unsigned int seq = 0;
	return seq++;
}


class NlMsg{


};

class NLsocket : public Socket{
public:
	explicit NLsocket(Messagehandle* handle = NULL):Socket(AF_NETLINK,SOCK_RAW,NETLINK_ROUTE),pid_(Id()),handle_(handle){
	}

	void PadAddr(sockaddr_nl* addr,int pid,int groups = 0){
		addr->nl_family = AF_NETLINK;
		addr->nl_pid = pid;
		addr->nl_groups = groups;
	}
	bool init(){
		Open();
		PadAddr(&local_addr,pid_);
		Bind();
		return true;
	}
	bool talk(NlMsg* nl_msg){
		nlmsghdr* msg = nl_msg->NlHdr();
		msg->nlmsg_seq = NlMsgSeq();
		msg->nlmsg_flags |= NLM_F_REQUEST | NLM_F_ACK;
		int sendn = msg->nlmsg_len;
		int ret = 0;
		const unsigned char* data = (const unsigned char*)msg;
		while(true){
			ret = ::send(fd_,data,sendn,0);
			if(ret == sendn){
				break;
			}else if(ret == -1){return false;}
			data += ret;
			sendn -= ret;
		}
	}


	void Monitor(){
		char buf[4096] = {0};
		while(true){
			unsigned int retval = ::read(fd_,buf,sizeof(buf));
			if((int)retval <= 0){continue;}
			for(nlmsghdr* nh = (nlmsghdr*)buf;NLMMSG_OK(nh,retval); nh = NLMSG_NEXT(nh,retval)){
				if(nh->nlmsg_type == NLMSG_DONE){
					break;
				}
				handle_->Run();
			}

		}
	}
private:
	unsigned int pid_;
	Messagehandle* handle_;
};


class iprouteImp{
public:
	iprouteImp(){}
	~iprouteImp(){}
	bool Init(){
		bool ret = talk_socket_->init();
		return ret;
	}

private:
	scoped_ptr<NLsocket> talk_socket_;

};
iprouteImp::iprouteImp(){
	talk_socket_.reset(new NLsocket);
	Init();
}

class nic_manager{
public:
	nic_manager(NLsocket* talk_sock):talk_sock_(talk_sock){
		msg_.reset(new NlMsg);
	}
	~nic_manager(){}

	NlMsg* Getmsg(){
	   return msg_.get();
	}
	
	void Run(){
	    talk_sock_->talk(Getmsg());
	}

private:
	NLsocket* talk_sock_;
	scoped_ptr<NlMsg>  msg_;
};
