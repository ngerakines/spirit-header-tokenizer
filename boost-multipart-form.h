
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

#define LOG_INFO(X) std::cout << X;
#define LOG_ERROR(X) std::cerr << X;

class Header;
class Part;
class MultiPartFormData;

std::ostream& operator<<(std::ostream& o, const MultiPartFormData& b);
std::ostream& operator<<(std::ostream& o, const Header& b);
std::ostream& operator<<(std::ostream& o, const Part& b);

class Header {

	public:

		Header(std::string name, std::map<std::string, std::string> attributes);

		std::string name() {
			return name_;
		}

		std::map<std::string, std::string> attributes() const {
			return attributes_;
		}

		std::string attributeValue(std::string name);

		virtual std::ostream& dump(std::ostream& o) const {
			std::stringstream ss;
			ss << "Header{";
			ss << " name=" << name_;

			std::map<std::string, std::string> attrs = attributes();
			std::map<std::string, std::string>::iterator iter;
			for (iter = attrs.begin(); iter != attrs.end(); ++iter) {
				ss << " " << iter->first;
				if (iter->second != "") {
					ss << "=" << iter->second;
				}
			}

			ss << "}";

			return o << ss.str();
		}

	private:
		std::string name_;
		std::map<std::string, std::string> attributes_;
};

class Part {

	public:
		std::vector<Header*> headers() {
			return headers_;
		}

		std::vector<char> payload() {
			return payload_;
		}

		void payload(std::string payload) {
			payload_.assign(payload.begin(), payload.end());
		}

		void payload(std::vector<char> payload) {
			payload_ = payload;
		}

		void add_header(Header* header) {
			headers_.push_back(header);
		}

		std::vector<Header*> headers() const {
			return headers_;
		} 

		void add_header(std::string input);

		virtual std::ostream& dump(std::ostream& o) const {
			std::stringstream ss;
			ss << "Part{";
			std::string payload(payload_.begin(), payload_.end());
			ss << " payload=***" << payload << "***";

			std::vector<Header*> hdrs = headers();
			std::vector<Header*>::iterator iter;
			for (iter = hdrs.begin(); iter != hdrs.end(); ++iter) {
				ss << " " << *(*iter);
			}

			ss << "}";

			return o << ss.str();
		}

	private:
		std::vector<Header*> headers_;
		std::vector<char> payload_;

};

class MultiPartFormData {

	public:

		std::string boundry() {
			return boundry_;
		}

		void boundry(std::string boundry) {
			boundry_ = boundry;
		}

		std::vector<Part*> parts() const {
			return parts_;
		}

		void add_part(Part* part) {
			parts_.push_back(part);
		}

		virtual std::ostream& dump(std::ostream& o) const {

			std::stringstream ss;
			ss << "MultiPartFormData{ boundry=" << boundry_ ;
			std::vector<Part*> prts = parts();
			std::vector<Part*>::iterator iter;
			ss << "parts=[" << std::endl;
			for (iter = prts.begin(); iter != prts.end(); ++iter) {
				ss << " " << *(*iter) << std::endl;
			}
			ss << "]" << std::endl;
			ss << "}";

			return o << ss.str();
		}

	private:
		std::string boundry_;
		std::vector<Part*> parts_;
};
