#pragma once

#include "ConfigManager.h"
#include <vector>
#include <unordered_map>
#include "Process.h"

class MemoryAllocator
{
	public:
		MemoryAllocator(ConfigManager* configManager);
		~MemoryAllocator();
			
		void showMemoryUsage();
		void logMemoryUsage(int core, int qq);
		bool addProcessMemory(Process* process);
		void removeProcessMemory(Process* process);


	private:
		int maxOverallMem; // max overall memory
		int memPerFrame; // memory per frame
		int memPerProcess; // memory per process

		struct ProcessMemory {
			string processName;
			int lowerLimit;
			int upperLimit;
		};
		struct MemorySegment {
			int lowerLimit;
			int upperLimit;
		};

		int totalSlots;
		std::vector<bool> memorySlots;
		std::unordered_map<std::string, int> processMap;

		int calculateExternalFrag() const;
		
};


