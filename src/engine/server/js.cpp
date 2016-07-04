#include <engine/console.h>
#include <engine/storage.h>
#include <v8.h>

#include "js.h"

using namespace v8;

CJs::CJs()
{
	m_pConsole = 0;
	m_pStorage = 0;
}

void CJs::Init(IConsole *pConsole, IStorage *pStorage)
{
	m_pConsole = pConsole;
	m_pStorage = pStorage;

	HandleScope pScope;

	Handle<ObjectTemplate> globalTemplate = ObjectTemplate::New();
	Persistent<Context> context = Context::New(0, globalTemplate);
	Context::Scope contextScope(context);

	auto global = context->Global();

	auto jsConsolePrint = FunctionTemplate::New([](const Arguments& args) -> Handle<Value>
	{
		if (args.Length() < 1) return v8::Undefined();
		HandleScope scope;
		Handle<Value> arg = args[0];
		String::Utf8Value value(arg);
		dbg_msg("js", *value);
		return v8::Undefined();
	});

	auto jsConsole = Object::New();
	jsConsole->Set(String::NewSymbol("print"), jsConsolePrint->GetFunction());

	global->Set(String::NewSymbol("console"), jsConsole);

	LoadFiles();
}

void CJs::LoadFiles()
{
	m_pStorage->ListDirectory(IStorage::TYPE_ALL, "js", LoadFilesCallback, this);
}

int CJs::LoadFilesCallback(const char *pName, int IsDir, int DirType, void *pUser)
{
	CJs *pSelf = (CJs *)pUser;

	int l = str_length(pName);
	if(l < 3 || IsDir || str_comp(pName+l-3, ".js") != 0)
		return 0;

	char pFilename[512];
	str_format(pFilename, sizeof(pFilename), "js/%s", pName);

	dbg_msg("js", "loading script '%s'", pFilename);
	pSelf->LoadScript(pFilename);

	return 0;
}

int CJs::LoadScript(const char *pFilename)
{
	FILE* file = fopen(pFilename, "rb");

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	char* chars = new char[size + 1];
	chars[size] = '\0';
	for (int i = 0; i < size;) {
		int read = static_cast<int>(fread(&chars[i], 1, size - i, file));
		i += read;
	}
	fclose(file);

	Handle<String> source = String::New(chars, size);
	delete[] chars;

	if (source.IsEmpty()) {
		dbg_msg("js", "error reading '%s'", pFilename);
		return 1;
	}

	Handle<Script> script = Script::Compile(source);

	script->Run();

	return 0;
}
