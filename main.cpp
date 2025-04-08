#include <windows.h>
#include <iostream>
#include <filesystem>
#include <shlobj.h>

namespace fs = std::filesystem;

bool is_admin(){
    HANDLE token;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
	{
		TOKEN_ELEVATION elevation{};
		DWORD size;

		if (GetTokenInformation(token, TokenElevation, &elevation, sizeof elevation, &size))
			return elevation.TokenIsElevated;
	}
	return false;
}

std::string getUsername() {
    char username[256];
    DWORD size = sizeof(username);
    if (GetUserName(username, &size)) {
        return std::string(username);
    } else {
        return "Problem with username";
    }
}

bool isFileInUse(const std::string& filePath) {
    HANDLE hFile = CreateFile(filePath.c_str(),
                              GENERIC_WRITE,
                              0, 
                              NULL, 
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return true;
    }

    CloseHandle(hFile);
    return false;
}

std::string username = getUsername();
std::string tempwithuserPATH = "C:\\Users\\" + username + "\\AppData\\Local\\Temp";
std::string tempwithwindows = "C:\\Windows\\Temp";
std::string chromecashPATH = "C:\\Users\\" + username + "\\AppData\\Local\\Google\\Chrome\\User Data";

void deletetempwithuser() {
    try {
        for (const auto& entry : fs::directory_iterator(tempwithuserPATH)) {
            std::string filePath = entry.path().string();
            if (isFileInUse(filePath)) {
                
            } else {
                fs::remove(filePath); 
            }
        }
        std::cout << "All deletable files processed from: " << tempwithuserPATH << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void deletetempwithwindows(){
    try {
        for (const auto& entry : fs::directory_iterator(tempwithwindows)) {
            std::string filePath = entry.path().string();
            if (isFileInUse(filePath)) {
                
            } else {
                fs::remove(filePath); 
            }
        }
        std::cout << "All deletable files processed from: " << tempwithwindows << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void ClearRecycleBin() {
    HRESULT hr = SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI);
    
    if (SUCCEEDED(hr)) {
        std::cout << "Recycle Bin cleared successfully!" << std::endl;
    } else {
        std::cout << "Error emptying or was empty recycle bin" << std::endl;
    }
}

void cashchrome() {
    for (const auto& entry : fs::directory_iterator(chromecashPATH)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            if (name.find("Profile ") == 0 || name == "Default") {
                fs::path cachePath = entry.path() / "Cache" / "Cache_Data";
                if (fs::exists(cachePath)) {
                    std::error_code ec;
                    fs::remove_all(cachePath, ec);
                }
            }
        }
    }
    std::cout << "cache deleted" << std::endl;
}

void ClearMemory() {
    HANDLE hProcess = GetCurrentProcess();
    if (SetProcessWorkingSetSize(hProcess, -1, -1)) {
        std::cout << "Memory cleared successfully!" << std::endl;
    } else {
        std::cout << "Failed to clear memory." << std::endl;
    }
}

void clearEventLogs(){
    system("wevtutil cl Application");
    system("wevtutil cl Security");
    system("wevtutil cl System");
    system("wevtutil cl Setup");
    system("wevtutil cl ForwardedEvents");

    std::cout << "Event logs have been cleared." << std::endl;
}

void Stop_Service(const char* serviceName) {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager == NULL) return;
    
    SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_STOP);
    if (hService == NULL) {
        CloseServiceHandle(hSCManager);
        return;
    }

    SERVICE_STATUS status;
    ControlService(hService, SERVICE_CONTROL_STOP, &status);
    
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
}

int main() {
    SetConsoleTitleA("Bye, Poor!");
    char choice;
    std::cout << "By Sl4ckStre1z" << std::endl;

    if (!is_admin())
	{
		std::cout << "Run the program as admin" << std::endl;
		system("pause");
		return 0;
	}


    std::cout << "Name: " << getUsername() << std::endl;
    std::cout << "Do you want to delete files from: " << tempwithuserPATH << " | Y / N: ";
    std::cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        deletetempwithuser();
    } else {
        std::cout << "Operation cancelled." << std::endl;
    }
    
    std::cout << "Do you want to delete files from: " << tempwithwindows << " | Y / N: ";
    std::cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        deletetempwithwindows();
    } else {
        std::cout << "Operation cancelled." << std::endl;
    }

    std::cout << "Do you want to delete cache from: " << chromecashPATH << " | Y / N: ";
    std::cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        cashchrome();
    } else {
        std::cout << "Operation cancelled." << std::endl;
    }

    std::cout << "Do you want to clear RAM | Y / N: ";
    std::cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        ClearMemory();
    } else {
        std::cout << "Operation cancelled." << std::endl;
    }

    std::cout << "Do you want to clear cache microsoft store? | Y / N: ";
    std::cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        system("wsreset.exe");
    } else {
        std::cout << "Operation cancelled." << std::endl;
    }

    std::cout << "Do you want to clear cache windows logs? | Y / N: ";
    std::cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        clearEventLogs();
    } else {
        std::cout << "Operation cancelled." << std::endl;
    }

    std::cout << "Do you want to disable Services? | Y / N: ";
    std::cin >> choice;
    if (choice == 'Y' || choice == 'y'){
        Stop_Service("wuauserv");  // Windows Update
        Stop_Service("WinDefend");  // Windows Defender
        Stop_Service("SysMain");    // Superfetch
        Stop_Service("WSearch");    // Windows Search
    }else{
        std::cout << "Operation cancelled." << std::endl;
    }
    ClearRecycleBin();
    system("pause");
    return 0;
}

//Steps
//1.Delete User Temp files
//2.Delete Windows Temp files
//3.Delete Cash From Chrome
//4.Clear RAM
//5.Clear Microsoft Store cache
//6.Clear Event Logs
//7.Stop service
//8.Recycle Bin

//Made By Sl4ckStre1z V:1.8