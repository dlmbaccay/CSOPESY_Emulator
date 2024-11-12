#pragma once

#include "ConfigManager.h"
#include <vector>
#include <unordered_map>
#include "Process.h"
#include <mutex>

class MemoryAllocator
{
	public:
		MemoryAllocator(ConfigManager* configManager);
		~MemoryAllocator();
			
		void showMemoryUsage();
		void logMemoryUsage();
		bool addProcessMemory(Process* process);
		void removeProcessMemory(Process* process);

		int getQuantumCycles() const { return quantumCycles; } // Homework 8
		void setQuantumCycles(int cycles) { quantumCycles = cycles; } // Homework 8


	private:
		int maxOverallMem; // max overall memory
		int memPerFrame; // memory per frame
		int memPerProcess; // memory per process

		int quantumCycles; // Homework 8
		

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

		std::mutex mapMutex;
		std::mutex logMutex;

		int calculateExternalFrag() const;
		
};


