#ifndef LOCKGUARD_H
#define LOCKGUARD_H

#include <frt.h>
#include <Arduino.h>

/// @brief Class for the locker system. \class LockGuard
class LockGuard
{
public:

	/**
     * @brief Constructs a LockGuard and immediately locks the mutex
     * @param mutex Reference to the mutex to manage
     * @exception None (constructor is noexcept)
     */
    explicit LockGuard(frt::Mutex& mutex) : mutex_(mutex)
    {
        lock();
    }
	
    /**
     * @brief Destructor automatically unlocks the mutex if still locked
     */
    ~LockGuard()
    {
        unlock();
    }

    /**
     * @brief Locks the managed mutex if not already locked
     * @note Idempotent - safe to call multiple times
     */
    void lock()
    {
        if (!locked_)
        {
            mutex_.lock();
            locked_ = true;
        }
    }

    /**
     * @brief Unlocks the managed mutex if currently locked
     * @note Idempotent - safe to call multiple times
     */
    void unlock()
    {
        if (locked_)
        {
            mutex_.unlock();
            locked_ = false;
        }
    }

    /**
     * @brief Checks if the mutex is currently locked by this guard
     * @return true if mutex is locked, false otherwise
     */
    bool isLocked() const
    {
        return locked_;
    }
	
    /**
     * @brief Attempts to lock the mutex without blocking
     * @return true if lock was acquired, false if mutex was unavailable
     */
    bool tryLock()
    {
        if (!locked_ && mutex_.tryLock())
        {
            locked_ = true;
            return true;
        }
        return false;
    }

	// no-copyable declaration
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    frt::Mutex& mutex_;
    bool locked_ = false;
};

/**
 * @brief helper Macros for name generation
 */
#define CONCAT_INTERNAL(x,y) x##y
#define CONCAT(x,y) CONCAT_INTERNAL(x,y)
#define UNIQUE_NAME(prefix) CONCAT(prefix, __LINE__)


/**
 * @brief Automatic scoped lock that unlocks at end of block
 * @usage SCOPE_LOCK(mutex) { critical_code(); }
 */
#define SCOPE_LOCK(mutex) \
    if (LockGuard UNIQUE_NAME(_lock_)(mutex); true)

/**
 * @brief Creates a named lock guard that can be manually unlocked
 * @usage DROP_LOCKER(locker, mutex) { locker.unlock(); }
 */
#define DROP_LOCKER(varName, mutex) \
    LockGuard varName(mutex)

/**
 * @brief Try-lock pattern that only enters block if lock was acquired
 * @usage TRY_LOCK(mutex) { critical_code(); }
 */
#define TRY_LOCK(mutex) \
    if (LockGuard UNIQUE_NAME(_lock_)(mutex); UNIQUE_NAME(_lock_).tryLock())

/**
 * @brief Conditional lock that only locks if condition is true
 * @usage COND_LOCK(mutex, shouldLock) { conditional_code(); }
 */
#define COND_LOCK(mutex, condition) \
    if (LockGuard UNIQUE_NAME(_lock_)(mutex); (condition) ? UNIQUE_NAME(_lock_).lock() : (void)0)


#endif // LOCKGUARD_H