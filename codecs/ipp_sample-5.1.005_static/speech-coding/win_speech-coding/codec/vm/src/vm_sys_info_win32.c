/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//       Copyright(c) 2003-2006 Intel Corporation. All Rights Reserved.
//
*/

#include "vm_sys_info.h"

#if defined(_WIN32) || defined(_WIN64)

void vm_sys_info_get_date(vm_char *m_date, DateFormat df)
{
    SYSTEMTIME SystemTime;

    /* check error(s) */
    if (NULL == m_date)
        return;

    GetLocalTime(&SystemTime );

    switch (df)
    {
    case DDMMYY:
        vm_string_sprintf(m_date,
                          VM_STRING("%.2d/%.2d/%d"),
                          SystemTime.wDay,
                          SystemTime.wMonth,
                          SystemTime.wYear);
        break;

    case MMDDYY:
        vm_string_sprintf(m_date,
                          VM_STRING("%.2d/%.2d/%d"),
                          SystemTime.wMonth,
                          SystemTime.wDay,
                          SystemTime.wYear);
        break;

    case YYMMDD:
        vm_string_sprintf(m_date,
                          VM_STRING("%d/%.2d/%.2d"),
                          SystemTime.wYear,
                          SystemTime.wMonth,
                          SystemTime.wDay);
        break;

    default:
        vm_string_sprintf(m_date,
                          VM_STRING("%2d/%.2d/%d"),
                          SystemTime.wMonth,
                          SystemTime.wDay,
                          SystemTime.wYear);
        break;
    }

} /* void vm_sys_info_get_date(vm_char *m_date, DateFormat df) */

void vm_sys_info_get_time(vm_char *m_time, TimeFormat tf)
{
    SYSTEMTIME SystemTime;

    /* check error(s) */
    if (NULL == m_time)
        return;

    GetLocalTime(&SystemTime);

    switch (tf)
    {
    case HHMMSS:
        vm_string_sprintf(m_time,
                          VM_STRING("%.2d:%.2d:%.2d"),
                          SystemTime.wHour,
                          SystemTime.wMinute,
                          SystemTime.wSecond);
        break;

    case HHMM:
        vm_string_sprintf(m_time,
                          VM_STRING("%.2d:%.2d"),
                          SystemTime.wHour,
                          SystemTime.wMinute);
        break;

    case HHMMSSMS1:
        vm_string_sprintf(m_time,
                          VM_STRING("%.2d:%.2d:%.2d.%d"),
                          SystemTime.wHour,
                          SystemTime.wMinute,
                          SystemTime.wSecond,
                          SystemTime.wMilliseconds / 100);
        break;

    case HHMMSSMS2:
        vm_string_sprintf(m_time,
                          VM_STRING("%.2d:%.2d:%.2d.%d"),
                          SystemTime.wHour,
                          SystemTime.wMinute,
                          SystemTime.wSecond,
                          SystemTime.wMilliseconds / 10);
        break;

    case HHMMSSMS3:
        vm_string_sprintf(m_time,
                          VM_STRING("%.2d:%.2d:%.2d.%d"),
                          SystemTime.wHour,
                          SystemTime.wMinute,
                          SystemTime.wSecond,
                          SystemTime.wMilliseconds);
        break;

    default:
        vm_string_sprintf(m_time,
                          VM_STRING("%.2d:%.2d:%.2d"),
                          SystemTime.wHour,
                          SystemTime.wMinute,
                          SystemTime.wSecond);
        break;
    }

} /* void vm_sys_info_get_time(vm_char *m_time, TimeFormat tf) */

void vm_sys_info_get_os_name(vm_char *os_name)
{
    OSVERSIONINFO osvi;
    BOOL bOsVersionInfo;

    /* check error(s) */
    if (NULL == os_name)
        return;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    bOsVersionInfo = GetVersionEx((OSVERSIONINFO *) &osvi);
    if (!bOsVersionInfo)
    {
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        if (!GetVersionEx((OSVERSIONINFO *) &osvi))
        {
            vm_string_sprintf(os_name, VM_STRING("Unknown"));
            return;
        }
    }

    switch (osvi.dwPlatformId)
    {
    case 2:
        /* test for the specific product family. */
        if ((5 == osvi.dwMajorVersion) && (2 == osvi.dwMinorVersion))
            vm_string_sprintf(os_name, VM_STRING("Win2003"));

        if ((5 == osvi.dwMajorVersion) && (1 == osvi.dwMinorVersion))
            vm_string_sprintf(os_name, VM_STRING("WinXP"));

        if ((5 == osvi.dwMajorVersion) && (0 == osvi.dwMinorVersion))
            vm_string_sprintf(os_name, VM_STRING("Win2000"));

        if (4 >= osvi.dwMajorVersion)
            vm_string_sprintf(os_name, VM_STRING("WinNT"));
        break;

        /* test for the Windows 95 product family. */
    case 1:
        if ((4 == osvi.dwMajorVersion) && (0 == osvi.dwMinorVersion))
        {
            vm_string_sprintf(os_name, VM_STRING("Win95"));
            if (('C' == osvi.szCSDVersion[1]) || ('B' == osvi.szCSDVersion[1]))
                vm_string_strcat(os_name, VM_STRING("OSR2" ));
        }

        if ((4 == osvi.dwMajorVersion) && (10 == osvi.dwMinorVersion))
        {
            vm_string_sprintf(os_name, VM_STRING("Win98"));
            if ('A' == osvi.szCSDVersion[1])
                vm_string_strcat(os_name, VM_STRING("SE"));
        }

        if ((4 == osvi.dwMajorVersion) && (90 == osvi.dwMinorVersion))
        {
            vm_string_sprintf(os_name,VM_STRING("WinME"));
        }
        break;

    case 3:
        /* get platform string */
        /* SystemParametersInfo(257, MAX_PATH, os_name, 0); */
        if ((4 == osvi.dwMajorVersion) && (20 == osvi.dwMinorVersion))
        {
            vm_string_sprintf(os_name, VM_STRING("PocketPC 2003"));
        }

        if ((4 == osvi.dwMajorVersion) && (21 == osvi.dwMinorVersion))
        {
            vm_string_sprintf(os_name, VM_STRING("WinMobile2003SE"));
        }

        if ((5 == osvi.dwMajorVersion) && (0 == osvi.dwMinorVersion))
        {
            vm_string_sprintf(os_name, VM_STRING("WinCE 5.0"));
        }
        break;

        /* Something else */
    default:
        vm_string_sprintf(os_name, VM_STRING("Win..."));
        break;
    }

} /* void vm_sys_info_get_os_name(vm_char *os_name) */

void vm_sys_info_get_program_name(vm_char *program_name)
{
    vm_char tmp[_MAX_LEN] = {0,};
    int i = 0;

    /* check error(s) */
    if (NULL == program_name)
        return;

    GetModuleFileName(NULL, tmp, _MAX_LEN);
    i = (int) (vm_string_strrchr(tmp, (vm_char)('\\')) - tmp + 1);
    vm_string_strncpy(program_name,tmp + i, vm_string_strlen(tmp) - i);

} /* void vm_sys_info_get_program_name(vm_char *program_name) */

void vm_sys_info_get_program_path(vm_char *program_path)
{
    vm_char tmp[_MAX_LEN] = {0,};
    int i = 0;

    /* check error(s) */
    if (NULL == program_path)
        return;

    GetModuleFileName(NULL, tmp, _MAX_LEN);
    i = (int) (vm_string_strrchr(tmp, (vm_char)('\\')) - tmp + 1);
    vm_string_strncpy(program_path, tmp, i - 1);

} /* void vm_sys_info_get_program_path(vm_char *program_path) */

vm_var32 vm_sys_info_get_cpu_num(void)
{
    SYSTEM_INFO siSysInfo;

    ZeroMemory(&siSysInfo, sizeof(SYSTEM_INFO));
    GetSystemInfo(&siSysInfo);

    return siSysInfo.dwNumberOfProcessors;

} /* vm_var32 vm_sys_info_get_cpu_num(void) */

vm_var32 vm_sys_info_get_mem_size(void)
{
    MEMORYSTATUS m_memstat;

    ZeroMemory(&m_memstat, sizeof(MEMORYSTATUS));
    GlobalMemoryStatus(&m_memstat);

    return (vm_var32)((double)m_memstat.dwTotalPhys / (1024 * 1024) + 0.5);

} /* vm_var32 vm_sys_info_get_mem_size(void) */

#if !defined(_WIN32_WCE)

#pragma comment(lib, "Setupapi.lib")
#include <setupapi.h>

void vm_sys_info_get_vga_card(vm_char *vga_card)
{
    SP_DEVINFO_DATA sp_dev_info;
    HDEVINFO DeviceInfoSet;
    vm_char string1[] = VM_STRING("Display");
    DWORD dwIndex = 0;
    vm_char data[_MAX_LEN] = {0,};

    /* check error(s) */
    if (NULL == vga_card)
        return;

    ZeroMemory(&sp_dev_info, sizeof(SP_DEVINFO_DATA));
    ZeroMemory(&DeviceInfoSet, sizeof(HDEVINFO));

    DeviceInfoSet = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    sp_dev_info.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    while (SetupDiEnumDeviceInfo(DeviceInfoSet, dwIndex, &sp_dev_info))
    {
        SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
                                         &sp_dev_info,
                                         SPDRP_CLASS,
                                         NULL,
                                         (unsigned char *) data,
                                         _MAX_LEN,
                                         NULL);
        if (!vm_string_strcmp((vm_char*)data, string1))
        {
            SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
                                             &sp_dev_info,
                                             SPDRP_DEVICEDESC,
                                             NULL,
                                             (PBYTE) vga_card,
                                             _MAX_LEN,
                                             NULL);
            break;
        }
        dwIndex++;
    }

    SetupDiDestroyDeviceInfoList(DeviceInfoSet);

} /* void vm_sys_info_get_vga_card(vm_char *vga_card) */

void vm_sys_info_get_cpu_name(vm_char *cpu_name)
{
    HKEY hKey;
    vm_char data[_MAX_LEN] = {0,};
    /* it can be wchar variable */
    DWORD dwSize = sizeof(data) / sizeof(data[0]);
    int i = 0;
    LONG lErr;

    /* check error(s) */
    if (NULL == cpu_name)
        return;

    lErr = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                        VM_STRING("Hardware\\Description\\System\\CentralProcessor\\0"),
                        0,
                        KEY_QUERY_VALUE,
                        &hKey);
    if (ERROR_SUCCESS == lErr)
    {
        RegQueryValueEx(hKey,
                        _T("ProcessorNameString"),
                        NULL,
                        NULL,
                        (LPBYTE)&data,
                        &dwSize);
        /* error protection */
        data[sizeof(data) / sizeof(data[0]) - 1] = 0;
        while (data[i++] == ' ');
        vm_string_strcpy(cpu_name, (vm_char *)(data + i - 1));
        RegCloseKey (hKey);

    }
    else
        vm_string_sprintf(cpu_name, VM_STRING("Unknown"));

} /* void vm_sys_info_get_cpu_name(vm_char *cpu_name) */

vm_var32 vm_sys_info_get_cpu_speed(void)
{
    HKEY hKey;
    vm_var32 data = 0;
    DWORD dwSize = sizeof(vm_var32);
    LONG lErr;

    lErr = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                        VM_STRING("Hardware\\Description\\System\\CentralProcessor\\0"),
                        0,
                        KEY_QUERY_VALUE,
                        &hKey);
    if (ERROR_SUCCESS == lErr)
    {
        RegQueryValueEx (hKey, _T("~MHz"), NULL, NULL, (LPBYTE)&data, &dwSize);

        RegCloseKey(hKey);

        return data;
    }
    else
        return 0;

} /* vm_var32 vm_sys_info_get_cpu_speed(void) */

void vm_sys_info_get_computer_name(vm_char *computer_name)
{
    vm_char data[_MAX_LEN] = {0,};
    /* it can be wchar variable */
    DWORD dwSize = sizeof(data) / sizeof(data[0]);

    /* check error(s) */
    if (NULL == computer_name)
        return;

    GetComputerName(data, &dwSize);
    vm_string_sprintf(computer_name, VM_STRING("%s"), data);

} /* void vm_sys_info_get_computer_name(vm_char* computer_name) */

#elif defined(_WIN32_WCE)

#include <winioctl.h>

BOOL KernelIoControl(DWORD dwIoControlCode,
                     LPVOID lpInBuf,
                     DWORD nInBufSize,
                     LPVOID lpOutBuf,
                     DWORD nOutBufSize,
                     LPDWORD lpBytesReturned);

#define IOCTL_PROCESSOR_INFORMATION CTL_CODE(FILE_DEVICE_HAL, 25, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _PROCESSOR_INFO
{
    vm_var16        wVersion;
    vm_char         szProcessCore[40];
    vm_var16        wCoreRevision;
    vm_char         szProcessorName[40];
    vm_var16        wProcessorRevision;
    vm_char         szCatalogNumber[100];
    vm_char         szVendor[100];
    vm_var32        dwInstructionSet;
    vm_var32        dwClockSpeed;

} PROCESSOR_INFO;

void vm_sys_info_get_vga_card(vm_char *vga_card)
{
    /* check error(s) */
    if (NULL == vga_card)
        return;

    vm_string_sprintf(vga_card, VM_STRING("Unknown"));

} /* void vm_sys_info_get_vga_card(vm_char *vga_card) */

void vm_sys_info_get_cpu_name(vm_char *cpu_name)
{
    PROCESSOR_INFO pi;
    DWORD dwBytesReturned;
    vm_var32 dwSize = sizeof(PROCESSOR_INFO);
    BOOL bResult;

    /* check error(s) */
    if (NULL == cpu_name)
        return;

    ZeroMemory(&pi, sizeof(PROCESSOR_INFO));
    bResult = KernelIoControl(IOCTL_PROCESSOR_INFORMATION,
                              NULL,
                              0,
                              &pi,
                              sizeof(PROCESSOR_INFO),
                              &dwBytesReturned);

    vm_string_sprintf(cpu_name,
                      VM_STRING("%s %s"),
                      pi.szProcessCore,
                      pi.szProcessorName);

} /* void vm_sys_info_get_cpu_name(vm_char *cpu_name) */

void vm_sys_info_get_computer_name(vm_char *computer_name)
{
    vm_char data[_MAX_LEN] = {0,};
    /* it can be wchar variable */
    DWORD dwSize = sizeof(data) / sizeof(data[0]);

    /* check error(s) */
    if (NULL == computer_name)
        return;

    SystemParametersInfo(SPI_GETOEMINFO, dwSize, data, 0);
    vm_string_sprintf(computer_name, VM_STRING("%s"), data);

} /* void vm_sys_info_get_computer_name(vm_char *computer_name) */

vm_var32 vm_sys_info_get_cpu_speed(void)
{
    PROCESSOR_INFO pi;
    vm_var32 res = 400;
    DWORD dwBytesReturned;
    vm_var32 dwSize = sizeof(PROCESSOR_INFO);
    BOOL bResult;

    ZeroMemory(&pi,sizeof(PROCESSOR_INFO));
    bResult = KernelIoControl(IOCTL_PROCESSOR_INFORMATION,
                              NULL,
                              0,
                              &pi,
                              sizeof(PROCESSOR_INFO),
                              &dwBytesReturned);

    if (pi.dwClockSpeed)
        res = pi.dwClockSpeed;

    return res;
} /* vm_var32 vm_sys_info_get_cpu_speed(void) */

#endif /* defined(_WIN32_WCE) */

#endif /* defined(_WIN32) || defined(_WIN64) */
