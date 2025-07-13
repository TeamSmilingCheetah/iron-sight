#include "pch.h"
#include "Engine/System/Public/Manager/FLogManager.h"

#include <iostream>
#include <array>

using std::unique_lock;
using std::lock_guard;
using std::cout;
using std::array;
using std::chrono::duration_cast;

constexpr array<const char*, static_cast<size_t>(ELogLevel::END)>
LogLevelArr = {
	"TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "UNKNOWN",
};

FLogManager::FLogManager() = default;

FLogManager::~FLogManager() = default;

/**
 * Log Message 구조체의 생성자
 * @param PLevel 로그 레벨
 * @param PMessage 로그가 담을 메시지
 */
LogMessage::LogMessage(ELogLevel PLevel, const string& PMessage)
{
	LogLevel = PLevel;
	Message = PMessage;

	// 타임스탬프 생성
	auto now = system_clock::now();
	auto time_t_value = system_clock::to_time_t(now);
	auto ms = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	tm tm_struct;
	(void)localtime_s(&tm_struct, &time_t_value);

	// 4. stringstream으로 원하는 포맷의 문자열 생성
	std::stringstream ss;
	ss << std::put_time(&tm_struct, "%Y-%m-%d %H:%M:%S"); // tm_struct의 주소 전달
	ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

	TimeStamp = ss.str();
	ThreadID = std::this_thread::get_id();
}

/**
 * @brief Log Manager 초기화 함수
 * 여기서 Worker Thread가 생성된다
 */
void FLogManager::Init(bool PConsoleOutput, bool PFileOutput)
{
	if (PConsoleOutput)
	{
		SetLogFlag(ELogFlag::ConsoleOutput);
	}
	if (PFileOutput)
	{
		SetLogFlag(ELogFlag::FileOutput);
	}

	if (IsFlagSet(ELogFlag::FileOutput))
	{
		// Log 파일을 담을 폴더가 존재하지 않는다면 생성
		path LogFilePath = CPathMgr::GetInst()->GetLogFilePath();
		if (!exists(LogFilePath))
		{
			create_directories(LogFilePath);
		}

		// 적당한 Log 파일명 작성
		auto now = system_clock::now();
		auto time_t_value = system_clock::to_time_t(now);

		tm tm_struct;
		(void)localtime_s(&tm_struct, &time_t_value);

		std::stringstream ss;
		ss << std::put_time(&tm_struct, "%Y-%m-%d_%H-%M-%S");
		path FileName = ss.str() + ".log";

		// Full Path 생성
		LogFilePath /= FileName;

		// Path를 활용하여 파일 생성
		MLogFile.open(LogFilePath, std::ios::out | std::ios::app);
	}

	MLogThread = thread(&FLogManager::LogWorker, this);

	LogInfo("Log Manager Initialized Successfully");
}

/**
 * Log Manager 종료 함수
 */
void FLogManager::Shutdown()
{
	// 다중 스레드의 중복 호출 방지
	if (MStopFlag.load())
	{
		return;
	}

	// Sleep된 스레드들 전부 기상
	MStopFlag = true;
	MCondition.notify_all();

	// 작업 중인 스레드 작업 종료 대기
	if (MLogThread.joinable())
	{
		MLogThread.join();
	}

	// File Stream 종료
	if (MLogFile.is_open())
	{
		MLogFile.close();
	}

	LogInfo("Log Manager Shutdown Completed");
}

/**
 * @brief Log를 처리하는 Worker Thread의 로직
 */
void FLogManager::LogWorker()
{
	while (!MStopFlag.load())
	{
		// LogQueue와 Flag가 조건을 만족하지 않는다면 계속 Sleep
		unique_lock QueueLock(MQueueMutex);
		MCondition.wait(QueueLock,
		                [this]
		                {
			                return !MLogQueue.empty() || MStopFlag.load();
		                });

		// 1개의 메시지마다 락을 잡았다 풀면서 순차적 처리
		while (!MLogQueue.empty())
		{
			LogMessage message = MLogQueue.front();
			MLogQueue.pop();

			// Critical Section
			QueueLock.unlock();
			ProcessLogMessage(message);
			QueueLock.lock();
		}
	}

	// Flag 변경으로 Loop가 종료되는 동안 발생했을 수도 있는 Message 처리
	lock_guard QueueLock(MQueueMutex);
	while (!MLogQueue.empty())
	{
		LogMessage Message = MLogQueue.front();
		MLogQueue.pop();
		ProcessLogMessage(Message);
	}
}

/**
 * Log Message를 받아, 미리 설정해 둔 로깅 옵션에 맞게 출력 및 저장하는 함수
 * @param PMessage 메시지 객체
 */
void FLogManager::ProcessLogMessage(const LogMessage& PMessage)
{
	stringstream ss;
	ss << "[" << PMessage.TimeStamp << "] ";
	ss << "[" << GetLogLevelString(PMessage.LogLevel) << "] ";
	ss << "[" << PMessage.ThreadID << "] ";
	ss << PMessage.Message;

	string FormattedMessage = ss.str();

	// Console Log
	if (IsFlagSet(ELogFlag::ConsoleOutput))
	{
		lock_guard ConsoleLock(MConsoleMutex);

		// Change Color By Log Level
		HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		WORD OriginalColor = 0;
		CONSOLE_SCREEN_BUFFER_INFO CSBI;
		if (GetConsoleScreenBufferInfo(ConsoleHandle, &CSBI))
		{
			OriginalColor = CSBI.wAttributes;
		}

		switch (PMessage.LogLevel)
		{
		case ELogLevel::TRACE:
			SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		case ELogLevel::DEBUG:
			SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		case ELogLevel::INFO:
			SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case ELogLevel::WARNING:
			SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case ELogLevel::ERR:
			SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case ELogLevel::CRITICAL:
			SetConsoleTextAttribute(
				ConsoleHandle,
				BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break;
		case ELogLevel::UNKNOWN:
			SetConsoleTextAttribute(
				ConsoleHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		default:
			{
				assert(!"여기 들어오지 못하도록 세팅해 둠");
			}
		}

		cout << FormattedMessage << "\n";

		// 기존 색상 복원
		SetConsoleTextAttribute(ConsoleHandle, OriginalColor);
	}

	// File Log
	if (IsFlagSet(ELogFlag::FileOutput) && MLogFile.is_open())
	{
		lock_guard FileLock(MQueueMutex);
		MLogFile << FormattedMessage << "\n";
		MLogFile.flush();
	}
}

/**
 * 로그 레벨에 해당하는 텍스트 변환 함수
 * @param PLevel Log Level Enum
 * @return
 */
string FLogManager::GetLogLevelString(ELogLevel PLevel)
{
	auto LevelIndex = static_cast<size_t>(PLevel);

	if (LevelIndex < LogLevelArr.size())
	{
		return LogLevelArr[LevelIndex];
	}

	assert(!"지정한 범위 바깥의 Log Level을 전달 받음");
	return "NULL";
}

/** Log Creation Functions **/

/**
 * @brief Log 생성 기본 함수
 */
void FLogManager::MakeLog(ELogLevel PLevel, const std::string& PMessage)
{
	LogMessage LogMessage(PLevel, PMessage);

	// Lock은 최소한으로 (구조체 생성에 Lock 잡지 않음)
	{
		lock_guard Lock(MQueueMutex);
		MLogQueue.push(LogMessage);
	}

	// Queue에 메시지가 들어갔음을 전달하여 Worker 스레드를 깨움
	MCondition.notify_one();
}

void FLogManager::LogTrace(const std::string& message)
{
	MakeLog(ELogLevel::TRACE, message);
}

void FLogManager::LogDebug(const std::string& message)
{
	MakeLog(ELogLevel::DEBUG, message);
}

void FLogManager::LogInfo(const std::string& message)
{
	MakeLog(ELogLevel::INFO, message);
}

void FLogManager::LogWarning(const std::string& message)
{
	MakeLog(ELogLevel::WARNING, message);
}

void FLogManager::LogError(const std::string& message)
{
	MakeLog(ELogLevel::ERR, message);
}

void FLogManager::LogCritical(const std::string& message)
{
	MakeLog(ELogLevel::CRITICAL, message);
}
