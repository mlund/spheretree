/*************************************************************************\

                             C O P Y R I G H T

  Copyright 2003 Image Synthesis Group, Trinity College Dublin, Ireland.
                        All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following paragraphs appear in all copies.


                             D I S C L A I M E R

  IN NO EVENT SHALL TRININTY COLLEGE DUBLIN BE LIABLE TO ANY PARTY FOR
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING,
  BUT NOT LIMITED TO, LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
  AND ITS DOCUMENTATION, EVEN IF TRINITY COLLEGE DUBLIN HAS BEEN ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGES.

  TRINITY COLLEGE DUBLIN DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND TRINITY
  COLLEGE DUBLIN HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
  ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted at the following e-mail addresses:

          Gareth_Bradshaw@yahoo.co.uk    isg@cs.tcd.ie

  Further information about the ISG and it's project can be found at the ISG
  web site :

          isg.cs.tcd.ie

\**************************************************************************/

#include "CallStack.h"
#include <windows.h>
#include <imagehlp.h>
#include <tlhelp32.h>

#define MAX_SYM_NAME 256
#define UND_FLAGS                                                              \
  UNDNAME_COMPLETE | UNDNAME_NO_MS_KEYWORDS | UNDNAME_NO_SPECIAL_SYMS |        \
      UNDNAME_NO_ACCESS_SPECIFIERS | UNDNAME_NO_MEMBER_TYPE

/*
  load the symbols for all modules (use ToolHelp)
*/
typedef BOOL(WINAPI *MODULEWALK)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef BOOL(WINAPI *THREADWALK)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
typedef BOOL(WINAPI *PROCESSWALK)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef HANDLE(WINAPI *CREATESNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);

BOOL SymLoadSymbolsToolHelp(HANDLE hProcess, LONG iProcess) {
  HINSTANCE hKernel = GetModuleHandleA("KERNEL32.DLL");
  if (!hKernel)
    return FALSE;

  CREATESNAPSHOT pCreateToolhelp32Snapshot;
  pCreateToolhelp32Snapshot =
      (CREATESNAPSHOT)GetProcAddress(hKernel, "CreateToolhelp32Snapshot");
  if (!pCreateToolhelp32Snapshot)
    return FALSE;

  MODULEWALK pModule32First, pModule32Next;
  pModule32First = (MODULEWALK)GetProcAddress(hKernel, "Module32First");
  pModule32Next = (MODULEWALK)GetProcAddress(hKernel, "Module32Next");
  if (!pModule32First || !pModule32Next)
    return FALSE;

  MODULEENTRY32 me;
  me.dwSize = sizeof(MODULEENTRY32);
  HANDLE hSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, iProcess);
  BOOLEAN gotMod = pModule32First(hSnap, &me);
  while (gotMod) {
    SymLoadModule(hProcess, 0, me.szExePath, me.szModule, (DWORD)me.modBaseAddr,
                  me.modBaseSize);
    gotMod = pModule32Next(hSnap, &me);
  }
  CloseHandle(hSnap);

  return TRUE;
}

/*
  load the symbols for all modules (use PSAPI.DLL)
*/
typedef BOOL(WINAPI *ENUMPROCESSMODULES)(HANDLE hProcess, HMODULE *lphModule,
                                         DWORD cb, LPDWORD lpcbNeeded);

struct MODULEINFO {
  LPVOID lpBaseOfDll;
  DWORD SizeOfImage;
  LPVOID EntryPoint;
};

typedef BOOL(WINAPI *GETMODULEINFORMATION)(HANDLE hProcess, HMODULE hModule,
                                           MODULEINFO *lpmodinfo, DWORD cb);

BOOL SymLoadSymbolsPSAPI(HANDLE hProcess, LONG iProcess) {
  //  load DLL
  HINSTANCE hInst = LoadLibraryA("PSAPI.DLL");
  if (hInst == NULL)
    return FALSE;

  // get functions
  ENUMPROCESSMODULES pEnumProcessModules;
  pEnumProcessModules =
      (ENUMPROCESSMODULES)GetProcAddress(hInst, "EnumProcessModules");
  GETMODULEINFORMATION pGetModuleInfo;
  pGetModuleInfo =
      (GETMODULEINFORMATION)GetProcAddress(hInst, "GetModuleInformation");
  if (!pEnumProcessModules || !pGetModuleInfo)
    return FALSE;

  //  get modults
  DWORD cbNeeded;
  HMODULE modules[250];
  pEnumProcessModules(hProcess, modules, 250 * sizeof(modules), &cbNeeded);
  int numMods = cbNeeded / sizeof(HMODULE);

  //  load modules
  MODULEINFO modInfo;
  char imagePath[1024];
  GetModuleFileName(NULL, imagePath, 1023);

  for (int i = 0; i < numMods; i++) {
    //  get info
    pGetModuleInfo(hProcess, modules[i], &modInfo, sizeof(modInfo));

    if (!SymLoadModule(hProcess, 0, imagePath, NULL, (DWORD)modInfo.EntryPoint,
                       modInfo.SizeOfImage))
      break;
  }

  return TRUE;
}
/*
BOOL SymLoad(HANDLE hProcess, LONG iProcess){
  MODULEENTRY32 me;
  me.dwSize = sizeof(MODULEENTRY32);
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, iProcess);
  BOOLEAN gotMod = Module32First(hSnap, &me);
  while (gotMod){
    SymLoadModule(hProcess, 0, me.szExePath, me.szModule, (DWORD)me.modBaseAddr,
me.modBaseSize); gotMod = Module32Next(hSnap, &me);
    }
  CloseHandle(hSnap);

  return TRUE;
}
*/

//  internals for stack walking, needs to be in SEH filter
int dumpCallStackInternal(struct _EXCEPTION_POINTERS *ePtr, FILE *f,
                          const char *indent) {
  //  no error structure
  if (!ePtr)
    return EXCEPTION_EXECUTE_HANDLER;

  //  header
  fprintf(f, "\tCall Stack : \n");

  //  handles for thread and process
  HANDLE hProcess = GetCurrentProcess();
  HANDLE hThread = GetCurrentThread();
  LONG iProcess = GetCurrentProcessId();

  // initialize the symbol engine
  SymSetOptions((SymGetOptions() | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES) &
                ~SYMOPT_UNDNAME);
  SymInitialize(hProcess, NULL, FALSE);

  // SymLoad(hProcess, iProcess);
  if (SymLoadSymbolsToolHelp(hProcess, iProcess)) { //  TRY TOOLHELP
    fprintf(f, "%s [using PSAPI.DLL]\n", indent);
    SymLoadSymbolsPSAPI(
        hProcess,
        iProcess); //  FALLBACK FOR NT4 (psapi.dll needs to be included)
  }

  //  need to setup get stack frame for first walk
  STACKFRAME stkFrame;
  ZeroMemory(&stkFrame, sizeof(stkFrame));
  stkFrame.AddrPC.Offset = ePtr->ContextRecord->Eip;
  stkFrame.AddrPC.Mode = AddrModeFlat;
  stkFrame.AddrStack.Offset = ePtr->ContextRecord->Esp;
  stkFrame.AddrStack.Mode = AddrModeFlat;
  stkFrame.AddrFrame.Offset = ePtr->ContextRecord->Ebp;
  stkFrame.AddrFrame.Mode = AddrModeFlat;

  //  will need a place to store the symbols
  IMAGEHLP_SYMBOL *pSym =
      (IMAGEHLP_SYMBOL *)malloc(sizeof(IMAGEHLP_SYMBOL) + MAX_SYM_NAME + 1);
  memset(pSym, '\0', sizeof(IMAGEHLP_SYMBOL) + MAX_SYM_NAME + 1);
  pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
  pSym->MaxNameLength = MAX_SYM_NAME;

  //  and the undecorated name
  char undecName[MAX_SYM_NAME + 1];

  //  and the line info, NT5 :(
  // IMAGEHLP_LINE line;
  // ZeroMemory(&line, sizeof(line));
  // line.SizeOfStruct = sizeof(line);

  //  and module info
  IMAGEHLP_MODULE mod;
  ZeroMemory(&mod, sizeof(mod));
  mod.SizeOfStruct = sizeof(mod);

  //  walk the stack dumping the info we want
  while (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stkFrame, NULL,
                   NULL, SymFunctionTableAccess, SymGetModuleBase, NULL)) {

    //  symbol name
    DWORD disp = 0;
    if (SymGetSymFromAddr(hProcess, stkFrame.AddrPC.Offset, &disp, pSym)) {
      //  indent
      if (indent)
        fprintf(f, indent);

      //  get the prototype name
      if (UnDecorateSymbolName(pSym->Name, undecName, MAX_SYM_NAME, UND_FLAGS))
        fprintf(f, "%s ", undecName);
      else if (UnDecorateSymbolName(pSym->Name, undecName, MAX_SYM_NAME,
                                    UNDNAME_NAME_ONLY))
        fprintf(f, "%s ", undecName);
      else
        fprintf(f, "%s  ", pSym->Name);

      //  offset in bytes
      fprintf(f, "%+ld bytes ", disp);

      // get line number, NT5 :(
      // SymGetLineFromAddr(hProcess, stkFrame.AddrPC.Offset, &disp, &line);
      // fprintf(f, "%s (%d) ", line.FileName, line.LineNumber);

      //  module info
      if (SymGetModuleInfo(hProcess, stkFrame.AddrPC.Offset, &mod))
        fprintf(f, "[%s]", mod.ModuleName);

      //  end line
      fprintf(f, "\n");
    } else {
      break;
    }
  }

  //  tidy up Symbol Engine
  free(pSym);
  SymCleanup(hProcess);

  //  just get the except block to execute
  return EXCEPTION_EXECUTE_HANDLER;
}

//  dump the call stack
void dumpCallStack(FILE *f, const char *indent) {
  //  need to generate an SEH error to do dump
  __try {
    int *i = NULL;
    *i = 69;
  } __except (dumpCallStackInternal(GetExceptionInformation(), f, indent)) {
  }
}
