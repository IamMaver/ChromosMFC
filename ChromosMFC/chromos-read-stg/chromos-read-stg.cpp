// chromos-read-stg.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include <locale>
#include "chromosfile.h"


int _tmain(int argc, _TCHAR* argv[])
{
	_tsetlocale(LC_ALL, _T("Russian"));

	if (argc != 2)
	{
		_tprintf(_T("Usage: %s filename.stg\n"), argv[0]);
		return 1;
	}

	chromosfile_t stg_file{};

	int err = ChromosFileRead(argv[1], stg_file);
	if (err)
	{
		_tprintf(_T("File read error occurred: %d\n"), err);
		return -1;
	}

	const auto& ps = stg_file.passport;
	_tprintf(_T("Sample: %s\n"), ps.sample.c_str());
	_tprintf(_T("Duration: %.2f\n"), ps.duration);
	_tprintf(_T("Method: %s\n"), ps.method.c_str());

	_tprintf(_T(" N           Height             Area    Concentration                 Name\n"));
	int n = 1;
	for (const auto& it : stg_file.peaks)
	{
		_tprintf(_T("%02d %16.06f %16.06f %16.06f %20s\n"),
			n++, it.height, it.area, it.concent, it.comment.c_str());
	}

	return 0;
}
