#include "pch.h"
#include "Engine/System/Public/Manager/LogManager.h"

#include <iostream>
#include <array>

using std::cout;
using std::ios;

using std::unique_lock;
using std::lock_guard;
using std::this_thread::get_id;

using std::array;
using std::setfill;
using std::setw;

using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::put_time;

using std::ranges::sort;

constexpr array<const char*, static_cast<size_t>(ELogLevel::END)>
LogLevelArr = {
	"TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "UNKNOWN",
};

LogManager::LogManager() = default;

LogManager::~LogManager() = default;

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
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

	tm tm_struct;
	(void)localtime_s(&tm_struct, &time_t_value);

	// 4. stringstream으로 원하는 포맷의 문자열 생성
	stringstream ss;
	ss << put_time(&tm_struct, "%Y-%m-%d %H:%M:%S"); // tm_struct의 주소 전달
	ss << '.' << setfill('0') << setw(3) << ms.count();

	TimeStamp = ss.str();
	ThreadID = get_id();
}

/**
 * @brief Log Manager 초기화 함수
 * 여기서 Worker Thread가 생성된다
 */
void LogManager::Init(bool InHasConsoleOutput, bool InHasFileOutput)
{
	if (InHasConsoleOutput)
	{
		SetLogFlag(ELogFlag::ConsoleOutput);
	}
	if (InHasFileOutput)
	{
		SetLogFlag(ELogFlag::FileOutput);
	}

	if (IsFlagSet(ELogFlag::FileOutput))
	{
		path LogFilePath = CPathMgr::GetInst()->GetLogFilePath();

		// Log 파일 경로 및 갯수 관리
		ManageLogFiles(LogFilePath);

		// 적당한 Log 파일명 작성
		auto now = system_clock::now();
		auto time_t_value = system_clock::to_time_t(now);

		tm tm_struct;
		(void)localtime_s(&tm_struct, &time_t_value);

		stringstream ss;
		ss << put_time(&tm_struct, "%Y-%m-%d_%H-%M-%S");
		path FileName = ss.str() + ".log";

		// Full Path 생성
		LogFilePath /= FileName;

		// Path를 활용하여 파일 생성
		LogFile.open(LogFilePath, ios::out | ios::app);
	}

	LogThread = thread(&LogManager::LogWorker, this);

	LogInfo("Log Manager Initialized Successfully");
}

/**
 * Log Manager 종료 함수
 */
void LogManager::Shutdown()
{
	// 다중 스레드의 중복 호출 방지
	if (StopFlag.load())
	{
		return;
	}

	// Sleep된 스레드들 전부 기상
	StopFlag = true;
	Condition.notify_all();

	// 작업 중인 스레드 작업 종료 대기
	if (LogThread.joinable())
	{
		LogThread.join();
	}

	// File Stream 종료
	if (LogFile.is_open())
	{
		LogFile.close();
	}

	LogInfo("Log Manager Shutdown Completed");
}

/**
 * @brief Log를 처리하는 Worker Thread의 로직
 */
void LogManager::LogWorker()
{
	while (!StopFlag.load())
	{
		// LogQueue와 Flag가 조건을 만족하지 않는다면 계속 Sleep
		unique_lock QueueLock(QueueMutex);
		Condition.wait(QueueLock,
		               [this]
		               {
			               return !LogQueue.empty() || StopFlag.load();
		               });

		// 1개의 메시지마다 락을 잡았다 풀면서 순차적 처리
		while (!LogQueue.empty())
		{
			LogMessage message = LogQueue.front();
			LogQueue.pop();

			// Critical Section
			QueueLock.unlock();
			ProcessLogMessage(message);
			QueueLock.lock();
		}
	}

	// Flag 변경으로 Loop가 종료되는 동안 발생했을 수도 있는 Message 처리
	lock_guard QueueLock(QueueMutex);
	while (!LogQueue.empty())
	{
		LogMessage Message = LogQueue.front();
		LogQueue.pop();
		ProcessLogMessage(Message);
	}
}

/**
 * Log Message를 받아, 미리 설정해 둔 로깅 옵션에 맞게 출력 및 저장하는 함수
 * @param InMessage 메시지 객체
 */
void LogManager::ProcessLogMessage(const LogMessage& InMessage)
{
	stringstream ss;
	ss << "[" << InMessage.TimeStamp << "]";
	ss << "[" << InMessage.ThreadID << "]";
	ss << "[" << GetLogLevelString(InMessage.LogLevel) << "] ";
	ss << InMessage.Message;

	string FormattedMessage = ss.str();

	// Console Log
	if (IsFlagSet(ELogFlag::ConsoleOutput))
	{
		lock_guard ConsoleLock(ConsoleMutex);

		// Change Color By Log Level
		HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		WORD OriginalColor = 0;
		CONSOLE_SCREEN_BUFFER_INFO CSBI;
		if (GetConsoleScreenBufferInfo(ConsoleHandle, &CSBI))
		{
			OriginalColor = CSBI.wAttributes;
		}

		switch (InMessage.LogLevel)
		{
		case ELogLevel::TRACE:
			{
				SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				break;
			}
		case ELogLevel::DEBUG:
			{
				SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				break;
			}
		case ELogLevel::INFO:
			{
				SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			}
		case ELogLevel::WARNING:
			{
				SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			}
		case ELogLevel::ERR:
			{
				SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			}
		case ELogLevel::CRITICAL:
			{
				SetConsoleTextAttribute(
					ConsoleHandle,
					BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			}
		case ELogLevel::UNKNOWN:
			{
				SetConsoleTextAttribute(
					ConsoleHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			}
		default:
			{
				assert(!"여기 들어오지 못하도록 세팅해 둠");
			}
		}

		cout << FormattedMessage << "\n";

		// 기존 색상 복원
		SetConsoleTextAttribute(ConsoleHandle, OriginalColor);
	}
	else
	{
		LOG_ERROR("Failed To Print In Console");
	}

	// File Log
	if (IsFlagSet(ELogFlag::FileOutput) && LogFile.is_open())
	{
		LogFile << FormattedMessage << "\n";
		LogFile.flush();
	}
	else
	{
		LOG_ERROR("Failed To Print In Log File");
	}
}

/**
 * 로그 레벨에 해당하는 텍스트 변환 함수
 * @param InLevel Log Level Enum
 * @return
 */
string LogManager::GetLogLevelString(ELogLevel InLevel)
{
	auto LevelIndex = static_cast<size_t>(InLevel);

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
void LogManager::MakeLog(ELogLevel InLevel, const string& InMessage)
{
	LogMessage LogMessage(InLevel, InMessage);

	// Lock은 최소한으로 (구조체 생성에 Lock 잡지 않음)
	{
		lock_guard Lock(QueueMutex);
		LogQueue.push(LogMessage);
	}

	// Queue에 메시지가 들어갔음을 전달하여 Worker 스레드를 깨움
	Condition.notify_one();
}

/**
 * @brief Log File을 저장할 경로를 생성하고, 이후 파일 갯수를 관리하는 함수
 * @param InDirectoryPath Log File 경로
 */
void LogManager::ManageLogFiles(const path& InDirectoryPath)
{
	constexpr int MAX_LOG_FILES = 20;

	// Log 파일을 담을 폴더가 존재하지 않는다면 생성
	if (!exists(InDirectoryPath))
	{
		create_directories(InDirectoryPath);
		return;
	}

	try
	{
		// Get File Information
		vector<path> Files;
		for (const auto& entry : directory_iterator(InDirectoryPath))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".log")
			{
				Files.push_back(entry.path());
			}
		}

		// 로그 파일이 20개를 초과하는 경우에만 정리 시작
		if (Files.size() > MAX_LOG_FILES)
		{
			// 최종 시간 순으로 정렬
			sort(Files,
			     [](const path& FileA, const path& FileB)
			     {
				     return last_write_time(FileA) < last_write_time(FileB);
			     });

			int DeleteCount = static_cast<int>(Files.size()) - MAX_LOG_FILES + 1;

			LogInfo("[Engine][Log] Log File Limit Exceeded. Deleting Oldest Log Files...");

			// 가장 오래된 파일부터 순서대로 삭제
			for (int i = 0; i < DeleteCount; ++i)
			{
				remove(Files[i]);
			}
		}
	}
	catch (const filesystem_error& e)
	{
		// File System Error
		std::cerr << "Error managing log files: " << e.what() << "\n";
	}
}

void LogManager::LogTrace(const string& InMessage)
{
	MakeLog(ELogLevel::TRACE, InMessage);
}

void LogManager::LogDebug(const string& InMessage)
{
	MakeLog(ELogLevel::DEBUG, InMessage);
}

void LogManager::LogInfo(const string& InMessage)
{
	MakeLog(ELogLevel::INFO, InMessage);
}

void LogManager::LogWarning(const string& InMessage)
{
	MakeLog(ELogLevel::WARNING, InMessage);
}

void LogManager::LogError(const string& InMessage)
{
	MakeLog(ELogLevel::ERR, InMessage);
}

void LogManager::LogCritical(const string& InMessage)
{
	MakeLog(ELogLevel::CRITICAL, InMessage);
}

void LogManager::LogUnknown(const string& InMessage)
{
	MakeLog(ELogLevel::UNKNOWN, InMessage);
}
