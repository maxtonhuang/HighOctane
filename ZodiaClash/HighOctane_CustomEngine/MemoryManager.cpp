/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		MemoryManager.cpp
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		1 February 2024
*
* *****************************************************************************
*
*	@brief		Memory Manager Class
*
*	This source file contains the implementation of a robust memory manager
*	that is fast and versatile.
*
******************************************************************************/



#include "MemoryManager.h"
#include <cstring>



/*****************************************************************************/
////////////////////////// MEMBER HELPER FUNCTIONS ////////////////////////////
/*****************************************************************************/



/******************************************************************************
 *
 *	Adds a given pointer to the Free Map
 *
 *****************************************************************************/
void ObjectAllocator::AddToMap(char* ptr) {
	uintptr_t u_ptr = reinterpret_cast<uintptr_t>(ptr);
	uintptr_t idx = u_ptr & 0xFFFF;
	try {
		FLMAP[idx] = std::make_unique<LList>(u_ptr, std::move(FLMAP[idx]));
	}
	catch (std::bad_alloc&) {
		throw OAException(OAException::E_NO_MEMORY, "out of physical memory (operator new fails)");
	}
}



/******************************************************************************
 *
 *	Removes a given pointer from the Free Map
 *
 *****************************************************************************/
void ObjectAllocator::RemoveFromMap(char* ptr) {
	uintptr_t u_ptr = reinterpret_cast<uintptr_t>(ptr);
	uintptr_t idx = u_ptr & 0xFFFF;

	std::unique_ptr<LList>* prevptr = nullptr;
	std::unique_ptr<LList>* currptr = &FLMAP[idx];

	while (currptr) {

		if ((*currptr)->ptr == u_ptr) {
			std::unique_ptr<LList> temp = std::move((*currptr)->next);
			// remove this node.
			if (prevptr == nullptr) {
				*currptr = std::move(temp);
			}
			else {
				(*prevptr)->next = std::move(temp);
			}
			return;

		}

		prevptr = currptr;
		currptr = &((*currptr)->next);

	}
}



/******************************************************************************
 *
 *	Checks whether a given pointer is in the Free Map
 *
 *****************************************************************************/
bool ObjectAllocator::FindInMap(char* ptr) const {

	uintptr_t u_ptr = reinterpret_cast<uintptr_t>(ptr);
	uintptr_t idx = u_ptr & 0xFFFF;

	const LList* currptr = FLMAP[idx].get();
	while (currptr) {

		if (currptr->ptr == u_ptr) {

			return true;

		}

		currptr = currptr->next.get();

	}

	return false;

}



/******************************************************************************
 *
 *	Writes in the patterns for new blocks
 *
 *****************************************************************************/
void ObjectAllocator::WriteNewBlockPattern(char* ptr, unsigned char pattern) {

	memset(ptr - m_config.PadBytes_, PAD_PATTERN, m_config.PadBytes_);

	memset(ptr, pattern, m_dataSize);

	memset(ptr + m_dataSize, PAD_PATTERN, m_config.PadBytes_);

}



/******************************************************************************
 *
 *	Removes the given pointer from the FreeList_
 *
 *****************************************************************************/
void ObjectAllocator::RemoveFromFreeList(char* ptr) {
	GenericObject* currentObject = FreeList_;
	GenericObject* previousObject = nullptr;
	while (currentObject) {
		if (ptr == reinterpret_cast<char*>(currentObject)) { // found in freelist 
			if (previousObject) {
				previousObject->Next = currentObject->Next;
			}
			else {
				FreeList_ = currentObject->Next;
			}
			break;
		}
		previousObject = currentObject;
		currentObject = currentObject->Next;
	}
}



/******************************************************************************
 *
 *	Adds the given pointer to the FreeList_
 *
 *****************************************************************************/
void ObjectAllocator::AddToFreeList(char* ptr) {
	GenericObject* temp = FreeList_;
	FreeList_ = reinterpret_cast<GenericObject*>(ptr);
	FreeList_->Next = temp;

	AddToMap(ptr);

}



/******************************************************************************
 *
 *	Writes in data pattern for unallocated, allocated, freed
 *
 *****************************************************************************/
void ObjectAllocator::ResetBlockPattern(char* ptr, unsigned char pattern) {
	memset(ptr, pattern, m_dataSize);
}



/******************************************************************************
 *
 *	Creates a new page and adds the new objects to the FreeList_
 *
 *****************************************************************************/
void ObjectAllocator::CreateNewPage() {
	try {
		if (m_stats.PagesInUse_ == m_config.MaxPages_ && m_config.MaxPages_ != 0) {
			throw OAException(OAException::E_NO_PAGES, "out of logical memory (max pages has been reached)");
		}
		GenericObject* tempP = PageList_;
		PageList_ = reinterpret_cast<GenericObject*>(new char[m_stats.PageSize_]);
		memset(reinterpret_cast<char*>(PageList_), 0, m_stats.PageSize_);
		PageList_->Next = tempP;
	}
	catch (...) {
		throw;
	}

	++m_stats.PagesInUse_;

	GenericObject* tempF = FreeList_;
	FreeList_ = reinterpret_cast<GenericObject*>(reinterpret_cast<char*>(PageList_) + m_pageHeaderSize + m_HPSize);

	AddToMap(reinterpret_cast<char*>(FreeList_));

	memset(reinterpret_cast<char*>(PageList_) + sizeof(void*), ALIGN_PATTERN, m_config.LeftAlignSize_);


	if (m_config.DebugOn_) {
		WriteNewBlockPattern(reinterpret_cast<char*>(FreeList_), ObjectAllocator::UNALLOCATED_PATTERN);
		memset(reinterpret_cast<char*>(FreeList_) + m_dataSize + m_config.PadBytes_, ALIGN_PATTERN, m_config.InterAlignSize_);
	}

	FreeList_->Next = tempF;

	for (unsigned i = 0; i < (m_config.ObjectsPerPage_ - 1); ++i) {

		tempF = FreeList_;
		FreeList_ = reinterpret_cast<GenericObject*>(reinterpret_cast<char*>(FreeList_) + m_blockSizeWithAlignment);

		AddToMap(reinterpret_cast<char*>(FreeList_));

		if (m_config.DebugOn_) {
			WriteNewBlockPattern(reinterpret_cast<char*>(FreeList_), ObjectAllocator::UNALLOCATED_PATTERN);
			if (i < (m_config.ObjectsPerPage_ - 2)) {
				memset(reinterpret_cast<char*>(FreeList_) + m_dataSize + m_config.PadBytes_, ALIGN_PATTERN, m_config.InterAlignSize_);
			}
		}
		FreeList_->Next = tempF;
	}

	m_stats.FreeObjects_ += m_config.ObjectsPerPage_;

}


/******************************************************************************
 *
 *	Checks whether the given pointer is valid
 *	If pointer is not valid, returns true, else returns false
 *
 *****************************************************************************/
bool ObjectAllocator::CheckBadBoundary(char* ptr) const {
	// 1. check if ptr is in page
	// 2. check if ptr is aligned
	// 3. if bad, return true, else return false
	GenericObject* currentPage = PageList_;
	while (currentPage) {
		if ((ptr >= (reinterpret_cast<char*>(currentPage))) && (ptr < (reinterpret_cast<char*>(currentPage) + m_stats.PageSize_))) {
			// is now in page, check for alignment.
			if ((ptr - (reinterpret_cast<char*>(currentPage) + m_pageHeaderSize + m_HPSize)) % m_blockSizeWithAlignment == 0) {
				return false;
			}
			return true;
		}
		currentPage = (currentPage)->Next;
	}
	return true;
}


/******************************************************************************
 *
 *	Checks whether Padding bytes have been overwritten
 *	If padding is corrupted, returns true, else returns false
 *
 *****************************************************************************/
bool ObjectAllocator::ValidatePadding(char* ptr) const {
	// check first padding
	for (unsigned i = 0; i < m_config.PadBytes_; ++i) {
		if (static_cast<unsigned char>(*(ptr - m_HPSize + m_config.HBlockInfo_.size_ + i)) != ObjectAllocator::PAD_PATTERN) {
			return true;
		}
	}

	// check second padding
	for (unsigned i = 0; i < m_config.PadBytes_; ++i) {
		if (static_cast<unsigned char>(*(ptr - m_HPSize + m_config.HBlockInfo_.size_ + m_config.PadBytes_ + m_stats.ObjectSize_ + i)) != ObjectAllocator::PAD_PATTERN) {
			return true;
		}
	}
	return false;
}

/*****************************************************************************/
////////////////////////// END OF HELPER FUNCTIONS ////////////////////////////
/*****************************************************************************/



/******************************************************************************
 *
 *	Creates the ObjectManager per the specified values
 *	Throws an exception if the construction fails. (Memory allocation problem)
 *
 *****************************************************************************/
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config) :
	m_config{ config },
	m_blockSize{ config.HBlockInfo_.size_ + config.PadBytes_ + ObjectSize + config.PadBytes_ },
	m_dataSize{ ObjectSize },
	m_HPSize{ config.HBlockInfo_.size_ + config.PadBytes_ },
	m_extCounterAlloc{ 0 },
	m_extCounterFree{ 0 }
{

	if (m_config.Alignment_) {

		size_t pageFrontSize = sizeof(void*) + m_HPSize;
		size_t pageFrontRemainder = pageFrontSize % m_config.Alignment_;
		pageFrontRemainder = (!pageFrontRemainder) ? m_config.Alignment_ : pageFrontRemainder;
		m_config.LeftAlignSize_ = static_cast<unsigned int>(m_config.Alignment_ - pageFrontRemainder);

		size_t blockSizeRemainder = m_blockSize % m_config.Alignment_;
		blockSizeRemainder = (!blockSizeRemainder) ? m_config.Alignment_ : blockSizeRemainder;
		m_config.InterAlignSize_ = static_cast<unsigned int>(m_config.Alignment_ - blockSizeRemainder);

	}

	m_pageHeaderSize = sizeof(void*) + m_config.LeftAlignSize_;
	m_blockSizeWithAlignment = m_blockSize + m_config.InterAlignSize_;

	PageList_ = nullptr;
	FreeList_ = nullptr;
	m_stats.ObjectSize_ = ObjectSize;

	m_stats.PageSize_ = sizeof(void*) + m_config.LeftAlignSize_ + m_config.ObjectsPerPage_ * m_blockSizeWithAlignment - m_config.InterAlignSize_;

	m_stats.FreeObjects_ = 0;
	m_stats.ObjectsInUse_ = 0;
	m_stats.PagesInUse_ = 0;
	m_stats.MostObjects_ = 0;
	m_stats.Allocations_ = 0;
	m_stats.Deallocations_ = 0;

	if (!m_config.UseCPPMemManager_) {
		try {
			CreateNewPage();
		}
		catch (std::bad_alloc&) {
			throw OAException(OAException::E_NO_MEMORY, "out of physical memory (operator new fails)");
		}
		catch (const OAException& e) {
			if (e.code() == OAException::E_NO_PAGES) {
				throw OAException(OAException::E_NO_PAGES, "out of logical memory (max pages has been reached)");
			}
		}
	}
}



/******************************************************************************
 *
 *	Destroys the ObjectManager (never throws)
 *
 *****************************************************************************/
ObjectAllocator::~ObjectAllocator() {

	while (PageList_) {
		GenericObject* nextToDelete = PageList_->Next;
		delete[] reinterpret_cast<char*>(PageList_);
		PageList_ = nextToDelete;
	}

}



/******************************************************************************
 *
 *	Take an object from the free list and give it to the client (simulates new)
 *	Throws an exception if the object can't be allocated. (Memory allocation problem)
 *
 *****************************************************************************/
void* ObjectAllocator::Allocate(const char* label) {

	GenericObject* temp = nullptr;

	if (m_config.UseCPPMemManager_) {
		temp = reinterpret_cast<GenericObject*>(new char[m_stats.ObjectSize_]);
		++m_stats.Allocations_;
	}
	else {
		// if freelist is empty, create new page
		if (!FreeList_) {
			try {
				CreateNewPage();
			}
			catch (std::bad_alloc&) {
				throw OAException(OAException::E_NO_MEMORY, "out of physical memory (operator new fails)");
			}
			catch (const OAException& e) {
				if (e.code() == OAException::E_NO_PAGES) {
					throw OAException(OAException::E_NO_PAGES, "out of logical memory (max pages has been reached)");
				}
			}
		}

		temp = FreeList_; // stores the new allocation's pointer
		FreeList_ = FreeList_->Next;

		RemoveFromMap(reinterpret_cast<char*>(temp));

		switch (m_config.HBlockInfo_.type_) {

		case OAConfig::hbNone:
			++m_stats.Allocations_;
			break;

		case OAConfig::hbBasic:
			*(reinterpret_cast<unsigned*>(reinterpret_cast<char*>(temp) - m_HPSize)) = ++m_stats.Allocations_;
			*(reinterpret_cast<char*>(temp) - m_HPSize + sizeof(unsigned)) |= 1;
			break;

		case OAConfig::hbExtended:
			++(*(reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(temp) - m_HPSize + m_config.HBlockInfo_.additional_)));
			*(reinterpret_cast<unsigned*>(reinterpret_cast<char*>(temp) - m_HPSize + m_config.HBlockInfo_.additional_ + sizeof(unsigned short))) = ++m_stats.Allocations_;
			*(reinterpret_cast<char*>(temp) - m_HPSize + m_config.HBlockInfo_.additional_ + sizeof(unsigned short) + sizeof(unsigned)) |= 1;
			break;

		case OAConfig::hbExternal:
			MemBlockInfo** ptrToExternal = reinterpret_cast<MemBlockInfo**>(reinterpret_cast<char*>(temp) - m_HPSize);
			try {
				MemBlockInfo* info = new MemBlockInfo{ true, new char[label ? strlen(label) + 1 : 1], ++m_stats.Allocations_ };
				if (label) {
					strcpy_s((info)->label, sizeof(label), label);
				}
				*ptrToExternal = info;
			}
			catch (std::bad_alloc&) {
				throw OAException(OAException::E_NO_MEMORY, "out of physical memory (operator new fails)");
			}
			break;

		}

		if (m_config.DebugOn_) {
			ResetBlockPattern(reinterpret_cast<char*>(temp), ObjectAllocator::ALLOCATED_PATTERN);
		}

	}

	--m_stats.FreeObjects_;
	++m_stats.ObjectsInUse_;
	m_stats.MostObjects_ = (m_stats.ObjectsInUse_ > m_stats.MostObjects_) ? m_stats.ObjectsInUse_ : m_stats.MostObjects_;

	return temp;

}



/******************************************************************************
 *
 *	Returns an object to the free list for the client (simulates delete)
 *	Throws an exception if the the object can't be freed. (Invalid object)
 *
 *****************************************************************************/
void ObjectAllocator::Free(void* Object) {

	if (m_config.UseCPPMemManager_) {
		delete[] reinterpret_cast<char*>(Object);
	}
	else {
		// 1. check for double free
		if (FindInMap(reinterpret_cast<char*>(Object))) {
			throw OAException(OAException::E_MULTIPLE_FREE, "FreeObject: Object has already been freed.");
		}

		// 2. check for bad boundary
		if (CheckBadBoundary(reinterpret_cast<char*>(Object))) {
			throw OAException(OAException::E_BAD_BOUNDARY, "validate_object: Object not on a boundary.");
		}

		if (m_config.PadBytes_) {
			// 3. validate padding
			if (ValidatePadding(reinterpret_cast<char*>(Object))) {
				throw OAException(OAException::E_CORRUPTED_BLOCK, "validate_padding: Pad bytes have been overwritten.");
			}
		}


		if (m_config.DebugOn_) {
			switch (m_config.HBlockInfo_.type_) {

			case OAConfig::hbNone:
				break;

			case OAConfig::hbBasic:
				for (unsigned i = 0; i < sizeof(unsigned); ++i) {
					*(reinterpret_cast<char*>(Object) - m_HPSize + i) &= 0;
				}
				*(reinterpret_cast<char*>(Object) - m_HPSize + sizeof(unsigned)) &= 0;
				break;

			case OAConfig::hbExtended:
				for (unsigned i = 0; i < sizeof(unsigned); ++i) {
					*(reinterpret_cast<char*>(Object) - m_HPSize + m_config.HBlockInfo_.additional_ + sizeof(unsigned short) + i) &= 0;
				}
				*(reinterpret_cast<char*>(Object) - m_HPSize + m_config.HBlockInfo_.additional_ + sizeof(unsigned short) + sizeof(unsigned)) &= 0;
				break;

			case OAConfig::hbExternal:
				MemBlockInfo** ptrToExternal = reinterpret_cast<MemBlockInfo**>(reinterpret_cast<char*>(Object) - m_HPSize);
				delete[](*ptrToExternal)->label;
				delete* ptrToExternal;
				memset(reinterpret_cast<char*>(Object) - m_HPSize, 0, sizeof(void*));
				break;

			}

			// 4. reset block pattern
			ResetBlockPattern(reinterpret_cast<char*>(Object), ObjectAllocator::FREED_PATTERN);
		}

		AddToFreeList(reinterpret_cast<char*>(Object));

	}

	--m_stats.ObjectsInUse_;
	++m_stats.FreeObjects_;
	++m_stats.Deallocations_;

}



/******************************************************************************
 *
 *	Calls the callback fn for each block still in use
 *
 *****************************************************************************/
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const {

	GenericObject* currentPage = PageList_;
	char* ptr = nullptr;
	unsigned numInUse = 0;

	while (currentPage) {
		ptr = reinterpret_cast<char*>(currentPage) + m_pageHeaderSize + m_HPSize;
		for (unsigned i = 0; i < m_config.ObjectsPerPage_; ++i) {

			if (!FindInMap(ptr)) {
				fn(ptr, m_stats.ObjectSize_);
				++numInUse;
			}

			ptr += m_blockSize + m_config.InterAlignSize_;
		}
		currentPage = currentPage->Next;
	}

	return numInUse;

}



/******************************************************************************
 *
 *	Calls the callback fn for each block that is potentially corrupted
 *
 *****************************************************************************/
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const {

	// check padding on freelist only
	GenericObject* currentPage = PageList_;
	char* ptr = nullptr;
	unsigned corruptedCount = 0;

	while (currentPage) {
		ptr = reinterpret_cast<char*>(currentPage) + m_pageHeaderSize + m_HPSize;
		for (unsigned i = 0; i < m_config.ObjectsPerPage_; ++i) {
			if (ValidatePadding(ptr)) {
				// corrupted
				fn(reinterpret_cast<GenericObject*>(ptr), m_stats.ObjectSize_);
				++corruptedCount;
			}

			ptr += m_blockSize + m_config.InterAlignSize_;
		}
		currentPage = currentPage->Next;
	}

	return corruptedCount;

}


/******************************************************************************
 *
 *	Frees all empty pages
 *
 *****************************************************************************/
unsigned ObjectAllocator::FreeEmptyPages() {

	unsigned pagesFreedCount = 0;
	GenericObject* currentPage = PageList_;
	GenericObject* prevPage = PageList_;
	char* ptr = nullptr;
	bool firstPage = true;

	bool thisPageIsEmpty = true;

	while (firstPage && currentPage) {

		// check for empty page
		thisPageIsEmpty = true;

		ptr = reinterpret_cast<char*>(currentPage) + m_pageHeaderSize + m_HPSize; // go to first block

		for (unsigned int i = 0; i < m_config.ObjectsPerPage_; ++i) {

			// find in free list
			if (!FindInMap(ptr + (i * m_blockSizeWithAlignment))) { // if not found in free list, means page empty
				thisPageIsEmpty = false; // as long as 1 is found, page is not empty
				firstPage = false;
				break;
			}

		}

		if (thisPageIsEmpty) {

			// remove Objects on the Page from free list
			ptr = reinterpret_cast<char*>(currentPage) + m_pageHeaderSize + m_HPSize;
			for (unsigned i = 0; i < m_config.ObjectsPerPage_; ++i) {
				RemoveFromFreeList(ptr + (i * m_blockSizeWithAlignment));
				RemoveFromMap(ptr + (i * m_blockSizeWithAlignment));
				--m_stats.FreeObjects_;
			}

			// remove from PageList_
			PageList_ = currentPage->Next;
			delete[] reinterpret_cast<char*>(currentPage);
			--m_stats.PagesInUse_;
			++pagesFreedCount;
			currentPage = PageList_;

		}

	}

	prevPage = currentPage;
	if (currentPage) {
		currentPage = currentPage->Next;
	}

	while (currentPage) {

		// check for empty page
		thisPageIsEmpty = true;

		ptr = reinterpret_cast<char*>(currentPage) + m_pageHeaderSize + m_HPSize; // go to first block

		for (unsigned i = 0; i < m_config.ObjectsPerPage_; ++i) {

			// find in freelist
			if (!FindInMap(ptr + (i * m_blockSizeWithAlignment))) {
				thisPageIsEmpty = false;
				break;
			}

		}

		if (thisPageIsEmpty) {

			// remove Objects on the Page from free list
			ptr = reinterpret_cast<char*>(currentPage) + m_pageHeaderSize + m_HPSize;
			for (unsigned i = 0; i < m_config.ObjectsPerPage_; ++i) {
				RemoveFromFreeList(ptr + (i * m_blockSizeWithAlignment));
				RemoveFromMap(ptr + (i * m_blockSizeWithAlignment));
				--m_stats.FreeObjects_;
			}

			// remove from PageList_
			prevPage->Next = currentPage->Next;
			delete[] reinterpret_cast<char*>(currentPage);
			--m_stats.PagesInUse_;
			++pagesFreedCount;

			currentPage = prevPage->Next;
		}
		else {
			currentPage = currentPage->Next;
		}

	}

	return pagesFreedCount;

}


/*****************************************************************************/
///////////////////////////// GETTERS & SETTERS ///////////////////////////////
//////////////////// Testing/Debugging/Statistic methods //////////////////////
/*****************************************************************************/

void ObjectAllocator::SetDebugState(bool State) {   // true=enable, false=disable
	m_config.DebugOn_ = State;
}


const void* ObjectAllocator::GetFreeList() const {  // returns a pointer to the internal free list
	return FreeList_;
}


const void* ObjectAllocator::GetPageList() const {  // returns a pointer to the internal page list
	return PageList_;
}


OAConfig ObjectAllocator::GetConfig() const {       // returns the configuration parameters
	return m_config;
}


OAStats ObjectAllocator::GetStats() const {         // returns the statistics for the allocator
	return m_stats;
}
