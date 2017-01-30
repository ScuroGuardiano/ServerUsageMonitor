///////////////////////////////////////////////////////////////
// Program jest bardzo prosty, nas³uchuje na porcie 6666
// Po tym jak ktoœ siê po³¹czy wysy³a mu informacje o u¿yciu
// w stringu oddzielone œrednikami:
// totalRAM;availableRAM;RAMUsage;totalVirtualMem;availableVirtualMem;virtualMemUsage;CPUUsage
// a nastêpnie zrywa po³¹czenie.
// Zastosowa³em tak¹ metodê, poniewa¿ dane zamierzam odbieraæ
// AJAXem przez poœrednicz¹cy skrypt PHP
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// U¿yte biblioteki zewnêtrzne:
// -> SFML 2.4
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#define MB 1048576
#define PORT 6666

using namespace std;

mutex usageInfoMutex;


struct UsageInfo
{
	DWORDLONG totalRAM;
	DWORDLONG availableRAM;
	DWORDLONG RAMUsage;
	DWORDLONG totalVirtualMem;
	DWORDLONG availableVirtualMem;
	DWORDLONG virtualMemUsage;
	double CPUUsage;
};

class UsageInformator
{
public:
	UsageInformator() {};
	void init()
	{
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		PdhOpenQuery(NULL, NULL, &cpuQuery);
		PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
		PdhCollectQueryData(cpuQuery);
	}

	UsageInfo GetUsageInfo()
	{
		UsageInfo usageInfo;

		PDH_FMT_COUNTERVALUE counterVal;
		GlobalMemoryStatusEx(&memInfo);
		PdhCollectQueryData(cpuQuery);
		PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		usageInfo.totalVirtualMem = memInfo.ullTotalPageFile;
		usageInfo.availableVirtualMem = memInfo.ullAvailPageFile;
		usageInfo.virtualMemUsage = usageInfo.totalVirtualMem - usageInfo.availableVirtualMem;

		usageInfo.totalRAM = memInfo.ullTotalPhys;
		usageInfo.availableRAM = memInfo.ullAvailPhys;
		usageInfo.RAMUsage = usageInfo.totalRAM - usageInfo.availableRAM;

		usageInfo.CPUUsage = counterVal.doubleValue;
		return usageInfo;
	}

private:
	MEMORYSTATUSEX memInfo;
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;
};

void ListeningThread(UsageInformator * usageInformator);
string UsageInfoToString(UsageInfo * ui);

int main()
{
	UsageInformator usageInformator;
	usageInformator.init();

	thread wontek(ListeningThread, &usageInformator);

	while (true)
	{
		usageInfoMutex.lock();
		UsageInfo UI = usageInformator.GetUsageInfo();
		usageInfoMutex.unlock();

		system("cls");
		cout << "CPU Usage: " << UI.CPUUsage << "%" << endl;
		cout << "RAM Usage: " << UI.RAMUsage / MB << "/" << UI.totalRAM / MB << " ("
			<< static_cast<int>((double)UI.RAMUsage / UI.totalRAM * 100) << "%)" << endl;
		cout << "Virtual Memory Usage: " << UI.virtualMemUsage / MB << "/" << UI.totalVirtualMem / MB << " ("
			<< static_cast<int>((double)UI.virtualMemUsage / UI.totalVirtualMem * 100) << "%)" << endl;
		
		Sleep(1000);
	}

    return 0;
}

void ListeningThread(UsageInformator * usageInformator)
{
	///W¹tek nas³uchuje -> akceptuje klienta -> wysy³a mu info o u¿yciu -> disconnectuje \/
	///			/\ <----------------------------------------------------------------------
	sf::TcpListener lystener;

	if (lystener.listen(PORT) == sf::Socket::Done)
	{
		while (true)
		{
			sf::TcpSocket * client = new sf::TcpSocket;
			if (lystener.accept(*client) == sf::Socket::Done)
			{
				usageInfoMutex.lock();
				UsageInfo ui = usageInformator->GetUsageInfo();
				usageInfoMutex.unlock();
				string dane = UsageInfoToString(&ui);
				client->send(dane.c_str(), dane.size() + 1);
				client->disconnect();
			}

			delete client;
		}
	}
}

string UsageInfoToString(UsageInfo * ui)
{
	///Funkcja zamienia w najprostszy string, bez ¿adnego zbêdnego pier...
	stringstream ss;
	ss << ui->totalRAM << ";" << ui->availableRAM << ";" << ui->RAMUsage << ";"
		<< ui->totalVirtualMem << ";" << ui->availableVirtualMem << ";" << ui->virtualMemUsage << ";"
		<< ui->CPUUsage;
	return ss.str();
}
