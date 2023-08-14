#ifndef requestValidator_HPP
#define requestValidator_HPP

# include <string>
# include "enums/HttpMethodsEnum.hpp"
# include <iostream>
# include <map>
# include "classes/Request.hpp"
# include "classes/Conf.hpp"
# include "ServerData.hpp"

class RequestValidator
{

	public:
		RequestValidator ();
		~RequestValidator();

		RequestValidator    		&requestValidator(ServerData &serverData, Request &request);
		HttpMethodEnum::httpMethod	getMethod(void) const;
		bool				        getPath(void) const;
		bool				        getHttpVersion(void) const;
		bool				        getBody(void) const;
		bool				        getHeader(std::string headerName) const;

	private:
		HttpMethodEnum::httpMethod	methodValidator(Request& request);
		void 						pathValidator(ServerData &serverData, Request& request);
		void						bodyValidator(Request& request);
		void						httpVersionValidator(Request& request);
		bool						isRootPath(const std::string& path, size_t	len);
		bool 						endsWithSlash(size_t position, size_t len);


    	void 						handleRootPath(ServerData &serverData, Request& request, const std::string& path, const std::string& root);
    	void 						handlePathWithTrailingSlash(ServerData &serverData, Request& request, const std::string& path, const std::string& root);
    	void 						handleNonTrailingSlashPath(ServerData &serverData, Request& request, const std::string& path, const std::string& root, size_t position);
    	void 						handleAssetsPath(Request& request, const std::string& path, const std::string& root);
		HttpMethodEnum::httpMethod	_method;
		bool                 		_path;
		bool                		_httpVersion;
		bool						_requestBody;
};

#endif
