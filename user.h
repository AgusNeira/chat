#ifndef USER_
#define USER_

#include <string>
#include <cstring>
#include <algorithm>
#include <vector>

struct User {
	User(int id, const char* name) : id(id), name(name) {}
	User(int id, const char* name, int len) : id(id), name(name, len) {}

	std::string name;
	int id = -1;
};

class UserComparator : public User {
public:
	UserComparator(int id) : User(id, "") {}
	UserComparator(char *name) : User(-1, name) {}

	bool operator()(const User& other){
		if (this->id > 0) return this->id == other.id;
		else return this->name == other.name;
	}
};

class UserTable {
public:
	typedef std::vector<User>::iterator					iterator;
	typedef std::vector<User>::const_iterator			const_iterator;
	typedef std::vector<User>::reverse_iterator			reverse_iterator;
	typedef std::vector<User>::const_reverse_iterator 	const_reverse_iterator;
	
	UserTable() : users_() {}

	void add(int id, char *name){
		users_.emplace_back(id, name);
	}
	void add(int id, char *name, int len){
		users_.emplace_back(id, name, len);
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
