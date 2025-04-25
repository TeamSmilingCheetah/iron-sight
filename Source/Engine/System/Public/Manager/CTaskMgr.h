#pragma once

class CTaskMgr
	: public singleton<CTaskMgr>
{
	SINGLE(CTaskMgr);

private:
	vector<tTask> m_vecTask;
	vector<tTask> m_vecDelayedTask;	// 한 프레임 지연시키는 task

	bool m_LevelChanged;

public:
	void AddTask(const tTask& _task) { m_vecTask.push_back(_task); }
	bool IsLevelChanged() const { return m_LevelChanged; }


	void Tick();
};
