/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_LIB_CONCURRENCY_THREADPOOL_H
#define SCORE_LIB_CONCURRENCY_THREADPOOL_H

#include "score/concurrency/condition_variable.h"
#include "score/concurrency/executor.h"
#include "score/concurrency/task.h"

#include <score/deque.hpp>
#include <score/jthread.hpp>
#include <score/memory.hpp>
#include <score/memory_resource.hpp>
#include <score/stop_token.hpp>
#include <score/vector.hpp>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <unordered_map>
#include <vector>

namespace score
{
namespace concurrency
{

/**
 * \brief ThreadPool is an execution policy for the Executor interface.
 * It's main idea is to provide upon startup a fixed number of threads
 * that will then be reused in any way to run asynchronous tasks.
 *
 * It has a dynamic size of Task queue, which requires dynamic memory allocation.
 * In order to be used in safety systems, a memory_resource can be injected
 * which will be used for the dynamic allocation.
 */
class ThreadPool final : public Executor
{
  public:
    /**
     * \brief Creates a thread pool with a fixed size number of threads.
     * It will use HEAP Memory allocation for queueing any enqueued tasks.
     *
     * \param number_of_threads The number of threads that will be started
     * \param name The name assigned to this pool. Threads will inherit this name with a counter attached to it.
     *
     * TODO: For now we use a const reference to an std::string for the name.
     *       Once amp supports stream naming, we should switch to whatever they use. (Ticket-99121)
     */
    explicit ThreadPool(const std::size_t number_of_threads, const std::string& name = "threadpool");

    /**
     * \brief Creates a thread pool with a fixed size number of threads.
     *
     * With the provided memory_resource _no_ HEAP allocation will be done
     * after initialization!
     *
     * \param number_of_threads The number of threads that will be started
     * \param memory_resource The resource to acquire memory for enqueuing tasks
     * \param name The name assigned to this pool. Threads will inherit this name with a counter attached to it.
     */
    explicit ThreadPool(const std::size_t number_of_threads,
                        score::cpp::pmr::memory_resource* memory_resource,
                        const std::string& name = "threadpool");

    ~ThreadPool() noexcept override;

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) noexcept = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) noexcept = delete;

    std::size_t MaxConcurrencyLevel() const noexcept override;
    bool ShutdownRequested() const noexcept override;
    void Shutdown() noexcept override;
    void Enqueue(score::cpp::pmr::unique_ptr<Task> task) override;

  private:
    void InitializeThreads(const std::size_t number_of_threads, const std::string& name);
    void Work(const std::size_t thread_number, const score::cpp::stop_token stop_token);
    void Execute(score::cpp::pmr::unique_ptr<Task> task);
    // Required since thread_pool needs to call shutdown within its destructor but that method is virtual.
    // See also MISRA.DTOR.DYNAMIC
    void InternalShutdown() noexcept;

    std::atomic_bool shutdown_reqested_{false};
    InterruptibleConditionalVariable condition_{};
    std::mutex queue_mutex_{};
    score::cpp::pmr::vector<score::cpp::stop_source> active_{this->GetMemoryResource()};
    score::cpp::pmr::deque<score::cpp::pmr::unique_ptr<Task>> queue_{this->GetMemoryResource()};

    // This is intentionally last, since this will ensure that on destruction we first wait for all threads to stop
    // before we destruct anything else.
    score::cpp::pmr::vector<score::cpp::jthread> pool_{this->GetMemoryResource()};
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_THREADPOOL_H
