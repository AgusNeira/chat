#ifndef USERTABLES_
#define USERTABLES_

#include <vector>

class UserTables {
public:
	typedef std::vector<User>::iterator					iterator;
	typedef std::vector<User>::const_iterator			const_iterator;
	typedef std::vector<User>::reverse_iterator			reverse_iterator;
	typedef std::vector<User>::const_reverse_iterator 	const_reverse_iterator;
	
	UserTables() : users_() {}

	void add(int id, char *name){
		users_.emplace_back(id, name);
	}
	void remove(int id){
		auto it = this->find(id);

		users_.erase(it);
	}

	iterator find(int id){
		return std::find_if(users_.begin(), users_.end(), UserComparator(id));
	}
	iterator find(char *str){
		return std::find_if(users_.begin(), users_.end(), UserComparator(str));
	}

	iterator begin()					{ return users_.begin(); }
	iterator end()						{ return users_.end(); }

	const_iterator cbegin()				{ return users_.cbegin(); }
	const_iterator cend()				{ return users_.cend(); }

	reverse_iterator rbegin()			{ return users_.rbegin(); }
	reverse_iterator rend()				{ return users_.rend(); }

	const_reverse_iterator crbegin()	{ return users_.crbegin(); }
	const_reverse_iterator brend()		{ return users_.crend(); }


private:
	std::vector<User> users_;
};

#endif
