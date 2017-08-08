#include "base.h"

int main(int argc, char** argv)
{
	AA *b = new AA();
	b->setcall(method);
	b->run(5);

	ins* a = new ins();
	a->dosomething();
	
	PluginContainer* pluginC = PluginContainer::Instance();
	new Fplugin(pluginC);
	new Splugin(pluginC);
	pluginC->Notify(100);
	pluginC->Notify(101);



	while(true){
	    Sleep(10);
	}
	return 0;
}
