#ifndef PTRUTILS_H
#define PTRUTILS_H

#include <Arduino.h>

/**
 * @brief Template function to safely delete a pointer and set it to nullptr.
 *
 * @tparam T -> The type of the pointer.
 * @param ptr -> The pointer to delete.
 */
template <typename T>
static inline void SafeDelete(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}

/**
 * @brief Function to safely delete an array and set it to nullptr.
 *
 * @tparam T -> The type of the array.
 * @param ptr -> The array to delete.
 */
template <typename T>
static inline void SafeDeleteArray(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete[] ptr;
        ptr = nullptr;
    }
}

/**
 * @brief Template to verify a pointer is not nullptr.
 *
 * @tparam T -> The type of the pointer.
 * @param value -> The pointer to verify.
 * @param expected -> The expected value of the pointer.
 * @param errorMsg -> The error message to print if the verification fails.
 */
template <typename T>
static inline void Verify(const T& value, const T& expected, const char* errorMsg = nullptr)
{
    if (value != expected)
    {
        if (errorMsg)
        {
            Serial.println(errorMsg);
        }
        else
        {
            Serial.print("Verification failed: Value (");
            Serial.print(value);
            Serial.print(") does not match expected (");
            Serial.print(expected);
            Serial.println(").");
        }
        while (true); // Halt execution
    }
}

/**
 * @brief Template Specialization for pointers (Pointer vs Pointer)
 *
 * @tparam T -> The type of the pointer.
 * @param value -> The pointer to verify.
 * @param expected -> The expected value of the pointer.
 * @param errorMsg -> The error message to print if the verification fails.
 */
template <typename T>
static inline void Verify(T* value, T* expected, const char* errorMsg = nullptr)
{
    if (value != expected)
    {
        if (errorMsg)
        {
            Serial.println(errorMsg);
        }
        else
        {
            Serial.print("Verification failed: Pointer (");
            Serial.print((unsigned long)value, HEX); // Print pointer address in HEX
            Serial.print(") does not match expected pointer (");
            Serial.print((unsigned long)expected, HEX);
            Serial.println(").");
        }
        while (true); // Halt execution
    }
}

/**
 * @brief Template Specialization for pointers (Pointer vs nullptr)
 *
 * @tparam T -> The type of the pointer.
 * @param value -> The pointer to verify.
 * @param errorMsg -> The error message to print if the verification fails.
 */
template <typename T>
static inline void Verify(T* value, const char* errorMsg = nullptr)
{
    if (value != nullptr)  // Directly compare with nullptr (no std::nullptr_t)
    {
        if (errorMsg)
        {
            Serial.println(errorMsg);
        }
        else
        {
            Serial.print("Verification failed: Pointer (");
            Serial.print((unsigned long)value, HEX); // Print pointer address in HEX
            Serial.println(") is not null.");
        }
        while (true); // Halt execution
    }
}


/**
 * @brief Macro to safely delete a pointer and verify it is nullptr.
 *
 */
#define tryDeletePtr(ptr) 										\
    SafeDelete(ptr);        									\
    //Verify(ptr, nullptr, "Pointer not null after deletion!"); 	\


/// @brief Utility class for pointer operations. \class PtrUtils
class PtrUtils
{
public:
    /**
     * Check if a pointer is nullptr.
     *
     * \param ptr Pointer to check.
     * \return true if the pointer is nullptr, false otherwise.
     */
    template <typename T>
    static inline bool IsNullPtr(T* ptr)
    {
        return ptr == nullptr;
    }

    /**
     * Check if a pointer is valid (not nullptr).
     *
     * \param ptr Pointer to check.
     * \return true if the pointer is not nullptr, false otherwise.
     */
    template <typename T>
    static inline bool IsValidPtr(T* ptr)
    {
        return ptr != nullptr;
    }
};

/**
 * @brief Template to Clear an array of pointers.
 *
 * @tparam T -> The type of the pointer.
 * @param array -> The array of pointers.
 * @param size -> The size of the array.
 */
template <typename T>
static inline void ClearArray(T* array, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        array[i] = T();
    }
}

/**
 * @brief Template to print info about a pointer.
 *
 * @tparam T -> The type of the pointer.
 * @param ptr -> The pointer to print info about.
 * @param ptrName -> The name of the pointer.
 */
template <typename T>
static inline void PrintPtrInfo(T* ptr, const char* ptrName = "Pointer")
{
    if (ptr == nullptr)
    {
        Serial.print(ptrName);
        Serial.println(" is nullptr.");
    }
    else
    {
        Serial.print(ptrName);
        Serial.print(" points to address: 0x");
        Serial.println((uintptr_t)ptr, HEX);
    }
}

/**
 * @brief Template class for a Scoped Pointer. \class ScopedPointer
 *
 * @tparam T -> The type of the pointer.
 */
template <typename T>
class ScopedPointer
{
private:
    T* ptr;

public:
    explicit ScopedPointer(T* p = nullptr) : ptr(p) {}
    ~ScopedPointer() { SafeDelete(ptr); }

    /**
     * @brief Function to get the pointer.
     *
     * @return T* -> The pointer.
     */
    T* get() const { return ptr; }

    /**
     * @brief Function to release the pointer.
     *
     * @return T* -> The released pointer.
     */
    T* release()
    {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    /**
     * @brief Function to reset the pointer.
     *
     * @param p -> The pointer to reset to.
     */
    void reset(T* p = nullptr)
    {
        SafeDelete(ptr);
        ptr = p;
    }

    /**
     * @brief Operator to dereference the pointer.
     *
     * @return T& -> The dereferenced pointer.
     */
    T& operator*() const { return *ptr; }

    /**
     * @brief Operator to access the pointer.
     *
     * @return T* -> The pointer.
     */
    T* operator->() const { return ptr; }
};

/**
 * @brief Tempalte class for wrapping a pointer. \class PointerWrapper
 *
 * @tparam T
 */
template <typename T>
class PointerWrapper
{
private:
    T* ptr;

public:
    explicit PointerWrapper(T* p = nullptr) : ptr(p) {}
    ~PointerWrapper() { SafeDelete(ptr); }

    /**
     * @brief Function to get the pointer.
     *
     * @return T* -> The pointer.
     */
    T* get() const { return ptr; }

    /**
     * @brief Function to release the pointer.
     *
     * @return T* -> The released pointer.
     */
    T* release()
    {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    /**
     * @brief Function to reset the pointer.
     *
     * @param p -> The pointer to reset to.
     */
    void reset(T* p = nullptr)
    {
        SafeDelete(ptr);
        ptr = p;
    }

    /**
     * @brief Operator to dereference the pointer.
     *
     * @return T& -> The dereferenced pointer.
     */
    T& operator*() { return *ptr; }

    /**
     * @brief Operator to access the pointer.
     *
     * @return T* -> The pointer.
     */
    T* operator->() { return ptr; }
};

#endif // PTRUTILS_H
