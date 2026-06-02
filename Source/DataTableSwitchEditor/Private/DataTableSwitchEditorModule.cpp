#include "Modules/ModuleManager.h"

class FDataTableSwitchEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FDataTableSwitchEditorModule, DataTableSwitchEditor)
