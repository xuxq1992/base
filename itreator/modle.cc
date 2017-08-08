#include <stdio.h>
#include <map>
#include <string>

using namespace std;

class A{
public:
	A(){}
	virtual ~A(){}
	static void open(A **p);
	virtual void dosth()=0;
};


class B : public A{
public:
	B(){}
	~B(){}
	void dosth();

};

void B::dosth(){
    printf("dosomething________________ok\n");
}

void A::open(A **p){
    B* a = new B();
    *p = a;
}

class C{  
public:
	 C(){}
	 ~C(){}
	 void init();
	 void dosomething();
private :
	 A* expl_;
	 //dbIterator iter_;
};

 
void C::init(){
    A* expl ;
    expl->open(&expl);
	expl_ = expl;

}
void C::dosomething(){
	 expl_->dosth();
}

typedef     map<string,C*> MY_MAP;
MY_MAP mymap;
class DBMap{
public :
	DBMap(){}
	~DBMap(){}

	
	bool valid(){
	   return it_ != end_;
	}
	bool seek(){
	   it_ = mymap_.begin();
	   end_ = mymap_.end();
	   return 1;
	}
	void Next(){
	   ++it_;
	}
	void Get(){
		printf("%s\n",it_->first.c_str());
		it_->second->dosomething();
	};
	void Put(){
		//it_->second->dosomething();
	}

	void Add(string key){
		C* p = new C();
		p->init();
	    mymap_[key] = p;
	}
	void Del(string key){
		mymap_.erase(key);
	}
private:
	MY_MAP mymap_;
	MY_MAP::iterator it_;
	MY_MAP::iterator end_;
	
};


void trans_path(const char* path, char* dst){
	string s(path);
	string spath = "/work" + s;
	strcpy(dst, spath.c_str());
}

void main(){
    string path1 = "C:/test1";
	string path2 = "C:/test2";
    
 
	DBMap* it = new DBMap();
	it->Add(path1);
	it->Add(path2);
	it->seek();
	while(it->valid()){
		it->Get();
		it->Next();
	}

	system ("pause");
}
