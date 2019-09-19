#ifndef HTTP_PAYLOAD_H_
#define HTTP_PAYLOAD_H_

#include <string>
#include <map>

#include "httpdef.h"
#include "http_content.h"
#include "hstring.h"

typedef std::map<std::string, std::string, StringCaseLess>  http_headers;
typedef std::string                                         http_body;

class HttpPayload {
public:
    int                 type;
    unsigned short      http_major;
    unsigned short      http_minor;

    http_headers        headers;
    http_body           body;

    // structured content
    void*               content;    // DATA_NO_COPY
    int                 content_length;
    http_content_type   content_type;
    Json                json;       // APPLICATION_JSON
    MultiPart           mp;         // FORM_DATA
    KeyValue            kv;         // X_WWW_FORM_URLENCODED

    HttpPayload() {
        Init();
    }

    virtual ~HttpPayload() {}

    void Init() {
        type = HTTP_BOTH;
        http_major = 1;
        http_minor = 1;
        content = NULL;
        content_length = 0;
        content_type = CONTENT_TYPE_NONE;
    }

    virtual void Reset() {
        Init();
        headers.clear();
        body.clear();
        json.clear();
        mp.clear();
        kv.clear();
    }

    // structured-content -> content_type <-> headers Content-Type
    void FillContentType();
    // body.size -> content_length <-> headers Content-Length
    void FillContentLength();

    // headers -> string
    void DumpHeaders(std::string& str);
    // structured content -> body
    void DumpBody();
    // body -> structured content
    // @retval 0:succeed
    int  ParseBody();

    virtual std::string Dump(bool is_dump_headers, bool is_dump_body);

    void* Content() {
        if (content == NULL && body.size() != 0) {
            content = (void*)body.data();
        }
        return content;
    }

    int ContentLength() {
        if (content_length == 0) {
            FillContentLength();
        }
        return content_length;
    }

    http_content_type ContentType() {
        if (content_type == CONTENT_TYPE_NONE) {
            FillContentType();
        }
        return content_type;
    }
};

#define DEFAULT_USER_AGENT "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36"
class HttpRequest : public HttpPayload {
public:
    http_method         method;
    // scheme:[//[user[:password]@]host[:port]][/path][?query][#fragment]
    std::string         url;
    // structured url
    bool                https;
    std::string         host;
    int                 port;
    std::string         path;
    QueryParams         query_params;

    HttpRequest() : HttpPayload() {
        type = HTTP_REQUEST;
        Init();
    }

    void Init() {
        headers["User-Agent"] = DEFAULT_USER_AGENT;
        headers["Accept"] = "*/*";
        method = HTTP_GET;
        https = 0;
        host = "127.0.0.1";
        port = DEFAULT_HTTP_PORT;
        path = "/";
    }

    virtual void Reset() {
        HttpPayload::Reset();
        Init();
        url.clear();
        query_params.clear();
    }

    virtual std::string Dump(bool is_dump_headers, bool is_dump_body);

    // structed url -> url
    void DumpUrl();
    // url -> structed url
    void ParseUrl();
};

class HttpResponse : public HttpPayload {
public:
    http_status         status_code;

    HttpResponse() : HttpPayload() {
        type = HTTP_RESPONSE;
        Init();
    }

    void Init() {
        status_code = HTTP_STATUS_OK;
    }

    virtual void Reset() {
        HttpPayload::Reset();
        Init();
    }

    virtual std::string Dump(bool is_dump_headers = true, bool is_dump_body = false);
};

#endif // HTTP_PAYLOAD_H_
