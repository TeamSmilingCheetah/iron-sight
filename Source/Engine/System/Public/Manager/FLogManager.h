#pragma once

#include <thread>
#include <chrono>
#include <mutex>
#include <sstream>
#include <fstream>

using std::thread;
using std::atomic;
using std::mutex;
using std::chrono::system_clock;
using std::stringstream;
using std::condition_variable;
using std::ofstream;

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
class FLogManager : public singleton<FLogManager>
{
	SINGLE(FLogManager)

private:
	ELogFlag MLogStatus;
	queue<LogMessage> MLogQueue;
	ofstream MLogFile;

	thread MLogThread;
	atomic<bool> MStopFlag;
	mutex MQueueMutex;
	mutex MConsoleMutex;
	condition_variable MCondition;

private:
	void LogWorker();
	void ProcessLogMessage(const LogMessage& PMessage);
	void MakeLog(ELogLevel PLevel, const string& PMessage);
	void LogVf(ELogLevel PLevel, const char* PFormat, va_list PArgs);

	static string GetLogLevelString(ELogLevel PLevel);

	// Getter & Setter
	[[nodiscard]]
	bool IsFlagSet(ELogFlag PFlag) const
	{
		return static_cast<UINT8>(MLogStatus & PFlag) != 0;
	}

	void SetLogFlag(ELogFlag PLogFlag)
	{
		MLogStatus |= PLogFlag;
	}

public:
	void Init(bool PConsoleOutput = true, bool PFileOutput = true);
	void Shutdown();

	// Log Make Function
	void LogTrace(const string& PMessage);
	void LogDebug(const string& PMessage);
	void LogInfo(const string& PMessage);
	void LogWarning(const string& PMessage);
	void LogError(const string& PMessage);
	void LogCritical(const string& PMessage);
	void LogUnknown(const string& PMessage);

	void LogTracef(const char* PFormat, ...);
	void LogDebugf(const char* PFormat, ...);
	void LogInfof(const char* PFormat, ...);
	void LogWarningf(const char* PFormat, ...);
	void LogErrorf(const char* PFormat, ...);
	void LogCriticalf(const char* PFormat, ...);
	void LogUnknownf(const char* PFormat, ...);
};
