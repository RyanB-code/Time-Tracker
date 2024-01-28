#include "Display.h"

namespace Display {

	DisplayManager& DisplayManager::getInstance() {
		static DisplayManager display;

		return display;
	}
	DisplayManager::DisplayManager() {
		m_currentTime.stamp();
	}
	DisplayManager::~DisplayManager() {

	}

	void DisplayManager::home() {
		std::stringstream ss;
		ss << std::format("{:%A, %d %b}", m_currentTime.getRawTime());

		uint64_t lengthToPad{ m_screenWidth - ss.str().length() };

		std::cout << std::format("{:{}}", " HOME", lengthToPad) << ss.str() << std::endl;
		std::cout << drawHorizontalLine(m_screenWidth) << std::endl; 


	}






	std::string drawHorizontalLine(int width) {
		std::string s;

		for (int w{ 0 }; w <= width; ++w) {
			s += '-';
		}
		return s;
	}
}