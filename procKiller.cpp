/*
    Process Killer
     - find an open window (by name)
     - get said windows PID
     - open that said windows proc (get Handle)
     - kill said window (proc)

    ** Written all in one piece coz I've read somewhere 
     its lighter and faster that way. 
     in any case, it's shorter.

    step 1 - FindWindowA
     - User32.dll
     - return HWND ? Handle : Null
        LPCSTR lpClassName, // if null => will find by name
        LPCSTR lpWindowName // if not null => uses GetWindowText for comparason

    step 2 - GetWindowThreadProcessId
     - User32.dll
     - return DWORD ? PID : ??
        HWND    hWnd            // handle to window
        LPDWORD lpdwProcessId   // null or copy (byReff)

    step 3 - OpenProcess
     - Kernel32.dll
     - return HANDLE ? openHanld : null
        DWORD dwDesiredAccess,  // desired access
        BOOL  bInheritHandle,   // will inheret handle (true - false)
        DWORD dwProcessId       // the PID we want to open. core(0x0)/null => invalid param

    step 4 - TerminateProcess
     - Kernel32.dll
     - return BOOL ? nonZero : zero
        HANDLE hProcess,    // handle to proc ** must have PROCESS_TERMINATE 
        UINT   uExitCode    // exit code result
*/

// -=Includes=-
#include <iostream>    
#include <windows.h>   

using std::string; 

// -=Headers=-
string errMsg(int errCode);

// -=Funcs=-
string errMsg(int errCode){
    string msg = "Empty_string";

    switch(errCode){
        case (0x00):msg = "ERROR_SUCCESS";              break; // (00) The operation completed successfully.
        case (0x01):msg = "ERROR_INVALID_FUNCTION";     break; // (01) Incorrect function.
        case (0x05):msg = "ERROR_ACCESS_DENIED";        break; // (02) Access is denied.
        case (0x57):msg = "ERROR_INVALID_PARAMETER";    break; // (87) The parameter is incorrect.

        default: msg = ("Unknown ErrCode : " + std::to_string(errCode)); break;
    }
    return msg;
}



// -=Main=-
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow){    
    std::cout<< "-= Start =-\n";
//=====================================================================

//== Input
    char windowName[100];
    std::cout << "Enter window name to terminate: ";
    std::cin.getline(windowName,sizeof(windowName));
    std::cout << " - " << windowName<<'\n';

//== Find Window
    std::cout << "Running => findWindowA \n";
    HWND wHandle = FindWindowA(NULL, windowName);
    
    if (NULL == wHandle){
        std::cout<< "Error type: "<<errMsg(GetLastError())<<"\n"; 
        return (0);
    }else if (wHandle == 0){
        std::cout<< "Window not found. (Is it open?..)\n"; 
        return (0);
    }else {
        std::cout<< " - Got window handle: "<<wHandle<<'\n';
    }

//=== Get window PID
    std::cout << "Running => GetWindowThreadProcessId \n";
    
    DWORD lpdwProcessId;    // null or copy (byReff)  
    GetWindowThreadProcessId(wHandle, &lpdwProcessId);
   
    std::cout<<" - Got pId : "<<lpdwProcessId<<"\n";

//== get window proc handle 
    std::cout << "Running => OpenProcess \n";

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, false, lpdwProcessId);

    if (NULL == pHandle){
        int errCode = GetLastError();
        std::cout<< "Error type: "<<errMsg(errCode)<<"\n"; 
        if(errCode == 87){
            std::cout<< "PID might be null or CoreProc (0x00)\n"; 
        }
        return (0);
    }else {
        std::cout<< " - Got proc handle: "<<pHandle<<'\n';
    }

//== KillProc
    std::cout << "Running => TerminateProcess \n";
    
    UINT exitCode;
    bool killSuccess = TerminateProcess(pHandle,exitCode);

    if (killSuccess == 0){
        std::cout<< "Failed to kill proc\n"; 
        std::cout<< "Error type: "<<errMsg(GetLastError())<<"\n"; 
        return (0);
    }else {
        std::cout<< " - Proc kille. exitCode "<<exitCode<<'\n';
    }

//=====================================================================
    std::cout<< "\n-= End =-";
    return 0;
}















