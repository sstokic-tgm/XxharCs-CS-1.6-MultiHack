#ifndef __MODULESECURITY_H__
#define __MODULESECURITY_H__

void HideModuleFromPEB(HINSTANCE hInstance);
void RemovePeHeader(DWORD ModuleBase);

#endif