#include "MemoryAllocator.h"
#include <iostream>
#include <fstream>
#include <iomanip>


MemoryAllocator::MemoryAllocator(ConfigManager* configManager)
{
	memPerFrame = configManager->getMemPerFrame();
	memPerProcess = configManager->getMemPerProcess();
	maxOverallMem = configManager->getMaxOverallMem();
	totalSlots = maxOverallMem / memPerProcess;
  memorySlots.resize(totalSlots, false);
}

MemoryAllocator::~MemoryAllocator()
{
}

void MemoryAllocator::showMemoryUsage()
{

	// get the current time in format MM/DD/YYYY HH:MM:SSp
	time_t now = time(0);
	tm localtm;
	localtime_s(&localtm, &now);  // Use thread-safe localtime_s
	char timestamp[100];
	strftime(timestamp, sizeof(timestamp), "(%m/%d/%Y %I:%M:%S%p)", &localtm);

	std::cout << "Timestamp: " << timestamp << std::endl;
	std::cout << "Number of processes in memory: " << processMap.size() << std::endl;
	std::cout << "Total external fragmentation in KB: " << calculateExternalFrag() << std::endl << std::endl;
	std::cout << "----end---- = " << maxOverallMem << std::endl << std::endl;

	// print process memories from the processMap with their upper limit and lower limit based on which slot and memory per process
	for (auto& pm : processMap) {
		std::cout << (pm.second + 1) * memPerProcess << std::endl;
		std::cout << pm.first << std::endl;
		std::cout << pm.second * memPerProcess << std::endl;
		std::cout << std::endl;
	}

	std::cout << std::endl << "----start---- = " << 0 << std::endl;
}

bool MemoryAllocator::addProcessMemory(Process* process)
{
  string processName = process->getProcessName();
  // Check if process is already in memory
  if (processMap.find(processName) != processMap.end()) {
    return false;
  }
  
  for (int i = 0; i < totalSlots; ++i) {
    if (!memorySlots[i]) {  // Slot is free
      memorySlots[i] = true;  // Mark slot as occupied
      processMap[processName] = i;  // Track the slot assigned to this process
      return true;
    }
  }

  return false;
}



int MemoryAllocator::calculateExternalFrag() const
{
	// Calculate external fragmentation by available free slots and memory per process
	int freeSlots = 0;
	for (int i = 0; i < totalSlots; ++i) {
		if (!memorySlots[i]) {
			++freeSlots;
		}
	}
	return freeSlots * memPerProcess;
}


void MemoryAllocator::removeProcessMemory(Process* process)
{
	string processName = process->getProcessName();
	auto it = processMap.find(processName);
	if (it != processMap.end()) {
		int slotIndex = it->second;
		memorySlots[slotIndex] = false;  // Mark slot as free
		processMap.erase(it);  // Remove process from map
	}
	else {
		std::cout << "Process " << processName << " not found in memory\n";
	}
}

// print out to text file in file name memory_stamp_<qq> where qq is current quantum cycle
void MemoryAllocator::logMemoryUsage(int qq) {

  // Create the filename with the current quantum cycle
	std::string filename = "memory_stamps/memory_stamp_" + std::to_string(qq) + ".txt";

  // Open the file for writing
  std::ofstream outputFile(filename);
  if (!outputFile.is_open()) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return;
  }

  // Get the current time in format MM/DD/YYYY HH:MM:SSp
  time_t now = time(0);
  tm localtm;
  localtime_s(&localtm, &now);  // Use thread-safe localtime_s
  char timestamp[100];
  strftime(timestamp, sizeof(timestamp), "(%m/%d/%Y %I:%M:%S%p)", &localtm);

  // Write the output to the file
  outputFile << "Timestamp: " << timestamp << std::endl;
  outputFile << "Number of processes in memory: " << processMap.size() << std::endl;
  outputFile << "Total external fragmentation in KB: " << calculateExternalFrag() << std::endl << std::endl;
  outputFile << "----end---- = " << maxOverallMem << std::endl << std::endl;

  // Print process memories from the processMap 
  for (auto& pm : processMap) {
    outputFile << (pm.second + 1) * memPerProcess << std::endl;
    outputFile << pm.first << std::endl;
    outputFile << pm.second * memPerProcess << std::endl;
    outputFile << std::endl;
  }

  outputFile << std::endl << "----start---- = " << 0 << std::endl;

  // Close the file
  outputFile.close();
}