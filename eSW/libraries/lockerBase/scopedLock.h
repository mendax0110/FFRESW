#ifndef LOCKER_BASE_SCOPEDLOCK_H
#define LOCKER_BASE_SCOPEDLOCK_H

#include <frt.h>

/// @brief Namespace for the locker system. \namespace locker
namespace locker
{   
    /// @brief Scoped lock class for mutexes. \class ScopedLock
    class ScopedLock
    {
    public:
        
        /**
         * @brief Construct a new Scoped Lock object
         * 
         * @param mutex -> The mutex to lock
         */
        explicit ScopedLock(frt::Mutex& mutex) : m_mutex(mutex), m_locked(true)
        {
            m_mutex.lock();
        }

        ScopedLock(const ScopedLock&) = delete;
        ScopedLock& operator=(const ScopedLock&) = delete;

        /**
         * @brief Construct a new Scoped Lock object
         * 
         * @param other -> The other ScopedLock object to move from
         */
        ScopedLock(ScopedLock&& other) noexcept : m_mutex(other.m_mutex), m_locked(other.m_locked)
        {
            other.m_locked = false;
        }

        // Deleted: assignment to a reference is illegal in C++
        ScopedLock& operator=(ScopedLock&&) = delete;

        ~ScopedLock()
        {
            if (m_locked)
                m_mutex.unlock();
        }

    private:
        frt::Mutex& m_mutex;
        bool m_locked;
    };
}

#endif // LOCKER_BASE_SCOPEDLOCK_H