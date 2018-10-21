
#include <list>
#include <regex>
#include <sstream>
#include <exception>
#include "curl/curl.h"        //libcurl interface
#include "HTTPHelper.h"

class HTTPImpl;

class HTTPLock
{
public:
#ifdef _WIN32
    HTTPLock() { InitializeCriticalSection(&m_cs); }
    ~HTTPLock() { DeleteCriticalSection(&m_cs); }
    
    void Lock() { EnterCriticalSection(&m_cs); }
    void UnLock() { LeaveCriticalSection(&m_cs); }
#else
    HTTPLock() { pthread_mutex_init(&m_lock, NULL); }
    ~HTTPLock() { pthread_mutex_destroy(&m_lock); }
    
    int Lock(){ return pthread_mutex_lock(&m_lock); }
    int UnLock() { return pthread_mutex_unlock(&m_lock); }
#endif
    
private:
#ifdef _WIN32
    CRITICAL_SECTION m_cs;
#else
    pthread_mutex_t    m_lock;
#endif
};

class DoHTTPLock
{
public:
    DoHTTPLock(std::shared_ptr<HTTPLock> & lock)
    : m_lock(lock)
    {
        m_lock->Lock();
    }
    
    ~DoHTTPLock()
    {
        m_lock->UnLock();
    }
    
private:
    std::shared_ptr<HTTPLock> m_lock;
};

class HTTPHelper
{
private:
    std::list<HTTPRequest* >        s_async_requests;
    std::shared_ptr<HTTPLock>       s_request_lock;
    CURLSH*                         s_share_handle;
    static int s_id;
    
    HTTPHelper()
    : s_share_handle(nullptr)
    , s_request_lock(new HTTPLock)
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        
        s_share_handle = curl_share_init();
        curl_share_setopt(s_share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
    }
public:
    ~HTTPHelper()
    {
        s_request_lock->Lock();
        s_async_requests.clear();
        s_request_lock->UnLock();
        
        curl_share_cleanup(s_share_handle);
        curl_global_cleanup();
        s_share_handle = nullptr;
    }
    static HTTPHelper* Instance()
    {
        static HTTPHelper the_single_instance;
        return &the_single_instance;
    }
    
    static void h_Sleep(unsigned int dt)
    {
#ifdef _WIN32
        ::Sleep(dt);
#else
        usleep(dt * 1000);
#endif
    }
    
    CURLSH* GetSharedCurl()
    {
        return s_share_handle;
    }
    int  GetID()
    {
        s_id++;
        return s_id;
    }
    bool FindRequest(HTTPRequest* request)
    {
        s_request_lock->Lock();
        for(HTTPRequest* v: s_async_requests)
        {
            if(v == request)
            {
                s_request_lock->UnLock();
                return true;
            }
        }
        s_request_lock->UnLock();
        return false;
    }
    void RemoveRequest(HTTPRequest*& request)
    {
        s_request_lock->Lock();
        s_async_requests.remove(request);
        s_request_lock->UnLock();
        if(request->IsAutoDestroy())
        {
            HTTPRequest::Destroy(request);
        }
    }
    void AddRequest(HTTPRequest* request)
    {
        s_request_lock->Lock();
        s_async_requests.push_back(request);
        s_request_lock->UnLock();
    }
    
    void Tick(float dt)
    {
        bool empty = false;
        while(!empty)
        {
            s_request_lock->Lock();
            empty = s_async_requests.empty();
            s_request_lock->UnLock();
            h_Sleep(200);
        }
    }
};

int HTTPHelper::s_id = 0;

class HTTPImpl
{
private:
    HTTPRequest*       m_owner;
    H_HTTPHANDLE       m_share_handle;
    H_HTTPHANDLE       m_http_headers;
#ifdef _WIN32
    H_HTTPHANDLE       m_perform_thread;
#else
    pthread_t          m_perform_thread;
#endif
    
#ifdef _WIN32
    std::list<H_HTTPHANDLE> m_download_threads;
#else
    std::list<pthread_t>    m_download_threads;
#endif
    
    int                m_id;
    int                m_retry_times;
    long               m_time_out;
    bool               m_is_running;
    bool               m_is_download;
    long               m_http_code;
    double             m_downloaded_size;
    double             m_total_size;
    size_t             m_thread_count;
    bool               m_multi_download;
    bool               m_download_fail;
    bool               m_is_cancel;
    
    std::string        m_url;
    std::string        m_download_path;
    std::string        m_receive_content;
    std::string        m_receive_header;
    std::string        m_error_string;
    char*              m_post_data;
    size_t             m_post_data_size;
    void*              m_userdata;
    
    ResultCallback     m_result_callback;
    ProgressCallback   m_progress_callback;
    IRequestHandle*    m_request_handle;
    std::shared_ptr<HTTPLock> m_download_lock;
    
    //
    bool               m_enable_ssl;
    bool               m_enable_redirect;
    bool               m_use_post;
    std::string        m_headers;
    std::string        m_proxy;
    long               m_proxy_port;
    
    typedef struct tDownloadThreadChunk
    {
        static size_t counter;
        size_t      _id;
        FILE*       _fp;
        long        _startidx;
        long        _endidx;
        
        HTTPImpl*     _download;
        
        tDownloadThreadChunk():_fp(nullptr), _download(nullptr)
        {
            _id = ++counter;
        }
    }DownloadThreadChunk;

public:
    HTTPImpl(int id, HTTPRequest* owner, H_HTTPHANDLE share_handle = nullptr)
    : m_id(id)
    , m_owner(owner)
    , m_share_handle(share_handle)
    , m_http_headers(nullptr)
#ifdef _WIN32
    , m_perform_thread(nullptr)
#else
    , m_perform_thread(pthread_t())
#endif
    , m_is_running(false)
    , m_retry_times(HTTPRequest::s_kRetryCount)
    , m_time_out(HTTPRequest::s_kTimeout)
    , m_http_code(0)
    , m_post_data(nullptr)
    , m_post_data_size(0)
    , m_result_callback(nullptr)
    , m_progress_callback(nullptr)
    , m_request_handle(nullptr)
    , m_userdata(nullptr)
    , m_is_download(false)
    , m_downloaded_size(0.0)
    , m_total_size(0.0)
    , m_thread_count(HTTPRequest::s_kThreadCount)
    , m_multi_download(false)
    , m_download_fail(false)
    , m_is_cancel(false)
    , m_download_lock(nullptr)
    
    , m_enable_ssl(false)
    , m_enable_redirect(true)
    , m_use_post(false)
    , m_proxy_port(0)
    {
    }
    ~HTTPImpl()
    {
        if(m_request_handle)
        {
            m_request_handle->OnDelete(m_owner);
        }
        Release();
        printf("~~HTTPImpl:%d\n", m_id);
    }
    void Release()
    {
        curl_slist* clist = static_cast<curl_slist*>(m_http_headers);
        if(clist)
            curl_slist_free_all(clist);
        m_http_headers = nullptr;
        
        delete m_post_data;
        m_post_data = nullptr;
        
        Close();
        
        m_owner = nullptr;
        m_is_running = false;
    }
    int GetRequestId()
    {
        return m_id;
    }
    void SetRetryTimes(int retry_times)
    {
        m_retry_times = retry_times;
    }
    void SetTimeout(long time_out)
    {
        m_time_out = time_out;
    }
    void SetSSLVerify(bool enable)
    {
        m_enable_ssl = enable;
    }
    void SetUrl(const std::string& url)
    {
        m_url = url;
    }
    void SetMovedUrl(bool get_moved_url)
    {
        m_enable_redirect = get_moved_url;
    }
    bool SetDownloadMode(bool is_download)
    {
        if(m_is_running) return false;
        m_is_download = is_download;
        return true;
    }
    void SetDownloadPath(const std::string& path)
    {
        m_download_path = path;
    }
    void SetDownloadThreadCount(size_t count)
    {
        m_thread_count = count;
    }
    void SetUserData(void* userdata)
    {
        m_userdata = userdata;
    }
    void* GetUserData()
    {
        return m_userdata;
    }

    void SetProgressCallback(ProgressCallback pc)
    {
        m_progress_callback = pc;
    }
    void SetResultCallback(ResultCallback rc)
    {
        m_result_callback = rc;
    }
    void SetRequestHandle(IRequestHandle* handle)
    {
        m_request_handle = handle;
    }
    void SetPostMethod(bool post)
    {
        m_use_post = post;
    }
    void SetPostData(const void* data, size_t size)
    {
        m_post_data_size = size;
        if (m_post_data)
        {
            delete m_post_data;
            m_post_data = nullptr;
        }
        if(size)
        {
            m_post_data = new char[size+1];
            m_post_data[size] = 0x0;
            memcpy(m_post_data, data, size);
        }
    }
    
    void SetRequestHeader(const std::string& header)
    {
        m_headers = header;
    }
    
    void SetRequestProxy(const std::string& proxy, long proxy_port)
    {
        m_proxy = proxy;
        m_proxy_port = proxy_port;
    }

    bool    GetHeader(std::string* header)
    {
        if (m_receive_header.empty()) return false;
        else if (header) *header = m_receive_header;
        
        return true;
    }
    bool     GetContent(std::string* receive)
    {
        if (m_receive_content.empty()) return false;
        else if (receive) *receive = m_receive_content;
        
        return true;
    }
    bool     GetErrorString(std::string* error_string)
    {
        if (m_error_string.empty()) return false;
        else if (error_string) *error_string = m_error_string;
        
        return true;
    }
    
    long     GetHttpCode() { return m_http_code; }
    
public:
    int Run(bool async)
    {
        if (m_is_running)
        {
            return 0;
        }
        
        HTTPHelper::Instance()->AddRequest(m_owner);
        
        if (!async)
        {
            return Perform();
        }
        else
        {
#ifdef _WIN32
            DWORD thread_id;
            H_HTTPHANDLE async_thread = (H_HTTPHANDLE)CreateThread(NULL, 0, RequestThread, this, 0, &thread_id);
            m_perform_thread = async_thread;
#else
            pthread_create(&m_perform_thread, NULL, RequestThread, this);
#endif
            return 0;
        }
    }
    void Close()
    {
        if(HTTPHelper::Instance()->FindRequest(m_owner))
        {
#ifdef _WIN32
            if (WaitForSingleObject(m_perform_thread, 10) == WAIT_OBJECT_0)
#else
            if(pthread_kill(m_perform_thread, 0) != 0)
#endif
            {
                printf("Failed to close thread.\n");
            }
            HTTPHelper::Instance()->RemoveRequest(m_owner);
            m_is_cancel = true;
        }
        
        try {
#ifdef _WIN32
            if (m_perform_thread)
            {
                CloseHandle(m_perform_thread);
                m_perform_thread = nullptr;
            }
#else
            pthread_kill(m_perform_thread, 0);
#endif
        } catch(std::exception e) {
            
        }
        
        m_is_running = false;
    }
protected:
    void    ReqeustResultDefault(bool success, const std::string& content)
    {
        if(m_request_handle)
        {
            m_request_handle->OnRequestFinished(success, content, m_owner);
        }
        if(m_result_callback)
        {
            m_result_callback(success, content, m_owner);
        }
    }
    int    ProgressDefault(double total_size, double downloaded_size)
    {
        if(m_request_handle)
        {
            m_request_handle->OnProgress(total_size, downloaded_size, m_owner);
        }
        if(m_progress_callback)
        {
            m_progress_callback(total_size, downloaded_size, m_owner);
        }
        
        return 0;
    }
    
    static size_t RetriveHeaderFunction(void *buffer, size_t size, size_t nitems, void *userdata)
    {
        std::string* receive_header = reinterpret_cast<std::string*>(userdata);
        if (receive_header && buffer)
        {
            receive_header->append(reinterpret_cast<const char*>(buffer), size * nitems);
        }
        
        return nitems * size;
    }
    
    static size_t RetriveContentFunction(void *ptr, size_t size, size_t nmemb, void *userdata)
    {
        std::string* receive_content = reinterpret_cast<std::string*>(userdata);
        if (receive_content && ptr)
        {
            receive_content->append(reinterpret_cast<const char*>(ptr), size * nmemb);
        }
        
        return nmemb * size;
    }
    
    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        DownloadThreadChunk* thread_chunk = reinterpret_cast<DownloadThreadChunk*>(userdata);
        
        if (thread_chunk->_download->m_is_cancel)
        {
            return 0;
        }
        
        DoHTTPLock http_lock(thread_chunk->_download->m_download_lock);
        
        size_t written = 0;
        size_t real_size = size * nmemb;
        if (thread_chunk->_endidx > 0)
        {
            if (thread_chunk->_startidx <= thread_chunk->_endidx)
            {
                if (thread_chunk->_startidx + real_size > thread_chunk->_endidx)
                {
                    real_size = thread_chunk->_endidx - thread_chunk->_startidx + 1;
                }
            }
        }
        
        int seek_error = fseek(thread_chunk->_fp, thread_chunk->_startidx, SEEK_SET);
        if (seek_error != 0)
        {
            perror("fseek");
        }
        else
        {
            written = fwrite(ptr, 1, real_size, thread_chunk->_fp);
        }
        thread_chunk->_download->m_downloaded_size += written;
        thread_chunk->_startidx += written;
        
        return written;
    }
    
    static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
    {
        DownloadThreadChunk* thread_chunk = reinterpret_cast<DownloadThreadChunk*>(clientp);
        DoHTTPLock http_lock(thread_chunk->_download->m_download_lock);
        double total_size = thread_chunk->_download->m_total_size;
        double downloaded_size = thread_chunk->_download->m_downloaded_size;
        thread_chunk->_download->ProgressDefault(total_size, downloaded_size);
        
        return 0;
    }
    
#ifdef _WIN32
    static DWORD WINAPI RequestThread(LPVOID param)
#else
    static void* RequestThread(void* param)
#endif
    {
        
        HTTPHelper::h_Sleep(10);

         HTTPImpl* request = reinterpret_cast<HTTPImpl*>(param);
        
        if (request)
        {
            request->Perform();
            HTTPHelper::Instance()->RemoveRequest(request->m_owner);
        }
        
#ifdef _WIN32
        return 1;
#else
        return NULL;
#endif
    }
#ifdef _WIN32
    static DWORD WINAPI DownloadWork(LPVOID param)
#else
    static void* DownloadWork(void* param)
#endif
    {
        DownloadThreadChunk* thread_chunk = reinterpret_cast<DownloadThreadChunk*>(param);
        
#ifdef _WIN32
        return thread_chunk->_download->DoDownload(thread_chunk);
#else
        thread_chunk->_download->DoDownload(thread_chunk);
        return NULL;
#endif
    }
    static int DoDownload(DownloadThreadChunk* thread_chunk)
    {
        CURL* curl_handle = curl_easy_init();
        if(!curl_handle)
        {
            thread_chunk->_download->m_download_fail = false;
            delete thread_chunk;
            return -1;
        }
        if(thread_chunk->_download->m_share_handle)
        {
            curl_easy_setopt(curl_handle, CURLOPT_SHARE, thread_chunk->_download->m_share_handle);
            curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);
        }
        
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, thread_chunk->_download->m_enable_ssl);
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, thread_chunk->_download->m_enable_ssl);
        
        curl_easy_setopt(curl_handle, CURLOPT_URL, thread_chunk->_download->m_url.c_str());
        
        const char* user_agent = ("Mozilla/5.0 (Windows NT 5.1; rv:5.0) Gecko/20100101 Firefox/5.0");
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, user_agent);
        
        curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 5L);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        
        curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_POST, 0L);
        
        curl_easy_setopt(curl_handle, CURLOPT_FORBID_REUSE, 1);
        curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT_MS, thread_chunk->_download->m_time_out); // ?
        curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, 0);   //0 means block always
        
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, thread_chunk);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, RetriveHeaderFunction);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, NULL);
        
        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(curl_handle, CURLOPT_XFERINFODATA, thread_chunk);
        
        curl_easy_setopt(curl_handle, CURLOPT_LOW_SPEED_LIMIT, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_LOW_SPEED_TIME, 5L);
        
        if (thread_chunk->_endidx != 0)
        {
            std::string down_range;
            std::ostringstream ostr;
            if (thread_chunk->_endidx > 0)
            {
                ostr << thread_chunk->_startidx << "-" << thread_chunk->_endidx;
            }
            else
            {
                ostr << thread_chunk->_startidx << "-";
            }
            
            down_range = ostr.str();
            curl_easy_setopt(curl_handle, CURLOPT_RANGE, down_range.c_str());
        }
        
        CURLcode curl_code = curl_easy_perform(curl_handle);
        if (curl_code == CURLE_OPERATION_TIMEDOUT)
        {
            int retry_count = thread_chunk->_download->m_retry_times;
            while (retry_count > 0)
            {
                printf("DoDownload[thread id:%d] Connect Timeout, Will Try %d/%d\n",
                       thread_chunk->_id,
                       1+thread_chunk->_download->m_retry_times-retry_count,
                       thread_chunk->_download->m_retry_times);
                curl_code = curl_easy_perform(curl_handle);
                if (curl_code != CURLE_OPERATION_TIMEDOUT) break;
                retry_count--;
            }
        }
        if (curl_code == CURLE_OPERATION_TIMEDOUT)
        {
            printf("DoDownload[thread id:%d] Connect Timeout\n", thread_chunk->_id);
        }
        
        long http_code;
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
        if (curl_code == CURLE_OK && (http_code >= 200 && http_code <= 300))
        {
            thread_chunk->_download->m_http_code = http_code;
            thread_chunk->_download->m_download_fail = false;
        }
        else
        {
            const char* err_string = curl_easy_strerror(curl_code);
            thread_chunk->_download->m_error_string = err_string;
            thread_chunk->_download->m_download_fail = true;
            thread_chunk->_download->m_http_code = http_code;
        }
        
        curl_easy_cleanup(curl_handle);
        
        delete thread_chunk;
        
        return curl_code;
    }
protected:
    int Perform()
    {
        m_is_running = true;
        m_is_cancel = false;
        m_receive_header.clear();
        m_receive_content.clear();
        int code = 0;
        if(m_is_download)
        {
            code = PerformDownload();
        }
        else
        {
            code = PerformRequest();
        }
        return code;
    }
    int PerformRequest()
    {
        H_HTTPHANDLE curl_handle = curl_easy_init();
        if(m_share_handle)
        {
            curl_easy_setopt(curl_handle, CURLOPT_SHARE, m_share_handle);
            curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);
        }
        
        if (curl_handle)
        {
            CURLcode curl_code;
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_URL, m_url.c_str());
            //set force http redirect
            if(m_enable_redirect)
            {
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 5);
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
            }
            else
            {
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 0L);
            }
            //Post
            if(m_use_post)
            {
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, m_post_data ? m_post_data : "");
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, m_post_data_size);
            }
            //headers
            if(!m_headers.empty())
            {
                m_http_headers = curl_slist_append(reinterpret_cast<curl_slist*>(m_http_headers), m_headers.c_str());
            }
            if (m_http_headers)
            {
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, reinterpret_cast<curl_slist*>(m_http_headers));
            }
            //Proxy
            if(!m_proxy.empty())
            {
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_PROXYPORT, m_proxy_port);
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_PROXY, m_proxy.c_str());
            }
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, RetriveHeaderFunction);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &m_receive_header);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, RetriveContentFunction);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &m_receive_content);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT_MS, 5000);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, m_time_out);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, m_enable_ssl);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, m_enable_ssl);
            
            curl_code = curl_easy_perform(curl_handle);
            if (curl_code == CURLE_OPERATION_TIMEDOUT)
            {
                int retry_count = m_retry_times;
                while (retry_count > 0)
                {
                    printf("PerformRequest Connect Timeout, Will Try %d/%d\n", 1+m_retry_times-retry_count, m_retry_times);
                    curl_code = curl_easy_perform(curl_handle);
                    if (curl_code != CURLE_OPERATION_TIMEDOUT) break;
                    retry_count--;
                }
            }
            if (curl_code == CURLE_OPERATION_TIMEDOUT)
            {
                printf("PerformRequest Connect Timeout\n");
            }
            
            curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &m_http_code);
            if (curl_code == CURLE_OK && m_http_code == 200)
            {
                ReqeustResultDefault(true, m_receive_content);
            }
            else
            {
                const char* err_string = curl_easy_strerror(curl_code);
                m_error_string = err_string;
                curl_code = CURLE_HTTP_POST_ERROR;
                ReqeustResultDefault(false, m_error_string);
            }
            
            if (m_http_headers)
            {
                curl_slist_free_all(reinterpret_cast<curl_slist*>(m_http_headers));
                m_http_headers = nullptr;
            }
            m_is_running = false;
            curl_easy_cleanup(curl_handle);
            return curl_code;
        }
        
        m_error_string = "Failed to curl_easy_init";
        ReqeustResultDefault(false, m_error_string);
        return -1;
    }
    
    int PerformDownload()
    {
        m_download_lock = std::shared_ptr<HTTPLock>(new HTTPLock);
        bool success = GetDownloadFileSize(m_total_size);
        printf("total size:%f\n", m_total_size);
        if (!success)
        {
            m_is_running = false;
            ReqeustResultDefault(false, m_error_string);
            return -1;
        }
        
        std::string out_file_name = m_download_path;
        std::string src_file_name = out_file_name;
        out_file_name += ".dl";
        
        FILE *fp = nullptr;
#ifdef _WIN32
        remove(out_file_name.c_str());
        fp = fopen(out_file_name.c_str(), "wb");
#else
        unlink(out_file_name.c_str());
        fp = fopen(out_file_name.c_str(), "wb");
#endif
        if (!fp)
        {
            m_is_running = false;
            ReqeustResultDefault(false, m_error_string);
            return -1;
        }
        
        int down_code = -1;
        int thread_count = SplitDownloadCount(m_total_size);
        
        m_thread_count = thread_count > m_thread_count ? m_thread_count : thread_count;
        //文件大小有分开下载的必要并且服务器支持多线程下载时，启用多线程下载
        if (m_multi_download && m_thread_count > 1)
        {
            long gap = static_cast<long>(m_total_size) / m_thread_count;
            
            for (int i = 0; i < m_thread_count; i++)
            {
                DownloadThreadChunk* thread_chunk = new DownloadThreadChunk;
                thread_chunk->_fp = fp;
                thread_chunk->_download = this;
                
                if (i < m_thread_count - 1)
                {
                    thread_chunk->_startidx = i * gap;
                    thread_chunk->_endidx = thread_chunk->_startidx + gap - 1;
                }
                else
                {
                    thread_chunk->_startidx = i * gap;
                    thread_chunk->_endidx = -1;
                }
                
#ifdef _WIN32
                DWORD thread_id;
                H_HTTPHANDLE hThread = (H_HTTPHANDLE)CreateThread(NULL, 0, DownloadWork, thread_chunk, 0, &(thread_id));
#else
                pthread_t hThread;
                pthread_create(&hThread, NULL, DownloadWork, thread_chunk);
#endif
                m_download_threads.push_back(hThread);
            }
            
#ifdef _WIN32
            WaitForMultipleObjects(m_download_threads.size(), &m_download_threads[0], TRUE, INFINITE);
            for (H_HTTPHANDLE handle : m_download_threads)
            {
                CloseHandle(handle);
            }
#else
            for(pthread_t thread : m_download_threads)
            {
                pthread_join(thread, NULL);
            }
#endif
            m_download_threads.clear();
            
            if(m_download_fail)
                down_code = -1;
        }
        else
        {
            DownloadThreadChunk* thread_chunk = new DownloadThreadChunk;
            thread_chunk->_fp = fp;
            thread_chunk->_download = this;
            thread_chunk->_startidx = 0;
            thread_chunk->_endidx = 0;
            down_code = DoDownload(thread_chunk);
        }
        
        fclose(fp);
        
        if (!m_download_fail)
        {
#ifdef _WIN32
            MoveFileExA(out_file_name.c_str(), src_file_name.c_str(), MOVEFILE_REPLACE_EXISTING);
#else
            unlink(src_file_name.c_str());
            rename(out_file_name.c_str(), src_file_name.c_str());
#endif
        }
        else
        {
#ifdef _WIN32
            remove(out_file_name.c_str());
#else
            unlink(out_file_name.c_str());
#endif
        }
        
        ReqeustResultDefault(m_download_fail ? false : true, m_error_string);
        
        m_is_running = false;

        return down_code;
    }
    
    bool GetDownloadFileSize(double& down_file_length)
    {
        if (m_url.empty())
        {
            return false;
        }
        else
        {
            CURL *handle = curl_easy_init();
            if(!handle)
                return false;
            if(m_share_handle)
            {
                curl_easy_setopt(handle, CURLOPT_SHARE, m_share_handle);
                curl_easy_setopt(handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);
            }
            
            if (handle)
            {
                curl_easy_setopt(handle, CURLOPT_URL, m_url.c_str());
                curl_easy_setopt(handle, CURLOPT_HEADER, 1);
                curl_easy_setopt(handle, CURLOPT_NOBODY, 1);
                curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, RetriveHeaderFunction);
                curl_easy_setopt(handle, CURLOPT_HEADERDATA, &m_receive_header);
                curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, RetriveContentFunction);
                curl_easy_setopt(handle, CURLOPT_WRITEDATA, NULL);
                curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT_MS, 5000);
                curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, m_time_out);   //0 means block always
                curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, m_enable_ssl);
                curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, m_enable_ssl);
                //set force http redirect
                if(m_enable_redirect)
                {
                    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 5);
                    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
                }
                else
                {
                    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 0L);
                }
                //Post
                if(m_use_post)
                {
                    curl_easy_setopt(handle, CURLOPT_POST, 1);
                    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, m_post_data ? m_post_data : "");
                    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, m_post_data_size);
                }
                //headers
                if(!m_headers.empty())
                {
                    m_http_headers = curl_slist_append(reinterpret_cast<curl_slist*>(m_http_headers), m_headers.c_str());
                }
                if (m_http_headers)
                {
                    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, reinterpret_cast<curl_slist*>(m_http_headers));
                }
                //Proxy
                if(!m_proxy.empty())
                {
                    curl_easy_setopt(handle, CURLOPT_PROXYPORT, m_proxy_port);
                    curl_easy_setopt(handle, CURLOPT_PROXY, m_proxy.c_str());
                }
                
                
                CURLcode curl_code = curl_easy_perform(handle);
                
                if (curl_code == CURLE_OPERATION_TIMEDOUT)
                {
                    int retry_count = m_retry_times;
                    while (retry_count > 0)
                    {
                        printf("GetDownloadFileSize Connect Timeout, Will Try %d/%d\n", 1+m_retry_times-retry_count, m_retry_times);
                        curl_code = curl_easy_perform(handle);
                        if (curl_code != CURLE_OPERATION_TIMEDOUT) break;
                        retry_count--;
                    }
                }
                
                if (curl_code == CURLE_OPERATION_TIMEDOUT)
                {
                    printf("GetDownloadFileSize Connect Timeout\n");
                }
                
                bool success = false;
                curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &m_http_code);
                
                if (curl_code == CURLE_OK)
                {
                    success = true;
                    curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &down_file_length);
                    
                    try
                    {
                        //匹配"Content-Range: bytes 2-1449/26620" 则证明支持多线程下载
                        std::regex pattern("CONTENT-RANGE\\s*:\\s*\\w+\\s*(\\d+)-(\\d*)/(\\d+)", std::regex::icase);
                        m_multi_download = std::regex_search(m_receive_header, pattern);
                    }
                    catch(std::exception e)
                    {
                        m_multi_download = false;
                        printf("[exception]%s\n", e.what());
                    }
                }
                else
                {
                    const char* err_string = curl_easy_strerror(curl_code);
                    m_error_string = err_string;
                }
                
                if (m_http_headers)
                {
                    curl_slist_free_all(reinterpret_cast<curl_slist*>(m_http_headers));
                    m_http_headers = nullptr;
                }
                
                curl_easy_cleanup(handle);
                return success;
            }
            
            return false;
        }
    }
    
    int SplitDownloadCount(double down_size)
    {
        const double size_2mb = 2.0 * 1024 * 1024;
        const double size_10mb = 10.0 * 1024 * 1024;
        const double size_50mb = 50.0 * 1024 * 1024;
        
        if (down_size <= size_2mb)
        {
            return 1;
        }
        else if (down_size > size_2mb && down_size <= size_10mb)
        {
            return static_cast<int>(down_size / (size_2mb));
        }
        else if (down_size > size_10mb && down_size <= size_50mb)
        {
            return HTTPRequest::s_kThreadCount + 1;
        }
        else
        {
            int down_count = static_cast<int>(down_size / size_10mb);
            return down_count > 10 ? 10 : down_count;
        }
        
        return 1;
    }
};

size_t HTTPImpl::DownloadThreadChunk::counter = 0;
////////////////////////////////////////////////////////////////////////
////
void         HTTPRequest::SetRetryTimes(int retry_times)
{
    if(!_impl) return;
    return _impl->SetRetryTimes(retry_times);
}
void         HTTPRequest::SetTimeout(long time_out)
{
    if(!_impl) return;
    return _impl->SetTimeout(time_out);
}
void         HTTPRequest::SetUrl(const std::string& url)
{
    if(!_impl) return;
    return _impl->SetUrl(url);
}
void         HTTPRequest::SetMovedUrl(bool get_moved_url)
{
    if(!_impl) return;
    return _impl->SetMovedUrl(get_moved_url);
}
void         HTTPRequest::SetUserData(void* userdata)
{
    if(!_impl) return;
    return _impl->SetUserData(userdata);
}
void*       HTTPRequest::GetUserData()
{
    if(!_impl) return nullptr;
    return _impl->GetUserData();
}
int         HTTPRequest::GetRequestId()
{
    if(!_impl) return 0;
    return _impl->GetRequestId();
}
void         HTTPRequest::SetProgressCallback(ProgressCallback pc)
{
    if(!_impl) return;
    return _impl->SetProgressCallback(pc);
}
void         HTTPRequest::SetResultCallback(ResultCallback rc)
{
    if(!_impl) return;
    return _impl->SetResultCallback(rc);
}
void         HTTPRequest::SetRequestHandle(IRequestHandle* handle)
{
    if(!_impl) return;
    return _impl->SetRequestHandle(handle);
}

void         HTTPRequest::SetPostMethod(bool post)
{
    if(!_impl) return;
    return _impl->SetPostMethod(post);
}
void         HTTPRequest::SetPostData(const std::string& message)
{
    return SetPostData(message.c_str(), message.size());
}
void         HTTPRequest::SetPostData(const void* data, size_t size)
{
    if(!_impl) return;
    return _impl->SetPostData(data, size);
}
void         HTTPRequest::SetRequestHeader(const std::map<std::string, std::string>& headers)
{
    for (auto it = headers.begin(); it != headers.end(); ++it)
    {
        std::string header = it->first;
        header += ": ";
        header += it->second;
        SetRequestHeader(header);
    }
}
void         HTTPRequest::SetRequestHeader(const std::string& header)
{
    if(!_impl) return;
    return _impl->SetRequestHeader(header);
}
void         HTTPRequest::SetRequestProxy(const std::string& proxy, long proxy_port)
{
    if(!_impl) return;
    return _impl->SetRequestProxy(proxy, proxy_port);
}

void         HTTPRequest::SetSSLVerify(bool enable)
{
    if(!_impl) return;
    return _impl->SetSSLVerify(enable);
}

bool         HTTPRequest::SetDownloadMode(bool is_download)
{
    if(!_impl) return false;
    return _impl->SetDownloadMode(is_download);
}

void         HTTPRequest::SetDownloadPath(const std::string& path)
{
    if(!_impl) return;
    return _impl->SetDownloadPath(path);
}
void         HTTPRequest::SetDownloadThreadCount(size_t count)
{
    if(!_impl) return;
    return _impl->SetDownloadThreadCount(count);
}

bool         HTTPRequest::GetHeader(std::string* header)
{
    if(!_impl) return false;
    return _impl->GetHeader(header);
}
bool         HTTPRequest::GetContent(std::string* content)
{
    if(!_impl) return false;
    return _impl->GetContent(content);
}
bool         HTTPRequest::GetErrorString(std::string* error_string)
{
    if(!_impl) return false;
    return _impl->GetErrorString(error_string);
}
long         HTTPRequest::GetHttpCode()
{
    if(!_impl) return 0;
    return _impl->GetHttpCode();
}

HTTPRequest::HTTPRequest(bool init)
:_impl(nullptr)
,_auto_destroy(true)
{
    if(init) Init();
}
HTTPRequest::~HTTPRequest()
{
}

void         HTTPRequest::Init()
{
    if(_impl) return;
    int id = HTTPHelper::Instance()->GetID();
    H_HTTPHANDLE s_shared_handle = HTTPHelper::Instance()->GetSharedCurl();
    _impl = std::shared_ptr<HTTPImpl>(new HTTPImpl(id, this, s_shared_handle));
}

int          HTTPRequest::Run(bool download, bool async)
{
    assert(_impl);
    _impl->SetDownloadMode(download);
    return _impl->Run(async);
}

void         HTTPRequest::Tick(float dt)
{
    HTTPHelper::Instance()->Tick(dt);
}

HTTPRequest* HTTPRequest::Create(bool init)
{
    return new HTTPRequest(init);
}
void  HTTPRequest::Destroy(HTTPRequest*& request)
{
    int id = request->GetRequestId();
    delete request;
    request = nullptr;
    printf("~HTTPRequest:%d\n", id);
}
