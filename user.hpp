#include <cstring>
#include <algorithm>
#include <vector>

struct User {
	User(int id, char* name){
		this->id = id;
		std::strcpy(this->name, name);
	}

	bool operator <(const User& other) const {
		return this->id < other.id;
	}
	bool operator <=(const User& other) const {
		return this->id <= other.id;
	}
	bool operator >(const User& other) const {
		return this->id > other.id;
	}
	bool operator >=(const User& other) const {
		return this->id >= other.id;
	}
	bool operator ==(const User& other) const {
		return this->id == other.id;
	}
	bool operator !=(const User& other) const {
		return this->id != other.id;
	}

	char name[20];
	int id = -1;
};

class UserComparator : public User {
	UserComparator(int id){
		this->id = id;
	}
	UserComparator(char *name){
		std::strcpy(this->name, name);
		this->id = -1;
	}

	bool operator(const User& other){
		if (this->id > 0) return this->id == other.id;
		else return std::strcmp(this->name, other.name) == 0;
	}
};

class UserTables {
public:

	add(int id, char *name){
		users_.emplace(users_.begin(), id, name);
	}

private:
	std::vector<User> users_;
}
