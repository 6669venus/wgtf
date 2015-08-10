//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  perforce_depot_view.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "perforce_depot_view.hpp"
#include "perforce_result.hpp"
#include "core_logging/logging.hpp"

#include <sstream>
#include <regex>

#pragma warning(push)
#pragma warning(disable:4267 4244)
#include "p4/clientApi.h"
#include "p4/spec.h"
#pragma warning(pop)

template<class TCollection>
std::string join(const TCollection& col)
{
	std::stringstream stream;
	for (auto i : col)
	{
		stream << i << std::endl;
	}
	return stream.str().c_str();
}

class P4ClientUser : public ClientUser
{
public:
	typedef std::vector<std::string> Errors;
	typedef std::vector<std::string> Output;

	void SetInput(const char* input)
	{
		input_ = input;
	}

	const Errors& GetErrors() const { return errors_; }
	const Output& GetOutput() const { return output_; }

	IResultPtr result() const
	{
		auto output = join(GetOutput());
		auto errors = join(GetErrors());
		return IResultPtr(new PerforceResult(output.c_str(), errors.c_str()));
	}

private:
	void OutputError(const char* error) override
	{
		//ClientUser::OutputError(error);
		errors_.emplace_back(error);
	}

	virtual void OutputInfo(char level, const char *data) override
	{
		//ClientUser::OutputInfo(level, data);
		output_.emplace_back(data);
	}

	virtual void OutputText(const char *data, int length) override
	{
		//ClientUser::OutputText(data, length);
		output_.emplace_back(data);
	}

	virtual void InputData(StrBuf *strbuf, Error *e) override
	{
		strbuf->Set(input_.c_str());
	}

	std::string input_;
	Errors errors_;
	Output output_;
};

struct PerforceDepotView::PerforceDepotViewImplementation
{
	PerforceDepotViewImplementation(ClientApiPtr clientApi)
		: clientApi(std::move(clientApi))
	{
	}

	~PerforceDepotViewImplementation()
	{
		Error e;
		clientApi->Final(&e);
	}
	ClientApiPtr clientApi;
};

PerforceDepotView::PerforceDepotView(ClientApiPtr clientApi, const char* /*depotPath*/, const char* clientPath)
	: impl_(new PerforceDepotViewImplementation(std::move(clientApi)))
{
	//auto results = RunCommand(std::string("client -o"));
	//NGT_TRACE_MSG("%s", results->output());
	//results = RunCommand(std::string("info"));
	//NGT_TRACE_MSG("%s", results->output());
}

IResultPtr PerforceDepotView::add(const PathList& filePaths, ChangeListId changeListId)
{
	std::stringstream command;
	if (changeListId != kDefaultChangelist)
	{
		command << "add -c " << changeListId << " -f " << EscapePaths(filePaths);
	}
	else
	{
		command << "add -f " << EscapePaths(filePaths);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::remove(const PathList& filePaths, ChangeListId changeListId)
{
	std::stringstream command;
	// The -v flag means that the files do not first have to be on the client
	if (changeListId != kDefaultChangelist)
	{
		command << "delete -c " << changeListId << " -v " << EscapePaths(filePaths);
	}
	else
	{
		command << "delete -v " << EscapePaths(filePaths);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::checkout(const PathList& filePaths, ChangeListId changeListId)
{
	std::stringstream command;
	if (changeListId != kDefaultChangelist)
	{
		command << "edit -c " << changeListId << EscapePaths(filePaths);
	}
	else
	{
		command << "edit " << EscapePaths(filePaths);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::rename(const FilePairs& filePairs, ChangeListId changeListId)
{
	std::unique_ptr<MultiResult> results(new MultiResult());
	for (auto pair : filePairs)
	{
		results->appendResult(move(pair.first.c_str(), pair.second.c_str(), changeListId));
	}
	return std::move(results);
}

IResultPtr PerforceDepotView::move(const char* srcFile, const char* dstFile, ChangeListId changeListId)
{
	std::stringstream command;
	PathList paths;
	paths.emplace_back(srcFile);
	paths.emplace_back(dstFile);
	getLatest(paths);
	checkout(paths, changeListId);
	if (changeListId != kDefaultChangelist)
	{
		command << "move -c " << changeListId << " -f " << EscapePaths(paths);
	}
	else
	{
		command << "move -f " << EscapePaths(paths);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::revert(const PathList& filePaths)
{
	std::string command("revert ");
	command += EscapePaths(filePaths);
	return RunCommand(command);
}

IResultPtr PerforceDepotView::get(const PathList& filePaths, Revision revision)
{
	std::stringstream command;
	if (revision == kHeadRevion)
	{
		command << "sync -f " << EscapePaths(filePaths);
	}
	else
	{
		command << "sync -f " << EscapeRevisionPaths(filePaths, revision);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::getLatest(const PathList& filePaths)
{
	return get(filePaths, kHeadRevion);
}

IResultPtr PerforceDepotView::status(const PathList& filePaths)
{
	std::string command("fstat ");
	command += EscapePaths(filePaths);
	return RunCommand(command);
}

IResultPtr PerforceDepotView::submit(const PathList& filePaths, const char* description /*= ""*/, bool bKeepCheckedOut /*= false*/)
{
	ChangeListId changelist;
	auto result = createChangeList(description, changelist);
	if (!result->hasErrors())
	{
		result = reopen(filePaths, changelist);
		if (!result->hasErrors())
		{
			return submit(changelist, bKeepCheckedOut);
		}
	}
	return result;
}

IResultPtr PerforceDepotView::submit(int changelistId, bool bKeepCheckedOut /*= false*/)
{
	std::stringstream command;
	if (bKeepCheckedOut)
	{
		command << "submit -r -c " << changelistId;
	}
	else
	{
		command << "submit -c " << changelistId;
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::reopen(const PathList& filePaths, ChangeListId changeListId)
{
	std::stringstream command;
	if (changeListId != kDefaultChangelist)
	{
		command << "reopen -c " << changeListId << " " << EscapePaths(filePaths);
	}
	else
	{
		command << "reopen -c default " << EscapePaths(filePaths);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::createChangeList(const char* description, ChangeListId& rChangeListId)
{
	std::stringstream input;
	
	input << "Change:	new" << std::endl;
	input << "Client:	" << impl_->clientApi->GetClient().Text() << std::endl;
	input << "User:	" << impl_->clientApi->GetUser().Text() << std::endl;
	input << "Status:	new" << std::endl;
	auto formatted = regex_replace(description, std::regex("(\r\n|\r|\n)"), std::string("$1\t"));
	input << "Description:" << formatted << std::endl;
	auto result = RunCommand(std::string("change -i"), input.str());
	if (result->output() && *result->output())
	{
		// Successful output will be something like "Change 23603 created."
		std::string output(result->output());
		auto& start = *std::find(output.begin(), output.end(), ' ');
		rChangeListId = atoi(&start);
	}
	return std::move(result);
}

IResultPtr PerforceDepotView::deleteEmptyChangeList(ChangeListId changeListId)
{
	if (changeListId == kDefaultChangelist)
		return IResultPtr(new PerforceResult("", "Invalid changelist"));

	std::stringstream command;
	command << "change -d " << changeListId;
	return RunCommand(command.str());
}

std::vector<char*> SplitParams(std::string &command)
{
	std::regex paramSplit("[^\\s\"']+|\"[^\"]*\"|'[^']*'");
	std::vector<char*> params;
	std::vector<char*> ends;
	for (auto i = std::cregex_iterator(command.data(), &command.data()[command.size()], paramSplit);
		i != std::cregex_iterator();
		++i)
	{
		for (auto match : *i)
		{
			if (match.length() > 0)
			{
				// Save the end positions to null terminate our strings
				auto end = match.second;
				while (*(end-1) == '"') --end;
				ends.emplace_back(const_cast<char*>(end));
				auto start = match.first;
				while (*start == '"') ++start;
				params.emplace_back(const_cast<char*>(start));
			}
		}
	}
	// Null terminate the strings, must be done after we find our matches
	for (auto& end : ends)
		*end = '\0';

	return std::move(params);
}

IResultPtr PerforceDepotView::RunCommand(std::string& command, const std::string& input)
{
	auto params = SplitParams(command);
	if (params.size() > 0)
	{
		P4ClientUser ui;
		impl_->clientApi->SetVar("enableStreams");
		if (params.size() > 1)
			impl_->clientApi->SetArgv(static_cast<int>(params.size()) - 1, &params[1]);
		else
			impl_->clientApi->SetArgv(0, nullptr);
		if (!input.empty())
			ui.SetInput(input.c_str());
		impl_->clientApi->Run(params[0], &ui);
		return ui.result();
	}
	
	return IResultPtr( new PerforceResult("", "Invalid Command") );
}

bool IsLegalChar(char c)
{
	return (c != '@' && c != '#' && c != '%' && c != '*');
}

std::string& ReplaceChars(std::string& depotPath)
{
	// This function takes a path that may contain illegal perforce characters.
	// It replaces the illegal characters with the hexadecimal equivalent, making them legal
	char newSubStr[4] = { 0 };

	for (int i = static_cast<int>(depotPath.size()) - 1; i >= 0; --i)
	{
		auto c = depotPath[i];
		if (!IsLegalChar(c))
		{
			// Replace the illegal char with "%" followed by the hex char value  (example "#" --> "%23")
			sprintf(newSubStr, "%%%2X", static_cast<unsigned char>(c));
			depotPath.replace(i, 1, newSubStr);
		}
	}
	return depotPath;
}

std::string PerforceDepotView::EscapePaths(const PathList& filePaths)
{
	std::stringstream escapedPaths;
	for (auto path : filePaths)
	{
		escapedPaths << '"' << ReplaceChars(path) << '"';
	}
	return escapedPaths.str();
}

std::string PerforceDepotView::EscapeRevisionPaths(const PathList& filePaths, const Revision revision)
{
	std::stringstream escapedPaths;
	for (auto path : filePaths)
	{
		escapedPaths << '"' << ReplaceChars(path) << "#" << revision << '"';
	}
	return escapedPaths.str();
}

