/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence    : http://creativecommons.org/licenses/by-nc-sa/3.0/fr/
	Ce fichier : http://creativecommons.org/licenses/by/3.0/fr/
*/
#pragma once
#include "../sekurlsa.h"
#include "msv1_0_helper.h"

bool searchMSVFuncs();
__kextdll bool __cdecl getMSVFunctions(mod_pipe * monPipe, vector<wstring> * mesArguments);
__kextdll bool __cdecl getMSV(mod_pipe * monPipe, vector<wstring> * mesArguments);
bool WINAPI getMSVLogonData(__in PLUID logId, __in mod_pipe * monPipe, __in bool justSecurity);

__kextdll bool __cdecl getLogonSessions(mod_pipe * monPipe, vector<wstring> * mesArguments);
__kextdll bool __cdecl delLogonSession(mod_pipe * monPipe, vector<wstring> * mesArguments);
__kextdll bool __cdecl addLogonSession(mod_pipe * monPipe, vector<wstring> * mesArguments);

void InitLsaStringToBuffer(LSA_UNICODE_STRING * LsaString, wstring &maDonnee, wchar_t monBuffer[]);
LUID wstringsToLUID(wstring &highPart, wstring &lowPart);
