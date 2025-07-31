#pragma once

#include <thread>
#include <chrono>
#include <mutex>
#include <fstream>

using namespace fs;

using std::thread;
using std::atomic;
using std::mutex;
using std::chrono::system_clock;
using std::stringstream;
using std::condition_variable;
using std::ofstream;
using std::format_string;
using std::format;

/**
 * @brief Log를 관리하는 매니저 클래스
 * 여러 개의 스레드가 Log를 작성하는 상황을 처리하기 위한 Lock이 세팅되어 있음
 *
 * @var MLogStatus Log Manager의 옵션을 저장하는 변수
 * @var MLogQueue Log Message가 들어오는 Queue
 * @var MLogFile Log를 파일에 기록하게 될 경우, 기록을 위한 Opened Stream
 * @var MLogThread Log를 작성하는 Worker Thread
 * @var MStopFlag Log 기록 시작과 끝을 정하는 변수
 * @var MQueueMutex Queue에 메시지 작업을 진행하기 위해 잡아야 하는 Mutex
 * @var MConsoleMutex Console에 메시지 출력을 진행하기 위해 잡아야 하는 Mutex
 * @var MCondition LogThread Wake를 위한 조건 변수
 */
class LogManager : public singleton<LogManager>
{
	SINGLE(LogManager)

private:
	ELogFlag LogStatus;
	queue<LogMessage> LogQueue;
	ofstream LogFile;

	thread LogThread;
	atomic<bool> StopFlag;
	mutex QueueMutex;
	mutex ConsoleMutex;
	condition_variable Condition;

private:
	void LogWorker();
	void ProcessLogMessage(const LogMessage& InMessage);
	void MakeLog(ELogLevel InLevel, const string& InMessage);

	void ManageLogFiles(const path& InDirectoryPath);
	static string GetLogLevelString(ELogLevel InLevel);

	// Getter & Setter
	[[nodiscard]]
	bool IsFlagSet(ELogFlag InFlag) const
	{
		return static_cast<UINT8>(LogStatus & InFlag) != 0;
	}

	void SetLogFlag(ELogFlag InLogFlag)
	{
		LogStatus |= InLogFlag;
	}

public:
	void Init(bool InHasConsoleOutput = true, bool InHasFileOutput = true);
	void Shutdown();

	// Log Make Function
	void LogTrace(const string& InMessage);
	void LogDebug(const string& InMessage);
	void LogInfo(const string& InMessage);
	void LogWarning(const string& InMessage);
	void LogError(const string& InMessage);
	void LogCritical(const string& InMessage);
	void LogUnknown(const string& InMessage);

	template <typename... Args>
	void LogTracef(std::format_string<Args...> fmt, Args&&... args)
	{
		MakeLog(ELogLevel::TRACE, format(fmt, std::forward<Args>(args)...));
	}

	template <typename... Args>
	void LogDebugf(format_string<Args...> fmt, Args&&... args)
	{
		MakeLog(ELogLevel::DEBUG, format(fmt, std::forward<Args>(args)...));
	}

	template <typename... Args>
	void LogInfof(format_string<Args...> fmt, Args&&... args)
	{
		MakeLog(ELogLevel::INFO, format(fmt, std::forward<Args>(args)...));
	}

	template <typename... Args>
	void LogWarningf(format_string<Args...> fmt, Args&&... args)
	{
		MakeLog(ELogLevel::WARNING, format(fmt, std::forward<Args>(args)...));
	}

	template <typename... Args>
	void LogErrorf(format_string<Args...> fmt, Args&&... args)
	{
		MakeLog(ELogLevel::ERR, format(fmt, std::forward<Args>(args)...));
	}

	template <typename... Args>
	void LogCriticalf(format_string<Args...> fmt, Args&&... args)
	{
		MakeLog(ELogLevel::CRITICAL, format(fmt, std::forward<Args>(args)...));
	}

	template <typename... Args>
	void LogUnknownf(format_string<Args...> fmt, Args&&... args)
	{
		MakeLog(ELogLevel::UNKNOWN, format(fmt, std::forward<Args>(args)...));
	}
};
