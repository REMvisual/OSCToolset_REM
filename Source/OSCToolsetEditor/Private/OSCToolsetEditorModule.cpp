#include "Modules/ModuleManager.h"

class FOSCToolsetEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FOSCToolsetEditorModule, OSCToolsetEditor)
