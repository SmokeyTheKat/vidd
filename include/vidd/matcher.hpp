#ifndef __VIDD_MATCHER_HPP__
#define __VIDD_MATCHER_HPP__

#include <unordered_map>
#include <vector>

enum class MatcherResult {
	Exact,
	Closest,
	Invalid
};

enum class MatcherPosition {
	End,
	Middle,
};

template <class T, class U, class Container = std::vector<T>>
class Matcher {
	struct MatchNode;

	using Branch = std::unordered_map<T, MatchNode*>;

	struct MatchNode {
		bool occupied = false;
		Container input;
		U value;
		Branch branch;
	};

	struct MatchData {
		Container input;
		U value;

		MatchData(Container input, U value)
		: input(input), value(value) {};
	};

	struct MatchResult {
		MatcherResult type;
		MatcherPosition position;
		Container input;
		U value;
	};

	MatchNode mRoot;

public:
	Matcher(void) {};
	Matcher(std::initializer_list<MatchData> ilist) {
		for (auto d : ilist) {
			insert(d.input, d.value);
		}
	};

	~Matcher(void) {
		recursiveFree(&mRoot);
	};

	bool has(const Container& find) const {
		auto first = std::begin(find);
		auto last = std::end(find);
		if (first == last) return false;
		return has(first, last);
	};

	template <class IterType>
	bool has(IterType first, IterType last) const {
		static_assert(
			std::is_constructible<typename std::iterator_traits<IterType>::value_type, T>::value,
			"result type must be constructible from input type"
		);

		MatchResult res = get(first, last);
		return res.type == MatcherResult::Exact;
	};

	bool hasBranch(const Container& find) const {
		auto first = std::begin(find);
		auto last = std::end(find);
		if (first == last) return false;
		return hasBranch(first, last);
	};

	template <class IterType>
	bool hasBranch(IterType first, IterType last) const {
		static_assert(
			std::is_constructible<typename std::iterator_traits<IterType>::value_type, T>::value,
			"result type must be constructible from input type"
		);

		MatchResult res = get(first, last);
		return res.position == MatcherPosition::Middle || res.type == MatcherResult::Exact || res.type == MatcherResult::Closest;
	};

	void insert(const Container& find, U value) {
		auto first = std::begin(find);
		auto last = std::end(find);
		if (first == last) return;
		insert(first, last, value);
	};

	template <class IterType>
	void insert(IterType first, IterType last, U value) {
		static_assert(
			std::is_constructible<typename std::iterator_traits<IterType>::value_type, T>::value,
			"result type must be constructible from input type"
		);

		MatchNode* node = &mRoot;

		for (IterType it = first; it != last; it++) {
			Branch& b = node->branch;
			auto bIt = b.find(*it);
			if (bIt == b.end()) {
				node = new MatchNode;
				b[*it] = node;
			} else {
				node = bIt->second;
			}
		}
		node->value = value;
		node->input = Container(first, last);
		node->occupied = true;
	}

	MatchResult get(const Container& find) const {
		auto first = std::begin(find);
		auto last = std::end(find);
		if (first == last) return {};
		return get(first, last);
	};

	template <class IterType>
	MatchResult get(IterType first, IterType last) const {
		static_assert(
			std::is_constructible<typename std::iterator_traits<IterType>::value_type, T>::value,
			"result type must be constructible from input type"
		);

		const MatchNode* node = &mRoot;
		const MatchNode* closestNode = 0;
		bool found = true;

		IterType it;
		for (it = first; it != last; it++) {
			const Branch& b = node->branch;
			auto bIt = b.find(*it);
			if (bIt == b.end()) {
				found = false;
				break;
			} else {
				node = bIt->second;
				if (node->occupied == true) {
					closestNode = node;
				}
			}
		}

		MatcherPosition position = MatcherPosition::End;
		if (found && node != &mRoot && node->branch.size() > 0) position = MatcherPosition::Middle;

		if (found && node->occupied == true) {
			return MatchResult{ MatcherResult::Exact, position, Container(first, last), node->value };
		} else if (closestNode && closestNode->occupied == true) {
			return MatchResult{ MatcherResult::Closest, MatcherPosition::Middle, Container(first, last), closestNode->value };
		} else {
			return MatchResult{ MatcherResult::Invalid, position, Container{}, U{} };
		}
	}

	std::vector<MatchData> getAll(const Container& find) const {
		auto first = std::begin(find);
		auto last = std::end(find);
		if (first == last) return {};
		return getAll(first, last);
	};

	template <class IterType>
	std::vector<MatchData> getAll(IterType first, IterType last) const {
		static_assert(
			std::is_constructible<typename std::iterator_traits<IterType>::value_type, T>::value,
			"result type must be constructible from input type"
		);

		const MatchNode* node = &mRoot;
		bool found = true;

		std::vector<MatchData> output;

		IterType it;
		for (it = first; it != last; it++) {
			const Branch& b = node->branch;
			auto bIt = b.find(*it);
			if (bIt == b.end()) {
				found = false;
				break;
			} else {
				node = bIt->second;
			}
		}

		if (!found) return output;

		getAllFrom(output, node);

		return output;
	};

	static const MatchResult invalid(void) {
		return MatchResult{ MatcherResult::Invalid };
	};

private:
	void getAllFrom(std::vector<MatchData>& output, const MatchNode* node) const {
		if (node->occupied)
			output.push_back(MatchData(node->input, node->value));
		for (const auto& [key, nextNode] : node->branch) {
			getAllFrom(output, nextNode);
		}
	};

	void recursiveFree(MatchNode* node) {
		for (const auto& [_, next] : node->branch) {
			recursiveFree(next);
			delete next;
		}
	};
};

#endif