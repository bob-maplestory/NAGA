#include "stdafx.h"
#include "log.h"
#include "RegistryUtil.h"
#include <malloc.h>
#include <comdef.h>
#include <CRTDBG.H>
#include <atlconv.h>

const wchar_t *GetWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}


bool
reg_key_callback(
	_In_ uint32_t index,
	_In_ const wchar_t* base_name,
	_In_ const wchar_t* sub_key_name,
	_In_ const wchar_t* class_name,
	_In_ DWORD_PTR tag
	)
{
	UNREFERENCED_PARAMETER(base_name);
	UNREFERENCED_PARAMETER(tag);
	if (!wcscmp(sub_key_name, TEXT("nProtect Online Security V1.0(PFS)"))) {
		log_err "index=%d sub_key_name = %ws",index,sub_key_name log_end;
	}
	else {
		log_dbg
			"index=%d sub_key_name = %ws",
			index,sub_key_name
			log_end;
	}
	return true;
}

bool
reg_value_callback(
	_In_ uint32_t index,
	_In_ uint32_t value_type,
	_In_ const wchar_t* value_name,
	_In_ uint32_t value_data_size,
	_In_ const uint8_t* value_data,
	_In_ DWORD_PTR tag
	)
{
	UNREFERENCED_PARAMETER(index);
	UNREFERENCED_PARAMETER(value_type);
	UNREFERENCED_PARAMETER(value_data_size);
	UNREFERENCED_PARAMETER(value_data);
	UNREFERENCED_PARAMETER(tag);
	if (!wcscmp(value_name, TEXT("UninstallString")) || !wcscmp(value_name, TEXT("DisplayName")) || !wcscmp(value_name, TEXT("DisplayVersion")))
	{
		log_dbg
			"%ws=%ws",
			value_name,value_data
			log_end;

	}
	return true;
}


bool
reg_enum_key(
	_In_ HKEY key,
	_In_ const wchar_t* base_name,
	_In_ fn_key_callback_tag key_cb,
	_In_ DWORD_PTR key_cb_tag,
	_In_ fn_key_value_callback_tag value_cb,
	_In_ DWORD_PTR value_cb_tag
)
{
	_ASSERTE(nullptr != key);
	if (nullptr == key) return false;

	DWORD	 sub_key_count = 0;
	DWORD	 max_sub_key_name_cc = 0;
	DWORD	 max_class_name_cc = 0;
	DWORD	 value_count = 0;
	DWORD	 max_value_name_cc = 0;
	DWORD	 max_value_data_byte = 0;
	DWORD	 max_security_desciptor_byte = 0;
	FILETIME last_write_time = { 0 };

	LSTATUS ret = RegQueryInfoKeyW(key,
		nullptr,
		nullptr,
		nullptr,
		&sub_key_count,
		&max_sub_key_name_cc,
		&max_class_name_cc,
		&value_count,
		&max_value_name_cc,
		&max_value_data_byte,
		&max_security_desciptor_byte,
		&last_write_time);
	if (ERROR_SUCCESS != ret)
	{
		log_err "RegQueryInfoKeyW() failed. ret = %u",
			ret
			log_end;
		return false;
	}

	//log_dbg
	//	"sub_key_count = %u, \n"\
	//	"max_sub_key_name_size = %u, \n"\
	//	"max_class_name_size = %u, \n"\
	//	"value_count = %u, \n"\
	//	"max_value_name_size = %u, \n"\
	//	"max_value_data_size = %u, \n"\
	//	"max_security_desciptor_size = %u",
	//	sub_key_count,
	//	max_sub_key_name_cc,
	//	max_class_name_cc,
	//	value_count,
	//	max_value_name_cc,
	//	max_value_data_byte,
	//	max_security_desciptor_byte
	//	log_end;

	// xxx_cc 값은 null terminate 를 포함하지 않는 character count 이다. 
	// RegEnumKeyEx() 같은 함수들은 파라미터를 받을때 null terminate 를 포함한 
	// 버퍼와 버퍼의 사이즈를 파라미터로 받기 때문에 편의상 xxx_cc 에 +1 을 해준다. 
	++max_sub_key_name_cc;
	++max_class_name_cc;
	++max_value_name_cc;

	// enum reg key
	if (NULL != key_cb)
	{
		if (0 < sub_key_count)
		{
			for (DWORD i = 0; i < sub_key_count; ++i)
			{
				wchar_t* sub_key_name = (wchar_t*)malloc(max_sub_key_name_cc * sizeof(wchar_t));
				wchar_t* class_name = (wchar_t*)malloc((max_class_name_cc) * sizeof(wchar_t));

				if (nullptr == sub_key_name)
				{
					log_err "Not enough memory. size=%u",
						max_sub_key_name_cc * sizeof(wchar_t)
						log_end;
					continue;
				}

				if (nullptr == class_name)
				{
					log_err "Not enough memory. size=%u",
						max_class_name_cc * sizeof(wchar_t)
						log_end;
					continue;
				}

				DWORD sub_key_name_cc = max_sub_key_name_cc;
				DWORD class_name_cc = max_class_name_cc;

				ret = RegEnumKeyEx(key,		//key
					i,						//index
					sub_key_name,			//name
					&sub_key_name_cc,		//
					nullptr,
					class_name,
					&class_name_cc,
					&last_write_time);
				if (ERROR_SUCCESS != ret)
				{
					log_err "RegEnumKeyEx() failed. ret = %u",
						ret
						log_end;
				}
				else
				{
					if (true != key_cb(i,
						base_name,
						sub_key_name,
						class_name,
						key_cb_tag))
					{
						// caller canceled;
						break;
					}
					
						//	
						//	하위 모두 읽기
						//
						USES_CONVERSION;						
						char result[5000] = {};
						const char* target = W2A(sub_key_name);
						const char* path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
						strcat(result, path);
						strcat(result, target);
						const wchar_t * test = GetWC(result);

						HKEY K = RUOpenKey(HKEY_LOCAL_MACHINE, (const wchar_t*)test, true);
						reg_enum_key(K,
							nullptr,
							NULL,
							NULL,
							reg_value_callback,
							NULL);
						RegCloseKey(K);
					
				}

				free_and_nil(sub_key_name);
				free_and_nil(class_name);
			}
		}
	}

	// enum reg values
	if (NULL != value_cb)
	{
		if (0 < value_count)
		{
			//log_dbg "value count = %u", value_count log_end;
			wchar_t* value_name = (wchar_t*)malloc(max_value_name_cc * sizeof(wchar_t));
			BYTE* value_data = (BYTE*)malloc(max_value_data_byte);
			if (nullptr != value_name && nullptr != value_data)
			{
				for (DWORD i = 0; i < value_count; i++)
				{
					RtlZeroMemory(value_name, (max_value_name_cc) * sizeof(wchar_t));
					RtlZeroMemory(value_data, max_value_data_byte);

					DWORD value_type = REG_NONE;
					DWORD value_name_cc = max_value_name_cc;
					DWORD value_data_size = max_value_data_byte;

					ret = RegEnumValue(key,
						i,
						value_name,
						&value_name_cc,
						NULL,
						&value_type,
						value_data,
						&value_data_size);
					if (ret == ERROR_SUCCESS)
					{
						//if (!wcscmp(value_name, L"UninstallString"))
						//{
						//	log_dbg
						//		"index = %u, value_name = %ws, value_type = %u, data = %ws",
						//		i,
						//		value_name,
						//		value_type,
						//		(char*)value_data
						//		log_end;
						//}
						if (true != value_cb(i,
							value_type,
							value_name,
							value_data_size,
							value_data,
							value_cb_tag))
						{
							// caller calceld.
							break;
						}
					}
					else
					{
						log_err "RegEnumValue() failed. ret = %u", ret log_end;
					}
				}
			}

			free_and_nil(value_name);
			free_and_nil(value_data);
		}
	}

	return true;
}

int main()
{
	//
	//	Initialize log 
	//
	initialize_log(0xffffffff, log_level_debug, log_to_con,NULL);//| log_to_file, L"log.txt");
	

	//
	//	Enumerate keys under Software
	//
	HKEY key = RUOpenKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"), true);//\\nProtect Online Security V1.0(PFS)", true);

	reg_enum_key(key,
						nullptr,
						reg_key_callback,
						NULL,
						reg_value_callback,
						NULL);
	RegCloseKey(key);


	log_err "Press any key to terminate..." log_end;

	finalize_log();
	return true;
}

