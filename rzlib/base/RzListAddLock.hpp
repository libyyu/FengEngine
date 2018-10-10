#ifndef   __RZLISTADDLOCK_HPP__
#define   __RZLISTADDLOCK_HPP__
#pragma once
#include "RzDReadAWriteLock.hpp"
_RzStdBegin
template <class T>
class CRzListAddLock
{
public:
	CRzListAddLock();
	~CRzListAddLock();
private:
	struct Node
	{
		Node* pNext;
		T data;
        
		Node():data(T()),pNext(NULL){};
	};
#if PLATFORM_TARGET == PLATFORM_WINDOWS
	typedef bool (__stdcall *EnumVaulePro)(T&,void*);
#else
    typedef bool (*EnumVaulePro)(T&,void*);
#endif
private:
	CRzDReadAWriteLock m_Lock;
	unsigned int m_nCount;
	Node* m_pHead;
public:
	void Add(T& data);
	bool Del(T& data);
	void Clear();
	bool Empty();
	unsigned int GetCount();
	bool EnumVaule(EnumVaulePro pEnumFun,void* pParam = NULL);
};
template <class T>
CRzListAddLock<T>::CRzListAddLock():m_pHead(NULL),m_nCount(0)
{
	
}
template <class T>
CRzListAddLock<T>::~CRzListAddLock()
{
	Clear();
}
template <class T>
void CRzListAddLock<T>::Add(T &data)
{
	Node* pNewNode = new Node;
	if (!pNewNode) return;
	pNewNode->data = data;

	m_Lock.EnterWrite();
	{
		if (!m_pHead)
		{
			m_pHead = pNewNode;		
		}
		else
		{
			pNewNode->pNext = m_pHead;
			m_pHead = pNewNode;
		}	
		++m_nCount;
	}
	m_Lock.LeaveWrite();
}
template <class T>
bool CRzListAddLock<T>::Del(T &data)
{
	bool bResult = false;
	Node* pDelNode = NULL;
	Node* pTmpNode = NULL;
	Node* pFirNode = NULL;
	m_Lock.EnterWrite();
	{
		if (!m_pHead)	goto __End_Fun;
		
		if (data == m_pHead->data)
		{
			pDelNode = m_pHead;
			m_pHead->pNext ?(m_pHead = m_pHead->pNext) : m_pHead = NULL;		
			goto __End_Fun;
		}

		pFirNode = m_pHead;
		pTmpNode = m_pHead->pNext;  //ע��ǰ���Ѿ��ų���ͷ�ڵ�Ŀ��ܣ��������ָ��ֱ��ָ��ͷ�ڵ����һ���ڵ�
		while(pTmpNode)
		{
			if (data == pTmpNode->data)
			{
				pDelNode = pTmpNode;
				pTmpNode->pNext ?(pFirNode->pNext  = pDelNode->pNext):(pFirNode->pNext = NULL);				
				goto __End_Fun;
			}
			pTmpNode = pTmpNode->pNext;
		}	
	}

__End_Fun:
	if (pDelNode)
	{
		bResult = true;
		delete pDelNode;
		pDelNode = NULL;
		--m_nCount;
	}
	m_Lock.LeaveWrite();
	return bResult;
}
template <class T>
void CRzListAddLock<T>::Clear()
{
	Node* pDleNode = NULL;
	m_Lock.EnterWrite();
	{
		while (m_pHead)
		{
			pDleNode = m_pHead;
			m_pHead->pNext ? (m_pHead = m_pHead->pNext):(m_pHead = NULL);			
     
			delete pDleNode;
			pDleNode = NULL;
			--m_nCount;
		}
	}
	m_Lock.LeaveWrite();
}
template <class T>
bool CRzListAddLock<T>::Empty()
{
	bool bResult = false;
	m_Lock.EnterRead();
	m_pHead ? bResult = false : bResult =true;
	m_Lock.LeaveRead();
	return bResult;
}

template <class T>
unsigned int CRzListAddLock<T>::GetCount()
{
	unsigned int nResult = 0;
	m_Lock.EnterRead();
	nResult = m_nCount;
	m_Lock.LeaveRead();
	return nResult;
}

template <class T>
bool CRzListAddLock<T>::EnumVaule(EnumVaulePro pEnumFun,void* pParam /* = NULL */)
{
	bool bResult = false;
	m_Lock.EnterRead();
	{
		Node* pNode = NULL;
		pNode = m_pHead;
		while(pNode)
		{
			if ( pEnumFun(pNode->data,pParam))
			{
				bResult = true;
				break;
			}
			pNode = pNode->pNext;
		}
	}
	m_Lock.LeaveRead();
	return bResult;
}
_RzStdEnd
#endif//__RZLISTADDLOCK_HPP__