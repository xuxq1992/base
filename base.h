#pragma once
#include <Windows.h>
#include <iostream>
#include <assert.h>
//#include <stdint.h>
//#define DISALLOW_COPY_AND_ASSIGN(TypeName) \   
//	TypeName(const TypeName&); \
//	void operator = (const TypeName&)
#include <map>




class Base{
public:
	virtual ~Base(){}  //基类对派生类对象做操作时只能影响到从基类继承下的成员，所以作virtual
	virtual void run()=0;
};

#define Test(type) class type##Base : public Base{void run(){}}  //宏定义

Test(a);
Test(b);

// class aBase;public Base{}............................

//#define max(a,b) a>b ? a : b

void method(int a){
    printf("ssssss");
}
class AA{
public:
	typedef  void(*meth)(int a);  //回调函数所用
	AA(){}
	~AA(){}
	void setcall(meth cb){
	_newcall = cb; 
	}
	void run(int a){
	_newcall(a);
	}
private:
	meth _newcall;
};


template<class C>
class scoped_ptr{
public:
	typedef C element_type;

	explicit scoped_ptr(C* p =NULL):_ptr(p){}

	~scoped_ptr(){
		enum{
		    type_must_be_complete = sizeof(C)
		};
		delete _ptr;
	}
	void reset(C* p = NULL){
		if(p != _ptr){
		    enum{
		        type_must_be_complete = sizeof(C)
			};
		    delete _ptr;
			_ptr = p;
		}
	}

	C& operator*() const{
	    assert(_ptr != NULL);
		return *_ptr;
	}
	C* operator->() const{
	    assert(_ptr != NULL);
		return _ptr;
	}
	C* get()const{
	    return _ptr;
	}

	bool operator==(C* p) const{
	    return _ptr == p;
	}

	bool operator!=(C* p) const{
	    return _ptr != p;
	}

	void swap(scoped_ptr& p2){
	   C* tmp = _ptr;
	   _ptr  = p2._ptr;
	   p2._ptr = tmp;
	}

	C* release() {
	    C* retVal = _ptr;
		_ptr = NULL;
		return retVal;
	}

private:
	C* _ptr;

	scoped_ptr(const scoped_ptr&);
	void operator=(const scoped_ptr&);
};

class Closure{
public:
	Closure(){}
	virtual ~Closure(){};
	virtual void Run() = 0;
	//virtual void run() = 0;

};



template<class T>   //callback共用一部分代码，只是参数bool不一样
class methClosure:public Closure{
public :
	typedef void(T::*methodtype)();
	methClosure(T* c,methodtype method1,bool self_del):_c(c),_method1(method1),_self_del(self_del){
	
	}
	virtual ~methClosure(){}
	void run(){
     (_c->*_method1)();
	 if(_self_del) delete this;
	}
private:
	T* _c;
	methodtype _method1;
	bool _self_del;
};

class StoppableClosure : public Closure{
public:
	explicit StoppableClosure(Closure* c)
		: _c(c),_exit(false){}
	virtual ~StoppableClosure(){}
	virtual void Run(){
		while(!_exit){
		    _c->Run();
		}
	}
	void Stop(){
	    if(_exit)return;
		_exit = true;
	}
private:
	scoped_ptr<Closure>_c;
	bool _exit;
	//DISALLOW_COPY_AND_ASSIGN(StoppableClosure);
};

template<typename C>
inline Closure* Callback(C* c,void (C::*meth)())
{
	return new methClosure<C>(c,meth,true);
}

template<typename C>
inline Closure* permanentCallback(C* c,void (C::*meth)())
{
	return new methClosure<C>(c,meth,false);
}
class Thread{
public:
	Thread(Closure* c)
		:_handle(NULL),_c(c){
	}
	bool Start(){
		_handle = ::CreateThread(NULL,0,&Thread::StartThread,(void*)_c.get(),0,NULL);
		if(_handle != NULL){
		    return true;
		}
		return false;
	}

	void join(){
		if(_handle != NULL){
			::WaitForSingleObject(_handle,INFINITE);
			CloseHandle(_handle);
			_handle = NULL;
		}
	}

	~Thread(){
	   join(); 
	}
private:
	HANDLE _handle;
	scoped_ptr<Closure> _c;
	static DWORD WINAPI StartThread(void* obj){
		Closure* cb = (Closure*) obj;
		cb->Run();
		return 0;
	}
};

class StoppableThread : public Thread{
public:
	explicit StoppableThread(Closure* c)
		:Thread(stoppable_closure_ = new StoppableClosure(c)){}//如果子类没有实现父类的纯虚函数，那么这个子类也是抽象类，例如closure中的run()
	virtual ~StoppableThread(){}
	virtual void Join(){
	    stoppable_closure_->Stop();
		Thread::join();
	}
private:
	StoppableClosure* stoppable_closure_;
};


class AutoThread{
public:
	static void Start(Closure* c){
	    new AutoThread(c);
	}
private:
	Closure* _c;

	explicit AutoThread(Closure* c):_c(c){
	
		Closure* cb(_c);
		//Closure* cb = callback(this,AutoThread::run );
		HANDLE handle = CreateThread(NULL,0,&StartThread,(void*)cb,0,NULL);
		CloseHandle(handle);
	}

	~AutoThread(){}

	/*void run(){
		_c->run();
		delete this;
	}*/
	static DWORD WINAPI StartThread(void* obj){
	
		Closure* cb = (Closure*) obj;
		cb->Run();
		return 0;
	}
};

inline void Newautothread(Closure* c){
	AutoThread::Start(c);
}


class ins{
public:
	ins(){
		_thread.reset(new Thread(Callback(this,&ins::method)));
	}
	~ins(){}
    void method(){
	    int a;
        while(true){
		   std::cin>> a;
		   std::cout<<a<<std::endl;
	   }
    }
	void dosomething(){
		_thread->Start();
	
	}
private:
	scoped_ptr<Thread> _thread;
};
//linux   pthread.h
/*class autoThread{
public:
	static void Start(Cmdrun* c){
	    new autoThread(c);
	}
private:
	Cmdrun* _c;

	explicit autoThread(Cmdrun* c):_c(c){
	    Cmdrun* cb(_c);
		pthread_t id;
		Cmdrun* cb(_c);
		int ret = pthread_create(&id,NULL,&StartThread,(void*)cb);
	}

	~autoThread(){}
	static void StartThread(void* obj){
	    Cmdrun* cb = (Cmdrun*) obj;
		cb->run();
	}
};*/


//插件模式：

class PluginContainer;
class Plugin{
public:
	virtual ~Plugin(){}

	//int Type() const{
	 //   return type_;
	//}

	int Index() const{
	    return index_;
	}
	
	void notify(int status);
protected:
	Plugin(int type,int index,PluginContainer* owner):type_(type),index_(index){
	}
	
	PluginContainer* owner_;
	
	void Registe(){
		owner_->attachplugin(this);
	}
	
	virtual void Run() = 0;

	bool Needupdate(int status){
	    return type_&status;
	}
private:
	int type_;
	int index_;
};

void Plugin::notify(int status){
    if(!Needupdate(status)) return;
	Run();
}
//for linux
// pthread.h

//class Mutex{
//public:
//	Mutex(){
//		pthread_mutex_init(&_pmutex,NULL);
//	}
//	~Mutex(){
//	    pthread_mutex_destroy(&_pmutex);
//	}
//	void Lock(){
//	    pthread_mutex_lock(&_pmutex);
//	}
//	void Unlock(){
//	    pthread_mutex_unlock(&_pmutex);
//	}
//private:
//	pthread_mutex_lock _pmutex;
//
//
//};


//class scopedMutex{
//public:
//	scopedMutex(Mutex* mutex):_mutex(mutex){
//	    _mutex->Lock();
//	}
//	~scopedMutex(){
//	    _mutex->Unlock();
//	}
//private:
//	Mutex* _mutex;
//};


class PluginContainer{
public:
	~PluginContainer();
	
	static PluginContainer* Instance(){
	    static PluginContainer plugin;
		return &plugin;
	}
	
	void Notify(int status){
		//scopedMutex l(_mutex);    //互斥锁用处
		for(Pluginmap::iterator it = _pluginmap.begin();it != _pluginmap.end();it++){
			Plugin* plugin = it->second;
			plugin->notify(status);
		}
	}
	void attachplugin(Plugin* plugin){
	    if(plugin = NULL) return;
		Pluginmap::iterator vit = _pluginmap.find(plugin->Index());
		if(vit != _pluginmap.end())return;

		_pluginmap[plugin->Index()] = plugin;

	}
	void detachplugin(Plugin* plugin){
	    removeplugin(plugin);
	}
private:
	PluginContainer();
	typedef std::map<int,Plugin*> Pluginmap;
	Pluginmap _pluginmap;

	//Mutex* _mutex;//多线程访问同一个对象的不同操作，需要添加一个互斥锁防止出现野指针等现象

	void removeplugin(Plugin* plugin){
		Pluginmap::iterator vit = _pluginmap.find(plugin->Index());
		if(vit == _pluginmap.end())return;

		_pluginmap.erase(vit);
	}
};


class Fplugin : public Plugin{
public:
	Fplugin(PluginContainer* owner):Plugin(100,1,owner){
	    Registe();
	}
	
	void Run(){
	    printf("do Fplugin run...............1111");
	}
};
class Splugin : public Plugin{
public:	
	Splugin(PluginContainer* owner):Plugin(101,2,owner){
	    Registe();
	}
	void Run(){
	    printf("do Splugin run...............2222");
	}
};
