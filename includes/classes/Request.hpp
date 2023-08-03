#ifndef REQUEST_H
# define REQUEST_H

# include <cstring>
# include <stdlib.h>
# include <iostream>
# include <unistd.h>
# include <cerrno>
# include "classes/Utils.hpp"
# include "classes/RequestParser.hpp" 
# include "enums/HttpMethodsEnum.hpp"

# define BUFFER_SIZE 30000

class Request
{
	public:

		Request();
		~Request();

		const char*         		get_mensage_request() const;
		size_t						get_size() const;

		RequestParser				_parser;
		std::string					get_method() const;
		std::string					get_path(void) const;
		std::string					get_http_version(void) const;
		std::string					get_body(void) const;
		std::string					get_header(std::string header_name) const;

		Request						&create_parsed_message(int fd_connecion);
		
		char            			_request[BUFFER_SIZE];



};

#endif
