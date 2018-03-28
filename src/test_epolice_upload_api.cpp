#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include "../inc/cJSON.h"
#include "../inc/curl/curl.h"


#define E_FAIL -1
#define E_OK 0

#define FILENAME "curlposttest.log"
#define  MAXSIZE 2048


std::string strsessionid("");
/*
 *	
 { 
 "loginname":"carupload",
 "password":"admin", 
 "key":"",
 "logintype":"0"
 }


 172.18.10.125:7899
 */

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

static size_t upload_back_data(void *buffer, size_t size, size_t nmemb, void *userp);



int login(cJSON *root, std::string url)
{
	// 构造http信息
	CURL *curl = NULL;             //定义CURL类型的指针

	//std::stringstream out;

//	FILE *fptr;  

// 	if ((fptr = fopen(FILENAME, "w")) == NULL) {  
// 		fprintf(stderr, "fopen file error: %s\n", FILENAME);  
// 		exit(1);  
// 	}  

	char strbuf[MAXSIZE] = {0};

	//init to NULL is important
	struct curl_slist *headers=NULL; 

	//headers = curl_slist_append(headers, "POST / HTTP1.1"); 
	//headers = curl_slist_append(headers, "Content-Type: application/json;charset=utf-8"); 
	 //headers = curl_slist_append(headers, "Accept:application/json");
     headers = curl_slist_append(headers, "Content-Type:application/json");
     headers = curl_slist_append(headers, "charset:utf-8");


	//定义CURLcode类型的变量，保存返回状态码
	CURLcode res = CURLE_OK;

	//初始化一个CURL类型的指针
	curl = curl_easy_init();        
	if(curl!=NULL)
	{
		//设置curl选项. 其中CURLOPT_URL是让用户指定url
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); 

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		//char  *cJSON_Print(cJSON *item);

		printf("%s..\n", cJSON_Print(root));

		// 设置要POST的JSON数据
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cJSON_Print(root));
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, );

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, upload_back_data);  
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, strbuf);

		curl_easy_setopt(curl, CURLOPT_POST, 1);

		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {  
			//curl_easy_strerror进行出错打印  
			//printf("");
			printf("login failed..\n");
			curl_easy_strerror(res);  
		}
		else
		{
			printf("login success..\n");
		//	printf("strbuf: %s\n", strbuf);
		}


		std::string strresult(strbuf);
		std::string::size_type pos = strresult.find("{");

		std::string strjson = strresult.substr(pos);

		cJSON *jsonresult = NULL;
		cJSON *jsondata = NULL;
		jsonresult = cJSON_Parse(strjson.c_str());

		if (NULL == jsonresult)
		{
			printf("parse failed. \n");
		}
		else
		{
			// cjson item data
			cJSON *jsondataiterm = cJSON_GetObjectItem(jsonresult, "data");

			if (NULL == jsondataiterm)
			{
				printf("parse failed..\n");
				goto OUT;
			}
			
			//char *strdata = jsondataiterm->valuestring;
			jsondata = cJSON_Parse(jsondataiterm->valuestring);
			if (NULL == jsondata)
			{
				printf("parse json data failed...\n");
				goto OUT;
			}

			//printf("\n data json: %s\n", cJSON_Print(jsondata));

			cJSON *sessionid = cJSON_GetObjectItem(jsondata, "sessionid");
			if (NULL == sessionid)
			{
				printf("get sessionid failed. \n");
				goto OUT;
			}
			else
			{
				printf("sessionid: %s\n", sessionid->valuestring);
				strsessionid = sessionid->valuestring;
			}
		}

OUT:
		//清除curl操作.
		curl_easy_cleanup(curl);
		if (jsonresult != NULL)
		{
			cJSON_Delete(jsonresult);
		}

		if (jsondata != NULL)
		{
			cJSON_Delete(jsondata);
		}
	}

	curl_slist_free_all(headers);

	return E_OK;
}


static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) 
{  
	FILE *fptr = (FILE*)userp;  
	fwrite(buffer, size, nmemb, fptr); 

	return size*nmemb;
} 

static size_t upload_back_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	char *str = (char *)userp;
	if (strlen(str) + strlen((char *)buffer) > MAXSIZE)
	{
		printf("buffer cache is not enough. \n");
		return 0;
	}
	else
	{
		strcat(str, (char *)buffer);
	}


	return size * nmemb;
}
int logout(std::string url)
{
	// 构造http信息
	CURL *curl = NULL;             //定义CURL类型的指针

	char strbuf[MAXSIZE] = {0};

	//init to NULL is important
	struct curl_slist *headers=NULL; 

	//headers = curl_slist_append(headers, strcookie.c_str());
	headers = curl_slist_append(headers, "Content-Type:application/json");
	headers = curl_slist_append(headers, "charset:utf-8");

	//定义CURLcode类型的变量，保存返回状态码
	CURLcode res = CURLE_OK;

	//初始化一个CURL类型的指针
	curl = curl_easy_init();        
	if(curl!=NULL)
	{
		//设置curl选项. 其中CURLOPT_URL是让用户指定url
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); 

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// 设置要POST的JSON数据
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, upload_back_data);  
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, strbuf);

		curl_easy_setopt(curl, CURLOPT_POST, 1);

		curl_easy_setopt(curl, CURLOPT_HEADER, 1);

		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, strsessionid.c_str());

		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);

		printf("1111111111111\n");

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) 
		{  
			//curl_easy_strerror进行出错打印  
			printf("logout failed..%s\n", curl_easy_strerror(res));
		}
		else
		{
			printf("logout success..\n");
			printf("logout: %s.\n", strbuf);
		}
	}

	//清除curl操作.
	curl_easy_cleanup(curl);

	curl_slist_free_all(headers);

	return E_OK;
}


int QueryNetStatus(std::string url)
{
	CURL *curl = NULL;             //定义CURL类型的指针

	char strbuf[MAXSIZE] = {0};

	//init to NULL is important
	struct curl_slist *headers=NULL; 

	headers = curl_slist_append(headers, "Content-Type:application/json");
	headers = curl_slist_append(headers, "charset:utf-8");

	//定义CURLcode类型的变量，保存返回状态码
	CURLcode res = CURLE_OK;

	//初始化一个CURL类型的指针
	curl = curl_easy_init();        
	if(curl!=NULL)
	{
		//设置curl选项. 其中CURLOPT_URL是让用户指定url
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); 

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// 设置要POST的JSON数据
		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, upload_back_data);  
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, strbuf);

		curl_easy_setopt(curl, CURLOPT_POST, 1);

		//curl_easy_setopt(curl, CURLOPT_HEADER, 1);

		printf("1111111111111\n");

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) 
		{  
			//curl_easy_strerror进行出错打印  
			printf("QueryNetStatus failed..%s\n", curl_easy_strerror(res));
		}
		else
		{
			printf("QueryNetStatus success..\n");
			printf("QueryNetStatus: %s.\n", strbuf);
		}
	}

	//清除curl操作.
	curl_easy_cleanup(curl);

	curl_slist_free_all(headers);


	return E_OK;
}


int create_userinfo(cJSON *root)
{
	cJSON_AddItemToObject(root, "loginname", cJSON_CreateString("carupload"));
	cJSON_AddItemToObject(root, "password", cJSON_CreateString("admin"));
	cJSON_AddItemToObject(root, "key", cJSON_CreateString(""));
	cJSON_AddItemToObject(root, "logintype", cJSON_CreateString("0"));

	return E_OK;
}

void testcjosn()
{
	char *str = "{\"errorcode\":\"0\",\"message\":\"执行成功\"}";

	cJSON *jsonresult = cJSON_Parse(str);
	if (NULL == jsonresult)
	{
		printf("parse failed. \n");
	}
	else
	{
		printf("\n");
		printf("%s\n", cJSON_Print(jsonresult));
	}

	return;
}

int main(int argc, char **argv)
{
	// 
	cJSON *userinfo = cJSON_CreateObject();
	create_userinfo(userinfo);
	
	///////http://172.18.20.125:7899/policeplatformweb/LoginManage/Login
	////172.18.10.17:8080
	//std::string str_login_url("http://172.18.20.125:7899/policeplatformweb/LoginManage/Login");
	//login(userinfo, str_login_url);

	//sleep(5);

	//std::string str_logout_url("http://172.18.20.125:7899/policeplatformweb/LoginManage/Logout");
	//logout(str_logout_url);
	std::string str_QueryNetStatus_url("http://172.18.20.125:7899/policeplatformweb/NetStatusManage/QueryNetStatus");
	QueryNetStatus(str_QueryNetStatus_url);

	return 0;
}