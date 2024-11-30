#include "MemoryAllocator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <algorithm> 

using namespace std;

MemoryAllocator::MemoryAllocator(ConfigManager* configManager) {
	maxOverallMem = configManager->getMaxOverallMem();
	memPerFrame = configManager->getMemPerFrame();
	minMemPerProcess = configManager->getMinMemPerProcess();
	maxMemPerProcess = configManager->getMaxMemPerProcess();

	if (isFlatAllocation()) {
		for (int i = 0; i < maxOverallMem; ++i) {
			memoryAddresses.push_back(i);
		}
	}
}

MemoryAllocator::~MemoryAllocator() {
}

bool MemoryAllocator::isFlatAllocation() const {
	return memPerFrame == maxOverallMem;
}

bool MemoryAllocator::isProcessInMemory(Process* process) const {
	return processMap.find(process->getProcessName()) != processMap.end();
}

bool MemoryAllocator::allocateMemory(Process* process) {
	if (isFlatAllocation()) {
		return allocateFlatMemory(process);
	}
	else {
		return allocatePagingMemory(process);
	}
	return false;
}

void MemoryAllocator::deallocateMemory(Process* process) {
	if (isFlatAllocation()) {
		removeFlatMemory(process);
	}
	else {
		removePagingMemory(process);
	}
}


bool MemoryAllocator::allocateFlatMemory(Process* process) {
	std::string processName = process->getProcessName();

	int memRequired = process->getMemorySize();

	if (maxOverallMem - allocatedAddresses.size() < memRequired) {
		// Not enough memory, find the oldest process
		auto oldestProcess = std::min_element(processMap.begin(), processMap.end(),
			[](const auto& a, const auto& b) {
				return a.second.allocationTime < b.second.allocationTime;
			});

		if (oldestProcess != processMap.end()) {
			saveProcessToBackingStore(oldestProcess->second.process);
			removeFlatMemory(oldestProcess->second.process);
		}
		else {
			// No processes to remove, allocation fails
			return false;
		}
	}

	int consecutiveFreeAddresses = 0;
	int startIndex = -1;

	// Find consecutive free addresses
	for (int i = 0; i < maxOverallMem; ++i) {
		if (find(allocatedAddresses.begin(), allocatedAddresses.end(), memoryAddresses[i]) == allocatedAddresses.end()) {
			// Address is free
			++consecutiveFreeAddresses;
			if (startIndex == -1) {
				startIndex = i;
			}
		}
		else {
			consecutiveFreeAddresses = 0;
			startIndex = -1;
		}

		if (consecutiveFreeAddresses >= memRequired) {
			for (int j = startIndex; j < startIndex + memRequired; ++j) {
				allocatedAddresses.push_back(memoryAddresses[j]);
			}

			int endIndex = startIndex + memRequired - 1;
			processMap[processName] = { process, startIndex, endIndex, time(0) };
			return true;
		}
	}
	return false;
}


void MemoryAllocator::removeFlatMemory(Process* process) {
	std::string processName = process->getProcessName();
	auto it = processMap.find(processName);
	if (it != processMap.end()) {
		int startIndex = it->second.startAddressIndex;
		int endIndex = it->second.endAddressIndex;
		for (int i = startIndex; i <= endIndex; ++i) {
			// Remove allocated addresses from the allocatedAddresses vector
			allocatedAddresses.erase(remove(allocatedAddresses.begin(), allocatedAddresses.end(), memoryAddresses[i]), allocatedAddresses.end());
		}
		processMap.erase(it);
	}
}


void MemoryAllocator::showFlatMemory() {
	cout << "Flat memory allocation:" << endl;
	cout << "-----------------------" << endl;
	cout << "Max overall memory: " << maxOverallMem << endl;
	cout << "Memory usage: " << allocatedAddresses.size() << " KB / " << maxOverallMem << " KB" << endl;
	// Display process names and corresponding memory size
	for (auto& entry : processMap) {
		cout << "Process: " << entry.first << " (Memory size: " << entry.second.endAddressIndex - entry.second.startAddressIndex + 1 << ")" << endl;
	}
	cout << endl;
}

bool MemoryAllocator::allocatePagingMemory(Process* process) {
	std::string processName = process->getProcessName();
	int pagesRequired = (process->getMemorySize() + memPerFrame - 1) / memPerFrame;

	// Check if there are enough free frames
	if (frameList.size() + pagesRequired > maxOverallMem / memPerFrame) {
		// Not enough frames, find the oldest process
		auto oldestProcessIt = std::min_element(processPageMap.begin(), processPageMap.end(),
			[](const auto& a, const auto& b) {
				return a.second.first < b.second.first; // Compare allocation times of processes
			});

		if (oldestProcessIt != processPageMap.end()) {
			std::string oldestProcessName = oldestProcessIt->first;

			// Remove pages of the oldest process from frameList
			frameList.erase(std::remove_if(frameList.begin(), frameList.end(),
				[&](const Page& page) { return page.process->getProcessName() == oldestProcessName; }), frameList.end());

			// get the process object from a page in the frameList with a process name that matches the oldest process name
			auto oldestProcessPage = std::find_if(
				frameList.begin(), frameList.end(),
				[&](const Page& page) { return page.process->getProcessName() == oldestProcessName; }
			);
			
			saveProcessToBackingStore(oldestProcessPage->process);

			// Remove the oldest process from processPageMap
			processPageMap.erase(oldestProcessIt);

		}
		else {
			// No processes to remove, allocation fails
			return false;
		}
	}

	// Allocate pages to the process
	for (int i = 0; i < pagesRequired; ++i) {
		Page newPage = { process, i };
		frameList.push_back(newPage);
		processPageMap[processName].second.push_back(i); // Add page number to the vector
	}

	// Set allocation time for the process (only when it's first allocated)
	if (processPageMap[processName].first == 0) {
		processPageMap[processName].first = time(0);
	}

	return true;
}

void MemoryAllocator::removePagingMemory(Process* process) {
	std::string processName = process->getProcessName();
	auto it = processPageMap.find(processName);
	if (it != processPageMap.end()) {
		// No need to iterate over page numbers, just remove all pages of the process
		frameList.erase(std::remove_if(frameList.begin(), frameList.end(),
			[&](const Page& page) {
				return page.process->getProcessName() == processName;
			}), frameList.end());

		processPageMap.erase(it); // Remove the process entry from the map
	}
}

void MemoryAllocator::showPagingMemory() {
	cout << "Paging memory allocation:" << endl;
	cout << "-------------------------" << endl;
	cout << "Total frames: " << maxOverallMem / memPerFrame << endl;
	cout << "Used frames: " << frameList.size() << endl;
	cout << "Free frames: " << maxOverallMem / memPerFrame - frameList.size() << endl;

	// Display processes and their allocated pages
	for (const auto& entry : processPageMap) {
		cout << "Process: " << entry.first << "Pages: ";
		for (int pageNumber : entry.second.second) { // Access the page numbers from the pair
			cout << pageNumber << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void MemoryAllocator::saveProcessToBackingStore(Process* process) {
	std::string processName = process->getProcessName();
	std::string filePath = backingStorePath + "/" + processName + ".txt";

	std::filesystem::create_directories(backingStorePath);

	std::ofstream file(filePath);

	if (file.is_open()) {
		file << process->getProcessName() << std::endl;
		file << std::hex << reinterpret_cast<uintptr_t>(process) << std::dec <<std::endl;
		file << "Executed Instructions: " << std::endl;
		file << process->getCommandIndex() << std::endl;
		file << "Total Instructions: " << std::endl;
		file << process->getTotalCommands() << std::endl;
		file << "Memory Size: " << std::endl;
		file << process->getMemorySize() << std::endl;
		file << "Number of Pages: " << std::endl;
		file << process->getNumPages() << std::endl;

		file.close();

		numPagesOut += process->getNumPages();
		backingStoreSet.insert(processName);
	}
	else {
		std::cerr << "Error: Unable to open file for saving process to backing store." << std::endl;
	}
}

Process* MemoryAllocator::loadProcessFromBackingStore(const std::string& processName) {
	std::string filePath = backingStorePath + "/" + processName + ".txt";

	std::ifstream file(filePath);
	if (file.is_open()) {
		uintptr_t processAddress;
		std::string name;
		int commandIndex, totalCommands, memorySize, numPages;

		file >> name;
		file >> std::hex >> processAddress;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		file >> commandIndex;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		file >> totalCommands;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		file >> memorySize;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		file >> numPages;

		file.close();


		Process* loadedProcess = reinterpret_cast<Process*>(processAddress);

		numPagesIn += loadedProcess->getNumPages();

		backingStoreSet.erase(processName);

		return loadedProcess;
	}
	else {
		std::cerr << "Error: Unable to open file for loading process from backing store." << std::endl;
		return nullptr;
	}
}

void MemoryAllocator::showProcessSMI(double cpuUtil) {
	int usedMemory, totalMemory;

	if (isFlatAllocation()) {
		usedMemory = allocatedAddresses.size();
		totalMemory = maxOverallMem;
	}
	else { // Paging allocation
		int usedFrames = frameList.size();
		int totalFrames = maxOverallMem / memPerFrame;
		usedMemory = usedFrames * memPerFrame;
		totalMemory = totalFrames * memPerFrame;
	}

	double memoryUtil = (static_cast<double>(usedMemory) / totalMemory) * 100;


	cout << "-------------------------------------------------" << endl;
	cout << "|  PROCESS-SMI V01.00 Driver Version: 01.00     |" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "CPU-Util: " << cpuUtil << "%" << endl;
	cout << "Memory Usage: " << usedMemory << " KB / " << totalMemory << " KB" << endl;
	cout << "Memory Util: " << memoryUtil << "%" << endl << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "|  Running Processes and Memory Usage:          |" << endl;
	cout << "-------------------------------------------------" << endl;

	// Display process names and corresponding memory size
	if (isFlatAllocation()) {
		for (const auto& entry : processMap) {
			cout << setw(12) << entry.first << "  " << entry.second.endAddressIndex - entry.second.startAddressIndex + 1 << "KB" << endl;
		}
	}
	else { // Paging allocation
		for (const auto& entry : processPageMap) {
			cout << setw(12) << entry.first << "  ";
			cout << entry.second.second.size() * memPerFrame << " KB" << endl;
		}
	}
	
	cout << endl << endl;
}

void MemoryAllocator::showVmStat(int idleCpuTicks, int activeCpuTicks) {
	cout << "-------------------------------------------------" << endl;
	cout << " VMSTAT" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << setw(9) << maxOverallMem << "  Total Memory" << endl;
	if (isFlatAllocation()) {
		cout << setw(9) << allocatedAddresses.size() << "  Used Memory" << endl;
		cout << setw(9) << maxOverallMem - allocatedAddresses.size() << "  Free Memory" << endl;
	}
	else { // Paging allocation
		int usedFrames = frameList.size();
		int totalFrames = maxOverallMem / memPerFrame;
		cout << setw(9) << usedFrames * memPerFrame << "  Used Memory" << endl;
		cout << setw(9) << (totalFrames - usedFrames) * memPerFrame << "  Free Memory" << endl;
	}
	cout << setw(9) << idleCpuTicks << "  Idle CPU ticks" << endl;
	cout << setw(9) << activeCpuTicks << "  Active CPU ticks" << endl;
	cout << setw(9) << idleCpuTicks + activeCpuTicks  << "  Total CPU ticks" << endl;
	cout << setw(9) << numPagesOut << "  Pages paged out" << endl;
	cout << setw(9) << numPagesIn << "  Pages paged in" << endl << endl;
	cout << "-------------------------------------------------" << endl<< endl;
}