#include <stdio.h>
// #include <string.h>
// #include <string>
#include <vector>
#include <map>
#include <ArduinoJson.h>
#include "pico/stdlib.h"
#include "httpd/fs.h"
#include "lwip/apps/httpd.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "httpd/fscustom.h"
#include "httpd/fsdata.h"
#include "httpd/httpd.h"
#include "rndis/rndis.h"
#include "webserver.h"

#define API_GET_TEST_DATA "/api/getTestData"
#define API_SET_TEST_DATA "/api/setTestData"

#define LWIP_HTTPD_POST_MAX_URI_LEN 128
#define LWIP_HTTPD_POST_MAX_PAYLOAD_LEN 2048

using namespace std;

extern struct fsdata_file file__index_html[];

static vector<string> spaPaths = { "/home" };
static vector<string> excludePaths = { "/css", "/images", "/js", "/static" };
static char *http_post_uri;
static char http_post_payload[LWIP_HTTPD_POST_MAX_PAYLOAD_LEN];
static uint16_t http_post_payload_len = 0;
static bool is_post = false;

void webserver()
{
	printf("[NET] BEGIN\n");
	rndis_init();
	while (1)
	{
		rndis_task();
	}
}

/*************************
 * Helper methods
 *************************/

DynamicJsonDocument get_post_data()
{
	vector<char> raw;
	for (int i = 0; i < http_post_payload_len; i++)
		raw.push_back(http_post_payload[i]);

	DynamicJsonDocument doc(LWIP_HTTPD_POST_MAX_PAYLOAD_LEN);
	deserializeJson(doc, raw);
	return doc;
}

inline string serialize_json(DynamicJsonDocument &doc)
{
	string data;
	serializeJson(doc, data);
	return data;
}

int set_file_data(struct fs_file *file, string data)
{
	static string returnData;

	returnData = data;
	file->data = returnData.c_str();
	file->len = returnData.size();
	file->index = file->len;
	file->http_header_included = 0;
	file->pextension = NULL;

	return 1;
}


/*************************
 * API methods
 *************************/

string getTestData()
{
	DynamicJsonDocument doc(LWIP_HTTPD_POST_MAX_PAYLOAD_LEN);

	doc.clear();
	doc["testString"]  = "test1";
	doc["testNumber"] = 1.35F;
	auto testArray = doc.createNestedArray("testArray");
	testArray.add("child1");
	testArray.add("child2");
	testArray.add("child3");

	return serialize_json(doc);
}

string setTestData()
{
	DynamicJsonDocument doc = get_post_data();

	// string testString = doc["testString"];
	// float testNumber = doc["testNumber"];
	// auto testArray = doc["testArray"].as<JsonArray>();
	// for (auto item : testArray) {
	// 	// auto itemString = item.as<string>();
	// }

	return serialize_json(doc);
}

/*************************
 * LWIP implementation
 *************************/

// LWIP callback on HTTP POST to validate the URI
err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       uint16_t http_request_len, int content_len, char *response_uri,
                       uint16_t response_uri_len, uint8_t *post_auto_wnd)
{
	LWIP_UNUSED_ARG(http_request);
	LWIP_UNUSED_ARG(http_request_len);
	LWIP_UNUSED_ARG(content_len);
	LWIP_UNUSED_ARG(response_uri);
	LWIP_UNUSED_ARG(response_uri_len);

	// Only allow POST requests to the api
	
	struct http_state *hs = (struct http_state *)connection;
	if (!uri || (uri[0] == '\0') || memcmp(uri, "/api", 4))
		return ERR_ARG;

	http_post_uri = (char *)uri;
	*post_auto_wnd = 1;
	is_post = true;
	return ERR_OK;
}

// LWIP callback on HTTP POST to for receiving payload
err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
	LWIP_UNUSED_ARG(connection);

	int count;
	uint32_t http_post_payload_full_flag = 0;

	// Cache the received data to http_post_payload
	http_post_payload_len = 0;
	memset(http_post_payload, 0, LWIP_HTTPD_POST_MAX_PAYLOAD_LEN);
	while (p != NULL)
	{
		if (http_post_payload_len + p->len <= LWIP_HTTPD_POST_MAX_PAYLOAD_LEN)
		{
			MEMCPY(http_post_payload + http_post_payload_len, p->payload, p->len);
			http_post_payload_len += p->len;
		}
		else // Buffer overflow Set overflow flag
		{
			http_post_payload_full_flag = 1;
			break;
		}

		p = p->next;
	}

	// Need to release memory here or will leak
	pbuf_free(p);

	// If the buffer overflows, error out
	if (http_post_payload_full_flag)
		return ERR_BUF;

	return ERR_OK;
}

// LWIP callback to set the HTTP POST response_uri, which can then be looked up via the fs_custom callbacks
void httpd_post_finished(void *connection, char *response_uri, uint16_t response_uri_len)
{
	LWIP_UNUSED_ARG(connection);
	LWIP_UNUSED_ARG(response_uri);
	LWIP_UNUSED_ARG(response_uri_len);

	response_uri = http_post_uri;
}

int fs_open_custom(struct fs_file *file, const char *name)
{
	// Handle the request based on name/path
	if (is_post)
	{
		if (!memcmp(http_post_uri, API_SET_TEST_DATA, sizeof(API_SET_TEST_DATA)))
			return set_file_data(file, setTestData());
 	}
	else
	{
		if (!memcmp(name, API_GET_TEST_DATA, sizeof(API_GET_TEST_DATA)))
			return set_file_data(file, getTestData());
	}

	bool isExclude = false;
	for (auto &excludePath : excludePaths)
		if (!excludePath.compare(name))
			return 0;

	for (auto &spaPath : spaPaths)
	{
		if (!spaPath.compare(name))
		{
			file->data = (const char *)file__index_html[0].data;
			file->len = file__index_html[0].len;
			file->index = file__index_html[0].len;
			file->http_header_included = file__index_html[0].http_header_included;
			file->pextension = NULL;
			return 1;
		}
	}

	return 0;
}

void fs_close_custom(struct fs_file *file)
{
	if (file && file->is_custom_file && file->pextension)
	{
		mem_free(file->pextension);
		file->pextension = NULL;
	}

	is_post = false;
}
