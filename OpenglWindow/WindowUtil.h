#ifndef WINDOW_UTIL_H
#define WINDOW_UTIL_H

namespace Util
{
	template <typename Type, int MAX_SIZE = 100>
	class StaticQueue {
	public:
		Type cont[MAX_SIZE];
		int start = 0;
		int end = 0;
		int size = 0;

		void insert (const Type& arg) {
			if (size < MAX_SIZE) {
				cont[start] = arg;
				start++;
				if (start >= MAX_SIZE)
					start = 0;
				size++;
			}
			else {
				cont[start] = arg;
				start++;
				end++;
				if (end >= MAX_SIZE)
					end = 0;
				if (start >= MAX_SIZE)
					start = 0;
			}
		}

		Type pop() {
			if (size > 0) {
				int toRet = end;
				size--;
				end++;
				if (end >= MAX_SIZE)
					end = 0;
				return cont[toRet];
			}
		}

		bool empty() {
			return size == 0;
		}
	};

	template <typename Type>
	bool isEqualToAny (Type val, const std::initializer_list<Type>& list) {
		for (const auto& i : list) {
			if (val == i) {
				return true;
			}
		}
		return false;
	}
}

#endif