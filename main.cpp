#define CURL_STATICLIB
#include <curl\curl.h>

int main()
{
    CURL* curl;

    curl = curl_easy_init();
    curl_easy_cleanup(curl);

    return 0;
}
