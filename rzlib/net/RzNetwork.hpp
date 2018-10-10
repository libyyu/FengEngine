#ifndef __RZNETWORK_HPP__
#define __RZNETWORK_HPP__
#pragma once
#include <stdio.h>
#include <string>
#include "RzSocket.hpp"
_RzStdBegin
_RzNameSpaceBegin(Net)
class IRzLinker
{
public:
	virtual void    onConnected(IRzLinker* h) = 0;
	virtual void    onDisconnected(IRzLinker* h,const int ec) = 0;
	virtual void    onSendCompleted(IRzLinker* h,const void* pbuffer,const int ec,size_t bytes) = 0;
	virtual size_t  onRecvCompleted(IRzLinker* h,const void* pbuffer,size_t bytes) = 0;
	virtual void    onDestroy(IRzLinker* h) = 0;
};

class IRzListener
{
public:
	virtual void    onConnected(IRzListener*,IRzLinker*) = 0;
	virtual void    onDisconnected(IRzListener*,IRzLinker*,const int ec) = 0;
	virtual void    onSendCompleted(IRzListener*,IRzLinker*,const void* pbuffer,const int ec,size_t bytes) = 0;
	virtual size_t  onRecvCompleted(IRzListener*,IRzLinker*,const void* pbuffer,size_t bytes) = 0;
	virtual void    onDestroy(IRzListener*,IRzLinker*) = 0;
};
_RzNameSpaceEnd
_RzStdEnd

#endif//__RZNETWORK_HPP__