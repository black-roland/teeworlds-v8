#ifndef ENGINE_SERVER_JS_H
#define ENGINE_SERVER_JS_H

class CJs
{
  class IConsole *m_pConsole;
  class IStorage *m_pStorage;

public:
	CJs();
	void Init(class IConsole *pConsole, class IStorage *pStorage);

private:
	void LoadFiles();
	static int LoadFilesCallback(const char *pName, int IsDir, int DirType, void *pUser);
	int LoadScript(const char *pFilename);
};

#endif
