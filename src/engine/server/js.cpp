#include <engine/console.h>
#include <v8.h>

#include "js.h"

using namespace v8;

CJs::CJs()
{
	m_pConsole = 0;
}

void CJs::Init(IConsole *pConsole)
{
	m_pConsole = pConsole;

	HandleScope scope;

	Handle<ObjectTemplate> global_template = ObjectTemplate::New();

	Persistent<Context> context = Context::New(0, global_template);
	Context::Scope context_scope(context);

	Handle<String> source = String::New("'Hello' + ', World!'");
	Handle<Script> script = Script::Compile(source);

	Handle<Value> result = script->Run();

	String::Utf8Value utf8(result);

	m_pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "js", *utf8);
}
