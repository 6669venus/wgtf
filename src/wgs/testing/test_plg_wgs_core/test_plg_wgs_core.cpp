#include "stdafx.h"
#include "CppUnitTest.h"
#include "generic_plugin_manager/generic_plugin_manager.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "serialization/interfaces/i_file_system.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace UnitTests
{		
	TEST_CLASS(UnitTest1)
	{
		static DWORD runThread(LPVOID args)
		{
			GenericPluginManager pluginManager;
			std::vector<std::wstring> plugins;
			LPTSTR  modulePath = new TCHAR[_MAX_PATH];
			::GetModuleFileName((HINSTANCE)&__ImageBase, modulePath, _MAX_PATH);
			std::wstring basePath(modulePath);
			basePath = basePath.substr(0, basePath.rfind('\\'));
			plugins.emplace_back(basePath + L"/plugins/plg_wgs_core");
			pluginManager.loadPlugins(plugins);

			auto& fileSystem_ = *pluginManager.getContextManager().getGlobalContext()->queryInterface<IFileSystem>();
			char tempPath[MAX_PATH] = { 0 };
			::GetTempPathA(MAX_PATH, tempPath);
			Assert::IsTrue(fileSystem_.exists(tempPath));
			Assert::IsTrue(fileSystem_.getFileInfo(tempPath).isDirectory());
			Assert::IsTrue(fileSystem_.getFileType(tempPath) == IFileSystem::Directory);

			char filename[MAX_PATH] = { 0 };
			char filename2[MAX_PATH] = { 0 };
			::GetTempFileNameA(tempPath, "", 0, filename);
			::GetTempFileNameA(tempPath, "", 0, filename2);
			Assert::IsTrue(fileSystem_.exists(filename));
			Assert::IsTrue(fileSystem_.exists(filename2));
			//Assert::IsTrue(fileSystem_.remove(filename));
			//Assert::IsTrue(fileSystem_.remove(filename2));
			//Assert::IsTrue(!fileSystem_.exists(filename));
			//Assert::IsTrue(!fileSystem_.exists(filename2));

			//auto data = "TestString";
			//auto datalen = strlen(data);
			//fileSystem_.writeFile(filename, data, datalen, std::ios::out);
			//Assert::IsTrue(fileSystem_.exists(filename));
			//Assert::IsTrue(fileSystem_.getFileType(filename) == IFileSystem::File);
			//{
			//	auto stream = fileSystem_.readFile(filename, std::ios::in);
			//	char buffer[MAX_PATH] = { 0 };
			//	stream->readRaw(&buffer, datalen);
			//	Assert::IsTrue(strcmp(data, buffer) == 0);
			//}
			//
			//Assert::IsTrue(fileSystem_.copy(filename, filename2));
			//Assert::IsTrue(fileSystem_.exists(filename2));
			//{
			//	auto stream = fileSystem_.readFile(filename2, std::ios::in);
			//	char buffer[MAX_PATH] = { 0 };
			//	stream->readRaw(&buffer, datalen);
			//	Assert::IsTrue(strcmp(data, buffer) == 0);
			//}
			//
			//Assert::IsTrue(fileSystem_.remove(filename2));
			//Assert::IsTrue(!fileSystem_.exists(filename2));
			//
			//Assert::IsTrue(fileSystem_.move(filename, filename2));
			//Assert::IsTrue(!fileSystem_.exists(filename));
			//Assert::IsTrue(fileSystem_.exists(filename2));
			//{
			//	auto stream = fileSystem_.readFile(filename2, std::ios::in);
			//	char buffer[MAX_PATH] = { 0 };
			//	stream->readRaw(&buffer, datalen);
			//	Assert::IsTrue(strcmp(data, buffer) == 0);
			//}
			//
			//Assert::IsTrue(fileSystem_.remove(filename2));
			//Assert::IsTrue(!fileSystem_.exists(filename2));
			//
			std::vector<FileInfo> infos;
			fileSystem_.enumerate(tempPath, [&](FileInfo&& info){
				infos.emplace_back(std::move(info));
				return true;
			});

			fileSystem_.enumerate("\\", [&](FileInfo&& info){
				Assert::IsTrue(fileSystem_.exists(info.fullPath.c_str()));
				Assert::IsTrue(fileSystem_.getFileInfo(info.fullPath.c_str()).size == info.size);
				return true;
			});
			return 0;
		}
	public:
		TEST_METHOD(TestWGSFileSystem)
		{
			// Force to run on a thread otherwise ThreadLocalBase::FreeTlsId() will assert when our test thread exits
			DWORD threadId = 0;
			HANDLE handle = CreateThread(NULL, 0, &runThread, nullptr, 0, &threadId);
			WaitForSingleObject(handle, INFINITE);
		}
	};
}