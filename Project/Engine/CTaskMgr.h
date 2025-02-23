#pragma once

class CTaskMgr
    : public singleton<CTaskMgr>
{
    SINGLE(CTaskMgr);
    vector<tTask> m_vecTask;
    vector<CGameObject*> m_vecGC;

    bool m_LevelChanged;

public:
    void AddTask(const tTask& _task) { m_vecTask.push_back(_task); }
    bool IsLevelChanged() { return m_LevelChanged; }


    void Tick();
};
