#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <curl/curl.h>
#include "picosha2.h"

// Create a vector to hold the headers
std::vector<std::string> headers;

// This function will be called by libcurl for each header
static size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
	// What we get is not null-terminated, so we might want to copy it somewhere and null-terminate it ourselves.
	std::string header(buffer, size * nitems);

	// Find the separator between header name and value
	size_t separator = header.find(":");
	if (separator != std::string::npos)	{
		// Extract header name
		std::string name = header.substr(0, separator);
		// Insert it into the vector
		headers.push_back(name);
	}

	// Return the number of bytes processed, or curl will consider it an error.
	return size * nitems;
}

// Function to handle response body
size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *s) {
	size_t newLength = size * nmemb;
	size_t oldLength = s->size();
	try	{
		s->resize(oldLength + newLength);
	}	catch (std::bad_alloc &e)	{
		return 0;
	}

	std::copy((char *)contents, (char *)contents + newLength, s->begin() + oldLength);
	return newLength;
}

template <class S, class T> std::string join(std::vector<T> &elems, S &delim) {
	std::stringstream ss;
	typename std::vector<T>::iterator e = elems.begin();
	ss << *e++;
	for (; e != elems.end(); ++e)	{
		ss << delim << *e;
	}
	return ss.str();
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <url>" << std::endl;
		return 1;
	}

	// Process the command-line argument
	std::string url = argv[1];

	CURL *curl = curl_easy_init();
	std::string response;

	if (curl)	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		// Set the header output option
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);

		// Register the write function
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

		// Set write function to handle response body
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		// Perform the request
		CURLcode res = curl_easy_perform(curl);

		// Always cleanup
		curl_easy_cleanup(curl);
	}

	// Compute SHA-256
	std::string hash = picosha2::hash256_hex_string(join(headers, ":"));

	// Print the hash
	std::cout << "hhh:1:" << hash << std::endl;

	return 0;
}
