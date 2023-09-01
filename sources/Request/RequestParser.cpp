#include "classes/RequestParser.hpp"
# include <sstream>
#define SP				" "
RequestParser::RequestParser(void) : _headers(), _method(""), _path(""), _httpVersion(""), _requestBody("") { }

static bool has_delimiter(std::string line, std::string delimiter)
{
	return (line.rfind(delimiter) != std::string::npos);
}

RequestParser::~RequestParser(void) {}

void    							RequestParser::parserHttpRequest(int fdConection)
{
    std::string line;
    char		buffer[2] = {0};
	ssize_t		num_of_bytes;	
	std::string temp_line;
    this->_fdClient = fdConection;
	while (true)
	{
		num_of_bytes = recv(_fdClient, buffer, 1, 0);
		if (num_of_bytes == -1)
			throw (std::exception());
		if (num_of_bytes == 0)
			break ;
		temp_line += buffer;
        if (has_delimiter(temp_line, CRLF))
            break ;
	}
	line = temp_line;
    if (has_delimiter(temp_line, CRLF))
		line.resize(line.rfind(CRLF));
	
	std::istringstream	iss(line);
    // std::string line2;
    // std::string temp_line2;
    // std::string lin;

	parseRequestStartLine(temp_line, iss);
  
        // std::cout << temp_line2 << std::endl;

	parseRequestHeader(line, iss);
	parseRequestBody(fdConection);
    parseRequestPort();
    parserServerName();
    parseRquestQuery();
}

void    RequestParser::parseRquestQuery()
{	
    size_t queryStart = this->_path.find_first_of('?');
    if (queryStart != std::string::npos)
	{
		this->_query =  this->_path.substr(queryStart + 1);
	}
}

void    RequestParser::parseRequestPort()
{   
    std::string host = getHeader("Host");
    size_t      colonPos = host.find(':');

    if (colonPos != std::string::npos)
    {
        std::string portSubstring = host.substr(colonPos + 1);
        if (!portSubstring.empty())
        {
            this->_port = portSubstring;
            this->_portNumber = std::atoi(portSubstring.c_str());
        }
    }
}

void RequestParser::parserServerName(void)
{
	std::string host = getHeader("Host");
	this->_serverName = host.substr(0, host.find(':'));
}

void RequestParser::parseChunkedBody(std::istringstream& iss)
{  
    std::string chunkSizeLine;
    std::string chunk;
    size_t chunkSize;

    while (std::getline(iss, chunkSizeLine))
    {
        std::stringstream chunkSizeStream(chunkSizeLine);
        chunkSizeStream >> std::hex >> chunkSize;
        if (chunkSize == 0)
        {
            break;
        }
        chunk.resize(chunkSize);
        iss.read(&chunk[0], chunkSize);
        std::string crlf;
        std::getline(iss, crlf);
        _requestBody += chunk;
    }
}

void RequestParser::_clean_header(std::string &temp_line)
{
	std::string header;
	header = temp_line.substr(0, temp_line.find(CRLF_DOUBLE));
	temp_line.erase(0, header.length() + strlen(CRLF_DOUBLE));
}

void RequestParser::_clean_footer(std::string &temp_line)
{
	std::string footer;
	footer = temp_line.substr(temp_line.rfind(CRLF), temp_line.npos);
	temp_line.erase(temp_line.length() - footer.length(), temp_line.npos);
}


void	receive_line(int fd, std::string &line, std::string delimiter)
{
	char		buffer[2] = {0};
	ssize_t		num_of_bytes;	
	std::string temp_line;

	while (true)
	{
		num_of_bytes = recv(fd, buffer, 1, 0);
		if (num_of_bytes == -1)
			return;
		if (num_of_bytes == 0)
			break ;
		temp_line += buffer;
        if (has_delimiter(temp_line, delimiter))
            break ;
	}
	line = temp_line;
    if (has_delimiter(temp_line, delimiter))
		line.resize(line.rfind(delimiter));
}

void RequestParser::parseRequestBody(int fdFon)
{
    (void)fdFon;

        // std::cout <<  "ola" << line << std::endl;

    // (void)line;
    // std::cout << std::endl <<  "parseRequestBody antes:" << _requestBody.length() << std::endl;

    // std::string transferEncoding = getHeader("Transfer-Encoding");
    // if (transferEncoding == "chunked")
    // {
    //     parseChunkedBody(iss);
    // }

    // if (!getHeader("Content-Type").empty() && getHeader("Content-Type").find("multipart/form-data") != std::string::npos)
    // {   
    //     // setFileName(iss);
    //     size_t pos = getHeader("Content-Type").find("boundary=", 0);
    //     if (pos != std::string::npos)
    //     {
    //         _clean_header(line);
	// 	    _clean_footer(line);

    //     }
    // }
    // _requestBody = line;
    // else if (!getHeader("Content-Length").empty())
    // {
    //     parseContentLengthBody(iss);
    //     std::cout << std::endl <<  "_requestBody antes:" << _requestBody.length() << std::endl;

    // }
}

void 	RequestParser::parseRequestHeader( std::string &line, std::istringstream &iss )
{
    (void)iss;
        (void)line;

    std::string line2;

    while (true)
    {
        receive_line(_fdClient, line2, CRLF);

    if (line2 == CRLF || line2.empty())
    {
        // Fim dos cabeçalhos
            std::cout << "break = " << line2 << std::endl;; 

        break;
    }
     else
        {
            size_t colonPos = line2.find(':');
            if (colonPos != std::string::npos)
            {
                size_t lastNonCRLF = line2.find_last_not_of("\r\n");
                if (lastNonCRLF != std::string::npos) 
                {
                    line2 = line2.substr(0, lastNonCRLF + 1);
                    std::string headerName = line2.substr(0, colonPos);
                    std::string headerValue = line2.substr(colonPos + 2);
                    this->_headers[headerName] = headerValue;
                                this->_request += line2 + "\n";

                }
            }
        }
    }
}

void	RequestParser::parseRequestStartLine(std::string &line, std::istringstream &iss)
{
	if (std::getline(iss, line))
	{
        std::istringstream lineStream(line);
        lineStream >> this->_method >> _path >> this->_httpVersion;
        this->_request += line +  "\n";
        std::cout <<  "starLine = "<< line << std::endl;

    }
}

std::string RequestParser::getMethod(void) const
{
    return this->_method;
}

std::string RequestParser::getPath(void) const
{
    return this->_path;
}

std::string RequestParser::getHttpVersion(void) const
{
    return this->_httpVersion;
}

std::string RequestParser::getBody(void) const
{
    return this->_requestBody;
}

std::string RequestParser::getPort(void) const
{
    return this->_port;
}

int RequestParser::getPortNumber(void) const
{
    return this->_portNumber;
}

std::string RequestParser::getHeader(std::string headerName) const
{
    // std::cout <<  "header name = " << _headers[headerName] << std::endl;
    std::map<std::string, std::string>::const_iterator it = this->_headers.find(headerName);

    if (it != this->_headers.end())
    {
        return it->second;
    }
    else 
    {
        return "nao deu";
    }
}

std::string RequestParser::getQuery(void) const
{
    return this->_query;
}

std::string RequestParser::getServerName(void) const
{
    return this->_serverName;;
}

std::string RequestParser::getFileName(void) const
{
    return this->_fileName;
}


std::string RequestParser::getRequest(void) const
{
    return this->_request;
}

void RequestParser::setPath(std::string newPath)
{
    this->_path = newPath;
}

void RequestParser::setBody(std::string newBody)
{
    this->_requestBody = newBody;
}

std::string RequestParser::getFileExec(void) const
{
    return this->_fileExec;
}

void RequestParser::setFileExec(std::string fileExec)
{
    this->_fileExec = fileExec;
}

// void       RequestParser::setFileName( void )
// {
//     std::string fileName = _requestBody;
//     size_t filenamePos = fileName.find("filename=\"");
//     if (filenamePos != std::string::npos) 
//     {
//         fileName = fileName.substr(filenamePos + 10);
//         fileName = fileName.substr(0, fileName.find("\""));
//     }
//     _fileName = fileName;
// }
void RequestParser::setFileName(std::istringstream& iss)
{
    std::string fileName;
    size_t contentLength = getContentLength();
    char buffer;
    size_t bytesRead = 0;

    while (contentLength > 0 && bytesRead < contentLength && iss.get(buffer))
    {
        fileName += buffer;
        bytesRead++;

        if (fileName.find("filename=\"") != std::string::npos)
        {
            size_t filenameEndPos = fileName.find("\"", fileName.find("filename=\"") + 10);
            if (filenameEndPos != std::string::npos)
            {
                _fileName = fileName.substr(fileName.find("filename=\"") + 10, filenameEndPos - (fileName.find("filename=\"") + 10));
                return;
            }
        }
    }
}


int RequestParser::getContentLength() const
{
    std::string contentLengthStr = getHeader("Content-Length");
    
    if (!contentLengthStr.empty())
    {
        std::cout <<  "tamanho = " << atol(contentLengthStr.c_str()) <<std::endl;
        return atol(contentLengthStr.c_str());
    }
    return 0;
}