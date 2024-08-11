#pragma once

#include "Base.h"


BEGIN(Engine)

/// <summary>
/// ������ ó���� ���� ������ ������ Ǯ
/// �񵿱� ó������ �� ü��ȭ�� ������ �۾��� �Ϸ��� �����Ͽ���.
/// ���� �׳� �ְ� �; �����Ŵ�.
/// </summary>
class ENGINE_DLL CThreadPool : public CBase
{
    INFO_CLASS(CThreadPool, CBase)

private:
    explicit CThreadPool();
    explicit CThreadPool(const CThreadPool& rhs);
    virtual ~CThreadPool();

public:
    HRESULT Initialize(_uint iNumThreads, _bool bAutoAllocateNum);

public:
    static CThreadPool* Create(_uint iNumThreads, _bool bAutoAllocateNum);
    virtual void Free() override;
    
public:
    // job �� �߰��Ѵ�. ���� ť�� ����ϴ� �Լ�.
    template <class F, class... Args>
    auto EnqueueJob(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

private:
    // Worker ������
    void WorkerThread();

private:
    _uint                   m_iNumThreads = { 0 };      // �� Worker �������� ����.
    vector<thread>          m_WorkderThreads;           // Worker �����带 �����ϴ� ����.
    queue<function<void()>> m_Jobs;                     // ���ϵ��� �����ϴ� job ť.
    condition_variable      m_JobQ_CV;                  // ���� job ť�� ���� ����� ���
    mutex                   m_JobQ_Mutex;               // ��ť ���ؽ�
    _bool                   m_bStopAll = { false };     // ��� ������ ���� ����

};


template <class F, class... Args>
auto CThreadPool::EnqueueJob(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
    using return_type = std::invoke_result_t<F, Args...>;

    if (m_bStopAll) {
        throw std::runtime_error("ThreadPool ��� ������");
    }

    auto job = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> job_result_future = job->get_future();
    {
        std::lock_guard<std::mutex> lock(m_JobQ_Mutex);
        m_Jobs.push([job]() { (*job)(); });
    }

    m_JobQ_CV.notify_one();

    return job_result_future;
}

END