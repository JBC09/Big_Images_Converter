#pragma once

#include <memory>
#include <mutex>

template <typename T>
class ISingleton
{
protected:
	explicit ISingleton() = default;
	virtual ~ISingleton() = default;

public:
	ISingleton(const ISingleton&) = delete;
	ISingleton &operator=(const ISingleton&) = delete;

	ISingleton(ISingleton&&) noexcept = delete;
	ISingleton &operator=(ISingleton&&) noexcept = delete;

private:
	static std::unique_ptr<T> instance;
	static std::once_flag onceFlag;

public:
	static T& GetInstance() noexcept
	{
		std::call_once(onceFlag, []()
		{
			instance.reset(new T);
		});

		return *instance.get();
	}
};

template <typename T>
std::unique_ptr<T> ISingleton<T>::instance;

template <typename T>
std::once_flag ISingleton<T>::onceFlag;
