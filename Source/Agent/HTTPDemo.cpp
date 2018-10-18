// http_request.cpp : 定义控制台应用程序的入口点。
//

#include "HTTPRequest.hpp"
#include "HTTPHelper.h"

#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <cstdlib>
#include <string.h>

class DownCallbackClass
{
public:
        DownCallbackClass() :m_down_finished(false) {}
        ~DownCallbackClass() {}
public:
        void DownResultCallback(int id, bool success, const std::string& error_string)
        {
                if(!success)
                {
                        std::cerr << "download error:" << error_string.c_str() << std::endl;
                }
                m_down_finished = true;
        }
        int down_callback(double total_size, double downloaded_size, void* userdata)
        {
                long tmp = static_cast<long>(downloaded_size / total_size * 100);
                printf("\rdownload progress:%ld%%\n", tmp);
                return 0;
        }
        bool IsDownFinished(void) { return m_down_finished;  }
private:
        bool m_down_finished;
};

class MyResultClass
{
public:
        MyResultClass() : m_request_finished(false) { }
        ~MyResultClass() { }

public:
        void MyRequestResultCallback(int id, bool success, const std::string& data)
        {
                if (success)
                {
                        std::ofstream outfile;
                        outfile.open("baidu.html", std::ios_base::binary | std::ios_base::trunc);
                        if (outfile.good()) outfile.write(data.c_str(), data.size());
                }
                m_request_finished = true;
        }
        bool IsRequestFinish(void) { return m_request_finished;  }
private:
        bool m_request_finished;
};

static void TestRequest();

extern "C"
{
	extern HttpRequest* exp_CreateHttpRequest(const char* url = NULL);
    extern void exp_DestroyHttpRequest(HttpRequest*& request);
	extern int exp_SetRequestUrl(HttpRequest * request,const char* url);
	extern int exp_SetRequestHeader(HttpRequest * request,const char* header);
#ifdef _WIN32
	extern int exp_SetRequestResultCallback(HttpRequest * request,void (__stdcall* Request_Callback)(int,bool,const char*,size_t));
#else
        extern int exp_SetRequestResultCallback(HttpRequest * request,void (* Request_Callback)(int,bool,const char*,size_t));
#endif
	extern void* exp_PerformRequest(HttpRequest * request,int type);
	extern int exp_GetRequestId(HttpRequest * request);
};

static bool bFinished = false;
#ifdef _WIN32
static __stdcall void On_Agent_Request_Callback(int id, bool success, const char* data,size_t len)
#else
static void On_Agent_Request_Callback(int id, bool success, const char* data, size_t len)
#endif
{
	if (success && data)
	{
		std::ofstream outfile;
		outfile.open("baidu.html", std::ios_base::binary | std::ios_base::trunc);
		if (outfile.good()) outfile.write(data, len);
	}
	bFinished = true;
}

static void TestRequest()
{
        printf(">>>>>>>>>>>>>TestRequest\n");
	HttpRequest* request = exp_CreateHttpRequest("http://www.qq.com");

	exp_SetRequestResultCallback(request, On_Agent_Request_Callback);
	exp_SetRequestHeader(request, "User-Agent:Mozilla/4.04[en](Win95;I;Nav)");

        printf("request-id:%d\n",exp_GetRequestId(request));

	H_HTTPHANDLE hRequest  = (H_HTTPHANDLE)exp_PerformRequest(request, 1);
	if (hRequest)
	{
		while (bFinished == false) h_Sleep(300);
		long http_code;
		if (request->GetHttpCode(hRequest, &http_code))
                        printf("http code:%ld\n",http_code);

		std::string header;
		if (request->GetReceiveHeader(hRequest, &header))
		{
                        printf("%s\n",header.c_str());
		}

		HttpRequest::Close(hRequest);
	}
    exp_DestroyHttpRequest(request);
}

void TestDownload()
{
        printf(">>>>>>>>>>>>TestDownload\n");
        HttpDownloader download;
        DownCallbackClass dc;
        const char* down_url = "https://www.baidu.com";
        const char* down_file = "./BaiduPlayer.exe";
        printf("download %s\n", down_url);
        download.SetDownloadUrl(down_url);
        download.SetProgressCallback(std::bind(&DownCallbackClass::down_callback, &dc, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        download.SetResultCallback(std::bind(&DownCallbackClass::DownResultCallback, &dc, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        download.DownloadFile(down_file);
        //download.SetTimeout(3);
        H_HTTPHANDLE hDownload = download.StartDownload(HttpDownloader::DOWN_ASYNC);
        if (hDownload)
        {
                while (dc.IsDownFinished() == false)
                {
                        h_Sleep(300);
                }
                //to do download finish clean up
                HttpDownloader::Close(hDownload);
        }
}

#ifdef _WIN32
static __stdcall void On_HTTP_Request_Callback(bool success, const std::string& data, void* userdata)
#else
static void On_HTTP_Request_Callback(bool success, const std::string& data, void* userdata)
#endif
{
    if (success)
    {
        std::ofstream outfile;
        outfile.open("baidu.html", std::ios_base::binary | std::ios_base::trunc);
        if (outfile.good()) outfile.write(data.c_str(), data.size());
    }
    else
        printf("error:%s\n", data.c_str());
}

class MyRequestHandle : public IRequestHandle
{
public:
    ~MyRequestHandle()
    {
        printf("~MyRequestHandle\n");
    }
    void OnRequestFinished(bool success, const std::string& content, void* userdata)
    {
        if(!success)
        {
            std::cerr << "download error:" << content.c_str() << std::endl;
        }
        else
        {
            std::cout << "Download Finished!" << std::endl;
        }
    }
    int  OnProgress(double total_size, double handle_size, void* userdata)
    {
        long tmp = static_cast<long>(handle_size / total_size * 100);
        printf("\rdownload progress:%ld%%\n", tmp);
        
        return 0;
    }
};

void TestHTTPRequest()
{
    auto v = HTTPRequest::Create(true);
    HTTPRequest& http = *v;
    http.SetUrl("https://www.baidu.com");
    http.SetSSLVerify(true);
    http.SetTimeout(5000);
    http.SetResultCallback(On_HTTP_Request_Callback);
    http.SetRequestHeader("User-Agent:Mozilla/4.04[en](Win95;I;Nav)");
    http.Run(false, true);
    
    //HTTPRequest::Tick(0);
}
void TestHTTPRequestDownload()
{
    auto v = HTTPRequest::Create(true);
    HTTPRequest& http = *v;
    static MyRequestHandle dc;
    http.SetUrl("https://curl.haxx.saae/download/curl-7.61.1.tar.gz");
    http.SetSSLVerify(true);
    http.SetTimeout(50000);
    const char* down_file = "./cur3.tar.gz";
    http.SetRequestHandle(&dc);
    http.SetDownloadPath(down_file);
    http.Run(true, true);
}

int main(int argc, char* argv[])
{
        //TestRequest();

        //TestDownload();
    
   // TestHTTPRequest();
    TestHTTPRequestDownload();
    
    HTTPRequest::Tick(0);
    
        return 0;
}
